// ==WindhawkMod==
// @id system-sounds-volume
// @name System Sounds Volume
// @description Control Windows System Sounds volume independently from applications and games.
// @version 2.0.0
// @author The.BARDIA
// @include windhawk.exe
// @compilerOptions -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Sounds Volume

Controls the Windows System Sounds audio session independently from
applications, games, browsers and music players.

The default volume is 40%.

The mod automatically reapplies the selected volume when:

- The default audio output device changes.
- The Windows System Sounds audio session is recreated.
- Windows changes the System Sounds session volume.

The original System Sounds volume of each audio device is saved before
the mod changes it and restored when the mod is disabled.

This mod does not change the Windows master volume or application volumes.

## Test Sound

Enable Test Sound and save the settings.

A Windows system sound will be played once using the configured volume.

Disable Test Sound afterwards.

## Why this mod?

Windows already provides a System Sounds volume control.

This mod is useful when you want a preferred System Sounds level to be
automatically maintained across audio-device changes and System Sounds
session recreation.

Author: The.BARDIA
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Volume: 40
  $name: System Sounds Volume
  $description: System Sounds volume. Enter a value from 0 to 100.

- TestSound: false
  $name: Test Sound
  $description: Enable and save to play one test sound at the configured volume.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>

#include <atomic>
#include <string>
#include <vector>

#include <windhawk_api.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")


// ============================================================================
// Constants
// ============================================================================

static const GUID kIID_IUnknownLocal =
{
    0x00000000,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

static const GUID kVolumeEventContext =
{
    0x5baf9e31,
    0x6a41,
    0x4b18,
    {0x91, 0x4d, 0x63, 0x2f, 0x73, 0x6e, 0x5a, 0x10}
};


// ============================================================================
// Global state
// ============================================================================

static HANDLE g_stopEvent = nullptr;
static HANDLE g_applyEvent = nullptr;
static HANDLE g_workerThread = nullptr;

static std::atomic<int> g_volume(40);
static std::atomic<bool> g_testSound(false);
static std::atomic<bool> g_testSoundPending(false);

static std::wstring g_activeEndpointId;


// ============================================================================
// Saved original endpoint volume
// ============================================================================

struct SavedEndpoint
{
    std::wstring id;
    float volume;
};

static std::vector<SavedEndpoint> g_savedEndpoints;


// ============================================================================
// Settings
// ============================================================================

static void LoadSettings()
{
    int volume = Wh_GetIntSetting(L"Volume");

    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    g_volume.store(
        volume,
        std::memory_order_release
    );

    g_testSound.store(
        Wh_GetIntSetting(L"TestSound") != 0,
        std::memory_order_release
    );
}


// ============================================================================
// Endpoint storage helpers
// ============================================================================

static unsigned long long HashEndpointId(
    const std::wstring& id
)
{
    // FNV-1a 64-bit.
    unsigned long long hash =
        14695981039346656037ULL;

    for (wchar_t c : id)
    {
        hash ^= static_cast<unsigned long long>(
            static_cast<unsigned int>(c)
        );

        hash *= 1099511628211ULL;
    }

    return hash;
}


static void MakeStorageName(
    const std::wstring& endpointId,
    const wchar_t* suffix,
    wchar_t* buffer,
    size_t bufferCount
)
{
    unsigned long long hash =
        HashEndpointId(endpointId);

    swprintf_s(
        buffer,
        bufferCount,
        L"Endpoint_%016llX_%s",
        hash,
        suffix
    );
}


static bool FindSavedEndpoint(
    const std::wstring& endpointId,
    float* volume
)
{
    for (const auto& entry : g_savedEndpoints)
    {
        if (entry.id == endpointId)
        {
            if (volume)
                *volume = entry.volume;

            return true;
        }
    }

    return false;
}


static bool SaveOriginalEndpoint(
    const std::wstring& endpointId,
    float volume
)
{
    float existingVolume;

    if (FindSavedEndpoint(
            endpointId,
            &existingVolume
        ))
    {
        return true;
    }

    SavedEndpoint entry;

    entry.id = endpointId;
    entry.volume = volume;

    g_savedEndpoints.push_back(
        entry
    );

    wchar_t idName[128];
    wchar_t volumeName[128];

    MakeStorageName(
        endpointId,
        L"Id",
        idName,
        ARRAYSIZE(idName)
    );

    MakeStorageName(
        endpointId,
        L"Volume",
        volumeName,
        ARRAYSIZE(volumeName)
    );

    if (!Wh_SetStringValue(
            idName,
            endpointId.c_str()
        ))
    {
        Wh_Log(
            L"Wh_SetStringValue failed for endpoint"
        );

        return false;
    }

    int volumePercent =
        static_cast<int>(
            volume * 100.0f + 0.5f
        );

    if (volumePercent < 0)
        volumePercent = 0;

    if (volumePercent > 100)
        volumePercent = 100;

    if (!Wh_SetIntValue(
            volumeName,
            volumePercent
        ))
    {
        Wh_Log(
            L"Wh_SetIntValue failed for endpoint"
        );

        return false;
    }

    return true;
}


static void LoadSavedEndpoints()
{
    g_savedEndpoints.clear();

    // We keep an index of endpoint hashes so the values remain
    // addressable even if the original audio device is disconnected.
    int count =
        Wh_GetIntValue(
            L"SavedEndpointCount",
            0
        );

    if (count < 0 || count > 128)
        return;

    for (int i = 0; i < count; i++)
    {
        wchar_t hashName[128];

        swprintf_s(
            hashName,
            L"SavedEndpointHash_%d",
            i
        );

        WCHAR hashText[32];

        if (
            Wh_GetStringValue(
                hashName,
                hashText,
                ARRAYSIZE(hashText)
            ) == 0
        )
        {
            continue;
        }

        wchar_t idName[128];
        wchar_t volumeName[128];

        swprintf_s(
            idName,
            L"Endpoint_%s_Id",
            hashText
        );

        swprintf_s(
            volumeName,
            L"Endpoint_%s_Volume",
            hashText
        );

        WCHAR endpointId[1024];

        if (
            Wh_GetStringValue(
                idName,
                endpointId,
                ARRAYSIZE(endpointId)
            ) == 0
        )
        {
            continue;
        }

        int volume =
            Wh_GetIntValue(
                volumeName,
                -1
            );

        if (volume < 0 || volume > 100)
            continue;

        SavedEndpoint entry;

        entry.id = endpointId;
        entry.volume =
            static_cast<float>(volume) / 100.0f;

        g_savedEndpoints.push_back(
            entry
        );
    }
}


static void SaveEndpointIndex()
{
    int count =
        static_cast<int>(
            g_savedEndpoints.size()
        );

    if (count > 128)
        count = 128;

    Wh_SetIntValue(
        L"SavedEndpointCount",
        count
    );

    for (int i = 0; i < count; i++)
    {
        unsigned long long hash =
            HashEndpointId(
                g_savedEndpoints[i].id
            );

        wchar_t hashText[32];

        swprintf_s(
            hashText,
            L"%016llX",
            hash
        );

        wchar_t hashName[128];

        swprintf_s(
            hashName,
            L"SavedEndpointHash_%d",
            i
        );

        Wh_SetStringValue(
            hashName,
            hashText
        );
    }
}


// ============================================================================
// Restore a specific endpoint
// ============================================================================

static bool RestoreEndpoint(
    IMMDeviceEnumerator* deviceEnumerator,
    const std::wstring& endpointId,
    float volume
)
{
    if (!deviceEnumerator)
        return false;

    IMMDevice* device = nullptr;

    HRESULT hr =
        deviceEnumerator->GetDevice(
            endpointId.c_str(),
            &device
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"GetDevice failed while restoring: 0x%08X",
            hr
        );

        return false;
    }

    IAudioSessionManager2* manager = nullptr;

    hr =
        device->Activate(
            __uuidof(IAudioSessionManager2),
            CLSCTX_INPROC_SERVER,
            nullptr,
            reinterpret_cast<void**>(&manager)
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"Activate session manager failed: 0x%08X",
            hr
        );

        device->Release();
        return false;
    }

    IAudioSessionEnumerator* enumerator = nullptr;

    hr =
        manager->GetSessionEnumerator(
            &enumerator
        );

    if (FAILED(hr))
    {
        manager->Release();
        device->Release();

        return false;
    }

    int count = 0;

    enumerator->GetCount(&count);

    bool restored = false;

    for (int i = 0; i < count; i++)
    {
        IAudioSessionControl* session = nullptr;

        if (
            FAILED(
                enumerator->GetSession(
                    i,
                    &session
                )
            )
        )
        {
            continue;
        }

        IAudioSessionControl2* session2 = nullptr;

        if (
            SUCCEEDED(
                session->QueryInterface(
                    __uuidof(IAudioSessionControl2),
                    reinterpret_cast<void**>(&session2)
                )
            )
        )
        {
            if (
                session2->IsSystemSoundsSession()
                == S_OK
            )
            {
                ISimpleAudioVolume* simpleVolume = nullptr;

                if (
                    SUCCEEDED(
                        session->QueryInterface(
                            __uuidof(ISimpleAudioVolume),
                            reinterpret_cast<void**>(
                                &simpleVolume
                            )
                        )
                    )
                )
                {
                    HRESULT setHr =
                        simpleVolume->SetMasterVolume(
                            volume,
                            &kVolumeEventContext
                        );

                    if (FAILED(setHr))
                    {
                        Wh_Log(
                            L"Restore SetMasterVolume failed: 0x%08X",
                            setHr
                        );
                    }
                    else
                    {
                        restored = true;
                    }

                    simpleVolume->Release();
                }
            }

            session2->Release();
        }

        session->Release();
    }

    enumerator->Release();
    manager->Release();
    device->Release();

    return restored;
}


// ============================================================================
// Restore all saved endpoints
// ============================================================================

static void RestoreAllSavedEndpoints()
{
    if (g_savedEndpoints.empty())
        return;

    IMMDeviceEnumerator* deviceEnumerator = nullptr;

    HRESULT hr =
        CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(
                &deviceEnumerator
            )
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"Restore CoCreateInstance failed: 0x%08X",
            hr
        );

        return;
    }

    for (const auto& entry : g_savedEndpoints)
    {
        RestoreEndpoint(
            deviceEnumerator,
            entry.id,
            entry.volume
        );
    }

    deviceEnumerator->Release();

    g_savedEndpoints.clear();

    Wh_SetIntValue(
        L"SavedEndpointCount",
        0
    );
}


// ============================================================================
// System Sounds session discovery
// ============================================================================

static bool ApplySystemSoundsVolume(
    IMMDeviceEnumerator* deviceEnumerator,
    std::wstring* endpointIdOut
)
{
    if (!deviceEnumerator)
        return false;

    IMMDevice* device = nullptr;

    HRESULT hr =
        deviceEnumerator->GetDefaultAudioEndpoint(
            eRender,
            eConsole,
            &device
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"GetDefaultAudioEndpoint failed: 0x%08X",
            hr
        );

        return false;
    }

    LPWSTR endpointIdRaw = nullptr;

    hr =
        device->GetId(
            &endpointIdRaw
        );

    if (FAILED(hr))
    {
        device->Release();
        return false;
    }

    std::wstring endpointId(
        endpointIdRaw
    );

    CoTaskMemFree(endpointIdRaw);

    if (endpointIdOut)
        *endpointIdOut = endpointId;

    IAudioSessionManager2* manager = nullptr;

    hr =
        device->Activate(
            __uuidof(IAudioSessionManager2),
            CLSCTX_INPROC_SERVER,
            nullptr,
            reinterpret_cast<void**>(&manager)
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"Activate IAudioSessionManager2 failed: 0x%08X",
            hr
        );

        device->Release();
        return false;
    }

    IAudioSessionEnumerator* enumerator = nullptr;

    hr =
        manager->GetSessionEnumerator(
            &enumerator
        );

    if (FAILED(hr))
    {
        manager->Release();
        device->Release();

        return false;
    }

    int count = 0;

    enumerator->GetCount(&count);

    bool found = false;

    for (int i = 0; i < count; i++)
    {
        IAudioSessionControl* session = nullptr;

        if (
            FAILED(
                enumerator->GetSession(
                    i,
                    &session
                )
            )
        )
        {
            continue;
        }

        IAudioSessionControl2* session2 = nullptr;

        if (
            SUCCEEDED(
                session->QueryInterface(
                    __uuidof(IAudioSessionControl2),
                    reinterpret_cast<void**>(&session2)
                )
            )
        )
        {
            if (
                session2->IsSystemSoundsSession()
                == S_OK
            )
            {
                ISimpleAudioVolume* simpleVolume = nullptr;

                if (
                    SUCCEEDED(
                        session->QueryInterface(
                            __uuidof(ISimpleAudioVolume),
                            reinterpret_cast<void**>(
                                &simpleVolume
                            )
                        )
                    )
                )
                {
                    float currentVolume = 1.0f;

                    if (
                        SUCCEEDED(
                            simpleVolume->GetMasterVolume(
                                &currentVolume
                            )
                        )
                    )
                    {
                        float savedVolume = 0.0f;

                        if (
                            !FindSavedEndpoint(
                                endpointId,
                                &savedVolume
                            )
                        )
                        {
                            SaveOriginalEndpoint(
                                endpointId,
                                currentVolume
                            );

                            SaveEndpointIndex();
                        }
                    }

                    int configuredVolume =
                        g_volume.load(
                            std::memory_order_acquire
                        );

                    float target =
                        static_cast<float>(
                            configuredVolume
                        ) / 100.0f;

                    HRESULT setHr =
                        simpleVolume->SetMasterVolume(
                            target,
                            &kVolumeEventContext
                        );

                    if (FAILED(setHr))
                    {
                        Wh_Log(
                            L"SetMasterVolume failed: 0x%08X",
                            setHr
                        );
                    }
                    else
                    {
                        found = true;
                    }

                    simpleVolume->Release();
                }
            }

            session2->Release();
        }

        session->Release();
    }

    enumerator->Release();
    manager->Release();
    device->Release();

    return found;
}


// ============================================================================
// Audio device notifications
// ============================================================================

class AudioDeviceNotification
    : public IMMNotificationClient
{
private:
    ULONG m_refCount = 1;

public:

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void** object
    ) override
    {
        if (!object)
            return E_POINTER;

        *object = nullptr;

        if (
            IsEqualGUID(
                riid,
                kIID_IUnknownLocal
            ) ||
            IsEqualGUID(
                riid,
                __uuidof(IMMNotificationClient)
            )
        )
        {
            *object =
                static_cast<IMMNotificationClient*>(this);

            AddRef();

            return S_OK;
        }

        return E_NOINTERFACE;
    }


    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(
            reinterpret_cast<LONG*>(&m_refCount)
        );
    }


    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG result =
            InterlockedDecrement(
                reinterpret_cast<LONG*>(&m_refCount)
            );

        if (result == 0)
            delete this;

        return result;
    }


    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(
        LPCWSTR,
        DWORD
    ) override
    {
        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnDeviceAdded(
        LPCWSTR
    ) override
    {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(
        LPCWSTR
    ) override
    {
        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
        EDataFlow flow,
        ERole role,
        LPCWSTR
    ) override
    {
        if (
            flow == eRender &&
            (
                role == eConsole ||
                role == eMultimedia ||
                role == eCommunications
            )
        )
        {
            if (g_applyEvent)
                SetEvent(g_applyEvent);
        }

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
        LPCWSTR,
        const PROPERTYKEY
    ) override
    {
        return S_OK;
    }
};


// ============================================================================
// Audio session notifications
// ============================================================================

class AudioSessionNotification
    : public IAudioSessionNotification
{
private:
    ULONG m_refCount = 1;

public:

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void** object
    ) override
    {
        if (!object)
            return E_POINTER;

        *object = nullptr;

        if (
            IsEqualGUID(
                riid,
                kIID_IUnknownLocal
            ) ||
            IsEqualGUID(
                riid,
                __uuidof(IAudioSessionNotification)
            )
        )
        {
            *object =
                static_cast<IAudioSessionNotification*>(this);

            AddRef();

            return S_OK;
        }

        return E_NOINTERFACE;
    }


    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(
            reinterpret_cast<LONG*>(&m_refCount)
        );
    }


    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG result =
            InterlockedDecrement(
                reinterpret_cast<LONG*>(&m_refCount)
            );

        if (result == 0)
            delete this;

        return result;
    }


    HRESULT STDMETHODCALLTYPE OnSessionCreated(
        IAudioSessionControl*
    ) override
    {
        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }
};


// ============================================================================
// Audio session volume notifications
// ============================================================================

class AudioSessionEvents
    : public IAudioSessionEvents
{
private:
    ULONG m_refCount = 1;

public:

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void** object
    ) override
    {
        if (!object)
            return E_POINTER;

        *object = nullptr;

        if (
            IsEqualGUID(
                riid,
                kIID_IUnknownLocal
            ) ||
            IsEqualGUID(
                riid,
                __uuidof(IAudioSessionEvents)
            )
        )
        {
            *object =
                static_cast<IAudioSessionEvents*>(this);

            AddRef();

            return S_OK;
        }

        return E_NOINTERFACE;
    }


    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(
            reinterpret_cast<LONG*>(&m_refCount)
        );
    }


    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG result =
            InterlockedDecrement(
                reinterpret_cast<LONG*>(&m_refCount)
            );

        if (result == 0)
            delete this;

        return result;
    }


    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(
        LPCWSTR,
        LPCGUID
    ) override
    {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnIconPathChanged(
        LPCWSTR,
        LPCGUID
    ) override
    {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
        float,
        BOOL,
        LPCGUID eventContext
    ) override
    {
        if (
            eventContext &&
            IsEqualGUID(
                *eventContext,
                kVolumeEventContext
            )
        )
        {
            return S_OK;
        }

        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
        DWORD,
        float[],
        DWORD,
        LPCGUID
    ) override
    {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
        LPCGUID,
        LPCGUID
    ) override
    {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnStateChanged(
        AudioSessionState
    ) override
    {
        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
        AudioSessionDisconnectReason
    ) override
    {
        if (g_applyEvent)
            SetEvent(g_applyEvent);

        return S_OK;
    }
};


// ============================================================================
// Test sound
// ============================================================================

static void PlayTestSound()
{
    MessageBeep(
        MB_ICONEXCLAMATION
    );
}


// ============================================================================
// Worker thread
// ============================================================================

static DWORD WINAPI AudioWorkerThread(
    LPVOID
)
{
    HRESULT hr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"CoInitializeEx failed: 0x%08X",
            hr
        );

        return 0;
    }

    IMMDeviceEnumerator* deviceEnumerator = nullptr;

    hr =
        CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(
                &deviceEnumerator
            )
        );

    if (FAILED(hr))
    {
        Wh_Log(
            L"CoCreateInstance failed: 0x%08X",
            hr
        );

        CoUninitialize();

        return 0;
    }

    AudioDeviceNotification* deviceNotification =
        new AudioDeviceNotification();

    AudioSessionNotification* sessionNotification =
        new AudioSessionNotification();

    AudioSessionEvents* sessionEvents =
        new AudioSessionEvents();

    IAudioSessionManager2* registeredManager = nullptr;
    IAudioSessionControl* registeredSystemSession = nullptr;

    std::wstring currentEndpointId;

    auto UnregisterAudioNotifications =
        [&]()
        {
            if (
                registeredSystemSession &&
                sessionEvents
            )
            {
                registeredSystemSession->
                    UnregisterAudioSessionNotification(
                        sessionEvents
                    );
            }

            if (
                registeredManager &&
                sessionNotification
            )
            {
                registeredManager->
                    UnregisterSessionNotification(
                        sessionNotification
                    );
            }

            if (registeredSystemSession)
            {
                registeredSystemSession->Release();
                registeredSystemSession = nullptr;
            }

            if (registeredManager)
            {
                registeredManager->Release();
                registeredManager = nullptr;
            }
        };

    auto RegisterAudioNotifications =
        [&]()
        {
            UnregisterAudioNotifications();

            IMMDevice* device = nullptr;

            HRESULT registerHr =
                deviceEnumerator->
                    GetDefaultAudioEndpoint(
                        eRender,
                        eConsole,
                        &device
                    );

            if (FAILED(registerHr))
                return;

            registerHr =
                device->
                    Activate(
                        __uuidof(IAudioSessionManager2),
                        CLSCTX_INPROC_SERVER,
                        nullptr,
                        reinterpret_cast<void**>(
                            &registeredManager
                        )
                    );

            if (FAILED(registerHr))
            {
                device->Release();
                registeredManager = nullptr;
                return;
            }

            registerHr =
                registeredManager->
                    RegisterSessionNotification(
                        sessionNotification
                    );

            if (FAILED(registerHr))
            {
                registeredManager->Release();
                registeredManager = nullptr;

                device->Release();
                return;
            }

            IAudioSessionEnumerator* enumerator =
                nullptr;

            registerHr =
                registeredManager->
                    GetSessionEnumerator(
                        &enumerator
                    );

            if (SUCCEEDED(registerHr))
            {
                int count = 0;

                enumerator->GetCount(&count);

                for (int i = 0; i < count; i++)
                {
                    IAudioSessionControl* session =
                        nullptr;

                    if (
                        FAILED(
                            enumerator->GetSession(
                                i,
                                &session
                            )
                        )
                    )
                    {
                        continue;
                    }

                    IAudioSessionControl2* session2 =
                        nullptr;

                    if (
                        SUCCEEDED(
                            session->QueryInterface(
                                __uuidof(
                                    IAudioSessionControl2
                                ),
                                reinterpret_cast<void**>(
                                    &session2
                                )
                            )
                        )
                    )
                    {
                        if (
                            session2->
                                IsSystemSoundsSession()
                            == S_OK
                        )
                        {
                            if (
                                SUCCEEDED(
                                    session->
                                        RegisterAudioSessionNotification(
                                            sessionEvents
                                        )
                                )
                            )
                            {
                                registeredSystemSession =
                                    session;

                                session->AddRef();
                            }

                            session2->Release();
                            session->Release();

                            break;
                        }

                        session2->Release();
                    }

                    session->Release();
                }

                enumerator->Release();
            }

            device->Release();
        };

    if (
        SUCCEEDED(
            deviceEnumerator->
                RegisterEndpointNotificationCallback(
                    deviceNotification
                )
        )
    )
    {
        // Registered successfully.
    }
    else
    {
        Wh_Log(
            L"RegisterEndpointNotificationCallback failed"
        );
    }

    RegisterAudioNotifications();

    // Initial application.
    ApplySystemSoundsVolume(
        deviceEnumerator,
        &currentEndpointId
    );

    g_activeEndpointId =
        currentEndpointId;

    HANDLE waitHandles[2] =
    {
        g_stopEvent,
        g_applyEvent
    };

    bool running = true;

    while (running)
    {
        DWORD result =
            WaitForMultipleObjects(
                2,
                waitHandles,
                FALSE,
                INFINITE
            );

        if (result == WAIT_OBJECT_0)
        {
            running = false;
            break;
        }

        if (result == WAIT_OBJECT_0 + 1)
        {
            ResetEvent(
                g_applyEvent
            );

            std::wstring newEndpointId;

            // Detect endpoint changes.
            IMMDevice* newDevice = nullptr;

            HRESULT deviceHr =
                deviceEnumerator->
                    GetDefaultAudioEndpoint(
                        eRender,
                        eConsole,
                        &newDevice
                    );

            if (SUCCEEDED(deviceHr))
            {
                LPWSTR rawId = nullptr;

                if (
                    SUCCEEDED(
                        newDevice->GetId(
                            &rawId
                        )
                    )
                )
                {
                    newEndpointId = rawId;

                    CoTaskMemFree(rawId);
                }

                newDevice->Release();
            }

            if (
                !g_activeEndpointId.empty() &&
                !newEndpointId.empty() &&
                g_activeEndpointId != newEndpointId
            )
            {
                float originalVolume = 0.0f;

                if (
                    FindSavedEndpoint(
                        g_activeEndpointId,
                        &originalVolume
                    )
                )
                {
                    RestoreEndpoint(
                        deviceEnumerator,
                        g_activeEndpointId,
                        originalVolume
                    );
                }
            }

            // Re-register notifications for the current device.
            RegisterAudioNotifications();

            ApplySystemSoundsVolume(
                deviceEnumerator,
                &newEndpointId
            );

            g_activeEndpointId =
                newEndpointId;

            if (
                g_testSoundPending.exchange(
                    false,
                    std::memory_order_acq_rel
                )
            )
            {
                PlayTestSound();
            }
        }
    }

    UnregisterAudioNotifications();

    deviceEnumerator->
        UnregisterEndpointNotificationCallback(
            deviceNotification
        );

    deviceNotification->Release();
    sessionNotification->Release();
    sessionEvents->Release();

    deviceEnumerator->Release();

    CoUninitialize();

    return 0;
}


// ============================================================================
// Tool callbacks
// ============================================================================

BOOL WhTool_ModInit()
{
    Wh_Log(
        L"System Sounds Volume 2.0.0 - The.BARDIA"
    );

    LoadSettings();

    // Recover values saved by a previous instance.
    // This is useful if the previous process terminated unexpectedly.
    HRESULT hr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    if (SUCCEEDED(hr))
    {
        LoadSavedEndpoints();

        if (!g_savedEndpoints.empty())
        {
            RestoreAllSavedEndpoints();
        }

        CoUninitialize();
    }

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_stopEvent)
    {
        Wh_Log(
            L"CreateEvent(stop) failed"
        );

        return FALSE;
    }

    g_applyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_applyEvent)
    {
        Wh_Log(
            L"CreateEvent(apply) failed"
        );

        CloseHandle(
            g_stopEvent
        );

        g_stopEvent = nullptr;

        return FALSE;
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            AudioWorkerThread,
            nullptr,
            0,
            nullptr
        );

    if (!g_workerThread)
    {
        Wh_Log(
            L"CreateThread failed"
        );

        CloseHandle(
            g_applyEvent
        );

        CloseHandle(
            g_stopEvent
        );

        g_applyEvent = nullptr;
        g_stopEvent = nullptr;

        return FALSE;
    }

    return TRUE;
}


void WhTool_ModSettingsChanged()
{
    bool previousTest =
        g_testSound.load(
            std::memory_order_acquire
        );

    int previousVolume =
        g_volume.load(
            std::memory_order_acquire
        );

    LoadSettings();

    int newVolume =
        g_volume.load(
            std::memory_order_acquire
        );

    bool newTest =
        g_testSound.load(
            std::memory_order_acquire
        );

    if (
        newTest &&
        (
            !previousTest ||
            previousVolume != newVolume
        )
    )
    {
        g_testSoundPending.store(
            true,
            std::memory_order_release
        );
    }

    if (g_applyEvent)
        SetEvent(
            g_applyEvent
        );
}


void WhTool_ModUninit()
{
    if (g_stopEvent)
    {
        SetEvent(
            g_stopEvent
        );
    }

    if (g_workerThread)
    {
        // Never unload the mod while the worker is still executing.
        WaitForSingleObject(
            g_workerThread,
            INFINITE
        );

        CloseHandle(
            g_workerThread
        );

        g_workerThread = nullptr;
    }

    if (g_applyEvent)
    {
        CloseHandle(
            g_applyEvent
        );

        g_applyEvent = nullptr;
    }

    if (g_stopEvent)
    {
        CloseHandle(
            g_stopEvent
        );

        g_stopEvent = nullptr;
    }

    // Restore all volumes changed by this mod.
    HRESULT hr =
        CoInitializeEx(
            nullptr,
            COINIT_MULTITHREADED
        );

    if (SUCCEEDED(hr))
    {
        LoadSavedEndpoints();

        RestoreAllSavedEndpoints();

        CoUninitialize();
    }
}


// ============================================================================
// Windhawk Tool Mod launcher
// ============================================================================

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject into
// other processes or hook other functions. Context:
//
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook()
{
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit()
{
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;

    LPWSTR* argv =
        CommandLineToArgvW(
            GetCommandLine(),
            &argc
        );

    if (!argv)
    {
        Wh_Log(
            L"CommandLineToArgvW failed"
        );

        return FALSE;
    }

    for (int i = 1; i < argc; i++)
    {
        if (
            wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0
        )
        {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++)
    {
        if (
            wcscmp(
                argv[i],
                L"-tool-mod"
            ) == 0
        )
        {
            isToolModProcess = true;

            if (
                wcscmp(
                    argv[i + 1],
                    WH_MOD_ID
                ) == 0
            )
            {
                isCurrentToolModProcess = true;
            }

            break;
        }
    }

    LocalFree(argv);

    if (isExcluded)
    {
        return FALSE;
    }

    if (isCurrentToolModProcess)
    {
        g_toolModProcessMutex =
            CreateMutex(
                nullptr,
                TRUE,
                L"windhawk-tool-mod_" WH_MOD_ID
            );

        if (!g_toolModProcessMutex)
        {
            Wh_Log(
                L"CreateMutex failed"
            );

            ExitProcess(1);
        }

        if (
            GetLastError() ==
            ERROR_ALREADY_EXISTS
        )
        {
            Wh_Log(
                L"Tool mod already running (%s)",
                WH_MOD_ID
            );

            ExitProcess(1);
        }

        if (!WhTool_ModInit())
        {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)
                GetModuleHandle(nullptr);

        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)
                (
                    (BYTE*)dosHeader +
                    dosHeader->e_lfanew
                );

        DWORD entryPointRVA =
            ntHeaders->
                OptionalHeader.
                AddressOfEntryPoint;

        void* entryPoint =
            (BYTE*)dosHeader +
            entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            (void*)EntryPoint_Hook,
            nullptr
        );

        return TRUE;
    }

    if (isToolModProcess)
    {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;

    return TRUE;
}


void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher)
    {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];

    switch (
        GetModuleFileName(
            nullptr,
            currentProcessPath,
            ARRAYSIZE(currentProcessPath)
        )
    )
    {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(
                L"GetModuleFileName failed"
            );

            return;
    }

    WCHAR commandLine[
        MAX_PATH +
        2 +
        (
            sizeof(
                L" -tool-mod \"" WH_MOD_ID "\""
            ) / sizeof(WCHAR)
        ) -
        1
    ];

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule =
        GetModuleHandle(
            L"kernelbase.dll"
        );

    if (!kernelModule)
    {
        kernelModule =
            GetModuleHandle(
                L"kernel32.dll"
            );

        if (!kernelModule)
        {
            Wh_Log(
                L"No kernelbase.dll/kernel32.dll"
            );

            return;
        }
    }

    using CreateProcessInternalW_t =
        BOOL(WINAPI*)(
            HANDLE hUserToken,
            LPCWSTR lpApplicationName,
            LPWSTR lpCommandLine,
            LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes,
            WINBOOL bInheritHandles,
            DWORD dwCreationFlags,
            LPVOID lpEnvironment,
            LPCWSTR lpCurrentDirectory,
            LPSTARTUPINFOW lpStartupInfo,
            LPPROCESS_INFORMATION lpProcessInformation,
            PHANDLE hRestrictedUserToken
        );

    CreateProcessInternalW_t
        pCreateProcessInternalW =
            (CreateProcessInternalW_t)
                GetProcAddress(
                    kernelModule,
                    "CreateProcessInternalW"
                );

    if (!pCreateProcessInternalW)
    {
        Wh_Log(
            L"No CreateProcessInternalW"
        );

        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };

    PROCESS_INFORMATION pi;

    if (
        !pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr
        )
    )
    {
        Wh_Log(
            L"CreateProcess failed"
        );

        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


void Wh_ModSettingsChanged()
{
    if (g_isToolModProcessLauncher)
    {
        return;
    }

    WhTool_ModSettingsChanged();
}


void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher)
    {
        return;
    }

    WhTool_ModUninit();

    ExitProcess(0);
}

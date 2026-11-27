// ==WindhawkMod==
// @id              bluetooth-no-sleep
// @name            Bluetooth No Sleep
// @description     Prevents Bluetooth audio devices from sleeping, disconnecting on idle timeout, or truncating initial playback by streaming inaudible digital silence
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bluetooth No Sleep (Bluetooth Audio Keep-Alive)

A lightweight Windows background utility and Windhawk tool mod that prevents
Bluetooth audio devices (headphones, wireless earbuds, soundbars, DACs, and
speakers) from falling asleep, dropping initial playback, or disconnecting
during silence.

### Why this mod?
Modern Bluetooth audio peripherals employ aggressive power-saving timers:
1. **Initial Audio Truncation:** When no audio plays for a few seconds, the
   device enters a low-power state and shuts off its internal DAC/amplifier.
   When audio resumes, waking up takes 1-2 seconds, dropping the beginning of
   system notifications, speech, or media playback.
2. **Pops and Static Bursts:** Rapid DAC wakeups often create audible transients,
   clicks, or static bursts ("pink noise").
3. **Unsolicited Idle Disconnections:** Many wireless devices shut down
   completely after 5 to 10 minutes of silence, forcing manual power cycling.

### How it works
This mod uses Windows Audio Session API (WASAPI) Shared Mode to continuously
stream an inaudible digital silence buffer (`0x00` PCM or sub-audible dither)
to the active audio endpoint:
- **Non-Intrusive Shared Mode:** Runs alongside your normal applications
  without monopolizing or locking audio devices.
- **Event-Driven Endpoint Tracking:** Uses `IMMNotificationClient` to monitor
  device transitions. When your default audio device switches or a Bluetooth
  headset reconnects, it rebinds automatically without crashes.
- **Near-Zero CPU Overhead:** Uses buffer-synchronized wait events and sleep
  cycles to keep CPU usage below 0.1%.
- **Optional Inactivity Gate:** Can automatically pause the keep-alive stream
  after prolonged user inactivity to allow intentional battery conservation
  and permit normal Windows system sleep (`powercfg /requests`).
- **Dedicated Tool Mod:** Runs inside `windhawk.exe` as a dedicated background
  tool without injecting into or hooking any system processes.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- targetMode: bluetoothOnly
  $name: Target Playback Endpoints
  $description: Choose which audio playback devices should be kept awake.
  $options:
  - bluetoothOnly: Default Device (Only if Bluetooth transport detected, recommended)
  - defaultDevice: Default Playback Device (Tracks system default output regardless of transport)
  - allDevices: All Active Playback Devices (Keeps all connected outputs awake)

- silenceMode: zero
  $name: Silence Generation Mode
  $description: Format of keep-alive signal pumped to endpoints.
  $options:
  - zero: Pure Digital Silence (Zero-amplitude PCM 0x00, standard)
  - dither: Sub-audible Dither (Alternating 1-LSB dither for stubborn DACs)

- keepAliveStrategy: continuous
  $name: Keep-Alive Strategy
  $description: Choose whether to maintain the stream continuously or pause during prolonged inactivity.
  $options:
  - continuous: Continuous (Always awake while device is connected)
  - timeoutGate: Inactivity Timeout Gate (Pause keep-alive during prolonged user idle)

- idleTimeoutMinutes: 30
  $name: Inactivity Timeout Duration (minutes)
  $description: "Number of minutes of user inactivity before pausing the keep-alive stream (Range: 1-240 minutes)."
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>
#include <windhawk_utils.h>

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <algorithm>
#include <cwctype>

// Static subformat GUID definitions for WAVEFORMATEXTENSIBLE without requiring ksuser.lib
static const GUID GUID_SUBTYPE_PCM =
    { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
static const GUID GUID_SUBTYPE_IEEE_FLOAT =
    { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

constexpr REFERENCE_TIME REFTIMES_PER_SEC = 10000000;
constexpr REFERENCE_TIME BUFFER_DURATION_HNS = (REFTIMES_PER_SEC / 5); // 200 ms buffer

enum class TargetMode {
    BluetoothOnly,
    DefaultDevice,
    AllDevices
};

enum class SilenceMode {
    Zero,
    Dither
};

enum class KeepAliveStrategy {
    Continuous,
    TimeoutGate
};

struct ModSettings {
    TargetMode targetMode = TargetMode::BluetoothOnly;
    SilenceMode silenceMode = SilenceMode::Zero;
    KeepAliveStrategy strategy = KeepAliveStrategy::Continuous;
    int idleTimeoutMinutes = 30;
};

static ModSettings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static HANDLE g_hStopEvent = NULL;
static HANDLE g_hReconfigureEvent = NULL;
static HANDLE g_hManagerThread = NULL;

static bool IsBluetoothDevice(IMMDevice* pDevice) {
    if (!pDevice) return false;

    IPropertyStore* pProps = nullptr;
    HRESULT hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (FAILED(hr) || !pProps) return false;

    bool isBluetooth = false;
    PROPVARIANT var;
    PropVariantInit(&var);

    // 1. Check Enumerator Name (BTHENUM, BTHHFENUM, BTHLEENUM, etc.)
    if (SUCCEEDED(pProps->GetValue(PKEY_Device_EnumeratorName, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
        if (wcsstr(var.pwszVal, L"BTH") != nullptr) {
            isBluetooth = true;
        }
    }
    PropVariantClear(&var);

    // 2. Check Instance ID for Bluetooth hardware bus identifiers
    if (!isBluetooth && SUCCEEDED(pProps->GetValue(PKEY_Device_InstanceId, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
        std::wstring id = var.pwszVal;
        std::transform(id.begin(), id.end(), id.begin(), ::towlower);
        if (id.find(L"bthenum") != std::wstring::npos ||
            id.find(L"bthhfenum") != std::wstring::npos ||
            id.find(L"bluetooth") != std::wstring::npos) {
            isBluetooth = true;
        }
    }
    PropVariantClear(&var);

    pProps->Release();
    return isBluetooth;
}

static std::wstring GetDeviceFriendlyName(IMMDevice* pDevice) {
    if (!pDevice) return L"Unknown Device";

    IPropertyStore* pProps = nullptr;
    HRESULT hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (FAILED(hr) || !pProps) return L"Unknown Device";

    std::wstring name = L"Unknown Device";
    PROPVARIANT var;
    PropVariantInit(&var);

    if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
        name = var.pwszVal;
    }
    PropVariantClear(&var);
    pProps->Release();

    return name;
}

class AudioStreamSession {
public:
    std::wstring deviceId;
    std::wstring friendlyName;
    bool isBluetooth = false;

    AudioStreamSession(const std::wstring& id, const std::wstring& name, bool bth)
        : deviceId(id), friendlyName(name), isBluetooth(bth),
          m_pDevice(nullptr), m_pAudioClient(nullptr), m_pRenderClient(nullptr),
          m_pwfx(nullptr), m_hBufferEvent(NULL), m_hThread(NULL),
          m_bStop(false), m_bPaused(false), m_bEventMode(false),
          m_bufferFrameCount(0) {}

    ~AudioStreamSession() {
        Stop();
    }

    bool Start(IMMDevice* pDevice) {
        if (!pDevice) return false;

        m_pDevice = pDevice;
        m_pDevice->AddRef();

        HRESULT hr = m_pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pAudioClient);
        if (FAILED(hr) || !m_pAudioClient) {
            Wh_Log(L"Failed to activate IAudioClient on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        hr = m_pAudioClient->GetMixFormat(&m_pwfx);
        if (FAILED(hr) || !m_pwfx) {
            Wh_Log(L"GetMixFormat failed on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        // Attempt event-driven shared mode
        m_bEventMode = true;
        hr = m_pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
            BUFFER_DURATION_HNS,
            0,
            m_pwfx,
            nullptr
        );

        if (FAILED(hr)) {
            // Fallback to timer-driven shared mode
            m_bEventMode = false;
            hr = m_pAudioClient->Initialize(
                AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_NOPERSIST,
                BUFFER_DURATION_HNS,
                0,
                m_pwfx,
                nullptr
            );
        }

        if (FAILED(hr)) {
            Wh_Log(L"AudioClient Initialize failed on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        if (m_bEventMode) {
            m_hBufferEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (!m_hBufferEvent || FAILED(m_pAudioClient->SetEventHandle(m_hBufferEvent))) {
                m_bEventMode = false;
            }
        }

        hr = m_pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);
        if (FAILED(hr) || !m_pRenderClient) {
            Wh_Log(L"GetService(IAudioRenderClient) failed on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        hr = m_pAudioClient->GetBufferSize(&m_bufferFrameCount);
        if (FAILED(hr) || m_bufferFrameCount == 0) {
            Wh_Log(L"GetBufferSize failed on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        // Set friendly display name in Windows Volume Mixer
        IAudioSessionControl* pSessionControl = nullptr;
        if (SUCCEEDED(m_pAudioClient->GetService(__uuidof(IAudioSessionControl), (void**)&pSessionControl)) && pSessionControl) {
            pSessionControl->SetDisplayName(L"Bluetooth No Sleep", nullptr);
            pSessionControl->Release();
        }

        // Pre-fill buffer with digital silence before starting playback
        BYTE* pBuffer = nullptr;
        if (SUCCEEDED(m_pRenderClient->GetBuffer(m_bufferFrameCount, &pBuffer)) && pBuffer) {
            memset(pBuffer, 0, m_bufferFrameCount * m_pwfx->nBlockAlign);
            m_pRenderClient->ReleaseBuffer(m_bufferFrameCount, 0);
        }

        hr = m_pAudioClient->Start();
        if (FAILED(hr)) {
            Wh_Log(L"AudioClient Start failed on %s (0x%08X)", friendlyName.c_str(), hr);
            return false;
        }

        m_bStop = false;
        m_bPaused = false;
        m_hThread = CreateThread(nullptr, 0, StreamThreadStatic, this, 0, nullptr);
        if (!m_hThread) {
            Wh_Log(L"Failed to create stream thread for %s", friendlyName.c_str());
            m_pAudioClient->Stop();
            return false;
        }

        Wh_Log(L"Keep-alive active on: %s (%s)",
               friendlyName.c_str(),
               isBluetooth ? L"Bluetooth" : L"Direct/Other");
        return true;
    }

    void Stop() {
        m_bStop = true;

        if (m_hBufferEvent) {
            SetEvent(m_hBufferEvent);
        }

        if (m_hThread) {
            WaitForSingleObject(m_hThread, INFINITE);
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }

        if (m_pAudioClient) {
            m_pAudioClient->Stop();
        }

        if (m_hBufferEvent) {
            CloseHandle(m_hBufferEvent);
            m_hBufferEvent = NULL;
        }

        if (m_pRenderClient) {
            m_pRenderClient->Release();
            m_pRenderClient = nullptr;
        }

        if (m_pAudioClient) {
            m_pAudioClient->Release();
            m_pAudioClient = nullptr;
        }

        if (m_pwfx) {
            CoTaskMemFree(m_pwfx);
            m_pwfx = nullptr;
        }

        if (m_pDevice) {
            m_pDevice->Release();
            m_pDevice = nullptr;
        }
    }

    void SetPaused(bool pause) {
        if (m_bPaused == pause) return;
        m_bPaused = pause;

        if (m_pAudioClient) {
            if (pause) {
                m_pAudioClient->Stop();
                Wh_Log(L"Inactivity timeout reached: Keep-alive paused for %s", friendlyName.c_str());
            } else {
                m_pAudioClient->Start();
                Wh_Log(L"User activity resumed: Keep-alive resumed for %s", friendlyName.c_str());
            }
        }
    }

    bool IsPaused() const { return m_bPaused; }

    bool IsRunning() const {
        return m_hThread && WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT;
    }

private:
    IMMDevice* m_pDevice;
    IAudioClient* m_pAudioClient;
    IAudioRenderClient* m_pRenderClient;
    WAVEFORMATEX* m_pwfx;
    HANDLE m_hBufferEvent;
    HANDLE m_hThread;
    std::atomic<bool> m_bStop;
    std::atomic<bool> m_bPaused;
    bool m_bEventMode;
    UINT32 m_bufferFrameCount;

    static DWORD WINAPI StreamThreadStatic(LPVOID lpParam) {
        AudioStreamSession* self = static_cast<AudioStreamSession*>(lpParam);
        self->StreamLoop();
        return 0;
    }

    void StreamLoop() {
        bool ditherState = false;
        bool exitWithError = false;

        // Inspect format characteristics for dither generation
        bool isFloat = false;
        bool is16BitPCM = false;

        if (m_pwfx) {
            if (m_pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
                isFloat = true;
            } else if (m_pwfx->wFormatTag == WAVE_FORMAT_PCM && m_pwfx->wBitsPerSample == 16) {
                is16BitPCM = true;
            } else if (m_pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                const auto* pEx = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(m_pwfx);
                if (IsEqualGUID(pEx->SubFormat, GUID_SUBTYPE_IEEE_FLOAT) && m_pwfx->wBitsPerSample == 32) {
                    isFloat = true;
                } else if (IsEqualGUID(pEx->SubFormat, GUID_SUBTYPE_PCM) && m_pwfx->wBitsPerSample == 16) {
                    is16BitPCM = true;
                }
            }
        }

        while (!m_bStop) {
            if (m_bPaused) {
                Sleep(200);
                continue;
            }

            if (m_bEventMode && m_hBufferEvent) {
                HANDLE waitHandles[2] = { g_hStopEvent, m_hBufferEvent };
                DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, 200);
                if (waitRes == WAIT_OBJECT_0 || m_bStop) {
                    break;
                }
            } else {
                Sleep(50); // Fallback for timer-driven mode
            }

            if (m_bStop) break;

            UINT32 padding = 0;
            HRESULT hr = m_pAudioClient->GetCurrentPadding(&padding);
            if (FAILED(hr)) {
                Wh_Log(L"GetCurrentPadding failed on %s (0x%08X)", friendlyName.c_str(), hr);
                exitWithError = true;
                break;
            }

            UINT32 framesNeeded = (m_bufferFrameCount > padding) ? (m_bufferFrameCount - padding) : 0;
            if (framesNeeded == 0) continue;

            BYTE* pData = nullptr;
            hr = m_pRenderClient->GetBuffer(framesNeeded, &pData);
            if (FAILED(hr) || !pData) {
                Wh_Log(L"GetBuffer failed on %s (0x%08X)", friendlyName.c_str(), hr);
                exitWithError = true;
                break;
            }

            AcquireSRWLockShared(&g_settingsLock);
            SilenceMode sMode = g_settings.silenceMode;
            ReleaseSRWLockShared(&g_settingsLock);

            if (sMode == SilenceMode::Zero) {
                memset(pData, 0, framesNeeded * m_pwfx->nBlockAlign);
            } else {
                // Sub-audible 1-LSB dither (~ -90 dBFS, alternating per sample)
                if (isFloat) {
                    float* pSamples = reinterpret_cast<float*>(pData);
                    size_t sampleCount = framesNeeded * m_pwfx->nChannels;
                    for (size_t i = 0; i < sampleCount; ++i) {
                        ditherState = !ditherState;
                        pSamples[i] = ditherState ? (1.0f / 32768.0f) : (-1.0f / 32768.0f);
                    }
                } else if (is16BitPCM) {
                    int16_t* pSamples = reinterpret_cast<int16_t*>(pData);
                    size_t sampleCount = framesNeeded * m_pwfx->nChannels;
                    for (size_t i = 0; i < sampleCount; ++i) {
                        ditherState = !ditherState;
                        pSamples[i] = ditherState ? 1 : -1;
                    }
                } else {
                    memset(pData, 0, framesNeeded * m_pwfx->nBlockAlign);
                }
            }

            hr = m_pRenderClient->ReleaseBuffer(framesNeeded, 0);
            if (FAILED(hr)) {
                Wh_Log(L"ReleaseBuffer failed on %s (0x%08X)", friendlyName.c_str(), hr);
                exitWithError = true;
                break;
            }
        }

        if (exitWithError && !m_bStop) {
            Wh_Log(L"Audio endpoint %s encountered an error, signaling reconfigure.", friendlyName.c_str());
            SetEvent(g_hReconfigureEvent);
        }
    }
};

class AudioKeepAliveManager : public IMMNotificationClient {
public:
    AudioKeepAliveManager() : m_cRef(1), m_pEnumerator(nullptr) {}

    ~AudioKeepAliveManager() {
        StopAll();
        if (m_pEnumerator) {
            m_pEnumerator->UnregisterEndpointNotificationCallback(this);
            m_pEnumerator->Release();
            m_pEnumerator = nullptr;
        }
    }

    bool Initialize() {
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            nullptr,
            CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            (void**)&m_pEnumerator
        );

        if (FAILED(hr) || !m_pEnumerator) {
            Wh_Log(L"Failed to create MMDeviceEnumerator (0x%08X)", hr);
            return false;
        }

        hr = m_pEnumerator->RegisterEndpointNotificationCallback(this);
        if (FAILED(hr)) {
            Wh_Log(L"Warning: RegisterEndpointNotificationCallback failed (0x%08X)", hr);
        }

        Reconfigure();
        return true;
    }

    void Reconfigure() {
        AcquireSRWLockShared(&g_settingsLock);
        TargetMode targetMode = g_settings.targetMode;
        ReleaseSRWLockShared(&g_settingsLock);

        if (!m_pEnumerator) return;

        if (targetMode == TargetMode::AllDevices) {
            ConfigureAllDevices();
        } else {
            ConfigureDefaultDevice(targetMode == TargetMode::BluetoothOnly);
        }
    }

    void CheckIdleTimeout() {
        AcquireSRWLockShared(&g_settingsLock);
        KeepAliveStrategy strategy = g_settings.strategy;
        int timeoutMinutes = g_settings.idleTimeoutMinutes;
        ReleaseSRWLockShared(&g_settingsLock);

        if (strategy != KeepAliveStrategy::TimeoutGate) {
            // Restore playback if strategy switched back to Continuous
            for (auto& session : m_sessions) {
                if (session->IsPaused()) {
                    session->SetPaused(false);
                }
            }
            return;
        }

        LASTINPUTINFO lii{ .cbSize = sizeof(LASTINPUTINFO) };
        if (!GetLastInputInfo(&lii)) return;

        DWORD idleMs = GetTickCount() - lii.dwTime;
        bool shouldPause = (idleMs >= static_cast<DWORD>(timeoutMinutes * 60 * 1000));

        for (auto& session : m_sessions) {
            session->SetPaused(shouldPause);
        }
    }

    void EnsureActiveStreams() {
        // If there are no running sessions, attempt reconfigure (e.g. exclusive lock was released)
        bool hasRunning = false;
        for (const auto& s : m_sessions) {
            if (s->IsRunning()) {
                hasRunning = true;
                break;
            }
        }
        if (!hasRunning) {
            Reconfigure();
        }
    }

    void StopAll() {
        for (auto& session : m_sessions) {
            session->Stop();
        }
        m_sessions.clear();
    }

    // IMMNotificationClient Implementation
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG ulRef = InterlockedDecrement(&m_cRef);
        if (ulRef == 0) {
            delete this;
        }
        return ulRef;
    }

    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) override {
        if (flow == eRender && (role == eMultimedia || role == eConsole)) {
            Wh_Log(L"Default audio endpoint changed: %s", pwstrDeviceId ? pwstrDeviceId : L"None");
            SetEvent(g_hReconfigureEvent);
        }
        return S_OK;
    }

    STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override {
        if (dwNewState == DEVICE_STATE_ACTIVE || dwNewState == DEVICE_STATE_UNPLUGGED || dwNewState == DEVICE_STATE_NOTPRESENT) {
            SetEvent(g_hReconfigureEvent);
        }
        return S_OK;
    }

    STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
        SetEvent(g_hReconfigureEvent);
        return S_OK;
    }

    STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId) override {
        SetEvent(g_hReconfigureEvent);
        return S_OK;
    }

    STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override {
        return S_OK;
    }

private:
    LONG m_cRef;
    IMMDeviceEnumerator* m_pEnumerator;
    std::vector<std::unique_ptr<AudioStreamSession>> m_sessions;

    void ConfigureDefaultDevice(bool bluetoothOnly) {
        IMMDevice* pDefaultDevice = nullptr;
        HRESULT hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
        if (FAILED(hr) || !pDefaultDevice) {
            StopAll();
            return;
        }

        LPWSTR pwstrId = nullptr;
        pDefaultDevice->GetId(&pwstrId);
        std::wstring currentId = pwstrId ? pwstrId : L"";
        if (pwstrId) CoTaskMemFree(pwstrId);

        std::wstring friendlyName = GetDeviceFriendlyName(pDefaultDevice);
        bool isBth = IsBluetoothDevice(pDefaultDevice);

        if (bluetoothOnly && !isBth) {
            StopAll();
            pDefaultDevice->Release();
            return;
        }

        // Check if existing session already targets this device and is actively running
        if (m_sessions.size() == 1 && m_sessions[0]->deviceId == currentId && m_sessions[0]->IsRunning()) {
            pDefaultDevice->Release();
            return; // Already streaming cleanly
        }

        StopAll();

        auto session = std::make_unique<AudioStreamSession>(currentId, friendlyName, isBth);
        if (session->Start(pDefaultDevice)) {
            m_sessions.push_back(std::move(session));
        }

        pDefaultDevice->Release();
    }

    void ConfigureAllDevices() {
        // Prune dead sessions before enumerating
        m_sessions.erase(
            std::remove_if(m_sessions.begin(), m_sessions.end(),
                [](const std::unique_ptr<AudioStreamSession>& s) {
                    return !s->IsRunning();
                }),
            m_sessions.end()
        );

        IMMDeviceCollection* pCollection = nullptr;
        HRESULT hr = m_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr) || !pCollection) {
            StopAll();
            return;
        }

        UINT count = 0;
        pCollection->GetCount(&count);

        std::vector<std::wstring> activeIds;
        for (UINT i = 0; i < count; ++i) {
            IMMDevice* pDevice = nullptr;
            if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
                LPWSTR pwstrId = nullptr;
                pDevice->GetId(&pwstrId);
                std::wstring devId = pwstrId ? pwstrId : L"";
                if (pwstrId) CoTaskMemFree(pwstrId);

                activeIds.push_back(devId);

                bool alreadyRunning = false;
                for (auto& s : m_sessions) {
                    if (s->deviceId == devId && s->IsRunning()) {
                        alreadyRunning = true;
                        break;
                    }
                }

                if (!alreadyRunning) {
                    std::wstring name = GetDeviceFriendlyName(pDevice);
                    bool isBth = IsBluetoothDevice(pDevice);
                    auto session = std::make_unique<AudioStreamSession>(devId, name, isBth);
                    if (session->Start(pDevice)) {
                        m_sessions.push_back(std::move(session));
                    }
                }

                pDevice->Release();
            }
        }

        pCollection->Release();

        // Prune removed sessions
        m_sessions.erase(
            std::remove_if(m_sessions.begin(), m_sessions.end(),
                [&activeIds](const std::unique_ptr<AudioStreamSession>& s) {
                    return std::find(activeIds.begin(), activeIds.end(), s->deviceId) == activeIds.end();
                }),
            m_sessions.end()
        );
    }
};

static AudioKeepAliveManager* g_pManager = nullptr;

static DWORD WINAPI ManagerThreadProc(LPVOID lpParam) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to initialize COM on manager thread (0x%08X)", hr);
        return 1;
    }

    g_pManager = new AudioKeepAliveManager();
    if (!g_pManager->Initialize()) {
        delete g_pManager;
        g_pManager = nullptr;
        CoUninitialize();
        return 1;
    }

    const DWORD timeoutCheckIntervalMs = 2500;
    while (true) {
        HANDLE waitHandles[2] = { g_hStopEvent, g_hReconfigureEvent };
        DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, timeoutCheckIntervalMs);

        if (waitRes == WAIT_OBJECT_0) {
            // Stop requested
            break;
        } else if (waitRes == WAIT_OBJECT_0 + 1) {
            // Reconfigure requested
            if (g_pManager) {
                g_pManager->Reconfigure();
            }
        } else if (waitRes == WAIT_TIMEOUT) {
            // Periodic idle timeout check and stream health check
            if (g_pManager) {
                g_pManager->CheckIdleTimeout();
                g_pManager->EnsureActiveStreams();
            }
        }
    }

    if (g_pManager) {
        g_pManager->StopAll();
        g_pManager->Release();
        g_pManager = nullptr;
    }

    CoUninitialize();
    return 0;
}

static void LoadSettings() {
    AcquireSRWLockExclusive(&g_settingsLock);

    // 1. targetMode
    auto targetMode = WindhawkUtils::StringSetting::make(L"targetMode");
    if (wcscmp(targetMode.get(), L"defaultDevice") == 0) {
        g_settings.targetMode = TargetMode::DefaultDevice;
    } else if (wcscmp(targetMode.get(), L"allDevices") == 0) {
        g_settings.targetMode = TargetMode::AllDevices;
    } else {
        g_settings.targetMode = TargetMode::BluetoothOnly;
    }

    // 2. silenceMode
    auto silenceMode = WindhawkUtils::StringSetting::make(L"silenceMode");
    if (wcscmp(silenceMode.get(), L"dither") == 0) {
        g_settings.silenceMode = SilenceMode::Dither;
    } else {
        g_settings.silenceMode = SilenceMode::Zero;
    }

    // 3. keepAliveStrategy
    auto strategy = WindhawkUtils::StringSetting::make(L"keepAliveStrategy");
    if (wcscmp(strategy.get(), L"timeoutGate") == 0) {
        g_settings.strategy = KeepAliveStrategy::TimeoutGate;
    } else {
        g_settings.strategy = KeepAliveStrategy::Continuous;
    }

    // 4. idleTimeoutMinutes (range: 1 - 240)
    int timeoutMin = Wh_GetIntSetting(L"idleTimeoutMinutes");
    if (timeoutMin < 1) timeoutMin = 1;
    if (timeoutMin > 240) timeoutMin = 240;
    g_settings.idleTimeoutMinutes = timeoutMin;

    ReleaseSRWLockExclusive(&g_settingsLock);
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Bluetooth No Sleep tool mod initializing...");

    g_hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_hReconfigureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (!g_hStopEvent || !g_hReconfigureEvent) {
        Wh_Log(L"Failed to create synchronization events.");
        return FALSE;
    }

    LoadSettings();

    g_hManagerThread = CreateThread(nullptr, 0, ManagerThreadProc, nullptr, 0, nullptr);
    if (!g_hManagerThread) {
        Wh_Log(L"Failed to launch manager thread.");
        return FALSE;
    }

    Wh_Log(L"Bluetooth No Sleep tool mod initialized successfully.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings change detected, reloading configuration...");
    LoadSettings();
    if (g_hReconfigureEvent) {
        SetEvent(g_hReconfigureEvent);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Bluetooth No Sleep tool mod uninitializing...");

    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    if (g_hManagerThread) {
        WaitForSingleObject(g_hManagerThread, 3000);
        CloseHandle(g_hManagerThread);
        g_hManagerThread = NULL;
    }

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    if (g_hReconfigureEvent) {
        CloseHandle(g_hReconfigureEvent);
        g_hReconfigureEvent = NULL;
    }

    Wh_Log(L"Bluetooth No Sleep tool mod uninitialized.");
}

// clang-format off
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
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

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") /
                       sizeof(WCHAR)) -
                      1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// clang-format on

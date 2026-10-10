// ==WindhawkMod==
// @id              audio-hardware-volume-preset
// @name            Audio Hardware Volume Preset
// @description     Sets audio playback devices to a configured volume whenever they connect
// @version         1.1.0
// @author          Community
// @github          https://github.com/
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Hardware Volume Preset

Automatically sets your headphones, speakers, Bluetooth headsets, USB DACs, or any other audio playback hardware to a specific preset volume whenever they connect.

## Key Features
- **Prevent Sudden Hearing Blasts:** Never plug in headphones only to be blasted by 100% volume again.
- **Smart Hardware Matching:** Supports flexible keyword and token matching. You can enter simple keywords (like `AirPods`, `Sony`, `DAC`, `Speakers`) or full device names.
- **Max Volume Safety Cap:** Set a safety ceiling (up to 100%) so no device ever exceeds your safe threshold.
- **Global Fallback:** Set any unrecognized audio device to a safe default volume (e.g., 30%).
- **Auto-Unmute:** Optionally unmute the device on connection.
- **Intelligent Debouncing:** Prevents duplicate triggers when Windows fires multiple rapid connection events.
- **Safe & Efficient:** Runs inside `explorer.exe` using standard Windows Core Audio event notifications (`IMMNotificationClient`). Zero polling, zero idle CPU usage.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultVolume: 30
  $name: Default Volume (%)
  $description: Master volume percentage (0-100) applied when any audio hardware connects, if no specific rule matches.

- maxVolumeLimit: 100
  $name: Max Volume Safety Limit (%)
  $description: Absolute maximum volume ceiling (0-100). No preset will ever exceed this value.

- applyToAllDevices: true
  $name: Apply to all devices by default
  $description: If enabled, any connected playback device without an explicit rule will use the Default Volume.

- autoUnmute: true
  $name: Unmute on connect
  $description: Automatically unmutes the audio device when setting its volume.

- applyOnStartup: true
  $name: Apply on mod startup
  $description: Immediately set volume for currently connected audio devices when the mod initializes.

- deviceRules:
  - - name: "AirPods"
      $name: Device Name or Keyword
    - volume: 30
      $name: Volume (%)
  - - name: "Sony"
      $name: Device Name or Keyword
    - volume: 25
      $name: Volume (%)
  - - name: "Speakers"
      $name: Device Name or Keyword
    - volume: 50
      $name: Volume (%)
  - - name: "USB DAC"
      $name: Device Name or Keyword
    - volume: 40
      $name: Volume (%)
  $name: Custom Device Presets
  $description: Custom volume percentages for specific hardware. Simply enter a keyword from your device name (e.g. AirPods, Sony, Speakers, DAC).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <windhawk_api.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

struct DeviceRule {
    std::wstring originalName;
    std::wstring nameLower;
    std::vector<std::wstring> tokens;
    int volume;
};

// Global Configuration & State
static SRWLOCK g_lock = SRWLOCK_INIT;
static int g_defaultVolume = 30;
static int g_maxVolumeLimit = 100;
static BOOL g_applyToAllDevices = TRUE;
static BOOL g_autoUnmute = TRUE;
static BOOL g_applyOnStartup = TRUE;
static std::vector<DeviceRule> g_rules;

// Debouncing state to prevent rapid duplicate events from Windows
static std::wstring g_lastDeviceId;
static ULONGLONG g_lastApplyTick = 0;

// COM & Worker Thread handles
static HANDLE g_hWorkerThread = NULL;
static HANDLE g_hStopEvent = NULL;
static IMMDeviceEnumerator* g_pEnumerator = NULL;

static std::wstring ToLower(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return result;
}

// Splits a string into alphanumeric tokens, filtering out generic device words
static std::vector<std::wstring> GetTokens(const std::wstring& str) {
    std::vector<std::wstring> tokens;
    std::wstring cur;
    for (wchar_t ch : str) {
        if (iswalnum(ch)) {
            cur += (wchar_t)towlower(ch);
        } else {
            if (!cur.empty()) {
                if (cur != L"headphones" && cur != L"speakers" && cur != L"headset" && 
                    cur != L"earphone" && cur != L"earphones" && cur != L"audio" && 
                    cur != L"device" && cur != L"input" && cur != L"output") {
                    tokens.push_back(cur);
                }
                cur.clear();
            }
        }
    }
    if (!cur.empty()) {
        if (cur != L"headphones" && cur != L"speakers" && cur != L"headset" && 
            cur != L"earphone" && cur != L"earphones" && cur != L"audio" && 
            cur != L"device" && cur != L"input" && cur != L"output") {
            tokens.push_back(cur);
        }
    }
    return tokens;
}

// Checks if a candidate device string matches a rule
static bool CheckCandidateMatch(const std::wstring& candidate, const DeviceRule& rule) {
    if (candidate.empty() || rule.nameLower.empty()) return false;

    std::wstring candidateLower = ToLower(candidate);

    // 1. Direct substring match in either direction
    if (candidateLower.find(rule.nameLower) != std::wstring::npos ||
        rule.nameLower.find(candidateLower) != std::wstring::npos) {
        return true;
    }

    // 2. Token match (e.g. user entered "Headphones (3- Audiocular D07)" vs Windows "Speakers (3- Audiocular D07)")
    auto devTokens = GetTokens(candidateLower);
    for (const auto& rt : rule.tokens) {
        if (rt.length() >= 2) {
            for (const auto& dt : devTokens) {
                if (dt == rt || dt.find(rt) != std::wstring::npos || rt.find(dt) != std::wstring::npos) {
                    return true;
                }
            }
        }
    }

    return false;
}

static void LoadSettings() {
    AcquireSRWLockExclusive(&g_lock);

    g_defaultVolume = Wh_GetIntSetting(L"defaultVolume");
    if (g_defaultVolume < 0) g_defaultVolume = 0;
    if (g_defaultVolume > 100) g_defaultVolume = 100;

    g_maxVolumeLimit = Wh_GetIntSetting(L"maxVolumeLimit");
    if (g_maxVolumeLimit < 0) g_maxVolumeLimit = 0;
    if (g_maxVolumeLimit > 100) g_maxVolumeLimit = 100;

    g_applyToAllDevices = Wh_GetIntSetting(L"applyToAllDevices");
    g_autoUnmute = Wh_GetIntSetting(L"autoUnmute");
    g_applyOnStartup = Wh_GetIntSetting(L"applyOnStartup");

    g_rules.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR szKey[128];
        swprintf_s(szKey, L"deviceRules[%d].name", i);
        PCWSTR ruleName = Wh_GetStringSetting(szKey);
        if (!ruleName || !*ruleName) {
            if (ruleName) {
                Wh_FreeStringSetting(ruleName);
            }
            break;
        }

        swprintf_s(szKey, L"deviceRules[%d].volume", i);
        int ruleVol = Wh_GetIntSetting(szKey);
        if (ruleVol < 0) ruleVol = 0;
        if (ruleVol > 100) ruleVol = 100;

        DeviceRule rule;
        rule.originalName = ruleName;
        rule.nameLower = ToLower(ruleName);
        rule.tokens = GetTokens(rule.nameLower);
        rule.volume = ruleVol;
        g_rules.push_back(rule);

        Wh_FreeStringSetting(ruleName);
    }

    ReleaseSRWLockExclusive(&g_lock);

    Wh_Log(L"Settings loaded. Default Vol: %d%%, Max Limit: %d%%, Apply All: %d, Startup: %d, Rules count: %zu",
           g_defaultVolume, g_maxVolumeLimit, g_applyToAllDevices, g_applyOnStartup, g_rules.size());
}

static void ApplyVolumeToDevice(LPCWSTR pwstrDeviceId) {
    if (!pwstrDeviceId || !g_pEnumerator) {
        return;
    }

    ULONGLONG currentTick = GetTickCount64();

    // Check debounce (ignore duplicate notifications for same device within 1.5s)
    AcquireSRWLockExclusive(&g_lock);
    if (g_lastDeviceId == pwstrDeviceId && (currentTick - g_lastApplyTick) < 1500) {
        ReleaseSRWLockExclusive(&g_lock);
        return;
    }
    g_lastDeviceId = pwstrDeviceId;
    g_lastApplyTick = currentTick;
    ReleaseSRWLockExclusive(&g_lock);

    IMMDevice* pDevice = NULL;
    HRESULT hr = g_pEnumerator->GetDevice(pwstrDeviceId, &pDevice);
    if (FAILED(hr) || !pDevice) {
        return;
    }

    // Ensure it is a render (playback) endpoint, not an input/mic device
    IMMEndpoint* pEndpoint = NULL;
    hr = pDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pEndpoint);
    if (SUCCEEDED(hr) && pEndpoint) {
        EDataFlow flow;
        hr = pEndpoint->GetDataFlow(&flow);
        pEndpoint->Release();
        if (SUCCEEDED(hr) && flow != eRender) {
            pDevice->Release();
            return;
        }
    }

    // Retrieve device properties (friendly name, interface name, description)
    WCHAR szFriendlyName[256] = L"Unknown Device";
    WCHAR szInterfaceName[256] = L"";
    WCHAR szDeviceDesc[256] = L"";

    IPropertyStore* pProps = NULL;
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (SUCCEEDED(hr) && pProps) {
        PROPVARIANT var;
        PropVariantInit(&var);

        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
            wcsncpy_s(szFriendlyName, var.pwszVal, _TRUNCATE);
        }
        PropVariantClear(&var);

        if (SUCCEEDED(pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
            wcsncpy_s(szInterfaceName, var.pwszVal, _TRUNCATE);
        }
        PropVariantClear(&var);

        if (SUCCEEDED(pProps->GetValue(PKEY_Device_DeviceDesc, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
            wcsncpy_s(szDeviceDesc, var.pwszVal, _TRUNCATE);
        }
        PropVariantClear(&var);

        pProps->Release();
    }

    Wh_Log(L"[Audio Event] Device connected: Friendly=\"%s\", Interface=\"%s\", Desc=\"%s\"",
           szFriendlyName, szInterfaceName, szDeviceDesc);

    int targetVolume = -1;
    std::wstring matchedRuleName;

    AcquireSRWLockShared(&g_lock);
    // 1. Check custom rules against all candidate names
    for (const auto& rule : g_rules) {
        if (CheckCandidateMatch(szFriendlyName, rule) ||
            CheckCandidateMatch(szInterfaceName, rule) ||
            CheckCandidateMatch(szDeviceDesc, rule)) {
            targetVolume = rule.volume;
            matchedRuleName = rule.originalName;
            break;
        }
    }

    // 2. If no custom rule matched, check if global default applies
    if (targetVolume == -1 && g_applyToAllDevices) {
        targetVolume = g_defaultVolume;
        Wh_Log(L"  -> No custom rule matched. Applying global default volume: %d%%", targetVolume);
    } else if (targetVolume != -1) {
        Wh_Log(L"  -> Matched rule \"%s\" -> Target Volume: %d%%", matchedRuleName.c_str(), targetVolume);
    }

    // 3. Enforce max volume safety cap
    if (targetVolume > g_maxVolumeLimit) {
        targetVolume = g_maxVolumeLimit;
        Wh_Log(L"  -> Capped by max volume limit to %d%%", targetVolume);
    }

    BOOL doUnmute = g_autoUnmute;
    ReleaseSRWLockShared(&g_lock);

    // If neither rule matched nor global default enabled, do not touch device
    if (targetVolume < 0) {
        pDevice->Release();
        return;
    }

    // Activate volume control interface and apply settings
    IAudioEndpointVolume* pEndpointVolume = NULL;
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pEndpointVolume);
    if (SUCCEEDED(hr) && pEndpointVolume) {
        float scalar = (float)targetVolume / 100.0f;
        hr = pEndpointVolume->SetMasterVolumeLevelScalar(scalar, NULL);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"  -> Volume successfully applied: %d%% on \"%s\"", targetVolume, szFriendlyName);
        } else {
            Wh_Log(L"  -> Failed to set volume on \"%s\" (hr=0x%08X)", szFriendlyName, hr);
        }

        if (doUnmute) {
            pEndpointVolume->SetMute(FALSE, NULL);
        }

        pEndpointVolume->Release();
    } else {
        Wh_Log(L"  -> Failed to activate IAudioEndpointVolume for \"%s\" (hr=0x%08X)", szFriendlyName, hr);
    }

    pDevice->Release();
}

class AudioNotificationClient : public IMMNotificationClient {
    LONG m_cRef;
public:
    AudioNotificationClient() : m_cRef(1) {}
    virtual ~AudioNotificationClient() {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown)) {
            *ppv = static_cast<IUnknown*>(this);
        } else if (riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
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

    // Called when a device state changes (e.g. plugged in / Bluetooth connected -> ACTIVE)
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override {
        if (dwNewState == DEVICE_STATE_ACTIVE && pwstrDeviceId) {
            Wh_Log(L"OnDeviceStateChanged (ACTIVE): %s", pwstrDeviceId);
            ApplyVolumeToDevice(pwstrDeviceId);
        }
        return S_OK;
    }

    STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
        return S_OK;
    }

    STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId) override {
        return S_OK;
    }

    // Called when the default output device switches
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) override {
        if (flow == eRender && pwstrDeviceId) {
            Wh_Log(L"OnDefaultDeviceChanged (Render, role=%d): %s", role, pwstrDeviceId);
            ApplyVolumeToDevice(pwstrDeviceId);
        }
        return S_OK;
    }

    STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override {
        return S_OK;
    }
};

static AudioNotificationClient* g_pClient = NULL;

static void ApplyToCurrentDefaultDevice() {
    if (!g_pEnumerator) return;

    IMMDevice* pDefaultDevice = NULL;
    HRESULT hr = g_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
    if (SUCCEEDED(hr) && pDefaultDevice) {
        LPWSTR pwstrId = NULL;
        if (SUCCEEDED(pDefaultDevice->GetId(&pwstrId)) && pwstrId) {
            ApplyVolumeToDevice(pwstrId);
            CoTaskMemFree(pwstrId);
        }
        pDefaultDevice->Release();
    }
}

static void LogConnectedDevices() {
    if (!g_pEnumerator) return;

    IMMDeviceCollection* pCollection = NULL;
    HRESULT hr = g_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr) || !pCollection) return;

    UINT count = 0;
    pCollection->GetCount(&count);

    Wh_Log(L"============================================================");
    Wh_Log(L"[Audio Hardware Volume Preset] Detected %u Active Playback Device(s):", count);

    for (UINT i = 0; i < count; i++) {
        IMMDevice* pDevice = NULL;
        if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
            IPropertyStore* pProps = NULL;
            WCHAR szName[256] = L"Unknown Device";
            if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
                PROPVARIANT varName;
                PropVariantInit(&varName);
                if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                    if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                        wcsncpy_s(szName, varName.pwszVal, _TRUNCATE);
                    }
                    PropVariantClear(&varName);
                }
                pProps->Release();
            }

            float fVol = 0.0f;
            IAudioEndpointVolume* pEndpointVolume = NULL;
            if (SUCCEEDED(pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pEndpointVolume))) {
                pEndpointVolume->GetMasterVolumeLevelScalar(&fVol);
                pEndpointVolume->Release();
            }

            Wh_Log(L"  -> Device #%u: \"%s\" (Current Volume: %d%%)", i + 1, szName, (int)(fVol * 100.0f + 0.5f));
            pDevice->Release();
        }
    }
    Wh_Log(L"Tip: In settings, you can enter any keyword from the device name above (e.g. \"AirPods\", \"Sony\", \"Speakers\", \"DAC\")");
    Wh_Log(L"============================================================");
    pCollection->Release();
}

static DWORD WINAPI AudioMonitorThread(LPVOID lpParam) {
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrCom)) {
        Wh_Log(L"CoInitializeEx failed in AudioMonitorThread: 0x%08X", hrCom);
        return 1;
    }

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&g_pEnumerator
    );

    if (SUCCEEDED(hr) && g_pEnumerator) {
        g_pClient = new AudioNotificationClient();
        hr = g_pEnumerator->RegisterEndpointNotificationCallback(g_pClient);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"Audio endpoint notification callback successfully registered.");

            // Automatically detect and log all active audio devices currently connected
            LogConnectedDevices();

            BOOL applyStartup = FALSE;
            AcquireSRWLockShared(&g_lock);
            applyStartup = g_applyOnStartup;
            ReleaseSRWLockShared(&g_lock);

            if (applyStartup) {
                ApplyToCurrentDefaultDevice();
            }

            // Wait until mod uninitialization is requested
            WaitForSingleObject(g_hStopEvent, INFINITE);

            g_pEnumerator->UnregisterEndpointNotificationCallback(g_pClient);
        } else {
            Wh_Log(L"RegisterEndpointNotificationCallback failed: 0x%08X", hr);
        }

        g_pClient->Release();
        g_pClient = NULL;

        g_pEnumerator->Release();
        g_pEnumerator = NULL;
    } else {
        Wh_Log(L"CoCreateInstance MMDeviceEnumerator failed: 0x%08X", hr);
    }

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Audio Hardware Volume Preset initializing...");

    LoadSettings();

    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) {
        Wh_Log(L"CreateEvent failed: %d", GetLastError());
        return FALSE;
    }

    g_hWorkerThread = CreateThread(NULL, 0, AudioMonitorThread, NULL, 0, NULL);
    if (!g_hWorkerThread) {
        Wh_Log(L"CreateThread failed: %d", GetLastError());
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    Wh_Log(L"Audio Hardware Volume Preset initialized successfully.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Audio Hardware Volume Preset uninitializing...");

    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, 5000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    Wh_Log(L"Audio Hardware Volume Preset uninitialized.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed notification received.");
    LoadSettings();
}

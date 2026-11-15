// ==WindhawkMod==
// @id              audio-hardware-volume-preset
// @name            Audio Hardware Volume Preset
// @description     Sets audio playback devices to a configured volume whenever they connect
// @version         1.3.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Hardware Volume Preset

Automatically sets your headphones, speakers, Bluetooth headsets, USB DACs, or
any other audio playback hardware to a specific preset volume whenever they
connect.

## Key Features
- **Prevent Sudden Hearing Blasts:** Never plug in headphones only to be blasted
by 100% volume again.
- **Smart Hardware Matching:** Supports flexible keyword and token matching. You
can enter simple keywords (like `AirPods`, `Sony`, `DAC`, `Speakers`) or full
device names.
- **Max Volume Safety Cap:** Set a ceiling on the preset volume applied at
connect time (up to 100%).
- **Global Fallback:** Set any unrecognized audio device to a safe default
volume (e.g., 30%).
- **Auto-Unmute:** Optionally unmute the device on connection.
- **Intelligent Debouncing:** Prevents duplicate triggers when Windows fires
multiple rapid connection events.
- **Safe & Efficient:** Runs as a dedicated background tool mod using standard Windows Core
Audio event notifications (`IMMNotificationClient`). Zero polling, zero idle CPU
usage.

**Note:** Volume presets are also applied when switching the default audio
output between already-connected devices. The first matching custom rule wins,
so place more specific rules before general ones.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultVolume: 30
  $name: Default Volume (%)
  $description: Master volume percentage (0-100) applied when any audio hardware connects, if no specific rule matches.

- maxVolumeLimit: 100
  $name: Max Volume Safety Limit (%)
  $description: Maximum volume ceiling (0-100) applied to presets at connect time. Does not prevent manual volume changes afterward.

- applyToAllDevices: true
  $name: Apply to all devices by default
  $description: If enabled, any connected playback device without an explicit rule will use the Default Volume.

- autoUnmute: true
  $name: Unmute on connect
  $description: Automatically unmutes the audio device when setting its volume.

- applyOnStartup: true
  $name: Apply on mod startup
  $description: Apply volume preset to the current default audio device when the mod initializes.

- deviceRules:
  - - name: ""
      $name: Device Name or Keyword
    - volume: 30
      $name: Volume (%)
  $name: Custom Device Presets
  $description: Volume presets for specific hardware. Enter a keyword from your device name (e.g. AirPods, Sony, DAC). The first matching rule wins.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cwctype>
#include <string>
#include <vector>

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

// Debouncing state to prevent rapid duplicate events from Windows (worker thread only)
static std::wstring g_lastDeviceId;
static ULONGLONG g_lastApplyTick = 0;

// Queue and event handles for offloading notification work to the worker thread
static HANDLE g_hWorkEvent = NULL;
static SRWLOCK g_queueLock = SRWLOCK_INIT;
static std::vector<std::wstring> g_pendingDeviceIds;

// COM & Worker Thread handles
static HANDLE g_hWorkerThread = NULL;
static HANDLE g_hStopEvent = NULL;
static IMMDeviceEnumerator *g_pEnumerator = NULL;

static std::wstring ToLower(const std::wstring &str) {
  std::wstring result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](wchar_t c) { return (wchar_t)towlower(c); });
  return result;
}

// Returns true for generic device type words that should be ignored in matching
static bool IsStopWord(const std::wstring &word) {
  static const std::wstring kStopWords[] = {
      L"headphones", L"speakers", L"headset", L"earphone", L"earphones",
      L"audio",      L"device",   L"input",   L"output"};
  for (const auto &sw : kStopWords) {
    if (word == sw)
      return true;
  }
  return false;
}

// Splits a string into alphanumeric tokens, filtering short and generic words
static std::vector<std::wstring> GetTokens(const std::wstring &str) {
  std::vector<std::wstring> tokens;
  std::wstring cur;
  for (wchar_t ch : str) {
    if (iswalnum(ch)) {
      cur += (wchar_t)towlower(ch);
    } else {
      if (!cur.empty()) {
        if (cur.length() >= 2 && !IsStopWord(cur)) {
          tokens.push_back(cur);
        }
        cur.clear();
      }
    }
  }
  if (!cur.empty()) {
    if (cur.length() >= 2 && !IsStopWord(cur)) {
      tokens.push_back(cur);
    }
  }
  return tokens;
}

// Checks if a candidate device string matches a rule
static bool CheckCandidateMatch(const std::wstring &candidate,
                                const DeviceRule &rule,
                                bool allowTokens) {
  if (candidate.empty() || rule.nameLower.empty())
    return false;

  std::wstring candidateLower = ToLower(candidate);

  // 1. Direct substring match: device name contains the rule keyword
  if (candidateLower.find(rule.nameLower) != std::wstring::npos) {
    return true;
  }

  if (!allowTokens) {
    return false;
  }

  // 2. Token match: check if any device token contains a rule token
  auto devTokens = GetTokens(candidateLower);
  for (const auto &rt : rule.tokens) {
    for (const auto &dt : devTokens) {
      if (dt == rt || dt.find(rt) != std::wstring::npos) {
        return true;
      }
    }
  }

  return false;
}

static void LoadSettings() {
  AcquireSRWLockExclusive(&g_lock);

  g_defaultVolume = Wh_GetIntSetting(L"defaultVolume");
  if (g_defaultVolume < 0)
    g_defaultVolume = 0;
  if (g_defaultVolume > 100)
    g_defaultVolume = 100;

  g_maxVolumeLimit = Wh_GetIntSetting(L"maxVolumeLimit");
  if (g_maxVolumeLimit < 0)
    g_maxVolumeLimit = 0;
  if (g_maxVolumeLimit > 100)
    g_maxVolumeLimit = 100;

  g_applyToAllDevices = Wh_GetIntSetting(L"applyToAllDevices");
  g_autoUnmute = Wh_GetIntSetting(L"autoUnmute");
  g_applyOnStartup = Wh_GetIntSetting(L"applyOnStartup");

  g_rules.clear();
  for (int i = 0; i < 100; i++) {
    WindhawkUtils::StringSetting ruleName =
        WindhawkUtils::StringSetting::make(L"deviceRules[%d].name", i);
    if (!*ruleName.get()) {
      continue;
    }

    int ruleVol = Wh_GetIntSetting(L"deviceRules[%d].volume", i);
    if (ruleVol < 0)
      ruleVol = 0;
    if (ruleVol > 100)
      ruleVol = 100;

    DeviceRule rule;
    rule.originalName = ruleName.get();
    rule.nameLower = ToLower(ruleName.get());
    rule.tokens = GetTokens(rule.nameLower);
    rule.volume = ruleVol;
    g_rules.push_back(rule);
  }

  ReleaseSRWLockExclusive(&g_lock);

  Wh_Log(L"Settings loaded. Default Vol: %d%%, Max Limit: %d%%, Apply All: %d, "
         L"Startup: %d, Rules count: %zu",
         g_defaultVolume, g_maxVolumeLimit, g_applyToAllDevices,
         g_applyOnStartup, g_rules.size());
}

static void QueueApply(LPCWSTR deviceId) {
  if (!deviceId)
    return;

  AcquireSRWLockExclusive(&g_queueLock);
  g_pendingDeviceIds.emplace_back(deviceId);
  ReleaseSRWLockExclusive(&g_queueLock);
  SetEvent(g_hWorkEvent);
}

static void ApplyVolumeToDevice(LPCWSTR pwstrDeviceId) {
  if (!pwstrDeviceId || !g_pEnumerator) {
    return;
  }

  ULONGLONG currentTick = GetTickCount64();

  // Check debounce (ignore duplicate notifications for same device within 1.5s)
  if (g_lastDeviceId == pwstrDeviceId &&
      (currentTick - g_lastApplyTick) < 1500) {
    return;
  }
  g_lastDeviceId = pwstrDeviceId;
  g_lastApplyTick = currentTick;

  IMMDevice *pDevice = NULL;
  HRESULT hr = g_pEnumerator->GetDevice(pwstrDeviceId, &pDevice);
  if (FAILED(hr) || !pDevice) {
    return;
  }

  // Ensure it is a render (playback) endpoint, not an input/mic device
  IMMEndpoint *pEndpoint = NULL;
  hr = pDevice->QueryInterface(__uuidof(IMMEndpoint), (void **)&pEndpoint);
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

  IPropertyStore *pProps = NULL;
  hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
  if (SUCCEEDED(hr) && pProps) {
    PROPVARIANT var;
    PropVariantInit(&var);

    if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &var)) &&
        var.vt == VT_LPWSTR && var.pwszVal) {
      wcsncpy_s(szFriendlyName, var.pwszVal, _TRUNCATE);
    }
    PropVariantClear(&var);

    if (SUCCEEDED(pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &var)) &&
        var.vt == VT_LPWSTR && var.pwszVal) {
      wcsncpy_s(szInterfaceName, var.pwszVal, _TRUNCATE);
    }
    PropVariantClear(&var);

    if (SUCCEEDED(pProps->GetValue(PKEY_Device_DeviceDesc, &var)) &&
        var.vt == VT_LPWSTR && var.pwszVal) {
      wcsncpy_s(szDeviceDesc, var.pwszVal, _TRUNCATE);
    }
    PropVariantClear(&var);

    pProps->Release();
  }

  Wh_Log(L"[Audio Event] Device connected: Friendly=\"%s\", Interface=\"%s\", "
         L"Desc=\"%s\"",
         szFriendlyName, szInterfaceName, szDeviceDesc);

  int targetVolume = -1;
  std::wstring matchedRuleName;

  AcquireSRWLockShared(&g_lock);
  // Pass 1: Direct substring match across all custom rules first
  for (const auto &rule : g_rules) {
    if (CheckCandidateMatch(szFriendlyName, rule, false) ||
        CheckCandidateMatch(szInterfaceName, rule, false) ||
        CheckCandidateMatch(szDeviceDesc, rule, false)) {
      targetVolume = rule.volume;
      matchedRuleName = rule.originalName;
      break;
    }
  }

  // Pass 2: Fall back to token match if no custom rule matched direct substring
  if (targetVolume == -1) {
    for (const auto &rule : g_rules) {
      if (CheckCandidateMatch(szFriendlyName, rule, true) ||
          CheckCandidateMatch(szInterfaceName, rule, true) ||
          CheckCandidateMatch(szDeviceDesc, rule, true)) {
        targetVolume = rule.volume;
        matchedRuleName = rule.originalName;
        break;
      }
    }
  }

  // If no custom rule matched, check if global default applies
  if (targetVolume == -1 && g_applyToAllDevices) {
    targetVolume = g_defaultVolume;
    Wh_Log(L"  -> No custom rule matched. Applying global default volume: %d%%",
           targetVolume);
  } else if (targetVolume != -1) {
    Wh_Log(L"  -> Matched rule \"%s\" -> Target Volume: %d%%",
           matchedRuleName.c_str(), targetVolume);
  }

  // Enforce max volume safety cap
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
  IAudioEndpointVolume *pEndpointVolume = NULL;
  hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                         (void **)&pEndpointVolume);
  if (SUCCEEDED(hr) && pEndpointVolume) {
    float scalar = (float)targetVolume / 100.0f;
    hr = pEndpointVolume->SetMasterVolumeLevelScalar(scalar, NULL);
    if (SUCCEEDED(hr)) {
      Wh_Log(L"  -> Volume successfully applied: %d%% on \"%s\"", targetVolume,
             szFriendlyName);
    } else {
      Wh_Log(L"  -> Failed to set volume on \"%s\" (hr=0x%08X)", szFriendlyName,
             hr);
    }

    if (doUnmute) {
      pEndpointVolume->SetMute(FALSE, NULL);
    }

    pEndpointVolume->Release();
  } else {
    Wh_Log(
        L"  -> Failed to activate IAudioEndpointVolume for \"%s\" (hr=0x%08X)",
        szFriendlyName, hr);
  }

  pDevice->Release();
}

class AudioNotificationClient : public IMMNotificationClient {
  LONG m_cRef;

public:
  AudioNotificationClient() : m_cRef(1) {}
  virtual ~AudioNotificationClient() {}

  STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override {
    if (!ppv)
      return E_POINTER;
    if (riid == __uuidof(IUnknown)) {
      *ppv = static_cast<IUnknown *>(this);
    } else if (riid == __uuidof(IMMNotificationClient)) {
      *ppv = static_cast<IMMNotificationClient *>(this);
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

  // Called when a device state changes (e.g. plugged in / Bluetooth connected
  // -> ACTIVE)
  STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId,
                                    DWORD dwNewState) override {
    if (dwNewState == DEVICE_STATE_ACTIVE && pwstrDeviceId) {
      Wh_Log(L"OnDeviceStateChanged (ACTIVE): %s", pwstrDeviceId);
      QueueApply(pwstrDeviceId);
    }
    return S_OK;
  }

  STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId) override { return S_OK; }

  STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId) override { return S_OK; }

  // Called when the default output device switches
  STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role,
                                      LPCWSTR pwstrDeviceId) override {
    if (flow == eRender && pwstrDeviceId) {
      Wh_Log(L"OnDefaultDeviceChanged (Render, role=%d): %s", role,
             pwstrDeviceId);
      QueueApply(pwstrDeviceId);
    }
    return S_OK;
  }

  STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId,
                                      const PROPERTYKEY key) override {
    return S_OK;
  }
};

static AudioNotificationClient *g_pClient = NULL;

static void ApplyToCurrentDefaultDevice() {
  if (!g_pEnumerator)
    return;

  IMMDevice *pDefaultDevice = NULL;
  HRESULT hr = g_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                      &pDefaultDevice);
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
  if (!g_pEnumerator)
    return;

  IMMDeviceCollection *pCollection = NULL;
  HRESULT hr = g_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                                 &pCollection);
  if (FAILED(hr) || !pCollection)
    return;

  UINT count = 0;
  pCollection->GetCount(&count);

  Wh_Log(L"============================================================");
  Wh_Log(L"Detected %u Active Playback Device(s):", count);

  for (UINT i = 0; i < count; i++) {
    IMMDevice *pDevice = NULL;
    if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
      IPropertyStore *pProps = NULL;
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
      IAudioEndpointVolume *pEndpointVolume = NULL;
      if (SUCCEEDED(pDevice->Activate(__uuidof(IAudioEndpointVolume),
                                      CLSCTX_ALL, NULL,
                                      (void **)&pEndpointVolume))) {
        pEndpointVolume->GetMasterVolumeLevelScalar(&fVol);
        pEndpointVolume->Release();
      }

      Wh_Log(L"  -> Device #%u: \"%s\" (Current Volume: %d%%)", i + 1, szName,
             (int)(fVol * 100.0f + 0.5f));
      pDevice->Release();
    }
  }
  Wh_Log(L"Tip: In settings, you can enter any keyword from the device name "
         L"above (e.g. \"AirPods\", \"Sony\", \"Speakers\", \"DAC\")");
  Wh_Log(L"============================================================");
  pCollection->Release();
}

static DWORD WINAPI AudioMonitorThread(LPVOID lpParam) {
  HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hrCom)) {
    Wh_Log(L"CoInitializeEx failed in AudioMonitorThread: 0x%08X", hrCom);
    return 1;
  }

  HRESULT hr =
      CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                       __uuidof(IMMDeviceEnumerator), (void **)&g_pEnumerator);

  if (SUCCEEDED(hr) && g_pEnumerator) {
    g_pClient = new AudioNotificationClient();
    hr = g_pEnumerator->RegisterEndpointNotificationCallback(g_pClient);
    if (SUCCEEDED(hr)) {
      Wh_Log(L"Audio endpoint notification callback successfully registered.");

      // Automatically detect and log all active audio devices currently
      // connected
      LogConnectedDevices();

      BOOL applyStartup = FALSE;
      AcquireSRWLockShared(&g_lock);
      applyStartup = g_applyOnStartup;
      ReleaseSRWLockShared(&g_lock);

      if (applyStartup) {
        ApplyToCurrentDefaultDevice();
      }

      // Process queued audio events until mod uninitialization is requested
      HANDLE handles[] = {g_hStopEvent, g_hWorkEvent};
      while (WaitForMultipleObjects(2, handles, FALSE, INFINITE) ==
             WAIT_OBJECT_0 + 1) {
        std::vector<std::wstring> ids;
        AcquireSRWLockExclusive(&g_queueLock);
        ids.swap(g_pendingDeviceIds);
        ReleaseSRWLockExclusive(&g_queueLock);
        for (const auto &id : ids) {
          ApplyVolumeToDevice(id.c_str());
        }
      }

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

BOOL WhTool_ModInit() {
  Wh_Log(L"Audio Hardware Volume Preset tool mod initializing...");

  LoadSettings();

  g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (!g_hStopEvent) {
    Wh_Log(L"CreateEvent (stop) failed: %d", GetLastError());
    return FALSE;
  }

  g_hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!g_hWorkEvent) {
    Wh_Log(L"CreateEvent (work) failed: %d", GetLastError());
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  g_hWorkerThread = CreateThread(NULL, 0, AudioMonitorThread, NULL, 0, NULL);
  if (!g_hWorkerThread) {
    Wh_Log(L"CreateThread failed: %d", GetLastError());
    CloseHandle(g_hWorkEvent);
    g_hWorkEvent = NULL;
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  Wh_Log(L"Audio Hardware Volume Preset tool mod initialized successfully.");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Audio Hardware Volume Preset tool mod uninitializing...");

  if (g_hStopEvent) {
    SetEvent(g_hStopEvent);
  }

  if (g_hWorkerThread) {
    WaitForSingleObject(g_hWorkerThread, INFINITE);
    CloseHandle(g_hWorkerThread);
    g_hWorkerThread = NULL;
  }

  if (g_hStopEvent) {
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
  }

  if (g_hWorkEvent) {
    CloseHandle(g_hWorkEvent);
    g_hWorkEvent = NULL;
  }

  Wh_Log(L"Audio Hardware Volume Preset tool mod uninitialized.");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed notification received.");
  LoadSettings();
}

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
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
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

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS *ntHeaders =
        (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

    Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
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
                    (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
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

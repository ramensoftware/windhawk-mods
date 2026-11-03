// ==WindhawkMod==
// @id              device-connect-volume
// @name            Device Connect Volume
// @description     Sets the audio devices you list to a chosen volume when they connect
// @version         1.0.0
// @author          0Allu
// @github          https://github.com/0Allu
// @homepage        https://github.com/0Allu/device-connect-volume
// @donateUrl       https://ko-fi.com/0allu
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Device Connect Volume

Sets the audio devices you list to a chosen volume when they connect. Some
Bluetooth devices store their volume and keep it as they move between phones and
computers, and this saves you from correcting it by hand each time.

## Setup

Add each device to the list in the mod's settings with the volume you want. For
the name, use all or part of what Windows Sound settings shows, such as
`WH-1000XM4` or `AirPods`. Capitalization doesn't matter. If a device matches
more than one entry, the first one is used.

Devices that aren't in the list are never changed.

## When the volume is set

The mod sets a listed device's volume when the device connects and when the mod
starts. Starting covers devices that connected before Windhawk was running, and
it includes restarting Windhawk or turning the mod off and on. If Windows audio
isn't ready yet, the mod keeps trying until it is.

Changes to the device list apply the next time a device connects.

## Changing the volume yourself

Windows and Bluetooth devices can restore a saved volume a few seconds after
connecting. To catch that, the mod watches the volume for 15 seconds and sets it
again if it changes, up to twice per connection.

The mod can't tell who changed the volume, so it may undo the first two changes
you make in those 15 seconds. A third change stays and ends the watch. After the
15 seconds, the mod leaves the volume alone until the next connection or start.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- devices:
  - - name: ""
      $name: Device name
      $description: All or part of the name shown in Windows Sound settings
    - volume: 30
      $name: Volume (%)
  $name: Devices
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <string>
#include <vector>

struct DeviceRule {
  std::wstring nameLower;
  int volume;
};

// A connected device whose volume is being set or watched
struct TrackedDevice {
  std::wstring id;
  std::wstring name;
  ULONGLONG nextTick = 0;
  ULONGLONG stopTick = 0;
  bool applied = false;
  bool failureLogged = false;
  int corrections = 0;
  int targetVolume = -1;
  float appliedLevel = 0.0f;
};

// How often to check a tracked device
static const ULONGLONG kCheckIntervalMs = 500;

// How long to keep trying a device that won't accept a volume change yet
static const ULONGLONG kRetryWindowMs = 60000;

// How long to watch a device's volume after setting it
static const ULONGLONG kHoldMs = 15000;

// How many times to set the volume again during the watch. A restored level
// changes once or twice, so another change is taken to be the user's.
static const int kMaxCorrections = 2;

// How often to retry when Windows audio isn't ready
static const DWORD kAudioRetryMs = 1000;

// How long after starting to keep looking for connected devices if none are found
static const ULONGLONG kStartupScanWindowMs = 60000;

// Volume differences below this are ignored when checking for changes
static const float kLevelTolerance = 0.02f;

// A level read back within this distance of the one just set is the driver
// rounding to a hardware step. Anything further means it was overwritten.
static const float kMaxHardwareStep = 0.1f;

static SRWLOCK g_rulesLock = SRWLOCK_INIT;
static std::vector<DeviceRule> g_rules;

// Devices currently being tracked (worker thread only)
static std::vector<TrackedDevice> g_trackedDevices;

// Device IDs passed from the notification callbacks to the worker thread
static SRWLOCK g_queueLock = SRWLOCK_INIT;
static std::vector<std::wstring> g_queuedDeviceIds;

static HANDLE g_hWorkEvent = NULL;
static HANDLE g_hStopEvent = NULL;
static HANDLE g_hWorkerThread = NULL;
static IMMDeviceEnumerator *g_pEnumerator = NULL;

static std::wstring ToLower(const std::wstring &str) {
  std::wstring result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](wchar_t c) { return (wchar_t)towlower(c); });
  return result;
}

static void LoadSettings() {
  AcquireSRWLockExclusive(&g_rulesLock);

  g_rules.clear();
  for (int i = 0; i < 100; i++) {
    WindhawkUtils::StringSetting name =
        WindhawkUtils::StringSetting::make(L"devices[%d].name", i);
    if (!*name.get()) {
      continue;
    }

    int volume = Wh_GetIntSetting(L"devices[%d].volume", i);
    if (volume < 0)
      volume = 0;
    if (volume > 100)
      volume = 100;

    g_rules.push_back({ToLower(name.get()), volume});
  }

  Wh_Log(L"Settings loaded. Devices: %d", (int)g_rules.size());

  ReleaseSRWLockExclusive(&g_rulesLock);
}

// Returns the volume configured for a device name, or -1 if it isn't listed
static int FindConfiguredVolume(const std::wstring &deviceName) {
  std::wstring deviceNameLower = ToLower(deviceName);
  int volume = -1;

  AcquireSRWLockShared(&g_rulesLock);
  for (const auto &rule : g_rules) {
    if (deviceNameLower.find(rule.nameLower) != std::wstring::npos) {
      volume = rule.volume;
      break;
    }
  }
  ReleaseSRWLockShared(&g_rulesLock);

  return volume;
}

// Returns the device's name, or an empty string if it isn't available yet
static std::wstring GetDeviceName(IMMDevice *pDevice) {
  std::wstring name;

  IPropertyStore *pProps = NULL;
  if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
    PROPVARIANT var;
    PropVariantInit(&var);
    if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &var)) &&
        var.vt == VT_LPWSTR && var.pwszVal) {
      name = var.pwszVal;
    }
    PropVariantClear(&var);
    pProps->Release();
  }

  return name;
}

static bool IsPlaybackDevice(IMMDevice *pDevice) {
  IMMEndpoint *pEndpoint = NULL;
  if (FAILED(pDevice->QueryInterface(__uuidof(IMMEndpoint),
                                     (void **)&pEndpoint)) ||
      !pEndpoint) {
    return false;
  }

  EDataFlow flow = eCapture;
  HRESULT hr = pEndpoint->GetDataFlow(&flow);
  pEndpoint->Release();
  return SUCCEEDED(hr) && flow == eRender;
}

// Starts tracking a device, or starts over if it's already being tracked
static void TrackDevice(const std::wstring &deviceId) {
  ULONGLONG now = GetTickCount64();

  for (auto &device : g_trackedDevices) {
    if (device.id == deviceId) {
      device.nextTick = now;
      device.stopTick = now + (device.applied ? kHoldMs : kRetryWindowMs);
      device.corrections = 0;
      return;
    }
  }

  TrackedDevice device;
  device.id = deviceId;
  device.nextTick = now;
  device.stopTick = now + kRetryWindowMs;
  g_trackedDevices.push_back(device);
}

// Sets the device to its configured volume, or sets it again if the volume has
// changed. Returns false once the device no longer needs tracking.
static bool UpdateDevice(TrackedDevice &device) {
  IMMDevice *pDevice = NULL;
  HRESULT hr = g_pEnumerator->GetDevice(device.id.c_str(), &pDevice);
  if (FAILED(hr) || !pDevice) {
    return false;
  }

  // If the device has been disconnected, the next connect event will start
  // tracking it again
  DWORD state = 0;
  if (!IsPlaybackDevice(pDevice) || FAILED(pDevice->GetState(&state)) ||
      state != DEVICE_STATE_ACTIVE) {
    pDevice->Release();
    return false;
  }

  // A device that is still initializing may not have its name yet, and without
  // one it would wrongly be treated as not being in the list
  std::wstring name = GetDeviceName(pDevice);
  if (name.empty()) {
    pDevice->Release();
    return true;
  }

  int targetVolume = FindConfiguredVolume(name);
  if (targetVolume < 0) {
    Wh_Log(L"\"%s\" isn't in the device list, leaving it unchanged",
           name.c_str());
    pDevice->Release();
    return false;
  }

  device.name = name;

  // "Master" here means the device's overall level across its channels. It
  // doesn't affect other devices.
  IAudioEndpointVolume *pEndpointVolume = NULL;
  hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
                         (void **)&pEndpointVolume);
  pDevice->Release();
  if (FAILED(hr) || !pEndpointVolume) {
    if (!device.failureLogged) {
      Wh_Log(L"Can't access the volume of \"%s\" yet (hr=0x%08X)", name.c_str(),
             hr);
      device.failureLogged = true;
    }
    return true;
  }

  // Set the level the first time, and again if something has since changed it
  bool needsSet = !device.applied || targetVolume != device.targetVolume;
  if (!needsSet) {
    float level = 0.0f;
    if (SUCCEEDED(pEndpointVolume->GetMasterVolumeLevelScalar(&level)) &&
        std::fabs(level - device.appliedLevel) > kLevelTolerance) {
      Wh_Log(L"Volume of \"%s\" changed to %d%%", name.c_str(),
             (int)(level * 100.0f + 0.5f));

      if (device.corrections >= kMaxCorrections) {
        Wh_Log(L"Leaving \"%s\" at that volume", name.c_str());
        pEndpointVolume->Release();
        return false;
      }

      needsSet = true;
    }
  }

  if (needsSet) {
    float scalar = (float)targetVolume / 100.0f;
    hr = pEndpointVolume->SetMasterVolumeLevelScalar(scalar, NULL);
    if (SUCCEEDED(hr)) {
      Wh_Log(L"Set volume of \"%s\" to %d%%", name.c_str(), targetVolume);

      // Compare later checks against the level the device settled on, since the
      // driver may round to its nearest hardware step
      float level = scalar;
      if (SUCCEEDED(pEndpointVolume->GetMasterVolumeLevelScalar(&level)) &&
          std::fabs(level - scalar) > kMaxHardwareStep) {
        level = scalar;
      }

      device.targetVolume = targetVolume;
      device.appliedLevel = level;

      if (device.applied) {
        device.corrections++;
      } else {
        device.applied = true;
        device.stopTick = GetTickCount64() + kHoldMs;
      }
    } else if (!device.failureLogged) {
      Wh_Log(L"Can't set the volume of \"%s\" yet (hr=0x%08X)", name.c_str(),
             hr);
      device.failureLogged = true;
    }
  }

  pEndpointVolume->Release();
  return true;
}

static void ProcessQueuedDevices() {
  std::vector<std::wstring> deviceIds;
  AcquireSRWLockExclusive(&g_queueLock);
  deviceIds.swap(g_queuedDeviceIds);
  ReleaseSRWLockExclusive(&g_queueLock);

  for (const auto &deviceId : deviceIds) {
    TrackDevice(deviceId);
  }
}

static void UpdateDueDevices() {
  ULONGLONG now = GetTickCount64();

  for (auto it = g_trackedDevices.begin(); it != g_trackedDevices.end();) {
    if (it->nextTick > now) {
      ++it;
      continue;
    }

    bool keepTracking = UpdateDevice(*it);
    now = GetTickCount64();

    if (keepTracking && now >= it->stopTick) {
      if (it->applied) {
        Wh_Log(L"Stopped watching the volume of \"%s\"", it->name.c_str());
      } else {
        Wh_Log(L"Gave up on a device that never accepted a volume change: %s",
               it->id.c_str());
      }
      keepTracking = false;
    }

    if (keepTracking) {
      it->nextTick = now + kCheckIntervalMs;
      ++it;
    } else {
      it = g_trackedDevices.erase(it);
    }
  }
}

// Returns how long to wait until the next device is due, or INFINITE if there
// are none, so that no CPU is used while idle
static DWORD GetNextWaitMs() {
  if (g_trackedDevices.empty())
    return INFINITE;

  ULONGLONG now = GetTickCount64();
  ULONGLONG waitMs = kCheckIntervalMs;
  for (const auto &device : g_trackedDevices) {
    if (device.nextTick <= now)
      return 0;
    if (device.nextTick - now < waitMs)
      waitMs = device.nextTick - now;
  }
  return (DWORD)waitMs;
}

// Tracks devices that connected before the mod started. Returns false if none
// were found, which can mean Windows audio is still starting.
static bool TrackConnectedDevices() {
  IMMDeviceCollection *pCollection = NULL;
  HRESULT hr = g_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                                 &pCollection);
  if (FAILED(hr) || !pCollection) {
    Wh_Log(L"EnumAudioEndpoints failed: 0x%08X", hr);
    return false;
  }

  UINT count = 0;
  pCollection->GetCount(&count);

  for (UINT i = 0; i < count; i++) {
    IMMDevice *pDevice = NULL;
    if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
      LPWSTR pwstrId = NULL;
      if (SUCCEEDED(pDevice->GetId(&pwstrId)) && pwstrId) {
        TrackDevice(pwstrId);
        CoTaskMemFree(pwstrId);
      }
      pDevice->Release();
    }
  }

  pCollection->Release();
  return count > 0;
}

class AudioNotificationClient : public IMMNotificationClient {
  LONG m_cRef;

  static void QueueDevice(LPCWSTR deviceId) {
    AcquireSRWLockExclusive(&g_queueLock);
    g_queuedDeviceIds.emplace_back(deviceId);
    ReleaseSRWLockExclusive(&g_queueLock);
    SetEvent(g_hWorkEvent);
  }

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

  // Called when Windows first creates an audio endpoint, such as when pairing a
  // Bluetooth device
  STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
    if (pwstrDeviceId) {
      QueueDevice(pwstrDeviceId);
    }
    return S_OK;
  }

  STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId) override { return S_OK; }

  // Called when a known device connects, such as a Bluetooth device
  // reconnecting or a USB device being plugged back in
  STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId,
                                    DWORD dwNewState) override {
    if (dwNewState == DEVICE_STATE_ACTIVE && pwstrDeviceId) {
      QueueDevice(pwstrDeviceId);
    }
    return S_OK;
  }

  STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role,
                                      LPCWSTR pwstrDeviceId) override {
    return S_OK;
  }

  STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId,
                                      const PROPERTYKEY key) override {
    return S_OK;
  }
};

// Returns the registered notification client, or NULL if Windows audio isn't
// ready
static AudioNotificationClient *ConnectToWindowsAudio() {
  HRESULT hr =
      CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                       __uuidof(IMMDeviceEnumerator), (void **)&g_pEnumerator);
  if (FAILED(hr) || !g_pEnumerator) {
    Wh_Log(L"CoCreateInstance MMDeviceEnumerator failed: 0x%08X", hr);
    g_pEnumerator = NULL;
    return NULL;
  }

  AudioNotificationClient *pClient = new AudioNotificationClient();
  hr = g_pEnumerator->RegisterEndpointNotificationCallback(pClient);
  if (FAILED(hr)) {
    Wh_Log(L"RegisterEndpointNotificationCallback failed: 0x%08X", hr);
    pClient->Release();
    g_pEnumerator->Release();
    g_pEnumerator = NULL;
    return NULL;
  }

  return pClient;
}

static DWORD WINAPI AudioMonitorThread(LPVOID lpParam) {
  HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hr)) {
    Wh_Log(L"CoInitializeEx failed: 0x%08X", hr);
    return 1;
  }

  AudioNotificationClient *pClient = NULL;
  while (!(pClient = ConnectToWindowsAudio())) {
    if (WaitForSingleObject(g_hStopEvent, kAudioRetryMs) == WAIT_OBJECT_0) {
      CoUninitialize();
      return 0;
    }
  }

  // Notifications are registered before this scan, so a device connecting during
  // the scan still gets picked up
  bool startupScanDone = TrackConnectedDevices();
  ULONGLONG startupScanStopTick = GetTickCount64() + kStartupScanWindowMs;

  HANDLE handles[] = {g_hStopEvent, g_hWorkEvent};
  while (true) {
    DWORD waitMs = GetNextWaitMs();
    if (!startupScanDone && waitMs > kAudioRetryMs) {
      waitMs = kAudioRetryMs;
    }

    DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, waitMs);
    if (waitResult != WAIT_OBJECT_0 + 1 && waitResult != WAIT_TIMEOUT) {
      break;
    }

    if (!startupScanDone) {
      startupScanDone = TrackConnectedDevices() ||
                        GetTickCount64() >= startupScanStopTick;
    }

    ProcessQueuedDevices();
    UpdateDueDevices();
  }

  g_pEnumerator->UnregisterEndpointNotificationCallback(pClient);
  pClient->Release();

  g_pEnumerator->Release();
  g_pEnumerator = NULL;

  CoUninitialize();
  return 0;
}

BOOL WhTool_ModInit() {
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

  return TRUE;
}

void WhTool_ModUninit() {
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
}

void WhTool_ModSettingsChanged() {
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

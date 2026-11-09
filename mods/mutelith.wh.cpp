// ==WindhawkMod==
// @id              mutelith
// @name            Mutelith
// @description     Stops Discord Screen Share echo with SteelSeries Sonar or FxSound: viewers no longer hear audio twice or their own voice
// @version         1.0
// @author          Faelayis
// @github          https://github.com/Faelayis
// @homepage        https://github.com/Faelayis/Mutelith
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mutelith

Fixes **Discord double audio when screen sharing** on Windows setups that use
virtual audio devices, where viewers hear the game or desktop audio twice, or
hear their own microphone coming back with a delay.

| Provider          | Target device (Discord muted)    |
| ----------------- | -------------------------------- |
| SteelSeries Sonar | `SteelSeries Sonar - Microphone` |
| FxSound           | `FxSound Speakers`               |

The provider is detected from the **default output device**.

While Discord is running, the mod:

1. Mutes Discord on the provider's target device and on every device outside
   the provider (speakers, headphones).
2. Leaves the provider's other virtual devices alone.
3. When the mod is disabled, or the default device switches to another
   provider, unmutes only the sessions it muted and gives them back their
   previous volume.

It reacts to Windows audio notifications right away (default device changed,
device added or removed, new audio session such as a screen share, a muted
session was unmuted) and also checks every 30 seconds.

This is the same rule as the [Mutelith app](https://github.com/Faelayis/Mutelith).
**Use one or the other:** while `Mutelith.exe` is running, the mod does nothing.

Designed and tested for one virtual audio provider at a time.
*/
// ==/WindhawkModReadme==

#include <windows.h>

#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <powrprof.h>
#include <tlhelp32.h>
#include <wrl/client.h>

#include <atomic>
#include <cwctype>
#include <map>
#include <string>
#include <utility>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

// GUIDs and property keys are spelled out so the mod links without -luuid.
constexpr GUID kClsidMMDeviceEnumerator = {
    0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
constexpr GUID kIidUnknown = {
    0x00000000, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIidMMDeviceEnumerator = {
    0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}};
constexpr GUID kIidAudioSessionManager2 = {
    0x77aa99a0, 0x1bd6, 0x484f, {0x8b, 0xc7, 0x2c, 0x65, 0x4c, 0x9a, 0x9b, 0x6f}};
constexpr GUID kIidAudioSessionControl2 = {
    0xbfb7ff88, 0x7239, 0x4fc9, {0x8f, 0xa2, 0x07, 0xc9, 0x50, 0xbe, 0x9c, 0x6d}};
constexpr GUID kIidSimpleAudioVolume = {
    0x87ce5498, 0x68d6, 0x44e5, {0x92, 0x15, 0x6d, 0xa4, 0x7e, 0xf8, 0x83, 0xd8}};
constexpr GUID kIidMMNotificationClient = {
    0x7991eec9, 0x7e89, 0x4d85, {0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0}};
constexpr GUID kIidAudioSessionNotification = {
    0x641dd20b, 0x4d41, 0x49cc, {0xab, 0xa3, 0x17, 0x4b, 0x94, 0x77, 0xbb, 0x08}};
constexpr GUID kIidAudioSessionEvents = {
    0x24918acc, 0x64b3, 0x37c1, {0x8c, 0xa9, 0x74, 0xa6, 0x6e, 0x99, 0x57, 0xa8}};

// "Speakers (Realtek(R) Audio)".
constexpr PROPERTYKEY kPkeyDeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
// The adapter behind the endpoint, e.g. "SteelSeries Sonar Virtual Audio Device".
constexpr PROPERTYKEY kPkeyDeviceInterfaceFriendlyName = {
    {0x026e516e, 0xb814, 0x414b, {0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22}},
    2};

constexpr DWORD kFallbackPollMs = 30 * 1000;
constexpr DWORD kPollMs = 2 * 1000;
// Time to let audio devices settle after the system resumes, like the app.
constexpr DWORD kResumeSettleMs = 3 * 1000;

constexpr const wchar_t* kDiscordExes[] = {L"Discord.exe", L"DiscordPTB.exe",
                                          L"DiscordDevelopment.exe"};
constexpr const wchar_t* kAppExe = L"Mutelith.exe";

struct Provider {
    const wchar_t* name;
    // Name prefix shared by all devices this provider creates.
    const wchar_t* prefix;
    // The provider device where Discord is muted too.
    const wchar_t* target;
};

constexpr Provider kSonar{L"SteelSeries Sonar", L"SteelSeries Sonar",
                          L"SteelSeries Sonar - Microphone"};
constexpr Provider kFxSound{L"FxSound", L"FxSound", L"FxSound Speakers"};

std::wstring Lower(std::wstring text) {
    for (auto& c : text) {
        c = towlower(c);
    }
    return text;
}

bool ContainsCi(const std::wstring& haystack, const wchar_t* needle) {
    return Lower(haystack).find(Lower(needle)) != std::wstring::npos;
}

const Provider* DetectProvider(const std::wstring& name,
                               const std::wstring& controller) {
    auto either = [&](const wchar_t* needle) {
        return ContainsCi(name, needle) || ContainsCi(controller, needle);
    };
    if (either(L"SteelSeries Sonar Virtual Audio Device")) {
        return &kSonar;
    }
    if (either(L"FxSound Audio Enhancer") || either(L"FxSound")) {
        return &kFxSound;
    }
    return nullptr;
}

std::wstring ReadString(IMMDevice* device, const PROPERTYKEY& key) {
    ComPtr<IPropertyStore> store;
    if (FAILED(device->OpenPropertyStore(STGM_READ, &store))) {
        return {};
    }
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(store->GetValue(key, &value)) && value.vt == VT_LPWSTR &&
        value.pwszVal) {
        text = value.pwszVal;
    }
    PropVariantClear(&value);
    return text;
}

std::wstring DeviceId(IMMDevice* device) {
    LPWSTR id = nullptr;
    if (FAILED(device->GetId(&id)) || !id) {
        return {};
    }
    std::wstring text = id;
    CoTaskMemFree(id);
    return text;
}

struct Processes {
    std::map<DWORD, std::wstring> discord;
    bool appRunning = false;
};

Processes ScanProcesses() {
    Processes result;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return result;
    }
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    for (BOOL more = Process32FirstW(snapshot, &entry); more;
         more = Process32NextW(snapshot, &entry)) {
        if (_wcsicmp(entry.szExeFile, kAppExe) == 0) {
            result.appRunning = true;
        }
        for (auto exe : kDiscordExes) {
            if (_wcsicmp(entry.szExeFile, exe) == 0) {
                result.discord[entry.th32ProcessID] = entry.szExeFile;
            }
        }
    }
    CloseHandle(snapshot);
    return result;
}

// One object for every Core Audio callback. Callbacks only signal the worker:
// Microsoft requires IMMNotificationClient methods to be nonblocking.
class Waker final : public IMMNotificationClient,
                    public IAudioSessionNotification,
                    public IAudioSessionEvents {
   public:
    explicit Waker(HANDLE event) : m_event(event) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }
        if (IsEqualIID(riid, kIidUnknown) ||
            IsEqualIID(riid, kIidMMNotificationClient)) {
            *object = static_cast<IMMNotificationClient*>(this);
        } else if (IsEqualIID(riid, kIidAudioSessionNotification)) {
            *object = static_cast<IAudioSessionNotification*>(this);
        } else if (IsEqualIID(riid, kIidAudioSessionEvents)) {
            *object = static_cast<IAudioSessionEvents*>(this);
        } else {
            *object = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() override { return ++m_refs; }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG refs = --m_refs;
        if (refs == 0) {
            delete this;
        }
        return refs;
    }

    // IMMNotificationClient.
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override { return Wake(); }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return Wake(); }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return Wake(); }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) override {
        return flow == eRender ? Wake() : S_OK;
    }
    // Fires often (volume, formats) and never changes what the rule does.
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

    // IAudioSessionNotification.
    STDMETHODIMP OnSessionCreated(IAudioSessionControl*) override { return Wake(); }

    // IAudioSessionEvents, registered on sessions the mod muted. Also fires for
    // the mod's own changes; the extra pass finds them muted and does nothing.
    STDMETHODIMP OnSimpleVolumeChanged(float, BOOL, LPCGUID) override {
        return Wake();
    }
    STDMETHODIMP OnDisplayNameChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnIconPathChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID) override {
        return S_OK;
    }
    STDMETHODIMP OnGroupingParamChanged(LPCGUID, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnStateChanged(AudioSessionState) override { return S_OK; }
    STDMETHODIMP OnSessionDisconnected(AudioSessionDisconnectReason) override {
        return S_OK;
    }

   private:
    HRESULT Wake() {
        SetEvent(m_event);
        return S_OK;
    }

    std::atomic<ULONG> m_refs{1};
    HANDLE m_event;
};

struct Device {
    ComPtr<IMMDevice> device;
    std::wstring id;
    std::wstring name;
};

// Runs on the worker thread only, with COM initialized as MTA.
class Monitor {
   public:
    Monitor(HANDLE wakeEvent) : m_waker(new Waker(wakeEvent)) {}

    ~Monitor() {
        Restore();
        UnwatchSessionManagers();
        if (m_enumerator && m_notificationsRegistered) {
            m_enumerator->UnregisterEndpointNotificationCallback(m_waker);
        }
        m_waker->Release();
    }

    bool Init() {
        if (FAILED(CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                    kIidMMDeviceEnumerator,
                                    (void**)m_enumerator.GetAddressOf()))) {
            Wh_Log(L"Failed to create the device enumerator");
            return false;
        }
        m_notificationsRegistered = SUCCEEDED(
            m_enumerator->RegisterEndpointNotificationCallback(m_waker));
        if (!m_notificationsRegistered) {
            Wh_Log(L"Audio notifications unavailable, polling every 2 seconds");
        }
        return true;
    }

    // Run one pass. Returns how long to wait before the next one.
    DWORD Tick() {
        std::vector<Device> devices = ActiveDevices();
        if (m_notificationsRegistered) {
            WatchSessionManagers(devices);
        }

        Processes processes = ScanProcesses();
        if (processes.appRunning) {
            if (!m_appRunning) {
                Wh_Log(L"Mutelith.exe is running, the mod stays idle");
            }
            m_appRunning = true;
            Restore();
            return PollMs();
        }
        if (m_appRunning) {
            Wh_Log(L"Mutelith.exe stopped, the mod takes over");
            m_appRunning = false;
        }

        const Provider* detected = DefaultProvider();
        if (detected != m_provider) {
            Wh_Log(L"Default device provider changed from %s to %s",
                   m_provider ? m_provider->name : L"none",
                   detected ? detected->name : L"none");
            // Undo mutes that the new provider's rule does not cover.
            Restore();
            m_provider = detected;
        }

        if (m_provider && !processes.discord.empty()) {
            Apply(devices, processes.discord);
        }
        return PollMs();
    }

    // Unmute every session this mod muted and give it back its volume.
    void Restore() {
        if (m_muted.empty() || !m_provider) {
            ClearMuted();
            return;
        }
        int count = 0;
        Processes processes = ScanProcesses();
        for (auto& device : ActiveDevices()) {
            if (!(ContainsCi(device.name, m_provider->target) ||
                  !ContainsCi(device.name, m_provider->prefix))) {
                continue;
            }
            ForEachSession(device, [&](IAudioSessionControl* control, DWORD pid) {
                auto muted = m_muted.find({device.name, pid});
                if (muted == m_muted.end() || !processes.discord.contains(pid)) {
                    return;
                }
                ComPtr<ISimpleAudioVolume> volume;
                if (SUCCEEDED(control->QueryInterface(
                        kIidSimpleAudioVolume, (void**)volume.GetAddressOf())) &&
                    SUCCEEDED(volume->SetMute(FALSE, nullptr)) &&
                    SUCCEEDED(volume->SetMasterVolume(muted->second, nullptr))) {
                    count++;
                    Wh_Log(L"Discord (PID %u) unmuted in %s", pid,
                           device.name.c_str());
                }
            });
        }
        Wh_Log(L"Unmuted %d Discord session(s)", count);
        ClearMuted();
    }

   private:
    using SessionKey = std::pair<std::wstring, DWORD>;

    DWORD PollMs() const {
        return m_notificationsRegistered ? kFallbackPollMs : kPollMs;
    }

    std::vector<Device> ActiveDevices() {
        std::vector<Device> devices;
        ComPtr<IMMDeviceCollection> collection;
        UINT count = 0;
        if (FAILED(m_enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                                   &collection)) ||
            FAILED(collection->GetCount(&count))) {
            Wh_Log(L"Failed to list audio devices");
            return devices;
        }
        for (UINT i = 0; i < count; i++) {
            Device device;
            if (SUCCEEDED(collection->Item(i, &device.device))) {
                device.id = DeviceId(device.device.Get());
                device.name =
                    ReadString(device.device.Get(), kPkeyDeviceFriendlyName);
                devices.push_back(std::move(device));
            }
        }
        return devices;
    }

    const Provider* DefaultProvider() {
        ComPtr<IMMDevice> device;
        if (FAILED(m_enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                        &device))) {
            return nullptr;
        }
        return DetectProvider(
            ReadString(device.Get(), kPkeyDeviceFriendlyName),
            ReadString(device.Get(), kPkeyDeviceInterfaceFriendlyName));
    }

    template <typename F>
    void ForEachSession(const Device& device, F&& visit) {
        ComPtr<IAudioSessionManager2> manager;
        ComPtr<IAudioSessionEnumerator> sessions;
        int count = 0;
        if (FAILED(device.device->Activate(kIidAudioSessionManager2, CLSCTX_ALL,
                                           nullptr,
                                           (void**)manager.GetAddressOf())) ||
            FAILED(manager->GetSessionEnumerator(&sessions)) ||
            FAILED(sessions->GetCount(&count))) {
            return;
        }
        for (int i = 0; i < count; i++) {
            ComPtr<IAudioSessionControl> control;
            ComPtr<IAudioSessionControl2> control2;
            DWORD pid = 0;
            if (SUCCEEDED(sessions->GetSession(i, &control)) &&
                SUCCEEDED(control->QueryInterface(
                    kIidAudioSessionControl2, (void**)control2.GetAddressOf())) &&
                SUCCEEDED(control2->GetProcessId(&pid)) && pid != 0) {
                visit(control.Get(), pid);
            }
        }
    }

    void Apply(const std::vector<Device>& devices,
               const std::map<DWORD, std::wstring>& discord) {
        const Provider& provider = *m_provider;
        bool targetFound = false;
        std::vector<const Device*> targets;
        for (auto& device : devices) {
            if (!targetFound && ContainsCi(device.name, provider.target)) {
                targetFound = true;
                targets.push_back(&device);
            } else if (!ContainsCi(device.name, provider.prefix)) {
                targets.push_back(&device);
            }
        }
        if (!targetFound) {
            if (m_lastCount != -2) {
                Wh_Log(L"%s not found", provider.target);
                m_lastCount = -2;
            }
            return;
        }

        int count = 0;
        for (const Device* device : targets) {
            ForEachSession(*device, [&](IAudioSessionControl* control, DWORD pid) {
                auto process = discord.find(pid);
                if (process == discord.end()) {
                    return;
                }
                ComPtr<ISimpleAudioVolume> volume;
                if (FAILED(control->QueryInterface(
                        kIidSimpleAudioVolume, (void**)volume.GetAddressOf()))) {
                    return;
                }

                SessionKey key{device->name, pid};
                bool known = m_muted.contains(key);
                BOOL muted = FALSE;
                if (known && SUCCEEDED(volume->GetMute(&muted)) && muted) {
                    count++;
                    return;
                }

                // Read before muting; a session muted again keeps its first
                // saved volume.
                float level = 1.0f;
                if (FAILED(volume->GetMasterVolume(&level))) {
                    level = 1.0f;
                }
                if (FAILED(volume->SetMasterVolume(0.0f, nullptr)) ||
                    FAILED(volume->SetMute(TRUE, nullptr))) {
                    return;
                }
                count++;
                if (known) {
                    return;
                }

                m_muted[key] = level;
                Wh_Log(L"%s (PID %u) muted in %s", process->second.c_str(), pid,
                       device->name.c_str());
                if (m_notificationsRegistered &&
                    SUCCEEDED(control->RegisterAudioSessionNotification(
                        static_cast<IAudioSessionEvents*>(m_waker)))) {
                    control->AddRef();
                    m_watched.push_back(control);
                }
            });
        }

        if (count != m_lastCount) {
            if (count > 0) {
                Wh_Log(L"Total Discord sessions muted: %d", count);
            } else {
                Wh_Log(L"Waiting for Discord audio session (e.g., screen share)");
            }
            m_lastCount = count;
        }
    }

    void ClearMuted() {
        for (IAudioSessionControl* control : m_watched) {
            control->UnregisterAudioSessionNotification(
                static_cast<IAudioSessionEvents*>(m_waker));
            control->Release();
        }
        m_watched.clear();
        m_muted.clear();
        m_lastCount = -1;
    }

    // Register new-session notifications on every active device and drop
    // devices that are gone.
    void WatchSessionManagers(const std::vector<Device>& devices) {
        std::map<std::wstring, ComPtr<IAudioSessionManager2>> current;
        for (auto& device : devices) {
            auto known = m_managers.find(device.id);
            if (known != m_managers.end()) {
                current[device.id] = std::move(known->second);
                m_managers.erase(known);
                continue;
            }
            ComPtr<IAudioSessionManager2> manager;
            ComPtr<IAudioSessionEnumerator> sessions;
            int count = 0;
            // Windows discards new-session notifications until the existing
            // sessions have been counted once.
            if (SUCCEEDED(device.device->Activate(
                    kIidAudioSessionManager2, CLSCTX_ALL, nullptr,
                    (void**)manager.GetAddressOf())) &&
                SUCCEEDED(manager->RegisterSessionNotification(
                    static_cast<IAudioSessionNotification*>(m_waker)))) {
                if (SUCCEEDED(manager->GetSessionEnumerator(&sessions))) {
                    sessions->GetCount(&count);
                }
                current[device.id] = std::move(manager);
            } else {
                Wh_Log(L"Failed to watch sessions on %s", device.name.c_str());
            }
        }
        UnwatchSessionManagers();
        m_managers = std::move(current);
    }

    void UnwatchSessionManagers() {
        for (auto& [id, manager] : m_managers) {
            manager->UnregisterSessionNotification(
                static_cast<IAudioSessionNotification*>(m_waker));
        }
        m_managers.clear();
    }

    Waker* m_waker;
    ComPtr<IMMDeviceEnumerator> m_enumerator;
    bool m_notificationsRegistered = false;
    bool m_appRunning = false;
    const Provider* m_provider = nullptr;
    // Muted sessions and the volume each had before the mod muted it.
    // shortcut: one volume per process and device, so several sessions of one
    // process on one device all get the first one's volume back.
    std::map<SessionKey, float> m_muted;
    std::vector<IAudioSessionControl*> m_watched;
    std::map<std::wstring, ComPtr<IAudioSessionManager2>> m_managers;
    // -1: nothing logged yet, -2: target device missing.
    int m_lastCount = -1;
};

HANDLE g_stopEvent;
HANDLE g_wakeEvent;
HANDLE g_workerThread;
HPOWERNOTIFY g_powerNotify;
std::atomic<bool> g_resumed;

// Declared here because the SDK headers only have them for kernel drivers.
constexpr DWORD kDeviceNotifyCallback = 2;
struct DeviceNotifySubscribeParameters {
    ULONG(CALLBACK* callback)(PVOID context, ULONG type, PVOID setting);
    PVOID context;
};

ULONG CALLBACK OnPowerEvent(PVOID, ULONG type, PVOID) {
    if (type == PBT_APMRESUMEAUTOMATIC) {
        g_resumed = true;
        SetEvent(g_wakeEvent);
    }
    return 0;
}

// Must outlive the registration.
DeviceNotifySubscribeParameters g_powerParams{OnPowerEvent, nullptr};

DWORD WINAPI WorkerThread(LPVOID) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        Wh_Log(L"COM init failed");
        return 1;
    }
    HANDLE events[] = {g_stopEvent, g_wakeEvent};
    bool stop = false;
    while (!stop) {
        Monitor monitor(g_wakeEvent);
        if (!monitor.Init()) {
            break;
        }
        DWORD wait = 0;
        while (true) {
            if (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, wait) ==
                WAIT_OBJECT_0) {
                stop = true;
                break;
            }
            if (g_resumed.exchange(false)) {
                // Like the app: wait for audio devices to settle, then unmute
                // (the destructor) and start over with fresh notifications.
                Wh_Log(L"System resumed, restarting in 3 seconds");
                stop = WaitForSingleObject(g_stopEvent, kResumeSettleMs) ==
                       WAIT_OBJECT_0;
                break;
            }
            wait = monitor.Tick();
        }
        // The destructor restores Discord and unregisters every notification.
    }
    CoUninitialize();
    Wh_Log(L"Stopped monitoring");
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }
    // The tool process's main thread ends at the hooked entry point, so this
    // thread keeps the process alive.
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    if (PowerRegisterSuspendResumeNotification(kDeviceNotifyCallback,
                                               &g_powerParams,
                                               &g_powerNotify) != ERROR_SUCCESS) {
        // Not fatal: device events and the poll still catch up after resume.
        Wh_Log(L"Failed to register for resume notifications");
        g_powerNotify = nullptr;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_powerNotify) {
        PowerUnregisterSuspendResumeNotification(g_powerNotify);
    }
    SetEvent(g_stopEvent);
    // Wait for the worker to unmute Discord before the process exits.
    WaitForSingleObject(g_workerThread, 10 * 1000);
    CloseHandle(g_workerThread);
    CloseHandle(g_wakeEvent);
    CloseHandle(g_stopEvent);
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

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
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

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
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

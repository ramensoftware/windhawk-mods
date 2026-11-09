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
   the provider (speakers, headphones). Only the mute changes, never the
   volume.
2. Leaves the provider's other virtual devices alone.
3. When the mod is disabled, or the default device switches to another
   provider, unmutes Discord on the devices the rule no longer covers.

With FxSound, `FxSound Speakers` is usually the only FxSound device, so every
other output device is outside the provider and Discord is muted on all of
them, voice chat included.

It reacts to Windows audio notifications right away (default device changed,
device added or removed, new audio session such as a screen share, a muted
session was unmuted) and also checks every 30 seconds.

## Windows remembers the mute

The mute is Discord's per-app setting in the Volume Mixer, and Windows saves it
for each output device, so it outlives the mod. The mod records each device it
muted Discord on and unmutes Discord there once the rule no longer covers it,
including after a crash. Windows only lets the mod do that while Discord is
running. If Discord is closed when you disable the mod, Discord stays muted on
those devices: open the Volume Mixer and unmute Discord, or enable the mod
again while Discord is running and switch the default device away from the
provider.

## Mutelith app

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
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

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
                                          L"DiscordCanary.exe",
                                          L"DiscordDevelopment.exe"};
constexpr const wchar_t* kAppExe = L"Mutelith.exe";


struct Provider {
    // Provider name, also the name prefix shared by all devices it creates.
    const wchar_t* name;
    // The provider device where Discord is muted too.
    const wchar_t* target;
};

constexpr Provider kSonar{L"SteelSeries Sonar", L"SteelSeries Sonar - Microphone"};
constexpr Provider kFxSound{L"FxSound", L"FxSound Speakers"};

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
    if (either(L"FxSound")) {
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

std::wstring SessionInstanceId(IAudioSessionControl2* control) {
    LPWSTR id = nullptr;
    if (FAILED(control->GetSessionInstanceIdentifier(&id)) || !id) {
        return {};
    }
    std::wstring text = id;
    CoTaskMemFree(id);
    return text;
}

// Windows saves Discord's per-app mute for each output device, so the mod
// records where it muted each Discord build and can undo leftovers after an
// unclean exit. One storage value per build and endpoint.
std::wstring MutedValueName(const wchar_t* exe, const std::wstring& endpointId) {
    return std::wstring(L"muted-") + exe + L"-" + endpointId;
}

bool IsRecorded(const wchar_t* exe, const std::wstring& endpointId) {
    return Wh_GetIntValue(MutedValueName(exe, endpointId).c_str(), 0) != 0;
}

void Record(const wchar_t* exe, const std::wstring& endpointId) {
    if (!IsRecorded(exe, endpointId) &&
        !Wh_SetIntValue(MutedValueName(exe, endpointId).c_str(), 1)) {
        Wh_Log(L"Failed to record muted device %s for %s", endpointId.c_str(),
               exe);
    }
}

void Forget(const wchar_t* exe, const std::wstring& endpointId) {
    Wh_DeleteValue(MutedValueName(exe, endpointId).c_str());
}

using DiscordMap = std::map<DWORD, const wchar_t*>;

struct Processes {
    // Discord PIDs and their entry in kDiscordExes.
    DiscordMap discord;
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
                result.discord[entry.th32ProcessID] = exe;
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
        if (IsEqualIID(riid, __uuidof(IUnknown)) ||
            IsEqualIID(riid, __uuidof(IMMNotificationClient))) {
            *object = static_cast<IMMNotificationClient*>(this);
        } else if (IsEqualIID(riid, __uuidof(IAudioSessionNotification))) {
            *object = static_cast<IAudioSessionNotification*>(this);
        } else if (IsEqualIID(riid, __uuidof(IAudioSessionEvents))) {
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

    // Leaves Discord muted, so restarting after resume has no unmuted gap.
    // Call Restore() first to unmute.
    ~Monitor() {
        ClearMuted();
        UnwatchSessionManagers();
        if (m_enumerator && m_notificationsRegistered) {
            m_enumerator->UnregisterEndpointNotificationCallback(m_waker);
        }
        m_waker->Release();
    }

    bool Init() {
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                    CLSCTX_ALL, IID_PPV_ARGS(&m_enumerator)))) {
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
                m_appRunning = true;
                // Once, so the mod never fights the app's own mutes.
                Release(devices, {}, processes.discord);
                ClearMuted();
            }
            return PollMs();
        }
        if (m_appRunning) {
            Wh_Log(L"Mutelith.exe stopped, the mod takes over");
            m_appRunning = false;
        }

        ForgetExited(processes.discord);

        const Provider* detected = DefaultProvider();
        if (detected != m_provider) {
            Wh_Log(L"Default device provider changed from %s to %s",
                   m_provider ? m_provider->name : L"none",
                   detected ? detected->name : L"none");
            // Nothing is unmuted here: Release() below unmutes Discord on the
            // devices the new rule does not cover, without a gap on the others.
            ClearMuted();
            m_provider = detected;
        }

        if (!m_provider) {
            Release(devices, {}, processes.discord);
            return PollMs();
        }

        std::optional<std::vector<const Device*>> targets = Targets(devices);
        if (!targets) {
            if (m_lastCount != -2) {
                Wh_Log(L"%s not found", m_provider->target);
                m_lastCount = -2;
            }
            return PollMs();
        }

        if (!processes.discord.empty()) {
            Apply(*targets, processes.discord);
        }
        std::set<std::wstring> keep;
        for (const Device* device : *targets) {
            keep.insert(device->id);
        }
        Release(devices, keep, processes.discord);
        return PollMs();
    }

    // Unmute Discord on every device where the mod muted it.
    void Restore() {
        int count = Release(ActiveDevices(), {}, ScanProcesses().discord);
        Wh_Log(L"Unmuted %d Discord session(s)", count);
        ClearMuted();
    }

   private:
    // Endpoint ID and session instance ID.
    using SessionKey = std::pair<std::wstring, std::wstring>;

    struct Muted {
        DWORD pid;
        // Registered for volume events; unregistered and released on erase.
        IAudioSessionControl* watched;
    };

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

    // Calls visit(control, control2, pid) for each session with an owner.
    template <typename F>
    void ForEachSession(const Device& device, F&& visit) {
        ComPtr<IAudioSessionManager2> manager;
        ComPtr<IAudioSessionEnumerator> sessions;
        int count = 0;
        if (FAILED(device.device->Activate(__uuidof(IAudioSessionManager2),
                                           CLSCTX_ALL, nullptr,
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
                SUCCEEDED(control.As(&control2)) &&
                SUCCEEDED(control2->GetProcessId(&pid)) && pid != 0) {
                visit(control.Get(), control2.Get(), pid);
            }
        }
    }

    // The first target device, then every device outside the provider.
    // Empty optional when the target device is missing.
    std::optional<std::vector<const Device*>> Targets(
        const std::vector<Device>& devices) {
        bool targetFound = false;
        std::vector<const Device*> targets;
        for (auto& device : devices) {
            if (!targetFound && ContainsCi(device.name, m_provider->target)) {
                targetFound = true;
                targets.push_back(&device);
            } else if (!ContainsCi(device.name, m_provider->name)) {
                targets.push_back(&device);
            }
        }
        if (!targetFound) {
            return std::nullopt;
        }
        return targets;
    }

    void Apply(const std::vector<const Device*>& targets,
               const DiscordMap& discord) {
        int count = 0;
        for (const Device* device : targets) {
            ForEachSession(*device, [&](IAudioSessionControl* control,
                                        IAudioSessionControl2* control2,
                                        DWORD pid) {
                auto process = discord.find(pid);
                if (process == discord.end()) {
                    return;
                }
                ComPtr<ISimpleAudioVolume> volume;
                if (FAILED(control->QueryInterface(IID_PPV_ARGS(&volume)))) {
                    return;
                }

                SessionKey key{device->id, SessionInstanceId(control2)};
                bool known = m_muted.contains(key);
                BOOL muted = FALSE;
                if (known && SUCCEEDED(volume->GetMute(&muted)) && muted) {
                    count++;
                    return;
                }

                // Recorded first, so a crash right after muting still leaves
                // a record.
                Record(process->second, device->id);
                if (FAILED(volume->SetMute(TRUE, nullptr))) {
                    return;
                }
                count++;
                if (known) {
                    return;
                }

                Wh_Log(L"%s (PID %u) muted in %s", process->second, pid,
                       device->name.c_str());
                IAudioSessionControl* watched = nullptr;
                if (m_notificationsRegistered &&
                    SUCCEEDED(control->RegisterAudioSessionNotification(
                        static_cast<IAudioSessionEvents*>(m_waker)))) {
                    control->AddRef();
                    watched = control;
                }
                m_muted[key] = {pid, watched};
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

    // Unmute Discord on recorded devices that are not in keep, and forget each
    // record once that Discord build is unmuted there. Returns the number of
    // unmuted sessions.
    int Release(const std::vector<Device>& devices,
                const std::set<std::wstring>& keep,
                const DiscordMap& discord) {
        // Discord's saved mute can only be reached through a running Discord.
        if (discord.empty()) {
            return 0;
        }
        int count = 0;
        for (auto& device : devices) {
            if (device.id.empty() || keep.contains(device.id)) {
                continue;
            }
            for (const wchar_t* exe : kDiscordExes) {
                if (!IsRecorded(exe, device.id)) {
                    continue;
                }
                bool found = false;
                bool failed = false;
                ForEachSession(device, [&](IAudioSessionControl* control,
                                           IAudioSessionControl2*, DWORD pid) {
                    auto process = discord.find(pid);
                    if (process == discord.end() || process->second != exe) {
                        return;
                    }
                    found = true;
                    ComPtr<ISimpleAudioVolume> volume;
                    BOOL muted = FALSE;
                    if (FAILED(control->QueryInterface(IID_PPV_ARGS(&volume))) ||
                        FAILED(volume->GetMute(&muted))) {
                        failed = true;
                        return;
                    }
                    if (!muted) {
                        return;
                    }
                    if (FAILED(volume->SetMute(FALSE, nullptr))) {
                        failed = true;
                        return;
                    }
                    count++;
                    Wh_Log(L"%s (PID %u) unmuted in %s", exe, pid,
                           device.name.c_str());
                });
                // Kept until a session of that Discord build shows up there,
                // so its saved mute is undone.
                if (found && !failed) {
                    Forget(exe, device.id);
                }
            }
        }
        return count;
    }

    void Erase(std::map<SessionKey, Muted>::iterator entry) {
        if (IAudioSessionControl* control = entry->second.watched) {
            control->UnregisterAudioSessionNotification(
                static_cast<IAudioSessionEvents*>(m_waker));
            control->Release();
        }
        m_muted.erase(entry);
    }

    void ForgetExited(const DiscordMap& discord) {
        for (auto entry = m_muted.begin(); entry != m_muted.end();) {
            auto next = std::next(entry);
            if (!discord.contains(entry->second.pid)) {
                Erase(entry);
            }
            entry = next;
        }
    }

    void ClearMuted() {
        while (!m_muted.empty()) {
            Erase(m_muted.begin());
        }
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
                    __uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
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
    // Sessions the mod muted, by endpoint and session instance.
    std::map<SessionKey, Muted> m_muted;
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
                // Like the app: wait for audio devices to settle, then start
                // over with fresh notifications. Discord stays muted meanwhile.
                Wh_Log(L"System resumed, restarting in 3 seconds");
                stop = WaitForSingleObject(g_stopEvent, kResumeSettleMs) ==
                       WAIT_OBJECT_0;
                break;
            }
            wait = monitor.Tick();
        }
        if (stop) {
            monitor.Restore();
        }
        // The destructor unregisters every notification.
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

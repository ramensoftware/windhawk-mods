// ==WindhawkMod==
// @id              automatic-whatsapp-audio-ducking
// @name            Automatic WhatsApp Audio Ducking
// @description     Lowers the volume of chosen apps (e.g. Spotify) while WhatsApp or other apps play audio, then restores it
// @description:de-DE Senkt die Lautstärke ausgewählter Apps (z. B. Spotify), solange WhatsApp Audio abspielt, und stellt sie danach wieder her
// @version         1.0
// @author          wa0dim
// @github          https://github.com/wa0dim
// @include         windhawk.exe
// @compilerOptions -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Automatic WhatsApp Audio Ducking

While WhatsApp or other chosen apps (voice messages, videos, calls' media, ...) produce sound, the
per-app volume of the configured target apps (default: Spotify) is lowered to
a fixed percentage. When WhatsApp or the specified apps has been silent for a short moment, the
original volume is restored.

This only changes the per-app volume slider of the Windows volume mixer, not
the system volume.

## Choosing messengers
In the settings under "Messengers" you can switch on WhatsApp (default),
Telegram, Signal, Discord, Microsoft Teams, Skype, Zoom, Slack, Viber and
Element. Any other app can be added by its process name under "Additional
trigger apps" (e.g. `Threema.exe`).

## Calls
Windows lowers other apps by itself during calls (communication streams). The
mod detects this via the Windows ducking notification and stays out of the way,
so the volume is not lowered twice (changeable with "Also duck calls that
Windows already ducks"). If a call is not flagged by Windows but a selected
messenger has an open microphone stream, the mod ducks for the whole call,
also between ringtones and beeps. The log shows who handles the call.

## Notes
- Messengers are detected by process name. Audio from `msedgewebview2.exe`
  helper processes started by a selected messenger counts as well; other child
  processes (e.g. a browser opened from a link) do not.
- If nothing happens, check the Windhawk log: the mod logs the process name of
  every audio session it sees when a new one appears.
- If you change the volume of a target app yourself while it is ducked, your
  value is taken as the new "original" volume.
- Only the default playback device is handled.

## Warning:
I used AI (Claude Sonnet 5.5) to create this mod. I am not responsible for any damage using this mod may cause. Use at your own risk.
However I did ask Claude to check for security vulnerabilities.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duckPercent: 20
  $name: Ducked volume (%)
  $description: Volume the target apps are lowered to while a trigger app plays audio (0-100)
- targetApps: [Spotify.exe]
  $name: Apps to duck
  $description: Process names of the apps whose volume gets lowered
- messengers:
  - whatsapp: true
    $name: WhatsApp
  - telegram: false
    $name: Telegram
  - signal: false
    $name: Signal
  - discord: false
    $name: Discord
  - teams: false
    $name: Microsoft Teams
  - skype: false
    $name: Skype
  - zoom: false
    $name: Zoom
  - slack: false
    $name: Slack
  - viber: false
    $name: Viber
  - element: false
    $name: Element
  $name: Messengers
  $description: Messengers whose audio lowers the volume of the apps to duck
- triggerApps: [""]
  $name: Additional trigger apps
  $description: Further process names (e.g. Threema.exe) that trigger ducking when they play audio (their msedgewebview2.exe helpers count too)
- peakThresholdPermille: 10
  $name: Audio detection threshold (per mille)
  $description: Peak level above which an app counts as playing audio (1-1000)
- releaseDelayMs: 800
  $name: Release delay (ms)
  $description: How long to wait after the trigger went silent before restoring the volume
- duckWindowsCalls: false
  $name: Also duck calls that Windows already ducks
  $description: Off = during calls that Windows itself ducks (communication streams) the mod does nothing, so the volume isn't lowered twice. On = the mod lowers the volume additionally
- fadeMs: 200
  $name: Fade time (ms)
  $description: Time for a full-range fade, 0 = instant
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <shellapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// The bundled MinGW headers only forward-declare IAudioMeterInformation, so
// define the interface (vtable layout from endpointvolume.idl) here.
struct IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(
        UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(
        DWORD* pdwHardwareSupportMask) = 0;
};

// C02216F6-8C67-4B5B-9D00-D008E73E0064
constexpr GUID kIidAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

namespace {

constexpr DWORD kPollMs = 60;
constexpr ULONGLONG kProcessCacheMs = 3000;
constexpr float kUserChangeTolerance = 0.02f;

struct Settings {
    float duckVolume = 0.2f;
    float peakThreshold = 0.01f;
    DWORD releaseDelayMs = 800;
    DWORD fadeMs = 200;
    bool duckWindowsCalls = false;
    std::vector<std::wstring> targets;
    std::vector<std::wstring> triggers;
};

// Selectable messengers: settings key and (lowercase) process names.
struct MessengerPreset {
    PCWSTR setting;
    PCWSTR exes[2];
};

constexpr MessengerPreset kMessengers[] = {
    {L"messengers.whatsapp", {L"whatsapp.exe", L"whatsapp.root.exe"}},
    {L"messengers.telegram", {L"telegram.exe", nullptr}},
    {L"messengers.signal", {L"signal.exe", nullptr}},
    {L"messengers.discord", {L"discord.exe", nullptr}},
    {L"messengers.teams", {L"ms-teams.exe", L"teams.exe"}},
    {L"messengers.skype", {L"skype.exe", nullptr}},
    {L"messengers.zoom", {L"zoom.exe", nullptr}},
    {L"messengers.slack", {L"slack.exe", nullptr}},
    {L"messengers.viber", {L"viber.exe", nullptr}},
    {L"messengers.element", {L"element.exe", nullptr}},
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

HANDLE g_stopEvent;
HANDLE g_thread;

template <typename T>
struct Com {
    T* p = nullptr;
    Com() = default;
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    ~Com() { Reset(); }
    void Reset() {
        if (p) {
            p->Release();
            p = nullptr;
        }
    }
    T** operator&() {
        Reset();
        return &p;
    }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

std::wstring Lower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), towlower);
    return s;
}

bool Contains(const std::vector<std::wstring>& list, const std::wstring& lowerName) {
    return std::find(list.begin(), list.end(), lowerName) != list.end();
}

std::vector<std::wstring> LoadList(const wchar_t* key) {
    std::vector<std::wstring> result;
    for (int i = 0; i < 64; i++) {
        PCWSTR value = Wh_GetStringSetting(L"%s[%d]", key, i);
        if (!value) {
            break;
        }
        std::wstring s = value;
        Wh_FreeStringSetting(value);
        if (s.empty()) {
            break;
        }
        result.push_back(Lower(s));
    }
    return result;
}

void LoadSettings() {
    Settings s;
    s.duckVolume = std::clamp(Wh_GetIntSetting(L"duckPercent"), 0, 100) / 100.0f;
    s.peakThreshold =
        std::clamp(Wh_GetIntSetting(L"peakThresholdPermille"), 1, 1000) / 1000.0f;
    s.releaseDelayMs = std::max(0, Wh_GetIntSetting(L"releaseDelayMs"));
    s.duckWindowsCalls = Wh_GetIntSetting(L"duckWindowsCalls") != 0;
    s.fadeMs = std::max(0, Wh_GetIntSetting(L"fadeMs"));
    s.targets = LoadList(L"targetApps");
    s.triggers = LoadList(L"triggerApps");
    for (const auto& preset : kMessengers) {
        if (Wh_GetIntSetting(preset.setting)) {
            for (PCWSTR exe : preset.exes) {
                if (exe) {
                    s.triggers.push_back(exe);
                }
            }
        }
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// ---- process info cache ---------------------------------------------------

struct ProcInfo {
    DWORD parent;
    std::wstring name;  // lowercase
};

std::map<DWORD, ProcInfo> g_procs;
ULONGLONG g_procsTime = 0;

void RefreshProcs() {
    g_procs.clear();
    g_procsTime = GetTickCount64();

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return;
    }
    PROCESSENTRY32W pe{sizeof(pe)};
    if (Process32FirstW(snap, &pe)) {
        do {
            g_procs[pe.th32ProcessID] = {pe.th32ParentProcessID, Lower(pe.szExeFile)};
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
}

const ProcInfo* GetProc(DWORD pid) {
    auto it = g_procs.find(pid);
    if (it == g_procs.end() && GetTickCount64() - g_procsTime > 500) {
        RefreshProcs();
        it = g_procs.find(pid);
    }
    return it == g_procs.end() ? nullptr : &it->second;
}

// True if the process is a trigger app itself, or is a WebView2 helper whose
// ancestor chain contains a trigger app. Other child processes (e.g. a browser
// opened from a WhatsApp link) deliberately do not count.
// Note: GetProc may refresh the cache, so no ProcInfo pointer is kept across
// calls.
bool IsTriggerProcess(DWORD pid, const std::wstring& name,
                      const std::vector<std::wstring>& triggers) {
    if (Contains(triggers, name)) {
        return true;
    }
    if (name != L"msedgewebview2.exe") {
        return false;
    }
    for (int depth = 0; depth < 8; depth++) {
        const ProcInfo* info = GetProc(pid);
        if (!info) {
            return false;
        }
        DWORD parent = info->parent;
        if (parent == pid || parent == 0) {
            return false;
        }
        pid = parent;

        info = GetProc(pid);
        if (!info) {
            return false;
        }
        if (Contains(triggers, info->name)) {
            return true;
        }
    }
    return false;
}

// ---- ducking state ---------------------------------------------------------

struct DuckState {
    float original;
    float lastSet;
};

std::map<std::wstring, DuckState> g_duck;  // key: session instance identifier
ULONGLONG g_lastTriggerActive = 0;
int g_callState = 0;  // 0 none, 1 Windows handles the call, 2 mod handles it
std::set<DWORD> g_loggedPids;

float MoveToward(float cur, float target, float step) {
    if (cur < target) {
        return std::min(cur + step, target);
    }
    return std::max(cur - step, target);
}

// ---- call detection ----------------------------------------------------------

// Session instance identifiers of communication streams, as reported by the
// Windows ducking notification (the mechanism Windows itself uses to lower
// other audio during calls).
std::set<std::wstring> g_commSessions;
SRWLOCK g_commLock = SRWLOCK_INIT;

bool IsCommunicationSession(const wchar_t* id) {
    AcquireSRWLockShared(&g_commLock);
    bool found = g_commSessions.count(id) != 0;
    ReleaseSRWLockShared(&g_commLock);
    return found;
}

class DuckNotifier final : public IAudioVolumeDuckNotification {
   public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == __uuidof(IAudioVolumeDuckNotification)) {
            *ppv = static_cast<IAudioVolumeDuckNotification*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return ref;
    }
    STDMETHODIMP OnVolumeDuckNotification(LPCWSTR sessionID,
                                          UINT32 countCommunicationSessions) override {
        // Runs on a Windows audio thread: no exception may escape.
        try {
            AcquireSRWLockExclusive(&g_commLock);
            if (countCommunicationSessions == 0) {
                g_commSessions.clear();
            } else if (sessionID && wcslen(sessionID) < 512 &&
                       g_commSessions.size() < 64) {
                g_commSessions.insert(sessionID);
            }
            ReleaseSRWLockExclusive(&g_commLock);
        } catch (...) {
            ReleaseSRWLockExclusive(&g_commLock);
        }
        return S_OK;
    }
    STDMETHODIMP OnVolumeUnduckNotification(LPCWSTR sessionID) override {
        // Sent when the last communication stream ended.
        AcquireSRWLockExclusive(&g_commLock);
        g_commSessions.clear();
        ReleaseSRWLockExclusive(&g_commLock);
        return S_OK;
    }

   private:
    LONG m_ref = 1;
};

// Keeps the duck notification registered on the default communications device.
struct CommWatcher {
    Com<IAudioSessionManager2> mgr;
    DuckNotifier* notifier = nullptr;
    std::wstring deviceId;

    void Update(IMMDeviceEnumerator* enumerator) {
        Com<IMMDevice> device;
        if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eCommunications,
                                                       &device)) ||
            !device) {
            return;
        }
        LPWSTR id = nullptr;
        if (FAILED(device->GetId(&id)) || !id) {
            return;
        }
        std::wstring newId = id;
        CoTaskMemFree(id);
        if (newId == deviceId) {
            return;  // already registered, or registration failed for this device
        }

        Stop();
        if (FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                    nullptr, (void**)&mgr)) ||
            !mgr) {
            return;
        }
        notifier = new DuckNotifier();
        HRESULT hr = mgr->RegisterDuckNotification(nullptr, notifier);
        if (FAILED(hr)) {
            Wh_Log(L"RegisterDuckNotification failed: 0x%08X", (unsigned)hr);
            Stop();
            deviceId = newId;  // don't retry every second
            return;
        }
        deviceId = newId;
    }

    void Stop() {
        if (mgr && notifier) {
            mgr->UnregisterDuckNotification(notifier);
        }
        if (notifier) {
            notifier->Release();
            notifier = nullptr;
        }
        mgr.Reset();
        deviceId.clear();
        AcquireSRWLockExclusive(&g_commLock);
        g_commSessions.clear();
        ReleaseSRWLockExclusive(&g_commLock);
    }
};

void ReleaseManagers(std::vector<IAudioSessionManager2*>& managers) {
    for (auto* m : managers) {
        m->Release();
    }
    managers.clear();
}

void RefreshCaptureManagers(IMMDeviceEnumerator* enumerator,
                            std::vector<IAudioSessionManager2*>& out) {
    ReleaseManagers(out);
    Com<IMMDeviceCollection> devices;
    UINT n = 0;
    if (FAILED(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE,
                                              &devices)) ||
        !devices || FAILED(devices->GetCount(&n))) {
        return;
    }
    for (UINT i = 0; i < n && i < 16; i++) {
        Com<IMMDevice> device;
        IAudioSessionManager2* m = nullptr;
        if (SUCCEEDED(devices->Item(i, &device)) && device &&
            SUCCEEDED(device->Activate(__uuidof(IAudioSessionManager2),
                                       CLSCTX_ALL, nullptr, (void**)&m)) &&
            m) {
            out.push_back(m);
        }
    }
}

// A trigger app with an open (active) microphone stream is in a call. This
// works even when the app doesn't flag its stream as a communication stream.
bool CaptureCallActive(const std::vector<IAudioSessionManager2*>& managers,
                       const std::vector<std::wstring>& triggers) {
    for (auto* m : managers) {
        Com<IAudioSessionEnumerator> sessions;
        int count = 0;
        if (FAILED(m->GetSessionEnumerator(&sessions)) || !sessions ||
            FAILED(sessions->GetCount(&count))) {
            continue;
        }
        for (int i = 0; i < count; i++) {
            Com<IAudioSessionControl> ctl;
            AudioSessionState state;
            if (FAILED(sessions->GetSession(i, &ctl)) || !ctl ||
                FAILED(ctl->GetState(&state)) || state != AudioSessionStateActive) {
                continue;
            }
            Com<IAudioSessionControl2> ctl2;
            DWORD pid = 0;
            if (FAILED(ctl->QueryInterface(__uuidof(IAudioSessionControl2),
                                           (void**)&ctl2)) ||
                !ctl2 || FAILED(ctl2->GetProcessId(&pid)) || pid == 0) {
                continue;
            }
            const ProcInfo* info = GetProc(pid);
            if (!info) {
                continue;
            }
            const std::wstring name = info->name;
            if (IsTriggerProcess(pid, name, triggers)) {
                return true;
            }
        }
    }
    return false;
}

void Poll(IAudioSessionManager2* mgr,
          const std::vector<IAudioSessionManager2*>& captureMgrs, bool restoreAll) {
    Settings s = GetSettings();

    ULONGLONG now = GetTickCount64();
    if (now - g_procsTime > kProcessCacheMs) {
        RefreshProcs();
    }

    Com<IAudioSessionEnumerator> sessions;
    int count = 0;
    if (FAILED(mgr->GetSessionEnumerator(&sessions)) || !sessions ||
        FAILED(sessions->GetCount(&count))) {
        return;
    }

    struct Target {
        std::wstring id;
        ISimpleAudioVolume* vol;
        std::wstring name;
    };
    std::vector<Target> targets;
    bool triggerActive = false;
    bool windowsCall = false;

    for (int i = 0; i < count; i++) {
        Com<IAudioSessionControl> ctl;
        if (FAILED(sessions->GetSession(i, &ctl)) || !ctl) {
            continue;
        }
        Com<IAudioSessionControl2> ctl2;
        if (FAILED(ctl->QueryInterface(__uuidof(IAudioSessionControl2),
                                       (void**)&ctl2)) ||
            !ctl2 || ctl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }

        DWORD pid = 0;
        if (FAILED(ctl2->GetProcessId(&pid)) || pid == 0) {
            continue;
        }
        // Copy the name: later GetProc calls may invalidate ProcInfo pointers.
        const ProcInfo* info = GetProc(pid);
        if (!info) {
            continue;
        }
        const std::wstring name = info->name;

        if (g_loggedPids.size() > 1000) {
            g_loggedPids.clear();
        }
        if (g_loggedPids.insert(pid).second) {
            Wh_Log(L"Audio session: %s (pid %u)", name.c_str(), pid);
        }

        bool isTrigger = IsTriggerProcess(pid, name, s.triggers);
        bool isTarget = !isTrigger && Contains(s.targets, name);

        if (isTrigger && !triggerActive) {
            AudioSessionState state;
            Com<IAudioMeterInformation> meter;
            float peak = 0;
            if (SUCCEEDED(ctl->GetState(&state)) &&
                state == AudioSessionStateActive &&
                SUCCEEDED(ctl->QueryInterface(kIidAudioMeterInformation,
                                              (void**)&meter)) &&
                meter && SUCCEEDED(meter->GetPeakValue(&peak)) &&
                peak > s.peakThreshold) {
                triggerActive = true;
            }
        }

        // Windows reports communication (call) streams through the ducking
        // notification; a trigger app owning one of them means a call.
        if (isTrigger && !windowsCall) {
            AudioSessionState state;
            LPWSTR id = nullptr;
            if (SUCCEEDED(ctl->GetState(&state)) &&
                state == AudioSessionStateActive &&
                SUCCEEDED(ctl2->GetSessionInstanceIdentifier(&id)) && id &&
                IsCommunicationSession(id)) {
                windowsCall = true;
            }
            if (id) {
                CoTaskMemFree(id);
            }
        }

        if (isTarget) {
            LPWSTR id = nullptr;
            ISimpleAudioVolume* vol = nullptr;
            if (SUCCEEDED(ctl2->GetSessionInstanceIdentifier(&id)) && id &&
                SUCCEEDED(ctl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                              (void**)&vol)) &&
                vol) {
                targets.push_back({id, vol, name});
            }
            if (id) {
                CoTaskMemFree(id);
            }
        }
    }

    // Calls: Windows already lowers other apps itself for communication
    // streams, so the mod stays out of the way there (unless asked otherwise)
    // to avoid ducking twice. A call that Windows doesn't flag (detected only
    // by an open microphone stream) is ducked by the mod for its whole length,
    // regardless of the pauses between ringtones/beeps/speech.
    bool windowsHandlesCall = windowsCall && !s.duckWindowsCalls;
    bool modHandlesCall = false;
    if (!restoreAll && !windowsHandlesCall) {
        modHandlesCall =
            windowsCall || CaptureCallActive(captureMgrs, s.triggers);
    }

    int callState = windowsHandlesCall ? 1 : (modHandlesCall ? 2 : 0);
    if (callState != g_callState) {
        g_callState = callState;
        if (callState == 1) {
            Wh_Log(L"Call started (Windows ducks, mod stays out)");
        } else if (callState == 2) {
            Wh_Log(L"Call started (mod ducks)");
        } else {
            Wh_Log(L"Call ended");
        }
    }

    if (windowsHandlesCall) {
        g_lastTriggerActive = 0;  // restore anything we ducked, without delay
    } else if (triggerActive || modHandlesCall) {
        g_lastTriggerActive = now;
    }
    bool ducking = !restoreAll && g_lastTriggerActive != 0 &&
                   now - g_lastTriggerActive <= s.releaseDelayMs;

    float step = s.fadeMs > 0 ? (float)kPollMs / s.fadeMs : 1.0f;
    std::set<std::wstring> seen;

    for (auto& t : targets) {
        seen.insert(t.id);

        float cur = 0;
        if (SUCCEEDED(t.vol->GetMasterVolume(&cur))) {
            auto it = g_duck.find(t.id);

            if (ducking) {
                if (it == g_duck.end()) {
                    it = g_duck.insert({t.id, {cur, cur}}).first;
                    Wh_Log(L"Ducking %s from %d%%", t.name.c_str(),
                           (int)(cur * 100 + 0.5f));
                } else if (fabsf(cur - it->second.lastSet) > kUserChangeTolerance) {
                    // User changed the volume while ducked: new baseline.
                    it->second.original = cur;
                }
                float desired = std::min(it->second.original, s.duckVolume);
                float next = MoveToward(cur, desired, step);
                if (next != cur) {
                    t.vol->SetMasterVolume(next, nullptr);
                }
                it->second.lastSet = next;
            } else if (it != g_duck.end()) {
                if (fabsf(cur - it->second.lastSet) > kUserChangeTolerance) {
                    // User took over; don't touch it.
                    g_duck.erase(it);
                } else {
                    float next = restoreAll
                                     ? it->second.original
                                     : MoveToward(cur, it->second.original, step);
                    if (next != cur) {
                        t.vol->SetMasterVolume(next, nullptr);
                    }
                    if (next == it->second.original) {
                        Wh_Log(L"Restored %s to %d%%", t.name.c_str(),
                               (int)(next * 100 + 0.5f));
                        g_duck.erase(it);
                    } else {
                        it->second.lastSet = next;
                    }
                }
            }
        }
        t.vol->Release();
    }

    for (auto it = g_duck.begin(); it != g_duck.end();) {
        it = seen.count(it->first) ? std::next(it) : g_duck.erase(it);
    }
}

bool GetManager(Com<IMMDeviceEnumerator>& enumerator,
                Com<IAudioSessionManager2>& mgr) {
    mgr.Reset();
    Com<IMMDevice> device;
    return SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                         &device)) &&
           device &&
           SUCCEEDED(device->Activate(__uuidof(IAudioSessionManager2),
                                      CLSCTX_ALL, nullptr, (void**)&mgr)) &&
           mgr;
}

DWORD WINAPI WorkerThread(LPVOID) {
    bool comInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    Com<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator),
                                (void**)&enumerator)) ||
        !enumerator) {
        Wh_Log(L"Failed to create device enumerator");
        if (comInit) {
            CoUninitialize();
        }
        return 1;
    }

    Com<IAudioSessionManager2> mgr;
    ULONGLONG mgrTime = 0;
    CommWatcher commWatcher;
    std::vector<IAudioSessionManager2*> captureMgrs;

    do {
        ULONGLONG now = GetTickCount64();
        // Re-query the devices regularly to follow device changes.
        if (!mgr || mgrTime == 0 || now - mgrTime > 1000) {
            Com<IAudioSessionManager2> fresh;
            if (GetManager(enumerator, fresh)) {
                mgr.Reset();
                mgr.p = fresh.p;
                fresh.p = nullptr;
            }
            commWatcher.Update(enumerator.p);
            RefreshCaptureManagers(enumerator.p, captureMgrs);
            mgrTime = now;
        }
        if (mgr) {
            try {
                Poll(mgr.p, captureMgrs, false);
            } catch (...) {
                // e.g. out of memory: skip this cycle instead of crashing
            }
        }
    } while (WaitForSingleObject(g_stopEvent, kPollMs) == WAIT_TIMEOUT);

    // Restore everything before exiting.
    if (mgr) {
        Poll(mgr.p, {}, true);
    }
    g_duck.clear();

    commWatcher.Stop();
    ReleaseManagers(captureMgrs);
    mgr.Reset();
    enumerator.Reset();
    if (comInit) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

// ---- tool mod entry points -------------------------------------------------

BOOL WhTool_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
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

        DWORD_PTR entryPoint =
            (DWORD_PTR)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook((void*)entryPoint, (void*)EntryPoint_Hook, nullptr);
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
        HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
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
                                 nullptr, nullptr, FALSE,
                                 CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
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

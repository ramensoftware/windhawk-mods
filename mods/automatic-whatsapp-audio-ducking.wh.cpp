// ==WindhawkMod==
// @id              automatic-whatsapp-audio-ducking
// @name            Automatic WhatsApp Audio Ducking
// @description     Lowers the volume of chosen apps (e.g. Spotify) while WhatsApp or other apps play audio, then restores it
// @description:de-DE Senkt die Lautstärke ausgewählter Apps (z. B. Spotify), solange WhatsApp Audio abspielt, und stellt sie danach wieder her
// @version         1.0
// @author          wa0dim
// @github          https://github.com/wa0dim
// @include         windhawk.exe
// @compilerOptions -lole32 -luuid -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Automatic WhatsApp Audio Ducking

While WhatsApp or other chosen apps (voice messages, videos, calls' media, ...) produce sound, the
per-app volume of the configured target apps (default: Spotify) is lowered to
a fixed percentage. When WhatsApp or the specified apps have been silent for a short moment, the
original volume is restored.

This only changes the per-app volume slider of the Windows volume mixer, not
the system volume.

## Choosing messengers
In the settings under "Messengers" you can switch on WhatsApp (default),
Telegram, Signal, Discord, Microsoft Teams, Zoom, Slack, Viber and Element.
Any other app can be added by its process name under "Additional trigger apps"
(e.g. `Threema.exe`).

## Calls
Windows lowers other apps by itself during calls (communication streams) unless
"Do nothing" is selected in the Sound control panel (Communications tab). The
mod detects such calls via the Windows ducking notification and stays out of
the way, so the volume is not lowered twice (changeable with "Also duck calls
that Windows already ducks"). If a call is not flagged by Windows but a
selected messenger has an open microphone stream, the mod ducks for the whole
call, also between ringtones and beeps. The log shows who handles the call.

## Notes
- Messengers are detected by process name. Audio from `msedgewebview2.exe`
  helper processes started by a selected messenger counts as well; other child
  processes (e.g. a browser opened from a link) do not.
- If nothing happens, check the Windhawk log: the mod logs the process name of
  every audio session it sees when a new one appears.
- If you change the volume of a target app yourself while it is ducked, your
  value is taken as the new "original" volume.
- All active playback devices are handled.
- The mod only works while audio is involved: it waits for Windows audio events
  and does not poll while no trigger app has an open audio stream.
- Windows remembers the volume of each app. While an app is ducked, the mod
  keeps a record of its original volume in Windhawk's storage, so the volume is
  restored after a restart of the mod, a closed and reopened app or a changed
  playback device. If an app stays closed while the mod is turned off, Windows
  can keep its lowered volume until you raise it in the volume mixer.

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
#include <sddl.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
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

constexpr DWORD kPollMs = 60;  // only while audio is involved
constexpr DWORD kIdleWatchdogMs = 5 * 60 * 1000;  // safety net while idle
constexpr ULONGLONG kProcessCacheMs = 3000;
constexpr float kUserChangeTolerance = 0.02f;
constexpr size_t kMaxDuckEntries = 64;
constexpr size_t kMaxWatchedSessions = 256;
constexpr UINT kMaxDevices = 16;

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
    {L"messengers.zoom", {L"zoom.exe", nullptr}},
    {L"messengers.slack", {L"slack.exe", nullptr}},
    {L"messengers.viber", {L"viber.exe", nullptr}},
    {L"messengers.element", {L"element.exe", nullptr}},
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

HANDLE g_stopEvent;
HANDLE g_wakeEvent;  // auto-reset: something happened in the audio system
HANDLE g_thread;
LONG g_devicesDirty = 1;  // the device lists must be rebuilt

void Wake() {
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

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
        WindhawkUtils::StringSetting value =
            WindhawkUtils::StringSetting::make(L"%s[%d]", key, i);
        if (!*value.get()) {
            continue;  // empty entry: skip it, later entries still count
        }
        result.push_back(Lower(value.get()));
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

// Windows lowers other apps during calls unless the user chose "Do nothing"
// (UserDuckingPreference == 3) in the Sound control panel.
bool WindowsDucksCalls() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Multimedia\\Audio",
                     L"UserDuckingPreference", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return true;
    }
    return value != 3;
}

// ---- COM helpers ------------------------------------------------------------

template <typename Interface>
class Unknown : public Interface {
   public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == __uuidof(Interface)) {
            *ppv = static_cast<Interface*>(this);
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

   protected:
    virtual ~Unknown() = default;

   private:
    LONG m_ref = 1;
};

// Playback/capture devices were added, removed or the defaults changed.
class DeviceNotifier final : public Unknown<IMMNotificationClient> {
   public:
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override { return Dirty(); }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return Dirty(); }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return Dirty(); }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override {
        return Dirty();
    }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

   private:
    static HRESULT Dirty() {
        InterlockedExchange(&g_devicesDirty, 1);
        Wake();
        return S_OK;
    }
};

// A new audio session was created on a device.
class SessionCreatedNotifier final : public Unknown<IAudioSessionNotification> {
   public:
    STDMETHODIMP OnSessionCreated(IAudioSessionControl*) override {
        Wake();
        return S_OK;
    }
};

// A watched (trigger app) session changed its state or went away.
class SessionEventsNotifier final : public Unknown<IAudioSessionEvents> {
   public:
    STDMETHODIMP OnDisplayNameChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnIconPathChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnSimpleVolumeChanged(float, WINBOOL, LPCGUID) override {
        return S_OK;
    }
    STDMETHODIMP OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID) override {
        return S_OK;
    }
    STDMETHODIMP OnGroupingParamChanged(LPCGUID, LPCGUID) override { return S_OK; }
    STDMETHODIMP OnStateChanged(AudioSessionState) override {
        Wake();
        return S_OK;
    }
    STDMETHODIMP OnSessionDisconnected(AudioSessionDisconnectReason) override {
        Wake();
        return S_OK;
    }
};

SessionCreatedNotifier* g_createdNotifier;
SessionEventsNotifier* g_eventsNotifier;

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
    float original;     // volume to restore
    float lastSet;      // volume we set last (to notice user changes)
    float duckedLevel;  // level we ducked it to
    bool adopted;       // false for records loaded from storage until seen again
};

// Key: session identifier (stable for an app on a device across launches).
std::map<std::wstring, DuckState> g_duck;
bool g_duckDirty = false;
ULONGLONG g_lastTriggerActive = 0;
int g_callState = 0;  // 0 none, 1 Windows handles the call, 2 mod handles it
std::set<DWORD> g_loggedPids;
std::map<DWORD, ULONGLONG> g_missedPids;  // PID -> process snapshot time of the first miss

float MoveToward(float cur, float target, float step) {
    if (cur < target) {
        return std::min(cur + step, target);
    }
    return std::max(cur - step, target);
}

// Windhawk's mod storage is shared by all signed-in users, while the volumes
// belong to one user's session: the value name contains the user's SID. Empty
// if the SID can't be determined (then nothing is persisted).
std::wstring g_storeName;

std::wstring StoreValueName() {
    std::wstring name;
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        BYTE buf[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD len;
        LPWSTR sid;
        if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &len) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &sid)) {
            name = L"duckedVolumes_";
            name += sid;
            LocalFree(sid);
        }
        CloseHandle(token);
    }
    return name;
}

// Mirror of g_duck in Windhawk storage, so a ducked volume that Windows saved
// can be restored after the mod (or the app) restarted.
void SavePersisted() {
    g_duckDirty = false;
    if (g_storeName.empty()) {
        return;
    }
    if (g_duck.empty()) {
        Wh_DeleteValue(g_storeName.c_str());
        return;
    }
    std::wstring out;
    for (const auto& [key, st] : g_duck) {
        wchar_t nums[64];
        swprintf_s(nums, L"%.4f\t%.4f\t", st.original, st.duckedLevel);
        out += nums;
        out += key;
        out += L'\n';
    }
    Wh_SetStringValue(g_storeName.c_str(), out.c_str());
}

void LoadPersisted() {
    g_storeName = StoreValueName();
    if (g_storeName.empty()) {
        Wh_Log(L"Could not determine the user SID, volumes are not persisted");
        return;
    }
    std::wstring buf(32768, L'\0');
    buf.resize(Wh_GetStringValue(g_storeName.c_str(), buf.data(), buf.size()));

    size_t pos = 0;
    while (pos < buf.size() && g_duck.size() < kMaxDuckEntries) {
        size_t end = buf.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = buf.size();
        }
        std::wstring line = buf.substr(pos, end - pos);
        pos = end + 1;

        size_t t1 = line.find(L'\t');
        size_t t2 = t1 == std::wstring::npos ? t1 : line.find(L'\t', t1 + 1);
        if (t2 == std::wstring::npos) {
            continue;
        }
        float original = wcstof(line.substr(0, t1).c_str(), nullptr);
        float ducked = wcstof(line.substr(t1 + 1, t2 - t1 - 1).c_str(), nullptr);
        std::wstring key = line.substr(t2 + 1);
        if (!(original >= 0 && original <= 1 && ducked >= 0 && ducked <= 1) ||
            key.empty() || key.size() > 1024) {
            continue;
        }
        g_duck[key] = {original, original, ducked, false};
    }
    if (!g_duck.empty()) {
        Wh_Log(L"Loaded %d ducked volume record(s) from storage", (int)g_duck.size());
    }
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

class DuckNotifier final : public Unknown<IAudioVolumeDuckNotification> {
   public:
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
        Wake();
        return S_OK;
    }
    STDMETHODIMP OnVolumeUnduckNotification(LPCWSTR) override {
        // Sent when the last communication stream ended.
        AcquireSRWLockExclusive(&g_commLock);
        g_commSessions.clear();
        ReleaseSRWLockExclusive(&g_commLock);
        Wake();
        return S_OK;
    }
};

// Keeps the duck notification registered on the default communications device.
struct CommWatcher {
    Com<IAudioSessionManager2> mgr;
    DuckNotifier* notifier = nullptr;

    // The known communication sessions are kept when re-registering (e.g. after
    // a device change during a call); the unduck notification and the session
    // state check take care of stale entries.
    void Update(IMMDeviceEnumerator* enumerator) {
        Unregister();
        Com<IMMDevice> device;
        if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eCommunications,
                                                       &device)) ||
            !device ||
            FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                    nullptr, (void**)&mgr)) ||
            !mgr) {
            return;
        }
        notifier = new DuckNotifier();
        HRESULT hr = mgr->RegisterDuckNotification(nullptr, notifier);
        if (FAILED(hr)) {
            Wh_Log(L"RegisterDuckNotification failed: 0x%08X", (unsigned)hr);
            Unregister();
        }
    }

    void Unregister() {
        if (mgr && notifier) {
            mgr->UnregisterDuckNotification(notifier);
        }
        if (notifier) {
            notifier->Release();
            notifier = nullptr;
        }
        mgr.Reset();
    }

    void Stop() {
        Unregister();
        AcquireSRWLockExclusive(&g_commLock);
        g_commSessions.clear();
        ReleaseSRWLockExclusive(&g_commLock);
    }
};

// ---- device managers and watched sessions --------------------------------------

std::vector<IAudioSessionManager2*> g_renderMgrs;
std::vector<IAudioSessionManager2*> g_captureMgrs;

// Trigger-app sessions we get state-change events for. Key: instance identifier.
std::map<std::wstring, IAudioSessionControl*> g_watched;

void Unwatch(std::map<std::wstring, IAudioSessionControl*>::iterator it) {
    it->second->UnregisterAudioSessionNotification(g_eventsNotifier);
    it->second->Release();
}

void UnwatchAll() {
    for (auto it = g_watched.begin(); it != g_watched.end(); ++it) {
        Unwatch(it);
    }
    g_watched.clear();
}

void Watch(const std::wstring& id, IAudioSessionControl* ctl) {
    if (g_watched.count(id) || g_watched.size() >= kMaxWatchedSessions) {
        return;
    }
    if (SUCCEEDED(ctl->RegisterAudioSessionNotification(g_eventsNotifier))) {
        ctl->AddRef();
        g_watched[id] = ctl;
    }
}

void PruneWatched(const std::set<std::wstring>& seen) {
    for (auto it = g_watched.begin(); it != g_watched.end();) {
        if (seen.count(it->first)) {
            ++it;
        } else {
            Unwatch(it);
            it = g_watched.erase(it);
        }
    }
}

void ReleaseManagers(std::vector<IAudioSessionManager2*>& managers) {
    for (auto* m : managers) {
        m->UnregisterSessionNotification(g_createdNotifier);
        m->Release();
    }
    managers.clear();
}

void LoadManagers(IMMDeviceEnumerator* enumerator, EDataFlow flow,
                  std::vector<IAudioSessionManager2*>& out) {
    Com<IMMDeviceCollection> devices;
    UINT n = 0;
    if (FAILED(enumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE,
                                              &devices)) ||
        !devices || FAILED(devices->GetCount(&n))) {
        return;
    }
    for (UINT i = 0; i < n && i < kMaxDevices; i++) {
        Com<IMMDevice> device;
        IAudioSessionManager2* m = nullptr;
        if (SUCCEEDED(devices->Item(i, &device)) && device &&
            SUCCEEDED(device->Activate(__uuidof(IAudioSessionManager2),
                                       CLSCTX_ALL, nullptr, (void**)&m)) &&
            m) {
            m->RegisterSessionNotification(g_createdNotifier);
            out.push_back(m);
        }
    }
}

// ---- polling ---------------------------------------------------------------------

struct Target {
    std::wstring key;
    ISimpleAudioVolume* vol;
    std::wstring name;
    bool wanted;  // still in "Apps to duck" (otherwise only restored)
};

struct TargetList {
    std::vector<Target> items;
    TargetList() = default;
    TargetList(const TargetList&) = delete;
    TargetList& operator=(const TargetList&) = delete;
    ~TargetList() {
        for (auto& t : items) {
            t.vol->Release();
        }
    }
};

struct ScanResult {
    bool triggerPeak = false;  // a trigger app is audibly playing
    bool triggerOpen = false;  // a trigger app has an active stream
    bool windowsCall = false;  // active communication stream of a trigger app
    bool micCall = false;      // trigger app has an active microphone stream
    bool unresolved = false;   // a process wasn't in the (fresh) process cache yet
};

void ScanManager(IAudioSessionManager2* mgr, bool capture, const Settings& s,
                 ScanResult& r, TargetList& targets,
                 std::set<std::wstring>& watchSeen) {
    Com<IAudioSessionEnumerator> sessions;
    int count = 0;
    if (FAILED(mgr->GetSessionEnumerator(&sessions)) || !sessions ||
        FAILED(sessions->GetCount(&count))) {
        return;
    }

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
            // A just-started process may be missing from a very recent
            // snapshot (refreshes are rate limited): look again until a
            // snapshot newer than the first miss has been taken. After that
            // the session is ignored, so a stale session can't keep us polling.
            if (g_missedPids.size() > 1000) {
                g_missedPids.clear();
            }
            auto miss = g_missedPids.emplace(pid, g_procsTime);
            if (miss.second || g_procsTime <= miss.first->second) {
                r.unresolved = true;
            }
            continue;
        }
        if (!g_missedPids.empty()) {
            g_missedPids.erase(pid);
        }
        const std::wstring name = info->name;

        if (g_loggedPids.size() > 1000) {
            g_loggedPids.clear();
        }
        if (g_loggedPids.insert(pid).second) {
            Wh_Log(L"%s session: %s (pid %u)", capture ? L"Capture" : L"Audio",
                   name.c_str(), pid);
        }

        bool isTrigger = IsTriggerProcess(pid, name, s.triggers);

        if (isTrigger) {
            LPWSTR rawId = nullptr;
            if (SUCCEEDED(ctl2->GetSessionInstanceIdentifier(&rawId)) && rawId) {
                std::wstring id = rawId;
                CoTaskMemFree(rawId);

                // Watch first, then read the state: a change in between is
                // either seen by the state read or reported as an event.
                Watch(id, ctl.p);
                watchSeen.insert(id);

                AudioSessionState state;
                if (SUCCEEDED(ctl->GetState(&state)) &&
                    state == AudioSessionStateActive) {
                    r.triggerOpen = true;
                    if (capture) {
                        r.micCall = true;
                    } else {
                        if (IsCommunicationSession(id.c_str())) {
                            r.windowsCall = true;
                        }
                        Com<IAudioMeterInformation> meter;
                        float peak = 0;
                        if (!r.triggerPeak &&
                            SUCCEEDED(ctl->QueryInterface(kIidAudioMeterInformation,
                                                          (void**)&meter)) &&
                            meter && SUCCEEDED(meter->GetPeakValue(&peak)) &&
                            peak > s.peakThreshold) {
                            r.triggerPeak = true;
                        }
                    }
                }
            }
        } else if (!capture) {
            bool isTarget = Contains(s.targets, name);
            if (isTarget || !g_duck.empty()) {
                LPWSTR rawKey = nullptr;
                if (SUCCEEDED(ctl2->GetSessionIdentifier(&rawKey)) && rawKey) {
                    std::wstring key = rawKey;
                    CoTaskMemFree(rawKey);

                    ISimpleAudioVolume* vol = nullptr;
                    // Sessions that are no longer targets but still hold a
                    // ducked volume are kept so they get restored.
                    if ((isTarget || g_duck.count(key)) &&
                        SUCCEEDED(ctl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                      (void**)&vol)) &&
                        vol) {
                        try {
                            targets.items.push_back({key, vol, name, isTarget});
                        } catch (...) {
                            vol->Release();  // not owned by the list yet
                            throw;
                        }
                    }
                }
            }
        }
    }
}

// Returns true while the next poll should come soon (audio is involved).
bool Poll(bool restoreAll) {
    Settings s = GetSettings();

    ULONGLONG now = GetTickCount64();
    if (now - g_procsTime > kProcessCacheMs) {
        RefreshProcs();
    }

    ScanResult r;
    TargetList targets;
    std::set<std::wstring> watchSeen;
    for (auto* m : g_renderMgrs) {
        ScanManager(m, false, s, r, targets, watchSeen);
    }
    for (auto* m : g_captureMgrs) {
        ScanManager(m, true, s, r, targets, watchSeen);
    }
    PruneWatched(watchSeen);

    // Calls: Windows already lowers other apps itself for communication
    // streams, so the mod stays out of the way there (unless asked otherwise)
    // to avoid ducking twice. A call that Windows doesn't flag (or doesn't duck
    // because the user chose "Do nothing"), detected by an open microphone
    // stream, is ducked by the mod for its whole length, regardless of the
    // pauses between ringtones/beeps/speech.
    bool windowsHandlesCall =
        r.windowsCall && !s.duckWindowsCalls && WindowsDucksCalls();
    bool modHandlesCall = !restoreAll && !windowsHandlesCall &&
                          (r.windowsCall || r.micCall);

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
    } else if (r.triggerPeak || modHandlesCall) {
        g_lastTriggerActive = now;
    }
    bool ducking = !restoreAll && g_lastTriggerActive != 0 &&
                   now - g_lastTriggerActive <= s.releaseDelayMs;

    float step = s.fadeMs > 0 ? (float)kPollMs / s.fadeMs : 1.0f;
    bool fading = false;

    // Several sessions can share one key (e.g. two processes of the same exe on
    // the same device). The state machine runs once per key; the other sessions
    // with that key get the same volume (-1 = the first one was left alone).
    // While ducking, other sessions are only ever lowered, never raised.
    struct Applied {
        float volume;  // -1 = left alone
        bool lowering;
    };
    std::map<std::wstring, Applied> appliedThisPoll;

    for (auto& t : targets.items) {
        float cur = 0;
        if (FAILED(t.vol->GetMasterVolume(&cur))) {
            continue;
        }

        if (auto done = appliedThisPoll.find(t.key); done != appliedThisPoll.end()) {
            const Applied& a = done->second;
            bool skip = a.volume < 0 || (a.lowering && a.volume >= cur);
            if (!skip && fabsf(cur - a.volume) > 0.001f) {
                t.vol->SetMasterVolume(a.volume, nullptr);
            }
            continue;
        }
        float outcome = -1.0f;
        bool lowering = false;

        auto it = g_duck.find(t.key);

        if (it != g_duck.end() && !it->second.adopted) {
            // Record from storage: only valid if the app still sits between the
            // ducked level and its original volume (not changed meanwhile).
            const DuckState& d = it->second;
            float low = std::min(d.duckedLevel, d.original) - kUserChangeTolerance;
            if (cur >= low && cur <= d.original + kUserChangeTolerance) {
                it->second.adopted = true;
                it->second.lastSet = cur;
            } else {
                g_duck.erase(it);
                g_duckDirty = true;
                it = g_duck.end();
            }
        }

        if (ducking && t.wanted) {
            bool alreadyQuiet = it == g_duck.end() &&
                                (cur <= s.duckVolume + 0.001f ||
                                 g_duck.size() >= kMaxDuckEntries);
            if (!alreadyQuiet) {
                if (it == g_duck.end()) {
                    it = g_duck.insert({t.key, {cur, cur, s.duckVolume, true}}).first;
                    g_duckDirty = true;
                    Wh_Log(L"Ducking %s from %d%%", t.name.c_str(),
                           (int)(cur * 100 + 0.5f));
                } else if (fabsf(cur - it->second.lastSet) > kUserChangeTolerance) {
                    // User changed the volume while ducked: new baseline.
                    it->second.original = cur;
                    it->second.duckedLevel = std::min(cur, s.duckVolume);
                    g_duckDirty = true;
                }
                float desired = std::min(it->second.original, s.duckVolume);
                float next = MoveToward(cur, desired, step);
                if (next != cur) {
                    t.vol->SetMasterVolume(next, nullptr);
                }
                it->second.lastSet = next;
                outcome = next;
                lowering = true;
                fading = true;
            }
        } else if (it != g_duck.end()) {
            if (fabsf(cur - it->second.lastSet) > kUserChangeTolerance) {
                // User took over; don't touch it.
                g_duck.erase(it);
                g_duckDirty = true;
            } else {
                float next = restoreAll
                                 ? it->second.original
                                 : MoveToward(cur, it->second.original, step);
                if (next != cur) {
                    t.vol->SetMasterVolume(next, nullptr);
                }
                outcome = next;
                if (next == it->second.original) {
                    Wh_Log(L"Restored %s to %d%%", t.name.c_str(),
                           (int)(next * 100 + 0.5f));
                    g_duck.erase(it);
                    g_duckDirty = true;
                } else {
                    it->second.lastSet = next;
                    fading = true;
                }
            }
        }

        appliedThisPoll[t.key] = {outcome, lowering};
    }

    if (g_duckDirty) {
        SavePersisted();
    }

    // Poll quickly only while audio is involved. Records of apps that are not
    // running stay in storage and wake us again when their session appears.
    return (r.triggerOpen && !windowsHandlesCall) || ducking || fading ||
           r.unresolved;
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

    DeviceNotifier* deviceNotifier = new DeviceNotifier();
    g_createdNotifier = new SessionCreatedNotifier();
    g_eventsNotifier = new SessionEventsNotifier();
    bool deviceCallbackOk = SUCCEEDED(
        enumerator->RegisterEndpointNotificationCallback(deviceNotifier));
    if (!deviceCallbackOk) {
        Wh_Log(L"RegisterEndpointNotificationCallback failed");
    }

    CommWatcher commWatcher;
    LoadPersisted();

    HANDLE waitHandles[2] = {g_stopEvent, g_wakeEvent};
    for (;;) {
        if (InterlockedExchange(&g_devicesDirty, 0)) {
            UnwatchAll();
            ReleaseManagers(g_renderMgrs);
            ReleaseManagers(g_captureMgrs);
            LoadManagers(enumerator.p, eRender, g_renderMgrs);
            LoadManagers(enumerator.p, eCapture, g_captureMgrs);
            commWatcher.Update(enumerator.p);
        }

        bool busy = true;
        try {
            busy = Poll(false);
        } catch (...) {
            // e.g. out of memory: skip this cycle instead of crashing
        }

        DWORD result =
            WaitForMultipleObjects(2, waitHandles, FALSE, busy ? kPollMs : kIdleWatchdogMs);
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result == WAIT_TIMEOUT && !busy && !deviceCallbackOk) {
            InterlockedExchange(&g_devicesDirty, 1);  // no device events: rescan
        }
    }

    // Restore everything before exiting.
    try {
        Poll(true);
    } catch (...) {
    }

    UnwatchAll();
    commWatcher.Stop();
    ReleaseManagers(g_renderMgrs);
    ReleaseManagers(g_captureMgrs);
    if (deviceCallbackOk) {
        enumerator->UnregisterEndpointNotificationCallback(deviceNotifier);
    }
    deviceNotifier->Release();
    g_createdNotifier->Release();
    g_createdNotifier = nullptr;
    g_eventsNotifier->Release();
    g_eventsNotifier = nullptr;
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
    g_wakeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    Wake();
}

void WhTool_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        // The process exits right after; handles are only closed once the
        // worker is known to be done with them.
        if (WaitForSingleObject(g_thread, 5000) == WAIT_OBJECT_0) {
            CloseHandle(g_thread);
            g_thread = nullptr;
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
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

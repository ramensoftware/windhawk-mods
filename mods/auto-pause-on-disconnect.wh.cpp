// ==WindhawkMod==
// @id              auto-pause-on-disconnect
// @name            Auto-Pause on Disconnect
// @description     Pause, mute or unmute automatically when an external audio device connects or disconnects (wired headphones, Bluetooth, USB DAC).
// @version         1.2.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lole32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto-Pause on Disconnect

Pauses what you were listening to when the headphones go away: the 3.5 mm cable
pulled out, a Bluetooth headset dropped, a USB DAC unplugged. The action lands
after Windows has moved the sound back to the built-in speakers, so there is no
one-second blast through them.

![Auto-Pause in action](https://raw.githubusercontent.com/DeepakBalaraman/auto-pause-on-disconnect/master/assets/auto-pause-sample.gif)

It does not have to be a pause. The action is a setting, and so is whether it
runs on a disconnect, on a connect, or both.

## Features

- Watches the default output device, so wired, Bluetooth and USB removals are
  caught the same way.
- Toggle play/pause, mute the output, or unmute it.
- Waits 300 ms by default before acting, which is enough for Windows to finish
  moving the stream. Adjustable.
- Repeats inside one second are folded into a single action. One unplug raises
  several notifications, and play/pause is a toggle.
- Only acts while an app is actually playing by default, so it never starts
  playback on its own.
- Switching the output by hand is left alone; the old device has to be gone for
  the action to run.

## Settings

- **When a device disconnects** and **When a device connects** pick the
  direction. Connects are off by default, because plugging headphones in moves
  the default output too.
- **Audio role to watch** is Multimedia by default, or Console, or Both.
- **Action** is Toggle play/pause, Mute the default output, or Unmute it.
- **Delay before acting (ms)** is how long Windows gets before the action runs,
  0 to 3000.
- **Ignore repeats within (ms)** is the window used to fold the burst, 0 to 5000.
- **Act only while audio is playing** keeps the toggle from starting playback.

## Good to know

Play/pause is a toggle, not a pause: if playback was already stopped, it starts
again. Leaving *Act only while audio is playing* on keeps it honest, and the
Windows disconnect sound is not counted as playback.

The mute action applies to whichever device is the default after the switch,
which is normally the internal speakers. Pair *When a device connects* with
unmute if the headset should bring the sound back when it returns.

The mod runs in a small process of its own rather than inside Explorer, and uses
the Windows Core Audio API. The notification callback only wakes a worker thread;
the delay, the playback check and the media key all happen there.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Triggers:
  - OnDisconnect: true
    $name: When a device disconnects
    $description: "Act when headphones, a headset or a DAC is unplugged, or a Bluetooth device drops."
  - OnConnect: false
    $name: When a device connects
    $description: "Also act when a newly plugged device becomes the default output."
  - Role: multimedia
    $name: Audio role to watch
    $description: "Which default-device role counts as a change. Multimedia is what apps play through."
    $options:
      - multimedia: Multimedia
      - console: Console
      - both: Both
- Handling:
  - Action: playPause
    $name: Action
    $description: "What to run once a change is detected."
    $options:
      - playPause: Toggle play/pause
      - mute: Mute the default output
      - unmute: Unmute the default output
  - Delay: 300
    $name: Delay before acting (ms)
    $description: "How long Windows gets to move the default stream, 0 to 3000. Raise it if sound still leaks through the speakers for a moment."
  - Debounce: 1000
    $name: Ignore repeats within (ms)
    $description: "One unplug raises several notifications. Play/pause is a toggle, so a second trigger inside this window, 0 to 5000, would undo the first."
  - OnlyWhenPlaying: true
    $name: Act only while audio is playing
    $description: "Skip the action when no app is playing, so the mod never starts playback on its own."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <shellapi.h>

#include <windhawk_utils.h>

#include <atomic>
#include <cstdio>
#include <cwchar>
#include <new>

// Recently added / removed device ids are remembered for this long, which is
// what tells a disconnect (Windows falls back to an existing device) apart from
// a connect (a device that just showed up becomes the default).
static const DWORD kRecentDeviceWindowMs = 5000;
static const int kRecentDeviceSlots = 8;
static const int kMaxDeviceIdChars = 256;

// The default device is tracked per role, so a Console change is not compared
// against a Multimedia id.
static const int kRoleSlotMultimedia = 0;
static const int kRoleSlotConsole = 1;
static const int kRoleSlots = 2;

enum TriggerKind {
    kTriggerConnect,
    kTriggerDisconnect,
    // A change with no fresh device behind it: either a removal Windows has not
    // reported yet, or the output was switched by hand. Decided after the delay.
    kTriggerUnknown,
};

enum RoleFilter {
    kRoleMultimedia,
    kRoleConsole,
    kRoleBoth,
};

enum ActionKind {
    kActionPlayPause,
    kActionMute,
    kActionUnmute,
};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Written when the settings change, read from the notification callback and the
// audio thread, hence atomics. The values are filled in by LoadSettings() before
// the audio thread is started.
struct {
    std::atomic<bool> onDisconnect{};
    std::atomic<bool> onConnect{};
    std::atomic<int> roleFilter{};
    std::atomic<int> action{};
    std::atomic<int> delayMs{};
    std::atomic<int> debounceMs{};
    std::atomic<bool> onlyWhenPlaying{};
} g_settings;

static int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void LoadSettings() {
    g_settings.onDisconnect.store(Wh_GetIntSetting(L"Triggers.OnDisconnect") != 0);
    g_settings.onConnect.store(Wh_GetIntSetting(L"Triggers.OnConnect") != 0);

    {
        WindhawkUtils::StringSetting role =
            WindhawkUtils::StringSetting::make(L"Triggers.Role");
        int roleFilter = kRoleMultimedia;
        if (wcscmp(role.get(), L"console") == 0) {
            roleFilter = kRoleConsole;
        } else if (wcscmp(role.get(), L"both") == 0) {
            roleFilter = kRoleBoth;
        }
        g_settings.roleFilter.store(roleFilter);
    }

    {
        WindhawkUtils::StringSetting action =
            WindhawkUtils::StringSetting::make(L"Handling.Action");
        int actionKind = kActionPlayPause;
        if (wcscmp(action.get(), L"mute") == 0) {
            actionKind = kActionMute;
        } else if (wcscmp(action.get(), L"unmute") == 0) {
            actionKind = kActionUnmute;
        }
        g_settings.action.store(actionKind);
    }

    g_settings.delayMs.store(ClampInt(Wh_GetIntSetting(L"Handling.Delay"), 0, 3000));
    g_settings.debounceMs.store(ClampInt(Wh_GetIntSetting(L"Handling.Debounce"), 0, 5000));
    g_settings.onlyWhenPlaying.store(
        Wh_GetIntSetting(L"Handling.OnlyWhenPlaying") != 0);
}

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------

class CAudioNotificationClient;

// Manual-reset, set by WhTool_ModUninit() to end the audio thread.
static HANDLE g_hStopEvent = nullptr;

// Auto-reset, set by the notification callback to wake the audio thread.
static HANDLE g_hTriggerEvent = nullptr;

static HANDLE g_hAudioThread = nullptr;

// Set while the audio thread has the callback registered.
static std::atomic<bool> g_bActive{false};

// Tick of the last queued change, used for the debounce window.
static std::atomic<ULONGLONG> g_ullLastTriggerTick{0};

// Device bookkeeping shared between the notification callbacks.
static SRWLOCK g_deviceLock = SRWLOCK_INIT;

struct RecentDevice {
    wchar_t id[kMaxDeviceIdChars];
    ULONGLONG tick;
};

static RecentDevice g_recentAdded[kRecentDeviceSlots];
static RecentDevice g_recentRemoved[kRecentDeviceSlots];

// Last default output per role slot, the baseline a change is compared against.
static wchar_t g_lastDefault[kRoleSlots][kMaxDeviceIdChars];

// The change the audio thread is about to act on. Only one is kept: a newer
// change replaces an older one that has not been handled yet.
struct PendingChange {
    bool valid;
    int kind;
    wchar_t previous[kMaxDeviceIdChars];
};

static PendingChange g_pending;

static void CopyDeviceId(wchar_t* dst, size_t cchDst, LPCWSTR src) {
    if (!src) {
        dst[0] = L'\0';
        return;
    }

    size_t i = 0;
    while (i + 1 < cchDst && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = L'\0';
}

// Callers must hold g_deviceLock.
static bool ContainsRecentLocked(RecentDevice* slots, LPCWSTR deviceId,
                                 ULONGLONG now) {
    if (!deviceId || !deviceId[0]) {
        return false;
    }

    for (int i = 0; i < kRecentDeviceSlots; i++) {
        if (slots[i].id[0] && now - slots[i].tick <= kRecentDeviceWindowMs &&
            wcscmp(slots[i].id, deviceId) == 0) {
            return true;
        }
    }
    return false;
}

static void RememberDevice(RecentDevice* slots, LPCWSTR deviceId) {
    if (!deviceId || !deviceId[0]) {
        return;
    }

    AcquireSRWLockExclusive(&g_deviceLock);
    int oldest = 0;
    for (int i = 1; i < kRecentDeviceSlots; i++) {
        if (slots[i].tick < slots[oldest].tick) {
            oldest = i;
        }
    }
    CopyDeviceId(slots[oldest].id, ARRAYSIZE(slots[oldest].id), deviceId);
    slots[oldest].tick = GetTickCount64();
    ReleaseSRWLockExclusive(&g_deviceLock);
}

// Compares the new default with the last one for that role and classifies the
// change. Returns false when there is nothing to act on, either because the
// default did not move or because this is the first change (the baseline).
static bool TrackDefaultChange(int roleSlot, LPCWSTR newDeviceId,
                               wchar_t* previousOut, size_t cchPrevious,
                               int* kind) {
    wchar_t previous[kMaxDeviceIdChars];
    wchar_t current[kMaxDeviceIdChars];
    bool lastDefaultWasRemoved = false;
    bool newDeviceWasJustAdded = false;

    AcquireSRWLockExclusive(&g_deviceLock);
    CopyDeviceId(previous, ARRAYSIZE(previous), g_lastDefault[roleSlot]);
    CopyDeviceId(current, ARRAYSIZE(current), newDeviceId);
    CopyDeviceId(g_lastDefault[roleSlot], ARRAYSIZE(g_lastDefault[roleSlot]),
                 current);

    const ULONGLONG now = GetTickCount64();
    lastDefaultWasRemoved = ContainsRecentLocked(g_recentRemoved, previous, now);
    newDeviceWasJustAdded = ContainsRecentLocked(g_recentAdded, current, now);
    ReleaseSRWLockExclusive(&g_deviceLock);

    if (!previous[0] || wcscmp(previous, current) == 0) {
        return false;
    }

    CopyDeviceId(previousOut, cchPrevious, previous);

    // A device that just appeared becomes the default right away, while an
    // unplug leaves Windows falling back to a device that was already there.
    if (newDeviceWasJustAdded && !lastDefaultWasRemoved) {
        *kind = kTriggerConnect;
    } else if (lastDefaultWasRemoved) {
        *kind = kTriggerDisconnect;
    } else {
        *kind = kTriggerUnknown;
    }
    return true;
}

static void StorePendingChange(int kind, LPCWSTR previous) {
    AcquireSRWLockExclusive(&g_deviceLock);
    g_pending.valid = true;
    g_pending.kind = kind;
    CopyDeviceId(g_pending.previous, ARRAYSIZE(g_pending.previous), previous);
    ReleaseSRWLockExclusive(&g_deviceLock);
}

static bool TakePendingChange(int* kind, wchar_t* previous, size_t cchPrevious) {
    bool valid = false;

    AcquireSRWLockExclusive(&g_deviceLock);
    if (g_pending.valid) {
        valid = true;
        *kind = g_pending.kind;
        CopyDeviceId(previous, cchPrevious, g_pending.previous);
        g_pending.valid = false;
    }
    ReleaseSRWLockExclusive(&g_deviceLock);

    return valid;
}

// ---------------------------------------------------------------------------
// IMMNotificationClient implementation
// ---------------------------------------------------------------------------

class CAudioNotificationClient final : public IMMNotificationClient {
public:
    CAudioNotificationClient() : m_lRef(1) {}

    // --- IUnknown ---------------------------------------------------------

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, __uuidof(IUnknown)) ||
            IsEqualIID(riid, __uuidof(IMMNotificationClient))) {
            *ppvObject = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_lRef));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        const LONG lRef = InterlockedDecrement(&m_lRef);
        if (lRef == 0) {
            delete this;
        }
        return static_cast<ULONG>(lRef);
    }

    // --- IMMNotificationClient -------------------------------------------

    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId,
                                                   DWORD dwNewState) override {
        // A device Windows has already seen comes back as an active endpoint
        // rather than a new one, which is how headphones in the jack and a
        // paired Bluetooth headset announce themselves.
        if (dwNewState == DEVICE_STATE_ACTIVE) {
            RememberDevice(g_recentAdded, pwstrDeviceId);
        } else {
            RememberDevice(g_recentRemoved, pwstrDeviceId);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
        RememberDevice(g_recentAdded, pwstrDeviceId);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) override {
        RememberDevice(g_recentRemoved, pwstrDeviceId);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role,
                                                     LPCWSTR pwstrDefaultDeviceId) override {
        if (!g_bActive.load()) {
            return S_OK;
        }

        // Only render endpoints matter, and this callback fires for every kind
        // of removal: Bluetooth drop, 3.5 mm unplug, USB/DAC detach.
        if (flow != eRender || role == eCommunications) {
            return S_OK;
        }

        const int roleFilter = g_settings.roleFilter.load();
        if (roleFilter == kRoleMultimedia && role != eMultimedia) {
            return S_OK;
        }
        if (roleFilter == kRoleConsole && role != eConsole) {
            return S_OK;
        }

        wchar_t previous[kMaxDeviceIdChars] = {};
        int kind = kTriggerDisconnect;
        const int roleSlot = role == eConsole ? kRoleSlotConsole : kRoleSlotMultimedia;
        if (!TrackDefaultChange(roleSlot, pwstrDefaultDeviceId, previous,
                                ARRAYSIZE(previous), &kind)) {
            return S_OK;
        }

        const ULONGLONG now = GetTickCount64();
        const ULONGLONG last = g_ullLastTriggerTick.load();
        if (now - last < static_cast<ULONGLONG>(g_settings.debounceMs.load())) {
            Wh_Log(L"Default device changed, already handled -> skip");
            return S_OK;
        }
        g_ullLastTriggerTick.store(now);

        // Never block this callback: it runs on the Core Audio RPC thread pool.
        StorePendingChange(kind, previous);
        if (g_hTriggerEvent) {
            SetEvent(g_hTriggerEvent);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId,
                                                     const PROPERTYKEY key) override {
        return S_OK;
    }

private:
    LONG m_lRef;
};

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

static void SendPauseMediaKey() {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;
    inputs[0].ki.wScan = static_cast<WORD>(
        MapVirtualKeyW(VK_MEDIA_PLAY_PAUSE, MAPVK_VK_TO_VSC));
    inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs)) {
        Wh_Log(L"SendInput failed, error %u", GetLastError());
    } else {
        Wh_Log(L"Media play/pause key sent");
    }
}

static IMMDevice* GetDefaultRenderDevice(IMMDeviceEnumerator* pEnumerator) {
    IMMDevice* pDevice = nullptr;
    if (!pEnumerator) {
        return nullptr;
    }
    if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                    &pDevice))) {
        return nullptr;
    }
    return pDevice;
}

static bool IsDeviceActive(IMMDeviceEnumerator* pEnumerator, LPCWSTR deviceId) {
    if (!pEnumerator || !deviceId || !deviceId[0]) {
        return false;
    }

    IMMDevice* pDevice = nullptr;
    if (FAILED(pEnumerator->GetDevice(deviceId, &pDevice)) || !pDevice) {
        return false;
    }

    DWORD state = 0;
    const bool active =
        SUCCEEDED(pDevice->GetState(&state)) && state == DEVICE_STATE_ACTIVE;
    pDevice->Release();
    return active;
}

static void SetDefaultOutputMuted(IMMDeviceEnumerator* pEnumerator, bool mute) {
    IMMDevice* pDevice = GetDefaultRenderDevice(pEnumerator);
    if (!pDevice) {
        Wh_Log(L"No default render device to %s", mute ? L"mute" : L"unmute");
        return;
    }

    IAudioEndpointVolume* pVolume = nullptr;
    HRESULT hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                   nullptr,
                                   reinterpret_cast<void**>(&pVolume));
    pDevice->Release();
    if (FAILED(hr) || !pVolume) {
        Wh_Log(L"Activate(IAudioEndpointVolume) failed, error 0x%08X", hr);
        return;
    }

    hr = pVolume->SetMute(mute ? TRUE : FALSE, nullptr);
    pVolume->Release();
    if (FAILED(hr)) {
        Wh_Log(L"SetMute(%s) failed, error 0x%08X", mute ? L"true" : L"false",
               hr);
    } else {
        Wh_Log(L"Default output %s", mute ? L"muted" : L"unmuted");
    }
}

static bool IsSystemSoundsSession(IAudioSessionControl* pControl) {
    IAudioSessionControl2* pControl2 = nullptr;
    if (FAILED(pControl->QueryInterface(
            __uuidof(IAudioSessionControl2),
            reinterpret_cast<void**>(&pControl2))) ||
        !pControl2) {
        return false;
    }

    // S_OK means the session belongs to the system sounds, which play when a
    // device is removed and are not media playback.
    const bool isSystem = pControl2->IsSystemSoundsSession() == S_OK;
    pControl2->Release();
    return isSystem;
}

// 1 = a session is playing, 0 = nothing is playing, -1 = could not be checked.
static int GetPlaybackState(IMMDeviceEnumerator* pEnumerator) {
    IMMDevice* pDevice = GetDefaultRenderDevice(pEnumerator);
    if (!pDevice) {
        return -1;
    }

    IAudioSessionManager2* pManager = nullptr;
    HRESULT hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                   nullptr,
                                   reinterpret_cast<void**>(&pManager));
    pDevice->Release();
    if (FAILED(hr) || !pManager) {
        return -1;
    }

    int state = -1;
    IAudioSessionEnumerator* pSessions = nullptr;
    if (SUCCEEDED(pManager->GetSessionEnumerator(&pSessions)) && pSessions) {
        int count = 0;
        if (SUCCEEDED(pSessions->GetCount(&count))) {
            state = 0;
            for (int i = 0; i < count && state != 1; i++) {
                IAudioSessionControl* pControl = nullptr;
                if (FAILED(pSessions->GetSession(i, &pControl)) || !pControl) {
                    continue;
                }

                AudioSessionState sessionState = AudioSessionStateInactive;
                if (SUCCEEDED(pControl->GetState(&sessionState)) &&
                    sessionState == AudioSessionStateActive &&
                    !IsSystemSoundsSession(pControl)) {
                    state = 1;
                }
                pControl->Release();
            }
        }
        pSessions->Release();
    }

    pManager->Release();
    return state;
}

static void ApplyAction(int action, IMMDeviceEnumerator* pEnumerator) {
    switch (action) {
        case kActionMute:
            SetDefaultOutputMuted(pEnumerator, true);
            break;
        case kActionUnmute:
            SetDefaultOutputMuted(pEnumerator, false);
            break;
        default:
            SendPauseMediaKey();
            break;
    }
}

static void RunPendingAction(IMMDeviceEnumerator* pEnumerator) {
    int kind = kTriggerDisconnect;
    wchar_t previous[kMaxDeviceIdChars] = {};
    if (!TakePendingChange(&kind, previous, ARRAYSIZE(previous))) {
        return;
    }

    if (kind == kTriggerUnknown) {
        // The old device is still there, so the output was switched by hand.
        if (IsDeviceActive(pEnumerator, previous)) {
            Wh_Log(L"Default device switched by hand -> skip");
            return;
        }
        kind = kTriggerDisconnect;
    }

    const bool wanted = kind == kTriggerConnect ? g_settings.onConnect.load()
                                                : g_settings.onDisconnect.load();
    if (!wanted) {
        Wh_Log(L"Device change ignored by the trigger settings");
        return;
    }

    const bool idle = g_settings.onlyWhenPlaying.load() &&
                      pEnumerator && GetPlaybackState(pEnumerator) == 0;
    if (idle) {
        Wh_Log(L"No active audio session, skipping the action");
        return;
    }

    ApplyAction(g_settings.action.load(), pEnumerator);
}

// ---------------------------------------------------------------------------
// Audio thread
// ---------------------------------------------------------------------------

static void SeedBaseline(IMMDeviceEnumerator* pEnumerator, ERole role,
                         int roleSlot) {
    IMMDevice* pDevice = nullptr;
    if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, role, &pDevice)) ||
        !pDevice) {
        return;
    }

    LPWSTR pDeviceId = nullptr;
    if (SUCCEEDED(pDevice->GetId(&pDeviceId)) && pDeviceId) {
        AcquireSRWLockExclusive(&g_deviceLock);
        CopyDeviceId(g_lastDefault[roleSlot], ARRAYSIZE(g_lastDefault[roleSlot]),
                     pDeviceId);
        ReleaseSRWLockExclusive(&g_deviceLock);
        CoTaskMemFree(pDeviceId);
    }

    pDevice->Release();
}

static DWORD WINAPI AudioThread(LPVOID lpParameter) {
    // This thread owns the COM apartment, the enumerator and the callback, so
    // everything is created and released on the same thread.
    const HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IMMDeviceEnumerator* pEnumerator = nullptr;
    CAudioNotificationClient* pClient = nullptr;
    bool registered = false;

    if (SUCCEEDED(hrCom)) {
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                      CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                      reinterpret_cast<void**>(&pEnumerator));
        if (FAILED(hr) || !pEnumerator) {
            Wh_Log(L"CoCreateInstance(IMMDeviceEnumerator) failed, error 0x%08X",
                   hr);
        } else {
            pClient = new (std::nothrow) CAudioNotificationClient();
            if (!pClient) {
                Wh_Log(L"Allocation of the notification client failed");
            } else {
                hr = pEnumerator->RegisterEndpointNotificationCallback(pClient);
                registered = SUCCEEDED(hr);
                if (!registered) {
                    Wh_Log(L"RegisterEndpointNotificationCallback failed, error 0x%08X",
                           hr);
                }
            }
        }
    } else {
        Wh_Log(L"CoInitializeEx failed, error 0x%08X", hrCom);
    }

    if (registered) {
        // Seed the baseline so that the very first unplug is recognised instead
        // of being swallowed as the initial state.
        const int roleFilter = g_settings.roleFilter.load();
        if (roleFilter != kRoleConsole) {
            SeedBaseline(pEnumerator, eMultimedia, kRoleSlotMultimedia);
        }
        if (roleFilter != kRoleMultimedia) {
            SeedBaseline(pEnumerator, eConsole, kRoleSlotConsole);
        }

        g_bActive.store(true);

        HANDLE handles[2] = {g_hStopEvent, g_hTriggerEvent};
        while (WaitForMultipleObjects(2, handles, FALSE, INFINITE) ==
               WAIT_OBJECT_0 + 1) {
            // The delay returns early when the mod is being unloaded.
            if (WaitForSingleObject(g_hStopEvent,
                                    static_cast<DWORD>(g_settings.delayMs.load())) ==
                WAIT_OBJECT_0) {
                break;
            }

            // Fold triggers that arrived during the delay into this one.
            ResetEvent(g_hTriggerEvent);
            RunPendingAction(pEnumerator);
        }

        g_bActive.store(false);
        pEnumerator->UnregisterEndpointNotificationCallback(pClient);
        Wh_Log(L"Unregistered default render device notification callback");
    } else {
        // Stay alive and idle, so the mod can still be disabled and re-enabled
        // cleanly instead of its process dying on its own.
        Wh_Log(L"Core Audio is unavailable, the mod will not act on changes");
        WaitForSingleObject(g_hStopEvent, INFINITE);
    }

    if (pClient) {
        pClient->Release();
    }
    if (pEnumerator) {
        pEnumerator->Release();
    }
    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    LoadSettings();

    g_hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hTriggerEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_hStopEvent || !g_hTriggerEvent) {
        Wh_Log(L"CreateEvent failed, error %u", GetLastError());
        if (g_hTriggerEvent) {
            CloseHandle(g_hTriggerEvent);
            g_hTriggerEvent = nullptr;
        }
        if (g_hStopEvent) {
            CloseHandle(g_hStopEvent);
            g_hStopEvent = nullptr;
        }
        return FALSE;
    }

    g_hAudioThread = CreateThread(nullptr, 0, AudioThread, nullptr, 0, nullptr);
    if (!g_hAudioThread) {
        Wh_Log(L"CreateThread failed, error %u", GetLastError());
        CloseHandle(g_hTriggerEvent);
        g_hTriggerEvent = nullptr;
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings reloaded");
}

void WhTool_ModUninit() {
    g_bActive.store(false);

    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    // Joined without a timeout: the thread only waits on the events above, so
    // it always returns promptly, and nothing from the mod is left running when
    // the process unloads it.
    if (g_hAudioThread) {
        WaitForSingleObject(g_hAudioThread, INFINITE);
        CloseHandle(g_hAudioThread);
        g_hAudioThread = nullptr;
    }

    if (g_hTriggerEvent) {
        CloseHandle(g_hTriggerEvent);
        g_hTriggerEvent = nullptr;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
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

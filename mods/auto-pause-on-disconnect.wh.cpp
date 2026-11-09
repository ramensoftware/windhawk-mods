// ==WindhawkMod==
// @id              auto-pause-on-disconnect
// @name            Auto-Pause on Disconnect
// @description     Paste, mute or unmute automatically when an external audio device connects or disconnects (wired headphones, Bluetooth, USB DAC).
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto-Pause on Disconnect

Pauses what you were listening to when the headphones go away: the 3.5 mm cable
pulled out, a Bluetooth headset dropped, a USB DAC unplugged. The action lands
after Windows has moved the sound back to the built-in speakers, so there is no
one-second blast through them.

![Auto-Pause in action](https://raw.githubusercontent.com/DeepakBalaraman/auto-pause-on-disconnect/main/assets/auto-pause-sample.gif)

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
- Optional: only act while an app is actually playing.

## Settings

- **When a device disconnects** and **When a device connects** pick the
  direction. Connects are off by default, because plugging headphones in moves
  the default output too.
- **Audio role to watch** is Multimedia by default, or Console, or Both.
- **Action** is Toggle play/pause, Mute the default output, or Unmute it.
- **Delay before acting (ms)** is how long Windows gets before the action runs.
- **Ignore repeats within (ms)** is the window used to fold the burst.
- **Act only while audio is playing** turns the toggle into a real pause.

## Good to know

Play/pause is a toggle, not a pause: if playback was already stopped, it starts
again. Turn on *Act only while audio is playing* to keep it honest.

The mute action applies to whichever device is the default after the switch,
which is normally the internal speakers. Pair *When a device connects* with
unmute if the headset should bring the sound back when it returns.

The mod runs inside Explorer and uses the Windows Core Audio API. The
notification callback only queues the work; the delay and the media key happen on
a background thread.
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
    $description: "How long Windows gets to move the default stream. Raise it if sound still leaks through the speakers for a moment."
#! $min: 0
#! $max: 3000
  - Debounce: 1000
    $name: Ignore repeats within (ms)
    $description: "One unplug raises several notifications. Play/pause is a toggle, so a second trigger inside this window would undo the first."
#! $min: 0
#! $max: 5000
  - OnlyWhenPlaying: true
    $name: Act only while audio is playing
    $description: "Skip the action when no app is playing. Worth enabling with play/pause, so the mod never starts playback on its own."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>

#include <atomic>
#include <cwchar>
#include <new>

// Upper bound for waiting on a pending action when the mod is disabled. The
// worker itself only sleeps for the configured delay.
static const DWORD kWorkerShutdownWaitMs = 3000;

// Recently added / removed device ids are remembered for this long, which is
// what tells a disconnect (Windows falls back to an existing device) apart
// from a connect (a brand new device becomes the default).
static const DWORD kRecentDeviceWindowMs = 5000;
static const int kRecentDeviceSlots = 8;
static const int kMaxDeviceIdChars = 256;

// The GUIDs are spelled out explicitly: the MinGW-w64 SDK headers only declare
// CLSID_MMDeviceEnumerator / IID_IMMDeviceEnumerator / IID_IMMNotificationClient
// without defining them, so using the SDK names would need an extra import lib.
static const CLSID kCLSID_MMDeviceEnumerator = {
    0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
static const IID kIID_IMMDeviceEnumerator = {
    0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
static const IID kIID_IMMNotificationClient = {
    0x7991EEC9, 0x7E89, 0x4D85, {0x83, 0x90, 0x6C, 0x70, 0x3C, 0xEC, 0x60, 0xC0}};
static const IID kIID_IUnknown = {
    0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const IID kIID_IAudioEndpointVolume = {
    0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};
static const IID kIID_IAudioSessionManager2 = {
    0x77AA99A0, 0x1BD6, 0x484F, {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};

enum TriggerKind {
    kTriggerConnect,
    kTriggerDisconnect,
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

// Written on the Windhawk settings thread, read from the audio notification
// and worker threads, hence atomics.
struct {
    std::atomic<bool> onDisconnect{true};
    std::atomic<bool> onConnect{false};
    std::atomic<int> roleFilter{kRoleMultimedia};
    std::atomic<int> action{kActionPlayPause};
    std::atomic<int> delayMs{300};
    std::atomic<int> debounceMs{1000};
    std::atomic<bool> onlyWhenPlaying{false};
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

    PCWSTR role = Wh_GetStringSetting(L"Triggers.Role");
    int roleFilter = kRoleMultimedia;
    if (role && wcscmp(role, L"console") == 0) {
        roleFilter = kRoleConsole;
    } else if (role && wcscmp(role, L"both") == 0) {
        roleFilter = kRoleBoth;
    }
    Wh_FreeStringSetting(role);
    g_settings.roleFilter.store(roleFilter);

    PCWSTR action = Wh_GetStringSetting(L"Handling.Action");
    int actionKind = kActionPlayPause;
    if (action && wcscmp(action, L"mute") == 0) {
        actionKind = kActionMute;
    } else if (action && wcscmp(action, L"unmute") == 0) {
        actionKind = kActionUnmute;
    }
    Wh_FreeStringSetting(action);
    g_settings.action.store(actionKind);

    g_settings.delayMs.store(ClampInt(Wh_GetIntSetting(L"Handling.Delay"), 0, 3000));
    g_settings.debounceMs.store(ClampInt(Wh_GetIntSetting(L"Handling.Debounce"), 0, 5000));
    g_settings.onlyWhenPlaying.store(Wh_GetIntSetting(L"Handling.OnlyWhenPlaying") != 0);
}

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------

class CAudioNotificationClient;

// Set to false first thing in Wh_ModUninit(), so that in-flight notification
// callbacks stop queueing work before any COM object is torn down.
static std::atomic<bool> g_bActive{false};

// True while an action is pending. Also used to coalesce bursts.
static std::atomic<bool> g_bWorkerBusy{false};

// Tick of the last queued action, used for the debounce window.
static std::atomic<ULONGLONG> g_ullLastTriggerTick{0};

// Auto-reset event signalled by the worker when it is finished; lets
// Wh_ModUninit() wait for a pending action instead of pulling the COM objects
// out from under it.
static HANDLE g_hWorkerDone = nullptr;

// Owned COM objects. The client holds one reference for us; the enumerator
// keeps it alive as well while the callback is registered.
static IMMDeviceEnumerator* g_pEnumerator = nullptr;
static CAudioNotificationClient* g_pClient = nullptr;

static bool g_bComInitialized = false;

static void QueueActionOnWorkerThread();

// Device bookkeeping shared between the notification callbacks.
static CRITICAL_SECTION g_deviceCs;
static bool g_bDeviceCsInitialized = false;

struct RecentDevice {
    wchar_t id[kMaxDeviceIdChars];
    ULONGLONG tick;
};

static RecentDevice g_recentAdded[kRecentDeviceSlots];
static RecentDevice g_recentRemoved[kRecentDeviceSlots];
static wchar_t g_lastDefaultDevice[kMaxDeviceIdChars];

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

// Callers must hold g_deviceCs.
static bool ContainsRecentLocked(RecentDevice* slots, LPCWSTR deviceId, ULONGLONG now) {
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
    if (!deviceId || !deviceId[0] || !g_bDeviceCsInitialized) {
        return;
    }

    EnterCriticalSection(&g_deviceCs);
    int oldest = 0;
    for (int i = 1; i < kRecentDeviceSlots; i++) {
        if (slots[i].tick < slots[oldest].tick) {
            oldest = i;
        }
    }
    CopyDeviceId(slots[oldest].id, ARRAYSIZE(slots[oldest].id), deviceId);
    slots[oldest].tick = GetTickCount64();
    LeaveCriticalSection(&g_deviceCs);
}

// Decides whether the new default device means a connect or a disconnect and
// remembers it. Returns false when the default did not actually change.
static bool TrackDefaultChange(LPCWSTR newDeviceId, TriggerKind* kind) {
    wchar_t previous[kMaxDeviceIdChars];
    wchar_t current[kMaxDeviceIdChars];

    EnterCriticalSection(&g_deviceCs);
    CopyDeviceId(previous, ARRAYSIZE(previous), g_lastDefaultDevice);
    CopyDeviceId(current, ARRAYSIZE(current), newDeviceId);
    CopyDeviceId(g_lastDefaultDevice, ARRAYSIZE(g_lastDefaultDevice), current);

    const ULONGLONG now = GetTickCount64();
    const bool lastDefaultWasRemoved = ContainsRecentLocked(g_recentRemoved, previous, now);
    const bool newDeviceWasJustAdded = ContainsRecentLocked(g_recentAdded, current, now);
    LeaveCriticalSection(&g_deviceCs);

    if (wcscmp(previous, current) == 0) {
        return false;
    }

    // The first change only establishes the baseline; there is nothing to
    // compare it against yet.
    if (!previous[0]) {
        return false;
    }

    // A plugged in device becomes the default right after it appears, while an
    // unplug leaves Windows falling back to a device that was already there.
    if (newDeviceWasJustAdded && !lastDefaultWasRemoved) {
        *kind = kTriggerConnect;
    } else {
        *kind = kTriggerDisconnect;
    }
    return true;
}

// ---------------------------------------------------------------------------
// IMMNotificationClient implementation
// ---------------------------------------------------------------------------

class CAudioNotificationClient final : public IMMNotificationClient {
public:
    CAudioNotificationClient() : m_lRef(1) {}

    // --- IUnknown ---------------------------------------------------------

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, kIID_IUnknown) || IsEqualIID(riid, kIID_IMMNotificationClient)) {
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
        if (dwNewState != DEVICE_STATE_ACTIVE) {
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

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow,
                                                     ERole role,
                                                     LPCWSTR pwstrDefaultDeviceId) override {
        if (!g_bActive.load()) {
            return S_OK;
        }

        // Only render endpoints matter, and this callback fires for every kind
        // of removal: Bluetooth drop, 3.5 mm unplug, USB/DAC detach.
        if (flow != eRender) {
            return S_OK;
        }

        const int roleFilter = g_settings.roleFilter.load();
        if (roleFilter == kRoleMultimedia && role != eMultimedia) {
            return S_OK;
        }
        if (roleFilter == kRoleConsole && role != eConsole) {
            return S_OK;
        }

        TriggerKind kind = kTriggerDisconnect;
        if (!TrackDefaultChange(pwstrDefaultDeviceId, &kind)) {
            return S_OK;
        }

        const bool wanted = kind == kTriggerConnect ? g_settings.onConnect.load()
                                                    : g_settings.onDisconnect.load();
        if (!wanted) {
            Wh_Log(L"Default device changed (%s), ignored by the trigger settings",
                   kind == kTriggerConnect ? L"connect" : L"disconnect");
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
        QueueActionOnWorkerThread();
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
    inputs[0].ki.wScan = static_cast<WORD>(MapVirtualKeyW(VK_MEDIA_PLAY_PAUSE, MAPVK_VK_TO_VSC));
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
    if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice))) {
        return nullptr;
    }
    return pDevice;
}

static void SetDefaultOutputMuted(IMMDeviceEnumerator* pEnumerator, bool mute) {
    IMMDevice* pDevice = GetDefaultRenderDevice(pEnumerator);
    if (!pDevice) {
        Wh_Log(L"No default render device to %s", mute ? L"mute" : L"unmute");
        return;
    }

    IAudioEndpointVolume* pVolume = nullptr;
    HRESULT hr = pDevice->Activate(kIID_IAudioEndpointVolume, CLSCTX_ALL, nullptr,
                                   reinterpret_cast<void**>(&pVolume));
    pDevice->Release();
    if (FAILED(hr) || !pVolume) {
        Wh_Log(L"Activate(IAudioEndpointVolume) failed, error 0x%08X", hr);
        return;
    }

    hr = pVolume->SetMute(mute ? TRUE : FALSE, nullptr);
    pVolume->Release();
    if (FAILED(hr)) {
        Wh_Log(L"SetMute(%s) failed, error 0x%08X", mute ? L"true" : L"false", hr);
    } else {
        Wh_Log(L"Default output %s", mute ? L"muted" : L"unmuted");
    }
}

// 1 = a session is playing, 0 = nothing is playing, -1 = could not be checked.
static int GetPlaybackState(IMMDeviceEnumerator* pEnumerator) {
    IMMDevice* pDevice = GetDefaultRenderDevice(pEnumerator);
    if (!pDevice) {
        return -1;
    }

    IAudioSessionManager2* pManager = nullptr;
    HRESULT hr = pDevice->Activate(kIID_IAudioSessionManager2, CLSCTX_ALL, nullptr,
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
                if (SUCCEEDED(pSessions->GetSession(i, &pControl)) && pControl) {
                    AudioSessionState sessionState = AudioSessionStateInactive;
                    if (SUCCEEDED(pControl->GetState(&sessionState)) &&
                        sessionState == AudioSessionStateActive) {
                        state = 1;
                    }
                    pControl->Release();
                }
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

// ---------------------------------------------------------------------------
// Background worker
// ---------------------------------------------------------------------------

struct ActionRequest {
    int action;
    int delayMs;
    bool onlyWhenPlaying;
};

static DWORD WINAPI ActionWorkerThread(LPVOID lpParameter) {
    ActionRequest* pRequest = static_cast<ActionRequest*>(lpParameter);

    // The mute and playback checks talk to Core Audio, so this thread needs its
    // own COM apartment; the notification callback threads belong to RPC.
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // A private enumerator keeps all Core Audio use inside this apartment
    // instead of sharing the objects registered on the init thread.
    IMMDeviceEnumerator* pEnumerator = nullptr;
    if (SUCCEEDED(hr)) {
        CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                         kIID_IMMDeviceEnumerator,
                         reinterpret_cast<void**>(&pEnumerator));
    }

    // Give Windows time to finish moving the default stream, then act.
    Sleep(pRequest->delayMs);

    if (g_bActive.load()) {
        // Act when the playback check is unavailable rather than do nothing.
        const bool bIdle = pRequest->onlyWhenPlaying && GetPlaybackState(pEnumerator) == 0;
        if (bIdle) {
            Wh_Log(L"No active audio session, skipping the action");
        } else {
            ApplyAction(pRequest->action, pEnumerator);
        }
    }

    if (pEnumerator) {
        pEnumerator->Release();
    }
    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }

    delete pRequest;

    g_bWorkerBusy.store(false);
    if (g_hWorkerDone) {
        SetEvent(g_hWorkerDone);
    }

    return 0;
}

static void QueueActionOnWorkerThread() {
    bool bExpected = false;
    if (!g_bWorkerBusy.compare_exchange_strong(bExpected, true)) {
        // An action is already pending for this burst.
        return;
    }

    ActionRequest* pRequest = new (std::nothrow) ActionRequest();
    if (!pRequest) {
        Wh_Log(L"Allocation of the action request failed");
        g_bWorkerBusy.store(false);
        return;
    }

    pRequest->action = g_settings.action.load();
    pRequest->delayMs = g_settings.delayMs.load();
    pRequest->onlyWhenPlaying = g_settings.onlyWhenPlaying.load();

    HANDLE hThread = CreateThread(nullptr, 0, ActionWorkerThread, pRequest, 0, nullptr);
    if (hThread) {
        // The handle is not needed: completion is tracked through the event.
        CloseHandle(hThread);
    } else {
        Wh_Log(L"CreateThread failed, error %u", GetLastError());
        delete pRequest;
        g_bWorkerBusy.store(false);
    }
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

static void ReleaseComObjects() {
    if (g_pClient) {
        g_pClient->Release();
        g_pClient = nullptr;
    }

    if (g_pEnumerator) {
        g_pEnumerator->Release();
        g_pEnumerator = nullptr;
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (hr == RPC_E_CHANGED_MODE) {
        // The thread already lives in an STA; COM is usable regardless and we
        // must not balance it with CoUninitialize().
        Wh_Log(L"COM already initialized in STA mode, reusing it");
    } else if (FAILED(hr)) {
        Wh_Log(L"CoInitializeEx failed, error 0x%08X", hr);
        return FALSE;
    } else {
        g_bComInitialized = true;
    }

    InitializeCriticalSection(&g_deviceCs);
    g_bDeviceCsInitialized = true;

    g_lastDefaultDevice[0] = L'\0';
    ZeroMemory(g_recentAdded, sizeof(g_recentAdded));
    ZeroMemory(g_recentRemoved, sizeof(g_recentRemoved));

    // Declared up front because the failure path is reached through goto.
    IMMDevice* pDefaultDevice = nullptr;
    LPWSTR pDefaultId = nullptr;

    g_hWorkerDone = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_hWorkerDone) {
        Wh_Log(L"CreateEventW failed, error %u", GetLastError());
        goto fail;
    }

    hr = CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                          kIID_IMMDeviceEnumerator,
                          reinterpret_cast<void**>(&g_pEnumerator));
    if (FAILED(hr) || !g_pEnumerator) {
        Wh_Log(L"CoCreateInstance(IMMDeviceEnumerator) failed, error 0x%08X", hr);
        goto fail;
    }

    g_pClient = new (std::nothrow) CAudioNotificationClient();
    if (!g_pClient) {
        Wh_Log(L"Allocation of the notification client failed");
        goto fail;
    }

    hr = g_pEnumerator->RegisterEndpointNotificationCallback(g_pClient);
    if (FAILED(hr)) {
        Wh_Log(L"RegisterEndpointNotificationCallback failed, error 0x%08X", hr);
        goto fail;
    }

    // Seed the baseline so that the very first unplug is recognised instead of
    // being swallowed as the initial state.
    if (SUCCEEDED(g_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice)) &&
        pDefaultDevice) {
        if (SUCCEEDED(pDefaultDevice->GetId(&pDefaultId)) && pDefaultId) {
            CopyDeviceId(g_lastDefaultDevice, ARRAYSIZE(g_lastDefaultDevice), pDefaultId);
            CoTaskMemFree(pDefaultId);
        }
        pDefaultDevice->Release();
    }

    g_ullLastTriggerTick.store(0);
    g_bWorkerBusy.store(false);
    g_bActive.store(true);

    Wh_Log(L"Registered default render device notification callback");
    return TRUE;

fail:
    // The callback was never registered, so the client can only be ours.
    ReleaseComObjects();
    if (g_hWorkerDone) {
        CloseHandle(g_hWorkerDone);
        g_hWorkerDone = nullptr;
    }
    if (g_bDeviceCsInitialized) {
        DeleteCriticalSection(&g_deviceCs);
        g_bDeviceCsInitialized = false;
    }
    if (g_bComInitialized) {
        CoUninitialize();
        g_bComInitialized = false;
    }
    return FALSE;
}

void Wh_ModUninit() {
    // Stop accepting new events before anything is released.
    g_bActive.store(false);

    if (g_pEnumerator && g_pClient) {
        g_pEnumerator->UnregisterEndpointNotificationCallback(g_pClient);
    }

    // Let a pending action finish so it cannot touch the event or the global
    // state after teardown.
    for (DWORD waited = 0; g_bWorkerBusy.load() && waited < kWorkerShutdownWaitMs;
         waited += 100) {
        if (g_hWorkerDone) {
            WaitForSingleObject(g_hWorkerDone, 100);
        } else {
            break;
        }
    }

    ReleaseComObjects();

    if (g_hWorkerDone) {
        CloseHandle(g_hWorkerDone);
        g_hWorkerDone = nullptr;
    }

    if (g_bDeviceCsInitialized) {
        DeleteCriticalSection(&g_deviceCs);
        g_bDeviceCsInitialized = false;
    }

    if (g_bComInitialized) {
        CoUninitialize();
        g_bComInitialized = false;
    }

    Wh_Log(L"Unregistered default render device notification callback");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings reloaded");
}

// ==WindhawkMod==
// @id              mute-in-background
// @name            Mute in Background
// @name:zh-CN      后台自动静音
// @description     Automatically mutes the program when it goes to the background and restores it when it returns to the foreground.
// @description:zh-CN 当程序切换到后台时自动静音，切回前台时恢复
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @license         MIT
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mute in Background

Mutes the target program automatically while it is in the background, and
unmutes it when it comes back to the foreground.

## Choosing the target program

The mod targets nothing by default, so it does nothing until you tell it which
process to apply to. Open the mod in Windhawk, go to the **Advanced** tab, and
put the executable name of your target in the **Custom process inclusion list**.
The mod takes effect as soon as you save.

## Notes

- Mute is used, not the volume level, so a program's volume in the Volume Mixer
  is left as it is.
- Only sessions this mod muted itself are unmuted, so a program you muted by
  hand in the Volume Mixer stays muted. The one exception is the first pass after
  a run that left the program muted: a mute that was already there can't be told
  apart from one the user set, so it is treated as the mod's and undone.
- Windows remembers a program's mute state between launches. If the program is
  closed while it is in the background, it starts muted the next time; the mod
  clears that as soon as it is loaded into the program again. If the mod is
  disabled while the program is closed, the mute has to be cleared by hand in
  the Volume Mixer.
- **Don't add multi-process programs** (Chrome, Edge, Discord, Spotify and other
  Chromium/Electron apps). Their audio comes from a child process that never
  owns the foreground window, so the mod would keep them muted the whole time,
  including while they are in the foreground.
*/
// ==/WindhawkModReadme==

#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

#include <set>
#include <string>

using Microsoft::WRL::ComPtr;

// Posted to the worker thread when a new audio session appears, so that all
// session handling stays on the thread that owns the COM apartment.
#define WM_SYNC_MUTE (WM_APP + 1)

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_readyEvent = nullptr;

// Session instance identifiers of the sessions this mod muted. A session the
// user muted by hand is never added here, so it is never unmuted by the mod.
// Owned by the worker thread.
std::set<std::wstring> g_mutedSessions;

// Whether the sessions currently in g_mutedSessions were left muted by a
// previous run of the program rather than muted by this one.
bool g_adoptMutedSessions = false;

// The name of the value recording that this program was left muted. The mod's
// storage is shared by every process it runs in, so the key is per executable;
// two instances of the same executable still share one key, which is rare
// enough to accept.
std::wstring g_leftMutedValueName;

// Whether g_mutedSessions was non-empty when it was last written out, so the
// value is only written when it changes rather than on every sync.
bool g_leftMutedWritten = false;

void InitLeftMutedValueName() {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) {
        g_leftMutedValueName = L"leftMuted";
        return;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;

    g_leftMutedValueName = L"leftMuted_";
    for (; *name; name++) {
        g_leftMutedValueName.push_back(towlower(*name));
    }
}

// Opens the default render endpoint's session manager. Returns null on failure.
ComPtr<IAudioSessionManager2> GetSessionManager() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_ALL, IID_PPV_ARGS(&enumerator)))) {
        return nullptr;
    }

    ComPtr<IMMDevice> device;
    if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                   &device))) {
        return nullptr;
    }

    ComPtr<IAudioSessionManager2> manager;
    if (FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                nullptr, &manager))) {
        return nullptr;
    }

    return manager;
}

bool IsForegroundProcess() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    return pid == GetCurrentProcessId();
}

// Mutes or unmutes this process's audio sessions according to whether the
// process is in the foreground, and keeps g_mutedSessions in step.
void SyncMuteState() {
    ComPtr<IAudioSessionManager2> manager = GetSessionManager();
    if (!manager) {
        return;
    }

    ComPtr<IAudioSessionEnumerator> sessions;
    if (FAILED(manager->GetSessionEnumerator(&sessions))) {
        return;
    }

    int count = 0;
    if (FAILED(sessions->GetCount(&count))) {
        return;
    }

    DWORD selfPid = GetCurrentProcessId();
    bool shouldMute = !IsForegroundProcess();
    bool sawOwnSession = false;

    for (int i = 0; i < count; i++) {
        ComPtr<IAudioSessionControl> control;
        if (FAILED(sessions->GetSession(i, &control))) {
            continue;
        }

        ComPtr<IAudioSessionControl2> control2;
        if (FAILED(control.As(&control2))) {
            continue;
        }

        DWORD pid = 0;
        if (FAILED(control2->GetProcessId(&pid)) || pid != selfPid) {
            continue;
        }

        ComPtr<ISimpleAudioVolume> volume;
        if (FAILED(control.As(&volume))) {
            continue;
        }

        LPWSTR rawIdentifier = nullptr;
        if (FAILED(control2->GetSessionInstanceIdentifier(&rawIdentifier)) ||
            !rawIdentifier) {
            continue;
        }
        std::wstring identifier = rawIdentifier;
        CoTaskMemFree(rawIdentifier);

        BOOL muted = FALSE;
        volume->GetMute(&muted);
        sawOwnSession = true;

        // On the first pass of a run, a session that is already muted was
        // either left behind by a previous run or muted by the user; the two
        // cannot be told apart, so it is adopted and will be unmuted again.
        if (g_adoptMutedSessions && muted) {
            g_mutedSessions.insert(identifier);
        }

        bool ours = g_mutedSessions.count(identifier) > 0;

        if (shouldMute) {
            if (!muted && SUCCEEDED(volume->SetMute(TRUE, nullptr))) {
                g_mutedSessions.insert(identifier);
            }
        } else if (ours) {
            if (SUCCEEDED(volume->SetMute(FALSE, nullptr))) {
                g_mutedSessions.erase(identifier);
            }
        }
    }

    // Adoption stays pending until the program has an audio session to adopt.
    // At startup the mod loads before the program opens its device, so the
    // first pass usually finds nothing; OnSessionCreated brings us back here
    // once the session exists.
    if (sawOwnSession) {
        g_adoptMutedSessions = false;
    }

    // Remembered for the next run: if the program exits while muted, the next
    // run has to clear that. Written only when it changes, so the value is not
    // rewritten on every foreground change. A pending adoption keeps the record
    // set, since the mute it refers to has not been dealt with yet.
    bool leftMuted = !g_mutedSessions.empty() || g_adoptMutedSessions;
    if (leftMuted != g_leftMutedWritten) {
        g_leftMutedWritten = leftMuted;
        Wh_SetIntValue(g_leftMutedValueName.c_str(), leftMuted ? 1 : 0);
    }
}

// Unmutes everything this mod muted, for the mod unload path.
void UnmuteAll() {
    if (g_mutedSessions.empty()) {
        return;
    }

    ComPtr<IAudioSessionManager2> manager = GetSessionManager();
    if (!manager) {
        return;
    }

    ComPtr<IAudioSessionEnumerator> sessions;
    if (FAILED(manager->GetSessionEnumerator(&sessions))) {
        return;
    }

    int count = 0;
    if (FAILED(sessions->GetCount(&count))) {
        return;
    }

    for (int i = 0; i < count; i++) {
        ComPtr<IAudioSessionControl> control;
        if (FAILED(sessions->GetSession(i, &control))) {
            continue;
        }

        ComPtr<IAudioSessionControl2> control2;
        if (FAILED(control.As(&control2))) {
            continue;
        }

        LPWSTR rawIdentifier = nullptr;
        if (FAILED(control2->GetSessionInstanceIdentifier(&rawIdentifier)) ||
            !rawIdentifier) {
            continue;
        }
        std::wstring identifier = rawIdentifier;
        CoTaskMemFree(rawIdentifier);

        if (!g_mutedSessions.count(identifier)) {
            continue;
        }

        ComPtr<ISimpleAudioVolume> volume;
        if (SUCCEEDED(control.As(&volume))) {
            volume->SetMute(FALSE, nullptr);
        }
    }

    g_mutedSessions.clear();
    g_leftMutedWritten = false;
    Wh_SetIntValue(g_leftMutedValueName.c_str(), 0);
}

class SessionNotification : public IAudioSessionNotification {
  public:
    virtual ~SessionNotification() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }

        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IAudioSessionNotification)) {
            *ppvObject = static_cast<IAudioSessionNotification*>(this);
            AddRef();
            return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    HRESULT STDMETHODCALLTYPE OnSessionCreated(
        IAudioSessionControl* /*newSession*/) override {
        // Marshalled to the worker thread, which owns the session state.
        if (g_workerThreadId) {
            PostThreadMessage(g_workerThreadId, WM_SYNC_MUTE, 0, 0);
        }
        return S_OK;
    }

  private:
    LONG m_refCount = 1;
};

// The session manager and the notification are kept alive together by the
// worker thread for as long as the mod runs. Letting either go while the other
// is still registered would leave the audio stack calling into the mod after it
// has been unloaded.
//
// They are locals of WorkerThread rather than globals: a global with a
// non-trivial destructor is released during process shutdown, under the loader
// lock, with every other thread already terminated, and releasing a session
// manager there can hang or crash the program on exit. A thread that is
// terminated at process exit never runs its locals' destructors.
void RegisterSessionNotification(ComPtr<IAudioSessionManager2>& manager,
                                 ComPtr<SessionNotification>& notification) {
    manager = GetSessionManager();
    if (!manager) {
        return;
    }

    // Attached, not copied: this owns the initial reference.
    notification.Attach(new SessionNotification);

    if (FAILED(manager->RegisterSessionNotification(notification.Get()))) {
        manager.Reset();
        notification.Reset();
        return;
    }

    // WASAPI does not deliver OnSessionCreated to a manager whose sessions were
    // never enumerated.
    ComPtr<IAudioSessionEnumerator> sessions;
    manager->GetSessionEnumerator(&sessions);
}

void UnregisterSessionNotification(ComPtr<IAudioSessionManager2>& manager,
                                   ComPtr<SessionNotification>& notification) {
    if (manager) {
        manager->UnregisterSessionNotification(notification.Get());
        manager.Reset();
    }
    notification.Reset();
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND, LONG idObject,
                           LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }
    SyncMuteState();
}

DWORD WINAPI WorkerThread(LPVOID) {
    // The audio session APIs, the session notification callback and the
    // out-of-context WinEvent hook all need a thread this mod owns: the hook
    // has to be removed from the thread that installed it, and the COM
    // apartment must not be forced on the program's own threads.
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    MSG msg;
    PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

    // The message queue exists from here on, so the unload path can post to it.
    SetEvent(g_readyEvent);

    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);

    if (!hook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    }

    // Released again before the thread exits, or by process termination, which
    // is why these are locals rather than globals.
    ComPtr<IAudioSessionManager2> notifyManager;
    ComPtr<SessionNotification> notification;
    RegisterSessionNotification(notifyManager, notification);

    // A previous run may have left the program muted.
    g_adoptMutedSessions = Wh_GetIntValue(g_leftMutedValueName.c_str(), 0) != 0;
    g_leftMutedWritten = g_adoptMutedSessions;

    SyncMuteState();

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_SYNC_MUTE) {
            SyncMuteState();
            continue;
        }
        DispatchMessage(&msg);
    }

    if (hook) {
        UnhookWinEvent(hook);
    }

    UnregisterSessionNotification(notifyManager, notification);

    UnmuteAll();

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    InitLeftMutedValueName();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Waited on, not closed here: the worker may still signal it, and the
    // handle could already have been reused by then.
    WaitForSingleObject(g_readyEvent, 5000);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_workerThread) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

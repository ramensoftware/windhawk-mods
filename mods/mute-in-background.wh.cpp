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
// @compilerOptions -lole32 -loleaut32 -luuid
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
The mod is loaded into the program as soon as you save; if nothing happens right
away, restart the program.

## Notes

- Mute is used, not the volume level, so a program's volume in the Volume Mixer
  is left as it is.
- Only sessions this mod muted itself are unmuted, so a program you muted by
  hand in the Volume Mixer stays muted.
- Windows remembers a program's mute state between launches. If the program is
  closed while it is in the background, it starts muted the next time; the mod
  clears that as soon as it is loaded into the program again. If the mod is
  disabled while the program is closed, the mute has to be cleared by hand in
  the Volume Mixer.
- The mod works per process, so it only handles audio that comes from the
  process it is loaded into. Multi-process programs (Chromium/Electron apps such
  as Chrome, Edge, Discord and Spotify) play audio from a child process that
  never owns the foreground window, so their audio is not handled.
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
// Owned by the worker thread, except for the flag below.
std::set<std::wstring> g_mutedSessions;

// Whether the sessions currently in g_mutedSessions were left muted by a
// previous run of the program rather than muted by this one.
bool g_adoptMutedSessions = false;

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

    g_adoptMutedSessions = false;

    // Remembered for the next run: if the program exits while muted, the next
    // run has to clear that.
    Wh_SetIntValue(L"leftMuted", g_mutedSessions.empty() ? 0 : 1);
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
    Wh_SetIntValue(L"leftMuted", 0);
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

void RegisterSessionNotification() {
    ComPtr<IAudioSessionManager2> manager = GetSessionManager();
    if (!manager) {
        return;
    }

    manager->RegisterSessionNotification(new SessionNotification);
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

    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);

    if (!hook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    }

    RegisterSessionNotification();

    // A previous run may have left the program muted.
    g_adoptMutedSessions = Wh_GetIntValue(L"leftMuted", 0) != 0;

    SetEvent(g_readyEvent);

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

    UnmuteAll();

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

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

    WaitForSingleObject(g_readyEvent, 5000);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

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
}

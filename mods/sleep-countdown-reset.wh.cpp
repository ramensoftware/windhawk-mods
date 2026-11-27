// ==WindhawkMod==
// @id              sleep-countdown-reset
// @name            Sleep Countdown Reset
// @description     Delays idle timer countdown when external devices access the computer, avoiding shutdown between requests (media, data transfer, etc.)
// @version         1.0.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         Plex Media Server.exe
// @include         EmbyServer.exe
// @include         jellyfin.exe
// @compilerOptions -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Sleep Countdown Reset

Media servers such as Plex keep the PC awake while streaming, but idle time still accumulates. When playback ends and the request is released, Windows checks the expired idle time and sleeps immediately, leaving no time to start the next episode/media/exchange.

This mod hooks the keep-awake APIs inside each server process. Right before the server releases the request, the mod takes over with its own, for a configurable grace period or, by default, the active power plan's sleep timeout. After this period, if playback does not return, the PC sleeps normally.

Display timer and monitor state stay under Windows' control. While the grace period runs, `powercfg /requests` lists the mod's request under SYSTEM.

## Supported servers
Included by default:
- **Plex Media Server**
- **Emby Server** (`EmbyServer.exe`)
- **Jellyfin** (`jellyfin.exe`): only with the third-party [Prevent Sleep plugin](https://github.com/jonschz/jellyfin-plugin-preventsleep), since Jellyfin alone doesn't keep the PC awake. On PCs with Modern Standby the plugin switches power plans instead, which this mod can't see.

Can be added (see below):
- **Universal Media Server**: `javaw.exe`, with its "Prevent sleep" option set to *Playback*. That process name is shared by every Java program, so they all get the grace period.
- **qBittorrent** (`qbittorrent.exe`), with "Inhibit system sleep" enabled: grace period after downloads finish.

Not needed: remote desktop and game streaming hosts (Sunshine, Parsec, Steam Remote Play). They forward the remote keyboard and mouse as local input, so Windows already counts the user as present.

Not covered: Windows file sharing (SMB). It runs inside the kernel, with no process to hook.

## Adding a program
1. While the program is serving, run `powercfg /requests` in a terminal as administrator. The program must be listed under SYSTEM or EXECUTION.
2. In Windhawk, open this mod's **Advanced** tab, add the executable name (e.g. `qbittorrent.exe`) to the **custom process inclusion list**, and save.
3. With debug logging enabled, the log should show `hooked SetThreadExecutionState` for the new process. If the program runs as a Windows service and nothing shows up, check that Windhawk is allowed to inject into services.

## Limitation
If the server process exits while holding the request, Windows drops it without any API call, and the mod can't see the release.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- holdAfterRelease: true
  $name: Reset idle timer after server/program release
  $description: Off = only log releases (diagnostics)
- graceMinutes: 0
  $name: Grace period (minutes)
  $description: 0 = the sleep timeout of the active power plan
*/
// ==/WindhawkModSettings==

#include <powrprof.h>
#include <map>
#include <mutex>

static bool g_holdAfterRelease;
static int g_graceMinutes;

using STES_t = EXECUTION_STATE(WINAPI*)(EXECUTION_STATE);
using PowerReq_t = BOOL(WINAPI*)(HANDLE, POWER_REQUEST_TYPE);

static STES_t g_stesOrig;
static PowerReq_t g_powerClearOrig;

static std::mutex g_lock;
static std::map<DWORD, bool> g_threadHeld;

// ===== Grace period =====

static HANDLE g_graceRequest;
static HANDLE g_graceWake;
static HANDLE g_graceThread;
static bool g_graceHolding;
static bool g_graceStop;
static ULONGLONG g_graceDeadline;

static PowerReq_t PowerClear() { return g_powerClearOrig ? g_powerClearOrig : PowerClearRequest; }

static DWORD SleepTimeoutSeconds() {
    static const GUID kSleepSubgroup = {0x238c9fa8, 0x0aad, 0x41ed, {0x83, 0xf4, 0x97, 0xbe, 0x24, 0x2c, 0x8f, 0x20}};
    static const GUID kStandbyIdle = {0x29f6c1db, 0x86da, 0x48c5, {0x9f, 0xdb, 0xf2, 0xb6, 0x7b, 0x1f, 0x44, 0xda}};

    GUID* scheme = nullptr;
    if (PowerGetActiveScheme(nullptr, &scheme) != ERROR_SUCCESS)
        return 0;
    SYSTEM_POWER_STATUS ps{};
    bool onAC = !GetSystemPowerStatus(&ps) || ps.ACLineStatus != 0;
    DWORD seconds = 0;
    (onAC ? PowerReadACValueIndex : PowerReadDCValueIndex)(nullptr, scheme, &kSleepSubgroup, &kStandbyIdle, &seconds);
    LocalFree(scheme);
    return seconds;
}

static void StartGrace(const wchar_t* source) {
    if (!g_holdAfterRelease) {
        Wh_Log(L"release via %s (grace disabled)", source);
        return;
    }
    DWORD seconds = g_graceMinutes > 0 ? (DWORD)g_graceMinutes * 60 : SleepTimeoutSeconds();
    if (!seconds) {
        Wh_Log(L"release via %s: sleep is set to never, no grace needed", source);
        return;
    }

    // A one-shot SetThreadExecutionState(ES_SYSTEM_REQUIRED) does not postpone sleep on Win11 24H2 (tested): hold a request instead.
    {
        std::lock_guard<std::mutex> lk(g_lock);
        if (!g_graceHolding) {
            // Power requests are counted: each successful Set needs exactly one Clear.
            g_graceHolding = PowerSetRequest(g_graceRequest, PowerRequestSystemRequired);
        }
        g_graceDeadline = GetTickCount64() + (ULONGLONG)seconds * 1000;
    }
    SetEvent(g_graceWake);
    Wh_Log(L"release via %s: holding awake for %u s", source, seconds);
}

static DWORD WINAPI GraceThread(void*) {
    for (;;) {
        DWORD wait = INFINITE;
        {
            std::lock_guard<std::mutex> lk(g_lock);
            if (g_graceStop)
                break;
            if (g_graceHolding) {
                ULONGLONG now = GetTickCount64();
                if (now >= g_graceDeadline) {
                    PowerClear()(g_graceRequest, PowerRequestSystemRequired);
                    g_graceHolding = false;
                    Wh_Log(L"grace period ended, sleep allowed");
                } else {
                    wait = (DWORD)(g_graceDeadline - now);
                }
            }
        }
        WaitForSingleObject(g_graceWake, wait);
    }

    std::lock_guard<std::mutex> lk(g_lock);
    if (g_graceHolding) {
        PowerClear()(g_graceRequest, PowerRequestSystemRequired);
        g_graceHolding = false;
    }
    return 0;
}

// ===== Hooks =====

static EXECUTION_STATE WINAPI SetThreadExecutionState_Hook(EXECUTION_STATE flags) {
    if (!(flags & ES_CONTINUOUS))
        return g_stesOrig(flags);

    DWORD tid = GetCurrentThreadId();
    bool holdNow = flags & ES_SYSTEM_REQUIRED;
    bool heldBefore;
    {
        std::lock_guard<std::mutex> lk(g_lock);
        auto it = g_threadHeld.find(tid);
        heldBefore = it != g_threadHeld.end() && it->second;
    }

    // Take over before passing the release through: removing the request triggers an immediate sleep evaluation.
    if (heldBefore && !holdNow)
        StartGrace(L"SetThreadExecutionState");

    EXECUTION_STATE prev = g_stesOrig(flags);

    // Hold started before the mod loaded: only the return value reveals it.
    const EXECUTION_STATE held = ES_CONTINUOUS | ES_SYSTEM_REQUIRED;
    if (!heldBefore && !holdNow && (prev & held) == held)
        StartGrace(L"SetThreadExecutionState (untracked hold)");

    {
        std::lock_guard<std::mutex> lk(g_lock);
        if (holdNow)
            g_threadHeld[tid] = true;
        else
            g_threadHeld.erase(tid);
    }
    return prev;
}

static bool IsSystemRequest(POWER_REQUEST_TYPE type) {
    return type == PowerRequestSystemRequired || type == PowerRequestExecutionRequired;
}

static BOOL WINAPI PowerClearRequest_Hook(HANDLE h, POWER_REQUEST_TYPE type) {
    if (IsSystemRequest(type))
        StartGrace(L"PowerClearRequest");
    return g_powerClearOrig(h, type);
}

static void HookExport(const char* name, const wchar_t* displayName, void* hook, void** orig) {
    for (const wchar_t* dll : {L"kernelbase.dll", L"kernel32.dll"}) {
        HMODULE mod = GetModuleHandleW(dll);
        void* target = mod ? (void*)GetProcAddress(mod, name) : nullptr;
        if (target) {
            Wh_SetFunctionHook(target, hook, orig);
            Wh_Log(L"hooked %s in %s", displayName, dll);
            return;
        }
    }
    Wh_Log(L"%s not found", displayName);
}

// ===== Mod lifecycle =====

static void LoadSettings() {
    g_holdAfterRelease = Wh_GetIntSetting(L"holdAfterRelease");
    g_graceMinutes = Wh_GetIntSetting(L"graceMinutes");
}

BOOL Wh_ModInit() {
    LoadSettings();

    REASON_CONTEXT reason{POWER_REQUEST_CONTEXT_VERSION, POWER_REQUEST_CONTEXT_SIMPLE_STRING};
    reason.Reason.SimpleReasonString = (LPWSTR)L"Sleep Countdown Reset: grace period after streaming";
    g_graceRequest = PowerCreateRequest(&reason);
    g_graceWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_graceRequest == INVALID_HANDLE_VALUE || !g_graceWake) {
        Wh_Log(L"failed to create power request or event");
        return FALSE;
    }
    g_graceThread = CreateThread(nullptr, 0, GraceThread, nullptr, 0, nullptr);

    HookExport("SetThreadExecutionState", L"SetThreadExecutionState", (void*)SetThreadExecutionState_Hook, (void**)&g_stesOrig);
    HookExport("PowerClearRequest", L"PowerClearRequest", (void*)PowerClearRequest_Hook, (void**)&g_powerClearOrig);
    return TRUE;
}

void Wh_ModUninit() {
    {
        std::lock_guard<std::mutex> lk(g_lock);
        g_graceStop = true;
    }
    SetEvent(g_graceWake);
    if (g_graceThread) {
        WaitForSingleObject(g_graceThread, INFINITE);
        CloseHandle(g_graceThread);
    }
    CloseHandle(g_graceWake);
    CloseHandle(g_graceRequest);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

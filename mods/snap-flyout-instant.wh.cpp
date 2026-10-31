// ==WindhawkMod==
// @id              snap-flyout-instant
// @name            Snap Flyout Instant
// @description     Controls the delay before the snap layouts flyout appears and hides
// @version         1.1
// @author          pajs
// @github          https://github.com/pajs
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Flyout Instant

Windows 11 shows the snap layouts flyout only after you hover the maximize button for a moment (the Windows default is 600 ms). This mod lets you shorten or remove that delay, and optionally change how long the flyout stays visible after the pointer leaves it.

## Show delay

The mod finds the timer that Windows starts when the pointer enters the maximize button and replaces its duration with the value from the settings. Other timers in the shell are left alone.

## Hide delay

When "Hide delay" is above 0, the mod also replaces the timer that Windows starts when the pointer leaves the flyout (the Windows default is 200 ms). The hide timer is identified by its exact duration (200 ms), by being created through the WinRT thread pool, and by being armed on the flyout's thread within 10 seconds after the flyout is shown. Timers armed after that window use the system hide delay. With the default value of 0 this part is disabled and the behavior is the same as in version 1.0.

## Notes

The mod relies on a symbol in twinui.pcshell.dll that a Windows update may rename, which would stop the show delay from being applied.

Tested on Windows 11 build 26300.9457 (twinui.pcshell.dll 10.0.26100.9444).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delayMs: 1
  $name: Show delay (ms)
  $description: Delay before the snap layouts flyout appears. Minimum 1. The Windows default is 600.
- hideDelayMs: 0
  $name: Hide delay (ms)
  $description: Delay before the flyout hides after the pointer leaves it. 0 keeps the system default. The Windows default is 200.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <atomic>

static std::atomic<int> g_delayMs{1};
static std::atomic<int> g_hideDelayMs{0};
static std::atomic<DWORD> g_invokeTid{0};
static std::atomic<ULONGLONG> g_invokeTick{0};
static std::atomic<DWORD> g_flyoutTid{0};
static std::atomic<ULONGLONG> g_flyoutTick{0};
static std::atomic<bool> g_flyoutHookAttempted{false};

static constexpr ULONGLONG kHideWindowMs = 10000;
static constexpr long long kSystemHideMs = 200;

using Void_t = void(__cdecl*)();
static Void_t StartInvoke_Original;

void __cdecl StartInvoke_Hook() {
    g_invokeTick = GetTickCount64();
    g_invokeTid = GetCurrentThreadId();
    StartInvoke_Original();
}

static bool CallerIsWinrtTimer(void* returnAddress) {
    HMODULE winrtTimer = GetModuleHandleW(L"threadpoolwinrt.dll");
    if (!winrtTimer) return false;
    HMODULE caller = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)returnAddress, &caller)) {
        return false;
    }
    return caller == winrtTimer;
}

using TpSet_t = VOID(NTAPI*)(PVOID, PLARGE_INTEGER, ULONG, ULONG);
static TpSet_t TpSet_Original;

VOID NTAPI TpSet_Hook(PVOID timer, PLARGE_INTEGER due, ULONG period, ULONG window) {
    DWORD tid = GetCurrentThreadId();
    ULONGLONG now = GetTickCount64();

    if (g_invokeTid == tid) {
        if (now - g_invokeTick >= 50) {
            g_invokeTid = 0;
        } else if (due && due->QuadPart < 0) {
            long long ms = -due->QuadPart / 10000;
            if (ms >= 300 && ms <= 2000 && CallerIsWinrtTimer(__builtin_return_address(0))) {
                g_invokeTid = 0;
                g_flyoutTid = tid;
                g_flyoutTick = now;
                LARGE_INTEGER fast;
                fast.QuadPart = -(long long)g_delayMs * 10000;
                TpSet_Original(timer, &fast, period, window);
                return;
            }
        }
    }

    int hideMs = g_hideDelayMs;
    if (hideMs > 0 && g_flyoutTid == tid && due && due->QuadPart < 0) {
        if (now - g_flyoutTick >= kHideWindowMs) {
            g_flyoutTid = 0;
        } else {
            long long ms = -due->QuadPart / 10000;
            if (ms == kSystemHideMs &&
                CallerIsWinrtTimer(__builtin_return_address(0))) {
                LARGE_INTEGER custom;
                custom.QuadPart = -(long long)hideMs * 10000;
                TpSet_Original(timer, &custom, period, window);
                return;
            }
        }
    }

    TpSet_Original(timer, due, period, window);
}

static bool TryHookFlyout(HMODULE mod) {
    if (!mod || g_flyoutHookAttempted.exchange(true)) return false;

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {L"public: static void __cdecl SnapFlyoutTelemetry::SnapFlyout_StartInvokeTimer(void)"},
            &StartInvoke_Original,
            StartInvoke_Hook,
        },
    };

    return WindhawkUtils::HookSymbols(mod, hooks, ARRAYSIZE(hooks));
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE m = LoadLibraryExW_Original(name, file, flags);
    if (m && !g_flyoutHookAttempted &&
        !(flags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                   LOAD_LIBRARY_AS_IMAGE_RESOURCE)) &&
        m == GetModuleHandleW(L"twinui.pcshell.dll")) {
        if (TryHookFlyout(m)) Wh_ApplyHookOperations();
    }
    return m;
}

void LoadSettings() {
    int show = Wh_GetIntSetting(L"delayMs");
    if (show < 1) show = 1;
    g_delayMs = show;

    int hide = Wh_GetIntSetting(L"hideDelayMs");
    if (hide < 0) hide = 0;
    g_hideDelayMs = hide;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    void* tp = nt ? (void*)GetProcAddress(nt, "TpSetTimer") : nullptr;
    if (!tp) return FALSE;
    WindhawkUtils::SetFunctionHook((TpSet_t)tp, TpSet_Hook, &TpSet_Original);
    HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
    void* ll = kb ? (void*)GetProcAddress(kb, "LoadLibraryExW") : nullptr;
    if (ll) {
        WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)ll, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }
    TryHookFlyout(GetModuleHandleW(L"twinui.pcshell.dll"));
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

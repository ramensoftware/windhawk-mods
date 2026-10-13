// ==WindhawkMod==
// @id              snap-flyout-instant
// @name            Snap Flyout Instant
// @description     Removes the delay before the snap layouts flyout appears when hovering the maximize button
// @version         1.0
// @author          pajs
// @github          https://github.com/pajs
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Flyout Instant

Removes the delay before the snap layouts flyout appears when you hover the
maximize button of a window in Windows 11.

By default Windows waits 600 ms before showing the flyout. This mod shortens
that wait to a value you choose (1 ms by default).

## Settings

- **Delay in milliseconds**: time before the flyout is shown. Lower is faster.

## Notes

- The mod only shortens the timer that the shell creates right after the
  flyout's invoke timer starts, and only when it is created by the WinRT
  thread pool timer. Other timers in the shell are left alone.
- The mod looks up an internal symbol of `twinui.pcshell.dll`. A future Windows
  update may rename it, in which case the mod will simply have no effect.
- Tested on Windows 11 build 26300.9457.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delayMs: 1
  $name: Delay in milliseconds
  $description: Time before the flyout is shown. The Windows default is 600.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <atomic>

static std::atomic<int> g_delayMs{1};
static std::atomic<DWORD> g_invokeTid{0};
static std::atomic<ULONGLONG> g_invokeTick{0};
static std::atomic<bool> g_flyoutHookAttempted{false};

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
    if (g_invokeTid == GetCurrentThreadId()) {
        if (GetTickCount64() - g_invokeTick >= 50) {
            g_invokeTid = 0;
        } else if (due && due->QuadPart < 0) {
            long long ms = -due->QuadPart / 10000;
            if (ms >= 300 && ms <= 2000 && CallerIsWinrtTimer(__builtin_return_address(0))) {
                g_invokeTid = 0;
                LARGE_INTEGER fast;
                fast.QuadPart = -(long long)g_delayMs * 10000;
                TpSet_Original(timer, &fast, period, window);
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
    g_delayMs = Wh_GetIntSetting(L"delayMs");
    if (g_delayMs < 1) g_delayMs = 1;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    void* tp = nt ? (void*)GetProcAddress(nt, "TpSetTimer") : nullptr;
    if (!tp) return FALSE;
    Wh_SetFunctionHook(tp, (void*)TpSet_Hook, (void**)&TpSet_Original);
    HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
    void* ll = kb ? (void*)GetProcAddress(kb, "LoadLibraryExW") : nullptr;
    if (ll) Wh_SetFunctionHook(ll, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    TryHookFlyout(GetModuleHandleW(L"twinui.pcshell.dll"));
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// ==WindhawkMod==
// @id              snap-flyout-instant
// @name            Snap Flyout Instant
// @description     Removes the delay before the snap layouts flyout appears when hovering the maximize button
// @version         1.0
// @author          pajs
// @github          https://github.com/pajs
// @include         explorer.exe
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

- Only the flyout hover delay is changed. Other timers in the shell are not
  affected.
- The mod looks up internal symbols of `twinui.pcshell.dll`. A future Windows
  update may rename them, in which case the mod will simply have no effect.
- Tested on Windows 11 build 26100.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delayMs: 1
  $name: Delay in milliseconds
  $description: Time before the flyout is shown. The Windows default is 600.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>

static int g_delayMs = 1;
static volatile DWORD g_invokeTid = 0;
static volatile ULONGLONG g_invokeTick = 0;
static bool g_hooked = false;

using Void_t = void(__cdecl*)();
static Void_t StartInvoke_Original;

void __cdecl StartInvoke_Hook() {
    g_invokeTid = GetCurrentThreadId();
    g_invokeTick = GetTickCount64();
    StartInvoke_Original();
}

using TpSet_t = VOID(NTAPI*)(PVOID, PLARGE_INTEGER, ULONG, ULONG);
static TpSet_t TpSet_Original;

VOID NTAPI TpSet_Hook(PVOID timer, PLARGE_INTEGER due, ULONG period, ULONG window) {
    if (due && due->QuadPart < 0 && g_invokeTid == GetCurrentThreadId() &&
        GetTickCount64() - g_invokeTick < 100) {
        long long ms = -due->QuadPart / 10000;
        if (ms >= 100 && ms <= 2000) {
            LARGE_INTEGER fast;
            fast.QuadPart = -(long long)g_delayMs * 10000;
            g_invokeTid = 0;
            TpSet_Original(timer, &fast, period, window);
            return;
        }
    }
    TpSet_Original(timer, due, period, window);
}

static bool TryHookFlyout() {
    if (g_hooked) return false;
    HMODULE mod = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!mod) return false;
    WH_FIND_SYMBOL fs;
    HANDLE h = Wh_FindFirstSymbol(mod, nullptr, &fs);
    if (!h) return false;
    bool ok = false;
    do {
        std::wstring s = fs.symbol;
        if (s.rfind(L"public: static void __cdecl SnapFlyoutTelemetry::SnapFlyout_StartInvokeTimer(void)", 0) == 0) {
            Wh_SetFunctionHook(fs.address, (void*)StartInvoke_Hook, (void**)&StartInvoke_Original);
            ok = true;
            break;
        }
    } while (Wh_FindNextSymbol(h, &fs));
    Wh_FindCloseSymbol(h);
    g_hooked = ok;
    return ok;
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE m = LoadLibraryExW_Original(name, file, flags);
    if (m && !g_hooked && name && wcsstr(name, L"twinui.pcshell.dll")) {
        if (TryHookFlyout()) Wh_ApplyHookOperations();
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
    TryHookFlyout();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

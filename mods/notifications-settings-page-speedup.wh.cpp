// ==WindhawkMod==
// @id              notifications-settings-page-speedup
// @name            Faster Notifications Settings Page
// @description     Stops Settings from re-scanning every notification app on each visit, so revisits load in seconds instead of minutes
// @version         1.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         SystemSettings.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Faster Notifications Settings Page

**Settings, System, Notifications** rebuilds its full app list every time you
open the page, including when you edit one app and press Back. With many
notification senders this takes **minutes, on every visit**.

This mod keeps the first open as-is (it fills a cache) and makes every visit
after that load in a couple of seconds. It skips a per-app summary line (see the
trade-off below) and caches each app's settings object across visits so the page
stops repeating a slow per-app cross-process call. The cached objects read live
data, so **your edits still show correctly** and nothing goes stale.

Both options default on and can be turned off in the mod's settings, which
reverts the page to normal instantly. Press **Ctrl+Alt+R** to clear the cache
and force a full rescan.

## The summary trade-off

Most of the speedup comes from skipping a per-app summary. Normally each app in
the list shows a small subtitle summarizing part of its setup, such as whether
banners or sounds are on. With this skipped, every app still shows its **name,
icon, and main on/off toggle**, and you can still open any app to see and change
**all** of its options. You only lose the at-a-glance subtitle. If you would
rather keep it, turn off **Skip per-app summary lookup** below; revisits are
still cached, just a bit slower.

The first open after a fresh sign-in is still slow; only revisits are fast. The
mod hooks internal functions in `SettingsHandlers_Notifications.dll` by symbol,
so it may need an update after a major Windows release. Tested on Windows 11.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- skipSummary: true
  $name: Skip per-app summary lookup
  $description: The largest single speedup. Each app still shows its name, icon, and main on/off toggle, but loses the small subtitle summarizing banners/sounds status. You can still open any app to see and change those. Turn off to keep the subtitle (revisits stay cached, just a bit slower).
- cacheSettings: true
  $name: Cache app settings across visits
  $description: Makes revisiting the page fast. Cached objects read live data, so your edits are still reflected. Turn off if the list ever renders incorrectly.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shlwapi.h>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static std::atomic<bool> g_skipSummary{true};
static std::atomic<bool> g_cacheSettings{true};

// ---------------------------------------------------------------------------
// Per-AUMID cache of each app's IAumidNotificationSettings object.
//
// EnumerateAppSettingItems -> _AddEntry(aumid) per app -> controller
// ->vtable[0x70](aumid, &settings). The controller is an out-of-process COM
// object (CoCreateInstanceAsUser / MainNotificationController), so that call is
// a cross-process activation (~150-300ms each). We vtable-hook that slot: the
// first visit populates the cache, revisits reuse the live objects and skip the
// activation. Reads still hit the broker, so edits are reflected (no staleness).
// ---------------------------------------------------------------------------
static const size_t CTRL_PTR_OFFSET       = 0x108; // helper -> controller ComPtr
static const size_t GETSETTINGS_VTBL_SLOT = 14;    // 0x70 / sizeof(void*)

static std::unordered_map<std::wstring, IUnknown*> g_cache; // aumid -> settings obj
static std::shared_mutex g_cacheMutex;
static void** g_vtblSlot = nullptr;
static std::atomic<bool> g_vtblHooked{false};
static volatile bool g_running = true;

static void FlushCache() {
    std::unique_lock lock(g_cacheMutex);
    for (auto& kv : g_cache)
        if (kv.second) kv.second->Release();
    g_cache.clear();
    Wh_Log(L"cache cleared (manual rescan)");
}

// The controller vtable slot: HRESULT(controller, PCWSTR aumid, IUnknown** out)
using GetSettings_t = HRESULT (STDMETHODCALLTYPE*)(void*, PCWSTR, void**);
static GetSettings_t GetSettings_Orig = nullptr;

HRESULT STDMETHODCALLTYPE GetSettings_Hook(void* self, PCWSTR aumid, void** out) {
    if (g_cacheSettings.load() && aumid && out) {
        {
            std::shared_lock lk(g_cacheMutex);
            auto it = g_cache.find(aumid);
            if (it != g_cache.end()) {
                it->second->AddRef();
                *out = it->second;
                return S_OK;
            }
        }
        HRESULT hr = GetSettings_Orig(self, aumid, out);
        if (SUCCEEDED(hr) && *out) {
            IUnknown* p = reinterpret_cast<IUnknown*>(*out);
            p->AddRef();
            std::unique_lock lk(g_cacheMutex);
            g_cache.emplace(aumid, p);
        }
        return hr;
    }
    return GetSettings_Orig(self, aumid, out);
}

static void HookControllerVtable(void* helper) {
    if (g_vtblHooked.load()) return;
    void* controller = *reinterpret_cast<void**>(
        reinterpret_cast<char*>(helper) + CTRL_PTR_OFFSET);
    if (!controller) return;                 // controller not created yet
    if (g_vtblHooked.exchange(true)) return;
    void** vtbl = *reinterpret_cast<void***>(controller);
    g_vtblSlot = &vtbl[GETSETTINGS_VTBL_SLOT];
    DWORD oldProtect;
    if (VirtualProtect(g_vtblSlot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        GetSettings_Orig = reinterpret_cast<GetSettings_t>(*g_vtblSlot);
        *g_vtblSlot = reinterpret_cast<void*>(GetSettings_Hook);
        VirtualProtect(g_vtblSlot, sizeof(void*), oldProtect, &oldProtect);
        Wh_Log(L"settings cache active (controller vtable hooked)");
    } else {
        g_vtblHooked = false;
        Wh_Log(L"VirtualProtect failed; settings cache disabled");
    }
}

// ---------------------------------------------------------------------------
// Symbol hooks in SettingsHandlers_Notifications.dll
// ---------------------------------------------------------------------------

// NotificationsAppListHelper::_AddEntry(aumid, ...) runs once per app. We use it
// only as a reliable trigger: after the first entry the controller exists at
// helper+0x108, and we patch its vtable once (the patch persists for later visits).
using AddEntry_t = HRESULT (__cdecl*)(void* helper, PCWSTR aumid, unsigned long long opt);
static AddEntry_t AddEntry_Orig = nullptr;
HRESULT __cdecl AddEntry_Hook(void* helper, PCWSTR aumid, unsigned long long opt) {
    HookControllerVtable(helper); // no-op after the vtable is patched
    return AddEntry_Orig(helper, aumid, opt);
}

// AppInfo::PopulateSummaryValues(void) -> HRESULT. Skipped when enabled.
using PopSummary_t = HRESULT (__cdecl*)(void* self);
static PopSummary_t PopSummary_Orig = nullptr;
HRESULT __cdecl PopSummary_Hook(void* self) {
    if (g_skipSummary.load()) return S_OK;
    return PopSummary_Orig(self);
}

static std::atomic<bool> g_symHooked{false};
static void HookHandlerDll(bool applyNow) {
    if (g_symHooked.exchange(true)) return;
    HMODULE h = GetModuleHandleW(L"SettingsHandlers_Notifications.dll");
    if (!h) { g_symHooked = false; return; }

    // SettingsHandlers_Notifications.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {L"private: long __cdecl SystemSettings::NotificationsDataModel::NotificationsAppListHelper::_AddEntry(unsigned short const *,class std::optional<enum __MIDL___MIDL_itf_notificationsettings_0000_0000_0001>)"},
            (void**)&AddEntry_Orig,
            (void*)AddEntry_Hook,
            false,
        },
        {
            {L"private: long __cdecl SystemSettings::NotificationsDataModel::AppInfo::PopulateSummaryValues(void)"},
            (void**)&PopSummary_Orig,
            (void*)PopSummary_Hook,
            false,
        },
    };

    if (!WindhawkUtils::HookSymbols(h, hooks, ARRAYSIZE(hooks))) {
        g_symHooked = false;                    // symbols not ready; watcher retries
        Wh_Log(L"HookSymbols failed (symbols not ready?)");
        return;
    }

    if (applyNow) Wh_ApplyHookOperations();
    Wh_Log(L"installed symbol hooks");
}

// The handler DLL loads on demand when the Notifications page opens.
using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Orig;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE m = LoadLibraryExW_Orig(name, file, flags);
    if (m && name && StrStrIW(name, L"SettingsHandlers_Notifications"))
        HookHandlerDll(true);
    return m;
}

// Fallback in case the DLL loads via a path our LoadLibraryExW hook misses.
DWORD WINAPI WatchThread(LPVOID) {
    for (int i = 0; i < 600 && g_running; ++i) {
        if (!g_symHooked.load() &&
            GetModuleHandleW(L"SettingsHandlers_Notifications.dll"))
            HookHandlerDll(true);
        Sleep(200);
    }
    return 0;
}

// Ctrl+Alt+R clears the cache -> next visit does a full rescan.
DWORD WINAPI HotkeyThread(LPVOID) {
    while (g_running) {
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000) &&
            (GetAsyncKeyState('R') & 0x8000)) {
            FlushCache();
            Sleep(500);
        }
        Sleep(50);
    }
    return 0;
}

// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_skipSummary   = Wh_GetIntSetting(L"skipSummary") != 0;
    g_cacheSettings = Wh_GetIntSetting(L"cacheSettings") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook((void*)LoadLibraryExW, (void*)LoadLibraryExW_Hook,
                                   (void**)&LoadLibraryExW_Orig);
    HookHandlerDll(false); // in case the DLL is already loaded

    CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

void Wh_ModUninit() {
    g_running = false;
    Sleep(120);
    if (g_vtblHooked.load() && g_vtblSlot && GetSettings_Orig) {
        DWORD oldProtect;
        if (VirtualProtect(g_vtblSlot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
            *g_vtblSlot = reinterpret_cast<void*>(GetSettings_Orig);
            VirtualProtect(g_vtblSlot, sizeof(void*), oldProtect, &oldProtect);
        }
    }
    std::unique_lock lock(g_cacheMutex);
    for (auto& kv : g_cache)
        if (kv.second) kv.second->Release();
    g_cache.clear();
}

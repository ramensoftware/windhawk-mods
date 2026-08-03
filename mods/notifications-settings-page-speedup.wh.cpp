// ==WindhawkMod==
// @id              notifications-settings-page-speedup
// @name            Faster Notifications Settings Page
// @description     Stops Settings from re-scanning every notification app on each visit, so revisits load in seconds instead of minutes
// @version         1.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         SystemSettings.exe
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
reverts the page to normal. Turning **Cache app settings** off (then on again)
clears the cache, so the next open does a full rescan if you ever need one.

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
  $description: Makes revisiting the page fast. Cached objects read live data, so your edits are still reflected. Turning this off clears the cache and the page renders normally; turn it off then on to force a full rescan.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <atomic>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static std::atomic<bool> g_skipSummary{true};
static std::atomic<bool> g_cacheSettings{true};

// Set on unload so the worker thread exits its wait at once, and so an in-flight
// hook callback stops installing new hooks once teardown has started.
static HANDLE g_stopEvent = nullptr;
static std::atomic<bool> g_uninitializing{false};

// The Settings UI thread: the thread that runs _AddEntry and the controller call,
// and therefore owns the cached apartment-bound proxies. Captured in AddEntry_Hook.
static std::atomic<DWORD> g_uiThreadId{0};

// ---------------------------------------------------------------------------
// Run a callback on the thread that owns a given window. Used at unload to
// release the cached COM proxies on their owning apartment instead of from the
// unload thread. Standard Windhawk helper (see taskbar-vd-switcher).
// ---------------------------------------------------------------------------
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID);
static UINT g_runFromWindowThreadMsg = 0;
struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    PVOID param;
};

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == g_runFromWindowThreadMsg) {
            auto* p = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            p->proc(p->param);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Note this says nothing about whether the callback ran: the window can go away
// between the caller finding it and the message being sent, which is exactly what
// happens while the page is closing. The callback reports that itself.
//
// SendMessageTimeoutW rather than SendMessageW because SystemSettings.exe is a
// packaged app: once its window is closed the process lingers and its UI thread
// can be suspended, and an unbounded send there would hang the mod unload.
// UnhookWindowsHookEx runs before we return, so a message that arrives after the
// timeout can no longer call back into the mod.
static void RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return;
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                   nullptr, threadId);
    if (!hook) return;
    RUN_FROM_WINDOW_THREAD_PARAM param = {proc, procParam};
    SendMessageTimeoutW(hWnd, g_runFromWindowThreadMsg, 0,
                        reinterpret_cast<LPARAM>(&param),
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 5000, nullptr);
    UnhookWindowsHookEx(hook);
}

static BOOL CALLBACK FirstThreadWindowProc(HWND hWnd, LPARAM lParam) {
    *reinterpret_cast<HWND*>(lParam) = hWnd;
    return FALSE;  // first one is enough
}

static HWND GetWindowOnThread(DWORD threadId) {
    HWND result = nullptr;
    EnumThreadWindows(threadId, FirstThreadWindowProc, reinterpret_cast<LPARAM>(&result));
    return result;
}

// ---------------------------------------------------------------------------
// Per-AUMID cache of each app's IAumidNotificationSettings object.
//
// EnumerateAppSettingItems -> _AddEntry(aumid) per app -> controller
// ->vtable[0x70](aumid, &settings). The controller is an out-of-process COM
// object (CoCreateInstanceAsUser / MainNotificationController), so that call is
// a cross-process activation (~150-300ms each). We hook that slot: the first
// visit populates the cache, revisits reuse the live objects and skip the
// activation. Reads still hit the broker, so edits are reflected (no staleness).
//
// The cached objects are apartment-bound proxies owned by the Settings UI thread
// (the thread that runs _AddEntry / the controller call). They must be released
// on that thread, so a flush hands them to g_pendingRelease and the next
// _AddEntry drains it on the UI thread; at unload we release them by hopping onto
// the UI thread with RunFromWindowThread. The owning thread is latched by the
// first enumeration and never re-latched while it lives, so an enumeration on a
// different apartment skips the cache (slower, but it never gets a proxy it can't
// legally call). If that thread has exited, the proxies are unreachable and get
// dropped rather than released.
//
// The same goes for the handler DLL unloading: it serves all of these objects, so
// once it goes they are dropped without a Release, since calling through them
// would mean calling into an unmapped module.
// ---------------------------------------------------------------------------
static const size_t CTRL_PTR_OFFSET       = 0x108;  // helper -> controller ComPtr
static const size_t GETSETTINGS_VTBL_SLOT = 14;     // 0x70 / sizeof(void*)

static std::unordered_map<std::wstring, IUnknown*> g_cache;  // aumid -> settings obj
static std::vector<IUnknown*> g_pendingRelease;              // freed on the UI thread
static std::shared_mutex g_cacheMutex;
static void** g_vtblSlot = nullptr;
static IUnknown* g_controller = nullptr;  // pinned while the patch is installed
static std::atomic<bool> g_vtblHooked{false};

// Set when the handler DLL unloads. Everything we hold (the controller and every
// cached proxy) is served by that DLL, so once it goes the pointers are dead and
// calling Release through them would be calling into an unmapped module. The flag
// says "drop these, do not release them"; it is checked wherever the cache is read
// or freed, and cleared on the UI thread at the next enumeration, since the
// containers can't be touched from the loader callback.
static std::atomic<bool> g_cacheDead{false};

// Drop the cached pointers without releasing them, after the DLL that served them
// has gone. Must run on the UI thread (it takes the cache lock).
static void AbandonCache() {
    std::unique_lock lock(g_cacheMutex);
    g_cache.clear();
    g_pendingRelease.clear();
    Wh_Log(L"handler dll unloaded; cached objects dropped without release");
}

// Release proxies queued by a flush. Called from _AddEntry, i.e. the UI thread
// that owns the objects, so the release stays inside their apartment.
static void DrainPendingReleases() {
    std::vector<IUnknown*> toRelease;
    {
        std::unique_lock lock(g_cacheMutex);
        if (g_pendingRelease.empty()) return;
        toRelease.swap(g_pendingRelease);
    }
    if (g_cacheDead.load()) return;  // Their server is gone; dropping is all we can do.
    for (IUnknown* p : toRelease)
        if (p) p->Release();
}

// Hand the cached objects to the UI thread for release (next _AddEntry drains
// them), then the next page open does a full rescan.
static void FlushCache() {
    std::unique_lock lock(g_cacheMutex);
    for (auto& kv : g_cache)
        if (kv.second) g_pendingRelease.push_back(kv.second);
    g_cache.clear();
    Wh_Log(L"cache cleared (rescan on next open)");
}

// The controller vtable slot: HRESULT(controller, PCWSTR aumid, IUnknown** out)
using GetSettings_t = HRESULT (STDMETHODCALLTYPE*)(void*, PCWSTR, void**);
static GetSettings_t GetSettings_Orig = nullptr;

HRESULT STDMETHODCALLTYPE GetSettings_Hook(void* self, PCWSTR aumid, void** out) {
    // Only use the cache on the thread that created the proxies; a proxy handed to
    // another apartment would fail its reads with RPC_E_WRONG_THREAD.
    if (g_cacheSettings.load() && aumid && out && !g_cacheDead.load() &&
        GetCurrentThreadId() == g_uiThreadId.load()) {
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
            auto [it, inserted] = g_cache.emplace(aumid, p);
            if (!inserted) p->Release();  // another thread cached it first
        }
        return hr;
    }
    return GetSettings_Orig(self, aumid, out);
}

// Reject a garbage controller pointer before dereferencing it as a vtable, so a
// build where the +0x108 layout shifted no-ops instead of crashing SystemSettings.
static bool MemReadable(const void* p) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT) return false;
    return !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD));
}
static bool MemExecutable(const void* p) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT) return false;
    const DWORD exec = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                       PAGE_EXECUTE_WRITECOPY;
    return (mbi.Protect & exec) != 0;
}

// The notification controller interface (out-of-process COM object). We confirm
// the object at +0x108 is really it by identity rather than trusting the offset.
static const GUID IID_MainNotificationController = {
    0x2537d644, 0x8c2f, 0x4449,
    {0xb8, 0xb6, 0x10, 0x92, 0x88, 0x22, 0x63, 0x0c}};

// Verify the candidate at the known offset is the controller and return an
// AddRef'd pointer (caller frees), or nullptr. The readable-object plus
// executable-vtable[0] checks guard the single QueryInterface call. We only ever
// probe the one verified offset, never a blind scan of nearby members.
static IUnknown* FindController(void* helper) {
    char* base = reinterpret_cast<char*>(helper);
    if (!MemReadable(base + CTRL_PTR_OFFSET)) return nullptr;
    void* cand = *reinterpret_cast<void**>(base + CTRL_PTR_OFFSET);
    if (!cand || !MemReadable(cand)) return nullptr;
    void** vt = *reinterpret_cast<void***>(cand);
    if (!MemReadable(vt) || !MemExecutable(vt[0])) return nullptr;
    IUnknown* out = nullptr;
    if (SUCCEEDED(reinterpret_cast<IUnknown*>(cand)->QueryInterface(
            IID_MainNotificationController, reinterpret_cast<void**>(&out))))
        return out;
    return nullptr;
}

// Put the original pointer back in the patched vtable slot. VirtualProtect plus a
// single store, no COM and no loader re-entry, so it is safe to call from the DLL
// unload notification as well as at teardown. The compare means a slot somebody
// else has since taken over is left alone.
static void RestoreVtableSlot() {
    if (!g_vtblHooked.load() || !g_vtblSlot || !GetSettings_Orig) return;
    DWORD oldProtect;
    if (VirtualProtect(g_vtblSlot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        if (*g_vtblSlot == reinterpret_cast<void*>(GetSettings_Hook))
            *g_vtblSlot = reinterpret_cast<void*>(GetSettings_Orig);
        VirtualProtect(g_vtblSlot, sizeof(void*), oldProtect, &oldProtect);
    }
}

// Bound how many entries we probe before giving up, so a build where the
// controller never resolves can't turn into a QueryInterface on every app.
static std::atomic<int> g_hookAttempts{0};
static const int MAX_HOOK_ATTEMPTS = 8;

static void HookControllerVtable(void* helper) {
    if (g_uninitializing.load() || g_vtblHooked.load()) return;
    if (g_hookAttempts.fetch_add(1) >= MAX_HOOK_ATTEMPTS) return;

    IUnknown* controller = FindController(helper);
    if (!controller) return;  // not created yet, or layout shifted; maybe retry

    void** vtbl = *reinterpret_cast<void***>(controller);
    void* target = (vtbl && MemReadable(vtbl)) ? vtbl[GETSETTINGS_VTBL_SLOT] : nullptr;
    if (!target || !MemExecutable(target)) { controller->Release(); return; }

    if (g_vtblHooked.exchange(true)) { controller->Release(); return; }

    g_vtblSlot = &vtbl[GETSETTINGS_VTBL_SLOT];
    DWORD oldProtect;
    if (VirtualProtect(g_vtblSlot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        GetSettings_Orig = reinterpret_cast<GetSettings_t>(*g_vtblSlot);
        *g_vtblSlot = reinterpret_cast<void*>(GetSettings_Hook);
        VirtualProtect(g_vtblSlot, sizeof(void*), oldProtect, &oldProtect);
        g_controller = controller;  // pin the proxy so its vtable page stays mapped
        Wh_Log(L"settings cache active (controller vtable hooked)");
    } else {
        g_vtblHooked = false;
        controller->Release();
        Wh_Log(L"VirtualProtect failed; settings cache disabled");
    }
}

// ---------------------------------------------------------------------------
// Symbol hooks in SettingsHandlers_Notifications.dll
// ---------------------------------------------------------------------------

// NotificationsAppListHelper::_AddEntry(aumid, ...) runs once per app on the
// Settings UI thread. We use it as a reliable trigger: after the first entry the
// controller exists at helper+0x108, and we patch its vtable once. It is also
// where we record the owning thread and drain proxies queued by a flush, since
// this is their owning apartment.
using AddEntry_t = HRESULT (__cdecl*)(void* helper, PCWSTR aumid, unsigned long long opt);
static AddEntry_t AddEntry_Orig = nullptr;
// True while a thread id still refers to a running thread.
static bool ThreadAlive(DWORD threadId) {
    HANDLE h = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!h) return false;
    bool alive = WaitForSingleObject(h, 0) == WAIT_TIMEOUT;
    CloseHandle(h);
    return alive;
}

HRESULT __cdecl AddEntry_Hook(void* helper, PCWSTR aumid, unsigned long long opt) {
    DWORD self = GetCurrentThreadId();
    DWORD owner = g_uiThreadId.load();

    // The owner is latched by the first enumeration and then left alone. Storing it
    // on every call made the apartment check meaningless, because a second thread
    // would overwrite it and then match itself, which is the wrong-apartment case
    // the check exists for.
    if (owner == 0) {
        DWORD expected = 0;
        g_uiThreadId.compare_exchange_strong(expected, self);
        owner = g_uiThreadId.load();
    } else if (owner != self && !ThreadAlive(owner)) {
        // The apartment that made the cached proxies has exited, so nothing can
        // legally release them anymore. Drop them and let this thread take over,
        // otherwise the cache would be bypassed for the rest of the session.
        g_cacheDead = true;
        g_uiThreadId.store(self);
        owner = self;
    }

    // Anything that isn't the owning apartment falls through to the original path:
    // slower, but correct, instead of being handed a proxy it can't call.
    if (owner == self) {
        if (g_cacheDead.exchange(false)) {
            AbandonCache();
        }
        DrainPendingReleases();
        HookControllerVtable(helper);  // no-op after the vtable is patched
    }
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
        g_symHooked = false;  // symbols not ready yet; the worker retries on reload
        Wh_Log(L"HookSymbols failed (symbols not ready?)");
        return;
    }

    if (applyNow) Wh_ApplyHookOperations();
    Wh_Log(L"installed symbol hooks");
}

// The handler DLL loads on demand when the Notifications page opens. Rather than
// detour a load function (which misses load paths that don't go through it, and
// hooking kernelbase's LoadLibraryExW directly fast-fails SystemSettings under
// CFG), ask the loader to notify us. LdrRegisterDllNotification fires for every
// load path, needs no detour, and lets the polling backstop go away.
typedef struct _WH_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} WH_UNICODE_STRING;
typedef struct _WH_LDR_DLL_NOTIFICATION_DATA {
    ULONG Flags;
    const WH_UNICODE_STRING* FullDllName;
    const WH_UNICODE_STRING* BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
} WH_LDR_DLL_NOTIFICATION_DATA;
typedef VOID(CALLBACK* WH_LDR_DLL_NOTIFICATION_FUNCTION)(
    ULONG, const WH_LDR_DLL_NOTIFICATION_DATA*, PVOID);
typedef LONG(NTAPI* LdrRegisterDllNotification_t)(
    ULONG, WH_LDR_DLL_NOTIFICATION_FUNCTION, PVOID, PVOID*);
typedef LONG(NTAPI* LdrUnregisterDllNotification_t)(PVOID);
static const ULONG WH_LDR_DLL_NOTIFICATION_REASON_LOADED   = 1;
static const ULONG WH_LDR_DLL_NOTIFICATION_REASON_UNLOADED = 2;

static PVOID  g_ldrCookie = nullptr;
static HANDLE g_dllLoadedEvent = nullptr;    // signalled by the loader callback
static HANDLE g_hookWorkerThread = nullptr;

// Runs under the loader lock, so keep it minimal: exact name check, then either
// signal the worker (on load) or drop our latches (on unload) with no COM calls.
static VOID CALLBACK DllNotification(ULONG reason,
                                     const WH_LDR_DLL_NOTIFICATION_DATA* data, PVOID) {
    if (!data || !data->BaseDllName || !data->BaseDllName->Buffer) return;
    const WH_UNICODE_STRING* n = data->BaseDllName;
    USHORT chars = n->Length / sizeof(WCHAR);
    if (chars == 0 || chars > MAX_PATH) return;
    wchar_t name[MAX_PATH + 1];
    memcpy(name, n->Buffer, chars * sizeof(WCHAR));
    name[chars] = L'\0';
    if (_wcsicmp(name, L"SettingsHandlers_Notifications.dll") != 0) return;

    if (reason == WH_LDR_DLL_NOTIFICATION_REASON_LOADED) {
        if (g_dllLoadedEvent) SetEvent(g_dllLoadedEvent);
    } else if (reason == WH_LDR_DLL_NOTIFICATION_REASON_UNLOADED) {
        // The DLL, its helper, and everything it served are going away, and this
        // is the last moment the patched slot can be reached: g_vtblSlot is the
        // only handle on it, so dropping the pointer without restoring first would
        // leave a live slot aimed at mod code that is about to unmap. The restore
        // is only VirtualProtect plus a store, which is fine under the loader lock.
        RestoreVtableSlot();
        g_vtblHooked  = false;
        g_vtblSlot    = nullptr;
        // GetSettings_Orig deliberately keeps its value: a thread that is already
        // inside the hook still has to have something to call through.
        // No COM here. The controller and every cached proxy are served by the DLL
        // that is unloading, so releasing them would call into an unmapping module;
        // they get dropped instead, on the UI thread, at the next enumeration.
        g_controller  = nullptr;
        g_cacheDead   = true;
        g_hookAttempts = 0;
        g_symHooked   = false;
    }
}

// Installs the symbol hooks off the loader lock once the handler DLL appears.
DWORD WINAPI HookWorkerThread(LPVOID) {
    HANDLE waits[2] = {g_stopEvent, g_dllLoadedEvent};
    for (;;) {
        DWORD w = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (w == WAIT_OBJECT_0 + 1)      // g_dllLoadedEvent
            HookHandlerDll(true);
        else                             // g_stopEvent, or an error
            break;
    }
    return 0;
}

// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_skipSummary   = Wh_GetIntSetting(L"skipSummary") != 0;
    g_cacheSettings = Wh_GetIntSetting(L"cacheSettings") != 0;
}

// Reports back, because a caller can't tell from RunFromWindowThread whether the
// message was ever delivered.
struct RELEASE_PARAM {
    bool ran = false;
};

// Runs on the UI thread (via RunFromWindowThread) at unload: pull the vtable
// hook and release everything in the apartment that owns it.
static void WINAPI ReleaseComStateOnUiThread(PVOID p) {
    RestoreVtableSlot();

    bool dead = g_cacheDead.load();  // Server gone: drop the pointers, don't call them.
    if (g_controller) {
        if (!dead) g_controller->Release();
        g_controller = nullptr;
    }

    std::vector<IUnknown*> toRelease;
    {
        std::unique_lock lock(g_cacheMutex);
        toRelease.swap(g_pendingRelease);
        for (auto& kv : g_cache)
            if (kv.second) toRelease.push_back(kv.second);
        g_cache.clear();
    }
    if (!dead) {
        for (IUnknown* q : toRelease)
            if (q) q->Release();
    }

    if (p) static_cast<RELEASE_PARAM*>(p)->ran = true;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_runFromWindowThreadMsg = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_notifications-settings-page-speedup");
    g_stopEvent      = CreateEventW(nullptr, TRUE, FALSE, nullptr);   // manual reset
    g_dllLoadedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // auto reset

    // Registered before the already-loaded check, not after, so a load that lands
    // in between is caught rather than missed for the rest of the session. The
    // g_symHooked latch makes the duplicate attempt a no-op.
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto reg = ntdll ? (LdrRegisterDllNotification_t)GetProcAddress(
                           ntdll, "LdrRegisterDllNotification")
                     : nullptr;
    if (!reg || reg(0, DllNotification, nullptr, &g_ldrCookie) != 0)
        Wh_Log(L"LdrRegisterDllNotification unavailable");

    HookHandlerDll(false);  // in case the DLL is already loaded (Windhawk applies
                            // the hooks after Wh_ModInit returns)
    return TRUE;
}

// Hook operations are only legal between Wh_ModInit returning and
// Wh_ModBeforeUninit returning, so the worker (which calls Wh_ApplyHookOperations)
// lives entirely inside that window.
void Wh_ModAfterInit() {
    g_hookWorkerThread =
        CreateThread(nullptr, 0, HookWorkerThread, nullptr, 0, nullptr);
}

void Wh_ModSettingsChanged() {
    bool wasCaching = g_cacheSettings.load();
    LoadSettings();
    if (wasCaching && !g_cacheSettings.load())
        FlushCache();  // turning caching off clears the cache; next open rescans
}

void Wh_ModBeforeUninit() {
    g_uninitializing.store(true);

    // Stop the loader callback first so it can't fire during teardown.
    if (g_ldrCookie) {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        auto unreg = ntdll ? (LdrUnregisterDllNotification_t)GetProcAddress(
                                 ntdll, "LdrUnregisterDllNotification")
                           : nullptr;
        if (unreg) unreg(g_ldrCookie);
        g_ldrCookie = nullptr;
    }

    // Stop and join the worker while hook operations are still legal, so any hook
    // it was mid-installing is applied here and Windhawk still removes it.
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_hookWorkerThread) {
        WaitForSingleObject(g_hookWorkerThread, INFINITE);
        CloseHandle(g_hookWorkerThread);
        g_hookWorkerThread = nullptr;
    }
}

void Wh_ModUninit() {
    // Restore the vtable slot and release the proxies on their owning UI thread.
    RELEASE_PARAM param;
    if (DWORD tid = g_uiThreadId.load()) {
        if (HWND hWnd = GetWindowOnThread(tid))
            RunFromWindowThread(hWnd, ReleaseComStateOnUiThread, &param);
    }
    if (!param.ran) {
        // The callback never got to run: no window, the window died while we were
        // reaching for it, or the send timed out against a suspended thread. Pull
        // the hook from here anyway so SystemSettings can't call into unmapped
        // code, and leave the proxies alone, since a cross-apartment release is
        // worse than a leak at process teardown.
        RestoreVtableSlot();
    }

    if (g_dllLoadedEvent) { CloseHandle(g_dllLoadedEvent); g_dllLoadedEvent = nullptr; }
    if (g_stopEvent)      { CloseHandle(g_stopEvent);      g_stopEvent = nullptr; }
}

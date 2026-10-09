// ==WindhawkMod==
// @id              secondary-monitor-pin
// @name            Secondary Monitor Pin
// @description     Keeps windows that are placed on secondary monitors pinned so they appear on all virtual desktops.
// @version         1.1
// @author          tafadias
// @github          https://github.com/tafadias
// @include         explorer.exe
// @compilerOptions -luser32 -lole32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Secondary Monitor Pin

Automatically pins windows that are placed on a **secondary monitor** so that
they are shown on **all virtual desktops**, the same as choosing *Show this
window on all desktops* from the window's title bar context menu.

When a window moves back to the primary monitor it is unpinned again, but only
if this mod was the one that pinned it. Windows that were pinned manually are
left untouched.

The mod only talks to the virtual desktop manager when a window changes
monitors, when a new window appears or when a window closes. This keeps the
number of shell calls low, so switching virtual desktops or dragging windows
between monitors stays smooth.

## Notes

- Uses undocumented Windows virtual desktop interfaces, which may change with
  future Windows updates.
- Works on Windows 10 and Windows 11.
- Do not run this mod together with another tool that also pins windows across
  virtual desktops (for example an AutoHotkey script doing the same), as two
  implementations competing over the same shell interfaces can hang the shell.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalMs: 800
  $name: Poll interval (ms)
  $description: >-
    How often all windows are scanned and the pin state is re-applied, in
    milliseconds. Lower values react faster but use more CPU.
- unpinOnUnload: false
  $name: Unpin windows when the mod is unloaded
  $description: >-
    When enabled, every window pinned by this mod is unpinned when the mod is
    disabled or unloaded.
- excludedClasses: [""]
  $name: Additional excluded window classes
  $description: >-
    Window classes that should never be pinned, one per entry. A number of
    system classes are always excluded.
- logActions: false
  $name: Log pin and unpin actions
  $description: Writes an entry to the Windhawk log whenever a window is pinned or unpinned.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <string>
#include <vector>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

// Undocumented virtual desktop interfaces, stable across Windows 10 and 11.
static const GUID kCLSID_ImmersiveShell =
    {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
static const GUID kIID_IServiceProvider =
    {0x6D5140C1, 0x7436, 0x11CE, {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}};
static const GUID kCLSID_VirtualDesktopPinnedApps =
    {0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};
static const GUID kIID_IVirtualDesktopPinnedApps =
    {0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};
static const GUID kIID_IApplicationViewCollection =
    {0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};
static const GUID kIID_IApplicationViewCollectionWin10 =
    {0x2C08ADF0, 0xA386, 0x4B35, {0x92, 0x50, 0x0F, 0xE1, 0x83, 0x47, 0x6F, 0xCC}};

// Method order follows the raw vtable layout of the COM interfaces.
struct IServiceProviderLocal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService,
                                                   REFIID riid,
                                                   void** ppvObject) = 0;
};

struct IApplicationViewCollectionLocal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetViews(IUnknown** ppViews) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IUnknown** ppViews) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(PCWSTR appUserModelId,
                                                              IUnknown** ppViews) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND hwnd, IUnknown** ppView) = 0;
};

struct IVirtualDesktopPinnedAppsLocal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsAppIdPinned(PCWSTR appId, BOOL* pinned) = 0;
    virtual HRESULT STDMETHODCALLTYPE PinAppId(PCWSTR appId) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnpinAppId(PCWSTR appId) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsViewPinned(IUnknown* view, BOOL* pinned) = 0;
    virtual HRESULT STDMETHODCALLTYPE PinView(IUnknown* view) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnpinView(IUnknown* view) = 0;
};

#define WM_SMP_RELOAD (WM_APP + 1)
#define WM_SMP_QUIT   (WM_APP + 2)

#define TIMER_POLL       1
#define TIMER_DEBOUNCE   2
#define TIMER_COM_RETRY  3

static IServiceProviderLocal* g_sp = nullptr;
static IVirtualDesktopPinnedAppsLocal* g_pinnedApps = nullptr;
static IApplicationViewCollectionLocal* g_views = nullptr;

static HWND g_hwnd = nullptr;
static HANDLE g_thread = nullptr;

static HWINEVENTHOOK g_hookShow = nullptr;
static HWINEVENTHOOK g_hookDestroy = nullptr;
static HWINEVENTHOOK g_hookMoveSize = nullptr;

static std::vector<HWND> g_wePinned;
static HWND g_movingHwnd = nullptr;
static int g_comRetries = 0;
static bool g_started = false;
static volatile LONG g_inSync = 0;

struct CachedWindow {
    HWND hwnd;
    bool onPrimary;
};
static std::vector<CachedWindow> g_cache;

static volatile LONG g_pollIntervalMs = 800;
static volatile LONG g_unpinOnUnload = 0;
static volatile LONG g_logActions = 0;
static std::vector<std::wstring> g_extraExcluded;

static bool ContainsPinned(HWND hwnd) {
    for (HWND h : g_wePinned) {
        if (h == hwnd)
            return true;
    }
    return false;
}

static void RemovePinned(HWND hwnd) {
    for (size_t i = 0; i < g_wePinned.size(); i++) {
        if (g_wePinned[i] == hwnd) {
            g_wePinned.erase(g_wePinned.begin() + i);
            return;
        }
    }
}

static bool CacheLookup(HWND hwnd, bool* onPrimary) {
    for (const CachedWindow& entry : g_cache) {
        if (entry.hwnd == hwnd) {
            *onPrimary = entry.onPrimary;
            return true;
        }
    }
    return false;
}

static void CacheStore(HWND hwnd, bool onPrimary) {
    for (CachedWindow& entry : g_cache) {
        if (entry.hwnd == hwnd) {
            entry.onPrimary = onPrimary;
            return;
        }
    }
    g_cache.push_back({hwnd, onPrimary});
}

static void CacheRemove(HWND hwnd) {
    for (size_t i = 0; i < g_cache.size(); i++) {
        if (g_cache[i].hwnd == hwnd) {
            g_cache.erase(g_cache.begin() + i);
            return;
        }
    }
}

static bool IsExcludedClass(const wchar_t* cls) {
    static const wchar_t* kBuiltin[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"NotifyIconOverflowWindow",
        L"MultitaskingViewFrame",
        L"ForegroundStaging",
        L"XamlExplorerHostIslandWindow",
        L"Windows.UI.Core.CoreWindow",
        L"ApplicationManager_ImmersiveShellWindow",
        L"Shell_LightDismissOverlay",
        L"#32771",
    };
    for (const wchar_t* name : kBuiltin) {
        if (_wcsicmp(cls, name) == 0)
            return true;
    }
    for (const std::wstring& name : g_extraExcluded) {
        if (_wcsicmp(cls, name.c_str()) == 0)
            return true;
    }
    return false;
}

static bool IsOnPrimaryMonitor(HWND hwnd) {
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!hMon)
        return true;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi))
        return true;

    return (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
}

static bool IsCandidate(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    if (GetAncestor(hwnd, GA_ROOT) != hwnd)
        return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD)
        return false;

    bool visible = (style & WS_VISIBLE) != 0;
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (!visible && !cloaked)
        return false;

    WCHAR cls[256];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return false;
    if (IsExcludedClass(cls))
        return false;

    if (IsIconic(hwnd))
        return true;

    RECT rc;
    if (!GetWindowRect(hwnd, &rc))
        return false;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w < 0)
        w = -w;
    if (h < 0)
        h = -h;
    return w >= 32 && h >= 32;
}

static bool VdGetView(HWND hwnd, IUnknown** view) {
    *view = nullptr;
    if (!g_views)
        return false;
    if (FAILED(g_views->GetViewForHwnd(hwnd, view)) || !*view)
        return false;
    return true;
}

static bool VdIsPinned(HWND hwnd, BOOL* pinned) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->IsViewPinned(view, pinned);
    view->Release();
    return SUCCEEDED(hr);
}

static bool VdPin(HWND hwnd) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->PinView(view);
    view->Release();
    return SUCCEEDED(hr);
}

static bool VdUnpin(HWND hwnd) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->UnpinView(view);
    view->Release();
    return SUCCEEDED(hr);
}

static bool VdSyncWindow(HWND hwnd, bool onPrimary) {
    BOOL pinned = FALSE;
    if (!VdIsPinned(hwnd, &pinned))
        return false;

    if (!onPrimary) {
        if (!pinned) {
            if (!VdPin(hwnd))
                return false;
            g_wePinned.push_back(hwnd);
            if (g_logActions)
                Wh_Log(L"Pinned window %p", hwnd);
        }
        return true;
    }

    if (hwnd == g_movingHwnd)
        return false;

    if (ContainsPinned(hwnd)) {
        if (pinned)
            VdUnpin(hwnd);
        RemovePinned(hwnd);
        if (g_logActions)
            Wh_Log(L"Unpinned window %p", hwnd);
    }
    return true;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* seen = reinterpret_cast<std::vector<HWND>*>(lParam);
    if (hwnd != g_movingHwnd && IsCandidate(hwnd)) {
        bool onPrimary = IsOnPrimaryMonitor(hwnd);
        bool cachedOnPrimary = false;
        if (!CacheLookup(hwnd, &cachedOnPrimary) || cachedOnPrimary != onPrimary) {
            if (VdSyncWindow(hwnd, onPrimary))
                CacheStore(hwnd, onPrimary);
        }
    }
    seen->push_back(hwnd);
    return TRUE;
}

static void VdSyncAll() {
    if (InterlockedCompareExchange(&g_inSync, 1, 0) != 0)
        return;

    std::vector<HWND> seen;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&seen));

    for (size_t i = g_wePinned.size(); i-- > 0;) {
        HWND hwnd = g_wePinned[i];
        bool present = false;
        for (HWND h : seen) {
            if (h == hwnd) {
                present = true;
                break;
            }
        }
        if (!present || !IsWindow(hwnd))
            g_wePinned.erase(g_wePinned.begin() + i);
    }

    for (size_t i = g_cache.size(); i-- > 0;) {
        HWND hwnd = g_cache[i].hwnd;
        bool present = false;
        for (HWND h : seen) {
            if (h == hwnd) {
                present = true;
                break;
            }
        }
        if (!present || !IsWindow(hwnd))
            g_cache.erase(g_cache.begin() + i);
    }

    InterlockedExchange(&g_inSync, 0);
}

static void VdRequestSync() {
    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_DEBOUNCE);
        SetTimer(g_hwnd, TIMER_DEBOUNCE, 200, nullptr);
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG,
                                  DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd)
        return;

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        g_movingHwnd = hwnd;
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        g_movingHwnd = nullptr;
        VdRequestSync();
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        RemovePinned(hwnd);
        CacheRemove(hwnd);
        return;
    }

    if (event == EVENT_OBJECT_SHOW)
        VdRequestSync();
}

static void LoadSettings() {
    LONG poll = Wh_GetIntSetting(L"pollIntervalMs");
    if (poll < 100)
        poll = 100;
    if (poll > 10000)
        poll = 10000;
    g_pollIntervalMs = poll;

    g_unpinOnUnload = Wh_GetIntSetting(L"unpinOnUnload") ? 1 : 0;
    g_logActions = Wh_GetIntSetting(L"logActions") ? 1 : 0;

    g_extraExcluded.clear();
    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(L"excludedClasses[%d]", i);
        bool hasValue = value && *value;
        if (hasValue)
            g_extraExcluded.push_back(value);
        Wh_FreeStringSetting(value);
        if (!hasValue)
            break;
    }
}

static bool InitCom() {
    if (g_sp && g_pinnedApps && g_views)
        return true;

    if (!g_sp) {
        if (FAILED(CoCreateInstance(kCLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                    kIID_IServiceProvider, (void**)&g_sp)))
            return false;
    }

    if (!g_pinnedApps && FAILED(g_sp->QueryService(kCLSID_VirtualDesktopPinnedApps,
                                                   kIID_IVirtualDesktopPinnedApps,
                                                   (void**)&g_pinnedApps)))
        return false;

    if (!g_views &&
        FAILED(g_sp->QueryService(kIID_IApplicationViewCollection,
                                  kIID_IApplicationViewCollection, (void**)&g_views)) &&
        FAILED(g_sp->QueryService(kIID_IApplicationViewCollectionWin10,
                                  kIID_IApplicationViewCollectionWin10, (void**)&g_views)))
        return false;

    return true;
}

static void Start() {
    g_started = true;
    VdSyncAll();

    SetTimer(g_hwnd, TIMER_POLL, (UINT)g_pollIntervalMs, nullptr);

    g_hookShow = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WinEventProc,
                                 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hookDestroy = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr,
                                    WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hookMoveSize = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                     nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    Wh_Log(L"Started (poll %d ms)", g_pollIntervalMs);
}

static void StopTimersAndHooks() {
    if (g_hwnd) {
        KillTimer(g_hwnd, TIMER_POLL);
        KillTimer(g_hwnd, TIMER_DEBOUNCE);
        KillTimer(g_hwnd, TIMER_COM_RETRY);
    }
    if (g_hookShow) {
        UnhookWinEvent(g_hookShow);
        g_hookShow = nullptr;
    }
    if (g_hookDestroy) {
        UnhookWinEvent(g_hookDestroy);
        g_hookDestroy = nullptr;
    }
    if (g_hookMoveSize) {
        UnhookWinEvent(g_hookMoveSize);
        g_hookMoveSize = nullptr;
    }
}

static void ReleaseCom() {
    if (g_views) {
        g_views->Release();
        g_views = nullptr;
    }
    if (g_pinnedApps) {
        g_pinnedApps->Release();
        g_pinnedApps = nullptr;
    }
    if (g_sp) {
        g_sp->Release();
        g_sp = nullptr;
    }
}

static bool ShellAlive() {
    HWND shell = GetShellWindow();
    return shell && IsWindow(shell);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_POLL) {
            VdSyncAll();
        } else if (wParam == TIMER_DEBOUNCE) {
            KillTimer(hwnd, TIMER_DEBOUNCE);
            VdSyncAll();
        } else if (wParam == TIMER_COM_RETRY) {
            KillTimer(hwnd, TIMER_COM_RETRY);
            if (InitCom()) {
                Start();
            } else if (g_comRetries++ < 60) {
                SetTimer(hwnd, TIMER_COM_RETRY, 1000, nullptr);
            } else {
                Wh_Log(L"Virtual desktop interfaces unavailable");
            }
        }
        return 0;

    case WM_SMP_RELOAD:
        LoadSettings();
        if (g_started && g_hwnd)
            SetTimer(g_hwnd, TIMER_POLL, (UINT)g_pollIntervalMs, nullptr);
        return 0;

    case WM_SMP_QUIT:
        StopTimersAndHooks();
        if (ShellAlive()) {
            if (g_unpinOnUnload) {
                for (HWND h : g_wePinned)
                    VdUnpin(h);
            }
            ReleaseCom();
        } else {
            g_sp = nullptr;
            g_pinnedApps = nullptr;
            g_views = nullptr;
        }
        g_wePinned.clear();
        g_cache.clear();
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI ThreadProc(LPVOID) {
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool shouldUninitialize = SUCCEEDED(coHr);
    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", (unsigned)coHr);
        return 1;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"SecondaryMonitorPinWindow";
    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                             wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        if (shouldUninitialize)
            CoUninitialize();
        return 1;
    }

    LoadSettings();

    if (InitCom())
        Start();
    else
        SetTimer(g_hwnd, TIMER_COM_RETRY, 1000, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    ReleaseCom();
    if (shouldUninitialize)
        CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hwnd)
        PostMessageW(g_hwnd, WM_SMP_QUIT, 0, 0);

    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    if (g_hwnd)
        PostMessageW(g_hwnd, WM_SMP_RELOAD, 0, 0);
}

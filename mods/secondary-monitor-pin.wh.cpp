// ==WindhawkMod==
// @id              secondary-monitor-pin
// @name            Secondary Monitor Pin
// @description     Keeps windows that are placed on secondary monitors pinned so they appear on all virtual desktops.
// @version         1.1
// @author          tafadias
// @github          https://github.com/tafadias
// @include         windhawk.exe
// @license         MIT
// @compilerOptions -luser32 -lole32 -ldwmapi -luuid
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

The mod runs as a tool mod in a dedicated process, so it does not inject into
`explorer.exe`. It only talks to the virtual desktop manager when a window
changes monitors, when a new window appears or when a window closes, which keeps
the number of shell calls low and keeps switching virtual desktops smooth. A
timer polls for monitor changes that the window event hooks do not report, such
as `Win`+`Shift`+arrow, programmatic moves and display changes.

Windows that are on another virtual desktop are pinned too, so the
secondary-monitor windows of every desktop end up shown together on all
desktops.

## Notes

- Uses undocumented Windows virtual desktop interfaces, which may change with
  future Windows updates.
- Works on Windows 10 and Windows 11.
- Do not run this mod together with another tool that also pins windows across
  virtual desktops, as two implementations competing over the same shell
  interfaces can cause issues.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalMs: 800
  $name: Poll interval (ms)
  $description: >-
    How often all windows are scanned for monitor changes, in milliseconds.
    Lower values react faster but use more CPU.
- excludedClasses: [""]
  $name: Additional excluded window classes
  $description: >-
    Window classes that should never be pinned, one per entry. A number of
    system classes are always excluded.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <servprov.h>
#include <shellapi.h>

#include <atomic>
#include <string>
#include <vector>

// Undocumented virtual desktop interfaces. The pinned apps IID and the view
// collection vtable are shared by Windows 10 and Windows 11; only the view
// collection IID differs between them.
static const GUID kCLSID_ImmersiveShell =
    {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
static const GUID kCLSID_VirtualDesktopPinnedApps =
    {0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};
static const GUID kIID_IVirtualDesktopPinnedApps =
    {0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};
static const GUID kIID_IApplicationViewCollection =
    {0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};
static const GUID kIID_IApplicationViewCollectionWin10 =
    {0x2C08ADF0, 0xA386, 0x4B35, {0x92, 0x50, 0x0F, 0xE1, 0x83, 0x47, 0x6F, 0xCC}};

// Method order follows the raw vtable layout of the COM interfaces.
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

static constexpr DWORD kNoViewRetryMs = 5000;
static constexpr DWORD kComRetryMs = 2000;

static IServiceProvider* g_sp = nullptr;
static IVirtualDesktopPinnedAppsLocal* g_pinnedApps = nullptr;
static IApplicationViewCollectionLocal* g_views = nullptr;

static std::atomic<HWND> g_hwnd{nullptr};
static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_initOk = false;

static HWINEVENTHOOK g_hookShow = nullptr;
static HWINEVENTHOOK g_hookDestroy = nullptr;
static HWINEVENTHOOK g_hookMoveSize = nullptr;

static std::vector<HWND> g_wePinned;
static HWND g_movingHwnd = nullptr;
static bool g_started = false;
static bool g_reconnectPending = false;
static DWORD g_shellPid = 0;

struct CachedWindow {
    HWND hwnd;
    bool onPrimary;
    bool noView;
    ULONGLONG retryAt;
};
static std::vector<CachedWindow> g_cache;

static LONG g_pollIntervalMs = 800;
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

static CachedWindow* CacheFind(HWND hwnd) {
    for (CachedWindow& entry : g_cache) {
        if (entry.hwnd == hwnd)
            return &entry;
    }
    return nullptr;
}

static void CacheStore(HWND hwnd, bool onPrimary, bool noView, ULONGLONG retryAt) {
    CachedWindow* entry = CacheFind(hwnd);
    if (entry) {
        entry->onPrimary = onPrimary;
        entry->noView = noView;
        entry->retryAt = retryAt;
        return;
    }
    g_cache.push_back({hwnd, onPrimary, noView, retryAt});
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

static bool IsAltTabWindow(HWND hwnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW)
        return false;
    if (exStyle & WS_EX_NOACTIVATE)
        return false;
    if (GetAncestor(hwnd, GA_ROOTOWNER) != hwnd && !(exStyle & WS_EX_APPWINDOW))
        return false;
    return true;
}

static bool IsCandidate(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    if (!IsAltTabWindow(hwnd))
        return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
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

    return (rc.right - rc.left) >= 32 && (rc.bottom - rc.top) >= 32;
}

// HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) and (RPC_S_CALL_FAILED), which a
// call returns when the shell process crashed or was killed.
static constexpr HRESULT kRpcServerUnavailable = (HRESULT)0x800706BAL;
static constexpr HRESULT kRpcCallFailed = (HRESULT)0x800706BEL;

static bool IsConnectionLost(HRESULT hr) {
    return hr == RPC_E_DISCONNECTED || hr == RPC_E_SERVER_DIED ||
           hr == RPC_E_SERVER_DIED_DNE || hr == CO_E_OBJNOTCONNECTED ||
           hr == kRpcServerUnavailable || hr == kRpcCallFailed;
}

static DWORD GetShellPid() {
    DWORD pid = 0;
    HWND shell = GetShellWindow();
    if (shell)
        GetWindowThreadProcessId(shell, &pid);
    return pid;
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

static bool InitCom();

static void StopTimersAndHooks();

static void ReinitializeCom() {
    ReleaseCom();
    // The windows, and possibly their pins, outlive the COM objects; only the
    // cached monitor state needs to be rebuilt.
    g_cache.clear();

    if (!InitCom() && g_started) {
        StopTimersAndHooks();
        g_started = false;
    }

    if (!g_started && g_hwnd)
        SetTimer(g_hwnd.load(), TIMER_COM_RETRY, kComRetryMs, nullptr);
}

static bool VdGetView(HWND hwnd, IUnknown** view) {
    *view = nullptr;
    if (!g_views)
        return false;
    HRESULT hr = g_views->GetViewForHwnd(hwnd, view);
    if (FAILED(hr)) {
        if (IsConnectionLost(hr))
            g_reconnectPending = true;
        return false;
    }
    return *view != nullptr;
}

static bool VdIsPinned(HWND hwnd, BOOL* pinned) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->IsViewPinned(view, pinned);
    view->Release();
    if (FAILED(hr) && IsConnectionLost(hr))
        g_reconnectPending = true;
    return SUCCEEDED(hr);
}

static bool VdPin(HWND hwnd) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->PinView(view);
    view->Release();
    if (FAILED(hr) && IsConnectionLost(hr))
        g_reconnectPending = true;
    return SUCCEEDED(hr);
}

static bool VdUnpin(HWND hwnd) {
    IUnknown* view = nullptr;
    if (!VdGetView(hwnd, &view))
        return false;
    HRESULT hr = g_pinnedApps->UnpinView(view);
    view->Release();
    if (FAILED(hr) && IsConnectionLost(hr))
        g_reconnectPending = true;
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
            if (!ContainsPinned(hwnd))
                g_wePinned.push_back(hwnd);
            Wh_Log(L"Pinned window %p", (void*)hwnd);
        }
        return true;
    }

    if (ContainsPinned(hwnd)) {
        if (pinned)
            VdUnpin(hwnd);
        RemovePinned(hwnd);
        Wh_Log(L"Unpinned window %p", (void*)hwnd);
    }
    return true;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* seen = reinterpret_cast<std::vector<HWND>*>(lParam);
    seen->push_back(hwnd);

    if (hwnd == g_movingHwnd || !IsCandidate(hwnd))
        return TRUE;

    bool onPrimary = IsOnPrimaryMonitor(hwnd);
    ULONGLONG now = GetTickCount64();

    CachedWindow* entry = CacheFind(hwnd);
    if (entry) {
        if (entry->noView) {
            if (now < entry->retryAt)
                return TRUE;
        } else if (entry->onPrimary == onPrimary) {
            return TRUE;
        }
    }

    if (VdSyncWindow(hwnd, onPrimary))
        CacheStore(hwnd, onPrimary, false, 0);
    else
        CacheStore(hwnd, onPrimary, true, now + kNoViewRetryMs);

    return TRUE;
}

static void VdSyncAll() {
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
}

static void VdRequestSync() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        KillTimer(hwnd, TIMER_DEBOUNCE);
        SetTimer(hwnd, TIMER_DEBOUNCE, 200, nullptr);
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                                  DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd)
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

    if (event == EVENT_OBJECT_SHOW) {
        if (GetAncestor(hwnd, GA_ROOT) == hwnd)
            VdRequestSync();
    }
}

static void LoadSettings() {
    LONG poll = Wh_GetIntSetting(L"pollIntervalMs");
    if (poll < 100)
        poll = 100;
    if (poll > 10000)
        poll = 10000;
    g_pollIntervalMs = poll;

    g_extraExcluded.clear();
    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(L"excludedClasses[%d]", i);
        if (!*value) {
            Wh_FreeStringSetting(value);
            break;
        }
        g_extraExcluded.push_back(value);
        Wh_FreeStringSetting(value);
    }
}

static bool InitCom() {
    if (g_sp && g_pinnedApps && g_views)
        return true;

    if (!g_sp) {
        if (FAILED(CoCreateInstance(kCLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                                    IID_IServiceProvider, (void**)&g_sp))) {
            ReleaseCom();
            return false;
        }
    }

    if (!g_pinnedApps && FAILED(g_sp->QueryService(kCLSID_VirtualDesktopPinnedApps,
                                                   kIID_IVirtualDesktopPinnedApps,
                                                   (void**)&g_pinnedApps))) {
        ReleaseCom();
        return false;
    }

    if (!g_views &&
        FAILED(g_sp->QueryService(kIID_IApplicationViewCollection,
                                  kIID_IApplicationViewCollection, (void**)&g_views)) &&
        FAILED(g_sp->QueryService(kIID_IApplicationViewCollectionWin10,
                                  kIID_IApplicationViewCollectionWin10, (void**)&g_views))) {
        ReleaseCom();
        return false;
    }

    g_shellPid = GetShellPid();
    return true;
}

static void StopTimersAndHooks() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) {
        KillTimer(hwnd, TIMER_POLL);
        KillTimer(hwnd, TIMER_DEBOUNCE);
        KillTimer(hwnd, TIMER_COM_RETRY);
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

static void Start() {
    g_started = true;

    HWND hwnd = g_hwnd.load();
    SetTimer(hwnd, TIMER_POLL, (UINT)g_pollIntervalMs, nullptr);

    g_hookShow = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WinEventProc,
                                 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hookDestroy = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr,
                                    WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_hookMoveSize = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                     nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    Wh_Log(L"Started (poll %d ms)", g_pollIntervalMs);

    VdSyncAll();
}

static bool ShellAlive() {
    HWND shell = GetShellWindow();
    return shell && IsWindow(shell);
}

static void UnpinExcluded() {
    for (size_t i = g_wePinned.size(); i-- > 0;) {
        HWND hwnd = g_wePinned[i];
        WCHAR cls[256];
        if (!IsWindow(hwnd) || !GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
            IsExcludedClass(cls)) {
            if (IsWindow(hwnd))
                VdUnpin(hwnd);
            g_wePinned.erase(g_wePinned.begin() + i);
        }
    }
}

static void UnpinAll() {
    if (ShellAlive()) {
        for (HWND h : g_wePinned)
            VdUnpin(h);
    }
    g_wePinned.clear();
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_POLL || wParam == TIMER_DEBOUNCE) {
            if (wParam == TIMER_DEBOUNCE)
                KillTimer(hwnd, TIMER_DEBOUNCE);
            if (g_reconnectPending || GetShellPid() != g_shellPid) {
                g_reconnectPending = false;
                ReinitializeCom();
            }
            if (g_started)
                VdSyncAll();
        } else if (wParam == TIMER_COM_RETRY) {
            KillTimer(hwnd, TIMER_COM_RETRY);
            if (InitCom()) {
                Start();
            } else {
                SetTimer(hwnd, TIMER_COM_RETRY, kComRetryMs, nullptr);
            }
        }
        return 0;

    case WM_SMP_RELOAD:
        LoadSettings();
        UnpinExcluded();
        g_cache.clear();
        if (g_started && g_hwnd)
            SetTimer(g_hwnd.load(), TIMER_POLL, (UINT)g_pollIntervalMs, nullptr);
        return 0;

    case WM_SMP_QUIT:
        StopTimersAndHooks();
        UnpinAll();
        if (ShellAlive())
            ReleaseCom();
        else {
            g_sp = nullptr;
            g_pinnedApps = nullptr;
            g_views = nullptr;
        }
        g_cache.clear();
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static HMODULE GetCurrentModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WndProc, &module);
    return module;
}

static DWORD WINAPI ThreadProc(LPVOID) {
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool shouldUninitialize = SUCCEEDED(coHr);
    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", (unsigned)coHr);
        g_initOk = false;
        SetEvent(g_readyEvent);
        return 1;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetCurrentModule();
    wc.lpszClassName = L"SecondaryMonitorPinWindow";

    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        g_initOk = false;
        SetEvent(g_readyEvent);
        if (shouldUninitialize)
            CoUninitialize();
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_initOk = false;
        SetEvent(g_readyEvent);
        if (shouldUninitialize)
            CoUninitialize();
        return 1;
    }
    g_hwnd = hwnd;

    g_initOk = true;
    SetEvent(g_readyEvent);

    LoadSettings();

    if (InitCom())
        Start();
    else
        SetTimer(hwnd, TIMER_COM_RETRY, kComRetryMs, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (shouldUninitialize)
        CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);

    if (!g_initOk) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd)
        PostMessageW(hwnd, WM_SMP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    if (g_readyEvent)
        WaitForSingleObject(g_readyEvent, INFINITE);

    HWND hwnd = g_hwnd.load();
    if (hwnd)
        PostMessageW(hwnd, WM_SMP_QUIT, 0, 0);

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

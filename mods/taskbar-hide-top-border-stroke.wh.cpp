// ==WindhawkMod==
// @id              taskbar-hide-top-border-stroke
// @license         MIT
// @github          https://github.com/nayanct
// @name            Taskbar Hide Top Border
// @description     Removes the thin line above the Windows 11 taskbar
// @version         0.1
// @author          nayanct
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Hide Top Border

Removes the thin line at the top of the Windows 11 taskbar.

## Notes
- Windows 11 only.
- Only one mod that uses XAML diagnostics can run at a time — if you already
  have *Windows 11 Taskbar Styler* enabled, this won't attach.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <xamlom.h>

#include <Unknwn.h>
#include <combaseapi.h>
#include <ocidl.h>

#include <atomic>

// winbase.h defines GetCurrentTime as a macro that expands to GetTickCount(),
// which then corrupts the WinRT IInputElement::GetCurrentTime method signature
// pulled in by winrt/Windows.UI.Xaml.h. Undefine it before the winrt headers.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

// ----------------------------------------------------------------------------
// Win11 detection
// ----------------------------------------------------------------------------

bool IsWindows11OrLater() {
    HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
    if (!ntdll) {
        return false;
    }
    using RtlGetVersionFn = LONG(WINAPI*)(OSVERSIONINFOEXW*);
    auto fn = reinterpret_cast<RtlGetVersionFn>(
        GetProcAddress(ntdll, "RtlGetVersion"));
    if (!fn) {
        return false;
    }
    OSVERSIONINFOEXW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (fn(&info) != 0) {
        return false;
    }
    return info.dwBuildNumber >= 22000;
}

// ----------------------------------------------------------------------------
// Module handle (own DLL)
// ----------------------------------------------------------------------------

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
                           &module)) {
        return nullptr;
    }
    return module;
}

// ----------------------------------------------------------------------------
// VisualTreeWatcher: hides any FrameworkElement named "BackgroundStroke"
// ----------------------------------------------------------------------------

thread_local bool g_initializedForThread = false;
std::atomic<bool> g_initialized = false;

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);
    ~VisualTreeWatcher();

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    void UnadviseVisualTreeChange();

   private:
    HRESULT STDMETHODCALLTYPE
    OnVisualTreeChange(ParentChildRelation relation,
                       VisualElement element,
                       VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE
    OnElementStateChanged(InstanceHandle element,
                          VisualElementState elementState,
                          LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle) {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics;
};

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
    : m_XamlDiagnostics(site.as<IXamlDiagnostics>()) {
    Wh_Log(L"Constructing VisualTreeWatcher");

    // AdviseVisualTreeChange from the originating thread can hang in
    // Advising::RunOnUIThread. Call it from a worker thread instead.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto* watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()
                             ->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"AdviseVisualTreeChange failed: 0x%08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher() {
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange() {
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()
                     ->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed: 0x%08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation,
                                              VisualElement element,
                                              VisualMutationType mutationType) try {
    if (!g_initializedForThread) {
        return S_OK;
    }
    if (mutationType != Add) {
        return S_OK;
    }

    auto inspectable = FromHandle(element.Handle);
    auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
    if (!frameworkElement) {
        return S_OK;
    }

    if (frameworkElement.Name() == L"BackgroundStroke") {
        Wh_Log(L"Hiding BackgroundStroke (handle %llu)", element.Handle);
        frameworkElement.Visibility(wux::Visibility::Collapsed);
    }

    return S_OK;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"OnVisualTreeChange error: 0x%08X", hr);
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle,
                                                 VisualElementState,
                                                 LPCWSTR) noexcept {
    return S_OK;
}

// ----------------------------------------------------------------------------
// WindhawkTAP: COM object the XAML Diagnostics endpoint hands a site to
// ----------------------------------------------------------------------------

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {7B3F8A91-2C4D-4E5F-9A8B-1C2D3E4F5A6B}
static constexpr CLSID CLSID_WindhawkTAP = {
    0x7b3f8a91,
    0x2c4d,
    0x4e5f,
    {0x9a, 0x8b, 0x1c, 0x2d, 0x3e, 0x4f, 0x5a, 0x6b}};

class WindhawkTAP : public winrt::implements<WindhawkTAP,
                                             IObjectWithSite,
                                             winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override;

   private:
    winrt::com_ptr<IUnknown> m_site;
};

HRESULT WindhawkTAP::SetSite(IUnknown* pUnkSite) try {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    m_site.copy_from(pUnkSite);

    if (m_site) {
        // Balance the AddRef performed by InitializeXamlDiagnosticsEx on us.
        FreeLibrary(GetCurrentModuleHandle());
        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(m_site);
    }

    return S_OK;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"SetSite error: 0x%08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void** ppvSite) noexcept {
    return m_site.as(riid, ppvSite);
}

// ----------------------------------------------------------------------------
// Class factory + DllGetClassObject (queried by Windows.UI.Xaml.dll)
// ----------------------------------------------------------------------------

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter,
                                             REFIID riid,
                                             void** ppvObject) override try {
        if (pUnkOuter) {
            return CLASS_E_NOAGGREGATION;
        }
        *ppvObject = nullptr;
        return winrt::make<T>().as(riid, ppvObject);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_ STDAPI
    DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try {
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow(void) {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

// ----------------------------------------------------------------------------
// Inject our TAP into the current process via InitializeXamlDiagnosticsEx
// ----------------------------------------------------------------------------

bool g_inInjectWindhawkTAP = false;

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept {
    HMODULE selfModule = GetCurrentModuleHandle();
    if (!selfModule) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    DWORD len = GetModuleFileName(selfModule, location, ARRAYSIZE(location));
    if (len == 0 || len == ARRAYSIZE(location)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE wux =
        LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wux) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    g_inInjectWindhawkTAP = true;

    // The connection-name slot may be taken; try a handful of suffixes.
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[64];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);
        hr = ixde(connectionName, GetCurrentProcessId(), L"", location,
                  CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    g_inInjectWindhawkTAP = false;
    return hr;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }
    g_initializedForThread = true;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }
    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"InjectWindhawkTAP failed: 0x%08X", hr);
    }
}

// ----------------------------------------------------------------------------
// Run a callback on a window's UI thread (the only thread allowed to touch
// that thread's XAML objects)
// ----------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// ----------------------------------------------------------------------------
// Locate the taskbar's XAML host window
// ----------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

HWND GetTaskbarUiWnd() {
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar) {
        return nullptr;
    }
    return FindWindowEx(taskbar, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

// ----------------------------------------------------------------------------
// CreateWindowExW hook: catch the taskbar's XAML host window being (re)created
// after explorer restarts the taskbar, so we attach again automatically
// ----------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !hWndParent) {
        return hWnd;
    }

    WCHAR className[64];
    if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar DesktopWindowContentBridge created; initializing");
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
    }

    return hWnd;
}

// ----------------------------------------------------------------------------
// Windhawk entry points
// ----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    if (!IsWindows11OrLater()) {
        Wh_Log(L"Not Windows 11; mod will load but do nothing");
        return TRUE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    if (!IsWindows11OrLater()) {
        return;
    }

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Found existing taskbar UI window; initializing");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        InitializeSettingsAndTap();
    } else {
        Wh_Log(L"Taskbar UI window not yet present; waiting for creation");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }
    g_initialized = false;
}

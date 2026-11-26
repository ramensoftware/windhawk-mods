// ==WindhawkMod==
// @id              explorer-default-folder
// @name            Explorer Default Folder
// @description     Open new File Explorer windows and tabs in a folder of your choice instead of This PC or Home
// @version         1.0.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @donateUrl       https://buymeacoffee.com/widget/page/danrotaru
// @homepage        https://dan13.me/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Default Folder

Opens every new File Explorer **window or tab** in a folder of your choice
when it would otherwise open **This PC**, **Home** or **Quick access**.

Enter a folder in the mod settings, for example `D:\Documents`,
`%USERPROFILE%\Downloads`, `~\Downloads`, `Downloads` or `shell:Downloads`.
Environment variables are expanded, and `~` or a relative path starts at your
user folder. Leave the setting empty to keep Explorer's normal behavior.

You can apply the folder to new windows only, new tabs only, or both, and
optionally reopen the folder you closed most recently instead.

Windows and tabs opened for any other folder are not changed, and navigating
to This PC or Home yourself in an existing tab is not redirected. A new window
or tab that is explicitly opened at This PC or Home (for example with the This
PC desktop icon) can't be told apart from a default one, so it is redirected
too.

## How it works

The mod listens to Shell window events instead of polling, so it costs
nothing while idle and redirects a new tab as soon as its first location
loads. Its background thread never holds up Explorer's windows, even when the
folder is on a slow or offline network share.

## Compatibility

Designed for File Explorer on Windows 10 and 11. It uses documented Shell COM
interfaces rather than version-specific Explorer symbols, so it does not need
an update for each Explorer build.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultFolder: ""
  $name: Default folder
  $description: >-
    Folder opened in new Explorer windows and tabs. Accepts normal paths,
    environment variables (%USERPROFILE%\Downloads), paths relative to your
    user folder (~\Downloads or just Downloads) and shell locations
    (shell:Downloads). Leave empty to keep Explorer's default behavior.
- applyTo: both
  $name: Apply to
  $description: >-
    Which new Explorer instances are redirected, to the default folder or to
    the last closed folder.
  $options:
  - both: Both (new window and new tab)
  - window: New window
  - tab: New tab
- reopenLastClosed: false
  $name: Reopen last closed folder (ignores Default folder)
  $description: >-
    Open the folder of the most recently closed Explorer window or tab instead
    of the Default folder. The Default folder is only used until a folder has
    been closed, or if the last closed folder no longer exists.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <exdisp.h>
#include <exdispid.h>
#include <shlobj.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

constexpr UINT kMsgScan = WM_APP;              // ShellWindows changed.
constexpr UINT kMsgNavigated = WM_APP + 1;     // wParam: tab id.
constexpr UINT kMsgSettingsChanged = WM_APP + 2;
constexpr DWORD kRecheckMs = 100;              // Fallback while a new tab loads.
constexpr DWORD kConnectRetryMs = 500;         // While the shell isn't ready.
constexpr ULONGLONG kGiveUpMs = 10000;
constexpr PCWSTR kLastClosedValue = L"lastClosedFolder";
// Longest last closed folder that is remembered. Wh_GetStringValue returns an
// empty string, not a truncated one, for values that don't fit the buffer, so
// longer names are never stored.
constexpr size_t kMaxStoredName = 4096;

enum class ApplyTo { Both, Window, Tab };

struct Settings {
    std::wstring folder;
    ApplyTo applyTo = ApplyTo::Both;
    bool reopenLastClosed = false;

    // Nothing to do (no subscriptions at all) unless one of these is set.
    bool Active() const { return !folder.empty() || reopenLastClosed; }
};

HANDLE g_stopEvent;
HANDLE g_thread;
std::atomic<DWORD> g_workerThreadId;
std::atomic<bool> g_scanQueued;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
Settings g_settings;

std::wstring ExpandFolder(std::wstring value) {
    size_t first = value.find_first_not_of(L" \t\"");
    size_t last = value.find_last_not_of(L" \t\"");
    value = first == std::wstring::npos ? L"" : value.substr(first, last - first + 1);
    if (value.empty()) return value;

    // "~", "~\x" and relative paths such as "Downloads" start at the user
    // folder; absolute, UNC, %VAR%, shell: and ::{CLSID} paths are kept.
    if (value[0] == L'~' && (value.size() == 1 || value[1] == L'\\' || value[1] == L'/')) {
        value.replace(0, 1, L"%USERPROFILE%");
    } else if (value[0] != L'%' && value[0] != L'\\' && value[0] != L'/' &&
               value.find(L':') == std::wstring::npos) {
        value.insert(0, L"%USERPROFILE%\\");
    }

    DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (size) {
        std::wstring expanded(size, L'\0');
        DWORD len = ExpandEnvironmentStringsW(value.c_str(), expanded.data(), size);
        if (len && len <= size) {
            expanded.resize(len - 1);
            value = std::move(expanded);
        }
    }
    return value;
}

void LoadSettings() {
    Settings s;
    s.folder = ExpandFolder(WindhawkUtils::StringSetting::make(L"defaultFolder").get());
    auto applyTo = WindhawkUtils::StringSetting::make(L"applyTo");
    s.applyTo = wcscmp(applyTo, L"window") == 0 ? ApplyTo::Window
                : wcscmp(applyTo, L"tab") == 0  ? ApplyTo::Tab
                                                : ApplyTo::Both;
    s.reopenLastClosed = Wh_GetIntSetting(L"reopenLastClosed");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

// Event sink for ShellWindows (tabId == 0) or one tab's browser. The worker
// thread is in the multithreaded apartment, so COM delivers the events on its
// own MTA threads, never on the worker. Invoke only posts a message to the
// worker and returns, so the Explorer thread raising the event is released
// right away, even while the worker is busy (for example resolving a folder
// on a slow network share).
struct EventSink : IDispatch {
    std::atomic<ULONG> refs{1};
    UINT_PTR tabId;
    explicit EventSink(UINT_PTR tabId) : tabId(tabId) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch) ||
            riid == (tabId ? DIID_DWebBrowserEvents2 : DIID_DShellWindowsEvents)) {
            *ppv = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs; }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG n = --refs;
        if (!n) delete this;
        return n;
    }
    STDMETHODIMP GetTypeInfoCount(UINT* n) override { *n = 0; return S_OK; }
    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) override { return E_NOTIMPL; }
    STDMETHODIMP GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) override { return E_NOTIMPL; }
    STDMETHODIMP Invoke(DISPID id, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) override {
        DWORD worker = g_workerThreadId;
        if (!tabId) {
            if ((id == DISPID_WINDOWREGISTERED || id == DISPID_WINDOWREVOKED) && !g_scanQueued.exchange(true)) {
                if (!PostThreadMessageW(worker, kMsgScan, 0, 0)) g_scanQueued = false;
            }
        } else if (id == DISPID_NAVIGATECOMPLETE2 || id == DISPID_DOCUMENTCOMPLETE) {
            PostThreadMessageW(worker, kMsgNavigated, tabId, 0);
        }
        return S_OK;
    }
};

ComPtr<IConnectionPoint> Advise(IUnknown* source, UINT_PTR tabId, DWORD* cookie) {
    ComPtr<IConnectionPointContainer> container;
    ComPtr<IConnectionPoint> cp;
    auto sink = new EventSink(tabId);
    bool ok = SUCCEEDED(source->QueryInterface(IID_PPV_ARGS(&container))) &&
              SUCCEEDED(container->FindConnectionPoint(
                  tabId ? DIID_DWebBrowserEvents2 : DIID_DShellWindowsEvents, &cp)) &&
              SUCCEEDED(cp->Advise(sink, cookie));
    sink->Release();  // The connection point holds it from now on.
    return ok ? cp : nullptr;
}

// Parsing name of a tab's current folder (C:\Folder, ::{CLSID}, ...), or an
// empty string while it has no view yet. Uses the shell's scripting objects
// (Document.Folder.Self.Path), which are automation interfaces and can be
// called from the worker's apartment.
std::wstring GetLocation(IWebBrowser2* browser) {
    ComPtr<IDispatch> document;
    ComPtr<IShellFolderViewDual> view;
    ComPtr<Folder> folder;
    ComPtr<Folder2> folder2;
    ComPtr<FolderItem> self;
    BSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(browser->get_Document(&document)) && document && SUCCEEDED(document.As(&view)) &&
        SUCCEEDED(view->get_Folder(&folder)) && folder && SUCCEEDED(folder.As(&folder2)) &&
        SUCCEEDED(folder2->get_Self(&self)) && self && SUCCEEDED(self->get_Path(&path)) && path) {
        result = path;
    }
    SysFreeString(path);
    return result;
}

// Parses a folder to a PIDL; fails if it doesn't exist.
PIDLIST_ABSOLUTE ParseFolder(const std::wstring& folder) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (folder.empty() || FAILED(SHParseDisplayName(folder.c_str(), nullptr, &pidl, 0, nullptr))) return nullptr;
    return pidl;
}

// Navigating by PIDL works for file system paths and shell: locations.
bool Navigate(IWebBrowser2* browser, const std::wstring& folder) {
    PIDLIST_ABSOLUTE target = ParseFolder(folder);
    if (!target) return false;
    UINT size = ILGetSize(target);
    VARIANT where = {};
    where.vt = VT_ARRAY | VT_UI1;
    where.parray = SafeArrayCreateVector(VT_UI1, 0, size);
    HRESULT hr = E_OUTOFMEMORY;
    if (where.parray) {
        memcpy(where.parray->pvData, target, size);
        VARIANT empty = {};
        hr = browser->Navigate2(&where, &empty, &empty, &empty, &empty);
        VariantClear(&where);
    }
    CoTaskMemFree(target);
    return SUCCEEDED(hr);
}

bool IsDisconnected(HRESULT hr) {
    return hr == RPC_E_DISCONNECTED || hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) ||
           hr == HRESULT_FROM_WIN32(RPC_S_CALL_FAILED) || hr == CO_E_OBJNOTCONNECTED;
}

struct Entry {
    UINT_PTR id = 0;               // Passed to the tab's event sink.
    ComPtr<IUnknown> identity;     // Tells tabs apart across scans.
    HWND root = nullptr;           // Top-level Explorer window owning the tab.
    bool pending = false;          // New tab whose first location isn't known yet.
    bool isTab = false;            // Opened in an existing window.
    ULONGLONG created = 0;
    ULONGLONG lastNavigated = 0;
    std::wstring folder;           // Last folder shown, except start pages.
    ComPtr<IWebBrowser2> browser;
    ComPtr<IConnectionPoint> connection;  // Set while subscribed to the tab.
    DWORD cookie = 0;
};

// All state of the worker thread. It's a local of the worker, so the COM
// proxies it holds are released on that thread, before CoUninitialize, and
// never from a global destructor at process exit.
class Worker {
   public:
    Worker() { LoadLastClosed(); }

    ~Worker() { Disconnect(); }

    void Run() {
        bool active = GetSettings().Active();
        for (;;) {
            if (!m_shellWindows) {
                if (active && Connect()) continue;
                // Wait for Explorer, or for the settings to enable the mod.
                DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, active ? kConnectRetryMs : INFINITE,
                                                         QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) return;
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (msg.hwnd) DispatchMessageW(&msg);
                    else if (msg.message == kMsgSettingsChanged) active = GetSettings().Active();
                }
                continue;
            }

            bool pending = std::any_of(m_entries.begin(), m_entries.end(),
                                       [](const Entry& e) { return e.pending; });
            HANDLE handles[] = {g_stopEvent, m_explorerProcess};
            DWORD wait = MsgWaitForMultipleObjectsEx(ARRAYSIZE(handles), handles, pending ? kRecheckMs : INFINITE,
                                                     QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) return;
            if (wait == WAIT_OBJECT_0 + 1) {
                Wh_Log(L"The shell exited, waiting for it to restart");
                Disconnect();
                continue;
            }
            if (wait == WAIT_TIMEOUT) {
                for (Entry& e : m_entries) {
                    if (e.pending) Update(e);
                }
                continue;
            }

            bool connected = true;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.hwnd) {
                    DispatchMessageW(&msg);
                } else if (msg.message == kMsgScan) {
                    g_scanQueued = false;
                    if (connected) connected = Scan(false);
                } else if (msg.message == kMsgNavigated) {
                    for (Entry& e : m_entries) {
                        if (e.id == msg.wParam) Update(e);
                    }
                } else if (msg.message == kMsgSettingsChanged) {
                    active = GetSettings().Active();
                    if (active) ApplyReopenSetting();
                }
            }
            if (!connected) {
                Wh_Log(L"Lost the connection to the shell, reconnecting");
                Disconnect();
            } else if (!active) {
                Disconnect();
            }
        }
    }

   private:
    // Subscribes to ShellWindows of the running shell. Waits until the shell
    // has registered it: creating it earlier would launch a separate rundll32
    // server instead. In a separate folder process, the shell process can
    // restart on its own, so its process handle is watched too.
    bool Connect() {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD pid = 0;
        if (!tray || !GetWindowThreadProcessId(tray, &pid)) return false;
        m_explorerProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
        if (m_explorerProcess &&
            SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&m_shellWindows))) &&
            (m_connection = Advise(m_shellWindows.Get(), 0, &m_cookie))) {
            g_scanQueued = false;
            if (Scan(true)) {
                Wh_Log(L"Connected to the shell (pid %u)", pid);
                return true;
            }
        }
        Disconnect();
        return false;
    }

    void Disconnect() {
        for (Entry& e : m_entries) Unadvise(e);
        m_entries.clear();
        if (m_connection) m_connection->Unadvise(m_cookie);
        m_connection.Reset();
        m_shellWindows.Reset();
        if (m_explorerProcess) CloseHandle(m_explorerProcess);
        m_explorerProcess = nullptr;
    }

    // The stored value is shared by all explorer.exe processes (with "Launch
    // folder windows in a separate process"), so it's read again before use.
    void LoadLastClosed() {
        std::wstring stored(kMaxStoredName + 1, L'\0');
        size_t len = Wh_GetStringValue(kLastClosedValue, stored.data(), stored.size());
        stored.resize(len ? wcslen(stored.c_str()) : 0);
        m_lastClosed = std::move(stored);
    }

    // This PC, Home (Windows 11) and Quick access (Windows 10).
    static bool IsStartPage(const std::wstring& location) {
        static constexpr PCWSTR kStartPages[] = {
            L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}",
            L"::{F874310E-B6B7-47DC-BC84-B9E6B38F5903}",
            L"::{679F85CB-0220-4080-B29B-5540CC05AAB6}",
        };
        for (PCWSTR page : kStartPages) {
            if (_wcsicmp(location.c_str(), page) == 0) return true;
        }
        return false;
    }

    static void Unadvise(Entry& e) {
        if (e.connection) e.connection->Unadvise(e.cookie);
        e.connection.Reset();
    }

    // Tabs are only subscribed to while they're pending, or all the time when
    // the last closed folder is needed.
    void Subscribe(Entry& e) {
        if (!e.connection && e.browser) e.connection = Advise(e.browser.Get(), e.id, &e.cookie);
    }

    void ApplyReopenSetting() {
        bool reopen = GetSettings().reopenLastClosed;
        for (Entry& e : m_entries) {
            if (reopen && !e.connection) {
                Subscribe(e);
                Update(e);
            } else if (!reopen && !e.pending) {
                e.folder.clear();
                Unadvise(e);
            }
        }
    }

    void Redirect(const Entry& e) {
        Settings s = GetSettings();
        if (s.applyTo == (e.isTab ? ApplyTo::Window : ApplyTo::Tab)) return;
        if (s.reopenLastClosed) LoadLastClosed();
        PCWSTR kind = e.isTab ? L"tab" : L"window";
        if (s.reopenLastClosed && Navigate(e.browser.Get(), m_lastClosed)) {
            Wh_Log(L"New %s -> last closed folder %s", kind, m_lastClosed.c_str());
        } else if (Navigate(e.browser.Get(), s.folder)) {
            Wh_Log(L"New %s -> %s", kind, s.folder.c_str());
        } else if (!s.folder.empty()) {
            Wh_Log(L"Default folder not found: %s", s.folder.c_str());
        }
    }

    // Records a tab's current folder; a new tab's first location decides
    // whether it gets redirected.
    void Update(Entry& e) {
        if (!e.browser) return;
        std::wstring location = GetLocation(e.browser.Get());
        if (!location.empty()) {
            bool startPage = IsStartPage(location);
            if (e.pending) {
                e.pending = false;
                if (startPage) Redirect(e);
            }
            if (!startPage && location.size() <= kMaxStoredName) {
                e.folder = std::move(location);
                e.lastNavigated = GetTickCount64();
            }
        } else if (e.pending && GetTickCount64() - e.created > kGiveUpMs) {
            e.pending = false;
        }
        if (!e.pending && !GetSettings().reopenLastClosed) Unadvise(e);
    }

    // Syncs the tracked tabs with ShellWindows. New tabs become pending,
    // unless they were already open when the mod connected. Returns false if
    // Explorer is gone.
    bool Scan(bool initial) {
        long count = 0;
        HRESULT hr = m_shellWindows->get_Count(&count);
        if (FAILED(hr)) {
            Wh_Log(L"ShellWindows::get_Count failed: 0x%08X", (unsigned)hr);
            return false;
        }

        bool reopen = GetSettings().reopenLastClosed;
        std::vector<Entry> entries;
        entries.reserve(count);
        for (long i = 0; i < count; ++i) {
            VARIANT index = {};
            index.vt = VT_I4;
            index.lVal = i;
            ComPtr<IDispatch> item;
            ComPtr<IWebBrowser2> browser;
            ComPtr<IUnknown> identity;
            SHANDLE_PTR frame = 0;
            hr = m_shellWindows->Item(index, &item);
            if (IsDisconnected(hr)) return false;
            if (FAILED(hr) || !item || FAILED(item.As(&browser)) || FAILED(item.As(&identity))) {
                continue;
            }

            auto same = [&identity](const Entry& e) { return e.identity.Get() == identity.Get(); };
            auto old = std::find_if(m_entries.begin(), m_entries.end(), same);
            if (old != m_entries.end()) {
                entries.push_back(std::move(*old));
                m_entries.erase(old);
                continue;
            }
            if (std::any_of(entries.begin(), entries.end(), same)) continue;
            browser->get_HWND(&frame);

            Entry e;
            e.id = ++m_nextId;
            e.identity = std::move(identity);
            e.root = GetAncestor((HWND)frame, GA_ROOT);
            e.pending = !initial;
            auto sameRoot = [&e](const Entry& o) { return e.root && o.root == e.root; };
            e.isTab = std::any_of(entries.begin(), entries.end(), sameRoot) ||
                      std::any_of(m_entries.begin(), m_entries.end(), sameRoot);
            e.created = GetTickCount64();
            DWORD pid = 0;
            GetWindowThreadProcessId(e.root, &pid);
            if (pid != GetCurrentProcessId()) {
                // Tabs of another explorer.exe process are handled by that
                // process's copy of the mod; they're only kept to tell new
                // windows from new tabs.
                e.pending = false;
            } else {
                e.browser = std::move(browser);
            }
            if (e.browser && (e.pending || reopen)) {
                // Subscribe before reading the location, so the first
                // navigation can't slip in between.
                Subscribe(e);
                Update(e);
            }
            entries.push_back(std::move(e));
        }

        // Closed tabs: remember the most recently used folder among them.
        const Entry* closed = nullptr;
        for (Entry& e : m_entries) {
            if (e.connection && !e.folder.empty() && (!closed || e.lastNavigated > closed->lastNavigated)) {
                closed = &e;
            }
            Unadvise(e);
        }
        if (closed) {  // Always written: another Explorer process may have changed it.
            m_lastClosed = closed->folder;
            Wh_SetStringValue(kLastClosedValue, m_lastClosed.c_str());
        }
        m_entries = std::move(entries);
        return true;
    }

    UINT_PTR m_nextId = 0;
    std::wstring m_lastClosed;
    HANDLE m_explorerProcess = nullptr;
    ComPtr<IShellWindows> m_shellWindows;
    ComPtr<IConnectionPoint> m_connection;
    DWORD m_cookie = 0;
    std::vector<Entry> m_entries;
};

DWORD WINAPI WorkerThread(void*) {
    // Create the message queue before any event sink can post to it.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId = GetCurrentThreadId();

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE))) {
        return 0;
    }
    {
        Worker worker;
        worker.Run();
    }
    CoUninitialize();
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = g_stopEvent ? CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr) : nullptr;
    return g_thread != nullptr;
}

void Wh_ModUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (DWORD worker = g_workerThreadId) PostThreadMessageW(worker, kMsgSettingsChanged, 0, 0);
}

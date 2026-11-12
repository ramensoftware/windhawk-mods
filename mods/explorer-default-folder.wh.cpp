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
// @compilerOptions -lole32 -loleaut32 -lshell32
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

Explorer windows opened for an explicit folder are not changed, and navigating
to This PC or Home yourself in an existing tab is not redirected.

## How it works

The mod listens to Shell window events instead of polling, so it costs nothing
while idle and redirects a new tab as soon as its first location loads.

## Compatibility

Designed for 64-bit Windows 10 and 11 File Explorer. It uses documented Shell
COM interfaces rather than version-specific Explorer symbols, so it does not
need an update for each Explorer build.
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

#include <windows.h>

#include <exdisp.h>
#include <exdispid.h>
#include <shlobj.h>
#include <wrl/client.h>

#include <algorithm>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

constexpr GUID kCLSID_ShellWindows = {0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr GUID kDIID_DShellWindowsEvents = {0xfe4106e0, 0x399a, 0x11d0, {0xa4, 0x8c, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr GUID kDIID_DWebBrowserEvents2 = {0x34a715a0, 0x6587, 0x11d0, {0x92, 0x4a, 0x00, 0x20, 0xaf, 0xc7, 0xac, 0x4d}};
constexpr GUID kSID_STopLevelBrowser = {0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

constexpr UINT kMsgScan = WM_APP;          // ShellWindows changed.
constexpr UINT kMsgNavigated = WM_APP + 1;  // wParam: tab HWND.
constexpr DWORD kRecheckMs = 100;           // Fallback while a new tab loads.
constexpr ULONGLONG kGiveUpMs = 10000;
constexpr PCWSTR kLastClosedValue = L"lastClosedFolder";

enum class ApplyTo { Both, Window, Tab };

HANDLE g_stopEvent;
HANDLE g_thread;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
struct {
    std::wstring folder;
    ApplyTo applyTo;
    bool reopenLastClosed;
} g_settings;

// Everything below is used only by the worker thread.
struct Entry {
    HWND hwnd;
    HWND root;                // Top-level Explorer window owning the tab.
    bool pending;             // New tab whose first location isn't known yet.
    bool isTab;               // Opened in an existing window.
    ULONGLONG created;
    ULONGLONG lastNavigated;
    std::wstring folder;      // Last folder shown, except start pages.
    ComPtr<IWebBrowser2> browser;  // Set for tabs of this process only.
    ComPtr<IShellBrowser> shellBrowser;
    ComPtr<IConnectionPoint> connection;
    DWORD cookie;
};
std::vector<Entry> g_entries;
PIDLIST_ABSOLUTE g_thisPc, g_home, g_quickAccess;
std::wstring g_lastClosed;
bool g_scanQueued;

void LoadSettings() {
    PCWSTR raw = Wh_GetStringSetting(L"defaultFolder");
    std::wstring value = raw;
    Wh_FreeStringSetting(raw);

    size_t first = value.find_first_not_of(L" \t\"");
    size_t last = value.find_last_not_of(L" \t\"");
    value = first == std::wstring::npos ? L"" : value.substr(first, last - first + 1);
    if (!value.empty()) {
        // "~", "~\x" and relative paths such as "Downloads" start at the user
        // folder; absolute, UNC, %VAR%, shell: and ::{CLSID} paths are kept.
        if (value[0] == L'~' && (value.size() == 1 || value[1] == L'\\' || value[1] == L'/')) {
            value.replace(0, 1, L"%USERPROFILE%");
        } else if (value[0] != L'%' && value[0] != L'\\' && value[0] != L'/' &&
                   value.find(L':') == std::wstring::npos) {
            value.insert(0, L"%USERPROFILE%\\");
        }
        wchar_t expanded[MAX_PATH * 2];
        DWORD len = ExpandEnvironmentStringsW(value.c_str(), expanded, ARRAYSIZE(expanded));
        if (len && len <= ARRAYSIZE(expanded)) value = expanded;
    }

    PCWSTR applyTo = Wh_GetStringSetting(L"applyTo");
    ApplyTo mode = wcscmp(applyTo, L"window") == 0 ? ApplyTo::Window
                   : wcscmp(applyTo, L"tab") == 0  ? ApplyTo::Tab
                                                   : ApplyTo::Both;
    Wh_FreeStringSetting(applyTo);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings.folder = std::move(value);
    g_settings.applyTo = mode;
    g_settings.reopenLastClosed = Wh_GetIntSetting(L"reopenLastClosed");
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// Event sink for ShellWindows (hwnd == null) or one tab's browser. COM calls
// it on the worker thread (STA); it only posts a message, so Explorer's UI
// thread is never held up and navigation never happens re-entrantly.
struct EventSink : IDispatch {
    LONG refs = 1;
    HWND hwnd;
    explicit EventSink(HWND hwnd) : hwnd(hwnd) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch) ||
            riid == (hwnd ? kDIID_DWebBrowserEvents2 : kDIID_DShellWindowsEvents)) {
            *ppv = this;
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
        if (!hwnd) {
            if ((id == DISPID_WINDOWREGISTERED || id == DISPID_WINDOWREVOKED) && !g_scanQueued) {
                g_scanQueued = PostThreadMessageW(GetCurrentThreadId(), kMsgScan, 0, 0);
            }
        } else if (id == DISPID_NAVIGATECOMPLETE2 || id == DISPID_DOCUMENTCOMPLETE) {
            PostThreadMessageW(GetCurrentThreadId(), kMsgNavigated, (WPARAM)hwnd, 0);
        }
        return S_OK;
    }
};

ComPtr<IConnectionPoint> Advise(IUnknown* source, HWND hwnd, DWORD* cookie) {
    ComPtr<IConnectionPointContainer> container;
    ComPtr<IConnectionPoint> cp;
    auto sink = new EventSink(hwnd);
    bool ok = SUCCEEDED(source->QueryInterface(IID_PPV_ARGS(&container))) &&
              SUCCEEDED(container->FindConnectionPoint(
                  hwnd ? kDIID_DWebBrowserEvents2 : kDIID_DShellWindowsEvents, &cp)) &&
              SUCCEEDED(cp->Advise(sink, cookie));
    sink->Release();  // The connection point holds it from now on.
    return ok ? cp : nullptr;
}

bool IsStartPage(PCIDLIST_ABSOLUTE pidl) {
    for (PCIDLIST_ABSOLUTE page : {g_thisPc, g_home, g_quickAccess}) {
        if (page && ILIsEqual(pidl, page)) return true;
    }
    return false;
}

// Current folder of a tab, or null while it has no view yet.
PIDLIST_ABSOLUTE GetLocation(IShellBrowser* shellBrowser) {
    ComPtr<IShellView> view;
    ComPtr<IFolderView> folderView;
    ComPtr<IPersistFolder2> folder;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(shellBrowser->QueryActiveShellView(&view)) && view &&
        ((SUCCEEDED(view.As(&folderView)) &&
          SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) ||
         SUCCEEDED(view.As(&folder)))) {
        folder->GetCurFolder(&pidl);
    }
    return pidl;
}

// Navigating by PIDL works for file system paths and shell: locations.
bool Navigate(IWebBrowser2* browser, const std::wstring& folder) {
    PIDLIST_ABSOLUTE target = nullptr;
    if (folder.empty() || FAILED(SHParseDisplayName(folder.c_str(), nullptr, &target, 0, nullptr))) {
        return false;
    }
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

void Redirect(const Entry& e) {
    AcquireSRWLockShared(&g_settingsLock);
    std::wstring folder = g_settings.folder;
    ApplyTo applyTo = g_settings.applyTo;
    bool reopen = g_settings.reopenLastClosed;
    ReleaseSRWLockShared(&g_settingsLock);

    if (applyTo == (e.isTab ? ApplyTo::Window : ApplyTo::Tab)) return;
    PCWSTR kind = e.isTab ? L"tab" : L"window";
    if (reopen && Navigate(e.browser.Get(), g_lastClosed)) {
        Wh_Log(L"New %s -> last closed folder %s", kind, g_lastClosed.c_str());
    } else if (Navigate(e.browser.Get(), folder)) {
        Wh_Log(L"New %s -> %s", kind, folder.c_str());
    } else if (!folder.empty()) {
        Wh_Log(L"Default folder not found: %s", folder.c_str());
    }
}

// Records a tab's current folder; a new tab's first location decides whether
// it gets redirected.
void Update(Entry& e) {
    PIDLIST_ABSOLUTE location = GetLocation(e.shellBrowser.Get());
    if (!location) {
        if (e.pending && GetTickCount64() - e.created > kGiveUpMs) e.pending = false;
        return;
    }
    bool startPage = IsStartPage(location);
    if (e.pending) {
        e.pending = false;
        if (startPage) Redirect(e);
    }
    PWSTR name;
    if (!startPage && SUCCEEDED(SHGetNameFromIDList(location, SIGDN_DESKTOPABSOLUTEPARSING, &name))) {
        e.folder = name;
        e.lastNavigated = GetTickCount64();
        CoTaskMemFree(name);
    }
    CoTaskMemFree(location);
}

void Unadvise(Entry& e) {
    if (e.connection) e.connection->Unadvise(e.cookie);
}

// Syncs the tracked tabs with ShellWindows. New tabs become pending, unless
// they were already open when the mod started.
void Scan(IShellWindows* shellWindows, bool initial) {
    long count = 0;
    shellWindows->get_Count(&count);

    std::vector<Entry> entries;
    entries.reserve(count);
    for (long i = 0; i < count; ++i) {
        VARIANT index = {};
        index.vt = VT_I4;
        index.lVal = i;
        ComPtr<IDispatch> item;
        ComPtr<IServiceProvider> provider;
        ComPtr<IShellBrowser> shellBrowser;
        HWND hwnd = nullptr;
        if (FAILED(shellWindows->Item(index, &item)) || !item ||
            FAILED(item.As(&provider)) ||
            FAILED(provider->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&shellBrowser))) ||
            FAILED(shellBrowser->GetWindow(&hwnd)) || !hwnd) {
            continue;
        }

        auto byHwnd = [hwnd](const Entry& e) { return e.hwnd == hwnd; };
        auto old = std::find_if(g_entries.begin(), g_entries.end(), byHwnd);
        if (old != g_entries.end()) {
            entries.push_back(std::move(*old));
            g_entries.erase(old);
            continue;
        }
        if (std::any_of(entries.begin(), entries.end(), byHwnd)) continue;

        Entry e{hwnd, GetAncestor(hwnd, GA_ROOT), !initial};
        auto sameRoot = [&e](const Entry& o) { return o.root == e.root; };
        e.isTab = std::any_of(entries.begin(), entries.end(), sameRoot) ||
                  std::any_of(g_entries.begin(), g_entries.end(), sameRoot);
        e.created = GetTickCount64();
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        // Tabs of other explorer.exe processes are handled by their own copy
        // of the mod; they are only kept to tell new windows from new tabs.
        if (pid == GetCurrentProcessId() && SUCCEEDED(item.As(&e.browser))) {
            e.shellBrowser = std::move(shellBrowser);
            // Subscribe before reading the location, so the first navigation
            // can't slip in between.
            e.connection = Advise(e.browser.Get(), hwnd, &e.cookie);
            Update(e);
        } else {
            e.pending = false;
        }
        entries.push_back(std::move(e));
    }

    // Closed tabs: remember the most recently used folder among them.
    const Entry* closed = nullptr;
    for (Entry& e : g_entries) {
        Unadvise(e);
        if (!e.folder.empty() && (!closed || e.lastNavigated > closed->lastNavigated)) closed = &e;
    }
    if (closed && closed->folder != g_lastClosed) {
        g_lastClosed = closed->folder;
        Wh_SetStringValue(kLastClosedValue, g_lastClosed.c_str());
    }
    g_entries = std::move(entries);
}

DWORD WINAPI WorkerThread(void*) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
        return 0;
    }

    SHGetFolderLocation(nullptr, CSIDL_DRIVES, nullptr, 0, &g_thisPc);
    SHParseDisplayName(L"shell:::{f874310e-b6b7-47dc-bc84-b9e6b38f5903}", nullptr, &g_home, 0, nullptr);
    SHParseDisplayName(L"shell:::{679f85cb-0220-4080-b29b-5540cc05aab6}", nullptr, &g_quickAccess, 0, nullptr);

    wchar_t lastClosed[MAX_PATH * 2];
    if (Wh_GetStringValue(kLastClosedValue, lastClosed, ARRAYSIZE(lastClosed))) {
        g_lastClosed = lastClosed;
    }

    // Wait until the shell has registered ShellWindows: creating it earlier
    // would launch a separate rundll32 server instead.
    ComPtr<IShellWindows> shellWindows;
    ComPtr<IConnectionPoint> connection;
    DWORD cookie = 0;
    while (g_thisPc && WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        if (FindWindowW(L"Shell_TrayWnd", nullptr) &&
            SUCCEEDED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                       IID_PPV_ARGS(&shellWindows))) &&
            (connection = Advise(shellWindows.Get(), nullptr, &cookie))) {
            break;
        }
        shellWindows.Reset();
        WaitForSingleObject(g_stopEvent, 500);
    }

    if (connection) {
        Scan(shellWindows.Get(), true);
        for (;;) {
            bool pending = std::any_of(g_entries.begin(), g_entries.end(),
                                       [](const Entry& e) { return e.pending; });
            DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, pending ? kRecheckMs : INFINITE,
                                                     QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_TIMEOUT) {
                for (Entry& e : g_entries) {
                    if (e.pending) Update(e);
                }
                continue;
            }
            if (wait != WAIT_OBJECT_0 + 1) break;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.hwnd) {
                    DispatchMessageW(&msg);
                } else if (msg.message == kMsgScan) {
                    g_scanQueued = false;
                    Scan(shellWindows.Get(), false);
                } else if (msg.message == kMsgNavigated) {
                    for (Entry& e : g_entries) {
                        if (e.hwnd == (HWND)msg.wParam && e.browser) Update(e);
                    }
                }
            }
        }
        for (Entry& e : g_entries) Unadvise(e);
        g_entries.clear();
        connection->Unadvise(cookie);
        connection.Reset();
    }

    shellWindows.Reset();
    CoTaskMemFree(g_thisPc);
    CoTaskMemFree(g_home);
    CoTaskMemFree(g_quickAccess);
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
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

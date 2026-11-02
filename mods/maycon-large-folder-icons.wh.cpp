// ==WindhawkMod==
// @id              maycon-large-folder-icons
// @name            Large icons — DevMaycon
// @description     Consistent large icons for Explorer folders and removable drives. Created for DevMaycon.
// @version         1.1.0
// @author          DevMaycon
// @github          https://github.com/MayconDIS
// @homepage        https://github.com/MayconDIS/Windhawk_Mods
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Large icons — DevMaycon

![Large icons — DevMaycon](https://raw.githubusercontent.com/MayconDIS/Windhawk_Mods/main/assets/large-icons.svg)

*Original project illustration.*

**A consistent folder view, wherever your files live.**

Created for DevMaycon with Codex assistance. This project keeps Explorer
folders in large-icon view using Windows Shell interfaces.

## Quick start

1. Enable this mod in Windhawk.
2. Open a regular folder in File Explorer.
3. Its view switches to large icons at the configured size.
4. Open **Details > Settings** to customize the behavior.

## What this project does

| Feature | Behavior |
| --- | --- |
| Consistent sizing | Uses 96 pixels by default; adjustable from 32 to 256. |
| Local and removable drives | Applies to open filesystem folders, including USB drives. |
| Network folders | Uses the same Shell view interface for network paths. |
| Keep enforcing | Reapplies the selected view after a manual change. |
| Navigation-only mode | Allows manual changes until the next folder navigation. |
| Lightweight updates | Checks open views; does not scan folder contents. |

## Settings

**Icon size:** 96 pixels corresponds to Large icons.
**Check interval:** 1500 milliseconds by default; lower values react sooner.
**Keep enforcing:** turn this off if you prefer to change a view temporarily.

## Scope and limitations

The desktop, Home, This PC, Recycle Bin, and Open/Save dialogs are excluded.
Minimized Explorer windows are processed after restoration. Tabs are processed
when the Shell exposes their views. A brief delay before applying the view is
normal. Removable drives were not physically connected during validation.

## Your files stay in place

This mod changes presentation only. It does not move, delete, upload, or index
your files and does not clear Explorer's saved view cache. It does not require
WinSetView. Disabling stops enforcement; Windows may retain the last view.

## Project files

- `maycon-large-folder-icons.wh.cpp` — complete source, settings, and this guide.
- `scripts/validate.ps1` — metadata, guide, and credit checks.
- `scripts/build.ps1` — reproducible build without installing the mod.
- `docs/large-icons/README.md` — this guide and recovery notes.

## Version 1.1.0

- DevMaycon branding and original English project documentation.
- Clearer function names for view processing and monitoring.
- Existing mod identity and user settings preserved during the update.


## Credits and links

Original project: **DevMaycon**, developed with Codex assistance. Licensed under **GPL-3.0-only**.

- Custom edition and maintenance: **DevMaycon**.
- Development assistance: **OpenAI Codex**.
- Repository: [MayconDIS/Windhawk_Mods](https://github.com/MayconDIS/Windhawk_Mods).
- GitHub profile: [MayconDIS](https://github.com/MayconDIS).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconSize: 96
  $name: Icon size (pixels)
  $description: '96 corresponds to Large icons. Accepted range: 32 to 256.'
- intervalMs: 1500
  $name: Check interval (milliseconds)
  $description: 'Default 1500. Accepted range: 500 to 10000.'
- alwaysEnforce: true
  $name: Keep enforcing large icons
  $description: Turn off to allow manual view changes until the next folder navigation.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <exdisp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <atomic>
#include <map>
#include <string>
#include <cstdio>

// All Shell COM objects live on the worker's single STA. No private symbols,
// hooks, code patches, registry view edits, or directory scans are needed.
template<class T> class ComPtr {
    T* p_ = nullptr;
public:
    ~ComPtr() { if (p_) p_->Release(); }
    ComPtr() = default;
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    T* operator->() const { return p_; }
    T* get() const { return p_; }
    T** put() { return &p_; }
    void** out() { return reinterpret_cast<void**>(&p_); }
};

static HANDLE g_stop = nullptr;
static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static std::atomic<int> g_iconSize{96}, g_interval{1500};
static std::atomic<bool> g_always{true};
static std::atomic<unsigned> g_generation{0};
static std::map<HWND, std::wstring> g_previous;

static bool Stopping() {
    return g_stop && WaitForSingleObject(g_stop, 0) == WAIT_OBJECT_0;
}

static void ProcessFolderView(IDispatch* dispatch, std::map<HWND, std::wstring>& seen) {
    ComPtr<IWebBrowser2> web;
    if (FAILED(dispatch->QueryInterface(IID_IWebBrowser2, web.out()))) return;
    SHANDLE_PTR handle = 0;
    if (FAILED(web->get_HWND(&handle))) return;
    HWND hwnd = reinterpret_cast<HWND>(handle);
    wchar_t className[64]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (wcscmp(className, L"CabinetWClass") != 0) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
#ifndef MAYCON_DIAGNOSTIC
    if (pid != GetCurrentProcessId() || !IsWindowVisible(hwnd) || IsIconic(hwnd)) return;
#endif
    ComPtr<IServiceProvider> service;
    if (FAILED(web->QueryInterface(IID_IServiceProvider, service.out()))) return;
    ComPtr<IShellBrowser> browser;
    if (FAILED(service->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, browser.out()))) return;
    ComPtr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put()))) return;
    ComPtr<IFolderView2> folderView;
    if (FAILED(view->QueryInterface(IID_IFolderView2, folderView.out()))) return;
    ComPtr<IPersistFolder2> folder;
    if (FAILED(folderView->GetFolder(IID_IPersistFolder2, folder.out()))) return;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(folder->GetCurFolder(&pidl)) || !pidl) return;
    ComPtr<IShellItem> item;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_IShellItem, item.out());
    CoTaskMemFree(pidl);
    if (FAILED(hr)) return;
    SFGAOF attributes = 0;
    if (FAILED(item->GetAttributes(SFGAO_FILESYSTEM | SFGAO_FOLDER, &attributes)) ||
        (attributes & (SFGAO_FILESYSTEM | SFGAO_FOLDER)) != (SFGAO_FILESYSTEM | SFGAO_FOLDER)) return;
    PWSTR pathRaw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &pathRaw)) || !pathRaw) return;
    std::wstring path(pathRaw);
    CoTaskMemFree(pathRaw);
    seen[hwnd] = path;
    FOLDERVIEWMODE mode{};
    int size = 0;
    if (FAILED(folderView->GetViewModeAndIconSize(&mode, &size))) return;
#ifdef MAYCON_DIAGNOSTIC
    wprintf(L"HWND=%p PID=%lu Mode=%d IconSize=%d Path=%ls\n", hwnd, pid, int(mode), size, path.c_str());
#else
    const auto previous = g_previous.find(hwnd);
    if (!g_always.load() && previous != g_previous.end() && previous->second == path) return;
    const int desired = g_iconSize.load();
    if (mode == FVM_ICON && size == desired) return;
    if (Stopping()) return;
    hr = folderView->SetViewModeAndIconSize(FVM_ICON, desired);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"Folder view applied: window=%p, size=%d", hwnd, desired);
    } else {
        // Retry on the next scan, including in navigation-only mode.
        seen.erase(hwnd);
    }
#endif
}

static void ScanOpenFolders() {
    ComPtr<IShellWindows> windows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_IShellWindows, windows.out()))) return;
    long count = 0;
    if (FAILED(windows->get_Count(&count))) return;
    std::map<HWND, std::wstring> seen;
    for (long i = 0; i < count && !Stopping(); ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        ComPtr<IDispatch> dispatch;
        if (SUCCEEDED(windows->Item(index, dispatch.put()))) {
            // The collection can change during enumeration: Item may return S_FALSE.
            if (dispatch.get()) ProcessFolderView(dispatch.get(), seen);
        }
    }
    g_previous.swap(seen);
}

#ifdef MAYCON_DIAGNOSTIC
int wmain() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return 1;
    ScanOpenFolders();
    CoUninitialize();
    return 0;
}
#else
static int ClampSetting(int value, int minimum, int maximum) {
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}
static void LoadSettings() {
    g_iconSize = ClampSetting(Wh_GetIntSetting(L"iconSize"), 32, 256);
    g_interval = ClampSetting(Wh_GetIntSetting(L"intervalMs"), 500, 10000);
    g_always = Wh_GetIntSetting(L"alwaysEnforce") != 0;
    ++g_generation;
}
static DWORD WINAPI MonitorThread(void*) {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return 1;
    CoEnableCallCancellation(nullptr);
    unsigned generation = g_generation.load();
    while (!Stopping()) {
        const auto current = g_generation.load();
        if (current != generation) {
            g_previous.clear();
            generation = current;
        }
        ScanOpenFolders();
        // An STA must pump messages while waiting for its next scan.
        const ULONGLONG until = GetTickCount64() + g_interval.load();
        while (!Stopping()) {
            const auto now = GetTickCount64();
            if (now >= until) break;
            DWORD result = MsgWaitForMultipleObjects(1, &g_stop, FALSE,
                static_cast<DWORD>(until - now), QS_ALLINPUT);
            if (result == WAIT_OBJECT_0 || result == WAIT_TIMEOUT || result == WAIT_FAILED) break;
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }
    g_previous.clear();
    CoDisableCallCancellation(nullptr);
    CoUninitialize();
    return 0;
}
BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    return g_stop != nullptr;
}
void Wh_ModAfterInit() {
    g_thread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, &g_threadId);
    if (!g_thread) Wh_Log(L"Could not start worker: %lu", GetLastError());
}
void Wh_ModSettingsChanged() { LoadSettings(); }
void Wh_ModUninit() {
    if (g_stop) SetEvent(g_stop);
    if (g_thread) {
        // Never unload code still used by the worker. Cancel a pending outbound
        // COM call if another shell window is not responding during unload.
        while (WaitForSingleObject(g_thread, 1000) == WAIT_TIMEOUT)
            CoCancelCall(g_threadId, 0);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stop) { CloseHandle(g_stop); g_stop = nullptr; }
}
#endif

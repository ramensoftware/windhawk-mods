// ==WindhawkMod==
// @id              explorer-windows-to-tabs
// @name            Explorer: New Windows Open as Tabs
// @description     Stop File Explorer from piling up separate windows - folders open as tabs in the window you already have
// @version         1.0
// @author          xcloudx01
// @github          https://github.com/xcloudx01
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer: New Windows Open as Tabs

**One File Explorer window, many tabs - no more window clutter.**

Windows often ignores your tab preferences and opens a brand new Explorer
window anyway. With this mod, those folders open as a new tab in the Explorer window you
already have open instead.

## How it works

Whenever File Explorer is about to show a new window while another Explorer
window already exists, this mod hides the new window, opens the same folder
(keeping the selected file, e.g. from `explorer.exe /select,<file>`) in a new
tab of the most recently used Explorer window, and closes the hidden one.

Works regardless of what launched the window: other apps' "Show in folder",
scripts/agents running `explorer.exe <path>`, Win+E, the taskbar, etc.

Not merged:
- Dragging a tab out of a window (left mouse button held).
- Holding **Ctrl** or **Shift** while the window opens (optional, see settings),
  e.g. Ctrl+N in Explorer still gives a real new window.
- The first Explorer window, or when the only other Explorer windows are on a
  different virtual desktop.

If anything fails, the original window is simply shown as normal.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- bypassModifiers: true
  $name: Ctrl/Shift opens a real new window
  $description: Holding Ctrl or Shift while a window is opening skips merging
- activateTarget: true
  $name: Bring the existing window to the front
- selectionWaitMs: 300
  $name: Selection wait (ms)
  $description: How long to wait for the new window to select an item (explorer.exe /select) before moving it
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <algorithm>
#include <mutex>
#include <vector>

static const GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

constexpr UINT kCmdNewTab = 0xA21B;

struct {
    bool bypassModifiers;
    bool activateTarget;
    int selectionWaitMs;
} g_settings;

HANDLE g_workerThread;
DWORD g_workerThreadId;
std::mutex g_pendingMutex;
std::vector<HWND> g_pending;

// ---------------------------------------------------------------------------
// Window helpers

bool HasClass(HWND hwnd, const wchar_t* cls) {
    wchar_t buf[64];
    return GetClassNameW(hwnd, buf, ARRAYSIZE(buf)) && wcscmp(buf, cls) == 0;
}

bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

void SetCloak(HWND hwnd, bool cloak) {
    BOOL v = cloak;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &v, sizeof(v));
}

void RemovePending(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_pendingMutex);
    std::erase(g_pending, hwnd);
}

// Most recently active visible Explorer window on the current desktop, other
// than `exclude`. EnumWindows walks top-level windows in Z order.
HWND FindTargetWindow(HWND exclude) {
    struct Ctx {
        HWND exclude;
        HWND result;
    } ctx{exclude, nullptr};
    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            if (hwnd == c->exclude || !IsWindowVisible(hwnd) ||
                !HasClass(hwnd, L"CabinetWClass") || IsCloaked(hwnd) ||
                !FindWindowExW(hwnd, nullptr, L"ShellTabWindowClass",
                               nullptr)) {
                return TRUE;
            }
            c->result = hwnd;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    return ctx.result;
}

std::vector<HWND> GetTabs(HWND frame) {
    std::vector<HWND> tabs;
    for (HWND t = nullptr;
         (t = FindWindowExW(frame, t, L"ShellTabWindowClass", nullptr));) {
        tabs.push_back(t);
    }
    return tabs;
}

// ---------------------------------------------------------------------------
// Shell COM helpers

template <class T>
void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

IShellBrowser* GetShellBrowser(IWebBrowser2* wb) {
    IServiceProvider* sp = nullptr;
    IShellBrowser* sb = nullptr;
    if (SUCCEEDED(wb->QueryInterface(IID_PPV_ARGS(&sp)))) {
        sp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&sb));
        sp->Release();
    }
    return sb;
}

IFolderView* GetFolderView(IWebBrowser2* wb) {
    IFolderView* fv = nullptr;
    if (IShellBrowser* sb = GetShellBrowser(wb)) {
        IShellView* sv = nullptr;
        if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
            sv->QueryInterface(IID_PPV_ARGS(&fv));
            sv->Release();
        }
        sb->Release();
    }
    return fv;
}

PIDLIST_ABSOLUTE GetFolderPidl(IWebBrowser2* wb) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (IFolderView* fv = GetFolderView(wb)) {
        IPersistFolder2* pf = nullptr;
        if (SUCCEEDED(fv->GetFolder(IID_PPV_ARGS(&pf)))) {
            pf->GetCurFolder(&pidl);
            pf->Release();
        }
        fv->Release();
    }
    return pidl;
}

std::vector<PITEMID_CHILD> GetSelection(IWebBrowser2* wb) {
    std::vector<PITEMID_CHILD> items;
    if (IFolderView* fv = GetFolderView(wb)) {
        IEnumIDList* e = nullptr;
        if (SUCCEEDED(fv->Items(SVGIO_SELECTION, IID_PPV_ARGS(&e))) && e) {
            PITEMID_CHILD child;
            while (e->Next(1, &child, nullptr) == S_OK) {
                items.push_back(child);
            }
            e->Release();
        }
        fv->Release();
    }
    return items;
}

void FreeSelection(std::vector<PITEMID_CHILD>& items) {
    for (auto p : items) {
        CoTaskMemFree(p);
    }
    items.clear();
}

bool IsBusy(IWebBrowser2* wb) {
    VARIANT_BOOL busy = VARIANT_FALSE;
    return SUCCEEDED(wb->get_Busy(&busy)) && busy;
}

// Find the browser object for a frame (match==frame HWND) or for a tab.
IWebBrowser2* FindBrowser(IShellWindows* sw, HWND frame, HWND tab) {
    long count = 0;
    if (FAILED(sw->get_Count(&count))) {
        return nullptr;
    }
    for (long i = 0; i < count; i++) {
        VARIANT idx;
        idx.vt = VT_I4;
        idx.lVal = i;
        IDispatch* disp = nullptr;
        if (FAILED(sw->Item(idx, &disp)) || !disp) {
            continue;
        }
        IWebBrowser2* wb = nullptr;
        disp->QueryInterface(IID_PPV_ARGS(&wb));
        disp->Release();
        if (!wb) {
            continue;
        }
        SHANDLE_PTR h = 0;
        if (SUCCEEDED(wb->get_HWND(&h)) && (HWND)h == frame) {
            if (!tab) {
                return wb;
            }
            HWND tabHwnd = nullptr;
            if (IShellBrowser* sb = GetShellBrowser(wb)) {
                sb->GetWindow(&tabHwnd);
                sb->Release();
            }
            if (tabHwnd == tab) {
                return wb;
            }
        }
        wb->Release();
    }
    return nullptr;
}

bool NavigateToPidl(IWebBrowser2* wb, PCIDLIST_ABSOLUTE pidl) {
    UINT cb = ILGetSize(pidl);
    SAFEARRAY* sa = SafeArrayCreateVector(VT_UI1, 0, cb);
    if (!sa) {
        return false;
    }
    void* data = nullptr;
    SafeArrayAccessData(sa, &data);
    memcpy(data, pidl, cb);
    SafeArrayUnaccessData(sa);

    VARIANT target;
    target.vt = VT_ARRAY | VT_UI1;
    target.parray = sa;
    VARIANT empty;
    VariantInit(&empty);
    HRESULT hr = wb->Navigate2(&target, &empty, &empty, &empty, &empty);
    VariantClear(&target);
    return SUCCEEDED(hr);
}

// ---------------------------------------------------------------------------
// Merge logic (worker thread)

bool StillPending(HWND hwnd) {
    return IsWindow(hwnd);
}

bool MergeWindow(HWND newFrame) {
    IShellWindows* sw = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&sw)))) {
        Wh_Log(L"No IShellWindows");
        return false;
    }

    bool ok = false;
    IWebBrowser2* src = nullptr;
    IWebBrowser2* dst = nullptr;
    PIDLIST_ABSOLUTE folder = nullptr;
    std::vector<PITEMID_CHILD> selection;
    HWND target = nullptr;

    // 1. Wait for the new window to finish navigating to its folder.
    for (DWORD start = GetTickCount(); GetTickCount() - start < 5000;
         Sleep(30)) {
        if (!StillPending(newFrame)) {
            ok = true;  // Closed already, nothing to do.
            goto done;
        }
        if (!src) {
            src = FindBrowser(sw, newFrame, nullptr);
        }
        if (src && !IsBusy(src)) {
            folder = GetFolderPidl(src);
            if (folder) {
                break;
            }
        }
    }
    if (!folder) {
        Wh_Log(L"New window never reported a folder");
        goto done;
    }

    // 2. Give explorer.exe /select a moment to apply its selection.
    for (DWORD start = GetTickCount();
         GetTickCount() - start < (DWORD)g_settings.selectionWaitMs;
         Sleep(30)) {
        selection = GetSelection(src);
        if (!selection.empty()) {
            break;
        }
    }

    // 3. Open a new tab in the target window.
    target = FindTargetWindow(newFrame);
    if (!target) {
        Wh_Log(L"No target window");
        goto done;
    }
    {
        std::vector<HWND> before = GetTabs(target);
        if (before.empty()) {
            goto done;
        }
        DWORD_PTR res;
        if (!SendMessageTimeoutW(before[0], WM_COMMAND, kCmdNewTab, 0,
                                 SMTO_ABORTIFHUNG, 3000, &res)) {
            Wh_Log(L"New tab command failed");
            goto done;
        }

        HWND newTab = nullptr;
        for (DWORD start = GetTickCount();
             GetTickCount() - start < 3000 && !dst; Sleep(30)) {
            if (!newTab) {
                for (HWND t : GetTabs(target)) {
                    if (std::find(before.begin(), before.end(), t) ==
                        before.end()) {
                        newTab = t;
                        break;
                    }
                }
            }
            if (newTab) {
                dst = FindBrowser(sw, target, newTab);
            }
        }
        if (!dst) {
            Wh_Log(L"Could not find the new tab");
            goto done;
        }
    }

    // 4. Navigate it, then get rid of the hidden window.
    if (!NavigateToPidl(dst, folder)) {
        Wh_Log(L"Navigate2 failed");
        goto done;
    }
    ok = true;
    PostMessageW(newFrame, WM_CLOSE, 0, 0);

    if (g_settings.activateTarget) {
        if (IsIconic(target)) {
            ShowWindow(target, SW_RESTORE);
        }
        SetForegroundWindow(target);
    }

    // 5. Re-apply the selection once the tab has arrived.
    if (!selection.empty()) {
        for (DWORD start = GetTickCount(); GetTickCount() - start < 5000;
             Sleep(50)) {
            if (IsBusy(dst)) {
                continue;
            }
            PIDLIST_ABSOLUTE cur = GetFolderPidl(dst);
            bool arrived = cur && ILIsEqual(cur, folder);
            CoTaskMemFree(cur);
            if (!arrived) {
                continue;
            }
            IShellBrowser* sb = GetShellBrowser(dst);
            IShellView* sv = nullptr;
            if (sb && SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
                bool first = true;
                for (auto child : selection) {
                    UINT flags = SVSI_SELECT;
                    if (first) {
                        flags |= SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE |
                                 SVSI_FOCUSED;
                    }
                    sv->SelectItem(child, flags);
                    first = false;
                }
                sv->Release();
            }
            SafeRelease(sb);
            break;
        }
    }

done:
    FreeSelection(selection);
    CoTaskMemFree(folder);
    SafeRelease(src);
    SafeRelease(dst);
    SafeRelease(sw);
    return ok;
}

DWORD WINAPI WorkerThread(LPVOID readyEvent) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // Create the queue.
    SetEvent((HANDLE)readyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP && !msg.hwnd) {
            HWND hwnd = (HWND)msg.wParam;
            if (!MergeWindow(hwnd) && IsWindow(hwnd)) {
                SetCloak(hwnd, false);
            }
            RemovePending(hwnd);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Hook

void OnNewExplorerWindow(HWND hwnd) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    if (GetAsyncKeyState(swapped ? VK_RBUTTON : VK_LBUTTON) & 0x8000) {
        return;  // Tab being dragged out into its own window.
    }
    if (g_settings.bypassModifiers &&
        ((GetAsyncKeyState(VK_CONTROL) | GetAsyncKeyState(VK_SHIFT)) &
         0x8000)) {
        return;
    }
    if (!FindTargetWindow(hwnd)) {
        return;
    }

    SetCloak(hwnd, true);
    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        g_pending.push_back(hwnd);
    }
    if (!PostThreadMessageW(g_workerThreadId, WM_APP, (WPARAM)hwnd, 0)) {
        SetCloak(hwnd, false);
        RemovePending(hwnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 LPCWSTR className,
                                 LPCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 LPVOID param) {
    HWND hwnd = CreateWindowExW_Orig(exStyle, className, windowName, style, x,
                                     y, width, height, parent, menu, instance,
                                     param);
    if (hwnd && !(style & WS_CHILD) && g_workerThreadId &&
        HasClass(hwnd, L"CabinetWClass")) {
        OnNewExplorerWindow(hwnd);
    }
    return hwnd;
}

// ---------------------------------------------------------------------------
// Mod lifecycle

void LoadSettings() {
    g_settings.bypassModifiers = Wh_GetIntSetting(L"bypassModifiers");
    g_settings.activateTarget = Wh_GetIntSetting(L"activateTarget");
    g_settings.selectionWaitMs = Wh_GetIntSetting(L"selectionWaitMs");
    if (g_settings.selectionWaitMs < 0 || g_settings.selectionWaitMs > 5000) {
        g_settings.selectionWaitMs = 300;
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, ready, 0, &g_workerThreadId);
    if (!g_workerThread) {
        CloseHandle(ready);
        return FALSE;
    }
    WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Orig);
    return TRUE;
}

void Wh_ModUninit() {
    DWORD tid = g_workerThreadId;
    g_workerThreadId = 0;
    PostThreadMessageW(tid, WM_QUIT, 0, 0);
    WaitForSingleObject(g_workerThread, 10000);
    CloseHandle(g_workerThread);

    std::lock_guard<std::mutex> lock(g_pendingMutex);
    for (HWND hwnd : g_pending) {
        if (IsWindow(hwnd)) {
            SetCloak(hwnd, false);
        }
    }
    g_pending.clear();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

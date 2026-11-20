// ==WindhawkMod==
// @id              desktopfolders-instant-open
// @name            DesktopFolders: Instant Smart Folder Open
// @description     Opens DesktopFolders smart folders on the desktop instantly and reliably on double-click or Enter
// @version         1.0.0
// @author          RFplanejamentofinanceiro-hub
// @github          https://github.com/RFplanejamentofinanceiro-hub
// @homepage        https://github.com/RFplanejamentofinanceiro-hub/MultiFolder-
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DesktopFolders: Instant Smart Folder Open

Companion mod for **[DesktopFolders](https://github.com/RFplanejamentofinanceiro-hub/MultiFolder-)**,
a free, open source app that adds iPhone/macOS-style smart folders to the Windows desktop:
group app shortcuts into a folder, and double-click it to get a popup with the apps inside.

A smart folder is a regular desktop shortcut that points to `DesktopFolders.exe`. Explorer
sometimes ignores the double-click on these shortcuts, or opens the popup without focus.
This mod handles the double-click (and the Enter key) on the desktop itself, inside Explorer:

- if the item is a shortcut whose target is `DesktopFolders.exe`, the mod starts it directly
  with the shortcut's arguments, so the folder opens immediately and in the foreground;
- any other desktop item is left untouched and opens normally.

**Requirement:** DesktopFolders must be installed. Get it from the
[releases page](https://github.com/RFplanejamentofinanceiro-hub/MultiFolder-/releases).
Without it the mod does nothing.

Optionally, smart folders can open with a **single click** (see the mod settings). Dragging a
smart folder still works, and Ctrl/Shift+click still selects it.

Only the desktop is affected; File Explorer windows are not changed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- singleClick: false
  $name: Open smart folders with a single click
  $description: Other desktop icons keep the normal double-click behavior
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <exdisp.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <string>

#include <windhawk_utils.h>

namespace {

// Window property that tells the DesktopFolders app the mod is active, so the
// app does not run its own double-click detection.
constexpr WCHAR kActiveProp[] = L"DesktopFolders.ModActive";

HANDLE g_stopEvent;
HANDLE g_watchThread;
HWND g_listView;  // desktop SysListView32 currently subclassed
bool g_singleClick;

// single click: item pressed, waiting for the button release
int g_pressIndex = -1;
POINT g_pressPoint;

void LoadSettings() {
    g_singleClick = Wh_GetIntSetting(L"singleClick") != 0;
}

int HitTestItem(HWND listView, POINT pt) {
    LVHITTESTINFO hit = {};
    hit.pt = pt;
    int index = (int)SendMessageW(listView, LVM_HITTEST, 0, (LPARAM)&hit);
    return (index >= 0 && (hit.flags & LVHT_ONITEM)) ? index : -1;
}

bool IsClickRelease(HWND listView, POINT pt) {
    return g_pressIndex >= 0 &&
           abs(pt.x - g_pressPoint.x) <= GetSystemMetrics(SM_CXDRAG) &&
           abs(pt.y - g_pressPoint.y) <= GetSystemMetrics(SM_CYDRAG) &&
           HitTestItem(listView, pt) == g_pressIndex;
}

// Full parsing path of the desktop item at the given list view index.
bool GetDesktopItemPath(int index, std::wstring& path) {
    bool ok = false;
    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows)))) {
        return false;
    }

    VARIANT empty;
    VariantInit(&empty);
    long hwnd = 0;
    IDispatch* dispatch = nullptr;
    if (SUCCEEDED(shellWindows->FindWindowSW(&empty, &empty, SWC_DESKTOP,
                                              &hwnd, SWFO_NEEDDISPATCH,
                                              &dispatch)) &&
        dispatch) {
        IServiceProvider* provider = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS(&provider)))) {
            IShellBrowser* browser = nullptr;
            if (SUCCEEDED(provider->QueryService(SID_STopLevelBrowser,
                                                 IID_PPV_ARGS(&browser)))) {
                IShellView* view = nullptr;
                if (SUCCEEDED(browser->QueryActiveShellView(&view))) {
                    IFolderView* folderView = nullptr;
                    if (SUCCEEDED(view->QueryInterface(
                            IID_PPV_ARGS(&folderView)))) {
                        IShellFolder* folder = nullptr;
                        PITEMID_CHILD child = nullptr;
                        if (SUCCEEDED(folderView->GetFolder(
                                IID_PPV_ARGS(&folder))) &&
                            SUCCEEDED(folderView->Item(index, &child))) {
                            STRRET str;
                            WCHAR buffer[MAX_PATH * 2];
                            if (SUCCEEDED(folder->GetDisplayNameOf(
                                    child, SHGDN_FORPARSING, &str)) &&
                                SUCCEEDED(StrRetToBufW(&str, child, buffer,
                                                       ARRAYSIZE(buffer)))) {
                                path = buffer;
                                ok = true;
                            }
                        }
                        if (child) {
                            CoTaskMemFree(child);
                        }
                        if (folder) {
                            folder->Release();
                        }
                        folderView->Release();
                    }
                    view->Release();
                }
                browser->Release();
            }
            provider->Release();
        }
        dispatch->Release();
    }
    shellWindows->Release();
    return ok;
}

// Last step reached by the most recent activation, kept as a window property
// so problems can be diagnosed without debug logging:
// 1 no item hit, 2 path from the shell view failed, 3 path not found at all,
// 4 not a .lnk, 5 not a DesktopFolders shortcut, 6 launch failed, 9 opened.
constexpr WCHAR kStageProp[] = L"DesktopFolders.ModStage";

void SetStage(HWND listView, int stage) {
    SetPropW(listView, kStageProp, (HANDLE)(INT_PTR)stage);
}

// Fallback: the item's label plus ".lnk" in the user's or the public desktop.
bool GetDesktopItemPathByName(HWND listView, int index, std::wstring& path) {
    WCHAR text[MAX_PATH] = {};
    LVITEMW item = {};
    item.iSubItem = 0;
    item.pszText = text;
    item.cchTextMax = ARRAYSIZE(text);
    if (SendMessageW(listView, LVM_GETITEMTEXTW, index, (LPARAM)&item) <= 0) {
        return false;
    }
    const KNOWNFOLDERID* folders[] = {&FOLDERID_Desktop, &FOLDERID_PublicDesktop};
    for (const KNOWNFOLDERID* folder : folders) {
        PWSTR dir = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(*folder, 0, nullptr, &dir))) {
            std::wstring candidate = std::wstring(dir) + L"\\" + text + L".lnk";
            CoTaskMemFree(dir);
            if (GetFileAttributesW(candidate.c_str()) !=
                INVALID_FILE_ATTRIBUTES) {
                path = candidate;
                return true;
            }
        } else if (dir) {
            CoTaskMemFree(dir);
        }
    }
    return false;
}

// If the item is a DesktopFolders smart folder shortcut, starts it directly.
bool TryOpenSmartFolder(HWND listView, int index) {
    if (index < 0) {
        SetStage(listView, 1);
        return false;
    }
    std::wstring path;
    if (!GetDesktopItemPath(index, path)) {
        SetStage(listView, 2);
        if (!GetDesktopItemPathByName(listView, index, path)) {
            SetStage(listView, 3);
            return false;
        }
    }
    if (_wcsicmp(PathFindExtensionW(path.c_str()), L".lnk") != 0) {
        SetStage(listView, 4);
        return false;
    }
    SetStage(listView, 5);

    bool opened = false;
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) {
        return false;
    }
    IPersistFile* file = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))) {
        WCHAR target[MAX_PATH] = {};
        WCHAR args[2048] = {};
        if (SUCCEEDED(file->Load(path.c_str(), STGM_READ)) &&
            SUCCEEDED(link->GetPath(target, ARRAYSIZE(target), nullptr,
                                    SLGP_RAWPATH)) &&
            _wcsicmp(PathFindFileNameW(target), L"DesktopFolders.exe") == 0 &&
            GetFileAttributesW(target) != INVALID_FILE_ATTRIBUTES) {
            link->GetArguments(args, ARRAYSIZE(args));
            SetStage(listView, 6);

            // Start the process directly, so opening the folder does not
            // depend on how the shell handles the shortcut.
            std::wstring cmd = L"\"" + std::wstring(target) + L"\" " + args;
            std::wstring dir = target;
            dir.resize(PathFindFileNameW(target) - target);
            STARTUPINFOW si = {sizeof(si)};
            PROCESS_INFORMATION pi = {};
            if (CreateProcessW(target, cmd.data(), nullptr, nullptr, FALSE, 0,
                               nullptr, dir.empty() ? nullptr : dir.c_str(),
                               &si, &pi)) {
                AllowSetForegroundWindow(pi.dwProcessId);
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                Wh_Log(L"Opened smart folder: %s", path.c_str());
                SetStage(listView, 9);
                opened = true;
            }
        }
        file->Release();
    }
    link->Release();
    return opened;
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_LBUTTONDBLCLK: {
            g_pressIndex = -1;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (TryOpenSmartFolder(hWnd, HitTestItem(hWnd, pt))) {
                return 0;
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            g_pressIndex = -1;
            if (!g_singleClick || (wParam & (MK_CONTROL | MK_SHIFT))) {
                break;
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = HitTestItem(hWnd, pt);
            if (index < 0) {
                break;
            }
            g_pressIndex = index;
            g_pressPoint = pt;
            // The list view tracks the mouse inside WM_LBUTTONDOWN until the
            // button is released or a drag starts.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (g_pressIndex >= 0 && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                POINT now;
                GetCursorPos(&now);
                ScreenToClient(hWnd, &now);
                if (IsClickRelease(hWnd, now)) {
                    TryOpenSmartFolder(hWnd, g_pressIndex);
                }
                g_pressIndex = -1;
            }
            return result;
        }
        case WM_LBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (IsClickRelease(hWnd, pt)) {
                TryOpenSmartFolder(hWnd, g_pressIndex);
            }
            g_pressIndex = -1;
            break;
        }
        case WM_KEYDOWN:
            if (wParam == VK_RETURN &&
                SendMessageW(hWnd, LVM_GETSELECTEDCOUNT, 0, 0) == 1) {
                int index = (int)SendMessageW(hWnd, LVM_GETNEXTITEM, (WPARAM)-1,
                                              LVNI_SELECTED);
                if (TryOpenSmartFolder(hWnd, index)) {
                    return 0;
                }
            }
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HWND FindDesktopListView() {
    HWND defView = nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    // the desktop view can live in a WorkerW window instead
    for (HWND worker = nullptr; !defView;) {
        worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr);
        if (!worker) {
            break;
        }
        defView = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    if (!defView) {
        return nullptr;
    }
    HWND listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    DWORD pid = 0;
    if (!listView || !GetWindowThreadProcessId(listView, &pid) ||
        pid != GetCurrentProcessId()) {
        return nullptr;  // the desktop belongs to another explorer process
    }
    return listView;
}

// The desktop may not exist yet when the mod loads, and it can be recreated:
// keep the subclass attached to the current desktop list view.
DWORD WINAPI WatchThread(LPVOID) {
    do {
        HWND listView = FindDesktopListView();
        // new list view, or the mark is gone (explorer reset the window):
        // (re)attach; subclassing twice with the same proc is harmless
        if (listView &&
            (listView != g_listView || !GetPropW(listView, kActiveProp))) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                    listView, ListViewSubclassProc, 0)) {
                if (g_listView && IsWindow(g_listView)) {
                    RemovePropW(g_listView, kActiveProp);
                }
                g_listView = listView;
                SetPropW(listView, kActiveProp, (HANDLE)1);
                Wh_Log(L"Subclassed desktop list view %p", listView);
            }
        }
    } while (WaitForSingleObject(g_stopEvent, 2000) == WAIT_TIMEOUT);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_watchThread = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    return g_watchThread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_watchThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_watchThread, INFINITE);
        CloseHandle(g_watchThread);
        g_watchThread = nullptr;
    }
    if (g_listView && IsWindow(g_listView)) {
        RemovePropW(g_listView, kActiveProp);
        RemovePropW(g_listView, kStageProp);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_listView,
                                                         ListViewSubclassProc);
    }
    g_listView = nullptr;
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

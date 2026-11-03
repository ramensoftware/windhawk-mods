// ==WindhawkMod==
// @id              explorer-large-icons-view
// @name            Large Icons View for File Explorer
// @description     Keeps File Explorer folders in Large icons view, with a configurable icon size.
// @version         2.0.0
// @author          Dev_Maycon
// @github          https://github.com/MayconDIS
// @homepage        https://github.com/MayconDIS/Windhawk_Mods
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lshell32 -lcomctl32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Large Icons View for File Explorer

Keeps File Explorer folders in **Large icons** view. The icon size is
configurable: 96 pixels matches Windows' own Large icons.

## How it works

The view is switched when a folder opens in a File Explorer window, without
waiting or polling.

- **Keep enforcing** (default): if you switch a folder to another view, it is
  switched back to large icons.
- With **Keep enforcing** off, every folder opens in large icons, but a view
  you pick manually stays until you navigate to another folder.

It applies to file-system folders (local, removable and network drives). The
desktop, Open/Save dialogs, Home, This PC, the Recycle Bin and other virtual
folders are not affected. Folder contents are never read, moved or changed.

## Disabling the mod and restoring views

When *Remember each folder's view settings* is on (the Windows default),
Explorer saves the view of each folder you open. Every folder opened while the
mod is enabled is therefore saved in large icons, and keeps that view after the
mod is disabled or removed.

To restore the default view of all folders, open **Folder Options > View** and
select **Reset Folders**.

## Credits and links

Original project by **Dev_Maycon**, developed with AI assistance. Licensed
under **GPL-3.0-only**.

- Repository: [MayconDIS/Windhawk_Mods](https://github.com/MayconDIS/Windhawk_Mods).
- GitHub profile: [MayconDIS](https://github.com/MayconDIS).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconSize: 96
  $name: Icon size (pixels)
  $description: '96 corresponds to Large icons. Accepted range: 32 to 256.'
- alwaysEnforce: true
  $name: Keep enforcing large icons
  $description: >-
    Turn off to keep a view you pick manually until you navigate to another
    folder.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <exdisp.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <atomic>
#include <mutex>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;

static std::atomic<int> g_iconSize{96};
static std::atomic<bool> g_alwaysEnforce{true};

// View windows (SHELLDLL_DefView) already handled. Explorer creates a new view
// window for every navigation and every tab, so "handled once per view window"
// is exactly navigation-only mode. Each Explorer window runs on its own thread.
static std::mutex g_handledMutex;
static std::unordered_set<HWND> g_handled;

// View windows the mod is changing right now, so the SetCurrentViewMode hook
// ignores the mod's own change. Keyed by view, not by thread: through
// ShellWindows the change is requested from another thread but runs on the
// view's thread. Guarded by g_handledMutex.
static std::unordered_set<HWND> g_applying;

static bool IsApplying(HWND viewWindow) {
    std::lock_guard<std::mutex> lock(g_handledMutex);
    return g_applying.count(viewWindow) != 0;
}

static bool BeginApplying(HWND viewWindow) {
    std::lock_guard<std::mutex> lock(g_handledMutex);
    return g_applying.insert(viewWindow).second;
}

static void EndApplying(HWND viewWindow) {
    std::lock_guard<std::mutex> lock(g_handledMutex);
    g_applying.erase(viewWindow);
}

// Pure decision logic, kept free of COM so it can be unit tested.
static bool ShouldApplyView(bool alwaysEnforce, bool alreadyHandled, FOLDERVIEWMODE mode,
                            int size, int desired) {
    if (alreadyHandled && !alwaysEnforce) return false;
    return !(mode == FVM_ICON && size == desired);
}

static LRESULT CALLBACK ViewSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                         DWORD_PTR) {
    if (msg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_handledMutex);
        g_handled.erase(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static bool IsHandled(HWND viewWindow) {
    std::lock_guard<std::mutex> lock(g_handledMutex);
    return g_handled.count(viewWindow) != 0;
}

static void MarkHandled(HWND viewWindow) {
    bool inserted;
    {
        std::lock_guard<std::mutex> lock(g_handledMutex);
        inserted = g_handled.insert(viewWindow).second;
    }
    // Outside the lock: from another thread this sends a message to the view's
    // thread, which may itself be waiting for the lock in a hook.
    if (inserted &&
        !WindhawkUtils::SetWindowSubclassFromAnyThread(viewWindow, ViewSubclassProc, 0)) {
        // Without the subclass the entry could outlive its window.
        std::lock_guard<std::mutex> lock(g_handledMutex);
        g_handled.erase(viewWindow);
    }
}

// Only views hosted in a File Explorer window: the desktop and file dialogs
// opened by explorer.exe use the same view class.
static bool IsInFileExplorerWindow(HWND viewWindow) {
    wchar_t className[32]{};
    HWND root = GetAncestor(viewWindow, GA_ROOT);
    return root && GetClassNameW(root, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CabinetWClass") == 0;
}

// Home, This PC, the Recycle Bin, search results and other virtual folders
// keep their own views.
static bool IsFileSystemFolder(IFolderView2* folderView) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetIDListFromObject(folderView, &pidl)) || !pidl) return false;
    ComPtr<IShellItem> item;
    SFGAOF attributes = 0;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item));
    if (SUCCEEDED(hr)) hr = item->GetAttributes(SFGAO_FILESYSTEM | SFGAO_FOLDER, &attributes);
    CoTaskMemFree(pidl);
    return SUCCEEDED(hr) &&
           (attributes & (SFGAO_FILESYSTEM | SFGAO_FOLDER)) == (SFGAO_FILESYSTEM | SFGAO_FOLDER);
}

// `view` is a CDefView (in a hook, on its own thread) or a proxy to one.
static void ApplyLargeIcons(IUnknown* view, bool ignoreHandled) {
    ComPtr<IShellView> shellView;
    ComPtr<IFolderView2> folderView;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&shellView))) ||
        FAILED(view->QueryInterface(IID_PPV_ARGS(&folderView)))) {
        return;
    }
    HWND viewWindow = nullptr;
    if (FAILED(shellView->GetWindow(&viewWindow)) || !viewWindow || IsApplying(viewWindow)) return;
    if (!IsInFileExplorerWindow(viewWindow) || !IsFileSystemFolder(folderView.Get())) return;

    FOLDERVIEWMODE mode{};
    int size = 0;
    if (FAILED(folderView->GetViewModeAndIconSize(&mode, &size))) return;
    const int desired = g_iconSize.load();
    const bool handled = !ignoreHandled && IsHandled(viewWindow);
    if (ShouldApplyView(g_alwaysEnforce.load(), handled, mode, size, desired)) {
        if (!BeginApplying(viewWindow)) return;  // another thread is changing it
        const HRESULT hr = folderView->SetViewModeAndIconSize(FVM_ICON, desired);
        EndApplying(viewWindow);
        if (FAILED(hr)) {
            // Not marked as handled: retried at the next activation.
            Wh_Log(L"SetViewModeAndIconSize failed: 0x%08X", static_cast<unsigned>(hr));
            return;
        }
        Wh_Log(L"Large icons applied: view=%p, size=%d", viewWindow, desired);
    }
    MarkHandled(viewWindow);
}

// Folders that were already open when the mod was enabled or its settings
// changed. Their views are reached through proxies, so they are not retained.
static void ApplyToOpenWindows(bool ignoreHandled) {
    const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    {
        ComPtr<IShellWindows> shellWindows;
        long count = 0;
        if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                    IID_PPV_ARGS(&shellWindows))) ||
            FAILED(shellWindows->get_Count(&count))) {
            count = 0;
        }
        const DWORD processId = GetCurrentProcessId();
        for (long i = 0; i < count; ++i) {
            VARIANT index{};
            index.vt = VT_I4;
            index.lVal = i;
            ComPtr<IDispatch> dispatch;
            ComPtr<IWebBrowser2> browser;
            // The collection can change during enumeration: Item may return S_FALSE.
            if (FAILED(shellWindows->Item(index, &dispatch)) || !dispatch ||
                FAILED(dispatch.As(&browser))) {
                continue;
            }
            SHANDLE_PTR handle = 0;
            DWORD windowProcessId = 0;
            if (FAILED(browser->get_HWND(&handle))) continue;
            GetWindowThreadProcessId(reinterpret_cast<HWND>(handle), &windowProcessId);
            // Windows of other explorer.exe processes are handled by their own
            // instance of the mod.
            if (windowProcessId != processId) continue;
            ComPtr<IServiceProvider> services;
            ComPtr<IShellBrowser> shellBrowser;
            ComPtr<IShellView> shellView;
            if (SUCCEEDED(browser.As(&services)) &&
                SUCCEEDED(services->QueryService(SID_STopLevelBrowser,
                                                 IID_PPV_ARGS(&shellBrowser))) &&
                SUCCEEDED(shellBrowser->QueryActiveShellView(&shellView)) && shellView) {
                ApplyLargeIcons(shellView.Get(), ignoreHandled);
            }
        }
    }
    if (SUCCEEDED(init)) CoUninitialize();
}

using CDefView_UIActivate_t = HRESULT(__thiscall*)(void*, UINT);
static CDefView_UIActivate_t CDefView_UIActivate_orig;

// Runs when a view is shown, after a navigation or when switching tabs.
static HRESULT __thiscall CDefView_UIActivate_hook(void* pThis, UINT state) {
    const HRESULT hr = CDefView_UIActivate_orig(pThis, state);
    if (SUCCEEDED(hr) && (state == SVUIA_ACTIVATE_FOCUS || state == SVUIA_ACTIVATE_NOFOCUS))
        ApplyLargeIcons(static_cast<IUnknown*>(pThis), false);
    return hr;
}

// After the view of an open folder changes, "Keep enforcing" switches it back.
// In navigation-only mode the view is already handled and is left alone. The
// mod's own change is skipped by ApplyLargeIcons (the view is in g_applying).
static void OnViewChanged(void* pThis) {
    ApplyLargeIcons(static_cast<IUnknown*>(pThis), false);
}

using CDefView_SetCurrentViewMode_t = HRESULT(__thiscall*)(void*, UINT);
static CDefView_SetCurrentViewMode_t CDefView_SetCurrentViewMode_orig;

// Public entry point for view mode changes.
static HRESULT __thiscall CDefView_SetCurrentViewMode_hook(void* pThis, UINT viewMode) {
    const HRESULT hr = CDefView_SetCurrentViewMode_orig(pThis, viewMode);
    if (SUCCEEDED(hr)) OnViewChanged(pThis);
    return hr;
}

using CDefView_SwitchToViewModeAndIconSize_t = void(__thiscall*)(void*, FOLDERVIEWMODE, int);
static CDefView_SwitchToViewModeAndIconSize_t CDefView_SwitchToViewModeAndIconSize_orig;

// Internal switch also used for icon size changes (e.g. "Medium icons", or a
// size set through IFolderView2), which don't go through SetCurrentViewMode.
// Optional: a private symbol that may not exist on every Windows build.
static void __thiscall CDefView_SwitchToViewModeAndIconSize_hook(void* pThis, FOLDERVIEWMODE mode,
                                                                 int size) {
    CDefView_SwitchToViewModeAndIconSize_orig(pThis, mode, size);
    OnViewChanged(pThis);
}

static int ClampSetting(int value, int minimum, int maximum) {
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}

static void LoadSettings() {
    g_iconSize = ClampSetting(Wh_GetIntSetting(L"iconSize"), 32, 256);
    g_alwaysEnforce = Wh_GetIntSetting(L"alwaysEnforce") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!shell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }
    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
                L"public: virtual long __cdecl CDefView::UIActivate(unsigned int)",
                L"long __cdecl CDefView::UIActivate(unsigned int)",
            },
            &CDefView_UIActivate_orig,
            CDefView_UIActivate_hook,
        },
        {
            {
                L"public: virtual long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
                L"long __cdecl CDefView::SetCurrentViewMode(unsigned int)",
            },
            &CDefView_SetCurrentViewMode_orig,
            CDefView_SetCurrentViewMode_hook,
        },
        {
            {
                L"private: void __cdecl CDefView::_SwitchToViewModeAndIconSize(enum FOLDERVIEWMODE,int)",
                L"void __cdecl CDefView::_SwitchToViewModeAndIconSize(enum FOLDERVIEWMODE,int)",
            },
            &CDefView_SwitchToViewModeAndIconSize_orig,
            CDefView_SwitchToViewModeAndIconSize_hook,
            true,
        },
    };
    if (!WindhawkUtils::HookSymbols(shell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook CDefView symbols");
        return FALSE;
    }
    if (!CDefView_SwitchToViewModeAndIconSize_orig)
        Wh_Log(L"Icon size hook unavailable: size changes are reverted at the next activation");
    return TRUE;
}

void Wh_ModAfterInit() { ApplyToOpenWindows(false); }

void Wh_ModSettingsChanged() {
    LoadSettings();
    // A new size applies to the folders already open, in both modes.
    ApplyToOpenWindows(true);
}

void Wh_ModUninit() {
    std::vector<HWND> handled;
    {
        std::lock_guard<std::mutex> lock(g_handledMutex);
        handled.assign(g_handled.begin(), g_handled.end());
        g_handled.clear();
    }
    for (HWND hwnd : handled) WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ViewSubclassProc);
}

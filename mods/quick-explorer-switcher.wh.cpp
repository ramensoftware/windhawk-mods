// ==WindhawkMod==
// @id              quick-explorer-switcher
// @name            Quick Explorer Switcher
// @description     Shows the folders currently open in File Explorer (up to 3) as shortcuts in the Open/Save dialogs of any program
// @version         1.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         *
// @exclude         conhost.exe
// @exclude         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Explorer Switcher

When a program shows an **Open**, **Save As** or **Select Folder** dialog, this
mod adds the folders currently open in File Explorer (up to 3) to the bottom of
the dialog's navigation pane, most recently used first. One click and you're
there.

Optionally, the dialog can open directly in the most recently used File
Explorer folder.

![Showcase](https://i.imgur.com/3E1dyV3.jpeg)

## Details

* Folders are ordered by window Z-order, so the Explorer window you used last
  comes first.
* With Windows 11 File Explorer tabs, the active tab of each window takes
  precedence over the inactive ones.
* Duplicate folders are shown only once.
* Virtual locations such as "Home" or "This PC" are skipped by default (this
  can be changed in the settings).

## Limitations

* Works with the standard Windows file dialog (Vista and later). Not affected:
  programs that draw their own dialogs, and legacy-style dialogs (programs
  that customize `GetOpenFileName`/`GetSaveFileName` with a hook procedure or
  a dialog template get the older dialog, which has no navigation pane).
* A dialog that is already open when the mod is enabled won't show the
  suggestions; they appear the next time a dialog is opened.

If you don't want the mod in a specific program (for example, a browser that
already remembers its download folder), exclude it in the mod's **Advanced**
tab.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxSuggestions: 3
  $name: Maximum number of suggestions
  $description: From 1 to 3
- autoSwitch: false
  $name: Open the most recent folder automatically
  $description: >-
    If enabled, the dialog opens directly in the most recently used File
    Explorer folder. This overrides the folder chosen by the program (for
    example, "Save As" normally opening next to the original file).
- onlyFileSystem: true
  $name: Real folders only
  $description: Skip virtual locations such as "Home", "This PC" or "Recycle Bin"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commdlg.h>
#include <exdisp.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <algorithm>
#include <climits>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    int maxSuggestions;
    bool autoSwitch;
    bool onlyFileSystem;
} g_settings;

void LoadSettings() {
    g_settings.maxSuggestions =
        std::clamp(Wh_GetIntSetting(L"maxSuggestions"), 1, 3);

    g_settings.autoSwitch = Wh_GetIntSetting(L"autoSwitch") != 0;
    g_settings.onlyFileSystem = Wh_GetIntSetting(L"onlyFileSystem") != 0;
}

// ---------------------------------------------------------------------------
// Reading the folders open in File Explorer
// ---------------------------------------------------------------------------

// SID_STopLevelBrowser, defined here to avoid depending on SDK headers.
static const GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

struct FolderCandidate {
    IShellItem* item;
    int zOrder;      // 0 = topmost window
    bool activeTab;  // active tab of its window
};

static BOOL CALLBACK EnumWindowsZOrderProc(HWND hwnd, LPARAM lParam) {
    auto* map = reinterpret_cast<std::unordered_map<HWND, int>*>(lParam);
    int index = static_cast<int>(map->size());
    (*map)[hwnd] = index;
    return TRUE;
}

// Returns the current folder of an IShellWindows entry (caller releases it),
// or nullptr.
static IShellItem* ReadFolderFromShellWindow(IDispatch* disp,
                                             HWND* topWindow,
                                             bool* activeTab) {
    *topWindow = nullptr;
    *activeTab = true;

    IWebBrowserApp* browserApp = nullptr;
    if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp,
                                       (void**)&browserApp)) &&
        browserApp) {
        SHANDLE_PTR hwnd = 0;
        if (SUCCEEDED(browserApp->get_HWND(&hwnd))) {
            *topWindow = (HWND)hwnd;
        }
        browserApp->Release();
    }

    IShellItem* result = nullptr;

    IServiceProvider* serviceProvider = nullptr;
    if (FAILED(disp->QueryInterface(IID_IServiceProvider,
                                    (void**)&serviceProvider)) ||
        !serviceProvider) {
        return nullptr;
    }

    IShellBrowser* browser = nullptr;
    if (SUCCEEDED(serviceProvider->QueryService(
            kSID_STopLevelBrowser, IID_IShellBrowser, (void**)&browser)) &&
        browser) {
        IShellView* view = nullptr;
        if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
            HWND hwndView = nullptr;
            if (SUCCEEDED(view->GetWindow(&hwndView)) && hwndView) {
                // Inactive tabs (Windows 11) are hidden.
                *activeTab = IsWindowVisible(hwndView) != FALSE;
            }

            IFolderView* folderView = nullptr;
            if (SUCCEEDED(view->QueryInterface(IID_IFolderView,
                                               (void**)&folderView)) &&
                folderView) {
                IPersistFolder2* persistFolder = nullptr;
                if (SUCCEEDED(folderView->GetFolder(
                        IID_IPersistFolder2, (void**)&persistFolder)) &&
                    persistFolder) {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    if (SUCCEEDED(persistFolder->GetCurFolder(&pidl)) &&
                        pidl) {
                        SHCreateItemFromIDList(pidl, IID_IShellItem,
                                               (void**)&result);
                        CoTaskMemFree(pidl);
                    }
                    persistFolder->Release();
                }
                folderView->Release();
            }
            view->Release();
        }
        browser->Release();
    }
    serviceProvider->Release();

    if (result && g_settings.onlyFileSystem) {
        SFGAOF attributes = 0;
        if (FAILED(result->GetAttributes(SFGAO_FILESYSTEM, &attributes)) ||
            !(attributes & SFGAO_FILESYSTEM)) {
            result->Release();
            result = nullptr;
        }
    }

    return result;
}

// Returns up to maxCount folders, most recent first. The caller must Release()
// every returned item.
static std::vector<IShellItem*> GetExplorerFolders(int maxCount) {
    std::vector<IShellItem*> result;

    std::unordered_map<HWND, int> zOrder;
    EnumWindows(EnumWindowsZOrderProc, (LPARAM)&zOrder);

    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&shellWindows)) ||
        !shellWindows) {
        Wh_Log(L"IShellWindows is not available");
        return result;
    }

    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) {
        count = 0;
    }

    std::vector<FolderCandidate> candidates;
    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* disp = nullptr;
        if (shellWindows->Item(index, &disp) != S_OK || !disp) {
            continue;
        }

        HWND topWindow = nullptr;
        bool activeTab = true;
        IShellItem* item =
            ReadFolderFromShellWindow(disp, &topWindow, &activeTab);
        disp->Release();

        if (!item) {
            continue;
        }

        int z = INT_MAX;
        auto it = zOrder.find(topWindow);
        if (it != zOrder.end()) {
            z = it->second;
        }

        candidates.push_back({item, z, activeTab});
    }
    shellWindows->Release();

    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const FolderCandidate& a, const FolderCandidate& b) {
                         if (a.zOrder != b.zOrder) {
                             return a.zOrder < b.zOrder;
                         }
                         return a.activeTab && !b.activeTab;
                     });

    for (auto& candidate : candidates) {
        bool keep = static_cast<int>(result.size()) < maxCount;

        if (keep) {
            for (IShellItem* existing : result) {
                int order = 1;
                if (SUCCEEDED(existing->Compare(candidate.item,
                                                SICHINT_CANONICAL, &order)) &&
                    order == 0) {
                    keep = false;  // duplicate folder
                    break;
                }
            }
        }

        if (keep) {
            result.push_back(candidate.item);
        } else {
            candidate.item->Release();
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Main logic: runs right before the dialog is shown
// ---------------------------------------------------------------------------

static void OnBeforeFileDialogShow(IFileDialog* dialog) {
    if (!dialog) {
        return;
    }

    std::vector<IShellItem*> folders =
        GetExplorerFolders(g_settings.maxSuggestions);

    // Modern versions of Windows show custom places in a fixed spot of the
    // navigation pane, so FDAP_TOP vs FDAP_BOTTOM makes no visible difference.
    for (IShellItem* folder : folders) {
        dialog->AddPlace(folder, FDAP_TOP);
    }

    if (g_settings.autoSwitch && !folders.empty()) {
        dialog->SetFolder(folders[0]);
    }

    Wh_Log(L"Added %d folder(s) to the file dialog",
           static_cast<int>(folders.size()));

    for (IShellItem* folder : folders) {
        folder->Release();
    }
}

// ---------------------------------------------------------------------------
// IFileDialog::Show hook (vtable index 3, right after IUnknown)
// ---------------------------------------------------------------------------

using Show_t = HRESULT(WINAPI*)(IFileDialog*, HWND);

template <int N>
struct ShowSlot {
    static Show_t original;
    static HRESULT WINAPI Hook(IFileDialog* self, HWND owner) {
        OnBeforeFileDialogShow(self);
        return original(self, owner);
    }
};
template <int N>
Show_t ShowSlot<N>::original = nullptr;

constexpr int kMaxShowSlots = 4;
static void* const g_showHooks[kMaxShowSlots] = {
    (void*)ShowSlot<0>::Hook, (void*)ShowSlot<1>::Hook,
    (void*)ShowSlot<2>::Hook, (void*)ShowSlot<3>::Hook};
static Show_t* const g_showOriginals[kMaxShowSlots] = {
    &ShowSlot<0>::original, &ShowSlot<1>::original, &ShowSlot<2>::original,
    &ShowSlot<3>::original};
static void* g_showTargets[kMaxShowSlots] = {};
static int g_showSlotCount = 0;
static SRWLOCK g_showLock = SRWLOCK_INIT;

// Returns true if IFileDialog::Show of this object is hooked (now or before).
static bool CaptureShowFromObject(IUnknown* unknown) {
    if (!unknown) {
        return false;
    }

    IFileDialog* dialog = nullptr;
    if (FAILED(unknown->QueryInterface(IID_IFileDialog, (void**)&dialog)) ||
        !dialog) {
        return false;
    }
    void* showFunction = (*reinterpret_cast<void***>(dialog))[3];
    dialog->Release();

    // Only hook the real in-process implementation. If the object was created
    // from an MTA thread, COM returns a proxy whose slot 3 is a generic
    // stubless-proxy thunk shared by every marshaled interface in the process;
    // hooking it would redirect unrelated COM calls and crash the process.
    // The same check rejects any other wrapper or shim object.
    HMODULE comdlg32 = GetModuleHandleW(L"comdlg32.dll");
    HMODULE owner = nullptr;
    if (!comdlg32 ||
        !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)showFunction, &owner) ||
        owner != comdlg32) {
        Wh_Log(L"IFileDialog::Show at %p is not in comdlg32.dll, skipping",
               showFunction);
        return false;
    }

    bool hooked = false;

    AcquireSRWLockExclusive(&g_showLock);

    for (int i = 0; i < g_showSlotCount; i++) {
        if (g_showTargets[i] == showFunction) {
            hooked = true;
            break;
        }
    }

    if (!hooked && g_showSlotCount < kMaxShowSlots) {
        int slot = g_showSlotCount;
        if (Wh_SetFunctionHook(showFunction, g_showHooks[slot],
                               (void**)g_showOriginals[slot])) {
            g_showTargets[slot] = showFunction;
            g_showSlotCount++;
            Wh_ApplyHookOperations();
            Wh_Log(L"Hooked IFileDialog::Show (slot %d)", slot);
            hooked = true;
        }
    }

    ReleaseSRWLockExclusive(&g_showLock);

    return hooked;
}

static bool IsFileDialogClsid(REFCLSID clsid) {
    return IsEqualCLSID(clsid, CLSID_FileOpenDialog) ||
           IsEqualCLSID(clsid, CLSID_FileSaveDialog);
}

// ---------------------------------------------------------------------------
// CoCreateInstance / CoCreateInstanceEx hooks
// (programs that use IFileDialog directly)
// ---------------------------------------------------------------------------

using CoCreateInstance_t = HRESULT(WINAPI*)(REFCLSID,
                                            LPUNKNOWN,
                                            DWORD,
                                            REFIID,
                                            LPVOID*);
CoCreateInstance_t CoCreateInstance_Original;

HRESULT WINAPI CoCreateInstance_Hook(REFCLSID rclsid,
                                     LPUNKNOWN pUnkOuter,
                                     DWORD dwClsContext,
                                     REFIID riid,
                                     LPVOID* ppv) {
    HRESULT hr =
        CoCreateInstance_Original(rclsid, pUnkOuter, dwClsContext, riid, ppv);
    if (SUCCEEDED(hr) && ppv && *ppv && IsFileDialogClsid(rclsid)) {
        CaptureShowFromObject(reinterpret_cast<IUnknown*>(*ppv));
    }
    return hr;
}

using CoCreateInstanceEx_t = HRESULT(WINAPI*)(REFCLSID,
                                              IUnknown*,
                                              DWORD,
                                              COSERVERINFO*,
                                              DWORD,
                                              MULTI_QI*);
CoCreateInstanceEx_t CoCreateInstanceEx_Original;

HRESULT WINAPI CoCreateInstanceEx_Hook(REFCLSID rclsid,
                                       IUnknown* punkOuter,
                                       DWORD dwClsCtx,
                                       COSERVERINFO* pServerInfo,
                                       DWORD dwCount,
                                       MULTI_QI* pResults) {
    HRESULT hr = CoCreateInstanceEx_Original(rclsid, punkOuter, dwClsCtx,
                                             pServerInfo, dwCount, pResults);
    if (SUCCEEDED(hr) && pResults && IsFileDialogClsid(rclsid)) {
        for (DWORD i = 0; i < dwCount; i++) {
            if (SUCCEEDED(pResults[i].hr) && pResults[i].pItf) {
                CaptureShowFromObject(pResults[i].pItf);
                break;
            }
        }
    }
    return hr;
}

// ---------------------------------------------------------------------------
// GetOpenFileName / GetSaveFileName hooks
// (legacy API, which uses IFileDialog internally)
// ---------------------------------------------------------------------------

static volatile LONG g_showHookedViaTempInstance = 0;

static void EnsureShowHooked() {
    if (g_showHookedViaTempInstance) {
        return;
    }

    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (hrInit == RPC_E_CHANGED_MODE) {
        // MTA thread: CoCreateInstance would return a proxy, never the real
        // object, so there is nothing to hook from here. Try again on the
        // next call from an STA thread.
        return;
    }

    bool anyHooked = false;
    const CLSID* clsids[] = {&CLSID_FileOpenDialog, &CLSID_FileSaveDialog};
    for (const CLSID* clsid : clsids) {
        IUnknown* object = nullptr;
        if (SUCCEEDED(CoCreateInstance(*clsid, nullptr, CLSCTX_INPROC_SERVER,
                                       IID_IUnknown, (void**)&object)) &&
            object) {
            if (CaptureShowFromObject(object)) {
                anyHooked = true;
            }
            object->Release();
        }
    }

    if (SUCCEEDED(hrInit)) {
        CoUninitialize();
    }

    // Only stop trying once hooking actually succeeded.
    if (anyHooked) {
        InterlockedExchange(&g_showHookedViaTempInstance, 1);
    }
}

using GetFileNameW_t = BOOL(WINAPI*)(LPOPENFILENAMEW);
using GetFileNameA_t = BOOL(WINAPI*)(LPOPENFILENAMEA);

GetFileNameW_t GetOpenFileNameW_Original;
GetFileNameW_t GetSaveFileNameW_Original;
GetFileNameA_t GetOpenFileNameA_Original;
GetFileNameA_t GetSaveFileNameA_Original;

BOOL WINAPI GetOpenFileNameW_Hook(LPOPENFILENAMEW ofn) {
    EnsureShowHooked();
    return GetOpenFileNameW_Original(ofn);
}

BOOL WINAPI GetSaveFileNameW_Hook(LPOPENFILENAMEW ofn) {
    EnsureShowHooked();
    return GetSaveFileNameW_Original(ofn);
}

BOOL WINAPI GetOpenFileNameA_Hook(LPOPENFILENAMEA ofn) {
    EnsureShowHooked();
    return GetOpenFileNameA_Original(ofn);
}

BOOL WINAPI GetSaveFileNameA_Hook(LPOPENFILENAMEA ofn) {
    EnsureShowHooked();
    return GetSaveFileNameA_Original(ofn);
}

// ---------------------------------------------------------------------------
// Module hooking (also for modules loaded after startup)
// ---------------------------------------------------------------------------

static volatile bool g_comHooked = false;
static volatile bool g_comdlgHooked = false;
static SRWLOCK g_moduleLock = SRWLOCK_INIT;

static void TryHookModules(bool applyNow) {
    if (g_comHooked && g_comdlgHooked) {
        return;
    }

    AcquireSRWLockExclusive(&g_moduleLock);

    bool changed = false;

    if (!g_comHooked) {
        if (HMODULE module = GetModuleHandleW(L"combase.dll")) {
            if (void* p = (void*)GetProcAddress(module, "CoCreateInstance")) {
                Wh_SetFunctionHook(p, (void*)CoCreateInstance_Hook,
                                   (void**)&CoCreateInstance_Original);
            }
            if (void* p =
                    (void*)GetProcAddress(module, "CoCreateInstanceEx")) {
                Wh_SetFunctionHook(p, (void*)CoCreateInstanceEx_Hook,
                                   (void**)&CoCreateInstanceEx_Original);
            }
            g_comHooked = true;
            changed = true;
        }
    }

    if (!g_comdlgHooked) {
        if (HMODULE module = GetModuleHandleW(L"comdlg32.dll")) {
            struct {
                const char* name;
                void* hook;
                void** original;
            } hooks[] = {
                {"GetOpenFileNameW", (void*)GetOpenFileNameW_Hook,
                 (void**)&GetOpenFileNameW_Original},
                {"GetSaveFileNameW", (void*)GetSaveFileNameW_Hook,
                 (void**)&GetSaveFileNameW_Original},
                {"GetOpenFileNameA", (void*)GetOpenFileNameA_Hook,
                 (void**)&GetOpenFileNameA_Original},
                {"GetSaveFileNameA", (void*)GetSaveFileNameA_Hook,
                 (void**)&GetSaveFileNameA_Original},
            };
            for (auto& hook : hooks) {
                if (void* p = (void*)GetProcAddress(module, hook.name)) {
                    Wh_SetFunctionHook(p, hook.hook, hook.original);
                }
            }
            g_comdlgHooked = true;
            changed = true;
        }
    }

    if (changed && applyNow) {
        Wh_ApplyHookOperations();
    }

    ReleaseSRWLockExclusive(&g_moduleLock);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module &&
        !(dwFlags & (LOAD_LIBRARY_AS_DATAFILE |
                     LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                     LOAD_LIBRARY_AS_IMAGE_RESOURCE))) {
        TryHookModules(true);
    }
    return module;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    TryHookModules(false);

    if (HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll")) {
        if (void* p = (void*)GetProcAddress(kernelBase, "LoadLibraryExW")) {
            Wh_SetFunctionHook(p, (void*)LoadLibraryExW_Hook,
                               (void**)&LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

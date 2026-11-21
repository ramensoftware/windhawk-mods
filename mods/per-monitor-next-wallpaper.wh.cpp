// ==WindhawkMod==
// @id              per-monitor-next-wallpaper
// @name            Per-Monitor Next Desktop Background
// @description     Makes "Next desktop background" change the monitor you right-clicked, and adds an option to sync all monitors
// @version         1.0.0
// @author          Sp1d3rM0nk3y
// @github          https://github.com/andrew-brainerd
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Per-Monitor Next Desktop Background

With a slideshow wallpaper on several monitors, Windows' **Next desktop
background** menu item changes the monitors in turn, no matter which one you
right-clicked. This mod makes it change the monitor you right-clicked.

It also adds a **Sync desktop backgrounds** item, which copies the
right-clicked monitor's image to every other monitor. You can also set
**Next desktop background** to change all monitors to the same new image.

![Desktop context menu](https://raw.githubusercontent.com/andrew-brainerd/per-monitor-next-wallpaper/main/images/context-menu.png)

## How it works

Windows can't advance the slideshow on one chosen monitor:
`IDesktopWallpaper::AdvanceSlideshow` returns `E_NOTIMPL` for anything but
"the next monitor in turn". So the mod picks the image itself from the
slideshow folder (including subfolders), shuffling if the slideshow is set to
shuffle. It sets that image on the monitor, then turns the slideshow back on
with the same folder and settings.

## Limitations

- The slideshow timer still changes one monitor at a time, so synced monitors
  go out of sync at the next timed change.
- The mod uses its own shuffle, separate from Windows'. An image Windows
  showed recently can come up again.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- nextMode: clicked
  $name: Next desktop background changes
  $options:
  - clicked: The monitor you right-clicked
  - all: All monitors, to the same new image
- syncMenuItem: true
  $name: Show "Sync desktop backgrounds" menu item
  $description: Copies the right-clicked monitor's image to every other monitor
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <strsafe.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

enum class Action { Next, NextAll, Sync };

struct {
    bool nextAll;
    bool syncMenuItem;
} g_settings;

const CLSID kClsidSlideshowContextMenu = {
    0x0bf754aa,
    0xc967,
    0x445c,
    {0xab, 0x3d, 0xd8, 0xfd, 0xa9, 0xba, 0xe7, 0xef}};
const wchar_t kSyncMenuText[] = L"Sync desktop backgrounds";
const wchar_t kSyncHelpText[] =
    L"Copies this monitor's background to every other monitor";
const wchar_t kNextVerb[] = L"NextDesktopBackground";
const wchar_t kSyncVerb[] = L"SyncDesktopBackgrounds";

std::atomic<UINT> g_syncOffset{UINT_MAX};
std::atomic<DWORD> g_menuMessagePos{0};
std::mutex g_workMutex;

struct Monitor {
    std::wstring id;
    RECT rect;
    std::wstring wallpaper;
};

bool SamePath(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

std::wstring GetWallpaper(IDesktopWallpaper* dw, const std::wstring& id) {
    LPWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(dw->GetWallpaper(id.c_str(), &path)) && path) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

std::vector<Monitor> GetMonitors(IDesktopWallpaper* dw) {
    std::vector<Monitor> monitors;
    UINT count = 0;
    if (FAILED(dw->GetMonitorDevicePathCount(&count))) {
        return monitors;
    }
    for (UINT i = 0; i < count; i++) {
        LPWSTR id = nullptr;
        if (FAILED(dw->GetMonitorDevicePathAt(i, &id)) || !id) {
            continue;
        }
        Monitor monitor{id};
        CoTaskMemFree(id);
        // Disconnected monitors fail here.
        if (FAILED(dw->GetMonitorRECT(monitor.id.c_str(), &monitor.rect))) {
            continue;
        }
        monitor.wallpaper = GetWallpaper(dw, monitor.id);
        monitors.push_back(std::move(monitor));
    }
    return monitors;
}

int FindMonitorAt(const std::vector<Monitor>& monitors, POINT pt) {
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                       &info)) {
        for (size_t i = 0; i < monitors.size(); i++) {
            if (EqualRect(&monitors[i].rect, &info.rcMonitor)) {
                return (int)i;
            }
        }
    }
    for (size_t i = 0; i < monitors.size(); i++) {
        if (PtInRect(&monitors[i].rect, pt)) {
            return (int)i;
        }
    }
    return monitors.empty() ? -1 : 0;
}

bool IsImageFile(PCWSTR name) {
    static const PCWSTR extensions[] = {
        L".jpg",  L".jpeg", L".jpe",  L".jfif", L".png",  L".bmp",
        L".dib",  L".gif",  L".tif",  L".tiff", L".wdp",  L".jxr",
        L".heic", L".heif", L".webp", L".avif",
    };
    PCWSTR ext = PathFindExtensionW(name);
    return std::any_of(std::begin(extensions), std::end(extensions),
                       [&](PCWSTR e) { return _wcsicmp(ext, e) == 0; });
}

void CollectImages(const std::wstring& dir,
                   std::vector<std::wstring>& out,
                   int depth = 0) {
    if (depth > 16) {
        return;
    }
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic,
                                   &data, FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (data.dwFileAttributes &
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }
        std::wstring path = dir + L"\\" + data.cFileName;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(data.cFileName, L".") != 0 &&
                wcscmp(data.cFileName, L"..") != 0) {
                CollectImages(path, out, depth + 1);
            }
        } else if (IsImageFile(data.cFileName)) {
            out.push_back(std::move(path));
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
}

std::vector<std::wstring> GetSlideshowImages(IShellItemArray* slideshow) {
    std::vector<std::wstring> images;
    DWORD count = 0;
    if (FAILED(slideshow->GetCount(&count))) {
        return images;
    }
    for (DWORD i = 0; i < count; i++) {
        IShellItem* item = nullptr;
        if (FAILED(slideshow->GetItemAt(i, &item))) {
            continue;
        }
        LPWSTR path = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            DWORD attributes = GetFileAttributesW(path);
            if (attributes != INVALID_FILE_ATTRIBUTES &&
                (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
                CollectImages(path, images);
            } else if (IsImageFile(path)) {
                images.push_back(path);
            }
            CoTaskMemFree(path);
        }
        item->Release();
    }
    std::sort(images.begin(), images.end(),
              [](const std::wstring& a, const std::wstring& b) {
                  return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
              });
    images.erase(std::unique(images.begin(), images.end(), SamePath),
                 images.end());
    return images;
}

std::wstring PickNextImage(const std::vector<std::wstring>& images,
                           const std::wstring& current,
                           const std::vector<std::wstring>& avoid,
                           bool shuffle) {
    auto isAvoided = [&](const std::wstring& path) {
        return std::any_of(avoid.begin(), avoid.end(),
                           [&](const std::wstring& a) {
                               return SamePath(a, path);
                           });
    };

    std::vector<size_t> candidates;
    for (size_t i = 0; i < images.size(); i++) {
        if (!isAvoided(images[i])) {
            candidates.push_back(i);
        }
    }
    if (candidates.empty()) {
        for (size_t i = 0; i < images.size(); i++) {
            if (!SamePath(images[i], current)) {
                candidates.push_back(i);
            }
        }
    }
    if (candidates.empty()) {
        return {};
    }

    if (shuffle) {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<size_t> pick(0, candidates.size() - 1);
        return images[candidates[pick(rng)]];
    }

    auto currentIt = std::find_if(
        images.begin(), images.end(),
        [&](const std::wstring& path) { return SamePath(path, current); });
    if (currentIt != images.end()) {
        size_t currentIndex = currentIt - images.begin();
        for (size_t index : candidates) {
            if (index > currentIndex) {
                return images[index];
            }
        }
    }
    return images[candidates.front()];
}

bool WaitFor(const std::function<bool()>& condition, DWORD timeoutMs) {
    DWORD start = GetTickCount();
    while (!condition()) {
        if (GetTickCount() - start >= timeoutMs) {
            return false;
        }
        Sleep(100);
    }
    return true;
}

bool IsSlideshowActive(IDesktopWallpaper* dw) {
    DESKTOP_SLIDESHOW_STATE state{};
    return SUCCEEDED(dw->GetStatus(&state)) && (state & DSS_SLIDESHOW);
}

// Setting a wallpaper turns the slideshow off, so it is re-armed afterwards
// once Windows has finished switching it off.
void ApplyWallpapers(
    IDesktopWallpaper* dw,
    IShellItemArray* slideshow,
    const std::vector<std::pair<std::wstring, std::wstring>>& changes) {
    DESKTOP_SLIDESHOW_OPTIONS options{};
    UINT tick = 0;
    bool haveOptions = SUCCEEDED(dw->GetSlideshowOptions(&options, &tick));

    for (const auto& [id, path] : changes) {
        HRESULT hr = dw->SetWallpaper(id.c_str(), path.c_str());
        if (FAILED(hr)) {
            Wh_Log(L"SetWallpaper(%s) failed: 0x%08X", path.c_str(), hr);
        }
    }

    bool settled = WaitFor(
        [&] {
            if (IsSlideshowActive(dw)) {
                return false;
            }
            return std::all_of(changes.begin(), changes.end(),
                               [&](const auto& change) {
                                   return SamePath(
                                       GetWallpaper(dw, change.first),
                                       change.second);
                               });
        },
        5000);
    if (!settled) {
        Wh_Log(L"Timed out waiting for the wallpaper change to settle");
    }

    HRESULT hr = dw->SetSlideshow(slideshow);
    if (FAILED(hr)) {
        Wh_Log(L"SetSlideshow failed: 0x%08X", hr);
    }

    if (haveOptions) {
        DESKTOP_SLIDESHOW_OPTIONS newOptions{};
        UINT newTick = 0;
        if (SUCCEEDED(dw->GetSlideshowOptions(&newOptions, &newTick)) &&
            (newOptions != options || newTick != tick)) {
            dw->SetSlideshowOptions(options, tick);
        }
    }

    if (!WaitFor([&] { return IsSlideshowActive(dw); }, 5000)) {
        Wh_Log(L"Slideshow did not turn back on");
    }
}

void RunAction(Action action, POINT pt) {
    std::lock_guard lock(g_workMutex);

    IDesktopWallpaper* dw = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&dw));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(DesktopWallpaper) failed: 0x%08X", hr);
        return;
    }

    IShellItemArray* slideshow = nullptr;
    std::vector<Monitor> monitors = GetMonitors(dw);
    int target = FindMonitorAt(monitors, pt);
    std::vector<std::pair<std::wstring, std::wstring>> changes;

    if (!IsSlideshowActive(dw) || FAILED(dw->GetSlideshow(&slideshow)) ||
        target < 0) {
        Wh_Log(L"No active slideshow or monitor, using the default action");
        if (action != Action::Sync) {
            dw->AdvanceSlideshow(nullptr, DSD_FORWARD);
        }
    } else if (action == Action::Sync) {
        const std::wstring& source = monitors[target].wallpaper;
        for (const Monitor& monitor : monitors) {
            if (!source.empty() && !SamePath(monitor.wallpaper, source)) {
                changes.emplace_back(monitor.id, source);
            }
        }
    } else {
        DESKTOP_SLIDESHOW_OPTIONS options{};
        UINT tick = 0;
        dw->GetSlideshowOptions(&options, &tick);
        bool shuffle = options & DSO_SHUFFLEIMAGES;

        std::vector<std::wstring> showing;
        for (const Monitor& monitor : monitors) {
            showing.push_back(monitor.wallpaper);
        }

        std::wstring next =
            PickNextImage(GetSlideshowImages(slideshow),
                          monitors[target].wallpaper, showing, shuffle);
        if (next.empty()) {
            Wh_Log(L"No other image found, using the default action");
            dw->AdvanceSlideshow(nullptr, DSD_FORWARD);
        } else if (action == Action::NextAll) {
            for (const Monitor& monitor : monitors) {
                changes.emplace_back(monitor.id, next);
            }
        } else {
            changes.emplace_back(monitors[target].id, next);
        }
    }

    if (!changes.empty()) {
        ApplyWallpapers(dw, slideshow, changes);
    }

    if (slideshow) {
        slideshow->Release();
    }
    dw->Release();
}

struct WorkItem {
    Action action;
    POINT pt;
    HMODULE module;
};

DWORD WINAPI WorkerThread(void* param) {
    WorkItem item = *(WorkItem*)param;
    delete (WorkItem*)param;

    if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        RunAction(item.action, item.pt);
        CoUninitialize();
    }

    FreeLibraryAndExitThread(item.module, 0);
}

// The worker holds a reference to the mod DLL so it can't be unloaded while
// a wallpaper change is in progress.
bool StartWorker(Action action, POINT pt) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            (PCWSTR)&WorkerThread, &module)) {
        return false;
    }
    auto* item = new WorkItem{action, pt, module};
    HANDLE thread = CreateThread(nullptr, 0, WorkerThread, item, 0, nullptr);
    if (!thread) {
        delete item;
        FreeLibrary(module);
        return false;
    }
    CloseHandle(thread);
    return true;
}

std::optional<Action> ClassifyCommand(CMINVOKECOMMANDINFO* info) {
    Action next = g_settings.nextAll ? Action::NextAll : Action::Next;

    if (IS_INTRESOURCE(info->lpVerb)) {
        UINT offset = LOWORD((UINT_PTR)info->lpVerb);
        UINT syncOffset = g_syncOffset;
        if (offset == syncOffset) {
            return Action::Sync;
        }
        return offset < syncOffset ? std::optional(next) : std::nullopt;
    }

    std::wstring verb;
    if (info->cbSize >= sizeof(CMINVOKECOMMANDINFOEX) &&
        (info->fMask & CMIC_MASK_UNICODE)) {
        auto* infoEx = (CMINVOKECOMMANDINFOEX*)info;
        if (infoEx->lpVerbW && !IS_INTRESOURCE(infoEx->lpVerbW)) {
            verb = infoEx->lpVerbW;
        }
    }
    if (verb.empty()) {
        int length =
            MultiByteToWideChar(CP_ACP, 0, info->lpVerb, -1, nullptr, 0);
        if (length > 0) {
            verb.resize(length);
            MultiByteToWideChar(CP_ACP, 0, info->lpVerb, -1, verb.data(),
                                length);
            verb.resize(length - 1);
        }
    }

    if (_wcsicmp(verb.c_str(), kNextVerb) == 0) {
        return next;
    }
    if (_wcsicmp(verb.c_str(), kSyncVerb) == 0) {
        return Action::Sync;
    }
    return std::nullopt;
}

POINT GetInvokePoint(CMINVOKECOMMANDINFO* info) {
    if (info->cbSize >= sizeof(CMINVOKECOMMANDINFOEX) &&
        (info->fMask & CMIC_MASK_PTINVOKE)) {
        return ((CMINVOKECOMMANDINFOEX*)info)->ptInvoke;
    }
    DWORD pos = g_menuMessagePos;
    return {GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};
}

using QueryContextMenu_t = HRESULT(STDMETHODCALLTYPE*)(IContextMenu*,
                                                       HMENU,
                                                       UINT,
                                                       UINT,
                                                       UINT,
                                                       UINT);
QueryContextMenu_t QueryContextMenu_Original;

HRESULT STDMETHODCALLTYPE QueryContextMenu_Hook(IContextMenu* self,
                                                HMENU menu,
                                                UINT indexMenu,
                                                UINT idCmdFirst,
                                                UINT idCmdLast,
                                                UINT flags) {
    g_menuMessagePos = GetMessagePos();
    g_syncOffset = UINT_MAX;

    int countBefore = GetMenuItemCount(menu);
    HRESULT hr = QueryContextMenu_Original(self, menu, indexMenu, idCmdFirst,
                                           idCmdLast, flags);
    if (FAILED(hr) || !g_settings.syncMenuItem ||
        (flags & CMF_DEFAULTONLY)) {
        return hr;
    }

    UINT nextOffset = HRESULT_CODE(hr);
    int inserted = GetMenuItemCount(menu) - countBefore;
    if (nextOffset == 0 || inserted <= 0 ||
        idCmdFirst + nextOffset > idCmdLast) {
        return hr;
    }

    if (!InsertMenuW(menu, indexMenu + inserted, MF_BYPOSITION | MF_STRING,
                     idCmdFirst + nextOffset, kSyncMenuText)) {
        return hr;
    }
    g_syncOffset = nextOffset;
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, nextOffset + 1);
}

using GetCommandString_t = HRESULT(STDMETHODCALLTYPE*)(IContextMenu*,
                                                       UINT_PTR,
                                                       UINT,
                                                       UINT*,
                                                       CHAR*,
                                                       UINT);
GetCommandString_t GetCommandString_Original;

// The Windows 11 menu identifies items by verb, which the original handler
// can't supply for the added item.
HRESULT STDMETHODCALLTYPE GetCommandString_Hook(IContextMenu* self,
                                                UINT_PTR idCmd,
                                                UINT type,
                                                UINT* reserved,
                                                CHAR* name,
                                                UINT cchMax) {
    if (idCmd != g_syncOffset) {
        return GetCommandString_Original(self, idCmd, type, reserved, name,
                                         cchMax);
    }

    switch (type) {
        case GCS_VERBW:
            return StringCchCopyW((PWSTR)name, cchMax, kSyncVerb);
        case GCS_VERBA:
            return StringCchCopyA(name, cchMax, "SyncDesktopBackgrounds");
        case GCS_HELPTEXTW:
            return StringCchCopyW((PWSTR)name, cchMax, kSyncHelpText);
        case GCS_VALIDATEW:
        case GCS_VALIDATEA:
            return S_OK;
    }
    return E_INVALIDARG;
}

using InvokeCommand_t = HRESULT(STDMETHODCALLTYPE*)(IContextMenu*,
                                                    CMINVOKECOMMANDINFO*);
InvokeCommand_t InvokeCommand_Original;

HRESULT STDMETHODCALLTYPE InvokeCommand_Hook(IContextMenu* self,
                                             CMINVOKECOMMANDINFO* info) {
    std::optional<Action> action = info ? ClassifyCommand(info) : std::nullopt;
    if (!action) {
        return InvokeCommand_Original(self, info);
    }

    POINT pt = GetInvokePoint(info);
    if (!StartWorker(*action, pt)) {
        return *action == Action::Sync ? E_FAIL
                                       : InvokeCommand_Original(self, info);
    }
    return S_OK;
}

struct ContextMenuMethods {
    void* queryContextMenu;
    void* invokeCommand;
    void* getCommandString;
};

// Runs on its own thread so COM can be initialized as needed.
DWORD WINAPI FindContextMenuMethods(void* param) {
    auto* methods = (ContextMenuMethods*)param;
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IContextMenu* menu = nullptr;
    HRESULT hr =
        CoCreateInstance(kClsidSlideshowContextMenu, nullptr,
                         CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&menu));
    if (SUCCEEDED(hr)) {
        void** vtable = *(void***)menu;
        HMODULE module = nullptr;
        // Keep stobject.dll loaded for as long as the hooks exist.
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_PIN,
                               (PCWSTR)vtable[4], &module)) {
            methods->queryContextMenu = vtable[3];
            methods->invokeCommand = vtable[4];
            methods->getCommandString = vtable[5];
        }
        menu->Release();
    } else {
        Wh_Log(L"CoCreateInstance(SlideshowContextMenu) failed: 0x%08X", hr);
    }

    if (SUCCEEDED(hrInit)) {
        CoUninitialize();
    }
    return 0;
}

void LoadSettings() {
    PCWSTR nextMode = Wh_GetStringSetting(L"nextMode");
    g_settings.nextAll = wcscmp(nextMode, L"all") == 0;
    Wh_FreeStringSetting(nextMode);
    g_settings.syncMenuItem = Wh_GetIntSetting(L"syncMenuItem");
}

BOOL Wh_ModInit() {
    LoadSettings();

    ContextMenuMethods methods{};
    HANDLE thread =
        CreateThread(nullptr, 0, FindContextMenuMethods, &methods, 0, nullptr);
    if (!thread) {
        return FALSE;
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    if (!methods.queryContextMenu || !methods.invokeCommand ||
        !methods.getCommandString) {
        Wh_Log(L"Slideshow context menu handler not found");
        return FALSE;
    }

    Wh_SetFunctionHook(methods.queryContextMenu, (void*)QueryContextMenu_Hook,
                       (void**)&QueryContextMenu_Original);
    Wh_SetFunctionHook(methods.invokeCommand, (void*)InvokeCommand_Hook,
                       (void**)&InvokeCommand_Original);
    Wh_SetFunctionHook(methods.getCommandString, (void*)GetCommandString_Hook,
                       (void**)&GetCommandString_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

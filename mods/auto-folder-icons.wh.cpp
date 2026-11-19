// ==WindhawkMod==
// @id              auto-folder-icons
// @name            Automatic Folder Icons
// @description     Uses folder.ico or favicon.ico inside local folders without desktop.ini.
// @version         1.0.0
// @author          Sanekx Arcs
// @github          https://github.com/SanekxArcs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Automatic Folder Icons

Put a `folder.ico` or `favicon.ico` file directly inside a folder, and Explorer
shows it as the icon of that folder. Nothing else is needed: no `desktop.ini`,
no registry changes, no folder attributes, and the mod never writes to your
folders.

## How it works

* `folder.ico` takes priority. If it is missing or isn't a valid icon,
  `favicon.ico` is used, and then the original Windows icon.
* Hidden icon files work too.
* A folder that has an icon file shows the icon instead of a thumbnail preview.
* Adding, replacing or removing the icon file is picked up within a couple of
  seconds while an Explorer window or the desktop is in use. If Windows keeps a
  cached icon, press F5 or reopen the parent folder.

## Performance

* Files, including ZIP and CAB archives, are skipped without touching the
  disk. Only folders are checked, with two small file lookups per folder.
* Up to 256 recently shown folders are remembered and rechecked every two
  seconds, and only while Explorer is the active app and you are using the
  PC. Nothing is checked while you work in another app or are away, and the
  disk tree is never scanned.

## Limitations

* Only `explorer.exe` is affected. Other apps, including their open and save
  dialogs, show the original icons.
* Only local fixed and removable drives are supported. Network paths, virtual
  folders, reparse points, cloud placeholders, paths longer than about 245
  characters and icon files larger than 4 MiB are skipped.
* After enabling or disabling the mod, some icons may stay cached until
  Explorer is restarted. The mod never restarts Explorer by itself.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <thumbcache.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr size_t kMaxFolders = 256;
constexpr DWORD kPollMilliseconds = 2000;
constexpr ULONGLONG kCacheMilliseconds = 1000;
constexpr ULONGLONG kDriveCacheMilliseconds = 5000;
constexpr DWORD kMaxIconBytes = 4 * 1024 * 1024;
constexpr DWORD kIdleMilliseconds = 10000;
constexpr wchar_t kSeparator = L'\\';
// ZIP and CAB files report SFGAO_FOLDER too, together with SFGAO_STREAM.
constexpr SFGAOF kFolderMask = SFGAO_FOLDER | SFGAO_STREAM;
constexpr CLSID kThumbnailCacheClsid = {
    0x50ef4544, 0xac9f, 0x4a8e, {0xb2, 0x1b, 0x8a, 0x26, 0x18, 0x0d, 0xb1, 0x3f}};

struct IconState {
    std::wstring path;
    FILETIME modified{};
    DWORD size = 0;
    bool operator==(const IconState& other) const {
        return path == other.path && size == other.size &&
            CompareFileTime(&modified, &other.modified) == 0;
    }
};

struct CacheEntry {
    IconState icon;
    ULONGLONG checked = 0;
    ULONGLONG used = 0;
};

std::mutex g_cacheMutex;
std::unordered_map<std::wstring, CacheEntry> g_cache;
HANDLE g_stopEvent = nullptr;
HANDLE g_worker = nullptr;
std::atomic<bool> g_stopping = false;
// Set once any folder was given a custom icon.
std::atomic<bool> g_iconsReplaced = false;
thread_local bool g_resolving = false;
// Drive types by letter: (time << 8) | (type + 1), zero if not known yet.
std::atomic<ULONGLONG> g_driveTypes[26];

using GetUIObjectOfFunction = HRESULT(STDMETHODCALLTYPE*)(
    IShellFolder*, HWND, UINT, PCUITEMID_CHILD_ARRAY, REFIID, UINT*, void**);
using GetIconOfFunction = HRESULT(STDMETHODCALLTYPE*)(IShellIcon*, PCUITEMID_CHILD, UINT, int*);
using GetThumbnailFunction = HRESULT(STDMETHODCALLTYPE*)(
    IThumbnailCache*, IShellItem*, UINT, WTS_FLAGS, ISharedBitmap**,
    WTS_CACHEFLAGS*, WTS_THUMBNAILID*);
GetUIObjectOfFunction g_originalGetUIObjectOf = nullptr;
GetIconOfFunction g_originalGetIconOf = nullptr;
GetThumbnailFunction g_originalGetThumbnail = nullptr;

struct ResolutionGuard {
    ResolutionGuard() { g_resolving = true; }
    ~ResolutionGuard() { g_resolving = false; }
};

template <typename Interface>
class ComPointer {
public:
    Interface* pointer = nullptr;
    ComPointer() = default;
    ComPointer(const ComPointer&) = delete;
    ComPointer& operator=(const ComPointer&) = delete;
    ~ComPointer() {
        if (pointer) { pointer->Release(); }
    }
};

// Checks the path text only, without touching the disk.
bool IsSupportedPath(const std::wstring& path) {
    return path.size() >= 3 && path.size() + 12 < MAX_PATH &&
        path[1] == L':' && path[2] == kSeparator;
}

// GetDriveTypeW is slow, so the answer is remembered for a few seconds.
bool IsSupportedDrive(wchar_t letter) {
    unsigned index = (letter | 0x20) - L'a';
    if (index >= ARRAYSIZE(g_driveTypes)) { return false; }
    ULONGLONG now = GetTickCount64();
    ULONGLONG cached = g_driveTypes[index].load(std::memory_order_relaxed);
    UINT drive = 0;
    if ((cached & 0xFF) && now - (cached >> 8) < kDriveCacheMilliseconds) {
        drive = static_cast<UINT>(cached & 0xFF) - 1;
    } else {
        wchar_t root[] = {letter, L':', kSeparator, 0};
        drive = GetDriveTypeW(root);
        g_driveTypes[index].store((now << 8) | (drive + 1), std::memory_order_relaxed);
    }
    return drive == DRIVE_FIXED || drive == DRIVE_REMOVABLE;
}

bool IsSupportedFolder(const std::wstring& path) {
    if (!IsSupportedPath(path) || !IsSupportedDrive(path[0])) { return false; }
    DWORD attributes = GetFileAttributesW(path.c_str());
    constexpr DWORD excluded = FILE_ATTRIBUTE_REPARSE_POINT |
        FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS;
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY) && !(attributes & excluded);
}

// The caller is responsible for checking the folder with IsSupportedFolder.
IconState ProbeIcon(const std::wstring& folder, const IconState* previous = nullptr) {
    for (PCWSTR name : {L"folder.ico", L"favicon.ico"}) {
        std::wstring path = folder;
        if (path.back() != kSeparator) { path += kSeparator; }
        path += name;
        WIN32_FILE_ATTRIBUTE_DATA attributes{};
        constexpr DWORD excluded = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT |
            FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS;
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes) ||
            (attributes.dwFileAttributes & excluded) || attributes.nFileSizeHigh ||
            !attributes.nFileSizeLow || attributes.nFileSizeLow > kMaxIconBytes) { continue; }
        IconState state{path, attributes.ftLastWriteTime, attributes.nFileSizeLow};
        if (previous && state == *previous) { return state; }
        HICON icon = static_cast<HICON>(LoadImageW(
            nullptr, path.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
        if (icon) {
            DestroyIcon(icon);
            return state;
        }
    }
    return {};
}

void NotifyFolder(const std::wstring& folder, const IconState& oldIcon,
                  const IconState& newIcon) {
    if (!oldIcon.path.empty()) {
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW, oldIcon.path.c_str(), nullptr);
    }
    if (!newIcon.path.empty() && newIcon.path != oldIcon.path) {
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW, newIcon.path.c_str(), nullptr);
    }
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW, folder.c_str(), nullptr);
    std::wstring parent = folder;
    if (PathRemoveFileSpecW(parent.data())) {
        parent.resize(wcslen(parent.c_str()));
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, parent.c_str(), nullptr);
    }
}

IconState FindIcon(const std::wstring& folder) {
    // Only cheap checks before the cache lookup, the disk is touched on a miss.
    if (g_stopping || !IsSupportedPath(folder)) { return {}; }
    ULONGLONG now = GetTickCount64();
    IconState previous;
    {
        std::lock_guard lock(g_cacheMutex);
        auto found = g_cache.find(folder);
        if (found != g_cache.end()) {
            found->second.used = now;
            if (now - found->second.checked < kCacheMilliseconds) { return found->second.icon; }
            previous = found->second.icon;
        }
    }
    bool supported = IsSupportedFolder(folder);
    IconState icon = supported ? ProbeIcon(folder, &previous) : IconState{};
    bool changed = false;
    {
        std::lock_guard lock(g_cacheMutex);
        auto found = g_cache.find(folder);
        if (found != g_cache.end()) {
            if (found->second.checked > now) { return found->second.icon; }
            previous = found->second.icon;
            changed = !(previous == icon);
            found->second = {icon, now, now};
        } else if (supported) {
            if (g_cache.size() >= kMaxFolders) {
                auto oldest = std::min_element(g_cache.begin(), g_cache.end(),
                    [](const auto& left, const auto& right) {
                        return left.second.used < right.second.used;
                    });
                g_cache.erase(oldest);
            }
            g_cache.emplace(folder, CacheEntry{icon, now, now});
        }
    }
    if (!icon.path.empty()) { g_iconsReplaced = true; }
    if (changed) { NotifyFolder(folder, previous, icon); }
    return icon;
}

// Answered from the item identifier, without touching the disk. Files are the
// vast majority of the items Explorer asks about, and are rejected here.
bool IsFolderItem(IShellFolder* folder, PCUITEMID_CHILD child) {
    SFGAOF attributes = kFolderMask;
    return child && SUCCEEDED(folder->GetAttributesOf(1, &child, &attributes)) &&
        (attributes & kFolderMask) == SFGAO_FOLDER;
}

std::wstring ChildPath(IShellFolder* folder, PCUITEMID_CHILD child) {
    STRRET name{};
    wchar_t path[MAX_PATH]{};
    if (!child || FAILED(folder->GetDisplayNameOf(child, SHGDN_FORPARSING, &name)) ||
        FAILED(StrRetToBufW(&name, child, path, ARRAYSIZE(path)))) { return {}; }
    return path;
}

IconState ItemIcon(IShellItem* item) {
    SFGAOF attributes = 0;
    if (!item || FAILED(item->GetAttributes(kFolderMask, &attributes)) ||
        (attributes & kFolderMask) != SFGAO_FOLDER) { return {}; }
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) { return {}; }
    std::wstring folder;
    try { folder = path; }
    catch (...) {
        CoTaskMemFree(path);
        throw;
    }
    CoTaskMemFree(path);
    return FindIcon(folder);
}

HRESULT MakeExtractor(const IconState& icon, REFIID requested, void** result) {
    ComPointer<IDefaultExtractIconInit> initializer;
    HRESULT status = SHCreateDefaultExtractIcon(IID_PPV_ARGS(&initializer.pointer));
    if (FAILED(status)) { return status; }
    status = initializer.pointer->SetFlags(GIL_PERINSTANCE | GIL_DONTCACHE);
    if (SUCCEEDED(status)) { status = initializer.pointer->SetNormalIcon(icon.path.c_str(), 0); }
    if (SUCCEEDED(status)) { status = initializer.pointer->SetOpenIcon(icon.path.c_str(), 0); }
    return SUCCEEDED(status) ? initializer.pointer->QueryInterface(requested, result) : status;
}

HRESULT STDMETHODCALLTYPE GetUIObjectOfHook(
    IShellFolder* self, HWND owner, UINT count, PCUITEMID_CHILD_ARRAY children,
    REFIID requested, UINT* reserved, void** result) {
    if (!g_stopping && !g_resolving && count == 1 && children && result &&
        (requested == IID_IExtractIconW || requested == IID_IExtractIconA)) {
        ResolutionGuard guard;
        try {
            if (IsFolderItem(self, children[0])) {
                IconState icon = FindIcon(ChildPath(self, children[0]));
                if (!icon.path.empty() &&
                    SUCCEEDED(MakeExtractor(icon, requested, result))) { return S_OK; }
            }
        } catch (...) { Wh_Log(L"Icon extractor lookup failed; using the original icon"); }
    }
    return g_originalGetUIObjectOf(self, owner, count, children, requested, reserved, result);
}

HRESULT STDMETHODCALLTYPE GetIconOfHook(
    IShellIcon* self, PCUITEMID_CHILD child, UINT flags, int* result) {
    if (!g_stopping && !g_resolving && child && result) {
        ResolutionGuard guard;
        try {
            ComPointer<IShellFolder> folder;
            if (SUCCEEDED(self->QueryInterface(IID_PPV_ARGS(&folder.pointer))) &&
                IsFolderItem(folder.pointer, child)) {
                IconState icon = FindIcon(ChildPath(folder.pointer, child));
                if (!icon.path.empty()) {
                    // A direct lookup in the system image list, far cheaper than
                    // SHGetFileInfoW, which stays as the fallback.
                    int index = Shell_GetCachedImageIndexW(icon.path.c_str(), 0, 0);
                    if (index >= 0) {
                        *result = index;
                        return S_OK;
                    }
                    SHFILEINFOW information{};
                    if (SHGetFileInfoW(icon.path.c_str(), 0, &information,
                                      sizeof(information), SHGFI_SYSICONINDEX)) {
                        *result = information.iIcon;
                        return S_OK;
                    }
                }
            }
        } catch (...) { Wh_Log(L"Icon index lookup failed; using the original icon"); }
    }
    return g_originalGetIconOf(self, child, flags, result);
}

HRESULT STDMETHODCALLTYPE GetThumbnailHook(
    IThumbnailCache* self, IShellItem* item, UINT size, WTS_FLAGS flags,
    ISharedBitmap** thumbnail, WTS_CACHEFLAGS* cacheFlags, WTS_THUMBNAILID* thumbnailId) {
    if (!g_stopping && !g_resolving) {
        ResolutionGuard guard;
        try {
            if (!ItemIcon(item).path.empty()) {
                if (thumbnail) { *thumbnail = nullptr; }
                if (cacheFlags) { *cacheFlags = static_cast<WTS_CACHEFLAGS>(0); }
                if (thumbnailId) { *thumbnailId = {}; }
                return WTS_E_FAILEDEXTRACTION;
            }
        } catch (...) { Wh_Log(L"Thumbnail lookup failed; using the original thumbnail"); }
    }
    return g_originalGetThumbnail(self, item, size, flags, thumbnail, cacheFlags, thumbnailId);
}

// True if the foreground window belongs to this process, i.e. the user is
// looking at Explorer or the desktop.
bool IsExplorerInForeground() {
    DWORD process = 0;
    HWND window = GetForegroundWindow();
    return window && GetWindowThreadProcessId(window, &process) &&
        process == GetCurrentProcessId();
}

// True if there was no keyboard or mouse input for a while, for example when
// the PC is left at the desktop.
bool IsUserIdle() {
    LASTINPUTINFO input{sizeof(input)};
    return GetLastInputInfo(&input) && GetTickCount() - input.dwTime > kIdleMilliseconds;
}

void PollFolders() {
    // Don't touch the disks while the user is working in another app or is
    // away. Changes are picked up within one cycle after returning.
    if (!IsExplorerInForeground() || IsUserIdle()) { return; }
    std::vector<std::pair<std::wstring, CacheEntry>> snapshot;
    {
        std::lock_guard lock(g_cacheMutex);
        snapshot.assign(g_cache.begin(), g_cache.end());
    }
    for (const auto& [folder, entry] : snapshot) {
        if (g_stopping) { break; }
        ULONGLONG started = GetTickCount64();
        IconState icon = ProbeIcon(folder, &entry.icon);
        bool changed = false;
        {
            std::lock_guard lock(g_cacheMutex);
            auto found = g_cache.find(folder);
            if (found == g_cache.end() || found->second.checked != entry.checked) { continue; }
            changed = !(found->second.icon == icon);
            found->second.icon = icon;
            found->second.checked = started;
        }
        if (changed) { NotifyFolder(folder, entry.icon, icon); }
    }
}

DWORD WINAPI WatchFolders(void*) {
    HRESULT status = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(status)) {
        Wh_Log(L"Background COM initialization failed");
        return 1;
    }
    while (WaitForSingleObject(g_stopEvent, kPollMilliseconds) == WAIT_TIMEOUT) {
        try { PollFolders(); }
        catch (...) { Wh_Log(L"Folder polling failed; retrying next cycle"); }
    }
    CoUninitialize();
    return 0;
}

// True during the first seconds of the process. At that point Explorer hasn't
// shown any folder yet, so there are no icons to refresh.
bool IsProcessJustStarted() {
    FILETIME created{}, exited{}, kernel{}, user{}, now{};
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
        return false;
    }
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER start{}, current{};
    start.LowPart = created.dwLowDateTime;
    start.HighPart = created.dwHighDateTime;
    current.LowPart = now.dwLowDateTime;
    current.HighPart = now.dwHighDateTime;
    constexpr ULONGLONG tenSeconds = 10ULL * 10000000ULL;
    return current.QuadPart - start.QuadPart < tenSeconds;
}

bool InstallHook(void* target, void* replacement, void** original) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<PCWSTR>(target), &module)) { return false; }
    return Wh_SetFunctionHook(target, replacement, original);
}

bool InitializeHooks() {
    wchar_t windowsPath[MAX_PATH]{};
    if (!GetWindowsDirectoryW(windowsPath, ARRAYSIZE(windowsPath))) { return false; }
    PIDLIST_ABSOLUTE windowsId = nullptr;
    HRESULT status = SHParseDisplayName(windowsPath, nullptr, &windowsId, 0, nullptr);
    if (FAILED(status)) { return false; }
    ComPointer<IShellFolder> desktop;
    ComPointer<IShellFolder> folder;
    status = SHGetDesktopFolder(&desktop.pointer);
    if (SUCCEEDED(status)) {
        status = desktop.pointer->BindToObject(windowsId, nullptr, IID_PPV_ARGS(&folder.pointer));
    }
    CoTaskMemFree(windowsId);
    if (FAILED(status)) { return false; }
    ComPointer<IShellIcon> shellIcon;
    status = folder.pointer->QueryInterface(IID_PPV_ARGS(&shellIcon.pointer));
    if (FAILED(status)) { return false; }
    void** folderTable = *reinterpret_cast<void***>(folder.pointer);
    void** iconTable = *reinterpret_cast<void***>(shellIcon.pointer);
    if (!InstallHook(folderTable[10], reinterpret_cast<void*>(GetUIObjectOfHook),
                     reinterpret_cast<void**>(&g_originalGetUIObjectOf)) ||
        !InstallHook(iconTable[3], reinterpret_cast<void*>(GetIconOfHook),
                     reinterpret_cast<void**>(&g_originalGetIconOf))) { return false; }
    ComPointer<IThumbnailCache> thumbnailCache;
    status = CoCreateInstance(kThumbnailCacheClsid, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&thumbnailCache.pointer));
    if (FAILED(status)) { return false; }
    void** thumbnailTable = *reinterpret_cast<void***>(thumbnailCache.pointer);
    return InstallHook(thumbnailTable[3], reinterpret_cast<void*>(GetThumbnailHook),
                       reinterpret_cast<void**>(&g_originalGetThumbnail));
}

void StopWorker() {
    g_stopping = true;
    if (g_stopEvent) { SetEvent(g_stopEvent); }
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}

BOOL Wh_ModInit() {
    HRESULT status = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(status) && status != RPC_E_CHANGED_MODE) { return FALSE; }
    bool initialized = false;
    try { initialized = InitializeHooks(); }
    catch (...) { Wh_Log(L"COM hook initialization failed"); }
    if (SUCCEEDED(status)) { CoUninitialize(); }
    if (!initialized) {
        Wh_Log(L"Required shell hooks unavailable; mod was not enabled");
        return FALSE;
    }
    g_stopping = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) { g_worker = CreateThread(nullptr, 0, WatchFolders, nullptr, 0, nullptr); }
    if (!g_worker) {
        StopWorker();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // Refreshing every icon is expensive and is only needed if the mod was
    // enabled while Explorer was already showing folders, not at sign-in.
    if (!IsProcessJustStarted()) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

void Wh_ModBeforeUninit() { StopWorker(); }

void Wh_ModUninit() {
    StopWorker();
    std::vector<std::pair<std::wstring, CacheEntry>> snapshot;
    {
        std::lock_guard lock(g_cacheMutex);
        snapshot.assign(g_cache.begin(), g_cache.end());
        g_cache.clear();
    }
    for (const auto& [folder, entry] : snapshot) {
        if (!entry.icon.path.empty()) { NotifyFolder(folder, entry.icon, {}); }
    }
    // Nothing to restore if no folder icon was ever replaced.
    if (g_iconsReplaced) {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

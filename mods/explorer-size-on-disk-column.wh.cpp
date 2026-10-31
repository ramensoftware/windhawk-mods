// ==WindhawkMod==
// @id              explorer-size-on-disk-column
// @name            Size on disk column in Explorer details
// @description     Adds a "Size on disk" column to File Explorer's details view for files and folders
// @version         0.7.0
// @author          stoilms
// @github          https://github.com/stoilms
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lpropsys -lshlwapi -luuid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Structure and the windows.storage.dll symbol hooks are based on m417z's
// "Better file sizes in Explorer details" mod:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-details-better-file-sizes.wh.cpp

// ==WindhawkModReadme==
/*
# Size on disk column in Explorer details

Adds a **Size on disk** column to File Explorer's details view. The value 
matches the "Size on disk" figure in a file or folder's Properties dialog as 
closely as possible.

![Size on disk column showing sizes for files and folders](https://raw.githubusercontent.com/stoilms/windhawk-explorer-size-on-disk-column/main/images/screenshot2.png)

## How to use

1. Enable the mod and restart Explorer (or sign out and back in).
2. Open a folder in **Details** view.
3. Right-click any column header and tick **Size on disk**. 
If it isn't in the short list, click **More...** and find it there.

![Size on disk in the column header menu](https://raw.githubusercontent.com/stoilms/windhawk-explorer-size-on-disk-column/main/images/screenshot1.png)

The values match the Properties dialog:

![Properties dialog showing the same size on disk](https://raw.githubusercontent.com/stoilms/windhawk-explorer-size-on-disk-column/main/images/screenshot3.png)

## How the value is calculated

* **Files:** the file's allocation size, as reported by the file system. For
  NTFS-compressed, sparse and CompactOS/WOF-compressed files, the compressed
  size rounded up to the volume's cluster size is used instead. Cloud
  placeholders (OneDrive, etc.) report only what is stored locally and are
  never downloaded.
* **Folders:** the sum of the size on disk of every file underneath,
  calculated manually by walking the folder tree. Junctions and symbolic links
  are not followed: they aren't counted inside a folder, and show no value
  themselves. OneDrive and other cloud folders are walked.

## Notes

* Folder calculation can be slow for large trees, so it always runs in the
  background, at low priority and at most two folders at a time. A folder's
  value appears as soon as its calculation finishes. Calculating a
  folder also caches its subfolders (all of them, or only the direct ones -
  see **Remember subfolder sizes**), so browsing into them is instant.
  Cached values are shown immediately and refreshed in the background.
* Cloud folders whose contents aren't on this PC yet are counted as 0 bytes
  without being listed, so OneDrive isn't asked to fetch anything.
* Network drives are skipped by default and the column stays empty there,
  because querying them can make Explorer stop responding on slow
  connections. Turn on **Calculate sizes on network drives** to include them.
* Only regular file-system folders are supported. Libraries, search results,
  zip folders and the Recycle Bin are not.
* Hard links are counted once per link, as the Properties dialog does.

## Showing the column in every folder

With **Add to default folder layouts** enabled, the column is added to
Explorer's built-in templates for regular file folders (general items,
documents, pictures, music, videos, downloads, the user folder and OneDrive).
Other views, such as Home, Gallery, libraries and search results, are left
alone. Templates only apply to folders that
don't have saved view settings, so after enabling it either reset saved views
(Folder Options > View > **Reset Folders**) or set the column up in one folder
and use Folder Options > View > **Apply to Folders** for each folder type.

## How it works

Windows already defines a hidden System.FileAllocationSize property, and
Explorer's own getter for it (CFSFolder::_GetFileAllocationSize) just returns
the logical size. The mod exposes the property as a column and replaces that
getter with a real size on disk calculation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folderSizes: exceptSystemFolders
  $name: Show folder sizes
  $description: >-
    Folder sizes are calculated by walking the whole folder tree, which can be
    slow for large folders. Excluding system folders skips the Windows,
    Program Files and ProgramData folders and everything in them, which hold
    hundreds of thousands of files; they show no value.
  $options:
  - always: Enabled, calculated manually (can be slow)
  - exceptSystemFolders: Enabled, except system folders
  - disabled: Disabled (files only)
- folderMethod: accurate
  $name: Folder calculation method
  $description: >-
    Accurate asks the file system about every file, matching the Properties
    dialog. Fast reads sizes from directory listings, which is quicker but can
    be off for very small files and cloud files.
  $options:
  - accurate: Accurate (matches Properties)
  - fast: Fast (directory listings)
- networkDrives: false
  $name: Calculate sizes on network drives
  $description: >-
    Files and folders on network drives can be slow to query, and Explorer
    may stop responding while it waits, for example when sorting by this
    column. When off, the column stays empty on network drives.
- mixFoldersWhenSorting: false
  $name: Mix files and folders when sorting by size on disk
- addToDefaultColumns: true
  $name: Add to default folder layouts
  $description: >-
    Adds the column after Size in Explorer's folder templates. Only affects
    folders without saved view settings - see the mod description.
- refreshSeconds: 3600
  $name: Folder refresh interval (seconds)
  $description: >-
    A folder's value is shown from the cache straight away. If it's older than
    this, it's also recalculated in the background and updated if it changed.
    The default is one hour (3600 seconds).
- subfolderCache: all
  $name: Remember subfolder sizes
  $description: >-
    Calculating a folder also finds the size of every folder inside it.
    Remembering all of them makes browsing deeper instant, but uses more
    memory in Explorer (up to about 30 MB). With direct subfolders only,
    deeper folders are calculated again when you open them.
  $options:
  - all: All subfolders
  - directOnly: Direct subfolders only
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <propsys.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wrl/client.h>

#ifndef IO_REPARSE_TAG_WOF
#define IO_REPARSE_TAG_WOF 0x80000017L
#endif
#ifndef IsReparseTagNameSurrogate
#define IsReparseTagNameSurrogate(tag) (((tag)&0x20000000))
#endif
#ifndef FILE_ATTRIBUTE_RECALL_ON_OPEN
#define FILE_ATTRIBUTE_RECALL_ON_OPEN 0x00040000
#endif

using namespace std::string_view_literals;

////////////////////////////////////////////////////////////////////////////////
// Settings and shared state

enum class FolderSizes { always, exceptSystemFolders, disabled };

struct {
    FolderSizes folderSizes;
    bool accurateFolders;
    bool networkDrives;
    bool mixFoldersWhenSorting;
    bool addToDefaultColumns;
    ULONGLONG cacheMs;
    bool cacheAllSubfolders;
} g_settings;

constexpr GUID kFmtStorage = {0xB725F130,
                              0x47EF,
                              0x101A,
                              {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPKEY_Size = {kFmtStorage, 12};
// System.FileAllocationSize: defined by Windows but hidden from the column
// picker (IsColumn = false, no label, no display format).
constexpr PROPERTYKEY kPKEY_SizeOnDisk = {kFmtStorage, 18};

constexpr WCHAR kColumnTitle[] = L"Size on disk";
constexpr UINT kColumnWidthChars = 12;

std::atomic<int> g_hookRefCount;
extern std::atomic<bool> g_stopping;

auto HookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto refCount) { (*refCount)--; }};
}

// Logs only the first few times a code path is reached, so the log shows which
// hooks Explorer actually uses without being flooded by every call.
bool FirstHits(std::atomic<int>& counter, int limit = 3) {
    return counter < limit && counter++ < limit;
}

////////////////////////////////////////////////////////////////////////////////
// Path helpers

bool IsFileSystemPath(std::wstring_view path) {
    bool driveLetter = path.size() >= 3 && path[1] == L':' && path[2] == L'\\';
    bool unc = path.size() >= 3 && path.starts_with(L"\\\\"sv) &&
               !path.starts_with(L"\\\\?\\"sv);
    return driveLetter || unc;
}

std::wstring ToExtendedPath(const std::wstring& path) {
    if (path.starts_with(L"\\\\?\\"sv)) {
        return path;
    }
    if (path.starts_with(L"\\\\"sv)) {
        return L"\\\\?\\UNC\\" + path.substr(2);
    }
    return L"\\\\?\\" + path;
}

std::wstring JoinPath(const std::wstring& dir, std::wstring_view name) {
    std::wstring result = dir;
    if (!result.empty() && result.back() != L'\\') {
        result += L'\\';
    }
    result += name;
    return result;
}

bool IsNetworkPath(const std::wstring& path) {
    if (path.starts_with(L"\\\\"sv)) {
        return true;
    }
    if (path.size() >= 2 && path[1] == L':') {
        WCHAR root[] = {path[0], L':', L'\\', L'\0'};
        return GetDriveTypeW(root) == DRIVE_REMOTE;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Size on disk calculation

ULONGLONG GetClusterSize(const std::wstring& path) {
    std::wstring root;
    if (path.size() >= 3 && path[1] == L':') {
        root = path.substr(0, 3);
    } else {
        WCHAR volume[MAX_PATH];
        if (!GetVolumePathNameW(path.c_str(), volume, ARRAYSIZE(volume))) {
            return 0;
        }
        root = volume;
    }

    static std::mutex mutex;
    static std::unordered_map<std::wstring, ULONGLONG> cache;

    std::lock_guard lock(mutex);
    if (auto it = cache.find(root); it != cache.end()) {
        return it->second;
    }

    DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
    ULONGLONG clusterSize = 0;
    if (GetDiskFreeSpaceW(root.c_str(), &sectorsPerCluster, &bytesPerSector,
                          &freeClusters, &totalClusters)) {
        clusterSize = (ULONGLONG)sectorsPerCluster * bytesPerSector;
    }
    cache[root] = clusterSize;
    return clusterSize;
}

ULONGLONG RoundUp(ULONGLONG value, ULONGLONG granularity) {
    if (!granularity) {
        return value;
    }
    return (value + granularity - 1) / granularity * granularity;
}

// Allocation size as the Properties dialog reports it:
// * Compressed, sparse and WOF (CompactOS) files: the compressed size rounded
//   up to a whole cluster, since the main stream's allocation doesn't reflect
//   what's really on disk.
// * Everything else: the allocation rounded down to whole clusters. Real
//   allocations are always whole clusters; anything smaller is a tiny file
//   stored inside the file table itself, which Properties counts as 0 bytes.
ULONGLONG AdjustAllocationSize(const std::wstring& path,
                               DWORD attributes,
                               DWORD reparseTag,
                               ULONGLONG allocationSize,
                               ULONGLONG clusterSize) {
    bool needsCompressedSize =
        (attributes &
         (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_SPARSE_FILE)) ||
        ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
         reparseTag == IO_REPARSE_TAG_WOF);

    if (needsCompressedSize) {
        DWORD high = 0;
        DWORD low = GetCompressedFileSizeW(ToExtendedPath(path).c_str(), &high);
        if (low != INVALID_FILE_SIZE || GetLastError() == NO_ERROR) {
            ULONGLONG compressed = ((ULONGLONG)high << 32) | low;
            return RoundUp(compressed, clusterSize);
        }
    }

    if (clusterSize) {
        return allocationSize / clusterSize * clusterSize;
    }
    return allocationSize;
}

// FILE_STAT_INFORMATION, returned by GetFileInformationByName (Windows 11
// 24H2+) for FileStatByNameInfo. Reads a file's sizes without opening it.
struct FileStatInformation {
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
};
constexpr int kFileStatByNameInfo = 0;
using GetFileInformationByName_t = BOOL(WINAPI*)(PCWSTR fileName,
                                                 int infoClass,
                                                 PVOID buffer,
                                                 ULONG bufferSize);
GetFileInformationByName_t g_pGetFileInformationByName;

struct RawAllocation {
    DWORD attributes;
    DWORD reparseTag;
    ULONGLONG allocationSize;
};

std::optional<RawAllocation> ReadAllocationByHandle(const std::wstring& path) {
    // FILE_FLAG_OPEN_REPARSE_POINT avoids following symlinks and avoids
    // triggering cloud file downloads.
    HANDLE file = CreateFileW(
        ToExtendedPath(path).c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    FILE_ATTRIBUTE_TAG_INFO tagInfo{};
    FILE_STANDARD_INFO standard{};
    bool ok = GetFileInformationByHandleEx(file, FileAttributeTagInfo,
                                           &tagInfo, sizeof(tagInfo)) &&
              GetFileInformationByHandleEx(file, FileStandardInfo, &standard,
                                           sizeof(standard));
    CloseHandle(file);
    if (!ok) {
        return std::nullopt;
    }

    return RawAllocation{tagInfo.FileAttributes, tagInfo.ReparseTag,
                         (ULONGLONG)standard.AllocationSize.QuadPart};
}

std::optional<RawAllocation> ReadAllocationByName(const std::wstring& path) {
    if (!g_pGetFileInformationByName) {
        return std::nullopt;
    }

    FileStatInformation stat{};
    if (!g_pGetFileInformationByName(ToExtendedPath(path).c_str(),
                                     kFileStatByNameInfo, &stat,
                                     sizeof(stat))) {
        return std::nullopt;
    }

    return RawAllocation{stat.FileAttributes, stat.ReparseTag,
                         (ULONGLONG)stat.AllocationSize.QuadPart};
}

std::optional<RawAllocation> ReadAllocation(const std::wstring& path) {
    auto byName = ReadAllocationByName(path);
    return byName ? byName : ReadAllocationByHandle(path);
}

std::optional<ULONGLONG> GetFileSizeOnDisk(const std::wstring& path,
                                           ULONGLONG clusterSize) {
    auto raw = ReadAllocation(path);
    if (!raw) {
        return std::nullopt;
    }
    return AdjustAllocationSize(path, raw->attributes, raw->reparseTag,
                                raw->allocationSize, clusterSize);
}

// Junctions and directory symlinks are never followed.
bool IsFolderLink(DWORD attributes, DWORD reparseTag) {
    return (attributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
           IsReparseTagNameSurrogate(reparseTag);
}

// Cloud folders whose contents aren't on this PC yet hold 0 bytes locally,
// and listing them would make OneDrive fetch the listing from the internet.
bool IsOnlineOnlyFolder(DWORD attributes) {
    return attributes & FILE_ATTRIBUTE_RECALL_ON_OPEN;
}

// Walks a folder tree. Returns the total, and the subfolders' own totals via
// `subfolderTotals` (all of them, or only the direct subfolders, depending on
// the setting), so browsing into a subfolder afterwards is instant.
std::optional<ULONGLONG> GetFolderSizeOnDisk(
    const std::wstring& root,
    std::vector<std::pair<std::wstring, ULONGLONG>>* subfolderTotals) {
    // The folder itself follows the same rules as its subfolders. It's
    // checked here as well as by the caller, because cached values are
    // refreshed by walking the folder directly.
    if (auto raw = ReadAllocation(root)) {
        if (IsFolderLink(raw->attributes, raw->reparseTag)) {
            return std::nullopt;
        }
        if (IsOnlineOnlyFolder(raw->attributes)) {
            return 0;
        }
    }

    ULONGLONG clusterSize = GetClusterSize(root);

    // ULONGLONG elements keep the buffer 8-byte aligned, as required.
    std::vector<ULONGLONG> buffer(64 * 1024 / sizeof(ULONGLONG));
    const DWORD bufferBytes = (DWORD)(buffer.size() * sizeof(ULONGLONG));

    struct Node {
        std::wstring path;
        size_t parent;
        ULONGLONG total;
    };
    constexpr size_t kNoParent = (size_t)-1;
    std::vector<Node> nodes{{root, kNoParent, 0}};
    std::vector<size_t> pending{0};

    while (!pending.empty()) {
        if (g_stopping) {
            return std::nullopt;
        }

        size_t index = pending.back();
        pending.pop_back();
        std::wstring dir = nodes[index].path;

        HANDLE handle = CreateFileW(
            ToExtendedPath(dir).c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            if (index == 0) {
                return std::nullopt;
            }
            continue;  // Access denied etc. - skip, as Properties does.
        }

        // Also check the stop flag for every file, so a folder with many files
        // doesn't delay unloading.
        FILE_INFO_BY_HANDLE_CLASS infoClass = FileFullDirectoryRestartInfo;
        while (!g_stopping &&
               GetFileInformationByHandleEx(handle, infoClass, buffer.data(),
                                            bufferBytes)) {
            infoClass = FileFullDirectoryInfo;

            auto* entry = reinterpret_cast<FILE_FULL_DIR_INFO*>(buffer.data());
            while (!g_stopping) {
                std::wstring_view name(entry->FileName,
                                       entry->FileNameLength / sizeof(WCHAR));
                DWORD attributes = entry->FileAttributes;

                // For reparse points, EaSize holds the reparse tag.
                DWORD reparseTag = (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
                                       ? entry->EaSize
                                       : 0;

                if (name != L"."sv && name != L".."sv) {
                    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
                        if (IsFolderLink(attributes, reparseTag)) {
                            // Not counted, not cached.
                        } else if (IsOnlineOnlyFolder(attributes)) {
                            // Cached as 0 bytes without listing it.
                            nodes.push_back({JoinPath(dir, name), index, 0});
                        } else {
                            nodes.push_back({JoinPath(dir, name), index, 0});
                            pending.push_back(nodes.size() - 1);
                        }
                    } else {
                        std::wstring path = JoinPath(dir, name);
                        ULONGLONG size =
                            g_settings.accurateFolders
                                ? GetFileSizeOnDisk(path, clusterSize)
                                      .value_or(0)
                                : AdjustAllocationSize(
                                      path, attributes, reparseTag,
                                      entry->AllocationSize.QuadPart,
                                      clusterSize);
                        nodes[index].total += size;
                    }
                }

                if (!entry->NextEntryOffset) {
                    break;
                }
                entry = reinterpret_cast<FILE_FULL_DIR_INFO*>(
                    reinterpret_cast<BYTE*>(entry) + entry->NextEntryOffset);
            }
        }

        CloseHandle(handle);
    }

    // A walk stopped part-way through has only a partial total.
    if (g_stopping) {
        return std::nullopt;
    }

    // Children always come after their parent, so summing from the end rolls
    // every subtree up into its parent.
    for (size_t i = nodes.size() - 1; i > 0; i--) {
        nodes[nodes[i].parent].total += nodes[i].total;
    }

    // The root's direct subfolders always come first, as the root is listed
    // before any of them.
    if (subfolderTotals) {
        for (size_t i = 1; i < nodes.size(); i++) {
            if (!g_settings.cacheAllSubfolders && nodes[i].parent != 0) {
                break;
            }
            subfolderTotals->emplace_back(std::move(nodes[i].path),
                                          nodes[i].total);
        }
    }

    return nodes[0].total;
}

////////////////////////////////////////////////////////////////////////////////
// Cache
//
// Values are shown from the cache immediately, even when they're old. Old
// folder values are recalculated in the background, and Explorer is only
// asked to redraw an item when its value actually changed.

struct ItemSize {
    std::optional<ULONGLONG> size;
    bool isFolder = false;
};

struct CacheEntry {
    ItemSize item;
    ULONGLONG tick;
};

struct CacheLookup {
    ItemSize item;
    bool fresh;
};

constexpr size_t kMaxCacheEntries = 100000;

// File values are cheap to read again, so they're only reused for a few
// seconds: long enough for sorting, which asks for every item many times,
// but short enough that a changed file shows its new size after a refresh.
constexpr ULONGLONG kFileCacheMs = 5000;

std::mutex g_cacheMutex;
std::unordered_map<std::wstring, CacheEntry> g_cache;

// Background folder calculations.
std::atomic<bool> g_stopping;
std::mutex g_pendingMutex;
std::unordered_set<std::wstring> g_pendingPaths;

std::optional<CacheLookup> LookupCache(const std::wstring& path) {
    std::lock_guard lock(g_cacheMutex);
    auto it = g_cache.find(path);
    if (it == g_cache.end()) {
        return std::nullopt;
    }
    ULONGLONG maxAge =
        it->second.item.isFolder ? g_settings.cacheMs : kFileCacheMs;
    bool fresh = GetTickCount64() - it->second.tick < maxAge;
    return CacheLookup{it->second.item, fresh};
}

// Caller holds g_cacheMutex. Makes room by dropping values that are no longer
// fresh. Fresh values are never dropped, so values shown in other windows
// aren't lost; while the cache is full of them, new subfolder values just
// aren't added (see StoreSubfolderTotals).
void TrimCacheLocked() {
    if (g_cache.size() <= kMaxCacheEntries) {
        return;
    }

    // Each pass looks at every entry, so don't repeat it on every store while
    // the cache stays full.
    static ULONGLONG lastTrim;
    ULONGLONG now = GetTickCount64();
    if (lastTrim && now - lastTrim < 1000) {
        return;
    }
    lastTrim = now;

    std::erase_if(g_cache, [now](const auto& entry) {
        ULONGLONG maxAge =
            entry.second.item.isFolder ? g_settings.cacheMs : kFileCacheMs;
        return now - entry.second.tick >= maxAge;
    });
}

void StoreCache(const std::wstring& path, const ItemSize& item) {
    std::lock_guard lock(g_cacheMutex);
    g_cache[path] = {item, GetTickCount64()};
    TrimCacheLocked();
}

// Stores a walk's subfolder totals in batches, so Explorer's window threads
// aren't kept waiting for the lock while a very large walk is stored. Once
// the cache is full, values for folders not cached yet are skipped. The
// direct subfolders come first, so they're the ones kept.
void StoreSubfolderTotals(
    const std::vector<std::pair<std::wstring, ULONGLONG>>& totals) {
    constexpr size_t kBatchSize = 1000;
    ULONGLONG now = GetTickCount64();
    for (size_t start = 0; start < totals.size(); start += kBatchSize) {
        std::lock_guard lock(g_cacheMutex);
        TrimCacheLocked();
        size_t end = std::min(start + kBatchSize, totals.size());
        for (size_t i = start; i < end; i++) {
            const auto& [path, total] = totals[i];
            CacheEntry entry{ItemSize{total, true}, now};
            if (auto it = g_cache.find(path); it != g_cache.end()) {
                it->second = entry;
            } else if (g_cache.size() < kMaxCacheEntries) {
                g_cache.emplace(path, entry);
            }
        }
    }
}

// At most this many folder walks run at once, at background I/O priority, so
// they don't slow down Explorer itself. Walks run in a private thread pool
// capped at this many threads, so queued walks wait in its queue instead of
// tying up threads in Explorer's own pool.
constexpr DWORD kMaxConcurrentWalks = 2;
PTP_POOL g_walkPool;
PTP_CLEANUP_GROUP g_walkCleanupGroup;
TP_CALLBACK_ENVIRON g_walkEnvironment;

struct FolderJobParams {
    std::wstring path;
    // False if Explorer was given no value, so it must always be told when
    // the value is ready.
    bool explorerHasValue;
};

void CALLBACK FolderJob(PTP_CALLBACK_INSTANCE, void* parameter) {
    std::unique_ptr<FolderJobParams> params(
        static_cast<FolderJobParams*>(parameter));
    const std::wstring* path = &params->path;

    if (!g_stopping) {
        bool background =
            SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
        ULONGLONG start = GetTickCount64();

        std::vector<std::pair<std::wstring, ULONGLONG>> subfolderTotals;
        auto size = GetFolderSizeOnDisk(*path, &subfolderTotals);

        if (background) {
            SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
        }

        if (size && !g_stopping) {
            Wh_Log(L"Calculated %s in %I64u ms (%u subfolders)", path->c_str(),
                   GetTickCount64() - start, (unsigned)subfolderTotals.size());

            auto previous = LookupCache(*path);
            StoreSubfolderTotals(subfolderTotals);
            StoreCache(*path, {size, true});

            // Only ask Explorer to redraw the item if its value changed.
            // Redrawing unchanged items made Explorer ask again, which caused
            // the same folders to be recalculated over and over.
            if (!params->explorerHasValue || !previous ||
                previous->item.size != size) {
                SHChangeNotify(SHCNE_UPDATEITEM,
                               SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path->c_str(),
                               nullptr);
            }
        }
    }

    {
        std::lock_guard lock(g_pendingMutex);
        g_pendingPaths.erase(*path);
    }
}

void StartFolderJob(const std::wstring& path, bool explorerHasValue) {
    if (g_stopping || !g_walkPool) {
        return;
    }

    {
        std::lock_guard lock(g_pendingMutex);
        if (!g_pendingPaths.insert(path).second) {
            return;  // Already queued or running.
        }
    }

    auto* parameter = new FolderJobParams{path, explorerHasValue};
    if (!TrySubmitThreadpoolCallback(FolderJob, parameter,
                                     &g_walkEnvironment)) {
        delete parameter;
        std::lock_guard lock(g_pendingMutex);
        g_pendingPaths.erase(path);
    }
}

// Windows, Program Files, Program Files (x86) and ProgramData, read once at
// start-up from the environment, without trailing backslashes.
std::vector<std::wstring> g_systemFolders;

void LoadSystemFolders() {
    g_systemFolders.clear();

    WCHAR buffer[MAX_PATH];
    UINT length = GetWindowsDirectoryW(buffer, ARRAYSIZE(buffer));
    if (length && length < ARRAYSIZE(buffer)) {
        g_systemFolders.emplace_back(buffer, length);
    }

    for (PCWSTR name : {L"ProgramFiles", L"ProgramFiles(x86)", L"ProgramData"}) {
        DWORD envLength =
            GetEnvironmentVariableW(name, buffer, ARRAYSIZE(buffer));
        if (envLength && envLength < ARRAYSIZE(buffer)) {
            g_systemFolders.emplace_back(buffer, envLength);
        }
    }

    for (auto& folder : g_systemFolders) {
        while (!folder.empty() && folder.back() == L'\\') {
            folder.pop_back();
        }
    }
}

// True for a system folder itself or any folder inside one.
bool IsInSystemFolder(const std::wstring& path) {
    for (const auto& folder : g_systemFolders) {
        if (path.size() >= folder.size() &&
            _wcsnicmp(path.c_str(), folder.c_str(), folder.size()) == 0 &&
            (path.size() == folder.size() || path[folder.size()] == L'\\')) {
            return true;
        }
    }
    return false;
}

bool ShouldCalculateFolder(const std::wstring& path) {
    switch (g_settings.folderSizes) {
        case FolderSizes::disabled:
            return false;
        case FolderSizes::exceptSystemFolders:
            return !IsInSystemFolder(path);
        case FolderSizes::always:
            break;
    }

    return true;
}

// Reads whether an item is a folder from the data Explorer keeps in its pidl,
// without touching the disk or the network.
std::optional<bool> IsFolderFromPidl(IShellFolder* shellFolder,
                                     PCUITEMID_CHILD pidl) {
    WIN32_FIND_DATAW findData;
    if (FAILED(SHGetDataFromIDListW(shellFolder, pidl, SHGDFIL_FINDDATA,
                                    &findData, sizeof(findData)))) {
        return std::nullopt;
    }
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

std::optional<ItemSize> GetItemSizeOnDisk(const std::wstring& path,
                                          IShellFolder* shellFolder,
                                          PCUITEMID_CHILD pidl) {
    auto cached = LookupCache(path);
    if (cached && cached->fresh) {
        return cached->item;
    }

    if (cached && cached->item.isFolder) {
        // Show the old value now and refresh it in the background.
        if (ShouldCalculateFolder(path)) {
            StartFolderJob(path, cached->item.size.has_value());
        }
        return cached->item;
    }

    // Explorer also asks on its window threads, and each query on a network
    // drive takes several round trips, so don't touch network drives at all
    // unless enabled. Network items are then never cached, so checking after
    // the cache lookup lets cached local items skip the drive type check.
    // The item is shown empty, but is still marked as a file or folder (read
    // from the pidl, without I/O), so sorting keeps folders together.
    if (!g_settings.networkDrives && IsNetworkPath(path)) {
        return ItemSize{std::nullopt,
                        IsFolderFromPidl(shellFolder, pidl).value_or(false)};
    }

    // Reads the attributes and reparse tag without following links, and gives
    // a file's size in the same call.
    auto raw = ReadAllocation(path);
    if (!raw) {
        // Still tell files and folders apart, so sorting keeps them grouped.
        auto isFolder = IsFolderFromPidl(shellFolder, pidl);
        if (!isFolder) {
            return std::nullopt;
        }
        return ItemSize{std::nullopt, *isFolder};
    }

    ItemSize item;
    item.isFolder = raw->attributes & FILE_ATTRIBUTE_DIRECTORY;

    if (!item.isFolder) {
        item.size =
            AdjustAllocationSize(path, raw->attributes, raw->reparseTag,
                                 raw->allocationSize, GetClusterSize(path));
    } else if (IsFolderLink(raw->attributes, raw->reparseTag)) {
        return item;  // Shown empty, as links aren't followed.
    } else if (!ShouldCalculateFolder(path)) {
        return item;  // Not calculated, and not cached either.
    } else if (IsOnlineOnlyFolder(raw->attributes)) {
        item.size = 0;  // Not listed, so OneDrive isn't asked for anything.
    } else {
        // Folder trees are always walked in the background pool, never on the
        // thread that asked: on a GUI thread that could freeze Explorer, and
        // the pool keeps every walk to the same limit and low priority. The
        // value is shown when it's ready.
        StartFolderJob(path, false);
        return item;
    }

    if (item.size) {
        StoreCache(path, item);
    }

    return item;
}

std::optional<std::wstring> GetItemPath(IShellFolder* shellFolder,
                                        PCUITEMID_CHILD pidl) {
    STRRET strret;
    if (FAILED(shellFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING,
                                             &strret))) {
        return std::nullopt;
    }

    PWSTR raw = nullptr;
    if (FAILED(StrRetToStrW(&strret, pidl, &raw)) || !raw) {
        return std::nullopt;
    }

    std::wstring path = raw;
    CoTaskMemFree(raw);

    if (path.starts_with(L"\\\\?\\"sv) &&
        !path.starts_with(L"\\\\?\\UNC\\"sv)) {
        path = path.substr(4);
    }

    if (!IsFileSystemPath(path)) {
        return std::nullopt;
    }

    return path;
}

std::optional<ItemSize> GetItemSizeOnDisk(void* pFolder, PCUITEMID_CHILD pidl) {
    Microsoft::WRL::ComPtr<IShellFolder> shellFolder;
    HRESULT hr = static_cast<IUnknown*>(pFolder)->QueryInterface(
        IID_PPV_ARGS(&shellFolder));
    if (FAILED(hr) || !shellFolder) {
        return std::nullopt;
    }

    auto path = GetItemPath(shellFolder.Get(), pidl);
    if (!path) {
        return std::nullopt;
    }
    return GetItemSizeOnDisk(*path, shellFolder.Get(), pidl);
}

HRESULT SetStrRet(STRRET* strret, PCWSTR text) {
    strret->uType = STRRET_WSTR;
    return SHStrDupW(text, &strret->pOleStr);
}

////////////////////////////////////////////////////////////////////////////////
// windows.storage.dll hooks (CFSFolder)

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pThis,
                                                     UINT column,
                                                     PROPERTYKEY* key);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

// 0 = not checked yet, 1 = absent, 2 = present.
std::atomic<int> g_nativeColumnState;

bool IsNativeColumnPresent(void* pThis) {
    int state = g_nativeColumnState;
    if (state == 0) {
        state = 1;
        PROPERTYKEY key;
        for (UINT i = 0; i < 10000 && SUCCEEDED(CFSFolder_MapColumnToSCID_Original(
                                          pThis, i, &key));
             i++) {
            if (IsEqualPropertyKey(key, kPKEY_SizeOnDisk)) {
                state = 2;
                break;
            }
        }
        g_nativeColumnState = state;
        Wh_Log(L"Native size on disk column present: %d", state == 2);
    }
    return state == 2;
}

// True if `column` is the extra index we append after Explorer's last column.
bool IsAppendedColumn(void* pThis, UINT column) {
    PROPERTYKEY key;
    if (SUCCEEDED(CFSFolder_MapColumnToSCID_Original(pThis, column, &key))) {
        return false;
    }
    if (column == 0 ||
        FAILED(CFSFolder_MapColumnToSCID_Original(pThis, column - 1, &key))) {
        return false;
    }
    return !IsNativeColumnPresent(pThis);
}

HRESULT WINAPI CFSFolder_MapColumnToSCID_Hook(void* pThis,
                                              UINT column,
                                              PROPERTYKEY* key) {
    auto scope = HookRefCountScope();

    HRESULT hr = CFSFolder_MapColumnToSCID_Original(pThis, column, key);
    if (SUCCEEDED(hr) || !key || !IsAppendedColumn(pThis, column)) {
        return hr;
    }

    *key = kPKEY_SizeOnDisk;
    return S_OK;
}

HRESULT GetColumnKey(void* pThis, UINT column, PROPERTYKEY* key) {
    HRESULT hr = CFSFolder_MapColumnToSCID_Original(pThis, column, key);
    if (FAILED(hr) && IsAppendedColumn(pThis, column)) {
        *key = kPKEY_SizeOnDisk;
        hr = S_OK;
    }
    return hr;
}

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pThis,
                                                  PCUITEMID_CHILD pidl,
                                                  const PROPERTYKEY* key,
                                                  VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;
HRESULT WINAPI CFSFolder_GetDetailsEx_Hook(void* pThis,
                                           PCUITEMID_CHILD pidl,
                                           const PROPERTYKEY* key,
                                           VARIANT* value) {
    if (!pidl || !key || !value ||
        !IsEqualPropertyKey(*key, kPKEY_SizeOnDisk)) {
        return CFSFolder_GetDetailsEx_Original(pThis, pidl, key, value);
    }

    auto scope = HookRefCountScope();

    static std::atomic<int> hits;
    if (FirstHits(hits)) {
        Wh_Log(L"[diag] GetDetailsEx called for size on disk");
    }

    VariantInit(value);
    auto item = GetItemSizeOnDisk(pThis, pidl);
    if (item && item->size) {
        value->vt = VT_UI8;
        value->ullVal = *item->size;
    }
    return S_OK;
}

using CFSFolder_GetDetailsOf_t = HRESULT(WINAPI*)(void* pThis,
                                                  PCUITEMID_CHILD pidl,
                                                  UINT column,
                                                  SHELLDETAILS* details);
CFSFolder_GetDetailsOf_t CFSFolder_GetDetailsOf_Original;
HRESULT WINAPI CFSFolder_GetDetailsOf_Hook(void* pThis,
                                           PCUITEMID_CHILD pidl,
                                           UINT column,
                                           SHELLDETAILS* details) {
    if (!details || !IsAppendedColumn(pThis, column)) {
        return CFSFolder_GetDetailsOf_Original(pThis, pidl, column, details);
    }

    auto scope = HookRefCountScope();

    static std::atomic<int> hits;
    if (pidl && FirstHits(hits)) {
        Wh_Log(L"[diag] GetDetailsOf called for size on disk");
    }

    details->fmt = LVCFMT_RIGHT;
    details->cxChar = kColumnWidthChars;

    if (!pidl) {
        return SetStrRet(&details->str, kColumnTitle);
    }

    auto item = GetItemSizeOnDisk(pThis, pidl);
    if (!item || !item->size) {
        return SetStrRet(&details->str, L"");
    }

    PROPVARIANT propVariant;
    PropVariantInit(&propVariant);
    propVariant.vt = VT_UI8;
    propVariant.uhVal.QuadPart = *item->size;

    PWSTR text = nullptr;
    if (FAILED(PSFormatForDisplayAlloc(kPKEY_Size, propVariant, PDFF_DEFAULT,
                                       &text)) ||
        !text) {
        return SetStrRet(&details->str, L"");
    }

    details->str.uType = STRRET_WSTR;
    details->str.pOleStr = text;
    return S_OK;
}

using CFSFolder_GetDefaultColumnState_t = HRESULT(WINAPI*)(void* pThis,
                                                           UINT column,
                                                           SHCOLSTATEF* flags);
CFSFolder_GetDefaultColumnState_t CFSFolder_GetDefaultColumnState_Original;
HRESULT WINAPI CFSFolder_GetDefaultColumnState_Hook(void* pThis,
                                                    UINT column,
                                                    SHCOLSTATEF* flags) {
    if (!flags || !IsAppendedColumn(pThis, column)) {
        return CFSFolder_GetDefaultColumnState_Original(pThis, column, flags);
    }

    // SLOW asks the view to fetch values on a background thread.
    *flags = SHCOLSTATE_TYPE_INT | SHCOLSTATE_SLOW;
    return S_OK;
}

using CFSFolder_CompareIDs_t = HRESULT(WINAPI*)(void* pThis,
                                                LPARAM lParam,
                                                PCUIDLIST_RELATIVE pidl1,
                                                PCUIDLIST_RELATIVE pidl2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;
HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pThis,
                                         LPARAM lParam,
                                         PCUIDLIST_RELATIVE pidl1,
                                         PCUIDLIST_RELATIVE pidl2) {
    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pThis, lParam, pidl1, pidl2);
    };

    if (!pidl1 || !pidl2 ||
        (lParam & (SHCIDS_ALLFIELDS | SHCIDS_CANONICALONLY))) {
        return original();
    }

    PROPERTYKEY key;
    UINT column = (UINT)(lParam & SHCIDS_COLUMNMASK);
    if (FAILED(GetColumnKey(pThis, column, &key)) ||
        !IsEqualPropertyKey(key, kPKEY_SizeOnDisk)) {
        return original();
    }

    // Only handle direct children.
    if (!ILIsEmpty(ILNext(pidl1)) || !ILIsEmpty(ILNext(pidl2))) {
        return original();
    }

    auto scope = HookRefCountScope();

    static std::atomic<int> hits;
    if (FirstHits(hits)) {
        Wh_Log(L"[diag] CompareIDs called for size on disk");
    }

    // Tie-break (and fallback) by name, which is column 0 in CFSFolder.
    auto byName = [=]() {
        return CFSFolder_CompareIDs_Original(
            pThis, lParam & ~(LPARAM)SHCIDS_COLUMNMASK, pidl1, pidl2);
    };

    auto item1 = GetItemSizeOnDisk(pThis, (PCUITEMID_CHILD)pidl1);
    auto item2 = GetItemSizeOnDisk(pThis, (PCUITEMID_CHILD)pidl2);
    if (!item1 || !item2) {
        return byName();
    }

    if (!g_settings.mixFoldersWhenSorting &&
        item1->isFolder != item2->isFolder) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0,
                            item1->isFolder ? (USHORT)-1 : 1);
    }

    ULONGLONG size1 = item1->size.value_or(0);
    ULONGLONG size2 = item2->size.value_or(0);
    if (size1 != size2) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0,
                            size1 < size2 ? (USHORT)-1 : 1);
    }

    return byName();
}

// Copy, move and delete operations query sizes too. Leave those alone, as
// m417z's mod does.
thread_local bool g_inRecursiveFolderOperation;

using CRecursiveFolderOperation_t = HRESULT(WINAPI*)(void* pThis);
CRecursiveFolderOperation_t CRecursiveFolderOperation_Prepare_Original;
HRESULT WINAPI CRecursiveFolderOperation_Prepare_Hook(void* pThis) {
    auto scope = HookRefCountScope();
    bool previous = g_inRecursiveFolderOperation;
    g_inRecursiveFolderOperation = true;
    HRESULT hr = CRecursiveFolderOperation_Prepare_Original(pThis);
    g_inRecursiveFolderOperation = previous;
    return hr;
}

CRecursiveFolderOperation_t CRecursiveFolderOperation_Do_Original;
HRESULT WINAPI CRecursiveFolderOperation_Do_Hook(void* pThis) {
    auto scope = HookRefCountScope();
    bool previous = g_inRecursiveFolderOperation;
    g_inRecursiveFolderOperation = true;
    HRESULT hr = CRecursiveFolderOperation_Do_Original(pThis);
    g_inRecursiveFolderOperation = previous;
    return hr;
}

// Explorer's own getter for System.FileAllocationSize, which only returns the
// logical size.
using CFSFolder__GetFileAllocationSize_t =
    HRESULT(WINAPI*)(void* pFolder,
                     PCUITEMID_CHILD pidl,
                     const void* idFolder,
                     PROPVARIANT* value);
CFSFolder__GetFileAllocationSize_t CFSFolder__GetFileAllocationSize_Original;
HRESULT WINAPI CFSFolder__GetFileAllocationSize_Hook(void* pFolder,
                                                     PCUITEMID_CHILD pidl,
                                                     const void* idFolder,
                                                     PROPVARIANT* value) {
    auto original = [=]() {
        return CFSFolder__GetFileAllocationSize_Original(pFolder, pidl,
                                                         idFolder, value);
    };

    if (!pidl || !value || g_inRecursiveFolderOperation || g_stopping) {
        return original();
    }

    auto scope = HookRefCountScope();

    static std::atomic<int> hits;
    if (FirstHits(hits)) {
        Wh_Log(L"[diag] _GetFileAllocationSize called (GUI thread: %d)",
               IsGUIThread(FALSE));
    }

    auto item = GetItemSizeOnDisk(pFolder, pidl);
    if (!item) {
        return original();
    }

    PropVariantInit(value);
    if (item->size) {
        value->vt = VT_UI8;
        value->uhVal.QuadPart = *item->size;
    }
    return S_OK;
}

bool HookWindowsStorageSymbols() {
    HMODULE module = LoadLibraryExW(L"windows.storage.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))"},
            &CFSFolder_MapColumnToSCID_Original,
            CFSFolder_MapColumnToSCID_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))"},
            &CFSFolder_GetDetailsEx_Original,
            CFSFolder_GetDetailsEx_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))"},
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
        {
            {LR"(protected: static long __cdecl CFSFolder::_GetFileAllocationSize(class CFSFolder *,struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct tagPROPVARIANT *))"},
            &CFSFolder__GetFileAllocationSize_Original,
            CFSFolder__GetFileAllocationSize_Hook,
        },
        {
            {LR"(public: long __cdecl CRecursiveFolderOperation::Prepare(void))"},
            &CRecursiveFolderOperation_Prepare_Original,
            CRecursiveFolderOperation_Prepare_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CRecursiveFolderOperation::Do(void))"},
            &CRecursiveFolderOperation_Do_Original,
            CRecursiveFolderOperation_Do_Hook,
            true,
        },
        // Header text and column state for the appended column. Optional so
        // the mod still loads if a Windows update renames them.
        {
            {LR"(public: virtual long __cdecl CFSFolder::GetDetailsOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,struct _SHELLDETAILS *))"},
            &CFSFolder_GetDetailsOf_Original,
            CFSFolder_GetDetailsOf_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CFSFolder::GetDefaultColumnState(unsigned int,unsigned long *))"},
            &CFSFolder_GetDefaultColumnState_Original,
            CFSFolder_GetDefaultColumnState_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, windowsStorageHooks,
                                      ARRAYSIZE(windowsStorageHooks));
}

////////////////////////////////////////////////////////////////////////////////
// propsys.dll hooks

// System.FileAllocationSize has no display format of its own. Explorer formats
// column text through these exports, so swap in the Size key to get identical
// formatting.
using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& value,
                                            PROPDESC_FORMAT_FLAGS flags,
                                            PWSTR* display) {
    if (IsEqualPropertyKey(key, kPKEY_SizeOnDisk)) {
        static std::atomic<int> hits;
        if (FirstHits(hits, 5)) {
            Wh_Log(L"[diag] Formatting size on disk: vt=%u value=%I64u",
                   value.vt, value.vt == VT_UI8 ? value.uhVal.QuadPart : 0);
        }
    }
    return PSFormatForDisplayAlloc_Original(
        IsEqualPropertyKey(key, kPKEY_SizeOnDisk) ? kPKEY_Size : key, value,
        flags, display);
}

using PSFormatForDisplay_t = decltype(&PSFormatForDisplay);
PSFormatForDisplay_t PSFormatForDisplay_Original;
HRESULT WINAPI PSFormatForDisplay_Hook(const PROPERTYKEY& key,
                                       const PROPVARIANT& value,
                                       PROPDESC_FORMAT_FLAGS flags,
                                       LPWSTR text,
                                       DWORD textLength) {
    return PSFormatForDisplay_Original(
        IsEqualPropertyKey(key, kPKEY_SizeOnDisk) ? kPKEY_Size : key, value,
        flags, text, textLength);
}

////////////////////////////////////////////////////////////////////////////////
// Default folder layouts: add the column to Explorer's folder templates
// (the ColumnList values under ...\Explorer\FolderTypes\{type}\TopViews\{view}
// in HKLM) as Explorer reads them.

constexpr std::wstring_view kSizeOnDiskCanonicalName =
    L"System.FileAllocationSize"sv;
// Room for ";" + the Size entry's prefix (flags and width) + our name.
constexpr DWORD kColumnListExtraBytes = 96 * sizeof(WCHAR);

size_t FindCaseInsensitive(std::wstring_view haystack,
                           std::wstring_view needle) {
    auto it = std::search(haystack.begin(), haystack.end(), needle.begin(),
                          needle.end(), [](wchar_t a, wchar_t b) {
                              return std::towlower(a) == std::towlower(b);
                          });
    return it == haystack.end() ? std::wstring_view::npos
                                : (size_t)(it - haystack.begin());
}

bool ContainsCaseInsensitive(std::wstring_view haystack,
                             std::wstring_view needle) {
    return FindCaseInsensitive(haystack, needle) != std::wstring_view::npos;
}

// Folder types whose layouts get the column: the ones used for regular file
// system folders. Anything else, including special views such as Home and any
// added by future Windows versions, is left alone.
constexpr std::wstring_view kFileFolderTypes[] = {
    L"{5c4f28b5-f869-4e84-8e60-f11db97c5cc7}"sv,  // Generic
    L"{7d49d726-3c21-4f05-99aa-fdc2c9474656}"sv,  // Documents
    L"{b3690e58-e961-423b-b687-386ebfd83239}"sv,  // Pictures
    L"{94d6ddcc-4a68-4175-a374-bd584a510b78}"sv,  // Music
    L"{5fa96407-7e77-483c-ac93-691d05850de8}"sv,  // Videos
    L"{885a186e-a440-4ada-812b-db871b942259}"sv,  // Downloads
    L"{cd0fc69b-71e2-46e5-9690-5bcd9f57aab3}"sv,  // UserFiles
    L"{69f1e26b-ec64-4280-bc83-f1eb887ec35a}"sv,  // VersionControl
    L"{4f01ebc5-2385-41f2-a28e-2c5c91fb56e0}"sv,  // StorageProviderGeneric
    L"{dd61bd66-70e8-48dd-9655-65c5e1aac2d1}"sv,  // StorageProviderDocuments
    L"{71d642a9-f2b1-42cd-ad92-eb9300c7cc0a}"sv,  // StorageProviderPictures
    L"{672ecd7e-af04-4399-875c-0290845b6247}"sv,  // StorageProviderMusic
    L"{51294da1-d7b1-485b-9e9a-17cffe33e187}"sv,  // StorageProviderVideos
};

std::wstring GetPathFromHKEY(HKEY key) {
    // Predefined keys (HKLM etc.) can't be queried; the caller only needs the
    // tail of the path anyway.
    if (!key || key == HKEY_CLASSES_ROOT || key == HKEY_CURRENT_USER ||
        key == HKEY_LOCAL_MACHINE || key == HKEY_USERS ||
        key == HKEY_CURRENT_CONFIG || key == HKEY_PERFORMANCE_DATA) {
        return {};
    }

    using NtQueryKey_t = LONG(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);
    static NtQueryKey_t pNtQueryKey = []() {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        return ntdll ? (NtQueryKey_t)GetProcAddress(ntdll, "NtQueryKey")
                     : (NtQueryKey_t) nullptr;
    }();
    if (!pNtQueryKey) {
        return {};
    }

    constexpr int kKeyNameInformation = 3;
    std::vector<BYTE> buffer(1024);
    ULONG size = 0;
    LONG status = pNtQueryKey(key, kKeyNameInformation, buffer.data(),
                              (ULONG)buffer.size(), &size);
    if (status != 0 && size > buffer.size()) {
        buffer.resize(size);
        status = pNtQueryKey(key, kKeyNameInformation, buffer.data(),
                             (ULONG)buffer.size(), &size);
    }
    if (status != 0 || size < sizeof(ULONG)) {
        return {};
    }

    ULONG nameLength = *reinterpret_cast<ULONG*>(buffer.data());
    if (size < sizeof(ULONG) + nameLength) {
        return {};
    }
    return std::wstring(
        reinterpret_cast<PCWSTR>(buffer.data() + sizeof(ULONG)),
        nameLength / sizeof(WCHAR));
}

// True for ...\Explorer\FolderTypes\{type}\TopViews\{view} keys whose type is
// in kFileFolderTypes.
bool IsFolderTemplateKey(HKEY key, LPCWSTR subKey) {
    std::wstring path = GetPathFromHKEY(key);
    if (subKey && *subKey) {
        path += L'\\';
        path += subKey;
    }

    constexpr auto kFolderTypes = L"\\Explorer\\FolderTypes\\"sv;
    size_t typeStart = FindCaseInsensitive(path, kFolderTypes);
    if (typeStart == std::wstring::npos ||
        !ContainsCaseInsensitive(path, L"\\TopViews\\"sv)) {
        return false;
    }
    typeStart += kFolderTypes.size();
    size_t typeEnd = path.find(L'\\', typeStart);
    if (typeEnd == std::wstring::npos) {
        return false;
    }

    std::wstring_view type(path.data() + typeStart, typeEnd - typeStart);
    return std::any_of(std::begin(kFileFolderTypes),
                       std::end(kFileFolderTypes), [type](auto allowed) {
                           return type.size() == allowed.size() &&
                                  ContainsCaseInsensitive(type, allowed);
                       });
}

// "prop:0(34)System.ItemNameDisplay;0System.DateModified;0System.Size;..."
// becomes "...;0System.Size;0System.FileAllocationSize;...". The new entry
// copies the Size entry's prefix so it gets the same flags.
std::optional<std::wstring> InjectSizeOnDiskColumn(std::wstring_view value) {
    if (value.size() < 5 || _wcsnicmp(value.data(), L"prop:", 5) != 0 ||
        ContainsCaseInsensitive(value, kSizeOnDiskCanonicalName)) {
        return std::nullopt;
    }

    // Only change real details layouts, which always include the Name column.
    // Other lists (Gallery, property prefetch lists) aren't column layouts.
    if (!ContainsCaseInsensitive(value, L"System.ItemNameDisplay"sv)) {
        return std::nullopt;
    }

    // Leave the Home page's layout alone. Adding a column to it stopped
    // Explorer windows from opening at all.
    for (auto marker : {L"System.Home."sv, L"System.ActivityInfo"sv,
                        L"System.WebAccountID"sv}) {
        if (ContainsCaseInsensitive(value, marker)) {
            return std::nullopt;
        }
    }

    std::wstring result(value);
    size_t start = 5;
    while (start <= result.size()) {
        size_t end = result.find(L';', start);
        if (end == std::wstring::npos) {
            end = result.size();
        }

        std::wstring_view entry(result.data() + start, end - start);
        size_t nameStart = entry.find(L"System."sv);
        if (nameStart != std::wstring_view::npos &&
            entry.substr(nameStart) == L"System.Size"sv) {
            std::wstring inserted = L";";
            inserted += entry.substr(0, nameStart);
            inserted += kSizeOnDiskCanonicalName;
            result.insert(end, inserted);
            return result;
        }

        start = end + 1;
    }

    // No Size column in this template - leave it alone.
    return std::nullopt;
}

// Shared post-processing for RegQueryValueExW and RegGetValueW.
LSTATUS AdjustColumnListValue(LSTATUS ret,
                              void* data,
                              LPDWORD cbData,
                              DWORD bufferSize) {
    if (!cbData) {
        return ret;
    }

    if (ret == ERROR_MORE_DATA || (ret == ERROR_SUCCESS && !data)) {
        *cbData += kColumnListExtraBytes;
        return ret;
    }

    if (ret != ERROR_SUCCESS || *cbData < sizeof(WCHAR)) {
        return ret;
    }

    std::wstring_view current(static_cast<PCWSTR>(data),
                              *cbData / sizeof(WCHAR));
    while (!current.empty() && current.back() == L'\0') {
        current.remove_suffix(1);
    }

    auto updated = InjectSizeOnDiskColumn(current);
    if (!updated) {
        return ret;
    }

    DWORD required = (DWORD)((updated->size() + 1) * sizeof(WCHAR));
    if (bufferSize < required) {
        *cbData = required;
        return ERROR_MORE_DATA;
    }

    static std::atomic<int> hits;
    if (FirstHits(hits, 10)) {
        Wh_Log(L"[diag] ColumnList: %s", updated->c_str());
    }

    memcpy(data, updated->c_str(), required);
    *cbData = required;
    return ret;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY key,
                                     LPCWSTR valueName,
                                     LPDWORD reserved,
                                     LPDWORD type,
                                     LPBYTE data,
                                     LPDWORD cbData) {
    DWORD bufferSize = (data && cbData) ? *cbData : 0;
    LSTATUS ret = RegQueryValueExW_Original(key, valueName, reserved, type,
                                            data, cbData);
    if (!valueName || _wcsicmp(valueName, L"ColumnList") != 0 ||
        !IsFolderTemplateKey(key, nullptr)) {
        return ret;
    }
    return AdjustColumnListValue(ret, data, cbData, bufferSize);
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LSTATUS WINAPI RegGetValueW_Hook(HKEY key,
                                 LPCWSTR subKey,
                                 LPCWSTR valueName,
                                 DWORD flags,
                                 LPDWORD type,
                                 PVOID data,
                                 LPDWORD cbData) {
    DWORD bufferSize = (data && cbData) ? *cbData : 0;
    LSTATUS ret = RegGetValueW_Original(key, subKey, valueName, flags, type,
                                        data, cbData);
    if (!valueName || _wcsicmp(valueName, L"ColumnList") != 0 ||
        !IsFolderTemplateKey(key, subKey)) {
        return ret;
    }
    return AdjustColumnListValue(ret, data, cbData, bufferSize);
}

void HookRegistryFunctions() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return;
    }

    if (auto p = (RegQueryValueExW_t)GetProcAddress(kernelBase,
                                                    "RegQueryValueExW")) {
        WindhawkUtils::SetFunctionHook(p, RegQueryValueExW_Hook,
                                       &RegQueryValueExW_Original);
    }
    if (auto p = (RegGetValueW_t)GetProcAddress(kernelBase, "RegGetValueW")) {
        WindhawkUtils::SetFunctionHook(p, RegGetValueW_Hook,
                                       &RegGetValueW_Original);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

void LoadSettings() {
    auto folderSizes = WindhawkUtils::StringSetting::make(L"folderSizes");
    g_settings.folderSizes = FolderSizes::exceptSystemFolders;
    if (wcscmp(folderSizes, L"always") == 0) {
        g_settings.folderSizes = FolderSizes::always;
    } else if (wcscmp(folderSizes, L"disabled") == 0) {
        g_settings.folderSizes = FolderSizes::disabled;
    }

    auto method = WindhawkUtils::StringSetting::make(L"folderMethod");
    g_settings.accurateFolders = wcscmp(method, L"fast") != 0;

    g_settings.networkDrives = Wh_GetIntSetting(L"networkDrives");
    g_settings.mixFoldersWhenSorting =
        Wh_GetIntSetting(L"mixFoldersWhenSorting");
    g_settings.addToDefaultColumns = Wh_GetIntSetting(L"addToDefaultColumns");

    int refreshSeconds = Wh_GetIntSetting(L"refreshSeconds");
    if (refreshSeconds < 0) {
        refreshSeconds = 0;
    }
    g_settings.cacheMs = (ULONGLONG)refreshSeconds * 1000;

    auto subfolderCache = WindhawkUtils::StringSetting::make(L"subfolderCache");
    g_settings.cacheAllSubfolders = wcscmp(subfolderCache, L"directOnly") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    ULONGLONG initStart = GetTickCount64();

    LoadSettings();
    LoadSystemFolders();

    for (PCWSTR moduleName : {L"kernelbase.dll", L"kernel32.dll"}) {
        HMODULE module = GetModuleHandleW(moduleName);
        if (module && !g_pGetFileInformationByName) {
            g_pGetFileInformationByName =
                (GetFileInformationByName_t)GetProcAddress(
                    module, "GetFileInformationByName");
        }
    }

    if (!HookWindowsStorageSymbols()) {
        Wh_Log(L"Failed hooking windows.storage.dll symbols");
        return FALSE;
    }
    ULONGLONG symbolsMs = GetTickCount64() - initStart;

    WindhawkUtils::SetFunctionHook(PSFormatForDisplayAlloc,
                                   PSFormatForDisplayAlloc_Hook,
                                   &PSFormatForDisplayAlloc_Original);
    WindhawkUtils::SetFunctionHook(PSFormatForDisplay, PSFormatForDisplay_Hook,
                                   &PSFormatForDisplay_Original);

    if (g_settings.addToDefaultColumns) {
        HookRegistryFunctions();
    }

    // If the pool can't be created, folder sizes just stay empty.
    g_walkPool = CreateThreadpool(nullptr);
    g_walkCleanupGroup = CreateThreadpoolCleanupGroup();
    if (g_walkPool && g_walkCleanupGroup) {
        SetThreadpoolThreadMaximum(g_walkPool, kMaxConcurrentWalks);
        InitializeThreadpoolEnvironment(&g_walkEnvironment);
        SetThreadpoolCallbackPool(&g_walkEnvironment, g_walkPool);
        SetThreadpoolCallbackCleanupGroup(&g_walkEnvironment,
                                          g_walkCleanupGroup, nullptr);
    } else {
        if (g_walkCleanupGroup) {
            CloseThreadpoolCleanupGroup(g_walkCleanupGroup);
            g_walkCleanupGroup = nullptr;
        }
        if (g_walkPool) {
            CloseThreadpool(g_walkPool);
            g_walkPool = nullptr;
        }
    }

    Wh_Log(L"Init took %I64u ms (symbols %I64u ms), fast file queries: %s",
           GetTickCount64() - initStart, symbolsMs,
           g_pGetFileInformationByName ? L"yes" : L"no");

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // Stops folder walks at the next file or folder they reach.
    g_stopping = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_stopping = true;
    // Walks are only queued from hooks, so none can be added after this.
    while (g_hookRefCount > 0) {
        Sleep(100);
    }

    if (g_walkPool) {
        // Returns only after every walk callback has returned, so no mod code
        // is still running on a pool thread once the mod is unloaded. Queued
        // walks that haven't started yet see g_stopping and return at once.
        CloseThreadpoolCleanupGroupMembers(g_walkCleanupGroup, FALSE,
                                           nullptr);
        CloseThreadpoolCleanupGroup(g_walkCleanupGroup);
        g_walkCleanupGroup = nullptr;
        DestroyThreadpoolEnvironment(&g_walkEnvironment);
        CloseThreadpool(g_walkPool);
        g_walkPool = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}

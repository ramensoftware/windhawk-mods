// ==WindhawkMod==
// @id              move-quietly
// @name            Move Conflicts Quietly
// @description     Automatically keeps both items when Explorer copy or move operations hit a name conflict, without showing the replace/skip prompt.
// @version         1.0
// @author          Johhannas Reyn
// @github          https://github.com/JohhannasReyn
// @homepage        https://thegnosys.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Conflicts Quietly

Move Conflicts Quietly removes the repetitive filename-conflict decision from
Windows File Explorer copy and move operations.

When an incoming item has the same name as an item that already exists in the
destination, the mod keeps both by giving the incoming item the next free
`name (n).ext` style name instead of showing the **Replace or skip files**
prompt.

For example, a collision such as:

```
report.txt
```

will normally produce a unique name similar to:

```
report (2).txt
```

Further collisions take the next free number: `report (3).txt`, and so on. An
incoming `report (2).txt` that collides becomes `report (3).txt`, not
`report (2) (2).txt`.

## What it affects

The mod targets File Explorer (`explorer.exe`) and applies automatic
rename-on-collision behavior to copy and move operations performed through the
Windows `IFileOperation` interface, including common Explorer operations such
as copy/paste, cut/paste, and drag-and-drop moves.

The mod does **not** intentionally suppress unrelated error or security UI.
Permission errors, UAC/elevation prompts, disk errors, and other exceptional
conditions are still handled by Windows normally.

## How it works

Explorer uses the Windows Shell `IFileOperation` COM interface for many file
operations. This mod hooks the copy and move entry points. For every queued
item it resolves the destination path, checks whether the name is already
taken (on disk, or by another item queued in the same operation), and if so
passes an explicit `name (n).ext` name to the Shell.

As a safety net it also adds `FOF_RENAMEONCOLLISION` and
`FOFX_PRESERVEFILEEXTENSIONS` to the operation, so that even when the mod
cannot pick a name itself (a non-file-system destination, or a name that is
taken between the check and the copy) the Shell still keeps both instead of
prompting. In that fallback case the Shell chooses the name, which is its
usual `name - Copy.ext` pattern.

The flags are decided once, just before the operation runs, so that the whole
queue is known. Existing operation flags are retained; if the caller did not
set any, the documented `IFileOperation` defaults are used.

## Bypassing the mod for one operation

Hold **Shift** (configurable) while pasting or dropping and the mod leaves that
operation entirely to Explorer, prompt and all. Ctrl is already held during a
paste and every modifier changes what a drag-and-drop does — Shift forces a
move, Ctrl forces a copy, Alt creates a shortcut — so Shift is the default: it
is free during a paste, and during a drop it only matters when the drop would
otherwise have been a copy across drives (Explorer's cursor shows the change).
The Caps Lock and Scroll Lock options work as a mode instead of a held key.

## Important behavior and limitations

- Copying an item into its own folder keeps Explorer's normal
  `name - Copy.ext` behavior; the mod only renames items arriving from a
  different folder.
- Items that are not file-system objects (for example dragged out of a zip
  folder or a phone) fall back to the Shell's own collision naming.
- When a folder with the same name already exists in the destination, the
  default is to merge into it and apply the same keep-both renaming to any
  conflicting files inside. To do that the mod queues the folder's contents
  item by item, so Undo covers the individual items rather than the folder as
  a whole, and a merged move leaves the (now empty) source folder tree to be
  removed after the operation completes. The setting can instead hand merges
  to Explorer unchanged (conflicts inside prompt as usual) or keep both
  folders side by side as `Folder (2)`.
- Operations whose caller has already chosen a collision policy (for example
  "replace all" or "keep newer") are left untouched.
- This version targets 64-bit File Explorer.
- Rename-only and delete operations are not intentionally modified.

## Testing

Before relying on the mod with important data, test it with disposable files
and folders. Useful cases include:

1. Copy one file into a folder that already contains a file with the same name.
2. Repeat the copy several times and verify that every copy is preserved.
3. Test cut/paste and drag-and-drop moves with a destination conflict.
4. Test a multi-file copy containing several conflicts.
5. Test a same-name folder collision so you understand the folder behavior on
   your Windows version.

## Source and issues

Source code and issue tracking:
https://github.com/JohhannasReyn/move-quietly

If you find a copy or move path that still displays Explorer's conflict chooser,
please include your Windows version, the exact operation used, and the Windhawk
log output in the issue report.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folderCollision: merge
  $name: When a folder with the same name already exists
  $options:
  - merge: Merge, keep both copies of conflicting files
  - prompt: Merge, let Explorer handle conflicting files
  - rename: Keep both folders ("Folder (2)")
- bypassKey: shift
  $name: Hold this key to get Explorer's normal conflict handling
  $description: >-
    Checked at the moment you paste or drop. Ctrl is already held during a
    paste, and Ctrl/Shift/Alt change what a drag-and-drop does (Shift forces a
    move, Alt makes a shortcut), so Shift is the safest default. The Caps Lock
    and Scroll Lock options act as a mode: while the lock is on, the mod
    stands aside.
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - win: Windows key
  - capslock: Caps Lock (while on)
  - scrolllock: Scroll Lock (while on)
  - none: Disabled
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>

namespace {

// IFileOperation uses these defaults when SetOperationFlags isn't called.
constexpr DWORD kDefaultOperationFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR;

// Safety net only: we normally pick the name ourselves (see ChooseUniqueName).
constexpr DWORD kCollisionFlags =
    FOF_RENAMEONCOLLISION | FOFX_PRESERVEFILEEXTENSIONS;

// A caller that set any of these has already decided how collisions are
// resolved; the mod leaves such operations alone.
constexpr DWORD kCallerCollisionPolicy =
    FOF_NOCONFIRMATION | FOF_RENAMEONCOLLISION | FOFX_KEEPNEWERFILE;

// Upper bound on the "(n)" suffix we will search for.
constexpr unsigned kMaxSuffix = 1000;

// IFileOperation vtable slots:
//  0 QueryInterface
//  1 AddRef
//  2 Release
//  3 Advise
//  4 Unadvise
//  5 SetOperationFlags
// 14 MoveItem
// 15 MoveItems
// 16 CopyItem
// 17 CopyItems
// 21 PerformOperations
constexpr size_t kSetOperationFlagsIndex = 5;
constexpr size_t kMoveItemIndex = 14;
constexpr size_t kMoveItemsIndex = 15;
constexpr size_t kCopyItemIndex = 16;
constexpr size_t kCopyItemsIndex = 17;
constexpr size_t kPerformOperationsIndex = 21;

using SetOperationFlags_t =
    HRESULT(STDMETHODCALLTYPE*)(IFileOperation*, DWORD);
// MoveItem / CopyItem: (item, destination folder, new name, per-item sink).
using ItemOperation_t =
    HRESULT(STDMETHODCALLTYPE*)(IFileOperation*, IShellItem*, IShellItem*,
                                LPCWSTR, IFileOperationProgressSink*);
// MoveItems / CopyItems: (items, destination folder). No sink parameter.
using ItemsOperation_t =
    HRESULT(STDMETHODCALLTYPE*)(IFileOperation*, IUnknown*, IShellItem*);
using PerformOperations_t =
    HRESULT(STDMETHODCALLTYPE*)(IFileOperation*);

SetOperationFlags_t g_SetOperationFlags_Original = nullptr;
ItemOperation_t g_MoveItem_Original = nullptr;
ItemsOperation_t g_MoveItems_Original = nullptr;
ItemOperation_t g_CopyItem_Original = nullptr;
ItemsOperation_t g_CopyItems_Original = nullptr;
PerformOperations_t g_PerformOperations_Original = nullptr;

enum class FolderCollision { kMerge, kPrompt, kRename };

struct BypassKey {
    int virtualKey = 0;    // 0 disables the bypass.
    bool toggle = false;   // Lock keys: bypass while the lock is on.
    int secondKey = 0;     // Second virtual key to accept (Win has two).
};

struct {
    FolderCollision folderCollision = FolderCollision::kMerge;
    BypassKey bypassKey{VK_SHIFT, false, 0};
} g_settings;

// Guards against junction loops while expanding a folder for merging.
constexpr int kMaxMergeDepth = 64;

struct OperationState {
    DWORD callerFlags = 0;
    bool callerFlagsKnown = false;
    // A file was queued that the mod may need the Shell to rename for it.
    bool wantsFallback = false;
    // A folder was queued that must keep Explorer's merge behavior.
    bool mergeFolderSeen = false;
    // The bypass key was held when the operation was queued.
    bool bypassed = false;
    // Upper-cased full destination paths already claimed by items queued in
    // this operation but not yet on disk.
    std::unordered_set<std::wstring> reservedTargets;
    // Source folders whose contents were queued item by item for a merged
    // move; their empty remains are removed after the operation succeeds.
    std::vector<std::wstring> mergedMoveSources;
};

class SrwExclusiveGuard {
   public:
    explicit SrwExclusiveGuard(SRWLOCK* lock) : lock_(lock) {
        AcquireSRWLockExclusive(lock_);
    }
    ~SrwExclusiveGuard() { ReleaseSRWLockExclusive(lock_); }
    SrwExclusiveGuard(const SrwExclusiveGuard&) = delete;
    SrwExclusiveGuard& operator=(const SrwExclusiveGuard&) = delete;

   private:
    SRWLOCK* lock_;
};

SRWLOCK g_stateLock = SRWLOCK_INIT;

// Keyed by the IFileOperation interface pointer. Entries are removed in
// PerformOperations; an operation abandoned without running leaks only its
// small entry until the mod is unloaded. Never hold this lock across I/O.
std::unordered_map<IFileOperation*, OperationState> g_operationStates;

struct HookTargets {
    void* setOperationFlags = nullptr;
    void* moveItem = nullptr;
    void* moveItems = nullptr;
    void* copyItem = nullptr;
    void* copyItems = nullptr;
    void* performOperations = nullptr;
    HRESULT hr = E_FAIL;
} g_targets;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T** Put() { Reset(); return &ptr_; }
    void** PutVoid() { return reinterpret_cast<void**>(Put()); }
    T* Get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
    void Reset() { if (ptr_) { ptr_->Release(); ptr_ = nullptr; } }

   private:
    T* ptr_ = nullptr;
};

std::wstring DisplayName(IShellItem* item, SIGDN kind) {
    std::wstring result;
    PWSTR name = nullptr;
    if (item && SUCCEEDED(item->GetDisplayName(kind, &name)) && name) {
        result = name;
        CoTaskMemFree(name);
    }
    return result;
}

// File-system path of a folder item without a trailing separator, or empty.
std::wstring FolderPath(IShellItem* folder) {
    std::wstring path = DisplayName(folder, SIGDN_FILESYSPATH);
    while (!path.empty() && path.back() == L'\\') {
        path.pop_back();
    }
    return path;
}

std::wstring ToUpper(std::wstring text) {
    if (!text.empty()) {
        CharUpperBuffW(&text[0], static_cast<DWORD>(text.size()));
    }
    return text;
}

bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() &&
           CompareStringOrdinal(a.c_str(), static_cast<int>(a.size()),
                                b.c_str(), static_cast<int>(b.size()),
                                TRUE) == CSTR_EQUAL;
}

DWORD PathAttributes(const std::wstring& path) {
    std::wstring probe = path;
    // Let the check work past MAX_PATH on drive-letter paths.
    if (probe.size() >= MAX_PATH && probe.size() > 2 && probe[1] == L':') {
        probe = L"\\\\?\\" + probe;
    }
    return GetFileAttributesW(probe.c_str());
}

bool PathExists(const std::wstring& path) {
    return PathAttributes(path) != INVALID_FILE_ATTRIBUTES;
}

bool DirectoryExists(const std::wstring& path) {
    DWORD attributes = PathAttributes(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// Removes `path` and its subdirectories as far as they are empty. Never
// deletes files: RemoveDirectoryW refuses non-empty directories.
void PruneEmptyDirectories(const std::wstring& path, int depth) {
    if (depth > kMaxMergeDepth) {
        return;
    }
    WIN32_FIND_DATAW found;
    HANDLE find = FindFirstFileExW((path + L"\\*").c_str(), FindExInfoBasic,
                                   &found, FindExSearchLimitToDirectories,
                                   nullptr, 0);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                !(found.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                wcscmp(found.cFileName, L".") != 0 &&
                wcscmp(found.cFileName, L"..") != 0) {
                PruneEmptyDirectories(path + L"\\" + found.cFileName,
                                      depth + 1);
            }
        } while (FindNextFileW(find, &found));
        FindClose(find);
    }
    RemoveDirectoryW(path.c_str());
}

bool IsFolderItem(IShellItem* item) {
    SFGAOF attributes = 0;
    if (FAILED(item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &attributes))) {
        return false;
    }
    // Zip files are folders with SFGAO_STREAM; treat those as files so the
    // extension is preserved.
    return (attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM);
}

// Splits "name (7)" into "name" and 7. Returns false when there's no suffix.
bool StripNumberSuffix(std::wstring& base, unsigned& number) {
    if (base.size() < 4 || base.back() != L')') {
        return false;
    }
    size_t open = base.rfind(L" (");
    if (open == std::wstring::npos || open + 2 >= base.size() - 1) {
        return false;
    }
    unsigned value = 0;
    for (size_t i = open + 2; i < base.size() - 1; ++i) {
        if (base[i] < L'0' || base[i] > L'9' || value > kMaxSuffix) {
            return false;
        }
        value = value * 10 + (base[i] - L'0');
    }
    if (value == 0) {
        return false;
    }
    base.erase(open);
    number = value;
    return true;
}

// Decides the name `item` should get in the folder at `destinationPath`.
// Returns true and fills `uniqueName` only when the item must be renamed to
// avoid a collision. `destinationPath` is empty for non-file-system targets.
bool ChooseUniqueName(IFileOperation* fileOperation,
                      IShellItem* item,
                      const std::wstring& destinationPath,
                      LPCWSTR requestedName,
                      std::wstring& uniqueName) {
    if (destinationPath.empty()) {
        return false;  // Not a file-system destination; Shell fallback.
    }

    // Copying into the item's own folder keeps Explorer's "- Copy" naming,
    // and a move onto its own folder must not turn into a rename.
    ComPtr<IShellItem> parent;
    if (SUCCEEDED(item->GetParent(parent.Put())) && parent) {
        std::wstring parentPath = FolderPath(parent.Get());
        if (!parentPath.empty() && EqualsIgnoreCase(parentPath, destinationPath)) {
            return false;
        }
    }

    bool isFolder = IsFolderItem(item);

    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = g_operationStates[fileOperation];
        if (state.callerFlagsKnown && (state.callerFlags & kCallerCollisionPolicy)) {
            return false;  // Caller already chose a collision policy.
        }
        state.wantsFallback = true;
    }

    std::wstring name = (requestedName && *requestedName)
                            ? std::wstring(requestedName)
                            : DisplayName(item, SIGDN_PARENTRELATIVEPARSING);
    if (name.empty() || name.find(L'\\') != std::wstring::npos) {
        return false;  // Not a plain leaf name; Shell fallback.
    }

    std::wstring base = name;
    std::wstring extension;
    if (!isFolder) {
        size_t dot = name.rfind(L'.');
        if (dot != std::wstring::npos && dot > 0) {
            base = name.substr(0, dot);
            extension = name.substr(dot);
        }
    }

    unsigned next = 2;
    unsigned existing = 0;
    if (StripNumberSuffix(base, existing)) {
        next = existing + 1;
    }

    std::wstring candidate = name;
    for (unsigned n = next; n <= kMaxSuffix; ++n) {
        std::wstring fullPath = destinationPath + L"\\" + candidate;

        // Filesystem I/O outside the lock; the reservation test-and-insert
        // inside it. The window between the two is covered by the Shell's
        // own rename-on-collision fallback.
        if (!PathExists(fullPath)) {
            std::wstring key = ToUpper(fullPath);
            SrwExclusiveGuard guard(&g_stateLock);
            auto& reserved = g_operationStates[fileOperation].reservedTargets;
            if (reserved.insert(std::move(key)).second) {
                if (candidate == name) {
                    return false;  // No collision; keep the caller's name.
                }
                uniqueName = std::move(candidate);
                return true;
            }
        }

        candidate = base + L" (" + std::to_wstring(n) + L")" + extension;
    }

    return false;  // Gave up; Shell fallback.
}

void Unreserve(IFileOperation* fileOperation, const std::wstring& destinationPath,
               const std::wstring& name) {
    std::wstring key = ToUpper(destinationPath + L"\\" + name);
    SrwExclusiveGuard guard(&g_stateLock);
    auto it = g_operationStates.find(fileOperation);
    if (it != g_operationStates.end()) {
        it->second.reservedTargets.erase(key);
    }
}

// True when the user is asking, via the bypass key, for Explorer's normal
// handling. Hooks run on the thread that processed the paste or drop, so
// GetKeyState reflects the keyboard at that gesture.
bool BypassRequested() {
    const BypassKey& key = g_settings.bypassKey;
    if (key.virtualKey == 0) {
        return false;
    }
    if (key.toggle) {
        return (GetKeyState(key.virtualKey) & 1) != 0;
    }
    bool held = (GetKeyState(key.virtualKey) & 0x8000) != 0;
    if (!held && key.secondKey) {
        held = (GetKeyState(key.secondKey) & 0x8000) != 0;
    }
    return held;
}

// Records the bypass for this operation and reports whether it's active.
bool MarkBypass(IFileOperation* fileOperation) {
    if (!BypassRequested()) {
        return false;
    }
    SrwExclusiveGuard guard(&g_stateLock);
    auto& state = g_operationStates[fileOperation];
    if (!state.bypassed) {
        state.bypassed = true;
        Wh_Log(L"Bypass key held; leaving operation to Explorer");
    }
    return true;
}

HRESULT QueueItem(ItemOperation_t original,
                  IFileOperation* fileOperation,
                  IShellItem* item,
                  IShellItem* destinationFolder,
                  const std::wstring& destinationPath,
                  LPCWSTR requestedName,
                  IFileOperationProgressSink* sink,
                  int depth);

bool CallerChoseCollisionPolicy(IFileOperation* fileOperation) {
    SrwExclusiveGuard guard(&g_stateLock);
    auto it = g_operationStates.find(fileOperation);
    return it != g_operationStates.end() && it->second.callerFlagsKnown &&
           (it->second.callerFlags & kCallerCollisionPolicy);
}

// Queues the contents of `sourceFolder` into the existing folder at
// `targetPath`, item by item, so files inside get keep-both naming and
// subfolders that already exist are merged recursively.
HRESULT QueueMergedFolder(ItemOperation_t original,
                          IFileOperation* fileOperation,
                          IShellItem* sourceFolder,
                          const std::wstring& targetPath,
                          int depth) {
    if (depth > kMaxMergeDepth) {
        return HRESULT_FROM_WIN32(ERROR_CANT_RESOLVE_FILENAME);
    }

    ComPtr<IShellItem> target;
    HRESULT hr = SHCreateItemFromParsingName(targetPath.c_str(), nullptr,
                                             IID_IShellItem, target.PutVoid());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<IEnumShellItems> children;
    hr = sourceFolder->BindToHandler(nullptr, BHID_EnumItems,
                                     IID_IEnumShellItems, children.PutVoid());
    if (FAILED(hr)) {
        return hr;
    }

    for (;;) {
        ComPtr<IShellItem> child;
        ULONG fetched = 0;
        hr = children->Next(1, child.Put(), &fetched);
        if (FAILED(hr)) {
            return hr;
        }
        if (hr != S_OK || fetched == 0) {
            return S_OK;
        }
        hr = QueueItem(original, fileOperation, child.Get(), target.Get(),
                       targetPath, nullptr, nullptr, depth + 1);
        if (FAILED(hr)) {
            return hr;
        }
    }
}

// Queues one copy or move through the original single-item method, with a
// collision-free name when one is needed, or expanded into its contents when
// a same-name folder is being merged.
HRESULT QueueItem(ItemOperation_t original,
                  IFileOperation* fileOperation,
                  IShellItem* item,
                  IShellItem* destinationFolder,
                  const std::wstring& destinationPath,
                  LPCWSTR requestedName,
                  IFileOperationProgressSink* sink,
                  int depth) {
    if (item && destinationFolder && !destinationPath.empty() &&
        IsFolderItem(item) && !CallerChoseCollisionPolicy(fileOperation)) {
        std::wstring name = (requestedName && *requestedName)
                                ? std::wstring(requestedName)
                                : DisplayName(item, SIGDN_PARENTRELATIVEPARSING);
        std::wstring sourcePath = DisplayName(item, SIGDN_FILESYSPATH);
        std::wstring targetPath = destinationPath + L"\\" + name;

        bool collides = !name.empty() &&
                        name.find(L'\\') == std::wstring::npos &&
                        !sourcePath.empty() &&
                        !EqualsIgnoreCase(sourcePath, targetPath) &&
                        DirectoryExists(targetPath);

        if (collides && g_settings.folderCollision == FolderCollision::kMerge) {
            Wh_Log(L"Merging folder: %s -> %s", sourcePath.c_str(),
                   targetPath.c_str());
            if (original == g_MoveItem_Original) {
                SrwExclusiveGuard guard(&g_stateLock);
                g_operationStates[fileOperation].mergedMoveSources.push_back(
                    sourcePath);
            }
            return QueueMergedFolder(original, fileOperation, item, targetPath,
                                     depth);
        }

        if (g_settings.folderCollision != FolderCollision::kRename) {
            // Explorer's own merge. Make sure the fallback flags don't turn
            // it into a rename.
            if (collides) {
                SrwExclusiveGuard guard(&g_stateLock);
                g_operationStates[fileOperation].mergeFolderSeen = true;
            }
            return original(fileOperation, item, destinationFolder,
                            requestedName, sink);
        }
        // kRename falls through to the same naming as files.
    }

    std::wstring uniqueName;
    LPCWSTR name = requestedName;
    bool renamed = item && destinationFolder &&
                   ChooseUniqueName(fileOperation, item, destinationPath,
                                    requestedName, uniqueName);
    if (renamed) {
        Wh_Log(L"Collision: %s -> %s",
               requestedName ? requestedName : L"(item name)",
               uniqueName.c_str());
        name = uniqueName.c_str();
    }

    HRESULT hr = original(fileOperation, item, destinationFolder, name, sink);
    if (FAILED(hr) && renamed) {
        Unreserve(fileOperation, destinationPath, uniqueName);
    }
    return hr;
}

// Collects the items behind the IUnknown accepted by CopyItems/MoveItems.
// Per the docs it may be an IShellItemArray, IDataObject, IEnumShellItems,
// IPersistIDList, or a single IShellItem. Returns E_NOINTERFACE only when
// the container is none of those, before anything has been collected.
HRESULT CollectItems(IUnknown* items, std::vector<ComPtr<IShellItem>>& out) {
    ComPtr<IShellItem> single;
    if (SUCCEEDED(items->QueryInterface(IID_IShellItem, single.PutVoid()))) {
        out.push_back(std::move(single));
        return S_OK;
    }

    ComPtr<IShellItemArray> array;
    HRESULT hr = items->QueryInterface(IID_IShellItemArray, array.PutVoid());
    if (FAILED(hr)) {
        ComPtr<IDataObject> dataObject;
        if (SUCCEEDED(items->QueryInterface(IID_IDataObject,
                                            dataObject.PutVoid()))) {
            hr = SHCreateShellItemArrayFromDataObject(
                dataObject.Get(), IID_IShellItemArray, array.PutVoid());
            if (FAILED(hr)) {
                return hr;
            }
        }
    }
    if (array) {
        DWORD count = 0;
        hr = array->GetCount(&count);
        if (FAILED(hr)) {
            return hr;
        }
        out.reserve(count);
        for (DWORD i = 0; i < count; ++i) {
            ComPtr<IShellItem> item;
            hr = array->GetItemAt(i, item.Put());
            if (FAILED(hr)) {
                return hr;
            }
            out.push_back(std::move(item));
        }
        return S_OK;
    }

    ComPtr<IEnumShellItems> enumerator;
    if (SUCCEEDED(items->QueryInterface(IID_IEnumShellItems,
                                        enumerator.PutVoid()))) {
        for (;;) {
            ComPtr<IShellItem> item;
            ULONG fetched = 0;
            hr = enumerator->Next(1, item.Put(), &fetched);
            if (FAILED(hr)) {
                return hr;
            }
            if (hr != S_OK || fetched == 0) {
                return S_OK;
            }
            out.push_back(std::move(item));
        }
    }

    ComPtr<IPersistIDList> persistIdList;
    if (SUCCEEDED(items->QueryInterface(IID_IPersistIDList,
                                        persistIdList.PutVoid()))) {
        PIDLIST_ABSOLUTE idList = nullptr;
        hr = persistIdList->GetIDList(&idList);
        if (FAILED(hr)) {
            return hr;
        }
        ComPtr<IShellItem> item;
        hr = SHCreateItemFromIDList(idList, IID_IShellItem, item.PutVoid());
        CoTaskMemFree(idList);
        if (FAILED(hr)) {
            return hr;
        }
        out.push_back(std::move(item));
        return S_OK;
    }

    return E_NOINTERFACE;
}

HRESULT QueueItems(ItemOperation_t originalSingle,
                   ItemsOperation_t originalMany,
                   IFileOperation* fileOperation,
                   IUnknown* items,
                   IShellItem* destinationFolder) {
    std::vector<ComPtr<IShellItem>> collected;
    HRESULT hr = (items && destinationFolder) ? CollectItems(items, collected)
                                              : E_NOINTERFACE;
    if (hr == E_NOINTERFACE) {
        // Unknown container: let the Shell handle it. The fallback flags
        // still avoid the prompt, but the Shell picks the name.
        Wh_Log(L"Items hook: unrecognized container; Shell fallback");
        return originalMany(fileOperation, items, destinationFolder);
    }
    if (FAILED(hr)) {
        return hr;
    }

    // Once the container is understood we own the queueing; never fall back
    // to originalMany from here, or items would be queued twice.
    std::wstring destinationPath = FolderPath(destinationFolder);
    for (auto& item : collected) {
        hr = QueueItem(originalSingle, fileOperation, item.Get(),
                       destinationFolder, destinationPath, nullptr, nullptr, 0);
        if (FAILED(hr)) {
            return hr;
        }
    }
    return S_OK;
}

// Decides, once, whether the Shell's rename-on-collision fallback should be
// enabled for this operation, now that the whole queue is known.
void ArmCollisionFallback(IFileOperation* fileOperation) {
    DWORD flags = kDefaultOperationFlags;
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto it = g_operationStates.find(fileOperation);
        if (it == g_operationStates.end()) {
            return;  // Nothing we touched (rename/delete/new-item only).
        }
        const OperationState& state = it->second;
        if (state.bypassed || !state.wantsFallback || state.mergeFolderSeen) {
            return;
        }
        if (state.callerFlagsKnown) {
            if (state.callerFlags & kCallerCollisionPolicy) {
                return;
            }
            flags = state.callerFlags;
        }
    }

    HRESULT hr = g_SetOperationFlags_Original(fileOperation,
                                              flags | kCollisionFlags);
    if (FAILED(hr)) {
        Wh_Log(L"SetOperationFlags(collision flags) failed: 0x%08X",
               static_cast<unsigned>(hr));
    }
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE SetOperationFlags_Hook(IFileOperation* fileOperation,
                                                  DWORD flags) {
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = g_operationStates[fileOperation];
        state.callerFlags = flags;
        state.callerFlagsKnown = true;
    }
    return g_SetOperationFlags_Original(fileOperation, flags);
}

HRESULT STDMETHODCALLTYPE MoveItem_Hook(
    IFileOperation* fileOperation,
    IShellItem* item,
    IShellItem* destinationFolder,
    LPCWSTR newName,
    IFileOperationProgressSink* sink) {
    if (MarkBypass(fileOperation)) {
        return g_MoveItem_Original(fileOperation, item, destinationFolder, newName, sink);
    }
    return QueueItem(g_MoveItem_Original, fileOperation, item,
                     destinationFolder, FolderPath(destinationFolder), newName,
                     sink, 0);
}

HRESULT STDMETHODCALLTYPE MoveItems_Hook(
    IFileOperation* fileOperation,
    IUnknown* items,
    IShellItem* destinationFolder) {
    if (MarkBypass(fileOperation)) {
        return g_MoveItems_Original(fileOperation, items, destinationFolder);
    }
    return QueueItems(g_MoveItem_Original, g_MoveItems_Original, fileOperation,
                      items, destinationFolder);
}

HRESULT STDMETHODCALLTYPE CopyItem_Hook(
    IFileOperation* fileOperation,
    IShellItem* item,
    IShellItem* destinationFolder,
    LPCWSTR copyName,
    IFileOperationProgressSink* sink) {
    if (MarkBypass(fileOperation)) {
        return g_CopyItem_Original(fileOperation, item, destinationFolder, copyName, sink);
    }
    return QueueItem(g_CopyItem_Original, fileOperation, item,
                     destinationFolder, FolderPath(destinationFolder), copyName,
                     sink, 0);
}

HRESULT STDMETHODCALLTYPE CopyItems_Hook(
    IFileOperation* fileOperation,
    IUnknown* items,
    IShellItem* destinationFolder) {
    if (MarkBypass(fileOperation)) {
        return g_CopyItems_Original(fileOperation, items, destinationFolder);
    }
    return QueueItems(g_CopyItem_Original, g_CopyItems_Original, fileOperation,
                      items, destinationFolder);
}

HRESULT STDMETHODCALLTYPE PerformOperations_Hook(IFileOperation* fileOperation) {
    ArmCollisionFallback(fileOperation);
    HRESULT hr = g_PerformOperations_Original(fileOperation);

    std::vector<std::wstring> mergedMoveSources;
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto it = g_operationStates.find(fileOperation);
        if (it != g_operationStates.end()) {
            mergedMoveSources = std::move(it->second.mergedMoveSources);
            g_operationStates.erase(it);
        }
    }

    if (!mergedMoveSources.empty() && SUCCEEDED(hr)) {
        BOOL aborted = FALSE;
        if (SUCCEEDED(fileOperation->GetAnyOperationsAborted(&aborted)) &&
            !aborted) {
            for (const auto& source : mergedMoveSources) {
                PruneEmptyDirectories(source, 0);
            }
        }
    }
    return hr;
}

// Keeps the module that implements CFileOperation loaded for the lifetime of
// the process, so hooked code can never be unmapped underneath us.
void PinImplementationModule(void* codeAddress) {
    HMODULE module = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<LPCWSTR>(codeAddress), &module)) {
        WCHAR path[MAX_PATH];
        if (GetModuleFileNameW(module, path, MAX_PATH)) {
            Wh_Log(L"IFileOperation implemented in %s", path);
        }
    } else {
        Wh_Log(L"GetModuleHandleExW failed: %u", GetLastError());
    }
}

DWORD WINAPI ProbeFileOperationVtable(LPVOID) {
    // CLSID_FileOperation is ThreadingModel=Apartment. Probing from an STA
    // guarantees we get the real object, not an ole32 proxy whose vtable
    // would point at marshaling stubs.
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        g_targets.hr = hr;
        return 0;
    }

    IFileOperation* fileOperation = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IFileOperation,
                          reinterpret_cast<void**>(&fileOperation));

    if (SUCCEEDED(hr) && fileOperation) {
        void** vtable = *reinterpret_cast<void***>(fileOperation);

        g_targets.setOperationFlags = vtable[kSetOperationFlagsIndex];
        g_targets.moveItem = vtable[kMoveItemIndex];
        g_targets.moveItems = vtable[kMoveItemsIndex];
        g_targets.copyItem = vtable[kCopyItemIndex];
        g_targets.copyItems = vtable[kCopyItemsIndex];
        g_targets.performOperations = vtable[kPerformOperationsIndex];
        g_targets.hr = S_OK;

        PinImplementationModule(g_targets.setOperationFlags);

        fileOperation->Release();
    } else {
        g_targets.hr = hr;
    }

    CoUninitialize();
    return 0;
}

bool ResolveHookTargets() {
    HANDLE thread = CreateThread(nullptr, 0, ProbeFileOperationVtable, nullptr,
                                 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return false;
    }

    DWORD waitResult = WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Failed while resolving IFileOperation implementation");
        return false;
    }

    if (FAILED(g_targets.hr)) {
        Wh_Log(L"CoCreateInstance(CLSID_FileOperation) failed: 0x%08X",
               static_cast<unsigned>(g_targets.hr));
        return false;
    }

    void* targets[] = {
        g_targets.setOperationFlags, g_targets.moveItem,  g_targets.moveItems,
        g_targets.copyItem,          g_targets.copyItems, g_targets.performOperations,
    };

    for (size_t i = 0; i < std::size(targets); ++i) {
        if (!targets[i]) {
            Wh_Log(L"Null vtable entry at target %zu", i);
            return false;
        }
        for (size_t j = 0; j < i; ++j) {
            // Identical-COMDAT folding could alias two methods; hooking the
            // same address twice would misroute one of them.
            if (targets[i] == targets[j]) {
                Wh_Log(L"Vtable targets %zu and %zu alias the same code", i, j);
                return false;
            }
        }
    }

    return true;
}

}  // namespace

void LoadSettings() {
    PCWSTR value = Wh_GetStringSetting(L"folderCollision");
    FolderCollision mode = FolderCollision::kMerge;
    if (value) {
        if (wcscmp(value, L"prompt") == 0) {
            mode = FolderCollision::kPrompt;
        } else if (wcscmp(value, L"rename") == 0) {
            mode = FolderCollision::kRename;
        }
        Wh_FreeStringSetting(value);
    }
    g_settings.folderCollision = mode;

    BypassKey key{VK_SHIFT, false, 0};
    value = Wh_GetStringSetting(L"bypassKey");
    if (value) {
        if (wcscmp(value, L"ctrl") == 0) {
            key = {VK_CONTROL, false, 0};
        } else if (wcscmp(value, L"alt") == 0) {
            key = {VK_MENU, false, 0};
        } else if (wcscmp(value, L"win") == 0) {
            key = {VK_LWIN, false, VK_RWIN};
        } else if (wcscmp(value, L"capslock") == 0) {
            key = {VK_CAPITAL, true, 0};
        } else if (wcscmp(value, L"scrolllock") == 0) {
            key = {VK_SCROLL, true, 0};
        } else if (wcscmp(value, L"none") == 0) {
            key = {0, false, 0};
        }
        Wh_FreeStringSetting(value);
    }
    g_settings.bypassKey = key;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Move Conflicts Quietly");
    LoadSettings();

    if (!ResolveHookTargets()) {
        Wh_Log(L"Couldn't resolve IFileOperation methods");
        return FALSE;
    }

    bool ok = true;

    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<SetOperationFlags_t>(g_targets.setOperationFlags),
        SetOperationFlags_Hook, &g_SetOperationFlags_Original);
    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ItemOperation_t>(g_targets.moveItem), MoveItem_Hook,
        &g_MoveItem_Original);
    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ItemsOperation_t>(g_targets.moveItems), MoveItems_Hook,
        &g_MoveItems_Original);
    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ItemOperation_t>(g_targets.copyItem), CopyItem_Hook,
        &g_CopyItem_Original);
    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ItemsOperation_t>(g_targets.copyItems), CopyItems_Hook,
        &g_CopyItems_Original);
    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<PerformOperations_t>(g_targets.performOperations),
        PerformOperations_Hook, &g_PerformOperations_Original);

    if (!ok) {
        Wh_Log(L"Failed to register one or more IFileOperation hooks");
        return FALSE;
    }

    Wh_Log(L"IFileOperation hooks registered successfully");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    {
        SrwExclusiveGuard guard(&g_stateLock);
        g_operationStates.clear();
    }

    Wh_Log(L"Uninitialized Move Conflicts Quietly");
}

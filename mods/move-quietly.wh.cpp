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

Hold **Shift** (configurable) while dropping, or while clicking Paste in the
context menu or the command bar, and the mod leaves that operation entirely to
Explorer, prompt and all.

Be aware that Shift is also Explorer's "force a move" drag modifier. If you
habitually hold Shift while dragging, every such drop bypasses the mod — set
the option to **Disabled** or the Windows key instead. Ctrl and Alt are worse
choices: Ctrl is held during every Ctrl+V paste (so the mod would never act on
a keyboard paste) and forces a copy on drop; Alt turns a drop into a shortcut.

A held key cannot work with **Ctrl+V**: adding any modifier makes it a
different shortcut that Explorer does not treat as Paste (and Win+Ctrl+V is
taken by Windows). For keyboard pastes, set the **lock** option instead: while
Caps Lock or Scroll Lock is on, the mod stands aside.

## Important behavior and limitations

- Copying an item into its own folder is not renamed by the mod; Explorer's
  own naming applies (normally `name - Copy.ext`).
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
  "replace all" or "keep newer") before queueing items are left untouched.
- After a merged **move**, the emptied source folders are removed directly
  (not via the Recycle Bin, and not covered by Undo). Only empty folders are
  ever removed. Folders that are junctions or symbolic links are never merged;
  they are handed to Explorer unchanged.
- Merging a large folder resolves names for every item up front, before the
  progress dialog appears, and the dialog then lists the individual items.
- The hooks apply to every `IFileOperation` copy or move inside `explorer.exe`,
  which includes third-party shell extensions hosted there.
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
6. For a folder merge, include a hidden file and a hidden+system file (for
   example `desktop.ini`) in the source and verify they arrive.

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
  $name: Hold this key for Explorer's normal conflict handling
  $description: >-
    Works with the mouse: drag-and-drop, the context menu, and the command
    bar. It cannot work with Ctrl+V, because adding a modifier turns it into
    a different shortcut that Explorer ignores. Shift is also Explorer's
    "force a move" drag modifier, so drops made with Shift held bypass the
    mod. Ctrl is held during every Ctrl+V and forces a copy on drop; Alt
    turns a drop into a shortcut. The Windows key has no conflicts.
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - win: Windows key
  - none: Disabled
- bypassLock: none
  $name: While this lock is on, use Explorer's normal conflict handling
  $description: >-
    The way to bypass the mod for a keyboard paste (Ctrl+V). Acts as a mode
    rather than a held key.
  $options:
  - none: Disabled
  - capslock: Caps Lock
  - scrolllock: Scroll Lock
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
#include <atomic>
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
// resolved; the mod leaves such operations alone. Explorer sets flags before
// queueing items; a caller that queues first and sets flags afterwards still
// gets the mod's names, though the fallback flags are then withheld.
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
constexpr size_t kReleaseIndex = 2;
constexpr size_t kSetOperationFlagsIndex = 5;
constexpr size_t kMoveItemIndex = 14;
constexpr size_t kMoveItemsIndex = 15;
constexpr size_t kCopyItemIndex = 16;
constexpr size_t kCopyItemsIndex = 17;
constexpr size_t kPerformOperationsIndex = 21;

using Release_t = ULONG(STDMETHODCALLTYPE*)(IFileOperation*);
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

Release_t g_Release_Original = nullptr;
SetOperationFlags_t g_SetOperationFlags_Original = nullptr;
ItemOperation_t g_MoveItem_Original = nullptr;
ItemsOperation_t g_MoveItems_Original = nullptr;
ItemOperation_t g_CopyItem_Original = nullptr;
ItemsOperation_t g_CopyItems_Original = nullptr;
PerformOperations_t g_PerformOperations_Original = nullptr;

enum class FolderCollision { kMerge, kPrompt, kRename };

struct Settings {
    FolderCollision folderCollision = FolderCollision::kMerge;
    int bypassKey = VK_SHIFT;    // Held key; 0 disables.
    int bypassKeyAlt = 0;        // Second held key to accept (Win has two).
    int bypassLock = 0;          // Lock key; bypass while on. 0 disables.
};

// Written by Wh_ModSettingsChanged on the engine thread, read by hooks;
// always copied out whole under g_stateLock so one operation sees one
// consistent set of values.
Settings g_settings;

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

class SrwSharedGuard {
   public:
    explicit SrwSharedGuard(SRWLOCK* lock) : lock_(lock) {
        AcquireSRWLockShared(lock_);
    }
    ~SrwSharedGuard() { ReleaseSRWLockShared(lock_); }
    SrwSharedGuard(const SrwSharedGuard&) = delete;
    SrwSharedGuard& operator=(const SrwSharedGuard&) = delete;

   private:
    SRWLOCK* lock_;
};

SRWLOCK g_stateLock = SRWLOCK_INIT;

// Keyed by the IFileOperation interface pointer. Entries are removed in
// PerformOperations and, for operations abandoned without running, when the
// object's refcount reaches zero (Release_Hook), so a recycled address never
// inherits stale state. Never hold this lock across I/O.
std::unordered_map<IFileOperation*, OperationState> g_operationStates;

// Number of live entries; lets Release_Hook skip the lock when there is
// nothing to erase, which is almost always.
std::atomic<size_t> g_operationStateCount{0};

OperationState& StateLocked(IFileOperation* fileOperation) {
    auto [it, inserted] = g_operationStates.try_emplace(fileOperation);
    if (inserted) {
        g_operationStateCount.fetch_add(1, std::memory_order_relaxed);
    }
    return it->second;
}

void EraseStateLocked(IFileOperation* fileOperation) {
    if (g_operationStates.erase(fileOperation)) {
        g_operationStateCount.fetch_sub(1, std::memory_order_relaxed);
    }
}

Settings CurrentSettings() {
    SrwSharedGuard guard(&g_stateLock);
    return g_settings;
}

struct HookTargets {
    void* release = nullptr;
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

// One case folding for every "same name" decision in the mod: invariant
// upper-casing, which matches how the file system compares names.
std::wstring ToUpper(const std::wstring& text) {
    if (text.empty()) {
        return text;
    }
    std::wstring result(text.size(), L'\0');
    int written = LCMapStringW(LOCALE_INVARIANT, LCMAP_UPPERCASE, text.c_str(),
                               static_cast<int>(text.size()), &result[0],
                               static_cast<int>(result.size()));
    if (written <= 0) {
        return text;
    }
    result.resize(static_cast<size_t>(written));
    return result;
}

bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() && ToUpper(a) == ToUpper(b);
}

// Adds the \\?\ prefix so Win32 calls work past MAX_PATH.
std::wstring LongPath(const std::wstring& path) {
    if (path.size() < MAX_PATH || path.compare(0, 4, L"\\\\?\\") == 0) {
        return path;
    }
    if (path.size() > 2 && path[1] == L':') {
        return L"\\\\?\\" + path;
    }
    if (path.compare(0, 2, L"\\\\") == 0) {
        return L"\\\\?\\UNC\\" + path.substr(2);
    }
    return path;
}

DWORD PathAttributes(const std::wstring& path) {
    return GetFileAttributesW(LongPath(path).c_str());
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
    HANDLE find = FindFirstFileExW(LongPath(path + L"\\*").c_str(), FindExInfoBasic,
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
    RemoveDirectoryW(LongPath(path).c_str());
}

bool IsFolderItem(IShellItem* item) {
    SFGAOF attributes = 0;
    if (FAILED(item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &attributes))) {
        return DirectoryExists(DisplayName(item, SIGDN_FILESYSPATH));
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
// `reservedName` receives the name that was reserved for this item (renamed
// or not) so the caller can release it if queueing fails.
bool ChooseUniqueName(IFileOperation* fileOperation,
                      IShellItem* item,
                      const std::wstring& destinationPath,
                      LPCWSTR requestedName,
                      std::wstring& uniqueName,
                      std::wstring& reservedName) {
    if (destinationPath.empty()) {
        // Not a file-system destination: the Shell's own rename-on-collision
        // is the only way to keep both, so make sure it gets armed.
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = StateLocked(fileOperation);
        if (!(state.callerFlagsKnown && (state.callerFlags & kCallerCollisionPolicy))) {
            state.wantsFallback = true;
        }
        return false;
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
        auto& state = StateLocked(fileOperation);
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
    for (unsigned n = next;; ++n) {
        std::wstring fullPath = destinationPath + L"\\" + candidate;

        // Filesystem I/O outside the lock; the reservation test-and-insert
        // inside it. The window between the two is covered by the Shell's
        // own rename-on-collision fallback.
        if (!PathExists(fullPath)) {
            std::wstring key = ToUpper(fullPath);
            SrwExclusiveGuard guard(&g_stateLock);
            auto& reserved = StateLocked(fileOperation).reservedTargets;
            if (reserved.insert(std::move(key)).second) {
                reservedName = candidate;
                if (candidate == name) {
                    return false;  // No collision; keep the caller's name.
                }
                uniqueName = std::move(candidate);
                return true;
            }
        }

        if (n > kMaxSuffix) {
            return false;  // Gave up; Shell fallback.
        }
        candidate = base + L" (" + std::to_wstring(n) + L")" + extension;
    }
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

// True when the user is asking, via the bypass key or lock, for Explorer's
// normal handling. The held key is read with GetAsyncKeyState (physical state
// right now): the command bar dispatches Paste across threads, so a per-thread
// GetKeyState snapshot can predate the key press.
bool KeyHeld(int virtualKey) {
    return virtualKey != 0 && (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool BypassRequested() {
    const Settings settings = CurrentSettings();
    if (KeyHeld(settings.bypassKey) || KeyHeld(settings.bypassKeyAlt)) {
        return true;
    }
    // Toggle state has no thread-independent API; GetKeyState's low bit is
    // the best available and is correct on any thread that receives input.
    return settings.bypassLock != 0 &&
           (GetKeyState(settings.bypassLock) & 1) != 0;
}

// Records the bypass for this operation and reports whether it's active.
bool MarkBypass(IFileOperation* fileOperation) {
    {
        // Once bypassed, the whole operation stays bypassed even if the key
        // is released between two queued items.
        SrwSharedGuard guard(&g_stateLock);
        auto it = g_operationStates.find(fileOperation);
        if (it != g_operationStates.end() && it->second.bypassed) {
            return true;
        }
    }
    if (!BypassRequested()) {
        return false;
    }
    SrwExclusiveGuard guard(&g_stateLock);
    StateLocked(fileOperation).bypassed = true;
    Wh_Log(L"Bypass key held; leaving operation to Explorer");
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
// `queuedOut` reports how many items were queued, so the caller can tell a
// failure that touched nothing from a partial one.
HRESULT QueueMergedFolder(ItemOperation_t original,
                          IFileOperation* fileOperation,
                          IShellItem* sourceFolder,
                          const std::wstring& targetPath,
                          int depth,
                          size_t* queuedOut) {
    *queuedOut = 0;
    if (depth > kMaxMergeDepth) {
        return HRESULT_FROM_WIN32(ERROR_CANT_RESOLVE_FILENAME);
    }

    ComPtr<IShellItem> target;
    HRESULT hr = SHCreateItemFromParsingName(targetPath.c_str(), nullptr,
                                             IID_IShellItem, target.PutVoid());
    if (FAILED(hr)) {
        return hr;
    }

    // Enumerate through IShellFolder with explicit flags: the BHID_EnumItems
    // handler omits hidden and system items, which a merge must not skip.
    ComPtr<IShellFolder> folder;
    hr = sourceFolder->BindToHandler(nullptr, BHID_SFObject, IID_IShellFolder,
                                     folder.PutVoid());
    if (FAILED(hr)) {
        return hr;
    }
    ComPtr<IEnumIDList> children;
    hr = folder->EnumObjects(nullptr,
                             SHCONTF_FOLDERS | SHCONTF_NONFOLDERS |
                                 SHCONTF_INCLUDEHIDDEN |
                                 SHCONTF_INCLUDESUPERHIDDEN,
                             children.Put());
    if (FAILED(hr)) {
        return hr;
    }
    if (!children) {
        return S_OK;  // S_FALSE: nothing to enumerate.
    }

    HRESULT firstFailure = S_OK;
    for (;;) {
        PITEMID_CHILD childId = nullptr;
        ULONG fetched = 0;
        hr = children->Next(1, &childId, &fetched);
        if (FAILED(hr)) {
            Wh_Log(L"Enumerating %s failed: 0x%08X", targetPath.c_str(),
                   static_cast<unsigned>(hr));
            if (SUCCEEDED(firstFailure)) {
                firstFailure = hr;
            }
            break;
        }
        if (hr != S_OK || fetched == 0 || !childId) {
            break;
        }
        ComPtr<IShellItem> child;
        hr = SHCreateItemWithParent(nullptr, folder.Get(), childId,
                                    IID_IShellItem, child.PutVoid());
        CoTaskMemFree(childId);
        if (SUCCEEDED(hr)) {
            hr = QueueItem(original, fileOperation, child.Get(), target.Get(),
                           targetPath, nullptr, nullptr, depth + 1);
        }
        if (FAILED(hr)) {
            Wh_Log(L"Failed to queue an item: 0x%08X", static_cast<unsigned>(hr));
            if (SUCCEEDED(firstFailure)) {
                firstFailure = hr;
            }
        } else {
            ++*queuedOut;
        }
    }
    // An empty source folder queues nothing, and that's fine.
    return (*queuedOut || SUCCEEDED(firstFailure)) ? S_OK : firstFailure;
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
    const Settings settings = CurrentSettings();
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

        // A junction or symbolic link must not be merged: the merge would
        // drain the link's target and then remove the link itself.
        DWORD sourceAttributes = PathAttributes(sourcePath);
        bool isReparsePoint = sourceAttributes != INVALID_FILE_ATTRIBUTES &&
                              (sourceAttributes & FILE_ATTRIBUTE_REPARSE_POINT);

        FolderCollision mode = settings.folderCollision;
        if (isReparsePoint && mode == FolderCollision::kMerge) {
            mode = FolderCollision::kPrompt;
        }

        if (collides && mode == FolderCollision::kMerge) {
            Wh_Log(L"Merging folder: %s -> %s", sourcePath.c_str(),
                   targetPath.c_str());
            size_t queuedInMerge = 0;
            HRESULT hr = QueueMergedFolder(original, fileOperation, item,
                                           targetPath, depth, &queuedInMerge);
            if (FAILED(hr) && queuedInMerge == 0) {
                // Nothing was queued, so it's safe to let Explorer merge it.
                Wh_Log(L"Merge expansion failed (0x%08X); Explorer merge",
                       static_cast<unsigned>(hr));
                SrwExclusiveGuard guard(&g_stateLock);
                StateLocked(fileOperation).mergeFolderSeen = true;
                return original(fileOperation, item, destinationFolder,
                                requestedName, sink);
            }
            if (original == g_MoveItem_Original) {
                // Only now is it known that the folder's contents are on
                // the queue and its empty remains may be pruned.
                SrwExclusiveGuard guard(&g_stateLock);
                StateLocked(fileOperation).mergedMoveSources.push_back(sourcePath);
            }
            return hr;
        }

        if (mode != FolderCollision::kRename) {
            // Explorer's own merge. Make sure the fallback flags don't turn
            // it into a rename.
            if (collides) {
                SrwExclusiveGuard guard(&g_stateLock);
                StateLocked(fileOperation).mergeFolderSeen = true;
            }
            return original(fileOperation, item, destinationFolder,
                            requestedName, sink);
        }
        // kRename falls through to the same naming as files.
    }

    std::wstring uniqueName;
    std::wstring reservedName;
    LPCWSTR name = requestedName;
    bool renamed = item && destinationFolder &&
                   ChooseUniqueName(fileOperation, item, destinationPath,
                                    requestedName, uniqueName, reservedName);
    if (renamed) {
        Wh_Log(L"Collision: %s -> %s",
               requestedName ? requestedName : L"(item name)",
               uniqueName.c_str());
        name = uniqueName.c_str();
    }

    HRESULT hr = original(fileOperation, item, destinationFolder, name, sink);
    if (FAILED(hr) && !reservedName.empty()) {
        Unreserve(fileOperation, destinationPath, reservedName);
    }
    return hr;
}

// Collects the items behind the IUnknown accepted by CopyItems/MoveItems.
// Per the docs it may be an IShellItemArray, IDataObject, IEnumShellItems,
// IPersistIDList, or a single IShellItem. Returns E_NOINTERFACE only when
// the container is none of those, before anything has been collected.
HRESULT CollectItems(IUnknown* items, std::vector<ComPtr<IShellItem>>& out) {
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

    // Single item, tried last so a container that also exposes IShellItem
    // can't be mistaken for one item.
    ComPtr<IShellItem> single;
    if (SUCCEEDED(items->QueryInterface(IID_IShellItem, single.PutVoid()))) {
        out.push_back(std::move(single));
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
    if (FAILED(hr)) {
        // Nothing has been queued yet, so any failure to read the container
        // is safe to hand to the Shell. The fallback flags still avoid the
        // prompt, but the Shell picks the name.
        Wh_Log(L"Items hook: couldn't collect items (0x%08X); Shell fallback",
               static_cast<unsigned>(hr));
        return originalMany(fileOperation, items, destinationFolder);
    }

    // From here we own the queueing; never fall back to originalMany, or
    // items would be queued twice. Queued items can't be taken back, so a
    // failing item doesn't stop the rest; failure is reported only when
    // nothing at all was queued.
    std::wstring destinationPath = FolderPath(destinationFolder);
    HRESULT firstFailure = S_OK;
    size_t queued = 0;
    for (auto& item : collected) {
        hr = QueueItem(originalSingle, fileOperation, item.Get(),
                       destinationFolder, destinationPath, nullptr, nullptr, 0);
        if (FAILED(hr)) {
            Wh_Log(L"Failed to queue an item: 0x%08X", static_cast<unsigned>(hr));
            if (SUCCEEDED(firstFailure)) {
                firstFailure = hr;
            }
        } else {
            ++queued;
        }
    }
    return queued ? S_OK : firstFailure;
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

ULONG STDMETHODCALLTYPE Release_Hook(IFileOperation* fileOperation) {
    // Never call back into the object here; any COM call re-enters Release.
    ULONG refs = g_Release_Original(fileOperation);
    if (refs == 0 &&
        g_operationStateCount.load(std::memory_order_relaxed) != 0) {
        SrwExclusiveGuard guard(&g_stateLock);
        EraseStateLocked(fileOperation);
    }
    return refs;
}

HRESULT STDMETHODCALLTYPE SetOperationFlags_Hook(IFileOperation* fileOperation,
                                                  DWORD flags) {
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = StateLocked(fileOperation);
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

    // An IFileOperation may be reused for another run, so only the per-run
    // fields are reset here; the caller's flags stay, and Release_Hook
    // removes the entry when the object goes away.
    std::vector<std::wstring> mergedMoveSources;
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto it = g_operationStates.find(fileOperation);
        if (it != g_operationStates.end()) {
            OperationState& state = it->second;
            mergedMoveSources = std::move(state.mergedMoveSources);
            state.mergedMoveSources.clear();
            state.wantsFallback = false;
            state.mergeFolderSeen = false;
            state.bypassed = false;
            state.reservedTargets.clear();
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
// Hooking without the pin is not safe, so failure here is fatal to init.
bool PinImplementationModule(void* codeAddress) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_PIN,
                            reinterpret_cast<LPCWSTR>(codeAddress), &module)) {
        Wh_Log(L"GetModuleHandleExW failed: %u", GetLastError());
        return false;
    }
    WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(module, path, MAX_PATH)) {
        Wh_Log(L"IFileOperation implemented in %s", path);
    }
    return true;
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

        g_targets.release = vtable[kReleaseIndex];
        g_targets.setOperationFlags = vtable[kSetOperationFlagsIndex];
        g_targets.moveItem = vtable[kMoveItemIndex];
        g_targets.moveItems = vtable[kMoveItemsIndex];
        g_targets.copyItem = vtable[kCopyItemIndex];
        g_targets.copyItems = vtable[kCopyItemsIndex];
        g_targets.performOperations = vtable[kPerformOperationsIndex];
        g_targets.hr = PinImplementationModule(g_targets.setOperationFlags)
                           ? S_OK
                           : HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);

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
        g_targets.release,  g_targets.setOperationFlags, g_targets.moveItem,
        g_targets.moveItems, g_targets.copyItem,         g_targets.copyItems,
        g_targets.performOperations,
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

void LoadSettings() {
    Settings settings;

    WindhawkUtils::StringSetting folderCollision =
        WindhawkUtils::StringSetting::make(L"folderCollision");
    if (wcscmp(folderCollision.get(), L"prompt") == 0) {
        settings.folderCollision = FolderCollision::kPrompt;
    } else if (wcscmp(folderCollision.get(), L"rename") == 0) {
        settings.folderCollision = FolderCollision::kRename;
    }

    WindhawkUtils::StringSetting bypassKey =
        WindhawkUtils::StringSetting::make(L"bypassKey");
    if (wcscmp(bypassKey.get(), L"ctrl") == 0) {
        settings.bypassKey = VK_CONTROL;
    } else if (wcscmp(bypassKey.get(), L"alt") == 0) {
        settings.bypassKey = VK_MENU;
    } else if (wcscmp(bypassKey.get(), L"win") == 0) {
        settings.bypassKey = VK_LWIN;
        settings.bypassKeyAlt = VK_RWIN;
    } else if (wcscmp(bypassKey.get(), L"none") == 0) {
        settings.bypassKey = 0;
    }

    WindhawkUtils::StringSetting bypassLock =
        WindhawkUtils::StringSetting::make(L"bypassLock");
    if (wcscmp(bypassLock.get(), L"capslock") == 0) {
        settings.bypassLock = VK_CAPITAL;
    } else if (wcscmp(bypassLock.get(), L"scrolllock") == 0) {
        settings.bypassLock = VK_SCROLL;
    }

    SrwExclusiveGuard guard(&g_stateLock);
    g_settings = settings;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");
    LoadSettings();

    if (!ResolveHookTargets()) {
        Wh_Log(L"Couldn't resolve IFileOperation methods");
        return FALSE;
    }

    bool ok = true;

    ok &= !!WindhawkUtils::SetFunctionHook(
        reinterpret_cast<Release_t>(g_targets.release), Release_Hook,
        &g_Release_Original);
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

    Wh_Log(L"Uninitialized");
}

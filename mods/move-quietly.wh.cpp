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

Existing operation flags are retained. If the caller did not explicitly set
operation flags, the documented `IFileOperation` defaults are preserved before
the collision flags are added.

## Important behavior and limitations

- Copying an item into its own folder keeps Explorer's normal
  `name - Copy.ext` behavior; the mod only renames items arriving from a
  different folder.
- Items that are not file-system objects (for example dragged out of a zip
  folder or a phone) fall back to the Shell's own collision naming.
- Rename-on-collision applies to Shell items, which can include folders as well
  as files. A same-name folder encountered during a copy or move may therefore
  be renamed instead of following Explorer's normal folder-merge conflict path.
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

#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <windhawk_api.h>

namespace {

// IFileOperation uses these defaults when SetOperationFlags isn't called.
constexpr DWORD kDefaultOperationFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR;

#ifndef FOFX_PRESERVEFILEEXTENSIONS
#define FOFX_PRESERVEFILEEXTENSIONS 0x00200000
#endif

// Safety net only: we normally pick the name ourselves (see ChooseUniqueName).
constexpr DWORD kCollisionFlags =
    FOF_RENAMEONCOLLISION | FOFX_PRESERVEFILEEXTENSIONS;

// Upper bound on the "(n)" suffix we will search for.
constexpr unsigned kMaxSuffix = 100000;

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

struct OperationState {
    DWORD callerFlags = 0;
    bool callerFlagsKnown = false;
    bool copyOrMoveSeen = false;
    // Upper-cased full destination paths already claimed by items queued in
    // this operation but not yet on disk.
    std::unordered_set<std::wstring> reservedTargets;
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

// Keyed by the IFileOperation interface pointer. Every hooked method,
// including Release, is entered with that same pointer as `this`, so no
// identity lookup is needed. Never call back into the object from Release.
std::unordered_map<IFileOperation*, OperationState> g_operationStates;

void ForgetOperation(IFileOperation* fileOperation) {
    SrwExclusiveGuard guard(&g_stateLock);
    g_operationStates.erase(fileOperation);
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

bool PathExists(const std::wstring& path) {
    std::wstring probe = path;
    // Let the check work past MAX_PATH on drive-letter paths.
    if (probe.size() >= MAX_PATH && probe.size() > 2 && probe[1] == L':') {
        probe = L"\\\\?\\" + probe;
    }
    return GetFileAttributesW(probe.c_str()) != INVALID_FILE_ATTRIBUTES;
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

// Decides the name `item` should get in `destinationFolder`. Returns true and
// fills `uniqueName` only when the item must be renamed to avoid a collision.
bool ChooseUniqueName(IFileOperation* fileOperation,
                      IShellItem* item,
                      IShellItem* destinationFolder,
                      LPCWSTR requestedName,
                      std::wstring& uniqueName) {
    std::wstring destinationPath =
        DisplayName(destinationFolder, SIGDN_FILESYSPATH);
    if (destinationPath.empty()) {
        Wh_Log(L"Destination has no file-system path; Shell fallback");
        return false;
    }
    while (!destinationPath.empty() && destinationPath.back() == L'\\') {
        destinationPath.pop_back();
    }

    // Copying into the item's own folder keeps Explorer's "- Copy" naming,
    // and a move onto its own folder must not turn into a rename.
    ComPtr<IShellItem> parent;
    if (SUCCEEDED(item->GetParent(parent.Put())) && parent) {
        std::wstring parentPath = DisplayName(parent.Get(), SIGDN_FILESYSPATH);
        while (!parentPath.empty() && parentPath.back() == L'\\') {
            parentPath.pop_back();
        }
        if (!parentPath.empty() && EqualsIgnoreCase(parentPath, destinationPath)) {
            return false;
        }
    }

    std::wstring name = (requestedName && *requestedName)
                            ? std::wstring(requestedName)
                            : DisplayName(item, SIGDN_PARENTRELATIVEPARSING);
    if (name.empty() || name.find(L'\\') != std::wstring::npos) {
        return false;  // Not a plain leaf name; Shell fallback.
    }

    std::wstring base = name;
    std::wstring extension;
    if (!IsFolderItem(item)) {
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
    std::wstring fullPath;
    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& reserved = g_operationStates[fileOperation].reservedTargets;

        for (unsigned n = next;; ++n) {
            fullPath = destinationPath + L"\\" + candidate;
            std::wstring key = ToUpper(fullPath);
            if (!reserved.count(key) && !PathExists(fullPath)) {
                reserved.insert(key);
                break;
            }
            if (n > kMaxSuffix) {
                return false;  // Give up; Shell fallback.
            }
            candidate = base + L" (" + std::to_wstring(n) + L")" + extension;
        }
    }

    if (candidate == name) {
        return false;  // No collision; keep the caller's name.
    }

    uniqueName = candidate;
    return true;
}

void ArmRenameOnCollision(IFileOperation* fileOperation) {
    DWORD flags = kDefaultOperationFlags;

    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = g_operationStates[fileOperation];
        state.copyOrMoveSeen = true;
        if (state.callerFlagsKnown) {
            flags = state.callerFlags;
        }
    }

    // Call the trampoline directly so our SetOperationFlags hook doesn't
    // overwrite the caller's original flag snapshot.
    HRESULT hr = g_SetOperationFlags_Original(fileOperation,
                                              flags | kCollisionFlags);

    if (FAILED(hr)) {
        Wh_Log(L"SetOperationFlags(collision flags) failed: 0x%08X",
               static_cast<unsigned>(hr));
    }
}

// Queues one copy or move through the original single-item method, with a
// collision-free name when one is needed.
HRESULT QueueItem(ItemOperation_t original,
                  IFileOperation* fileOperation,
                  IShellItem* item,
                  IShellItem* destinationFolder,
                  LPCWSTR requestedName,
                  IFileOperationProgressSink* sink) {
    std::wstring uniqueName;
    LPCWSTR name = requestedName;
    if (item && destinationFolder &&
        ChooseUniqueName(fileOperation, item, destinationFolder, requestedName,
                         uniqueName)) {
        Wh_Log(L"Collision: %s -> %s",
               requestedName ? requestedName : L"(item name)",
               uniqueName.c_str());
        name = uniqueName.c_str();
    }
    return original(fileOperation, item, destinationFolder, name, sink);
}

// Queues every item behind the IUnknown accepted by CopyItems/MoveItems.
// Per the docs it may be an IShellItemArray, IDataObject, IEnumShellItems,
// IPersistIDList, or a single IShellItem. Returns E_NOINTERFACE when the
// container is none of those.
HRESULT QueueItemsFromContainer(ItemOperation_t originalSingle,
                                IFileOperation* fileOperation,
                                IUnknown* items,
                                IShellItem* destinationFolder) {
    // Single item (what Explorer's drop target sends, one call per item).
    ComPtr<IShellItem> single;
    if (SUCCEEDED(items->QueryInterface(IID_IShellItem, single.PutVoid()))) {
        return QueueItem(originalSingle, fileOperation, single.Get(),
                         destinationFolder, nullptr, nullptr);
    }

    // Array, possibly built from a data object.
    ComPtr<IShellItemArray> array;
    HRESULT hr = items->QueryInterface(IID_IShellItemArray, array.PutVoid());
    if (FAILED(hr)) {
        ComPtr<IDataObject> dataObject;
        if (SUCCEEDED(items->QueryInterface(IID_IDataObject,
                                            dataObject.PutVoid()))) {
            hr = SHCreateShellItemArrayFromDataObject(
                dataObject.Get(), IID_IShellItemArray, array.PutVoid());
        }
    }
    if (SUCCEEDED(hr) && array) {
        DWORD count = 0;
        hr = array->GetCount(&count);
        if (FAILED(hr)) {
            return hr;
        }
        for (DWORD i = 0; i < count; ++i) {
            ComPtr<IShellItem> item;
            hr = array->GetItemAt(i, item.Put());
            if (FAILED(hr)) {
                return hr;
            }
            hr = QueueItem(originalSingle, fileOperation, item.Get(),
                           destinationFolder, nullptr, nullptr);
            if (FAILED(hr)) {
                return hr;
            }
        }
        return S_OK;
    }

    // Enumerator.
    ComPtr<IEnumShellItems> enumerator;
    if (SUCCEEDED(items->QueryInterface(IID_IEnumShellItems,
                                        enumerator.PutVoid()))) {
        for (;;) {
            ComPtr<IShellItem> item;
            ULONG fetched = 0;
            hr = enumerator->Next(1, item.Put(), &fetched);
            if (hr != S_OK || fetched == 0) {
                return S_OK;
            }
            hr = QueueItem(originalSingle, fileOperation, item.Get(),
                           destinationFolder, nullptr, nullptr);
            if (FAILED(hr)) {
                return hr;
            }
        }
    }

    // Item identified by an ID list.
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
        return QueueItem(originalSingle, fileOperation, item.Get(),
                         destinationFolder, nullptr, nullptr);
    }

    return E_NOINTERFACE;
}

HRESULT QueueItems(ItemOperation_t originalSingle,
                   ItemsOperation_t originalMany,
                   IFileOperation* fileOperation,
                   IUnknown* items,
                   IShellItem* destinationFolder) {
    if (items && destinationFolder) {
        HRESULT hr = QueueItemsFromContainer(originalSingle, fileOperation,
                                             items, destinationFolder);
        if (hr != E_NOINTERFACE) {
            return hr;
        }
    }
    // Unknown container: let the Shell handle it; the collision flags still
    // avoid the prompt, but the Shell picks the name.
    Wh_Log(L"Items hook: unrecognized container; Shell fallback");
    return originalMany(fileOperation, items, destinationFolder);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

ULONG STDMETHODCALLTYPE Release_Hook(IFileOperation* fileOperation) {
    // Do NOT touch the object here (no QueryInterface, no AddRef): any COM
    // call from this hook re-enters Release and recurses without bound.
    // After the original returns 0 the pointer is only used as a map key.
    ULONG count = g_Release_Original(fileOperation);
    if (count == 0) {
        ForgetOperation(fileOperation);
    }
    return count;
}

HRESULT STDMETHODCALLTYPE SetOperationFlags_Hook(IFileOperation* fileOperation,
                                                  DWORD flags) {
    bool copyOrMoveSeen = false;

    {
        SrwExclusiveGuard guard(&g_stateLock);
        auto& state = g_operationStates[fileOperation];
        state.callerFlags = flags;
        state.callerFlagsKnown = true;
        copyOrMoveSeen = state.copyOrMoveSeen;
    }

    if (copyOrMoveSeen) {
        flags |= kCollisionFlags;
    }

    return g_SetOperationFlags_Original(fileOperation, flags);
}

HRESULT STDMETHODCALLTYPE MoveItem_Hook(
    IFileOperation* fileOperation,
    IShellItem* item,
    IShellItem* destinationFolder,
    LPCWSTR newName,
    IFileOperationProgressSink* sink) {
    ArmRenameOnCollision(fileOperation);
    return QueueItem(g_MoveItem_Original, fileOperation, item,
                     destinationFolder, newName, sink);
}

HRESULT STDMETHODCALLTYPE MoveItems_Hook(
    IFileOperation* fileOperation,
    IUnknown* items,
    IShellItem* destinationFolder) {
    ArmRenameOnCollision(fileOperation);
    return QueueItems(g_MoveItem_Original, g_MoveItems_Original, fileOperation,
                      items, destinationFolder);
}

HRESULT STDMETHODCALLTYPE CopyItem_Hook(
    IFileOperation* fileOperation,
    IShellItem* item,
    IShellItem* destinationFolder,
    LPCWSTR copyName,
    IFileOperationProgressSink* sink) {
    ArmRenameOnCollision(fileOperation);
    return QueueItem(g_CopyItem_Original, fileOperation, item,
                     destinationFolder, copyName, sink);
}

HRESULT STDMETHODCALLTYPE CopyItems_Hook(
    IFileOperation* fileOperation,
    IUnknown* items,
    IShellItem* destinationFolder) {
    ArmRenameOnCollision(fileOperation);
    return QueueItems(g_CopyItem_Original, g_CopyItems_Original, fileOperation,
                      items, destinationFolder);
}

HRESULT STDMETHODCALLTYPE PerformOperations_Hook(IFileOperation* fileOperation) {
    HRESULT hr = g_PerformOperations_Original(fileOperation);
    ForgetOperation(fileOperation);
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

        g_targets.release = vtable[kReleaseIndex];
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

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Move Conflicts Quietly");

    if (!ResolveHookTargets()) {
        Wh_Log(L"Couldn't resolve IFileOperation methods");
        return FALSE;
    }

    bool ok = true;

    ok &= !!Wh_SetFunctionHook(g_targets.release,
                               reinterpret_cast<void*>(Release_Hook),
                               reinterpret_cast<void**>(&g_Release_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.setOperationFlags,
        reinterpret_cast<void*>(SetOperationFlags_Hook),
        reinterpret_cast<void**>(&g_SetOperationFlags_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.moveItem, reinterpret_cast<void*>(MoveItem_Hook),
        reinterpret_cast<void**>(&g_MoveItem_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.moveItems, reinterpret_cast<void*>(MoveItems_Hook),
        reinterpret_cast<void**>(&g_MoveItems_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.copyItem, reinterpret_cast<void*>(CopyItem_Hook),
        reinterpret_cast<void**>(&g_CopyItem_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.copyItems, reinterpret_cast<void*>(CopyItems_Hook),
        reinterpret_cast<void**>(&g_CopyItems_Original));

    ok &= !!Wh_SetFunctionHook(
        g_targets.performOperations,
        reinterpret_cast<void*>(PerformOperations_Hook),
        reinterpret_cast<void**>(&g_PerformOperations_Original));

    if (!ok) {
        Wh_Log(L"Failed to register one or more IFileOperation hooks");
        return FALSE;
    }

    Wh_Log(L"IFileOperation hooks registered successfully");
    return TRUE;
}

void Wh_ModUninit() {
    {
        SrwExclusiveGuard guard(&g_stateLock);
        g_operationStates.clear();
    }

    Wh_Log(L"Uninitialized Move Conflicts Quietly");
}

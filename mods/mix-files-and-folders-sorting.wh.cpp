// ==WindhawkMod==
// @id              mix-files-and-folders-sorting
// @name            Mix Files and Folders When Sorting in Windows Explorer
// @description     Interleave files and folders when sorting Explorer by any column, instead of always grouping all folders before (or after) all files
// @version         1.5
// @author          Extremenis
// @github          https://github.com/Extremenis
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![MixFilesAndFolders](https://i.imgur.com/XVMBh65.png)

# Mix files and folders when sorting

Windows Explorer always lists all folders before (or after, when sorting
descending) all files, regardless of which column is sorted by. This is
hardcoded shell behavior, not a UI setting, and it's independent from
"Group by".

This mod removes that separation: a folder and a file are compared according
to the selected column's value (name, date, ...) instead of the folder
automatically winning. Folder-vs-folder and file-vs-file comparisons are left
to the shell untouched, so Explorer's own ordering still applies there.

Only real directories count as folders here. Items that Explorer shows as
browsable but stores as files — `.zip` and other compressed folders, folder
shortcuts — are treated as files, which is what stock Explorer does for its
folders-first rule too.

## Scope

This only affects regular file-system folders (the `CFSFolder` shell object,
i.e. anything you'd browse to on disk). Special views such as Libraries,
"This PC", and search results use different, independent shell folder
implementations that this mod doesn't touch, so folders may still be grouped
first there. The Desktop is a composite view that delegates comparisons
between two file-system items to the backing `CFSFolder`, so desktop icon
sorting is affected.

The mod is also scoped to `explorer.exe` only, so common file/save dialogs in
other applications — which use the same `CFSFolder` implementation, just in
a different process — keep the default folders-first ordering.

Sorting by Size is left to the shell entirely. Folders report an empty size, so
that column never interleaved on its own, and
[`explorer-details-better-file-sizes`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-details-better-file-sizes.wh.cpp)
already owns it — see below.

Sorting by Type does not interleave either, for a different reason: the Type
column carries `PKEY_ItemTypeText` ("File folder", "Application extension"),
which isn't one of `CFSFolder`'s columns at all — Explorer sorts it outside
`CFSFolder::CompareIDs`, so the folders-first grouping there is out of this
mod's reach.

## Interaction with `explorer-details-better-file-sizes`

[`explorer-details-better-file-sizes`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-details-better-file-sizes.wh.cpp)
hooks the same `CFSFolder::CompareIDs` function and has its own
"Mix files and folders when sorting by size" setting (`sortSizesMixFolders`,
on by default). Both hooks delegate to the original for anything they don't
handle, so they chain rather than fight: that mod handles the Size column,
this one handles the rest.

To keep that clean, this mod returns the Size column to the original
comparison untouched. That mod's `sortSizesMixFolders` setting therefore stays
authoritative for Size whether it's on or off, and the result no longer depends
on which mod's hook happens to run first.

## Known limitations

The shell's own file-vs-file comparison and this mod's folder-vs-file
comparison are separate pieces of code, so they can disagree at the margins
and place a folder in a spot that looks slightly off relative to nearby
files. Known cases: date columns reported as `VT_DATE` are compared as a
double (~ms resolution) where the shell compares `FILETIME` at 100 ns;
locale collation of punctuation or accented characters may differ; and a
blank value is sorted before any present value, which the shell doesn't
necessarily do for sparsely-populated columns (Date taken, Length,
Dimensions).

## Credit / related mod

The `CFSFolder` hooking scaffolding (symbol hook, hook lifetime tracking,
`CompareIDs` hook skeleton) is derived from m417z's
`explorer-details-better-file-sizes`, which is licensed under GPLv3; this mod
is licensed under GPLv3 for that reason.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <atomic>
#include <memory>

#include <initguid.h>
#include <propidl.h>
#include <propkey.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <shtypes.h>

#include <winrt/base.h>

std::atomic<int> g_hookRefCount;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                            void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// GetDetailsEx takes a VARIANT*, but a column supplied by a third-party
// property or column handler can come back tagged with a PROPVARIANT-only
// type (VT_LPWSTR, VT_FILETIME, VT_CLSID). VariantClear doesn't free those,
// so it would leak; PropVariantClear handles both families, and the two
// structs share their layout.
//
// Zero-initialize the whole struct rather than calling VariantInit: VariantInit
// only promises to set vt to VT_EMPTY, leaving wReserved1/2/3 and the union as
// stack garbage. PropVariantClear validates those reserved fields and rejects
// the structure if they're non-zero, which would leak the payload on every
// folder-vs-file comparison.
class ShellVariant {
   public:
    ShellVariant() = default;
    ~ShellVariant() { PropVariantClear(AsPropVariant()); }
    ShellVariant(const ShellVariant&) = delete;
    ShellVariant& operator=(const ShellVariant&) = delete;

    VARIANT* GetAddress() { return &m_value; }
    const VARIANT& Get() const { return m_value; }
    const PROPVARIANT* AsPropVariant() const {
        return reinterpret_cast<const PROPVARIANT*>(&m_value);
    }
    PROPVARIANT* AsPropVariant() {
        return reinterpret_cast<PROPVARIANT*>(&m_value);
    }

   private:
    VARIANT m_value{};
};

// Explorer honors the NoStrCmpLogical policy under
// Software\Microsoft\Windows\CurrentVersion\Policies\Explorer: when it's set,
// "numerical sorting" is off and the shell stops comparing names logically.
// This hook's folder-vs-file comparison has to follow the same rule as the
// shell's file-vs-file comparison, or the two orderings disagree (e.g. the
// shell puts a10.txt before a2.txt while a folder named a5 lands between
// them). It can be set per user or as a machine policy, so check both.
// Cached for the process lifetime; Explorer re-reads it on policy-change
// notifications, so flipping the policy mid-session needs an Explorer
// restart for the two to agree again.
bool NumericalSortEnabled() {
    static const bool enabled = [] {
        auto policySet = [](HKEY root) {
            DWORD value = 0;
            DWORD size = sizeof(value);
            LSTATUS status = RegGetValueW(
                root,
                LR"(Software\Microsoft\Windows\CurrentVersion\Policies\Explorer)",
                L"NoStrCmpLogical", RRF_RT_REG_DWORD, nullptr, &value, &size);
            return status == ERROR_SUCCESS && value != 0;
        };
        return !policySet(HKEY_CURRENT_USER) && !policySet(HKEY_LOCAL_MACHINE);
    }();
    return enabled;
}

using CFSFolder_CompareIDs_t =
    HRESULT(WINAPI*)(void* pCFSFolder,
                      LPARAM column,
                      const ITEMIDLIST_RELATIVE* itemid1,
                      const ITEMIDLIST_RELATIVE* itemid2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;

// CompareIDs returns its result as the low 16 bits of an HRESULT success
// code: 1 means itemid1 > itemid2, 0xFFFF (-1 as a short) means
// itemid1 < itemid2, and 0 means equal.
HRESULT CompareResultFromInt(int cmp) {
    if (cmp > 0) {
        return 1;
    }
    if (cmp < 0) {
        return 0xFFFF;
    }
    return 0;
}

// True if the pidl has more than one level. GetDetailsEx expects a child
// pidl, so deeper ones have to go to the shell. This is what ILNext does,
// open-coded to avoid pulling in <shlobj.h> for a single inline helper: the
// next SHITEMID starts cb bytes after this one, and a cb of 0 is the
// terminator.
bool IsMultiLevelPidl(const ITEMIDLIST_RELATIVE* itemid) {
    USHORT cb = itemid->mkid.cb;
    if (cb == 0) {
        return false;
    }
    auto next = (const ITEMIDLIST_RELATIVE*)((const BYTE*)itemid + cb);
    return next->mkid.cb != 0;
}

// True only for a real directory: SFGAO_FOLDER set and SFGAO_STREAM clear.
//
// SFGAO_FOLDER alone is not "is a directory": a .zip and a folder shortcut both
// report it, while Explorer's folders-first rule keys off
// FILE_ATTRIBUTE_DIRECTORY, which is why a .zip sorts among the files in stock
// Explorer. Treating one as a folder here would make the comparator
// non-transitive.
//
// Asking per item rather than passing both pidls in one call matters: the
// two-item form of GetAttributesOf returns the bits *common to both*, which
// can't distinguish "directory vs .zip" (common FOLDER) from "both directories",
// nor tell a pair that reports neither bit from a genuine folder/file pair.
// For CFSFolder children these attributes come out of the pidl, so the second
// call is cheap.
bool IsRealFolder(IShellFolder2* shellFolder,
                   const ITEMIDLIST_RELATIVE* itemid,
                   bool* isFolder) {
    PCUITEMID_CHILD child = (PCUITEMID_CHILD)itemid;
    SFGAOF attrs = SFGAO_FOLDER | SFGAO_STREAM;
    if (FAILED(shellFolder->GetAttributesOf(1, &child, &attrs))) {
        return false;
    }
    *isFolder = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
    return true;
}

HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pCFSFolder,
                                          LPARAM column,
                                          const ITEMIDLIST_RELATIVE* itemid1,
                                          const ITEMIDLIST_RELATIVE* itemid2) {
    auto hookScope = hookRefCountScope();

    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pCFSFolder, column, itemid1,
                                              itemid2);
    };

    if (!itemid1 || !itemid2) {
        return original();
    }

    // The low word of `column` is the actual column index
    // (SHCIDS_COLUMNMASK); the high bits carry flags such as
    // SHCIDS_ALLFIELDS (identity queries) or SHCIDS_CANONICALONLY. Only
    // handle plain column sorts and let the shell handle anything else.
    // This runs before any lookups below: CompareIDs is also called with
    // SHCIDS_ALLFIELDS for item-identity lookups (locating a row after a
    // rename, a change notification, a refresh), far more often than for
    // actual sorting, and those calls should cost nothing.
    if (column & ~(LPARAM)SHCIDS_COLUMNMASK) {
        return original();
    }

    // An empty pidl isn't a child item the property calls below can work with,
    // so bail out explicitly rather than letting them fail one by one.
    if (itemid1->mkid.cb == 0 || itemid2->mkid.cb == 0 ||
        IsMultiLevelPidl(itemid1) || IsMultiLevelPidl(itemid2)) {
        return original();
    }

    // Everything below goes through the folder's own vtable rather than
    // separately-resolved CFSFolder symbols: fewer mangled names to break on
    // a future Windows build, correct dispatch if the object is ever a
    // derived class, and -- unlike calling an unhooked _Original -- it picks
    // up other mods' hooks on these methods (e.g. computed folder sizes).
    winrt::com_ptr<IShellFolder2> shellFolder;
    if (FAILED(((IUnknown*)pCFSFolder)
                   ->QueryInterface(IID_IShellFolder2,
                                    shellFolder.put_void()))) {
        return original();
    }

    // Folders-first is the only rule this mod needs to override, so it only
    // steps in when exactly one of the two items is a real directory. Anything
    // else -- two directories, two files, or an item whose attributes couldn't
    // be read -- is left to the shell's own comparison.
    bool isFolder1, isFolder2;
    if (!IsRealFolder(shellFolder.get(), itemid1, &isFolder1) ||
        !IsRealFolder(shellFolder.get(), itemid2, &isFolder2) ||
        isFolder1 == isFolder2) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(shellFolder->MapColumnToSCID((UINT)column, &columnSCID))) {
        return original();
    }

    // Leave the Size column alone. explorer-details-better-file-sizes owns it:
    // it hooks the same function and exposes a dedicated "Mix files and folders
    // when sorting by size" toggle. Comparing size values here would override
    // that setting for anyone who deliberately turned it off, and the outcome
    // would otherwise depend on which mod's hook runs first. Folders report an
    // empty size anyway, so this column never interleaved on its own.
    if (IsEqualPropertyKey(columnSCID, PKEY_Size)) {
        return original();
    }

    // PKEY_ItemNameDisplay (the Name column) resolves to
    // GetDisplayNameOf(SHGDN_NORMAL), which honors "Hide extensions for known
    // file types" and so can omit a file's extension. The shell's own
    // file-vs-file CompareIDs for Name instead compares the real underlying
    // name, extension included. Match that here: using the display name for
    // one comparison key and the real name for another would make the overall
    // ordering non-transitive.
    const PROPERTYKEY& valueSCID =
        IsEqualPropertyKey(columnSCID, PKEY_ItemNameDisplay) ? PKEY_FileName
                                                              : columnSCID;

    ShellVariant value1;
    ShellVariant value2;
    if (FAILED(shellFolder->GetDetailsEx((PCUITEMID_CHILD)itemid1, &valueSCID,
                                          value1.GetAddress())) ||
        FAILED(shellFolder->GetDetailsEx((PCUITEMID_CHILD)itemid2, &valueSCID,
                                          value2.GetAddress()))) {
        return original();
    }

    const VARIANT& v1 = value1.Get();
    const VARIANT& v2 = value2.Get();

    auto isEmpty = [](const VARIANT& v) {
        return v.vt == VT_EMPTY || v.vt == VT_NULL;
    };

    if (v1.vt != v2.vt) {
        // A folder commonly leaves a column blank (VT_EMPTY) where a file has
        // a real value (Size, Dimensions, Length, ...). Treat a blank value as
        // sorting before any present value, rather than falling back to
        // folders-first.
        bool empty1 = isEmpty(v1);
        bool empty2 = isEmpty(v2);
        if (empty1 != empty2) {
            return CompareResultFromInt(empty1 ? -1 : 1);
        }
        return original();
    }

    int cmp;
    switch (v1.vt) {
        case VT_EMPTY:
        case VT_NULL:
            cmp = 0;
            break;

        case VT_BSTR:
        case VT_LPWSTR: {
            // Both tags carry a wide string in the same union slot; read it
            // through the PROPVARIANT view so VT_LPWSTR is well-defined.
            LPCWSTR s1 = value1.AsPropVariant()->pwszVal;
            LPCWSTR s2 = value2.AsPropVariant()->pwszVal;
            s1 = s1 ? s1 : L"";
            s2 = s2 ? s2 : L"";
            cmp = NumericalSortEnabled() ? StrCmpLogicalW(s1, s2)
                                          : lstrcmpiW(s1, s2);
            break;
        }

        case VT_FILETIME:
            cmp = CompareFileTime(&value1.AsPropVariant()->filetime,
                                   &value2.AsPropVariant()->filetime);
            break;

        case VT_UI8:
            cmp = (v1.ullVal > v2.ullVal) - (v1.ullVal < v2.ullVal);
            break;

        case VT_I8:
            cmp = (v1.llVal > v2.llVal) - (v1.llVal < v2.llVal);
            break;

        case VT_UI4:
            cmp = (v1.ulVal > v2.ulVal) - (v1.ulVal < v2.ulVal);
            break;

        case VT_I4:
            cmp = (v1.lVal > v2.lVal) - (v1.lVal < v2.lVal);
            break;

        case VT_UI2:
            cmp = (int)v1.uiVal - (int)v2.uiVal;
            break;

        case VT_I2:
            cmp = (int)v1.iVal - (int)v2.iVal;
            break;

        case VT_UI1:
            cmp = (int)v1.bVal - (int)v2.bVal;
            break;

        case VT_R8:
        case VT_DATE:
            cmp = (v1.dblVal > v2.dblVal) - (v1.dblVal < v2.dblVal);
            break;

        case VT_R4:
            cmp = (v1.fltVal > v2.fltVal) - (v1.fltVal < v2.fltVal);
            break;

        case VT_BOOL:
            // VARIANT_TRUE is -1, so comparing boolVal directly would sort
            // true before false. Normalize to 0/1 first.
            cmp = (int)(!!v1.boolVal) - (int)(!!v2.boolVal);
            break;

        default:
            // Unsupported/exotic column type: fall back to the default
            // folders-first behavior rather than guessing.
            return original();
    }

    if (cmp == 0) {
        // CompareIDs returning 0 means "these are the same item" to the shell
        // (used for locating a row after a rename/change notification), not
        // just "sorts equal". Two distinct items with equal values in this
        // column must not be reported as identical, so delegate the tiebreak
        // to the original implementation instead.
        return original();
    }

    return CompareResultFromInt(cmp);
}

bool HookWindowsStorageSymbols() {
    // Wh_ModInit runs before explorer.exe has necessarily loaded
    // windows.storage.dll (e.g. right after sign-in or an Explorer restart),
    // so GetModuleHandle can't be relied on here -- load it explicitly
    // instead. LOAD_LIBRARY_SEARCH_SYSTEM32 restricts the search to the
    // system directory, which is also what prevents this from being a
    // DLL-hijacking vector.
    HMODULE windowsStorageModule = LoadLibraryExW(
        L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::CompareIDs(long,struct _ITEMIDLIST_RELATIVE const *,struct _ITEMIDLIST_RELATIVE const *))",
#endif
            },
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
    };

    return HookSymbols(windowsStorageModule, hooks, ARRAYSIZE(hooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!HookWindowsStorageSymbols()) {
        Wh_Log(L"Failed hooking windows.storage.dll symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookRefCount > 0) {
        Sleep(200);
    }
}
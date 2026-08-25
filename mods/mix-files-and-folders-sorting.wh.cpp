// ==WindhawkMod==
// @id              mix-files-and-folders-sorting
// @name            Mix Files and Folders When Sorting in Windows Explorer
// @description     Interleave files and folders when sorting Explorer by any column, instead of always grouping all folders before (or after) all files
// @version         1.3
// @author          Extremenis
// @github          https://github.com/Extremenis
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi
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
to the selected column's value (name, type, date, ...) instead of the folder
automatically winning. Folder-vs-folder and file-vs-file comparisons are left
to the shell untouched, so Explorer's own ordering still applies there.

Only real directories count as folders here. Items that Explorer shows as
browsable but stores as files — `.zip` and other compressed folders, folder
shortcuts — are treated as files, which is what stock Explorer does for its
folders-first rule too.

## Scope

This only affects regular file-system folders (the `CFSFolder` shell object,
i.e. anything you'd browse to on disk). Special views such as the Desktop,
Libraries, "This PC", and search results use different, independent shell
folder implementations that this mod doesn't touch, so folders may still be
grouped first there.

The mod is also scoped to `explorer.exe` only, so common file/save dialogs in
other applications — which use the same `CFSFolder` implementation, just in
a different process — keep the default folders-first ordering.

Sorting by Size does not interleave on its own: folders report an empty size,
so a folder-vs-file comparison on that column ends up in the same order as
stock Explorer.

## Interaction with `explorer-details-better-file-sizes`

[`explorer-details-better-file-sizes`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-details-better-file-sizes.wh.cpp)
hooks the same `CFSFolder::CompareIDs` function and has its own
"Mix files and folders when sorting by size" setting (`sortSizesMixFolders`,
on by default). Both hooks delegate to the original for anything they don't
handle, so they chain rather than fight: that mod handles the Size column,
this one handles the rest.

Two things follow from running both:

- With that mod's `calculateFolderSizes` enabled, folders report a real size,
  so the Size column *does* interleave here — the caveat above no longer
  applies.
- This mod doesn't read `sortSizesMixFolders`, so setting it to `false` while
  `calculateFolderSizes` is on will still leave the Size column mixed.

## Known limitations

The shell's own file-vs-file comparison and this mod's folder-vs-file
comparison are separate pieces of code, so they can disagree at the margins
and place a folder in a spot that looks slightly off relative to nearby
files. Known cases: date columns are compared as `VT_DATE` (a double, ~ms
resolution) where the shell compares `FILETIME` at 100 ns; and locale
collation of punctuation or accented characters may differ from the shell's.

## Credit / related mod

The `CFSFolder` hooking scaffolding (symbol hooks, hook lifetime tracking,
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
class ShellVariant {
   public:
    ShellVariant() { VariantInit(&m_value); }
    ~ShellVariant() {
        PropVariantClear(reinterpret_cast<PROPVARIANT*>(&m_value));
    }
    ShellVariant(const ShellVariant&) = delete;
    ShellVariant& operator=(const ShellVariant&) = delete;

    VARIANT* GetAddress() { return &m_value; }
    const VARIANT& Get() const { return m_value; }

   private:
    VARIANT m_value;
};

// Explorer honors the NoStrCmpLogical policy under
// Software\Microsoft\Windows\CurrentVersion\Policies\Explorer: when it's set,
// "numerical sorting" is off and the shell stops comparing names logically.
// This hook's folder-vs-file comparison has to follow the same rule as the
// shell's file-vs-file comparison, or the two orderings disagree (e.g. the
// shell puts a10.txt before a2.txt while a folder named a5 lands between
// them). It can be set per user or as a machine policy, so check both. Read
// once -- it's a policy value, not something that changes while Explorer runs.
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

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                       int column,
                                                       PROPERTYKEY* scid);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                    const ITEMID_CHILD* itemid,
                                                    const PROPERTYKEY* scid,
                                                    VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;

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

// Reports whether an item is a real directory.
//
// SFGAO_FOLDER alone means "browsable container", not "directory": a .zip
// handled by Compressed Folders, and a shortcut to a folder, both report it.
// Explorer's own folders-first rule keys off FILE_ATTRIBUTE_DIRECTORY, which
// is why a .zip sorts among the files in stock Explorer. Treating one as a
// folder here would make the comparator non-transitive -- a .zip vs a real
// folder would be delegated to the shell (folder first, unconditionally)
// while .zip vs file and folder vs file both get compared by column value,
// which admits a cycle. SFGAO_STREAM is set for exactly these file-backed
// containers, so requiring it to be clear matches the shell's rule.
HRESULT IsFolderItem(IShellFolder* shellFolder,
                      const ITEMIDLIST_RELATIVE* itemid,
                      bool* isFolder) {
    SFGAOF attrs = SFGAO_FOLDER | SFGAO_STREAM;
    HRESULT hr = shellFolder->GetAttributesOf(
        1, (PCUITEMID_CHILD_ARRAY)&itemid, &attrs);
    if (FAILED(hr)) {
        Wh_Log(L"GetAttributesOf failed: 0x%08X", hr);
        return hr;
    }
    *isFolder = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
    return S_OK;
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

    // Folders-first is the only rule this mod needs to override, so only step
    // in when exactly one of the two items is a real directory.
    // Folder-vs-folder and file-vs-file pairs go to the shell's own
    // comparison: it's cheaper, and it guarantees the exact ordering Explorer
    // would otherwise use. QueryInterface rather than casting pCFSFolder --
    // the this-pointer's primary vtable being IShellFolder2's is an
    // undocumented layout assumption.
    IShellFolder2* shellFolder = nullptr;
    if (FAILED(((IUnknown*)pCFSFolder)
                   ->QueryInterface(IID_IShellFolder2, (void**)&shellFolder))) {
        return original();
    }
    auto shellFolderRelease = std::unique_ptr<IShellFolder2, void (*)(
        IShellFolder2*)>{shellFolder, [](auto p) { p->Release(); }};

    bool isFolder1, isFolder2;
    if (FAILED(IsFolderItem(shellFolder, itemid1, &isFolder1)) ||
        FAILED(IsFolderItem(shellFolder, itemid2, &isFolder2))) {
        return original();
    }
    if (isFolder1 == isFolder2) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(
            pCFSFolder, (int)(column & SHCIDS_COLUMNMASK), &columnSCID))) {
        return original();
    }

    // PKEY_ItemNameDisplay (the Name column) resolves to
    // ISF::GetDisplayNameOf(SHGDN_NORMAL), which honors "Hide extensions for
    // known file types" and so can omit a file's extension. The shell's own
    // file-vs-file CompareIDs for Name instead compares the real underlying
    // file/folder name (PKEY_FileName), extension included. Match that here:
    // using the display name for one comparison key and the real name for
    // another would make the overall ordering non-transitive.
    const PROPERTYKEY& valueSCID =
        IsEqualPropertyKey(columnSCID, PKEY_ItemNameDisplay) ? PKEY_FileName
                                                              : columnSCID;

    ShellVariant value1;
    ShellVariant value2;
    if (FAILED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid1, &valueSCID, value1.GetAddress())) ||
        FAILED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid2, &valueSCID, value2.GetAddress()))) {
        return original();
    }

    const VARIANT& v1 = value1.Get();
    const VARIANT& v2 = value2.Get();

    auto isEmpty = [](const VARIANT& v) {
        return v.vt == VT_EMPTY || v.vt == VT_NULL;
    };

    if (v1.vt != v2.vt) {
        // A folder commonly leaves a column blank (VT_EMPTY) where a file
        // has a real value (Size, Dimensions, Length, ...). Treat a blank
        // value as sorting before any present value, rather than falling
        // back to folders-first.
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

        case VT_BSTR: {
            LPCWSTR s1 = v1.bstrVal ? (LPCWSTR)v1.bstrVal : L"";
            LPCWSTR s2 = v2.bstrVal ? (LPCWSTR)v2.bstrVal : L"";
            cmp = NumericalSortEnabled() ? StrCmpLogicalW(s1, s2)
                                          : lstrcmpiW(s1, s2);
            break;
        }

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
        // CompareIDs returning 0 means "these are the same item" to the
        // shell (used for locating a row after a rename/change
        // notification), not just "sorts equal". Two distinct items with
        // equal values in this column must not be reported as identical,
        // so delegate the tiebreak to the original implementation instead.
        return original();
    }

    return CompareResultFromInt(cmp);
}

bool HookWindowsStorageSymbols() {
    // Wh_ModInit runs before explorer.exe has necessarily loaded
    // windows.storage.dll (e.g. right after sign-in or an Explorer
    // restart), so GetModuleHandle can't be relied on here -- load it
    // explicitly instead. LOAD_LIBRARY_SEARCH_SYSTEM32 restricts the search
    // to the system directory, which is also what prevents this from being
    // a DLL-hijacking vector.
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
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#endif
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *))",
#endif
            },
            &CFSFolder_GetDetailsEx_Original,
        },
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
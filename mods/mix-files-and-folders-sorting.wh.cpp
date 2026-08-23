// ==WindhawkMod==
// @id              mix-files-and-folders-sorting
// @name            Mix Files and Folders When Sorting in Windows Explorer
// @description     Interleave files and folders when sorting Explorer by any column, instead of always grouping all folders before (or after) all files
// @version         1.1
// @author          Extremenis
// @github          https://github.com/Extremenis
// @include         explorer.exe
// @compilerOptions -loleaut32 -lshlwapi
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

This mod removes that separation: with it enabled, a folder and a file are
compared purely according to the selected column's value (name, size, type,
date, ...), instead of the folder automatically winning. Folder-vs-folder
and file-vs-file comparisons are left untouched, so Explorer's own ordering
(including numerical sorting and locale rules) still applies there.

## Scope

This only affects regular file-system folders (the `CFSFolder` shell object,
i.e. anything you'd browse to on disk). Special views such as the Desktop,
Libraries, "This PC", and search results use different, independent shell
folder implementations that this mod doesn't touch, so folders may still be
grouped first there.

The mod is also scoped to `explorer.exe` only, so common file/save dialogs in
other applications — which use the same `CFSFolder` implementation, just in
a different process — keep the default folders-first ordering.

Sorting by Size does not actually interleave: folders always report an empty
size, so a folder-vs-file comparison on that column ends up in the same
order as stock Explorer. Real size-based mixing needs folders to have a
computed size, which this mod doesn't provide.

## Credit / related mod

The `CFSFolder` hooking scaffolding (symbol hooks, hook lifetime tracking,
`CompareIDs` hook skeleton) is derived from m417z's
[`explorer-details-better-file-sizes`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-details-better-file-sizes.wh.cpp),
which is licensed under GPLv3; this mod is licensed under GPLv3 as well for
that reason. That mod already has a "Mix files and folders when sorting by
size" setting implemented via the same hook. This mod generalizes the same
idea to every column, at the cost of overlapping functionality: both mods
hook `CFSFolder::CompareIDs`, so if both are enabled at once the result
depends on which mod's hook runs first.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mixFoldersAndFiles: true
  $name: Mix Files and Folders
  $description: >-
    When enabled, a folder and a file are interleaved when sorting by any
    column, instead of the folder always coming first. When disabled, the
    hook isn't installed at all and Explorer's default folders-first
    behavior applies.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <memory>

#include <comutil.h>
#include <initguid.h>
#include <propkey.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <shtypes.h>

struct {
    std::atomic<bool> mixFoldersAndFiles;
} g_settings;

std::atomic<int> g_hookRefCount;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                            void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
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

// Fetches PKEY_FileAttributes for the item and reports whether
// FILE_ATTRIBUTE_DIRECTORY is set. Returns false (via the HRESULT) if the
// attribute couldn't be read, so the caller can fall back to the default
// behavior instead of guessing.
HRESULT IsFolderItem(void* pCFSFolder,
                      const ITEMIDLIST_RELATIVE* itemid,
                      bool* isFolder) {
    _variant_t attrValue;
    HRESULT hr = CFSFolder_GetDetailsEx_Original(
        pCFSFolder, itemid, &PKEY_FileAttributes, attrValue.GetAddress());
    if (FAILED(hr)) {
        return hr;
    }
    if (attrValue.vt != VT_UI4) {
        return E_UNEXPECTED;
    }
    *isFolder = (attrValue.ulVal & FILE_ATTRIBUTE_DIRECTORY) != 0;
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

    if (!itemid1 || !itemid2 || !g_settings.mixFoldersAndFiles) {
        return original();
    }

    // Folders-first is the only rule this mod needs to override, so only
    // step in when exactly one of the two items is a folder. Folder-vs-folder
    // and file-vs-file pairs are left to the shell's own comparison: it's
    // cheaper (no property-system round trips for the common case), and it
    // guarantees the exact same ordering Explorer would otherwise use
    // (numerical sorting, locale rules, and comparing the real file name
    // rather than the display name with the extension hidden).
    bool isFolder1, isFolder2;
    if (FAILED(IsFolderItem(pCFSFolder, itemid1, &isFolder1)) ||
        FAILED(IsFolderItem(pCFSFolder, itemid2, &isFolder2))) {
        return original();
    }
    if (isFolder1 == isFolder2) {
        return original();
    }

    // The low word of `column` is the actual column index
    // (SHCIDS_COLUMNMASK); the high bits carry flags such as
    // SHCIDS_ALLFIELDS (identity queries) or SHCIDS_CANONICALONLY. Only
    // handle plain column sorts and let the shell handle anything else.
    if (column & ~(LPARAM)SHCIDS_COLUMNMASK) {
        return original();
    }
    int columnIndex = (int)(column & SHCIDS_COLUMNMASK);

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, columnIndex,
                                                    &columnSCID))) {
        return original();
    }

    _variant_t value1;
    _variant_t value2;
    if (FAILED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid1, &columnSCID, value1.GetAddress())) ||
        FAILED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid2, &columnSCID, value2.GetAddress()))) {
        return original();
    }

    auto isEmpty = [](const _variant_t& v) {
        return v.vt == VT_EMPTY || v.vt == VT_NULL;
    };

    if (value1.vt != value2.vt) {
        // A folder commonly leaves a column blank (VT_EMPTY) where a file
        // has a real value (Size, Dimensions, Length, ...). Treat a blank
        // value as sorting before any present value, rather than falling
        // back to folders-first. This still leaves Size itself effectively
        // unmixed, since a folder's size is always blank -- see the README.
        bool empty1 = isEmpty(value1);
        bool empty2 = isEmpty(value2);
        if (empty1 != empty2) {
            return CompareResultFromInt(empty1 ? -1 : 1);
        }
        return original();
    }

    int cmp;
    switch (value1.vt) {
        case VT_EMPTY:
        case VT_NULL:
            cmp = 0;
            break;

        case VT_BSTR: {
            LPCWSTR s1 = (LPCWSTR)value1.bstrVal;
            LPCWSTR s2 = (LPCWSTR)value2.bstrVal;
            cmp = StrCmpLogicalW(s1 ? s1 : L"", s2 ? s2 : L"");
            break;
        }

        case VT_UI8:
            cmp = (value1.ullVal > value2.ullVal) -
                  (value1.ullVal < value2.ullVal);
            break;

        case VT_I8:
            cmp = (value1.llVal > value2.llVal) -
                  (value1.llVal < value2.llVal);
            break;

        case VT_UI4:
            cmp = (value1.ulVal > value2.ulVal) -
                  (value1.ulVal < value2.ulVal);
            break;

        case VT_I4:
            cmp = (value1.lVal > value2.lVal) - (value1.lVal < value2.lVal);
            break;

        case VT_UI2:
            cmp = (int)value1.uiVal - (int)value2.uiVal;
            break;

        case VT_I2:
            cmp = (int)value1.iVal - (int)value2.iVal;
            break;

        case VT_UI1:
            cmp = (int)value1.bVal - (int)value2.bVal;
            break;

        case VT_R8:
        case VT_DATE:
            cmp = (value1.dblVal > value2.dblVal) -
                  (value1.dblVal < value2.dblVal);
            break;

        case VT_R4:
            cmp = (value1.fltVal > value2.fltVal) -
                  (value1.fltVal < value2.fltVal);
            break;

        case VT_BOOL:
            // VARIANT_TRUE is -1, so comparing boolVal directly would sort
            // true before false. Normalize to 0/1 first.
            cmp = (int)(!!value1.boolVal) - (int)(!!value2.boolVal);
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

void LoadSettings() {
    g_settings.mixFoldersAndFiles = Wh_GetIntSetting(L"mixFoldersAndFiles");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!g_settings.mixFoldersAndFiles) {
        // Nothing to do: don't install the hooks at all rather than
        // installing them inert. Toggling the setting back on reloads the
        // mod (see Wh_ModSettingsChanged), which gives Wh_ModInit another
        // chance to hook.
        return TRUE;
    }

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

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
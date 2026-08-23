// ==WindhawkMod==
// @id              mix-files-and-folders-sorting
// @name            Mix Files and Folders When Sorting in Windows Explorer
// @description     Interleave files and folders when sorting Explorer by any column, instead of always grouping all folders before (or after) all files
// @version         1.0
// @author          Extremenis
// @github          https://github.com/Extremenis
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -loleaut32 -lshlwapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mix files and folders when sorting

Windows Explorer always lists all folders before (or after, when sorting
descending) all files, regardless of which column is sorted by. This is
hardcoded shell behavior, not a UI setting, and it's independent from
"Group by".

This mod removes that separation: with it enabled, files and folders are
interleaved purely according to the selected column's value (name, size,
type, date, ...), the same way they would be if folders were just regular
rows.

## Scope

This only affects regular file-system folders (the `CFSFolder` shell object,
i.e. anything you'd browse to on disk). Special views such as the Desktop,
Libraries, "This PC", and search results use different, independent shell
folder implementations that this mod doesn't touch, so folders may still be
grouped first there.

The mod is also scoped to `explorer.exe` only, so common file/save dialogs in
other applications — which use the same `CFSFolder` implementation, just in
a different process — keep the default folders-first ordering.

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
    When enabled, folders and files are interleaved when sorting by any
    column. When disabled, Explorer's default folders-first behavior is
    restored without needing to reload the mod.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <memory>

#include <comutil.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <shtypes.h>

struct {
    bool mixFoldersAndFiles;
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
        // Columns like Size are left blank (VT_EMPTY) for folders in stock
        // Explorer, which would otherwise make every folder-vs-file pair
        // fall through to the default folders-first behavior even with
        // this mod enabled. Treat a blank value as sorting before any
        // present value instead.
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
            cmp = (int)value1.boolVal - (int)value2.boolVal;
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
        // equal values in this column (e.g. two .txt files compared by
        // Type) must not be reported as identical, so delegate the tiebreak
        // to the original implementation instead.
        return original();
    }

    return CompareResultFromInt(cmp);
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = GetModuleHandleW(L"windows.storage.dll");
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to get windows.storage.dll module handle");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
            },
            &CFSFolder_GetDetailsEx_Original,
        },
        {
            {
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
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

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

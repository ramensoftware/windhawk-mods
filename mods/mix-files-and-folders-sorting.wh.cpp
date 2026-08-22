// ==WindhawkMod==
// @id              mix-files-adn-folders-sorting
// @name            Mix Files and Folders When Sorting in Windows Explorer
// @description     Interleave files and folders when sorting Explorer by any column, instead of always grouping all folders before (or after) all files
// @version         1.0
// @author          Extremenis
// @github          https://github.com/Extremenis
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lpropsys -lshlwapi
// @license         MIT
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
#include <initguid.h>
#include <propkey.h>
#include <propsys.h>
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
                      int column,
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
                                          int column,
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

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, column,
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

    if (value1.vt != value2.vt) {
        return original();
    }

    int cmp;
    switch (value1.vt) {
        case VT_EMPTY:
        case VT_NULL:
            cmp = 0;
            break;

        // BSTR and LPWSTR share the same pointer-sized union slot, so the
        // raw pointer can be read the same way regardless of which of the
        // two tags GetDetailsEx used for a text column.
        case VT_BSTR:
        case VT_LPWSTR: {
            LPCWSTR s1 = (LPCWSTR)value1.bstrVal;
            LPCWSTR s2 = (LPCWSTR)value2.bstrVal;
            cmp = StrCmpLogicalW(s1 ? s1 : L"", s2 ? s2 : L"");
            break;
        }

        // FILETIME is a monotonically increasing 64-bit tick count, so it
        // sorts correctly as a plain unsigned compare, same union slot as
        // VT_UI8.
        case VT_UI8:
        case VT_FILETIME:
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

    return CompareResultFromInt(cmp);
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = LoadLibraryEx(
        L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
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

    return HookSymbols(windowsStorageModule, windowsStorageHooks,
                        ARRAYSIZE(windowsStorageHooks));
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

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}

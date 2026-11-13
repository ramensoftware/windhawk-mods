// ==WindhawkMod==
// @id              explorer-copyaspath-remove-quotes
// @name            Explorer Copy-as-path Remove quotes
// @description     Removes quotes added by Ctrl+Shift+C by changing GetShellItemsAsTextHGLOBAL flags from 0 to 2.
// @version         1.0.0
// @author          ilyfairy
// @github          https://github.com/ilyfairy
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Copy-as-path: Remove quotes

Windows Explorer's "Copy as path" (Ctrl+Shift+C) produces quoted paths (e.g. "C:\\A B\\file.txt").

This mod runs inside explorer.exe and hooks Windows.Storage.dll!GetShellItemsAsTextHGLOBAL.
When Explorer calls this function with flags=0 (the default, which produces quoted paths),
the mod rewrites flags to 2, which has been verified to return unquoted paths.

Other flags and callers are left unchanged.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_api.h>

// Treat IDataObject as an opaque COM pointer to avoid heavy headers.
struct IDataObject;

using GetShellItemsAsTextHGLOBAL_t = HRESULT(WINAPI*)(IDataObject* dataObject, UINT flags, HGLOBAL* phglobal);
static GetShellItemsAsTextHGLOBAL_t GetShellItemsAsTextHGLOBAL_Original = nullptr;

static HRESULT WINAPI GetShellItemsAsTextHGLOBAL_Hook(IDataObject* dataObject, UINT flags, HGLOBAL* phglobal) {
    // In shell32, CCopyAsPathMenu uses flags==0 for Ctrl+Shift+C (quoted paths).
    // flags==2 was verified to return unquoted paths.
    if (flags == 0) {
        static volatile LONG loggedOnce = 0;
        if (InterlockedCompareExchange(&loggedOnce, 1, 0) == 0) {
            Wh_Log(L"GetShellItemsAsTextHGLOBAL: rewriting flags 0 -> 2");
        }
        flags = 2;
    }

    return GetShellItemsAsTextHGLOBAL_Original(dataObject, flags, phglobal);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    HMODULE windowsStorage = GetModuleHandleW(L"Windows.Storage.dll");
    if (!windowsStorage) {
        windowsStorage = LoadLibraryExW(L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!windowsStorage) {
            Wh_Log(L"Failed to load Windows.Storage.dll");
            return FALSE;
        }
    }

    void* p = (void*)GetProcAddress(windowsStorage, "GetShellItemsAsTextHGLOBAL");
    if (!p) {
        Wh_Log(L"GetProcAddress(GetShellItemsAsTextHGLOBAL) failed");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(p,
                            (void*)GetShellItemsAsTextHGLOBAL_Hook,
                            (void**)&GetShellItemsAsTextHGLOBAL_Original)) {
        Wh_Log(L"Wh_SetFunctionHook failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

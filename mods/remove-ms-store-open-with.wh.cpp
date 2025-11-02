// ==WindhawkMod==
// @id              remove-ms-store-open-with
// @name            Open With - Remove Microsoft Store Menu Item
// @description     Removes the "Search with Microsoft Store" menu item from the "Open with" submenu
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open With - Remove Microsoft Store Menu Item

This mod removes the "Search with Microsoft Store" item from the "Open with"
submenu in File Explorer. The item is unremovable by normal means (MUI/registry
editing).

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/remove-ms-store-open-with-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/remove-ms-store-open-with-after.png)
*/
// ==/WindhawkModReadme==

#include <processthreadsapi.h>
#include <psapi.h>

HMODULE   g_hShell32 = nullptr;
ULONGLONG g_shell32Size = 0;
// Same buffer size as in shell32
WCHAR     g_szMsStore[80] = { 0 };

using InsertMenuItemW_t = decltype(&InsertMenuItemW);
InsertMenuItemW_t InsertMenuItemW_orig = nullptr;
BOOL WINAPI InsertMenuItemW_hook(
    HMENU            hmenu,
    UINT             item,
    BOOL             fByPosition,
    LPCMENUITEMINFOW lpmi
)
{
    // Ensure that we're only modifying menu items added from shell32
    void *retaddr = __builtin_return_address(0);
    if (((ULONGLONG)retaddr >= (ULONGLONG)g_hShell32) && ((ULONGLONG)retaddr < ((ULONGLONG)g_hShell32 + g_shell32Size)))
    {
        if (lpmi->fMask & MIIM_STRING && lpmi->dwTypeData && 0 == wcscmp(lpmi->dwTypeData, g_szMsStore))
        {
            return TRUE;
        }
    }
    return InsertMenuItemW_orig(
        hmenu,
        item,
        fByPosition,
        lpmi
    );
}

BOOL Wh_ModInit(void)
{
    g_hShell32 = LoadLibraryW(L"shell32.dll");
    if (!g_hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    MODULEINFO miShell32 = { 0 };
    if (!GetModuleInformation(
        GetCurrentProcess(),
        g_hShell32,
        &miShell32,
        sizeof(MODULEINFO)
    ))
    {
        Wh_Log(L"Failed to get size of shell32.dll");
        return FALSE;
    }
    g_shell32Size = miShell32.SizeOfImage;

    LoadStringW(g_hShell32, 0x1506, g_szMsStore, 80);
    if (!*g_szMsStore)
    {
        Wh_Log(L"Failed to load Microsoft Store string");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)InsertMenuItemW,
        (void *)InsertMenuItemW_hook,
        (void **)&InsertMenuItemW_orig
    ))
    {
        Wh_Log(L"Failed to hook InsertMenuItemW");
        return FALSE;
    }

    return TRUE;
}

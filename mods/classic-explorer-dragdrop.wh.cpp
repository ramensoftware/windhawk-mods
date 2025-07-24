// ==WindhawkMod==
// @id              classic-explorer-dragdrop
// @name            Classic Explorer Drag/Drop
// @description     Enables/fixes the classic drag/drop image in File Explorer.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @include         *
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Explorer Drag/Drop

Forces the classic drag/drop image resembling the list view item itself to be used in File Explorer,
and fixes a bug introduced in Windows 7 whereby the drag/drop image will not be displayed in classic
theme.

## Caveats

This does not work with the DirectUI list view used since Windows 7. In such cases, the Windows Vista
style will always be used.
*/
// ==/WindhawkModReadme==

#include <processenv.h>
#include <processthreadsapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <uxtheme.h>
#include <winerror.h>
#include <winnt.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <psapi.h>
#include <cstdint>

#if __WIN64
#define WINAPI_STR L"__cdecl"
#else
#define WINAPI_STR L"__stdcall"
#endif

#if __WIN64
#define THISCALL_STR L"__cdecl"
#else
#define THISCALL_STR L"__thiscall"
#endif

bool IsCallerModule(HMODULE hMod, void *pfn)
{
    MODULEINFO mi = { 0 };
    
    if (!GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi)))
    {
        return false;
    }

    return (uintptr_t)pfn >= (uintptr_t)hMod &&
           (uintptr_t)pfn < ( (uintptr_t)hMod + mi.SizeOfImage );
}

HANDLE g_hLocalTheme;

using OpenThemeData_t = decltype(&OpenThemeData);
OpenThemeData_t OpenThemeData_orig;
HTHEME WINAPI OpenThemeData_hook(HWND hwnd, LPCWSTR pszClassList)
{
    if (!IsAppThemed() && wcscmp(pszClassList, L"DragDrop") == 0 &&
        IsCallerModule(GetModuleHandleW(L"shell32.dll"), __builtin_return_address(0)))
    {
        // Return a fake theme handle to force the correct control flow
        // due to a bug introduced in Windows 7. The classic theme list-view drag/drop image
        // still exists perfectly intact as in Vista, but a bug makes it not render.
        return (HTHEME)1;
    }

    return OpenThemeData_orig(hwnd, pszClassList);
}

// This may be a null pointer. Prefer calling the standalone function GetDragImageMsg.
// This is because this function may be prone to aggressive inlining due to its
// simplicity.
static UINT (*pfnGetDragImageMsg)() = nullptr;

/**
 * Gets the drag image message registered for internal shell use.
 */
UINT GetDragImageMsg()
{
    if (pfnGetDragImageMsg)
    {
        // Call the implementation in shell32 if symbols are available.
        return pfnGetDragImageMsg();
    }

    // Otherwise, just reimplement that simple method on our own.
    // RegisterWindowMessage documentation does state that the message value will be
    // retrieved, but I don't trust it fully.
    static UINT s_uMsg = 0;

    if (s_uMsg)
        return s_uMsg;
    
    s_uMsg = RegisterWindowMessageW(L"ShellGetDragImage");
    return s_uMsg;
}

DWORD g_dwUnthemedThreadId = 0;

using IsAppThemed_t = decltype(&IsAppThemed);
IsAppThemed_t IsAppThemed_orig;
WINBOOL WINAPI IsAppThemed_hook()
{
    if (GetCurrentThreadId() == g_dwUnthemedThreadId)
    {
        Wh_Log(L"Lying about theme status on thread %d", GetCurrentThreadId());
        return FALSE;
    }
    else if (g_dwUnthemedThreadId)
    {
        Wh_Log(L"g_dwUnthemedThreadId is set to %d, which differs from us %d", g_dwUnthemedThreadId, GetCurrentThreadId());
    }

    return IsAppThemed_orig();
}

LRESULT CALLBACK (*CListViewHost__s_ListViewSubclassWndProc_orig)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK CListViewHost__s_ListViewSubclassWndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == GetDragImageMsg())
    {
        Wh_Log(L"Setting unthemed thread ID to %d", GetCurrentThreadId());
        g_dwUnthemedThreadId = GetCurrentThreadId();
    }

    LRESULT lRes = CListViewHost__s_ListViewSubclassWndProc_orig(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);

    if (uMsg == GetDragImageMsg())
    {
        Wh_Log(L"Clearing unthemed thread ID.");
        g_dwUnthemedThreadId = 0;
    }

    return lRes;
}

// shell32.dll
const WindhawkUtils::SYMBOL_HOOK c_rgShell32Hooks[] = {
    {
        { 
            L"GetDragImageMsg"
        },
        (void **)&pfnGetDragImageMsg,
        nullptr,
        true,
    },
    {
        {
#if _WIN64
            L"private: static __int64 __cdecl CListViewHost::s_ListViewSubclassWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,unsigned __int64,unsigned __int64)"
#else
            L"private: static long __stdcall CListViewHost::s_ListViewSubclassWndProc(struct HWND__ *,unsigned int,unsigned int,long,unsigned int,unsigned long)"
#endif
        },
        (void **)&CListViewHost__s_ListViewSubclassWndProc_orig,
        (void *)CListViewHost__s_ListViewSubclassWndProc_hook
    }
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    HMODULE hmUxtheme = GetModuleHandleW(L"uxtheme.dll");
    HMODULE hmShell32 = GetModuleHandleW(L"shell32.dll");

    if (hmUxtheme && hmShell32)
    {
        Wh_SetFunctionHook(
            (void *)OpenThemeData,
            (void *)OpenThemeData_hook,
            (void **)&OpenThemeData_orig
        );

        Wh_SetFunctionHook(
            (void *)IsAppThemed,
            (void *)IsAppThemed_hook,
            (void **)&IsAppThemed_orig
        );
        
        if (!WindhawkUtils::HookSymbols(hmShell32, c_rgShell32Hooks, ARRAYSIZE(c_rgShell32Hooks)))
        {
            Wh_Log(L"Failed to install shell32.dll hooks.");
            return FALSE;
        }
    }
    else
    {
        if (!hmUxtheme)
            Wh_Log(L"Failed to find handle to uxtheme.dll.");
        if (!hmShell32)
            Wh_Log(L"Failed to find handle to shell32.dll.");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{

}

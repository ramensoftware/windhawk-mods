// ==WindhawkMod==
// @id              virtual-desktop-hotkey-cycle
// @name            Virtual Desktop Hotkey Cycle
// @description     Makes Win+Ctrl+Left and Win+Ctrl+Right virtual desktop shortcuts loop through ends.
// @version         1.0.0
// @author          Amrsatrio
// @github          https://github.com/Amrsatrio
// @twitter         https://twitter.com/amrsatrio
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cycling Virtual Desktop Hotkey
This simple mod makes Win+Ctrl+Left and Win+Ctrl+Right cycle to the last/first desktops when the current virtual desktop is the first/last, respectively.

## Compatibility
* Version: Windows 10 1507 and up
* Architecture: x64, ARM64

Tested on Windows 11 24H2 x64 and ARM64.

## How it works

Windows normally calls `CVirtualDesktopCollection::GetAdjacentVirtualDesktop` with a reference to the **current** virtual desktop. This method is implemented in `twinui.pcshell.dll` on Windows builds 17763 (1809) and newer, and in `twinui.dll` on builds prior to 17763. When there is no desktop in the requested direction (for example, pressing **Win+Ctrl+Right** on the last desktop), the function returns `TYPE_E_OUTOFBOUNDS`, which prevents further navigation.

This mod hooks `GetAdjacentVirtualDesktop` and watches for that out-of-bounds result:

1. The original function is called first with the current desktop (`pReference`).
2. If it fails with `TYPE_E_OUTOFBOUNDS`, the mod checks how many virtual desktops exist.
3. If more than one desktop is present, the function is called again with `pReference` set to `nullptr`.

When `pReference` is `nullptr`, Windows interprets the request as:

* **Right / Down**: return the **first** virtual desktop
* **Left / Up**: return the **last** virtual desktop

By reissuing the call this way, the mod effectively turns the standard virtual desktop hotkeys into a loop, allowing seamless cycling from end to beginning and vice versa.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

interface IVirtualDesktop;
interface IVirtualDesktopCollection;

using CVirtualDesktopCollection_GetCount_t = size_t (STDMETHODCALLTYPE *)(IVirtualDesktopCollection* This);

using CVirtualDesktopCollection_GetAdjacentVirtualDesktop_t = HRESULT (STDMETHODCALLTYPE *)(
    IVirtualDesktopCollection* This, IVirtualDesktop* pReference, UINT uDirection, REFIID riid, void** ppv);
CVirtualDesktopCollection_GetAdjacentVirtualDesktop_t CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Original;
HRESULT STDMETHODCALLTYPE CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Hook(
    IVirtualDesktopCollection* This, IVirtualDesktop* pReference, UINT uDirection, REFIID riid, void** ppv)
{
    HRESULT hr = CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Original(This, pReference, uDirection, riid, ppv);
    if (hr == TYPE_E_OUTOFBOUNDS && pReference)
    {
        size_t cDesktops = ((CVirtualDesktopCollection_GetCount_t)(*(void***)This)[3])(This); // GetCount()
        if (cDesktops > 1)
        {
            hr = CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Original(This, nullptr, uDirection, riid, ppv);
        }
    }

    return hr;
}

#ifdef _WIN64
#   define  SSTDCALL  L"__cdecl"
#else
#   define  SSTDCALL  L"__stdcall"
#endif

BOOL Wh_ModInit()
{
    // twinui.pcshell.dll on 17763+
    // twinui.dll on <17763
    static const WindhawkUtils::SYMBOL_HOOK c_rgTwinUIPCShellHooks[] =
    {
        {
            {
                L"public: virtual long " SSTDCALL " CVirtualDesktopCollection::GetAdjacentVirtualDesktop(struct IVirtualDesktop *,unsigned int,struct _GUID const &,void * *)"
            },
            &CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Original,
            CVirtualDesktopCollection_GetAdjacentVirtualDesktop_Hook
        }
    };

    HMODULE hTwinUIPCShell = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!WindhawkUtils::HookSymbols(hTwinUIPCShell, c_rgTwinUIPCShellHooks, ARRAYSIZE(c_rgTwinUIPCShellHooks)))
    {
        Wh_Log(L"Failed to hook one or more functions in twinui.pcshell.dll, trying twinui.dll");

        HMODULE hTwinUI = LoadLibraryExW(L"twinui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!WindhawkUtils::HookSymbols(hTwinUI, c_rgTwinUIPCShellHooks, ARRAYSIZE(c_rgTwinUIPCShellHooks)))
        {
            Wh_Log(L"Failed to hook one or more functions in twinui.dll");
            return FALSE;
        }
    }

    return TRUE;
}

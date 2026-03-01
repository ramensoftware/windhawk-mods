// ==WindhawkMod==
// @id              explorerpatcher-dxgi-crash-fix
// @name            ExplorerPatcher DXGI Crash Fix
// @description     Resolves file picker crashes in Notepad & RegEdit when ExplorerPatcher is installed
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         notepad.exe
// @include         regedit.exe
// @compilerOptions -lntdll
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ExplorerPatcher DXGI Crash Fix

When ExplorerPatcher is installed, its `dxgi.dll` is placed in `C:\Windows` to
hook into Windows Explorer.

## The problem
Due to the standard DLL search order, other applications that live in
`C:\Windows` also load ExplorerPatcher's `dxgi.dll` instead of the original
one from `C:\Windows\System32`.

This unintended behavior causes Notepad (classic version) and Registry Editor to
crash when performing any of the following actions within a file picker dialog:
* Viewing any folders in one of the following view modes:
  * Tiles
  * Content
  * Medium-sized icons
  * Large icons
  * Extra large icons
* Dragging a folder
* Selecting any folder while the Preview Pane is shown
* Anything else that tries to render folder previews

## The solution
This mod forces both applications to load the original `dxgi.dll` from
`C:\Windows\System32` instead of the ExplorerPatcher version.

It is done by:
* Pre-loading the original `dxgi.dll` when the application launches
* Hooking the `LdrLoadDll` function to redirect any `dxgi.dll` load attempts to
  `C:\Windows\System32`
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlwapi.h>
#include <winternl.h>

// Helper: Check if the requested module name contains "dxgi.dll"
bool IsDxgiModule(PCWSTR pszModuleName)
{
    if (!pszModuleName) return false;
    return (StrStrIW(pszModuleName, L"dxgi.dll") != NULL);
}

// Helper: Pre-load the original dxgi.dll when the application launches
void PreloadSystemDxgi()
{
    // Check if dxgi.dll is already loaded in the process
    HMODULE hCurrentDxgi = GetModuleHandleW(L"dxgi.dll");
    if (hCurrentDxgi)
    {
        WCHAR szModulePath[MAX_PATH];
        if (GetModuleFileNameW(hCurrentDxgi, szModulePath, MAX_PATH))
        {
            Wh_Log(L"DXGI module is already loaded: %s", szModulePath);
        }
    }
    else
    {
        // If it's not loaded yet, pre-load the original dxgi.dll
        HMODULE hSystemDxgi = LoadLibraryExW(L"dxgi.dll", nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hSystemDxgi)
        {
            Wh_Log(L"Successfully pre-loaded the original dxgi.dll");
        }
        else
        {
            Wh_Log(L"Failed to pre-load the original dxgi.dll");
        }
    }
}

// Hook LdrLoadDll to redirect any dxgi.dll load attempts to C:\Windows\System32
using LdrLoadDll_t = NTSTATUS (NTAPI *)(
    PWCHAR pwSearchPath,
    PULONG pulLoadFlags,
    PUNICODE_STRING pModuleName,
    PHANDLE phModule
);
LdrLoadDll_t LdrLoadDll_Original;
NTSTATUS NTAPI LdrLoadDll_Hook(
    PWCHAR pwSearchPath,
    PULONG pulLoadFlags,
    PUNICODE_STRING pModuleName,
    PHANDLE phModule
)
{
    if (pModuleName && pModuleName->Buffer)
    {
        if (IsDxgiModule(pModuleName->Buffer))
        {
            UNICODE_STRING usSystemPath;
            RtlInitUnicodeString(&usSystemPath, L"C:\\Windows\\System32\\dxgi.dll");
            return LdrLoadDll_Original(NULL, pulLoadFlags, &usSystemPath, phModule);
        }
    }
    return LdrLoadDll_Original(pwSearchPath, pulLoadFlags, pModuleName, phModule);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    PreloadSystemDxgi();

    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (hNtDll)
    {
        void* pvLdrLoadDll = (void*)GetProcAddress(hNtDll, "LdrLoadDll");
        if (pvLdrLoadDll)
        {
            Wh_SetFunctionHook(
                pvLdrLoadDll,
                (void*)LdrLoadDll_Hook,
                (void**)&LdrLoadDll_Original
            );
        }
    }

    return TRUE;
}

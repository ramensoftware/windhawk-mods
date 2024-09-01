// ==WindhawkMod==
// @id              fake-explorer-path
// @name            Fake Explorer path
// @description     Allows to run explorer.exe in taskbar mode from any location
// @version         1.0.1
// @author 	    Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Allows to run explorer.exe in taskbar mode from any location. This can be used to run another version of Explorer
without replacing system files. Explorer can be launched manually or, alternatively, installed as default shell
by specifying the path to explorer.exe in the registry key
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Shell.
For running taskbar in Windows 10 mode, the localization folder should be available
at the location of explorer.exe. For Windows 11 taskbar mode, this is not required.

The mod makes explorer.exe think that it has been launched from the Windows folder.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>

typedef DWORD (WINAPI* GetModuleFileNameW_t)(HMODULE, LPWSTR, DWORD);
GetModuleFileNameW_t pOriginalGetModuleFileNameW = NULL;

DWORD WINAPI HookedGetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize) {
    // If the call is for the main executable, return the fake path
    if (hModule == NULL) {
        WCHAR systemRoot[MAX_PATH];
        GetEnvironmentVariableW(L"SystemRoot", systemRoot, MAX_PATH);
        wsprintfW(lpFilename, L"%s\\explorer.exe", systemRoot);
        return lstrlenW(lpFilename);
    }
    // Otherwise, call the original function
    return pOriginalGetModuleFileNameW(hModule, lpFilename, nSize);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)GetProcAddress(LoadLibrary(L"kernelbase.dll"), "GetModuleFileNameW"), (void*)HookedGetModuleFileNameW, (void**)&pOriginalGetModuleFileNameW);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

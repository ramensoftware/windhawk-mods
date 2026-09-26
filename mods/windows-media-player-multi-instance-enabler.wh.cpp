// ==WindhawkMod==
// @id              windows-media-player-multi-instance-enabler
// @name            Windows Media Player Multi-Instance Enabler
// @description     Enables running multiple concurrent instances of Windows Media Player while protecting the main library from corruption.
// @version         1.0.2.0
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @include         wmplayer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Media Player Multi-Instance Enabler
Unlocks the restriction preventing multiple instances of Windows Media Player from running simultaneously. 

### Features
- **Universal Architecture Support:** Automatically detects and targets both 32-bit and 64-bit builds of `wmplayer.exe`.
- **Version Agnostic:** Bypasses the core mutex checks used from legacy player builds all the way up to current versions.
- **Library Protection Sandbox:** Optional feature to force secondary player windows into a read-only database state, preventing race conditions or library data file corruption.

- **This is somewhat unsafe but really cool also use at own risk** 
- **Making Library modifications while multiple instances are open is not a good idea**

## Tested on Windows Media Player 11 (Vista Version) on Windows 10 IOT Enterprise LTSC 2021

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sandboxDatabase: true
  $name: Database Protection Sandbox
  $description: Forces secondary instances into a read-only state for database files (.wmdb) to prevent multi-instance write corruption.
*/
// ==/WindhawkModSettings==

#include <windows.h>

struct {
    bool sandboxDatabase;
} settings;

// State tracking variable to determine instance priority ranking
bool g_isSecondaryInstance = false;

// Function definitions and original pointer storage for API hooking
using CreateMutexW_t = HANDLE(WINAPI *)(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR);
CreateMutexW_t CreateMutexW_Original;

using CreateFileW_t = HANDLE(WINAPI *)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
CreateFileW_t CreateFileW_Original;

// Hook function for intercepting the single-instance synchronization object
HANDLE WINAPI CreateMutexW_Hook(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName) {
    if (lpName && wcsstr(lpName, L"CheckForOtherInstanceMutex")) {
        Wh_Log(L"Intercepted Windows Media Player single-instance mutex initialization.");

        // Check if a master instance is already holding the base system lock token
        HANDLE hMasterCheck = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, lpName);
        if (hMasterCheck) {
            // A master instance exists; flag this running execution block as secondary
            g_isSecondaryInstance = true;
            CloseHandle(hMasterCheck);
            Wh_Log(L"Active execution block identified as a secondary instance player.");

            // Mutate the string pointer uniqueness dynamically to allow successful initialization
            WCHAR szModifiedMutex[256];
            wsprintfW(szModifiedMutex, L"%s_%u", lpName, GetCurrentProcessId());
            return CreateMutexW_Original(lpMutexAttributes, bInitialOwner, szModifiedMutex);
        }
    }

    return CreateMutexW_Original(lpMutexAttributes, bInitialOwner, lpName);
}

// Hook function for managing asynchronous stream routing and file safety parameters
HANDLE WINAPI CreateFileW_Hook(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (settings.sandboxDatabase && g_isSecondaryInstance && lpFileName) {
        if (wcsstr(lpFileName, L".wmdb")) {
            Wh_Log(L"Intercepted secondary instance database access call. Stripping master write parameters.");
            
            // Remove write permissions aggressively to safeguard master tracking indexes
            dwDesiredAccess &= ~GENERIC_WRITE;
            dwDesiredAccess &= ~FILE_WRITE_DATA;
        }
    }

    return CreateFileW_Original(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

// Load and map configured engine values from the initialization stack
void LoadSettings() {
    settings.sandboxDatabase = Wh_GetIntSetting(L"sandboxDatabase");
}

// Core Entry Point required by the Windhawk virtualization subsystem
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Windows Media Player Multi-Instance Mod Execution Layout.");

    LoadSettings();

    // Set standard function redirection inside current memory space safely
    Wh_SetFunctionHook((void*)CreateMutexW,
                       (void*)CreateMutexW_Hook,
                       (void**)&CreateMutexW_Original);

    Wh_SetFunctionHook((void*)CreateFileW,
                       (void*)CreateFileW_Hook,
                       (void**)&CreateFileW_Original);

    return TRUE;
}

// Standard termination tracking sequence
void Wh_ModUninit() {
    Wh_Log(L"Unloading Multi-Instance Engine Mod.");
}

// Live update listener for processing configuration structural changes
void Wh_ModSettingsChanged() {
    Wh_Log(L"Mod settings state transformation recognized. Reloading mapping matrix.");
    LoadSettings();
}
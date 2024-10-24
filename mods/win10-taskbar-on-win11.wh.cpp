// ==WindhawkMod==
// @id           win10-taskbar-on-win11
// @name         Windows 10 taskbar on Windows 11
// @description  Enables Windows 10 taskbar on Windows 11 versions 21H2 to 23H2
// @version      1.0.2
// @author       Anixx
// @github       https://github.com/Anixx
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod enables Windows 10 taskbar on Windows 11 versions 21H2 to 23H2, without
using Explorer Patcher. Make sure you are using the appropriate version of Windows 11.

If you want to get rid of the cogwheel icon in the tray area, use the mod "Hide Action Center icon".
You can set up the clock and the tray icons appearance using legacy tray setup dialog,
which you can start with the command `explorer shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}`.
*/
// ==/WindhawkModReadme==

#include <windows.h>

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

REGQUERYVALUEEXW pOriginalRegQueryValueExW;

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{   

    if (lstrcmpiW(lpValueName, L"UndockingDisabled") == 0)
        
    { 
            if (lpType)
                *lpType = REG_DWORD;
            if (lpData && lpcbData && *lpcbData >= sizeof(DWORD))
            {
                *(DWORD*)lpData = 1;
                *lpcbData = sizeof(DWORD);
            }
            return ERROR_SUCCESS;
    }

    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)GetProcAddress(LoadLibrary(L"kernelbase.dll"), "RegQueryValueExW"), (void*)RegQueryValueExWHook, (void**)&pOriginalRegQueryValueExW);

    return TRUE;
}

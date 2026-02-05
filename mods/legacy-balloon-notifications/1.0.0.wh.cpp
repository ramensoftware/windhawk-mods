// ==WindhawkMod==
// @id           legacy-balloon-notifications
// @name         Legacy X-style balloon notifications
// @description  Enables legacy XP-style ballon notifications for Win10 taskbar
// @version      1.0.0
// @author       Anixx
// @github       https://github.com/Anixx
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Enables legacy XP-style ballon notifications for Win10 taskbar, 
running either under Windows 10 or Windows 11. This affects only old applications.
This mod need a restart of File Explorer.
*/
// ==/WindhawkModReadme==

#include <windows.h>

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

REGQUERYVALUEEXW pOriginalRegQueryValueExW;

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{   

    if ((lstrcmpiW(lpValueName, L"EnableLegacyBalloonNotifications") == 0) || (lstrcmpiW(lpValueName, L"EnableBalloonTips") == 0))
        
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

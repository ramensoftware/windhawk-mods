// ==WindhawkMod==
// @id              syslistview32-enabler
// @name            Enable SyslistView32
// @description     Enables SysListView32 folder layout in Explorer
// @version         2.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Enables the SysListView32 control in File Explorer folder windows.
This makes the view more compact and allows icon rearrangement.
SysListView32 control has been used by default before Windows 7.

Before:

![Default view](https://i.imgur.com/rPpiFEU.png)

After:

![SysListView32 mode](https://i.imgur.com/oqYf1YW.png)

*/
// ==/WindhawkModReadme==

typedef LONG (WINAPI *REGOPENKEYEXW)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);

REGOPENKEYEXW pOriginalRegOpenKeyExW;

const WCHAR* g_targetCLSID = L"{1eeb5b5a-06fb-4732-96b3-975c0194eb39}";

LONG WINAPI RegOpenKeyExWHook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    if (lpSubKey)
    {
        WCHAR lowerSubKey[512];
        WCHAR lowerCLSID[64];
        
        wcsncpy(lowerSubKey, lpSubKey, 511);
        lowerSubKey[511] = L'\0';
        wcscpy(lowerCLSID, g_targetCLSID);
        
        _wcslwr(lowerSubKey);
        _wcslwr(lowerCLSID);
        
        if (wcsstr(lowerSubKey, lowerCLSID) != NULL)
        {
            Wh_Log(L"Blocking RegOpenKeyExW: %s", lpSubKey);
            return ERROR_FILE_NOT_FOUND;
        }
    }

    return pOriginalRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init - blocking CLSID %s", g_targetCLSID);

    Wh_SetFunctionHook(
        (void*)GetProcAddress(LoadLibrary(L"kernelbase.dll"), "RegOpenKeyExW"),
        (void*)RegOpenKeyExWHook,
        (void**)&pOriginalRegOpenKeyExW
    );

    return TRUE;
}

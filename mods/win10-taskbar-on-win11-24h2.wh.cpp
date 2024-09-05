// ==WindhawkMod==
// @id              win10-taskbar-on-win11-24h2
// @name            Enables Win10 taskbar on Win11 24H2
// @description     Enables Windows 10 taskbar on Windows 11 version 24H2, Windows Server 2025 and Windows 11 IoT Enterprise LTSC 2024
// @version         0.1
// @architecture    x86-64
// @author          Anixx
// @github          https://github.com/Anixx
// @include         userinit.exe
// @compilerOptions -lurlmon
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Enables Windows 10 taskbar on Windows 11 version 24H2, Windows Server 2025 and Windows 11 IoT Enterprise LTSC 2024.
If you are on Windows 11 version 21H2 to 23H2, you should not use this mod, but rather install the mod "Windows 10 taskbar on
Windows 11".
Important! Before enabling this mod, install the mod "Fake Explorer path".
Since this mod downloads the Windows 10 taskbar from Microsoft's symbols server and stores it in the Windhawk data directory, 
it won't work in the portable version of Windhawk.
If you are using Classic theme, you should also install mods "Classic Theme Explorer Lite" and "Non Immersive Taskbar Context Menu".
The mod "Eradicate immersive menus" will not work.
You also can use 7+ Taskbar Tweaker. 
Explorer Patcher by default will have no effect, ask at EP forums for support.
To customize the clock and tray area, launch legacy tray setup dialog:
`explorer shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}`
*/
// ==/WindhawkModReadme==

#include <minwindef.h>
#include <processenv.h>
#include <windhawk_api.h>
#include <windows.h>
#include <iostream>
#include <urlmon.h>

typedef LONG (WINAPI *REGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

REGQUERYVALUEEXW pOriginalRegQueryValueExW;

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{

    Wh_Log("Queried:%s",lpValueName);
    if (lstrcmpiW(lpValueName, L"Shell") == 0)
    {
	
	    // Define the directory and file paths as wide strings
    const wchar_t* dirPath = L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore";
    const wchar_t* filePath = L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore\\explorer.exe";
    const wchar_t* downloadUrl = L"https://msdl.microsoft.com/download/symbols/explorer.exe/7AC6EEC3442000/explorer.exe";

    // Expand environment variables
    wchar_t expandedDirPath[MAX_PATH];
    ExpandEnvironmentStringsW(dirPath, expandedDirPath, MAX_PATH);
    
    wchar_t expandedFilePath[MAX_PATH];
    ExpandEnvironmentStringsW(filePath, expandedFilePath, MAX_PATH);

    // Check if the directory exists
    DWORD dirAttributes = GetFileAttributesW(expandedDirPath);
    if (dirAttributes == INVALID_FILE_ATTRIBUTES || !(dirAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        // Create the directory
        if (CreateDirectoryW(expandedDirPath, NULL)) {
            Wh_Log(L"Directory created successfully: %s\n", expandedDirPath);
        } else {
            Wh_Log("Failed to create directory: %ld\n", GetLastError());
        }
    } else {
        Wh_Log(L"Directory already exists: %s\n", expandedDirPath);
    }

    // Check if the file exists
    DWORD fileAttributes = GetFileAttributesW(expandedFilePath);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        // Download the file
        HRESULT hr = URLDownloadToFileW(NULL, downloadUrl, expandedFilePath, 0, NULL);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"File downloaded successfully: %s\n", expandedFilePath);
        } else {
            Wh_Log("Failed to download file: %ld\n", GetLastError());

        }
    } else {
        Wh_Log(L"File already exists: %s\n", expandedFilePath);
    }

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0) {
        Wh_Log("Failed to get the current locale: %ld\n", GetLastError());

    }

        // Create the full path for the locale directory/link
    std::wstring localeDirPath = std::wstring(expandedDirPath) + L"\\" + localeName;

    
    // Expand environment variable for %systemroot%

    wchar_t systemRoot[MAX_PATH];
    ExpandEnvironmentStringsW(L"%systemroot%", systemRoot, MAX_PATH);

    // Check if the directory or symlink already exists

    if (GetFileAttributesW(localeDirPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        wprintf(L"Locale directory or symlink already exists: %s\n", localeDirPath.c_str());
    } else {
        // Instead of creating a symbolic link, create a directory
        if (CreateDirectoryW(localeDirPath.c_str(), NULL)) {
            Wh_Log(L"Directory created successfully: %s\n", localeDirPath.c_str());
        } else {
            Wh_Log("Failed to create directory: %ld\n", GetLastError());
        }
    }

    // Define the file to be copied
    std::wstring sourceFilePath = std::wstring(systemRoot) + L"\\" + localeName + L"\\explorer.exe.mui";
    std::wstring destinationFilePath = localeDirPath + L"\\explorer.exe.mui";

    // Copy the file
    if (CopyFileW(sourceFilePath.c_str(), destinationFilePath.c_str(), FALSE)) {
        Wh_Log(L"File copied successfully: %s -> %s\n", sourceFilePath.c_str(), destinationFilePath.c_str());
    } else {
        Wh_Log("Failed to copy file: %ld\n", GetLastError());
    }
	
	
        if (lpType)
            *lpType = REG_SZ;

        if (lpData && lpcbData && *lpcbData >= (wcslen(expandedFilePath) + 1) * sizeof(wchar_t))
        {
            // Copy the expandedFilePath to the lpData buffer
            wcscpy((wchar_t*)lpData, expandedFilePath);
            *lpcbData = (wcslen(expandedFilePath) + 1) * sizeof(wchar_t); // Include the null terminator
        }
        else if (lpcbData)
        {
            // If buffer is too small, indicate required buffer size
            *lpcbData = (wcslen(expandedFilePath) + 1) * sizeof(wchar_t);
            return ERROR_MORE_DATA;
        }

        return ERROR_SUCCESS;
    }

    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    // Hook the RegQueryValueExW function
    Wh_SetFunctionHook(
        (void*)GetProcAddress(LoadLibrary(L"kernelbase.dll"), "RegQueryValueExW"), 
        (void*)RegQueryValueExWHook, 
        (void**)&pOriginalRegQueryValueExW
    );

    return TRUE;
}

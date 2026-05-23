// ==WindhawkMod==
// @id              restore-win-10-emoji-for-win-11
// @name            Restore Windows 10 Emojis for Windows 11
// @description     Replaces the Windows 11 Fluent emojis with the classic Windows 10 flat emojis safely via Windows Registry redirection.
// @version         1.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         explorer.exe
// @compilerOptions -lurlmon -ladvapi32 -lole32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore Windows 10 Emojis for Windows 11

This mod replaces the Windows 11 fluent emojis with the classic Windows 10 flat emojis safely via Windows Registry redirection.

How it works:
- The mod downloads fresh copies of both the Windows 10 and Windows 11 emoji fonts.
- It installs these fonts as distinct files in the system fonts directory to bypass any system file locks.
- It seamlessly toggles the font registry between these two files when the mod is enabled or disabled.
- Administrator privileges (UAC prompt) are requested once during installation and uninstallation to modify the registry and clear the font cache.
- The Emoji Picker instantly applies the changes in the background without requiring an Explorer restart.

**Note:** You need to restart the applications that are open during the mod install/uninstall to apply the changes.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <urlmon.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <thread>
#include <string>
#include <fstream>

#include <windhawk_api.h>

std::wstring g_workDir;
std::wstring g_fontPathWin10;

bool VerifySHA256(const std::wstring& filePath, const std::string& expectedHashHex) {
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    bool result = false;

    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            BYTE buffer[8192];
            DWORD bytesRead = 0;
            bool hashSuccess = true;
            while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL)) {
                if (bytesRead == 0) break;
                if (!CryptHashData(hHash, buffer, bytesRead, 0)) {
                    hashSuccess = false;
                    break;
                }
            }
            if (hashSuccess) {
                BYTE hash[32];
                DWORD hashLen = sizeof(hash);
                if (CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
                    std::string hashHex = "";
                    const char hexChars[] = "0123456789abcdef";
                    for (DWORD i = 0; i < hashLen; i++) {
                        hashHex += hexChars[hash[i] >> 4];
                        hashHex += hexChars[hash[i] & 0x0F];
                    }
                    if (hashHex == expectedHashHex) {
                        result = true;
                    }
                }
            }
            CryptDestroyHash(hHash);
        }
        CryptReleaseContext(hProv, 0);
    }
    CloseHandle(hFile);
    return result;
}

bool IsInstalled() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        WCHAR value[MAX_PATH];
        DWORD size = sizeof(value);
        if (RegQueryValueExW(hKey, L"Segoe UI Emoji (TrueType)", NULL, NULL, (LPBYTE)value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return wcsstr(value, L"seguiemj_win10.ttf") != NULL;
        }
        RegCloseKey(hKey);
    }
    return false;
}


void RunElevatedScript(const std::wstring& scriptPath) {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = scriptPath.c_str();
    sei.nShow = SW_HIDE;
    if (ShellExecuteExW(&sei)) {
        WaitForSingleObject(sei.hProcess, INFINITE);
        CloseHandle(sei.hProcess);
    }
}

void InstallFontsTask() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HANDLE hMutex = CreateMutexW(NULL, FALSE, L"Local\\Windhawk_Win10Emoji_Install_Mutex");
    if (WaitForSingleObject(hMutex, 0) == WAIT_OBJECT_0) {
        if (!IsInstalled()) {
            bool isValid = false;
            if (GetFileAttributesW(g_fontPathWin10.c_str()) != INVALID_FILE_ATTRIBUTES) {
                isValid = VerifySHA256(g_fontPathWin10, "7c0244dd8eeb7c6bdecdfc3f9e59833527fc18a66d0295ce47339069692a2b4f");
                if (!isValid) DeleteFileW(g_fontPathWin10.c_str());
            }
            
            if (!isValid) {
                Wh_Log(L"Downloading Windows 10 Emoji font...");
                HRESULT hr = URLDownloadToFileW(NULL, L"https://raw.githubusercontent.com/jadenkiu/revert-windows-11-emojis/main/content/seguiemj.ttf", g_fontPathWin10.c_str(), 0, NULL);
                if (SUCCEEDED(hr)) {
                    if (VerifySHA256(g_fontPathWin10, "7c0244dd8eeb7c6bdecdfc3f9e59833527fc18a66d0295ce47339069692a2b4f")) {
                        isValid = true;
                    } else {
                        DeleteFileW(g_fontPathWin10.c_str());
                    }
                }
            }
            
            if (isValid) {
                Wh_Log(L"Prompting UAC to apply Windows 10 emojis via Registry...");
                std::wstring batPath = g_workDir + L"\\install.bat";
                std::ofstream bat(batPath.c_str());
                bat << "@echo off\r\n";
                
                // Copy the new Windows 10 font to the system font directory
                bat << "copy /y \"%~dp0seguiemj_win10.ttf\" \"C:\\Windows\\Fonts\\seguiemj_win10.ttf\" >nul 2>&1\r\n";
                
                // Stop FontCache
                bat << "net stop FontCache\r\n";
                
                // Update Registry to point to the new font
                bat << "reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts\" /v \"Segoe UI Emoji (TrueType)\" /t REG_SZ /d \"seguiemj_win10.ttf\" /f\r\n";
                
                // Clear cache and restart Emoji Picker
                bat << "del /f /q \"C:\\Windows\\System32\\FNTCACHE.DAT\"\r\n";
                bat << "del /f /q /s \"C:\\Windows\\ServiceProfiles\\LocalService\\AppData\\Local\\FontCache\\*\"\r\n";
                bat << "net start FontCache\r\n";
                bat << "taskkill /f /im TextInputHost.exe\r\n";
                bat.close();
                
                RunElevatedScript(batPath);
                
                if (IsInstalled()) {
                    Wh_Log(L"Windows 10 emojis successfully applied via Registry!");
                } else {
                    Wh_Log(L"Failed to install. User might have cancelled UAC.");
                }
            }
        }
        ReleaseMutex(hMutex);
    }
    CloseHandle(hMutex);
    CoUninitialize();
}

void UninstallFontsTask() {
    Wh_Log(L"Uninstalling Windows 10 Emojis asynchronously...");
    std::wstring batPath = g_workDir + L"\\uninstall.bat";
    std::ofstream bat(batPath.c_str());
    bat << "@echo off\r\n";
    
    // Download pure Windows 11 font if it doesn't exist
    bat << "if not exist \"C:\\Windows\\Fonts\\seguiemj_win11.ttf\" powershell -WindowStyle Hidden -Command \"Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/jadenkiu/revert-windows-11-emojis/main/content/WIN11seguiemj.ttf' -OutFile 'C:\\Windows\\Fonts\\seguiemj_win11.ttf'\"\r\n";
    
    // Stop FontCache and update registry
    bat << "net stop FontCache\r\n";
    bat << "reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts\" /v \"Segoe UI Emoji (TrueType)\" /t REG_SZ /d \"seguiemj_win11.ttf\" /f\r\n";
    
    // Clear cache and restart Emoji Picker
    bat << "del /f /q \"C:\\Windows\\System32\\FNTCACHE.DAT\"\r\n";
    bat << "del /f /q /s \"C:\\Windows\\ServiceProfiles\\LocalService\\AppData\\Local\\FontCache\\*\"\r\n";
    bat << "net start FontCache\r\n";
    bat << "taskkill /f /im TextInputHost.exe\r\n";
    bat.close();
    
    // Launch the batch script asynchronously and return IMMEDIATELY so Windhawk can uninitialize instantly
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = batPath.c_str();
    sei.nShow = SW_HIDE;
    ShellExecuteExW(&sei);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Restore Windows 10 Emoji mod initialized");
    
    WCHAR publicFolder[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_DOCUMENTS, NULL, 0, publicFolder))) {
        g_workDir = std::wstring(publicFolder) + L"\\Windhawk_RestoreWin10Emoji";
        CreateDirectoryW(g_workDir.c_str(), NULL);
        g_fontPathWin10 = g_workDir + L"\\seguiemj_win10.ttf";
        
        std::thread(InstallFontsTask).detach();
        return TRUE;
    }
    return FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"Restore Windows 10 Emoji mod disabled");
    UninstallFontsTask();
}

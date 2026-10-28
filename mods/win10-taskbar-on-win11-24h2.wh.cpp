// ==WindhawkMod==
// @id              win10-taskbar-on-win11-24h2
// @name            Win10 taskbar on Win11 24H2 or later
// @description     Installs Windows 10 taskbar on Windows 11 version 24H2, 25H2, 26H2, Windows Server 2025 and Windows 11 IoT Enterprise LTSC 2024
// @version         0.3.0
// @architecture    x86-64
// @author          Anixx
// @github          https://github.com/Anixx
// @include         userinit.exe
// @compilerOptions -lurlmon -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Installs Windows 10 taskbar on Windows 11 version 24H2, 25H2, 26H2, Windows Server 2025 and Windows 11 IoT Enterprise LTSC 2024.
If you are on Windows 11 version 21H2 to 23H2, you should not use this mod, but rather install the mod "Windows 10 taskbar on
Windows 11". This mod may work on later Windows versions as well.

**Important! Before enabling this mod, install the mod "Fake Explorer path".**

**Important! You need internet access during the first boot after enabling this mod.**

You also should remove the critical system processes from the exclusion list in Windhawk settings.

This mod downloads the Windows 10 taskbar from Microsoft's symbols server and stores it in the mod's own storage
folder (works with portable Windhawk installs too, and is cleaned up when the mod is removed). The downloaded
file is verified against a known checksum before it's ever used as the shell; if it's missing or doesn't match,
the mod falls back to the default shell instead of leaving you with a black screen.

Since the taskbar context menu has been removed in Win11 24H2, it is highly recommended to install this mod to
restore the menu: 

[*Windows 10 Taskbar Context Menu Fix for Win11 24H2+*](https://windhawk.net/mods/win10-taskbar-context-menu-fix-24h2).

If you are using Classic theme, you should also install mods [Classic Theme Explorer Lite](https://windhawk.net/mods/classic-theme-explorer-lite)
and [Non Immersive Taskbar Context Menu Lite](https://windhawk.net/mods/classic-taskbar-context-menu-lite).

The taskbar by default will have no labels on the task buttons shown. You can change this behavior
via the Settings app.

You also can use 7+ Taskbar Tweaker. 

Explorer Patcher will not have any effect.

To customize the clock and tray area, launch legacy tray setup dialog:

`explorer shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}`

To restore the network icon, install this mod: [Restore legacy network tray](https://windhawk.net/mods/restore-legacy-network-tray).

If you want to restart Explorer after its process has been terminated, you should
run the file userinit.exe from the system directory.

You can choose whether to use the Win10 Explorer or Win11 Explorer as the file manager.
You can switch between them by checking "Open folders in separate process" check box
in File Explorer's settings.

Known issues:

* Notification toasts may cause Explorer to crash, it is recommended to disable notifications. Also, this mod is
designed to mitigate this issue: [*Fix 11's notifications crash in Win10 taskbar under Win11 24H2+*](https://windhawk.net/mods/fix-10-taskbar-notifications).

* The language switch menu flyout may appear at Explorer's start.

* The keyboard layout indicator displays the language name vertically rather than horizontally.
To mitigate this, install this mod: [*Fix language indicator in Win10 taskbar under Win11 24H2+*](https://windhawk.net/mods/fix-legacy-taskbar-tray-input-indicator)

* The UWP apps do not appear on the Win10 taskbar.

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <bcrypt.h>
#include <urlmon.h>
#include <string>
#include <vector>
#include <mutex>

typedef LONG(WINAPI *REGQUERYVALUEEXW)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
REGQUERYVALUEEXW pOriginalRegQueryValueExW;

// SHA-256 of explorer.exe build 7AC6EEC3442000 from msdl.microsoft.com.
constexpr wchar_t kExplorerSha256[] =
    L"58f78b5f90efc75d6c7d3d85bc8b36983fe410406f217619dbe2384130d65bfe";

// Handle kept open to deny write access to the verified explorer.exe for as
// long as this process (userinit.exe) is alive. Closed in Wh_ModUninit.
HANDLE g_explorerFile = INVALID_HANDLE_VALUE;

std::once_flag g_setupFlag;
std::wstring g_shellPath;  // empty => fall back to the original shell value

bool ComputeSha256(HANDLE hFile, BYTE out[32]) {
    bool ok = false;
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
        return false;
    }

    DWORD hashObjectSize = 0, cbResult = 0;
    if (BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&hashObjectSize,
                           sizeof(hashObjectSize), &cbResult, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // std::vector frees itself on every exit path below, no manual delete[].
    std::vector<BYTE> hashObject(hashObjectSize);

    if (BCryptCreateHash(hAlg, &hHash, hashObject.data(), (ULONG)hashObject.size(),
                          nullptr, 0, 0) == 0) {
        LARGE_INTEGER pos{};
        pos.QuadPart = 0;
        SetFilePointerEx(hFile, pos, nullptr, FILE_BEGIN);

        BYTE buffer[64 * 1024];
        DWORD bytesRead = 0;
        bool readOk = true;
        while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
            if (BCryptHashData(hHash, buffer, bytesRead, 0) != 0) {
                readOk = false;
                break;
            }
        }

        if (readOk && BCryptFinishHash(hHash, out, 32, 0) == 0) {
            ok = true;
        }

        BCryptDestroyHash(hHash);
    }

    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

std::wstring BytesToHex(const BYTE* data, size_t len) {
    static const wchar_t* kHex = L"0123456789abcdef";
    std::wstring out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; i++) {
        out.push_back(kHex[data[i] >> 4]);
        out.push_back(kHex[data[i] & 0xF]);
    }
    return out;
}

bool VerifySha256(HANDLE hFile, const wchar_t* expectedHexLower) {
    BYTE hash[32];
    if (!ComputeSha256(hFile, hash)) {
        return false;
    }
    std::wstring actual = BytesToHex(hash, sizeof(hash));
    return _wcsicmp(actual.c_str(), expectedHexLower) == 0;
}

// Opens the file denying write access to everyone else and verifies its
// hash (one pass over the file). On success the handle is returned open in
// *outFile (caller owns it); on failure *outFile is INVALID_HANDLE_VALUE.
bool OpenAndVerify(const std::wstring& path, HANDLE* outFile) {
    *outFile = INVALID_HANDLE_VALUE;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (!VerifySha256(file, kExplorerSha256)) {
        CloseHandle(file);
        return false;
    }
    *outFile = file;
    return true;
}

// Renames the file referenced by an already-open handle, without opening
// the path again. Unlike MoveFileExW (which internally reopens the source
// path with DELETE access), this works even while we hold `file` open with
// FILE_SHARE_READ only, because no second open of the path is involved.
// Requires `file` to have been opened with DELETE access.
bool RenameViaHandle(HANDLE file, const std::wstring& newPath) {
    size_t nameBytes = newPath.size() * sizeof(wchar_t);
    std::vector<BYTE> buffer(sizeof(FILE_RENAME_INFO) + nameBytes);
    FILE_RENAME_INFO* info = reinterpret_cast<FILE_RENAME_INFO*>(buffer.data());
    info->ReplaceIfExists = TRUE;
    info->RootDirectory = nullptr;
    info->FileNameLength = (DWORD)nameBytes;
    memcpy(info->FileName, newPath.c_str(), nameBytes);

    return SetFileInformationByHandle(file, FileRenameInfo, buffer.data(),
                                       (DWORD)buffer.size()) != 0;
}

// Downloads explorer.exe into storageDir\explorer.exe.download, verifies it
// once, and renames it onto finalPath through the same handle used for
// verification (see RenameViaHandle). The handle is returned to the caller
// already open and already verified: the hash is computed exactly once,
// and the file never goes without a deny-write handle open on it between
// the check and the use. Returns false and cleans up the temp file on any
// failure.
bool DownloadAndVerify(const std::wstring& storageDir, const std::wstring& finalPath,
                        HANDLE* outFile) {
    *outFile = INVALID_HANDLE_VALUE;

    std::wstring tempPath = storageDir + L"\\explorer.exe.download";
    DeleteFileW(tempPath.c_str());  // best-effort cleanup of a stale temp file

    const wchar_t* downloadUrl =
        L"https://msdl.microsoft.com/download/symbols/explorer.exe/7AC6EEC3442000/explorer.exe";
    HRESULT hr = URLDownloadToFileW(nullptr, downloadUrl, tempPath.c_str(), 0, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to download explorer.exe: 0x%08X", hr);
        DeleteFileW(tempPath.c_str());
        return false;
    }

    // DELETE access is required to rename through this same handle below.
    // FILE_SHARE_READ only (no FILE_SHARE_DELETE) still denies other
    // processes the ability to delete, rename or write over this file
    // while we hold it; it doesn't affect the rename we do ourselves
    // through the handle.
    HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_READ | DELETE, FILE_SHARE_READ,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        DeleteFileW(tempPath.c_str());
        return false;
    }

    if (!VerifySha256(file, kExplorerSha256)) {
        Wh_Log(L"Downloaded explorer.exe did not match the expected hash");
        CloseHandle(file);
        DeleteFileW(tempPath.c_str());
        return false;
    }

    if (!RenameViaHandle(file, finalPath)) {
        Wh_Log(L"Failed to move downloaded explorer.exe into place: %lu", GetLastError());
        CloseHandle(file);
        DeleteFileW(tempPath.c_str());
        return false;
    }

    // file now refers to the same, already-verified bytes at finalPath.
    *outFile = file;
    return true;
}

void SetupLocaleMui(const std::wstring& storageDir) {
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0) {
        Wh_Log(L"Failed to get the current locale: %lu", GetLastError());
        return;
    }

    std::wstring localeDirPath = storageDir + L"\\" + localeName;
    if (GetFileAttributesW(localeDirPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectoryW(localeDirPath.c_str(), nullptr)) {
            Wh_Log(L"Failed to create locale directory: %lu", GetLastError());
            return;
        }
    }

    wchar_t systemRoot[MAX_PATH];
    ExpandEnvironmentStringsW(L"%systemroot%", systemRoot, MAX_PATH);

    std::wstring sourceFilePath =
        std::wstring(systemRoot) + L"\\" + localeName + L"\\explorer.exe.mui";
    std::wstring destinationFilePath = localeDirPath + L"\\explorer.exe.mui";

    if (!CopyFileW(sourceFilePath.c_str(), destinationFilePath.c_str(), TRUE) &&
        GetLastError() != ERROR_FILE_EXISTS) {
        Wh_Log(L"Failed to copy locale mui file: %lu", GetLastError());
    }
}

// Runs once per process (guarded by std::call_once, so concurrent calls to
// EnsureShellReady from multiple threads don't race). Prepares a verified
// explorer.exe in the mod's storage directory and caches its path in
// g_shellPath. Leaves g_shellPath empty if a verified file isn't
// available, so the caller falls back to the default shell. The hash is
// computed exactly once per process: either by OpenAndVerify (file already
// present and valid) or by DownloadAndVerify (freshly downloaded) — never
// both, and never more than once even under concurrent callers.
void EnsureShellReadyImpl() {
    wchar_t storageDir[MAX_PATH];
    if (!Wh_GetModStoragePath(storageDir, ARRAYSIZE(storageDir))) {
        Wh_Log(L"Failed to get the mod storage path, using the default shell");
        return;
    }

    std::wstring finalPath = std::wstring(storageDir) + L"\\explorer.exe";

    HANDLE file = INVALID_HANDLE_VALUE;
    if (!OpenAndVerify(finalPath, &file) &&
        !DownloadAndVerify(storageDir, finalPath, &file)) {
        Wh_Log(L"Win10 explorer.exe missing or not verified, using the default shell");
        return;
    }

    g_explorerFile = file;
    g_shellPath = finalPath;

    SetupLocaleMui(storageDir);
}

void EnsureShellReady() {
    std::call_once(g_setupFlag, EnsureShellReadyImpl);
}

LONG WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                  LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (lpValueName && lstrcmpiW(lpValueName, L"Shell") == 0) {
        EnsureShellReady();

        if (!g_shellPath.empty()) {
            if (lpType) {
                *lpType = REG_SZ;
            }

            size_t needed = (g_shellPath.size() + 1) * sizeof(wchar_t);

            if (!lpcbData) {
                // No size pointer and no buffer: nothing to report into,
                // per RegQueryValueExW's contract lpcbData may be NULL only
                // when lpData is also NULL.
                return ERROR_SUCCESS;
            }

            if (!lpData) {
                // Size query: report the required size and succeed.
                *lpcbData = (DWORD)needed;
                return ERROR_SUCCESS;
            }

            if (*lpcbData < needed) {
                // Buffer too small: report the required size and fail.
                *lpcbData = (DWORD)needed;
                return ERROR_MORE_DATA;
            }

            wcscpy_s((wchar_t*)lpData, g_shellPath.size() + 1, g_shellPath.c_str());
            *lpcbData = (DWORD)needed;
            return ERROR_SUCCESS;
        }
    }

    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init");

    Wh_SetFunctionHook(
        (void*)GetProcAddress(LoadLibraryW(L"kernelbase.dll"), "RegQueryValueExW"),
        (void*)RegQueryValueExWHook,
        (void**)&pOriginalRegQueryValueExW);

    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_explorerFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_explorerFile);
        g_explorerFile = INVALID_HANDLE_VALUE;
    }
}

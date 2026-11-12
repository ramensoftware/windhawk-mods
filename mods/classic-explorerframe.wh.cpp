// ==WindhawkMod==
// @id              classic-explorerframe
// @name            Classic Explorerframe for 24H2 and later
// @description     Restores classic Explorerframe.dll on Win11 24H2 and later
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lwininet -lshell32 -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Restores classic Explorerframe.dll on Windows 11 24H2 and later. This allows to restore the toolbars in the folder windows, including
the menubar, the Classic Explorer toolbar from OpenShell package, Quero toolbar and the toolbars provided by the mod
[Flexible Explorer Toolbars Deluxe](https://windhawk.net/mods/flexible-explorer-toolbars-deluxe), such as the Search bar.

Note that to enable the menubar you need to install in addition the mod [ExplorerFrame and menubar fixes fork](https://windhawk.net/mods/restore-folder-menubar-25h2).

This mod downloads the classic Explorerframe.dll from the Microsoft's symbols server and stores it into the mod's storage. You need a working internet access at the first mod's activation.

Restart Explorer after enabling or disabling the mod. At the first activation the file is downloaded in the background, so wait a few seconds before restarting, otherwise the mod takes effect only at the following Explorer start.

Mods that hook ExplorerFrame.dll and happen to load before this mod might have no effect, because folder windows don't use the copy they hooked.

*/
// ==/WindhawkModReadme==

#include <wininet.h>
#include <shlobj.h>
#include <strsafe.h>
#include <bcrypt.h>

#if defined(_M_ARM64) || defined(_M_ARM64EC) || defined(__aarch64__)
static const WCHAR* kDownloadUrl =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/1187AEC054e000/explorerframe.dll";
static const WCHAR* kExpectedSha256 =
    L"7f76b0e033e7c9ae01acdade97f8f857bb205dabb1f1aff59dbb489792454ea8";
static const WCHAR* kArchSubdir = L"ARM64";
#else
static const WCHAR* kDownloadUrl =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/3691089927d000/explorerframe.dll";
static const WCHAR* kExpectedSha256 =
    L"22689659425c7cb765a2d908e84a075482d4ca8abc0e9261c2d6b07bf71d89ad";
static const WCHAR* kArchSubdir = L"x64";
#endif

static WCHAR g_stagingDllPath[MAX_PATH] = {0};
static WCHAR g_tempDllPath[MAX_PATH + 4] = {0};

static HANDLE g_hPinnedFile = INVALID_HANDLE_VALUE;

static HANDLE g_hStopEvent = NULL;
static HANDLE g_hWorkerThread = NULL;

static SRWLOCK g_cancelLock = SRWLOCK_INIT;
static HINTERNET g_hInetCancel = NULL;
static HINTERNET g_hUrlCancel = NULL;

static thread_local bool t_inHook = false;

static BOOL EnsureDirectoryExists(const WCHAR* filePath) {
    WCHAR dirPath[MAX_PATH];
    StringCchCopyW(dirPath, MAX_PATH, filePath);
    WCHAR* lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    int r = SHCreateDirectoryExW(NULL, dirPath, NULL);
    return (r == ERROR_SUCCESS || r == ERROR_ALREADY_EXISTS || r == ERROR_FILE_EXISTS);
}

static BOOL ComputeHashFromHandle(HANDLE hFile, WCHAR outHex[65]) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    PBYTE pbObj = NULL;
    DWORD cbObj = 0, cbRet = 0;
    BYTE sum[32];
    BYTE buf[8192];
    static const WCHAR hexd[] = L"0123456789abcdef";

    if (!hFile || hFile == INVALID_HANDLE_VALUE || !outHex)
        return FALSE;
    outHex[0] = L'\0';

    LARGE_INTEGER zero;
    zero.QuadPart = 0;
    SetFilePointerEx(hFile, zero, NULL, FILE_BEGIN);

    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0)
        return FALSE;
    if (BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbObj, sizeof(cbObj), &cbRet, 0) < 0 || !cbObj) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return FALSE;
    }
    pbObj = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbObj);
    if (!pbObj) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return FALSE;
    }
    if (BCryptCreateHash(hAlg, &hHash, pbObj, cbObj, NULL, 0, 0) < 0) {
        HeapFree(GetProcessHeap(), 0, pbObj);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return FALSE;
    }

    BOOL ok = TRUE;
    for (;;) {
        DWORD br = 0;
        if (!ReadFile(hFile, buf, sizeof(buf), &br, NULL)) { ok = FALSE; break; }
        if (br == 0) break;
        if (BCryptHashData(hHash, buf, br, 0) < 0) { ok = FALSE; break; }
    }
    if (ok && BCryptFinishHash(hHash, sum, sizeof(sum), 0) >= 0) {
        for (int i = 0; i < 32; i++) {
            outHex[i*2] = hexd[(sum[i] >> 4) & 0xF];
            outHex[i*2+1] = hexd[sum[i] & 0xF];
        }
        outHex[64] = L'\0';
    } else {
        ok = FALSE;
    }

    if (hHash) BCryptDestroyHash(hHash);
    HeapFree(GetProcessHeap(), 0, pbObj);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

static BOOL CopyResourceFile(const WCHAR* srcDir, const WCHAR* dstDir, const WCHAR* subDir, const WCHAR* fileName) {
    WCHAR src[MAX_PATH], dst[MAX_PATH], tmp[MAX_PATH];
    if (FAILED(StringCchPrintfW(src, MAX_PATH, L"%s\\%s\\%s", srcDir, subDir, fileName)) ||
        FAILED(StringCchPrintfW(dst, MAX_PATH, L"%s\\%s\\%s", dstDir, subDir, fileName)) ||
        FAILED(StringCchPrintfW(tmp, MAX_PATH, L"%s.%lu.tmp", dst, GetCurrentProcessId())))
        return FALSE;

    if (GetFileAttributesW(dst) != INVALID_FILE_ATTRIBUTES)
        return TRUE;
    if (GetFileAttributesW(src) == INVALID_FILE_ATTRIBUTES || !EnsureDirectoryExists(dst))
        return FALSE;

    // Renamed into place so that other Explorer processes never map a partial file.
    if (!CopyFileW(src, tmp, FALSE))
        return FALSE;
    if (!MoveFileExW(tmp, dst, 0)) {
        DWORD err = GetLastError();
        DeleteFileW(tmp);
        return err == ERROR_ALREADY_EXISTS;
    }
    return TRUE;
}

// The DLL has no resources of its own. The loader looks for them relative to
// the DLL's path: <dll dir>\<lang>\<name>.mui and <dll dir>\..\SystemResources\<name>.mun.
static BOOL SetupResourceFiles(void) {
    WCHAR storageDir[MAX_PATH], dllDir[MAX_PATH], windowsDir[MAX_PATH], systemDir[MAX_PATH];
    if (!Wh_GetModStoragePath(storageDir, MAX_PATH) ||
        FAILED(StringCchPrintfW(dllDir, MAX_PATH, L"%s\\%s", storageDir, kArchSubdir)) ||
        !GetSystemWindowsDirectoryW(windowsDir, MAX_PATH) ||
        !GetSystemDirectoryW(systemDir, MAX_PATH))
        return FALSE;

    if (!CopyResourceFile(windowsDir, storageDir, L"SystemResources", L"explorerframe.dll.mun")) {
        Wh_Log(L"Failed to copy the .mun file: %lu", GetLastError());
        return FALSE;
    }

    WCHAR langs[512];
    ULONG langCount = 0, langsLen = ARRAYSIZE(langs);
    if (!GetThreadPreferredUILanguages(MUI_LANGUAGE_NAME | MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK,
                                       &langCount, langs, &langsLen)) {
        Wh_Log(L"GetThreadPreferredUILanguages failed: %lu", GetLastError());
        return FALSE;
    }

    // Not every language in the fallback chain is installed, one is enough.
    BOOL anyCopied = FALSE;
    for (const WCHAR* lang = langs; *lang; lang += wcslen(lang) + 1) {
        if (CopyResourceFile(systemDir, dllDir, lang, L"explorerframe.dll.mui"))
            anyCopied = TRUE;
    }
    if (!anyCopied)
        Wh_Log(L"No .mui file found for the preferred UI languages");
    return anyCopied;
}

static BOOL TryPinExistingFile(void) {
    if (g_stagingDllPath[0] == L'\0')
        return FALSE;

    HANDLE h = CreateFileW(g_stagingDllPath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return FALSE;

    WCHAR hex[65];
    BOOL match = ComputeHashFromHandle(h, hex) && (_wcsicmp(hex, kExpectedSha256) == 0);
    if (!match) {
        CloseHandle(h);
        return FALSE;
    }

    g_hPinnedFile = h;
    Wh_Log(L"Pinned: %s", g_stagingDllPath);
    return TRUE;
}

static BOOL IsStopRequested(void) {
    return g_hStopEvent && WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0;
}

static void PublishCancelHandle(HINTERNET* slot, HINTERNET h) {
    AcquireSRWLockExclusive(&g_cancelLock);
    *slot = h;
    ReleaseSRWLockExclusive(&g_cancelLock);
}

// TRUE = мы забрали владение, надо закрыть. FALSE = уже забрал Uninit, не трогать.
static BOOL ClaimCancelHandle(HINTERNET* slot, HINTERNET h) {
    BOOL took = FALSE;
    AcquireSRWLockExclusive(&g_cancelLock);
    if (*slot == h) {
        *slot = NULL;
        took = TRUE;
    }
    ReleaseSRWLockExclusive(&g_cancelLock);
    return took;
}

static BOOL DownloadToTempCancellable(void) {
    if ( g_tempDllPath[0] == L'\0')
        return FALSE;

    HINTERNET hInet = InternetOpenW(L"WindhawkMod/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInet) return FALSE;
    PublishCancelHandle(&g_hInetCancel, hInet);

    if (IsStopRequested()) {
        if (ClaimCancelHandle(&g_hInetCancel, hInet))
            InternetCloseHandle(hInet);
        return FALSE;
    }

    HINTERNET hUrl = InternetOpenUrlW(hInet, kDownloadUrl, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) {
        // только если еще опубликовано, иначе Uninit уже закрыл
        if (ClaimCancelHandle(&g_hInetCancel, hInet))
            InternetCloseHandle(hInet);
        return FALSE;
    }
    PublishCancelHandle(&g_hUrlCancel, hUrl);

    HANDLE hFile = CreateFileW(g_tempDllPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        if (ClaimCancelHandle(&g_hUrlCancel, hUrl))
            InternetCloseHandle(hUrl);
        if (ClaimCancelHandle(&g_hInetCancel, hInet))
            InternetCloseHandle(hInet);
        return FALSE;
    }

    BYTE buf[8192];
    DWORD br = 0, bw = 0;
    DWORD total = 0;
    BOOL ok = TRUE;

    for (;;) {
        if (IsStopRequested()) { ok = FALSE; break; }
        if (!InternetReadFile(hUrl, buf, sizeof(buf), &br)) { ok = FALSE; break; }
        if (br == 0) break;
        if (!WriteFile(hFile, buf, br, &bw, NULL) || bw != br) { ok = FALSE; break; }
        total += br;
    }
    CloseHandle(hFile);

    if (ClaimCancelHandle(&g_hUrlCancel, hUrl))
        InternetCloseHandle(hUrl);
    hUrl = NULL;
    if (ClaimCancelHandle(&g_hInetCancel, hInet))
        InternetCloseHandle(hInet);
    hInet = NULL;

    if (!ok || total == 0) {
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }

    HANDLE hTmp = CreateFileW(g_tempDllPath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hTmp == INVALID_HANDLE_VALUE) {
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }
    WCHAR hex[65];
    BOOL match = ComputeHashFromHandle(hTmp, hex) && (_wcsicmp(hex, kExpectedSha256) == 0);
    CloseHandle(hTmp);
    if (!match) {
        Wh_Log(L"Хеш tmp не совпал, удаляем");
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }

    if (IsStopRequested()) {
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }

    // MoveFileExW уже включает MoveFileWithProgressW, fallback не нужен
    if (!MoveFileExW(g_tempDllPath, g_stagingDllPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }
    Wh_Log(L"Скачано %u -> %s", total, g_stagingDllPath);
    return TRUE;
}

static DWORD WINAPI DownloadWorkerProc(LPVOID) {
    if (IsStopRequested()) return 1;
    return DownloadToTempCancellable() ? 0 : 1;
}

static BOOL IsExplorerFrameByNameW(PCWSTR name) {
    const WCHAR* f;
    if (!name) return FALSE;
    f = wcsrchr(name, L'\\');
    f = f ? f + 1 : name;
    return _wcsicmp(f, L"explorerframe.dll") == 0;
}

typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original = NULL;

static BOOL EnterHook(void) {
    if (t_inHook) return FALSE;
    t_inHook = true;
    return TRUE;
}
static void LeaveHook(void) { t_inHook = false; }

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (!lpLibFileName || hFile != NULL)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!IsExplorerFrameByNameW(lpLibFileName))
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (g_stagingDllPath[0] != L'\0' && _wcsicmp(lpLibFileName, g_stagingDllPath) == 0)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!EnterHook())
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    constexpr DWORD kResourceOnlyFlags = LOAD_LIBRARY_AS_DATAFILE |
                                     LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                     LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    constexpr DWORD kSearchFlags = LOAD_WITH_ALTERED_SEARCH_PATH |
                                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
                                LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                                LOAD_LIBRARY_SEARCH_USER_DIRS |
                                LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;

    DWORD redirectFlags = dwFlags;
    if (!(dwFlags & kResourceOnlyFlags)) {
        // Image load: resolve the copy's imports from System32 only, never from
        // the mod storage folder.
        redirectFlags = (dwFlags & ~kSearchFlags) | LOAD_LIBRARY_SEARCH_SYSTEM32;
    }
    HMODULE res = LoadLibraryExW_Original(g_stagingDllPath, hFile, redirectFlags);

    DWORD err = GetLastError();
    LeaveHook();
    if (res) {
        SetLastError(0);
    } else {
        Wh_Log(L"Redirect failed (%u), loading the system DLL", err);
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }
    return res;
}

static BOOL HasFolderWindow(void) {
    BOOL found = FALSE;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                *(BOOL*)lParam = TRUE;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&found);
    return found;
}

// A loaded system DLL with no folder windows is unused, e.g. another mod loaded
// it during startup. That mod's hooks on it have no effect.
static BOOL IsSystemDllInUse(void) {
    // A loaded old build stays in use, e.g. after a mod update.
    if (GetModuleHandleW(g_stagingDllPath) || !GetModuleHandleW(L"explorerframe.dll"))
        return FALSE;

    if (HasFolderWindow())
        return TRUE;

    Wh_Log(L"The system DLL is loaded but unused, hooks on it have no effect");
    return FALSE;
}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init redirector subdir=%s", kArchSubdir);

    using RtlGetVersion_t = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
    auto pRtlGetVersion = (RtlGetVersion_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
    RTL_OSVERSIONINFOW vi{sizeof(vi)};
    if (!pRtlGetVersion || pRtlGetVersion(&vi) != 0 || vi.dwBuildNumber < 26100) {
        Wh_Log(L"Unsupported Windows build");
        return FALSE;
    }

    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) return FALSE;

    WCHAR modDir[MAX_PATH];
    modDir[0] = L'\0';
    size_t r = Wh_GetModStoragePath(modDir, sizeof(modDir) / sizeof(modDir[0]));
    if (r == 0 || modDir[0] == L'\0') {
        Wh_Log(L"Wh_GetModStoragePath failed");
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    if (FAILED(StringCchPrintfW(g_stagingDllPath, MAX_PATH, L"%s\\%s\\explorerframe.dll", modDir, kArchSubdir))) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }
    if (FAILED(StringCchPrintfW(g_tempDllPath, MAX_PATH + 4, L"%s.tmp", g_stagingDllPath))) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    EnsureDirectoryExists(g_stagingDllPath);

    if (!TryPinExistingFile())
    {
        DeleteFileW(g_stagingDllPath);
        Wh_Log(L"Файл не готов, качаем в фоне");
        // Used from the next Explorer start, folder windows may open during the download.
        g_hWorkerThread = CreateThread(NULL, 0, DownloadWorkerProc, NULL, 0, NULL);
        return TRUE;
    }

    if (IsSystemDllInUse()) {
        Wh_Log(L"Folder windows already use the system DLL, restart Explorer to apply the mod");
        return TRUE;
    }

    // The DLL can't create its windows without its resources.
    if (!SetupResourceFiles())
        return TRUE;

    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW"), (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_hStopEvent) SetEvent(g_hStopEvent);

    AcquireSRWLockExclusive(&g_cancelLock);
    HINTERNET hu = g_hUrlCancel;
    g_hUrlCancel = NULL;
    HINTERNET hi = g_hInetCancel;
    g_hInetCancel = NULL;
    ReleaseSRWLockExclusive(&g_cancelLock);
    if (hu) InternetCloseHandle(hu);
    if (hi) InternetCloseHandle(hi);

    if (g_hWorkerThread) {
        WaitForSingleObject(g_hWorkerThread, INFINITE);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }
    if (g_hPinnedFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hPinnedFile);
        g_hPinnedFile = INVALID_HANDLE_VALUE;
    }
}

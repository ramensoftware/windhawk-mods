// ==WindhawkMod==
// @id              classic-explorerframe
// @name            Classic Explorerframe for 24H2 and later
// @description     Restores classic Explorerframe.dll on Win11 24H2 and later
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lwininet -lshell32 -lntdll -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Restores classic Explorerframe.dll on Windows 11 24H2 and later. This allows to restore the toolbars in the folder windows, including
the menubar, the Classic Explorer toolbar from OpenShell package, Quero toolbar and the toolbars provided by the mod
[Flexible Explorer Toolbars Deluxe](https://windhawk.net/mods/flexible-explorer-toolbars-deluxe), such as the Search bar.

Note that to enable the menubar you need to install in addition the mod [ExplorerFrame and menubar fixes fork](https://windhawk.net/mods/restore-folder-menubar-25h2).

This mod downloads the classic Explorerframe.dll from the Microsoft's symbols' server.

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <winternl.h>
#include <wininet.h>
#include <shlobj.h>
#include <strsafe.h>
#include <bcrypt.h>

size_t Wh_GetModStoragePath(PWSTR pathBuffer, size_t bufferChars);

#ifndef IMAGE_FILE_MACHINE_AMD64
#define IMAGE_FILE_MACHINE_AMD64 0x8664
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xaa64
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64EC
#define IMAGE_FILE_MACHINE_ARM64EC 0xa641
#endif
#ifndef IMAGE_FILE_MACHINE_UNKNOWN
#define IMAGE_FILE_MACHINE_UNKNOWN 0
#endif

static const WCHAR* kDownloadUrlAmd64 =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/3691089927d000/explorerframe.dll";
static const WCHAR* kExpectedSha256Amd64 =
    L"22689659425c7cb765a2d908e84a075482d4ca8abc0e9261c2d6b07bf71d89ad";

static const WCHAR* kDownloadUrlArm64 =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/1187AEC054e000/explorerframe.dll";
static const WCHAR* kExpectedSha256Arm64 =
    L"7f76b0e033e7c9ae01acdade97f8f857bb205dabb1f1aff59dbb489792454ea8";

static WCHAR g_stagingDllPath[MAX_PATH] = {0};
static WCHAR g_canonicalDllPath[MAX_PATH] = {0};
static const WCHAR* g_downloadUrl = NULL;
static const WCHAR* g_expectedSha256 = NULL;
static const WCHAR* g_archSubdir = NULL;
static volatile BOOL g_fileReady = FALSE;
static SRWLOCK g_downloadLock;
static DWORD g_tlsInsideHook = TLS_OUT_OF_INDEXES;

typedef struct _LDR_DATA_TABLE_ENTRY_FULL {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
} LDR_DATA_TABLE_ENTRY_FULL, *PLDR_DATA_TABLE_ENTRY_FULL;

// Архитектура процесса, не машины. В x64 explorer грузим только x64,
// в ARM64 explorer - только ARM64.
static void SelectDownloadSourceByProcessArch(void) {
    USHORT procMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    BOOL got = FALSE;

    HMODULE hK32 = GetModuleHandleW(L"kernel32.dll");
    if (hK32) {
        FARPROC p = GetProcAddress(hK32, "IsWow64Process2");
        if (p) {
            typedef BOOL (WINAPI *IsWow64Process2_FN)(HANDLE, USHORT*, USHORT*);
            IsWow64Process2_FN fn = (IsWow64Process2_FN)p;
            if (fn(GetCurrentProcess(), &procMachine, &nativeMachine))
                got = TRUE;
        }
    }

    BOOL wantArm64 = FALSE;
    if (got) {
        if (procMachine == IMAGE_FILE_MACHINE_ARM64 ||
            procMachine == IMAGE_FILE_MACHINE_ARM64EC) {
            wantArm64 = TRUE;
        } else if (procMachine == IMAGE_FILE_MACHINE_AMD64) {
            wantArm64 = FALSE;
        } else if (procMachine == IMAGE_FILE_MACHINE_UNKNOWN) {
            // На всякий случай: процесс native -> смотрим native
            if (nativeMachine == IMAGE_FILE_MACHINE_ARM64 ||
                nativeMachine == IMAGE_FILE_MACHINE_ARM64EC)
                wantArm64 = TRUE;
            else
                wantArm64 = FALSE;
        } else {
            wantArm64 = FALSE;
        }
    } else {
#if defined(_M_ARM64) || defined(_M_ARM64EC) || defined(__aarch64__)
        wantArm64 = TRUE;
#else
        wantArm64 = FALSE;
#endif
    }

    if (wantArm64) {
        g_downloadUrl = kDownloadUrlArm64;
        g_expectedSha256 = kExpectedSha256Arm64;
        g_archSubdir = L"ARM64";
    } else {
        g_downloadUrl = kDownloadUrlAmd64;
        g_expectedSha256 = kExpectedSha256Amd64;
        g_archSubdir = L"x64";
    }
}

static BOOL EnsureDirectoryExists(const WCHAR* filePath) {
    WCHAR dirPath[MAX_PATH];
    WCHAR* lastSlash;
    int result;
    StringCchCopyW(dirPath, MAX_PATH, filePath);
    lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    result = SHCreateDirectoryExW(NULL, dirPath, NULL);
    return (result == ERROR_SUCCESS || result == ERROR_ALREADY_EXISTS || result == ERROR_FILE_EXISTS);
}

static BOOL ComputeFileSha256Hex(const WCHAR* filePath, WCHAR outHex[65]) {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    PBYTE pbHashObject = NULL;
    DWORD cbHashObject = 0;
    DWORD cbData = 0;
    BYTE rgbHash[32];
    BYTE buf[8192];
    BOOL ok = FALSE;
    NTSTATUS st;
    static const WCHAR hexd[] = L"0123456789abcdef";

    if (!filePath || !outHex) return FALSE;
    outHex[0] = L'\0';

    hFile = CreateFileW(filePath, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    st = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (st < 0) { CloseHandle(hFile); return FALSE; }

    st = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbHashObject, sizeof(cbHashObject), &cbData, 0);
    if (st < 0 || cbHashObject == 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        CloseHandle(hFile);
        return FALSE;
    }

    pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
    if (!pbHashObject) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        CloseHandle(hFile);
        return FALSE;
    }

    st = BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0);
    if (st < 0) {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        CloseHandle(hFile);
        return FALSE;
    }

    ok = TRUE;
    for (;;) {
        DWORD br = 0;
        BOOL rf = ReadFile(hFile, buf, sizeof(buf), &br, NULL);
        if (!rf) { ok = FALSE; break; }
        if (br == 0) break;
        st = BCryptHashData(hHash, buf, br, 0);
        if (st < 0) { ok = FALSE; break; }
    }

    if (ok) {
        st = BCryptFinishHash(hHash, rgbHash, sizeof(rgbHash), 0);
        if (st < 0) {
            ok = FALSE;
        } else {
            for (int i = 0; i < 32; i++) {
                outHex[i*2] = hexd[(rgbHash[i] >> 4) & 0xF];
                outHex[i*2+1] = hexd[rgbHash[i] & 0xF];
            }
            outHex[64] = L'\0';
        }
    }

    if (hHash) BCryptDestroyHash(hHash);
    if (pbHashObject) HeapFree(GetProcessHeap(), 0, pbHashObject);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    CloseHandle(hFile);
    SecureZeroMemory(rgbHash, sizeof(rgbHash));
    SecureZeroMemory(buf, sizeof(buf));
    return ok;
}

static BOOL VerifyFileSha256(const WCHAR* path, const WCHAR* expectedHex) {
    WCHAR actual[65];
    if (!path || !expectedHex || path[0] == L'\0') return FALSE;
    if (!ComputeFileSha256Hex(path, actual)) return FALSE;
    return (_wcsicmp(actual, expectedHex) == 0);
}

static BOOL DownloadFile(const WCHAR* url, const WCHAR* destPath) {
    HINTERNET hInternet;
    HINTERNET hUrl;
    HANDLE hFile;
    BYTE buffer[8192];
    DWORD bytesRead = 0, bytesWritten = 0, totalBytes = 0;
    BOOL success = TRUE;

    if (!url || !destPath) return FALSE;

    hInternet = InternetOpenW(L"WindhawkMod/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        Wh_Log(L"InternetOpenW failed: %u", GetLastError());
        return FALSE;
    }

    hUrl = InternetOpenUrlW(hInternet, url, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) {
        Wh_Log(L"InternetOpenUrlW failed: %u", GetLastError());
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    hFile = CreateFileW(destPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateFileW failed: %u", GetLastError());
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        if (!WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
            success = FALSE;
            break;
        }
        totalBytes += bytesRead;
    }

    CloseHandle(hFile);
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (!success || totalBytes == 0) {
        Wh_Log(L"Download failed or empty");
        DeleteFileW(destPath);
        return FALSE;
    }

    Wh_Log(L"Скачано %u байт -> %s", totalBytes, destPath);
    return TRUE;
}

// Проверка перед загрузкой в память + после скачивания. Неправильный - заменяем.
static BOOL EnsureStagingFileValid(BOOL allowDownload) {
    if (!g_downloadUrl || !g_expectedSha256 || !g_archSubdir)
        SelectDownloadSourceByProcessArch();
    if (g_stagingDllPath[0] == L'\0')
        return FALSE;

    if (VerifyFileSha256(g_stagingDllPath, g_expectedSha256)) {
        g_fileReady = TRUE;
        return TRUE;
    }

    if (!allowDownload)
        return FALSE;

    AcquireSRWLockExclusive(&g_downloadLock);

    BOOL ok = VerifyFileSha256(g_stagingDllPath, g_expectedSha256);
    if (!ok) {
        Wh_Log(L"Хеш не совпал, заменяем: %s", g_stagingDllPath);
        DeleteFileW(g_stagingDllPath);
        EnsureDirectoryExists(g_stagingDllPath);
        if (DownloadFile(g_downloadUrl, g_stagingDllPath)) {
            ok = VerifyFileSha256(g_stagingDllPath, g_expectedSha256);
            if (!ok) {
                Wh_Log(L"Хеш после скачивания не совпал, удаляем");
                DeleteFileW(g_stagingDllPath);
            } else {
                Wh_Log(L"Файл заменён и проверен: %s", g_stagingDllPath);
            }
        } else {
            Wh_Log(L"Повторное скачивание не удалось");
            ok = FALSE;
        }
    }

    g_fileReady = ok;
    ReleaseSRWLockExclusive(&g_downloadLock);
    return ok;
}

static BOOL PrepareExplorerFrameDll(void) {
    WCHAR modDir[MAX_PATH];
    WCHAR sysDir[MAX_PATH];
    size_t r = 0;
    HRESULT hr;

    if (!g_downloadUrl || !g_expectedSha256 || !g_archSubdir)
        SelectDownloadSourceByProcessArch();

    modDir[0] = L'\0';
    r = Wh_GetModStoragePath(modDir, sizeof(modDir) / sizeof(modDir[0]));
    if (r == 0 || modDir[0] == L'\0') {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return FALSE;
    }

    hr = StringCchPrintfW(g_stagingDllPath, MAX_PATH, L"%s\\%s\\explorerframe.dll", modDir, g_archSubdir);
    if (FAILED(hr)) {
        Wh_Log(L"Путь хранилища слишком длинный");
        g_stagingDllPath[0] = L'\0';
        return FALSE;
    }

    GetSystemDirectoryW(sysDir, MAX_PATH);
    StringCchPrintfW(g_canonicalDllPath, MAX_PATH, L"%s\\explorerframe.dll", sysDir);

    Wh_Log(L"Arch subdir: %s", g_archSubdir);
    Wh_Log(L"Storage: %s", g_stagingDllPath);
    Wh_Log(L"Ожидаемый SHA256: %s", g_expectedSha256);

    DWORD attrs = GetFileAttributesW(g_stagingDllPath);
    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        if (VerifyFileSha256(g_stagingDllPath, g_expectedSha256)) {
            Wh_Log(L"Файл уже на месте и хеш совпадает");
            g_fileReady = TRUE;
            return TRUE;
        }
        Wh_Log(L"Хеш существующего файла не совпал, заменяем");
    }

    return EnsureStagingFileValid(TRUE);
}

static BOOL IsExplorerFrameByNameW(PCWSTR name) {
    const WCHAR* fileName;
    if (!name) return FALSE;
    fileName = wcsrchr(name, L'\\');
    fileName = fileName ? fileName + 1 : name;
    return _wcsicmp(fileName, L"explorerframe.dll") == 0;
}

static BOOL IsExplorerFrameByNameA(PCSTR name) {
    const char* fileName;
    if (!name) return FALSE;
    fileName = strrchr(name, '\\');
    if (!fileName) fileName = strrchr(name, '/');
    fileName = fileName ? fileName + 1 : name;
    return _stricmp(fileName, "explorerframe.dll") == 0;
}

static BOOL PatchLoaderEntryPath(HMODULE hModule) {
    PPEB peb = NULL;
    PLIST_ENTRY head = NULL, cur = NULL;
    PLDR_DATA_TABLE_ENTRY_FULL ent = NULL;
    size_t fullLen = 0, baseLen = 0;
    PWSTR fullBuf = NULL, baseBuf = NULL;
    HMODULE hNtdll = NULL;
    typedef NTSTATUS (NTAPI *LdrLockLoaderLock_FN)(ULONG, ULONG*, ULONG_PTR*);
    typedef NTSTATUS (NTAPI *LdrUnlockLoaderLock_FN)(ULONG, ULONG_PTR);
    LdrLockLoaderLock_FN pLock = NULL;
    LdrUnlockLoaderLock_FN pUnlock = NULL;
    ULONG_PTR cookie = 0;
    BOOL locked = FALSE;
    int steps = 0;

    if (!hModule) return FALSE;
    if (g_canonicalDllPath[0] == L'\0') return FALSE;

    // Портативно для x64 и ARM64, без __readgsqword напрямую
    peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    if (!peb || !peb->Ldr) {
        Wh_Log(L"Не удалось получить PEB/Ldr");
        return FALSE;
    }

    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        pLock = (LdrLockLoaderLock_FN)GetProcAddress(hNtdll, "LdrLockLoaderLock");
        pUnlock = (LdrUnlockLoaderLock_FN)GetProcAddress(hNtdll, "LdrUnlockLoaderLock");
        if (pLock && pUnlock) {
            if (pLock(0, NULL, &cookie) >= 0) locked = TRUE;
        }
    }

    head = &peb->Ldr->InMemoryOrderModuleList;
    cur = head->Flink;

    while (cur && cur != head && steps < 10000) {
        PLDR_DATA_TABLE_ENTRY_FULL cand =
            CONTAINING_RECORD(cur, LDR_DATA_TABLE_ENTRY_FULL, InMemoryOrderLinks);
        if ((HMODULE)cand->DllBase == hModule) {
            ent = cand;
            break;
        }
        cur = cur->Flink;
        steps++;
    }

    if (!ent) {
        if (locked) pUnlock(0, cookie);
        Wh_Log(L"Не найдена loader entry для hModule=%p", hModule);
        return FALSE;
    }

    fullLen = wcslen(g_canonicalDllPath);
    baseLen = wcslen(L"explorerframe.dll");

    if (ent->FullDllName.Buffer && ent->FullDllName.Length == (USHORT)(fullLen * sizeof(WCHAR))) {
        if (_wcsnicmp(ent->FullDllName.Buffer, g_canonicalDllPath, fullLen) == 0) {
            if (locked) pUnlock(0, cookie);
            return TRUE;
        }
    }

    fullBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (fullLen + 1) * sizeof(WCHAR));
    baseBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (baseLen + 1) * sizeof(WCHAR));
    if (!fullBuf || !baseBuf) {
        if (fullBuf) HeapFree(GetProcessHeap(), 0, fullBuf);
        if (baseBuf) HeapFree(GetProcessHeap(), 0, baseBuf);
        if (locked) pUnlock(0, cookie);
        Wh_Log(L"HeapAlloc failed");
        return FALSE;
    }

    StringCchCopyW(fullBuf, fullLen + 1, g_canonicalDllPath);
    StringCchCopyW(baseBuf, baseLen + 1, L"explorerframe.dll");

    ent->FullDllName.Buffer = fullBuf;
    ent->FullDllName.Length = (USHORT)(fullLen * sizeof(WCHAR));
    ent->FullDllName.MaximumLength = (USHORT)((fullLen + 1) * sizeof(WCHAR));

    ent->BaseDllName.Buffer = baseBuf;
    ent->BaseDllName.Length = (USHORT)(baseLen * sizeof(WCHAR));
    ent->BaseDllName.MaximumLength = (USHORT)((baseLen + 1) * sizeof(WCHAR));

    if (locked) pUnlock(0, cookie);

    Wh_Log(L"Loader entry замаскирован: %p -> %s", hModule, g_canonicalDllPath);
    return TRUE;
}

static BOOL HookGuardEnter(void) {
    if (g_tlsInsideHook == TLS_OUT_OF_INDEXES) return TRUE;
    if (TlsGetValue(g_tlsInsideHook) != NULL) return FALSE;
    TlsSetValue(g_tlsInsideHook, (LPVOID)(ULONG_PTR)1);
    return TRUE;
}

static void HookGuardLeave(void) {
    if (g_tlsInsideHook != TLS_OUT_OF_INDEXES)
        TlsSetValue(g_tlsInsideHook, NULL);
}

typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
typedef HMODULE (WINAPI *LoadLibraryExA_t)(LPCSTR, HANDLE, DWORD);
typedef NTSTATUS (NTAPI *LdrLoadDll_t)(PWSTR, PULONG, PUNICODE_STRING, PHANDLE);

static LoadLibraryExW_t LoadLibraryExW_Original = NULL;
static LoadLibraryW_t LoadLibraryW_Original = NULL;
static LoadLibraryExA_t LoadLibraryExA_Original = NULL;
static LdrLoadDll_t LdrLoadDll_Original = NULL;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE res = NULL;
    DWORD err = 0;

    if (!lpLibFileName || hFile != NULL)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!IsExplorerFrameByNameW(lpLibFileName))
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (g_stagingDllPath[0] != L'\0' && _wcsicmp(lpLibFileName, g_stagingDllPath) == 0)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!HookGuardEnter())
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    Wh_Log(L"[LoadLibraryExW] Перехват: %s -> %s", lpLibFileName, g_stagingDllPath);

    if (!EnsureStagingFileValid(TRUE)) {
        Wh_Log(L"[LoadLibraryExW] Без редиректа, хеш плохой");
        HookGuardLeave();
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }

    res = LoadLibraryExW_Original(g_stagingDllPath, hFile, dwFlags);
    err = GetLastError();
    HookGuardLeave();

    if (res) {
        PatchLoaderEntryPath(res);
        Wh_Log(L"[LoadLibraryExW] Загружено: %p", res);
        SetLastError(0);
    } else {
        Wh_Log(L"[LoadLibraryExW] Не удалось загрузить: %u", err);
        SetLastError(err);
    }
    return res;
}

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    HMODULE res = NULL;
    DWORD err = 0;

    if (!lpLibFileName)
        return LoadLibraryW_Original(lpLibFileName);
    if (!IsExplorerFrameByNameW(lpLibFileName))
        return LoadLibraryW_Original(lpLibFileName);
    if (g_stagingDllPath[0] != L'\0' && _wcsicmp(lpLibFileName, g_stagingDllPath) == 0)
        return LoadLibraryW_Original(lpLibFileName);
    if (!HookGuardEnter())
        return LoadLibraryW_Original(lpLibFileName);

    if (!EnsureStagingFileValid(TRUE)) {
        HookGuardLeave();
        return LoadLibraryW_Original(lpLibFileName);
    }

    res = LoadLibraryW_Original(g_stagingDllPath);
    err = GetLastError();
    HookGuardLeave();

    if (res) {
        PatchLoaderEntryPath(res);
        SetLastError(0);
    } else {
        SetLastError(err);
    }
    return res;
}

HMODULE WINAPI LoadLibraryExA_Hook(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (!lpLibFileName || hFile != NULL)
        return LoadLibraryExA_Original(lpLibFileName, hFile, dwFlags);
    if (!IsExplorerFrameByNameA(lpLibFileName))
        return LoadLibraryExA_Original(lpLibFileName, hFile, dwFlags);
    if (!HookGuardEnter())
        return LoadLibraryExA_Original(lpLibFileName, hFile, dwFlags);

    Wh_Log(L"[LoadLibraryExA] Перехват -> %s", g_stagingDllPath);

    if (!EnsureStagingFileValid(TRUE)) {
        HookGuardLeave();
        return LoadLibraryExA_Original(lpLibFileName, hFile, dwFlags);
    }

    // Грузим по W-пути, чтобы не зависеть от ANSI кодовой страницы
    HMODULE res = LoadLibraryExW_Original(g_stagingDllPath, hFile, dwFlags);
    DWORD err = GetLastError();
    HookGuardLeave();

    if (res) {
        PatchLoaderEntryPath(res);
        SetLastError(0);
    } else {
        SetLastError(err);
    }
    return res;
}

NTSTATUS NTAPI LdrLoadDll_Hook(PWSTR SearchPath, PULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle) {
    WCHAR redirBuf[MAX_PATH];
    UNICODE_STRING redir;
    NTSTATUS st;
    BOOL needRedirect = FALSE;

    if (!ModuleFileName || !ModuleFileName->Buffer || !ModuleHandle)
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    // Быстрая проверка имени без аллокаций
    {
        PCWSTR n = ModuleFileName->Buffer;
        size_t len = ModuleFileName->Length / sizeof(WCHAR);
        // ищем хвост explorerframe.dll
        if (len >= 17) {
            PCWSTR tail = n + len - 17;
            WCHAR tmp[18];
            for (int i = 0; i < 17; i++) tmp[i] = tail[i];
            tmp[17] = L'\0';
            if (_wcsicmp(tmp, L"explorerframe.dll") == 0)
                needRedirect = TRUE;
        }
    }

    if (!needRedirect)
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    if (g_stagingDllPath[0] != L'\0') {
        // уже наш путь - не редиректим повторно
        size_t curLen = ModuleFileName->Length / sizeof(WCHAR);
        size_t ourLen = wcslen(g_stagingDllPath);
        if (curLen == ourLen && _wcsnicmp(ModuleFileName->Buffer, g_stagingDllPath, ourLen) == 0)
            return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);
    }

    if (!HookGuardEnter())
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    if (!EnsureStagingFileValid(TRUE)) {
        HookGuardLeave();
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);
    }

    StringCchCopyW(redirBuf, MAX_PATH, g_stagingDllPath);
    redir.Buffer = redirBuf;
    redir.Length = (USHORT)(wcslen(redirBuf) * sizeof(WCHAR));
    redir.MaximumLength = (USHORT)((wcslen(redirBuf) + 1) * sizeof(WCHAR));

    Wh_Log(L"[LdrLoadDll] Перехват -> %s", redirBuf);

    st = LdrLoadDll_Original(SearchPath, Flags, &redir, ModuleHandle);
    HookGuardLeave();

    if (st >= 0 && ModuleHandle && *ModuleHandle) {
        PatchLoaderEntryPath((HMODULE)*ModuleHandle);
    }
    return st;
}

BOOL Wh_ModInit(void) {
    HMODULE hKernelBase;
    HMODULE hNtdll;

    Wh_Log(L"Инициализация ExplorerFrame Redirector");

    InitializeSRWLock(&g_downloadLock);
    g_tlsInsideHook = TlsAlloc();

    SelectDownloadSourceByProcessArch();
    g_fileReady = PrepareExplorerFrameDll();
    if (!g_fileReady) {
        Wh_Log(L"Файл не готов на старте, хук ставим для повторной попытки");
    }

    hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernelBase) hKernelBase = GetModuleHandleW(L"kernel32.dll");
    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hKernelBase || !hNtdll) {
        Wh_Log(L"Не удалось получить kernelbase/ntdll");
        return FALSE;
    }

    FARPROC pExW = GetProcAddress(hKernelBase, "LoadLibraryExW");
    FARPROC pW = GetProcAddress(hKernelBase, "LoadLibraryW");
    FARPROC pExA = GetProcAddress(hKernelBase, "LoadLibraryExA");
    FARPROC pLdr = GetProcAddress(hNtdll, "LdrLoadDll");

    if (pExW)
        Wh_SetFunctionHook((void*)pExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    if (pW)
        Wh_SetFunctionHook((void*)pW, (void*)LoadLibraryW_Hook, (void**)&LoadLibraryW_Original);
    if (pExA)
        Wh_SetFunctionHook((void*)pExA, (void*)LoadLibraryExA_Hook, (void**)&LoadLibraryExA_Original);
    if (pLdr)
        Wh_SetFunctionHook((void*)pLdr, (void*)LdrLoadDll_Hook, (void**)&LdrLoadDll_Original);

    Wh_Log(L"Все хуки установлены, subdir=%s ready=%d", g_archSubdir ? g_archSubdir : L"?", g_fileReady);
    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_tlsInsideHook != TLS_OUT_OF_INDEXES) {
        TlsFree(g_tlsInsideHook);
        g_tlsInsideHook = TLS_OUT_OF_INDEXES;
    }
}

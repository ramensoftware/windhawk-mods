// ==WindhawkMod==
// @id              classic-explorerframe
// @name            Classic Explorerframe for 24H2 and later
// @description     Restores classic Explorerframe.dll on Win11 24H2 and later (x86-64 and ARM64, SHA256 verified)
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
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <winternl.h>
#include <wininet.h>
#include <shlobj.h>
#include <strsafe.h>
#include <bcrypt.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#ifndef IMAGE_FILE_MACHINE_AMD64
#define IMAGE_FILE_MACHINE_AMD64 0x8664
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xAA64
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64EC
#define IMAGE_FILE_MACHINE_ARM64EC 0xA641
#endif
#ifndef IMAGE_FILE_MACHINE_ARM64X
#define IMAGE_FILE_MACHINE_ARM64X 0xA64E
#endif

#ifndef BCRYPT_SHA256_ALGORITHM
#define BCRYPT_SHA256_ALGORITHM L"SHA256"
#endif
#ifndef BCRYPT_SUCCESS
#define BCRYPT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

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

WCHAR g_stagingDllPath[MAX_PATH];
WCHAR g_canonicalDllPath[MAX_PATH];
volatile BOOL g_fileReady = FALSE;
volatile BOOL g_insideHook = FALSE;
volatile BOOL g_isArm64 = FALSE;

static const WCHAR* DOWNLOAD_URL_X64 =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/3691089927d000/explorerframe.dll";
static const WCHAR* DOWNLOAD_URL_ARM64 =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/1187AEC054e000/explorerframe.dll";

static const WCHAR* EXPECTED_SHA256_X64 =
    L"22689659425c7cb765a2d908e84a075482d4ca8abc0e9261c2d6b07bf71d89ad";
static const WCHAR* EXPECTED_SHA256_ARM64 =
    L"7f76b0e033e7c9ae01acdade97f8f857bb205dabb1f1aff59dbb489792454ea8";

// Архитектура текущего процесса explorer.exe, проверка во время выполнения.
static USHORT GetCurrentProcessMachine(void) {
    HMODULE hSelf = GetModuleHandleW(NULL);
    if (hSelf) {
        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)hSelf;
        if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
            LONG lfanew = dos->e_lfanew;
            if (lfanew >= 0 && lfanew < 0x100000) {
                PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)hSelf + lfanew);
                if (nt->Signature == IMAGE_NT_SIGNATURE) {
                    return nt->FileHeader.Machine;
                }
            }
        }
    }

    {
        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (hKernel32) {
            typedef BOOL (WINAPI *IsWow64Process2_t)(HANDLE, USHORT*, USHORT*);
            IsWow64Process2_t pIsWow64Process2 =
                (IsWow64Process2_t)GetProcAddress(hKernel32, "IsWow64Process2");
            if (pIsWow64Process2) {
                USHORT procMachine = 0, nativeMachine = 0;
                if (pIsWow64Process2(GetCurrentProcess(), &procMachine, &nativeMachine)) {
                    if (procMachine != 0)
                        return procMachine;
                    if (nativeMachine != 0)
                        return nativeMachine;
                }
            }
        }
    }

#if defined(_M_ARM64) || defined(__aarch64__)
    return IMAGE_FILE_MACHINE_ARM64;
#else
    return IMAGE_FILE_MACHINE_AMD64;
#endif
}

static BOOL IsCurrentProcessArm64(void) {
    USHORT m = GetCurrentProcessMachine();
    return (m == IMAGE_FILE_MACHINE_ARM64 ||
            m == IMAGE_FILE_MACHINE_ARM64EC ||
            m == IMAGE_FILE_MACHINE_ARM64X);
}

static const WCHAR* CurrentUrl(void) {
    return g_isArm64 ? DOWNLOAD_URL_ARM64 : DOWNLOAD_URL_X64;
}

static const WCHAR* CurrentExpectedHash(void) {
    return g_isArm64 ? EXPECTED_SHA256_ARM64 : EXPECTED_SHA256_X64;
}

static BOOL HexCharToNibble(WCHAR c, BYTE* out) {
    if (c >= L'0' && c <= L'9') { *out = (BYTE)(c - L'0'); return TRUE; }
    if (c >= L'a' && c <= L'f') { *out = (BYTE)(c - L'a' + 10); return TRUE; }
    if (c >= L'A' && c <= L'F') { *out = (BYTE)(c - L'A' + 10); return TRUE; }
    return FALSE;
}

static BOOL ComputeFileSHA256(const WCHAR* filePath, BYTE outHash[32]) {
    HANDLE hFile = CreateFileW(filePath, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    BOOL bResult = FALSE;
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;

    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
        CloseHandle(hFile);
        return FALSE;
    }
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        CloseHandle(hFile);
        return FALSE;
    }

    BYTE buf[8192];
    DWORD dwRead = 0;
    BOOL bHashOk = TRUE;

    while (ReadFile(hFile, buf, sizeof(buf), &dwRead, NULL) && dwRead != 0) {
        if (!BCRYPT_SUCCESS(BCryptHashData(hHash, buf, dwRead, 0))) {
            bHashOk = FALSE;
            break;
        }
    }

    if (bHashOk) {
        if (BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash, 32, 0))) {
            bResult = TRUE;
        }
    }

    if (hHash) BCryptDestroyHash(hHash);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    CloseHandle(hFile);
    return bResult;
}

static BOOL VerifyFileSHA256(const WCHAR* path, const WCHAR* expectedHex) {
    BYTE hash[32];
    size_t i;

    if (!path || !expectedHex)
        return FALSE;
    if (wcslen(expectedHex) != 64)
        return FALSE;
    if (!ComputeFileSHA256(path, hash))
        return FALSE;

    for (i = 0; i < 32; i++) {
        BYTE hi, lo;
        if (!HexCharToNibble(expectedHex[i * 2], &hi))
            return FALSE;
        if (!HexCharToNibble(expectedHex[i * 2 + 1], &lo))
            return FALSE;
        if (hash[i] != (BYTE)((hi << 4) | lo))
            return FALSE;
    }
    return TRUE;
}

static BOOL EnsureDirectoryExists(const WCHAR* filePath) {
    WCHAR dirPath[MAX_PATH];
    WCHAR* lastSlash;
    int result;

    if (FAILED(StringCchCopyW(dirPath, MAX_PATH, filePath)))
        return FALSE;
    lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    result = SHCreateDirectoryExW(NULL, dirPath, NULL);
    return (result == ERROR_SUCCESS || result == ERROR_ALREADY_EXISTS || result == ERROR_FILE_EXISTS);
}

static BOOL DownloadFile(const WCHAR* url, const WCHAR* destPath) {
    HINTERNET hInternet;
    HINTERNET hUrl;
    HANDLE hFile;
    BYTE buffer[8192];
    DWORD bytesRead = 0, bytesWritten = 0, totalBytes = 0;
    BOOL success = TRUE;

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

// Проверка с заменой файла при неверной сумме. Вызывается при старте и перед загрузкой.
static BOOL EnsureStagingFileValid(void) {
    if (VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash()))
        return TRUE;

    Wh_Log(L"Хеш %s не совпал, перекачиваю...", g_stagingDllPath);
    DeleteFileW(g_stagingDllPath);

    if (!EnsureDirectoryExists(g_stagingDllPath)) {
        Wh_Log(L"Не удалось создать директорию");
        return FALSE;
    }
    if (!DownloadFile(CurrentUrl(), g_stagingDllPath))
        return FALSE;

    if (!VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash())) {
        Wh_Log(L"Хеш после загрузки не совпал! Удаляю.");
        DeleteFileW(g_stagingDllPath);
        return FALSE;
    }

    Wh_Log(L"Файл перекачан и проверен: %s", g_stagingDllPath);
    return TRUE;
}

static BOOL PrepareExplorerFrameDll(void) {
    WCHAR storageDir[MAX_PATH];
    WCHAR systemDir[MAX_PATH];
    HRESULT hr;
    DWORD attrs;
    const WCHAR* leaf;

    // На ARM64 ОС explorer.exe может быть эмулированным x86-64,
    // поэтому смотрим именно процесс, а не сборку мода.
    g_isArm64 = IsCurrentProcessArm64();
    Wh_Log(L"Архитектура текущего процесса: %s (Machine=0x%04X)",
        g_isArm64 ? L"ARM64" : L"x86-64", GetCurrentProcessMachine());

    memset(storageDir, 0, sizeof(storageDir));
    {
        size_t len = Wh_GetModStoragePath(storageDir, ARRAYSIZE(storageDir));
        if (len == 0 || len >= ARRAYSIZE(storageDir)) {
            Wh_Log(L"Wh_GetModStoragePath failed");
            return FALSE;
        }
    }

    leaf = g_isArm64 ? L"explorerframe_arm64.dll" : L"explorerframe_x64.dll";
    hr = StringCchPrintfW(g_stagingDllPath, MAX_PATH, L"%s\\%s", storageDir, leaf);
    if (FAILED(hr)) return FALSE;

    GetSystemDirectoryW(systemDir, MAX_PATH);
    StringCchPrintfW(g_canonicalDllPath, MAX_PATH, L"%s\\explorerframe.dll", systemDir);

    if (!EnsureDirectoryExists(g_stagingDllPath)) {
        Wh_Log(L"Не удалось создать директорию");
        return FALSE;
    }

    attrs = GetFileAttributesW(g_stagingDllPath);
    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        if (VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash())) {
            Wh_Log(L"Файл уже на месте и хеш совпадает: %s", g_stagingDllPath);
            return TRUE;
        }
        Wh_Log(L"Хеш существующего файла неверный, заменяю");
        DeleteFileW(g_stagingDllPath);
    }

    if (!DownloadFile(CurrentUrl(), g_stagingDllPath)) {
        Wh_Log(L"Ошибка загрузки");
        return FALSE;
    }

    if (!VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash())) {
        Wh_Log(L"Хеш после загрузки неверный, удаляю");
        DeleteFileW(g_stagingDllPath);
        return FALSE;
    }

    Wh_Log(L"Файл сохранён и проверен: %s", g_stagingDllPath);
    return TRUE;
}

static BOOL IsExplorerFrameByName(PCWSTR name) {
    const WCHAR* fileName;
    if (!name) return FALSE;
    fileName = wcsrchr(name, L'\\');
    fileName = fileName ? fileName + 1 : name;
    return _wcsicmp(fileName, L"explorerframe.dll") == 0;
}

// PEB через NtQueryInformationProcess: один код для x64 и ARM64.
static BOOL PatchLoaderEntryPath(HMODULE hModule) {
    HMODULE hNtdll;
    typedef NTSTATUS (NTAPI *NtQueryInformationProcess_t)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    NtQueryInformationProcess_t pNtQIP;
    PROCESS_BASIC_INFORMATION pbi;
    NTSTATUS st;
    PPEB peb;
    PLIST_ENTRY head, current;
    int i;

    size_t fullLen, baseLen;
    PWSTR fullBuf, baseBuf;

    if (!hModule)
        return FALSE;

    hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll)
        return FALSE;

    pNtQIP = (NtQueryInformationProcess_t)GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!pNtQIP)
        return FALSE;

    memset(&pbi, 0, sizeof(pbi));
    st = pNtQIP(GetCurrentProcess(), ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
    if (st != 0 || !pbi.PebBaseAddress) {
        Wh_Log(L"NtQueryInformationProcess failed: 0x%08X", (unsigned)st);
        return FALSE;
    }

    peb = pbi.PebBaseAddress;
    if (!peb || !peb->Ldr) {
        Wh_Log(L"Не удалось получить PEB/Ldr");
        return FALSE;
    }

    head = &peb->Ldr->InMemoryOrderModuleList;
    current = head->Flink;

    for (i = 0; i < 4096 && current && current != head; i++) {
        PLDR_DATA_TABLE_ENTRY_FULL entry =
            CONTAINING_RECORD(current, LDR_DATA_TABLE_ENTRY_FULL, InMemoryOrderLinks);

        if (entry->DllBase == (PVOID)hModule) {
            fullLen = wcslen(g_canonicalDllPath);
            baseLen = wcslen(L"explorerframe.dll");

            fullBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (fullLen + 1) * sizeof(WCHAR));
            baseBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (baseLen + 1) * sizeof(WCHAR));

            if (!fullBuf || !baseBuf) {
                Wh_Log(L"HeapAlloc failed");
                if (fullBuf) HeapFree(GetProcessHeap(), 0, fullBuf);
                if (baseBuf) HeapFree(GetProcessHeap(), 0, baseBuf);
                return FALSE;
            }

            wcscpy_s(fullBuf, fullLen + 1, g_canonicalDllPath);
            wcscpy_s(baseBuf, baseLen + 1, L"explorerframe.dll");

            entry->FullDllName.Buffer = fullBuf;
            entry->FullDllName.Length = (USHORT)(fullLen * sizeof(WCHAR));
            entry->FullDllName.MaximumLength = (USHORT)((fullLen + 1) * sizeof(WCHAR));

            entry->BaseDllName.Buffer = baseBuf;
            entry->BaseDllName.Length = (USHORT)(baseLen * sizeof(WCHAR));
            entry->BaseDllName.MaximumLength = (USHORT)((baseLen + 1) * sizeof(WCHAR));

            Wh_Log(L"Loader entry замаскирован: %p -> %s", hModule, g_canonicalDllPath);
            return TRUE;
        }

        current = current->Flink;
    }

    Wh_Log(L"Не найдена loader entry для hModule=%p", hModule);
    return FALSE;
}

typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original = NULL;

typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
LoadLibraryW_t LoadLibraryW_Original = NULL;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE result;

    if (g_insideHook || !g_fileReady || !lpLibFileName) {
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }

    if (!IsExplorerFrameByName(lpLibFileName)) {
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }

    Wh_Log(L"[LoadLibraryExW] Перехват: %s -> %s", lpLibFileName, g_stagingDllPath);

    if (!VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash())) {
        BOOL ok;
        Wh_Log(L"[LoadLibraryExW] Хеш неверный перед загрузкой");
        g_insideHook = TRUE;
        ok = EnsureStagingFileValid();
        g_insideHook = FALSE;
        if (!ok) {
            Wh_Log(L"[LoadLibraryExW] Не удалось восстановить файл, гружу оригинал");
            return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
        }
    }

    g_insideHook = TRUE;
    result = LoadLibraryExW_Original(g_stagingDllPath, hFile, dwFlags);
    g_insideHook = FALSE;

    if (result) {
        PatchLoaderEntryPath(result);
        Wh_Log(L"[LoadLibraryExW] Загружено и замаскировано: %p", result);
    } else {
        Wh_Log(L"[LoadLibraryExW] Не удалось загрузить: %u", GetLastError());
    }

    return result;
}

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    HMODULE result;

    if (g_insideHook || !g_fileReady || !lpLibFileName) {
        return LoadLibraryW_Original(lpLibFileName);
    }

    if (!IsExplorerFrameByName(lpLibFileName)) {
        return LoadLibraryW_Original(lpLibFileName);
    }

    Wh_Log(L"[LoadLibraryW] Перехват: %s -> %s", lpLibFileName, g_stagingDllPath);

    if (!VerifyFileSHA256(g_stagingDllPath, CurrentExpectedHash())) {
        BOOL ok;
        Wh_Log(L"[LoadLibraryW] Хеш неверный перед загрузкой");
        g_insideHook = TRUE;
        ok = EnsureStagingFileValid();
        g_insideHook = FALSE;
        if (!ok) {
            Wh_Log(L"[LoadLibraryW] Не удалось восстановить файл, гружу оригинал");
            return LoadLibraryW_Original(lpLibFileName);
        }
    }

    g_insideHook = TRUE;
    result = LoadLibraryW_Original(g_stagingDllPath);
    g_insideHook = FALSE;

    if (result) {
        PatchLoaderEntryPath(result);
        Wh_Log(L"[LoadLibraryW] Загружено и замаскировано: %p", result);
    } else {
        Wh_Log(L"[LoadLibraryW] Не удалось загрузить: %u", GetLastError());
    }

    return result;
}

BOOL Wh_ModInit(void) {
    HMODULE hKernelBase;
    HMODULE hKernel32;
    FARPROC pLoadLibraryExW;
    FARPROC pLoadLibraryW;

    Wh_Log(L"Инициализация ExplorerFrame Redirector v7.1 (x64/ARM64 runtime)");

    g_fileReady = PrepareExplorerFrameDll();
    if (!g_fileReady) {
        Wh_Log(L"Файл не готов, редирект отключён");
        return FALSE;
    }

    hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernelBase)
        hKernelBase = hKernel32;

    if (!hKernelBase || !hKernel32) {
        Wh_Log(L"Не удалось получить kernelbase/kernel32");
        return FALSE;
    }

    pLoadLibraryExW = GetProcAddress(hKernelBase, "LoadLibraryExW");
    if (pLoadLibraryExW) {
        Wh_SetFunctionHook((void*)pLoadLibraryExW, (void*)LoadLibraryExW_Hook,
            (void**)&LoadLibraryExW_Original);
        Wh_Log(L"LoadLibraryExW хук установлен");
    }

    pLoadLibraryW = GetProcAddress(hKernel32, "LoadLibraryW");
    if (pLoadLibraryW) {
        Wh_SetFunctionHook((void*)pLoadLibraryW, (void*)LoadLibraryW_Hook,
            (void**)&LoadLibraryW_Original);
        Wh_Log(L"LoadLibraryW хук установлен");
    }

    Wh_Log(L"Все хуки установлены");
    return TRUE;
}

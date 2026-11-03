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

// compile-time выбор, а не рантайм машины/процесса.
// Мод собирается под архитектуру процесса, в чужой процесс его dll не загрузить.
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

static WCHAR g_stagingDllPath[MAX_PATH] = {0};
static WCHAR g_tempDllPath[MAX_PATH + 4] = {0};
static WCHAR g_canonicalDllPath[MAX_PATH] = {0};

// Пин загружаемого файла: открыт с FILE_SHARE_READ без WRITE/DELETE
// от проверки в ModInit/worker до Uninit. Пока хэндл жив, файл нельзя
// подменить/усечь между проверкой и LoadLibrary.
static HANDLE g_hPinnedFile = INVALID_HANDLE_VALUE;
static LONG g_ready = 0;

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
        if (g_hStopEvent && WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
            // не отменяем хеш локального файла жестко, но дальше загрузка не нужна
            break;
        }
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

// Открыть staging только на чтение без SHARE_WRITE/DELETE, прохэшировать
// через этот же хэндл и оставить хэндл открытым.
static BOOL TryPinExistingFile(void) {
    if (InterlockedCompareExchange(&g_ready, 0, 0) != 0)
        return TRUE;
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
    InterlockedExchange(&g_ready, 1);
    Wh_Log(L"Pinned: %s", g_stagingDllPath);
    return TRUE;
}

static BOOL IsStopRequested(void) {
    return g_hStopEvent && WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0;
}

// Качалка только для worker-thread. Пишет в tmp, проверяет, потом rename.
static BOOL DownloadToTempCancellable(void) {
    if (!kDownloadUrl || g_tempDllPath[0] == L'\0')
        return FALSE;

    HINTERNET hInet = InternetOpenW(L"WindhawkMod/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInet) return FALSE;

    AcquireSRWLockExclusive(&g_cancelLock);
    if (IsStopRequested()) {
        ReleaseSRWLockExclusive(&g_cancelLock);
        InternetCloseHandle(hInet);
        return FALSE;
    }
    g_hInetCancel = hInet;
    ReleaseSRWLockExclusive(&g_cancelLock);

    HINTERNET hUrl = InternetOpenUrlW(hInet, kDownloadUrl, NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) {
        AcquireSRWLockExclusive(&g_cancelLock);
        if (g_hInetCancel == hInet) g_hInetCancel = NULL;
        ReleaseSRWLockExclusive(&g_cancelLock);
        InternetCloseHandle(hInet);
        return FALSE;
    }

    AcquireSRWLockExclusive(&g_cancelLock);
    g_hUrlCancel = hUrl;
    ReleaseSRWLockExclusive(&g_cancelLock);

    HANDLE hFile = CreateFileW(g_tempDllPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        AcquireSRWLockExclusive(&g_cancelLock);
        if (g_hUrlCancel == hUrl) g_hUrlCancel = NULL;
        if (g_hInetCancel == hInet) g_hInetCancel = NULL;
        ReleaseSRWLockExclusive(&g_cancelLock);
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInet);
        return FALSE;
    }

    BYTE buf[8192];
    DWORD br = 0, bw = 0;
    DWORD total = 0;
    BOOL ok = TRUE;

    for (;;) {
        if (IsStopRequested()) { ok = FALSE; break; }
        BOOL r = InternetReadFile(hUrl, buf, sizeof(buf), &br);
        if (!r) { ok = FALSE; break; } // сюда же приходим при cancel через CloseHandle
        if (br == 0) break;
        if (!WriteFile(hFile, buf, br, &bw, NULL) || bw != br) { ok = FALSE; break; }
        total += br;
    }
    CloseHandle(hFile);

    // снять публикацию без double-close
    AcquireSRWLockExclusive(&g_cancelLock);
    HINTERNET toCloseUrl = NULL, toCloseInet = NULL;
    if (g_hUrlCancel == hUrl) { g_hUrlCancel = NULL; toCloseUrl = hUrl; } else { hUrl = NULL; }
    if (g_hInetCancel == hInet) { g_hInetCancel = NULL; toCloseInet = hInet; } else { hInet = NULL; }
    ReleaseSRWLockExclusive(&g_cancelLock);
    if (toCloseUrl) InternetCloseHandle(toCloseUrl);
    if (toCloseInet) InternetCloseHandle(toCloseInet);

    if (!ok || total == 0) {
        DeleteFileW(g_tempDllPath);
        return FALSE;
    }

    // Проверка после скачивания из интернета, tmp еще не имеет финального имени
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

    // Только после verify отдаем финальное имя
    if (!MoveFileWithProgressW(g_tempDllPath, g_stagingDllPath, NULL, NULL, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        // fallback на MoveFileEx
        if (!MoveFileExW(g_tempDllPath, g_stagingDllPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            DeleteFileW(g_tempDllPath);
            return FALSE;
        }
    }
    Wh_Log(L"Скачано %u -> %s", total, g_stagingDllPath);
    return TRUE;
}

static DWORD WINAPI DownloadWorkerProc(LPVOID) {
    if (IsStopRequested()) return 1;
    // файл уже могли дописать/починить параллельно - сначала пин
    if (TryPinExistingFile())
        return 0;
    if (IsStopRequested()) return 1;
    if (!DownloadToTempCancellable())
        return 1;
    // после rename пиним финальный файл
    TryPinExistingFile();
    return 0;
}

static BOOL IsExplorerFrameByNameW(PCWSTR name) {
    const WCHAR* f;
    if (!name) return FALSE;
    f = wcsrchr(name, L'\\');
    f = f ? f + 1 : name;
    return _wcsicmp(f, L"explorerframe.dll") == 0;
}

static BOOL PatchLoaderEntryPath(HMODULE hModule) {
    if (!hModule || g_canonicalDllPath[0] == L'\0')
        return FALSE;

    PPEB peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    if (!peb || !peb->Ldr)
        return FALSE;

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    typedef NTSTATUS (NTAPI *LockFN)(ULONG, ULONG*, ULONG_PTR*);
    typedef NTSTATUS (NTAPI *UnlockFN)(ULONG, ULONG_PTR);
    LockFN pLock = NULL;
    UnlockFN pUnlock = NULL;
    ULONG_PTR cookie = 0;
    BOOL locked = FALSE;
    if (hNtdll) {
        pLock = (LockFN)GetProcAddress(hNtdll, "LdrLockLoaderLock");
        pUnlock = (UnlockFN)GetProcAddress(hNtdll, "LdrUnlockLoaderLock");
        if (pLock && pUnlock && pLock(0, NULL, &cookie) >= 0)
            locked = TRUE;
    }

    PLIST_ENTRY head = &peb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY cur = head->Flink;
    PLDR_DATA_TABLE_ENTRY_FULL ent = NULL;
    int steps = 0;
    while (cur && cur != head && steps < 10000) {
        PLDR_DATA_TABLE_ENTRY_FULL cand =
            CONTAINING_RECORD(cur, LDR_DATA_TABLE_ENTRY_FULL, InMemoryOrderLinks);
        if ((HMODULE)cand->DllBase == hModule) { ent = cand; break; }
        cur = cur->Flink;
        steps++;
    }
    if (!ent) {
        if (locked) pUnlock(0, cookie);
        return FALSE;
    }

    size_t fullLen = wcslen(g_canonicalDllPath);
    size_t baseLen = wcslen(L"explorerframe.dll");
    if (ent->FullDllName.Buffer && ent->FullDllName.Length == (USHORT)(fullLen * sizeof(WCHAR)) &&
        _wcsnicmp(ent->FullDllName.Buffer, g_canonicalDllPath, fullLen) == 0) {
        if (locked) pUnlock(0, cookie);
        return TRUE;
    }

    PWSTR fullBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (fullLen + 1) * sizeof(WCHAR));
    PWSTR baseBuf = (PWSTR)HeapAlloc(GetProcessHeap(), 0, (baseLen + 1) * sizeof(WCHAR));
    if (!fullBuf || !baseBuf) {
        if (fullBuf) HeapFree(GetProcessHeap(), 0, fullBuf);
        if (baseBuf) HeapFree(GetProcessHeap(), 0, baseBuf);
        if (locked) pUnlock(0, cookie);
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
    return TRUE;
}

typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef NTSTATUS (NTAPI *LdrLoadDll_t)(PWSTR, PULONG, PUNICODE_STRING, PHANDLE);
static LoadLibraryExW_t LoadLibraryExW_Original = NULL;
static LdrLoadDll_t LdrLoadDll_Original = NULL;

static BOOL EnterHook(void) {
    if (t_inHook) return FALSE;
    t_inHook = true;
    return TRUE;
}
static void LeaveHook(void) { t_inHook = false; }
static BOOL IsReady(void) {
    return InterlockedCompareExchange(&g_ready, 0, 0) != 0 &&
           g_hPinnedFile != INVALID_HANDLE_VALUE &&
           g_stagingDllPath[0] != L'\0';
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (!lpLibFileName || hFile != NULL)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!IsExplorerFrameByNameW(lpLibFileName))
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (g_stagingDllPath[0] != L'\0' && _wcsicmp(lpLibFileName, g_stagingDllPath) == 0)
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    // Хуки никогда не качают и не хешируют: пока не готов - сквозной проход
    if (!IsReady())
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!EnterHook())
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    HMODULE res = LoadLibraryExW_Original(g_stagingDllPath, hFile, dwFlags);
    DWORD err = GetLastError();
    LeaveHook();
    if (res) {
        PatchLoaderEntryPath(res);
        SetLastError(0);
    } else {
        SetLastError(err);
    }
    return res;
}

NTSTATUS NTAPI LdrLoadDll_Hook(PWSTR SearchPath, PULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle) {
    if (!ModuleFileName || !ModuleFileName->Buffer || !ModuleHandle)
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    size_t len = ModuleFileName->Length / sizeof(WCHAR);
    BOOL need = FALSE;
    if (len >= 17) {
        PCWSTR tail = ModuleFileName->Buffer + len - 17;
        WCHAR tmp[18];
        for (int i = 0; i < 17; i++) tmp[i] = tail[i];
        tmp[17] = L'\0';
        if (_wcsicmp(tmp, L"explorerframe.dll") == 0) need = TRUE;
    }
    if (!need)
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);
    if (!IsReady())
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    {
        size_t curLen = len;
        size_t ourLen = wcslen(g_stagingDllPath);
        if (curLen == ourLen && _wcsnicmp(ModuleFileName->Buffer, g_stagingDllPath, ourLen) == 0)
            return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);
    }
    if (!EnterHook())
        return LdrLoadDll_Original(SearchPath, Flags, ModuleFileName, ModuleHandle);

    WCHAR redirBuf[MAX_PATH];
    StringCchCopyW(redirBuf, MAX_PATH, g_stagingDllPath);
    UNICODE_STRING redir;
    redir.Buffer = redirBuf;
    redir.Length = (USHORT)(wcslen(redirBuf) * sizeof(WCHAR));
    redir.MaximumLength = (USHORT)((wcslen(redirBuf) + 1) * sizeof(WCHAR));

    NTSTATUS st = LdrLoadDll_Original(SearchPath, Flags, &redir, ModuleHandle);
    LeaveHook();
    if (st >= 0 && ModuleHandle && *ModuleHandle)
        PatchLoaderEntryPath((HMODULE)*ModuleHandle);
    return st;
}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init redirector subdir=%s", kArchSubdir);

    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) return FALSE;

    WCHAR modDir[MAX_PATH];
    modDir[0] = L'\0';
    size_t r = Wh_GetModStoragePath(modDir, sizeof(modDir) / sizeof(modDir[0]));
    if (r == 0 || modDir[0] == L'\0') {
        Wh_Log(L"Wh_GetModStoragePath failed");
        return FALSE;
    }

    // x64 -> <storage>\x64\explorerframe.dll, ARM64 -> <storage>\ARM64\explorerframe.dll
    if (FAILED(StringCchPrintfW(g_stagingDllPath, MAX_PATH, L"%s\\%s\\explorerframe.dll", modDir, kArchSubdir)))
        return FALSE;
    if (FAILED(StringCchPrintfW(g_tempDllPath, MAX_PATH + 4, L"%s.tmp", g_stagingDllPath)))
        return FALSE;

    WCHAR sysDir[MAX_PATH];
    GetSystemDirectoryW(sysDir, MAX_PATH);
    StringCchPrintfW(g_canonicalDllPath, MAX_PATH, L"%s\\explorerframe.dll", sysDir);

    EnsureDirectoryExists(g_stagingDllPath);

    // Одна проверка в init, без сети
    if (!TryPinExistingFile()) {
        // битый - удалить, дальше worker перезапишет через tmp+rename
        DeleteFileW(g_stagingDllPath);
        Wh_Log(L"Файл не готов, качаем в фоне для следующих загрузок");
        g_hWorkerThread = CreateThread(NULL, 0, DownloadWorkerProc, NULL, 0, NULL);
        // даже если поток не создался - хуки все равно ставим в pass-through
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hKernelBase || !hNtdll) return FALSE;

    FARPROC pExW = GetProcAddress(hKernelBase, "LoadLibraryExW");
    FARPROC pLdr = GetProcAddress(hNtdll, "LdrLoadDll");
    if (pExW) Wh_SetFunctionHook((void*)pExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    if (pLdr) Wh_SetFunctionHook((void*)pLdr, (void*)LdrLoadDll_Hook, (void**)&LdrLoadDll_Original);

    // TRUE даже когда не готов: хуки в pass-through до конца сессии/следующего старта
    return TRUE;
}

void Wh_ModUninit(void) {
    if (g_hStopEvent) SetEvent(g_hStopEvent);

    // сорвать блокирующий WinInet в worker
    AcquireSRWLockExclusive(&g_cancelLock);
    HINTERNET hu = g_hUrlCancel;
    HINTERNET hi = g_hInetCancel;
    g_hUrlCancel = NULL;
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
    InterlockedExchange(&g_ready, 0);
}

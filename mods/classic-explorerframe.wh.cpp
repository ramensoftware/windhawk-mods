// ==WindhawkMod==
// @id              classic-explorerframe
// @name            Classic Explorerframe for 24H2 and later
// @description     Restores classic Explorerframe.dll on Win11 24H2 and later
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lwininet -lshell32 -lntdll
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

const WCHAR* DOWNLOAD_URL =
    L"https://msdl.microsoft.com/download/symbols/explorerframe.dll/3691089927d000/explorerframe.dll";

BOOL EnsureDirectoryExists(const WCHAR* filePath) {
    WCHAR dirPath[MAX_PATH];
    WCHAR* lastSlash;
    int result;

    StringCchCopyW(dirPath, MAX_PATH, filePath);
    lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    result = SHCreateDirectoryExW(NULL, dirPath, NULL);
    return (result == ERROR_SUCCESS || result == ERROR_ALREADY_EXISTS || result == ERROR_FILE_EXISTS);
}

BOOL DownloadFile(const WCHAR* url, const WCHAR* destPath) {
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

BOOL PrepareExplorerFrameDll() {
    WCHAR programData[MAX_PATH];
    WCHAR systemDir[MAX_PATH];
    DWORD attrs;
    HRESULT hr;

    if (!GetEnvironmentVariableW(L"ProgramData", programData, MAX_PATH)) {
        Wh_Log(L"Не удалось получить ProgramData");
        return FALSE;
    }

    hr = StringCchPrintfW(
        g_stagingDllPath, MAX_PATH,
        L"%s\\Windhawk\\Engine\\ModsWritable\\win10-taskbar-on-win11-24h2\\explorerframe.dll",
        programData
    );
    if (FAILED(hr)) return FALSE;

    GetSystemDirectoryW(systemDir, MAX_PATH);
    StringCchPrintfW(g_canonicalDllPath, MAX_PATH, L"%s\\explorerframe.dll", systemDir);

    attrs = GetFileAttributesW(g_stagingDllPath);
    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"Файл уже на месте: %s", g_stagingDllPath);
        return TRUE;
    }

    if (!EnsureDirectoryExists(g_stagingDllPath)) {
        Wh_Log(L"Не удалось создать директорию");
        return FALSE;
    }
    if (!DownloadFile(DOWNLOAD_URL, g_stagingDllPath)) {
        Wh_Log(L"Ошибка загрузки");
        return FALSE;
    }

    Wh_Log(L"Файл сохранён: %s", g_stagingDllPath);
    return TRUE;
}

BOOL IsExplorerFrameByName(PCWSTR name) {
    const WCHAR* fileName;
    if (!name) return FALSE;
    fileName = wcsrchr(name, L'\\');
    fileName = fileName ? fileName + 1 : name;
    return _wcsicmp(fileName, L"explorerframe.dll") == 0;
}

PPEB GetCurrentPeb() {
#if defined(_M_X64) || defined(__x86_64__)
    return (PPEB)__readgsqword(0x60);
#elif defined(_M_IX86) || defined(__i386__)
    return (PPEB)__readfsdword(0x30);
#else
    #error "Unsupported architecture"
#endif
}

BOOL PatchLoaderEntryPath(HMODULE hModule) {
    PPEB peb;
    PLIST_ENTRY head, current;
    PLDR_DATA_TABLE_ENTRY_FULL entry;
    size_t fullLen, baseLen;
    PWSTR fullBuf, baseBuf;

    peb = GetCurrentPeb();
    if (!peb || !peb->Ldr) {
        Wh_Log(L"Не удалось получить PEB/Ldr");
        return FALSE;
    }

    head = &peb->Ldr->InMemoryOrderModuleList;
    current = head->Flink;

    while (current != head) {
        entry = CONTAINING_RECORD(current, LDR_DATA_TABLE_ENTRY_FULL, InMemoryOrderLinks);

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
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (g_insideHook || !g_fileReady || !lpLibFileName) {
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }

    if (!IsExplorerFrameByName(lpLibFileName)) {
        return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    }

    Wh_Log(L"[LoadLibraryExW] Перехват: %s -> %s", lpLibFileName, g_stagingDllPath);

    g_insideHook = TRUE;
    HMODULE result = LoadLibraryExW_Original(g_stagingDllPath, hFile, dwFlags);
    g_insideHook = FALSE;

    if (result) {
        PatchLoaderEntryPath(result);
        Wh_Log(L"[LoadLibraryExW] Загружено и замаскировано: %p", result);
    } else {
        Wh_Log(L"[LoadLibraryExW] Не удалось загрузить: %u", GetLastError());
    }

    return result;
}

BOOL Wh_ModInit() {
    HMODULE hNtdll;
    HMODULE hKernelBase;
    FARPROC pLoadLibraryExW;

    Wh_Log(L"Инициализация ExplorerFrame Redirector v6.1");

    g_fileReady = PrepareExplorerFrameDll();
    if (!g_fileReady) {
        Wh_Log(L"Файл не готов, редирект отключён");
        return FALSE;
    }

    hNtdll = GetModuleHandleW(L"ntdll.dll");
    hKernelBase = GetModuleHandleW(L"kernelbase.dll");

    if (!hNtdll || !hKernelBase) {
        Wh_Log(L"Не удалось получить ntdll/kernelbase");
        return FALSE;
    }

    pLoadLibraryExW = GetProcAddress(hKernelBase, "LoadLibraryExW");

    if (pLoadLibraryExW) {
        Wh_SetFunctionHook((void*)pLoadLibraryExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
        Wh_Log(L"LoadLibraryExW хук установлен");
    }

    Wh_Log(L"Все хуки установлены");
    return TRUE;
}

// ==WindhawkMod==
// @id              restore-legacy-network-tray
// @name            Restore legacy network tray
// @description     Restores the classic network tray icon on Win10 taskbar running under Win11 24H2+
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lwininet -lbcrypt
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Redstores the classic network tray icon on Windows 10 taskbar running under Windows 11 24H2 or later.
The mod downloads the network icon dll from the Microsoft's symbols server.

This mod downloads the version of the network icon dll from one of the latest builds of Windows 11 23H2, so it may conflict 
with the mod `Windows 10 legacy flyouts on Win11 24H2 restorer`. It is recommended to use only one of the mods.

The original version of the mod is by user DerBaron of WinClassic forum, published under MIT license:
https://winclassic.net/post/40623

*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>
#include <wininet.h>
#include <bcrypt.h>
#include <unknwn.h>
#include <cstdint>
#include <cstring>
#include <cwchar>

namespace {

constexpr wchar_t kLogPrefix[] = L"[NetworkTray]";

constexpr wchar_t kPniduiDownloadUrl[] =
    L"https://msdl.microsoft.com/download/symbols/pnidui.dll/"
    L"C506D2E820b000/pnidui.dll";

// Вставьте сюда SHA-256 строкой (как в логе мода: "Computed SHA-256: ...").
// Пока пусто - проверка хэша пропускается (с предупреждением в лог).
constexpr wchar_t kExpectedSha256Hex[] = L"0f929cf218142a2aab06d31d7fac4db5c419faf153a41c7f24d7c72ef338c76e";

const GUID kClsidNetworkTray = {
    0xC2796011, 0x81BA, 0x4148,
    {0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F}
};

// Private pnidui interface requested by stobject.dll on the tested setup.
const GUID kIidNetworkTray = {
    0x8E282AAE, 0xACE9, 0x4821,
    {0x83, 0xAD, 0x84, 0x9C, 0x1D, 0x08, 0x93, 0x9D}
};

// Standard COM IID_IClassFactory: 00000001-0000-0000-C000-000000000046.
// Important: Data4 contains EIGHT bytes; an omitted zero causes E_NOINTERFACE.
const GUID kIidClassFactory = {
    0x00000001, 0x0000, 0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

using DllGetClassObjectFn = HRESULT (WINAPI*)(REFCLSID, REFIID, LPVOID*);
using RegGetValueWFn = LSTATUS (WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD,
                                          LPDWORD, PVOID, LPDWORD);
using TrayActivateFn = HRESULT (WINAPI*)(void*, DWORD, void*, void*);
using TrayCleanupFn = HRESULT (WINAPI*)(void*);
using TrayReleaseFn = ULONG (WINAPI*)(void*);

HMODULE g_pnidui = nullptr;
RegGetValueWFn g_originalRegGetValue = nullptr;
void** g_importSlot = nullptr;

// Both must remain valid while the classic tray is active.
void* g_trayObject = nullptr;
HLOCAL g_trayContext = nullptr;

HANDLE g_worker = nullptr;
volatile LONG g_workerId = 0;
volatile LONG g_stopping = 0;

// ---------------------------------------------------------------------------
// Paths (mod-private storage, to avoid conflicts with other mods).
// ---------------------------------------------------------------------------

bool BuildPaths(wchar_t* dirPath, size_t dirPathSize,
                wchar_t* filePath, size_t filePathSize) {
    size_t written = Wh_GetModStoragePath(dirPath, dirPathSize);
    if (written == 0 || written >= dirPathSize) {
        Wh_Log(L"%s Wh_GetModStoragePath failed or buffer too small",
               kLogPrefix);
        return false;
    }

    if (swprintf_s(filePath, filePathSize, L"%s\\pnidui.dll", dirPath) < 0)
        return false;

    return true;
}

bool EnsureStorageDirectory(const wchar_t* dirPath) {
    if (!CreateDirectoryW(dirPath, nullptr)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            Wh_Log(L"%s CreateDirectoryW(%s) failed: %lu",
                   kLogPrefix, dirPath, error);
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// SHA-256 checksum verification via CNG (BCrypt).
// ---------------------------------------------------------------------------

bool ComputeSha256(const wchar_t* path, BYTE outHash[32]) {
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        Wh_Log(L"%s ComputeSha256: CreateFileW failed: %lu",
               kLogPrefix, GetLastError());
        return false;
    }

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    NTSTATUS status =
        BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
    if (!BCRYPT_SUCCESS(status)) {
        Wh_Log(L"%s BCryptOpenAlgorithmProvider failed: 0x%08X",
               kLogPrefix, static_cast<unsigned>(status));
        CloseHandle(hFile);
        return false;
    }

    DWORD hashObjectLength = 0, dataLength = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                       reinterpret_cast<PUCHAR>(&hashObjectLength),
                       sizeof(hashObjectLength), &dataLength, 0);

    DWORD hashLength = 0;
    BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
                       reinterpret_cast<PUCHAR>(&hashLength),
                       sizeof(hashLength), &dataLength, 0);

    bool ok = false;
    if (hashObjectLength > 0 && hashLength == 32) {
        BYTE* hashObject = static_cast<BYTE*>(
            HeapAlloc(GetProcessHeap(), 0, hashObjectLength));
        if (hashObject) {
            BCRYPT_HASH_HANDLE hHash = nullptr;
            status = BCryptCreateHash(hAlg, &hHash, hashObject,
                                       hashObjectLength, nullptr, 0, 0);
            if (BCRYPT_SUCCESS(status)) {
                BYTE buffer[64 * 1024];
                DWORD bytesRead = 0;
                bool readOk = true;
                for (;;) {
                    if (!ReadFile(hFile, buffer, sizeof(buffer), &bytesRead,
                                 nullptr)) {
                        Wh_Log(L"%s ComputeSha256: ReadFile failed: %lu",
                               kLogPrefix, GetLastError());
                        readOk = false;
                        break;
                    }
                    if (bytesRead == 0) break;  // EOF.

                    status = BCryptHashData(hHash, buffer, bytesRead, 0);
                    if (!BCRYPT_SUCCESS(status)) {
                        readOk = false;
                        break;
                    }
                }
                if (readOk) {
                    status = BCryptFinishHash(hHash, outHash, 32, 0);
                    ok = BCRYPT_SUCCESS(status);
                }
                BCryptDestroyHash(hHash);
            }
            HeapFree(GetProcessHeap(), 0, hashObject);
        }
    }

    BCryptCloseAlgorithmProvider(hAlg, 0);
    CloseHandle(hFile);
    return ok;
}

void BytesToHex(const BYTE* bytes, size_t count, wchar_t* out, size_t outSize) {
    static const wchar_t kHexDigits[] = L"0123456789abcdef";
    size_t i = 0;
    for (; i < count && (i * 2 + 1) < outSize; ++i) {
        out[i * 2]     = kHexDigits[bytes[i] >> 4];
        out[i * 2 + 1] = kHexDigits[bytes[i] & 0x0F];
    }
    size_t nullPos = i * 2;
    out[nullPos < outSize ? nullPos : outSize - 1] = L'\0';
}

bool VerifyChecksum(const wchar_t* path) {
    BYTE hash[32] = {};
    if (!ComputeSha256(path, hash)) {
        Wh_Log(L"%s Failed to compute SHA-256 for %s", kLogPrefix, path);
        return false;
    }

    wchar_t hashHex[65] = {};
    BytesToHex(hash, 32, hashHex, ARRAYSIZE(hashHex));
    Wh_Log(L"%s Computed SHA-256: %s", kLogPrefix, hashHex);

    if (!*kExpectedSha256Hex) {
        Wh_Log(L"%s WARNING: expected SHA-256 not configured, "
               L"skipping checksum verification", kLogPrefix);
        return true;
    }

    if (_wcsicmp(hashHex, kExpectedSha256Hex) != 0) {
        Wh_Log(L"%s Checksum mismatch (expected %s)",
               kLogPrefix, kExpectedSha256Hex);
        return false;
    }

    Wh_Log(L"%s Checksum verified", kLogPrefix);
    return true;
}

// ---------------------------------------------------------------------------
// Download pnidui.dll from the Microsoft symbol server if it's missing or
// fails checksum verification.
// ---------------------------------------------------------------------------

bool DownloadFile(const wchar_t* url, const wchar_t* destPath) {
    Wh_Log(L"%s Connecting to download server...", kLogPrefix);

    HINTERNET hInternet = InternetOpenW(
        L"WindhawkRestoreLegacyNetworkTray/1.0",
        INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        Wh_Log(L"%s InternetOpenW failed: %lu", kLogPrefix, GetLastError());
        return false;
    }

    DWORD timeoutMs = 20000;
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT,
                        &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT,
                        &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hInternet, INTERNET_OPTION_SEND_TIMEOUT,
                        &timeoutMs, sizeof(timeoutMs));

    HINTERNET hUrl = InternetOpenUrlW(
        hInternet, url, nullptr, 0,
        INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES |
            INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD |
            INTERNET_FLAG_SECURE,
        0);
    if (!hUrl) {
        Wh_Log(L"%s InternetOpenUrlW failed: %lu", kLogPrefix, GetLastError());
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    if (!HttpQueryInfoW(hUrl,
                        HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE,
                        &statusCode, &statusCodeSize, nullptr) ||
        statusCode != 200) {
        Wh_Log(L"%s Unexpected HTTP status: %lu", kLogPrefix, statusCode);
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    wchar_t tempPath[MAX_PATH] = {};
    if (swprintf_s(tempPath, ARRAYSIZE(tempPath), L"%s.download",
                   destPath) < 0) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    HANDLE hFile = CreateFileW(tempPath, GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        Wh_Log(L"%s CreateFileW(%s) failed: %lu",
               kLogPrefix, tempPath, GetLastError());
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return false;
    }

    bool ok = true;
    DWORD totalBytes = 0;
    BYTE buffer[64 * 1024];
    for (;;) {
        DWORD bytesRead = 0;
        if (!InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead)) {
            Wh_Log(L"%s InternetReadFile failed: %lu",
                   kLogPrefix, GetLastError());
            ok = false;
            break;
        }
        if (bytesRead == 0) break;  // EOF.

        DWORD written = 0;
        if (!WriteFile(hFile, buffer, bytesRead, &written, nullptr) ||
            written != bytesRead) {
            Wh_Log(L"%s WriteFile failed: %lu", kLogPrefix, GetLastError());
            ok = false;
            break;
        }
        totalBytes += bytesRead;
    }

    CloseHandle(hFile);
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (!ok || totalBytes < 64 * 1024) {
        Wh_Log(L"%s Download incomplete (%lu bytes)", kLogPrefix, totalBytes);
        DeleteFileW(tempPath);
        return false;
    }

    if (!MoveFileExW(tempPath, destPath, MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"%s MoveFileExW failed: %lu", kLogPrefix, GetLastError());
        DeleteFileW(tempPath);
        return false;
    }

    Wh_Log(L"%s Downloaded pnidui.dll (%lu bytes)", kLogPrefix, totalBytes);
    return true;
}

bool EnsurePniduiFile(const wchar_t* filePath) {
    DWORD attr = GetFileAttributesW(filePath);
    bool exists = attr != INVALID_FILE_ATTRIBUTES &&
                  !(attr & FILE_ATTRIBUTE_DIRECTORY);

    if (exists) {
        Wh_Log(L"%s pnidui.dll found at %s", kLogPrefix, filePath);
        if (VerifyChecksum(filePath)) return true;
        Wh_Log(L"%s Existing pnidui.dll failed checksum, re-downloading",
               kLogPrefix);
        DeleteFileW(filePath);
    } else {
        Wh_Log(L"%s pnidui.dll not found, will download", kLogPrefix);
    }

    constexpr int kMaxAttempts = 3;
    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        if (InterlockedCompareExchange(&g_stopping, 0, 0)) return false;

        Wh_Log(L"%s Download attempt %d/%d...",
               kLogPrefix, attempt, kMaxAttempts);

        if (DownloadFile(kPniduiDownloadUrl, filePath)) {
            if (VerifyChecksum(filePath)) return true;
            Wh_Log(L"%s Downloaded file failed checksum verification",
                   kLogPrefix);
            DeleteFileW(filePath);
        }

        if (attempt < kMaxAttempts) Sleep(3000);
    }

    Wh_Log(L"%s Giving up obtaining a valid pnidui.dll", kLogPrefix);
    return false;
}

// ---------------------------------------------------------------------------
// PE and pointer validation for the undocumented pnidui vtable.
// ---------------------------------------------------------------------------

bool GetNtHeaders(HMODULE module, PIMAGE_NT_HEADERS* output) {
    if (!module || !output) return false;

    auto* base = reinterpret_cast<BYTE*>(module);
    auto* dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return false;

    auto* nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
    *output = nt;
    return true;
}

bool InsideModule(HMODULE module, const void* address) {
    PIMAGE_NT_HEADERS nt = nullptr;
    if (!address || !GetNtHeaders(module, &nt)) return false;

    const auto base = reinterpret_cast<uintptr_t>(module);
    const auto addr = reinterpret_cast<uintptr_t>(address);
    return addr >= base && addr - base < nt->OptionalHeader.SizeOfImage;
}

bool InExecutableSection(HMODULE module, const void* address) {
    PIMAGE_NT_HEADERS nt = nullptr;
    if (!InsideModule(module, address) || !GetNtHeaders(module, &nt))
        return false;

    const auto base = reinterpret_cast<uintptr_t>(module);
    const auto addr = reinterpret_cast<uintptr_t>(address);
    auto* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        const auto start = base + section->VirtualAddress;
        const auto length = section->Misc.VirtualSize > section->SizeOfRawData
                                ? section->Misc.VirtualSize
                                : section->SizeOfRawData;
        if (addr >= start && addr - start < length)
            return (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
    }
    return false;
}

bool MemoryReadable(const void* address, SIZE_T bytes) {
    if (!address || !bytes) return false;

    MEMORY_BASIC_INFORMATION info = {};
    if (!VirtualQuery(address, &info, sizeof(info)) ||
        info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;

    const auto addr = reinterpret_cast<uintptr_t>(address);
    const auto base = reinterpret_cast<uintptr_t>(info.BaseAddress);
    return addr >= base && bytes <= info.RegionSize &&
           addr - base <= info.RegionSize - bytes;
}

bool ValidateTrayObject(void* object, void*** output) {
    if (!output || !MemoryReadable(object, sizeof(void*))) return false;
    auto** vtable = *reinterpret_cast<void***>(object);
    if (!vtable || !InsideModule(g_pnidui, vtable) ||
        !InsideModule(g_pnidui, vtable + 4) ||
        !MemoryReadable(vtable, sizeof(void*) * 5))
        return false;

    // [0] QueryInterface, [1] AddRef, [2] Release,
    // [3] private activation, [4] private cleanup.
    for (int i = 0; i < 5; ++i) {
        if (!vtable[i] || !InExecutableSection(g_pnidui, vtable[i])) {
            Wh_Log(L"%s Invalid pnidui vtable slot %d", kLogPrefix, i);
            return false;
        }
    }
    *output = vtable;
    return true;
}

// ---------------------------------------------------------------------------
// Temporary import-table patch for pnidui's legacy ReplaceVan lookup.
// ---------------------------------------------------------------------------

bool PatchImportByName(HMODULE module, const char* functionName,
                       void* replacement, void*** slotOut,
                       void** originalOut) {
    if (!module || !functionName || !replacement || !slotOut || !originalOut)
        return false;

    PIMAGE_NT_HEADERS nt = nullptr;
    if (!GetNtHeaders(module, &nt)) return false;
    auto* base = reinterpret_cast<BYTE*>(module);
    const auto& imports =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!imports.VirtualAddress) return false;

    auto* descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
        base + imports.VirtualAddress);
    for (; descriptor->Name; ++descriptor) {
        if (!descriptor->OriginalFirstThunk) continue;

        auto* names = reinterpret_cast<PIMAGE_THUNK_DATA>(
            base + descriptor->OriginalFirstThunk);
        auto* addresses = reinterpret_cast<PIMAGE_THUNK_DATA>(
            base + descriptor->FirstThunk);
        for (; names->u1.AddressOfData; ++names, ++addresses) {
            if (IMAGE_SNAP_BY_ORDINAL(names->u1.Ordinal)) continue;
            auto* import = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                base + names->u1.AddressOfData);
            if (strcmp(reinterpret_cast<const char*>(import->Name),
                       functionName) != 0)
                continue;

            auto** slot = reinterpret_cast<void**>(&addresses->u1.Function);
            DWORD oldProtect = 0;
            if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect))
                return false;

            *originalOut = *slot;
            *slot = replacement;
            DWORD ignored = 0;
            VirtualProtect(slot, sizeof(void*), oldProtect, &ignored);
            FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));
            *slotOut = slot;
            return true;
        }
    }
    return false;
}

void RestoreImport(void** slot, void* original) {
    if (!slot || !original) return;
    DWORD oldProtect = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect))
        return;

    *slot = original;
    DWORD ignored = 0;
    VirtualProtect(slot, sizeof(void*), oldProtect, &ignored);
    FlushInstructionCache(GetCurrentProcess(), slot, sizeof(void*));
}

LSTATUS WINAPI RegGetValueHook(HKEY key, LPCWSTR subkey, LPCWSTR value,
                               DWORD flags, LPDWORD type, PVOID data,
                               LPDWORD dataSize) {
    if (!g_originalRegGetValue) return ERROR_INVALID_FUNCTION;

    LSTATUS result = g_originalRegGetValue(
        key, subkey, value, flags, type, data, dataSize);
    if (result != ERROR_FILE_NOT_FOUND ||
        !value || wcscmp(value, L"ReplaceVan") != 0)
        return result;

    // Pretend the missing DWORD value exists with a value of zero.
    // Respect the Windows API buffer-size contract.
    if (data && (!dataSize || *dataSize < sizeof(DWORD))) {
        if (dataSize) *dataSize = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    if (type) *type = REG_DWORD;
    if (data) *static_cast<DWORD*>(data) = 0;
    if (dataSize) *dataSize = sizeof(DWORD);
    return ERROR_SUCCESS;
}

// ---------------------------------------------------------------------------
// DLL load and tray lifecycle.
// ---------------------------------------------------------------------------

bool PreparePnidui() {
    if (g_pnidui) return true;

    wchar_t dirPath[MAX_PATH] = {};
    wchar_t path[MAX_PATH] = {};
    if (!BuildPaths(dirPath, ARRAYSIZE(dirPath), path, ARRAYSIZE(path))) {
        Wh_Log(L"%s Unable to resolve pnidui.dll path", kLogPrefix);
        return false;
    }

    if (!EnsureStorageDirectory(dirPath)) return false;

    if (!EnsurePniduiFile(path)) {
        Wh_Log(L"%s pnidui.dll unavailable: %s", kLogPrefix, path);
        return false;
    }

    // Avoid loading a second copy if this exact module is already mapped
    // into the process (e.g. after a mod reload without restarting Explorer).
    HMODULE existing = GetModuleHandleW(path);
    if (existing) {
        g_pnidui = existing;
        Wh_Log(L"%s pnidui.dll already loaded, reusing existing handle",
               kLogPrefix);
    } else {
        g_pnidui = LoadLibraryW(path);
        if (!g_pnidui) {
            Wh_Log(L"%s LoadLibrary failed: %lu", kLogPrefix, GetLastError());
            return false;
        }
    }

    wchar_t actualPath[MAX_PATH] = {};
    DWORD actualLength = GetModuleFileNameW(
        g_pnidui, actualPath, ARRAYSIZE(actualPath));
    if (!actualLength || actualLength >= ARRAYSIZE(actualPath) ||
        _wcsicmp(path, actualPath) != 0) {
        Wh_Log(L"%s Loaded DLL path does not match", kLogPrefix);
        return false;
    }

    void* entry = reinterpret_cast<void*>(
        GetProcAddress(g_pnidui, "DllGetClassObject"));
    if (!entry || !InExecutableSection(g_pnidui, entry)) {
        Wh_Log(L"%s Invalid DllGetClassObject export", kLogPrefix);
        return false;
    }

    void* original = nullptr;
    if (!PatchImportByName(g_pnidui, "RegGetValueW",
                           reinterpret_cast<void*>(RegGetValueHook),
                           &g_importSlot, &original)) {
        Wh_Log(L"%s Could not patch RegGetValueW", kLogPrefix);
        return false;
    }
    g_originalRegGetValue = reinterpret_cast<RegGetValueWFn>(original);
    Wh_Log(L"%s pnidui.dll ready", kLogPrefix);
    return true;
}

bool StartTray() {
    if (!PreparePnidui()) return false;
    auto entry = reinterpret_cast<DllGetClassObjectFn>(
        GetProcAddress(g_pnidui, "DllGetClassObject"));
    if (!entry || !InExecutableSection(g_pnidui,
                                       reinterpret_cast<void*>(entry)))
        return false;

    IClassFactory* factory = nullptr;
    HRESULT hr = entry(kClsidNetworkTray, kIidClassFactory,
                       reinterpret_cast<void**>(&factory));
    Wh_Log(L"%s DllGetClassObject: 0x%08X", kLogPrefix,
           static_cast<unsigned>(hr));
    if (FAILED(hr) || !factory) return false;

    void* object = nullptr;
    hr = factory->CreateInstance(nullptr, kIidNetworkTray, &object);
    factory->Release();
    Wh_Log(L"%s CreateInstance: 0x%08X", kLogPrefix,
           static_cast<unsigned>(hr));
    if (FAILED(hr) || !object) return false;

    void** vtable = nullptr;
    if (!ValidateTrayObject(object, &vtable)) {
        // Do not call an unverified Release function pointer.
        Wh_Log(L"%s Unsafe private COM vtable; aborting", kLogPrefix);
        return false;
    }

    HLOCAL context = LocalAlloc(LMEM_ZEROINIT, 0x38);
    if (!context) {
        reinterpret_cast<TrayReleaseFn>(vtable[2])(object);
        return false;
    }

    // Recheck the undocumented methods immediately before calling them.
    if (!InExecutableSection(g_pnidui, vtable[3]) ||
        !InExecutableSection(g_pnidui, vtable[4])) {
        Wh_Log(L"%s Private activation pointers changed", kLogPrefix);
        LocalFree(context);
        reinterpret_cast<TrayReleaseFn>(vtable[2])(object);
        return false;
    }

    // Private activation uses two offsets in the retained context buffer.
    hr = reinterpret_cast<TrayActivateFn>(vtable[3])(
        object, 4, static_cast<BYTE*>(context) + 0x18,
        static_cast<BYTE*>(context) + 0x10);
    Wh_Log(L"%s Activate: 0x%08X", kLogPrefix, static_cast<unsigned>(hr));
    if (FAILED(hr)) {
        reinterpret_cast<TrayCleanupFn>(vtable[4])(object);
        LocalFree(context);
        reinterpret_cast<TrayReleaseFn>(vtable[2])(object);
        return false;
    }

    g_trayObject = object;
    g_trayContext = context;
    Wh_Log(L"%s Legacy network tray activated", kLogPrefix);
    return true;
}

void StopTray() {
    if (!g_trayObject) return;

    void** vtable = nullptr;
    if (!ValidateTrayObject(g_trayObject, &vtable)) {
        // Keep unknown allocations alive until Explorer exits.
        Wh_Log(L"%s Tray vtable changed; skipping cleanup", kLogPrefix);
        g_trayObject = nullptr;
        g_trayContext = nullptr;
        return;
    }

    HRESULT hr = reinterpret_cast<TrayCleanupFn>(vtable[4])(g_trayObject);
    Wh_Log(L"%s Cleanup: 0x%08X", kLogPrefix, static_cast<unsigned>(hr));
    if (g_trayContext) LocalFree(g_trayContext);
    reinterpret_cast<TrayReleaseFn>(vtable[2])(g_trayObject);
    g_trayContext = nullptr;
    g_trayObject = nullptr;
}

bool IsShellExplorer() {
    HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!window) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    return pid == GetCurrentProcessId();
}

DWORD WINAPI WorkerThread(LPVOID) {
    InterlockedExchange(&g_workerId, static_cast<LONG>(GetCurrentThreadId()));

    Wh_Log(L"%s Worker thread started", kLogPrefix);

    // Force the creation of the STA thread's message queue.
    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"%s CoInitializeEx failed: 0x%08X", kLogPrefix,
               static_cast<unsigned>(hr));
        InterlockedExchange(&g_workerId, 0);
        return 0;
    }

    bool shellFound = false;
    for (int i = 0; i < 100 &&
         !InterlockedCompareExchange(&g_stopping, 0, 0); ++i) {
        HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (window) {
            if (!IsShellExplorer()) {
                Wh_Log(L"%s Another explorer.exe owns the taskbar", kLogPrefix);
                CoUninitialize();
                InterlockedExchange(&g_workerId, 0);
                return 0;
            }
            shellFound = true;
            break;
        }
        Sleep(100);
    }

    if (shellFound) {
        Wh_Log(L"%s Shell_TrayWnd found, preparing tray", kLogPrefix);
        Sleep(200);  // Allow taskbar initialization to finish.
        if (!InterlockedCompareExchange(&g_stopping, 0, 0) &&
            IsShellExplorer() && StartTray()) {
            while (!InterlockedCompareExchange(&g_stopping, 0, 0)) {
                DWORD wait = MsgWaitForMultipleObjects(
                    0, nullptr, FALSE, 250, QS_ALLINPUT);
                if (wait != WAIT_OBJECT_0) continue;

                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        InterlockedExchange(&g_stopping, 1);
                        break;
                    }
                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                }
            }
            // COM cleanup must happen on the original STA thread.
            StopTray();
        } else {
            Wh_Log(L"%s Activation skipped or failed", kLogPrefix);
        }
    } else {
        Wh_Log(L"%s Shell_TrayWnd not found", kLogPrefix);
    }

    CoUninitialize();
    InterlockedExchange(&g_workerId, 0);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"%s Wh_ModInit", kLogPrefix);
    InterlockedExchange(&g_stopping, 0);
    g_worker = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"%s CreateThread failed: %lu", kLogPrefix, GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"%s Wh_ModUninit", kLogPrefix);
    InterlockedExchange(&g_stopping, 1);
    LONG workerId = InterlockedCompareExchange(&g_workerId, 0, 0);
    if (workerId)
        PostThreadMessageW(static_cast<DWORD>(workerId), WM_NULL, 0, 0);

    if (g_worker) {
        // Do not unload the Windhawk mod before its worker stops.
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }

    if (g_importSlot && g_originalRegGetValue) {
        RestoreImport(g_importSlot,
                      reinterpret_cast<void*>(g_originalRegGetValue));
        g_importSlot = nullptr;
        g_originalRegGetValue = nullptr;
        Wh_Log(L"%s Restored RegGetValueW import", kLogPrefix);
    }

    // Keep pnidui loaded as in the original mod; private COM references may
    // remain if their vtable failed validation during activation or cleanup.
}

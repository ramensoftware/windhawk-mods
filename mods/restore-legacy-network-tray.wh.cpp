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
#include <objbase.h>
#include <unknwn.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cwchar>

namespace {

constexpr wchar_t kDownloadUrl[] =
    L"https://msdl.microsoft.com/download/symbols/pnidui.dll/C506D2E820b000/pnidui.dll";

// Must be set to the SHA-256 of the exact file at kDownloadUrl, as a
// 64-character hex string (the mod logs the hash of any file it downloads
// or finds in storage as "Computed SHA-256: ..."; copy that value here).
// If this is left empty, verification fails closed: the mod will not load
// or activate anything. This is intentional, so that emptying the constant
// by mistake can't silently turn the check off.
constexpr wchar_t kExpectedSha256Hex[] = L"0f929cf218142a2aab06d31d7fac4db5c419faf153a41c7f24d7c72ef338c76e";

constexpr wchar_t kFileName[] = L"pnidui.dll";

constexpr CLSID kClsidNetworkTray = {
    0xC2796011,
    0x81BA,
    0x4148,
    {0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F}};

// IID_IClassFactory already has a declaration (in the SDK headers) and a
// definition (in uuid.lib, linked via -luuid) available, and
// __uuidof(IClassFactory) does work fine with this clang/mingw-based
// toolchain. kIidClassFactory below is a hand-written copy of that same
// well-known value, used only so the mod doesn't need the extra -luuid link
// dependency for this one constant.
//
// INetworkTrayPrivate, in contrast, is a private interface specific to this
// mod, so the SDK has no declared IID for it. Attaching one via
// __declspec(uuid("...")) does not work here: clang accepts the attribute
// syntax but ignores it ("unknown attribute 'uuid' ignored"), so
// __uuidof(INetworkTrayPrivate) would not resolve to the right value.
// kIidNetworkTrayPrivate is therefore a plain constant, passed explicitly
// wherever the IID is needed.

// {8E282AAE-ACE9-4821-83AD-849C1D08939D}
constexpr GUID kIidNetworkTrayPrivate = {
    0x8E282AAE,
    0xACE9,
    0x4821,
    {0x83, 0xAD, 0x84, 0x9C, 0x1D, 0x08, 0x93, 0x9D}};

// {00000001-0000-0000-C000-000000000046}
constexpr GUID kIidClassFactory = {
    0x00000001,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

struct INetworkTrayPrivate : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Activate(DWORD flags, void* out1,
                                                void* out2) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cleanup() = 0;
};

using DllGetClassObjectFn = HRESULT(WINAPI*)(REFCLSID, REFIID, void**);
using RegGetValueWFn = LONG(WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD,
                                      PVOID, LPDWORD);

HMODULE g_pnidui = nullptr;
DllGetClassObjectFn g_dllGetClassObject = nullptr;
RegGetValueWFn g_originalRegGetValue = nullptr;
void** g_importSlot = nullptr;
INetworkTrayPrivate* g_trayObject = nullptr;
HLOCAL g_trayContext = nullptr;
HANDLE g_worker = nullptr;
HANDLE g_stopEvent = nullptr;
std::atomic<HINTERNET> g_activeInternet{nullptr};
std::atomic<HINTERNET> g_activeUrl{nullptr};

// RAII guard for the worker thread's COM apartment. pnidui's tray object
// uses COM internally (network-status objects and their event sinks) and is
// designed to run on an STA thread, like stobject's tray thread normally
// provides. Without this, pnidui's COM calls either fail with
// CO_E_NOTINITIALIZED or silently join Explorer's implicit MTA, which can
// make status-change callbacks arrive on the wrong thread and the icon show
// stale state.
class ComInitGuard {
   public:
    ComInitGuard()
        : hr_(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)) {}

    ~ComInitGuard() {
        if (SUCCEEDED(hr_)) {
            CoUninitialize();
        }
    }

    ComInitGuard(const ComInitGuard&) = delete;
    ComInitGuard& operator=(const ComInitGuard&) = delete;

    bool Succeeded() const { return SUCCEEDED(hr_); }
    HRESULT Result() const { return hr_; }

   private:
    HRESULT hr_;
};

void CloseAndClear(std::atomic<HINTERNET>& handle) {
    HINTERNET h = handle.exchange(nullptr);
    if (h) {
        InternetCloseHandle(h);
    }
}

bool StopRequested() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

// Returns true if the stop event was signaled while waiting.
bool WaitOrStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

LONG WINAPI RegGetValueHook(HKEY hkey, LPCWSTR subKey, LPCWSTR value,
                             DWORD flags, LPDWORD type, PVOID data,
                             LPDWORD dataSize) {
    if (value && _wcsicmp(value, L"ReplaceVan") == 0) {
        if (type) {
            *type = REG_DWORD;
        }

        if (data && dataSize && *dataSize < sizeof(DWORD)) {
            *dataSize = sizeof(DWORD);
            return ERROR_MORE_DATA;
        }

        if (data) {
            *reinterpret_cast<DWORD*>(data) = 0;
        }

        if (dataSize) {
            *dataSize = sizeof(DWORD);
        }

        return ERROR_SUCCESS;
    }

    if (!g_originalRegGetValue) {
        return ERROR_CALL_NOT_IMPLEMENTED;
    }

    return g_originalRegGetValue(hkey, subKey, value, flags, type, data,
                                  dataSize);
}

PIMAGE_NT_HEADERS GetNtHeaders(HMODULE module) {
    auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }

    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<BYTE*>(module) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return nullptr;
    }

    return nt;
}

// Writes the original function pointer to *originalFuncOut before switching
// the IAT slot to newFunc. This ordering matters: callers that pass the
// address of a global used by the hook itself (e.g. &g_originalRegGetValue)
// are guaranteed that the global is populated before the hook can possibly
// be invoked, with no race window in between.
bool PatchImportByName(HMODULE module, const char* dllName,
                        const char* funcName, void* newFunc,
                        void*** originalSlotOut, void** originalFuncOut) {
    PIMAGE_NT_HEADERS nt = GetNtHeaders(module);
    if (!nt) {
        return false;
    }

    IMAGE_DATA_DIRECTORY importDir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (importDir.VirtualAddress == 0) {
        return false;
    }

    auto base = reinterpret_cast<BYTE*>(module);
    auto desc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
        base + importDir.VirtualAddress);

    for (; desc->Name != 0; desc++) {
        const char* name = reinterpret_cast<const char*>(base + desc->Name);
        if (_stricmp(name, dllName) != 0) {
            continue;
        }

        auto thunk =
            reinterpret_cast<PIMAGE_THUNK_DATA>(base + desc->FirstThunk);
        auto origThunk =
            desc->OriginalFirstThunk
                ? reinterpret_cast<PIMAGE_THUNK_DATA>(base +
                                                       desc->OriginalFirstThunk)
                : thunk;

        for (; origThunk->u1.AddressOfData != 0; origThunk++, thunk++) {
            if (IMAGE_SNAP_BY_ORDINAL(origThunk->u1.Ordinal)) {
                continue;
            }

            auto importByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                base + origThunk->u1.AddressOfData);
            if (_stricmp(reinterpret_cast<const char*>(importByName->Name),
                         funcName) != 0) {
                continue;
            }

            void** slot = reinterpret_cast<void**>(&thunk->u1.Function);
            DWORD oldProtect = 0;
            if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE,
                                 &oldProtect)) {
                return false;
            }

            if (originalFuncOut) {
                *originalFuncOut = *slot;
            }
            *slot = newFunc;
            VirtualProtect(slot, sizeof(void*), oldProtect, &oldProtect);

            if (originalSlotOut) {
                *originalSlotOut = slot;
            }
            return true;
        }
    }

    return false;
}

void RestoreImport(void** slot, void* originalFunc) {
    if (!slot || !originalFunc) {
        return;
    }

    DWORD oldProtect = 0;
    if (VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtect)) {
        *slot = originalFunc;
        VirtualProtect(slot, sizeof(void*), oldProtect, &oldProtect);
    }
}

// True if the OS ships its own pnidui.dll (Windows 10, and Windows 11 up to
// 23H2). On such systems the legacy network tray is already provided by the
// system and this mod should not activate a second copy.
bool SystemHasPnidui() {
    wchar_t path[MAX_PATH];
    UINT len = GetSystemDirectoryW(path, ARRAYSIZE(path));
    if (len == 0 || len >= ARRAYSIZE(path)) {
        return false;
    }

    if (wcscat_s(path, ARRAYSIZE(path), L"\\pnidui.dll") != 0) {
        return false;
    }

    return GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

bool BuildPaths(wchar_t* dirPath, size_t dirPathSize, wchar_t* filePath,
                size_t filePathSize) {
    size_t written = Wh_GetModStoragePath(dirPath, dirPathSize);
    if (written == 0 || written >= dirPathSize) {
        Wh_Log(L"Failed to get mod storage path");
        return false;
    }

    if (_snwprintf_s(filePath, filePathSize, _TRUNCATE, L"%s\\%s", dirPath,
                      kFileName) < 0) {
        Wh_Log(L"Failed to build file path");
        return false;
    }

    return true;
}

bool EnsureStorageDirectory(const wchar_t* dirPath) {
    if (CreateDirectoryW(dirPath, nullptr)) {
        return true;
    }

    return GetLastError() == ERROR_ALREADY_EXISTS;
}

// Hashes the remaining contents of an already-open file handle, starting
// from the beginning. The handle is not closed by this function.
bool ComputeSha256Handle(HANDLE file, BYTE (&hash)[32]) {
    if (SetFilePointer(file, 0, nullptr, FILE_BEGIN) ==
            INVALID_SET_FILE_POINTER &&
        GetLastError() != NO_ERROR) {
        return false;
    }

    BCRYPT_ALG_HANDLE algHandle = nullptr;
    if (BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_SHA256_ALGORITHM,
                                     nullptr, 0) != 0) {
        return false;
    }

    DWORD hashObjectSize = 0;
    DWORD dataSize = 0;
    if (BCryptGetProperty(algHandle, BCRYPT_OBJECT_LENGTH,
                           reinterpret_cast<PUCHAR>(&hashObjectSize),
                           sizeof(hashObjectSize), &dataSize, 0) != 0) {
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return false;
    }

    BYTE* hashObject = new BYTE[hashObjectSize];
    BCRYPT_HASH_HANDLE hashHandle = nullptr;
    if (BCryptCreateHash(algHandle, &hashHandle, hashObject, hashObjectSize,
                          nullptr, 0, 0) != 0) {
        delete[] hashObject;
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return false;
    }

    bool ok = true;
    BYTE buffer[64 * 1024];
    DWORD bytesRead = 0;
    while (ReadFile(file, buffer, sizeof(buffer), &bytesRead, nullptr) &&
           bytesRead > 0) {
        if (BCryptHashData(hashHandle, buffer, bytesRead, 0) != 0) {
            ok = false;
            break;
        }
    }

    if (ok && BCryptFinishHash(hashHandle, hash, sizeof(hash), 0) != 0) {
        ok = false;
    }

    BCryptDestroyHash(hashHandle);
    delete[] hashObject;
    BCryptCloseAlgorithmProvider(algHandle, 0);
    return ok;
}

bool ComputeSha256(const wchar_t* path, BYTE (&hash)[32]) {
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool ok = ComputeSha256Handle(file, hash);
    CloseHandle(file);
    return ok;
}

void BytesToHex(const BYTE* bytes, size_t count, wchar_t* out,
                size_t outSize) {
    static const wchar_t kHexDigits[] = L"0123456789abcdef";
    size_t i = 0;
    for (; i < count && (i * 2 + 1) < outSize; i++) {
        out[i * 2] = kHexDigits[bytes[i] >> 4];
        out[i * 2 + 1] = kHexDigits[bytes[i] & 0xF];
    }
    if (i * 2 < outSize) {
        out[i * 2] = L'\0';
    }
}

bool VerifyChecksumBytes(const BYTE (&hash)[32]) {
    wchar_t hex[65];
    BytesToHex(hash, sizeof(hash), hex, ARRAYSIZE(hex));
    Wh_Log(L"Computed SHA-256: %s", hex);

    if (kExpectedSha256Hex[0] == L'\0') {
        Wh_Log(L"No expected checksum configured in kExpectedSha256Hex, "
               L"failing closed");
        return false;
    }

    return _wcsicmp(hex, kExpectedSha256Hex) == 0;
}

bool VerifyChecksum(const wchar_t* path) {
    BYTE hash[32];
    if (!ComputeSha256(path, hash)) {
        Wh_Log(L"Failed to compute SHA-256 for %s", path);
        return false;
    }

    return VerifyChecksumBytes(hash);
}

bool VerifyChecksumHandle(HANDLE file) {
    BYTE hash[32];
    if (!ComputeSha256Handle(file, hash)) {
        Wh_Log(L"Failed to compute SHA-256");
        return false;
    }

    return VerifyChecksumBytes(hash);
}

bool DownloadFile(const wchar_t* url, const wchar_t* destPath) {
    wchar_t tempPath[MAX_PATH];
    if (_snwprintf_s(tempPath, ARRAYSIZE(tempPath), _TRUNCATE, L"%s.download",
                      destPath) < 0) {
        return false;
    }

    HINTERNET internet = InternetOpenW(L"WindhawkMod/1.0",
                                        INTERNET_OPEN_TYPE_PRECONFIG, nullptr,
                                        nullptr, 0);
    if (!internet) {
        Wh_Log(L"InternetOpenW failed: %u", GetLastError());
        return false;
    }
    g_activeInternet.store(internet);

    DWORD timeout = 20000;
    InternetSetOptionW(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                        sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                        sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_SEND_TIMEOUT, &timeout,
                        sizeof(timeout));

    if (StopRequested()) {
        CloseAndClear(g_activeInternet);
        return false;
    }

    DWORD flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES |
                  INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD |
                  INTERNET_FLAG_SECURE;
    HINTERNET urlHandle = InternetOpenUrlW(internet, url, nullptr, 0, flags, 0);
    if (!urlHandle) {
        Wh_Log(L"InternetOpenUrlW failed: %u", GetLastError());
        CloseAndClear(g_activeInternet);
        return false;
    }
    g_activeUrl.store(urlHandle);

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    if (!HttpQueryInfoW(urlHandle,
                         HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE,
                         &statusCode, &statusCodeSize, nullptr) ||
        statusCode != 200) {
        Wh_Log(L"Unexpected HTTP status: %u", statusCode);
        CloseAndClear(g_activeUrl);
        CloseAndClear(g_activeInternet);
        return false;
    }

    HANDLE file = CreateFileW(tempPath, GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Failed to create temp file: %u", GetLastError());
        CloseAndClear(g_activeUrl);
        CloseAndClear(g_activeInternet);
        return false;
    }

    bool ok = true;
    size_t totalBytes = 0;
    BYTE buffer[64 * 1024];
    DWORD bytesRead = 0;

    while (true) {
        if (StopRequested()) {
            Wh_Log(L"Download interrupted by mod unload");
            ok = false;
            break;
        }

        if (!InternetReadFile(urlHandle, buffer, sizeof(buffer), &bytesRead)) {
            Wh_Log(L"InternetReadFile failed: %u", GetLastError());
            ok = false;
            break;
        }

        if (bytesRead == 0) {
            break;
        }

        DWORD written = 0;
        if (!WriteFile(file, buffer, bytesRead, &written, nullptr) ||
            written != bytesRead) {
            Wh_Log(L"WriteFile failed: %u", GetLastError());
            ok = false;
            break;
        }

        totalBytes += bytesRead;
    }

    CloseHandle(file);
    CloseAndClear(g_activeUrl);
    CloseAndClear(g_activeInternet);

    if (!ok) {
        DeleteFileW(tempPath);
        return false;
    }

    if (totalBytes < 64 * 1024) {
        Wh_Log(L"Downloaded file too small: %zu bytes", totalBytes);
        DeleteFileW(tempPath);
        return false;
    }

    if (!MoveFileExW(tempPath, destPath, MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"MoveFileExW failed: %u", GetLastError());
        DeleteFileW(tempPath);
        return false;
    }

    return true;
}

bool EnsurePniduiFile(const wchar_t* filePath) {
    DWORD attrs = GetFileAttributesW(filePath);
    if (attrs != INVALID_FILE_ATTRIBUTES &&
        !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        if (VerifyChecksum(filePath)) {
            return true;
        }
        Wh_Log(L"Checksum mismatch, re-downloading");
        DeleteFileW(filePath);
    }

    for (int attempt = 0; attempt < 3; attempt++) {
        if (StopRequested()) {
            return false;
        }

        Wh_Log(L"Downloading pnidui.dll, attempt %d", attempt + 1);
        if (DownloadFile(kDownloadUrl, filePath)) {
            if (VerifyChecksum(filePath)) {
                return true;
            }
            Wh_Log(L"Downloaded file failed checksum verification");
            DeleteFileW(filePath);
        }

        if (attempt < 2) {
            if (WaitOrStop(3000)) {
                return false;
            }
        }
    }

    return false;
}

// Opens the file with a sharing mode that denies write access to other
// processes/threads, and keeps that handle open across both the checksum
// verification and the LoadLibraryW call, only releasing it afterwards.
// This closes the TOCTOU window between "the file on disk matches
// kExpectedSha256Hex" and "this is the file that actually got mapped into
// the process": nothing can replace or modify the file out from under us
// while this handle stays open.
HMODULE PinAndLoadLibrary(const wchar_t* path) {
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Failed to open %s: %u", path, GetLastError());
        return nullptr;
    }

    bool verified = VerifyChecksumHandle(file);

    HMODULE module = nullptr;
    if (verified) {
        module = LoadLibraryW(path);
        if (!module) {
            Wh_Log(L"LoadLibraryW failed: %u", GetLastError());
        }
    } else {
        Wh_Log(L"Refusing to load %s: checksum verification failed", path);
    }

    CloseHandle(file);
    return module;
}

bool PreparePnidui() {
    wchar_t dirPath[MAX_PATH];
    wchar_t filePath[MAX_PATH];
    if (!BuildPaths(dirPath, ARRAYSIZE(dirPath), filePath,
                    ARRAYSIZE(filePath))) {
        return false;
    }

    // GetModuleHandleW with a bare file name matches a module already
    // loaded in this process regardless of which folder it came from, so
    // this also catches another mod's private copy or ExplorerPatcher's
    // bundled DLL. Do this check before touching storage or the network, so
    // the mod doesn't download a file it's then going to refuse to load.
    g_pnidui = GetModuleHandleW(filePath);
    if (!g_pnidui) {
        HMODULE anyPnidui = GetModuleHandleW(kFileName);
        if (anyPnidui) {
            Wh_Log(L"A pnidui.dll is already loaded from another location, "
                   L"not activating a second copy");
            return false;
        }
    }

    if (!g_pnidui) {
        if (!EnsureStorageDirectory(dirPath)) {
            Wh_Log(L"Failed to create storage directory: %u", GetLastError());
            return false;
        }

        if (!EnsurePniduiFile(filePath)) {
            Wh_Log(L"Failed to ensure pnidui.dll is available");
            return false;
        }

        g_pnidui = PinAndLoadLibrary(filePath);
    }

    if (!g_pnidui) {
        Wh_Log(L"Failed to load pnidui.dll: %u", GetLastError());
        return false;
    }

    g_dllGetClassObject = reinterpret_cast<DllGetClassObjectFn>(
        GetProcAddress(g_pnidui, "DllGetClassObject"));
    if (!g_dllGetClassObject) {
        Wh_Log(L"Failed to find DllGetClassObject");
        return false;
    }

    static const char* const kRegGetValueModules[] = {
        "advapi32.dll",
        "api-ms-win-core-registry-l1-1-0.dll",
    };

    g_originalRegGetValue = nullptr;
    for (const char* moduleName : kRegGetValueModules) {
        if (PatchImportByName(
                g_pnidui, moduleName, "RegGetValueW",
                reinterpret_cast<void*>(RegGetValueHook), &g_importSlot,
                reinterpret_cast<void**>(&g_originalRegGetValue))) {
            break;
        }
    }

    if (!g_originalRegGetValue) {
        Wh_Log(L"Warning: failed to patch RegGetValueW import");
    }

    return true;
}

bool StartTray() {
    if (!g_dllGetClassObject) {
        return false;
    }

    IClassFactory* factory = nullptr;
    HRESULT hr =
        g_dllGetClassObject(kClsidNetworkTray, kIidClassFactory,
                             reinterpret_cast<void**>(&factory));
    if (FAILED(hr) || !factory) {
        Wh_Log(L"DllGetClassObject failed: 0x%08X", static_cast<unsigned>(hr));
        return false;
    }

    INetworkTrayPrivate* trayObject = nullptr;
    hr = factory->CreateInstance(nullptr, kIidNetworkTrayPrivate,
                                  reinterpret_cast<void**>(&trayObject));
    factory->Release();

    if (FAILED(hr) || !trayObject) {
        Wh_Log(L"CreateInstance failed: 0x%08X", static_cast<unsigned>(hr));
        return false;
    }

    HLOCAL context = LocalAlloc(LMEM_ZEROINIT, 0x38);
    if (!context) {
        Wh_Log(L"LocalAlloc failed: %u", GetLastError());
        trayObject->Release();
        return false;
    }

    BYTE* base = reinterpret_cast<BYTE*>(context);
    hr = trayObject->Activate(4, base + 0x18, base + 0x10);
    if (FAILED(hr)) {
        Wh_Log(L"Activate failed: 0x%08X", static_cast<unsigned>(hr));
        LocalFree(context);
        trayObject->Release();
        return false;
    }

    g_trayObject = trayObject;
    g_trayContext = context;
    return true;
}

void StopTray() {
    if (g_trayObject) {
        g_trayObject->Cleanup();
    }
    if (g_trayContext) {
        LocalFree(g_trayContext);
        g_trayContext = nullptr;
    }
    if (g_trayObject) {
        g_trayObject->Release();
        g_trayObject = nullptr;
    }
}

// Runs everything that needs the worker thread's COM apartment to be
// initialized: loading/activating the tray object and pumping messages for
// it. Called only after the taskbar-ownership check has passed.
void RunTrayOnStaThread() {
    ComInitGuard comGuard;
    if (!comGuard.Succeeded()) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X",
               static_cast<unsigned>(comGuard.Result()));
        return;
    }

    if (WaitOrStop(500)) {
        return;
    }

    if (!PreparePnidui()) {
        Wh_Log(L"Failed to prepare pnidui.dll");
        return;
    }

    if (!StartTray()) {
        Wh_Log(L"Failed to start legacy network tray");
        return;
    }

    Wh_Log(L"Legacy network tray started");

    while (true) {
        DWORD waitResult = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                                       INFINITE, QS_ALLINPUT);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            MSG msg;
            bool quit = false;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (quit) {
                break;
            }
            continue;
        }

        break;
    }

    StopTray();
}

DWORD WINAPI WorkerThread(LPVOID) {
    Wh_Log(L"Worker thread started");

    if (SystemHasPnidui()) {
        Wh_Log(L"System already provides pnidui.dll, not activating");
        return 0;
    }

    HWND trayWnd = nullptr;
    for (int i = 0; i < 100 && !trayWnd; i++) {
        if (WaitOrStop(100)) {
            Wh_Log(L"Worker thread stopping before tray window was found");
            return 0;
        }
        trayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    }

    if (!trayWnd) {
        Wh_Log(L"Shell_TrayWnd not found, giving up");
        return 0;
    }

    // @include explorer.exe also injects into File Explorer windows that run
    // in their own explorer.exe process (e.g. the "Open folders in a
    // separate process" setting). Shell_TrayWnd always refers to the single
    // real taskbar, so FindWindowW above can find it from any such process.
    // Only the process that actually owns the taskbar window should load
    // and activate the tray object; everyone else must bail out now, before
    // downloading or loading anything.
    DWORD trayPid = 0;
    GetWindowThreadProcessId(trayWnd, &trayPid);
    if (trayPid != GetCurrentProcessId()) {
        Wh_Log(L"The taskbar belongs to another process, not activating");
        return 0;
    }

    RunTrayOnStaThread();

    Wh_Log(L"Worker thread exiting");
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"Failed to create stop event: %u", GetLastError());
        return FALSE;
    }

    g_worker = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"Failed to create worker thread: %u", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Unblock a potential InternetReadFile() call stuck in the worker thread.
    CloseAndClear(g_activeUrl);
    CloseAndClear(g_activeInternet);

    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    if (g_importSlot && g_originalRegGetValue) {
        RestoreImport(g_importSlot, reinterpret_cast<void*>(g_originalRegGetValue));
        g_importSlot = nullptr;
        g_originalRegGetValue = nullptr;
    }

    Wh_Log(L"Uninitialized");
}

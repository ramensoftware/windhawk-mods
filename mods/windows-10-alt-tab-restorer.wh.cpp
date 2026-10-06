// ==WindhawkMod==
// @id              windows-10-alt-tab-restorer
// @name            Windows 10 Alt+Tab Restorer on Windows 11
// @description     Restores the native Windows 10 Alt+Tab switcher on Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @architecture    x86-64
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Alt+Tab Restorer

Restores the Windows 10 Alt+Tab switcher on Windows 11. The mod was tested on
Windows 11 24H2; other builds may work as well, but they are untested. If it
doesn't work, please open an issue so that it can be looked at.

## What it does

* Alt+Tab shows the Windows 10 switcher again: the switcher with the large
  window previews that steps through the windows while Tab is kept pressed.
* Both switchers ship with Windows - the mod only decides which of the two the
  shell uses, and only for Alt+Tab.
* Nothing is drawn by the mod, no keyboard hook is installed, nothing is written
  to the registry and no system file is replaced.

## How it works

Both switchers live in `twinui.pcshell.dll`, which the shell loads into
`explorer.exe`. `CMultitaskingViewManager::_CreateMTVHost` creates either the
XAML host (the Windows 11 switcher) or the DirectComposition host (the Windows 10
switcher), and `IsUndockedAssetAvailable` is the gate that the shell consults for
that decision. The mod:

* redirects `CMultitaskingViewManager::_CreateXamlMTVHost` to
  `CMultitaskingViewManager::_CreateDCompMTVHost` for the Alt+Tab host, and
* answers the gate with "not available" for the Alt+Tab host, so the code paths
  that consult it directly also end up on the Windows 10 host.

Both addresses are resolved with `WindhawkUtils::HookSymbols`, which caches them
per `twinui.pcshell.dll` version, so the symbol lookup is paid once per build and
not on every Explorer start. If a build doesn't expose the symbols, the mod falls
back to the ExplorerPatcher call-site signature of `_CreateMTVHost` on x86-64.

## Notes

* While the mod is active, every read of
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\AltTabSettings` is
  answered with a virtual 0, so a leftover value of 1 cannot send Alt+Tab to the
  legacy Windows XP style dialog and take the Windows 10 host out of the picture.
  The registry itself is not touched. For the same reason the mod conflicts with
  the **Legacy Alt+Tab dialog** mod: enable one or the other.
* Do not combine this mod with another Alt+Tab replacement.
* Windows can keep using a switcher that was already created. If Alt+Tab doesn't
  change after enabling the mod - or doesn't change back after disabling it -
  restart `explorer.exe`.
* Only `explorer.exe` is affected, and only Alt+Tab. Task View (Win+Tab) and the
  other multitasking views are left alone.

## Credits

The gate hook and the XAML to DirectComposition host redirect are the approach
used by [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher) by valinet.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>

// The two host factories of CMultitaskingViewManager. Both take the host kind,
// an application view collection, a view GUID and an out parameter.
using CreateHostFn = HRESULT(WINAPI*)(void* self,
                                      unsigned int kind,
                                      ULONG_PTR arg3,
                                      ULONG_PTR arg4,
                                      void** output);

// ExplorerPatcher's declaration for the gate that selects between the Windows 10
// and the Windows 11 host. It returns a non-zero value when the undocked asset
// (the DirectComposition host) is available.
using IsUndockedAssetAvailableFn =
    int64_t(__cdecl*)(int kind, int64_t arg2, int64_t arg3, const char* arg4);

using RegGetValueWFn = decltype(&RegGetValueW);
using RegQueryValueExWFn = decltype(&RegQueryValueExW);
using NtQueryKeyFn = LONG(NTAPI*)(HANDLE key,
                                  int keyInformationClass,
                                  void* keyInformation,
                                  ULONG length,
                                  ULONG* resultLength);

// The Alt+Tab host of CMultitaskingViewManager. Other kinds are the other
// multitasking views, which the mod leaves alone.
constexpr unsigned int kAltTabHostKind = 1;

// A DirectComposition host that can't be created is retried a few times before
// the mod stops routing and leaves Alt+Tab to the stock Windows 11 switcher for
// the rest of the session.
constexpr int kMaxDcompFailures = 3;

static HMODULE g_twinuiModule = nullptr;

// The DirectComposition factory is called directly, the XAML factory is hooked.
static CreateHostFn g_createDcompHost = nullptr;
static CreateHostFn g_createXamlHostOriginal = nullptr;
static IsUndockedAssetAvailableFn g_isUndockedAssetAvailableOriginal = nullptr;

static std::atomic<bool> g_routingEnabled{false};
static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_routingStopped{false};
static std::atomic<int> g_dcompFailures{0};
static std::atomic<bool> g_loggedFirstHost{false};

static bool IsRouting() {
    return g_routingEnabled.load(std::memory_order_acquire) &&
           !g_routingStopped.load(std::memory_order_acquire) &&
           !g_stopping.load(std::memory_order_acquire);
}

////////////////////////////////////////////////////////////////////////////////
// Alt+Tab host routing
////////////////////////////////////////////////////////////////////////////////

static int64_t __cdecl IsUndockedAssetAvailableHook(int kind,
                                                    int64_t arg2,
                                                    int64_t arg3,
                                                    const char* arg4) {
    // Answering "not available" makes the shell create the DirectComposition
    // host, i.e. the Windows 10 Alt+Tab switcher.
    if (kind == kAltTabHostKind && IsRouting()) {
        return 0;
    }

    return g_isUndockedAssetAvailableOriginal
        ? g_isUndockedAssetAvailableOriginal(kind, arg2, arg3, arg4)
        : 1;
}

static void ReportDcompFailure(unsigned int result, bool hasHost) {
    const int failures =
        g_dcompFailures.fetch_add(1, std::memory_order_relaxed) + 1;
    if (failures < kMaxDcompFailures) {
        Wh_Log(L"The DirectComposition host wasn't created (result 0x%08X, host %s), attempt %d of %d",
               result, hasHost ? L"present" : L"missing", failures,
               kMaxDcompFailures);
        return;
    }

    g_routingStopped.store(true, std::memory_order_release);
    Wh_Log(L"The DirectComposition host wasn't created (result 0x%08X, host %s); leaving Alt+Tab to the stock switcher until explorer.exe restarts",
           result, hasHost ? L"present" : L"missing");
}

static HRESULT WINAPI CreateXamlHostHook(void* self,
                                         unsigned int kind,
                                         ULONG_PTR arg3,
                                         ULONG_PTR arg4,
                                         void** output) {
    if (kind == kAltTabHostKind && IsRouting() && g_createDcompHost) {
        const HRESULT result = g_createDcompHost(self, kind, arg3, arg4, output);
        if (SUCCEEDED(result) && output && *output) {
            g_dcompFailures.store(0, std::memory_order_relaxed);
            if (!g_loggedFirstHost.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"The Windows 10 Alt+Tab switcher is in use");
            }
            return result;
        }

        ReportDcompFailure(static_cast<unsigned int>(result),
                           output && *output);
        return result;
    }

    return g_createXamlHostOriginal
        ? g_createXamlHostOriginal(self, kind, arg3, arg4, output)
        : E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
// AltTabSettings read shim
//
// The Windows 10 host is only used while this shim answers reads of
// HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\AltTabSettings with a
// virtual 0: a leftover value of 1 selects the legacy Windows XP style dialog,
// which isn't the Windows 10 switcher that this mod restores. Only that single
// value under that single key is answered, everything else is passed through,
// and nothing is ever written to the registry.
////////////////////////////////////////////////////////////////////////////////

// NtQueryKey class 3 is KeyNameInformation. It resolves the native path of an
// already opened key, which is what makes it possible to recognize the Explorer
// key when only a handle is available (RegQueryValueExW).
struct KeyNameInformation {
    ULONG nameLength;
    WCHAR name[2048];
};

static RegGetValueWFn g_regGetValueWOriginal = nullptr;
static RegQueryValueExWFn g_regQueryValueExWOriginal = nullptr;
static NtQueryKeyFn g_ntQueryKey = nullptr;
static std::wstring g_currentUserKeyPath;

static std::wstring QueryKeyPath(HKEY key) {
    if (!key || !g_ntQueryKey) {
        return {};
    }

    KeyNameInformation info{};
    ULONG required = 0;
    const LONG status = g_ntQueryKey(key, 3, &info, sizeof(info), &required);
    if (status < 0 || info.nameLength > sizeof(info.name) ||
        info.nameLength % sizeof(WCHAR) != 0) {
        return {};
    }
    return std::wstring(info.name, info.nameLength / sizeof(WCHAR));
}

static std::wstring KeyPath(HKEY key) {
    if (key == HKEY_CURRENT_USER) {
        return g_currentUserKeyPath;
    }
    if (key == HKEY_LOCAL_MACHINE) {
        return L"\\REGISTRY\\MACHINE";
    }
    return QueryKeyPath(key);
}

static std::wstring FullKeyPath(HKEY key, LPCWSTR subkey) {
    std::wstring path = KeyPath(key);
    if (path.empty()) {
        return {};
    }

    if (subkey && *subkey) {
        while (*subkey == L'\\') {
            ++subkey;
        }
        if (!path.empty() && path.back() != L'\\') {
            path.push_back(L'\\');
        }
        path += subkey;
    }

    while (!path.empty() && path.back() == L'\\') {
        path.pop_back();
    }
    return path;
}

static bool IsExplorerSettingsKey(HKEY key, LPCWSTR subkey) {
    if (g_currentUserKeyPath.empty()) {
        return false;
    }

    const std::wstring expected = g_currentUserKeyPath +
        L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer";
    return _wcsicmp(FullKeyPath(key, subkey).c_str(), expected.c_str()) == 0;
}

static bool InitializeCurrentUserPath() {
    const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    g_ntQueryKey = ntdll
        ? reinterpret_cast<NtQueryKeyFn>(GetProcAddress(ntdll, "NtQueryKey"))
        : nullptr;
    if (!g_ntQueryKey) {
        Wh_Log(L"NtQueryKey isn't available; AltTabSettings reads can't be recognized safely");
        return false;
    }

    HKEY currentUser = nullptr;
    if (RegOpenCurrentUser(KEY_QUERY_VALUE, &currentUser) != ERROR_SUCCESS) {
        Wh_Log(L"RegOpenCurrentUser failed; the AltTabSettings read shim isn't available");
        return false;
    }
    g_currentUserKeyPath = QueryKeyPath(currentUser);
    RegCloseKey(currentUser);

    if (g_currentUserKeyPath.empty()) {
        Wh_Log(L"The registry path of the current user couldn't be resolved");
        return false;
    }
    return true;
}

// Answers a read of AltTabSettings with the virtual DWORD 0, following the
// documented buffer, size and flag semantics of both read functions. This is an
// in-memory answer; the registry is never written to.
static LSTATUS CopyAltTabSettingsZero(bool getValue,
                                      DWORD flags,
                                      LPDWORD type,
                                      PVOID data,
                                      LPDWORD bytes) {
    const DWORD capacity = bytes ? *bytes : 0;
    if (data && !bytes) {
        return ERROR_INVALID_PARAMETER;
    }

    if (getValue) {
        constexpr DWORD allowedFlags = RRF_RT_ANY | RRF_NOEXPAND |
            RRF_ZEROONFAILURE | RRF_SUBKEY_WOW6432KEY | RRF_SUBKEY_WOW6464KEY;
        if ((flags & ~allowedFlags) ||
            ((flags & RRF_SUBKEY_WOW6432KEY) &&
             (flags & RRF_SUBKEY_WOW6464KEY))) {
            return ERROR_INVALID_PARAMETER;
        }

        const DWORD typeFilter = flags & RRF_RT_ANY;
        if (typeFilter && !(typeFilter & RRF_RT_REG_DWORD)) {
            if ((flags & RRF_ZEROONFAILURE) && data && bytes && capacity) {
                memset(data, 0, capacity);
            }
            return ERROR_UNSUPPORTED_TYPE;
        }
    }

    if (type) {
        *type = REG_DWORD;
    }
    if (bytes) {
        *bytes = sizeof(DWORD);
    }
    if (!data) {
        return ERROR_SUCCESS;
    }
    if (capacity < sizeof(DWORD)) {
        if (getValue && (flags & RRF_ZEROONFAILURE) && capacity) {
            memset(data, 0, capacity);
        }
        return ERROR_MORE_DATA;
    }

    const DWORD zero = 0;
    memcpy(data, &zero, sizeof(zero));
    return ERROR_SUCCESS;
}

static bool TryReadAltTabSettings(HKEY key,
                                  LPCWSTR subkey,
                                  LPCWSTR valueName,
                                  bool getValue,
                                  DWORD flags,
                                  LPDWORD type,
                                  PVOID data,
                                  LPDWORD bytes,
                                  LSTATUS* result) {
    if (!IsRouting() || !valueName ||
        _wcsicmp(valueName, L"AltTabSettings") != 0 ||
        !IsExplorerSettingsKey(key, subkey)) {
        return false;
    }

    *result = CopyAltTabSettingsZero(getValue, flags, type, data, bytes);
    return true;
}

static LSTATUS WINAPI RegGetValueWHook(HKEY key,
                                       LPCWSTR subkey,
                                       LPCWSTR valueName,
                                       DWORD flags,
                                       LPDWORD type,
                                       PVOID data,
                                       LPDWORD bytes) {
    try {
        LSTATUS result = ERROR_SUCCESS;
        if (TryReadAltTabSettings(key, subkey, valueName, true, flags, type,
                                  data, bytes, &result)) {
            return result;
        }
    } catch (...) {
        Wh_Log(L"The AltTabSettings read shim failed; using the real registry");
    }

    return g_regGetValueWOriginal
        ? g_regGetValueWOriginal(key, subkey, valueName, flags, type, data,
                                 bytes)
        : ERROR_PROC_NOT_FOUND;
}

static LSTATUS WINAPI RegQueryValueExWHook(HKEY key,
                                           LPCWSTR valueName,
                                           LPDWORD reserved,
                                           LPDWORD type,
                                           LPBYTE data,
                                           LPDWORD bytes) {
    try {
        // A non-null reserved parameter is invalid and must reach the original.
        if (!reserved) {
            LSTATUS result = ERROR_SUCCESS;
            if (TryReadAltTabSettings(key, nullptr, valueName, false, 0, type,
                                      data, bytes, &result)) {
                return result;
            }
        }
    } catch (...) {
        Wh_Log(L"The AltTabSettings read shim failed; using the real registry");
    }

    return g_regQueryValueExWOriginal
        ? g_regQueryValueExWOriginal(key, valueName, reserved, type, data,
                                     bytes)
        : ERROR_PROC_NOT_FOUND;
}

static bool InstallRegistryReadHooks() {
    const HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        Wh_Log(L"kernelbase.dll isn't loaded; the AltTabSettings read shim isn't available");
        return false;
    }

    auto regGetValue = reinterpret_cast<RegGetValueWFn>(
        GetProcAddress(kernelBase, "RegGetValueW"));
    auto regQueryValue = reinterpret_cast<RegQueryValueExWFn>(
        GetProcAddress(kernelBase, "RegQueryValueExW"));
    if (!regGetValue || !regQueryValue) {
        Wh_Log(L"RegGetValueW or RegQueryValueExW couldn't be resolved");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(regGetValue, RegGetValueWHook,
                                        &g_regGetValueWOriginal) ||
        !WindhawkUtils::SetFunctionHook(regQueryValue, RegQueryValueExWHook,
                                        &g_regQueryValueExWOriginal)) {
        Wh_Log(L"One of the AltTabSettings read hooks couldn't be installed");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Symbol resolution
////////////////////////////////////////////////////////////////////////////////

// The undecorated names of the three functions that the mod needs, as they
// appear in the twinui.pcshell.dll PDB. HookSymbols matches the names as they
// are and caches the result per binary version; alternative spellings are tried
// in order, and an entry that isn't found is not an error as long as the other
// route is available.
// twinui.pcshell.dll
const WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
    {
        {LR"(private: long __cdecl CMultitaskingViewManager::_CreateDCompMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))",
         LR"(private: long __cdecl CMultitaskingViewManager::_CreateDCompMTVHost(unsigned int,struct IApplicationViewCollection *,struct _GUID const &,void * *))",
         LR"(long __cdecl CMultitaskingViewManager::_CreateDCompMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))"},
        &g_createDcompHost,
        nullptr,
        true,
    },
    {
        {LR"(private: long __cdecl CMultitaskingViewManager::_CreateXamlMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))",
         LR"(private: long __cdecl CMultitaskingViewManager::_CreateXamlMTVHost(unsigned int,struct IApplicationViewCollection *,struct _GUID const &,void * *))",
         LR"(long __cdecl CMultitaskingViewManager::_CreateXamlMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))"},
        &g_createXamlHostOriginal,
        CreateXamlHostHook,
        true,
    },
    {
        {LR"(__int64 __cdecl IsUndockedAssetAvailable(int,__int64,__int64,char const *))",
         LR"(__int64 __cdecl IsUndockedAssetAvailable(int,unsigned __int64,unsigned __int64,char const *))"},
        &g_isUndockedAssetAvailableOriginal,
        IsUndockedAssetAvailableHook,
        true,
    },
};

static void ResolveBySymbols() {
    if (!WindhawkUtils::HookSymbols(g_twinuiModule, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Not every Alt+Tab symbol of twinui.pcshell.dll could be resolved");
    }
}

#if defined(_M_X64)
////////////////////////////////////////////////////////////////////////////////
// Call-site fallback (x86-64)
//
// Used only when the symbols of a build can't be resolved. The two factories are
// found through the two branches of the `GetMTVHostKind() == 1` check in
// CMultitaskingViewManager::_CreateMTVHost, the way ExplorerPatcher does.
////////////////////////////////////////////////////////////////////////////////

struct TextSection {
    const uint8_t* begin = nullptr;
    size_t size = 0;
};

static bool GetTextSection(HMODULE module, TextSection* section) {
    if (!module || !section) {
        return false;
    }

    const auto* base = reinterpret_cast<const uint8_t*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) {
        return false;
    }
    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        return false;
    }

    const IMAGE_SECTION_HEADER* imageSection = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++imageSection) {
        char name[IMAGE_SIZEOF_SHORT_NAME + 1] = {};
        memcpy(name, imageSection->Name, IMAGE_SIZEOF_SHORT_NAME);
        if (strcmp(name, ".text") != 0 ||
            !(imageSection->Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            continue;
        }

        size_t size = imageSection->Misc.VirtualSize;
        if (!size) {
            size = imageSection->SizeOfRawData;
        }
        if (!size ||
            imageSection->VirtualAddress >= nt->OptionalHeader.SizeOfImage ||
            size > nt->OptionalHeader.SizeOfImage -
                       imageSection->VirtualAddress) {
            return false;
        }

        section->begin = base + imageSection->VirtualAddress;
        section->size = size;
        return true;
    }
    return false;
}

static bool FindUniquePattern(const TextSection& section,
                              const uint8_t* pattern,
                              const char* mask,
                              size_t patternSize,
                              const uint8_t** matchOut) {
    if (!section.begin || !pattern || !mask || patternSize > section.size) {
        return false;
    }

    const uint8_t* match = nullptr;
    for (size_t i = 0; i <= section.size - patternSize; ++i) {
        bool matches = true;
        for (size_t j = 0; j < patternSize; ++j) {
            if (mask[j] == 'x' && section.begin[i + j] != pattern[j]) {
                matches = false;
                break;
            }
        }
        if (matches) {
            if (match) {
                return false;  // Ambiguous call site, refuse it.
            }
            match = section.begin + i;
        }
    }

    if (!match) {
        return false;
    }
    *matchOut = match;
    return true;
}

static bool IsInTextSection(const TextSection& section,
                            const uint8_t* address,
                            size_t length = 1) {
    const uintptr_t begin = reinterpret_cast<uintptr_t>(section.begin);
    const uintptr_t value = reinterpret_cast<uintptr_t>(address);
    return value >= begin && value - begin <= section.size &&
           length <= section.size - (value - begin);
}

static bool RelativeCallTarget(const TextSection& section,
                               const uint8_t* call,
                               void** targetOut) {
    if (!IsInTextSection(section, call, 5) || call[0] != 0xE8) {
        return false;
    }

    int32_t displacement = 0;
    memcpy(&displacement, call + 1, sizeof(displacement));
    const auto* target = call + 5 + displacement;
    if (!IsInTextSection(section, target)) {
        return false;
    }
    *targetOut = const_cast<uint8_t*>(target);
    return true;
}

static bool FollowJnz(const TextSection& section,
                      const uint8_t* instruction,
                      const uint8_t** targetOut,
                      size_t* instructionSizeOut) {
    if (IsInTextSection(section, instruction, 2) && instruction[0] == 0x75) {
        const auto* target =
            instruction + 2 + static_cast<int8_t>(instruction[1]);
        if (!IsInTextSection(section, target)) {
            return false;
        }
        *targetOut = target;
        *instructionSizeOut = 2;
        return true;
    }
    if (IsInTextSection(section, instruction, 6) && instruction[0] == 0x0F &&
        instruction[1] == 0x85) {
        int32_t displacement = 0;
        memcpy(&displacement, instruction + 2, sizeof(displacement));
        const auto* target = instruction + 6 + displacement;
        if (!IsInTextSection(section, target)) {
            return false;
        }
        *targetOut = target;
        *instructionSizeOut = 6;
        return true;
    }
    return false;
}

// _CreateMTVHost with the host factories inlined into it.
static bool FindInlinedFactories(const TextSection& section,
                                 void** xamlOut,
                                 void** dcompOut) {
    static const uint8_t xamlPattern[] = {
        0x4C, 0x89, 0x74, 0x24, 0, 0, 0x8B, 0, 0, 0x8B, 0, 0x8B,
        0xD7, 0x48, 0x8B, 0xCE, 0xE8, 0, 0, 0, 0, 0x8B};
    static const uint8_t dcompPattern[] = {
        0x4C, 0x89, 0x74, 0x24, 0, 0, 0x8B, 0, 0, 0x8B, 0, 0x8B,
        0xD7, 0x48, 0x8B, 0xCE, 0xE8, 0, 0, 0, 0, 0x90};
    static const char mask[] = "xxxx??x??x?xxxxxx????x";

    const uint8_t* xamlMatch = nullptr;
    const uint8_t* dcompMatch = nullptr;
    if (!FindUniquePattern(section, xamlPattern, mask, sizeof(xamlPattern),
                           &xamlMatch) ||
        !FindUniquePattern(section, dcompPattern, mask, sizeof(dcompPattern),
                           &dcompMatch)) {
        return false;
    }

    void* xaml = nullptr;
    void* dcomp = nullptr;
    if (!RelativeCallTarget(section, xamlMatch + 16, &xaml) ||
        !RelativeCallTarget(section, dcompMatch + 16, &dcomp) ||
        xaml == dcomp) {
        return false;
    }

    *xamlOut = xaml;
    *dcompOut = dcomp;
    return true;
}

// _CreateMTVHost as a function of its own, whose two branches call the factories.
static bool FindNonInlinedFactories(const TextSection& section,
                                    void** xamlOut,
                                    void** dcompOut) {
    // GetMTVHostKind(); compare the result with 1; the JNZ and the fall-through
    // each call one of the two host factories.
    static const uint8_t pattern[] = {
        0x8B, 0xCF, 0xE8, 0, 0, 0, 0, 0, 0x89, 0, 0x24, 0, 0,
        0x8B, 0, 0, 0x8B, 0, 0x8B, 0xD7, 0x48, 0x8B, 0xCE,
        0x83, 0xF8, 0x01};
    static const char mask[] = "xxx?????x?x??x??x?xxxxxxxx";

    const uint8_t* match = nullptr;
    if (!FindUniquePattern(section, pattern, mask, sizeof(pattern), &match)) {
        return false;
    }

    const uint8_t* branch = match + 26;
    const uint8_t* branchTarget = nullptr;
    size_t branchSize = 0;
    if (!FollowJnz(section, branch, &branchTarget, &branchSize)) {
        return false;
    }

    void* xaml = nullptr;
    void* dcomp = nullptr;
    if (!RelativeCallTarget(section, branch + branchSize, &xaml) ||
        !RelativeCallTarget(section, branchTarget, &dcomp) || xaml == dcomp) {
        return false;
    }

    *xamlOut = xaml;
    *dcompOut = dcomp;
    return true;
}

static bool ResolveByCallSites() {
    // The symbol of the XAML factory was already resolved and hooked; the
    // call-site addresses are only a replacement for the symbol route.
    if (g_createXamlHostOriginal) {
        return g_createDcompHost != nullptr;
    }

    TextSection section;
    if (!GetTextSection(g_twinuiModule, &section)) {
        Wh_Log(L"The .text section of twinui.pcshell.dll couldn't be read");
        return false;
    }

    void* xaml = nullptr;
    void* dcomp = nullptr;
    if (!FindInlinedFactories(section, &xaml, &dcomp) &&
        !FindNonInlinedFactories(section, &xaml, &dcomp)) {
        Wh_Log(L"The Alt+Tab host factories weren't found by their call sites");
        return false;
    }

    g_createDcompHost = reinterpret_cast<CreateHostFn>(dcomp);
    if (!WindhawkUtils::SetFunctionHook(reinterpret_cast<CreateHostFn>(xaml),
                                        CreateXamlHostHook,
                                        &g_createXamlHostOriginal)) {
        Wh_Log(L"The XAML host factory couldn't be hooked");
        g_createXamlHostOriginal = nullptr;
        return false;
    }

    return true;
}
#endif  // defined(_M_X64)

////////////////////////////////////////////////////////////////////////////////
// Mod lifetime
////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    // The read shim is installed first so that it is in place no matter which
    // code path resolves the host. It only answers while routing is enabled, so
    // on its own it can't change the switcher.
    if (!InitializeCurrentUserPath() || !InstallRegistryReadHooks()) {
        Wh_Log(L"The AltTabSettings read shim isn't available; a non-zero AltTabSettings may interfere with the Windows 10 host");
    }

    g_twinuiModule = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_twinuiModule) {
        Wh_Log(L"twinui.pcshell.dll couldn't be loaded");
        return FALSE;
    }

    ResolveBySymbols();

    const bool hasGate = g_isUndockedAssetAvailableOriginal != nullptr;
    bool hasRedirect =
        g_createDcompHost != nullptr && g_createXamlHostOriginal != nullptr;

#if defined(_M_X64)
    // Only when the symbols of the build didn't resolve. The call-site addresses
    // can't provide the gate, so the redirect is the only route then.
    if (!hasRedirect) {
        hasRedirect = ResolveByCallSites();
    }
#endif

    if (!hasGate && !hasRedirect) {
        Wh_Log(L"No Alt+Tab entry point of twinui.pcshell.dll could be resolved; the mod isn't activated");
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
        return FALSE;
    }

    Wh_Log(L"Routing ready (gate: %s, XAML to DirectComposition redirect: %s)",
           hasGate ? L"yes" : L"no", hasRedirect ? L"yes" : L"no");
    g_routingEnabled.store(true, std::memory_order_release);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_routingEnabled.store(false, std::memory_order_release);
}

void Wh_ModUninit() {
    if (g_twinuiModule) {
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
    }
}

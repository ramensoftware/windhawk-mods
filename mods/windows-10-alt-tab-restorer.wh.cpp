// ==WindhawkMod==
// @id              windows-10-alt-tab-restorer
// @name            Windows 10 Alt+Tab Restorer on Windows 11
// @description     This mod restores the native Windows 10 Alt+Tab switcher on Windows 11
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

This mod restores the classic Windows 10 Alt+Tab switcher on Windows 11. This mod has been tested on Windows 11 24H2 and may function on other versions. If not, please open an issue to inform the author about it so that the mod can be enhanced.

## What it does

- It restores the native Windows 10 Alt+Tab switcher when you press Alt+Tab.
- It uses the built-in Windows components, so no custom window or panel is drawn.
- It does not install a global keyboard hook.
- It does not write to the registry.

## Requirements

- Windows 11
- 64-bit `explorer.exe`.
- Windhawk installed.

## How to use

1. Install the mod in Windhawk and enable it.
2. Press Alt+Tab. The Windows 10 style switcher should appear.
3. To remove it, disable or uninstall the mod in Windhawk.

## Notes

- Do not use this mod together with another Alt+Tab replacement mod.
- The mod only targets `explorer.exe` and only affects the Alt+Tab switcher.
- While the mod is active it answers reads of `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\AltTabSettings` with 0, so the mod takes precedence over that setting. For the same reason it conflicts with the **Legacy Alt+Tab dialog** mod: enable one or the other.
- If Alt+Tab does not change, check the Windhawk log for the entries of this mod and open an issue to notify the author of the modification about the problem.

The gate hook and the XAML to DirectComposition host redirect are the approach used by [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher) by valinet.
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

// The manager function that hands the host back to the shell. It is hooked
// only for the E_UNEXPECTED safety net below; the argument order is the one
// ExplorerPatcher forwards from it to the host factories.
using CreateMtvHostFn = HRESULT(WINAPI*)(void* self,
                                         unsigned int kind,
                                         void* viewCollection,
                                         const GUID* viewId,
                                         void** output);

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

// The addresses that the symbol lookup returns, before any hook is installed.
static void* g_createDcompAddress = nullptr;
static void* g_createXamlAddress = nullptr;
static void* g_gateAddress = nullptr;
static void* g_createMtvHostAddress = nullptr;

// g_createDcompHost is either the trampoline of the counting hook below or the
// address of the factory itself, whichever is available.
static CreateHostFn g_createDcompHost = nullptr;
static CreateHostFn g_createDcompHostOriginal = nullptr;
static CreateHostFn g_createXamlHostOriginal = nullptr;
static IsUndockedAssetAvailableFn g_isUndockedAssetAvailableOriginal = nullptr;
static CreateMtvHostFn g_createMtvHostOriginal = nullptr;

// Counts the calls that reached one of the two host factories, so that the
// safety net can tell whether the manager gave up before calling either of
// them. The DirectComposition factory is counted by its own hook: the manager
// can call it directly through the gate, and such a call has to be visible
// here, otherwise the safety net would call the same factory a second time
// right after it failed.
static std::atomic<unsigned long long> g_factoryEntries{0};

// Set only when the safety net is armed in a state where every call to a host
// factory is visible in g_factoryEntries.
static std::atomic<bool> g_safetyNetEnabled{false};

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
    g_factoryEntries.fetch_add(1, std::memory_order_relaxed);

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

// Safety net for the case that was seen on Windows 11 24H2: the manager gives
// up with E_UNEXPECTED before it calls either host factory, with a null view
// collection, so no host is created and Alt+Tab has nothing to show. The native
// DirectComposition factory is then called directly, with the same argument
// order that the manager uses for the XAML factory - the redirect above
// forwards those arguments the same way, which is also what ExplorerPatcher
// does. The call is made only when the manager really didn't reach a factory
// and produced no host, and a few failures in a row turn the routing off for
// the rest of the session instead of retrying forever.
static HRESULT WINAPI CreateMtvHostHook(void* self,
                                        unsigned int kind,
                                        void* viewCollection,
                                        const GUID* viewId,
                                        void** output) {
    const unsigned long long entriesBefore =
        g_factoryEntries.load(std::memory_order_relaxed);
    const HRESULT result = g_createMtvHostOriginal
        ? g_createMtvHostOriginal(self, kind, viewCollection, viewId, output)
        : E_FAIL;

    if (kind == kAltTabHostKind && result == E_UNEXPECTED && IsRouting() &&
        g_safetyNetEnabled.load(std::memory_order_acquire) &&
        g_createDcompHost && viewId && output && !*output &&
        g_factoryEntries.load(std::memory_order_relaxed) == entriesBefore) {
        Wh_Log(L"_CreateMTVHost gave up before either host factory; creating the DirectComposition host directly");
        const HRESULT dcompResult = g_createDcompHost(
            self, kind, reinterpret_cast<ULONG_PTR>(viewCollection),
            reinterpret_cast<ULONG_PTR>(viewId), output);
        if (SUCCEEDED(dcompResult) && output && *output) {
            g_dcompFailures.store(0, std::memory_order_relaxed);
            return dcompResult;
        }

        ReportDcompFailure(static_cast<unsigned int>(dcompResult),
                           output && *output);
    }

    return result;
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

// The names of the functions that the mod needs, as they appear in the
// twinui.pcshell.dll PDB. HookSymbols matches these strings as they are and
// caches the result per binary version, so the potentially slow symbol handling
// is paid once per build. The names are the undecorated ones, exactly as the
// Windhawk Symbol Helper reports them for twinui.pcshell.dll - a mangled name
// would encode the parameter types, so a wrong one would silently ask for a
// function with a different prototype, while an undecorated name that doesn't
// match simply doesn't resolve. Every entry is optional: a name that isn't
// present on a build disables only the mechanism that needs it.
//
// On Windows 11 24H2 the helper reports _CreateMTVHost, _CreateDCompMTVHost,
// _CreateXamlMTVHost, CreateMultitaskingView and CreateMultitaskingViewWithFilter,
// but no IsUndockedAssetAvailable at all - the gate isn't in that build's PDB,
// which is why the mod can also work through the host manager alone there.
// twinui.pcshell.dll
const WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
    {
        {LR"(private: long __cdecl CMultitaskingViewManager::_CreateDCompMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))"},
        &g_createDcompAddress,
        nullptr,
        true,
    },
    {
        {LR"(private: long __cdecl CMultitaskingViewManager::_CreateXamlMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))"},
        &g_createXamlAddress,
        nullptr,
        true,
    },
    {
        // Not present in the 24H2 PDB; listed for builds that still expose the
        // gate by name. If it isn't found, the gate hook isn't installed and
        // the mod falls back to the host manager below.
        {LR"(__int64 __cdecl IsUndockedAssetAvailable(int,__int64,__int64,char const *))"},
        &g_gateAddress,
        nullptr,
        true,
    },
    {
        // Only used by the E_UNEXPECTED safety net.
        {LR"(private: long __cdecl CMultitaskingViewManager::_CreateMTVHost(enum MULTITASKING_VIEW_TYPES,struct IApplicationViewCollection *,struct _GUID const &,void * *))"},
        &g_createMtvHostAddress,
        nullptr,
        true,
    },
};

static void ResolveBySymbols() {
    if (!WindhawkUtils::HookSymbols(g_twinuiModule, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Not every Alt+Tab symbol of twinui.pcshell.dll could be resolved");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Hook installation
//
// The hooks are installed only once everything was resolved, and the XAML
// redirect only together with the gate: the redirect on its own leaves the
// shell with a host that it doesn't expect, which is worse than the stock
// switcher. The DirectComposition factory isn't hooked; the redirect and the
// safety net call it directly and look at the result.
////////////////////////////////////////////////////////////////////////////////

static bool InstallGateHook() {
    // Not resolved on this build; already reported by the resolution log.
    if (!g_gateAddress) {
        return false;
    }
    if (g_isUndockedAssetAvailableOriginal) {
        return true;
    }
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<IsUndockedAssetAvailableFn>(g_gateAddress),
            IsUndockedAssetAvailableHook,
            &g_isUndockedAssetAvailableOriginal)) {
        Wh_Log(L"The Alt+Tab host gate couldn't be hooked");
        g_isUndockedAssetAvailableOriginal = nullptr;
        return false;
    }
    return true;
}

static bool InstallXamlHook() {
    if (!g_createXamlAddress) {
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateHostFn>(g_createXamlAddress),
            CreateXamlHostHook, &g_createXamlHostOriginal)) {
        Wh_Log(L"The XAML host factory couldn't be hooked");
        g_createXamlHostOriginal = nullptr;
        return false;
    }
    return true;
}

static bool InstallMtvHostHook() {
    if (!g_createMtvHostAddress) {
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateMtvHostFn>(g_createMtvHostAddress),
            CreateMtvHostHook, &g_createMtvHostOriginal)) {
        Wh_Log(L"The host manager couldn't be hooked");
        g_createMtvHostOriginal = nullptr;
        return false;
    }
    return true;
}

// A pass-through hook that only counts the calls to the DirectComposition
// factory, so that the safety net can see the calls that the manager makes
// through the gate. The mod calls the factory through the trampoline, so the
// count stays the manager's alone.
static HRESULT WINAPI CreateDcompHostHook(void* self,
                                          unsigned int kind,
                                          ULONG_PTR arg3,
                                          ULONG_PTR arg4,
                                          void** output) {
    g_factoryEntries.fetch_add(1, std::memory_order_relaxed);
    return g_createDcompHostOriginal
        ? g_createDcompHostOriginal(self, kind, arg3, arg4, output)
        : E_FAIL;
}

static bool InstallDcompCountingHook() {
    if (!g_createDcompAddress ||
        !WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateHostFn>(g_createDcompAddress),
            CreateDcompHostHook, &g_createDcompHostOriginal)) {
        g_createDcompHostOriginal = nullptr;
        return false;
    }
    return true;
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
    // The factories were already resolved; the call-site addresses are only a
    // replacement for the symbol routes.
    if (g_createXamlAddress && g_createDcompAddress) {
        return true;
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

    g_createDcompAddress = dcomp;
    g_createXamlAddress = xaml;
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
    Wh_Log(L"Resolved twinui.pcshell.dll entry points (gate: %s, XAML factory: %s, DirectComposition factory: %s, host manager: %s)",
           g_gateAddress ? L"found" : L"not found",
           g_createXamlAddress ? L"found" : L"not found",
           g_createDcompAddress ? L"found" : L"not found",
           g_createMtvHostAddress ? L"found" : L"not found");

#if defined(_M_X64)
    // Last resort, and only for the two factories: the call-site addresses
    // can't provide the gate.
    if (!g_createDcompAddress || !g_createXamlAddress) {
        ResolveByCallSites();
    }
#endif

    // The gate is what makes the shell pick the Windows 10 host, and the
    // redirect covers the call sites that go straight to the XAML factory. The
    // redirect is only useful, and only safe, together with the gate; the
    // safety net needs the DirectComposition factory. The installation results
    // are checked, so that the mod can't announce a route that isn't there.
    const bool hasPair =
        g_createDcompAddress != nullptr && g_createXamlAddress != nullptr;
    const bool hasManager =
        g_createMtvHostAddress != nullptr && g_createDcompAddress != nullptr;

    const bool gateHook = InstallGateHook();
    const bool redirectHook = gateHook && hasPair && InstallXamlHook();
    const bool managerHook = hasManager && InstallMtvHostHook();

    if (!g_createDcompHost && g_createDcompAddress) {
        g_createDcompHost = reinterpret_cast<CreateHostFn>(g_createDcompAddress);
    }

    // The safety net fires only when the call it makes wasn't already made by
    // the manager, so it is armed only while the calls to the factories are
    // visible in g_factoryEntries. The counting hook on the DirectComposition
    // factory provides that; without it, the net is armed only when the gate
    // itself isn't available, which is the Windows 11 24H2 case above - there
    // the manager doesn't reach a factory at all and the net is the only
    // mechanism that restores the switcher.
    if (managerHook && InstallDcompCountingHook()) {
        g_createDcompHost = g_createDcompHostOriginal;
        g_safetyNetEnabled.store(true, std::memory_order_release);
    } else if (managerHook && !gateHook) {
        Wh_Log(L"The DirectComposition factory couldn't be hooked for counting; the safety net is armed anyway, because without the gate it is the only mechanism on this build");
        g_safetyNetEnabled.store(true, std::memory_order_release);
    } else if (managerHook) {
        Wh_Log(L"The DirectComposition factory couldn't be hooked for counting; the safety net stays disarmed, because the manager may have called the factory already");
    }

    // A working route is either the gate with the redirect, or the safety net.
    // The gate alone is not enough: call sites that go straight to the XAML
    // factory would still create the Windows 11 host.
    const bool hasRoute =
        (gateHook && redirectHook) ||
        g_safetyNetEnabled.load(std::memory_order_acquire);

    if (!hasRoute) {
        Wh_Log(L"No working route to the Windows 10 Alt+Tab host could be installed (gate: %s, XAML to DirectComposition redirect: %s, host manager safety net: %s); the mod isn't activated and Alt+Tab is left as it is",
               gateHook ? L"hooked" : L"not available",
               redirectHook ? L"hooked" : L"not available",
               g_safetyNetEnabled.load(std::memory_order_acquire) ? L"armed"
                                                                 : L"not available");
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
        return FALSE;
    }

    Wh_Log(L"Routing ready (gate: %s, XAML to DirectComposition redirect: %s, host manager safety net: %s)",
           gateHook ? L"hooked" : L"not available",
           redirectHook ? L"hooked" : L"not available",
           g_safetyNetEnabled.load(std::memory_order_acquire) ? L"armed"
                                                             : L"not available");
    g_routingEnabled.store(true, std::memory_order_release);
    return TRUE;
}


void Wh_ModBeforeUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_routingEnabled.store(false, std::memory_order_release);
    g_safetyNetEnabled.store(false, std::memory_order_release);
}

void Wh_ModUninit() {
    if (g_twinuiModule) {
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
    }
}

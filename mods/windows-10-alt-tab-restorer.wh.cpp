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
- If Alt+Tab does not change, check the Windhawk log for entries starting with `[Win10 Alt+Tab]` and open an issue to notify the author of the modification about the problem.
*/
// ==/WindhawkModReadme==


#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

using CreateHostFn = HRESULT(WINAPI*)(
    void* self,
    unsigned int kind,
    ULONG_PTR arg3,
    ULONG_PTR arg4,
    void** output);
using RegisterHotKeysFn = HRESULT(WINAPI*)(void* self);
using ShouldShowAltTabFn = bool(WINAPI*)(void* self);
using OnAltTabFn = HRESULT(WINAPI*)(void* self, unsigned int hotKeyId);
using AltTabHostShowFn = HRESULT(WINAPI*)(
    void* self, void* monitor, unsigned int flags, void* applicationView);
using CreateMultitaskingViewFn = HRESULT(WINAPI*)(
    void* self, unsigned int kind, const GUID* viewId, void** output);
using CreateMultitaskingViewWithFilterFn = HRESULT(WINAPI*)(
    void* self, unsigned int kind, void* viewCollection,
    const GUID* viewId, void** output);
using CreateMtvHostFn = HRESULT(WINAPI*)(
    void* self, unsigned int kind, void* viewCollection,
    const GUID* viewId, void** output);
// ExplorerPatcher's public declaration for twinui.pcshell.dll's gate.
using IsUndockedAssetAvailableFn = int64_t(*)(
    int kind, int64_t arg2, int64_t arg3, const char* arg4);
using RegGetValueWFn = decltype(&RegGetValueW);
using RegQueryValueExWFn = decltype(&RegQueryValueExW);
using NtQueryKeyFn = LONG(NTAPI*)(HANDLE, int, void*, ULONG, ULONG*);

static HMODULE g_twinuiModule = nullptr;
static CreateHostFn g_xamlHostOriginal = nullptr;
static CreateHostFn g_dcompHostOriginal = nullptr;
static RegisterHotKeysFn g_registerHotKeysOriginal = nullptr;
static ShouldShowAltTabFn g_shouldShowAltTabOriginal = nullptr;
static OnAltTabFn g_onAltTabOriginal = nullptr;
static AltTabHostShowFn g_altTabHostShowOriginal = nullptr;
static CreateMultitaskingViewFn g_createMultitaskingViewOriginal = nullptr;
static CreateMultitaskingViewWithFilterFn g_createMultitaskingViewWithFilterOriginal = nullptr;
static CreateMtvHostFn g_createMtvHostOriginal = nullptr;
static IsUndockedAssetAvailableFn g_isUndockedAssetAvailableOriginal = nullptr;
static RegGetValueWFn g_regGetValueOriginal = nullptr;
static RegQueryValueExWFn g_regQueryValueOriginal = nullptr;
static NtQueryKeyFn g_ntQueryKey = nullptr;
static std::wstring g_currentUserNativePath;
static std::atomic<bool> g_explorerProcess{false};
static std::atomic<bool> g_altTabReady{false};
static std::atomic<bool> g_altTabFaulted{false};
static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_loggedFactoryEntry{false};
static std::atomic<bool> g_loggedAltTabFactoryEntry{false};
static std::atomic<bool> g_loggedUndockedGate{false};
static std::atomic<bool> g_loggedDcompAltTab{false};
static std::atomic<bool> g_loggedNativeHost{false};
static std::atomic<bool> g_loggedRegistryOverride{false};
static std::atomic<unsigned> g_shouldShowCallCount{0};
static std::atomic<unsigned> g_onAltTabCallCount{0};
static std::atomic<unsigned> g_altTabShowCallCount{0};
static std::atomic<unsigned> g_managerCreateCallCount{0};
static std::atomic<unsigned> g_managerCreateWithFilterCallCount{0};
static std::atomic<unsigned> g_managerHostCallCount{0};
static std::atomic<unsigned long long> g_altTabFactoryEntrySequence{0};

static void LogCurrentProcess() {
    // Fake Explorer path mods commonly spoof GetModuleFileNameW(NULL). Query the
    // process image independently first, then log the spoofable value too so the
    // two can be compared without disabling that mod.
    wchar_t actualPath[MAX_PATH] = {};
    DWORD actualLength = ARRAYSIZE(actualPath);
    if (!QueryFullProcessImageNameW(
            GetCurrentProcess(), 0, actualPath, &actualLength)) {
        // Passing the real executable module handle also bypasses hooks that
        // only rewrite the NULL-handle form of GetModuleFileNameW.
        const HMODULE executable = GetModuleHandleW(nullptr);
        const DWORD length = executable
            ? GetModuleFileNameW(executable, actualPath, ARRAYSIZE(actualPath))
            : 0;
        if (!length || length >= ARRAYSIZE(actualPath)) {
            actualPath[0] = L'\0';
        }
    }

    wchar_t reportedPath[MAX_PATH] = {};
    const DWORD reportedLength = GetModuleFileNameW(
        nullptr, reportedPath, ARRAYSIZE(reportedPath));
    if (!reportedLength || reportedLength >= ARRAYSIZE(reportedPath)) {
        reportedPath[0] = L'\0';
    }

    const wchar_t* actual = actualPath[0] ? actualPath : L"<unavailable>";
    const wchar_t* reported = reportedPath[0] ? reportedPath : L"<unavailable>";
    const wchar_t* difference = actualPath[0] && reportedPath[0] &&
            _wcsicmp(actualPath, reportedPath) != 0
        ? L" [values differ]"
        : L"";
    Wh_Log(L"[Win10 Alt+Tab] Init in Explorer PID=%lu; actual image=%s; "
           L"GetModuleFileNameW(NULL)=%s%s",
           GetCurrentProcessId(), actual, reported, difference);
}

// NtQueryKey class 3 is KeyNameInformation. It lets the read shim match an
// already-opened HKCU\\...\\Explorer handle, not just RegGetValueW's subkey string.
struct KeyNameInformationBuffer {
    ULONG nameLength;
    WCHAR name[2048];
};

static std::wstring QueryNativeKeyPath(HKEY key) {
    if (!key || !g_ntQueryKey) {
        return {};
    }

    KeyNameInformationBuffer buffer{};
    ULONG required = 0;
    const LONG status = g_ntQueryKey(
        key, 3, &buffer, sizeof(buffer), &required);
    if (status < 0 || buffer.nameLength > sizeof(buffer.name) ||
        buffer.nameLength % sizeof(WCHAR) != 0) {
        return {};
    }
    return std::wstring(buffer.name, buffer.nameLength / sizeof(WCHAR));
}

static std::wstring KeyPath(HKEY key) {
    if (key == HKEY_CURRENT_USER) {
        return g_currentUserNativePath;
    }
    if (key == HKEY_LOCAL_MACHINE) {
        return L"\\REGISTRY\\MACHINE";
    }
    return QueryNativeKeyPath(key);
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
    if (g_currentUserNativePath.empty()) {
        return false;
    }
    const std::wstring expected = g_currentUserNativePath +
        L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer";
    return _wcsicmp(FullKeyPath(key, subkey).c_str(), expected.c_str()) == 0;
}

static bool InitializeCurrentUserPath() {
    g_currentUserNativePath.clear();
    g_ntQueryKey = nullptr;
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    g_ntQueryKey = ntdll
        ? reinterpret_cast<NtQueryKeyFn>(GetProcAddress(ntdll, "NtQueryKey"))
        : nullptr;
    if (!g_ntQueryKey) {
        Wh_Log(L"[Win10 Alt+Tab] NtQueryKey unavailable; cannot safely scope AltTabSettings reads");
        return false;
    }

    HKEY currentUser = nullptr;
    if (RegOpenCurrentUser(KEY_QUERY_VALUE, &currentUser) != ERROR_SUCCESS) {
        Wh_Log(L"[Win10 Alt+Tab] RegOpenCurrentUser failed; registry shim unavailable");
        return false;
    }
    g_currentUserNativePath = QueryNativeKeyPath(currentUser);
    RegCloseKey(currentUser);

    if (g_currentUserNativePath.empty()) {
        Wh_Log(L"[Win10 Alt+Tab] Could not resolve the current user's registry path");
        return false;
    }
    return true;
}

// Return the virtual DWORD 0 with the documented buffer/type behaviour. This is
// deliberately an in-memory answer; it never creates or changes a registry key.
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
            ((flags & RRF_SUBKEY_WOW6432KEY) && (flags & RRF_SUBKEY_WOW6464KEY))) {
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
    if (!g_explorerProcess.load(std::memory_order_acquire) ||
        g_stopping.load(std::memory_order_acquire) ||
        !g_altTabReady.load(std::memory_order_acquire) ||
        g_altTabFaulted.load(std::memory_order_acquire) ||
        !valueName || _wcsicmp(valueName, L"AltTabSettings") != 0 ||
        !IsExplorerSettingsKey(key, subkey)) {
        return false;
    }

    *result = CopyAltTabSettingsZero(getValue, flags, type, data, bytes);
    if (!g_loggedRegistryOverride.exchange(true, std::memory_order_relaxed)) {
        Wh_Log(L"[Win10 Alt+Tab] Virtual AltTabSettings=0 read in HKCU Explorer (no registry write)");
    }
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
        if (TryReadAltTabSettings(key, subkey, valueName, true,
                                  flags, type, data, bytes, &result)) {
            return result;
        }
    } catch (...) {
        Wh_Log(L"[Win10 Alt+Tab] RegGetValueW read shim failed; using the real registry");
    }
    return g_regGetValueOriginal
        ? g_regGetValueOriginal(key, subkey, valueName, flags, type, data, bytes)
        : ERROR_PROC_NOT_FOUND;
}

static LSTATUS WINAPI RegQueryValueExWHook(HKEY key,
                                            LPCWSTR valueName,
                                            LPDWORD reserved,
                                            LPDWORD type,
                                            LPBYTE data,
                                            LPDWORD bytes) {
    try {
        if (!reserved) {
            LSTATUS result = ERROR_SUCCESS;
            if (TryReadAltTabSettings(key, nullptr, valueName, false, 0,
                                      type, data, bytes, &result)) {
                return result;
            }
        }
    } catch (...) {
        Wh_Log(L"[Win10 Alt+Tab] RegQueryValueExW read shim failed; using the real registry");
    }
    return g_regQueryValueOriginal
        ? g_regQueryValueOriginal(key, valueName, reserved, type, data, bytes)
        : ERROR_PROC_NOT_FOUND;
}

static bool InstallRegistryReadHooks() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        Wh_Log(L"[Win10 Alt+Tab] kernelbase.dll unavailable; registry hooks not installed");
        return false;
    }

    auto regGetValue = reinterpret_cast<RegGetValueWFn>(
        GetProcAddress(kernelBase, "RegGetValueW"));
    auto regQueryValue = reinterpret_cast<RegQueryValueExWFn>(
        GetProcAddress(kernelBase, "RegQueryValueExW"));
    if (!regGetValue || !regQueryValue) {
        Wh_Log(L"[Win10 Alt+Tab] RegGetValueW/RegQueryValueExW exports unavailable");
        return false;
    }

    const bool getHooked = WindhawkUtils::SetFunctionHook(
        regGetValue, RegGetValueWHook, &g_regGetValueOriginal);
    const bool queryHooked = WindhawkUtils::SetFunctionHook(
        regQueryValue, RegQueryValueExWHook, &g_regQueryValueOriginal);
    if (!getHooked || !queryHooked) {
        Wh_Log(L"[Win10 Alt+Tab] Registry read hooks failed (RegGetValueW=%d, RegQueryValueExW=%d)",
               getHooked ? 1 : 0, queryHooked ? 1 : 0);
        return false;
    }

    Wh_Log(L"[Win10 Alt+Tab] Registry read hooks ready");
    return true;
}

class SymbolSearch {
    HANDLE value_ = nullptr;
public:
    explicit SymbolSearch(HANDLE value) noexcept : value_(value) {}
    ~SymbolSearch() noexcept {
        if (value_) {
            Wh_FindCloseSymbol(value_);
        }
    }
    SymbolSearch(const SymbolSearch&) = delete;
    SymbolSearch& operator=(const SymbolSearch&) = delete;
    HANDLE get() const noexcept { return value_; }
};

// Match the x64 HRESULT member-function ABI instead of depending on exact PDB
// spelling/access keywords or on one hard-coded list of parameter aliases.
static bool CompatibleHostSignature(const std::wstring& symbol,
                                    const wchar_t* method,
                                    std::wstring* suffixOut) {
    const size_t position = symbol.find(method);
    if (position == std::wstring::npos ||
        symbol.find(L"static ") != std::wstring::npos ||
        symbol.substr(0, position).find(L"long __cdecl ") == std::wstring::npos) {
        return false;
    }

    const size_t argsBegin = position + wcslen(method);
    if (argsBegin >= symbol.size() || symbol[argsBegin] != L'(' ||
        symbol.back() != L')') {
        return false;
    }

    const std::wstring suffix = symbol.substr(argsBegin);
    const std::wstring args = suffix.substr(1, suffix.size() - 2);
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        const size_t end = args.find(L',', start);
        parts.emplace_back(args.substr(
            start, end == std::wstring::npos ? end : end - start));
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }

    if (parts.size() != 4) {
        return false;
    }
    if (parts[0] != L"unsigned int" && parts[0].rfind(L"enum ", 0) != 0) {
        return false;
    }
    for (size_t i = 1; i != parts.size(); ++i) {
        const std::wstring& part = parts[i];
        if (part.find_first_of(L"()<>") != std::wstring::npos) {
            return false;
        }
        const bool pointer = part.find(L'*') != std::wstring::npos ||
                             part.find(L'&') != std::wstring::npos;
        if (!pointer && part != L"unsigned int" && part != L"int" &&
            part != L"unsigned __int64" && part != L"__int64") {
            return false;
        }
    }

    std::wstring output = parts[3];
    output.erase(std::remove(output.begin(), output.end(), L' '), output.end());
    if (output.size() < 2 || output.compare(output.size() - 2, 2, L"**") != 0) {
        return false;
    }

    if (suffixOut) {
        *suffixOut = suffix;
    }
    return true;
}

static bool CompatibleUndockedAssetSignature(const std::wstring& symbol) {
    static constexpr wchar_t method[] = L"IsUndockedAssetAvailable";
    const size_t position = symbol.find(method);
    if (position == std::wstring::npos ||
        symbol.substr(0, position).find(L"__cdecl") == std::wstring::npos) {
        return false;
    }
    const std::wstring resultType = symbol.substr(0, position);
    if (resultType.find(L"__int64") == std::wstring::npos &&
        resultType.find(L"long long") == std::wstring::npos &&
        resultType.find(L"INT64") == std::wstring::npos) {
        return false;
    }

    const size_t argsBegin = position + wcslen(method);
    if (argsBegin >= symbol.size() || symbol[argsBegin] != L'(' ||
        symbol.back() != L')') {
        return false;
    }

    const std::wstring args = symbol.substr(argsBegin + 1,
                                             symbol.size() - argsBegin - 2);
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        const size_t end = args.find(L',', start);
        parts.emplace_back(args.substr(
            start, end == std::wstring::npos ? end : end - start));
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    if (parts.size() != 4) {
        return false;
    }

    auto compact = [](std::wstring value) {
        value.erase(std::remove_if(value.begin(), value.end(),
                    [](wchar_t ch) { return std::iswspace(ch) != 0; }), value.end());
        return value;
    };
    const std::wstring first = compact(parts[0]);
    if (first != L"int" && first != L"unsignedint" &&
        first.rfind(L"enum", 0) != 0) {
        return false;
    }

    for (size_t i = 1; i != 3; ++i) {
        const std::wstring arg = compact(parts[i]);
        const bool pointer = arg.find(L'*') != std::wstring::npos ||
                             arg.find(L'&') != std::wstring::npos;
        const bool integer = arg == L"int" || arg == L"unsignedint" ||
            arg == L"__int64" || arg == L"unsigned__int64" ||
            arg == L"long" || arg == L"unsignedlong" ||
            arg == L"longlong" || arg == L"unsignedlonglong";
        if (!pointer && !integer) {
            return false;
        }
    }

    const std::wstring last = compact(parts[3]);
    return last.find(L"char") != std::wstring::npos &&
           last.find(L'*') != std::wstring::npos;
}

static void CaptureUniqueSymbolAddress(void** slot,
                                       bool* ambiguous,
                                       void* address) {
    if (*slot && *slot != address) {
        *ambiguous = true;
    } else {
        *slot = address;
    }
}

static bool ResolveFactoriesBySymbols(void** xamlOut,
                                      void** dcompOut,
                                      void** undockedOut,
                                      void** registerHotKeysOut,
                                      void** shouldShowOut,
                                      void** onAltTabOut,
                                      void** altTabHostShowOut,
                                      void** managerCreateOut,
                                      void** managerCreateWithFilterOut,
                                      void** managerHostOut,
                                      bool* ambiguousOut) {
    *xamlOut = nullptr;
    *dcompOut = nullptr;
    *undockedOut = nullptr;
    *registerHotKeysOut = nullptr;
    *shouldShowOut = nullptr;
    *onAltTabOut = nullptr;
    *altTabHostShowOut = nullptr;
    *managerCreateOut = nullptr;
    *managerCreateWithFilterOut = nullptr;
    *managerHostOut = nullptr;
    *ambiguousOut = false;

    WH_FIND_SYMBOL found{};
    SymbolSearch search(Wh_FindFirstSymbol(g_twinuiModule, nullptr, &found));
    if (!search.get()) {
        return false;
    }

    void* xaml = nullptr;
    void* dcomp = nullptr;
    void* undocked = nullptr;
    std::wstring xamlSignature;
    std::wstring dcompSignature;
    bool xamlAmbiguous = false;
    bool dcompAmbiguous = false;
    bool undockedAmbiguous = false;
    bool registerHotKeysAmbiguous = false;
    bool shouldShowAmbiguous = false;
    bool onAltTabAmbiguous = false;
    bool altTabHostShowAmbiguous = false;
    bool managerCreateAmbiguous = false;
    bool managerCreateWithFilterAmbiguous = false;
    bool managerHostAmbiguous = false;
    bool incompatibleUndockedLogged = false;
    static constexpr wchar_t kRegisterHotKeysDecorated[] =
        L"?_RegisterHotKeys@CMultitaskingViewHotKeyHandler@@AEAAJXZ";
    static constexpr wchar_t kShouldShowDecorated[] =
        L"?_ShouldShowMTVAltTab@CMultitaskingViewHotKeyHandler@@AEAA_NXZ";
    static constexpr wchar_t kOnAltTabDecorated[] =
        L"?_OnAltTab@CMultitaskingViewHotKeyHandler@@AEAAJW4IMMERSIVE_HOT_KEY_ID@@@Z";
    static constexpr wchar_t kAltTabHostShowDecorated[] =
        L"?Show@CAltTabViewHost@@UEAAJPEAUIImmersiveMonitor@@W4ALT_TAB_VIEW_FLAGS@@PEAUIApplicationView@@@Z";
    static constexpr wchar_t kManagerCreateDecorated[] =
        L"?CreateMultitaskingView@CMultitaskingViewManager@@UEAAJW4MULTITASKING_VIEW_TYPES@@AEBU_GUID@@PEAPEAX@Z";
    static constexpr wchar_t kManagerCreateWithFilterDecorated[] =
        L"?CreateMultitaskingViewWithFilter@CMultitaskingViewManager@@UEAAJW4MULTITASKING_VIEW_TYPES@@PEAUIApplicationViewCollection@@AEBU_GUID@@PEAPEAX@Z";
    static constexpr wchar_t kManagerHostDecorated[] =
        L"?_CreateMTVHost@CMultitaskingViewManager@@AEAAJW4MULTITASKING_VIEW_TYPES@@PEAUIApplicationViewCollection@@AEBU_GUID@@PEAPEAX@Z";
    do {
        if (!found.address) {
            continue;
        }
        if (found.symbolDecorated) {
            if (wcscmp(found.symbolDecorated, kRegisterHotKeysDecorated) == 0) {
                CaptureUniqueSymbolAddress(registerHotKeysOut,
                                           &registerHotKeysAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kShouldShowDecorated) == 0) {
                CaptureUniqueSymbolAddress(shouldShowOut,
                                           &shouldShowAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kOnAltTabDecorated) == 0) {
                CaptureUniqueSymbolAddress(onAltTabOut,
                                           &onAltTabAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kAltTabHostShowDecorated) == 0) {
                CaptureUniqueSymbolAddress(altTabHostShowOut,
                                           &altTabHostShowAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kManagerCreateDecorated) == 0) {
                CaptureUniqueSymbolAddress(managerCreateOut,
                                           &managerCreateAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kManagerCreateWithFilterDecorated) == 0) {
                CaptureUniqueSymbolAddress(managerCreateWithFilterOut,
                                           &managerCreateWithFilterAmbiguous,
                                           found.address);
            } else if (wcscmp(found.symbolDecorated, kManagerHostDecorated) == 0) {
                CaptureUniqueSymbolAddress(managerHostOut,
                                           &managerHostAmbiguous,
                                           found.address);
            }
        }
        if (!found.symbol) {
            continue;
        }

        std::wstring signature;
        if (CompatibleHostSignature(found.symbol,
                L"CMultitaskingViewManager::_CreateXamlMTVHost", &signature)) {
            if (xaml && xaml != found.address) {
                xamlAmbiguous = true;
            }
            xaml = found.address;
            xamlSignature = signature;
        } else if (CompatibleHostSignature(found.symbol,
                L"CMultitaskingViewManager::_CreateDCompMTVHost", &signature)) {
            if (dcomp && dcomp != found.address) {
                dcompAmbiguous = true;
            }
            dcomp = found.address;
            dcompSignature = signature;
        } else if (wcsstr(found.symbol, L"IsUndockedAssetAvailable")) {
            if (CompatibleUndockedAssetSignature(found.symbol)) {
                if (undocked && undocked != found.address) {
                    undockedAmbiguous = true;
                }
                undocked = found.address;
            } else if (!incompatibleUndockedLogged) {
                incompatibleUndockedLogged = true;
                Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable symbol found but signature not accepted: %s",
                       found.symbol);
            }
        }
    } while (Wh_FindNextSymbol(search.get(), &found));

    const bool factoryPairValid = xaml && dcomp && !xamlAmbiguous &&
        !dcompAmbiguous && xaml != dcomp && xamlSignature == dcompSignature;
    const bool directGateValid = dcomp && undocked && !dcompAmbiguous &&
        !undockedAmbiguous;

    *xamlOut = factoryPairValid ? xaml : nullptr;
    *dcompOut = (!dcompAmbiguous) ? dcomp : nullptr;
    *undockedOut = undockedAmbiguous ? nullptr : undocked;
    if (registerHotKeysAmbiguous) *registerHotKeysOut = nullptr;
    if (shouldShowAmbiguous) *shouldShowOut = nullptr;
    if (onAltTabAmbiguous) *onAltTabOut = nullptr;
    if (altTabHostShowAmbiguous) *altTabHostShowOut = nullptr;
    if (managerCreateAmbiguous) *managerCreateOut = nullptr;
    if (managerCreateWithFilterAmbiguous) *managerCreateWithFilterOut = nullptr;
    if (managerHostAmbiguous) *managerHostOut = nullptr;
    *ambiguousOut = xamlAmbiguous || dcompAmbiguous || undockedAmbiguous;
    return factoryPairValid || directGateValid;
}

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
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
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
        if (!size || imageSection->VirtualAddress >= nt->OptionalHeader.SizeOfImage ||
            size > nt->OptionalHeader.SizeOfImage - imageSection->VirtualAddress) {
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
                return false;  // Refuse ambiguous call sites.
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

static bool IsInTextSection(const TextSection& section, const uint8_t* address,
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

static bool FindInlinedFactory(const TextSection& section,
                               bool xamlFactory,
                               void** targetOut) {
    static const uint8_t xamlPattern[] = {
        0x4C, 0x89, 0x74, 0x24, 0, 0, 0x8B, 0, 0, 0x8B, 0, 0x8B,
        0xD7, 0x48, 0x8B, 0xCE, 0xE8, 0, 0, 0, 0, 0x8B};
    static const uint8_t dcompPattern[] = {
        0x4C, 0x89, 0x74, 0x24, 0, 0, 0x8B, 0, 0, 0x8B, 0, 0x8B,
        0xD7, 0x48, 0x8B, 0xCE, 0xE8, 0, 0, 0, 0, 0x90};
    static const char mask[] = "xxxx??x??x?xxxxxx????x";

    const uint8_t* match = nullptr;
    const uint8_t* pattern = xamlFactory ? xamlPattern : dcompPattern;
    if (!FindUniquePattern(section, pattern, mask, sizeof(xamlPattern), &match)) {
        return false;
    }
    return RelativeCallTarget(section, match + 16, targetOut);
}

static bool FollowJnz(const TextSection& section,
                      const uint8_t* instruction,
                      const uint8_t** targetOut,
                      size_t* instructionSizeOut) {
    if (IsInTextSection(section, instruction, 2) && instruction[0] == 0x75) {
        const auto* target = instruction + 2 + static_cast<int8_t>(instruction[1]);
        if (!IsInTextSection(section, target)) {
            return false;
        }
        *targetOut = target;
        *instructionSizeOut = 2;
        return true;
    }
    if (IsInTextSection(section, instruction, 6) &&
        instruction[0] == 0x0F && instruction[1] == 0x85) {
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

static bool FindNonInlinedFactory(const TextSection& section,
                                  bool xamlFactory,
                                  void** targetOut) {
    // CMultitaskingViewManager::_CreateMTVHost, as used by ExplorerPatcher:
    // GetMTVHostKind(); compare the result with 1; the JNZ and fall-through
    // each call one of the two native host factories.
    static const uint8_t pattern[] = {
        0x8B, 0xCF, 0xE8, 0, 0, 0, 0, 0, 0x89, 0, 0x24, 0, 0,
        0x8B, 0, 0, 0x8B, 0, 0x8B, 0xD7, 0x48, 0x8B, 0xCE,
        0x83, 0xF8, 0x01};
    static const char mask[] = "xxx?????x?x??x??x?xxxxxxxx";

    const uint8_t* match = nullptr;
    if (!FindUniquePattern(section, pattern, mask, sizeof(pattern), &match)) {
        return false;
    }

    const uint8_t* branchTarget = nullptr;
    size_t branchSize = 0;
    const uint8_t* branch = match + 26;
    if (!FollowJnz(section, branch, &branchTarget, &branchSize)) {
        return false;
    }

    const uint8_t* call = xamlFactory ? branch + branchSize : branchTarget;
    return RelativeCallTarget(section, call, targetOut);
}

static bool ResolveFactoriesBySignatures(void** xamlOut, void** dcompOut) {
    *xamlOut = nullptr;
    *dcompOut = nullptr;

    TextSection section;
    if (!GetTextSection(g_twinuiModule, &section)) {
        return false;
    }

    void* xamlInline = nullptr;
    void* dcompInline = nullptr;
    const bool xamlInlineFound = FindInlinedFactory(section, true, &xamlInline);
    const bool dcompInlineFound = FindInlinedFactory(section, false, &dcompInline);
    if (xamlInlineFound && dcompInlineFound && xamlInline != dcompInline) {
        *xamlOut = xamlInline;
        *dcompOut = dcompInline;
        return true;
    }

    // Do not mix one inlined target with one target from the non-inlined
    // pattern: both addresses must come from the same caller layout.
    void* xamlNonInline = nullptr;
    void* dcompNonInline = nullptr;
    const bool xamlNonInlineFound =
        FindNonInlinedFactory(section, true, &xamlNonInline);
    const bool dcompNonInlineFound =
        FindNonInlinedFactory(section, false, &dcompNonInline);
    if (xamlNonInlineFound && dcompNonInlineFound &&
        xamlNonInline != dcompNonInline) {
        *xamlOut = xamlNonInline;
        *dcompOut = dcompNonInline;
        return true;
    }
    return false;
}

static bool ResolveFactories(void** xamlOut,
                             void** dcompOut,
                             void** undockedOut,
                             void** registerHotKeysOut,
                             void** shouldShowOut,
                             void** onAltTabOut,
                             void** altTabHostShowOut,
                             void** managerCreateOut,
                             void** managerCreateWithFilterOut,
                             void** managerHostOut) {
    *undockedOut = nullptr;
    *registerHotKeysOut = nullptr;
    *shouldShowOut = nullptr;
    *onAltTabOut = nullptr;
    *altTabHostShowOut = nullptr;
    *managerCreateOut = nullptr;
    *managerCreateWithFilterOut = nullptr;
    *managerHostOut = nullptr;
    bool ambiguous = false;
    if (ResolveFactoriesBySymbols(xamlOut, dcompOut, undockedOut,
                                  registerHotKeysOut, shouldShowOut,
                                  onAltTabOut, altTabHostShowOut,
                                  managerCreateOut,
                                  managerCreateWithFilterOut,
                                  managerHostOut,
                                  &ambiguous)) {
        if (*xamlOut && *dcompOut) {
            Wh_Log(L"[Win10 Alt+Tab] Resolved both native host factories by Windhawk symbol enumeration");
        } else {
            Wh_Log(L"[Win10 Alt+Tab] Resolved the DComp factory and direct selection gate by Windhawk symbol enumeration");
        }
        if (*undockedOut) {
            Wh_Log(L"[Win10 Alt+Tab] Resolved IsUndockedAssetAvailable gate by Windhawk symbol enumeration");
        } else {
            Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable gate was not resolved; only the XAML-factory fallback is available");
        }
        return true;
    }

    Wh_Log(L"[Win10 Alt+Tab] Symbol enumeration did not resolve a unique compatible factory pair%s; trying the ExplorerPatcher x64 call-site signatures",
           ambiguous ? L" (ambiguous symbols)" : L"");
    if (ResolveFactoriesBySignatures(xamlOut, dcompOut)) {
        Wh_Log(L"[Win10 Alt+Tab] Resolved both native host factories by x64 call-site signatures");
        if (*undockedOut) {
            Wh_Log(L"[Win10 Alt+Tab] Resolved IsUndockedAssetAvailable gate by Windhawk symbols");
        } else {
            Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable gate unavailable; direct host selection cannot be forced on this build");
        }
        return true;
    }

    Wh_Log(L"[Win10 Alt+Tab] Native factories not resolved; no Alt+Tab hook installed");
    return false;
}

// Trace observers preserve the original arguments and return values. The
// _CreateMTVHost observer adds only the narrowly guarded E_UNEXPECTED fallback
// documented above; no hook changes window styles or visibility.
static HRESULT WINAPI RegisterHotKeysObserver(void* self) {
    Wh_Log(L"[Win10 Alt+Tab][trace] _RegisterHotKeys entered");
    const HRESULT result = g_registerHotKeysOriginal
        ? g_registerHotKeysOriginal(self)
        : E_FAIL;
    Wh_Log(L"[Win10 Alt+Tab][trace] _RegisterHotKeys returned 0x%08X",
           static_cast<unsigned int>(result));
    return result;
}

static bool WINAPI ShouldShowAltTabObserver(void* self) {
    const bool result = g_shouldShowAltTabOriginal
        ? g_shouldShowAltTabOriginal(self)
        : false;
    const unsigned call =
        g_shouldShowCallCount.fetch_add(1, std::memory_order_relaxed) + 1;
    Wh_Log(L"[Win10 Alt+Tab][trace] _ShouldShowMTVAltTab call=%u -> %u",
           call, result ? 1u : 0u);
    return result;
}

static HRESULT WINAPI OnAltTabObserver(void* self, unsigned int hotKeyId) {
    const unsigned call =
        g_onAltTabCallCount.fetch_add(1, std::memory_order_relaxed) + 1;
    Wh_Log(L"[Win10 Alt+Tab][trace] _OnAltTab call=%u entered; id=0x%X",
           call, hotKeyId);
    const HRESULT result = g_onAltTabOriginal
        ? g_onAltTabOriginal(self, hotKeyId)
        : E_FAIL;
    Wh_Log(L"[Win10 Alt+Tab][trace] _OnAltTab call=%u returned 0x%08X",
           call, static_cast<unsigned int>(result));
    return result;
}

static HRESULT WINAPI AltTabHostShowObserver(void* self,
                                             void* monitor,
                                             unsigned int flags,
                                             void* applicationView) {
    const unsigned call =
        g_altTabShowCallCount.fetch_add(1, std::memory_order_relaxed) + 1;
    const HRESULT result = g_altTabHostShowOriginal
        ? g_altTabHostShowOriginal(self, monitor, flags, applicationView)
        : E_FAIL;
    Wh_Log(L"[Win10 Alt+Tab][trace] CAltTabViewHost::Show call=%u flags=0x%X returned 0x%08X",
           call, flags, static_cast<unsigned int>(result));
    return result;
}

static HRESULT WINAPI CreateMultitaskingViewObserver(void* self,
                                                    unsigned int kind,
                                                    const GUID* viewId,
                                                    void** output) {
    const unsigned call = kind == 1
        ? g_managerCreateCallCount.fetch_add(1, std::memory_order_relaxed) + 1
        : 0;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] CMultitaskingViewManager::CreateMultitaskingView call=%u kind=1",
               call);
    }
    const HRESULT result = g_createMultitaskingViewOriginal
        ? g_createMultitaskingViewOriginal(self, kind, viewId, output)
        : E_FAIL;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] CreateMultitaskingView call=%u returned 0x%08X",
               call, static_cast<unsigned int>(result));
    }
    return result;
}

static HRESULT WINAPI CreateMultitaskingViewWithFilterObserver(
    void* self, unsigned int kind, void* viewCollection,
    const GUID* viewId, void** output) {
    const unsigned call = kind == 1
        ? g_managerCreateWithFilterCallCount.fetch_add(
              1, std::memory_order_relaxed) + 1
        : 0;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] CMultitaskingViewManager::CreateMultitaskingViewWithFilter call=%u kind=1",
               call);
    }
    const HRESULT result = g_createMultitaskingViewWithFilterOriginal
        ? g_createMultitaskingViewWithFilterOriginal(
              self, kind, viewCollection, viewId, output)
        : E_FAIL;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] CreateMultitaskingViewWithFilter call=%u returned 0x%08X",
               call, static_cast<unsigned int>(result));
    }
    return result;
}

static HRESULT WINAPI CreateDCompHostHook(void* self,
                                          unsigned int kind,
                                          ULONG_PTR arg3,
                                          ULONG_PTR arg4,
                                          void** output);

static HRESULT WINAPI CreateMtvHostObserver(void* self,
                                           unsigned int kind,
                                           void* viewCollection,
                                           const GUID* viewId,
                                           void** output) {
    const unsigned call = kind == 1
        ? g_managerHostCallCount.fetch_add(1, std::memory_order_relaxed) + 1
        : 0;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] CMultitaskingViewManager::_CreateMTVHost call=%u kind=1 collection=%p viewId=%p output=%p",
               call, viewCollection,
               const_cast<void*>(static_cast<const void*>(viewId)),
               static_cast<void*>(output));
    }

    const unsigned long long factorySequenceBefore =
        g_altTabFactoryEntrySequence.load(std::memory_order_relaxed);
    HRESULT result = g_createMtvHostOriginal
        ? g_createMtvHostOriginal(self, kind, viewCollection, viewId, output)
        : E_FAIL;
    const unsigned long long factorySequenceAfter =
        g_altTabFactoryEntrySequence.load(std::memory_order_relaxed);
    void* outputHost = output ? *output : nullptr;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] _CreateMTVHost call=%u original=0x%08X host=%p factoryEntered=%u",
               call, static_cast<unsigned int>(result), outputHost,
               factorySequenceAfter != factorySequenceBefore ? 1u : 0u);
    }

    // The user's trace shows E_UNEXPECTED from _CreateMTVHost before either
    // factory is entered, with a null viewCollection. The native DComp factory
    // has an explicit null-collection path; pass the original argument through
    // and never retry after a factory has already been invoked.
    if (kind == 1 && result == E_UNEXPECTED &&
        factorySequenceAfter == factorySequenceBefore && viewId &&
        output && !outputHost && g_dcompHostOriginal && g_altTabReady.load(std::memory_order_acquire) &&
        !g_altTabFaulted.load(std::memory_order_acquire) &&
        !g_stopping.load(std::memory_order_acquire)) {
        Wh_Log(L"[Win10 Alt+Tab] _CreateMTVHost returned E_UNEXPECTED before either factory; trying the native DirectComposition factory once with the same arguments");
        const HRESULT fallbackResult = CreateDCompHostHook(
            self, kind, reinterpret_cast<ULONG_PTR>(viewCollection),
            reinterpret_cast<ULONG_PTR>(viewId), output);
        if (SUCCEEDED(fallbackResult) && output && *output) {
            Wh_Log(L"[Win10 Alt+Tab] DirectComposition fallback created the host; continuing the native manager path");
            result = fallbackResult;
        } else if (SUCCEEDED(fallbackResult)) {
            g_altTabFaulted.store(true, std::memory_order_release);
            Wh_Log(L"[Win10 Alt+Tab] DirectComposition fallback returned success but no host; retaining the original E_UNEXPECTED");
        } else {
            Wh_Log(L"[Win10 Alt+Tab] DirectComposition fallback failed (0x%08X); retaining the original E_UNEXPECTED",
                   static_cast<unsigned int>(fallbackResult));
        }
    }

    outputHost = output ? *output : nullptr;
    if (kind == 1) {
        Wh_Log(L"[Win10 Alt+Tab][trace] _CreateMTVHost call=%u final=0x%08X host=%p",
               call, static_cast<unsigned int>(result), outputHost);
    }
    return result;
}

static void InstallAltTabDiagnosticHooks(void* registerHotKeysAddress,
                                         void* shouldShowAddress,
                                         void* onAltTabAddress,
                                         void* altTabHostShowAddress,
                                         void* managerCreateAddress,
                                         void* managerCreateWithFilterAddress,
                                         void* managerHostAddress) {
    const bool registerHooked = registerHotKeysAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<RegisterHotKeysFn>(registerHotKeysAddress),
            RegisterHotKeysObserver,
            &g_registerHotKeysOriginal);
    const bool shouldShowHooked = shouldShowAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<ShouldShowAltTabFn>(shouldShowAddress),
            ShouldShowAltTabObserver,
            &g_shouldShowAltTabOriginal);
    const bool onAltTabHooked = onAltTabAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<OnAltTabFn>(onAltTabAddress),
            OnAltTabObserver,
            &g_onAltTabOriginal);
    const bool hostShowHooked = altTabHostShowAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<AltTabHostShowFn>(altTabHostShowAddress),
            AltTabHostShowObserver,
            &g_altTabHostShowOriginal);
    const bool managerCreateHooked = managerCreateAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateMultitaskingViewFn>(managerCreateAddress),
            CreateMultitaskingViewObserver,
            &g_createMultitaskingViewOriginal);
    const bool managerCreateWithFilterHooked = managerCreateWithFilterAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateMultitaskingViewWithFilterFn>(
                managerCreateWithFilterAddress),
            CreateMultitaskingViewWithFilterObserver,
            &g_createMultitaskingViewWithFilterOriginal);
    const bool managerHostHooked = managerHostAddress &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateMtvHostFn>(managerHostAddress),
            CreateMtvHostObserver,
            &g_createMtvHostOriginal);

    Wh_Log(L"[Win10 Alt+Tab] Passive trace hooks: RegisterHotKeys=%d, gate=%d, OnAltTab=%d, native Show=%d, manager Create=%d, CreateWithFilter=%d, _CreateMTVHost=%d",
           registerHooked ? 1 : 0,
           shouldShowHooked ? 1 : 0,
           onAltTabHooked ? 1 : 0,
           hostShowHooked ? 1 : 0,
           managerCreateHooked ? 1 : 0,
           managerCreateWithFilterHooked ? 1 : 0,
           managerHostHooked ? 1 : 0);
}

static int64_t IsUndockedAssetAvailableHook(int kind,
                                            int64_t arg2,
                                            int64_t arg3,
                                            const char* arg4) {
    try {
        if (!g_stopping.load(std::memory_order_acquire) &&
            g_altTabReady.load(std::memory_order_acquire) &&
            !g_altTabFaulted.load(std::memory_order_acquire) && kind == 1) {
            if (!g_loggedUndockedGate.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable(kind=1) -> 0; requesting native DirectComposition host");
            }
            return 0;
        }
    } catch (...) {
        Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable hook exception; using original gate");
    }

    return g_isUndockedAssetAvailableOriginal
        ? g_isUndockedAssetAvailableOriginal(kind, arg2, arg3, arg4)
        : 1;
}

static HRESULT WINAPI CreateDCompHostHook(void* self,
                                          unsigned int kind,
                                          ULONG_PTR arg3,
                                          ULONG_PTR arg4,
                                          void** output) {
    try {
        if (kind == 1) {
            g_altTabFactoryEntrySequence.fetch_add(1, std::memory_order_relaxed);
        }
        if (kind == 1 &&
            !g_loggedDcompAltTab.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[Win10 Alt+Tab] DirectComposition host factory entered; kind=1");
        }

        const HRESULT result = g_dcompHostOriginal
            ? g_dcompHostOriginal(self, kind, arg3, arg4, output)
            : E_FAIL;
        if (kind == 1) {
            if (FAILED(result)) {
                g_altTabFaulted.store(true, std::memory_order_release);
                Wh_Log(L"[Win10 Alt+Tab] DirectComposition factory failed (0x%08X)",
                       static_cast<unsigned int>(result));
            } else if (!g_loggedNativeHost.exchange(true, std::memory_order_relaxed)) {
                Wh_Log(L"[Win10 Alt+Tab] Native Windows 10 DirectComposition host created");
            }
        }
        return result;
    } catch (...) {
        g_altTabFaulted.store(true, std::memory_order_release);
        Wh_Log(L"[Win10 Alt+Tab] DirectComposition factory callback exception; returning E_FAIL");
        return E_FAIL;
    }
}

static HRESULT WINAPI CreateXamlHostHook(void* self,
                                         unsigned int kind,
                                         ULONG_PTR arg3,
                                         ULONG_PTR arg4,
                                         void** output) {
    try {
        if (kind == 1) {
            g_altTabFactoryEntrySequence.fetch_add(1, std::memory_order_relaxed);
        }
        if (kind == 1 &&
            !g_loggedAltTabFactoryEntry.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[Win10 Alt+Tab] Alt+Tab XAML factory entry reached; kind=1");
        } else if (!g_loggedFactoryEntry.exchange(true, std::memory_order_relaxed)) {
            Wh_Log(L"[Win10 Alt+Tab] XAML host factory reached; first kind=%u", kind);
        }

        if (!g_stopping.load(std::memory_order_acquire) &&
            g_altTabReady.load(std::memory_order_acquire) &&
            !g_altTabFaulted.load(std::memory_order_acquire) &&
            kind == 1 && g_dcompHostOriginal) {
            return CreateDCompHostHook(self, kind, arg3, arg4, output);
        }

        return g_xamlHostOriginal
            ? g_xamlHostOriginal(self, kind, arg3, arg4, output)
            : E_FAIL;
    } catch (...) {
        g_altTabFaulted.store(true, std::memory_order_release);
        Wh_Log(L"[Win10 Alt+Tab] Factory callback C++ exception; returning E_FAIL");
        return E_FAIL;
    }
}

static bool InstallNativeHostHooks(void* xamlAddress,
                                   void* dcompAddress,
                                   void* undockedAddress) {
    if (!dcompAddress) {
        Wh_Log(L"[Win10 Alt+Tab] DirectComposition factory address is missing");
        return false;
    }

    const CreateHostFn dcompTarget = reinterpret_cast<CreateHostFn>(dcompAddress);
    g_dcompHostOriginal = dcompTarget;
    const bool dcompObserverHooked = WindhawkUtils::SetFunctionHook(
        dcompTarget, CreateDCompHostHook, &g_dcompHostOriginal);
    if (!dcompObserverHooked) {
        // The direct gate can still select the system's DComp host; this hook
        // only adds runtime confirmation, so keep the unhooked target pointer.
        g_dcompHostOriginal = dcompTarget;
        Wh_Log(L"[Win10 Alt+Tab] DComp observation hook unavailable; direct gate can still route");
    }

    bool xamlFallbackHooked = false;
    if (xamlAddress && xamlAddress != dcompAddress) {
        xamlFallbackHooked = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<CreateHostFn>(xamlAddress),
            CreateXamlHostHook,
            &g_xamlHostOriginal);
        if (!xamlFallbackHooked) {
            Wh_Log(L"[Win10 Alt+Tab] XAML-factory fallback hook unavailable");
        }
    }

    bool directGateHooked = false;
    if (undockedAddress) {
        directGateHooked = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<IsUndockedAssetAvailableFn>(undockedAddress),
            IsUndockedAssetAvailableHook,
            &g_isUndockedAssetAvailableOriginal);
        if (!directGateHooked) {
            Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable gate hook registration failed");
        }
    } else {
        Wh_Log(L"[Win10 Alt+Tab] IsUndockedAssetAvailable symbol missing; using only the XAML-factory fallback");
    }

    Wh_Log(L"[Win10 Alt+Tab] Native route hooks: gate=%d, XAML fallback=%d, DComp observer=%d",
           directGateHooked ? 1 : 0,
           xamlFallbackHooked ? 1 : 0,
           dcompObserverHooked ? 1 : 0);
    return directGateHooked || xamlFallbackHooked;
}

BOOL Wh_ModInit() {
    g_xamlHostOriginal = nullptr;
    g_dcompHostOriginal = nullptr;
    g_registerHotKeysOriginal = nullptr;
    g_shouldShowAltTabOriginal = nullptr;
    g_onAltTabOriginal = nullptr;
    g_altTabHostShowOriginal = nullptr;
    g_createMultitaskingViewOriginal = nullptr;
    g_createMultitaskingViewWithFilterOriginal = nullptr;
    g_createMtvHostOriginal = nullptr;
    g_isUndockedAssetAvailableOriginal = nullptr;
    g_regGetValueOriginal = nullptr;
    g_regQueryValueOriginal = nullptr;
    g_stopping.store(false, std::memory_order_release);
    g_altTabReady.store(false, std::memory_order_release);
    g_altTabFaulted.store(false, std::memory_order_release);
    g_loggedFactoryEntry.store(false, std::memory_order_relaxed);
    g_loggedAltTabFactoryEntry.store(false, std::memory_order_relaxed);
    g_loggedUndockedGate.store(false, std::memory_order_relaxed);
    g_loggedDcompAltTab.store(false, std::memory_order_relaxed);
    g_loggedNativeHost.store(false, std::memory_order_relaxed);
    g_loggedRegistryOverride.store(false, std::memory_order_relaxed);
    g_shouldShowCallCount.store(0, std::memory_order_relaxed);
    g_onAltTabCallCount.store(0, std::memory_order_relaxed);
    g_altTabShowCallCount.store(0, std::memory_order_relaxed);
    g_managerCreateCallCount.store(0, std::memory_order_relaxed);
    g_managerCreateWithFilterCallCount.store(0, std::memory_order_relaxed);
    g_managerHostCallCount.store(0, std::memory_order_relaxed);
    g_altTabFactoryEntrySequence.store(0, std::memory_order_relaxed);
    g_explorerProcess.store(true, std::memory_order_release);
    LogCurrentProcess();

    const bool registryPathReady = InitializeCurrentUserPath();
    if (!registryPathReady) {
        Wh_Log(L"[Win10 Alt+Tab] Scoped registry shim unavailable; continuing with direct native host selection");
    }
    const bool registryHooksReady =
        registryPathReady && InstallRegistryReadHooks();
    if (!registryHooksReady) {
        Wh_Log(L"[Win10 Alt+Tab] Registry read shim unavailable; continuing with native selection");
    }

    g_twinuiModule = LoadLibraryExW(
        L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_twinuiModule) {
        Wh_Log(L"[Win10 Alt+Tab] twinui.pcshell.dll could not be loaded");
        if (registryHooksReady) {
            g_altTabReady.store(true, std::memory_order_release);
            Wh_Log(L"[Win10 Alt+Tab] Activating the PR #5929 registry-read fallback only");
            return TRUE;
        }
        return FALSE;
    }

    void* xamlAddress = nullptr;
    void* dcompAddress = nullptr;
    void* undockedAddress = nullptr;
    void* registerHotKeysAddress = nullptr;
    void* shouldShowAddress = nullptr;
    void* onAltTabAddress = nullptr;
    void* altTabHostShowAddress = nullptr;
    void* managerCreateAddress = nullptr;
    void* managerCreateWithFilterAddress = nullptr;
    void* managerHostAddress = nullptr;
    if (!ResolveFactories(&xamlAddress, &dcompAddress, &undockedAddress,
                          &registerHotKeysAddress, &shouldShowAddress,
                          &onAltTabAddress, &altTabHostShowAddress,
                          &managerCreateAddress,
                          &managerCreateWithFilterAddress,
                          &managerHostAddress)) {
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
        if (registryHooksReady) {
            g_altTabReady.store(true, std::memory_order_release);
            Wh_Log(L"[Win10 Alt+Tab] Native symbols unavailable; activating the PR #5929 registry-read fallback only");
            return TRUE;
        }
        return FALSE;
    }

    InstallAltTabDiagnosticHooks(registerHotKeysAddress,
                                 shouldShowAddress,
                                 onAltTabAddress,
                                 altTabHostShowAddress,
                                 managerCreateAddress,
                                 managerCreateWithFilterAddress,
                                 managerHostAddress);

    const bool nativeRouteReady =
        InstallNativeHostHooks(xamlAddress, dcompAddress, undockedAddress);
    if (!nativeRouteReady && !registryHooksReady) {
        Wh_Log(L"[Win10 Alt+Tab] No usable native route hook or registry read shim could be installed");
        return TRUE;
    }
    if (!nativeRouteReady) {
        Wh_Log(L"[Win10 Alt+Tab] Native route hooks unavailable; retaining the PR #5929 registry-read fallback");
    }

    g_altTabReady.store(true, std::memory_order_release);
    Wh_Log(L"[Win10 Alt+Tab] Router ready (native route=%d, gate symbol=%d, registry shim=%d); press Alt+Tab for runtime verification",
           nativeRouteReady ? 1 : 0, undockedAddress ? 1 : 0,
           registryHooksReady ? 1 : 0);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_altTabReady.store(false, std::memory_order_release);
    Wh_Log(L"[Win10 Alt+Tab] BeforeUninit: routing disabled; Windhawk is removing hooks");
}

void Wh_ModUninit() {
    g_altTabReady.store(false, std::memory_order_release);
    Wh_Log(L"[Win10 Alt+Tab] Uninit: hook removal completed; releasing twinui module reference");
    if (g_twinuiModule) {
        FreeLibrary(g_twinuiModule);
        g_twinuiModule = nullptr;
    }
    Wh_Log(L"[Win10 Alt+Tab] Uninit complete");
}


// ==WindhawkMod==
// @id              disable-web-search-in-start-menu
// @name            Disable Web Search in Start Menu
// @description     Keeps the Start menu and taskbar search local by hiding web results and web suggestions
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @license         MIT
// @include         SearchHost.exe
// @include         SearchApp.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Web Search in Start Menu

![Before & After](https://raw.githubusercontent.com/DeepakBalaraman/disable-web-search-in-start-menu/master/assets/before-after.jpg)

The search UI that the Start menu and the taskbar open mixes results from Bing
into local results. Windows can be told to stop doing that with settings and
policies, but they are read when the search UI starts, so changing them means
signing out, and the registry entries stay behind afterwards.

This mod does the same thing without touching the registry. It sits in the
search UI process and answers the setting reads as if web search had been
turned off. Turning the mod off gives the old behavior back the next time the
search UI starts.

On both Windows 10 and Windows 11, typing in the Start menu hands the query to
this search UI, so the Start menu search is covered as well.

## Settings

- **Also hide cloud content results** reports the two "Cloud content search"
  switches from Settings as off. The Windows 11 search UI asks for them before
  it puts web results in the list, so turning this setting off can bring web
  results back, and leaving it on also removes OneDrive, Outlook and SharePoint
  results from search.

## Notes

- Nothing is written to the registry and no files are touched. The values the
  mod reports are the ones Windows itself uses:
  - `DisableSearchBoxSuggestions`, `ConnectedSearchUseWeb` and
    `DisableWebSearch`, the web search policies
  - `BingSearchEnabled`, the older web results switch
  - `IsMSACloudSearchEnabled`, `IsAADCloudSearchEnabled` and
    `AllowCloudSearch`, the cloud content switches, only with the setting above
  If you set any of these yourself, the mod simply agrees.
- The search UI is a packaged app and reaches the registry through the API sets,
  which end up in `kernelbase.dll`. Desktop callers import `advapi32.dll`, whose
  exports are separate entry points rather than forwarders on current builds.
  Both are hooked, so neither path is missed.
- Web results are decided while the search UI sets up, so after enabling the mod
  or changing a setting, end `SearchHost.exe` in Task Manager. Windows starts it
  again on the next search, and the new settings are read then.
- If web results survive, turn on the mod's log and open the search UI once. The
  log lists every read the mod answered, which shows whether the process asks
  for these values at all.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideCloudContent: true
  $name: Also hide cloud content results (OneDrive, Outlook, SharePoint)
  $description: Report the Cloud content search switches as off, which the Windows 11 search UI asks for before it shows web results. This also hides OneDrive, Outlook and SharePoint results. Turn it off if web results stay hidden without it and you want those results back.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

// A registry value the mod answers on its own. Only the name and the answer are
// stored, the type is always REG_DWORD, which is what all of these values are.
struct VirtualValue {
    PCWSTR name;
    DWORD value;
    // Cloud content results come from the user's accounts, not from the web, so
    // they are only reported when that is asked for.
    bool cloudContent;
};

// A key the mod answers for, named relative to the software root of its hive.
// Windows reads these from HKCU on some components and from HKLM on others, and
// matching on the relative path covers both with one entry.
struct VirtualKey {
    PCWSTR path;
    const VirtualValue* values;
    size_t valueCount;
};

// Turns off the web results and the web suggestions of the search box. This is
// the policy Windows writes behind "search the web" being gone.
const VirtualValue kExplorerPolicyValues[] = {
    {L"DisableSearchBoxSuggestions", 1, false},
};

// The Windows Search policy, which the search UI asks for before it queries the
// suggestion service at all.
const VirtualValue kWindowsSearchPolicyValues[] = {
    {L"ConnectedSearchUseWeb", 0, false},
    {L"DisableWebSearch", 1, false},
    {L"AllowCloudSearch", 0, true},
};

// The older web results switch, which the search UI still reads.
const VirtualValue kSearchValues[] = {
    {L"BingSearchEnabled", 0, false},
};

// The switches behind "Cloud content search" in Settings. The web API layer of
// the search UI asks for these, and on Windows 11 21H2 they do not appear to be
// read separately from the web search ones.
const VirtualValue kSearchSettingsValues[] = {
    {L"IsMSACloudSearchEnabled", 0, true},
    {L"IsAADCloudSearchEnabled", 0, true},
};

// Paths are stored lower case, because that is how they are compared.
const VirtualKey kVirtualKeys[] = {
    {L"software\\microsoft\\windows\\currentversion\\searchsettings",
     kSearchSettingsValues, ARRAYSIZE(kSearchSettingsValues)},
    {L"software\\microsoft\\windows\\currentversion\\search",
     kSearchValues, ARRAYSIZE(kSearchValues)},
    {L"software\\policies\\microsoft\\windows\\explorer",
     kExplorerPolicyValues, ARRAYSIZE(kExplorerPolicyValues)},
    {L"software\\policies\\microsoft\\windows\\windows search",
     kWindowsSearchPolicyValues, ARRAYSIZE(kWindowsSearchPolicyValues)},
};

// Settings are read by the hooks on whichever thread the shell calls them from.
std::atomic<bool> g_hideCloudContent{false};
std::atomic<bool> g_initialized{false};

// Keys the callers hold open, with the path the mod matched when they were
// opened. Only the handful of search keys ever end up here, so lookups stay
// small, and closing a key removes it again so a recycled handle cannot be
// mistaken for one of them.
struct TrackedKey {
    std::wstring path;
    bool standIn;
};

std::mutex g_trackedMutex;
std::unordered_map<HKEY, TrackedKey> g_trackedKeys;

// The hooks call registry functions of their own, and Windhawk's logging does
// too, so re-entry has to be cut short.
thread_local bool g_inHook = false;

// Every answered read would be logged, and the search UI reads these on each
// keystroke, so only the first few are kept.
std::atomic<int> g_loggedReads{0};
const int kMaxLoggedReads = 32;

bool IsActive() {
    return g_initialized && !g_inHook;
}

std::wstring ToLowercase(const std::wstring& text) {
    std::wstring lower = text;
    for (WCHAR& c : lower) {
        if (c >= L'A' && c <= L'Z') {
            c = static_cast<WCHAR>(c - L'A' + L'a');
        }
    }
    return lower;
}

// Paths are relative to the software root of their hive, so the paths in the
// table are matched no matter which hive the caller asked for.
bool GetSoftwareRelativePath(const std::wstring& fullPath,
                             std::wstring& relative) {
    std::wstring lower = ToLowercase(fullPath);

    static const wchar_t kRoot[] = L"software\\";
    if (lower.compare(0, ARRAYSIZE(kRoot) - 1, kRoot) == 0) {
        relative = lower;
        return true;
    }

    // Callers that open the key through HKEY_USERS already have a user part in
    // front of the software root.
    static const wchar_t kMarker[] = L"\\software\\";
    size_t markerPosition = lower.find(kMarker);
    if (markerPosition != std::wstring::npos) {
        relative = lower.substr(markerPosition + 1);
        return true;
    }

    return false;
}

bool IsRootHandle(HKEY hKey) {
    return hKey == HKEY_CURRENT_USER || hKey == HKEY_LOCAL_MACHINE ||
           hKey == HKEY_USERS || hKey == HKEY_CLASSES_ROOT ||
           hKey == HKEY_CURRENT_CONFIG;
}

bool GetTrackedKey(HKEY hKey, TrackedKey& tracked) {
    std::lock_guard<std::mutex> lock(g_trackedMutex);

    auto it = g_trackedKeys.find(hKey);
    if (it == g_trackedKeys.end()) {
        return false;
    }

    tracked = it->second;
    return true;
}

void TrackKey(HKEY hKey, const std::wstring& path, bool standIn) {
    std::lock_guard<std::mutex> lock(g_trackedMutex);

    // A handle closed through something other than the hooked functions would
    // stay behind, and a recycled handle must never be mistaken for one of
    // these keys, so the map is dropped rather than left to grow.
    if (g_trackedKeys.size() > 64) {
        g_trackedKeys.clear();
    }

    g_trackedKeys[hKey] = {path, standIn};
}

void UntrackKey(HKEY hKey) {
    std::lock_guard<std::mutex> lock(g_trackedMutex);
    g_trackedKeys.erase(hKey);
}

bool IsVirtualPath(const std::wstring& relative) {
    for (const auto& key : kVirtualKeys) {
        if (relative == key.path) {
            return true;
        }
    }
    return false;
}

const VirtualValue* FindVirtualValue(const std::wstring& relative,
                                     PCWSTR valueName) {
    if (!valueName) {
        return nullptr;
    }

    for (const auto& key : kVirtualKeys) {
        if (relative != key.path) {
            continue;
        }

        for (size_t i = 0; i < key.valueCount; i++) {
            const VirtualValue& value = key.values[i];
            if (value.cloudContent && !g_hideCloudContent) {
                continue;
            }

            if (_wcsicmp(value.name, valueName) == 0) {
                return &value;
            }
        }
    }

    return nullptr;
}

// Builds the relative path of a key a caller asked about. Roots are the common
// case. Deeper handles come from callers that open a key once and read it
// repeatedly, and those are known because they were tracked when opened.
// standIn reports a handle that only stands in for a key that does not exist,
// which has no other values to offer.
bool GetRequestedRelativePath(HKEY hKey,
                              LPCWSTR subKey,
                              std::wstring& relative,
                              bool& standIn) {
    standIn = false;

    TrackedKey tracked;
    bool isTracked = GetTrackedKey(hKey, tracked);

    if (!subKey) {
        if (!isTracked) {
            return false;
        }

        relative = tracked.path;
        standIn = tracked.standIn;
        return true;
    }

    if (IsRootHandle(hKey)) {
        return GetSoftwareRelativePath(subKey, relative);
    }

    if (!isTracked) {
        return false;
    }

    standIn = tracked.standIn;
    return GetSoftwareRelativePath(tracked.path + L"\\" + subKey, relative);
}

// Copies a virtual DWORD out the way the registry API would, including the size
// only and buffer too small cases.
LSTATUS ReturnVirtualDword(DWORD value,
                           LPDWORD type,
                           PVOID data,
                           LPDWORD dataSize) {
    if (type) {
        *type = REG_DWORD;
    }

    // Both APIs reject a request for data without a size, but a caller that
    // wants neither is just after the existence and the type.
    if (!dataSize) {
        return data ? ERROR_INVALID_PARAMETER : ERROR_SUCCESS;
    }

    if (!data) {
        *dataSize = sizeof(DWORD);
        return ERROR_SUCCESS;
    }

    if (*dataSize < sizeof(DWORD)) {
        *dataSize = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }

    *static_cast<DWORD*>(data) = value;
    *dataSize = sizeof(DWORD);
    return ERROR_SUCCESS;
}

void LogAnsweredRead(const std::wstring& path, PCWSTR valueName, DWORD value) {
    if (g_loggedReads.fetch_add(1) >= kMaxLoggedReads) {
        return;
    }

    g_inHook = true;
    Wh_Log(L"Answered %s\\%s with %lu", path.c_str(), valueName, value);
    g_inHook = false;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
using RegGetValueW_t = decltype(&RegGetValueW);
using RegCloseKey_t = decltype(&RegCloseKey);

// Every function has two entry points in a process: the advapi32 one that
// desktop callers import, and the kernelbase one that the API sets resolve to.
// On current builds these are separate addresses, so both are hooked. On a
// build where advapi32 only forwards, ShareAnExport below keeps the second set
// from being installed twice.
RegOpenKeyExW_t RegOpenKeyExW_Original;
RegOpenKeyExW_t RegOpenKeyExW_System_Original;
RegQueryValueExW_t RegQueryValueExW_Original;
RegQueryValueExW_t RegQueryValueExW_System_Original;
RegGetValueW_t RegGetValueW_Original;
RegGetValueW_t RegGetValueW_System_Original;
RegCloseKey_t RegCloseKey_Original;
RegCloseKey_t RegCloseKey_System_Original;

bool IsReadOnlyAccess(REGSAM samDesired) {
    // The composite rights are avoided on purpose. KEY_WRITE carries
    // READ_CONTROL, which KEY_READ carries as well, so checking for it would
    // mark every read as a write.
    const REGSAM kWriteAccess = GENERIC_WRITE | GENERIC_ALL | KEY_SET_VALUE |
                                KEY_CREATE_SUB_KEY | KEY_CREATE_LINK | DELETE |
                                WRITE_DAC | WRITE_OWNER;
    return (samDesired & kWriteAccess) == 0;
}

// The policy keys only exist once something has set them, which on a stock
// machine they have not. Opening the closest key that does exist gives the
// caller a handle it can read, and the values are answered by the mod anyway.
bool OpenExistingAncestor(RegOpenKeyExW_t original,
                          HKEY hKey,
                          LPCWSTR subKey,
                          REGSAM samDesired,
                          PHKEY result) {
    std::wstring path = subKey;

    for (int attempt = 0; attempt < 8; attempt++) {
        size_t separator = path.find_last_of(L'\\');
        if (separator == std::wstring::npos) {
            break;
        }

        path.erase(separator);
        if (path.empty()) {
            break;
        }

        if (original(hKey, path.c_str(), 0, samDesired, result) ==
            ERROR_SUCCESS) {
            return true;
        }
    }

    // Nothing along the way exists, which is unusual but possible on a stripped
    // down install. Any readable key will do, since the values come from the
    // mod.
    return original(HKEY_CURRENT_USER, L"Software", 0, KEY_READ, result) ==
           ERROR_SUCCESS;
}

LSTATUS RegOpenKeyExW_Impl(RegOpenKeyExW_t original,
                           HKEY hKey,
                           LPCWSTR lpSubKey,
                           DWORD ulOptions,
                           REGSAM samDesired,
                           PHKEY phkResult) {
    if (!IsActive() || !lpSubKey || !phkResult) {
        return original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }

    std::wstring relative;
    bool standIn = false;
    if (!GetRequestedRelativePath(hKey, lpSubKey, relative, standIn) ||
        !IsVirtualPath(relative)) {
        return original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }

    g_inHook = true;

    LSTATUS status = original(hKey, lpSubKey, ulOptions, samDesired, phkResult);

    if (status == ERROR_FILE_NOT_FOUND && IsReadOnlyAccess(samDesired)) {
        if (OpenExistingAncestor(original, hKey, lpSubKey, samDesired,
                                 phkResult)) {
            standIn = true;
            status = ERROR_SUCCESS;
        }
    }

    g_inHook = false;

    // A caller that opened the key for writing gets the real key and the real
    // values, so what it writes is what it reads back. Everything else is
    // tracked so the reads can be answered.
    if (status == ERROR_SUCCESS && IsReadOnlyAccess(samDesired)) {
        TrackKey(*phkResult, relative, standIn);
        g_inHook = true;
        Wh_Log(L"Tracking %s (stand-in %d)", relative.c_str(), standIn);
        g_inHook = false;
    }

    return status;
}

LSTATUS RegQueryValueExW_Impl(RegQueryValueExW_t original,
                              HKEY hKey,
                              LPCWSTR lpValueName,
                              LPDWORD lpReserved,
                              LPDWORD lpType,
                              LPBYTE lpData,
                              LPDWORD lpcbData) {
    if (!IsActive()) {
        return original(hKey, lpValueName, lpReserved, lpType, lpData,
                        lpcbData);
    }

    TrackedKey tracked;
    if (!GetTrackedKey(hKey, tracked)) {
        return original(hKey, lpValueName, lpReserved, lpType, lpData,
                        lpcbData);
    }

    // An empty name is the default value, which a stand-in key never had.
    if (!lpValueName || !*lpValueName) {
        if (tracked.standIn) {
            return ERROR_FILE_NOT_FOUND;
        }

        return original(hKey, lpValueName, lpReserved, lpType, lpData,
                        lpcbData);
    }

    const VirtualValue* value = FindVirtualValue(tracked.path, lpValueName);
    if (!value) {
        // A stand-in does not really hold the search key, so anything but the
        // values the mod answers for has to look missing, the way it would on a
        // key that never existed.
        if (tracked.standIn) {
            return ERROR_FILE_NOT_FOUND;
        }

        return original(hKey, lpValueName, lpReserved, lpType, lpData,
                        lpcbData);
    }

    LogAnsweredRead(tracked.path, lpValueName, value->value);
    return ReturnVirtualDword(value->value, lpType, lpData, lpcbData);
}

// RegGetValue already takes the type restriction the mod has to respect, since
// answering a DWORD for a caller that asked for a string would be wrong. A call
// without any type flag is rejected by the real API, so it is left alone.
bool IsDwordRequested(DWORD flags) {
    const DWORD kTypeMask = 0x0000FFFF;
    DWORD typeFlags = flags & kTypeMask;
    if (typeFlags == 0) {
        return false;
    }

    return typeFlags == RRF_RT_ANY || (typeFlags & RRF_RT_REG_DWORD) != 0;
}

LSTATUS RegGetValueW_Impl(RegGetValueW_t original,
                          HKEY hkey,
                          LPCWSTR lpSubKey,
                          LPCWSTR lpValue,
                          DWORD dwFlags,
                          LPDWORD pdwType,
                          PVOID pvData,
                          LPDWORD pcbData) {
    if (!IsActive() || !lpValue || !IsDwordRequested(dwFlags)) {
        return original(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData,
                        pcbData);
    }

    std::wstring relative;
    bool standIn = false;
    if (!GetRequestedRelativePath(hkey, lpSubKey, relative, standIn)) {
        return original(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData,
                        pcbData);
    }

    const VirtualValue* value = FindVirtualValue(relative, lpValue);
    if (!value) {
        if (standIn) {
            return ERROR_FILE_NOT_FOUND;
        }

        return original(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData,
                        pcbData);
    }

    LogAnsweredRead(relative, lpValue, value->value);
    return ReturnVirtualDword(value->value, pdwType, pvData, pcbData);
}

LSTATUS RegCloseKey_Impl(RegCloseKey_t original, HKEY hKey) {
    if (!g_inHook) {
        UntrackKey(hKey);
    }
    return original(hKey);
}

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                  LPCWSTR lpSubKey,
                                  DWORD ulOptions,
                                  REGSAM samDesired,
                                  PHKEY phkResult) {
    return RegOpenKeyExW_Impl(RegOpenKeyExW_Original, hKey, lpSubKey,
                              ulOptions, samDesired, phkResult);
}

LSTATUS WINAPI RegOpenKeyExW_System_Hook(HKEY hKey,
                                         LPCWSTR lpSubKey,
                                         DWORD ulOptions,
                                         REGSAM samDesired,
                                         PHKEY phkResult) {
    return RegOpenKeyExW_Impl(RegOpenKeyExW_System_Original, hKey, lpSubKey,
                              ulOptions, samDesired, phkResult);
}

LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    return RegQueryValueExW_Impl(RegQueryValueExW_Original, hKey, lpValueName,
                                 lpReserved, lpType, lpData, lpcbData);
}

LSTATUS WINAPI RegQueryValueExW_System_Hook(HKEY hKey,
                                            LPCWSTR lpValueName,
                                            LPDWORD lpReserved,
                                            LPDWORD lpType,
                                            LPBYTE lpData,
                                            LPDWORD lpcbData) {
    return RegQueryValueExW_Impl(RegQueryValueExW_System_Original, hKey,
                                 lpValueName, lpReserved, lpType, lpData,
                                 lpcbData);
}

LSTATUS WINAPI RegGetValueW_Hook(HKEY hkey,
                                 LPCWSTR lpSubKey,
                                 LPCWSTR lpValue,
                                 DWORD dwFlags,
                                 LPDWORD pdwType,
                                 PVOID pvData,
                                 LPDWORD pcbData) {
    return RegGetValueW_Impl(RegGetValueW_Original, hkey, lpSubKey, lpValue,
                             dwFlags, pdwType, pvData, pcbData);
}

LSTATUS WINAPI RegGetValueW_System_Hook(HKEY hkey,
                                        LPCWSTR lpSubKey,
                                        LPCWSTR lpValue,
                                        DWORD dwFlags,
                                        LPDWORD pdwType,
                                        PVOID pvData,
                                        LPDWORD pcbData) {
    return RegGetValueW_Impl(RegGetValueW_System_Original, hkey, lpSubKey,
                             lpValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS WINAPI RegCloseKey_Hook(HKEY hKey) {
    return RegCloseKey_Impl(RegCloseKey_Original, hKey);
}

LSTATUS WINAPI RegCloseKey_System_Hook(HKEY hKey) {
    return RegCloseKey_Impl(RegCloseKey_System_Original, hKey);
}

template <typename T>
bool HookOne(T target, T hook, T* original, PCWSTR moduleName, PCWSTR name) {
    if (!target) {
        Wh_Log(L"No %s!%s", moduleName, name);
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Couldn't hook %s!%s", moduleName, name);
        return false;
    }

    Wh_Log(L"Hooked %s!%s", moduleName, name);
    return true;
}

bool InstallHooks(HMODULE module, bool systemCopy) {
    PCWSTR moduleName = systemCopy ? L"kernelbase" : L"advapi32";

    auto openKey = (RegOpenKeyExW_t)GetProcAddress(module, "RegOpenKeyExW");
    auto queryValue =
        (RegQueryValueExW_t)GetProcAddress(module, "RegQueryValueExW");
    auto getValue = (RegGetValueW_t)GetProcAddress(module, "RegGetValueW");
    auto closeKey = (RegCloseKey_t)GetProcAddress(module, "RegCloseKey");

    bool hooked;

    if (systemCopy) {
        hooked = HookOne(openKey, RegOpenKeyExW_System_Hook,
                         &RegOpenKeyExW_System_Original, moduleName,
                         L"RegOpenKeyExW");
        hooked = HookOne(queryValue, RegQueryValueExW_System_Hook,
                         &RegQueryValueExW_System_Original, moduleName,
                         L"RegQueryValueExW") &&
                 hooked;
        hooked = HookOne(closeKey, RegCloseKey_System_Hook,
                         &RegCloseKey_System_Original, moduleName,
                         L"RegCloseKey") &&
                 hooked;

        // Reads through RegGetValue are an extra path, not the only one, so a
        // failure here is not fatal.
        HookOne(getValue, RegGetValueW_System_Hook,
                &RegGetValueW_System_Original, moduleName, L"RegGetValueW");
    } else {
        hooked =
            HookOne(openKey, RegOpenKeyExW_Hook, &RegOpenKeyExW_Original,
                    moduleName, L"RegOpenKeyExW");
        hooked = HookOne(queryValue, RegQueryValueExW_Hook,
                         &RegQueryValueExW_Original, moduleName,
                         L"RegQueryValueExW") &&
                 hooked;
        hooked = HookOne(closeKey, RegCloseKey_Hook, &RegCloseKey_Original,
                         moduleName, L"RegCloseKey") &&
                 hooked;

        HookOne(getValue, RegGetValueW_Hook, &RegGetValueW_Original, moduleName,
                L"RegGetValueW");
    }

    return hooked;
}

HMODULE LoadSystemModule(PCWSTR name) {
    HMODULE module = GetModuleHandle(name);
    if (module) {
        return module;
    }

    // The mod loads early enough that the DLL may not be mapped yet, so bring
    // it in rather than give up.
    return LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
}

// An export that is only a forwarder resolves to the same code in both modules,
// and hooking one address twice would hook the first hook's trampoline.
bool ShareAnExport(HMODULE first, HMODULE second) {
    static const char* kFunctions[] = {
        "RegOpenKeyExW",
        "RegQueryValueExW",
        "RegGetValueW",
        "RegCloseKey",
    };

    for (const char* function : kFunctions) {
        FARPROC firstAddress = GetProcAddress(first, function);
        FARPROC secondAddress = GetProcAddress(second, function);
        if (firstAddress && secondAddress && firstAddress == secondAddress) {
            return true;
        }
    }

    return false;
}

// @include already limits the mod to the search UI, but a DLL that ends up in
// any other process has no business answering registry reads.
bool IsSearchUiProcess() {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) {
        return false;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;

    return _wcsicmp(fileName, L"SearchHost.exe") == 0 ||
           _wcsicmp(fileName, L"SearchApp.exe") == 0;
}

void LoadSettings() {
    g_hideCloudContent = Wh_GetIntSetting(L"hideCloudContent") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!IsSearchUiProcess()) {
        Wh_Log(L"Not the search UI, nothing to do");
        return FALSE;
    }

    HMODULE advapiModule = LoadSystemModule(L"advapi32.dll");
    HMODULE kernelBaseModule = LoadSystemModule(L"kernelbase.dll");

    bool sharesImplementation = advapiModule && kernelBaseModule &&
                                ShareAnExport(advapiModule, kernelBaseModule);

    if (!kernelBaseModule && !advapiModule) {
        Wh_Log(L"No registry module");
        return FALSE;
    }

    bool hooked = false;

    if (kernelBaseModule) {
        hooked = InstallHooks(kernelBaseModule, true) || hooked;
    }

    if (advapiModule && !sharesImplementation) {
        hooked = InstallHooks(advapiModule, false) || hooked;
    }

    if (!hooked) {
        Wh_Log(L"Couldn't hook the registry functions");
        return FALSE;
    }

    Wh_Log(L"Cloud content results are %s",
           g_hideCloudContent.load() ? L"hidden" : L"left alone");

    g_initialized = true;
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    // The search UI reads the values while it sets up, so what the mod answers
    // is only picked up again after it restarts.
    Wh_Log(L"Settings loaded, restart SearchHost.exe to apply them");
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_initialized = false;

    std::lock_guard<std::mutex> lock(g_trackedMutex);
    g_trackedKeys.clear();
}

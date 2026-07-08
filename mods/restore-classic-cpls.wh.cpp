// ==WindhawkMod==
// @id              restore-classic-cpls
// @name            Restore the classic Personalization and other CPLs
// @description     Brings back the classic Personalization applet and other CPLs.
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Brings back the classic Control Panel applets:

* Personalization
* Notification area icons (for Win10 taskbar)
* Network Connections
* Printers and Faxes
* Suppresses the {98F2AB62-0E29-4E4C-8EE7-B542E66740B1}, originally called "Company Settings Sync", a non-functional icon that may appear if you are using the Classic view of Control Panel

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enablePersonalization: true
  $name: Personalizattion
  $description: Adds "Personalization" icon to the Control Panel
- enableNotificationIcons: true
  $name: Notification area icons
  $description: Adds "Notification area icons" icon to the Control Panel (intended for Windows 10 taskbar)
- enableNetworkConnections: true
  $name: Network connections
  $description: Adds "Network connections" icon to the Control Panel
- enablePrintersAndFaxes: true
  $name: Printers and Faxes
  $description: Adds "Printers and Faxes" icon to the Control Panel
- suppressCompanySync: true
  $name: Suppress the "Company Settings Sync" broken icon
  $description: Removes the {98F2AB62-0E29-4E4C-8EE7-B542E66740B1} non-functional icon
*/
// ==/WindhawkModSettings==

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <vector>

struct Settings {
    bool enablePersonalization;
    bool enableNotificationIcons;
    bool enableNetworkConnections;
    bool enablePrintersAndFaxes;
    bool suppressCompanySync;
} g_settings;

std::wstring g_personalizationName;
std::unordered_map<HKEY, std::wstring> g_keyPaths;
std::unordered_set<HKEY> g_fakeHandles;
std::unordered_map<HKEY, DWORD> g_namespaceEnumIndex;
std::mutex g_keyPathsMutex;

static const std::wstring kPersonalizationGuid    = L"{580722ff-16a7-44c1-bf74-7e1acd00f4f9}";
static const std::wstring kNotificationIconsGuid  = L"{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}";
static const std::wstring kNetworkConnectionsGuid = L"{7007acc7-3202-11d1-aad2-00805fc1270e}";
static const std::wstring kPrintersAndFaxesGuid   = L"{2227a280-3aea-1069-a2de-08002b30309d}";
static const std::wstring kSuppressedGuid         = L"{98f2ab62-0e29-4e4c-8ee7-b542e66740b1}";

static const DWORD kCategoryAppearance = 1;
static const DWORD kCategoryHardware   = 2;
static const DWORD kCategoryNetwork    = 3;

std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = towlower(c);
    return res;
}

bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void LoadSettings() {
    g_settings.enablePersonalization   = Wh_GetIntSetting(L"enablePersonalization");
    g_settings.enableNotificationIcons = Wh_GetIntSetting(L"enableNotificationIcons");
    g_settings.enableNetworkConnections= Wh_GetIntSetting(L"enableNetworkConnections");
    g_settings.enablePrintersAndFaxes  = Wh_GetIntSetting(L"enablePrintersAndFaxes");
    g_settings.suppressCompanySync   = Wh_GetIntSetting(L"suppressCompanySync");
}

void InitDisplayNames() {
    wchar_t buffer[256] = { 0 };
    HMODULE hTheme = LoadLibraryEx(L"themecpl.dll", nullptr, LOAD_LIBRARY_AS_DATAFILE);
    if (hTheme) {
        if (LoadStringW(hTheme, 1, buffer, 256) && buffer[0])
            g_personalizationName = buffer;
        else
            g_personalizationName = L"Personalization";
        FreeLibrary(hTheme);
    } else {
        g_personalizationName = L"Personalization";
    }
}

std::wstring GetTrackedPath(HKEY hKey) {
    if (!hKey) return L"";
    if ((uintptr_t)hKey == 0x80000000) return L"HKEY_CLASSES_ROOT";
    if ((uintptr_t)hKey == 0x80000001) return L"HKEY_CURRENT_USER";
    if ((uintptr_t)hKey == 0x80000002) return L"HKEY_LOCAL_MACHINE";
    if ((uintptr_t)hKey == 0x80000003) return L"HKEY_USERS";
    if ((uintptr_t)hKey == 0x80000004) return L"HKEY_CURRENT_CONFIG";

    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    auto it = g_keyPaths.find(hKey);
    if (it != g_keyPaths.end()) return it->second;
    return L"";
}

void TrackKey(HKEY hKey, const std::wstring& path) {
    if (!hKey || ((uintptr_t)hKey >= 0x80000000 && (uintptr_t)hKey <= 0x80000004)) return;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[hKey] = path;
}

void UntrackKey(HKEY hKey) {
    if (!hKey || ((uintptr_t)hKey >= 0x80000000 && (uintptr_t)hKey <= 0x80000004)) return;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths.erase(hKey);
}

HKEY CreateFakeHandle(const std::wstring& path) {
    int* dummy = new int(1);
    HKEY fake = (HKEY)dummy;
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    g_keyPaths[fake] = path;
    g_fakeHandles.insert(fake);
    return fake;
}

void FreeFakeHandle(HKEY hKey) {
    std::lock_guard<std::mutex> lock(g_keyPathsMutex);
    if (g_fakeHandles.count(hKey)) {
        g_fakeHandles.erase(hKey);
        g_keyPaths.erase(hKey);
        delete (int*)hKey;
    }
}

enum class VNode {
    None,
    ClsidRoot, DefaultIcon, Shell, ShellOpen, OpenCommand, NameSpaceEntry,
    ClsidRootCategoryOnly,
    Suppressed
};

enum class ItemKind { None, Personalization, CategoryOnly, Suppressed };

struct ClassifyResult {
    VNode    node;
    ItemKind kind;
    DWORD    category;
};

bool IsSuppressedNamespaceKey(const std::wstring& lower) {
    if (!g_settings.suppressCompanySync) return false;
    return EndsWith(lower, L"controlpanel\\namespace\\" + ToLower(kSuppressedGuid));
}

bool IsSuppressedNamespaceEntry(LPCWSTR name) {
    if (!g_settings.suppressCompanySync || !name) return false;
    return ToLower(name) == ToLower(kSuppressedGuid);
}

ClassifyResult ClassifyFullVirtual(const std::wstring& lower,
                                   const std::wstring& guid,
                                   ItemKind kind) {
    std::wstring g = ToLower(guid);
    if (EndsWith(lower, L"clsid\\" + g))                             return { VNode::ClsidRoot,     kind, 0 };
    if (EndsWith(lower, L"clsid\\" + g + L"\\defaulticon"))          return { VNode::DefaultIcon,   kind, 0 };
    if (EndsWith(lower, L"clsid\\" + g + L"\\shell"))                return { VNode::Shell,          kind, 0 };
    if (EndsWith(lower, L"clsid\\" + g + L"\\shell\\open"))          return { VNode::ShellOpen,      kind, 0 };
    if (EndsWith(lower, L"clsid\\" + g + L"\\shell\\open\\command")) return { VNode::OpenCommand,    kind, 0 };
    if (EndsWith(lower, L"controlpanel\\namespace\\" + g))           return { VNode::NameSpaceEntry, kind, 0 };
    return { VNode::None, ItemKind::None, 0 };
}

ClassifyResult ClassifyPath(const std::wstring& path) {
    std::wstring lower = ToLower(path);

    if (g_settings.suppressCompanySync) {
        std::wstring g = ToLower(kSuppressedGuid);
        if (EndsWith(lower, L"clsid\\" + g) ||
            EndsWith(lower, L"controlpanel\\namespace\\" + g))
            return { VNode::Suppressed, ItemKind::Suppressed, 0 };
    }

    if (g_settings.enablePersonalization) {
        auto cr = ClassifyFullVirtual(lower, kPersonalizationGuid, ItemKind::Personalization);
        if (cr.node != VNode::None) return cr;
    }

    struct { bool* enabled; const std::wstring* guid; DWORD cat; } categoryItems[] = {
        { &g_settings.enableNotificationIcons,  &kNotificationIconsGuid,  kCategoryAppearance },
        { &g_settings.enableNetworkConnections, &kNetworkConnectionsGuid, kCategoryNetwork    },
        { &g_settings.enablePrintersAndFaxes,   &kPrintersAndFaxesGuid,   kCategoryHardware   },
    };
    for (auto& item : categoryItems) {
        if (!*item.enabled) continue;
        if (EndsWith(lower, L"clsid\\" + ToLower(*item.guid)))
            return { VNode::ClsidRootCategoryOnly, ItemKind::CategoryOnly, item.cat };
    }

    return { VNode::None, ItemKind::None, 0 };
}

bool IsTargetKey(const std::wstring& path) {
    return ClassifyPath(path).node != VNode::None;
}

bool IsNameSpaceParentKey(const std::wstring& path) {
    return EndsWith(ToLower(path), L"controlpanel\\namespace");
}

LSTATUS ProvideStringValue(LPBYTE lpData, LPDWORD lpcbData, const std::wstring& str) {
    DWORD requiredSize = (DWORD)((str.length() + 1) * sizeof(wchar_t));
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData || *lpcbData < requiredSize) {
        *lpcbData = requiredSize;
        return ERROR_MORE_DATA;
    }
    *lpcbData = requiredSize;
    memcpy(lpData, str.c_str(), requiredSize);
    return ERROR_SUCCESS;
}

LSTATUS ProvideDwordValue(LPBYTE lpData, LPDWORD lpcbData, DWORD value) {
    if (!lpcbData) return ERROR_INVALID_PARAMETER;
    if (!lpData || *lpcbData < sizeof(DWORD)) {
        *lpcbData = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    *lpcbData = sizeof(DWORD);
    *(DWORD*)lpData = value;
    return ERROR_SUCCESS;
}

bool TryProvideValue(const std::wstring& path, const std::wstring& valueName,
                     LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData, LSTATUS& outStatus) {
    ClassifyResult cr = ClassifyPath(path);
    if (cr.node == VNode::None) return false;

    if (cr.kind == ItemKind::Suppressed) {
        outStatus = ERROR_FILE_NOT_FOUND;
        return true;
    }

    if (cr.kind == ItemKind::CategoryOnly) {
        if (valueName == L"System.ControlPanel.Category") {
            if (lpType) *lpType = REG_DWORD;
            outStatus = ProvideDwordValue(lpData, lpcbData, cr.category);
            return true;
        }
        return false;
    }

    if (cr.kind == ItemKind::Personalization) {
        if (cr.node == VNode::NameSpaceEntry) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            }
        } else if (cr.node == VNode::ClsidRoot) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, g_personalizationName);
                return true;
            } else if (valueName == L"InfoTip") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"@%SystemRoot%\\System32\\themecpl.dll,-2#immutable1");
                return true;
            } else if (valueName == L"System.ApplicationName") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"Microsoft.Personalization");
                return true;
            } else if (valueName == L"System.ControlPanel.Category") {
                if (lpType) *lpType = REG_DWORD;
                outStatus = ProvideDwordValue(lpData, lpcbData, kCategoryAppearance);
                return true;
            } else if (valueName == L"System.Software.TasksFileUrl") {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"Internal");
                return true;
            }
        } else if (cr.node == VNode::DefaultIcon) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"%SystemRoot%\\System32\\themecpl.dll,-1");
                return true;
            }
        } else if (cr.node == VNode::OpenCommand) {
            if (valueName.empty()) {
                if (lpType) *lpType = REG_SZ;
                outStatus = ProvideStringValue(lpData, lpcbData, L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}");
                return true;
            }
        }
    }

    return false;
}

std::vector<std::wstring> GetNamespaceClsids() {
    std::vector<std::wstring> result;
    if (g_settings.enablePersonalization)    result.push_back(kPersonalizationGuid);
    if (g_settings.enableNotificationIcons)  result.push_back(kNotificationIconsGuid);
    if (g_settings.enableNetworkConnections) result.push_back(kNetworkConnectionsGuid);
    if (g_settings.enablePrintersAndFaxes)   result.push_back(kPrintersAndFaxesGuid);
    return result;
}

bool GetVirtualSubKeyName(VNode node, DWORD index, std::wstring& outName) {
    switch (node) {
        case VNode::ClsidRoot:
            if (index == 0) { outName = L"DefaultIcon"; return true; }
            if (index == 1) { outName = L"Shell";       return true; }
            return false;
        case VNode::Shell:
            if (index == 0) { outName = L"Open"; return true; }
            return false;
        case VNode::ShellOpen:
            if (index == 0) { outName = L"command"; return true; }
            return false;
        default:
            return false;
    }
}

LSTATUS TryInjectNamespaceEx(HKEY hKey, LPWSTR lpName, LPDWORD lpcchName,
                              PFILETIME lpftLastWriteTime) {
    std::vector<std::wstring> clsids = GetNamespaceClsids();
    if (clsids.empty()) return ERROR_NO_MORE_ITEMS;

    DWORD injectedIndex = 0;
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        auto it = g_namespaceEnumIndex.find(hKey);
        if (it != g_namespaceEnumIndex.end()) injectedIndex = it->second;
    }
    if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;

    const wchar_t* clsid = clsids[injectedIndex].c_str();
    size_t len = wcslen(clsid);
    if (lpcchName && *lpcchName < len + 1) {
        *lpcchName = (DWORD)(len + 1);
        return ERROR_MORE_DATA;
    }
    if (lpName && lpcchName) wcscpy_s(lpName, *lpcchName, clsid);
    if (lpcchName) *lpcchName = (DWORD)len;
    if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        g_namespaceEnumIndex[hKey] = injectedIndex + 1;
    }
    return ERROR_SUCCESS;
}

LSTATUS TryInjectNamespace(HKEY hKey, LPWSTR lpName, DWORD cchName) {
    std::vector<std::wstring> clsids = GetNamespaceClsids();
    if (clsids.empty()) return ERROR_NO_MORE_ITEMS;

    DWORD injectedIndex = 0;
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        auto it = g_namespaceEnumIndex.find(hKey);
        if (it != g_namespaceEnumIndex.end()) injectedIndex = it->second;
    }
    if (injectedIndex >= clsids.size()) return ERROR_NO_MORE_ITEMS;

    const wchar_t* clsid = clsids[injectedIndex].c_str();
    size_t len = wcslen(clsid);
    if (cchName <= len) return ERROR_MORE_DATA;
    if (lpName) wcscpy_s(lpName, cchName, clsid);
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        g_namespaceEnumIndex[hKey] = injectedIndex + 1;
    }
    return ERROR_SUCCESS;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExWOriginal;
LSTATUS WINAPI RegOpenKeyExWHook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                 REGSAM samDesired, PHKEY phkResult) {
    bool parentIsFake = false;
    std::wstring parentPath;
    {
        std::lock_guard<std::mutex> lock(g_keyPathsMutex);
        if (g_fakeHandles.count(hKey)) {
            parentIsFake = true;
            auto it = g_keyPaths.find(hKey);
            if (it != g_keyPaths.end()) parentPath = it->second;
        }
    }

    if (parentIsFake) {
        std::wstring fullPath = parentPath;
        if (lpSubKey && *lpSubKey) {
            if (!fullPath.empty()) fullPath += L"\\";
            fullPath += lpSubKey;
        }
        if (IsTargetKey(fullPath)) {
            HKEY fake = CreateFakeHandle(fullPath);
            if (phkResult) *phkResult = fake;
            return ERROR_SUCCESS;
        }
        return ERROR_FILE_NOT_FOUND;
    }

    if (g_settings.suppressCompanySync && lpSubKey) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (*lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        if (IsSuppressedNamespaceKey(ToLower(fullPath))) return ERROR_FILE_NOT_FOUND;
    }

    LSTATUS status = RegOpenKeyExWOriginal(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    if (status == ERROR_SUCCESS && phkResult && *phkResult) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        TrackKey(*phkResult, fullPath);
    } else if (status == ERROR_FILE_NOT_FOUND && phkResult) {
        std::wstring basePath = GetTrackedPath(hKey);
        std::wstring fullPath = basePath;
        if (lpSubKey && *lpSubKey) { if (!fullPath.empty()) fullPath += L"\\"; fullPath += lpSubKey; }
        if (IsTargetKey(fullPath)) {
            HKEY fake = CreateFakeHandle(fullPath);
            *phkResult = fake;
            return ERROR_SUCCESS;
        }
    }
    return status;
}

using RegCloseKey_t = decltype(&RegCloseKey);
RegCloseKey_t RegCloseKeyOriginal;
LSTATUS WINAPI RegCloseKeyHook(HKEY hKey) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) { FreeFakeHandle(hKey); return ERROR_SUCCESS; }
    LSTATUS status = RegCloseKeyOriginal(hKey);
    UntrackKey(hKey);
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); g_namespaceEnumIndex.erase(hKey); }
    return status;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExWOriginal;
LSTATUS WINAPI RegQueryValueExWHook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                                    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    std::wstring path = GetTrackedPath(hKey);
    std::wstring valueName = lpValueName ? lpValueName : L"";
    LSTATUS outStatus;
    if (TryProvideValue(path, valueName, lpType, lpData, lpcbData, outStatus)) return outStatus;

    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) return ERROR_FILE_NOT_FOUND;

    return RegQueryValueExWOriginal(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueWOriginal;
LSTATUS WINAPI RegGetValueWHook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
                                DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    std::wstring path = GetTrackedPath(hkey);
    if (lpSubKey && *lpSubKey) { if (!path.empty()) path += L"\\"; path += lpSubKey; }
    std::wstring valueName = lpValue ? lpValue : L"";
    LSTATUS outStatus;
    if (TryProvideValue(path, valueName, pdwType, (LPBYTE)pvData, pcbData, outStatus)) return outStatus;
    return RegGetValueWOriginal(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

using RegEnumKeyExW_t = decltype(&RegEnumKeyExW);
RegEnumKeyExW_t RegEnumKeyExWOriginal;
LSTATUS WINAPI RegEnumKeyExWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName,
                                 LPDWORD lpReserved, LPWSTR lpClass, LPDWORD lpcchClass,
                                 PFILETIME lpftLastWriteTime) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) {
        std::wstring path = GetTrackedPath(hKey);
        ClassifyResult cr = ClassifyPath(path);
        std::wstring subName;
        if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
        if (lpcchName && *lpcchName < subName.size() + 1) {
            *lpcchName = (DWORD)(subName.size() + 1); return ERROR_MORE_DATA;
        }
        if (lpName) wcscpy_s(lpName, subName.size() + 1, subName.c_str());
        if (lpcchName) *lpcchName = (DWORD)subName.size();
        if (lpftLastWriteTime) GetSystemTimeAsFileTime(lpftLastWriteTime);
        return ERROR_SUCCESS;
    }

    std::wstring path = GetTrackedPath(hKey);
    bool isNamespace = IsNameSpaceParentKey(path);

    if (isNamespace && g_settings.suppressCompanySync) {
        DWORD realIndex = dwIndex;
        LSTATUS status;
        wchar_t nameBuf[256];
        DWORD cch;
        while (true) {
            cch = lpcchName ? *lpcchName : 256;
            LPWSTR namePtr = lpName ? lpName : nameBuf;
            LPDWORD cchPtr = lpcchName ? lpcchName : &cch;
            status = RegEnumKeyExWOriginal(hKey, realIndex, namePtr, cchPtr,
                                          lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
            if (status != ERROR_SUCCESS) break;
            if (!IsSuppressedNamespaceEntry(namePtr)) break;
            realIndex++;
        }
        if (status == ERROR_NO_MORE_ITEMS)
            return TryInjectNamespaceEx(hKey, lpName, lpcchName, lpftLastWriteTime);
        return status;
    }

    LSTATUS status = RegEnumKeyExWOriginal(hKey, dwIndex, lpName, lpcchName,
                                          lpReserved, lpClass, lpcchClass, lpftLastWriteTime);
    if (isNamespace && status == ERROR_NO_MORE_ITEMS)
        return TryInjectNamespaceEx(hKey, lpName, lpcchName, lpftLastWriteTime);
    return status;
}

using RegEnumKeyW_t = decltype(&RegEnumKeyW);
RegEnumKeyW_t RegEnumKeyWOriginal;
LSTATUS WINAPI RegEnumKeyWHook(HKEY hKey, DWORD dwIndex, LPWSTR lpName, DWORD cchName) {
    bool isFake = false;
    { std::lock_guard<std::mutex> lock(g_keyPathsMutex); isFake = g_fakeHandles.count(hKey) > 0; }
    if (isFake) {
        std::wstring path = GetTrackedPath(hKey);
        ClassifyResult cr = ClassifyPath(path);
        std::wstring subName;
        if (!GetVirtualSubKeyName(cr.node, dwIndex, subName)) return ERROR_NO_MORE_ITEMS;
        if (cchName <= subName.size()) return ERROR_MORE_DATA;
        wcscpy_s(lpName, cchName, subName.c_str());
        return ERROR_SUCCESS;
    }

    std::wstring path = GetTrackedPath(hKey);
    bool isNamespace = IsNameSpaceParentKey(path);

    if (isNamespace && g_settings.suppressCompanySync) {
        DWORD realIndex = dwIndex;
        LSTATUS status;
        while (true) {
            status = RegEnumKeyWOriginal(hKey, realIndex, lpName, cchName);
            if (status != ERROR_SUCCESS) break;
            if (!IsSuppressedNamespaceEntry(lpName)) break;
            realIndex++;
        }
        if (status == ERROR_NO_MORE_ITEMS)
            return TryInjectNamespace(hKey, lpName, cchName);
        return status;
    }

    LSTATUS status = RegEnumKeyWOriginal(hKey, dwIndex, lpName, cchName);
    if (isNamespace && status == ERROR_NO_MORE_ITEMS)
        return TryInjectNamespace(hKey, lpName, cchName);
    return status;
}

void* GetRegFunc(const char* name) {
    HMODULE hKb = GetModuleHandleW(L"kernelbase.dll");
    if (hKb) { void* p = (void*)GetProcAddress(hKb, name); if (p) return p; }
    HMODULE hAdv = GetModuleHandleW(L"advapi32.dll");
    if (!hAdv) hAdv = LoadLibraryW(L"advapi32.dll");
    if (hAdv) { void* p = (void*)GetProcAddress(hAdv, name); if (p) return p; }
    return nullptr;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

BOOL Wh_ModInit() {
    LoadSettings();

    void* pRegOpenKeyExW   = GetRegFunc("RegOpenKeyExW");
    void* pRegCloseKey     = GetRegFunc("RegCloseKey");
    void* pRegQueryValueExW= GetRegFunc("RegQueryValueExW");
    void* pRegGetValueW    = GetRegFunc("RegGetValueW");
    void* pRegEnumKeyExW   = GetRegFunc("RegEnumKeyExW");
    void* pRegEnumKeyW     = GetRegFunc("RegEnumKeyW");

    if (!pRegOpenKeyExW || !pRegCloseKey || !pRegQueryValueExW ||
        !pRegGetValueW  || !pRegEnumKeyExW || !pRegEnumKeyW) {
        Wh_Log(L"Failed to get one or more registry functions");
        return FALSE;
    }

    InitDisplayNames();

    if (!Wh_SetFunctionHook(pRegOpenKeyExW,    (void*)RegOpenKeyExWHook,    (void**)&RegOpenKeyExWOriginal))    { Wh_Log(L"Failed to hook RegOpenKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegCloseKey,      (void*)RegCloseKeyHook,      (void**)&RegCloseKeyOriginal))      { Wh_Log(L"Failed to hook RegCloseKey");      return FALSE; }
    if (!Wh_SetFunctionHook(pRegQueryValueExW, (void*)RegQueryValueExWHook, (void**)&RegQueryValueExWOriginal)) { Wh_Log(L"Failed to hook RegQueryValueExW"); return FALSE; }
    if (!Wh_SetFunctionHook(pRegGetValueW,     (void*)RegGetValueWHook,     (void**)&RegGetValueWOriginal))     { Wh_Log(L"Failed to hook RegGetValueW");     return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyExW,    (void*)RegEnumKeyExWHook,    (void**)&RegEnumKeyExWOriginal))    { Wh_Log(L"Failed to hook RegEnumKeyExW");    return FALSE; }
    if (!Wh_SetFunctionHook(pRegEnumKeyW,      (void*)RegEnumKeyWHook,      (void**)&RegEnumKeyWOriginal))      { Wh_Log(L"Failed to hook RegEnumKeyW");      return FALSE; }

    Wh_Log(L"Hooks set successfully");
    return TRUE;
}

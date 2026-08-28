// ==WindhawkMod==
// @id              persistent-tray-icon-visibility
// @name            Persistent Tray Icon Visibility
// @description     Keeps tray icons pinned or hidden after app updates and optionally removes obsolete versioned entries
// @version         1.0
// @author          Nelethor
// @github          https://github.com/Nelethor
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Persistent Tray Icon Visibility

Windows 11 can forget whether a tray icon was pinned or hidden when an app
update changes the executable path. This is common with Electron/Squirrel and
MSIX apps because the version number is part of their installation directory.

This mod watches:

`HKCU\Control Panel\NotifyIconSettings`

When an update creates a new entry for a recognized app, the mod copies the
previous `IsPromoted` preference:

- `1` keeps the icon visible directly on the taskbar.
- `0` keeps the icon in the tray overflow menu.

## Main options

### Preserve visibility across app updates

Matches the new versioned executable path with the previous version and applies
the same pinned or hidden state.

### Learn visibility changes automatically

Remembers manual visibility changes made in Windows settings. The learned state
is stored in Windhawk's local mod storage, so it can still be restored after a
restart or when an updater removes the old registry entry before creating the
new one.

### Clean obsolete tray entries

Optional and disabled by default. It removes only obsolete entries belonging to
recognized versioned applications when a matching live replacement exists:

- Regular versioned apps: the old executable must no longer exist.
- MSIX apps: the old package must no longer be registered for the current user.

The cleaner deletes registry entries only. It never deletes application files,
packages, or folders. It also does not remove arbitrary duplicates such as
multiple `explorer.exe` entries or identical non-versioned executable paths.

## Recognized update paths

It recognizes versioned Squirrel/Electron directories such as `app-12.45.0`
and versioned MSIX package directories under `WindowsApps`, including registry
paths which begin with a Known Folder GUID or encode package-name underscores
as `\_`.

## Safety and operation

- Runs as a Windhawk tool in a dedicated `windhawk.exe` process.
- Does not inject into Explorer.
- Does not require administrator rights.
- Uses `RegNotifyChangeKeyValue`; the registry is not polled.
- Logs every `IsPromoted` write and every cleanup deletion before it happens.
- Never deletes registry entries unless the cleaner option is enabled.

## First run

The mod begins learning from the state that exists when it starts. It cannot
reconstruct a preference which was already lost before the first run unless a
previously learned state is still present in Windhawk's local mod storage.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preserveVisibility: true
  $name: Preserve visibility across app updates
  $description: Keep each recognized app pinned or hidden when an update creates a new versioned tray entry.
- learnVisibilityChanges: true
  $name: Learn visibility changes automatically
  $description: Remember manual pinned or hidden changes for future versions, restarts, and updates which remove the previous entry first.
- cleanObsoleteEntries: false
  $name: Clean obsolete tray entries
  $description: Remove obsolete registry entries for recognized versioned apps when a matching live replacement exists. Application files are never deleted.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <appmodel.h>
#include <objbase.h>
#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cwctype>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr wchar_t kNotifyIconSettingsPath[] =
    L"Control Panel\\NotifyIconSettings";

struct Settings {
    bool preserveVisibility;
    bool learnVisibilityChanges;
    bool cleanObsoleteEntries;
};

struct Entry {
    std::wstring subkey;
    std::wstring executablePath;
    std::wstring normalizedIdentity;
    FILETIME lastWrite{};
    int promoted = -1;
    bool pathWasVersionNormalized = false;
    bool executableExists = false;
    bool packageRegistrationKnown = false;
    bool packageRegistered = false;
};

std::atomic_bool g_preserveVisibility{true};
std::atomic_bool g_learnVisibilityChanges{true};
std::atomic_bool g_cleanObsoleteEntries{false};
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsChangedEvent = nullptr;
HANDLE g_workerThread = nullptr;
thread_local bool g_knownFolderResolutionAvailable = false;

using EntryMap = std::unordered_map<std::wstring, Entry>;
using StateMap = std::unordered_map<std::wstring, int>;

Settings GetSettingsSnapshot() {
    return {
        g_preserveVisibility.load(std::memory_order_relaxed),
        g_learnVisibilityChanges.load(std::memory_order_relaxed),
        g_cleanObsoleteEntries.load(std::memory_order_relaxed),
    };
}

void LoadSettings() {
    g_preserveVisibility.store(Wh_GetIntSetting(L"preserveVisibility") != 0,
                               std::memory_order_relaxed);
    g_learnVisibilityChanges.store(
        Wh_GetIntSetting(L"learnVisibilityChanges") != 0,
        std::memory_order_relaxed);
    g_cleanObsoleteEntries.store(
        Wh_GetIntSetting(L"cleanObsoleteEntries") != 0,
        std::memory_order_relaxed);
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
    return value;
}

bool IsAsciiDigit(wchar_t ch) {
    return ch >= L'0' && ch <= L'9';
}

bool LooksLikeVersion(const std::wstring& value) {
    // Require at least two numeric components. Suffixes such as -beta.1 are
    // accepted, but the token must begin with a digit and contain a dot.
    if (value.empty() || !IsAsciiDigit(value.front())) {
        return false;
    }

    bool sawDot = false;
    bool previousWasDot = false;
    size_t index = 0;
    for (; index < value.size(); ++index) {
        wchar_t ch = value[index];
        if (IsAsciiDigit(ch)) {
            previousWasDot = false;
            continue;
        }
        if (ch == L'.') {
            if (index == 0 || previousWasDot || index + 1 == value.size()) {
                return false;
            }
            sawDot = true;
            previousWasDot = true;
            continue;
        }
        break;
    }

    if (!sawDot || previousWasDot) {
        return false;
    }

    // A prerelease/build suffix may contain only conservative semver-like
    // characters. This avoids treating an arbitrary folder as a version.
    for (; index < value.size(); ++index) {
        wchar_t ch = value[index];
        if (!(std::iswalnum(ch) || ch == L'.' || ch == L'-' || ch == L'+')) {
            return false;
        }
    }
    return true;
}

std::vector<std::wstring> SplitKeepingEmpty(const std::wstring& value,
                                             wchar_t separator) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    while (true) {
        size_t end = value.find(separator, start);
        parts.push_back(value.substr(
            start, end == std::wstring::npos ? end : end - start));
        if (end == std::wstring::npos) {
            return parts;
        }
        start = end + 1;
    }
}

bool IsMsixArchitecture(const std::wstring& value) {
    std::wstring lower = ToLower(value);
    return lower == L"x86" || lower == L"x64" || lower == L"arm" ||
           lower == L"arm64" || lower == L"neutral";
}

std::wstring NormalizePathComponent(const std::wstring& component,
                                    bool* changed) {
    std::wstring lower = ToLower(component);

    // Squirrel/Electron: app-1.2.3, app-12.45.0-beta.1, etc.
    constexpr wchar_t kAppPrefix[] = L"app-";
    if (lower.rfind(kAppPrefix, 0) == 0 &&
        LooksLikeVersion(lower.substr(ARRAYSIZE(kAppPrefix) - 1))) {
        *changed = true;
        return L"app-{version}";
    }

    // MSIX package full name:
    // Name_Version_Architecture_ResourceId_PublisherId.
    // Search for the version/architecture pair so names containing underscores
    // remain intact and an empty ResourceId is preserved.
    auto parts = SplitKeepingEmpty(lower, L'_');
    if (parts.size() >= 5) {
        for (size_t i = 1; i + 3 < parts.size(); ++i) {
            if (!LooksLikeVersion(parts[i]) ||
                !IsMsixArchitecture(parts[i + 1])) {
                continue;
            }

            parts[i] = L"{version}";
            std::wstring result;
            for (size_t partIndex = 0; partIndex < parts.size(); ++partIndex) {
                if (partIndex) {
                    result.push_back(L'_');
                }
                result += parts[partIndex];
            }
            *changed = true;
            return result;
        }
    }

    return lower;
}

std::wstring ResolveKnownFolderPrefix(const std::wstring& path) {
    // NotifyIconSettings commonly stores paths as, for example:
    // {6D809377-6AF0-444B-8957-A3773F02200E}\WindowsApps\...
    // Resolve the leading KNOWNFOLDERID before testing whether the EXE exists.
    constexpr size_t kGuidStringLength = 38;
    if (!g_knownFolderResolutionAvailable ||
        path.size() < kGuidStringLength || path[0] != L'{' ||
        path[kGuidStringLength - 1] != L'}' ||
        (path.size() > kGuidStringLength &&
         path[kGuidStringLength] != L'\\' &&
         path[kGuidStringLength] != L'/')) {
        return path;
    }

    std::wstring guidString = path.substr(0, kGuidStringLength);
    GUID folderId{};
    if (FAILED(CLSIDFromString(guidString.c_str(), &folderId))) {
        return path;
    }

    PWSTR knownFolderPath = nullptr;
    HRESULT result = SHGetKnownFolderPath(
        folderId, KF_FLAG_DONT_VERIFY, nullptr, &knownFolderPath);
    if (FAILED(result) || !knownFolderPath) {
        return path;
    }

    std::wstring resolved = knownFolderPath;
    CoTaskMemFree(knownFolderPath);
    if (path.size() > kGuidStringLength) {
        if (!resolved.empty() && resolved.back() == L'\\') {
            resolved.pop_back();
        }
        resolved += path.substr(kGuidStringLength);
    }
    return resolved;
}

std::wstring DecodeWindowsAppsPath(std::wstring path) {
    // Explorer can store a shell parsing path instead of a literal filesystem
    // path. Within WindowsApps, underscores in the package full name are then
    // represented as "\_", for example:
    // Name\_2.0.0.0\_x64\_\_publisher\App.exe
    // Decode only this WindowsApps form so ordinary directories named _foo are
    // left untouched elsewhere.
    std::wstring lower = ToLower(path);
    constexpr wchar_t kWindowsAppsMarker[] = L"\\windowsapps\\";
    size_t windowsApps = lower.find(kWindowsAppsMarker);
    if (windowsApps == std::wstring::npos) {
        return path;
    }

    size_t searchFrom = windowsApps + ARRAYSIZE(kWindowsAppsMarker) - 1;
    while (true) {
        size_t escapedUnderscore = path.find(L"\\_", searchFrom);
        if (escapedUnderscore == std::wstring::npos) {
            break;
        }
        path.erase(escapedUnderscore, 1);
        searchFrom = escapedUnderscore + 1;
    }
    return path;
}

std::wstring PrepareExecutablePath(const std::wstring& path) {
    return DecodeWindowsAppsPath(ResolveKnownFolderPrefix(path));
}

std::wstring TrimAndExpandPath(const std::wstring& input) {
    size_t first = input.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return {};
    }
    size_t last = input.find_last_not_of(L" \t\r\n");
    std::wstring path = input.substr(first, last - first + 1);
    if (path.size() >= 2 && path.front() == L'\"' && path.back() == L'\"') {
        path = path.substr(1, path.size() - 2);
    }

    DWORD required = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
    if (!required) {
        return PrepareExecutablePath(path);
    }
    std::vector<wchar_t> buffer(required);
    if (!ExpandEnvironmentStringsW(path.c_str(), buffer.data(), required)) {
        return PrepareExecutablePath(path);
    }
    return PrepareExecutablePath(buffer.data());
}

std::wstring NormalizeExecutablePath(const std::wstring& input,
                                     bool* versionWasNormalized) {
    std::wstring path = TrimAndExpandPath(input);
    std::replace(path.begin(), path.end(), L'/', L'\\');

    std::wstring result;
    size_t start = 0;
    while (true) {
        size_t end = path.find(L'\\', start);
        std::wstring component = path.substr(
            start, end == std::wstring::npos ? end : end - start);
        if (!result.empty() || start > 0) {
            result.push_back(L'\\');
        }
        result += NormalizePathComponent(component, versionWasNormalized);
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    return result;
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool GetMsixPackageRegistration(const std::wstring& executablePath,
                                bool* registered) {
    std::wstring lower = ToLower(executablePath);
    constexpr wchar_t kWindowsAppsMarker[] = L"\\windowsapps\\";
    size_t marker = lower.find(kWindowsAppsMarker);
    if (marker == std::wstring::npos) {
        return false;
    }

    size_t packageStart = marker + ARRAYSIZE(kWindowsAppsMarker) - 1;
    size_t packageEnd = executablePath.find(L'\\', packageStart);
    if (packageEnd == std::wstring::npos || packageEnd == packageStart) {
        return false;
    }

    std::wstring packageFullName =
        executablePath.substr(packageStart, packageEnd - packageStart);
    UINT32 bufferLength = 0;
    LONG result = PackageIdFromFullName(packageFullName.c_str(),
                                        PACKAGE_INFORMATION_FULL,
                                        &bufferLength, nullptr);
    if (result == ERROR_INSUFFICIENT_BUFFER || result == ERROR_SUCCESS) {
        *registered = true;
        return true;
    }
    if (result == ERROR_NOT_FOUND) {
        *registered = false;
        return true;
    }

    // Unknown/access-related results are not treated as "not registered".
    // The cleaner will fall back to the conservative filesystem check.
    return false;
}

bool ReadRegistryString(HKEY key, const wchar_t* valueName,
                        std::wstring* result) {
    DWORD type = 0;
    DWORD bytes = 0;
    LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &type, nullptr,
                                      &bytes);
    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) || bytes == 0) {
        return false;
    }

    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t) + 1, L'\0');
    status = RegQueryValueExW(key, valueName, nullptr, &type,
                              reinterpret_cast<BYTE*>(buffer.data()), &bytes);
    if (status != ERROR_SUCCESS) {
        return false;
    }
    buffer.back() = L'\0';
    *result = buffer.data();
    return !result->empty();
}

int ReadPromoted(HKEY key) {
    DWORD value = 0;
    DWORD type = 0;
    DWORD bytes = sizeof(value);
    LSTATUS status = RegQueryValueExW(
        key, L"IsPromoted", nullptr, &type, reinterpret_cast<BYTE*>(&value),
        &bytes);
    if (status != ERROR_SUCCESS || type != REG_DWORD ||
        bytes != sizeof(value)) {
        return -1;
    }
    return value != 0 ? 1 : 0;
}

EntryMap ReadEntries(HKEY baseKey) {
    EntryMap result;
    DWORD subkeyCount = 0;
    DWORD maxSubkeyLength = 0;
    if (RegQueryInfoKeyW(baseKey, nullptr, nullptr, nullptr, &subkeyCount,
                         &maxSubkeyLength, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr) != ERROR_SUCCESS) {
        return result;
    }

    std::vector<wchar_t> name(maxSubkeyLength + 2);
    for (DWORD index = 0; index < subkeyCount; ++index) {
        DWORD nameLength = static_cast<DWORD>(name.size());
        FILETIME lastWrite{};
        LSTATUS status = RegEnumKeyExW(baseKey, index, name.data(), &nameLength,
                                       nullptr, nullptr, nullptr, &lastWrite);
        if (status == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (status != ERROR_SUCCESS) {
            continue;
        }

        std::wstring subkeyName(name.data(), nameLength);
        HKEY entryKey = nullptr;
        if (RegOpenKeyExW(baseKey, subkeyName.c_str(), 0, KEY_QUERY_VALUE,
                          &entryKey) != ERROR_SUCCESS) {
            continue;
        }

        Entry entry;
        entry.subkey = subkeyName;
        entry.lastWrite = lastWrite;
        if (!ReadRegistryString(entryKey, L"ExecutablePath",
                                &entry.executablePath)) {
            RegCloseKey(entryKey);
            continue;
        }
        entry.promoted = ReadPromoted(entryKey);
        RegCloseKey(entryKey);

        entry.executablePath = TrimAndExpandPath(entry.executablePath);
        entry.normalizedIdentity = NormalizeExecutablePath(
            entry.executablePath, &entry.pathWasVersionNormalized);
        entry.executableExists = FileExists(entry.executablePath);
        entry.packageRegistrationKnown = GetMsixPackageRegistration(
            entry.executablePath, &entry.packageRegistered);
        result.emplace(ToLower(subkeyName), std::move(entry));
    }
    return result;
}

uint64_t Fnv1a64(const std::wstring& value) {
    uint64_t hash = 14695981039346656037ULL;
    for (wchar_t ch : value) {
        uint16_t codeUnit = static_cast<uint16_t>(ch);
        hash ^= static_cast<uint8_t>(codeUnit & 0xff);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint8_t>(codeUnit >> 8);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::wstring LearnedValueName(const wchar_t* kind,
                              const std::wstring& identity) {
    wchar_t buffer[80];
    swprintf_s(buffer, L"learned-%s-%016llx", kind,
               static_cast<unsigned long long>(Fnv1a64(identity)));
    return buffer;
}

bool GetLearnedState(const std::wstring& identity, int* state) {
    std::wstring identityName = LearnedValueName(L"identity", identity);
    std::vector<wchar_t> storedIdentity(32768, L'\0');
    if (!Wh_GetStringValue(identityName.c_str(), storedIdentity.data(),
                           storedIdentity.size()) ||
        identity != storedIdentity.data()) {
        return false;
    }

    int storedState =
        Wh_GetIntValue(LearnedValueName(L"state", identity).c_str(), -1);
    if (storedState != 0 && storedState != 1) {
        return false;
    }
    *state = storedState;
    return true;
}

void StoreLearnedState(const std::wstring& identity, int state) {
    if (state != 0 && state != 1) {
        return;
    }

    int existing = -1;
    if (GetLearnedState(identity, &existing) && existing == state) {
        return;
    }

    std::wstring identityName = LearnedValueName(L"identity", identity);
    std::wstring stateName = LearnedValueName(L"state", identity);
    if (!Wh_SetStringValue(identityName.c_str(), identity.c_str()) ||
        !Wh_SetIntValue(stateName.c_str(), state)) {
        Wh_Log(L"Failed to persist learned tray state for %s",
               identity.c_str());
        return;
    }
    Wh_Log(L"Learned tray state: IsPromoted=%d, identity=%s", state,
           identity.c_str());
}

const Entry* FindNewestMatchingEntry(const EntryMap& entries,
                                     const Entry& target) {
    const Entry* best = nullptr;
    for (const auto& [key, candidate] : entries) {
        if (candidate.normalizedIdentity != target.normalizedIdentity ||
            candidate.promoted < 0) {
            continue;
        }
        if (!best || CompareFileTime(&candidate.lastWrite, &best->lastWrite) > 0) {
            best = &candidate;
        }
    }
    return best;
}

bool SetPromoted(HKEY baseKey, Entry* entry, int newState,
                 const wchar_t* sourceDescription) {
    if (entry->promoted == newState) {
        return true;
    }

    HKEY entryKey = nullptr;
    LSTATUS status = RegOpenKeyExW(baseKey, entry->subkey.c_str(), 0,
                                   KEY_QUERY_VALUE | KEY_SET_VALUE, &entryKey);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Cannot open tray entry for writing: key=%s, error=%ld",
               entry->subkey.c_str(), status);
        return false;
    }

    DWORD value = newState ? 1 : 0;
    Wh_Log(L"Changing IsPromoted: key=%s, path=%s, before=%d, after=%d, "
           L"source=%s",
           entry->subkey.c_str(), entry->executablePath.c_str(),
           entry->promoted, newState, sourceDescription);
    status = RegSetValueExW(entryKey, L"IsPromoted", 0, REG_DWORD,
                            reinterpret_cast<const BYTE*>(&value),
                            sizeof(value));
    RegCloseKey(entryKey);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to change IsPromoted: key=%s, error=%ld",
               entry->subkey.c_str(), status);
        return false;
    }

    entry->promoted = newState;
    return true;
}

void ReconcileInitialStates(HKEY baseKey, EntryMap* entries,
                            StateMap* sessionStates,
                            const Settings& settings) {
    std::unordered_map<std::wstring, const Entry*> newestByIdentity;
    for (const auto& [key, entry] : *entries) {
        if (entry.promoted < 0) {
            continue;
        }
        auto it = newestByIdentity.find(entry.normalizedIdentity);
        if (it == newestByIdentity.end() ||
            (entry.executableExists && !it->second->executableExists) ||
            (entry.executableExists == it->second->executableExists &&
             CompareFileTime(&entry.lastWrite, &it->second->lastWrite) > 0)) {
            newestByIdentity[entry.normalizedIdentity] = &entry;
        }
    }

    for (const auto& [identity, selectedEntry] : newestByIdentity) {
        auto mapIt = entries->find(ToLower(selectedEntry->subkey));
        if (mapIt == entries->end()) {
            continue;
        }

        Entry& entry = mapIt->second;
        int learnedState = -1;
        bool hadLearnedState = GetLearnedState(identity, &learnedState);
        if (settings.preserveVisibility && hadLearnedState) {
            SetPromoted(baseKey, &entry, learnedState,
                        L"previously learned state at startup");
        } else if (settings.learnVisibilityChanges) {
            StoreLearnedState(identity, entry.promoted);
        }
        (*sessionStates)[identity] = entry.promoted;
    }
}

void CleanObsoleteEntries(EntryMap* entries) {
    std::vector<std::wstring> keysToDelete;
    for (const auto& [key, candidate] : *entries) {
        if (!candidate.pathWasVersionNormalized) {
            continue;
        }

        bool obsolete = candidate.packageRegistrationKnown
                            ? !candidate.packageRegistered
                            : !candidate.executableExists;
        if (!obsolete) {
            continue;
        }

        bool hasLiveMatch = false;
        for (const auto& [otherKey, other] : *entries) {
            if (otherKey == key ||
                other.normalizedIdentity != candidate.normalizedIdentity) {
                continue;
            }

            bool otherIsLive = other.packageRegistrationKnown
                                   ? other.packageRegistered
                                   : other.executableExists;
            if (!otherIsLive) {
                continue;
            }

            // Package registration is authoritative: an unregistered MSIX
            // entry is stale even if its protected package directory remains.
            // For ordinary executables retain the conservative timestamp gate.
            if (candidate.packageRegistrationKnown ||
                CompareFileTime(&other.lastWrite, &candidate.lastWrite) > 0) {
                hasLiveMatch = true;
                break;
            }
        }
        if (hasLiveMatch) {
            keysToDelete.push_back(key);
        }
    }

    if (keysToDelete.empty()) {
        return;
    }

    // Request deletion access only when cleanup is enabled and there is
    // actually something eligible. Normal operation stays read/notify-only on
    // the parent key and opens individual entries solely for IsPromoted writes.
    HKEY deleteBaseKey = nullptr;
    LSTATUS openStatus = RegOpenKeyExW(
        HKEY_CURRENT_USER, kNotifyIconSettingsPath, 0,
        KEY_READ | KEY_WRITE | DELETE, &deleteBaseKey);
    if (openStatus != ERROR_SUCCESS) {
        Wh_Log(L"Cannot open tray settings for cleanup: error=%ld", openStatus);
        return;
    }

    for (const std::wstring& mapKey : keysToDelete) {
        auto it = entries->find(mapKey);
        if (it == entries->end()) {
            continue;
        }
        const Entry& entry = it->second;
        Wh_Log(L"Deleting obsolete tray entry: key=%s, path=%s",
               entry.subkey.c_str(), entry.executablePath.c_str());
        LSTATUS status = RegDeleteTreeW(deleteBaseKey, entry.subkey.c_str());
        if (status == ERROR_SUCCESS) {
            entries->erase(it);
        } else {
            Wh_Log(L"Failed to delete obsolete tray entry: key=%s, error=%ld",
                   entry.subkey.c_str(), status);
        }
    }
    RegCloseKey(deleteBaseKey);
}

void ProcessSnapshot(HKEY baseKey, EntryMap* previous,
                     StateMap* sessionStates, bool initialScan) {
    EntryMap current = ReadEntries(baseKey);
    Settings settings = GetSettingsSnapshot();

    if (initialScan) {
        // A previously learned preference is authoritative across restarts.
        // Without one, observe the newest live entry instead of guessing from
        // already-existing duplicates.
        ReconcileInitialStates(baseKey, &current, sessionStates, settings);
    } else {
        for (auto& [key, entry] : current) {
            auto oldIt = previous->find(key);
            bool isNewOrCompleted = oldIt == previous->end();
            bool pathChanged = false;
            if (oldIt != previous->end()) {
                pathChanged = oldIt->second.executablePath != entry.executablePath;
                isNewOrCompleted = oldIt->second.promoted < 0 && entry.promoted >= 0;
            }

            if (entry.promoted < 0) {
                continue;
            }

            if (pathChanged || isNewOrCompleted) {
                int inheritedState = -1;
                const wchar_t* sourceDescription = L"none";

                if (settings.preserveVisibility) {
                    const Entry* source = FindNewestMatchingEntry(*previous, entry);
                    if (source) {
                        inheritedState = source->promoted;
                        sourceDescription = L"previous registry entry";
                    } else if (auto stateIt = sessionStates->find(
                                   entry.normalizedIdentity);
                               stateIt != sessionStates->end()) {
                        inheritedState = stateIt->second;
                        sourceDescription = L"session state";
                    } else if (GetLearnedState(entry.normalizedIdentity,
                                               &inheritedState)) {
                        sourceDescription = L"learned state";
                    }
                }

                if (inheritedState >= 0) {
                    SetPromoted(baseKey, &entry, inheritedState,
                                sourceDescription);
                }
                if (settings.learnVisibilityChanges) {
                    StoreLearnedState(entry.normalizedIdentity, entry.promoted);
                    (*sessionStates)[entry.normalizedIdentity] = entry.promoted;
                } else if (!sessionStates->contains(entry.normalizedIdentity)) {
                    (*sessionStates)[entry.normalizedIdentity] = entry.promoted;
                }
                continue;
            }

            // An existing entry changed without its executable path changing:
            // treat this as a user/system visibility change and learn it.
            if (settings.learnVisibilityChanges && oldIt != previous->end() &&
                oldIt->second.promoted >= 0 &&
                oldIt->second.promoted != entry.promoted) {
                StoreLearnedState(entry.normalizedIdentity, entry.promoted);
                (*sessionStates)[entry.normalizedIdentity] = entry.promoted;
            }
        }
    }

    if (settings.cleanObsoleteEntries) {
        CleanObsoleteEntries(&current);
    }
    *previous = std::move(current);
}

DWORD WINAPI RegistryWatcherThread(void*) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool uninitializeCom = SUCCEEDED(comResult);
    g_knownFolderResolutionAvailable =
        SUCCEEDED(comResult) || comResult == RPC_E_CHANGED_MODE;
    if (!g_knownFolderResolutionAvailable) {
        Wh_Log(L"Known Folder path resolution unavailable: error=0x%08lX",
               static_cast<unsigned long>(comResult));
    }

    HKEY baseKey = nullptr;
    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER, kNotifyIconSettingsPath, 0,
        KEY_READ | KEY_NOTIFY, &baseKey);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Cannot open HKCU\\%s: error=%ld", kNotifyIconSettingsPath,
               status);
        if (uninitializeCom) {
            CoUninitialize();
        }
        return 1;
    }

    HANDLE notifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!notifyEvent) {
        Wh_Log(L"CreateEvent for registry notification failed: error=%lu",
               GetLastError());
        RegCloseKey(baseKey);
        if (uninitializeCom) {
            CoUninitialize();
        }
        return 1;
    }

    auto armNotification = [&]() -> bool {
        LSTATUS notifyStatus = RegNotifyChangeKeyValue(
            baseKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
            notifyEvent, TRUE);
        if (notifyStatus != ERROR_SUCCESS) {
            Wh_Log(L"RegNotifyChangeKeyValue failed: error=%ld", notifyStatus);
            return false;
        }
        return true;
    };

    EntryMap previous;
    StateMap sessionStates;
    if (!armNotification()) {
        CloseHandle(notifyEvent);
        RegCloseKey(baseKey);
        if (uninitializeCom) {
            CoUninitialize();
        }
        return 1;
    }
    ProcessSnapshot(baseKey, &previous, &sessionStates, true);

    HANDLE waitHandles[] = {g_stopEvent, g_settingsChangedEvent, notifyEvent};
    while (true) {
        DWORD wait = WaitForMultipleObjects(ARRAYSIZE(waitHandles), waitHandles,
                                            FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
        if (wait == WAIT_OBJECT_0 + 1) {
            ProcessSnapshot(baseKey, &previous, &sessionStates, false);
            continue;
        }
        if (wait == WAIT_OBJECT_0 + 2) {
            // Rearm before scanning so writes occurring during the scan are not
            // lost. Our own writes simply schedule one harmless follow-up scan.
            if (!armNotification()) {
                break;
            }
            ProcessSnapshot(baseKey, &previous, &sessionStates, false);
            continue;
        }

        Wh_Log(L"Registry watcher wait failed: result=%lu, error=%lu", wait,
               GetLastError());
        break;
    }

    CloseHandle(notifyEvent);
    RegCloseKey(baseKey);
    g_knownFolderResolutionAvailable = false;
    if (uninitializeCom) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_settingsChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_settingsChangedEvent) {
        Wh_Log(L"Failed to create worker events: error=%lu", GetLastError());
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_settingsChangedEvent) {
            CloseHandle(g_settingsChangedEvent);
            g_settingsChangedEvent = nullptr;
        }
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, RegistryWatcherThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create registry watcher: error=%lu", GetLastError());
        CloseHandle(g_settingsChangedEvent);
        CloseHandle(g_stopEvent);
        g_settingsChangedEvent = nullptr;
        g_stopEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Persistent Tray Icon Visibility started");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
}

void WhTool_ModUninit() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_settingsChangedEvent) {
        CloseHandle(g_settingsChangedEvent);
        g_settingsChangedEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    Wh_Log(L"Persistent Tray Icon Visibility stopped");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[
        MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}

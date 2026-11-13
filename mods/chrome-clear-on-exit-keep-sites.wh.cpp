// ==WindhawkMod==
// @id              chrome-clear-on-exit-keep-sites
// @name            Chrome clear on exit with keep sites
// @description     Clears Chrome browsing data on exit while keeping selected website logins.
// @version         0.4.1
// @author          haniobaid
// @github          https://github.com/haniobaid
// @include         chrome.exe
// @compilerOptions -ladvapi32 -lshell32 -lole32 -luserenv
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome clear on exit with keep sites

Clear Chrome's browsing data when its last window closes normally, while keeping
login cookies and ordinary site storage for domains you choose. Chrome performs
the deletion. The mod supplies its settings through Chrome's native policy reader.

**Cleanup is off by default.** Review the keep list, then turn on **Enable automatic
cleanup** in the mod's Settings tab. The list starts empty: enabling cleanup with
no exceptions signs you out of most websites. Deleted data cannot be recovered.
Enter domains such as `example.com`; subdomains are included. Full HTTP/HTTPS URLs
are accepted, with their paths and ports ignored. Use ASCII/punycode domain names.

## Check your configuration

Restart Chrome after installing the mod. At `chrome://policy`, select **Reload
policies** and check these three entries:

- `ClearBrowsingDataOnExitList`: browsing history, download history, cached images
  and files, autofill, site settings, and hosted-app data, for all time.
- `DefaultCookiesSetting`: `4`, making ordinary site storage session-only.
- `CookiesAllowedForUrls`: your keep list, preserving cookies and ordinary storage.

The keep list does not preserve history, cache, permissions, or legacy hosted-app
storage. Downloaded files, bookmarks, and saved passwords are not selected for
deletion. Websites can still expire their own sessions or require a new login.

Chrome already exposes the cookie/storage controls at
`chrome://settings/content/siteData`. This mod sets those controls and their
**Allowed to save data on your device** exceptions from one Windhawk keep list,
and adds the broader exit cleanup policy which has no equivalent Chrome setting.
Another clearing extension or an existing Chrome cleanup rule can still remove
the data you intended to keep.

## Changes and removal

After changing settings, use **Reload policies** to verify the result. The mod
also requests a refresh when it has observed Chrome registering its policy events.
Chrome processes these requests asynchronously; the mod cannot acknowledge when
Chrome has finished applying them.

To stop cleanup, turn the setting off or disable the mod, then **Reload policies
before closing Chrome** and confirm that the supplied entries have disappeared.
Chrome can otherwise retain its cached policies for the remainder of that session.
The next Chrome start with the mod disabled receives no policies from this mod.
No administrator prompt, PowerShell helper, or policy-removal permission is needed.

The mod runs only in Chrome's browser process and does not change its real policy
registry keys or profile files. It uses private temporary storage within
Windhawk's mod storage while cleanup is enabled. Inactive files can remain after
Chrome exits; a later enabled start attempts to remove verified leftovers when
they are no longer in use. These files are never reused as configuration or
needed for backup.

## Scope and limitations

- Applies to all profiles in each Chrome browser process that loads the mod.
  Chrome must be allowed to load Windhawk mods.
- Intended for personal installations. Conflicting existing local Chrome policies
  prevent injection; they are never replaced. Chrome's normal cloud-policy
  precedence still applies. Check Windhawk's mod log if the entries do not appear.
- If a policy view cannot be prepared, Chrome uses its genuine policies for that
  load. The mod logs the reason; check the policy page to confirm cleanup is active.
  If private storage could not be initialized, save the mod's settings again to
  retry. Once storage is ready, reloading Chrome policies retries a failed read.
- Chrome can show **Managed by your organization** while these policies are active.
  Chrome also disables synchronization of affected data categories.
- Normal last-window close is the supported cleanup path. Crashes, forced
  termination, Windows shutdown, and some restart/quit paths can
  prevent completion or defer selective site-data deletion until the next start.
- Disabling the mod cannot restore browsing data Chrome has already deleted.

Back up the source and your Windhawk settings to reproduce the configuration on a
fresh Windows installation. The source's defaults do not contain your saved list.

## References

- [Chrome exit cleanup policy](https://chromeenterprise.google/policies/clear-browsing-data-on-exit-list/)
- [Chromium Windows policy loader](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/components/policy/core/common/policy_loader_win.cc)
- [Chromium registry policy conversion](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/components/policy/core/common/registry_dict.cc)
- [Windows application hives](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regloadappkeyw)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: false
  $name: Enable automatic cleanup
  $description: Deletes browsing data on normal Chrome exit. Review the keep list first; an empty list preserves no website logins. Reload Chrome policies after changing this setting.
- keepSites:
  - ""
  $name: Keep signed in on these sites
  $description: Domains such as example.com, including subdomains. Full HTTP/HTTPS URLs are accepted. Blank rows are ignored. Maximum 256 rows; an empty list keeps no sites.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <aclapi.h>
#include <shellapi.h>
#include <sddl.h>
#include <objbase.h>
#include <userenv.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
// BEGIN_KEEP_SITE_SETTINGS
struct SettingsError {
    std::wstring message;
};

[[noreturn]] void SettingsFail(const wchar_t* message) {
    throw SettingsError{message};
}

std::wstring TrimSite(std::wstring value) {
    const auto first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return {};
    }
    return value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
}

std::wstring NormalizeSite(std::wstring input) {
    input = TrimSite(std::move(input));
    for (auto& c : input) {
        if (c >= L'A' && c <= L'Z') {
            c += L'a' - L'A';
        }
    }
    if (input.starts_with(L"[*.]")) {
        input.erase(0, 4);
    }
    if (input.starts_with(L"http://") || input.starts_with(L"https://")) {
        input.erase(0, input.starts_with(L"https://") ? 8 : 7);
        input = input.substr(0, input.find_first_of(L"/?#"));
        if (input.find(L'@') != std::wstring::npos) {
            SettingsFail(L"Keep-site URLs must not contain credentials.");
        }
        const auto colon = input.find(L':');
        if (colon != std::wstring::npos) {
            const auto port = input.substr(colon + 1);
            if (port.empty() || port.size() > 5 ||
                port.find_first_not_of(L"0123456789") != std::wstring::npos) {
                SettingsFail(L"Invalid port in a keep-site URL.");
            }
            unsigned number = 0;
            for (const auto c : port) {
                number = number * 10 + static_cast<unsigned>(c - L'0');
            }
            if (!number || number > 65535) {
                SettingsFail(L"Invalid port in a keep-site URL.");
            }
            input.resize(colon);
        }
    }
    if (!input.empty() && input.back() == L'.') {
        input.pop_back();
    }
    if (input.empty() || input.size() > 253 ||
        input.find(L'.') == std::wstring::npos) {
        SettingsFail(L"Use a complete domain such as example.com.");
    }
    size_t start = 0;
    while (start < input.size()) {
        auto end = input.find(L'.', start);
        if (end == std::wstring::npos) {
            end = input.size();
        }
        if (end == start || end - start > 63 || input[start] == L'-' ||
            input[end - 1] == L'-') {
            SettingsFail(L"Invalid domain in the keep list.");
        }
        for (size_t i = start; i < end; ++i) {
            const auto c = input[i];
            if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') ||
                  c == L'-')) {
                SettingsFail(L"Use ASCII/punycode domains or HTTP/HTTPS URLs.");
            }
        }
        start = end + 1;
    }
    const auto tld = input.substr(input.rfind(L'.') + 1);
    if (tld.size() < 2 ||
        tld.find_first_of(L"abcdefghijklmnopqrstuvwxyz") == std::wstring::npos) {
        SettingsFail(L"Use a domain name, not an IP address.");
    }
    return input;
}

std::vector<std::wstring> ReadKeepSites() {
    constexpr int maxRows = 256;
    std::vector<std::wstring> sites;
    for (int i = 0; i <= maxRows; ++i) {
        WindhawkUtils::StringSetting entry(
            Wh_GetStringSetting(L"keepSites[%d]", i));
        auto text = TrimSite(entry.get());
        if (text.empty()) {
            continue;
        }
        if (i == maxRows) {
            SettingsFail(L"The keep list is limited to 256 rows.");
        }
        auto site = NormalizeSite(std::move(text));
        if (std::find(sites.begin(), sites.end(), site) == sites.end()) {
            sites.push_back(std::move(site));
        }
    }
    return sites;
}
// END_KEEP_SITE_SETTINGS
// BEGIN_TRANSIENT_POLICY_OVERLAY
// A private application hive supplies genuine native registry handles. Only
// volatile keys contain copied policy data; HKCU/HKLM policy keys are read-only.
// https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regloadappkeyw
// https://learn.microsoft.com/windows/win32/api/winreg/nf-winreg-regcreatekeyexw

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
using RegCloseKey_t = decltype(&RegCloseKey);
RegOpenKeyExW_t RegOpenKeyExW_Original = nullptr;
RegCloseKey_t RegCloseKey_Original = nullptr;

constexpr wchar_t kOverlayChromePath[] = L"Software\\Policies\\Google\\Chrome";
constexpr const wchar_t* kOverlayOwned[] = {
    L"DefaultCookiesSetting", L"ClearBrowsingDataOnExitList", L"CookiesAllowedForUrls"};
constexpr wchar_t kOverlayClearJson[] =
    LR"(["browsing_history","download_history","cached_images_and_files","autofill","site_settings","hosted_app_data"])";
constexpr size_t kOverlayMaxBytes = 4 * 1024 * 1024;
constexpr size_t kOverlayMaxItems = 4096;
constexpr unsigned kOverlayMaxDepth = 32;

thread_local unsigned g_overlayBypass = 0;
SRWLOCK g_overlayLock = SRWLOCK_INIT;
std::atomic<size_t> g_overlayTrackedCount{0};
std::atomic<bool> g_overlayEnabled{false};
std::atomic<unsigned long long> g_overlayRequestVersion{0};
std::atomic<bool> g_overlayHooksRemoved{false};
struct OverlayBypass {
    OverlayBypass() { ++g_overlayBypass; }
    ~OverlayBypass() { --g_overlayBypass; }
};
// Metadata only: never call Windows, log, allocate, or release an owning
// reference while either bookkeeping lock is held.
struct OverlayLock {
    OverlayLock() { AcquireSRWLockExclusive(&g_overlayLock); }
    ~OverlayLock() { ReleaseSRWLockExclusive(&g_overlayLock); }
};
struct OverlayReadLock {
    OverlayReadLock() { AcquireSRWLockShared(&g_overlayLock); }
    ~OverlayReadLock() { ReleaseSRWLockShared(&g_overlayLock); }
};
LSTATUS OverlayNativeOpen(HKEY key, LPCWSTR name, DWORD options, REGSAM access, PHKEY result) {
    return (g_overlayHooksRemoved.load(std::memory_order_acquire) || !RegOpenKeyExW_Original
                ? RegOpenKeyExW : RegOpenKeyExW_Original)(
        key, name, options, access, result);
}
LSTATUS OverlayNativeClose(HKEY key) {
    return (g_overlayHooksRemoved.load(std::memory_order_acquire) || !RegCloseKey_Original
                ? RegCloseKey : RegCloseKey_Original)(key);
}
struct OverlayKey {
    HKEY value = nullptr;
    OverlayKey() = default;
    OverlayKey(const OverlayKey&) = delete;
    ~OverlayKey() { if (value) OverlayNativeClose(value); }
    HKEY release() { HKEY result = value; value = nullptr; return result; }
};
struct OverlayHandle {
    HANDLE value = nullptr;
    OverlayHandle() = default;
    OverlayHandle(const OverlayHandle&) = delete;
    ~OverlayHandle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
};
struct OverlayLocalMemory {
    void* value = nullptr;
    ~OverlayLocalMemory() { if (value) LocalFree(value); }
};
struct OverlayError { LSTATUS code; const wchar_t* action; };
void OverlayCheck(LSTATUS result, const wchar_t* action) {
    if (result != ERROR_SUCCESS) throw OverlayError{result, action};
}
bool OverlayMissing(LSTATUS result) {
    return result == ERROR_FILE_NOT_FOUND || result == ERROR_PATH_NOT_FOUND;
}

struct OverlayHive {
    HKEY root = nullptr;
    std::atomic<unsigned long long> nextSnapshot{0};
    HANDLE directoryHandle = INVALID_HANDLE_VALUE;
    std::wstring directory;
    std::wstring file;
    std::wstring log1;
    std::wstring log2;
    ~OverlayHive() {
        OverlayBypass bypass;
        if (root) OverlayNativeClose(root);
        if (!file.empty()) {
            // A caller-owned key can keep the hive open after mod unload.
            // In that case leave the empty backing file in Windhawk storage.
            DeleteFileW(file.c_str());
            if (!log1.empty()) DeleteFileW(log1.c_str());
            if (!log2.empty()) DeleteFileW(log2.c_str());
        }
        if (directoryHandle != INVALID_HANDLE_VALUE) CloseHandle(directoryHandle);
        if (!directory.empty()) RemoveDirectoryW(directory.c_str());
    }
};
struct OverlaySnapshot {
    std::shared_ptr<OverlayHive> hive;
    HKEY root = nullptr;
    std::wstring name;
    std::atomic<bool> preserveNativeKeys{false};
    ~OverlaySnapshot() {
        OverlayBypass bypass;
        // After hooks disappear, Chrome still owns any returned HKEY handles.
        // Keep their native keys intact; closing our references is sufficient.
        if (!preserveNativeKeys.load(std::memory_order_acquire) && hive && !name.empty())
            RegDeleteTreeW(hive->root, name.c_str());
        if (root) OverlayNativeClose(root);
    }
};
struct OverlayConfig {
    std::shared_ptr<OverlayHive> hive;
    std::wstring keepJson;
};
using OverlayClients = std::map<HKEY, std::shared_ptr<OverlaySnapshot>>;
struct OverlayState {
    std::shared_ptr<const OverlayConfig> config;
    OverlayClients clients;
    const wchar_t* lastErrorAction = nullptr;
    LSTATUS lastErrorCode = ERROR_SUCCESS;
};
// Deliberately not a global owning object: process termination needs no cleanup
// callback, policy restoration, locks, or calls through trampoline pointers.
OverlayState* g_overlayState = nullptr;

void OverlayLogError(const wchar_t* action, LSTATUS code) noexcept {
    bool report = true;
    {
        OverlayLock lock;
        if (g_overlayState) {
            report = g_overlayState->lastErrorAction != action ||
                     g_overlayState->lastErrorCode != code;
            g_overlayState->lastErrorAction = action;
            g_overlayState->lastErrorCode = code;
        }
    }
    if (report)
        Wh_Log(L"Chrome policy overlay: %s (Windows error %ld). No real Chrome policy was changed.",
               action, static_cast<long>(code));
}

std::shared_ptr<const OverlayConfig> OverlayCaptureConfig() {
    std::shared_ptr<const OverlayConfig> result;
    if (g_overlayEnabled.load(std::memory_order_acquire)) {
        OverlayReadLock lock;
        if (g_overlayState) result = g_overlayState->config;
    }
    return result;
}
std::shared_ptr<OverlaySnapshot> OverlayCaptureParent(HKEY key) {
    std::shared_ptr<OverlaySnapshot> result;
    if (g_overlayTrackedCount.load(std::memory_order_acquire)) {
        OverlayReadLock lock;
        if (g_overlayState) {
            auto found = g_overlayState->clients.find(key);
            if (found != g_overlayState->clients.end()) result = found->second;
        }
    }
    return result;
}

// Pin stale objects before examining them. Only our exact same-user names and
// files qualify, and a living/unknown creator is skipped, including the small
// interval between its CreateDirectory and acquisition of the directory pin.
bool OverlayCleanupPid(const wchar_t* name, const std::wstring& prefix, DWORD& pid) {
    std::wstring_view text(name);
    if (!text.starts_with(prefix)) return false;
    text.remove_prefix(prefix.size());
    const size_t dash = text.find(L'-');
    if (dash == 0 || dash == std::wstring_view::npos || dash > 10) return false;
    unsigned long long value = 0;
    for (wchar_t c : text.substr(0, dash)) {
        if (c < L'0' || c > L'9') return false;
        value = value * 10 + c - L'0';
        if (value > MAXDWORD) return false;
    }
    if (!value) return false;
    text.remove_prefix(dash + 1);
    if (text.size() != 38 || text.front() != L'{' || text.back() != L'}') return false;
    for (size_t i = 1; i < 37; ++i) {
        wchar_t c = text[i];
        if (i == 9 || i == 14 || i == 19 || i == 24) {
            if (c != L'-') return false;
        } else if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') ||
                     (c >= L'A' && c <= L'F'))) return false;
    }
    pid = static_cast<DWORD>(value);
    return true;
}
bool OverlayCreatorExited(DWORD pid) {
    if (pid == GetCurrentProcessId()) return false;
    OverlayHandle process;
    process.value = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!process.value) return GetLastError() == ERROR_INVALID_PARAMETER;
    // Opportunistic cleanup only. Never wait, elevate, or block configuration.
    return WaitForSingleObject(process.value, 0) == WAIT_OBJECT_0;
}
bool OverlayOwnedObject(HANDLE handle, PSID owner, bool directory) {
    BY_HANDLE_FILE_INFORMATION info{};
    if (!GetFileInformationByHandle(handle, &info) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
        !!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != directory) return false;
    PSID actualOwner = nullptr;
    OverlayLocalMemory security;
    if (GetSecurityInfo(handle, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
                        &actualOwner, nullptr, nullptr, nullptr,
                        reinterpret_cast<PSECURITY_DESCRIPTOR*>(&security.value)) != ERROR_SUCCESS)
        return false;
    return actualOwner && EqualSid(owner, actualOwner);
}
struct OverlayFindHandle {
    HANDLE value = INVALID_HANDLE_VALUE;
    ~OverlayFindHandle() { if (value != INVALID_HANDLE_VALUE) FindClose(value); }
};
bool OverlayPruneDirectory(const std::wstring& directory, PSID owner) {
    OverlayHandle pin;
    pin.value = CreateFileW(directory.c_str(), DELETE | READ_CONTROL | FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (pin.value == INVALID_HANDLE_VALUE || !OverlayOwnedObject(pin.value, owner, true)) return false;
    constexpr const wchar_t* names[] = {L"view.dat", L"view.dat.LOG1", L"view.dat.LOG2"};
    bool present[3]{};
    WIN32_FIND_DATAW entry{};
    OverlayFindHandle search;
    const std::wstring pattern = directory + L"\\*";
    search.value = FindFirstFileW(pattern.c_str(), &entry);
    if (search.value == INVALID_HANDLE_VALUE) return false;
    do {
        if (!wcscmp(entry.cFileName, L".") || !wcscmp(entry.cFileName, L"..")) continue;
        if (entry.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) return false;
        unsigned index = 0;
        while (index < 3 && _wcsicmp(entry.cFileName, names[index])) ++index;
        if (index == 3 || present[index]) return false;
        present[index] = true;
    } while (FindNextFileW(search.value, &entry));
    if (GetLastError() != ERROR_NO_MORE_FILES) return false;
    FindClose(search.value);
    search.value = INVALID_HANDLE_VALUE;
    OverlayHandle files[3];
    for (unsigned i = 0; i < 3; ++i) {
        if (!present[i]) continue;
        const std::wstring path = directory + L"\\" + names[i];
        files[i].value = CreateFileW(path.c_str(), DELETE | READ_CONTROL | FILE_READ_ATTRIBUTES,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                     FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
        if (files[i].value == INVALID_HANDLE_VALUE || !OverlayOwnedObject(files[i].value, owner, false))
            return false;
    }
    FILE_DISPOSITION_INFO dispose{TRUE};
    for (unsigned i = 0; i < 3; ++i) {
        if (present[i] && !SetFileInformationByHandle(files[i].value, FileDispositionInfo,
                                                     &dispose, sizeof(dispose))) return false;
    }
    for (auto& file : files) {
        if (file.value && file.value != INVALID_HANDLE_VALUE) CloseHandle(file.value);
        file.value = nullptr;
    }
    // The native API refuses a non-empty directory. No recursive deletion.
    return SetFileInformationByHandle(pin.value, FileDispositionInfo, &dispose, sizeof(dispose));
}
void OverlayCleanupOldHives(const std::wstring& base, PSID owner, const wchar_t* sid) noexcept {
    try {
        const std::wstring prefix = L"overlay-" + std::wstring(sid) + L"-";
        const std::wstring pattern = base + L"\\" + prefix + L"*";
        WIN32_FIND_DATAW entry{};
        OverlayFindHandle search;
        search.value = FindFirstFileW(pattern.c_str(), &entry);
        if (search.value == INVALID_HANDLE_VALUE) return;
        unsigned inspected = 0, removed = 0;
        do {
            if (++inspected > 64) break;
            DWORD pid = 0;
            if (!(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                (entry.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
                !OverlayCleanupPid(entry.cFileName, prefix, pid) || !OverlayCreatorExited(pid)) continue;
            if (OverlayPruneDirectory(base + L"\\" + entry.cFileName, owner)) ++removed;
        } while (FindNextFileW(search.value, &entry));
        if (removed) Wh_Log(L"Removed %u inactive private policy-hive directories from Windhawk storage.", removed);
    } catch (...) {
        // Unknown, busy, or unverifiable files never prevent cleanup settings.
        Wh_Log(L"Skipped private-hive cache cleanup; configuration can continue.");
    }
}

std::shared_ptr<OverlayHive> OverlayCreateHive() {
    std::vector<wchar_t> storage(32768);
    size_t length = Wh_GetModStoragePath(storage.data(), storage.size());
    if (!length || length >= storage.size())
        throw OverlayError{ERROR_PATH_NOT_FOUND, L"Windhawk storage is unavailable"};
    std::wstring base(storage.data(), length);
    while (!base.empty() && (base.back() == L'\\' || base.back() == L'/')) base.pop_back();
    if (base.empty()) throw OverlayError{ERROR_BAD_PATHNAME, L"Windhawk storage path is invalid"};
    if (!CreateDirectoryW(base.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot create the Windhawk storage directory"};

    OverlayHandle token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token.value))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot identify the Windows user"};
    DWORD tokenBytes = 0;
    GetTokenInformation(token.value, TokenUser, nullptr, 0, &tokenBytes);
    if (!tokenBytes || tokenBytes > 65536)
        throw OverlayError{ERROR_INVALID_DATA, L"The Windows user token is invalid"};
    std::vector<BYTE> tokenData(tokenBytes);
    if (!GetTokenInformation(token.value, TokenUser, tokenData.data(), tokenBytes, &tokenBytes))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot read the Windows user token"};
    PSID userSid = reinterpret_cast<TOKEN_USER*>(tokenData.data())->User.Sid;
    OverlayLocalMemory sidText;
    if (!ConvertSidToStringSidW(userSid, reinterpret_cast<LPWSTR*>(&sidText.value)))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot format the Windows user identifier"};
    const wchar_t* sid = static_cast<const wchar_t*>(sidText.value);
    OverlayCleanupOldHives(base, userSid, sid);
    GUID guid;
    if (FAILED(CoCreateGuid(&guid)))
        throw OverlayError{ERROR_GEN_FAILURE, L"Cannot generate a private storage identifier"};
    wchar_t guidText[40];
    if (!StringFromGUID2(guid, guidText, ARRAYSIZE(guidText)))
        throw OverlayError{ERROR_GEN_FAILURE, L"Cannot format the private storage identifier"};

    std::wstring directory = base + L"\\overlay-" + sid + L"-" +
                             std::to_wstring(GetCurrentProcessId()) + L"-" + guidText;
    std::wstring descriptor = L"O:" + std::wstring(sid) +
        L"D:P(A;OICI;FA;;;SY)(A;OICI;FA;;;" + sid + L")";
    OverlayLocalMemory security;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            descriptor.c_str(), SDDL_REVISION_1,
            reinterpret_cast<PSECURITY_DESCRIPTOR*>(&security.value), nullptr))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot secure the private storage directory"};
    SECURITY_ATTRIBUTES attributes{sizeof(attributes), security.value, FALSE};
    auto hive = std::make_shared<OverlayHive>();
    // Never reuse a pre-existing directory or hive file.
    if (!CreateDirectoryW(directory.c_str(), &attributes))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot create a new private storage directory"};
    hive->directory = std::move(directory);
    hive->directoryHandle = CreateFileW(
        hive->directory.c_str(), FILE_READ_ATTRIBUTES | READ_CONTROL,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (hive->directoryHandle == INVALID_HANDLE_VALUE)
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot protect the private storage directory"};
    BY_HANDLE_FILE_INFORMATION info{};
    if (!GetFileInformationByHandle(hive->directoryHandle, &info))
        throw OverlayError{static_cast<LSTATUS>(GetLastError()), L"Cannot inspect the private storage directory"};
    if (!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
        throw OverlayError{ERROR_ACCESS_DENIED, L"The private storage directory was replaced"};
    PSID actualOwner = nullptr;
    OverlayLocalMemory actualSecurity;
    OverlayCheck(GetSecurityInfo(hive->directoryHandle, SE_FILE_OBJECT,
                                 OWNER_SECURITY_INFORMATION, &actualOwner, nullptr,
                                 nullptr, nullptr,
                                 reinterpret_cast<PSECURITY_DESCRIPTOR*>(&actualSecurity.value)),
                 L"Cannot verify the private storage directory owner");
    if (!actualOwner || !EqualSid(userSid, actualOwner))
        throw OverlayError{ERROR_ACCESS_DENIED, L"The private storage directory owner does not match"};
    hive->file = hive->directory + L"\\view.dat";
    hive->log1 = hive->file + L".LOG1";
    hive->log2 = hive->file + L".LOG2";
    if (GetFileAttributesW(hive->file.c_str()) != INVALID_FILE_ATTRIBUTES)
        throw OverlayError{ERROR_ALREADY_EXISTS, L"A private hive file unexpectedly already exists"};
    DWORD missing = GetLastError();
    if (missing != ERROR_FILE_NOT_FOUND)
        throw OverlayError{static_cast<LSTATUS>(missing), L"Cannot verify the private hive file path"};
    OverlayCheck(RegLoadAppKeyW(hive->file.c_str(), &hive->root, KEY_ALL_ACCESS,
                               REG_PROCESS_APPKEY, 0),
                 L"Cannot create the private application hive");
    return hive;
}

bool InitializeOverlayStorage() {
    const auto lifecycle = g_overlayRequestVersion.load(std::memory_order_acquire);
    try {
        {
            OverlayReadLock lock;
            if (g_overlayHooksRemoved.load(std::memory_order_acquire)) return false;
            if (g_overlayState) return true;
        }
        auto candidate = std::make_unique<OverlayState>();
        bool initialized = false;
        {
            OverlayLock lock;
            if (!g_overlayHooksRemoved.load(std::memory_order_acquire) &&
                g_overlayRequestVersion.load(std::memory_order_acquire) == lifecycle) {
                if (!g_overlayState) {
                    g_overlayState = candidate.release();
                }
                initialized = true;
            }
        }
        // File/hive preparation happens only in enabled SetOverlaySites.
        return initialized;
    } catch (...) {
        Wh_Log(L"Chrome policy overlay: cannot allocate bookkeeping state.");
        return false;
    }
}

void OverlayRequireAbsent(HKEY key, const wchar_t* name, REGSAM view) {
    DWORD bytes = 0;
    LSTATUS result = RegQueryValueExW(key, name, nullptr, nullptr, nullptr, &bytes);
    if (result == ERROR_SUCCESS || result == ERROR_MORE_DATA)
        throw OverlayError{ERROR_ALREADY_EXISTS, name};
    if (!OverlayMissing(result)) OverlayCheck(result, L"Cannot check an existing Chrome policy value");
    OverlayKey child;
    result = OverlayNativeOpen(key, name, 0, KEY_READ | view, &child.value);
    if (result == ERROR_SUCCESS) throw OverlayError{ERROR_ALREADY_EXISTS, name};
    if (!OverlayMissing(result)) OverlayCheck(result, L"Cannot check an existing Chrome policy subkey");
}
void OverlayCheckPolicyKey(HKEY key, REGSAM view) {
    for (const wchar_t* name : kOverlayOwned) OverlayRequireAbsent(key, name, view);
    for (const wchar_t* name : {L"CookiesBlockedForUrls", L"CookiesSessionOnlyForUrls", L"BrowsingDataLifetime"})
        OverlayRequireAbsent(key, name, view);
    DWORD value = 0, bytes = sizeof(value), type = 0;
    LSTATUS result = RegQueryValueExW(key, L"AllowDeletingBrowserHistory", nullptr, &type,
                                    reinterpret_cast<BYTE*>(&value), &bytes);
    if (result == ERROR_SUCCESS && (type != REG_DWORD || bytes != sizeof(value) || value == 0))
        throw OverlayError{ERROR_ACCESS_DENIED, L"AllowDeletingBrowserHistory prevents the overlay"};
    if (result != ERROR_SUCCESS && !OverlayMissing(result))
        OverlayCheck(result, L"Cannot check AllowDeletingBrowserHistory");
    OverlayKey child;
    result = OverlayNativeOpen(key, L"AllowDeletingBrowserHistory", 0, KEY_READ | view, &child.value);
    if (result == ERROR_SUCCESS)
        throw OverlayError{ERROR_ALREADY_EXISTS, L"An unexpected AllowDeletingBrowserHistory subkey exists"};
    if (!OverlayMissing(result)) OverlayCheck(result, L"Cannot check the history policy subkey");
}
void OverlayPreflight() {
    for (HKEY hive : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        for (REGSAM view : {KEY_WOW64_64KEY, KEY_WOW64_32KEY}) {
            OverlayKey key;
            LSTATUS result = OverlayNativeOpen(hive, kOverlayChromePath, 0, KEY_READ | view, &key.value);
            if (OverlayMissing(result)) continue;
            OverlayCheck(result, L"Cannot inspect genuine Chrome policy configuration");
            OverlayCheckPolicyKey(key.value, view);
        }
    }
}

struct OverlayCopyBudget { size_t bytes = 0; size_t items = 0; };
void OverlayCharge(OverlayCopyBudget& budget, size_t bytes) {
    if (++budget.items > kOverlayMaxItems || bytes > kOverlayMaxBytes - budget.bytes)
        throw OverlayError{ERROR_FILE_TOO_LARGE, L"The genuine Chrome policy tree exceeds the overlay copy limit"};
    budget.bytes += bytes;
}
void OverlayCopyTree(HKEY source, HKEY target, REGSAM view, unsigned depth, OverlayCopyBudget& budget) {
    if (depth > kOverlayMaxDepth)
        throw OverlayError{ERROR_FILE_TOO_LARGE, L"The genuine Chrome policy tree exceeds the overlay depth limit"};
    DWORD subkeys = 0, maxKey = 0, values = 0, maxName = 0, maxData = 0;
    FILETIME before{};
    OverlayCheck(RegQueryInfoKeyW(source, nullptr, nullptr, nullptr, &subkeys, &maxKey,
                                 nullptr, &values, &maxName, &maxData, nullptr, &before),
                 L"Cannot inspect genuine Chrome policy data");
    if (subkeys > kOverlayMaxItems || values > kOverlayMaxItems || maxKey > 32767 ||
        maxName > 32767 || maxData > kOverlayMaxBytes)
        throw OverlayError{ERROR_FILE_TOO_LARGE, L"The genuine Chrome policy tree is too large to copy safely"};
    std::vector<wchar_t> name(std::max(maxKey, maxName) + 2);
    std::vector<BYTE> data(std::max<DWORD>(maxData, 1));
    for (DWORD index = 0; index < values; ++index) {
        DWORD nameChars = static_cast<DWORD>(name.size()), dataBytes = static_cast<DWORD>(data.size()), type = 0;
        OverlayCheck(RegEnumValueW(source, index, name.data(), &nameChars, nullptr,
                                  &type, data.data(), &dataBytes),
                     L"Genuine Chrome policy values changed or could not be copied");
        OverlayCharge(budget, (nameChars + 1) * sizeof(wchar_t) + dataBytes);
        OverlayCheck(RegSetValueExW(target, name.data(), 0, type, data.data(), dataBytes),
                     L"Cannot populate a private policy snapshot");
    }
    for (DWORD index = 0; index < subkeys; ++index) {
        DWORD nameChars = static_cast<DWORD>(name.size());
        OverlayCheck(RegEnumKeyExW(source, index, name.data(), &nameChars, nullptr, nullptr, nullptr, nullptr),
                     L"Genuine Chrome policy subkeys changed or could not be copied");
        OverlayCharge(budget, (nameChars + 1) * sizeof(wchar_t));
        OverlayKey from, to;
        OverlayCheck(OverlayNativeOpen(source, name.data(), 0, KEY_READ | view, &from.value),
                     L"Cannot read a genuine Chrome policy subkey");
        // Do not use RegCopyTree: it copies security descriptors. Every private
        // app-hive key must instead inherit the same descriptor and be volatile.
        OverlayCheck(RegCreateKeyExW(target, name.data(), 0, nullptr, REG_OPTION_VOLATILE,
                                    KEY_ALL_ACCESS, nullptr, &to.value, nullptr),
                     L"Cannot create a volatile private policy subkey");
        OverlayCopyTree(from.value, to.value, view, depth + 1, budget);
    }
    DWORD finalSubkeys = 0, finalValues = 0;
    FILETIME after{};
    OverlayCheck(RegQueryInfoKeyW(source, nullptr, nullptr, nullptr, &finalSubkeys, nullptr,
                                 nullptr, &finalValues, nullptr, nullptr, nullptr, &after),
                 L"Cannot verify the copied genuine policy tree");
    if (subkeys != finalSubkeys || values != finalValues ||
        before.dwLowDateTime != after.dwLowDateTime || before.dwHighDateTime != after.dwHighDateTime)
        throw OverlayError{ERROR_RETRY, L"Genuine Chrome policies changed during the snapshot"};
}

void OverlaySetString(HKEY key, const wchar_t* name, const std::wstring& text) {
    OverlayCheck(RegSetValueExW(key, name, 0, REG_SZ,
                               reinterpret_cast<const BYTE*>(text.c_str()),
                               static_cast<DWORD>((text.size() + 1) * sizeof(wchar_t))),
                 L"Cannot add a transient Chrome policy");
}

std::shared_ptr<OverlaySnapshot> OverlayBuildSnapshot(const OverlayConfig& config, REGSAM view) {
    OverlayPreflight();
    OverlayKey genuine;
    LSTATUS result = OverlayNativeOpen(HKEY_CURRENT_USER, kOverlayChromePath, 0,
                                      KEY_READ | view, &genuine.value);
    if (!OverlayMissing(result)) OverlayCheck(result, L"Cannot read genuine Chrome policy data");
    const auto serial = config.hive->nextSnapshot.fetch_add(1, std::memory_order_relaxed) + 1;
    if (serial == 0)
        throw OverlayError{ERROR_ARITHMETIC_OVERFLOW, L"The private snapshot sequence is exhausted"};
    auto snapshot = std::make_shared<OverlaySnapshot>();
    snapshot->hive = config.hive;
    std::wstring name = L"View" + std::to_wstring(serial);
    OverlayKey root;
    DWORD disposition = 0;
    OverlayCheck(RegCreateKeyExW(config.hive->root, name.c_str(), 0, nullptr,
                                REG_OPTION_VOLATILE, KEY_ALL_ACCESS, nullptr,
                                &root.value, &disposition),
                 L"Cannot create a volatile policy snapshot");
    if (disposition != REG_CREATED_NEW_KEY)
        throw OverlayError{ERROR_ALREADY_EXISTS, L"A private snapshot name unexpectedly already exists"};
    snapshot->root = root.release();
    snapshot->name = std::move(name);
    if (genuine.value) {
        OverlayCopyBudget budget;
        OverlayCopyTree(genuine.value, snapshot->root, view, 0, budget);
    }
    // Retain genuine nested values for native RegGetValue/subkey consumers.
    // Reject a conflicting owned policy copied during a concurrent HKCU change.
    // Registry loading does not promise atomicity across the real user/machine hives.
    OverlayCheckPolicyKey(snapshot->root, 0);
    OverlaySetString(snapshot->root, kOverlayOwned[1], kOverlayClearJson);
    OverlaySetString(snapshot->root, kOverlayOwned[2], config.keepJson);
    const DWORD sessionOnly = 4;
    OverlayCheck(RegSetValueExW(snapshot->root, kOverlayOwned[0], 0, REG_DWORD,
                               reinterpret_cast<const BYTE*>(&sessionOnly), sizeof(sessionOnly)),
                 L"Cannot add the transient session-only policy");
    return snapshot;
}

std::wstring OverlayKeepJson(const std::vector<std::wstring>& sites) {
    if (sites.size() > 256)
        throw OverlayError{ERROR_INVALID_DATA, L"Too many keep-site entries were supplied"};
    std::wstring result = L"[";
    bool first = true;
    for (const auto& site : sites) {
        if (site.empty() || site.size() > 253)
            throw OverlayError{ERROR_INVALID_DATA, L"An invalid normalized keep-site entry was supplied"};
        if (!first) result += L",";
        first = false;
        result += L"\"[*.]";
        for (wchar_t c : site) {
            // The settings parser supplies normalized ASCII/punycode domains.
            if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || c == L'-' || c == L'.'))
                throw OverlayError{ERROR_INVALID_DATA, L"A keep-site entry was not a normalized domain"};
            result += c;
        }
        result += L"\"";
    }
    return result + L"]";
}

bool OverlayPublishConfig(unsigned long long request,
                          std::shared_ptr<const OverlayConfig> replacement) {
    std::shared_ptr<const OverlayConfig> retired;
    bool accepted = false;
    {
        OverlayLock lock;
        if (g_overlayState && g_overlayRequestVersion.load(std::memory_order_acquire) == request) {
            retired.swap(g_overlayState->config);
            g_overlayState->config.swap(replacement);
            g_overlayEnabled.store(!!g_overlayState->config, std::memory_order_release);
            g_overlayState->lastErrorAction = nullptr;
            accepted = true;
        }
    }
    // Replaced configuration/hive destructors run after the bookkeeping lock.
    return accepted;
}

// Called by AfterInit/settings callbacks, never by a registry hook. A prepared
// hive is reused while enabled; requests are ordered so an old slow preparation
// cannot publish after a later disable or settings change.
bool SetOverlaySites(const std::vector<std::wstring>* sites) {
    const auto request = g_overlayRequestVersion.fetch_add(1, std::memory_order_acq_rel) + 1;
    OverlayBypass bypass;
    try {
        if (!sites) {
            OverlayPublishConfig(request, nullptr);
            return true;
        }
        bool initialized = false;
        {
            OverlayReadLock lock;
            initialized = g_overlayState &&
                          !g_overlayHooksRemoved.load(std::memory_order_acquire);
        }
        if (!initialized) return false;
        auto config = std::make_shared<OverlayConfig>();
        config->keepJson = OverlayKeepJson(*sites);
        auto current = OverlayCaptureConfig();
        if (current) config->hive = current->hive;
        if (!config->hive) config->hive = OverlayCreateHive();
        return OverlayPublishConfig(request, std::move(config));
    } catch (const OverlayError& error) {
        if (!OverlayPublishConfig(request, nullptr)) return false;
        OverlayLogError(error.action, error.code);
    } catch (...) {
        if (!OverlayPublishConfig(request, nullptr)) return false;
        OverlayLogError(L"Cannot prepare the requested cleanup configuration", ERROR_NOT_ENOUGH_MEMORY);
    }
    Wh_Log(L"Save the mod settings or toggle cleanup off/on to retry preparation; Chrome Reload policies alone cannot prepare a hive.");
    return false;
}

bool OverlayReadAccess(REGSAM access) {
    constexpr REGSAM views = KEY_WOW64_32KEY | KEY_WOW64_64KEY;
    return (access & ~(KEY_READ | views)) == 0 &&
           // Preserve genuine registry notifications. A KEY_NOTIFY-only open
           // must watch the real key, rather than an immutable private copy.
           (access & (KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS)) != 0 &&
           (access & views) != views;
}
bool OverlayRootPath(HKEY root, LPCWSTR path) {
    return root == HKEY_CURRENT_USER && path &&
           CompareStringOrdinal(path, -1, kOverlayChromePath, -1, TRUE) == CSTR_EQUAL;
}
void OverlayReportFailure(const wchar_t* action, LSTATUS error) noexcept {
    OverlayLogError(action, error);
    Wh_Log(L"Chrome will read its genuine policies for this request. Reload policies to retry a complete transient view.");
}

LSTATUS OverlayRegisterClient(HKEY key, const std::shared_ptr<OverlaySnapshot>& snapshot) {
    // Allocate the node before locking. std::map node insertion cannot rehash
    // or allocate while the bookkeeping lock is held.
    OverlayClients staging;
    staging.emplace(key, snapshot);
    auto node = staging.extract(key);
    OverlayClients::node_type rejected;
    LSTATUS status = ERROR_CANCELLED;
    {
        OverlayLock lock;
        if (g_overlayState) {
            auto inserted = g_overlayState->clients.insert(std::move(node));
            rejected = std::move(inserted.node);
            status = inserted.inserted ? ERROR_SUCCESS : ERROR_ALREADY_EXISTS;
            g_overlayTrackedCount.store(g_overlayState->clients.size(), std::memory_order_release);
        }
    }
    return status;
}

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY key, LPCWSTR subkey, DWORD options,
                                 REGSAM access, PHKEY result) {
    if (g_overlayBypass) return OverlayNativeOpen(key, subkey, options, access, result);
    OverlayBypass bypass;
    bool trackedParent = false;
    LSTATUS failure = ERROR_NOT_ENOUGH_MEMORY;
    try {
        auto parent = OverlayCaptureParent(key);
        if (parent) {
            trackedParent = true;
            if (!result) return ERROR_INVALID_PARAMETER;
            OverlayKey opened;
            LSTATUS status = OverlayNativeOpen(key, subkey, options, access, &opened.value);
            if (status != ERROR_SUCCESS) return status;
            OverlayCheck(OverlayRegisterClient(opened.value, parent), L"Cannot track a relative policy-key handle");
            *result = opened.release();
            return ERROR_SUCCESS;
        }
        if (g_overlayEnabled.load(std::memory_order_acquire) && options == 0 &&
            OverlayReadAccess(access) && OverlayRootPath(key, subkey)) {
            if (!result) return ERROR_INVALID_PARAMETER;
            auto config = OverlayCaptureConfig();
            if (config) {
                std::shared_ptr<OverlaySnapshot> snapshot;
                for (unsigned attempt = 0; attempt < 2; ++attempt) {
                    try {
                        snapshot = OverlayBuildSnapshot(*config, access & (KEY_WOW64_32KEY | KEY_WOW64_64KEY));
                        break;
                    } catch (const OverlayError& error) {
                        if (attempt || (error.code != ERROR_RETRY && error.code != ERROR_MORE_DATA)) throw;
                        Wh_Log(L"Chrome policies changed during a snapshot; retrying once.");
                    }
                }
                OverlayKey opened;
                OverlayCheck(OverlayNativeOpen(snapshot->root, L"", 0, access, &opened.value),
                             L"Cannot open the private policy snapshot for Chrome");
                OverlayCheck(OverlayRegisterClient(opened.value, snapshot), L"Cannot track a policy snapshot handle");
                *result = opened.release();
                return ERROR_SUCCESS;
            }
        }
    } catch (const OverlayError& error) {
        failure = error.code;
        OverlayReportFailure(error.action, error.code);
    } catch (...) {
        OverlayReportFailure(L"Cannot allocate a transient Chrome policy view", ERROR_NOT_ENOUGH_MEMORY);
    }
    // A child whose bookkeeping failed was closed by RAII; never return an
    // untracked replacement which could outlive its parent snapshot.
    if (trackedParent) return failure;
    return OverlayNativeOpen(key, subkey, options, access, result);
}
LSTATUS WINAPI RegCloseKey_Hook(HKEY key) {
    if (g_overlayBypass || !g_overlayTrackedCount.load(std::memory_order_acquire))
        return OverlayNativeClose(key);
    OverlayBypass bypass;
    auto snapshot = OverlayCaptureParent(key);
    OverlayClients::node_type node;
    // Retain the mapping while the native close runs unlocked. A concurrent
    // relative open that still succeeds must be able to retain this snapshot.
    LSTATUS status = OverlayNativeClose(key);
    if (status == ERROR_SUCCESS && snapshot) {
        OverlayLock lock;
        if (g_overlayState) {
            auto found = g_overlayState->clients.find(key);
            if (found != g_overlayState->clients.end() && found->second == snapshot)
                node = g_overlayState->clients.extract(found);
            g_overlayTrackedCount.store(g_overlayState->clients.size(), std::memory_order_release);
        }
    }
    // Failure leaves its mapping intact without any allocation. If Windows
    // reuses the numeric HKEY before retirement, duplicate registration fails
    // conservatively instead of replacing another caller's tracked handle.
    // Node/snapshot/native-key destruction occurs here, without a held lock.
    return status;
}

void ShutdownOverlay() {
    // Called after hooks are removed, but an in-flight hook can still finish.
    // Keep original pointers immutable; atomically select native exports for
    // cleanup and late calls instead of racing readers of those pointers.
    g_overlayHooksRemoved.store(true, std::memory_order_release);
    g_overlayRequestVersion.fetch_add(1, std::memory_order_acq_rel);
    OverlayBypass bypass;
    OverlayState* retired = nullptr;
    {
        OverlayLock lock;
        retired = g_overlayState;
        g_overlayState = nullptr;
        g_overlayEnabled.store(false, std::memory_order_release);
        g_overlayTrackedCount.store(0, std::memory_order_release);
    }
    if (!retired) return;
    for (auto& entry : retired->clients) entry.second->preserveNativeKeys = true;
    // Only internal references are released. Caller-owned HKEYs stay native and
    // valid after this DLL unloads. All deletion/closure runs outside the lock.
    delete retired;
}
// END_TRANSIENT_POLICY_OVERLAY
// BEGIN_POLICY_REFRESH
// Included inside the mod's anonymous namespace.
// Required headers: windows.h, shellapi.h, userenv.h, string_view.
// Chromium sources:
// https://github.com/chromium/chromium/blob/main/components/policy/core/common/policy_loader_win.cc
// https://github.com/chromium/chromium/blob/main/base/command_line.cc

decltype(&::RegisterGPNotification) RegisterGPNotification_Original = nullptr;
decltype(&::UnregisterGPNotification) UnregisterGPNotification_Original = nullptr;

// POD-only globals need no destructor at process termination. The small bounded
// table is ample for Chrome's two policy events. Nothing under this lock calls
// Windows, logs, allocates, or destroys an owning C++ object.
constexpr size_t kPolicyRefreshEventLimit = 64;
struct PolicyRefreshEvent {
    HANDLE original;
    HANDLE duplicate;
    size_t requests;
    bool retired;
};
SRWLOCK g_policyRefreshLock = SRWLOCK_INIT;
PolicyRefreshEvent g_policyRefreshEvents[kPolicyRefreshEventLimit]{};
unsigned long long g_policyRefreshEpoch = 0;
bool g_policyRefreshStopped = false;

class PolicyRefreshGuard {
public:
    PolicyRefreshGuard() { AcquireSRWLockExclusive(&g_policyRefreshLock); }
    ~PolicyRefreshGuard() { ReleaseSRWLockExclusive(&g_policyRefreshLock); }
    PolicyRefreshGuard(const PolicyRefreshGuard&) = delete;
    PolicyRefreshGuard& operator=(const PolicyRefreshGuard&) = delete;
};

// Caller holds g_policyRefreshLock. The returned handle is closed only after
// unlocking. An outstanding request keeps its duplicate alive until release.
HANDLE RetirePolicyRefreshEvent(PolicyRefreshEvent& entry) {
    entry.original = nullptr;
    entry.retired = true;
    if (entry.requests) {
        return nullptr;
    }
    HANDLE duplicate = entry.duplicate;
    entry = {};
    return duplicate;
}

BOOL WINAPI RegisterGPNotification_Hook(HANDLE event, BOOL machine) {
    unsigned long long epoch;
    bool track;
    {
        PolicyRefreshGuard lock;
        epoch = g_policyRefreshEpoch;
        track = !g_policyRefreshStopped;
    }
    const BOOL result = RegisterGPNotification_Original(event, machine);
    const DWORD error = GetLastError();
    bool needsDuplicate = false;
    if (result && track) {
        PolicyRefreshGuard lock;
        if (!g_policyRefreshStopped && g_policyRefreshEpoch == epoch) {
            needsDuplicate = true;
            for (const auto& entry : g_policyRefreshEvents) {
                if (entry.original == event && !entry.retired) {
                    needsDuplicate = false;
                    break;
                }
            }
        }
    }
    if (needsDuplicate) {
        HANDLE duplicate = nullptr;
        if (DuplicateHandle(GetCurrentProcess(), event, GetCurrentProcess(),
                            &duplicate, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            bool adopted = false;
            bool full = false;
            {
                PolicyRefreshGuard lock;
                // A concurrent successful unregister invalidates this attempt.
                // Missing a refresh event only requires a manual policy reload;
                // retaining a potentially stale registration would be worse.
                if (!g_policyRefreshStopped && g_policyRefreshEpoch == epoch) {
                    bool exists = false;
                    PolicyRefreshEvent* available = nullptr;
                    for (auto& entry : g_policyRefreshEvents) {
                        if (entry.original == event && !entry.retired) {
                            exists = true;
                            break;
                        }
                        if (!entry.duplicate && !available) {
                            available = &entry;
                        }
                    }
                    if (!exists && available) {
                        *available = {event, duplicate, 0, false};
                        adopted = true;
                    } else if (!exists) {
                        full = true;
                    }
                }
            }
            if (!adopted) {
                CloseHandle(duplicate);
            }
            if (full) {
                Wh_Log(L"Chrome policy event tracking is full. Use chrome://policy "
                       L"to reload policies after changing the mod's settings.");
            }
        } else {
            Wh_Log(L"Could not retain Chrome's policy refresh event: %lu",
                   GetLastError());
        }
    }
    SetLastError(error);
    return result;
}

BOOL WINAPI UnregisterGPNotification_Hook(HANDLE event) {
    const BOOL result = UnregisterGPNotification_Original(event);
    const DWORD error = GetLastError();
    HANDLE retired = nullptr;
    if (result) {
        PolicyRefreshGuard lock;
        ++g_policyRefreshEpoch;
        for (auto& entry : g_policyRefreshEvents) {
            if (entry.original == event && !entry.retired) {
                retired = RetirePolicyRefreshEvent(entry);
                break;
            }
        }
    }
    if (retired) {
        CloseHandle(retired);
    }
    SetLastError(error);
    return result;
}

void RequestPolicyRefresh(bool logIfUnavailable = true) {
    const DWORD originalError = GetLastError();
    struct CapturedEvent {
        size_t index;
        HANDLE duplicate;
    };
    CapturedEvent captured[kPolicyRefreshEventLimit]{};
    size_t count = 0;
    bool saturated = false;
    {
        PolicyRefreshGuard lock;
        if (!g_policyRefreshStopped) {
            for (size_t i = 0; i < kPolicyRefreshEventLimit; ++i) {
                auto& entry = g_policyRefreshEvents[i];
                if (!entry.duplicate || entry.retired) {
                    continue;
                }
                if (entry.requests == static_cast<size_t>(-1)) {
                    saturated = true;
                    continue;
                }
                ++entry.requests;
                captured[count++] = {i, entry.duplicate};
            }
        }
    }
    if (!count && logIfUnavailable) {
        Wh_Log(L"No Chrome policy event was captured. Use chrome://policy "
               L"to reload policies, or restart Chrome.");
    }
    if (saturated) {
        Wh_Log(L"Could not retain a policy refresh request. Use chrome://policy "
               L"to reload policies.");
    }
    for (size_t i = 0; i < count; ++i) {
        bool active;
        {
            PolicyRefreshGuard lock;
            active = !g_policyRefreshEvents[captured[i].index].retired;
        }
        // A request lease protects this duplicate from closure or reuse.
        // Signaling requests an asynchronous reload, not an acknowledgement.
        if (active && !SetEvent(captured[i].duplicate)) {
            Wh_Log(L"Could not request Chrome policy refresh: %lu", GetLastError());
        }
    }
    HANDLE retired[kPolicyRefreshEventLimit]{};
    size_t retiredCount = 0;
    {
        PolicyRefreshGuard lock;
        for (size_t i = 0; i < count; ++i) {
            auto& entry = g_policyRefreshEvents[captured[i].index];
            --entry.requests;
            if (entry.retired && !entry.requests) {
                retired[retiredCount++] = RetirePolicyRefreshEvent(entry);
            }
        }
    }
    for (size_t i = 0; i < retiredCount; ++i) {
        CloseHandle(retired[i]);
    }
    SetLastError(originalError);
}

void ClearPolicyRefreshEvents() {
    const DWORD originalError = GetLastError();
    HANDLE retired[kPolicyRefreshEventLimit]{};
    size_t count = 0;
    {
        PolicyRefreshGuard lock;
        // Called during unload, after the final refresh request. Outstanding
        // requests release their own leases; no new registration can be kept.
        g_policyRefreshStopped = true;
        ++g_policyRefreshEpoch;
        for (auto& entry : g_policyRefreshEvents) {
            if (entry.duplicate) {
                if (HANDLE duplicate = RetirePolicyRefreshEvent(entry)) {
                    retired[count++] = duplicate;
                }
            }
        }
    }
    for (size_t i = 0; i < count; ++i) {
        CloseHandle(retired[i]);
    }
    SetLastError(originalError);
}

bool IsChromeCommandLineWhitespace(wchar_t ch) {
    // Chromium trims Unicode whitespace from each parsed Windows argument.
    return (ch >= L'\t' && ch <= L'\r') || ch == L' ' || ch == 0x0085 ||
           ch == 0x00a0 || ch == 0x1680 || (ch >= 0x2000 && ch <= 0x200a) ||
           ch == 0x2028 || ch == 0x2029 || ch == 0x202f || ch == 0x205f ||
           ch == 0x3000;
}

bool IsAsciiSwitchName(std::wstring_view value, std::wstring_view expected) {
    if (value.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < value.size(); ++i) {
        wchar_t ch = value[i];
        if (ch >= L'A' && ch <= L'Z') {
            ch += L'a' - L'A';
        }
        if (ch != expected[i]) {
            return false;
        }
    }
    return true;
}

BOOL IsMainChromeProcess() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"Cannot parse Chrome's process arguments: %lu", GetLastError());
        return FALSE;
    }
    BOOL isMain = argc > 0;
    for (int i = 1; i < argc; ++i) {
        std::wstring_view arg(argv[i]);
        while (!arg.empty() && IsChromeCommandLineWhitespace(arg.front())) {
            arg.remove_prefix(1);
        }
        while (!arg.empty() && IsChromeCommandLineWhitespace(arg.back())) {
            arg.remove_suffix(1);
        }
        if (arg == L"--") {
            break;
        }
        if (arg.size() >= 2 && arg[0] == L'-' && arg[1] == L'-') {
            arg.remove_prefix(2);
        } else if (!arg.empty() && (arg[0] == L'-' || arg[0] == L'/')) {
            arg.remove_prefix(1);
        } else {
            continue;
        }
        const auto equals = arg.find(L'=');
        const auto name = arg.substr(0, equals);
        // Windows shell launches can carry the rest of the raw command line as
        // one URL argument. Chromium recognizes this marker before lowercasing.
        if (name == L"single-argument") {
            break;
        }
        if (IsAsciiSwitchName(name, L"type")) {
            isMain = FALSE;
            break;
        }
    }
    LocalFree(argv);
    return isMain;
}
// END_POLICY_REFRESH
void ApplySettings() {
    try {
        if (!Wh_GetIntSetting(L"enabled")) {
            SetOverlaySites(nullptr);
            Wh_Log(L"Cleanup is disabled. Reload Chrome policies to clear cached settings.");
            return;
        }
        const auto sites = ReadKeepSites();
        if (SetOverlaySites(&sites)) {
            Wh_Log(L"Cleanup requested with %zu keep sites. Reload Chrome policies to verify.",
                   sites.size());
        }
    } catch (const SettingsError& error) {
        SetOverlaySites(nullptr);
        Wh_Log(L"Cleanup disabled: %s", error.message.c_str());
    } catch (const std::exception& error) {
        SetOverlaySites(nullptr);
        Wh_Log(L"Cleanup disabled after a settings error: %hs", error.what());
    } catch (...) {
        SetOverlaySites(nullptr);
        Wh_Log(L"Cleanup disabled after an unexpected settings error.");
    }
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsMainChromeProcess()) {
        return FALSE;
    }
    Wh_Log(L"Initializing version %s", WH_MOD_VERSION);
    if (!InitializeOverlayStorage()) {
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(RegOpenKeyExW, RegOpenKeyExW_Hook,
                                       &RegOpenKeyExW_Original) ||
        !WindhawkUtils::SetFunctionHook(RegCloseKey, RegCloseKey_Hook,
                                       &RegCloseKey_Original) ||
        !WindhawkUtils::SetFunctionHook(RegisterGPNotification,
                                       RegisterGPNotification_Hook,
                                       &RegisterGPNotification_Original) ||
        !WindhawkUtils::SetFunctionHook(UnregisterGPNotification,
                                       UnregisterGPNotification_Hook,
                                       &UnregisterGPNotification_Original)) {
        Wh_Log(L"Failed to register the Chrome policy hooks.");
        ShutdownOverlay();
        ClearPolicyRefreshEvents();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // Prepare private storage outside registry hooks, after every hook is live.
    // At normal startup Chrome has not registered its policy events yet; the
    // initial policy load will see these settings without a refresh warning.
    ApplySettings();
    RequestPolicyRefresh(false);
}

void Wh_ModSettingsChanged() {
    ApplySettings();
    RequestPolicyRefresh();
}

void Wh_ModBeforeUninit() {
    // Stop new redirections before the hooks disappear. Existing readers finish
    // their complete snapshot, including its keep list, and retain valid HKEYs.
    SetOverlaySites(nullptr);
    RequestPolicyRefresh();
    ClearPolicyRefreshEvents();
}

void Wh_ModUninit() {
    ClearPolicyRefreshEvents();
    ShutdownOverlay();
    Wh_Log(L"Unloaded. Chrome may require Reload policies to discard its cache.");
}

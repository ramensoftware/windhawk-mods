// ==WindhawkMod==
// @id              chrome-clear-on-exit-keep-sites
// @name            Chrome clear on exit with keep sites
// @description     Clears Chrome browsing data on exit while keeping selected website logins.
// @version         0.4.0
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

The mod runs only in Chrome's browser process, not its renderer or utility
processes. It does not change Chrome's real policy registry keys or profile files.
A private application hive under Windhawk's own mod storage provides genuine
Windows registry handles; the supplied values live in volatile keys. Files use a
separate private directory for each user/process instance and are never reused as
configuration. Completed reads normally remove their backing files. Files can
remain if a process exits during a read, cleanup fails, or Chrome still holds a
key during mod unload; those files are inactive and remain within Windhawk's
removable mod storage. There is no recovery journal to back up.

## Scope and limitations

- Applies to all profiles in each Chrome browser process that loads the mod.
  Chrome must be allowed to load Windhawk mods.
- Intended for personal installations. Conflicting existing local Chrome policies
  prevent injection; they are never replaced. Chrome's normal cloud-policy
  precedence still applies. Check Windhawk's mod log if the entries do not appear.
- Chrome can show **Managed by your organization** while these policies are active.
  Chrome also disables synchronization of affected data categories.
- Normal last-window close is the supported cleanup path. Crashes, forced
  termination, Windows shutdown, and some restart/quit paths can
  prevent completion or defer selective site-data deletion until the next start.
- Disabling the mod cannot restore browsing data Chrome has already deleted.

Back up the source and your Windhawk settings to reproduce the configuration on a
fresh Windows installation. The source's defaults do not contain your saved list.

If upgrading from the unpublished 0.2/0.3 policy-writing prototype, turn its cleanup
setting off and approve its removal prompt first. Confirm that its three real
policies are gone before replacing that prototype with this version.

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
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
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
struct OverlayBypass {
    OverlayBypass() { ++g_overlayBypass; }
    ~OverlayBypass() { --g_overlayBypass; }
};
struct OverlayLock {
    OverlayLock() { AcquireSRWLockExclusive(&g_overlayLock); }
    ~OverlayLock() { ReleaseSRWLockExclusive(&g_overlayLock); }
};
LSTATUS OverlayNativeOpen(HKEY key, LPCWSTR name, DWORD options, REGSAM access, PHKEY result) {
    return (RegOpenKeyExW_Original ? RegOpenKeyExW_Original : RegOpenKeyExW)(
        key, name, options, access, result);
}
LSTATUS OverlayNativeClose(HKEY key) {
    return (RegCloseKey_Original ? RegCloseKey_Original : RegCloseKey)(key);
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
    HANDLE directoryHandle = INVALID_HANDLE_VALUE;
    std::wstring directory;
    std::wstring file;
    std::wstring log1;
    std::wstring log2;
    ~OverlayHive() {
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
    bool preserveNativeKeys = false;
    ~OverlaySnapshot() {
        // After hooks disappear, Chrome still owns any returned HKEY handles.
        // Keep their native keys intact; closing our references is sufficient.
        if (!preserveNativeKeys && hive && !name.empty())
            RegDeleteTreeW(hive->root, name.c_str());
        if (root) OverlayNativeClose(root);
    }
};
struct OverlayState {
    std::shared_ptr<OverlayHive> hive;
    std::unordered_map<HKEY, std::shared_ptr<OverlaySnapshot>> clients;
    std::wstring keepJson;
    const wchar_t* lastErrorAction = nullptr;
    LSTATUS lastErrorCode = ERROR_SUCCESS;
    unsigned long long nextSnapshot = 0;
    bool enabled = false;
};
// Deliberately not a global owning object: process termination needs no cleanup
// callback, policy restoration, locks, or calls through trampoline pointers.
OverlayState* g_overlayState = nullptr;

void OverlayLogError(const wchar_t* action, LSTATUS code) noexcept {
    if (!g_overlayState || g_overlayState->lastErrorAction != action ||
        g_overlayState->lastErrorCode != code) {
        Wh_Log(L"Chrome policy overlay: %s (Windows error %ld). No real Chrome policy was changed.",
               action, static_cast<long>(code));
        if (g_overlayState) {
            g_overlayState->lastErrorAction = action;
            g_overlayState->lastErrorCode = code;
        }
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
    OverlayLock lock;
    OverlayBypass bypass;
    try {
        if (!g_overlayState) g_overlayState = new OverlayState;
        // File/hive creation is lazy: installing a disabled mod needs no disk I/O.
        return true;
    } catch (const OverlayError& error) {
        OverlayLogError(error.action, error.code);
    } catch (...) {
        Wh_Log(L"Chrome policy overlay: storage initialization failed. No real Chrome policy was changed.");
    }
    return false;
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

std::shared_ptr<OverlaySnapshot> OverlayBuildSnapshot(REGSAM view) {
    OverlayState& state = *g_overlayState;
    OverlayPreflight();
    OverlayKey genuine;
    LSTATUS result = OverlayNativeOpen(HKEY_CURRENT_USER, kOverlayChromePath, 0,
                                      KEY_READ | view, &genuine.value);
    if (!OverlayMissing(result)) OverlayCheck(result, L"Cannot read genuine Chrome policy data");
    if (++state.nextSnapshot == 0)
        throw OverlayError{ERROR_ARITHMETIC_OVERFLOW, L"The private snapshot sequence is exhausted"};
    if (!state.hive) state.hive = OverlayCreateHive();
    auto snapshot = std::make_shared<OverlaySnapshot>();
    snapshot->hive = state.hive;
    std::wstring name = L"View" + std::to_wstring(state.nextSnapshot);
    OverlayKey root;
    DWORD disposition = 0;
    OverlayCheck(RegCreateKeyExW(state.hive->root, name.c_str(), 0, nullptr,
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
    // Recheck the copied data and real policy locations before adding anything.
    OverlayCheckPolicyKey(snapshot->root, 0);
    OverlayPreflight();
    OverlaySetString(snapshot->root, kOverlayOwned[1], kOverlayClearJson);
    OverlaySetString(snapshot->root, kOverlayOwned[2], state.keepJson);
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

// Returns whether the requested configuration was accepted. Existing native
// handles keep a complete immutable snapshot; settings affect future root opens.
// Genuine policy conflicts are checked on every read, not merely on enable.
bool SetOverlaySites(const std::vector<std::wstring>* sites) {
    try {
        std::wstring json;
        if (sites) {
            json = OverlayKeepJson(*sites);
            if (!InitializeOverlayStorage()) {
                SetOverlaySites(nullptr);
                return false;
            }
        }
        OverlayLock lock;
        OverlayBypass bypass;
        if (!g_overlayState) return sites == nullptr;
        g_overlayState->enabled = false;
        g_overlayState->keepJson = std::move(json);
        g_overlayState->enabled = sites != nullptr;
        g_overlayState->lastErrorAction = nullptr;
        if (g_overlayState->clients.empty()) g_overlayState->hive.reset();
        return true;
    } catch (const OverlayError& error) {
        OverlayLock lock;
        OverlayBypass bypass;
        if (g_overlayState) {
            g_overlayState->enabled = false;
            if (g_overlayState->clients.empty()) g_overlayState->hive.reset();
        }
        OverlayLogError(error.action, error.code);
    } catch (...) {
        OverlayLock lock;
        OverlayBypass bypass;
        if (g_overlayState) {
            g_overlayState->enabled = false;
            if (g_overlayState->clients.empty()) g_overlayState->hive.reset();
        }
        Wh_Log(L"Chrome policy overlay: configuration was disabled after an unexpected error.");
    }
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
    try {
        OverlayLock lock;
        OverlayBypass bypass;
        OverlayLogError(action, error);
        if (g_overlayState && g_overlayState->clients.empty()) g_overlayState->hive.reset();
    } catch (...) {
        Wh_Log(L"Chrome policy overlay: a transient view could not be prepared; Chrome will read its genuine policies.");
    }
}

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY key, LPCWSTR subkey, DWORD options,
                                 REGSAM access, PHKEY result) {
    if (g_overlayBypass) return OverlayNativeOpen(key, subkey, options, access, result);
    bool trackedParent = false;
    LSTATUS failure = ERROR_NOT_ENOUGH_MEMORY;
    try {
        OverlayLock lock;
        OverlayBypass bypass;
        if (g_overlayState) {
            auto parent = g_overlayState->clients.find(key);
            if (parent != g_overlayState->clients.end()) {
                trackedParent = true;
                if (!result) return ERROR_INVALID_PARAMETER;
                OverlayKey opened;
                LSTATUS status = OverlayNativeOpen(key, subkey, options, access, &opened.value);
                if (status != ERROR_SUCCESS) return status;
                g_overlayState->clients.insert_or_assign(opened.value, parent->second);
                *result = opened.release();
                return ERROR_SUCCESS;
            }
            if (g_overlayState->enabled && options == 0 && OverlayReadAccess(access) &&
                OverlayRootPath(key, subkey)) {
                if (!result) return ERROR_INVALID_PARAMETER;
                auto snapshot = OverlayBuildSnapshot(access & (KEY_WOW64_32KEY | KEY_WOW64_64KEY));
                OverlayKey opened;
                OverlayCheck(OverlayNativeOpen(snapshot->root, L"", 0, access, &opened.value),
                             L"Cannot open the private policy snapshot for Chrome");
                g_overlayState->clients.insert_or_assign(opened.value, snapshot);
                *result = opened.release();
                g_overlayState->lastErrorAction = nullptr;
                return ERROR_SUCCESS;
            }
        }
    } catch (const OverlayError& error) {
        failure = error.code;
        OverlayReportFailure(error.action, error.code);
    } catch (...) {
        OverlayReportFailure(L"Cannot allocate a transient Chrome policy view", ERROR_NOT_ENOUGH_MEMORY);
    }
    // A relative child must share its parent's lifetime. If bookkeeping failed,
    // its RAII handle is already closed; never return an untracked replacement.
    if (trackedParent) return failure;
    OverlayBypass bypass;
    return OverlayNativeOpen(key, subkey, options, access, result);
}
LSTATUS WINAPI RegCloseKey_Hook(HKEY key) {
    if (g_overlayBypass) return OverlayNativeClose(key);
    LSTATUS status = ERROR_INVALID_HANDLE;
    bool closed = false;
    try {
        OverlayLock lock;
        OverlayBypass bypass;
        status = OverlayNativeClose(key);
        closed = true;
        // Keep the lock across native close + erase to prevent handle reuse
        // from deleting a different thread's newly registered snapshot handle.
        if (status == ERROR_SUCCESS && g_overlayState) {
            g_overlayState->clients.erase(key);
            if (g_overlayState->clients.empty()) g_overlayState->hive.reset();
        }
        return status;
    } catch (...) {
        if (closed) return status;
        OverlayBypass bypass;
        return OverlayNativeClose(key);
    }
}

void ShutdownOverlay() {
    // Called only after Windhawk has removed the hooks. Trampolines may have
    // been freed, so all final RAII cleanup must use the real exports instead.
    RegOpenKeyExW_Original = RegOpenKeyExW;
    RegCloseKey_Original = RegCloseKey;
    OverlayLock lock;
    OverlayBypass bypass;
    if (!g_overlayState) return;
    g_overlayState->enabled = false;
    for (auto& entry : g_overlayState->clients) entry.second->preserveNativeKeys = true;
    // Only internal references are released. Caller-owned HKEYs remain native,
    // valid, and Chrome can close them normally after this DLL has unloaded.
    g_overlayState->clients.clear();
    g_overlayState->hive.reset();
    delete g_overlayState;
    g_overlayState = nullptr;
}
// END_TRANSIENT_POLICY_OVERLAY
// BEGIN_POLICY_REFRESH
// Included inside the mod's anonymous namespace.
// Required headers: windows.h, shellapi.h, userenv.h, map, string_view.
// Chromium sources:
// https://github.com/chromium/chromium/blob/main/components/policy/core/common/policy_loader_win.cc
// https://github.com/chromium/chromium/blob/main/base/command_line.cc

decltype(&::RegisterGPNotification) RegisterGPNotification_Original = nullptr;
decltype(&::UnregisterGPNotification) UnregisterGPNotification_Original = nullptr;

// No global C++ destructors run for this state during process termination.
// Explicit unload releases the heap map and duplicate handles. Windows reclaims
// them at process exit, when Windhawk does not call the unload callbacks.
SRWLOCK g_policyRefreshLock = SRWLOCK_INIT;
using PolicyRefreshEvents = std::map<HANDLE, HANDLE>;
PolicyRefreshEvents* g_policyRefreshEvents = nullptr;
bool g_policyRefreshStopped = false;

class PolicyRefreshGuard {
public:
    PolicyRefreshGuard() { AcquireSRWLockExclusive(&g_policyRefreshLock); }
    ~PolicyRefreshGuard() { ReleaseSRWLockExclusive(&g_policyRefreshLock); }
    PolicyRefreshGuard(const PolicyRefreshGuard&) = delete;
    PolicyRefreshGuard& operator=(const PolicyRefreshGuard&) = delete;
};

BOOL WINAPI RegisterGPNotification_Hook(HANDLE event, BOOL machine) {
    BOOL result = FALSE;
    DWORD error = ERROR_SUCCESS;
    bool originalCalled = false;
    HANDLE unownedDuplicate = nullptr;
    try {
        // Serialize registration bookkeeping with unregister and shutdown. The
        // original API is always called, even if our bookkeeping fails.
        PolicyRefreshGuard lock;
        result = RegisterGPNotification_Original(event, machine);
        error = GetLastError();
        originalCalled = true;
        if (result && !g_policyRefreshStopped) {
            if (!g_policyRefreshEvents) {
                g_policyRefreshEvents = new PolicyRefreshEvents;
            }
            if (g_policyRefreshEvents->find(event) ==
                g_policyRefreshEvents->end()) {
                if (DuplicateHandle(GetCurrentProcess(), event,
                                    GetCurrentProcess(), &unownedDuplicate,
                                    0, FALSE, DUPLICATE_SAME_ACCESS)) {
                    g_policyRefreshEvents->emplace(event, unownedDuplicate);
                    unownedDuplicate = nullptr;
                } else {
                    Wh_Log(L"Could not retain Chrome's policy refresh event: %lu",
                           GetLastError());
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Could not track Chrome's policy refresh event; its Windows "
               L"registration remains unchanged.");
    }
    if (unownedDuplicate) {
        CloseHandle(unownedDuplicate);
    }
    if (!originalCalled) {
        result = RegisterGPNotification_Original(event, machine);
        error = GetLastError();
    }
    SetLastError(error);
    return result;
}

BOOL WINAPI UnregisterGPNotification_Hook(HANDLE event) {
    BOOL result = FALSE;
    DWORD error = ERROR_SUCCESS;
    bool originalCalled = false;
    try {
        PolicyRefreshGuard lock;
        result = UnregisterGPNotification_Original(event);
        error = GetLastError();
        originalCalled = true;
        if (result && g_policyRefreshEvents) {
            auto found = g_policyRefreshEvents->find(event);
            if (found != g_policyRefreshEvents->end()) {
                // The documented API unregisters this event, not one reference
                // to it. Repeated registration must not retain a stale event.
                CloseHandle(found->second);
                g_policyRefreshEvents->erase(found);
            }
        }
    } catch (...) {
        Wh_Log(L"Could not update policy refresh bookkeeping during unregister.");
    }
    if (!originalCalled) {
        result = UnregisterGPNotification_Original(event);
        error = GetLastError();
    }
    SetLastError(error);
    return result;
}

void RequestPolicyRefresh() {
    const DWORD originalError = GetLastError();
    try {
        PolicyRefreshGuard lock;
        if (!g_policyRefreshEvents || g_policyRefreshEvents->empty()) {
            Wh_Log(L"No Chrome policy event was captured. Use chrome://policy "
                   L"to reload policies, or restart Chrome.");
        }
        if (g_policyRefreshEvents) {
            for (const auto& [original, duplicate] : *g_policyRefreshEvents) {
                (void)original;
                // This wakes Chromium's existing asynchronous loader. It
                // neither changes Group Policy nor acknowledges completion.
                if (!SetEvent(duplicate)) {
                    Wh_Log(L"Could not request Chrome policy refresh: %lu",
                           GetLastError());
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Could not request Chrome policy refresh. Use chrome://policy "
               L"to reload policies, or restart Chrome.");
    }
    SetLastError(originalError);
}

void ClearPolicyRefreshEvents() {
    const DWORD originalError = GetLastError();
    try {
        PolicyRefreshGuard lock;
        // Called during unload, after the final refresh request. A concurrent
        // successful registration must not acquire a new duplicate afterward.
        g_policyRefreshStopped = true;
        if (g_policyRefreshEvents) {
            for (const auto& [original, duplicate] : *g_policyRefreshEvents) {
                (void)original;
                CloseHandle(duplicate);
            }
            delete g_policyRefreshEvents;
            g_policyRefreshEvents = nullptr;
        }
    } catch (...) {
        Wh_Log(L"Could not release policy refresh bookkeeping.");
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
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(RegOpenKeyExW),
                            reinterpret_cast<void*>(RegOpenKeyExW_Hook),
                            reinterpret_cast<void**>(&RegOpenKeyExW_Original)) ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(RegCloseKey),
                            reinterpret_cast<void*>(RegCloseKey_Hook),
                            reinterpret_cast<void**>(&RegCloseKey_Original)) ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(RegisterGPNotification),
                            reinterpret_cast<void*>(RegisterGPNotification_Hook),
                            reinterpret_cast<void**>(&RegisterGPNotification_Original)) ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(UnregisterGPNotification),
                            reinterpret_cast<void*>(UnregisterGPNotification_Hook),
                            reinterpret_cast<void**>(&UnregisterGPNotification_Original))) {
        Wh_Log(L"Failed to register the Chrome policy hooks.");
        ShutdownOverlay();
        ClearPolicyRefreshEvents();
        return FALSE;
    }
    ApplySettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    RequestPolicyRefresh();
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

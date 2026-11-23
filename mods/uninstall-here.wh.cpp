// ==WindhawkMod==
// @id              uninstall-here
// @name            Uninstall Here
// @description     Uninstall desktop apps directly from Start without opening Settings or Control Panel.
// @version         1.0.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/uninstall-here
// @donateUrl       https://github.com/sponsors/luizbizzio
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lruntimeobject -lmsi -lshell32 -luser32 -luuid -lversion -lwintrust -lcrypt32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Uninstall Here

Open a desktop app's own uninstaller from the Windows 11 Start menu. Uninstall Here saves you a trip through **Installed apps** when it can identify the app's uninstaller.

## How to use

**Start → All apps → right-click the app → Uninstall**

Confirm the product name before its uninstaller opens. MSI products open Windows Installer in uninstall mode. Follow any subsequent prompts from the uninstaller or Windows.

This mod targets 64-bit Windows 11. The **All apps** flow was tested on x64 build `26300.10075`.

## If Installed apps still opens

Windows handles packaged apps and desktop apps without one identifiable registered installation.

If you expected the app to work, enable logging for **Uninstall Here** in Windhawk and repeat the click. Include the app version, Windows build, and the mod's log entries in your report. [Report an issue](https://github.com/luizbizzio/uninstall-here/issues)

## Screenshot

![The Uninstall command highlighted in the Windows 11 Start menu](https://raw.githubusercontent.com/luizbizzio/uninstall-here/main/assets/uninstall-here-cover.png)

[Project on GitHub](https://github.com/luizbizzio/uninstall-here)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <inspectable.h>
#include <msi.h>
#include <msiquery.h>
#include <shellapi.h>
#include <shldisp.h>
#include <shobjidl.h>
#include <softpub.h>
#include <wintrust.h>
#include <windhawk_utils.h>
#include <winrt/base.h>

#include <cwchar>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace {

constexpr wchar_t kUninstallKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
constexpr size_t kMaxPath = 32767;

struct AppIdentity {
    std::wstring executable;
    std::wstring productCode;
};

struct UninstallCommand {
    std::wstring executable;
    std::wstring arguments;
};

struct ControlPanelAction {
    winrt::com_ptr<FolderItem2> item;
};

struct RegisteredUninstaller {
    std::variant<UninstallCommand, ControlPanelAction> action;
    std::wstring displayName;
    std::wstring publisher;
};

struct RegistryKey {
    HKEY key = nullptr;

    RegistryKey() = default;
    RegistryKey(const RegistryKey&) = delete;
    RegistryKey& operator=(const RegistryKey&) = delete;

    ~RegistryKey() {
        if (key) {
            RegCloseKey(key);
        }
    }
};

bool Equals(std::wstring_view a, std::wstring_view b) {
    if (a.empty() || b.empty()) {
        return a.empty() && b.empty();
    }
    return CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(),
                                static_cast<int>(b.size()), TRUE) == CSTR_EQUAL;
}

std::wstring_view Trim(std::wstring_view text) {
    const auto first = text.find_first_not_of(L" \t\r\n");
    if (first == text.npos) {
        return {};
    }
    const auto last = text.find_last_not_of(L" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool EndsWith(std::wstring_view text, std::wstring_view suffix) {
    return suffix.size() <= text.size() && Equals(text.substr(text.size() - suffix.size()), suffix);
}

// Accept local drive paths only. GetFullPathName normalizes dot segments and
// does not start the executable or resolve a shell item.
std::wstring LocalPath(std::wstring_view text) {
    text = Trim(text);
    if (text.size() >= 2 && text.front() == L'"' && text.back() == L'"') {
        text = text.substr(1, text.size() - 2);
    }
    if (text.size() < 3 || text.size() >= kMaxPath || text[1] != L':' ||
        (text[2] != L'\\' && text[2] != L'/') ||
        text.substr(2).find_first_of(L"\"<>|*?:\r\n") != text.npos) {
        return {};
    }
    const wchar_t root[] = {text[0], L':', L'\\', L'\0'};
    if (GetDriveTypeW(root) != DRIVE_FIXED) {
        return {};
    }
    const std::wstring input(text);
    DWORD capacity = GetFullPathNameW(input.c_str(), 0, nullptr, nullptr);
    if (capacity == 0 || capacity > kMaxPath) {
        return {};
    }
    std::wstring fullPath(capacity, L'\0');
    const DWORD length = GetFullPathNameW(input.c_str(), capacity, fullPath.data(), nullptr);
    if (length == 0 || length >= capacity) {
        return {};
    }
    fullPath.resize(length);
    while (fullPath.size() > 3 && fullPath.back() == L'\\') {
        fullPath.pop_back();
    }
    return fullPath;
}

bool IsFile(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    const DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool IsUnder(std::wstring_view file, std::wstring_view directory) {
    return directory.size() > 3 && file.size() > directory.size() &&
           file[directory.size()] == L'\\' && Equals(file.substr(0, directory.size()), directory);
}

std::wstring EnvironmentPath(PCWSTR name) {
    const DWORD capacity = GetEnvironmentVariableW(name, nullptr, 0);
    if (!capacity || capacity > kMaxPath) {
        return {};
    }
    std::wstring value(capacity, L'\0');
    const DWORD length = GetEnvironmentVariableW(name, value.data(), capacity);
    if (!length || length >= capacity) {
        return {};
    }
    value.resize(length);
    return LocalPath(value);
}

bool SpecificLocation(const std::wstring& directory) {
    const auto localAppData = EnvironmentPath(L"LOCALAPPDATA");
    return directory.size() > 3 && !Equals(directory, EnvironmentPath(L"ProgramFiles")) &&
           !Equals(directory, EnvironmentPath(L"ProgramFiles(x86)")) &&
           !Equals(directory, EnvironmentPath(L"CommonProgramFiles")) &&
           !Equals(directory, EnvironmentPath(L"CommonProgramFiles(x86)")) &&
           !Equals(directory, EnvironmentPath(L"ProgramData")) &&
           !Equals(directory, EnvironmentPath(L"USERPROFILE")) &&
           !Equals(directory, EnvironmentPath(L"APPDATA")) &&
           !Equals(directory, localAppData) && !Equals(directory, localAppData + L"\\Programs");
}

std::wstring ReadString(HKEY key, PCWSTR name) {
    constexpr DWORD types = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ;
    DWORD bytes = 0;
    if (RegGetValueW(key, nullptr, name, types, nullptr, nullptr, &bytes) != ERROR_SUCCESS ||
        bytes < sizeof(wchar_t) || bytes > (kMaxPath + 1) * sizeof(wchar_t)) {
        return {};
    }
    std::wstring value((bytes + sizeof(wchar_t) - 1) / sizeof(wchar_t), L'\0');
    if (RegGetValueW(key, nullptr, name, types, nullptr, value.data(), &bytes) != ERROR_SUCCESS) {
        return {};
    }
    const auto terminator = value.find(L'\0');
    if (terminator == value.npos) {
        return {};
    }
    value.resize(terminator);
    return value;
}

bool RegistryFlag(HKEY key, PCWSTR name) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    return RegGetValueW(key, nullptr, name, RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS && value;
}

std::wstring IconPath(std::wstring_view value) {
    const auto comma = value.find_last_of(L',');
    if (comma != value.npos) {
        auto index = Trim(value.substr(comma + 1));
        if (!index.empty() && index.front() == L'-') {
            index.remove_prefix(1);
        }
        if (!index.empty() && index.find_first_not_of(L"0123456789") == index.npos) {
            value = value.substr(0, comma);
        }
    }
    return LocalPath(value);
}

std::optional<UninstallCommand> ParseExecutableCommand(std::wstring_view command) {
    command = Trim(command);
    if (command.empty()) {
        return std::nullopt;
    }
    if (command.front() == L'"') {
        const auto quote = command.find(L'"', 1);
        if (quote == command.npos || (quote + 1 < command.size() && command[quote + 1] != L' ' &&
                                      command[quote + 1] != L'\t')) {
            return std::nullopt;
        }
        auto path = LocalPath(command.substr(1, quote - 1));
        if (EndsWith(path, L".exe") && IsFile(path)) {
            return UninstallCommand{std::move(path), std::wstring(Trim(command.substr(quote + 1)))};
        }
        return std::nullopt;
    }

    // Some installers register an unquoted path containing spaces. Resolve
    // exactly one existing executable before handing arguments to the shell.
    std::optional<UninstallCommand> matched;
    for (size_t end = 4; end <= command.size(); ++end) {
        if ((end != command.size() && command[end] != L' ' && command[end] != L'\t') ||
            !EndsWith(command.substr(0, end), L".exe")) {
            continue;
        }
        auto path = LocalPath(command.substr(0, end));
        if (IsFile(path)) {
            if (matched) {
                return std::nullopt;
            }
            matched = UninstallCommand{std::move(path), std::wstring(Trim(command.substr(end)))};
        }
    }
    return matched;
}

bool IsProductCode(const wchar_t* value) {
    GUID product{};
    return wcslen(value) == 38 && value[0] == L'{' && value[37] == L'}' &&
           SUCCEEDED(CLSIDFromString(value, &product));
}

UninstallCommand MsiCommand(const std::wstring& productCode) {
    wchar_t systemDirectory[MAX_PATH];
    const UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (length == 0 || length >= ARRAYSIZE(systemDirectory)) {
        return {};
    }
    return {std::wstring(systemDirectory, length) + L"\\msiexec.exe", L"/x " + productCode};
}

// An MSI can omit DisplayIcon. Use its installed component registration to
// verify the exact executable, with the cached database supplying component IDs.
bool MsiOwnsExecutable(PCWSTR productCode, const std::wstring& executable, const std::wstring& windows) {
    wchar_t packagePath[MAX_PATH];
    DWORD size = ARRAYSIZE(packagePath);
    if (windows.empty() ||
        MsiGetProductInfoW(productCode, INSTALLPROPERTY_LOCALPACKAGE, packagePath, &size) != ERROR_SUCCESS) {
        return false;
    }
    const auto package = LocalPath(packagePath);
    if (!IsUnder(package, windows + L"\\Installer") || !EndsWith(package, L".msi")) {
        return false;
    }
    PMSIHANDLE database;
    PMSIHANDLE view;
    if (MsiOpenDatabaseW(package.c_str(), MSIDBOPEN_READONLY, &database) != ERROR_SUCCESS ||
        MsiDatabaseOpenViewW(database,
                            L"SELECT `File`.`FileName`, `Component`.`ComponentId` FROM `File`, `Component` "
                            L"WHERE `File`.`Component_` = `Component`.`Component` AND `File`.`File` = `Component`.`KeyPath`",
                            &view) != ERROR_SUCCESS ||
        MsiViewExecute(view, 0) != ERROR_SUCCESS) {
        return false;
    }
    const auto filename = std::wstring_view(executable).substr(executable.find_last_of(L'\\') + 1);
    for (;;) {
        PMSIHANDLE record;
        if (MsiViewFetch(view, &record) != ERROR_SUCCESS) {
            return false;
        }
        wchar_t fileNameField[2 * MAX_PATH];
        size = ARRAYSIZE(fileNameField);
        if (MsiRecordGetStringW(record, 1, fileNameField, &size) != ERROR_SUCCESS) {
            continue;
        }
        const std::wstring_view names(fileNameField, size);
        const auto separator = names.find(L'|');
        const auto longName = separator == names.npos ? names : names.substr(separator + 1);
        if (!Equals(longName, filename)) {
            continue;
        }
        wchar_t component[39];
        size = ARRAYSIZE(component);
        if (MsiRecordGetStringW(record, 2, component, &size) != ERROR_SUCCESS || !IsProductCode(component)) {
            continue;
        }
        wchar_t installedPath[MAX_PATH];
        size = ARRAYSIZE(installedPath);
        if (MsiGetComponentPathW(productCode, component, installedPath, &size) != INSTALLSTATE_LOCAL ||
            !Equals(LocalPath(installedPath), executable)) {
            continue;
        }
        wchar_t owner[39];
        if (MsiEnumClientsW(component, 0, owner) == ERROR_SUCCESS && Equals(owner, productCode) &&
            MsiEnumClientsW(component, 1, owner) == ERROR_NO_MORE_ITEMS) {
            return true;
        }
    }
}

// Squirrel registers its package ID as the uninstall key. The root launcher
// and the matching executable in the installed release belong to that package.
bool IsSquirrelUninstaller(HKEY entry, std::wstring_view packageId, const std::wstring& executable,
                           const std::wstring& location, const std::wstring& localAppData,
                           const UninstallCommand& command) {
    if (localAppData.empty() || !Equals(location, localAppData + L"\\" + std::wstring(packageId)) ||
        !Equals(command.executable, location + L"\\Update.exe") || command.arguments != L"--uninstall") {
        return false;
    }
    const auto version = ReadString(entry, L"DisplayVersion");
    constexpr std::wstring_view digits = L"0123456789";
    if (version.empty() || digits.find(version.front()) == digits.npos ||
        version.find_first_not_of(L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.+-") != version.npos) {
        return false;
    }
    const auto name = executable.substr(executable.find_last_of(L'\\') + 1);
    const auto launcher = location + L"\\" + name;
    const auto releaseExecutable = location + L"\\app-" + version + L"\\" + name;
    return !Equals(name, L"Update.exe") && (Equals(executable, launcher) || Equals(executable, releaseExecutable)) &&
           IsFile(launcher) && IsFile(releaseExecutable) && IsFile(location + L"\\packages\\RELEASES");
}

struct SignedProduct {
    std::wstring name;
    std::wstring version;
    std::wstring publisher;
};

std::wstring VerifiedPublisher(const std::wstring& path, HANDLE fileHandle) {
    WINTRUST_FILE_INFO file{};
    file.cbStruct = sizeof(file);
    file.pcwszFilePath = path.c_str();
    file.hFile = fileHandle;
    WINTRUST_DATA trust{};
    trust.cbStruct = sizeof(trust);
    trust.dwUIChoice = WTD_UI_NONE;
    trust.dwUnionChoice = WTD_CHOICE_FILE;
    trust.pFile = &file;
    trust.dwStateAction = WTD_STATEACTION_VERIFY;
    // Keep certificate validation local to avoid a network wait on Start's UI thread.
    constexpr DWORD disableMd2Md4 = 0x00002000;  // WTD_DISABLE_MD2_MD4
    trust.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL | disableMd2Md4;
    GUID policy = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    wchar_t name[256]{};
    DWORD length = 0;
    if (WinVerifyTrust(nullptr, &policy, &trust) == ERROR_SUCCESS) {
        auto provider = WTHelperProvDataFromStateData(trust.hWVTStateData);
        auto signer = provider ? WTHelperGetProvSignerFromChain(provider, 0, FALSE, 0) : nullptr;
        if (signer && signer->csCertChain && signer->pasCertChain[0].pCert) {
            const auto cert = signer->pasCertChain[0].pCert;
            length = CertGetNameStringW(cert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, nullptr, 0);
            if (length > 1 && length <= ARRAYSIZE(name)) {
                length = CertGetNameStringW(cert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr,
                                            name, ARRAYSIZE(name));
            } else {
                length = 0;
            }
        }
    }
    trust.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policy, &trust);
    return length > 1 ? std::wstring(name, length - 1) : std::wstring();
}

std::optional<SignedProduct> ReadSignedProduct(const std::wstring& path) {
    // Hold the executable open across both reads so it cannot be replaced between them.
    const winrt::file_handle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file) {
        return std::nullopt;
    }
    const auto size = GetFileVersionInfoSizeW(path.c_str(), nullptr);
    if (size == 0 || size > 1024 * 1024) {
        return std::nullopt;
    }
    std::vector<BYTE> versionInfo(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, versionInfo.data())) {
        return std::nullopt;
    }
    struct Translation {
        WORD language;
        WORD codePage;
    };
    void* data = nullptr;
    UINT length = 0;
    if (!VerQueryValueW(versionInfo.data(), L"\\VarFileInfo\\Translation", &data, &length) ||
        length < sizeof(Translation)) {
        return std::nullopt;
    }
    const auto translation = *static_cast<const Translation*>(data);
    const auto readField = [&](PCWSTR field) -> std::wstring {
        wchar_t key[64]{};
        std::swprintf(key, ARRAYSIZE(key), L"\\StringFileInfo\\%04x%04x\\%ls",
                      static_cast<unsigned>(translation.language), static_cast<unsigned>(translation.codePage), field);
        void* value = nullptr;
        UINT count = 0;
        if (!VerQueryValueW(versionInfo.data(), key, &value, &count) || count <= 1 || count > 256) {
            return {};
        }
        const std::wstring_view fieldValue(static_cast<const wchar_t*>(value), count);
        const auto terminator = fieldValue.find(L'\0');
        if (terminator == fieldValue.npos) {
            return {};
        }
        return std::wstring(Trim(fieldValue.substr(0, terminator)));
    };
    auto name = readField(L"ProductName");
    auto version = readField(L"ProductVersion");
    if (name.empty() || version.empty()) {
        return std::nullopt;
    }
    auto publisher = VerifiedPublisher(path, file.get());
    if (publisher.empty()) {
        return std::nullopt;
    }
    return SignedProduct{std::move(name), std::move(version), std::move(publisher)};
}

struct AutomationValue {
    VARIANT value{};

    AutomationValue() = default;
    explicit AutomationValue(PCWSTR text) {
        value.vt = VT_BSTR;
        value.bstrVal = SysAllocString(text);
    }
    AutomationValue(const AutomationValue&) = delete;
    AutomationValue& operator=(const AutomationValue&) = delete;
    ~AutomationValue() { VariantClear(&value); }

    std::wstring text() const {
        return value.vt == VT_BSTR && value.bstrVal
                   ? std::wstring(value.bstrVal, SysStringLen(value.bstrVal))
                   : std::wstring();
    }
};

std::wstring ControlPanelProperty(FolderItem2* item, BSTR key) {
    AutomationValue property;
    return SUCCEEDED(item->ExtendedProperty(key, &property.value)) ? property.text() : std::wstring();
}

std::optional<RegisteredUninstaller> FindControlPanelUninstaller(const std::wstring& executable) {
    const auto product = ReadSignedProduct(executable);
    if (!product) {
        return std::nullopt;
    }
    winrt::com_ptr<IShellDispatch> shell;
    winrt::com_ptr<Folder> folder;
    winrt::com_ptr<FolderItems> entries;
    AutomationValue location(L"shell:ChangeRemoveProgramsFolder");
    if (!location.value.bstrVal ||
        FAILED(CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER, IID_IShellDispatch, shell.put_void())) ||
        FAILED(shell->NameSpace(location.value, folder.put())) || !folder ||
        FAILED(folder->Items(entries.put())) || !entries) {
        return std::nullopt;
    }
    const AutomationValue publisherKey(L"System.Software.Publisher");
    const AutomationValue versionKey(L"System.Software.ProductVersion");
    long count = 0;
    if (!publisherKey.value.bstrVal || !versionKey.value.bstrVal || FAILED(entries->get_Count(&count))) {
        return std::nullopt;
    }
    const auto versionedName = product->name + L" " + product->version;
    winrt::com_ptr<FolderItem2> matched;
    std::wstring displayName;
    VARIANT index{};
    index.vt = VT_I4;
    for (index.lVal = 0; index.lVal < count; ++index.lVal) {
        winrt::com_ptr<FolderItem> entry;
        AutomationValue entryName;
        entryName.value.vt = VT_BSTR;
        if (FAILED(entries->Item(index, entry.put())) || !entry ||
            FAILED(entry->get_Name(&entryName.value.bstrVal))) {
            return std::nullopt;
        }
        auto name = entryName.text();
        if (!Equals(name, product->name) && !Equals(name, versionedName)) {
            continue;
        }
        const auto item = entry.try_as<FolderItem2>();
        if (!item ||
            !Equals(ControlPanelProperty(item.get(), versionKey.value.bstrVal), product->version) ||
            !Equals(ControlPanelProperty(item.get(), publisherKey.value.bstrVal), product->publisher)) {
            continue;
        }
        if (matched) {
            Wh_Log(L"More than one Control Panel entry matches the selected executable.");
            return std::nullopt;
        }
        matched = item;
        displayName = std::move(name);
    }
    if (!matched) {
        return std::nullopt;
    }
    return RegisteredUninstaller{ControlPanelAction{std::move(matched)}, std::move(displayName), product->publisher};
}

std::optional<RegisteredUninstaller> FindUninstallCommand(const AppIdentity& app) {
    struct Root {
        HKEY hive;
        REGSAM view;
    };
    const Root roots[] = {
        {HKEY_CURRENT_USER, 0},
        {HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY},
        {HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY},
    };
    const auto windows = EnvironmentPath(L"SystemRoot");
    const auto localAppData = EnvironmentPath(L"LOCALAPPDATA");
    std::optional<RegisteredUninstaller> matched;
    size_t matches = 0;

    for (const auto& root : roots) {
        RegistryKey list;
        if (RegOpenKeyExW(root.hive, kUninstallKey, 0, KEY_READ | root.view, &list.key) != ERROR_SUCCESS) {
            continue;
        }
        for (DWORD index = 0;; ++index) {
            wchar_t subkey[256];
            DWORD length = ARRAYSIZE(subkey);
            const LSTATUS status = RegEnumKeyExW(list.key, index, subkey, &length, nullptr, nullptr, nullptr, nullptr);
            if (status == ERROR_NO_MORE_ITEMS) {
                break;
            }
            if (status != ERROR_SUCCESS) {
                return std::nullopt;
            }
            if (!app.productCode.empty() && !Equals(app.productCode, subkey)) {
                continue;
            }
            RegistryKey entry;
            if (RegOpenKeyExW(list.key, subkey, 0, KEY_READ | root.view, &entry.key) != ERROR_SUCCESS ||
                RegistryFlag(entry.key, L"SystemComponent") ||
                RegistryFlag(entry.key, L"NoRemove") || !ReadString(entry.key, L"ParentKeyName").empty()) {
                continue;
            }
            auto displayName = ReadString(entry.key, L"DisplayName");
            if (Trim(displayName).empty()) {
                continue;
            }
            const bool msi = RegistryFlag(entry.key, L"WindowsInstaller") && IsProductCode(subkey);
            if (!app.productCode.empty() && !msi) {
                continue;
            }

            std::wstring location;
            bool iconMatches = false;
            if (app.productCode.empty()) {
                if (app.executable.empty()) {
                    continue;
                }
                iconMatches = Equals(IconPath(ReadString(entry.key, L"DisplayIcon")), app.executable);
                const auto registeredLocation = ReadString(entry.key, L"InstallLocation");
                if (Trim(registeredLocation).empty()) {
                    if (msi || !iconMatches) {
                        continue;
                    }
                    location = app.executable.substr(0, app.executable.find_last_of(L'\\'));
                } else {
                    location = LocalPath(registeredLocation);
                }
                if (!IsUnder(app.executable, location) || !SpecificLocation(location) ||
                    (msi && !iconMatches && !MsiOwnsExecutable(subkey, app.executable, windows))) {
                    continue;
                }
            }
            std::optional<UninstallCommand> command;
            if (!msi) {
                command = ParseExecutableCommand(ReadString(entry.key, L"UninstallString"));
                if (!iconMatches &&
                    (root.hive != HKEY_CURRENT_USER || !command ||
                     !IsSquirrelUninstaller(entry.key, subkey, app.executable, location, localAppData, *command))) {
                    continue;
                }
            }
            if (++matches > 1) {
                Wh_Log(L"More than one installation claims the selected app.");
                return std::nullopt;
            }
            auto publisher = ReadString(entry.key, L"Publisher");
            if (msi) {
                auto msiCommand = MsiCommand(subkey);
                if (IsFile(msiCommand.executable)) {
                    matched = RegisteredUninstaller{std::move(msiCommand), std::move(displayName), std::move(publisher)};
                }
                continue;
            }
            if (command && !IsUnder(command->executable, windows) &&
                IsUnder(command->executable, location)) {
                matched = RegisteredUninstaller{std::move(*command), std::move(displayName), std::move(publisher)};
            }
        }
    }
    if (matches == 0 && app.productCode.empty() && !app.executable.empty()) {
        return FindControlPanelUninstaller(app.executable);
    }
    return matched;
}

AppIdentity ResolveShortcut(std::wstring path) {
    AppIdentity app;
    if (EndsWith(path, L".lnk") && IsFile(path)) {
        wchar_t productCode[39]{};
        if (MsiGetShortcutTargetW(path.c_str(), productCode, nullptr, nullptr) == ERROR_SUCCESS &&
            IsProductCode(productCode)) {
            app.productCode = productCode;
            return app;
        }
        winrt::com_ptr<IShellLinkW> link;
        winrt::com_ptr<IPersistFile> file;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW,
                                    link.put_void())) ||
            FAILED(link->QueryInterface(IID_IPersistFile, file.put_void())) ||
            FAILED(file->Load(path.c_str(), STGM_READ))) {
            return app;
        }
        wchar_t executable[MAX_PATH]{};
        if (link->GetPath(executable, ARRAYSIZE(executable), nullptr, 0) != S_OK) {
            return app;
        }
        path = LocalPath(executable);
    }
    if (EndsWith(path, L".exe") && IsFile(path) && !IsUnder(path, EnvironmentPath(L"SystemRoot"))) {
        app.executable = std::move(path);
    }
    return app;
}

// The tile and shortcut arguments use the WinRT ABI. Only the named interfaces
// and property slots required to identify the app are read.
constexpr GUID kTileIdentifierInterface{0xd3653510, 0x4fff, 0x4bfa, {0x90, 0x5b, 0xea, 0x03, 0x8b, 0x14, 0x2f, 0xa5}};
constexpr GUID kUnifiedTileInterface{0x65b4e03e, 0xa32e, 0x40cf, {0x8b, 0xab, 0xb2, 0xd9, 0xc5, 0x28, 0x73, 0x07}};
constexpr GUID kWin32ShortcutInterface{0x861778d6, 0xac6c, 0x456f, {0xbd, 0x3c, 0x32, 0xab, 0x60, 0x12, 0x45, 0xe1}};

template <typename Output>
HRESULT GetProperty(IInspectable* object, size_t slot, Output* value) {
    using Getter = HRESULT(STDMETHODCALLTYPE*)(IInspectable*, Output*);
    auto vtable = *reinterpret_cast<void* const* const*>(object);
    return reinterpret_cast<Getter>(vtable[slot])(object, value);
}

AppIdentity ReadTile(IInspectable* tile) {
    winrt::com_ptr<IInspectable> unifiedTile;
    winrt::com_ptr<IInspectable> identifier;
    if (!tile || FAILED(tile->QueryInterface(kUnifiedTileInterface, unifiedTile.put_void())) ||
        FAILED(GetProperty(unifiedTile.get(), 6, identifier.put())) || !identifier) {
        return {};
    }
    winrt::com_ptr<IInspectable> id;
    int kind = 0;
    if (FAILED(identifier->QueryInterface(kTileIdentifierInterface, id.put_void())) ||
        FAILED(GetProperty(id.get(), 6, &kind)) || kind != 2) {  // Win32
        return {};
    }
    winrt::com_ptr<IInspectable> shortcutInfo;
    winrt::com_ptr<IInspectable> shortcut;
    if (FAILED(GetProperty(unifiedTile.get(), 12, shortcutInfo.put())) || !shortcutInfo ||
        FAILED(shortcutInfo->QueryInterface(kWin32ShortcutInterface, shortcut.put_void()))) {
        return {};
    }
    BOOLEAN targetFolder = FALSE;
    winrt::hstring path;
    winrt::hstring arguments;
    if (FAILED(GetProperty(shortcut.get(), 6, &targetFolder)) || targetFolder ||
        FAILED(GetProperty(shortcut.get(), 7, winrt::put_abi(path))) ||
        FAILED(GetProperty(shortcut.get(), 8, winrt::put_abi(arguments)))) {
        return {};
    }
    auto app = ResolveShortcut(LocalPath(std::wstring_view(path)));
    // Some shortcuts address a browser profile, web app, or other product by
    // passing arguments to a common executable. That path alone is insufficient.
    if (!Trim(std::wstring_view(arguments)).empty() && app.productCode.empty()) {
        return {};
    }
    return app;
}

using GetVerbs = HRESULT(WINAPI*)(void*, IInspectable*, void*, void**);
using InitVerb = HRESULT(WINAPI*)(void*, void*, void*, HSTRING, HSTRING, bool, PCWSTR);
using ExecuteVerb = HRESULT(WINAPI*)(void*, void*);
using DeleteVerb = void*(WINAPI*)(void*, unsigned int);

GetVerbs g_originalAppResolver;
GetVerbs g_originalTileStore;
GetVerbs g_originalStateRepository;
InitVerb g_originalInit;
ExecuteVerb g_originalExecute;
DeleteVerb g_originalScalarDelete;
DeleteVerb g_originalVectorDelete;
HMODULE g_startTileData;

thread_local IInspectable* g_enumeratedTile;
std::mutex g_appMutex;
std::unordered_map<void*, AppIdentity> g_apps;

struct EnumerationScope {
    IInspectable* previous;

    explicit EnumerationScope(IInspectable* tile) : previous(g_enumeratedTile) {
        g_enumeratedTile = tile;
    }

    ~EnumerationScope() {
        g_enumeratedTile = previous;
    }
};

HRESULT WINAPI AppResolverHook(void* self, IInspectable* tile, void* args, void** result) {
    EnumerationScope scope(tile);
    return g_originalAppResolver(self, tile, args, result);
}

HRESULT WINAPI TileStoreHook(void* self, IInspectable* tile, void* args, void** result) {
    EnumerationScope scope(tile);
    return g_originalTileStore(self, tile, args, result);
}

HRESULT WINAPI StateRepositoryHook(void* self, IInspectable* tile, void* args, void** result) {
    EnumerationScope scope(tile);
    return g_originalStateRepository(self, tile, args, result);
}

HRESULT WINAPI InitHook(void* self, void* args, void* strings, HSTRING provider, HSTRING group,
                        bool optionalFeature, PCWSTR featureName) {
    {
        // An address can be reused even when the new verb has no desktop app identity.
        std::lock_guard lock(g_appMutex);
        g_apps.erase(self);
    }
    const HRESULT result = g_originalInit(self, args, strings, provider, group, optionalFeature, featureName);
    if (SUCCEEDED(result) && g_enumeratedTile && !optionalFeature) {
        try {
            auto app = ReadTile(g_enumeratedTile);
            if (!app.executable.empty() || !app.productCode.empty()) {
                std::lock_guard lock(g_appMutex);
                g_apps.insert_or_assign(self, std::move(app));
            }
        } catch (...) {
            Wh_Log(L"Could not read the selected tile's identity.");
        }
    }
    return result;
}

template <DeleteVerb& Original>
void* WINAPI DeleteHook(void* self, unsigned int flags) {
    {
        std::lock_guard lock(g_appMutex);
        g_apps.erase(self);
    }
    return Original(self, flags);
}

bool ConfirmUninstall(const RegisteredUninstaller& uninstaller) {
    // Registry labels are displayed as single lines so the product and publisher
    // cannot run into the surrounding confirmation text.
    auto singleLine = [](std::wstring label) {
        for (auto& character : label) {
            if (character < L' ') {
                character = L' ';
            }
        }
        return std::wstring(Trim(label));
    };
    auto message = L"Uninstall " + singleLine(uninstaller.displayName) + L"?";
    const auto publisher = singleLine(uninstaller.publisher);
    if (!publisher.empty()) {
        message += L"\n\nPublisher: " + publisher;
    }
    // Start has its own window band. Use its foreground window as the owner
    // when it is still open, and otherwise let the confirmation stand alone.
    HWND owner = GetForegroundWindow();
    DWORD ownerProcess = 0;
    GetWindowThreadProcessId(owner, &ownerProcess);
    if (ownerProcess != GetCurrentProcessId()) {
        owner = nullptr;
    }
    return MessageBoxW(owner, message.c_str(), L"Uninstall Here",
                       MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TASKMODAL |
                           MB_SETFOREGROUND | MB_TOPMOST) == IDYES;
}

bool OpenUninstaller(const RegisteredUninstaller& uninstaller) {
    if (const auto action = std::get_if<ControlPanelAction>(&uninstaller.action)) {
        const AutomationValue verb(L"changeremove");
        if (!verb.value.bstrVal) {
            return false;
        }
        const auto result = action->item->InvokeVerb(verb.value);
        return SUCCEEDED(result) || result == HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    const auto& command = std::get<UninstallCommand>(uninstaller.action);
    const auto directory = command.executable.substr(0, command.executable.find_last_of(L'\\'));
    SHELLEXECUTEINFOW execute{};
    execute.cbSize = sizeof(execute);
    execute.fMask = SEE_MASK_FLAG_NO_UI;
    execute.lpFile = command.executable.c_str();
    execute.lpParameters = command.arguments.empty() ? nullptr : command.arguments.c_str();
    execute.lpDirectory = directory.c_str();
    execute.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&execute)) {
        Wh_Log(L"Opened the registered uninstaller.");
        return true;
    }
    const DWORD error = GetLastError();
    if (error == ERROR_CANCELLED) {
        Wh_Log(L"Uninstaller launch was cancelled.");
        return true;
    }
    Wh_Log(L"Windows could not open the registered uninstaller.");
    return false;
}

HRESULT WINAPI ExecuteHook(void* self, void* args) {
    try {
        AppIdentity app;
        {
            std::lock_guard lock(g_appMutex);
            auto it = g_apps.find(self);
            if (it != g_apps.end()) {
                app = it->second;
            }
        }
        if (!app.executable.empty() || !app.productCode.empty()) {
            const auto uninstaller = FindUninstallCommand(app);
            if (uninstaller) {
                if (!ConfirmUninstall(*uninstaller) || OpenUninstaller(*uninstaller)) {
                    return S_OK;
                }
            }
            Wh_Log(L"No uninstaller was launched for this installation.");
        } else {
            Wh_Log(L"The uninstall action has no resolved desktop tile identity.");
        }
    } catch (...) {
        Wh_Log(L"Resolving the uninstaller failed.");
    }
    return g_originalExecute(self, args);
}

}  // namespace

BOOL Wh_ModInit() {
    // Keep the module loaded while Windhawk owns hooks into its code.
    g_startTileData = LoadLibraryExW(L"StartTileData.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_startTileData) {
        return FALSE;
    }
    DeleteVerb scalarDelete = nullptr;
    DeleteVerb vectorDelete = nullptr;
    WindhawkUtils::SYMBOL_HOOK startTileDataDllHooks[] = {
        {{L"?Execute@SettingsUninstallVerb@Private@UnifiedTile@Shell@WindowsInternal@@UEAAJPEAUIVerbExecutionArgs@345@@Z"},
         &g_originalExecute, ExecuteHook},
        {{L"?RuntimeClassInitialize@SettingsUninstallVerb@Private@UnifiedTile@Shell@WindowsInternal@@QEAAJPEAUIVerbEnumerationArgs@345@PEAUIVerbStringHelperStatics@2345@PEAUHSTRING__@@2_NPEBG@Z"},
         &g_originalInit, InitHook},
        {{L"??_GSettingsUninstallVerb@Private@UnifiedTile@Shell@WindowsInternal@@UEAAPEAXI@Z"},
         &scalarDelete, nullptr, true},
        {{L"??_ESettingsUninstallVerb@Private@UnifiedTile@Shell@WindowsInternal@@UEAAPEAXI@Z"},
         &vectorDelete, nullptr, true},
        {{L"?GetVerbs@AppResolverTransformer@DataStoreCache@@UEAAJPEAUIUnifiedTile@UnifiedTile@Shell@WindowsInternal@@PEAUIVerbEnumerationArgs@456@PEAPEAU?$IVectorView@PEAVTileVerb@UnifiedTile@Shell@WindowsInternal@@@Collections@Foundation@Windows@@@Z"},
         &g_originalAppResolver, AppResolverHook, true},
        {{L"?GetVerbs@TileStoreTransformer@DataStoreCache@@UEAAJPEAUIUnifiedTile@UnifiedTile@Shell@WindowsInternal@@PEAUIVerbEnumerationArgs@456@PEAPEAU?$IVectorView@PEAVTileVerb@UnifiedTile@Shell@WindowsInternal@@@Collections@Foundation@Windows@@@Z"},
         &g_originalTileStore, TileStoreHook, true},
        {{L"?GetVerbs@StateRepositoryTransformer@DataStoreCache@@UEAAJPEAUIUnifiedTile@UnifiedTile@Shell@WindowsInternal@@PEAUIVerbEnumerationArgs@456@PEAPEAU?$IVectorView@PEAVTileVerb@UnifiedTile@Shell@WindowsInternal@@@Collections@Foundation@Windows@@@Z"},
         &g_originalStateRepository, StateRepositoryHook, true},
    };
    const WH_HOOK_SYMBOLS_OPTIONS options{sizeof(options), nullptr, TRUE, nullptr};
    const bool symbolsFound =
        WindhawkUtils::HookSymbols(g_startTileData, startTileDataDllHooks,
                                   ARRAYSIZE(startTileDataDllHooks), &options) &&
        (g_originalAppResolver || g_originalTileStore || g_originalStateRepository) &&
        (scalarDelete || vectorDelete);
    // Some builds give both destructor symbols the same address.
    if (!symbolsFound ||
        (scalarDelete && !WindhawkUtils::SetFunctionHook(
                             scalarDelete, DeleteHook<g_originalScalarDelete>, &g_originalScalarDelete)) ||
        (vectorDelete && vectorDelete != scalarDelete && !WindhawkUtils::SetFunctionHook(
                             vectorDelete, DeleteHook<g_originalVectorDelete>, &g_originalVectorDelete))) {
        Wh_Log(L"Could not install the Start menu hooks on this build.");
        FreeLibrary(g_startTileData);
        g_startTileData = nullptr;
        return FALSE;
    }
    Wh_Log(L"Start menu hooks installed.");
    return TRUE;
}

void Wh_ModUninit() {
    if (g_startTileData) {
        FreeLibrary(g_startTileData);
    }
}

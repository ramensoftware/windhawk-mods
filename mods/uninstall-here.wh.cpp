// ==WindhawkMod==
// @id              uninstall-here
// @name            Uninstall Here
// @description     Uninstall desktop apps directly from Start and Search without opening Settings or Control Panel.
// @version         1.1.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/uninstall-here
// @donateUrl       https://github.com/sponsors/luizbizzio
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lruntimeobject -lmsi -lshell32 -luser32 -luuid -lversion -lwintrust -lcrypt32 -lcomctl32 -ldwmapi -lgdi32 -lgdiplus
// @license         Apache-2.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Uninstall Here

Open a desktop app's own uninstaller from the Windows 11 Start menu. Uninstall Here saves you a trip through **Installed apps** when it can identify the app's uninstaller.

## How to use

Right-click a desktop app and choose **Uninstall** in any of these places:

- **Start → All**
- **Start → Pinned**
- **Start → Recent**
- **Search results**, from the right-click menu or the **Options** button

Confirm the product name before its uninstaller opens. The confirmation looks the same everywhere and follows the Windows light or dark mode and accent color. **Cancel** is the default. MSI products open Windows Installer in uninstall mode. Follow any subsequent prompts from the uninstaller or Windows.

This mod targets 64-bit Windows 11. These places were tested on x64 build `26300.10075`.

## How it works

Start and Search send the selected shortcut to Explorer over a local pipe. Explorer accepts requests only from those two, then looks up the app, shows the confirmation, and opens the uninstaller. Search runs in a sandbox that can't see every installed app or start an uninstaller properly, so this can't happen there. For this reason, the mod is also loaded into `explorer.exe`, where it hooks nothing.

## If Installed apps still opens

Windows handles packaged apps and desktop apps without one identifiable registered installation. Office apps, which share one installation, are an example.

A shortcut that passes arguments to a program may open something else inside it, such as a web app inside a browser, so the mod leaves most of them to Windows. The exceptions are known forms that start the program itself: the Start shortcuts of Chromium browsers such as Brave and Chrome, Squirrel apps such as Discord, the Java Control Panel pages, and Git Bash and Git CMD.

If you expected the app to work, enable logging for **Uninstall Here** in Windhawk and repeat the click. Include the app version, Windows build, and the mod's log entries in your report. [Report an issue](https://github.com/luizbizzio/uninstall-here/issues)

## Screenshot

![The Uninstall command highlighted in the Windows 11 Start menu](https://raw.githubusercontent.com/luizbizzio/uninstall-here/main/assets/uninstall-here-cover.png)

[Project on GitHub](https://github.com/luizbizzio/uninstall-here)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <inspectable.h>
#include <msi.h>
#include <msiquery.h>
#include <objidl.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shldisp.h>
#include <shobjidl.h>
#include <sddl.h>
#include <softpub.h>
#include <wintrust.h>
#include <windhawk_utils.h>
#include <winrt/base.h>
#include <winrt/Windows.UI.ViewManagement.h>

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

// The clicked tile's shortcut, as Windows reports it.
struct TileShortcut {
    std::wstring path;
    std::wstring arguments;
};

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

std::wstring ParentFolder(const std::wstring& path) {
    const auto separator = path.find_last_of(L'\\');
    return separator == path.npos ? std::wstring() : path.substr(0, separator);
}

std::wstring_view FileName(std::wstring_view path) {
    return path.substr(path.find_last_of(L'\\') + 1);
}

// A shortcut with arguments can open something else inside a program, such as a
// web app inside a browser. Only these forms are known to start the program itself.
bool ArgumentsKeepApp(std::wstring_view executable, std::wstring_view arguments) {
    arguments = Trim(arguments);
    if (arguments.empty()) {
        return true;
    }
    const auto singleWord = [](std::wstring_view text) {
        return !text.empty() && text.find_first_of(L" \t\"") == text.npos;
    };
    // Chromium browsers mark their own Start shortcut.
    constexpr std::wstring_view source = L"--source-shortcut-location=";
    if (arguments.starts_with(source)) {
        return singleWord(arguments.substr(source.size()));
    }
    const auto name = FileName(executable);
    // A Squirrel app's shortcut may repeat the name of the program it starts,
    // with or without quotes.
    constexpr std::wstring_view processStart = L"--processStart ";
    if (arguments.starts_with(processStart)) {
        auto target = Trim(arguments.substr(processStart.size()));
        if (target.size() >= 2 && target.front() == L'"' && target.back() == L'"') {
            target = target.substr(1, target.size() - 2);
        }
        return Equals(target, name);
    }
    // The pages of the Java Control Panel.
    constexpr std::wstring_view tab = L"-tab ";
    if (Equals(name, L"javacpl.exe")) {
        return arguments.starts_with(tab) && singleWord(arguments.substr(tab.size()));
    }
    // Git's own launchers.
    return (Equals(name, L"git-bash.exe") || Equals(name, L"git-cmd.exe")) && arguments == L"--cd-to-home";
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
    // Weaker evidence, used only when no registration names the app directly: the
    // uninstaller sits in the same folder as the selected executable.
    std::optional<RegisteredUninstaller> sameFolder;
    size_t sameFolderMatches = 0;
    const auto appFolder = ParentFolder(app.executable);

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
            bool noLocation = false;
            if (app.productCode.empty()) {
                if (app.executable.empty()) {
                    continue;
                }
                iconMatches = Equals(IconPath(ReadString(entry.key, L"DisplayIcon")), app.executable);
                const auto registeredLocation = ReadString(entry.key, L"InstallLocation");
                noLocation = Trim(registeredLocation).empty();
                if (!noLocation) {
                    location = LocalPath(registeredLocation);
                } else if (iconMatches && !msi) {
                    location = appFolder;
                }
                if (msi) {
                    // Without a registered folder, only Windows Installer's own
                    // record that it installed this exact file is enough.
                    const bool inLocation = IsUnder(app.executable, location) && SpecificLocation(location);
                    if (noLocation ? !SpecificLocation(appFolder) || !MsiOwnsExecutable(subkey, app.executable, windows)
                                   : !inLocation || (!iconMatches && !MsiOwnsExecutable(subkey, app.executable, windows))) {
                        continue;
                    }
                }
            }
            std::optional<UninstallCommand> command;
            if (!msi) {
                command = ParseExecutableCommand(ReadString(entry.key, L"UninstallString"));
                if (app.productCode.empty()) {
                    const bool direct =
                        IsUnder(app.executable, location) && SpecificLocation(location) &&
                        (iconMatches || (root.hive == HKEY_CURRENT_USER && command &&
                                         IsSquirrelUninstaller(entry.key, subkey, app.executable, location,
                                                               localAppData, *command)));
                    if (!direct) {
                        // A launcher can register commands in its own folder that
                        // remove something it manages, such as a Steam game.
                        const bool managedItem =
                            command && (command->arguments.find(L"://") != std::wstring::npos ||
                                        (!command->arguments.empty() && Equals(command->executable, app.executable)));
                        if (command && !managedItem && (noLocation || Equals(location, appFolder)) &&
                            SpecificLocation(appFolder) &&
                            Equals(ParentFolder(command->executable), appFolder) &&
                            !IsUnder(command->executable, windows) && ++sameFolderMatches == 1) {
                            sameFolder = RegisteredUninstaller{std::move(*command), std::move(displayName),
                                                               ReadString(entry.key, L"Publisher")};
                        }
                        continue;
                    }
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
        if (sameFolderMatches > 1) {
            Wh_Log(L"More than one uninstaller shares the selected app's folder.");
            return std::nullopt;
        }
        return sameFolder ? sameFolder : FindControlPanelUninstaller(app.executable);
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

TileShortcut ReadTile(IInspectable* tile) {
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
    return {std::wstring(std::wstring_view(path)), std::wstring(std::wstring_view(arguments))};
}

std::optional<RegisteredUninstaller> FindTileUninstaller(const TileShortcut& tile) {
    const auto app = ResolveShortcut(LocalPath(tile.path));
    if ((app.executable.empty() && app.productCode.empty()) ||
        (app.productCode.empty() && !ArgumentsKeepApp(app.executable, tile.arguments))) {
        Wh_Log(L"The selected shortcut does not identify one desktop app.");
        return std::nullopt;
    }
    return FindUninstallCommand(app);
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
std::unordered_map<void*, TileShortcut> g_apps;

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
            auto tile = ReadTile(g_enumeratedTile);
            if (!tile.path.empty()) {
                std::lock_guard lock(g_appMutex);
                g_apps.insert_or_assign(self, std::move(tile));
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

// Set when the mod stops, so an open confirmation or a pending request ends.
HANDLE g_stopEvent;

// The confirmation follows the Windows 11 dialog that Start shows: a title, a
// short message, and two buttons in a separate bottom area, in the system theme.
constexpr wchar_t kDialogClass[] = L"WindhawkUninstallHereConfirmation";
// Sizes at 96 DPI.
constexpr int kDialogWidth = 372;
constexpr int kDialogPadding = 24;
constexpr int kDialogTextGap = 12;
constexpr int kButtonHeight = 32;
constexpr int kButtonGap = 8;
constexpr int kButtonRadius = 4;

struct ButtonColors {
    COLORREF fill;
    COLORREF hover;
    COLORREF pressed;
    COLORREF edge;
    COLORREF text;
};

struct DialogColors {
    COLORREF content;
    COLORREF footer;
    COLORREF divider;
    COLORREF edge;
    COLORREF text;
    ButtonColors button;
    ButtonColors accent;
    bool dark;
};

struct Confirmation {
    DialogColors colors{};
    HBRUSH contentBrush = nullptr;
    HWND hovered = nullptr;
    int lastFocus = IDNO;
    int footerTop = 0;
    UINT dpi = USER_DEFAULT_SCREEN_DPI;
    bool confirmed = false;
    bool closed = false;

    int Scale(int value) const { return MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI); }
};

COLORREF Mix(COLORREF color, COLORREF other, int percent) {
    const auto channel = [percent](int a, int b) { return static_cast<BYTE>((a * percent + b * (100 - percent)) / 100); };
    return RGB(channel(GetRValue(color), GetRValue(other)), channel(GetGValue(color), GetGValue(other)),
               channel(GetBValue(color), GetBValue(other)));
}

DialogColors CurrentDialogColors() {
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        const COLORREF window = GetSysColor(COLOR_WINDOW);
        const COLORREF text = GetSysColor(COLOR_WINDOWTEXT);
        const COLORREF highlight = GetSysColor(COLOR_HIGHLIGHT);
        const COLORREF highlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
        return {window, window, text, text, text,
                {window, window, window, text, text},
                {highlight, highlight, highlight, text, highlightText},
                false};
    }
    // Start follows the Windows mode, which can differ from the app mode.
    DWORD light = 0;
    DWORD size = sizeof(light);
    const bool dark = RegGetValueW(HKEY_CURRENT_USER,
                                   L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                   L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size) != ERROR_SUCCESS ||
                      !light;
    COLORREF accent = dark ? RGB(96, 205, 255) : RGB(0, 95, 184);
    try {
        using winrt::Windows::UI::ViewManagement::UIColorType;
        const auto color = winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(
            dark ? UIColorType::AccentLight2 : UIColorType::AccentDark1);
        accent = RGB(color.R, color.G, color.B);
    } catch (...) {
        Wh_Log(L"Using the default accent color.");
    }
    const COLORREF footer = dark ? RGB(32, 32, 32) : RGB(243, 243, 243);
    return {dark ? RGB(43, 43, 43) : RGB(255, 255, 255),
            footer,
            dark ? RGB(29, 29, 29) : RGB(229, 229, 229),
            dark ? RGB(69, 69, 69) : RGB(200, 200, 200),
            dark ? RGB(255, 255, 255) : RGB(27, 27, 27),
            dark ? ButtonColors{RGB(55, 55, 55), RGB(62, 62, 62), RGB(46, 46, 46), RGB(66, 66, 66), RGB(255, 255, 255)}
                 : ButtonColors{RGB(251, 251, 251), RGB(246, 246, 246), RGB(240, 240, 240), RGB(214, 214, 214),
                                RGB(27, 27, 27)},
            {accent, Mix(accent, footer, 90), Mix(accent, footer, 80), Mix(accent, footer, 90),
             dark ? RGB(0, 0, 0) : RGB(255, 255, 255)},
            dark};
}

void FillArea(HDC dc, const RECT& area, COLORREF color) {
    SetDCBrushColor(dc, color);
    FillRect(dc, &area, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

void AddRoundedRect(Gdiplus::GraphicsPath& path, const Gdiplus::RectF& area, float radius) {
    const float diameter = radius * 2;
    path.AddArc(area.X, area.Y, diameter, diameter, 180, 90);
    path.AddArc(area.X + area.Width - diameter, area.Y, diameter, diameter, 270, 90);
    path.AddArc(area.X + area.Width - diameter, area.Y + area.Height - diameter, diameter, diameter, 0, 90);
    path.AddArc(area.X, area.Y + area.Height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
}

Gdiplus::Color GdiColor(COLORREF color) {
    return Gdiplus::Color(GetRValue(color), GetGValue(color), GetBValue(color));
}

void DrawDialogButton(const Confirmation& dialog, const DRAWITEMSTRUCT& item) {
    // Cancel is the default choice and carries the accent color.
    const auto& colors = item.CtlID == IDNO ? dialog.colors.accent : dialog.colors.button;
    const COLORREF fill = (item.itemState & ODS_SELECTED) ? colors.pressed
                          : dialog.hovered == item.hwndItem ? colors.hover
                                                            : colors.fill;
    FillArea(item.hDC, item.rcItem, dialog.colors.footer);
    {
        Gdiplus::Graphics graphics(item.hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        const float width = static_cast<float>(item.rcItem.right - item.rcItem.left);
        const float height = static_cast<float>(item.rcItem.bottom - item.rcItem.top);
        const float radius = static_cast<float>(dialog.Scale(kButtonRadius));
        Gdiplus::GraphicsPath shape;
        AddRoundedRect(shape, {0.5f, 0.5f, width - 1, height - 1}, radius);
        const Gdiplus::SolidBrush brush(GdiColor(fill));
        graphics.FillPath(&brush, &shape);
        const Gdiplus::Pen edge(GdiColor(colors.edge), 1);
        graphics.DrawPath(&edge, &shape);
        // Show keyboard focus once the keyboard is in use.
        if ((item.itemState & ODS_FOCUS) && !(item.itemState & ODS_NOFOCUSRECT)) {
            const float inset = static_cast<float>(dialog.Scale(2));
            Gdiplus::GraphicsPath focus;
            AddRoundedRect(focus, {inset, inset, width - 2 * inset, height - 2 * inset}, radius);
            const Gdiplus::Pen ring(GdiColor(colors.text), static_cast<float>(dialog.Scale(2)));
            graphics.DrawPath(&ring, &focus);
        }
    }
    wchar_t label[32]{};
    GetWindowTextW(item.hwndItem, label, ARRAYSIZE(label));
    RECT area = item.rcItem;
    SetBkMode(item.hDC, TRANSPARENT);
    SetTextColor(item.hDC, colors.text);
    const auto previous = SelectObject(item.hDC, reinterpret_cast<HFONT>(SendMessageW(item.hwndItem, WM_GETFONT, 0, 0)));
    DrawTextW(item.hDC, label, -1, &area, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(item.hDC, previous);
}

LRESULT CALLBACK DialogButtonProc(HWND button, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR id,
                                  DWORD_PTR data) {
    auto& dialog = *reinterpret_cast<Confirmation*>(data);
    if (message == WM_MOUSEMOVE && dialog.hovered != button) {
        dialog.hovered = button;
        TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, button, 0};
        TrackMouseEvent(&track);
        InvalidateRect(button, nullptr, FALSE);
    } else if (message == WM_MOUSELEAVE && dialog.hovered == button) {
        dialog.hovered = nullptr;
        InvalidateRect(button, nullptr, FALSE);
    } else if (message == WM_SETFOCUS) {
        dialog.lastFocus = GetDlgCtrlID(button);
    } else if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(button, DialogButtonProc, id);
    }
    return DefSubclassProc(button, message, wParam, lParam);
}

LRESULT CALLBACK ConfirmationProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCCREATE) {
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams));
    }
    auto dialog = reinterpret_cast<Confirmation*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (!dialog) {
        return DefWindowProcW(window, message, wParam, lParam);
    }
    switch (message) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            const HDC dc = BeginPaint(window, &paint);
            RECT area;
            GetClientRect(window, &area);
            FillArea(dc, {0, 0, area.right, dialog->footerTop}, dialog->colors.content);
            FillArea(dc, {0, dialog->footerTop, area.right, area.bottom}, dialog->colors.footer);
            FillArea(dc, {0, dialog->footerTop, area.right, dialog->footerTop + dialog->Scale(1)},
                     dialog->colors.divider);
            EndPaint(window, &paint);
            return 0;
        }
        case WM_CTLCOLORSTATIC:
            SetTextColor(reinterpret_cast<HDC>(wParam), dialog->colors.text);
            SetBkColor(reinterpret_cast<HDC>(wParam), dialog->colors.content);
            return reinterpret_cast<LRESULT>(dialog->contentBrush);
        case WM_DRAWITEM:
            DrawDialogButton(*dialog, *reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
            return TRUE;
        case DM_GETDEFID:
            // Enter chooses the focused button, and Cancel otherwise.
            return MAKELRESULT(GetFocus() == GetDlgItem(window, IDYES) ? IDYES : IDNO, DC_HASDEFID);
        case WM_SETFOCUS:
            SetFocus(GetDlgItem(window, dialog->lastFocus));
            return 0;
        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED &&
                (LOWORD(wParam) == IDYES || LOWORD(wParam) == IDNO || LOWORD(wParam) == IDCANCEL)) {
                dialog->confirmed = LOWORD(wParam) == IDYES;
                DestroyWindow(window);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(window);
            return 0;
        case WM_DESTROY:
            dialog->closed = true;
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

int WrappedTextHeight(HFONT font, const std::wstring& text, int width) {
    const HDC dc = GetDC(nullptr);
    const auto previous = SelectObject(dc, font);
    RECT area{0, 0, width, 0};
    DrawTextW(dc, text.c_str(), -1, &area, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
    SelectObject(dc, previous);
    ReleaseDC(nullptr, dc);
    return area.bottom;
}

// Lays out and shows the dialog, then waits for an answer.
bool RunConfirmation(HWND window, HINSTANCE module, Confirmation& dialog, const std::wstring& title,
                     const std::wstring& body, const RECT& workArea, HFONT titleFont, HFONT bodyFont) {
    const int width = dialog.Scale(kDialogWidth);
    const int padding = dialog.Scale(kDialogPadding);
    const int textWidth = width - 2 * padding;
    const int titleHeight = WrappedTextHeight(titleFont, title, textWidth);
    const int bodyTop = padding + titleHeight + dialog.Scale(kDialogTextGap);
    const int bodyHeight = WrappedTextHeight(bodyFont, body, textWidth);
    dialog.footerTop = bodyTop + bodyHeight + padding;
    const int height = dialog.footerTop + 2 * padding + dialog.Scale(kButtonHeight);
    const int buttonWidth = (textWidth - dialog.Scale(kButtonGap)) / 2;
    const int buttonTop = dialog.footerTop + padding;

    const auto add = [&](PCWSTR type, const std::wstring& text, DWORD style, int id, RECT area, HFONT font) {
        const HWND control = CreateWindowExW(0, type, text.c_str(), WS_CHILD | WS_VISIBLE | style, area.left, area.top,
                                             area.right, area.bottom, window,
                                             reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), module, nullptr);
        if (control) {
            SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font), FALSE);
        }
        return control;
    };
    constexpr DWORD textStyle = SS_NOPREFIX | SS_EDITCONTROL;
    constexpr DWORD buttonStyle = WS_TABSTOP | BS_OWNERDRAW;
    const HWND titleText = add(L"STATIC", title, textStyle, -1, {padding, padding, textWidth, titleHeight}, titleFont);
    const HWND bodyText = add(L"STATIC", body, textStyle, -1, {padding, bodyTop, textWidth, bodyHeight}, bodyFont);
    const HWND uninstall = add(L"BUTTON", L"Uninstall", buttonStyle | WS_GROUP, IDYES,
                               {padding, buttonTop, buttonWidth, dialog.Scale(kButtonHeight)}, bodyFont);
    const HWND cancel = add(L"BUTTON", L"Cancel", buttonStyle, IDNO,
                            {width - padding - buttonWidth, buttonTop, buttonWidth, dialog.Scale(kButtonHeight)},
                            bodyFont);
    if (!titleText || !bodyText || !uninstall || !cancel ||
        !SetWindowSubclass(uninstall, DialogButtonProc, 1, reinterpret_cast<DWORD_PTR>(&dialog)) ||
        !SetWindowSubclass(cancel, DialogButtonProc, 1, reinterpret_cast<DWORD_PTR>(&dialog))) {
        return false;
    }

    const BOOL dark = dialog.colors.dark;
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    const auto corners = DWMWCP_ROUND;
    DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
    DwmSetWindowAttribute(window, DWMWA_BORDER_COLOR, &dialog.colors.edge, sizeof(dialog.colors.edge));

    SetWindowPos(window, HWND_TOPMOST, workArea.left + (workArea.right - workArea.left - width) / 2,
                 workArea.top + (workArea.bottom - workArea.top - height) / 2, width, height, SWP_NOACTIVATE);
    ShowWindow(window, SW_SHOWNORMAL);
    SetForegroundWindow(window);
    SetFocus(cancel);

    while (!dialog.closed) {
        MSG message;
        while (!dialog.closed && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (!IsDialogMessageW(window, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        if (!dialog.closed && MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                                          MWMO_INPUTAVAILABLE) != WAIT_OBJECT_0 + 1) {
            // The mod is stopping, or waiting failed. Either way, don't uninstall.
            DestroyWindow(window);
        }
    }
    return true;
}

// Returns the user's answer, or nothing if the dialog could not be shown.
std::optional<bool> ShowConfirmation(const std::wstring& title, const std::wstring& body) {
    HINSTANCE module = nullptr;
    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplus = 0;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<PCWSTR>(&ConfirmationProc), &module) ||
        Gdiplus::GdiplusStartup(&gdiplus, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        return std::nullopt;
    }
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.lpfnWndProc = ConfirmationProc;
    windowClass.hInstance = module;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kDialogClass;

    std::optional<bool> answer;
    if (RegisterClassExW(&windowClass)) {
        Confirmation dialog;
        dialog.colors = CurrentDialogColors();
        dialog.contentBrush = CreateSolidBrush(dialog.colors.content);
        // Open on the screen where the click happened.
        POINT cursor{};
        GetCursorPos(&cursor);
        MONITORINFO monitor{sizeof(monitor)};
        GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &monitor);
        const HWND window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_CONTROLPARENT, kDialogClass, L"Uninstall Here",
                                            WS_POPUP | WS_CLIPCHILDREN, monitor.rcWork.left, monitor.rcWork.top, 0, 0,
                                            nullptr, nullptr, module, &dialog);
        if (window) {
            dialog.dpi = GetDpiForWindow(window);
            const auto font = [&](int size, int weight, PCWSTR face) {
                return CreateFontW(-dialog.Scale(size), 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, face);
            };
            const HFONT titleFont = font(20, FW_SEMIBOLD, L"Segoe UI Variable Display");
            const HFONT bodyFont = font(14, FW_NORMAL, L"Segoe UI Variable Text");
            if (RunConfirmation(window, module, dialog, title, body, monitor.rcWork, titleFont, bodyFont)) {
                answer = dialog.confirmed;
            }
            if (!dialog.closed) {
                DestroyWindow(window);
            }
            DeleteObject(titleFont);
            DeleteObject(bodyFont);
        }
        DeleteObject(dialog.contentBrush);
        UnregisterClassW(kDialogClass, module);
    }
    Gdiplus::GdiplusShutdown(gdiplus);
    return answer;
}

std::optional<bool> ConfirmUninstall(const RegisteredUninstaller& uninstaller) {
    // Registry labels are displayed as single lines of a limited length, so the
    // product and publisher cannot run into the surrounding text.
    auto singleLine = [](std::wstring label) {
        for (auto& character : label) {
            if (character < L' ') {
                character = L' ';
            }
        }
        label = std::wstring(Trim(label));
        constexpr size_t limit = 160;
        if (label.size() > limit) {
            label = label.substr(0, limit) + L"…";
        }
        return label;
    };
    const auto publisher = singleLine(uninstaller.publisher);
    return ShowConfirmation(L"Uninstall \"" + singleLine(uninstaller.displayName) + L"\"?",
                            publisher.empty() ? std::wstring(L"The app's uninstaller will open.")
                                              : L"Publisher: " + publisher);
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

// Start and Search only send the clicked shortcut to this mod's copy in
// Explorer, which does the lookup, confirmation, and launch. One process draws
// the confirmation, so it looks the same everywhere. It has to be Explorer:
// Search runs in an AppContainer that can't read per-user installations or start
// an uninstaller outside its sandbox, and Windows hides the Start menu process's
// windows while Start is closed.
constexpr wchar_t kStartPackage[] = L"Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy";
constexpr wchar_t kSearchPackage[] = L"MicrosoftWindows.Client.CBS_cw5n1h2txyewy";
// The AppContainer SID of that package, the same on every PC. Deriving it with
// DeriveAppContainerSidFromAppContainerName inside the packaged Start menu
// process returns a different SID, so it's written out here.
#define SEARCH_PACKAGE_SID \
    L"S-1-15-2-283421221-3183566570-1718213290-751554359-3541592344-2312209569-3374928651"
constexpr size_t kMaxRequest = 2 * kMaxPath;  // In characters.
constexpr DWORD kPipeTimeout = 2000;
constexpr DWORD kLookupTimeout = 15000;
constexpr DWORD kListenRetry = 5000;
// Read and write, without the right to create more instances of the pipe.
constexpr DWORD kPipeClientAccess = FILE_GENERIC_READ | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES;

bool g_isExplorer;
HANDLE g_receiverThread;

std::wstring PipeName() {
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    return L"\\\\.\\pipe\\WindhawkUninstallHere.v1." + std::to_wstring(session);
}

// Returns true when Explorer takes over the uninstall.
bool ForwardToExplorer(const TileShortcut& tile) {
    std::wstring request = tile.path;
    request.push_back(L'\0');
    request += tile.arguments;
    if (request.size() > kMaxRequest) {
        return false;
    }
    const auto name = PipeName();
    winrt::file_handle pipe;
    for (int attempt = 0; attempt < 2; ++attempt) {
        pipe.attach(CreateFileW(name.c_str(), kPipeClientAccess, 0, nullptr, OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS, nullptr));
        if (pipe || GetLastError() != ERROR_PIPE_BUSY || !WaitNamedPipeW(name.c_str(), kPipeTimeout)) {
            break;
        }
    }
    if (!pipe) {
        Wh_Log(L"Could not reach Explorer. Error %u.", GetLastError());
        return false;
    }
    // Pipe names are shared by all sessions, so another signed-in user could
    // create this one first.
    ULONG serverSession = 0;
    DWORD session = 0;
    if (!GetNamedPipeServerSessionId(pipe.get(), &serverSession) ||
        !ProcessIdToSessionId(GetCurrentProcessId(), &session) || serverSession != session) {
        Wh_Log(L"The pipe isn't served from this session.");
        return false;
    }
    DWORD mode = PIPE_READMODE_MESSAGE;
    const winrt::handle completed(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!completed || !SetNamedPipeHandleState(pipe.get(), &mode, nullptr, nullptr)) {
        return false;
    }
    // Search owns the foreground. Let the receiver bring its confirmation forward.
    ULONG receiver = 0;
    if (GetNamedPipeServerProcessId(pipe.get(), &receiver)) {
        AllowSetForegroundWindow(receiver);
    }
    OVERLAPPED io{};
    io.hEvent = completed.get();
    BYTE accepted = 0;
    DWORD received = 0;
    if (!TransactNamedPipe(pipe.get(), request.data(), static_cast<DWORD>(request.size() * sizeof(wchar_t)),
                           &accepted, sizeof(accepted), nullptr, &io)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return false;
        }
        if (WaitForSingleObject(io.hEvent, kLookupTimeout) != WAIT_OBJECT_0) {
            CancelIoEx(pipe.get(), &io);
        }
    }
    return GetOverlappedResult(pipe.get(), &io, &received, TRUE) && received == sizeof(accepted) && accepted == 1;
}

// Finishes an overlapped operation for the receiver. A timeout or a stop request
// cancels it. The receiver is a COM STA, so it handles messages while waiting.
bool CompleteReceiverIo(HANDLE pipe, OVERLAPPED& io, BOOL done, DWORD timeout, DWORD& bytes) {
    if (!done) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return false;
        }
        const HANDLE handles[] = {io.hEvent, g_stopEvent};
        const ULONGLONG start = GetTickCount64();
        for (;;) {
            DWORD remaining = INFINITE;
            if (timeout != INFINITE) {
                const ULONGLONG elapsed = GetTickCount64() - start;
                remaining = elapsed < timeout ? static_cast<DWORD>(timeout - elapsed) : 0;
            }
            const DWORD result = MsgWaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, remaining, QS_ALLINPUT);
            if (result != WAIT_OBJECT_0 + ARRAYSIZE(handles)) {
                if (result != WAIT_OBJECT_0) {
                    CancelIoEx(pipe, &io);
                }
                break;
            }
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }
    return GetOverlappedResult(pipe, &io, &bytes, TRUE);
}

HANDLE CreateRequestPipe() {
    winrt::handle token;
    BYTE userBuffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
    DWORD size = 0;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.put()) ||
        !GetTokenInformation(token.get(), TokenUser, userBuffer, sizeof(userBuffer), &size)) {
        return INVALID_HANDLE_VALUE;
    }
    PWSTR userText = nullptr;
    if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(userBuffer)->User.Sid, &userText)) {
        return INVALID_HANDLE_VALUE;
    }
    // Only this user, which covers the Start menu process, and the Search package
    // get access. The low integrity label lets sandboxed Search write its request.
    wchar_t access[11]{};
    std::swprintf(access, ARRAYSIZE(access), L"0x%08x", static_cast<unsigned>(kPipeClientAccess));
    const auto rules = std::wstring(L"D:P(A;;GA;;;") + userText + L")(A;;" + access +
                       L";;;" SEARCH_PACKAGE_SID L")S:(ML;;NW;;;LW)";
    LocalFree(userText);

    SECURITY_ATTRIBUTES security{sizeof(security), nullptr, FALSE};
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(rules.c_str(), SDDL_REVISION_1,
                                                              &security.lpSecurityDescriptor, nullptr)) {
        return INVALID_HANDLE_VALUE;
    }
    const HANDLE pipe = CreateNamedPipeW(
        PipeName().c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1, sizeof(BYTE),
        64 * 1024, 0, &security);
    LocalFree(security.lpSecurityDescriptor);
    return pipe;
}

bool IsStartOrSearchClient(HANDLE pipe) {
    ULONG processId = 0;
    if (!GetNamedPipeClientProcessId(pipe, &processId)) {
        return false;
    }
    const winrt::handle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId));
    std::wstring image(kMaxPath, L'\0');
    DWORD length = static_cast<DWORD>(image.size());
    if (!process || !QueryFullProcessImageNameW(process.get(), 0, image.data(), &length)) {
        return false;
    }
    image.resize(length);
    const auto apps = EnvironmentPath(L"SystemRoot") + L"\\SystemApps\\";
    return Equals(image, apps + kStartPackage + L"\\StartMenuExperienceHost.exe") ||
           Equals(image, apps + kSearchPackage + L"\\SearchHost.exe");
}

// Reads one request and replies. Returns the uninstaller only after the sender
// has received the reply, so an abandoned request never opens a confirmation.
std::optional<RegisteredUninstaller> AnswerRequest(HANDLE pipe, HANDLE completed) {
    if (!IsStartOrSearchClient(pipe)) {
        Wh_Log(L"Ignored a request that did not come from Start or Search.");
        return std::nullopt;
    }
    std::wstring request(kMaxRequest, L'\0');
    OVERLAPPED io{};
    io.hEvent = completed;
    DWORD bytes = 0;
    BOOL done = ReadFile(pipe, request.data(), static_cast<DWORD>(request.size() * sizeof(wchar_t)), nullptr, &io);
    if (!CompleteReceiverIo(pipe, io, done, kPipeTimeout, bytes) || bytes % sizeof(wchar_t)) {
        return std::nullopt;
    }
    request.resize(bytes / sizeof(wchar_t));
    const auto separator = request.find(L'\0');
    if (separator == request.npos) {
        return std::nullopt;
    }
    auto uninstaller = FindTileUninstaller({request.substr(0, separator), request.substr(separator + 1)});

    const BYTE accepted = uninstaller.has_value();
    io = {};
    io.hEvent = completed;
    done = WriteFile(pipe, &accepted, sizeof(accepted), nullptr, &io);
    if (!CompleteReceiverIo(pipe, io, done, kPipeTimeout, bytes)) {
        return std::nullopt;
    }
    // The sender closes its end after reading. Wait for that before disconnecting.
    BYTE unused = 0;
    io = {};
    io.hEvent = completed;
    done = ReadFile(pipe, &unused, sizeof(unused), nullptr, &io);
    CompleteReceiverIo(pipe, io, done, kPipeTimeout, bytes);
    return uninstaller;
}

// Serves Start and Search until the mod stops or the pipe fails.
void ServeRequests(HANDLE pipe, HANDLE completed) {
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        OVERLAPPED io{};
        io.hEvent = completed;
        DWORD bytes = 0;
        const BOOL done = ConnectNamedPipe(pipe, &io);
        if (!(!done && GetLastError() == ERROR_PIPE_CONNECTED) &&
            !CompleteReceiverIo(pipe, io, done, INFINITE, bytes)) {
            if (GetLastError() != ERROR_NO_DATA) {
                return;
            }
            DisconnectNamedPipe(pipe);
            continue;
        }
        try {
            const auto uninstaller = AnswerRequest(pipe, completed);
            DisconnectNamedPipe(pipe);
            if (uninstaller) {
                const auto confirmed = ConfirmUninstall(*uninstaller);
                if (!confirmed || (*confirmed && !OpenUninstaller(*uninstaller))) {
                    // The sender already handed over the click, so open Installed apps from here.
                    ShellExecuteW(nullptr, L"open", L"ms-settings:appsfeatures", nullptr, nullptr, SW_SHOWNORMAL);
                }
            }
        } catch (...) {
            DisconnectNamedPipe(pipe);
            Wh_Log(L"Handling an uninstall request failed.");
        }
    }
}

DWORD WINAPI ReceiveRequests(void*) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const winrt::handle completed(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    // Only one Explorer process can own the pipe. The others keep waiting, so one
    // of them takes over if the owner exits.
    while (completed && WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        const winrt::file_handle pipe(CreateRequestPipe());
        if (pipe) {
            ServeRequests(pipe.get(), completed.get());
        }
        WaitForSingleObject(g_stopEvent, kListenRetry);
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return 0;
}

void StopReceiver() {
    if (g_receiverThread) {
        // This also cancels an open confirmation.
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_receiverThread, INFINITE);
        CloseHandle(g_receiverThread);
        g_receiverThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

HRESULT WINAPI ExecuteHook(void* self, void* args) {
    try {
        TileShortcut tile;
        {
            std::lock_guard lock(g_appMutex);
            auto it = g_apps.find(self);
            if (it != g_apps.end()) {
                tile = it->second;
            }
        }
        if (tile.path.empty()) {
            Wh_Log(L"The uninstall action has no resolved desktop tile identity.");
        } else if (ForwardToExplorer(tile)) {
            return S_OK;
        } else {
            Wh_Log(L"Explorer did not take over this uninstall.");
        }
    } catch (...) {
        Wh_Log(L"Forwarding the uninstall failed.");
    }
    return g_originalExecute(self, args);
}

}  // namespace

BOOL Wh_ModInit() {
    wchar_t process[MAX_PATH]{};
    GetModuleFileNameW(nullptr, process, ARRAYSIZE(process));
    g_isExplorer = EndsWith(process, L"\\explorer.exe");
    if (g_isExplorer) {
        // Explorer only receives requests from Start and Search. It gets no hooks.
        return TRUE;
    }
    // Keep the module loaded while Windhawk owns hooks into its code.
    g_startTileData = LoadLibraryExW(L"StartTileData.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_startTileData) {
        return FALSE;
    }
    DeleteVerb scalarDelete = nullptr;
    DeleteVerb vectorDelete = nullptr;
    WindhawkUtils::SYMBOL_HOOK startTileDataDllHooks[] = {
        {{L"public: virtual long __cdecl WindowsInternal::Shell::UnifiedTile::Private::SettingsUninstallVerb::Execute(struct WindowsInternal::Shell::UnifiedTile::IVerbExecutionArgs *)"},
         &g_originalExecute, ExecuteHook},
        {{L"public: long __cdecl WindowsInternal::Shell::UnifiedTile::Private::SettingsUninstallVerb::RuntimeClassInitialize(struct WindowsInternal::Shell::UnifiedTile::IVerbEnumerationArgs *,struct WindowsInternal::Shell::UnifiedTile::Private::IVerbStringHelperStatics *,struct HSTRING__ *,struct HSTRING__ *,bool,unsigned short const *)"},
         &g_originalInit, InitHook},
        {{L"public: virtual void * __cdecl WindowsInternal::Shell::UnifiedTile::Private::SettingsUninstallVerb::`scalar deleting destructor'(unsigned int)"},
         &scalarDelete, nullptr, true},
        {{L"public: virtual void * __cdecl WindowsInternal::Shell::UnifiedTile::Private::SettingsUninstallVerb::`vector deleting destructor'(unsigned int)"},
         &vectorDelete, nullptr, true},
        {{L"public: virtual long __cdecl DataStoreCache::AppResolverTransformer::GetVerbs(struct WindowsInternal::Shell::UnifiedTile::IUnifiedTile *,struct WindowsInternal::Shell::UnifiedTile::IVerbEnumerationArgs *,struct Windows::Foundation::Collections::IVectorView<class WindowsInternal::Shell::UnifiedTile::TileVerb *> * *)"},
         &g_originalAppResolver, AppResolverHook, true},
        {{L"public: virtual long __cdecl DataStoreCache::TileStoreTransformer::GetVerbs(struct WindowsInternal::Shell::UnifiedTile::IUnifiedTile *,struct WindowsInternal::Shell::UnifiedTile::IVerbEnumerationArgs *,struct Windows::Foundation::Collections::IVectorView<class WindowsInternal::Shell::UnifiedTile::TileVerb *> * *)"},
         &g_originalTileStore, TileStoreHook, true},
        {{L"public: virtual long __cdecl DataStoreCache::StateRepositoryTransformer::GetVerbs(struct WindowsInternal::Shell::UnifiedTile::IUnifiedTile *,struct WindowsInternal::Shell::UnifiedTile::IVerbEnumerationArgs *,struct Windows::Foundation::Collections::IVectorView<class WindowsInternal::Shell::UnifiedTile::TileVerb *> * *)"},
         &g_originalStateRepository, StateRepositoryHook, true},
    };
    const bool symbolsFound =
        WindhawkUtils::HookSymbols(g_startTileData, startTileDataDllHooks,
                                   ARRAYSIZE(startTileDataDllHooks)) &&
        (g_originalAppResolver || g_originalTileStore || g_originalStateRepository) &&
        (scalarDelete || vectorDelete);
    // Some builds give both destructor symbols the same address.
    if (!symbolsFound ||
        (scalarDelete && !WindhawkUtils::SetFunctionHook(
                             scalarDelete, DeleteHook<g_originalScalarDelete>, &g_originalScalarDelete)) ||
        (vectorDelete && vectorDelete != scalarDelete && !WindhawkUtils::SetFunctionHook(
                             vectorDelete, DeleteHook<g_originalVectorDelete>, &g_originalVectorDelete))) {
        Wh_Log(L"Could not install the uninstall hooks on this build.");
        FreeLibrary(g_startTileData);
        g_startTileData = nullptr;
        return FALSE;
    }
    Wh_Log(L"Uninstall hooks installed.");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isExplorer) {
        return;
    }
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_receiverThread = CreateThread(nullptr, 0, ReceiveRequests, nullptr, 0, nullptr);
    }
}

void Wh_ModUninit() {
    StopReceiver();
    if (g_startTileData) {
        FreeLibrary(g_startTileData);
    }
}

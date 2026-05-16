// ==WindhawkMod==
// @id              open-in-admin-terminal
// @name            Open in Admin Terminal
// @description     Adds Explorer context menu entries to open Windows Terminal elevated in the current or selected folder.
// @version         1.0
// @author          aimagist
// @github          https://github.com/aimagist
// @include         explorer.exe
// @compilerOptions -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open in Admin Terminal

This mod adds an **Open in Admin Terminal** entry to Explorer context menus by
registering per-user shell verbs while the mod is enabled.

By default it adds the entry in these places:

- Folder background in File Explorer
- Folder items
- Drive items

Notes:

- The command launches the selected terminal host elevated with UAC.
- On Windows 11, simple shell verbs may appear in the classic menu (`Show more options`)
  depending on Explorer's current context menu behavior.
- Disabling the mod removes the registry entries that it created.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- menuText: Open in Admin Terminal
  $name: Menu text
  $description: Leave empty to use a terminal-specific default label.
- terminalChoice: wt
  $name: Terminal type
  $description: Choose which terminal host to launch elevated.
  $options:
    - wt: Windows Terminal
    - pwsh: PowerShell 7
    - powershell: Windows PowerShell
    - cmd: Command Prompt
    - custom: Custom command
- customTerminalCommand: wt.exe
  $name: Custom terminal command
  $description: Used only when Terminal type is set to Custom command.
- showOnFolderBackground: true
  $name: Show on folder background
  $description: Add the entry when right-clicking empty space inside a folder.
- showOnFolderItem: true
  $name: Show on folder items
  $description: Add the entry when right-clicking a folder.
- showOnDriveItem: true
  $name: Show on drives
  $description: Add the entry when right-clicking a drive.
- position: Top
  $name: Menu position
  $description: Preferred position for the shell verb.
  $options:
    - Top: Top
    - Bottom: Bottom
    - Default: Default
- appendTerminalName: false
  $name: Append terminal name
  $description: When Menu text is set, append the selected terminal name in parentheses.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <appmodel.h>

#include <string>
#include <vector>

struct Settings {
    std::wstring menuText;
    bool appendTerminalName;
    std::wstring terminalChoice;
    std::wstring customTerminalCommand;
    std::wstring terminalDisplayCommand;
    std::wstring iconCommand;
    bool showOnFolderBackground;
    bool showOnFolderItem;
    bool showOnDriveItem;
    std::wstring position;
};

namespace {

constexpr PCWSTR kVerbName = L"Windhawk.OpenInAdminTerminal";

const std::vector<std::wstring> kManagedKeys = {
    L"Software\\Classes\\Directory\\Background\\shell\\" + std::wstring(kVerbName),
    L"Software\\Classes\\Directory\\shell\\" + std::wstring(kVerbName),
    L"Software\\Classes\\Drive\\shell\\" + std::wstring(kVerbName),
};

std::wstring GetSettingString(PCWSTR name, const wchar_t* fallback) {
    std::wstring value = fallback;
    if (PCWSTR setting = Wh_GetStringSetting(name)) {
        value = setting;
        Wh_FreeStringSetting(setting);
    }
    return value;
}

bool GetSettingBool(PCWSTR name) {
    return Wh_GetIntSetting(name) != 0 ? true : false;
}

std::wstring TrimString(const std::wstring& value) {
    const auto start = value.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return L"";
    }

    const auto end = value.find_last_not_of(L" \t\r\n");
    return value.substr(start, end - start + 1);
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::wstring ExpandEnvironmentStringsValue(const std::wstring& value) {
    DWORD required = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (required == 0) {
        return value;
    }

    std::wstring expanded(required, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(value.c_str(), expanded.data(), required);
    if (written == 0 || written > required) {
        return value;
    }

    if (!expanded.empty() && expanded.back() == L'\0') {
        expanded.pop_back();
    }

    return expanded;
}

std::wstring ResolveIconCommand(const std::wstring& preferredIconPath,
                                const std::wstring& fallbackIconPath) {
    std::wstring expandedPreferred = ExpandEnvironmentStringsValue(preferredIconPath);
    if (!expandedPreferred.empty() && FileExists(expandedPreferred)) {
        return preferredIconPath;
    }

    return fallbackIconPath;
}

std::wstring GetPackageInstallPath(PCWSTR packageFamilyName) {
    UINT32 count = 0;
    UINT32 bufferLength = 0;
    LONG rc = GetPackagesByPackageFamily(packageFamilyName,
                                         &count,
                                         nullptr,
                                         &bufferLength,
                                         nullptr);
    if (rc != ERROR_INSUFFICIENT_BUFFER || count == 0 || bufferLength == 0) {
        return L"";
    }

    std::vector<PWSTR> packageFullNames(count);
    std::vector<wchar_t> buffer(bufferLength);
    rc = GetPackagesByPackageFamily(packageFamilyName,
                                    &count,
                                    packageFullNames.data(),
                                    &bufferLength,
                                    buffer.data());
    if (rc != ERROR_SUCCESS || count == 0) {
        return L"";
    }

    for (UINT32 i = 0; i < count; ++i) {
        UINT32 pathLength = 0;
        LONG pathRc = GetPackagePathByFullName(packageFullNames[i], &pathLength, nullptr);
        if (pathRc != ERROR_INSUFFICIENT_BUFFER || pathLength == 0) {
            continue;
        }

        std::wstring packagePath(pathLength, L'\0');
        pathRc = GetPackagePathByFullName(packageFullNames[i],
                                          &pathLength,
                                          packagePath.data());
        if (pathRc != ERROR_SUCCESS) {
            continue;
        }

        if (!packagePath.empty() && packagePath.back() == L'\0') {
            packagePath.pop_back();
        }

        if (!packagePath.empty()) {
            return packagePath;
        }
    }

    return L"";
}

std::wstring GetWindowsTerminalIconPath() {
    static constexpr PCWSTR kTerminalFamilies[] = {
        L"Microsoft.WindowsTerminal_8wekyb3d8bbwe",
        L"Microsoft.WindowsTerminalPreview_8wekyb3d8bbwe",
    };

    for (PCWSTR packageFamilyName : kTerminalFamilies) {
        std::wstring packagePath = GetPackageInstallPath(packageFamilyName);
        if (packagePath.empty()) {
            continue;
        }

        std::wstring windowsTerminalExe = packagePath + L"\\WindowsTerminal.exe";
        if (FileExists(windowsTerminalExe)) {
            return windowsTerminalExe;
        }

        std::wstring terminalAppExe = packagePath + L"\\wt.exe";
        if (FileExists(terminalAppExe)) {
            return terminalAppExe;
        }
    }

    return ResolveIconCommand(L"%LOCALAPPDATA%\\Microsoft\\WindowsApps\\wt.exe",
                              L"wt.exe");
}

std::wstring GetTerminalDisplayName(const Settings& settings) {
    if (settings.terminalChoice == L"wt") {
        return L"Windows Terminal";
    }

    if (settings.terminalChoice == L"pwsh") {
        return L"PowerShell 7";
    }

    if (settings.terminalChoice == L"powershell") {
        return L"Windows PowerShell";
    }

    if (settings.terminalChoice == L"cmd") {
        return L"Command Prompt";
    }

    return L"Custom Terminal";
}

std::wstring GetDefaultMenuText(const Settings& settings) {
    return L"Open " + GetTerminalDisplayName(settings) + L" as Administrator";
}

Settings LoadSettings() {
    Settings settings;
    settings.menuText = TrimString(GetSettingString(L"menuText", L""));
    settings.appendTerminalName = GetSettingBool(L"appendTerminalName");
    settings.terminalChoice = GetSettingString(L"terminalChoice", L"wt");
    settings.customTerminalCommand =
        GetSettingString(L"customTerminalCommand", L"wt.exe");
    settings.showOnFolderBackground = GetSettingBool(L"showOnFolderBackground");
    settings.showOnFolderItem = GetSettingBool(L"showOnFolderItem");
    settings.showOnDriveItem = GetSettingBool(L"showOnDriveItem");
    settings.position = GetSettingString(L"position", L"Top");

    if (settings.terminalChoice == L"wt") {
        settings.terminalDisplayCommand = L"wt.exe";
        std::wstring windowsTerminalIconPath = GetWindowsTerminalIconPath();
        settings.iconCommand = windowsTerminalIconPath.empty()
                                   ? settings.terminalDisplayCommand
                                   : windowsTerminalIconPath;
    } else if (settings.terminalChoice == L"pwsh") {
        settings.terminalDisplayCommand = L"pwsh.exe";
        settings.iconCommand = settings.terminalDisplayCommand;
    } else if (settings.terminalChoice == L"powershell") {
        settings.terminalDisplayCommand = L"powershell.exe";
        settings.iconCommand = settings.terminalDisplayCommand;
    } else if (settings.terminalChoice == L"cmd") {
        settings.terminalDisplayCommand = L"cmd.exe";
        settings.iconCommand = settings.terminalDisplayCommand;
    } else {
        settings.terminalDisplayCommand = settings.customTerminalCommand.empty()
                                              ? L"wt.exe"
                                              : settings.customTerminalCommand;
        settings.iconCommand = settings.terminalDisplayCommand;
    }

    if (settings.menuText.empty()) {
        settings.menuText = GetDefaultMenuText(settings);
    } else if (settings.appendTerminalName) {
        settings.menuText += L" (" + GetTerminalDisplayName(settings) + L")";
    }

    return settings;
}

void LogLastError(const wchar_t* action, LONG error) {
    Wh_Log(L"%s failed with error %ld", action, error);
}

bool SetStringValue(HKEY key, PCWSTR name, const std::wstring& value) {
    const BYTE* data = reinterpret_cast<const BYTE*>(value.c_str());
    const DWORD size = static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t));
    LONG result = RegSetValueExW(key, name, 0, REG_SZ, data, size);
    if (result != ERROR_SUCCESS) {
        LogLastError(L"RegSetValueExW", result);
        return false;
    }
    return true;
}

std::wstring EscapeForSingleQuotedPowerShell(const std::wstring& input) {
    std::wstring output;
    output.reserve(input.size());

    for (wchar_t ch : input) {
        if (ch == L'\'') {
            output += L"''";
        } else {
            output += ch;
        }
    }

    return output;
}

std::wstring BuildStartProcessCommand(const std::wstring& filePath,
                                      const std::wstring& argumentListLiteral) {
    std::wstring command =
        L"powershell.exe -NoProfile -WindowStyle Hidden -Command "
        L"\"Start-Process -FilePath '";
    command += EscapeForSingleQuotedPowerShell(filePath);
    command += L"' -Verb RunAs";

    if (!argumentListLiteral.empty()) {
        command += L" -ArgumentList ";
        command += argumentListLiteral;
    }

    command += L"\"";
    return command;
}

std::wstring BuildCommand(const Settings& settings,
                          const std::wstring& targetPlaceholder) {
    if (settings.terminalChoice == L"wt") {
        return BuildStartProcessCommand(
            L"wt.exe",
            L"'-d','" + EscapeForSingleQuotedPowerShell(targetPlaceholder) + L"'");
    }

    if (settings.terminalChoice == L"pwsh") {
        return BuildStartProcessCommand(
            L"pwsh.exe",
            L"'-NoExit','-Command','Set-Location -LiteralPath ''" +
                EscapeForSingleQuotedPowerShell(targetPlaceholder) + L"'''");
    }

    if (settings.terminalChoice == L"powershell") {
        return BuildStartProcessCommand(
            L"powershell.exe",
            L"'-NoExit','-Command','Set-Location -LiteralPath ''" +
                EscapeForSingleQuotedPowerShell(targetPlaceholder) + L"'''");
    }

    if (settings.terminalChoice == L"cmd") {
        return BuildStartProcessCommand(
            L"cmd.exe",
            L"'/k','cd /d \"" + targetPlaceholder + L"\"'");
    }

    return BuildStartProcessCommand(settings.terminalDisplayCommand, L"");
}

bool CreateShellEntry(const std::wstring& keyPath,
                      const std::wstring& menuText,
                      const Settings& settings,
                      const std::wstring& targetPlaceholder,
                      const std::wstring& position) {
    HKEY key;
    DWORD disposition;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER,
                                  keyPath.c_str(),
                                  0,
                                  nullptr,
                                  0,
                                  KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
                                  nullptr,
                                  &key,
                                  &disposition);
    if (result != ERROR_SUCCESS) {
        LogLastError(L"RegCreateKeyExW", result);
        return false;
    }

    bool ok = true;
    ok &= SetStringValue(key, L"MUIVerb", menuText);
    ok &= SetStringValue(key, L"Icon", settings.iconCommand);
    ok &= SetStringValue(key, L"HasLUAShield", L"");

    if (position == L"Top" || position == L"Bottom") {
        ok &= SetStringValue(key, L"Position", position);
    } else {
        RegDeleteValueW(key, L"Position");
    }

    HKEY commandKey;
    result = RegCreateKeyExW(key,
                             L"command",
                             0,
                             nullptr,
                             0,
                             KEY_SET_VALUE,
                             nullptr,
                             &commandKey,
                             &disposition);
    if (result != ERROR_SUCCESS) {
        LogLastError(L"RegCreateKeyExW(command)", result);
        ok = false;
    } else {
        std::wstring commandLine = BuildCommand(settings, targetPlaceholder);
        ok &= SetStringValue(commandKey, nullptr, commandLine);
        RegCloseKey(commandKey);
    }

    RegCloseKey(key);
    return ok;
}

void DeleteManagedKeys() {
    for (const auto& keyPath : kManagedKeys) {
        LONG result = RegDeleteTreeW(HKEY_CURRENT_USER, keyPath.c_str());
        if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND &&
            result != ERROR_PATH_NOT_FOUND) {
            LogLastError(L"RegDeleteTreeW", result);
        }
    }
}

bool ApplySettings() {
    Settings settings = LoadSettings();
    DeleteManagedKeys();

    bool ok = true;

    if (settings.showOnFolderBackground) {
        ok &= CreateShellEntry(
            kManagedKeys[0],
            settings.menuText,
            settings,
            L"%V",
            settings.position);
    }

    if (settings.showOnFolderItem) {
        ok &= CreateShellEntry(
            kManagedKeys[1],
            settings.menuText,
            settings,
            L"%1",
            settings.position);
    }

    if (settings.showOnDriveItem) {
        ok &= CreateShellEntry(
            kManagedKeys[2],
            settings.menuText,
            settings,
            L"%1",
            settings.position);
    }

    return ok;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing %s", WH_MOD_ID);
    return ApplySettings() ? TRUE : FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing %s", WH_MOD_ID);
    DeleteManagedKeys();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
    return ApplySettings() ? TRUE : FALSE;
}

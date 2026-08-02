// ==WindhawkMod==
// @id              open-in-admin-terminal
// @name            Open in Admin Terminal
// @description     Adds an Explorer classic context menu entry to open an elevated terminal in the current or selected folder.
// @version         1.17.1
// @author          aimagist
// @github          https://github.com/aimagist
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open in Admin Terminal

**TL;DR:** Right-click any folder or drive in File Explorer and open an elevated terminal there — no more navigating after the fact.

## With this mod you can:

- Right-click a folder background and open an admin terminal in that location
- Right-click a folder item and open an admin terminal inside it
- Right-click a drive and open an admin terminal at its root
- Optionally add a second terminal entry that opens without administrator privileges
- Optionally run `.ps1`, `.bat`, `.cmd`, `.vbs`, and `.js` script files normally, as administrator, or both
- Optionally show the entry when right-clicking filesystem folders and drives in the navigation pane and Quick access
- Choose your preferred terminal: Auto, Windows Terminal, PowerShell 7, Windows PowerShell, Command Prompt, WSL, Git Bash, WezTerm, Alacritty, ConEmu, or a custom command
- Customize the context menu label, or let the mod use a smart default based on your terminal choice
- Optionally append the terminal name to a custom label (e.g. "Open elevated (Windows Terminal)")
- Choose whether script entries include the terminal name or use shorter labels

## Preview

![Normal usage — right-click folder background or folder item to open elevated Windows Terminal](https://i.imgur.com/qEgGpvc.gif)

*Right-clicking a folder item to open an elevated terminal in that location (Windows Terminal)*

![Context menu with Windows Terminal option](https://i.imgur.com/HziEs16.png) ![Context menu with PowerShell 7 option](https://i.imgur.com/CCE3vlP.png)

![Mod settings showcase](https://i.imgur.com/8KgCdju.gif)

*Settings panel — choose terminal, customize label, toggle terminal name appending*

Screenshots may show earlier builds, but current releases use runtime classic-menu injection. The mod does not write Explorer context menu entries to the registry.

## Notes

- On Windows 11, Explorer may place this entry under `Show more options` depending on your context menu setup.
- The entry is injected only while Explorer's classic menu is open; disabling the mod leaves no registry cleanup behind.
- The mod intentionally targets filesystem folders and drive roots only, including optional navigation pane and Quick access support.
- Auto chooses Windows Terminal, PowerShell 7, Windows PowerShell, then Command Prompt. If another built-in preset is unavailable, the mod falls back to Auto instead of hiding the entry.
- Script actions use the selected terminal when it can host the required Windows interpreter. WSL, Git Bash, and custom commands fall back to PowerShell, Command Prompt, or Windows Script Host and use that actual program name in the menu.
- Diagnostics use Windhawk's built-in logging controls.

## Version log
- 1.17.1: Fix navigation pane menu not appearing: the entry was missing when right-clicking folders or drives in File Explorer’s left sidebar and Quick access. Now it works :)
- 1.17: Added optional non-elevated terminal entries, configurable normal/elevated script actions (.ps1/.bat/.cmd/.vbs/.js), and shorter script labels. The two extra entry features are opt-in and disabled by default; terminal names remain shown in script entries by default for compatibility.
- 1.16: Added native UAC shield overlay composition on the terminal menu icon using `SHGetStockIconInfo(SIID_SHIELD)`.
- 1.15: Added optional navigation pane and Quick access support for filesystem folders and drives.
- 1.14: Added support for Desktop context menu targets.
- 1.13: Fixed elevated terminal launches for folder paths containing spaces.
- 1.12: Added Auto fallback and WSL, Git Bash, WezTerm, Alacritty, and ConEmu terminal presets.
- 1.11: Fixed Windows Terminal menu icon lookup when wt.exe is an app execution alias.
- 1.10: Reduced Explorer menu-open work by limiting selection path reads and caching terminal icon lookup successes and failures.
- 1.9: Added quiet-by-default debug logging, improved menu placement, tightened filesystem target eligibility, and refreshed docs for classic-menu runtime injection.
- 1.8: Switched to direct Explorer classic-menu injection with no persistent registry writes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- menuText: Open in Admin Terminal
  $name: Menu text
  $description: Leave empty to use a terminal-specific default label.
- terminalChoice: auto
  $name: Terminal type
  $description: Choose which terminal host to launch elevated. If the selected terminal is unavailable, the mod falls back to Auto mode instead of hiding the entry.
  $options:
    - auto: Auto
    - wt: Windows Terminal
    - pwsh: PowerShell 7
    - powershell: Windows PowerShell
    - cmd: Command Prompt
    - wsl: WSL
    - gitbash: Git Bash
    - wezterm: WezTerm
    - alacritty: Alacritty
    - conemu: ConEmu
    - custom: Custom command
- customTerminalCommand: wt.exe
  $name: Custom terminal command
  $description: Used only when Terminal type is set to Custom command. Use %V or %1 in arguments to insert the selected folder.
- showOnFolderBackground: true
  $name: Show on folder background
  $description: Add the entry when right-clicking empty space inside a folder.
- showOnFolderItem: true
  $name: Show on folder items
  $description: Add the entry when right-clicking a folder.
- showOnDriveItem: true
  $name: Show on drives
  $description: Add the entry when right-clicking a drive.
- showOnNavigationPane: false
  $name: Show in navigation pane
  $description: Add the entry when right-clicking filesystem folders and drives in Explorer's navigation pane and Quick access.
- position: Top
  $name: Menu position
  $description: Preferred position in the classic context menu.
  $options:
    - Top: Top
    - Bottom: Bottom
    - Default: Default
- appendTerminalName: false
  $name: Append terminal name
  $description: When Menu text is set, append the selected terminal name in parentheses.
- showNonElevatedEntry: false
  $name: Show non-elevated entry
  $description: Add a second context menu entry to open a terminal without admin privileges.
- nonElevatedMenuText: ""
  $name: Non-elevated menu text
  $description: Leave empty for auto label based on terminal name.
- showOnScriptFiles: false
  $name: Show on script files
  $description: Add context menu entries when right-clicking supported script files.
- scriptMenuEntries: admin
  $name: Script menu entries
  $description: Choose which actions to show for script files.
  $options:
    - admin: Run as administrator
    - normal: Run normally
    - both: Show both
- appendScriptTerminalName: true
  $name: Append terminal name to script entries
  $description: Add the selected terminal name to script actions. Disable for shorter labels.
- scriptExtensions: ".ps1;.bat;.cmd;.vbs;.js"
  $name: Script extensions
  $description: Semicolon-separated filter for supported script extensions: .ps1, .bat, .cmd, .vbs, and .js.
- keepOpenAfterScript: true
  $name: Keep terminal open after script
  $description: Terminal stays open after script finishes.
- scriptExecutionPolicyBypass: false
  $name: Bypass execution policy for .ps1
  $description: Adds -ExecutionPolicy Bypass for PowerShell scripts. Disabled by default.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <ShObjIdl.h>
#include <shlwapi.h>
#include <exdisp.h>
#include <shellapi.h>
#include <winioctl.h>

#include <string>
#include <utility>
#include <vector>

struct Settings {
    std::wstring menuText;
    bool appendTerminalName;
    std::wstring terminalChoice;
    std::wstring customTerminalCommand;
    std::wstring terminalEffectiveChoice;
    std::wstring terminalDisplayCommand;
    bool terminalUsedFallback;
    bool showOnFolderBackground;
    bool showOnFolderItem;
    bool showOnDriveItem;
    bool showOnNavigationPane;
    std::wstring position;
    bool showNonElevatedEntry;
    std::wstring nonElevatedMenuText;
    bool showOnScriptFiles;
    std::wstring scriptMenuEntries;
    bool appendScriptTerminalName;
    std::wstring scriptExtensions;
    bool keepOpenAfterScript;
    bool scriptExecutionPolicyBypass;
};

enum class TargetKind {
    None,
    FolderBackground,
    FolderItem,
    DriveItem,
    ScriptItem,
};

struct MenuTarget {
    TargetKind kind = TargetKind::None;
    std::wstring path;
};

struct LaunchSpec {
    std::wstring executable;
    std::wstring parameters;
    std::wstring workingDirectory;
};

static Settings GetSettingsSnapshot();
static void LaunchTerminalNonElevated(const MenuTarget&);
static LaunchSpec BuildScriptLaunchSpec(const Settings&, const std::wstring&);
static bool IsScriptExtension(const std::wstring&, const Settings&);
static bool IsShellViewWindow(HWND hwnd);

static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static const UINT kMenuCommandId = 0xBF31;
static const UINT kMenuCommandIdNonElevated = 0xBF32;

#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

#ifndef MAXIMUM_REPARSE_DATA_BUFFER_SIZE
#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE (16 * 1024)
#endif

static thread_local HWND g_currentMenuHwnd = nullptr;
static thread_local bool g_currentMenuEligible = false;
static thread_local MenuTarget g_currentTarget;

struct MenuBitmapCacheEntry {
    std::wstring key;
    HBITMAP bitmap;
};

static CLIPFORMAT g_shellIdListClipboardFormat = 0;
static SRWLOCK g_menuBitmapLock = SRWLOCK_INIT;
static std::vector<MenuBitmapCacheEntry> g_menuBitmapCache;

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Orig;

using PostMessageW_t = BOOL(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
static PostMessageW_t PostMessageW_Orig;

static std::wstring GetSettingString(PCWSTR name, const wchar_t* fallback) {
    std::wstring value = fallback;
    if (PCWSTR s = Wh_GetStringSetting(name)) {
        value = s;
        Wh_FreeStringSetting(s);
    }
    return value;
}

static bool GetSettingBool(PCWSTR name) {
    return Wh_GetIntSetting(name) != 0;
}

static std::wstring TrimString(const std::wstring& v) {
    auto first = v.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return {};
    }

    auto last = v.find_last_not_of(L" \t\r\n");
    return v.substr(first, last - first + 1);
}

static std::wstring TrimQuotes(const std::wstring& v) {
    if (v.size() >= 2 && v.front() == L'"' && v.back() == L'"') {
        return v.substr(1, v.size() - 2);
    }
    return v;
}

static bool IsFilePath(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool GetEnvironmentString(PCWSTR name, std::wstring& value) {
    value.clear();

    DWORD required = GetEnvironmentVariableW(name, nullptr, 0);
    if (required == 0) {
        return false;
    }

    std::vector<wchar_t> buffer(required);
    DWORD copied = GetEnvironmentVariableW(name, buffer.data(),
                                           static_cast<DWORD>(buffer.size()));
    if (copied == 0 || copied >= buffer.size()) {
        return false;
    }

    value.assign(buffer.data(), copied);
    return true;
}

static void AddEnvironmentPath(std::vector<std::wstring>& candidates,
                               PCWSTR envName,
                               PCWSTR relativePath) {
    std::wstring base;
    if (!GetEnvironmentString(envName, base) || base.empty()) {
        return;
    }

    if (base.back() != L'\\' && base.back() != L'/') {
        base += L"\\";
    }
    candidates.push_back(base + relativePath);
}

static bool SearchExecutablePath(PCWSTR exe, std::wstring& exeOut) {
    exeOut.clear();

    WCHAR resolved[MAX_PATH] = {};
    DWORD len = SearchPathW(nullptr, exe, nullptr, ARRAYSIZE(resolved), resolved,
                            nullptr);
    if (len > 0 && len < ARRAYSIZE(resolved)) {
        exeOut = resolved;
        return true;
    }

    if (len >= ARRAYSIZE(resolved)) {
        std::vector<wchar_t> buffer(len + 1);
        len = SearchPathW(nullptr, exe, nullptr,
                          static_cast<DWORD>(buffer.size()), buffer.data(),
                          nullptr);
        if (len > 0 && len < buffer.size()) {
            exeOut = buffer.data();
            return true;
        }
    }

    return false;
}

static bool ResolveExecutableCommand(PCWSTR exe,
                                     const std::vector<std::wstring>& candidates,
                                     std::wstring& commandOut) {
    if (SearchExecutablePath(exe, commandOut)) {
        return true;
    }

    for (const auto& candidate : candidates) {
        if (IsFilePath(candidate)) {
            commandOut = candidate;
            return true;
        }
    }

    return false;
}

static bool ResolveTerminalChoiceExecutable(const std::wstring& choice,
                                            std::wstring& commandOut) {
    if (choice == L"wt") {
        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Microsoft\\WindowsApps\\wt.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Microsoft\\WindowsApps\\"
                           L"Microsoft.WindowsTerminal_8wekyb3d8bbwe\\wt.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Microsoft\\WindowsApps\\"
                           L"Microsoft.WindowsTerminalPreview_8wekyb3d8bbwe\\wt.exe");
        return ResolveExecutableCommand(L"wt.exe", candidates, commandOut);
    }
    if (choice == L"pwsh") {
        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"ProgramFiles", L"PowerShell\\7\\pwsh.exe");
        AddEnvironmentPath(candidates, L"ProgramFiles(x86)",
                           L"PowerShell\\7\\pwsh.exe");
        return ResolveExecutableCommand(L"pwsh.exe", candidates, commandOut);
    }
    if (choice == L"powershell") {
        return ResolveExecutableCommand(L"powershell.exe", {}, commandOut);
    }
    if (choice == L"cmd") {
        return ResolveExecutableCommand(L"cmd.exe", {}, commandOut);
    }
    if (choice == L"wsl") {
        return ResolveExecutableCommand(L"wsl.exe", {}, commandOut);
    }
    if (choice == L"gitbash") {
        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"ProgramFiles", L"Git\\git-bash.exe");
        AddEnvironmentPath(candidates, L"ProgramFiles(x86)", L"Git\\git-bash.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Programs\\Git\\git-bash.exe");
        return ResolveExecutableCommand(L"git-bash.exe", candidates, commandOut);
    }
    if (choice == L"wezterm") {
        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"ProgramFiles", L"WezTerm\\wezterm.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Programs\\WezTerm\\wezterm.exe");
        return ResolveExecutableCommand(L"wezterm.exe", candidates, commandOut);
    }
    if (choice == L"alacritty") {
        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"ProgramFiles",
                           L"Alacritty\\alacritty.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Programs\\Alacritty\\alacritty.exe");
        return ResolveExecutableCommand(L"alacritty.exe", candidates, commandOut);
    }
    if (choice == L"conemu") {
        std::vector<std::wstring> candidates64;
        AddEnvironmentPath(candidates64, L"ProgramFiles",
                           L"ConEmu\\ConEmu64.exe");
        AddEnvironmentPath(candidates64, L"ProgramFiles(x86)",
                           L"ConEmu\\ConEmu64.exe");
        AddEnvironmentPath(candidates64, L"LocalAppData",
                           L"Programs\\ConEmu\\ConEmu64.exe");
        if (ResolveExecutableCommand(L"ConEmu64.exe", candidates64, commandOut)) {
            return true;
        }

        std::vector<std::wstring> candidates;
        AddEnvironmentPath(candidates, L"ProgramFiles", L"ConEmu\\ConEmu.exe");
        AddEnvironmentPath(candidates, L"ProgramFiles(x86)",
                           L"ConEmu\\ConEmu.exe");
        AddEnvironmentPath(candidates, L"LocalAppData",
                           L"Programs\\ConEmu\\ConEmu.exe");
        return ResolveExecutableCommand(L"ConEmu.exe", candidates, commandOut);
    }

    return false;
}

static bool ResolveAutoTerminal(std::wstring& choiceOut,
                                std::wstring& commandOut) {
    static constexpr PCWSTR kAutoChoices[] = {
        L"wt",
        L"pwsh",
        L"powershell",
        L"cmd",
    };

    for (PCWSTR choice : kAutoChoices) {
        if (ResolveTerminalChoiceExecutable(choice, commandOut)) {
            choiceOut = choice;
            return true;
        }
    }

    return false;
}

static void ResolveSettingsTerminal(Settings& s) {
    s.terminalChoice = TrimString(s.terminalChoice);
    if (s.terminalChoice.empty()) {
        s.terminalChoice = L"auto";
    }

    s.terminalEffectiveChoice.clear();
    s.terminalDisplayCommand.clear();
    s.terminalUsedFallback = false;

    if (s.terminalChoice == L"custom") {
        s.terminalDisplayCommand = TrimQuotes(TrimString(s.customTerminalCommand));
        if (!s.terminalDisplayCommand.empty()) {
            s.terminalEffectiveChoice = L"custom";
            return;
        }

        s.terminalUsedFallback = true;
    } else if (s.terminalChoice == L"auto") {
        if (ResolveAutoTerminal(s.terminalEffectiveChoice,
                                s.terminalDisplayCommand)) {
            return;
        }
    } else if (ResolveTerminalChoiceExecutable(s.terminalChoice,
                                               s.terminalDisplayCommand)) {
        s.terminalEffectiveChoice = s.terminalChoice;
        return;
    } else {
        s.terminalUsedFallback = true;
    }

    if (ResolveAutoTerminal(s.terminalEffectiveChoice, s.terminalDisplayCommand)) {
        return;
    }

    s.terminalEffectiveChoice = L"cmd";
    s.terminalDisplayCommand = L"cmd.exe";
}

static std::wstring GetTerminalDisplayNameForChoice(const std::wstring& choice) {
    if (choice == L"wt") {
        return L"Windows Terminal";
    }
    if (choice == L"pwsh") {
        return L"PowerShell 7";
    }
    if (choice == L"powershell") {
        return L"Windows PowerShell";
    }
    if (choice == L"cmd") {
        return L"Command Prompt";
    }
    if (choice == L"wsl") {
        return L"WSL";
    }
    if (choice == L"gitbash") {
        return L"Git Bash";
    }
    if (choice == L"wezterm") {
        return L"WezTerm";
    }
    if (choice == L"alacritty") {
        return L"Alacritty";
    }
    if (choice == L"conemu") {
        return L"ConEmu";
    }
    if (choice == L"custom") {
        return L"Custom Terminal";
    }
    return L"Terminal";
}

static std::wstring GetTerminalDisplayName(const Settings& s) {
    if (!s.terminalEffectiveChoice.empty()) {
        return GetTerminalDisplayNameForChoice(s.terminalEffectiveChoice);
    }
    if (s.terminalChoice == L"auto") {
        return L"Terminal";
    }
    return L"Custom Terminal";
}

static Settings LoadSettings() {
    Settings s;
    s.menuText = TrimString(GetSettingString(L"menuText", L""));
    s.appendTerminalName = GetSettingBool(L"appendTerminalName");
    s.terminalChoice = GetSettingString(L"terminalChoice", L"auto");
    s.customTerminalCommand = GetSettingString(L"customTerminalCommand", L"wt.exe");
    s.showOnFolderBackground = GetSettingBool(L"showOnFolderBackground");
    s.showOnFolderItem = GetSettingBool(L"showOnFolderItem");
    s.showOnDriveItem = GetSettingBool(L"showOnDriveItem");
    s.showOnNavigationPane = GetSettingBool(L"showOnNavigationPane");
    s.position = GetSettingString(L"position", L"Top");
    s.showNonElevatedEntry = GetSettingBool(L"showNonElevatedEntry");
    s.nonElevatedMenuText = GetSettingString(L"nonElevatedMenuText", L"");
    s.showOnScriptFiles = GetSettingBool(L"showOnScriptFiles");
    s.scriptMenuEntries = GetSettingString(L"scriptMenuEntries", L"admin");
    s.appendScriptTerminalName = GetSettingBool(L"appendScriptTerminalName");
    s.scriptExtensions = GetSettingString(L"scriptExtensions", L".ps1;.bat;.cmd;.vbs;.js");
    s.keepOpenAfterScript = GetSettingBool(L"keepOpenAfterScript");
    s.scriptExecutionPolicyBypass = GetSettingBool(L"scriptExecutionPolicyBypass");

    ResolveSettingsTerminal(s);

    if (s.menuText.empty()) {
        s.menuText = L"Open " + GetTerminalDisplayName(s) + L" as Administrator";
    } else if (s.appendTerminalName) {
        s.menuText += L" (" + GetTerminalDisplayName(s) + L")";
    }

    return s;
}

static bool IsTargetEnabled(const Settings& s, TargetKind kind) {
    if (kind == TargetKind::FolderBackground) {
        return s.showOnFolderBackground;
    }
    if (kind == TargetKind::FolderItem) {
        return s.showOnFolderItem;
    }
    if (kind == TargetKind::DriveItem) {
        return s.showOnDriveItem;
    }
    if (kind == TargetKind::ScriptItem) {
        return s.showOnScriptFiles;
    }
    return false;
}

static bool ResolveSystemExecutablePath(PCWSTR exe, std::wstring& pathOut) {
    pathOut.clear();

    WCHAR systemDirectory[MAX_PATH] = {};
    UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (!length || length >= ARRAYSIZE(systemDirectory)) {
        return false;
    }

    pathOut.assign(systemDirectory, length);
    pathOut += L'\\';
    pathOut += exe;
    if (!IsFilePath(pathOut)) {
        pathOut.clear();
        return false;
    }

    return true;
}

static PCWSTR TargetKindName(TargetKind kind) {
    if (kind == TargetKind::FolderBackground) {
        return L"folder-background";
    }
    if (kind == TargetKind::FolderItem) {
        return L"folder-item";
    }
    if (kind == TargetKind::DriveItem) {
        return L"drive-item";
    }
    if (kind == TargetKind::ScriptItem) {
        return L"script-item";
    }
    return L"none";
}

static bool IsDirectoryPath(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool IsScriptExtension(const std::wstring& path, const Settings& s) {
    PCWSTR ext = PathFindExtensionW(path.c_str());
    if (!ext || !*ext) {
        return false;
    }

    if (_wcsicmp(ext, L".ps1") != 0 && _wcsicmp(ext, L".bat") != 0 &&
        _wcsicmp(ext, L".cmd") != 0 && _wcsicmp(ext, L".vbs") != 0 &&
        _wcsicmp(ext, L".js") != 0) {
        return false;
    }

    size_t start = 0;
    while (start <= s.scriptExtensions.size()) {
        size_t end = s.scriptExtensions.find(L';', start);
        std::wstring token = TrimString(s.scriptExtensions.substr(start, end - start));
        if (_wcsicmp(token.c_str(), ext) == 0) {
            return true;
        }
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    return false;
}

static bool IsDriveRootPath(const std::wstring& path) {
    return PathIsRootW(path.c_str()) != FALSE;
}

static std::wstring QuoteCommandLineArgument(const std::wstring& arg) {
    if (arg.empty()) {
        return L"\"\"";
    }

    if (arg.find_first_of(L" \t\r\n\"") == std::wstring::npos) {
        return arg;
    }

    std::wstring result;
    result.reserve(arg.size() + 2);
    result += L'"';

    size_t backslashes = 0;
    for (wchar_t c : arg) {
        if (c == L'\\') {
            backslashes++;
            continue;
        }

        if (c == L'"') {
            result.append(backslashes * 2 + 1, L'\\');
            result += c;
            backslashes = 0;
            continue;
        }

        if (backslashes > 0) {
            result.append(backslashes, L'\\');
            backslashes = 0;
        }
        result += c;
    }

    if (backslashes > 0) {
        result.append(backslashes * 2, L'\\');
    }
    result += L'"';
    return result;
}

static std::wstring JoinCommandLineArguments(
    const std::vector<std::wstring>& args) {
    std::wstring result;
    for (const auto& arg : args) {
        if (!result.empty()) {
            result += L' ';
        }
        result += QuoteCommandLineArgument(arg);
    }
    return result;
}

static std::wstring EscapePowerShellSingleQuotedString(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t c : s) {
        if (c == L'\'') {
            out += L"''";
        } else {
            out += c;
        }
    }
    return out;
}

static std::wstring BuildSetLocationCommand(const std::wstring& target) {
    return L"Set-Location -LiteralPath '" +
           EscapePowerShellSingleQuotedString(target) + L"'";
}

static bool SplitCustomCommand(const std::wstring& command,
                               std::wstring& executable,
                               std::wstring& parameters) {
    executable.clear();
    parameters.clear();

    std::wstring trimmed = TrimString(command);
    if (trimmed.empty()) {
        return false;
    }

    if (trimmed.front() == L'"') {
        size_t endQuote = trimmed.find(L'"', 1);
        if (endQuote == std::wstring::npos) {
            executable = TrimString(trimmed.substr(1));
        } else {
            executable = trimmed.substr(1, endQuote - 1);
            parameters = TrimString(trimmed.substr(endQuote + 1));
        }
    } else {
        size_t split = trimmed.find_first_of(L" \t\r\n");
        if (split == std::wstring::npos) {
            executable = trimmed;
        } else {
            executable = trimmed.substr(0, split);
            parameters = TrimString(trimmed.substr(split + 1));
        }
    }

    executable = TrimString(executable);
    return !executable.empty();
}

static void ReplaceAll(std::wstring& value,
                       const std::wstring& placeholder,
                       const std::wstring& replacement) {
    size_t pos = 0;
    while ((pos = value.find(placeholder, pos)) != std::wstring::npos) {
        value.replace(pos, placeholder.size(), replacement);
        pos += replacement.size();
    }
}

static std::wstring ExpandCustomParameters(const std::wstring& parameters,
                                           const std::wstring& target) {
    std::wstring result = parameters;
    ReplaceAll(result, L"%V", target);
    ReplaceAll(result, L"%1", target);
    return result;
}

static LaunchSpec BuildLaunchSpec(const Settings& s, const std::wstring& target) {
    const std::wstring& choice = s.terminalEffectiveChoice.empty()
                                     ? s.terminalChoice
                                     : s.terminalEffectiveChoice;
    LaunchSpec spec;
    spec.executable =
        s.terminalDisplayCommand.empty() ? L"cmd.exe" : s.terminalDisplayCommand;
    spec.workingDirectory = target;

    if (choice == L"custom") {
        std::wstring executable;
        std::wstring parameters;
        if (SplitCustomCommand(s.customTerminalCommand, executable, parameters)) {
            spec.executable = executable;
            spec.parameters = ExpandCustomParameters(parameters, target);
        }
        return spec;
    }

    if (choice == L"wt") {
        spec.parameters = JoinCommandLineArguments({L"-d", target});
        return spec;
    }
    if (choice == L"pwsh") {
        spec.parameters = JoinCommandLineArguments(
            {L"-NoExit", L"-Command", BuildSetLocationCommand(target)});
        return spec;
    }
    if (choice == L"powershell") {
        spec.parameters = JoinCommandLineArguments(
            {L"-NoExit", L"-Command", BuildSetLocationCommand(target)});
        return spec;
    }
    if (choice == L"cmd") {
        spec.parameters = L"/k cd /d " + QuoteCommandLineArgument(target);
        return spec;
    }
    if (choice == L"wsl") {
        spec.parameters = JoinCommandLineArguments({L"--cd", target});
        return spec;
    }
    if (choice == L"gitbash") {
        return spec;
    }
    if (choice == L"wezterm") {
        spec.parameters = JoinCommandLineArguments({L"start", L"--cwd", target});
        return spec;
    }
    if (choice == L"alacritty") {
        spec.parameters =
            JoinCommandLineArguments({L"--working-directory", target});
        return spec;
    }
    if (choice == L"conemu") {
        spec.parameters = JoinCommandLineArguments({L"-Dir", target});
        return spec;
    }

    return spec;
}

static LaunchSpec BuildScriptInterpreterSpec(const Settings& s,
                                             const std::wstring& scriptPath) {
    LaunchSpec spec;
    PCWSTR ext = PathFindExtensionW(scriptPath.c_str());
    size_t separator = scriptPath.find_last_of(L"\\/");
    spec.workingDirectory = scriptPath.substr(
        0, separator == 2 && scriptPath.size() > 2 && scriptPath[1] == L':'
               ? separator + 1
               : separator);

    if (_wcsicmp(ext, L".ps1") == 0) {
        if (s.terminalEffectiveChoice == L"pwsh" ||
            s.terminalEffectiveChoice == L"powershell") {
            spec.executable = s.terminalDisplayCommand;
        } else if (!ResolveTerminalChoiceExecutable(L"pwsh", spec.executable)) {
            ResolveTerminalChoiceExecutable(L"powershell", spec.executable);
        }
        std::vector<std::wstring> args;
        if (s.keepOpenAfterScript) {
            args.push_back(L"-NoExit");
        }
        if (s.scriptExecutionPolicyBypass) {
            args.push_back(L"-ExecutionPolicy");
            args.push_back(L"Bypass");
        }
        args.push_back(L"-File");
        args.push_back(scriptPath);
        spec.parameters = JoinCommandLineArguments(args);
    } else if (_wcsicmp(ext, L".bat") == 0 ||
               _wcsicmp(ext, L".cmd") == 0) {
        ResolveSystemExecutablePath(L"cmd.exe", spec.executable);
        spec.parameters =
            std::wstring(s.keepOpenAfterScript ? L"/k " : L"/c ") +
            QuoteCommandLineArgument(scriptPath);
    } else if (_wcsicmp(ext, L".vbs") == 0 ||
               _wcsicmp(ext, L".js") == 0) {
        std::wstring cscriptPath;
        if (!ResolveSystemExecutablePath(L"cscript.exe", cscriptPath)) {
            return spec;
        }
        if (s.keepOpenAfterScript) {
            if (!ResolveSystemExecutablePath(L"cmd.exe", spec.executable)) {
                return {};
            }
            spec.parameters = L"/k " + QuoteCommandLineArgument(cscriptPath) +
                              L" //nologo " + QuoteCommandLineArgument(scriptPath);
        } else {
            spec.executable = std::move(cscriptPath);
            spec.parameters = L"//nologo " + QuoteCommandLineArgument(scriptPath);
        }
    }
    return spec;
}

static bool IsScriptHostChoice(const std::wstring& choice) {
    return choice == L"wt" || choice == L"wezterm" ||
           choice == L"alacritty" || choice == L"conemu";
}

static LaunchSpec BuildScriptLaunchSpec(const Settings& s,
                                        const std::wstring& scriptPath) {
    LaunchSpec interpreter = BuildScriptInterpreterSpec(s, scriptPath);
    if (!IsScriptHostChoice(s.terminalEffectiveChoice)) {
        return interpreter;
    }

    LaunchSpec spec;
    spec.executable = s.terminalDisplayCommand;
    spec.workingDirectory = interpreter.workingDirectory;
    std::vector<std::wstring> args;

    if (s.terminalEffectiveChoice == L"wt") {
        args = {L"new-tab", L"-d", interpreter.workingDirectory,
                interpreter.executable};
    } else if (s.terminalEffectiveChoice == L"wezterm") {
        args = {L"start", L"--cwd", interpreter.workingDirectory, L"--",
                interpreter.executable};
    } else if (s.terminalEffectiveChoice == L"alacritty") {
        args = {L"--working-directory", interpreter.workingDirectory, L"-e",
                interpreter.executable};
    } else {
        args = {L"-Dir", interpreter.workingDirectory, L"-run",
                interpreter.executable};
    }

    spec.parameters = JoinCommandLineArguments(args);
    if (!interpreter.parameters.empty()) {
        spec.parameters += L" " + interpreter.parameters;
    }
    return spec;
}

static IServiceProvider* GetExplorerServiceProviderForHwnd(HWND topLevel) {
    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_IShellWindows,
                                  reinterpret_cast<void**>(&shellWindows));
    if (FAILED(hr) || !shellWindows) {
        return nullptr;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    IServiceProvider* result = nullptr;
    for (long i = 0; i < count && !result; i++) {
        VARIANT index = {};
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* dispatch = nullptr;
        if (FAILED(shellWindows->Item(index, &dispatch)) || !dispatch) {
            continue;
        }

        IWebBrowserApp* browser = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_IWebBrowserApp,
                                               reinterpret_cast<void**>(&browser))) &&
            browser) {
            HWND browserHwnd = nullptr;
            browser->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&browserHwnd));
            if (browserHwnd == topLevel) {
                browser->QueryInterface(IID_IServiceProvider,
                                        reinterpret_cast<void**>(&result));
            }
            browser->Release();
        }
        dispatch->Release();
    }

    shellWindows->Release();
    return result;
}

static IShellBrowser* GetShellBrowserForHwnd(HWND topLevel) {
    IServiceProvider* serviceProvider = GetExplorerServiceProviderForHwnd(topLevel);
    if (!serviceProvider) {
        return nullptr;
    }

    IShellBrowser* result = nullptr;
    serviceProvider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                  reinterpret_cast<void**>(&result));
    serviceProvider->Release();
    return result;
}

static IShellView* GetActiveShellViewForHwnd(HWND topLevel) {
    IShellBrowser* shellBrowser = GetShellBrowserForHwnd(topLevel);
    if (!shellBrowser) {
        return nullptr;
    }

    IShellView* result = nullptr;
    shellBrowser->QueryActiveShellView(&result);
    shellBrowser->Release();
    return result;
}

static bool IsDesktopShellViewWindow(HWND hwnd) {
    bool sawShellView = false;
    HWND w = hwnd;
    while (w) {
        WCHAR className[64] = {};
        GetClassNameW(w, className, ARRAYSIZE(className));
        if (_wcsicmp(className, L"SHELLDLL_DefView") == 0) {
            sawShellView = true;
        } else if (sawShellView &&
                   (_wcsicmp(className, L"Progman") == 0 ||
                    _wcsicmp(className, L"WorkerW") == 0)) {
            return true;
        }

        HWND parent = GetParent(w);
        if (!parent) {
            parent = GetWindow(w, GW_OWNER);
        }
        w = parent;
    }
    return false;
}

static IShellView* GetDesktopShellView() {
    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_IShellWindows,
                                  reinterpret_cast<void**>(&shellWindows));
    if (FAILED(hr) || !shellWindows) {
        return nullptr;
    }

    VARIANT location = {};
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty = {};
    long hwnd = 0;
    IDispatch* dispatch = nullptr;
    IShellView* result = nullptr;
    if (SUCCEEDED(shellWindows->FindWindowSW(
            &location, &empty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH,
            &dispatch)) &&
        dispatch) {
        IServiceProvider* serviceProvider = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(
                IID_IServiceProvider,
                reinterpret_cast<void**>(&serviceProvider))) &&
            serviceProvider) {
            IShellBrowser* shellBrowser = nullptr;
            if (SUCCEEDED(serviceProvider->QueryService(
                    SID_STopLevelBrowser,
                    IID_IShellBrowser,
                    reinterpret_cast<void**>(&shellBrowser))) &&
                shellBrowser) {
                shellBrowser->QueryActiveShellView(&result);
                shellBrowser->Release();
            }
            serviceProvider->Release();
        }
        dispatch->Release();
    }

    shellWindows->Release();
    return result;
}

static bool IsDesktopFolderPidl(LPCITEMIDLIST pidl) {
    LPITEMIDLIST desktopPidl = nullptr;
    bool result = false;
    if (SUCCEEDED(SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP,
                                             &desktopPidl)) &&
        desktopPidl) {
        result = ILIsEqual(pidl, desktopPidl) != FALSE;
        CoTaskMemFree(desktopPidl);
    }
    return result;
}

static bool GetDesktopFolderPath(std::wstring& folderOut) {
    folderOut.clear();

    PWSTR path = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path)) ||
        !path) {
        return false;
    }

    folderOut = path;
    CoTaskMemFree(path);
    return !folderOut.empty();
}

static bool GetCurrentFolderPath(IFolderView* folderView, std::wstring& folderOut) {
    folderOut.clear();

    IPersistFolder2* persistFolder = nullptr;
    if (FAILED(folderView->GetFolder(IID_IPersistFolder2,
                                     reinterpret_cast<void**>(&persistFolder))) ||
        !persistFolder) {
        return false;
    }

    LPITEMIDLIST folderPidl = nullptr;
    bool ok = false;
    if (SUCCEEDED(persistFolder->GetCurFolder(&folderPidl)) && folderPidl) {
        WCHAR path[MAX_PATH] = {};
        if (SHGetPathFromIDListW(folderPidl, path)) {
            folderOut = path;
            ok = true;
        } else if (IsDesktopFolderPidl(folderPidl)) {
            ok = GetDesktopFolderPath(folderOut);
        }
        CoTaskMemFree(folderPidl);
    }

    persistFolder->Release();
    return ok;
}

static UINT GetSelectedPaths(IShellView* shellView,
                             std::vector<std::wstring>& selectionOut,
                             size_t maxPaths) {
    selectionOut.clear();
    if (maxPaths == 0 || !g_shellIdListClipboardFormat) {
        return 0;
    }

    IDataObject* dataObject = nullptr;
    if (FAILED(shellView->GetItemObject(SVGIO_SELECTION, IID_IDataObject,
                                        reinterpret_cast<void**>(&dataObject))) ||
        !dataObject) {
        return 0;
    }

    FORMATETC format = {
        g_shellIdListClipboardFormat,
        nullptr,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL,
    };
    STGMEDIUM medium = {};
    UINT selectedCount = 0;
    if (SUCCEEDED(dataObject->GetData(&format, &medium))) {
        CIDA* cida = static_cast<CIDA*>(GlobalLock(medium.hGlobal));
        if (cida) {
            selectedCount = cida->cidl;
            LPCITEMIDLIST parent =
                reinterpret_cast<LPCITEMIDLIST>(
                    reinterpret_cast<BYTE*>(cida) + cida->aoffset[0]);
            LPITEMIDLIST parentAbs = ILClone(parent);
            IShellFolder* folder = nullptr;
            if (parentAbs &&
                SUCCEEDED(SHBindToObject(nullptr, parentAbs, nullptr,
                                         IID_IShellFolder,
                                         reinterpret_cast<void**>(&folder))) &&
                folder) {
                UINT pathsToRead = cida->cidl;
                if (static_cast<size_t>(pathsToRead) > maxPaths) {
                    pathsToRead = static_cast<UINT>(maxPaths);
                }
                for (UINT i = 0; i < pathsToRead; i++) {
                    LPCITEMIDLIST child =
                        reinterpret_cast<LPCITEMIDLIST>(
                            reinterpret_cast<BYTE*>(cida) + cida->aoffset[i + 1]);
                    STRRET name = {};
                    if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_FORPARSING,
                                                           &name))) {
                        WCHAR path[MAX_PATH] = {};
                        if (SUCCEEDED(StrRetToBufW(&name, child, path, MAX_PATH)) &&
                            path[0]) {
                            selectionOut.push_back(path);
                        }
                    }
                }
                folder->Release();
            }
            if (parentAbs) {
                ILFree(parentAbs);
            }
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
    }

    dataObject->Release();
    return selectedCount;
}

static bool GetFilesystemPathFromShellItem(IShellItem* item,
                                           std::wstring& pathOut) {
    pathOut.clear();
    if (!item) {
        return false;
    }

    PWSTR path = nullptr;
    bool ok = SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) &&
              path && path[0];
    if (ok) {
        pathOut = path;
    }
    if (path) {
        CoTaskMemFree(path);
    }
    return ok;
}

static bool IsNavigationPaneWindow(HWND hwnd) {
    bool sawNavigationTreeClass = false;
    HWND w = hwnd;
    while (w) {
        WCHAR className[64] = {};
        GetClassNameW(w, className, ARRAYSIZE(className));

        if (_wcsicmp(className, L"NamespaceTreeControl") == 0 ||
            _wcsicmp(className, L"SysTreeView32") == 0) {
            sawNavigationTreeClass = true;
        } else if (_wcsicmp(className, L"CabinetWClass") == 0 ||
                   _wcsicmp(className, L"ExploreWClass") == 0) {
            return sawNavigationTreeClass;
        }

        HWND parent = GetParent(w);
        if (!parent) {
            parent = GetWindow(w, GW_OWNER);
        }
        w = parent;
    }

    return false;
}

static bool IsNavigationPaneContextWindow(const POINT& invocationPoint) {
    HWND invocationWindow = WindowFromPoint(invocationPoint);
    return invocationWindow && IsNavigationPaneWindow(invocationWindow);
}

static bool ResolveNavigationPaneMenuTarget(HWND hwnd,
                                            const POINT& invocationPoint,
                                            MenuTarget& targetOut) {
    targetOut = {};

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        root = hwnd;
    }

    IServiceProvider* serviceProvider = GetExplorerServiceProviderForHwnd(root);
    if (!serviceProvider) {
        return false;
    }

    bool ok = false;
    INameSpaceTreeControl* navigationPane = nullptr;
    if (SUCCEEDED(serviceProvider->QueryService(
            SID_SNavigationPane, IID_INameSpaceTreeControl,
            reinterpret_cast<void**>(&navigationPane))) &&
        navigationPane) {
        IShellBrowser* shellBrowser = nullptr;
        HWND treeWindow = nullptr;
        if (SUCCEEDED(serviceProvider->QueryService(
                SID_STopLevelBrowser, IID_IShellBrowser,
                reinterpret_cast<void**>(&shellBrowser))) &&
            shellBrowser) {
            shellBrowser->GetControlWindow(FCW_TREE, &treeWindow);
            shellBrowser->Release();
        }

        POINT clientPoint = invocationPoint;
        if (treeWindow && ScreenToClient(treeWindow, &clientPoint)) {
            IShellItem* hitItem = nullptr;
            if (SUCCEEDED(navigationPane->HitTest(&clientPoint, &hitItem)) &&
                hitItem) {
                std::wstring path;
                if (GetFilesystemPathFromShellItem(hitItem, path) &&
                    IsDirectoryPath(path)) {
                    targetOut.path = std::move(path);
                    targetOut.kind = IsDriveRootPath(targetOut.path)
                                         ? TargetKind::DriveItem
                                         : TargetKind::FolderItem;
                    ok = true;
                }
                hitItem->Release();
            }
        }

        navigationPane->Release();
    }

    serviceProvider->Release();
    return ok;
}

static bool ResolveMenuTarget(HWND hwnd,
                              const Settings& settings,
                              const POINT& invocationPoint,
                              MenuTarget& targetOut) {
    targetOut = {};

    bool isNavigationPane = IsNavigationPaneContextWindow(invocationPoint);
    if (isNavigationPane) {
        if (!settings.showOnNavigationPane) {
            return false;
        }
        return ResolveNavigationPaneMenuTarget(hwnd, invocationPoint, targetOut);
    }

    if (!IsShellViewWindow(hwnd)) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        root = hwnd;
    }

    bool isDesktopShellView = IsDesktopShellViewWindow(hwnd);
    IShellView* shellView = GetActiveShellViewForHwnd(root);
    if (!shellView && isDesktopShellView) {
        shellView = GetDesktopShellView();
    }
    if (!shellView) {
        return false;
    }

    bool ok = false;
    IFolderView* folderView = nullptr;
    if (SUCCEEDED(shellView->QueryInterface(IID_IFolderView,
                                            reinterpret_cast<void**>(&folderView))) &&
        folderView) {
        int selectedCount = 0;
        bool hasSelectedCount =
            SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &selectedCount));

        if (hasSelectedCount && selectedCount > 1) {
            ok = false;
        } else if (hasSelectedCount && selectedCount == 0) {
            std::wstring folderPath;
            GetCurrentFolderPath(folderView, folderPath);
            if (!folderPath.empty() && IsDirectoryPath(folderPath)) {
                targetOut.kind = TargetKind::FolderBackground;
                targetOut.path = folderPath;
                ok = true;
            }
        } else {
            std::vector<std::wstring> selectedPaths;
            UINT shellSelectedCount = GetSelectedPaths(shellView, selectedPaths, 2);

            if (selectedPaths.empty()) {
                if (!hasSelectedCount && shellSelectedCount == 0) {
                    std::wstring folderPath;
                    GetCurrentFolderPath(folderView, folderPath);
                    if (!folderPath.empty() && IsDirectoryPath(folderPath)) {
                        targetOut.kind = TargetKind::FolderBackground;
                        targetOut.path = folderPath;
                        ok = true;
                    }
                }
            } else if ((hasSelectedCount ? selectedCount == 1
                                         : shellSelectedCount == 1) &&
                       selectedPaths.size() == 1 &&
                       IsDirectoryPath(selectedPaths[0])) {
                targetOut.path = selectedPaths[0];
                targetOut.kind = IsDriveRootPath(targetOut.path) ? TargetKind::DriveItem
                                                                 : TargetKind::FolderItem;
                ok = true;
            } else if (selectedPaths.size() == 1 &&
                       IsFilePath(selectedPaths[0])) {
                if (settings.showOnScriptFiles &&
                    IsScriptExtension(selectedPaths[0], settings)) {
                    targetOut.path = selectedPaths[0];
                    targetOut.kind = TargetKind::ScriptItem;
                    ok = true;
                }
            }
        }

        folderView->Release();
    }

    if (!ok && isDesktopShellView) {
        std::wstring folderPath;
        if (GetDesktopFolderPath(folderPath) && IsDirectoryPath(folderPath)) {
            targetOut.kind = TargetKind::FolderBackground;
            targetOut.path = folderPath;
            ok = true;
        }
    }

    shellView->Release();
    return ok;
}

static bool IsShellViewWindow(HWND hwnd) {
    HWND w = hwnd;
    while (w) {
        WCHAR className[64] = {};
        GetClassNameW(w, className, ARRAYSIZE(className));
        if (_wcsicmp(className, L"SHELLDLL_DefView") == 0) {
            return true;
        }

        HWND parent = GetParent(w);
        if (!parent) {
            parent = GetWindow(w, GW_OWNER);
        }
        w = parent;
    }
    return false;
}

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

struct ReparseDataHeader {
    ULONG reparseTag;
    USHORT reparseDataLength;
    USHORT reserved;
};

static bool ResolveAppExecutionAliasTarget(const std::wstring& aliasPath,
                                           std::wstring& targetOut) {
    targetOut.clear();

    HANDLE file = CreateFileW(aliasPath.c_str(), FILE_READ_ATTRIBUTES,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    std::vector<BYTE> buffer(MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(file, FSCTL_GET_REPARSE_POINT, nullptr, 0,
                              buffer.data(), static_cast<DWORD>(buffer.size()),
                              &bytesReturned, nullptr);
    CloseHandle(file);

    if (!ok || bytesReturned < sizeof(ReparseDataHeader)) {
        return false;
    }

    const auto* header =
        reinterpret_cast<const ReparseDataHeader*>(buffer.data());
    if (header->reparseTag != IO_REPARSE_TAG_APPEXECLINK ||
        header->reparseDataLength <= sizeof(DWORD) ||
        bytesReturned < sizeof(ReparseDataHeader) + header->reparseDataLength) {
        return false;
    }

    const BYTE* payload = buffer.data() + sizeof(ReparseDataHeader);
    const wchar_t* field = reinterpret_cast<const wchar_t*>(payload + sizeof(DWORD));
    size_t remainingChars =
        (header->reparseDataLength - sizeof(DWORD)) / sizeof(wchar_t);
    int fieldIndex = 0;

    while (remainingChars > 0) {
        const wchar_t* start = field;
        size_t length = 0;
        while (length < remainingChars && start[length] != L'\0') {
            length++;
        }

        if (length > 0 && fieldIndex >= 2) {
            std::wstring candidate(start, length);
            if (IsFilePath(candidate)) {
                targetOut = std::move(candidate);
                return true;
            }
        }

        if (length == remainingChars) {
            break;
        }

        field += length + 1;
        remainingChars -= length + 1;
        fieldIndex++;
    }

    return false;
}

static bool ResolveExecutablePathForIcon(const Settings& settings, std::wstring& exeOut) {
    exeOut.clear();

    std::wstring candidate = TrimQuotes(TrimString(settings.terminalDisplayCommand));
    if (candidate.empty()) {
        return false;
    }

    if (IsFilePath(candidate)) {
        exeOut = candidate;
    } else if (!SearchExecutablePath(candidate.c_str(), exeOut)) {
        return false;
    }

    if (settings.terminalEffectiveChoice == L"wt") {
        std::wstring aliasTarget;
        if (ResolveAppExecutionAliasTarget(exeOut, aliasTarget)) {
            exeOut = std::move(aliasTarget);
        }
    }
    return true;
}

static HICON TryLoadShieldIcon() {
    SHSTOCKICONINFO stockIconInfo = {};
    stockIconInfo.cbSize = sizeof(stockIconInfo);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | SHGSI_SMALLICON,
                                     &stockIconInfo))) {
        return stockIconInfo.hIcon;
    }

    return nullptr;
}

static HBITMAP CreateMenuBitmapFromIcon(HICON icon, HICON overlayIcon = nullptr) {
    if (!icon) {
        return nullptr;
    }

    const int size = GetSystemMetrics(SM_CXMENUCHECK);
    if (size <= 0) {
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }
    HBITMAP bitmap = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    if (!memDc) {
        DeleteObject(bitmap);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    if (bits) {
        ZeroMemory(bits, static_cast<SIZE_T>(size) * static_cast<SIZE_T>(size) * 4);
    }
    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
    DrawIconEx(memDc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
    if (overlayIcon) {
        int overlaySize = MulDiv(size, 5, 8);
        const int minOverlaySize = 10;
        const int maxOverlaySize = size - 2;
        if (overlaySize < minOverlaySize) {
            overlaySize = minOverlaySize;
        }
        if (overlaySize > maxOverlaySize) {
            overlaySize = maxOverlaySize;
        }
        if (overlaySize > 0) {
            const int overlayX = size - overlaySize;
            const int overlayY = size - overlaySize;
            DrawIconEx(memDc, overlayX, overlayY, overlayIcon, overlaySize, overlaySize, 0,
                       nullptr, DI_NORMAL);
        }
    }
    SelectObject(memDc, oldBitmap);

    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
    return bitmap;
}

static HBITMAP TryCreateMenuBitmapForTerminal(const Settings& settings) {
    std::wstring exePath;
    if (!ResolveExecutablePathForIcon(settings, exePath)) {
        return nullptr;
    }

    SHFILEINFOW shfi = {};
    if (!SHGetFileInfoW(exePath.c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
                        SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES)) {
        return nullptr;
    }

    HICON shieldIcon = TryLoadShieldIcon();
    HBITMAP bitmap = CreateMenuBitmapFromIcon(shfi.hIcon, shieldIcon);
    if (shieldIcon) {
        DestroyIcon(shieldIcon);
    }
    if (shfi.hIcon) {
        DestroyIcon(shfi.hIcon);
    }
    return bitmap;
}

static std::wstring GetMenuBitmapCacheKey(const Settings& settings) {
    return settings.terminalEffectiveChoice + L"\n" + settings.terminalDisplayCommand;
}

static HBITMAP GetCachedMenuBitmapForTerminal(const Settings& settings) {
    std::wstring key = GetMenuBitmapCacheKey(settings);

    AcquireSRWLockShared(&g_menuBitmapLock);
    for (const auto& entry : g_menuBitmapCache) {
        if (entry.key == key) {
            HBITMAP bitmap = entry.bitmap;
            ReleaseSRWLockShared(&g_menuBitmapLock);
            return bitmap;
        }
    }
    ReleaseSRWLockShared(&g_menuBitmapLock);

    HBITMAP bitmap = TryCreateMenuBitmapForTerminal(settings);

    AcquireSRWLockExclusive(&g_menuBitmapLock);
    for (const auto& entry : g_menuBitmapCache) {
        if (entry.key == key) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
            bitmap = entry.bitmap;
            ReleaseSRWLockExclusive(&g_menuBitmapLock);
            return bitmap;
        }
    }
    g_menuBitmapCache.push_back({std::move(key), bitmap});
    ReleaseSRWLockExclusive(&g_menuBitmapLock);

    return bitmap;
}

static HBITMAP GetCachedMenuBitmapNoShield(const Settings& settings) {
    std::wstring key = GetMenuBitmapCacheKey(settings) + L"\nno-shield";

    AcquireSRWLockShared(&g_menuBitmapLock);
    for (const auto& entry : g_menuBitmapCache) {
        if (entry.key == key) {
            HBITMAP bitmap = entry.bitmap;
            ReleaseSRWLockShared(&g_menuBitmapLock);
            return bitmap;
        }
    }
    ReleaseSRWLockShared(&g_menuBitmapLock);

    std::wstring exePath;
    HBITMAP bitmap = nullptr;
    if (ResolveExecutablePathForIcon(settings, exePath)) {
        SHFILEINFOW shfi = {};
        if (SHGetFileInfoW(exePath.c_str(), FILE_ATTRIBUTE_NORMAL, &shfi,
                           sizeof(shfi),
                           SHGFI_ICON | SHGFI_SMALLICON |
                               SHGFI_USEFILEATTRIBUTES)) {
            bitmap = CreateMenuBitmapFromIcon(shfi.hIcon, nullptr);
            if (shfi.hIcon) {
                DestroyIcon(shfi.hIcon);
            }
        }
    }

    AcquireSRWLockExclusive(&g_menuBitmapLock);
    for (const auto& entry : g_menuBitmapCache) {
        if (entry.key == key) {
            if (bitmap) {
                DeleteObject(bitmap);
            }
            bitmap = entry.bitmap;
            ReleaseSRWLockExclusive(&g_menuBitmapLock);
            return bitmap;
        }
    }
    g_menuBitmapCache.push_back({std::move(key), bitmap});
    ReleaseSRWLockExclusive(&g_menuBitmapLock);

    return bitmap;
}

static void ClearMenuBitmapCache() {
    AcquireSRWLockExclusive(&g_menuBitmapLock);
    for (const auto& entry : g_menuBitmapCache) {
        if (entry.bitmap) {
            DeleteObject(entry.bitmap);
        }
    }
    g_menuBitmapCache.clear();
    ReleaseSRWLockExclusive(&g_menuBitmapLock);
}

static void ClearCurrentMenuState() {
    g_currentMenuEligible = false;
    g_currentTarget = {};
    g_currentMenuHwnd = nullptr;
}

static std::wstring GetScriptTerminalDisplayName(const Settings& settings,
                                                 const std::wstring& scriptPath) {
    if (IsScriptHostChoice(settings.terminalEffectiveChoice)) {
        return GetTerminalDisplayName(settings);
    }

    PCWSTR extension = PathFindExtensionW(scriptPath.c_str());
    if (_wcsicmp(extension, L".bat") == 0 ||
        _wcsicmp(extension, L".cmd") == 0) {
        return L"Command Prompt";
    }
    if (_wcsicmp(extension, L".vbs") == 0 ||
        _wcsicmp(extension, L".js") == 0) {
        return L"Windows Script Host";
    }

    LaunchSpec interpreter = BuildScriptInterpreterSpec(settings, scriptPath);
    PCWSTR executableName = PathFindFileNameW(interpreter.executable.c_str());
    return _wcsicmp(executableName, L"pwsh.exe") == 0 ? L"PowerShell 7"
                                                       : L"Windows PowerShell";
}

static Settings GetScriptIconSettings(const Settings& settings,
                                      const std::wstring& scriptPath) {
    if (IsScriptHostChoice(settings.terminalEffectiveChoice)) {
        return settings;
    }

    Settings iconSettings = settings;
    LaunchSpec interpreter = BuildScriptInterpreterSpec(settings, scriptPath);
    iconSettings.terminalEffectiveChoice.clear();
    iconSettings.terminalDisplayCommand = interpreter.executable;
    return iconSettings;
}

static void InsertAdminTerminalMenuItem(HMENU menu, const Settings& settings,
                                        const MenuTarget& target) {
    int itemCount = GetMenuItemCount(menu);
    int insertPos = 0;
    bool separatorAbove = false;

    if (_wcsicmp(settings.position.c_str(), L"Bottom") == 0) {
        insertPos = itemCount < 0 ? 0 : itemCount;
        while (insertPos > 0) {
            MENUITEMINFOW itemInfo = {};
            itemInfo.cbSize = sizeof(itemInfo);
            itemInfo.fMask = MIIM_FTYPE;
            if (!GetMenuItemInfoW(menu, insertPos - 1, TRUE, &itemInfo) ||
                !(itemInfo.fType & MFT_SEPARATOR)) {
                break;
            }
            insertPos--;
        }
        separatorAbove = true;
    } else if (_wcsicmp(settings.position.c_str(), L"Default") == 0) {
        insertPos = 0;
        for (int i = 0; i < itemCount; i++) {
            WCHAR text[128] = {};
            if (GetMenuStringW(menu, i, text, ARRAYSIZE(text), MF_BYPOSITION) > 0 &&
                (StrStrIW(text, L"Open") || StrStrIW(text, L"Terminal"))) {
                insertPos = i + 1;
                break;
            }
        }
    } else {
        insertPos = 0;
    }

    bool isScript = target.kind == TargetKind::ScriptItem;
    bool showAdmin = !isScript || settings.scriptMenuEntries != L"normal";
    bool showNormal = isScript
                          ? settings.scriptMenuEntries == L"normal" ||
                                settings.scriptMenuEntries == L"both"
                          : settings.showNonElevatedEntry;
    Settings iconSettings = isScript
                                ? GetScriptIconSettings(settings, target.path)
                                : settings;
    std::wstring displayName = isScript
                                   ? GetScriptTerminalDisplayName(settings, target.path)
                                   : GetTerminalDisplayName(settings);
    std::wstring adminLabel;
    if (isScript) {
        adminLabel = L"Run script";
        if (settings.appendScriptTerminalName) {
            adminLabel += L" in " + displayName;
        }
        adminLabel += L" as administrator";
    } else {
        adminLabel = settings.menuText;
    }
    std::wstring normalLabel;
    if (isScript) {
        normalLabel = L"Run script";
        if (settings.appendScriptTerminalName) {
            normalLabel += L" in " + displayName;
        }
    } else {
        normalLabel = settings.nonElevatedMenuText;
        if (normalLabel.empty()) {
            normalLabel = L"Open " + displayName + L" here";
        }
    }

    int actionPos = insertPos;
    if (separatorAbove) {
        InsertMenuW(menu, insertPos, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        actionPos++;
    }

    if (showAdmin) {
        InsertMenuW(menu, actionPos++, MF_BYPOSITION | MF_STRING, kMenuCommandId,
                    adminLabel.c_str());
    }
    if (showNormal) {
        InsertMenuW(menu, actionPos++, MF_BYPOSITION | MF_STRING,
                    kMenuCommandIdNonElevated, normalLabel.c_str());
    }
    if (!separatorAbove) {
        InsertMenuW(menu, actionPos, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    }

    if (showAdmin) {
        HBITMAP menuBitmap = GetCachedMenuBitmapForTerminal(iconSettings);
        if (menuBitmap) {
            MENUITEMINFOW itemInfo = {};
            itemInfo.cbSize = sizeof(itemInfo);
            itemInfo.fMask = MIIM_BITMAP;
            itemInfo.hbmpItem = menuBitmap;
            if (!SetMenuItemInfoW(menu, kMenuCommandId, FALSE, &itemInfo)) {
                Wh_Log(L"Menu icon assignment failed");
            } else {
                Wh_Log(L"Menu icon assigned");
            }
        } else {
            Wh_Log(L"Menu icon unavailable");
        }
    }

    if (showNormal) {
        HBITMAP menuBitmapNoShield = GetCachedMenuBitmapNoShield(iconSettings);
        if (menuBitmapNoShield) {
            MENUITEMINFOW itemInfo = {};
            itemInfo.cbSize = sizeof(itemInfo);
            itemInfo.fMask = MIIM_BITMAP;
            itemInfo.hbmpItem = menuBitmapNoShield;
            if (!SetMenuItemInfoW(menu, kMenuCommandIdNonElevated, FALSE,
                                  &itemInfo)) {
                Wh_Log(L"Non-elevated menu icon assignment failed");
            } else {
                Wh_Log(L"Non-elevated menu icon assigned");
            }
        } else {
            Wh_Log(L"Non-elevated menu icon unavailable");
        }
    }
}

static void LaunchAdminTerminal(const MenuTarget& target) {
    Settings settings = GetSettingsSnapshot();
    LaunchSpec launch = (target.kind == TargetKind::ScriptItem)
                            ? BuildScriptLaunchSpec(settings, target.path)
                            : BuildLaunchSpec(settings, target.path);
    if (launch.executable.empty()) {
        Wh_Log(L"Launch failed: no executable resolved target=%ls",
               target.path.c_str());
        return;
    }

    SHELLEXECUTEINFOW executeInfo = {};
    executeInfo.cbSize = sizeof(executeInfo);
    executeInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    executeInfo.lpVerb = L"runas";
    executeInfo.lpFile = launch.executable.c_str();
    executeInfo.lpParameters =
        launch.parameters.empty() ? nullptr : launch.parameters.c_str();
    executeInfo.lpDirectory =
        launch.workingDirectory.empty() ? nullptr : launch.workingDirectory.c_str();
    executeInfo.nShow = SW_SHOWNORMAL;

    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        executeInfo.fMask |= SEE_MASK_HMONITOR;
        executeInfo.hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    }

    if (ShellExecuteExW(&executeInfo)) {
        Wh_Log(L"Launch succeeded: target=%ls executable=%ls parameters=%ls",
               target.path.c_str(), launch.executable.c_str(),
               launch.parameters.c_str());
    } else {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) {
            Wh_Log(L"Launch failed: error=%lu target=%ls executable=%ls parameters=%ls",
                   error, target.path.c_str(), launch.executable.c_str(),
                   launch.parameters.c_str());
        }
    }
}

static void LaunchTerminalNonElevated(const MenuTarget& target) {
    Settings settings = GetSettingsSnapshot();
    LaunchSpec launch = (target.kind == TargetKind::ScriptItem)
                            ? BuildScriptLaunchSpec(settings, target.path)
                            : BuildLaunchSpec(settings, target.path);
    if (launch.executable.empty()) {
        Wh_Log(L"Launch failed: no executable resolved target=%ls",
               target.path.c_str());
        return;
    }

    SHELLEXECUTEINFOW executeInfo = {};
    executeInfo.cbSize = sizeof(executeInfo);
    executeInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    executeInfo.lpVerb = L"open";
    executeInfo.lpFile = launch.executable.c_str();
    executeInfo.lpParameters =
        launch.parameters.empty() ? nullptr : launch.parameters.c_str();
    executeInfo.lpDirectory =
        launch.workingDirectory.empty() ? nullptr : launch.workingDirectory.c_str();
    executeInfo.nShow = SW_SHOWNORMAL;

    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        executeInfo.fMask |= SEE_MASK_HMONITOR;
        executeInfo.hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    }

    if (ShellExecuteExW(&executeInfo)) {
        Wh_Log(L"Launch succeeded: target=%ls executable=%ls parameters=%ls",
               target.path.c_str(), launch.executable.c_str(),
               launch.parameters.c_str());
    } else {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) {
            Wh_Log(L"Launch failed: error=%lu target=%ls executable=%ls parameters=%ls",
                   error, target.path.c_str(), launch.executable.c_str(),
                   launch.parameters.c_str());
        }
    }
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU menu,
                                  UINT flags,
                                  int x,
                                  int y,
                                  HWND hwnd,
                                  LPTPMPARAMS params) {
    bool injected = false;
    ClearCurrentMenuState();

    HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
    WCHAR rootClass[64] = {};
    if (root) {
        GetClassNameW(root, rootClass, ARRAYSIZE(rootClass));
    }
    bool eligibleWindow = hwnd &&
                          (IsShellViewWindow(hwnd) ||
                           _wcsicmp(rootClass, L"CabinetWClass") == 0 ||
                           _wcsicmp(rootClass, L"ExploreWClass") == 0);

    if (menu && eligibleWindow) {
        MenuTarget target;
        Settings settings = GetSettingsSnapshot();
        POINT invocationPoint = {x, y};
        if (!ResolveMenuTarget(hwnd, settings, invocationPoint, target)) {
            Wh_Log(L"Injection skipped: no eligible filesystem directory target");
        } else if (!IsTargetEnabled(settings, target.kind)) {
            Wh_Log(L"Injection skipped: target kind disabled kind=%ls path=%ls",
                   TargetKindName(target.kind), target.path.c_str());
        } else {
            if (settings.terminalUsedFallback) {
                Wh_Log(L"Terminal fallback: requested=%ls effective=%ls command=%ls",
                       settings.terminalChoice.c_str(),
                       settings.terminalEffectiveChoice.c_str(),
                       settings.terminalDisplayCommand.c_str());
            }
            Wh_Log(L"Injection target: kind=%ls path=%ls",
                   TargetKindName(target.kind), target.path.c_str());
            InsertAdminTerminalMenuItem(menu, settings, target);
            Wh_Log(L"Injection inserted: position=%ls kind=%ls",
                   settings.position.c_str(), TargetKindName(target.kind));
            injected = true;
            g_currentMenuHwnd = hwnd;
            g_currentMenuEligible = true;
            g_currentTarget = std::move(target);
        }
    }

    BOOL result = TrackPopupMenuEx_Orig(menu, flags, x, y, hwnd, params);

    if (injected && (flags & TPM_RETURNCMD) &&
        static_cast<UINT>(result) == kMenuCommandId) {
        MenuTarget target = g_currentTarget;
        ClearCurrentMenuState();
        LaunchAdminTerminal(target);
        return 0;
    }

    if (injected && (flags & TPM_RETURNCMD) &&
        static_cast<UINT>(result) == kMenuCommandIdNonElevated) {
        MenuTarget target = g_currentTarget;
        ClearCurrentMenuState();
        LaunchTerminalNonElevated(target);
        return 0;
    }

    ClearCurrentMenuState();
    return result;
}

BOOL WINAPI PostMessageW_Hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND &&
        LOWORD(wParam) == kMenuCommandId &&
        g_currentMenuEligible &&
        hwnd == g_currentMenuHwnd) {
        MenuTarget target = g_currentTarget;
        ClearCurrentMenuState();
        LaunchAdminTerminal(target);
        return TRUE;
    }

    if (message == WM_COMMAND &&
        LOWORD(wParam) == kMenuCommandIdNonElevated &&
        g_currentMenuEligible &&
        hwnd == g_currentMenuHwnd) {
        MenuTarget target = g_currentTarget;
        ClearCurrentMenuState();
        LaunchTerminalNonElevated(target);
        return TRUE;
    }

    return PostMessageW_Orig(hwnd, message, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_shellIdListClipboardFormat = RegisterClipboardFormatW(L"Shell IDList Array");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = LoadSettings();
    Wh_Log(L"Settings: background=%d folder=%d drive=%d nav=%d terminal=%ls effective=%ls command=%ls fallback=%d position=%ls",
           static_cast<int>(g_settings.showOnFolderBackground),
           static_cast<int>(g_settings.showOnFolderItem),
           static_cast<int>(g_settings.showOnDriveItem),
           static_cast<int>(g_settings.showOnNavigationPane),
           g_settings.terminalChoice.c_str(),
           g_settings.terminalEffectiveChoice.c_str(),
           g_settings.terminalDisplayCommand.c_str(),
           static_cast<int>(g_settings.terminalUsedFallback),
           g_settings.position.c_str());
    ReleaseSRWLockExclusive(&g_settingsLock);

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(TrackPopupMenuEx),
                            reinterpret_cast<void*>(TrackPopupMenuEx_Hook),
                            reinterpret_cast<void**>(&TrackPopupMenuEx_Orig))) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(PostMessageW),
                            reinterpret_cast<void*>(PostMessageW_Hook),
                            reinterpret_cast<void**>(&PostMessageW_Orig))) {
        Wh_Log(L"Failed to hook PostMessageW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    ClearMenuBitmapCache();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = FALSE;

    Settings newSettings = LoadSettings();
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(newSettings);
    ReleaseSRWLockExclusive(&g_settingsLock);
    ClearMenuBitmapCache();

    return TRUE;
}

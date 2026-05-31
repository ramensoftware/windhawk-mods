// ==WindhawkMod==
// @id              open-in-admin-terminal
// @name            Open in Admin Terminal
// @description     Adds an Explorer classic context menu entry to open an elevated terminal in the current or selected folder.
// @version         1.15
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
- Optionally show the entry when right-clicking filesystem folders and drives in the navigation pane and Quick access
- Choose your preferred terminal: Auto, Windows Terminal, PowerShell 7, Windows PowerShell, Command Prompt, WSL, Git Bash, WezTerm, Alacritty, ConEmu, or a custom command
- Customize the context menu label, or let the mod use a smart default based on your terminal choice
- Optionally append the terminal name to a custom label (e.g. "Open elevated (Windows Terminal)")

## Preview

![Normal usage — right-click folder background or folder item to open elevated Windows Terminal](https://i.imgur.com/qEgGpvc.gif)

*Right-clicking a folder item to open an elevated terminal in that location (Windows Terminal)*

![Context menu with Windows Terminal option](https://i.imgur.com/HziEs16.png) ![Context menu with PowerShell 7 option](https://i.imgur.com/CCE3vlP.png)

![Mod settings showcase](https://i.imgur.com/8KgCdju.gif)

*Settings panel — choose terminal, customize label, toggle terminal name appending*

Screenshots may show earlier builds, but current releases use runtime classic-menu injection. The mod does not write Explorer context menu entries to the registry.

## Files

- `open-in-admin-terminal.wh.cpp` — Windhawk mod source

## How to use

1. Open Windhawk.
2. Create a new mod.
3. Paste in `open-in-admin-terminal.wh.cpp`.
4. Compile and enable it.

## Notes

- On Windows 11, Explorer may place this entry under `Show more options` depending on your context menu setup.
- The entry is injected only while Explorer's classic menu is open; disabling the mod leaves no registry cleanup behind.
- The mod intentionally targets filesystem folders and drive roots only, including optional navigation pane and Quick access support.
- Auto chooses Windows Terminal, PowerShell 7, Windows PowerShell, then Command Prompt. If another built-in preset is unavailable, the mod falls back to Auto instead of hiding the entry.
- Routine diagnostics are quiet by default. Enable debug logging in the settings when troubleshooting target detection or launch behavior.

## Version log
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
- debugLogging: false
  $name: Debug logging
  $description: Log target detection, injection decisions, and successful launches.
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
    bool debugLogging;
};

enum class TargetKind {
    None,
    FolderBackground,
    FolderItem,
    DriveItem,
};

struct MenuTarget {
    TargetKind kind = TargetKind::None;
    std::wstring path;
};

static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static const UINT kMenuCommandId = 0xBF31;

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

#define DEBUG_LOG(settings, ...) \
    do {                         \
        if ((settings).debugLogging) { \
            Wh_Log(__VA_ARGS__); \
        }                        \
    } while (false)

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
    s.debugLogging = GetSettingBool(L"debugLogging");

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
    return false;
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
    return L"none";
}

static bool IsDirectoryPath(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool IsDriveRootPath(const std::wstring& path) {
    return PathIsRootW(path.c_str()) != FALSE;
}

static std::wstring EscapePS(const std::wstring& s) {
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

static std::wstring BuildPSStringLiteral(const std::wstring& value) {
    return L"'" + EscapePS(value) + L"'";
}

static std::wstring QuoteProcessArgument(const std::wstring& arg) {
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

static std::wstring BuildPSArgumentArray(const std::vector<std::wstring>& args) {
    std::wstring result = L"@(";
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) {
            result += L",";
        }
        result += BuildPSStringLiteral(QuoteProcessArgument(args[i]));
    }
    result += L")";
    return result;
}

static std::wstring Base64Encode(const BYTE* bytes, size_t size) {
    static constexpr wchar_t kBase64[] =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::wstring result;
    result.reserve(((size + 2) / 3) * 4);

    for (size_t i = 0; i < size; i += 3) {
        BYTE b0 = bytes[i];
        BYTE b1 = i + 1 < size ? bytes[i + 1] : 0;
        BYTE b2 = i + 2 < size ? bytes[i + 2] : 0;

        result += kBase64[b0 >> 2];
        result += kBase64[((b0 & 0x03) << 4) | (b1 >> 4)];
        result += i + 1 < size ? kBase64[((b1 & 0x0F) << 2) | (b2 >> 6)]
                               : L'=';
        result += i + 2 < size ? kBase64[b2 & 0x3F] : L'=';
    }

    return result;
}

static std::wstring EncodePowerShellCommand(const std::wstring& script) {
    return Base64Encode(reinterpret_cast<const BYTE*>(script.data()),
                        script.size() * sizeof(wchar_t));
}

static std::wstring BuildSetLocationCommand(const std::wstring& target) {
    return L"Set-Location -LiteralPath '" + EscapePS(target) + L"'";
}

static std::wstring BuildStartProcess(const std::wstring& exe,
                                      const std::vector<std::wstring>& args,
                                      const std::wstring& workingDirectory) {
    std::wstring script =
        L"Start-Process -FilePath " + BuildPSStringLiteral(exe) + L" -Verb RunAs";
    if (!workingDirectory.empty()) {
        script += L" -WorkingDirectory " + BuildPSStringLiteral(workingDirectory);
    }
    if (!args.empty()) {
        script += L" -ArgumentList " + BuildPSArgumentArray(args);
    }

    return L"powershell.exe -NoProfile -WindowStyle Hidden -EncodedCommand " +
           EncodePowerShellCommand(script);
}

static std::wstring BuildCommand(const Settings& s, const std::wstring& target) {
    const std::wstring& choice = s.terminalEffectiveChoice.empty()
                                     ? s.terminalChoice
                                     : s.terminalEffectiveChoice;
    const std::wstring exe =
        s.terminalDisplayCommand.empty() ? L"cmd.exe" : s.terminalDisplayCommand;

    if (choice == L"wt") {
        return BuildStartProcess(exe, {L"-d", target}, {});
    }
    if (choice == L"pwsh") {
        return BuildStartProcess(exe,
                                 {L"-NoExit", L"-Command",
                                  BuildSetLocationCommand(target)},
                                 {});
    }
    if (choice == L"powershell") {
        return BuildStartProcess(exe,
                                 {L"-NoExit", L"-Command",
                                  BuildSetLocationCommand(target)},
                                 {});
    }
    if (choice == L"cmd") {
        return BuildStartProcess(exe, {L"/k", L"cd /d \"" + target + L"\""}, {});
    }
    if (choice == L"wsl") {
        return BuildStartProcess(exe, {L"--cd", target}, {});
    }
    if (choice == L"gitbash") {
        return BuildStartProcess(exe, {}, target);
    }
    if (choice == L"wezterm") {
        return BuildStartProcess(exe, {L"start", L"--cwd", target}, {});
    }
    if (choice == L"alacritty") {
        return BuildStartProcess(exe, {L"--working-directory", target}, {});
    }
    if (choice == L"conemu") {
        return BuildStartProcess(exe, {L"-Dir", target}, {});
    }

    return BuildStartProcess(exe, {}, {});
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

static bool GetSingleFilesystemPathFromShellItemArray(IShellItemArray* items,
                                                       std::wstring& pathOut) {
    pathOut.clear();
    if (!items) {
        return false;
    }

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)) || count != 1) {
        return false;
    }

    IShellItem* item = nullptr;
    if (FAILED(items->GetItemAt(0, &item)) || !item) {
        return false;
    }

    PWSTR path = nullptr;
    bool ok = SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path &&
              path[0];
    if (ok) {
        pathOut = path;
    }
    if (path) {
        CoTaskMemFree(path);
    }

    item->Release();
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

static bool ResolveNavigationPaneMenuTarget(HWND hwnd, MenuTarget& targetOut) {
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
        IShellItemArray* selectedItems = nullptr;
        if (SUCCEEDED(navigationPane->GetSelectedItems(&selectedItems)) &&
            selectedItems) {
            std::wstring path;
            if (GetSingleFilesystemPathFromShellItemArray(selectedItems, path) &&
                IsDirectoryPath(path)) {
                targetOut.path = std::move(path);
                targetOut.kind = IsDriveRootPath(targetOut.path)
                                     ? TargetKind::DriveItem
                                     : TargetKind::FolderItem;
                ok = true;
            }
            selectedItems->Release();
        }
        navigationPane->Release();
    }

    serviceProvider->Release();
    return ok;
}

static bool ResolveMenuTarget(HWND hwnd,
                              bool allowNavigationPane,
                              MenuTarget& targetOut) {
    targetOut = {};

    bool isNavigationPane = IsNavigationPaneWindow(hwnd);
    if (isNavigationPane) {
        if (!allowNavigationPane) {
            return false;
        }
        return ResolveNavigationPaneMenuTarget(hwnd, targetOut);
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

static HBITMAP CreateMenuBitmapFromIcon(HICON icon) {
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

    HBITMAP bitmap = CreateMenuBitmapFromIcon(shfi.hIcon);
    DestroyIcon(shfi.hIcon);
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

static void InsertAdminTerminalMenuItem(HMENU menu, const Settings& settings) {
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

    if (separatorAbove) {
        InsertMenuW(menu, insertPos, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        InsertMenuW(menu, insertPos + 1, MF_BYPOSITION | MF_STRING, kMenuCommandId,
                    settings.menuText.c_str());
    } else {
        InsertMenuW(menu, insertPos, MF_BYPOSITION | MF_STRING, kMenuCommandId,
                    settings.menuText.c_str());
        InsertMenuW(menu, insertPos + 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    }

    HBITMAP menuBitmap = GetCachedMenuBitmapForTerminal(settings);
    if (menuBitmap) {
        MENUITEMINFOW itemInfo = {};
        itemInfo.cbSize = sizeof(itemInfo);
        itemInfo.fMask = MIIM_BITMAP;
        itemInfo.hbmpItem = menuBitmap;
        if (!SetMenuItemInfoW(menu, kMenuCommandId, FALSE, &itemInfo)) {
            DEBUG_LOG(settings, L"Menu icon assignment failed");
        } else {
            DEBUG_LOG(settings, L"Menu icon assigned");
        }
    } else {
        DEBUG_LOG(settings, L"Menu icon unavailable");
    }
}

static void LaunchAdminTerminal(const MenuTarget& target) {
    Settings settings = GetSettingsSnapshot();
    std::wstring command = BuildCommand(settings, target.path);
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startupInfo = {};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo = {};

    BOOL ok = CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, FALSE,
                             0, nullptr, nullptr, &startupInfo, &processInfo);
    if (ok) {
        DEBUG_LOG(settings, L"Launch succeeded: target=%ls command=%ls",
                  target.path.c_str(), command.c_str());
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    } else {
        Wh_Log(L"Launch failed: error=%lu target=%ls command=%ls",
               GetLastError(), target.path.c_str(), command.c_str());
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

    if (menu && hwnd &&
        (IsShellViewWindow(hwnd) || IsNavigationPaneWindow(hwnd))) {
        MenuTarget target;
        Settings settings = GetSettingsSnapshot();
        if (!ResolveMenuTarget(hwnd, settings.showOnNavigationPane, target)) {
            DEBUG_LOG(settings, L"Injection skipped: no eligible filesystem directory target");
        } else if (!IsTargetEnabled(settings, target.kind)) {
            DEBUG_LOG(settings,
                      L"Injection skipped: target kind disabled kind=%ls path=%ls",
                      TargetKindName(target.kind), target.path.c_str());
        } else {
            if (settings.terminalUsedFallback) {
                DEBUG_LOG(settings,
                          L"Terminal fallback: requested=%ls effective=%ls command=%ls",
                          settings.terminalChoice.c_str(),
                          settings.terminalEffectiveChoice.c_str(),
                          settings.terminalDisplayCommand.c_str());
            }
            DEBUG_LOG(settings, L"Injection target: kind=%ls path=%ls",
                      TargetKindName(target.kind), target.path.c_str());
            InsertAdminTerminalMenuItem(menu, settings);
            DEBUG_LOG(settings, L"Injection inserted: position=%ls text=%ls",
                      settings.position.c_str(), settings.menuText.c_str());
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

    return PostMessageW_Orig(hwnd, message, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init v1.15-classic");

    g_shellIdListClipboardFormat = RegisterClipboardFormatW(L"Shell IDList Array");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = LoadSettings();
    DEBUG_LOG(g_settings,
              L"Settings: background=%d folder=%d drive=%d nav=%d terminal=%ls effective=%ls command=%ls fallback=%d position=%ls",
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
    Settings settings = GetSettingsSnapshot();
    DEBUG_LOG(settings, L"Uninit");
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

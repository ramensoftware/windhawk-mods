// ==WindhawkMod==
// @id              explorer-terminal-admin-alias
// @name            File Explorer Terminal Admin Aliases
// @description     Launch elevated or custom terminals (cmd, Windows Terminal, PowerShell) directly from the File Explorer address bar with configurable aliases.
// @version         1.0.4
// @author          RobotsNeverDie
// @github          https://github.com/RBND
// @homepage        https://github.com/RBND/CMD-Admin
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🛠️ File Explorer Terminal Admin Aliases

Launch elevated (Administrator) command prompts, Windows Terminal, PowerShell, or custom shells directly from any Windows File Explorer address bar, opened to your active directory.

Just like typing `cmd`, `wt`, or `powershell` into File Explorer—but with administrator privileges and completely customizable aliases!

---

## ⚡ Features
- **Configurable Aliases**: Choose any alias trigger you want (`cmda`, `wta`, `psa`, `pwsha`, `basha`, or your own custom commands).
- **Native & Silent Elevation**: Intercepts shell execution directly inside File Explorer and launches with native `runas` elevation—no background PowerShell spawns, no `.bat` files, and zero flickering.
- **Zero PATH Pollution**: Works without needing to edit your system/user `Path` environment variable.
- **Directory Preservation**: Accurately preserves the active directory in elevated sessions, handling spaces, special characters, drive roots, and network UNC paths.
- **Virtual Folder Fallback**: Automatically defaults to `%USERPROFILE%` when launched from virtual folders (e.g., "This PC", "Quick Access", or "Home").
- **Clean UAC Cancellation**: If you cancel or dismiss the UAC prompt, the mod cleanly suppresses the annoying "Windows cannot find..." error dialog.
- **Inline Arguments Supported**: Type extra arguments directly in the address bar (e.g. `cmda dir` or `wta -p PowerShell`).

---

## ⚙️ Configuration
Open the mod's **Settings** tab in Windhawk to:
1. Customize aliases for **Command Prompt**, **Windows Terminal**, **PowerShell 5.1**, **PowerShell 7+**, and **Git Bash**.
2. Toggle elevation (`runas`) on or off per shell.
3. Configure custom aliases for any tool (e.g. `code`, `wsl`, `nvim`).
4. Set an optional profile name for Windows Terminal.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cmd_alias: "cmda"
  $name: "Command Prompt (Admin) Alias"
  $description: >-
    Address bar alias to launch an elevated Command Prompt in the active directory. Leave empty to disable.
- cmd_elevated: true
  $name: "Run Command Prompt Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).

- wt_alias: "wta"
  $name: "Windows Terminal (Admin) Alias"
  $description: >-
    Address bar alias to launch an elevated Windows Terminal in the active directory. Leave empty to disable.
- wt_elevated: true
  $name: "Run Windows Terminal Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).
- wt_profile: ""
  $name: "Windows Terminal Profile (Optional)"
  $description: >-
    Optional profile name to launch (e.g. "Command Prompt", "Windows PowerShell", "Ubuntu"). Leave empty for default profile.
- wt_fallback_cmd: true
  $name: "Fallback to CMD if WT is unavailable"
  $description: >-
    If wt.exe is not installed on the system, fallback to launching Command Prompt.

- ps_alias: "psa"
  $name: "PowerShell (Admin) Alias"
  $description: >-
    Address bar alias to launch an elevated Windows PowerShell (5.1) prompt in the active directory. Leave empty to disable.
- ps_elevated: true
  $name: "Run PowerShell Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).

- pwsh_alias: "pwsha"
  $name: "PowerShell 7+ (pwsh) Alias"
  $description: >-
    Address bar alias to launch an elevated PowerShell Core (pwsh.exe) prompt. Leave empty to disable.
- pwsh_elevated: true
  $name: "Run PowerShell 7+ Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).

- gitbash_alias: ""
  $name: "Git Bash Alias (Optional)"
  $description: >-
    Address bar alias to launch Git Bash in the active directory (e.g. "basha"). Leave empty to disable.
- gitbash_elevated: true
  $name: "Run Git Bash Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).
- gitbash_path: ""
  $name: "Git Bash Executable Path (Optional)"
  $description: >-
    Path to git-bash.exe. Leave empty to auto-detect from Program Files.

- custom1_alias: ""
  $name: "Custom Alias 1 - Name"
  $description: >-
    Custom alias trigger name (e.g. "codea"). Leave empty to disable.
- custom1_target: ""
  $name: "Custom Alias 1 - Target Executable"
  $description: >-
    Executable name or full path (e.g. "code.cmd").
- custom1_args: "%DIR%"
  $name: "Custom Alias 1 - Arguments"
  $description: >-
    Arguments passed to target. Supports "%DIR%" or "<DIR>" (active directory) and "%ARGS%" or "<ARGS>" (inline arguments).
- custom1_elevated: false
  $name: "Custom Alias 1 - Run Elevated"
  $description: >-
    Launch as Administrator (UAC prompt).

- fallback_path: "%USERPROFILE%"
  $name: "Fallback Directory"
  $description: >-
    Directory used when typing an alias inside virtual folders (e.g. "This PC", "Quick Access", or "Home").
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <string>

struct ModSettings {
    std::wstring cmdAlias;
    bool cmdElevated = true;

    std::wstring wtAlias;
    bool wtElevated = true;
    std::wstring wtProfile;
    bool wtFallbackCmd = true;

    std::wstring psAlias;
    bool psElevated = true;

    std::wstring pwshAlias;
    bool pwshElevated = true;

    std::wstring gitBashAlias;
    bool gitBashElevated = true;
    std::wstring gitBashPath;

    std::wstring custom1Alias;
    std::wstring custom1Target;
    std::wstring custom1Args;
    bool custom1Elevated = false;

    std::wstring fallbackPath;
};

static ModSettings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

thread_local bool g_inHook = false;

// Thread-safe settings getter
static ModSettings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

// Helper: Trim whitespace
static std::wstring Trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Helper: Read a string setting safely without substituting non-empty defaults when user intentionally cleared it
static std::wstring ReadStringSetting(PCWSTR name) {
    PCWSTR val = Wh_GetStringSetting(name);
    std::wstring result = (val != nullptr) ? val : L"";
    if (val != nullptr) Wh_FreeStringSetting(val);
    return Trim(result);
}

// Helper: Case-insensitive equality
static bool EqualsIgnoreCase(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

// Helper: Expand environment strings
static std::wstring ExpandEnv(const std::wstring& path) {
    if (path.empty()) return L"";
    WCHAR buffer[MAX_PATH * 4];
    DWORD len = ExpandEnvironmentStringsW(path.c_str(), buffer, ARRAYSIZE(buffer));
    if (len > 0 && len <= ARRAYSIZE(buffer)) {
        return std::wstring(buffer);
    }
    return path;
}

// Helper: Check if directory exists
static bool IsDirectory(const std::wstring& path) {
    if (path.empty()) return false;
    DWORD attrs = GetFileAttributesW(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
}

// Helper: Check if file exists
static bool FileExists(const std::wstring& path) {
    if (path.empty()) return false;
    DWORD attrs = GetFileAttributesW(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

// Helper: Check if an executable is in PATH or exists
static bool ExecutableExists(const std::wstring& exe) {
    if (exe.empty()) return false;
    if (FileExists(exe)) return true;
    WCHAR foundPath[MAX_PATH * 2];
    if (SearchPathW(nullptr, exe.c_str(), L".exe", ARRAYSIZE(foundPath), foundPath, nullptr) > 0) {
        return true;
    }
    return false;
}

// Helper: Replace all occurrences in a string
static void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return;
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::wstring::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

// Helper: Normalize directory string (strip trailing backslash for subdirectories, leave drive root like C:\)
static std::wstring NormalizeDir(const std::wstring& dir) {
    if (dir.empty()) return dir;
    std::wstring result = dir;
    if (result.length() > 3 && result.back() == L'\\') {
        result.pop_back();
    }
    return result;
}

// Helper: Safely enclose a path in quotes, escaping trailing backslashes so Windows CLI parser does not treat \" as an escaped quote
static std::wstring QuotePath(const std::wstring& path) {
    if (path.empty()) return L"\"\"";
    std::wstring result = path;
    if (result.back() == L'\\') {
        result.push_back(L'\\'); // e.g. "C:\" becomes "C:\\" inside quotes so closing quote is preserved
    }
    return L"\"" + result + L"\"";
}

// Helper: Escape single-quote delimiters for PowerShell -LiteralPath string literal.
// PowerShell treats ASCII ' (U+0027) as well as typographic single quotes
// U+2018 (‘), U+2019 (’), U+201A (‚), and U+201B (‛) as string delimiters.
static std::wstring EscapePowerShellLiteral(const std::wstring& str) {
    std::wstring escaped;
    escaped.reserve(str.length() + 8);
    for (WCHAR ch : str) {
        escaped.push_back(ch);
        if (ch == L'\'' || ch == (WCHAR)0x2018 || ch == (WCHAR)0x2019 || ch == (WCHAR)0x201A || ch == (WCHAR)0x201B) {
            escaped.push_back(ch); // Double the quote delimiter
        }
    }
    return escaped;
}

// Helper: Query active explorer directory from HWND via Shell COM
static bool GetExplorerWindowFolder(HWND hwnd, std::wstring& outPath) {
    if (!hwnd) return false;
    HWND hwndTop = GetAncestor(hwnd, GA_ROOT);
    if (!hwndTop) hwndTop = hwnd;

    // Ensure COM is initialized on the current thread
    HRESULT hrCoInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    IShellWindows* pShellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pShellWindows));
    if (FAILED(hr) || !pShellWindows) {
        if (SUCCEEDED(hrCoInit)) CoUninitialize();
        return false;
    }

    long count = 0;
    pShellWindows->get_Count(&count);
    bool found = false;

    for (long i = 0; i < count; i++) {
        VARIANT varIndex{};
        VariantInit(&varIndex);
        varIndex.vt = VT_I4;
        varIndex.lVal = i;

        IDispatch* pDisp = nullptr;
        if (SUCCEEDED(pShellWindows->Item(varIndex, &pDisp)) && pDisp) {
            IWebBrowserApp* pApp = nullptr;
            if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pApp))) && pApp) {
                HWND appHwnd = nullptr;
                pApp->get_HWND((SHANDLE_PTR*)&appHwnd);
                if (appHwnd == hwndTop) {
                    IServiceProvider* pSp = nullptr;
                    if (SUCCEEDED(pApp->QueryInterface(IID_PPV_ARGS(&pSp))) && pSp) {
                        IShellBrowser* pSb = nullptr;
                        if (SUCCEEDED(pSp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pSb))) && pSb) {
                            IShellView* pSv = nullptr;
                            if (SUCCEEDED(pSb->QueryActiveShellView(&pSv)) && pSv) {
                                IFolderView* pFv = nullptr;
                                if (SUCCEEDED(pSv->QueryInterface(IID_PPV_ARGS(&pFv))) && pFv) {
                                    IPersistFolder2* pPf = nullptr;
                                    if (SUCCEEDED(pFv->GetFolder(IID_PPV_ARGS(&pPf))) && pPf) {
                                        PIDLIST_ABSOLUTE pidl = nullptr;
                                        if (SUCCEEDED(pPf->GetCurFolder(&pidl)) && pidl) {
                                            WCHAR szPath[MAX_PATH];
                                            if (SHGetPathFromIDListW(pidl, szPath)) {
                                                outPath = szPath;
                                                found = true;
                                            }
                                            CoTaskMemFree(pidl);
                                        }
                                        pPf->Release();
                                    }
                                    pFv->Release();
                                }
                                pSv->Release();
                            }
                            pSb->Release();
                        }
                        pSp->Release();
                    }
                }
                pApp->Release();
            }
            pDisp->Release();
        }
        if (found) break;
    }

    pShellWindows->Release();
    if (SUCCEEDED(hrCoInit)) CoUninitialize();
    return found;
}

// Load / Reload Settings with exclusive lock
static void LoadSettings() {
    ModSettings s;
    s.cmdAlias = ReadStringSetting(L"cmd_alias");
    s.cmdElevated = Wh_GetIntSetting(L"cmd_elevated") != 0;

    s.wtAlias = ReadStringSetting(L"wt_alias");
    s.wtElevated = Wh_GetIntSetting(L"wt_elevated") != 0;
    s.wtProfile = ReadStringSetting(L"wt_profile");
    s.wtFallbackCmd = Wh_GetIntSetting(L"wt_fallback_cmd") != 0;

    s.psAlias = ReadStringSetting(L"ps_alias");
    s.psElevated = Wh_GetIntSetting(L"ps_elevated") != 0;

    s.pwshAlias = ReadStringSetting(L"pwsh_alias");
    s.pwshElevated = Wh_GetIntSetting(L"pwsh_elevated") != 0;

    s.gitBashAlias = ReadStringSetting(L"gitbash_alias");
    s.gitBashElevated = Wh_GetIntSetting(L"gitbash_elevated") != 0;
    s.gitBashPath = ReadStringSetting(L"gitbash_path");

    s.custom1Alias = ReadStringSetting(L"custom1_alias");
    s.custom1Target = ReadStringSetting(L"custom1_target");
    s.custom1Args = ReadStringSetting(L"custom1_args");
    if (s.custom1Args.empty()) {
        s.custom1Args = L"\"%DIR%\"";
    }
    s.custom1Elevated = Wh_GetIntSetting(L"custom1_elevated") != 0;

    s.fallbackPath = ReadStringSetting(L"fallback_path");
    if (s.fallbackPath.empty()) {
        s.fallbackPath = L"%USERPROFILE%";
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

enum class ShellType {
    None,
    Cmd,
    WindowsTerminal,
    PowerShell,
    PowerShellCore,
    GitBash,
    Custom1
};

struct MatchedAlias {
    ShellType type = ShellType::None;
    bool elevated = true;
    std::wstring aliasToken;
    std::wstring extraUserArgs;
};

// Check if input matches any configured alias
static bool MatchAlias(LPCWSTR lpFile, LPCWSTR lpParameters, MatchedAlias& outMatch, const ModSettings& settings) {
    if (!lpFile || lpFile[0] == L'\0') return false;

    std::wstring fullCommand = Trim(lpFile);
    if (fullCommand.empty()) return false;

    // Separate primary alias token and potential inline arguments
    size_t firstSpace = fullCommand.find_first_of(L" \t");
    std::wstring aliasToken;
    std::wstring inlineArgs;
    if (firstSpace != std::wstring::npos) {
        aliasToken = fullCommand.substr(0, firstSpace);
        inlineArgs = Trim(fullCommand.substr(firstSpace + 1));
    } else {
        aliasToken = fullCommand;
    }

    // Strip leading path if synthesized (e.g. C:\Windows\System32\cmda.exe -> cmda.exe)
    size_t lastSlash = aliasToken.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        aliasToken = aliasToken.substr(lastSlash + 1);
    }

    // Strip trailing ".exe" or ".bat" if typed
    if (aliasToken.length() > 4) {
        std::wstring ext = aliasToken.substr(aliasToken.length() - 4);
        if (EqualsIgnoreCase(ext, L".exe") || EqualsIgnoreCase(ext, L".bat")) {
            aliasToken.erase(aliasToken.length() - 4);
        }
    }

    // Combine inline arguments with lpParameters
    std::wstring userArgs;
    if (!inlineArgs.empty()) {
        userArgs = inlineArgs;
    }
    if (lpParameters && lpParameters[0] != L'\0') {
        std::wstring paramStr = Trim(lpParameters);
        if (!paramStr.empty()) {
            if (!userArgs.empty()) userArgs += L" ";
            userArgs += paramStr;
        }
    }
    outMatch.extraUserArgs = userArgs;
    outMatch.aliasToken = aliasToken;

    // Check Command Prompt
    if (!settings.cmdAlias.empty() && EqualsIgnoreCase(aliasToken, settings.cmdAlias)) {
        outMatch.type = ShellType::Cmd;
        outMatch.elevated = settings.cmdElevated;
        return true;
    }

    // Check Windows Terminal
    if (!settings.wtAlias.empty() && EqualsIgnoreCase(aliasToken, settings.wtAlias)) {
        outMatch.type = ShellType::WindowsTerminal;
        outMatch.elevated = settings.wtElevated;
        return true;
    }

    // Check PowerShell 5.1
    if (!settings.psAlias.empty() && EqualsIgnoreCase(aliasToken, settings.psAlias)) {
        outMatch.type = ShellType::PowerShell;
        outMatch.elevated = settings.psElevated;
        return true;
    }

    // Check PowerShell 7+
    if (!settings.pwshAlias.empty() && EqualsIgnoreCase(aliasToken, settings.pwshAlias)) {
        outMatch.type = ShellType::PowerShellCore;
        outMatch.elevated = settings.pwshElevated;
        return true;
    }

    // Check Git Bash
    if (!settings.gitBashAlias.empty() && EqualsIgnoreCase(aliasToken, settings.gitBashAlias)) {
        outMatch.type = ShellType::GitBash;
        outMatch.elevated = settings.gitBashElevated;
        return true;
    }

    // Check Custom 1
    if (!settings.custom1Alias.empty() && EqualsIgnoreCase(aliasToken, settings.custom1Alias)) {
        outMatch.type = ShellType::Custom1;
        outMatch.elevated = settings.custom1Elevated;
        return true;
    }

    return false;
}

// Resolve the active directory with virtual folder fallbacks
static std::wstring ResolveTargetDirectory(HWND hwnd, LPCWSTR lpDirectory, const ModSettings& settings) {
    WCHAR szSysDir[MAX_PATH];
    GetSystemDirectoryW(szSysDir, ARRAYSIZE(szSysDir));

    std::wstring target;

    // 1. Try COM folder from active Explorer window first if hwnd is valid.
    // If the user is viewing a real folder (including C:\Windows\System32), COM returns the exact folder.
    // Virtual folders (e.g. "This PC") fail SHGetPathFromIDListW and return false.
    if (hwnd) {
        std::wstring comPath;
        if (GetExplorerWindowFolder(hwnd, comPath) && IsDirectory(comPath)) {
            target = comPath;
        }
    }

    // 2. If COM didn't resolve, check lpDirectory (ignoring System32 which is Explorer's default for virtual folders)
    if (target.empty() && lpDirectory && lpDirectory[0] != L'\0') {
        std::wstring rawDir = Trim(lpDirectory);
        if (IsDirectory(rawDir) && !EqualsIgnoreCase(rawDir, szSysDir)) {
            target = rawDir;
        }
    }

    // 3. If still empty, use configured fallback directory (%USERPROFILE%)
    if (target.empty()) {
        std::wstring fallback = ExpandEnv(settings.fallbackPath);
        if (IsDirectory(fallback)) {
            target = fallback;
        } else {
            target = ExpandEnv(L"%USERPROFILE%");
        }
    }

    return target;
}

// Declarations of original functions
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original = nullptr;

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original = nullptr;

using SearchPathW_t = decltype(&SearchPathW);
SearchPathW_t SearchPathW_Original = nullptr;

using PathFindOnPathW_t = decltype(&PathFindOnPathW);
PathFindOnPathW_t PathFindOnPathW_Original = nullptr;

// Core execution handler
static bool HandleExecution(HWND hwnd, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory) {
    ModSettings settings = GetSettings();
    MatchedAlias match;
    if (!MatchAlias(lpFile, lpParameters, match, settings)) {
        return false;
    }

    std::wstring rawTargetDir = ResolveTargetDirectory(hwnd, lpDirectory, settings);
    std::wstring targetDir = NormalizeDir(rawTargetDir);
    std::wstring quotedTargetDir = QuotePath(targetDir);
    Wh_Log(L"Intercepted alias: raw='%s' dir='%s'", lpFile ? lpFile : L"", targetDir.c_str());

    std::wstring executable;
    std::wstring arguments;
    bool elevate = match.elevated;

    switch (match.type) {
        case ShellType::Cmd: {
            executable = L"cmd.exe";
            arguments = L"/k pushd " + quotedTargetDir;
            if (!match.extraUserArgs.empty()) {
                arguments += L" & " + match.extraUserArgs;
            }
            break;
        }

        case ShellType::WindowsTerminal: {
            bool hasWT = ExecutableExists(L"wt.exe");
            if (!hasWT && settings.wtFallbackCmd) {
                executable = L"cmd.exe";
                arguments = L"/k pushd " + quotedTargetDir;
                if (!match.extraUserArgs.empty()) {
                    arguments += L" & " + match.extraUserArgs;
                }
            } else {
                executable = L"wt.exe";
                arguments = L"";
                if (!settings.wtProfile.empty()) {
                    std::wstring wtProfileEscaped = settings.wtProfile;
                    ReplaceAll(wtProfileEscaped, L";", L"\\;");
                    arguments += L"-p " + QuotePath(wtProfileEscaped) + L" ";
                }
                std::wstring wtDirEscaped = targetDir;
                ReplaceAll(wtDirEscaped, L";", L"\\;");
                arguments += L"-d " + QuotePath(wtDirEscaped);
                if (!match.extraUserArgs.empty()) {
                    arguments += L" " + match.extraUserArgs;
                }
            }
            break;
        }

        case ShellType::PowerShell: {
            executable = L"powershell.exe";
            std::wstring escapedDir = EscapePowerShellLiteral(targetDir);
            if (match.extraUserArgs.empty()) {
                arguments = L"-NoExit -Command \"Set-Location -LiteralPath '" + escapedDir + L"'\"";
            } else {
                arguments = L"-NoExit -Command \"Set-Location -LiteralPath '" + escapedDir + L"'; " + match.extraUserArgs + L"\"";
            }
            break;
        }

        case ShellType::PowerShellCore: {
            executable = L"pwsh.exe";
            std::wstring escapedDir = EscapePowerShellLiteral(targetDir);
            if (match.extraUserArgs.empty()) {
                arguments = L"-NoExit -Command \"Set-Location -LiteralPath '" + escapedDir + L"'\"";
            } else {
                arguments = L"-NoExit -Command \"Set-Location -LiteralPath '" + escapedDir + L"'; " + match.extraUserArgs + L"\"";
            }
            break;
        }

        case ShellType::GitBash: {
            std::wstring bashPath = settings.gitBashPath;
            if (bashPath.empty()) {
                std::wstring testPath = ExpandEnv(L"%ProgramFiles%\\Git\\git-bash.exe");
                if (FileExists(testPath)) {
                    bashPath = testPath;
                } else {
                    testPath = ExpandEnv(L"%ProgramFiles(x86)%\\Git\\git-bash.exe");
                    if (FileExists(testPath)) {
                        bashPath = testPath;
                    } else {
                        bashPath = L"git-bash.exe";
                    }
                }
            } else {
                bashPath = ExpandEnv(bashPath);
            }
            executable = bashPath;
            arguments = L"--cd=" + quotedTargetDir;
            if (!match.extraUserArgs.empty()) {
                arguments += L" " + match.extraUserArgs;
            }
            break;
        }

        case ShellType::Custom1: {
            executable = ExpandEnv(settings.custom1Target);
            arguments = settings.custom1Args;
            ReplaceAll(arguments, L"%DIR%", targetDir);
            ReplaceAll(arguments, L"<DIR>", targetDir);
            if (arguments.find(L"%ARGS%") != std::wstring::npos || arguments.find(L"<ARGS>") != std::wstring::npos) {
                ReplaceAll(arguments, L"%ARGS%", match.extraUserArgs);
                ReplaceAll(arguments, L"<ARGS>", match.extraUserArgs);
            } else if (!match.extraUserArgs.empty()) {
                if (!arguments.empty()) arguments += L" ";
                arguments += match.extraUserArgs;
            }
            break;
        }

        default:
            return false;
    }

    Wh_Log(L"Launching: exe='%s', args='%s', elevated=%d", executable.c_str(), arguments.c_str(), elevate ? 1 : 0);

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = 0;
    sei.hwnd = hwnd;
    sei.lpVerb = elevate ? L"runas" : L"open";
    sei.lpFile = executable.c_str();
    sei.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
    sei.lpDirectory = targetDir.c_str();
    sei.nShow = SW_SHOWNORMAL;

    g_inHook = true;
    BOOL bSuccess = ShellExecuteExW_Original(&sei);
    g_inHook = false;

    if (!bSuccess) {
        DWORD dwErr = GetLastError();
        Wh_Log(L"Launch failed with error %lu", dwErr);
        // If user cancelled the UAC prompt, return true to suppress Explorer error dialog
        if (dwErr == ERROR_CANCELLED) {
            return true;
        }
        // If launch failed for another reason (e.g. binary not found), return false
        // so Explorer can report the error properly
        return false;
    }

    return true;
}

// Hook SearchPathW so Explorer considers the alias a valid command instead of treating it as a URL
DWORD WINAPI SearchPathW_Hook(
    LPCWSTR lpPath,
    LPCWSTR lpFileName,
    LPCWSTR lpExtension,
    DWORD nBufferLength,
    LPWSTR lpBuffer,
    LPWSTR* lpFilePart
) {
    if (g_inHook || !lpFileName || lpFileName[0] == L'\0') {
        return SearchPathW_Original(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
    }

    ModSettings settings = GetSettings();
    MatchedAlias match;
    if (MatchAlias(lpFileName, nullptr, match, settings)) {
        Wh_Log(L"SearchPathW intercepted alias: %s", lpFileName);
        WCHAR sysDir[MAX_PATH];
        GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir));
        WCHAR dummyPath[MAX_PATH];
        std::wstring fakeExe = match.aliasToken + L".exe";
        PathCombineW(dummyPath, sysDir, fakeExe.c_str());
        DWORD len = (DWORD)wcslen(dummyPath);
        if (lpBuffer && nBufferLength > len) {
            wcscpy_s(lpBuffer, nBufferLength, dummyPath);
            if (lpFilePart) {
                *lpFilePart = wcsrchr(lpBuffer, L'\\') ? wcsrchr(lpBuffer, L'\\') + 1 : lpBuffer;
            }
            return len;
        }
        // Comply with Win32 SearchPath contract: return required size including null terminator
        return len + 1;
    }

    return SearchPathW_Original(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}

// Hook PathFindOnPathW for shlwapi resolution
BOOL WINAPI PathFindOnPathW_Hook(LPWSTR pszPath, PZPCWSTR ppszOtherDirs) {
    if (g_inHook || !pszPath || pszPath[0] == L'\0') {
        return PathFindOnPathW_Original(pszPath, ppszOtherDirs);
    }

    ModSettings settings = GetSettings();
    MatchedAlias match;
    if (MatchAlias(pszPath, nullptr, match, settings)) {
        Wh_Log(L"PathFindOnPathW intercepted alias: %s", pszPath);
        WCHAR sysDir[MAX_PATH];
        GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir));
        std::wstring fakeExe = match.aliasToken + L".exe";
        PathCombineW(pszPath, sysDir, fakeExe.c_str());
        return TRUE;
    }

    return PathFindOnPathW_Original(pszPath, ppszOtherDirs);
}

// Hook for ShellExecuteExW
BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* pExecInfo) {
    if (g_inHook || !pExecInfo) {
        return ShellExecuteExW_Original(pExecInfo);
    }

    if (pExecInfo->lpFile && HandleExecution(pExecInfo->hwnd, pExecInfo->lpFile, pExecInfo->lpParameters, pExecInfo->lpDirectory)) {
        pExecInfo->hInstApp = (HINSTANCE)42;
        return TRUE;
    }

    return ShellExecuteExW_Original(pExecInfo);
}

// Hook for ShellExecuteW
HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) {
    if (g_inHook) {
        return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
    }

    if (lpFile && HandleExecution(hwnd, lpFile, lpParameters, lpDirectory)) {
        return (HINSTANCE)42;
    }

    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Terminal Admin Aliases mod init");
    LoadSettings();

    // Hook SearchPathW (prefer kernelbase.dll so API-set callers are intercepted directly)
    HMODULE hKernelbase = GetModuleHandle(L"kernelbase.dll");
    void* pSearchPathW = hKernelbase ? (void*)GetProcAddress(hKernelbase, "SearchPathW") : nullptr;
    if (!pSearchPathW) {
        HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
        if (hKernel32) {
            pSearchPathW = (void*)GetProcAddress(hKernel32, "SearchPathW");
        }
    }
    if (pSearchPathW) {
        Wh_SetFunctionHook(pSearchPathW, (void*)SearchPathW_Hook, (void**)&SearchPathW_Original);
    }

    // Hook PathFindOnPathW
    HMODULE hShlwapi = GetModuleHandle(L"shlwapi.dll");
    if (!hShlwapi) hShlwapi = LoadLibrary(L"shlwapi.dll");
    if (hShlwapi) {
        void* pPathFindOnPathW = (void*)GetProcAddress(hShlwapi, "PathFindOnPathW");
        if (pPathFindOnPathW) {
            Wh_SetFunctionHook(pPathFindOnPathW, (void*)PathFindOnPathW_Hook, (void**)&PathFindOnPathW_Original);
        }
    }

    // Hook ShellExecuteExW and ShellExecuteW
    HMODULE hShell32 = GetModuleHandle(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibrary(L"shell32.dll");
    if (hShell32) {
        void* pShellExecuteExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
        if (pShellExecuteExW) {
            Wh_SetFunctionHook(pShellExecuteExW, (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original);
        }

        void* pShellExecuteW = (void*)GetProcAddress(hShell32, "ShellExecuteW");
        if (pShellExecuteW) {
            Wh_SetFunctionHook(pShellExecuteW, (void*)ShellExecuteW_Hook, (void**)&ShellExecuteW_Original);
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Terminal Admin Aliases mod uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading");
    LoadSettings();
}

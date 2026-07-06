// ==WindhawkMod==
// @id             pivotlink-browser-router
// @name           PivotLink: Browser Router
// @description    Lightweight link redirection tool with an intuitive 5-tier ranked configuration layout and automatic game detection.
// @version        1.0
// @author         You
// @github         https://github.com/gauthumj
// @include        *
// @compilerOptions -lshell32
// @license        MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Ever had a browser open already and Windows opened a Discord/Slack link in your default browser by launching it from scratch? Now it won't.

## What It Does

PivotLink intercepts outgoing URL launches system-wide and redirects them to whichever browser you already have running, based on a 5-tier priority list you configure. Instead of Windows blindly spawning your default browser, PivotLink checks what's actually open and sends the link there.

## How It Works

- Hooks `ShellExecuteW`, `ShellExecuteExW`, and `CreateProcessW` to catch URL opens before they reach the OS default handler.
- Scans the running process list using a single-pass converging search to find the highest-priority browser that's already active.
- If a match is found, the link is silently rerouted to that browser. If none of your ranked browsers are running, the call falls through to normal Windows behavior.
- Circular routing is prevented — if you're already inside the target browser, the hook steps aside.

## Configuration

- **Priority 1–5 Browsers**: Rank up to five browsers by executable name (e.g. `brave.exe`, `firefox.exe`). The first one found running wins.
- **Gaming Override Allow List**: The mod auto-disables in any process that loads DirectX or Vulkan graphics DLLs (i.e. games). Add process names here to keep the mod active in specific games anyway (e.g. games with in-game browsers).

## Notes

- The mod skips Session 0 processes (system services) automatically.
- Game detection works by checking for loaded `d3d9.dll`, `d3d11.dll`, `d3d12.dll`, and `vulkan-1.dll`. Configured browsers are excluded from this check since they load D3D for hardware acceleration.
- A thread-local guard prevents recursive hook calls during redirection.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- browser1: "brave.exe"
  $name: Priority 1 Browser (Highest)
  $description: Your primary choice for link redirection (e.g., brave.exe). Leave blank to skip.
- browser2: "firefox.exe"
  $name: Priority 2 Browser
  $description: Second choice browser if Priority 1 is not running. Leave blank to skip.
- browser3: "chrome.exe"
  $name: Priority 3 Browser
  $description: Third choice browser if higher priorities are closed. Leave blank to skip.
- browser4: "msedge.exe"
  $name: Priority 4 Browser
  $description: Fourth choice browser fallback. Leave blank to skip.
- browser5: ""
  $name: Priority 5 Browser (Lowest)
  $description: Fifth choice browser fallback. Leave blank to skip.
- allowGameProcesses: ""
  $name: Gaming Override Allow List
  $description: The mod auto-disables in processes that load DirectX/Vulkan. Add process names here (semicolon-separated) to keep the mod active in specific games (e.g., games with in-game browsers).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <sstream>

std::vector<std::wstring> g_priorityBrowsers;
std::vector<std::wstring> g_allowedGameProcesses;
thread_local bool t_inHook = false; 

std::wstring TrimString(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Checks if a process has at least one visible top-level window (i.e., is truly "open")
struct VisibleWindowCheck {
    DWORD processId;
    bool found;
};

static BOOL CALLBACK CheckVisibleWindowProc(HWND hwnd, LPARAM lParam) {
    VisibleWindowCheck* check = reinterpret_cast<VisibleWindowCheck*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == check->processId && IsWindowVisible(hwnd)) {
        RECT rect;
        if (GetWindowRect(hwnd, &rect) && (rect.right - rect.left) > 1 && (rect.bottom - rect.top) > 1) {
            check->found = true;
            return FALSE;
        }
    }
    return TRUE;
}

static bool HasVisibleWindow(DWORD processId) {
    VisibleWindowCheck check = { processId, false };
    EnumWindows(CheckVisibleWindowProc, reinterpret_cast<LPARAM>(&check));
    return check.found;
}

// Finds the highest-priority browser that is actively open (has a visible window).
// Background-only processes (e.g., Edge service workers) are ignored.
std::wstring GetHighestPriorityRunningBrowser() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return L"";

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    
    std::vector<std::vector<DWORD>> browserPids(g_priorityBrowsers.size());

    if (Process32FirstW(hSnap, &pe)) {
        do {
            for (size_t i = 0; i < g_priorityBrowsers.size(); ++i) {
                if (_wcsicmp(pe.szExeFile, g_priorityBrowsers[i].c_str()) == 0) {
                    browserPids[i].push_back(pe.th32ProcessID);
                    break;
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);

    for (size_t i = 0; i < g_priorityBrowsers.size(); ++i) {
        for (DWORD pid : browserPids[i]) {
            if (HasVisibleWindow(pid)) {
                return g_priorityBrowsers[i];
            }
        }
    }
    
    return L"";
}

std::wstring GetCurrentProcessName() {
    WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH)) {
        std::wstring sPath(path);
        size_t pos = sPath.find_last_of(L"\\/");
        if (pos != std::wstring::npos) return sPath.substr(pos + 1);
        return sPath;
    }
    return L"UNKNOWN";
}

// Detects if the current process is a game by checking for loaded graphics DLLs.
// Returns false for configured browsers (which load D3D for HW acceleration)
// and for processes explicitly allowed by the user.
bool IsCurrentProcessGame() {
    const WCHAR* graphicsDlls[] = {
        L"d3d9.dll",
        L"d3d11.dll",
        L"d3d12.dll",
        L"vulkan-1.dll",
    };

    bool hasGraphics = false;
    for (const auto& dll : graphicsDlls) {
        if (GetModuleHandleW(dll)) {
            hasGraphics = true;
            break;
        }
    }
    if (!hasGraphics) return false;

    // Browsers load D3D for hardware acceleration — don't flag them as games
    std::wstring currentProc = GetCurrentProcessName();
    for (const auto& browser : g_priorityBrowsers) {
        if (_wcsicmp(currentProc.c_str(), browser.c_str()) == 0) {
            return false;
        }
    }

    for (const auto& allowed : g_allowedGameProcesses) {
        if (_wcsicmp(currentProc.c_str(), allowed.c_str()) == 0) {
            return false;
        }
    }

    return true;
}

void ParseSemicolonList(PCWSTR settingName, std::vector<std::wstring>& targetVector) {
    targetVector.clear();
    PCWSTR rawSetting = Wh_GetStringSetting(settingName);
    
    if (rawSetting && wcslen(rawSetting) > 0) {
        std::wstring s(rawSetting);
        std::wstringstream ss(s);
        std::wstring item;
        while (std::getline(ss, item, L';')) {
            std::wstring trimmed = TrimString(item);
            if (!trimmed.empty()) targetVector.push_back(trimmed);
        }
        Wh_FreeStringSetting(rawSetting);
    }
}

void LoadSettings() {
    g_priorityBrowsers.clear();

    const WCHAR* browserKeys[] = { L"browser1", L"browser2", L"browser3", L"browser4", L"browser5" };
    const std::wstring hardcodedDefaults[] = { L"brave.exe", L"firefox.exe", L"chrome.exe", L"msedge.exe", L"" };

    for (int i = 0; i < 5; ++i) {
        PCWSTR rawBrowser = Wh_GetStringSetting(browserKeys[i]);
        if (rawBrowser) {
            std::wstring trimmed = TrimString(rawBrowser);
            if (!trimmed.empty()) {
                g_priorityBrowsers.push_back(trimmed);
            }
            Wh_FreeStringSetting(rawBrowser);
        } else if (!hardcodedDefaults[i].empty()) {
            g_priorityBrowsers.push_back(hardcodedDefaults[i]);
        }
    }

    if (g_priorityBrowsers.empty()) {
        g_priorityBrowsers = { L"brave.exe", L"firefox.exe", L"chrome.exe", L"msedge.exe" };
    }

    ParseSemicolonList(L"allowGameProcesses", g_allowedGameProcesses);
}

bool RouteLinkIfNecessary(const WCHAR* lpFile, const WCHAR* lpParameters, int nShow) {
    if (!lpFile || t_inHook) return false;
    if (IsCurrentProcessGame()) return false;

    bool isLink = (_wcsnicmp(lpFile, L"http://", 7) == 0 || _wcsnicmp(lpFile, L"https://", 8) == 0);
    if (!isLink) return false;

    std::wstring currentProc = GetCurrentProcessName();
    std::wstring targetBrowser = GetHighestPriorityRunningBrowser();

    if (!targetBrowser.empty()) {
        // Prevent circular routing inside the target browser
        if (_wcsicmp(currentProc.c_str(), targetBrowser.c_str()) == 0) {
            return false;
        }

        std::wstring cleanUrl = lpFile;
        if (cleanUrl.length() >= 2 && cleanUrl.front() == L'"' && cleanUrl.back() == L'"') {
            cleanUrl = cleanUrl.substr(1, cleanUrl.length() - 2);
        }

        Wh_Log(L"[PivotLink] Routing link to: %s", targetBrowser.c_str());

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpFile = targetBrowser.c_str();         
        sei.lpParameters = cleanUrl.c_str(); 
        sei.nShow = nShow;
        
        t_inHook = true;
        BOOL success = ShellExecuteExW(&sei);
        t_inHook = false;

        if (success) return true;
    }
    return false;
}

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(LPSHELLEXECUTEINFOW pExecInfo) {
    if (pExecInfo && pExecInfo->lpFile) {
        if (RouteLinkIfNecessary(pExecInfo->lpFile, pExecInfo->lpParameters, pExecInfo->nShow)) {
            pExecInfo->hInstApp = (HINSTANCE)32; 
            return TRUE;
        }
    }
    return ShellExecuteExW_Original(pExecInfo);
}

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShow) {
    if (RouteLinkIfNecessary(lpFile, lpParameters, nShow)) {
        return (HINSTANCE)32;
    }
    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShow);
}

using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
                                LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                LPPROCESS_INFORMATION lpProcessInformation) {
    if (t_inHook || IsCurrentProcessGame()) {
        return CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                       bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

    bool targetMatched = false;
    if (lpCommandLine && wcsstr(lpCommandLine, L"zen.exe")) targetMatched = true;
    else if (lpApplicationName && wcsstr(lpApplicationName, L"zen.exe")) targetMatched = true;

    if (targetMatched && lpCommandLine && (wcsstr(lpCommandLine, L"http://") || wcsstr(lpCommandLine, L"https://"))) {
        
        std::wstring cmdLine(lpCommandLine);
        size_t urlPos = cmdLine.find(L"https://");
        if (urlPos == std::wstring::npos) urlPos = cmdLine.find(L"http://");

        if (urlPos != std::wstring::npos) {
            std::wstring extractedUrl = cmdLine.substr(urlPos);
            
            size_t cleanupQuote = extractedUrl.find(L'"');
            if (cleanupQuote != std::wstring::npos) extractedUrl = extractedUrl.substr(0, cleanupQuote);
            size_t cleanupSpace = extractedUrl.find(L' ');
            if (cleanupSpace != std::wstring::npos) extractedUrl = extractedUrl.substr(0, cleanupSpace);

            std::wstring targetBrowser = GetHighestPriorityRunningBrowser();

            if (!targetBrowser.empty() && _wcsicmp(targetBrowser.c_str(), L"zen.exe") != 0) {
                
                Wh_Log(L"[PivotLink] CreateProcessW intercept, routing to: %s", targetBrowser.c_str());

                SHELLEXECUTEINFOW sei = { sizeof(sei) };
                sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
                sei.lpFile = targetBrowser.c_str();         
                sei.lpParameters = extractedUrl.c_str(); 
                sei.nShow = SW_SHOWNORMAL;
                
                t_inHook = true;
                BOOL ok = ShellExecuteExW(&sei);
                t_inHook = false;

                if (ok) {
                    if (lpProcessInformation) {
                        lpProcessInformation->hProcess = sei.hProcess;
                        lpProcessInformation->hThread = NULL; 
                        lpProcessInformation->dwProcessId = GetProcessId(sei.hProcess);
                        lpProcessInformation->dwThreadId = 0;
                    }
                    return TRUE;
                }
            }
        }
    }

    return CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL Wh_ModInit() {
    // Session 0 bypass
    DWORD dwSessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &dwSessionId) && dwSessionId == 0) {
        return FALSE; 
    }

    LoadSettings();

    if (IsCurrentProcessGame()) {
        Wh_Log(L"[PivotLink] Graphics DLLs detected, disabling in game process.");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original);
    Wh_SetFunctionHook((void*)ShellExecuteW, (void*)ShellExecuteW_Hook, (void**)&ShellExecuteW_Original);
    Wh_SetFunctionHook((void*)CreateProcessW, (void*)CreateProcessW_Hook, (void**)&CreateProcessW_Original);

    return TRUE;
}

void Wh_ModUninit() {
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
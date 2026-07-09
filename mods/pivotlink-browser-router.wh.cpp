// ==WindhawkMod==
// @id             pivotlink-browser-router
// @name           PivotLink: Browser Router
// @description    Lightweight link redirection tool with an intuitive 5-tier ranked configuration layout.
// @version        1.0
// @author         gauthumj
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

## Notes

- The mod skips Session 0 processes (system services) automatically.
- A thread-local guard prevents recursive hook calls during redirection.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- browser1: "brave.exe"
  $name: Priority 1 Browser (Highest)
  $description: Ideally your default browser — but any browser works. Links route to the highest-priority one that's running.
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
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>
#include <mutex>
#include <vector>
#include <string>

std::mutex g_settingsMutex;
std::vector<std::wstring> g_priorityBrowsers;
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
    if (pid != check->processId) return TRUE;

    if (!IsWindowVisible(hwnd)) return TRUE;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;

    RECT rect = {};
    GetWindowRect(hwnd, &rect);
    if ((rect.right - rect.left) <= 1 || (rect.bottom - rect.top) <= 1) return TRUE;

    check->found = true;
    return FALSE;
}

static bool HasVisibleWindow(DWORD processId) {
    VisibleWindowCheck check = { processId, false };
    EnumWindows(CheckVisibleWindowProc, reinterpret_cast<LPARAM>(&check));
    return check.found;
}

// Finds the highest-priority browser that is actively open (has a visible window).
// Background-only processes (e.g., Edge service workers) are ignored.
std::wstring GetHighestPriorityRunningBrowser() {
    std::vector<std::wstring> browsers;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        browsers = g_priorityBrowsers;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return L"";

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    
    std::vector<std::vector<DWORD>> browserPids(browsers.size());

    if (Process32FirstW(hSnap, &pe)) {
        do {
            for (size_t i = 0; i < browsers.size(); ++i) {
                if (_wcsicmp(pe.szExeFile, browsers[i].c_str()) == 0) {
                    browserPids[i].push_back(pe.th32ProcessID);
                    break;
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);

    for (size_t i = 0; i < browsers.size(); ++i) {
        for (DWORD pid : browserPids[i]) {
            if (HasVisibleWindow(pid)) return browsers[i];
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

void LoadSettings() {
    const WCHAR* browserKeys[] = { L"browser1", L"browser2", L"browser3", L"browser4", L"browser5" };

    std::vector<std::wstring> browsers;
    for (int i = 0; i < 5; ++i) {
        auto setting = WindhawkUtils::StringSetting::make(browserKeys[i]);
        std::wstring trimmed = TrimString(setting.get());
        if (!trimmed.empty()) {
            browsers.push_back(trimmed);
        }
    }

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_priorityBrowsers = std::move(browsers);
}

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

bool RouteLinkIfNecessary(const WCHAR* lpFile, const WCHAR* lpVerb, const WCHAR* lpParameters, int nShow) {
    if (!lpFile || t_inHook) return false;

    // Only redirect default (NULL) or "open" verbs
    if (lpVerb && _wcsicmp(lpVerb, L"open") != 0) return false;

    // Strip surrounding quotes — some apps (Discord, VSCodium) pass quoted URLs
    std::wstring cleanUrl = lpFile;
    if (cleanUrl.length() >= 2 && cleanUrl.front() == L'"' && cleanUrl.back() == L'"') {
        cleanUrl = cleanUrl.substr(1, cleanUrl.length() - 2);
    }

    bool isLink = (_wcsnicmp(cleanUrl.c_str(), L"http://", 7) == 0 || _wcsnicmp(cleanUrl.c_str(), L"https://", 8) == 0);
    if (!isLink) return false;

    std::wstring currentProc = GetCurrentProcessName();
    std::wstring targetBrowser = GetHighestPriorityRunningBrowser();

    if (!targetBrowser.empty()) {
        // Prevent circular routing inside the target browser
        if (_wcsicmp(currentProc.c_str(), targetBrowser.c_str()) == 0) {
            return false;
        }

        Wh_Log(L"Routing link to: %s", targetBrowser.c_str());

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpFile = targetBrowser.c_str();         
        sei.lpParameters = cleanUrl.c_str(); 
        sei.nShow = nShow;
        
        t_inHook = true;
        BOOL success = ShellExecuteExW_Original(&sei);
        t_inHook = false;

        if (success) return true;
    }
    return false;
}

BOOL WINAPI ShellExecuteExW_Hook(LPSHELLEXECUTEINFOW pExecInfo) {
    if (pExecInfo && pExecInfo->lpFile) {
        if (RouteLinkIfNecessary(pExecInfo->lpFile, pExecInfo->lpVerb, pExecInfo->lpParameters, pExecInfo->nShow)) {
            pExecInfo->hInstApp = (HINSTANCE)33; 
            return TRUE;
        }
    }
    return ShellExecuteExW_Original(pExecInfo);
}

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_Original;

// Resolve full path of a browser via Windows App Paths registry
std::wstring GetBrowserFullPath(const std::wstring& exeName) {
    std::wstring keyPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + exeName;
    WCHAR path[MAX_PATH] = {};
    DWORD size = sizeof(path);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, keyPath.c_str(), NULL, RRF_RT_REG_SZ, NULL, path, &size) == ERROR_SUCCESS)
        return path;
    size = sizeof(path);
    if (RegGetValueW(HKEY_CURRENT_USER, keyPath.c_str(), NULL, RRF_RT_REG_SZ, NULL, path, &size) == ERROR_SUCCESS)
        return path;
    return L"";
}

HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShow) {
    if (RouteLinkIfNecessary(lpFile, lpOperation, lpParameters, nShow)) {
        return (HINSTANCE)33;
    }
    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShow);
}

BOOL WINAPI CreateProcessW_Hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation)
{
    if (lpCommandLine && !t_inHook) {
        // Skip if the calling process is itself a configured browser —
        // prevents catching internal browser URLs (cr.brave.com, telemetry)
        // and cascading redirects when the default browser starts up.
        std::wstring currentProc = GetCurrentProcessName();
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            for (const auto& b : g_priorityBrowsers) {
                if (_wcsicmp(currentProc.c_str(), b.c_str()) == 0) {
                    goto passthrough;
                }
            }
        }

        {
            // Determine the target executable name being launched
            std::wstring targetExe;
            if (lpApplicationName) {
                std::wstring appPath(lpApplicationName);
                size_t pos = appPath.find_last_of(L"\\/");
                targetExe = (pos != std::wstring::npos) ? appPath.substr(pos + 1) : appPath;
            } else if (lpCommandLine) {
                std::wstring cl(lpCommandLine);
                size_t start = (cl[0] == L'"') ? 1 : 0;
                size_t end = (cl[0] == L'"') ? cl.find(L'"', 1) : cl.find_first_of(L" \t");
                std::wstring appPath = (end != std::wstring::npos) ? cl.substr(start, end - start) : cl.substr(start);
                size_t pos = appPath.find_last_of(L"\\/");
                targetExe = (pos != std::wstring::npos) ? appPath.substr(pos + 1) : appPath;
            }

            // Only intercept if the target process is a browser in our priority list.
            // This prevents false positives from git, curl, etc. that have URLs in args.
            bool targetIsBrowser = false;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                for (const auto& b : g_priorityBrowsers) {
                    if (_wcsicmp(targetExe.c_str(), b.c_str()) == 0) {
                        targetIsBrowser = true;
                        break;
                    }
                }
            }
            if (!targetIsBrowser) goto passthrough;

            std::wstring cmdLine(lpCommandLine);

            // Quick scan for URL in command line
            std::wstring::size_type urlPos = cmdLine.find(L"https://");
            if (urlPos == std::wstring::npos)
                urlPos = cmdLine.find(L"http://");

            if (urlPos != std::wstring::npos) {
                // Extract URL (may be quoted or unquoted)
                std::wstring url;
                size_t end = cmdLine.find_first_of(L" \t\"", urlPos);
                url = (end != std::wstring::npos)
                    ? cmdLine.substr(urlPos, end - urlPos)
                    : cmdLine.substr(urlPos);

                std::wstring targetBrowser = GetHighestPriorityRunningBrowser();
                if (!targetBrowser.empty() &&
                    _wcsicmp(currentProc.c_str(), targetBrowser.c_str()) != 0) {

                    std::wstring targetPath = GetBrowserFullPath(targetBrowser);
                    if (!targetPath.empty()) {
                        Wh_Log(L"Rewriting CreateProcessW to %s", targetBrowser.c_str());

                        // Replace the browser in the command line
                        std::wstring newCmdLine = L"\"" + targetPath + L"\" " + url;
                        std::vector<wchar_t> cmdBuf(newCmdLine.begin(), newCmdLine.end());
                        cmdBuf.push_back(L'\0');

                        return CreateProcessW_Original(
                            targetPath.c_str(), cmdBuf.data(),
                            lpProcessAttributes, lpThreadAttributes,
                            bInheritHandles, dwCreationFlags, lpEnvironment,
                            lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
                    }
                }
            }
        }
    }

passthrough:
    return CreateProcessW_Original(lpApplicationName, lpCommandLine,
        lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        dwCreationFlags, lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation);
}

BOOL Wh_ModInit() {
    // Session 0 bypass
    DWORD dwSessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &dwSessionId) && dwSessionId == 0) {
        return FALSE; 
    }

    LoadSettings();

    WindhawkUtils::SetFunctionHook(ShellExecuteExW, ShellExecuteExW_Hook, &ShellExecuteExW_Original);
    WindhawkUtils::SetFunctionHook(ShellExecuteW, ShellExecuteW_Hook, &ShellExecuteW_Original);
    WindhawkUtils::SetFunctionHook(CreateProcessW, CreateProcessW_Hook, &CreateProcessW_Original);
    return TRUE;
}

void Wh_ModUninit() {
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
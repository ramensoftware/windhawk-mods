// ==WindhawkMod==
// @id             pivotlink-browser-router
// @name           PivotLink: Browser Router
// @description    Lightweight link redirection tool with an intuitive 5-tier ranked configuration layout.
// @version        1.1
// @author         gauthumj
// @github         https://github.com/gauthumj
// @homepage       https://www.gauthumj.in/
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

- Hooks `ShellExecuteW`, `ShellExecuteExW`, `ShellExecuteA`, `ShellExecuteExA`, and `CreateProcessW` to catch URL opens before they reach the OS default handler.
- Scans the running process list using a single-pass converging search to find the highest-priority browser that's already active.
- If a match is found, the link is silently rerouted to that browser. If none of your ranked browsers are running, the call falls through to normal Windows behavior.
- Circular routing is prevented — if you're already inside the target browser, the hook steps aside.

## Configuration

- **Priority 1–5 Browsers**: Rank up to five browsers by executable name (e.g. `brave.exe`, `firefox.exe`). The first one found running wins.
- **Bypass Method**: Choose how to skip routing and send a link to the OS default browser instead. Default is Mouse Back + Click (press both simultaneously).

## Notes

- The mod skips Session 0 processes (system services) automatically.
- A thread-local guard prevents recursive hook calls during redirection.

If you'd like to see other features or have suggestions, feel free to open an issue on the [GitHub repository](https://github.com/gauthumj/pivotlink-browser-router)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- browser1: "brave.exe"
  $name: Priority 1 Browser (Highest)
  $description: Executable name (e.g. brave.exe) or full path for portable installs (e.g. C:\\Browsers\\firefox.exe).
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
- bypassMethod: xbutton1
  $name: Bypass Method
  $description: Skip routing and let the OS default browser handle a link.
  $options:
  - xbutton1: Mouse Back + Click (press together)
  - xbutton2: Mouse Forward + Click (press together)
  - rightclick: Right + Left Click (press together)
  - ctrl: Ctrl (hold while clicking — tab may open in background)
  - none: Disabled
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>

std::mutex g_settingsMutex;
std::vector<std::wstring> g_priorityBrowsers;

enum class BypassMethod { None, XButton1, XButton2, RightClick, Ctrl };
std::atomic<BypassMethod> g_bypassMethod{BypassMethod::XButton1};

struct BypassSharedState {
    volatile LONG64 lastActiveTickCount;
};
HANDLE g_hSharedMem = NULL;
BypassSharedState* g_pSharedState = NULL;
HANDLE g_hPollerThread = NULL;
HANDLE g_hPollerStopEvent = NULL;

thread_local bool t_inHook = false; 

std::wstring TrimString(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Extract just the filename from a browser setting (which may be a full path or just an exe name)
static std::wstring GetExeName(const std::wstring& browserEntry) {
    size_t pos = browserEntry.find_last_of(L"\\/");
    return (pos != std::wstring::npos) ? browserEntry.substr(pos + 1) : browserEntry;
}

// Resolve full path of a browser. If the setting already contains a backslash,
// treat it as a full path (supports portable installs). Otherwise consult App Paths.
std::wstring GetBrowserFullPath(const std::wstring& exeName) {
    if (exeName.find(L'\\') != std::wstring::npos)
        return exeName;

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

// Finds the highest-priority browser that is actively open (has a visible window).
// Background-only processes (e.g., Edge service workers) are ignored.
// Uses a single EnumWindows pass to collect PIDs with qualifying windows.
struct WindowOwnerCollector {
    std::vector<DWORD> pidsWithWindows;
};

static BOOL CALLBACK CollectWindowOwners(HWND hwnd, LPARAM lParam) {
    WindowOwnerCollector* collector = reinterpret_cast<WindowOwnerCollector*>(lParam);

    if (!IsWindowVisible(hwnd)) return TRUE;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;

    RECT rect = {};
    GetWindowRect(hwnd, &rect);
    if ((rect.right - rect.left) <= 1 || (rect.bottom - rect.top) <= 1) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    collector->pidsWithWindows.push_back(pid);
    return TRUE;
}

std::wstring GetHighestPriorityRunningBrowser() {
    std::vector<std::wstring> browsers;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        browsers = g_priorityBrowsers;
    }

    // Single EnumWindows pass: collect all PIDs that own a qualifying window
    WindowOwnerCollector collector;
    EnumWindows(CollectWindowOwners, reinterpret_cast<LPARAM>(&collector));

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return L"";

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);

    std::vector<std::vector<DWORD>> browserPids(browsers.size());

    if (Process32FirstW(hSnap, &pe)) {
        do {
            for (size_t i = 0; i < browsers.size(); ++i) {
                if (_wcsicmp(pe.szExeFile, GetExeName(browsers[i]).c_str()) == 0) {
                    browserPids[i].push_back(pe.th32ProcessID);
                    break;
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);

    // Check priority order: first browser that has a PID in the visible-window set wins
    for (size_t i = 0; i < browsers.size(); ++i) {
        for (DWORD pid : browserPids[i]) {
            for (DWORD visiblePid : collector.pidsWithWindows) {
                if (pid == visiblePid) return browsers[i];
            }
        }
    }

    return L"";
}


const std::wstring& GetCurrentProcessName() {
    static std::wstring name = []() -> std::wstring {
        WCHAR path[MAX_PATH];
        if (GetModuleFileNameW(NULL, path, MAX_PATH)) {
            std::wstring sPath(path);
            size_t pos = sPath.find_last_of(L"\\/");
            if (pos != std::wstring::npos) return sPath.substr(pos + 1);
            return sPath;
        }
        return L"UNKNOWN";
    }();
    return name;
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

    auto bypassSetting = WindhawkUtils::StringSetting::make(L"bypassMethod");
    std::wstring bypassStr = TrimString(bypassSetting.get());
    BypassMethod method = BypassMethod::XButton1;
    if (_wcsicmp(bypassStr.c_str(), L"xbutton2") == 0) method = BypassMethod::XButton2;
    else if (_wcsicmp(bypassStr.c_str(), L"rightclick") == 0) method = BypassMethod::RightClick;
    else if (_wcsicmp(bypassStr.c_str(), L"ctrl") == 0) method = BypassMethod::Ctrl;
    else if (_wcsicmp(bypassStr.c_str(), L"none") == 0) method = BypassMethod::None;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_priorityBrowsers = std::move(browsers);
    g_bypassMethod.store(method, std::memory_order_relaxed);
}

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
ShellExecuteExW_t ShellExecuteExW_Original;

static int GetBypassVKey() {
    switch (g_bypassMethod.load(std::memory_order_relaxed)) {
        case BypassMethod::XButton1: return VK_XBUTTON1;
        case BypassMethod::XButton2: return VK_XBUTTON2;
        case BypassMethod::RightClick: return VK_RBUTTON;
        case BypassMethod::Ctrl: return VK_CONTROL;
        default: return 0;
    }
}

static bool IsBypassActive() {
    int vk = GetBypassVKey();
    if (!vk) return false;

    if (GetAsyncKeyState(vk) & 0x8000)
        return true;

    // Fallback for processes where GetAsyncKeyState doesn't work (e.g. MSIX-packaged apps)
    if (g_pSharedState) {
        LONG64 lastActive = InterlockedCompareExchange64(
            &g_pSharedState->lastActiveTickCount, 0, 0);
        if (lastActive > 0 && ((LONG64)GetTickCount64() - lastActive) < 500)
            return true;
    }

    return false;
}

static DWORD WINAPI BypassPollerThread(LPVOID) {
    while (WaitForSingleObject(g_hPollerStopEvent, 50) == WAIT_TIMEOUT) {
        int vk = GetBypassVKey();
        if (vk && (GetAsyncKeyState(vk) & 0x8000) && g_pSharedState)
            InterlockedExchange64(&g_pSharedState->lastActiveTickCount, (LONG64)GetTickCount64());
    }
    return 0;
}

bool RouteLinkIfNecessary(const WCHAR* lpFile, const WCHAR* lpVerb, int nShow) {
    if (!lpFile || t_inHook) return false;

    if (IsBypassActive()) return false;

    // Only redirect default (NULL) or "open" verbs
    if (lpVerb && _wcsicmp(lpVerb, L"open") != 0) return false;

    // Strip surrounding quotes — some apps (Discord, VSCodium) pass quoted URLs
    std::wstring cleanUrl = lpFile;
    if (cleanUrl.length() >= 2 && cleanUrl.front() == L'"' && cleanUrl.back() == L'"') {
        cleanUrl = cleanUrl.substr(1, cleanUrl.length() - 2);
    }

    bool isLink = (_wcsnicmp(cleanUrl.c_str(), L"http://", 7) == 0 || _wcsnicmp(cleanUrl.c_str(), L"https://", 8) == 0);
    if (!isLink) return false;

    // Reject URLs with characters that could break command-line quoting
    if (cleanUrl.find_first_of(L"\" \t\r\n") != std::wstring::npos)
        return false;

    const auto& currentProc = GetCurrentProcessName();
    std::wstring targetBrowser = GetHighestPriorityRunningBrowser();

    if (!targetBrowser.empty()) {
        // Prevent circular routing inside the target browser
        if (_wcsicmp(currentProc.c_str(), GetExeName(targetBrowser).c_str()) == 0) {
            return false;
        }

        Wh_Log(L"Routing link to: %s", targetBrowser.c_str());

        std::wstring targetPath = GetBrowserFullPath(targetBrowser);
        if (targetPath.empty()) return false;

        std::wstring quotedUrl = L"\"" + cleanUrl + L"\"";
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpFile = targetPath.c_str();
        sei.lpParameters = quotedUrl.c_str();
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
        if (RouteLinkIfNecessary(pExecInfo->lpFile, pExecInfo->lpVerb, pExecInfo->nShow)) {
            pExecInfo->hInstApp = (HINSTANCE)33;
            if (pExecInfo->fMask & SEE_MASK_NOCLOSEPROCESS)
                pExecInfo->hProcess = NULL;
            return TRUE;
        }
    }
    return ShellExecuteExW_Original(pExecInfo);
}

using ShellExecuteW_t = decltype(&ShellExecuteW);
ShellExecuteW_t ShellExecuteW_Original;

using ShellExecuteExA_t = decltype(&ShellExecuteExA);
ShellExecuteExA_t ShellExecuteExA_Original;

using ShellExecuteA_t = decltype(&ShellExecuteA);
ShellExecuteA_t ShellExecuteA_Original;

using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_Original;



HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShow) {
    if (RouteLinkIfNecessary(lpFile, lpOperation, nShow)) {
        return (HINSTANCE)33;
    }
    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShow);
}

static std::wstring WideFromAnsi(LPCSTR str) {
    if (!str) return L"";
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    if (len <= 0) return L"";
    std::wstring out(len - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str, -1, &out[0], len);
    return out;
}

BOOL WINAPI ShellExecuteExA_Hook(LPSHELLEXECUTEINFOA pExecInfo) {
    if (pExecInfo && pExecInfo->lpFile) {
        std::wstring file = WideFromAnsi(pExecInfo->lpFile);
        std::wstring verb = WideFromAnsi(pExecInfo->lpVerb);
        if (RouteLinkIfNecessary(file.c_str(),
                pExecInfo->lpVerb ? verb.c_str() : NULL,
                pExecInfo->nShow)) {
            pExecInfo->hInstApp = (HINSTANCE)33;
            if (pExecInfo->fMask & SEE_MASK_NOCLOSEPROCESS)
                pExecInfo->hProcess = NULL;
            return TRUE;
        }
    }
    return ShellExecuteExA_Original(pExecInfo);
}

HINSTANCE WINAPI ShellExecuteA_Hook(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShow) {
    std::wstring file = WideFromAnsi(lpFile);
    std::wstring op = WideFromAnsi(lpOperation);
    if (RouteLinkIfNecessary(file.c_str(),
            lpOperation ? op.c_str() : NULL,
            nShow)) {
        return (HINSTANCE)33;
    }
    return ShellExecuteA_Original(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShow);
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
        const auto& currentProc = GetCurrentProcessName();
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            for (const auto& b : g_priorityBrowsers) {
                if (_wcsicmp(currentProc.c_str(), GetExeName(b).c_str()) == 0) {
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
                    if (_wcsicmp(targetExe.c_str(), GetExeName(b).c_str()) == 0) {
                        targetIsBrowser = true;
                        break;
                    }
                }
            }
            if (!targetIsBrowser) goto passthrough;

            // Check bypass after confirming this is a browser launch with a URL
            if (IsBypassActive()) goto passthrough;

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
                    _wcsicmp(currentProc.c_str(), GetExeName(targetBrowser).c_str()) != 0 &&
                    _wcsicmp(targetExe.c_str(), GetExeName(targetBrowser).c_str()) != 0) {

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

    // Shared memory for bypass state — allows processes where GetAsyncKeyState
    // doesn't work (MSIX-packaged apps) to read bypass state from a poller process.
    if (g_bypassMethod.load(std::memory_order_relaxed) != BypassMethod::None) {
        g_hSharedMem = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL,
            PAGE_READWRITE, 0, sizeof(BypassSharedState), L"Local\\PivotLinkBypassState");
        if (!g_hSharedMem)
            g_hSharedMem = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE,
                L"Local\\PivotLinkBypassState");
        if (g_hSharedMem)
            g_pSharedState = (BypassSharedState*)MapViewOfFile(
                g_hSharedMem, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(BypassSharedState));

        // Only explorer.exe runs the poller thread (single system-wide poller)
        const std::wstring& proc = GetCurrentProcessName();
        if (_wcsicmp(proc.c_str(), L"explorer.exe") == 0 && g_pSharedState) {
            g_hPollerStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
            g_hPollerThread = CreateThread(NULL, 0, BypassPollerThread, NULL, 0, NULL);
        }
    }

    WindhawkUtils::SetFunctionHook(ShellExecuteExW, ShellExecuteExW_Hook, &ShellExecuteExW_Original);
    WindhawkUtils::SetFunctionHook(ShellExecuteW, ShellExecuteW_Hook, &ShellExecuteW_Original);
    WindhawkUtils::SetFunctionHook(ShellExecuteExA, ShellExecuteExA_Hook, &ShellExecuteExA_Original);
    WindhawkUtils::SetFunctionHook(ShellExecuteA, ShellExecuteA_Hook, &ShellExecuteA_Original);

    // Hook kernelbase's CreateProcessW — the single choke point all CreateProcess variants funnel through
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        void* pKB = (void*)GetProcAddress(hKernelBase, "CreateProcessW");
        if (pKB)
            Wh_SetFunctionHook(pKB, (void*)CreateProcessW_Hook, (void**)&CreateProcessW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hPollerStopEvent) {
        SetEvent(g_hPollerStopEvent);
        if (g_hPollerThread) {
            WaitForSingleObject(g_hPollerThread, INFINITE);
            CloseHandle(g_hPollerThread);
        }
        CloseHandle(g_hPollerStopEvent);
    }
    if (g_pSharedState) UnmapViewOfFile((void*)g_pSharedState);
    if (g_hSharedMem) CloseHandle(g_hSharedMem);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    BypassMethod oldMethod = g_bypassMethod.load(std::memory_order_relaxed);
    LoadSettings();
    BypassMethod newMethod = g_bypassMethod.load(std::memory_order_relaxed);

    // Reload when crossing the None <-> non-None boundary
    bool wasNone = (oldMethod == BypassMethod::None);
    bool isNone = (newMethod == BypassMethod::None);
    if (wasNone != isNone) {
        *bReload = TRUE;
    }
    return TRUE;
}
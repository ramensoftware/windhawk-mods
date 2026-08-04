// ==WindhawkMod==
// @id              taskbar-disappearing-tray-icons-fix
// @name            Disappearing Tray Icons Fix
// @description     Fixes missing system tray icons by broadcasting TaskbarCreated when the taskbar initializes.
// @version         1.2
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disappearing Tray Icons Fix

Fixes system tray icons going missing in Windows 11 by rebroadcasting the
`TaskbarCreated` window message.

## The Problem

Windows 11 has a known issue where system tray icons can randomly vanish (not
just hide in the overflow area). This happens because:

1. Apps register tray icons by calling `Shell_NotifyIcon`
2. When `explorer.exe` restarts or the shell initializes, it broadcasts
   `TaskbarCreated` to tell apps to re-register their icons
3. Some apps miss this message due to race conditions, sleep/wake cycles, or
   timing issues
4. Result: icons vanish completely with no way to recover except restarting
   the app

## The Solution

This mod detects when the taskbar is created and rebroadcasts the `TaskbarCreated`
message after a defined delay, giving apps another chance to register their tray
icons. This mod can also run a passive rebroadcast to catch bizarre instances of this
occurring while the taskbar is running.

Processes can be excluded from receiving the rebroadcast by adding their executable
names to the excluded processes setting.

## GDI Leak Fix

When passive broadcast is enabled, this mod hooks icon creation APIs to track and
destroy icons created during each broadcast cycle, preventing GDI handle leaks that
would otherwise crash explorer.exe.

Only the explorer.exe instance that owns the taskbar will activate this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- initialDelaySeconds: 5
  $name: Initial delay (seconds)
  $description: How long to wait after taskbar init before first broadcast
- passiveBroadcast: false
  $name: Enable passive broadcast
  $description: Continue broadcasting periodically after initial broadcast
- intervalSeconds: 5
  $name: Broadcast interval (seconds)
  $description: How often to broadcast in passive mode
- excludedProcesses: [""]
  $name: Excluded processes
  $description: >-
    Executable names that must not receive TaskbarCreated. For example: steam.exe
    Matching is case-insensitive.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct {
    std::atomic<int> initialDelaySeconds;
    std::atomic<bool> passiveBroadcast;
    std::atomic<int> intervalSeconds;
    std::vector<std::wstring> excludedProcessNames;
} g_settings;

UINT g_taskbarCreatedMsg = 0;
HANDLE g_workerThread = nullptr;
DWORD g_taskbarThreadId = 0;

// ============================================================================
// GDI Leak Prevention (active only when passive broadcast is enabled)
// ============================================================================

enum class IconSource : int {
    LoadImageW = 0,
    CreateIconIndirect = 1,
    CopyIcon = 2,
    CreateIconFromResourceEx = 3
};

struct TrackedIcon {
    HICON hIcon;
    IconSource source;
};

std::mutex g_iconMutex;
std::vector<TrackedIcon> g_currentIcons;
std::vector<TrackedIcon> g_previousIcons;
std::atomic<bool> g_trackingEnabled{false};

using LoadImageW_t = HANDLE(WINAPI*)(HINSTANCE, LPCWSTR, UINT, int, int, UINT);
LoadImageW_t LoadImageW_Original = nullptr;

HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst, LPCWSTR name, UINT type,
                               int cx, int cy, UINT fuLoad) {
    HANDLE result = LoadImageW_Original(hInst, name, type, cx, cy, fuLoad);
    if (result && type == IMAGE_ICON && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({(HICON)result, IconSource::LoadImageW});
    }
    return result;
}

using CreateIconIndirect_t = HICON(WINAPI*)(PICONINFO);
CreateIconIndirect_t CreateIconIndirect_Original = nullptr;

HICON WINAPI CreateIconIndirect_Hook(PICONINFO piconinfo) {
    HICON result = CreateIconIndirect_Original(piconinfo);
    if (result && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CreateIconIndirect});
    }
    return result;
}

using CopyIcon_t = HICON(WINAPI*)(HICON);
CopyIcon_t CopyIcon_Original = nullptr;

HICON WINAPI CopyIcon_Hook(HICON hIcon) {
    HICON result = CopyIcon_Original(hIcon);
    if (result && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CopyIcon});
    }
    return result;
}

using CreateIconFromResourceEx_t = HICON(WINAPI*)(PBYTE, DWORD, BOOL, DWORD, int, int, UINT);
CreateIconFromResourceEx_t CreateIconFromResourceEx_Original = nullptr;

HICON WINAPI CreateIconFromResourceEx_Hook(PBYTE presbits, DWORD dwResSize,
                                            BOOL fIcon, DWORD dwVer,
                                            int cxDesired, int cyDesired, UINT Flags) {
    HICON result = CreateIconFromResourceEx_Original(presbits, dwResSize, fIcon,
                                                      dwVer, cxDesired, cyDesired, Flags);
    if (result && fIcon && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CreateIconFromResourceEx});
    }
    return result;
}

void DestroyTrackedIcons() {
    std::vector<TrackedIcon> toDestroy;
    {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        toDestroy = std::move(g_previousIcons);
        g_previousIcons = std::move(g_currentIcons);
        g_currentIcons.clear();
    }

    int destroyed[4] = {0, 0, 0, 0};
    for (const auto& ti : toDestroy) {
        if (ti.hIcon) {
            DestroyIcon(ti.hIcon);
            destroyed[(int)ti.source]++;
        }
    }

    if (destroyed[0] || destroyed[1] || destroyed[2] || destroyed[3]) {
        Wh_Log(L"Destroyed icons: LoadImage=%d CreateIconIndirect=%d CopyIcon=%d CreateIconFromRes=%d",
               destroyed[0], destroyed[1], destroyed[2], destroyed[3]);
    }
}

// ============================================================================

constexpr DWORD PROCESS_PATH_INITIAL_CAPACITY = MAX_PATH;
constexpr DWORD PROCESS_PATH_MAXIMUM_CAPACITY = 32768;
constexpr WCHAR EXECUTABLE_NAME_TRIM_CHARACTERS[] = L" \t\r\n";

struct FilteredBroadcastContext {
    UINT message;
    const std::vector<std::wstring>* excludedProcessNames;
    std::unordered_map<DWORD, bool> excludedProcessCache;
    DWORD postedWindowCount;
    DWORD excludedWindowCount;
    DWORD failedWindowCount;
};

std::wstring NormalizeExecutableName(PCWSTR value) {
    if (!value) {
        return {};
    }

    std::wstring executableName(value);
    size_t firstCharacter =
        executableName.find_first_not_of(EXECUTABLE_NAME_TRIM_CHARACTERS);
    if (firstCharacter == std::wstring::npos) {
        return {};
    }

    size_t lastCharacter =
        executableName.find_last_not_of(EXECUTABLE_NAME_TRIM_CHARACTERS);
    executableName =
        executableName.substr(firstCharacter, lastCharacter - firstCharacter + 1);

    size_t lastSeparator = executableName.find_last_of(L"\\/");
    if (lastSeparator != std::wstring::npos) {
        executableName.erase(0, lastSeparator + 1);
    }

    return executableName;
}

std::wstring GetProcessExecutableName(DWORD processID) {
    HANDLE processHandle =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);
    if (!processHandle) {
        return {};
    }

    DWORD pathCapacity = PROCESS_PATH_INITIAL_CAPACITY;
    while (pathCapacity <= PROCESS_PATH_MAXIMUM_CAPACITY) {
        std::wstring processPath(pathCapacity, L'\0');
        DWORD pathLength = pathCapacity;
        if (QueryFullProcessImageNameW(processHandle, 0, processPath.data(),
                                       &pathLength)) {
            CloseHandle(processHandle);
            processPath.resize(pathLength);
            return NormalizeExecutableName(processPath.c_str());
        }

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER ||
            pathCapacity == PROCESS_PATH_MAXIMUM_CAPACITY) {
            break;
        }

        pathCapacity =
            pathCapacity > PROCESS_PATH_MAXIMUM_CAPACITY / 2
                ? PROCESS_PATH_MAXIMUM_CAPACITY
                : pathCapacity * 2;
    }

    CloseHandle(processHandle);
    return {};
}

bool IsProcessExcluded(DWORD processID,
                       FilteredBroadcastContext* broadcastContext) {
    std::unordered_map<DWORD, bool>::const_iterator cachedResult =
        broadcastContext->excludedProcessCache.find(processID);
    if (cachedResult != broadcastContext->excludedProcessCache.end()) {
        return cachedResult->second;
    }

    std::wstring executableName = GetProcessExecutableName(processID);
    bool excluded = false;
    if (!executableName.empty()) {
        for (const std::wstring& excludedProcessName :
             *broadcastContext->excludedProcessNames) {
            if (CompareStringOrdinal(executableName.c_str(), -1,
                                     excludedProcessName.c_str(), -1,
                                     TRUE) == CSTR_EQUAL) {
                excluded = true;
                break;
            }
        }
    }

    broadcastContext->excludedProcessCache.emplace(processID, excluded);
    return excluded;
}

BOOL CALLBACK PostTaskbarCreatedCallback(HWND window, LPARAM parameter) {
    FilteredBroadcastContext* broadcastContext =
        reinterpret_cast<FilteredBroadcastContext*>(parameter);

    DWORD processID = 0;
    if (GetWindowThreadProcessId(window, &processID) &&
        IsProcessExcluded(processID, broadcastContext)) {
        broadcastContext->excludedWindowCount++;
        return TRUE;
    }

    if (PostMessageW(window, broadcastContext->message, 0, 0)) {
        broadcastContext->postedWindowCount++;
    } else {
        broadcastContext->failedWindowCount++;
    }

    return TRUE;
}

void BroadcastTaskbarCreated() {
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"ERROR: TaskbarCreated message not registered");
        return;
    }

    if (g_trackingEnabled.load()) {
        DestroyTrackedIcons();
    }

    if (g_settings.excludedProcessNames.empty()) {
        Wh_Log(L"Broadcasting TaskbarCreated (msg=%u)", g_taskbarCreatedMsg);
        if (!PostMessageW(HWND_BROADCAST, g_taskbarCreatedMsg, 0, 0)) {
            Wh_Log(L"Failed to broadcast TaskbarCreated (error: %lu)",
                   GetLastError());
        }
        return;
    }

    FilteredBroadcastContext broadcastContext{
        g_taskbarCreatedMsg,
        &g_settings.excludedProcessNames,
        {},
        0,
        0,
        0,
    };

    if (!EnumWindows(PostTaskbarCreatedCallback,
                     reinterpret_cast<LPARAM>(&broadcastContext))) {
        Wh_Log(L"Failed to enumerate windows for TaskbarCreated (error: %lu)",
               GetLastError());
        return;
    }

    Wh_Log(L"Posted TaskbarCreated to %lu windows; excluded %lu; failed %lu",
           broadcastContext.postedWindowCount,
           broadcastContext.excludedWindowCount,
           broadcastContext.failedWindowCount);
}

void LoadSettings() {
    int initialDelay = Wh_GetIntSetting(L"initialDelaySeconds");
    int interval = Wh_GetIntSetting(L"intervalSeconds");

    g_settings.initialDelaySeconds.store(initialDelay < 1 ? 5 : initialDelay);
    g_settings.intervalSeconds.store(interval < 1 ? 5 : interval);
    g_settings.passiveBroadcast.store(Wh_GetIntSetting(L"passiveBroadcast") != 0);

    g_settings.excludedProcessNames.clear();
    for (int index = 0;; index++) {
        PCWSTR setting =
            Wh_GetStringSetting(L"excludedProcesses[%d]", index);
        if (!setting || !*setting) {
            if (setting) {
                Wh_FreeStringSetting(setting);
            }
            break;
        }

        std::wstring executableName = NormalizeExecutableName(setting);
        Wh_FreeStringSetting(setting);

        if (executableName.empty()) {
            continue;
        }

        Wh_Log(L"Excluding TaskbarCreated from %s", executableName.c_str());
        g_settings.excludedProcessNames.push_back(std::move(executableName));
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    // Poll for taskbar creation
    HWND hTaskbarWnd = nullptr;
    while (!hTaskbarWnd) {
        hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!hTaskbarWnd) {
            Sleep(1000);
            continue;
        }

        // Check if we own it
        DWORD dwTaskbarProcessId;
        GetWindowThreadProcessId(hTaskbarWnd, &dwTaskbarProcessId);

        if (dwTaskbarProcessId != GetCurrentProcessId()) {
            Wh_Log(L"Taskbar owned by PID %lu, not us - terminating", dwTaskbarProcessId);
            return 0;
        }

        g_taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
        Wh_Log(L"Found taskbar: %08X (thread %lu)",
               (DWORD)(ULONG_PTR)hTaskbarWnd, g_taskbarThreadId);
    }

    // Initial delay before first broadcast
    int initialDelayMs = g_settings.initialDelaySeconds.load() * 1000;
    Wh_Log(L"Waiting %dms before initial broadcast", initialDelayMs);
    Sleep(initialDelayMs);

    BroadcastTaskbarCreated();

    // Passive broadcast loop
    if (!g_settings.passiveBroadcast.load()) {
        Wh_Log(L"Passive broadcast disabled");
        return 0;
    }

    g_trackingEnabled.store(true);

    int intervalMs = g_settings.intervalSeconds.load() * 1000;
    Wh_Log(L"Starting passive broadcast every %dms", intervalMs);

    MSG msg;
    UINT_PTR timerId = SetTimer(nullptr, 0, intervalMs, nullptr);

    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.message == WM_TIMER && msg.wParam == timerId) {
            BroadcastTaskbarCreated();
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (timerId) {
        KillTimer(nullptr, timerId);
    }

    g_trackingEnabled.store(false);
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing...");

    LoadSettings();

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"Failed to register TaskbarCreated message (error: %lu)", GetLastError());
        return FALSE;
    }

    // Check if taskbar already exists and owned by another process
    HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd) {
        DWORD dwTaskbarProcessId;
        GetWindowThreadProcessId(hTaskbarWnd, &dwTaskbarProcessId);

        if (dwTaskbarProcessId != GetCurrentProcessId()) {
            Wh_Log(L"Taskbar owned by PID %lu, not us - unloading", dwTaskbarProcessId);
            return FALSE;
        }
    }

    // Hook icon APIs only if passive broadcast is enabled (to fix GDI leaks)
    if (g_settings.passiveBroadcast.load()) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* p;

            if ((p = (void*)GetProcAddress(hUser32, "LoadImageW"))) {
                Wh_SetFunctionHook(p, (void*)LoadImageW_Hook, (void**)&LoadImageW_Original);
                Wh_Log(L"Hooked LoadImageW");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CreateIconIndirect"))) {
                Wh_SetFunctionHook(p, (void*)CreateIconIndirect_Hook, (void**)&CreateIconIndirect_Original);
                Wh_Log(L"Hooked CreateIconIndirect");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CopyIcon"))) {
                Wh_SetFunctionHook(p, (void*)CopyIcon_Hook, (void**)&CopyIcon_Original);
                Wh_Log(L"Hooked CopyIcon");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CreateIconFromResourceEx"))) {
                Wh_SetFunctionHook(p, (void*)CreateIconFromResourceEx_Hook, (void**)&CreateIconFromResourceEx_Original);
                Wh_Log(L"Hooked CreateIconFromResourceEx");
            }
        }
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread (error: %lu)", GetLastError());
        return FALSE;
    }

    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModUninit() {
    g_trackingEnabled.store(false);

    if (g_workerThread) {
        PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    // Destroy any remaining tracked icons
    {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        for (const auto& ti : g_currentIcons) {
            if (ti.hIcon) DestroyIcon(ti.hIcon);
        }
        for (const auto& ti : g_previousIcons) {
            if (ti.hIcon) DestroyIcon(ti.hIcon);
        }
        g_currentIcons.clear();
        g_previousIcons.clear();
    }

    Wh_Log(L"Uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}

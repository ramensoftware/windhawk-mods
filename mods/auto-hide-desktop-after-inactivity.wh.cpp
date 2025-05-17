// ==WindhawkMod==
// @id              auto-hide-desktop-after-inactivity
// @name            Auto Hide Desktop After Inactivity
// @description     Automatically hides all windows and shows the desktop after a period of user inactivity
// @version         0.5
// @author          alekseev2014s
// @github          https://github.com/Alekseev2014s
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Hide Desktop After Inactivity

This mod automatically minimizes all open windows and hides the desktop
after a period of user inactivity. It's useful for shared environments,
kiosks, or simply to reduce visual clutter after you're away from your PC.

## Features
- **Inactivity timeout**: Automatically hide the desktop after no input for a configurable number of seconds.
- **Lock screen timeout**: Optionally lock the workstation after a longer period of inactivity.
- **Fullscreen app detection**: Prevents the desktop from being hidden if a fullscreen application is active.
- **Process exclusions**: You can specify a list of executable names (like `vlc.exe`) to ignore during monitoring.

## How it works
- When the system is idle for the specified timeout, the mod simulates a `Win+D` keystroke to hide all windows and show the desktop.
- If configured, after further inactivity, it automatically locks the workstation.
- User input such as mouse movement or keyboard activity restores the previous window state.
- Foreground fullscreen applications and excluded processes will prevent the desktop from being hidden.

## Getting started
1. Install the mod in Windhawk.
2. Adjust the settings for:
   - Timeout (in seconds)
   - Lock timeout (optional)
   - Excluded processes
   - Fullscreen app handling
3. Let the system idle and watch the desktop auto-hide behavior in action.

## Example use cases
- **Privacy**: Automatically hides sensitive windows when away.
- **Kiosk mode**: Resets to desktop after inactivity.
- **Clutter reduction**: Keeps your desktop clean when you're not using the PC.

## Troubleshooting
- If the desktop is not hidden as expected, check that no fullscreen app or excluded process is active.
- To ensure it only runs once, the mod uses a global mutex to avoid interfering with other `explorer.exe` instances.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeoutSeconds: 300
  $name: Inactivity timeout (seconds)
  $description: The number of seconds of inactivity before hiding the desktop.

- lockTimeoutSeconds: 600
  $name: Lock screen timeout (seconds)
  $description: The number of seconds of inactivity before locking the workstation.

- excludeProcesses: ""
  $name: Excluded processes
  $description: Semicolon-separated list of executable names to exclude from triggering desktop show (e.g. "vlc.exe;notepad.exe")

- ignoreFullscreenApps: true
  $name: Ignore fullscreen applications
  $description: Prevent desktop from being shown if a fullscreen application is active.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <vector>

std::vector<HWND> g_minimizedWindows;
std::vector<std::wstring> excludedProcessList;

DWORD setting_timeoutSeconds = 300;
DWORD setting_lockTimeoutSeconds = 600;
bool setting_ignoreFullscreenApps = true;

constexpr DWORD kMouseMoveThreshold = 10;

HANDLE hThread = nullptr;
bool stopThread = false;
bool desktopShownByMod = false;
POINT lastMousePos = {};

HANDLE g_singletonMutex = nullptr;

bool lockPending = false;
DWORD timeDesktopShown = 0;

enum class DesktopState {
    Normal,
    HiddenByMod
};

DesktopState desktopState = DesktopState::Normal;

bool IsDesktopVisible() {
    HWND shellWindow = GetShellWindow();
    HWND foreground = GetForegroundWindow();
    return foreground == shellWindow;
}

void ToggleDesktopWithWinD() {
    keybd_event(VK_LWIN, 0, 0, 0);           // Win down
    keybd_event('D', 0, 0, 0);               // D down
    keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // D up
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Win up
}

void HideDesktop() {
    if (!IsDesktopVisible()) {
        ToggleDesktopWithWinD();
    }
    GetCursorPos(&lastMousePos);
    desktopShownByMod = true;
    timeDesktopShown = GetTickCount();
    lockPending = (setting_lockTimeoutSeconds > 0);
}

void RestoreWindows() {
    if (IsDesktopVisible()) {
        ToggleDesktopWithWinD();
    }

    desktopShownByMod = false;
}

void LoadSettings() {
    int value;
    value = Wh_GetIntSetting(L"timeoutSeconds");
    setting_timeoutSeconds = (value > 0) ? value : 300;

    value = Wh_GetIntSetting(L"lockTimeoutSeconds");
    setting_lockTimeoutSeconds = (value >= 0) ? value : 0;

    setting_ignoreFullscreenApps = Wh_GetIntSetting(L"ignoreFullscreenApps");

    const wchar_t* excludeList = Wh_GetStringSetting(L"excludeProcesses");
    excludedProcessList.clear();
    if (excludeList && *excludeList) {
        std::wstring listStr(excludeList);
        size_t pos = 0;
        while ((pos = listStr.find(L';')) != std::wstring::npos) {
            std::wstring token = listStr.substr(0, pos);
            if (!token.empty())
                excludedProcessList.push_back(token);
            listStr.erase(0, pos + 1);
        }
        if (!listStr.empty())
            excludedProcessList.push_back(listStr);
    }

    Wh_Log(L"Settings loaded: timeoutSeconds = %d, lockTimeoutSeconds = %d",
        setting_timeoutSeconds, setting_lockTimeoutSeconds);
}

bool IsExcludedProcessRunning() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd)
        return false;

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess)
        return false;

    wchar_t exePath[MAX_PATH];
    DWORD size = ARRAYSIZE(exePath);
    bool result = false;

    if (QueryFullProcessImageNameW(hProcess, 0, exePath, &size)) {
        std::wstring exeName = exePath;
        size_t pos = exeName.find_last_of(L"\\/");
        if (pos != std::wstring::npos)
            exeName = exeName.substr(pos + 1);

        Wh_Log(L"Foreground exe: %s", exeName.c_str()); // ADD THIS

        for (const auto& excluded : excludedProcessList) {
            if (_wcsicmp(exeName.c_str(), excluded.c_str()) == 0) {
                result = true;
                break;
            }
        }
    }

    CloseHandle(hProcess);
    return result;
}

bool IsForegroundWindowFullscreen() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetShellWindow())
        return false;

    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    if (!GetMonitorInfo(hMonitor, &monitorInfo))
        return false;

    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);

    return EqualRect(&windowRect, &monitorInfo.rcMonitor);
}

DWORD WINAPI MonitorThread(LPVOID) {
    Wh_Log(L"Monitor thread started");

    DWORD effectiveLastInputTime = GetTickCount();

    while (!stopThread) {
        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        if (GetLastInputInfo(&lii)) {
            DWORD currentTick = GetTickCount();
            DWORD systemLastInputTime = lii.dwTime;

            if (systemLastInputTime > effectiveLastInputTime) {
                effectiveLastInputTime = systemLastInputTime;
            }

            DWORD idleTime = currentTick - effectiveLastInputTime;

            DWORD desktopTimeoutMs = setting_timeoutSeconds * 1000;
            DWORD lockTimeoutMs = setting_lockTimeoutSeconds * 1000;

            if (idleTime >= lockTimeoutMs) {
                Wh_Log(L"Locking workstation");
                RestoreWindows();
                LockWorkStation();
                effectiveLastInputTime = GetTickCount();
                continue;
            }

            if (desktopState == DesktopState::Normal &&
                idleTime >= desktopTimeoutMs)
            {
                if (setting_ignoreFullscreenApps && IsForegroundWindowFullscreen()) {
                    Wh_Log(L"Skipping hide: Fullscreen app detected");
                    effectiveLastInputTime = GetTickCount();
                } else if (IsExcludedProcessRunning()) {
                    Wh_Log(L"Skipping hide: Excluded process detected");
                    effectiveLastInputTime = GetTickCount();
                } else {
                    Wh_Log(L"Inactivity detected, showing desktop");
                    HideDesktop();
                    desktopState = DesktopState::HiddenByMod;
                }
            }

            if (desktopState == DesktopState::HiddenByMod) {
                POINT currentMousePos;
                GetCursorPos(&currentMousePos);
                DWORD dx = abs(currentMousePos.x - lastMousePos.x);
                DWORD dy = abs(currentMousePos.y - lastMousePos.y);

                if (dx >= kMouseMoveThreshold || dy >= kMouseMoveThreshold) {
                    Wh_Log(L"Restoring windows unlock input");
                    RestoreWindows();
                    desktopState = DesktopState::Normal;
                    effectiveLastInputTime = GetTickCount();
                }
            }
        }

        Sleep(200);
    }

    Wh_Log(L"Monitor thread exiting");
    return 0;
}

bool IsMainExplorerProcess() {
    g_singletonMutex = CreateMutexW(nullptr, TRUE, L"Global\\WH_AutoShowDesktop_Mutex");
    if (g_singletonMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
        return false;
    }
    return true;
}

void CleanupSingletonMutex() {
    if (g_singletonMutex) {
        CloseHandle(g_singletonMutex);
        g_singletonMutex = nullptr;
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!IsMainExplorerProcess())
        return TRUE;

    stopThread = false;
    hThread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);

    if (!hThread) {
        CleanupSingletonMutex();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    stopThread = true;

    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = nullptr;
    }

    CleanupSingletonMutex();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// ==WindhawkMod==
// @id              auto-hide-desktop-after-inactivity
// @name            Auto Hide Desktop After Inactivity
// @description     Automatically hides all windows and shows the desktop after a period of user inactivity
// @version         0.6
// @author          alekseev2014s
// @github          https://github.com/Alekseev2014s
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Hide Desktop After Inactivity

This mod automatically minimizes all open windows and shows the desktop
after a period of user inactivity. Optionally, it can also lock the workstation
after continued inactivity. It is useful for shared environments, or simply to reduce visual clutter and protect your session when you're away from your PC.

## Features
- **Inactivity timeout**: Automatically shows the desktop after a configurable number of seconds without input.
- **Lock screen timeout**: Optionally locks the workstation after additional idle time.
- **Fullscreen app detection**: Prevents the desktop from being shown if a fullscreen application is active (optional).
- **Process exclusions**: Prevents desktop from hiding if any excluded program is currently focused.
- **Smart restoration**: Moving the mouse restores the previous window state automatically.

## How it works
- After the inactivity timeout (`timeoutSeconds`), the desktop is shown by simulating the `Win+D` keystroke.
- If `lockTimeoutSeconds` is configured and idle time continues past this value, the workstation is locked.
- The desktop is not shown if a fullscreen app is active (unless disabled), or if a specified excluded process is in the focus.
- After the desktop is shown, moving the mouse restores the previous window state.
- Foreground fullscreen applications and excluded processes prevent activation of the mod.

## Configuration
- `Inactivity timeout (seconds)`: Time of user inactivity after which the desktop will be shown.  
  - Set to **0** to disable automatic desktop hiding.
- `Lock screen timeout (seconds)`: Time of user inactivity after which the workstation will be locked.  
  - Set to **0** to disable automatic locking.
- `Excluded programs`: An array of process names (e.g., `vlc.exe`, `notepad.exe`) that will prevent the desktop from being shown or the system from being locked when focused.
- `Ignore fullscreen applications`: If enabled, fullscreen apps block desktop hiding.

## Notes
- Runs only in the main `explorer.exe` process (the one that owns the taskbar).
- Uses `GetLastInputInfo` to track idle time.
- Foreground process name is compared case-insensitively against the excluded list.
- Fullscreen state is detected via `SHQueryUserNotificationState`.

## Troubleshooting
- If nothing happens after the timeout, ensure:
  - No excluded process is in the foreground.
  - No fullscreen app is active (unless ignored).
  - Mod settings are configured correctly and saved.
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

- excludedPrograms: [excluded1.exe] 
  $name: Excluded programs 
  $description: Semicolon-separated list of executable names to exclude from triggering desktop show (e.g. "vlc.exe;notepad.exe")

- ignoreFullscreenApps: true
  $name: Ignore fullscreen applications
  $description: Prevent desktop from being shown if a fullscreen application is active.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <vector>

#define TIMEIOUT_HIDE_DEFAULT (300)
#define TIMEIOUT_LOCK_DEFAULT (600)

std::vector<HWND> g_minimizedWindows;
std::vector<std::wstring> excludedProcessList;

DWORD setting_timeoutSeconds = TIMEIOUT_HIDE_DEFAULT;
DWORD setting_lockTimeoutSeconds = TIMEIOUT_LOCK_DEFAULT;
bool setting_ignoreFullscreenApps = true;

bool timeoutHide = true;
bool timeoutLock = true;

constexpr DWORD kMouseMoveThreshold = 10;

HANDLE hStopEvent = nullptr;
HANDLE hThread = nullptr;
bool stopThread = false;
bool desktopShownByMod = false;
POINT lastMousePos = {};

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

void LogExcludedProcesses() {
    if (excludedProcessList.empty()) {
        Wh_Log(L"Excluded processes list is empty");
        return;
    }

    std::wstring message = L"Excluded processes:";

    for (const auto& proc : excludedProcessList) {
        message += L" " + proc;
    }

    Wh_Log(L"%s", message.c_str());
}

void LoadSettings() {
    int value;
    value = Wh_GetIntSetting(L"timeoutSeconds");
    if (value > 4) {
        setting_timeoutSeconds = value;
    } else {
        setting_timeoutSeconds = TIMEIOUT_HIDE_DEFAULT;
        timeoutHide = false;
    }

    value = Wh_GetIntSetting(L"lockTimeoutSeconds");
        if (value > 5) {
        setting_lockTimeoutSeconds = value;
    } else {
        setting_lockTimeoutSeconds = TIMEIOUT_LOCK_DEFAULT;
        timeoutLock = false;
    }

    setting_ignoreFullscreenApps = Wh_GetIntSetting(L"ignoreFullscreenApps");

    excludedProcessList.clear();

    for (int i = 0;; i++) {
        wchar_t settingName[64];
        swprintf(settingName, ARRAYSIZE(settingName), L"excludedPrograms[%d]", i);

        const wchar_t* item = Wh_GetStringSetting(settingName);
        if (!item || *item == L'\0') {
            break;
        }
        Wh_FreeStringSetting(item);

        excludedProcessList.emplace_back(item);
    }

    Wh_Log(L"Settings loaded: timeoutSeconds = %d, lockTimeoutSeconds = %d",
        setting_timeoutSeconds, setting_lockTimeoutSeconds);

    LogExcludedProcesses();
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
    QUERY_USER_NOTIFICATION_STATE pquns; 
    if (FAILED(SHQueryUserNotificationState(&pquns))) {
        return false; 
    }
  
    switch (pquns) {
        case QUNS_NOT_PRESENT:
        case QUNS_BUSY:
        case QUNS_RUNNING_D3D_FULL_SCREEN:
            return true;

        case QUNS_PRESENTATION_MODE:
        case QUNS_ACCEPTS_NOTIFICATIONS:
        case QUNS_QUIET_TIME:
        case QUNS_APP:
            return false;

        default:
            Wh_Log(L"Unhandled QUERY_USER_NOTIFICATION_STATE: %d", pquns);
    } 
  
    return false;
}

DWORD WINAPI MonitorThread(LPVOID) {
    Wh_Log(L"Monitor thread started");

    DWORD effectiveLastInputTime = GetTickCount();

    while (true) {
        LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
        DWORD waitTime = 200;

        if (GetLastInputInfo(&lii)) {
            DWORD currentTick = GetTickCount();
            DWORD systemLastInputTime = lii.dwTime;

            if (systemLastInputTime > effectiveLastInputTime) {
                effectiveLastInputTime = systemLastInputTime;
            }

            DWORD idleTime = currentTick - effectiveLastInputTime;

            DWORD desktopTimeoutMs = setting_timeoutSeconds * 1000;
            DWORD lockTimeoutMs = setting_lockTimeoutSeconds * 1000;

            if (desktopState == DesktopState::Normal) {
                DWORD minTimeout = std::min(desktopTimeoutMs, lockTimeoutMs);
                if (idleTime < minTimeout) {
                    waitTime = std::min<DWORD>(minTimeout - idleTime, desktopTimeoutMs);
                } else {
                    waitTime = 0;
                }
            }

            if (timeoutLock && 
                idleTime >= lockTimeoutMs &&
                desktopState == DesktopState::HiddenByMod) {
                Wh_Log(L"Locking workstation");
                RestoreWindows();
                LockWorkStation();
                effectiveLastInputTime = GetTickCount();
                continue;
            }

            if (timeoutHide &&
                desktopState == DesktopState::Normal &&
                idleTime >= desktopTimeoutMs)
            {
                if (setting_ignoreFullscreenApps && IsForegroundWindowFullscreen()) {
                    Wh_Log(L"Skipping hide: Fullscreen app detected");
                    effectiveLastInputTime = GetTickCount();
                } else if (IsExcludedProcessRunning()) {
                    Wh_Log(L"Skipping hide: Excluded process detected");
                    effectiveLastInputTime = GetTickCount();
                } else {
                    Wh_Log(L"Inactivity detected, hiding desktop");
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
        if (waitTime < 200) waitTime = 200;

        DWORD waitResult = WaitForSingleObject(hStopEvent, waitTime);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
    }

    Wh_Log(L"Monitor thread exiting");
    return 0;
}

static BOOL CALLBACK EnumTaskbarWndProc(HWND hWnd, LPARAM lParam)
{
    DWORD pid = 0;
    WCHAR className[32] = { 0 };
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() &&
        GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0)
    {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE; // stop enumeration
    }
    return TRUE; // continue
}

static HWND FindCurrentProcessTaskbarWnd()
{
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumTaskbarWndProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

BOOL Wh_ModInit() {
    if (!FindCurrentProcessTaskbarWnd()) {
        HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (hTaskbarWnd) {
            // The taskbar exists, but it's not owned by the current process.
            return FALSE;
        }
    }

    LoadSettings();

    hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hStopEvent) {
        return FALSE;
    }

    stopThread = false;
    hThread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);

    if (!hThread) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (hStopEvent) {
        SetEvent(hStopEvent);
    }

    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = nullptr;
    }

    if (hStopEvent) {
        CloseHandle(hStopEvent);
        hStopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

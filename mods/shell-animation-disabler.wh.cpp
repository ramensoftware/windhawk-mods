// ==WindhawkMod==
// @id              shell-animation-disabler
// @name            Shell Animation Disabler
// @description     Selectively disable UWP shell animations while preserving other system animations.
// @version         1.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         StartMenuExperienceHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         SearchHost.exe
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shell Animation Disabler

This mod allows you to selectively disable the animations for the start menu, the search menu, the quick settings panel and the notification center.

![](https://i.imgur.com/YeEeM4M.gif)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disable_start: true
  $name: Disable Start Menu Animations
  $description: Turn off flyout animations for the Start Menu.
- disable_search: true
  $name: Disable Search Menu Animations
  $description: Turn off flyout animations for the Search flyout.
- disable_quick_settings: true
  $name: Disable Quick Settings Animations
  $description: Turn off flyout animations for Quick Settings (ShellHost.exe).
- disable_notification_center: true
  $name: Disable Notification Center Animations
  $description: Turn off flyout animations for the Notification Center (ShellExperienceHost.exe).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>
#include <tlhelp32.h>
#include <atomic>

// Global flags and cached process state
std::atomic<bool> g_bDisableAnimations{false};

enum class AppType { Unknown, Explorer, Start, Search, ShellHost, ShellExperience };
AppType g_CurrentApp = AppType::Unknown;

// ==============================================================================
// PROCESS TERMINATION HELPER
// ==============================================================================

void RestartTargetProcesses() {
    // Take one snapshot of the system to avoid CPU spikes
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    const WCHAR* targetProcesses[] = {
        L"StartMenuExperienceHost.exe",
        L"SearchHost.exe",
        L"ShellHost.exe",
        L"ShellExperienceHost.exe"
    };

    if (Process32FirstW(hSnap, &pe32)) {
        do {
            for (const WCHAR* target : targetProcesses) {
                if (_wcsicmp(pe32.szExeFile, target) == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    if (hProcess) {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                    }
                    break; // Found match, stop checking other targets for this process
                }
            }
        } while (Process32NextW(hSnap, &pe32));
    }
    CloseHandle(hSnap);
}

// ==============================================================================
// UTILITY HELPER
// ==============================================================================

// Extracts just the executable name without mutating the original buffer
const WCHAR* GetCurrentExeName(const WCHAR* buffer) {
    const WCHAR* fileName = wcsrchr(buffer, L'\\');
    return fileName ? (fileName + 1) : buffer;
}

// ==============================================================================
// SETTINGS LOADER
// ==============================================================================

void LoadSettings() {
    bool disable = false;
    switch (g_CurrentApp) {
        case AppType::Start:
            disable = Wh_GetIntSetting(L"disable_start") != 0;
            break;
        case AppType::Search:
            disable = Wh_GetIntSetting(L"disable_search") != 0;
            break;
        case AppType::ShellHost:
            disable = Wh_GetIntSetting(L"disable_quick_settings") != 0;
            break;
        case AppType::ShellExperience:
            disable = Wh_GetIntSetting(L"disable_notification_center") != 0;
            break;
        default:
            return; // Not a target process, do nothing
    }
    g_bDisableAnimations.store(disable, std::memory_order_relaxed);
}

// ==============================================================================
// UWP HOOK
// ==============================================================================

typedef BOOL (WINAPI *SystemParametersInfoW_t)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
SystemParametersInfoW_t SystemParametersInfoW_Original;

BOOL WINAPI SystemParametersInfoW_Hook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    // Fast-path: Skip our custom logic entirely if it's not an animation query
    // Use [[likely]] because the vast majority of calls will not match these actions
    if (uiAction != SPI_GETANIMATION && uiAction != SPI_GETCLIENTAREAANIMATION) [[likely]] {
        return SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);
    }

    // Call the original function
    BOOL result = SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);

    // If animations shouldn't be disabled, original call failed, or pointer is null, return early
    if (!g_bDisableAnimations.load(std::memory_order_relaxed) || !result || !pvParam) {
        return result;
    }

    // Modify the out parameters safely
    if (uiAction == SPI_GETANIMATION) {
        ANIMATIONINFO* ai = (ANIMATIONINFO*)pvParam;
        if (ai->cbSize >= sizeof(ANIMATIONINFO)) { 
            ai->iMinAnimate = 0; 
        }
    } else { // SPI_GETCLIENTAREAANIMATION
        *(BOOL*)pvParam = FALSE;
    }

    return result;
}

// ==============================================================================
// TASKBAR HELPER
// ==============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            } 
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
  
    return hTaskbarWnd;
}

// ==============================================================================
// INITIALIZATION, UNINITIALIZATION & SETTINGS UPDATES
// ==============================================================================

BOOL Wh_ModInit() {
    WCHAR pathBuffer[MAX_PATH];
    if (!GetModuleFileNameW(NULL, pathBuffer, MAX_PATH)) return FALSE;
    
    const WCHAR* exeName = GetCurrentExeName(pathBuffer);

    // Cache the application type once to avoid repeated string comparisons
    if (_wcsicmp(exeName, L"explorer.exe") == 0) g_CurrentApp = AppType::Explorer;
    else if (_wcsicmp(exeName, L"StartMenuExperienceHost.exe") == 0) g_CurrentApp = AppType::Start;
    else if (_wcsicmp(exeName, L"SearchHost.exe") == 0) g_CurrentApp = AppType::Search;
    else if (_wcsicmp(exeName, L"ShellHost.exe") == 0) g_CurrentApp = AppType::ShellHost;
    else if (_wcsicmp(exeName, L"ShellExperienceHost.exe") == 0) g_CurrentApp = AppType::ShellExperience;

    LoadSettings();

    if (g_CurrentApp == AppType::Explorer) {
        // Only restart processes if this explorer instance owns the taskbar
        if (FindCurrentProcessTaskbarWnd() != nullptr) {
            RestartTargetProcesses();
        }
    } else {
        Wh_SetFunctionHook((void*)SystemParametersInfoW, (void*)SystemParametersInfoW_Hook, (void**)&SystemParametersInfoW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_CurrentApp == AppType::Explorer && FindCurrentProcessTaskbarWnd() != nullptr) {
        RestartTargetProcesses();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (g_CurrentApp == AppType::Explorer && FindCurrentProcessTaskbarWnd() != nullptr) {
        RestartTargetProcesses();
    }
}

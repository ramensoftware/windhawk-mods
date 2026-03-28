// ==WindhawkMod==
// @id              force-kill-active-window
// @name            Force Kill Active Window
// @description     Press Ctrl+Alt+F4 to forcefully kill the active foreground window.
// @version         1.0
// @author          vfxturjo
// @include         explorer.exe
// @github          https://github.com/zunaidFarouque
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Force Kill Active Window

Instantly terminate any frozen or unresponsive application with a keyboard shortcut. 

By default, pressing **Ctrl+Alt+F4** will execute a hard system kill on the currently active foreground window, bypassing the standard Windows polite closure request.

### ⚠️ Warning
This performs a hard system termination (`TerminateProcess`). The target application will **not** be given a chance to save your work. Use strictly as a last resort.

### Features
* **Customizable Hotkey:** Change the trigger shortcut in the settings. Updates apply instantly.
* **Process Blacklist:** Define apps (like `winword.exe` or `chrome.exe`) that the mod is forbidden from killing to protect your most critical work.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- kill_key: "F4"
  $name: Hotkey Character
  $description: The main key to press (e.g., F4, Q, X, 1).
- req_ctrl: true
  $name: Require Ctrl key
- req_alt: true
  $name: Require Alt key
- req_shift: false
  $name: Require Shift key
- req_win: false
  $name: Require Windows key
- protected_apps: "devenv.exe, winword.exe, excel.exe"
  $name: Protected Applications (Blacklist)
  $description: Comma-separated list of executable names that this mod will refuse to terminate.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <mutex>

#define HOTKEY_ID 1
#define WM_RELOAD_HOTKEY (WM_APP + 1)

HWND g_hHiddenWindow = NULL;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

// Global settings and synchronization mutex
std::mutex g_settingsMutex;
UINT g_modifiers = 0;
UINT g_vkCode = VK_F4;
std::vector<std::wstring> g_blacklist;

UINT ParseKeyCode(PCWSTR keyStr) {
    std::wstring k(keyStr);
    if (k.empty()) return VK_F4;

    if (k.length() == 1) {
        wchar_t c = towupper(k[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return c;
    } else if (k.length() >= 2 && towupper(k[0]) == L'F') {
        int fNum = _wtoi(k.c_str() + 1);
        if (fNum >= 1 && fNum <= 24) return VK_F1 + fNum - 1;
    }
    return VK_F4; 
}

void LoadSettings() {
    // Lock the mutex before modifying global settings
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_modifiers = 0;
    if (Wh_GetIntSetting(L"req_ctrl")) g_modifiers |= MOD_CONTROL;
    if (Wh_GetIntSetting(L"req_alt")) g_modifiers |= MOD_ALT;
    if (Wh_GetIntSetting(L"req_shift")) g_modifiers |= MOD_SHIFT;
    if (Wh_GetIntSetting(L"req_win")) g_modifiers |= MOD_WIN;

    PCWSTR keyStr = Wh_GetStringSetting(L"kill_key");
    g_vkCode = ParseKeyCode(keyStr);
    Wh_FreeStringSetting(keyStr);

    g_blacklist.clear();
    PCWSTR appsStr = Wh_GetStringSetting(L"protected_apps");
    std::wstring rawApps(appsStr);
    Wh_FreeStringSetting(appsStr);

    size_t start = 0, end = 0;
    while ((end = rawApps.find(L',', start)) != std::wstring::npos) {
        std::wstring app = rawApps.substr(start, end - start);
        app.erase(std::remove_if(app.begin(), app.end(), iswspace), app.end()); 
        std::transform(app.begin(), app.end(), app.begin(), ::towlower);
        if (!app.empty()) g_blacklist.push_back(app);
        start = end + 1;
    }
    std::wstring lastApp = rawApps.substr(start);
    lastApp.erase(std::remove_if(lastApp.begin(), lastApp.end(), iswspace), lastApp.end());
    std::transform(lastApp.begin(), lastApp.end(), lastApp.begin(), ::towlower);
    if (!lastApp.empty()) g_blacklist.push_back(lastApp);
}

bool IsProcessProtected(DWORD pid) {
    if (pid <= 4 || pid == GetCurrentProcessId()) return true; 

    // Lock the mutex before reading the blacklist
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    
    if (g_blacklist.empty()) return false;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false; 

    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    bool isProtected = false;

    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        std::wstring wPath(path);
        size_t pos = wPath.find_last_of(L"\\/");
        std::wstring exeName = (pos != std::wstring::npos) ? wPath.substr(pos + 1) : wPath;
        std::transform(exeName.begin(), exeName.end(), exeName.begin(), ::towlower);

        for (const auto& blocked : g_blacklist) {
            if (exeName == blocked) {
                isProtected = true;
                break;
            }
        }
    }
    CloseHandle(hProc);
    return isProtected;
}

LRESULT CALLBACK HiddenWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_HOTKEY && wParam == HOTKEY_ID) {
        HWND hActive = GetForegroundWindow();
        if (!hActive || hActive == GetShellWindow()) return 0;

        DWORD pid = 0;
        GetWindowThreadProcessId(hActive, &pid);
        
        if (IsProcessProtected(pid)) {
            Wh_Log(L"[ForceKill] Target PID %lu is on the protected blacklist. Aborting.", pid);
            return 0;
        }

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess) {
            TerminateProcess(hProcess, 1); 
            CloseHandle(hProcess);
            Wh_Log(L"[ForceKill] Successfully terminated PID: %lu", pid);
        } else {
            Wh_Log(L"[ForceKill] Failed to terminate PID: %lu. (UAC/Access Denied).", pid);
        }
        return 0;
    } 
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI HotkeyThread(LPVOID lpParam) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = HiddenWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkForceKillClass";
    RegisterClass(&wc);

    g_hHiddenWindow = CreateWindowEx(0, wc.lpszClassName, L"WindhawkForceKill", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        RegisterHotKey(g_hHiddenWindow, HOTKEY_ID, g_modifiers, g_vkCode);
    }

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        // Intercept our custom reload message
        if (msg.message == WM_RELOAD_HOTKEY) {
            UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            if (!RegisterHotKey(g_hHiddenWindow, HOTKEY_ID, g_modifiers, g_vkCode)) {
                Wh_Log(L"[ForceKill] Failed to re-register hotkey after settings change.");
            }
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (g_hHiddenWindow) {
        UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
        DestroyWindow(g_hHiddenWindow);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL Wh_ModInit() {
    HWND hShell = GetShellWindow();
    DWORD shellPid = 0;
    if (hShell) GetWindowThreadProcessId(hShell, &shellPid);
    if (shellPid != 0 && shellPid != GetCurrentProcessId()) return TRUE; 

    LoadSettings(); 

    g_hThread = CreateThread(NULL, 0, HotkeyThread, NULL, 0, &g_dwThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_dwThreadId) PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_dwThreadId = 0;
    }
}

// Triggered by Windhawk whenever the user clicks "Save" in the Settings UI
void Wh_ModSettingsChanged() {
    Wh_Log(L"[ForceKill] Settings changed. Reloading...");
    LoadSettings();
    if (g_dwThreadId) {
        // Tell the background thread to safely unbind and rebind the hotkey
        PostThreadMessage(g_dwThreadId, WM_RELOAD_HOTKEY, 0, 0);
    }
}

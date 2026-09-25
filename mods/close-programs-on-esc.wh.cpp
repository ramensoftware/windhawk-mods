// ==WindhawkMod==
// @id              close-programs-on-esc
// @name            Close Apps on Esc (Advanced)
// @description     Professional-grade window termination via Esc with per-item modifier support, dynamic arrays, and predefined toggles.
// @version         4.5.0.2
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4/
// @homepage        https://main.elitesoftwaretech.cc
// @include         *
// @compilerOptions -lcomdlg32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Close Apps on Esc

Press **Esc** to terminate target applications. 

### Core Features
- **Unified List**: All apps (Default and Custom) live in one clean, high-density list.
- **Per-Item Modifiers**: Bind specific key combos (Ctrl, Shift, Alt) to individual applications.
- **Top-Most Dialogs**: Confirmations punch through all other windows without locking the UI thread.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logErrors: true
  $name: Enable Error Logging

- targetApps:
  - - processName: "pwsh.exe"
      $name: Process Name
    - confirmExit: false
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  - - processName: "powershell.exe"
      $name: Process Name
    - confirmExit: false
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  - - processName: "cmd.exe"
      $name: Process Name
    - confirmExit: false
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  - - processName: "ResourceHacker.exe"
      $name: Process Name
    - confirmExit: true
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  - - processName: "windowsterminal.exe"
      $name: Process Name
    - confirmExit: false
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  - - processName: "SnippingTool.exe"
      $name: Process Name
    - confirmExit: false
      $name: Confirm Exit
    - modCtrl: false
      $name: Ctrl
    - modShift: false
      $name: Shift
    - modAlt: false
      $name: Alt
  $name: Monitored Applications List
  $description: Add or modify processes that should be closed when Esc is pressed.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")

// ---------- Structures & Globals ----------
struct TargetApp {
    std::wstring processName;
    bool confirm;
    bool ctrl;
    bool shift;
    bool alt;
};

struct {
    bool logErrors;
    std::vector<TargetApp> targetApps;
} g_cfg;

HHOOK g_hook = nullptr;
DWORD g_hookThreadId = 0;

#define WM_SHOW_CONFIRM (WM_APP + 1)
bool g_isConfirming = false;

enum MatchResult {
    MATCH_NONE,
    MATCH_NOCONFIRM,
    MATCH_CONFIRM
};

// ---------- Error Handling ----------
void LogError(const std::wstring& code, const std::wstring& message) {
    if (!g_cfg.logErrors) return;
    
    std::wofstream logFile(L"CloseAppsEsc_ErrorLog.txt", std::ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        tm ltm;
        localtime_s(&ltm, &now);
        logFile << L"[" << (1900 + ltm.tm_year) << L"-" 
                << (1 + ltm.tm_mon) << L"-" << ltm.tm_mday << L" " 
                << ltm.tm_hour << L":" << ltm.tm_min << L":" << ltm.tm_sec << L"] " 
                << L"[ErrCode: " << code << L"] " << message << std::endl;
    }
}

// ---------- Settings Parsing ----------
bool ParseJsonBool(const std::wstring& json, size_t startPos, const std::wstring& key) {
    size_t pos = json.find(key, startPos);
    if (pos != std::wstring::npos) {
        pos += key.length();
        while (pos < json.length() && (json[pos] == L' ' || json[pos] == L':')) pos++;
        if (json.length() >= pos + 4 && json.substr(pos, 4) == L"true") return true;
    }
    return false;
}

void ParseTargetApps(PCWSTR jsonStr) {
    g_cfg.targetApps.clear();
    if (!jsonStr) return;

    std::wstring s(jsonStr);
    size_t pos = 0;
    
    while ((pos = s.find(L"\"processName\":\"", pos)) != std::wstring::npos) {
        pos += 15; 
        size_t endQuote = s.find(L"\"", pos);
        if (endQuote == std::wstring::npos) break;
        
        std::wstring pName = s.substr(pos, endQuote - pos);
        
        bool conf = ParseJsonBool(s, endQuote, L"\"confirmExit\":");
        bool c    = ParseJsonBool(s, endQuote, L"\"modCtrl\":");
        bool sh   = ParseJsonBool(s, endQuote, L"\"modShift\":");
        bool a    = ParseJsonBool(s, endQuote, L"\"modAlt\":");
        
        g_cfg.targetApps.push_back({pName, conf, c, sh, a});
    }
}

void LoadSettings() {
    g_cfg.logErrors = Wh_GetIntSetting(L"logErrors") != 0;

    PCWSTR rawApps = Wh_GetStringSetting(L"targetApps");
    ParseTargetApps(rawApps);
    Wh_FreeStringSetting(rawApps);
}

// ---------- Execution Logic ----------
MatchResult CheckProcessMatch(HWND hWnd, bool ctrl, bool shift, bool alt) {
    DWORD pid;
    if (!GetWindowThreadProcessId(hWnd, &pid)) {
        LogError(L"0x1A1", L"Failed to get thread process ID.");
        return MATCH_NONE;
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        LogError(L"0x1A2", L"Failed to open process handle.");
        return MATCH_NONE;
    }

    WCHAR path[MAX_PATH];
    DWORD size = MAX_PATH;
    MatchResult result = MATCH_NONE;

    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        LPWSTR exeName = PathFindFileNameW(path);
        
        for (const auto& app : g_cfg.targetApps) {
            if (app.processName == L"*" || lstrcmpiW(exeName, app.processName.c_str()) == 0) {
                if (ctrl == app.ctrl && shift == app.shift && alt == app.alt) {
                    result = app.confirm ? MATCH_CONFIRM : MATCH_NOCONFIRM;
                    break;
                }
            }
        }
    } else {
        LogError(L"0x1A3", L"QueryFullProcessImageNameW failed.");
    }

    CloseHandle(hProc);
    return result;
}

// ---------- Keyboard Hook ----------
LRESULT CALLBACK EscKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* ks = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && ks->vkCode == VK_ESCAPE) {
            
            if (g_isConfirming) {
                return CallNextHookEx(g_hook, code, wParam, lParam);
            }

            bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            HWND fg = GetForegroundWindow();
            if (fg) {
                MatchResult match = CheckProcessMatch(fg, ctrl, shift, alt);
                
                if (match == MATCH_NOCONFIRM) {
                    PostMessageW(fg, WM_CLOSE, 0, 0);
                    return 1; 
                } 
                else if (match == MATCH_CONFIRM) {
                    PostThreadMessageW(g_hookThreadId, WM_SHOW_CONFIRM, (WPARAM)fg, 0);
                    return 1; 
                }
            }
        }
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

// ---------- Lifecycle & Threading ----------
DWORD WINAPI HookThread(LPVOID) {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&EscKeyboardProc), &hMod);

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, EscKeyboardProc, hMod, 0);
    if (!g_hook) {
        LogError(L"0x1F1", L"Critical: SetWindowsHookExW failed to attach.");
        return 0;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_SHOW_CONFIRM) {
            HWND targetHwnd = (HWND)msg.wParam;
            g_isConfirming = true;
            
            int result = MessageBoxW(targetHwnd, 
                L"Do you wish to Exit from this process?", 
                L"Confirm Exit", 
                MB_YESNO | MB_ICONQUESTION | MB_TOPMOST | MB_SETFOREGROUND);
                
            g_isConfirming = false;
            
            if (result == IDYES) {
                PostMessageW(targetHwnd, WM_CLOSE, 0, 0);
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_hook) UnhookWindowsHookEx(g_hook);
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HANDLE hThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_hookThreadId);
    if (hThread) CloseHandle(hThread);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
// ==WindhawkMod==
// @id              prevent-focus-stealing
// @name            Prevent Focus Stealing
// @description     Prevents specific apps from stealing focus when a window is opened.
// @version         1.0.0
// @author          bawNg
// @github          https://github.com/bawng
// @include         *
// @architecture    x86-64
// @compilerOptions -lshlwapi -lcomctl32
// ==/WindhawkMod==

// Source code is published under the MIT License.

// ==WindhawkModReadme==
/*
# Prevent Focus Stealing

Focus stealing can be very annoying and disruptive to workflow and productivity.

This mod can be used to prevent any app from stealing focus when it opens a window. App-specific examples can be found below.

## Performance Optimization

By default, **the mod will be injected into all processes** and unload when there's no matching application in settings. To reduce overhead, enable `Ignore mod inclusion/exclusion lists` and add each executable to `Custom process inclusion list` in the `Advanced` options menu.

## Application Settings

- `Application Executables` must be one or more exe names, exe paths or path globs.
- `Required Arguments` can be used to only include processes with specific command line arguments. All arguments must match the process.
- `Excluded Arguments` can be used to exclude processes with specific command line arguments. Any matching argument excludes the process.
- `Window Title` is a title prefix of the window you want to prevent focus being stolen by. Leave it blank to target all windows.
- `Never Focus` prevents manual activation, so it should only be enabled if you never want to interact with the window.

​

---

​

## Preventing focus being stolen by Chromium (Electron / Browsers)

The Chromium DevTools window is especially annoying, as it opens asynchronously and has to be reopened every time you apply changes while working on development of a Chrome extension or an Electron-based app. You can now auto reload changes and have one or more DevTools windows reopen without your editor losing focus.

### Application Executables

Make sure `Application Executables` includes the process you are using (such as `electron.exe`, `brave.exe` or `chrome.exe`).

### Required Arguments

You can leave `Required Arguments` blank unless you only want to target a specific instance of an app, such as an electron app which you launch with custom argument during development.

### Excluded Arguments

You should set `Excluded Arguments` to `--type` for efficiency so that only the main chromium process which creates windows is hooked.

### Window Title

- Leave title blank to prevent initial focus of all windows for the application.
- `Developer Tools` will target all DevTools windows.
- `Developer Tools - https://example.com` will target only DevTools for that site.
- `Developer Tools - chrome-extension://<extension_id>` will target only DevTools windows for that extension.

### Never Focus

Leave `Never Focus` disabled so that you can manually activate the window and interact with it.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- applications:
  - - exes: [electron.exe, brave.exe, chrome.exe]
      $name: Application Executables
      $description: "Executables cannot be blank. Each executable can be either a file name (e.g. electron.exe), full path (e.g. C:\\Program Files\\Brave\\brave.exe), or path glob (e.g. *electron.exe)."
    - args: ['']
      $name: Required Arguments
      $description: "Each argument is a substring to match in the command line. All must match. Leave empty to include all processes."
    - exclude_args: [--type]
      $name: Excluded Arguments
      $description: "Each argument is a substring to exclude from the command line. Any match excludes the process. Leave empty to include all processes."
    - title: Developer Tools
      $name: Window Title
      $description: "The window title prefix to match. Leave blank to match all windows for the process."
    - never_focus: false
      $name: Never Focus
      $description: "Never allow the window to be become focused, instead of only when it initially opens."
  $name: Application Windows
  $description: "Prevent focus being stolen by these application windows."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlwapi.h>
#include <vector>
#include <set>
#include <map>

#define WM_USER_REMOVE_NOACTIVATE (WM_USER + 1)

const uint64_t NEW_WINDOW_TRACKING_MS = 1000;
const uint64_t INITIAL_UI_FOCUS_BLOCK_MS = 100;
const uint64_t INITIAL_FRAME_ACTIVATE_BLOCK_MS = 8000;

struct WindowInfo {
    const wchar_t* title;
    bool neverFocus;
};

std::vector<WindowInfo> g_includedWindows;

std::map<HWND, uint64_t> g_newHwnds;
std::set<HWND> g_hookedHwnds;
std::map<HWND, uint64_t> g_blockedHwnds; // Store creation time for timeout

WindowInfo* GetWindowInfo(HWND hWnd, LPCWSTR title) {
    for (WindowInfo& info : g_includedWindows) {
        if (*info.title && _wcsnicmp(title, info.title, wcslen(info.title)) != 0)
            continue;
        if (!info.neverFocus && g_hookedHwnds.contains(hWnd))
            continue;
        return &info;
    }
    return nullptr;
}

LRESULT CALLBACK WindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_DESTROY) {
        Wh_Log(L"Stopped tracking window 0x%p due to WM_DESTROY", hWnd);
        g_newHwnds.erase(hWnd);
        g_hookedHwnds.erase(hWnd);
        g_blockedHwnds.erase(hWnd);
    }
    if (uMsg == WM_USER_REMOVE_NOACTIVATE) {
        Wh_Log(L"Removing WS_EX_NOACTIVATE from window 0x%p", hWnd);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    }
    if (uMsg == WM_ACTIVATE && (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)) {
        auto it = g_blockedHwnds.find(hWnd);
        if (it != g_blockedHwnds.end() && GetTickCount64() - it->second < INITIAL_UI_FOCUS_BLOCK_MS) {
            // Prevent the UI from thinking the window is focused (like Chromium rendering a blinking cursor in DevTools console)
            Wh_Log(L"Blocked WM_ACTIVATE for window 0x%p - wParam: %llu", hWnd, wParam);
            return FALSE;
        }
    }
    if (uMsg == WM_NCACTIVATE && wParam == TRUE) {
        auto it = g_blockedHwnds.find(hWnd);
        if (it != g_blockedHwnds.end()) {
            if (GetForegroundWindow() != hWnd && GetTickCount64() - it->second < INITIAL_FRAME_ACTIVATE_BLOCK_MS) {
                // Chromium repeatedly attempts to activate the window frame every second for ~7s after the DevTools window has been opened
                Wh_Log(L"Blocked WM_NCACTIVATE for window 0x%p", hWnd);
                return TRUE; // Prevents the window frame from becoming active
            }
            g_blockedHwnds.erase(it);
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef BOOL(WINAPI *SetForegroundWindow_t)(HWND);
SetForegroundWindow_t SetForegroundWindow_Orig;

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (g_hookedHwnds.contains(hWnd)) {
        wchar_t title[256];
        GetWindowTextW(hWnd, title, _countof(title));
        Wh_Log(L"Blocked SetForegroundWindow for new window: %ls", title);
        return FALSE;
    }
    return SetForegroundWindow_Orig(hWnd);
}

void HookNewWindow(HWND hWnd, bool neverFocus) {
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, WindowSubclassProc, NULL)) {
        g_hookedHwnds.insert(hWnd);
        g_blockedHwnds[hWnd] = GetTickCount64();
    }
    if (!neverFocus) {
        PostMessageW(hWnd, WM_USER_REMOVE_NOACTIVATE, 0, 0);
    }
    if (GetForegroundWindow() == hWnd) {
        // In rare cases, the window becomes focused before the title has been set
        Wh_Log(L"Window stole focus before title was set");
        HWND prev_hwnd = GetNextWindow(hWnd, GW_HWNDNEXT);
        if (prev_hwnd && prev_hwnd != hWnd) {
            DWORD active_pid;
            GetWindowThreadProcessId(hWnd, &active_pid);
            while (prev_hwnd && prev_hwnd != hWnd) {
                DWORD prev_pid;
                GetWindowThreadProcessId(prev_hwnd, &prev_pid);
                if (prev_pid != active_pid && IsWindowVisible(prev_hwnd)) {
                    wchar_t prev_title[256];
                    GetWindowTextW(prev_hwnd, prev_title, _countof(prev_title));
                    WCHAR class_name[256];
                    GetClassNameW(prev_hwnd, class_name, 256);
                    Wh_Log(L"Restoring focus to window 0x%p - Title: '%ls', Class: %ls, pid: %u", prev_hwnd, prev_title, class_name, prev_pid);
                    SetForegroundWindow_Orig(prev_hwnd);
                    break;
                }
                prev_hwnd = GetWindow(prev_hwnd, GW_HWNDNEXT);
            }
        }
    }
}

typedef HWND(WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Orig;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        if (lpWindowName && *lpWindowName) {
            WindowInfo* window_info = GetWindowInfo(hWnd, lpWindowName);
            if (window_info) {
                HookNewWindow(hWnd, window_info->neverFocus);
                return hWnd;
            }
        }
        uint64_t now = GetTickCount64();
        for (auto it = g_newHwnds.begin(); it != g_newHwnds.end(); ) {
            if (now - it->second >= NEW_WINDOW_TRACKING_MS)
                it = g_newHwnds.erase(it);
            else
                it++;
        }
        g_newHwnds[hWnd] = now;
        //Wh_Log(L"Tracked new window 0x%p via CreateWindowExW");
    }
    return hWnd;
}

typedef BOOL(WINAPI *SetWindowTextW_t)(HWND, LPCWSTR);
SetWindowTextW_t SetWindowTextW_Orig;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR title) {
    BOOL result = SetWindowTextW_Orig(hWnd, title);
    auto it = g_newHwnds.find(hWnd);
    if (it != g_newHwnds.end() && GetTickCount64() - it->second < NEW_WINDOW_TRACKING_MS) {
        WindowInfo* window_info = title ? GetWindowInfo(hWnd, title) : nullptr;
        if (window_info) {
            Wh_Log(L"Detected new window via SetWindowTextW: %ls", title);
            g_newHwnds.erase(it);
            HookNewWindow(hWnd, window_info->neverFocus);
        }
    }
    return result;
}

BOOL Wh_ModInit() {
    WCHAR exe_path[MAX_PATH];
    GetModuleFileNameW(NULL, exe_path, MAX_PATH);
    //Wh_Log(L"Process path: %ls", exe_path);
    LPCWSTR exe_name = PathFindFileNameW(exe_path);
    LPCWSTR process_args = nullptr;
    for (int application_index = 0; ; application_index++) {
        for (int exe_index = 0; ; exe_index++) {
            PCWSTR exe_setting = Wh_GetStringSetting(L"applications[%d].exes[%d]", application_index, exe_index);
            if(!*exe_setting) {
                Wh_FreeStringSetting(exe_setting);
                if (exe_index == 0)
                    goto Done;
                break;
            }
            size_t size = wcslen(exe_setting);
            bool is_match = false;
            if (wcspbrk(exe_setting, L"*?") != nullptr) {
                if (size >= MAX_PATH) {
                    Wh_Log(L"ERROR: Executable glob is more than %d characters: '%ls'", MAX_PATH - 1, exe_setting);
                    Wh_FreeStringSetting(exe_setting);
                    continue;
                }
                is_match = PathMatchSpecW(exe_path, exe_setting);
            } else if (wcschr(exe_setting, L'\\') != nullptr) {
                is_match = _wcsnicmp(exe_path, exe_setting, size) == 0 && exe_path[size] == 0;
            } else {
                is_match = wcslen(exe_name) == size && _wcsnicmp(exe_name, exe_setting, size) == 0;
            }
            if (is_match) {
                if (!process_args) {
                    process_args = GetCommandLineW();
                    //Wh_Log(L"Process command line: %ls", process_args);
                    if (*process_args == L'"') {
                        process_args++;
                        while (*process_args && *process_args != L'"')
                            process_args++;
                        if (*process_args == L'"')
                            process_args++;
                    } else {
                        while (*process_args && *process_args != L' ' && *process_args != L'\t')
                            process_args++;
                    }
                    while (*process_args == L' ' || *process_args == L'\t')
                        process_args++;
                }
                //Wh_Log(L"Process arguments: %ls", process_args);
                for (int arg_index = 0; ; arg_index++) {
                    PCWSTR arg = Wh_GetStringSetting(L"applications[%d].args[%d]", application_index, arg_index);
                    if (!*arg) {
                        Wh_FreeStringSetting(arg);
                        break;
                    }
                    if (wcsstr(process_args, arg) == nullptr)
                        is_match = false;
                    Wh_FreeStringSetting(arg);
                }
                for (int arg_index = 0; ; arg_index++) {
                    PCWSTR arg = Wh_GetStringSetting(L"applications[%d].exclude_args[%d]", application_index, arg_index);
                    if (!*arg) {
                        Wh_FreeStringSetting(arg);
                        break;
                    }
                    if (wcsstr(process_args, arg) != nullptr)
                        is_match = false;
                    Wh_FreeStringSetting(arg);
                }
                if (is_match) {
                    PCWSTR title = Wh_GetStringSetting(L"applications[%d].title", application_index);
                    bool never_focus = Wh_GetIntSetting(L"applications[%d].never_focus", application_index);
                    Wh_Log(L"Hooking process for window title: %ls", title);
                    g_includedWindows.push_back(WindowInfo { title, never_focus });
                    break;
                }
            }
            Wh_FreeStringSetting(exe_setting);
        }
    }
Done:
    if (g_includedWindows.empty()) {
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void *)CreateWindowExW, (void *)CreateWindowExW_Hook, (void **)&CreateWindowExW_Orig)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void *)SetForegroundWindow, (void *)SetForegroundWindow_Hook, (void **)&SetForegroundWindow_Orig)) {
        Wh_Log(L"Failed to hook SetForegroundWindow");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void *)SetWindowTextW, (void *)SetWindowTextW_Hook, (void **)&SetWindowTextW_Orig)) {
        Wh_Log(L"Failed to hook SetWindowTextW");
        return FALSE;
    }

    Wh_Log(L"Mod initialized");
    return TRUE;
}

void Wh_ModUninit() {
    g_newHwnds.clear();
    for (HWND hWnd : g_hookedHwnds) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, WindowSubclassProc);
    }
    g_hookedHwnds.clear();
    for (WindowInfo info : g_includedWindows) {
        Wh_FreeStringSetting(info.title);
    }
    g_includedWindows.clear();
    Wh_Log(L"Mod unloaded");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Mod settings changed");
    *bReload = true;
    return TRUE;
}
// ==WindhawkMod==
// @id              prevent-focus-stealing
// @name            Prevent Focus Stealing
// @description     Prevents specific apps from stealing focus when a window is opened.
// @version         1.0.0
// @author          bawNg
// @github          https://github.com/bawNg
// @include         *
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

#ifdef _WIN64
    const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
    const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif

#define MOD_MSG_PREFIX L"Wh_FocusSteal_"

#define Wh_SetProp(hWnd, name, value) SetPropW(hWnd, MOD_MSG_PREFIX name, reinterpret_cast<HANDLE>(value))
#define Wh_GetProp(type, hWnd, name)  static_cast<type>(reinterpret_cast<intptr_t>(GetPropW(hWnd, MOD_MSG_PREFIX name)))
#define Wh_RemoveProp(hWnd, name)     RemoveProp(hWnd, MOD_MSG_PREFIX name)

const uint64_t INITIAL_UI_FOCUS_BLOCK_MS = 100;

struct WindowInfo {
    WindhawkUtils::StringSetting title;
    bool neverFocus;
};

std::vector<WindowInfo> g_includedWindows;

bool g_initialized;

LPCWSTR ShowCmdToString(int cmd) {
    switch (cmd) {
        case SW_HIDE: return L"SW_HIDE";
        case SW_SHOWNORMAL: return L"SW_SHOWNORMAL";
        case SW_SHOWMINIMIZED: return L"SW_SHOWMINIMIZED";
        case SW_SHOWMAXIMIZED: return L"SW_SHOWMAXIMIZED";
        case SW_SHOWNOACTIVATE: return L"SW_SHOWNOACTIVATE";
        case SW_SHOW: return L"SW_SHOW";
        case SW_MINIMIZE: return L"SW_MINIMIZE";
        case SW_SHOWMINNOACTIVE: return L"SW_SHOWMINNOACTIVE";
        case SW_SHOWNA: return L"SW_SHOWNA";
        case SW_RESTORE: return L"SW_RESTORE";
        case SW_SHOWDEFAULT: return L"SW_SHOWDEFAULT";
        case SW_FORCEMINIMIZE: return L"SW_FORCEMINIMIZE";
        default: return L"SW_UNKNOWN";
    }
}

struct EnumWindowsContext {
    DWORD pid;
    std::vector<HWND> hwnds;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto context = reinterpret_cast<EnumWindowsContext*>(lParam);
    DWORD window_pid = 0;
    GetWindowThreadProcessId(hwnd, &window_pid);
    if (window_pid == context->pid) context->hwnds.push_back(hwnd);
    return TRUE;
}

WindowInfo* GetWindowInfo(LPCWSTR title) {
    for (WindowInfo& info : g_includedWindows) {
        if (!(*info.title) || _wcsnicmp(title, info.title, wcslen(info.title)) == 0)
            return &info;
    }
    return nullptr;
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_Orig;

// May be called from other threads, in which case it posts a WM_SETTEXT message
BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR title) {
    BOOL result = SetWindowTextW_Orig(hWnd, title);
    uint64_t now = GetTickCount64();
    uint64_t created_at = Wh_GetProp(uint64_t, hWnd, L"CreatedAt");
    if (!created_at) {
        // CreateWindowExW was missed due to late injection
        created_at = now;
        Wh_SetProp(hWnd, L"CreatedAt", created_at);
    }
    if (title && !Wh_GetProp(uint64_t, hWnd, L"NoActivateAt")) {
        WindowInfo* window_info = GetWindowInfo(title);
        if (window_info) {
            Wh_Log(L"SetWindowTextW for window 0x%p: '%ls'", hWnd, title);
            Wh_SetProp(hWnd, L"NoActivateAt", now);
        }
    }
    return result;
}

using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Orig;

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    uint64_t no_activate_at = Wh_GetProp(uint64_t, hWnd, L"NoActivateAt");
    if (no_activate_at) {
        if (GetTickCount64() - no_activate_at < INITIAL_UI_FOCUS_BLOCK_MS) {
            wchar_t title[256];
            GetWindowTextW(hWnd, title, _countof(title));
            Wh_Log(L"SetForegroundWindow blocked for new window 0x%p: '%ls'", hWnd, title);
            return FALSE;
        }
    } else {
        Wh_Log(L"SetForegroundWindow allowed for new window 0x%p", hWnd);
    }
    return SetForegroundWindow_Orig(hWnd);
}

using SetFocus_t = decltype(&SetFocus);
SetFocus_t SetFocus_Orig;

HWND WINAPI SetFocus_Hook(HWND hWnd) {
    uint64_t no_activate_at = Wh_GetProp(uint64_t, hWnd, L"NoActivateAt");
    if (no_activate_at) {
        if (GetTickCount64() - no_activate_at < INITIAL_UI_FOCUS_BLOCK_MS) {
            Wh_Log(L"SetFocus blocked for window 0x%p", hWnd);
            return GetFocus();
        } else {
            Wh_Log(L"SetFocus allowed for window 0x%p", hWnd);
        }
    }
    HWND result = SetFocus_Orig(hWnd);
    return result;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Orig;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (!(uFlags & SWP_NOACTIVATE)) {
        uint64_t no_activate_at = Wh_GetProp(uint64_t, hWnd, L"NoActivateAt");
        if (no_activate_at) {
            if (GetTickCount64() - no_activate_at < INITIAL_UI_FOCUS_BLOCK_MS) {
                Wh_Log(L"SetWindowPos for window 0x%p - Adding SWP_NOACTIVATE to flags", hWnd);
                uFlags |= SWP_NOACTIVATE;
            }
        } else {
            Wh_Log(L"SetWindowPos for new window 0x%p", hWnd);
        }
    }
    return SetWindowPos_Orig(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Orig;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    bool show_maximize = false;
    uint64_t no_activate_at = Wh_GetProp(uint64_t, hWnd, L"NoActivateAt");
    if ((nCmdShow == SW_SHOW) | (nCmdShow == SW_SHOWNORMAL) | (nCmdShow == SW_RESTORE) | (nCmdShow == SW_SHOWMAXIMIZED) | (nCmdShow == SW_SHOWDEFAULT)) {
        if (no_activate_at) {
            if (GetTickCount64() - no_activate_at < INITIAL_UI_FOCUS_BLOCK_MS) {
                Wh_Log(L"Showing window 0x%p with SW_SHOWNOACTIVATE instead of %ls", hWnd, ShowCmdToString(nCmdShow));
                show_maximize = nCmdShow == SW_SHOWMAXIMIZED;
                nCmdShow = SW_SHOWNOACTIVATE;
            }
        } else {
            Wh_Log(L"Showing window 0x%p with %ls", hWnd, ShowCmdToString(nCmdShow));
        }
    } else if (no_activate_at) {
        Wh_Log(L"Showing window 0x%p with %ls", hWnd, ShowCmdToString(nCmdShow));
    }
    BOOL result = ShowWindow_Orig(hWnd, nCmdShow);
    if (show_maximize) {
        Wh_Log(L"Maximizing window 0x%p", hWnd);
        SendMessageW(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }
    return result;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    WindowInfo* window_info = lpWindowName ? GetWindowInfo(lpWindowName) : nullptr;
    if ((dwStyle & WS_VISIBLE) && window_info) {
        Wh_Log(L"CreateWindowExW with WS_VISIBLE: '%ls'", lpWindowName);
        dwStyle &= ~WS_VISIBLE;
    }
    HWND hWnd = CreateWindowExW_Orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        uint64_t now = GetTickCount64();
        Wh_SetProp(hWnd, L"CreatedAt", now);
        if (window_info) {
            Wh_Log(L"Created window 0x%p is being tracked: '%ls'", hWnd, lpWindowName);
            Wh_SetProp(hWnd, L"NoActivateAt", now);
            ShowWindow_Orig(hWnd, SW_SHOWNOACTIVATE);
        }
    }
    return hWnd;
}

void SwitchToPreviousWindow(HWND hWnd) {
    HWND prev_hwnd = GetWindow(hWnd, GW_HWNDNEXT);
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
                Wh_Log(L"Restoring focus to pid %u's window 0x%p: '%ls'", prev_pid, prev_hwnd, prev_title);
                if (SetForegroundWindow_Orig(prev_hwnd)) {
                    // SetForegroundWindow causes the window that stole focus to go behind the previous window, which wouldn't
                    // have happened if the window was blocked from stealing focus, so we need to bring it to the top again
                    SetWindowPos_Orig(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                } else {
                    Wh_Log(L"SetForegroundWindow failed");
                }
                break;
            }
            prev_hwnd = GetWindow(prev_hwnd, GW_HWNDNEXT);
        }
    }
}

BOOL Wh_ModInit() {
    WCHAR exe_path[MAX_PATH];
    GetModuleFileNameW(NULL, exe_path, MAX_PATH);
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
                    auto& window_info = g_includedWindows.emplace_back();
                    window_info.title = WindhawkUtils::StringSetting::make(L"applications[%d].title", application_index);
                    window_info.neverFocus = Wh_GetIntSetting(L"applications[%d].never_focus", application_index);
                    Wh_Log(L"Hooking process for window title: %ls", window_info.title.get());
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
        goto Failed;
    }
    if (!Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig)) {
        Wh_Log(L"Failed to hook ShowWindow");
        goto Failed;
    }
    if (!Wh_SetFunctionHook((void *)SetForegroundWindow, (void *)SetForegroundWindow_Hook, (void **)&SetForegroundWindow_Orig)) {
        Wh_Log(L"Failed to hook SetForegroundWindow");
        goto Failed;
    }

    if (!Wh_SetFunctionHook((void *)SetFocus, (void *)SetFocus_Hook, (void **)&SetFocus_Orig)) {
        Wh_Log(L"Failed to hook SetFocus");
        goto Failed;
    }

    if (!Wh_SetFunctionHook((void *)SetWindowPos, (void *)SetWindowPos_Hook, (void **)&SetWindowPos_Orig)) {
        Wh_Log(L"Failed to hook SetWindowPos");
        goto Failed;
    }

    if (!Wh_SetFunctionHook((void *)SetWindowTextW, (void *)SetWindowTextW_Hook, (void **)&SetWindowTextW_Orig)) {
        Wh_Log(L"Failed to hook SetWindowTextW");
        goto Failed;
    }

    Wh_Log(L"Mod loaded");
    return TRUE;

Failed:
    g_includedWindows.clear();
    return FALSE;
}

// Hook original pointers are only valid after ModInit so SwitchToPreviousWindow has to be called from ModAfterInit
void Wh_ModAfterInit() {
    bool injected_late = !(*(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400);
    if (injected_late) {
        // Windows may have been created before the mod was asynchronously injected if the parent process isn't whitelisted
        EnumWindowsContext existing_windows { GetCurrentProcessId() };
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&existing_windows));
        Wh_Log(L"Checking %lld existing windows", existing_windows.hwnds.size());
        uint64_t now = GetTickCount64();
        for (HWND hWnd : existing_windows.hwnds) {
            uint64_t created_at = Wh_GetProp(uint64_t, hWnd, L"CreatedAt");
            if (created_at)
                continue;
            Wh_SetProp(hWnd, L"CreatedAt", now);
            wchar_t title[256];
            GetWindowTextW(hWnd, title, _countof(title));
            WindowInfo* window_info = GetWindowInfo(title);
            if (window_info) {
                Wh_Log(L"Existing window 0x%p needs to be tracked: %ls", hWnd, title);
                Wh_SetProp(hWnd, L"NoActivateAt", now);
                if (GetForegroundWindow() == hWnd) {
                    Wh_Log(L"Window 0x%p stole focus before mod initialized - switching to previous", hWnd);
                    SwitchToPreviousWindow(hWnd);
                }
            }
        }
    } else {
        Wh_Log(L"Injection was not asynchronous");
    }

    g_initialized = true;

    Wh_Log(L"Mod initialized");
}

void Wh_ModUninit() {
    Wh_Log(L"Mod unloaded");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Mod settings changed");
    *bReload = true;
    return TRUE;
}
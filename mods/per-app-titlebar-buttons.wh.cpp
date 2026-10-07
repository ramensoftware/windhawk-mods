// ==WindhawkMod==
// @id              per-app-titlebar-buttons
// @name            Per-App Titlebar Buttons
// @description     Hide selected native titlebar buttons for configured applications
// @version         1.0
// @author          Casket Pizza
// @github          https://github.com/CasketPizza
// @include         *
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Per-app Titlebar Buttons

Hide selected native Windows titlebar buttons on a per-application basis.

Add one or more process names in the mod settings and choose whether to hide
the Minimize, Maximize/Restore, and Close buttons for each application.

Process names can be entered with or without the `.exe` extension. For example,
both `taskmgr` and `taskmgr.exe` match Task Manager. Matching is
case-insensitive, and a full executable path is also accepted.

## Close button behavior

Windows provides independent window style flags for the Minimize and
Maximize/Restore buttons, but not for the Close button.

When Close is selected together with both Minimize and Maximize/Restore, the
native caption button cluster is removed.

When Close is selected while either Minimize or Maximize/Restore remains
visible, the Close command is disabled instead. This preserves the remaining
native caption controls and the window system menu.

## Compatibility

The mod targets standard native Win32 titlebars. Applications that draw their
own titlebars, including some Chromium, Electron, and WinUI applications, may
not respond to these settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- applications:
  - - process: notepad
      $name: Process
      $description: Executable name with or without .exe, for example notepad or notepad.exe.
    - hideMinimize: false
      $name: Hide Minimize
    - hideMaximize: false
      $name: Hide Maximize / Restore
    - hideClose: false
      $name: Hide Close
      $description: If other caption buttons remain visible, Close is disabled instead of removed.
  $name: Applications
  $description: Add an entry for each application to customize.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>

#include <algorithm>
#include <cwctype>
#include <string>
#include <unordered_map>

struct AppSettings {
    bool matched;
    bool hideMinimize;
    bool hideMaximize;
    bool hideClose;
};

struct WindowBackup {
    LONG_PTR style;
    bool styleCaptured;
    bool closeCaptured;
    UINT closeState;
};

static AppSettings g_settings = {};
static std::unordered_map<HWND, WindowBackup> g_backups;

static HANDLE g_eventThread = nullptr;
static DWORD g_eventThreadId = 0;

static std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return value;
}

static std::wstring NormalizeProcessName(std::wstring value) {
    value = ToLower(value);

    const size_t slash = value.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        value = value.substr(slash + 1);
    }

    if (value.size() >= 4 &&
        value.compare(value.size() - 4, 4, L".exe") == 0) {
        value.resize(value.size() - 4);
    }

    return value;
}

static std::wstring CurrentProcessName() {
    wchar_t path[MAX_PATH] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return L"";
    }

    return NormalizeProcessName(path);
}

static AppSettings LoadSettingsForCurrentProcess() {
    AppSettings result = {};
    const std::wstring currentProcess = CurrentProcessName();

    if (currentProcess.empty()) {
        return result;
    }

    for (int i = 0;; i++) {
        PCWSTR processValue =
            Wh_GetStringSetting(L"applications[%d].process", i);

        if (!processValue || !*processValue) {
            if (processValue) {
                Wh_FreeStringSetting(processValue);
            }
            break;
        }

        std::wstring process = NormalizeProcessName(processValue);
        Wh_FreeStringSetting(processValue);

        if (process == currentProcess) {
            result.matched = true;
            result.hideMinimize =
                Wh_GetIntSetting(L"applications[%d].hideMinimize", i) != 0;
            result.hideMaximize =
                Wh_GetIntSetting(L"applications[%d].hideMaximize", i) != 0;
            result.hideClose =
                Wh_GetIntSetting(L"applications[%d].hideClose", i) != 0;
            break;
        }
    }

    return result;
}

static bool IsTopLevelCaptionWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if (!(style & WS_CAPTION)) {
        return false;
    }

    return true;
}

static void RefreshFrame(HWND hwnd) {
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOACTIVATE | SWP_FRAMECHANGED);

    RedrawWindow(hwnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
}

static void CaptureBackup(HWND hwnd) {
    auto& backup = g_backups[hwnd];

    if (!backup.styleCaptured) {
        backup.style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        backup.styleCaptured = true;
    }

    if (!backup.closeCaptured) {
        HMENU menu = GetSystemMenu(hwnd, FALSE);
        if (menu) {
            backup.closeState = GetMenuState(menu, SC_CLOSE, MF_BYCOMMAND);
            backup.closeCaptured = true;
        }
    }
}

static void ApplyToWindow(HWND hwnd) {
    if (!g_settings.matched || !IsTopLevelCaptionWindow(hwnd)) {
        return;
    }

    CaptureBackup(hwnd);

    LONG_PTR oldStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR newStyle = oldStyle;

    if (g_settings.hideMinimize) {
        newStyle &= ~static_cast<LONG_PTR>(WS_MINIMIZEBOX);
    }

    if (g_settings.hideMaximize) {
        newStyle &= ~static_cast<LONG_PTR>(WS_MAXIMIZEBOX);
    }

    const bool hideWholeCluster =
        g_settings.hideClose &&
        g_settings.hideMinimize &&
        g_settings.hideMaximize;

    if (hideWholeCluster) {
        newStyle &= ~static_cast<LONG_PTR>(WS_SYSMENU);
    }

    if (newStyle != oldStyle) {
        SetWindowLongPtrW(hwnd, GWL_STYLE, newStyle);
        RefreshFrame(hwnd);
    }

    if (g_settings.hideClose && !hideWholeCluster) {
        HMENU menu = GetSystemMenu(hwnd, FALSE);
        if (menu) {
            EnableMenuItem(menu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
            DrawMenuBar(hwnd);
        }
    }
}

static BOOL CALLBACK ApplyEnumProc(HWND hwnd, LPARAM) {
    ApplyToWindow(hwnd);
    return TRUE;
}

static void ApplyToExistingWindows() {
    EnumWindows(ApplyEnumProc, 0);
}

static void CALLBACK WindowEventProc(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD eventThread,
    DWORD eventTime) {

    if (!g_settings.matched || !hwnd) {
        return;
    }

    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    ApplyToWindow(hwnd);
}

static DWORD WINAPI WindowEventThread(LPVOID) {
    const DWORD processId = GetCurrentProcessId();

    HWINEVENTHOOK createHook = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_CREATE,
        nullptr,
        WindowEventProc,
        processId,
        0,
        WINEVENT_OUTOFCONTEXT);

    HWINEVENTHOOK showHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WindowEventProc,
        processId,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!createHook || !showHook) {
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (showHook) {
        UnhookWinEvent(showHook);
    }

    if (createHook) {
        UnhookWinEvent(createHook);
    }

    return 0;
}

static bool StartWindowEventThread() {
    if (g_eventThread) {
        return true;
    }

    g_eventThread = CreateThread(
        nullptr,
        0,
        WindowEventThread,
        nullptr,
        0,
        &g_eventThreadId);

    return g_eventThread != nullptr;
}

static void StopWindowEventThread() {
    if (!g_eventThread) {
        return;
    }

    PostThreadMessageW(g_eventThreadId, WM_NULL, 0, 0);
    PostThreadMessageW(g_eventThreadId, WM_QUIT, 0, 0);

    WaitForSingleObject(g_eventThread, INFINITE);
    CloseHandle(g_eventThread);

    g_eventThread = nullptr;
    g_eventThreadId = 0;
}

static void RestoreAllWindows() {
    for (auto& item : g_backups) {
        HWND hwnd = item.first;
        WindowBackup& backup = item.second;

        if (!IsWindow(hwnd)) {
            continue;
        }

        if (backup.styleCaptured) {
            SetWindowLongPtrW(hwnd, GWL_STYLE, backup.style);
        }

        if (backup.closeCaptured) {
            GetSystemMenu(hwnd, TRUE);
            HMENU menu = GetSystemMenu(hwnd, FALSE);

            if (menu && backup.closeState != static_cast<UINT>(-1)) {
                if (backup.closeState & (MF_DISABLED | MF_GRAYED)) {
                    EnableMenuItem(menu, SC_CLOSE,
                                   MF_BYCOMMAND | MF_GRAYED);
                } else {
                    EnableMenuItem(menu, SC_CLOSE,
                                   MF_BYCOMMAND | MF_ENABLED);
                }
            }
        }

        RefreshFrame(hwnd);
        DrawMenuBar(hwnd);
    }

    g_backups.clear();
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param) {

    HWND hwnd = CreateWindowExW_Original(
        exStyle, className, windowName, style,
        x, y, width, height, parent, menu, instance, param);

    if (hwnd) {
        ApplyToWindow(hwnd);
    }

    return hwnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
static CreateWindowExA_t CreateWindowExA_Original;

static HWND WINAPI CreateWindowExA_Hook(
    DWORD exStyle,
    LPCSTR className,
    LPCSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param) {

    HWND hwnd = CreateWindowExA_Original(
        exStyle, className, windowName, style,
        x, y, width, height, parent, menu, instance, param);

    if (hwnd) {
        ApplyToWindow(hwnd);
    }

    return hwnd;
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;

static BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmdShow) {
    BOOL result = ShowWindow_Original(hwnd, cmdShow);

    if (hwnd) {
        ApplyToWindow(hwnd);
    }

    return result;
}

using ShowWindowAsync_t = decltype(&ShowWindowAsync);
static ShowWindowAsync_t ShowWindowAsync_Original;

static BOOL WINAPI ShowWindowAsync_Hook(HWND hwnd, int cmdShow) {
    BOOL result = ShowWindowAsync_Original(hwnd, cmdShow);

    if (hwnd) {
        ApplyToWindow(hwnd);
    }

    return result;
}

BOOL Wh_ModInit() {
    g_settings = LoadSettingsForCurrentProcess();

    if (!g_settings.matched) {
        return FALSE;
    }

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExW),
        reinterpret_cast<void*>(CreateWindowExW_Hook),
        reinterpret_cast<void**>(&CreateWindowExW_Original));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExA),
        reinterpret_cast<void*>(CreateWindowExA_Hook),
        reinterpret_cast<void**>(&CreateWindowExA_Original));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(ShowWindow),
        reinterpret_cast<void*>(ShowWindow_Hook),
        reinterpret_cast<void**>(&ShowWindow_Original));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(ShowWindowAsync),
        reinterpret_cast<void*>(ShowWindowAsync_Hook),
        reinterpret_cast<void**>(&ShowWindowAsync_Original));

    if (!StartWindowEventThread()) {
        Wh_Log(L"Failed to start window event thread");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyToExistingWindows();
}

void Wh_ModSettingsChanged() {
    RestoreAllWindows();
    g_settings = LoadSettingsForCurrentProcess();

    if (g_settings.matched) {
        ApplyToExistingWindows();
    }
}

void Wh_ModUninit() {
    StopWindowEventThread();
    RestoreAllWindows();
}

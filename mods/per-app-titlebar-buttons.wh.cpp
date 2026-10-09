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

Hide or disable selected native Windows titlebar buttons on a per-application
basis.

Add one or more process names in the mod settings and choose whether to affect
the Minimize, Maximize/Restore, and Close buttons for each application.

Process names can be entered with or without the `.exe` extension. For example,
both `taskmgr` and `taskmgr.exe` match Task Manager. Matching is
case-insensitive, and a full executable path is also accepted.

## Native titlebar behavior

Windows treats the Minimize and Maximize/Restore buttons as a pair. If only one
of those style flags is removed, Windows may keep both buttons visible and show
the affected button as disabled. Removing both flags removes the pair.

These are native window style flags, so disabling Maximize also disables native
maximize actions such as titlebar double-click and the Maximize system-menu
command while the setting is active.

Windows doesn't provide an independent window style flag for the Close button.
If Close is selected while Minimize or Maximize/Restore is still available,
the Close command is disabled.

If Close is selected and the resulting window has neither Minimize nor
Maximize/Restore available, the mod removes `WS_SYSMENU`. This removes the
native caption button cluster, and also removes the titlebar icon and the
Alt+Space system menu for that window while the setting is active.

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
      $description: On native titlebars, hiding only one of Minimize or Maximize may leave a disabled button.
    - hideMaximize: false
      $name: Hide Maximize / Restore
      $description: On native titlebars, hiding only one of Minimize or Maximize may leave a disabled button.
    - hideClose: false
      $name: Hide Close
      $description: If Minimize or Maximize remains available, Close is disabled instead of removing the whole caption cluster.
  $name: Applications
  $description: Add an entry for each application to customize.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>

#include <algorithm>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

struct AppSettings {
    bool matched;
    bool hideMinimize;
    bool hideMaximize;
    bool hideClose;
};

struct WindowBackup {
    LONG_PTR removedStyles = 0;
    bool closeDisabled = false;
};

static std::mutex g_stateMutex;
static AppSettings g_settings = {};
static std::unordered_map<HWND, WindowBackup> g_backups;
static std::unordered_set<HWND> g_applyingWindows;

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

        if (!*processValue) {
            Wh_FreeStringSetting(processValue);
            break;
        }

        std::wstring process = NormalizeProcessName(processValue);
        Wh_FreeStringSetting(processValue);

        if (process == currentProcess) {
            result.hideMinimize =
                Wh_GetIntSetting(L"applications[%d].hideMinimize", i) != 0;
            result.hideMaximize =
                Wh_GetIntSetting(L"applications[%d].hideMaximize", i) != 0;
            result.hideClose =
                Wh_GetIntSetting(L"applications[%d].hideClose", i) != 0;
            result.matched =
                result.hideMinimize ||
                result.hideMaximize ||
                result.hideClose;
            break;
        }
    }

    return result;
}

static AppSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return g_settings;
}

static void SetSettings(const AppSettings& settings) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_settings = settings;
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

    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if ((style & WS_CAPTION) != WS_CAPTION) {
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

using RunFromWindowThreadProc = void(WINAPI*)(PVOID parameter);

static bool RunFromWindowThread(HWND hwnd,
                                RunFromWindowThreadProc proc,
                                PVOID procParam) {
    static const UINT runMessage =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParam {
        RunFromWindowThreadProc proc;
        PVOID procParam;
    };

    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* message =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (message->message == runMessage) {
                    RunParam* param =
                        reinterpret_cast<RunParam*>(message->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr,
        threadId);

    if (!hook) {
        return false;
    }

    RunParam param = {proc, procParam};
    SendMessageW(hwnd, runMessage, 0, reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

class ApplyWindowGuard {
public:
    explicit ApplyWindowGuard(HWND hwnd) : m_hwnd(hwnd), m_acquired(false) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        m_acquired = g_applyingWindows.insert(hwnd).second;
    }

    ~ApplyWindowGuard() {
        if (!m_acquired) {
            return;
        }

        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_applyingWindows.erase(m_hwnd);
    }

    explicit operator bool() const {
        return m_acquired;
    }

private:
    HWND m_hwnd;
    bool m_acquired;
};

static void RecordRemovedStyles(HWND hwnd, LONG_PTR removedStyles) {
    if (!removedStyles) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_backups[hwnd].removedStyles |= removedStyles;
}

static void RecordCloseDisabled(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_backups[hwnd].closeDisabled = true;
}

static void RemoveWindowBackup(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_backups.erase(hwnd);
    g_applyingWindows.erase(hwnd);
}

// Called on the thread that owns hwnd.
static void ApplyToWindow(HWND hwnd) {
    const AppSettings settings = GetSettingsSnapshot();
    if (!settings.matched || !IsTopLevelCaptionWindow(hwnd)) {
        return;
    }

    ApplyWindowGuard applyGuard(hwnd);
    if (!applyGuard) {
        return;
    }

    const LONG_PTR oldStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR newStyle = oldStyle;

    if (settings.hideMinimize) {
        newStyle &= ~static_cast<LONG_PTR>(WS_MINIMIZEBOX);
    }

    if (settings.hideMaximize) {
        newStyle &= ~static_cast<LONG_PTR>(WS_MAXIMIZEBOX);
    }

    const bool hideWholeCluster =
        settings.hideClose &&
        !(newStyle & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX));

    if (hideWholeCluster) {
        newStyle &= ~static_cast<LONG_PTR>(WS_SYSMENU);
    }

    const LONG_PTR removedStyles = oldStyle & ~newStyle;
    if (removedStyles) {
        SetLastError(ERROR_SUCCESS);
        const LONG_PTR previousStyle =
            SetWindowLongPtrW(hwnd, GWL_STYLE, newStyle);

        if (previousStyle != 0 || GetLastError() == ERROR_SUCCESS) {
            RecordRemovedStyles(hwnd, removedStyles);
            RefreshFrame(hwnd);
        }
    }

    if (settings.hideClose && !hideWholeCluster) {
        HMENU menu = GetSystemMenu(hwnd, FALSE);
        if (menu) {
            const UINT closeState =
                GetMenuState(menu, SC_CLOSE, MF_BYCOMMAND);

            if (closeState != static_cast<UINT>(-1) &&
                !(closeState & (MF_DISABLED | MF_GRAYED))) {
                if (EnableMenuItem(menu, SC_CLOSE,
                                   MF_BYCOMMAND | MF_GRAYED) !=
                    static_cast<UINT>(-1)) {
                    RecordCloseDisabled(hwnd);
                    DrawMenuBar(hwnd);
                }
            }
        }
    }
}

static void WINAPI ApplyToWindowProc(PVOID param) {
    ApplyToWindow(static_cast<HWND>(param));
}

static void ApplyToWindowOnOwnerThread(HWND hwnd) {
    const AppSettings settings = GetSettingsSnapshot();

    if (!settings.matched || !IsTopLevelCaptionWindow(hwnd)) {
        return;
    }

    RunFromWindowThread(hwnd, ApplyToWindowProc, hwnd);
}

static BOOL CALLBACK ApplyEnumProc(HWND hwnd, LPARAM) {
    ApplyToWindowOnOwnerThread(hwnd);
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

    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        RemoveWindowBackup(hwnd);
        return;
    }

    if (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_SHOW) {
        ApplyToWindowOnOwnerThread(hwnd);
    }
}

static DWORD WINAPI WindowEventThread(LPVOID parameter) {
    HANDLE readyEvent = static_cast<HANDLE>(parameter);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    const DWORD processId = GetCurrentProcessId();

    HWINEVENTHOOK eventHook = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_SHOW,
        nullptr,
        WindowEventProc,
        processId,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!eventHook) {
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    }

    SetEvent(readyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (eventHook) {
        UnhookWinEvent(eventHook);
    }

    return 0;
}

static bool StartWindowEventThread() {
    if (g_eventThread) {
        return true;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return false;
    }

    g_eventThread = CreateThread(
        nullptr,
        0,
        WindowEventThread,
        readyEvent,
        0,
        &g_eventThreadId);

    if (!g_eventThread) {
        CloseHandle(readyEvent);
        g_eventThreadId = 0;
        return false;
    }

    const DWORD waitResult = WaitForSingleObject(readyEvent, 5000);

    if (waitResult != WAIT_OBJECT_0) {
        while (!PostThreadMessageW(g_eventThreadId, WM_QUIT, 0, 0) &&
               WaitForSingleObject(g_eventThread, 10) == WAIT_TIMEOUT) {
        }

        WaitForSingleObject(g_eventThread, INFINITE);
        CloseHandle(g_eventThread);
        g_eventThread = nullptr;
        g_eventThreadId = 0;

        CloseHandle(readyEvent);
        return false;
    }

    CloseHandle(readyEvent);
    return true;
}

static void StopWindowEventThread() {
    if (!g_eventThread) {
        return;
    }

    while (!PostThreadMessageW(g_eventThreadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(g_eventThread, 10) == WAIT_TIMEOUT) {
    }

    WaitForSingleObject(g_eventThread, INFINITE);
    CloseHandle(g_eventThread);

    g_eventThread = nullptr;
    g_eventThreadId = 0;
}

struct RestoreWindowParam {
    HWND hwnd;
    WindowBackup backup;
};

static void WINAPI RestoreWindowProc(PVOID parameter) {
    RestoreWindowParam* param =
        static_cast<RestoreWindowParam*>(parameter);

    const HWND hwnd = param->hwnd;
    const WindowBackup& backup = param->backup;

    if (!IsWindow(hwnd)) {
        return;
    }

    bool frameChanged = false;

    if (backup.removedStyles) {
        const LONG_PTR currentStyle =
            GetWindowLongPtrW(hwnd, GWL_STYLE);
        const LONG_PTR restoredStyle =
            currentStyle | backup.removedStyles;

        if (restoredStyle != currentStyle) {
            SetLastError(ERROR_SUCCESS);
            const LONG_PTR previousStyle =
                SetWindowLongPtrW(hwnd, GWL_STYLE, restoredStyle);

            if (previousStyle != 0 ||
                GetLastError() == ERROR_SUCCESS) {
                frameChanged = true;
            }
        }
    }

    if (backup.closeDisabled) {
        HMENU menu = GetSystemMenu(hwnd, FALSE);
        if (menu) {
            EnableMenuItem(menu, SC_CLOSE,
                           MF_BYCOMMAND | MF_ENABLED);
            DrawMenuBar(hwnd);
        }
    }

    if (frameChanged) {
        RefreshFrame(hwnd);
    }
}

static void RestoreAllWindows() {
    std::unordered_map<HWND, WindowBackup> backups;

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        backups = std::exchange(g_backups, {});
        g_applyingWindows.clear();
    }

    for (const auto& [hwnd, backup] : backups) {
        if (!IsWindow(hwnd)) {
            continue;
        }

        RestoreWindowParam param = {hwnd, backup};
        RunFromWindowThread(hwnd, RestoreWindowProc, &param);
    }
}

BOOL Wh_ModInit() {
    const AppSettings settings = LoadSettingsForCurrentProcess();
    SetSettings(settings);

    if (!settings.matched) {
        return FALSE;
    }

    if (!StartWindowEventThread()) {
        Wh_Log(L"Failed to start window event thread");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyToExistingWindows();
}

void Wh_ModSettingsChanged() {
    StopWindowEventThread();
    RestoreAllWindows();

    const AppSettings settings = LoadSettingsForCurrentProcess();
    SetSettings(settings);

    if (!settings.matched) {
        return;
    }

    if (!StartWindowEventThread()) {
        Wh_Log(L"Failed to restart window event thread");
        return;
    }

    ApplyToExistingWindows();
}

void Wh_ModUninit() {
    StopWindowEventThread();
    RestoreAllWindows();
}

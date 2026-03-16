// ==WindhawkMod==
// @id              ctrl-to-cross-monitors
// @name            Hold Ctrl to cross monitors
// @description     Prevents the mouse cursor from moving to another monitor unless Ctrl is held while moving the mouse.
// @version         0.1
// @author          manudesir
// @github          https://github.com/manudesir
// @include         explorer.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hold Ctrl to cross monitors

This mod blocks the mouse cursor from crossing from one monitor to another unless
Ctrl is currently held down.

## Behavior
- Normal mouse movement inside the current monitor is unaffected.
- If the cursor tries to move to another monitor and Ctrl is **not** pressed,
  the cursor is forced back to its previous position.
- If Ctrl is pressed while continuing the mouse movement, the cross-monitor move
  is allowed.

## Notes
- This is a single-file Windhawk mod, which matches Windhawk's documented mod format. ([github.com](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod))
- It uses a low-level mouse hook (`WH_MOUSE_LL`), which Microsoft says should run
  on a dedicated thread with a message loop and return quickly. ([learn.microsoft.com](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelmouseproc))
- It checks Ctrl with `GetAsyncKeyState(VK_CONTROL)`. Microsoft documents `VK_CONTROL`
  specifically for querying the state of Ctrl without distinguishing left vs right. ([learn.microsoft.com](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate))

## Limitation
If this still has absolutely no effect on your machine, the likely issue is that
Windhawk didn't actually get the hook active in the target process, not the logic
itself.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enabled
- logTransitions: false
  $name: Log transitions
*/
// ==/WindhawkModSettings==

#include <windows.h>

struct Settings {
    bool enabled;
    bool logTransitions;
};

static Settings g_settings{};
static HHOOK g_mouseHook = nullptr;
static HMODULE g_moduleHandle = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_threadReadyEvent = nullptr;

static bool g_ignoreForcedPoint = false;
static POINT g_forcedPoint{};
static POINT g_lastPoint{};
static bool g_haveLastPoint = false;

static void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    g_settings.logTransitions = Wh_GetIntSetting(L"logTransitions") != 0;
}

static bool GetCurrentModuleHandle(HMODULE* moduleHandle) {
    return GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        moduleHandle);
}

static bool IsCtrlHeld() {
    return GetAsyncKeyState(VK_CONTROL) < 0;
}

static void ForceCursorTo(const POINT& point) {
    g_forcedPoint = point;
    g_ignoreForcedPoint = true;
    SetCursorPos(point.x, point.y);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (wParam != WM_MOUSEMOVE) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto* mouseInfo = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
    const POINT currentPoint = mouseInfo->pt;

    if (g_ignoreForcedPoint &&
        currentPoint.x == g_forcedPoint.x &&
        currentPoint.y == g_forcedPoint.y) {
        g_ignoreForcedPoint = false;
        g_lastPoint = currentPoint;
        g_haveLastPoint = true;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (!g_settings.enabled) {
        g_lastPoint = currentPoint;
        g_haveLastPoint = true;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (!g_haveLastPoint) {
        g_lastPoint = currentPoint;
        g_haveLastPoint = true;
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    HMONITOR lastMonitor = MonitorFromPoint(g_lastPoint, MONITOR_DEFAULTTONEAREST);
    HMONITOR currentMonitor = MonitorFromPoint(currentPoint, MONITOR_DEFAULTTONEAREST);

    if (lastMonitor && currentMonitor && lastMonitor != currentMonitor && !IsCtrlHeld()) {
        if (g_settings.logTransitions) {
            Wh_Log(L"Blocked monitor crossing from (%d, %d) to (%d, %d)",
                   g_lastPoint.x,
                   g_lastPoint.y,
                   currentPoint.x,
                   currentPoint.y);
        }

        ForceCursorTo(g_lastPoint);
        return 1;
    }

    if (lastMonitor && currentMonitor && lastMonitor != currentMonitor && g_settings.logTransitions) {
        Wh_Log(L"Allowed monitor crossing with Ctrl from (%d, %d) to (%d, %d)",
               g_lastPoint.x,
               g_lastPoint.y,
               currentPoint.x,
               currentPoint.y);
    }

    g_lastPoint = currentPoint;
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static DWORD WINAPI HookThreadProc(void*) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_moduleHandle, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        SetEvent(g_threadReadyEvent);
        return 1;
    }

    SetEvent(g_threadReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!GetCurrentModuleHandle(&g_moduleHandle) || !g_moduleHandle) {
        Wh_Log(L"Failed to get current module handle");
        return FALSE;
    }

    g_threadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_threadReadyEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_threadReadyEvent, INFINITE);

    if (!g_mouseHook) {
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Hold Ctrl to cross monitors initialized");
    return TRUE;
}

void Wh_ModBeforeUninit() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
}

void Wh_ModUninit() {
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    if (g_threadReadyEvent) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = nullptr;
    }

    g_mouseHook = nullptr;
    g_hookThreadId = 0;
    g_haveLastPoint = false;
    g_ignoreForcedPoint = false;

    Wh_Log(L"Hold Ctrl to cross monitors uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    LoadSettings();
    g_haveLastPoint = false;
    g_ignoreForcedPoint = false;

    if (reload) {
        *reload = FALSE;
    }

    return TRUE;
}

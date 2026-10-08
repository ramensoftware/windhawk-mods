// ==WindhawkMod==
// @id              alt-tab-mouse-focus
// @name            AltTab Mouse Focus
// @description     Moves the mouse cursor to the center of the window that gets focused after switching with Alt+Tab.
// @version         1.0
// @author          tafadias
// @github          https://github.com/tafadias
// @include         explorer.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AltTab Mouse Focus

Moves the mouse cursor to the center of the window that gets focused after
switching windows with **Alt+Tab**.

- Only acts on Alt+Tab. Clicking, other keyboard shortcuts or focus changes
  made by other programs do not move the cursor.
- The cursor is moved once the **Alt** key is released, after the window
  switcher closes.
- Ignores system overlays (desktop, taskbar, the Alt+Tab switcher).
- Retries briefly while the target window settles.
- Works with multiple monitors: the cursor is centered on the monitor where
  the focused window is.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delayMs: 80
  $name: Delay after Alt release (ms)
  $description: How long to wait after releasing Alt before moving the cursor.
- onlyOnWindowChange: false
  $name: Only move when the window changes
  $description: If enabled, only move when the focused window actually changes.
- retryIntervalMs: 50
  $name: Retry interval (ms)
  $description: Interval between attempts while the target window is not ready yet.
- retryCount: 12
  $name: Retry count
  $description: Maximum number of attempts to move the cursor.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#define WM_ALT_TAB_QUIT   (WM_APP + 1)
#define WM_ALT_TAB_RELOAD (WM_APP + 2)
#define TIMER_MOVE 1

static HHOOK g_kbHook = nullptr;
static HWND g_hwnd = nullptr;
static HANDLE g_thread = nullptr;

static bool g_armed = false;
static HWND g_fgAtArm = nullptr;
static int g_retries = 0;

static volatile LONG g_delayMs = 80;
static volatile LONG g_onlyOnWindowChange = 0;
static volatile LONG g_retryIntervalMs = 50;
static volatile LONG g_retryCount = 12;

static void LoadSettings() {
    g_delayMs = Wh_GetIntSetting(L"delayMs");
    g_onlyOnWindowChange = Wh_GetIntSetting(L"onlyOnWindowChange") ? 1 : 0;
    g_retryIntervalMs = Wh_GetIntSetting(L"retryIntervalMs");
    g_retryCount = Wh_GetIntSetting(L"retryCount");

    if (g_delayMs < 0) g_delayMs = 0;
    if (g_delayMs > 2000) g_delayMs = 2000;
    if (g_retryIntervalMs < 10) g_retryIntervalMs = 10;
    if (g_retryIntervalMs > 500) g_retryIntervalMs = 500;
    if (g_retryCount < 0) g_retryCount = 0;
    if (g_retryCount > 100) g_retryCount = 100;
}

// System overlays that should never receive the cursor.
static bool IsOverlay(HWND hwnd) {
    if (!hwnd)
        return true;

    WCHAR cls[256] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return true;

    static const WCHAR* kOverlays[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"MultitaskingViewFrame",
        L"ForegroundStaging",
        L"XamlExplorerHostIslandWindow",
        L"#32771",
    };
    for (const WCHAR* name : kOverlays) {
        if (wcscmp(cls, name) == 0)
            return true;
    }
    return false;
}

// Visible rectangle (excluding invisible borders), with a GetWindowRect fallback.
static bool GetVisibleRect(HWND hwnd, RECT* rc) {
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, rc, sizeof(*rc))))
        return true;
    return GetWindowRect(hwnd, rc) != FALSE;
}

static bool TryMoveToCenter() {
    HWND fg = GetForegroundWindow();
    if (!fg || IsOverlay(fg))
        return false;
    if (IsIconic(fg))
        return false;

    // Only relevant if the window changed; otherwise keep retrying.
    if (g_onlyOnWindowChange && fg == g_fgAtArm)
        return false;

    RECT rc;
    if (!GetVisibleRect(fg, &rc))
        return false;

    LONG w = rc.right - rc.left;
    LONG h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0)
        return false;

    SetCursorPos(rc.left + w / 2, rc.top + h / 2);
    return true;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_MOVE) {
            KillTimer(hwnd, TIMER_MOVE);
            if (TryMoveToCenter() || g_retries >= g_retryCount) {
                g_retries = 0;
            } else {
                g_retries++;
                SetTimer(hwnd, TIMER_MOVE, (UINT)g_retryIntervalMs, nullptr);
            }
        }
        return 0;
    case WM_ALT_TAB_RELOAD:
        LoadSettings();
        return 0;
    case WM_ALT_TAB_QUIT:
        if (g_kbHook) {
            UnhookWindowsHookEx(g_kbHook);
            g_kbHook = nullptr;
        }
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isDown && kbd->vkCode == VK_TAB && (kbd->flags & LLKHF_ALTDOWN)) {
            // Alt+Tab: arm and remember the originating window.
            g_armed = true;
            g_fgAtArm = GetForegroundWindow();
        } else if (isDown && kbd->vkCode == VK_ESCAPE) {
            g_armed = false;
        } else if (isUp && g_armed &&
                   (kbd->vkCode == VK_MENU || kbd->vkCode == VK_LMENU ||
                    kbd->vkCode == VK_RMENU)) {
            // Alt released: schedule the move.
            g_armed = false;
            g_retries = 0;
            SetTimer(g_hwnd, TIMER_MOVE, (UINT)g_delayMs, nullptr);
        }
    }
    return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

static DWORD WINAPI ThreadProc(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"AltTabMouseFocusWindow";
    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
                             HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        return 1;
    }

    g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                 GetModuleHandleW(nullptr), 0);
    if (!g_kbHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
        return 1;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hwnd)
        PostMessageW(g_hwnd, WM_ALT_TAB_QUIT, 0, 0);

    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    if (g_hwnd)
        PostMessageW(g_hwnd, WM_ALT_TAB_RELOAD, 0, 0);
}

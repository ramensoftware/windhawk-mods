// ==WindhawkMod==
// @id              snipping-tool-prefocus
// @name            Snipping Tool Pre-Focus
// @description     Focus the taskbar before Print Screen opens the native Windows Snipping Tool
// @version         0.1
// @author          satoshate
// @github          https://github.com/satoshate
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snipping Tool Pre-Focus

Focuses the Windows taskbar immediately before a plain `Print Screen` key press
continues to the native Windows Snipping Tool.

This keeps the built-in screenshot UI available, including region capture,
window capture, full-screen capture, annotation, and OCR/text actions. The goal
is to automate the manual workaround of clicking the taskbar before taking a
screenshot when the currently focused elevated window makes Snipping Tool behave
poorly.

Notes:

- Enable Windows' "Use the Print screen key to open Snipping Tool" setting.
- Only plain `Print Screen` is handled. Modifier combinations are left alone.
- Secure Desktop/UAC prompts are not captured and are intentionally out of
  scope.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- focusDelayMs: 80
  $name: Focus delay in milliseconds
  $description: Small delay after focusing the taskbar before Windows handles Print Screen.
- ignoreIfCtrlAltShiftPressed: true
  $name: Ignore modifier combinations
  $description: Leave Ctrl/Alt/Shift + Print Screen combinations untouched.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <windows.h>

struct Settings {
    int focusDelayMs;
    bool ignoreIfCtrlAltShiftPressed;
};

Settings g_settings;
HHOOK g_keyboardHook;
HANDLE g_hookThread;
DWORD g_hookThreadId;
std::atomic<DWORD> g_lastFocusTick;

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&LowLevelKeyboardProc),
                            &module)) {
        Wh_Log(L"GetModuleHandleExW failed: %u", GetLastError());
    }

    return module;
}

void LoadSettings() {
    g_settings.focusDelayMs = Wh_GetIntSetting(L"focusDelayMs");
    if (g_settings.focusDelayMs < 0) {
        g_settings.focusDelayMs = 0;
    }
    if (g_settings.focusDelayMs > 1000) {
        g_settings.focusDelayMs = 1000;
    }

    g_settings.ignoreIfCtrlAltShiftPressed =
        Wh_GetIntSetting(L"ignoreIfCtrlAltShiftPressed") != 0;
}

bool IsKeyDown(int virtualKey) {
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool AnyModifierDown() {
    return IsKeyDown(VK_CONTROL) || IsKeyDown(VK_LCONTROL) ||
           IsKeyDown(VK_RCONTROL) || IsKeyDown(VK_MENU) ||
           IsKeyDown(VK_LMENU) || IsKeyDown(VK_RMENU) ||
           IsKeyDown(VK_SHIFT) || IsKeyDown(VK_LSHIFT) ||
           IsKeyDown(VK_RSHIFT);
}

void FocusTaskbar() {
    // Guard against keyboard auto-repeat and duplicate low-level hook delivery.
    DWORD now = GetTickCount();
    DWORD previous = g_lastFocusTick.exchange(now);
    if (previous && now - previous < 250) {
        return;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        Wh_Log(L"Shell_TrayWnd not found");
        return;
    }

    SetForegroundWindow(taskbar);

    if (g_settings.focusDelayMs > 0) {
        Sleep((DWORD)g_settings.focusDelayMs);
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* data =
        reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

    if (data->vkCode != VK_SNAPSHOT) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    if (data->flags & (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    if (g_settings.ignoreIfCtrlAltShiftPressed && AnyModifierDown()) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    FocusTaskbar();

    // Let Windows continue handling Print Screen, preserving native Snipping
    // Tool behavior and OCR/text actions.
    return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
}

DWORD WINAPI HookThreadProc(void*) {
    g_hookThreadId = GetCurrentThreadId();

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                          GetCurrentModuleHandle(), 0);
    if (!g_keyboardHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        return 1;
    }

    Wh_Log(L"Snipping Tool Pre-Focus hook installed");

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_lastFocusTick.store(0);

    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

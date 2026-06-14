// ==WindhawkMod==
// @id              capslock-copy-paste-alternator
// @name            Caps Lock Copy/Paste Alternator
// @description     Remap Caps Lock to alternating Ctrl+C and Ctrl+V
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
# Caps Lock Copy/Paste Alternator

Turns the physical Caps Lock key into a stateful clipboard helper:

- first press sends `Ctrl+C`;
- second press sends `Ctrl+V`;
- third press sends `Ctrl+C` again;
- the original Caps Lock key event is suppressed;
- Caps Lock LED/state is forced off by default.

The mod installs a low-level keyboard hook from Explorer. It works best for
normal desktop windows. Windows integrity levels can block input into elevated
applications if Explorer is not elevated, so for elevated/admin windows an
elevated helper or elevated Windhawk process may still be required.

Secure Desktop/UAC prompts are intentionally out of scope.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startWithPaste: false
  $name: Start with Paste
  $description: If enabled, the next Caps Lock press after loading the mod sends Ctrl+V instead of Ctrl+C.
- forceCapsLockOff: true
  $name: Force Caps Lock off
  $description: Keep the Caps Lock toggle state off while the remap is active.
- resetToCopyAfterSeconds: 0
  $name: Reset to Copy after idle seconds
  $description: Set to 0 to keep strict alternation forever. Set a positive value to reset the next action to Copy after inactivity.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <windows.h>

enum class Action {
    Copy,
    Paste,
};

struct Settings {
    bool startWithPaste;
    bool forceCapsLockOff;
    int resetToCopyAfterSeconds;
};

Settings g_settings;
HHOOK g_keyboardHook;
HANDLE g_hookThread;
DWORD g_hookThreadId;
std::atomic<bool> g_capsDown;
std::atomic<int> g_nextAction;
ULONGLONG g_lastActionTick;

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
    g_settings.startWithPaste = Wh_GetIntSetting(L"startWithPaste") != 0;
    g_settings.forceCapsLockOff = Wh_GetIntSetting(L"forceCapsLockOff") != 0;
    g_settings.resetToCopyAfterSeconds =
        Wh_GetIntSetting(L"resetToCopyAfterSeconds");

    if (g_settings.resetToCopyAfterSeconds < 0) {
        g_settings.resetToCopyAfterSeconds = 0;
    }

    g_nextAction.store(g_settings.startWithPaste ? (int)Action::Paste
                                                 : (int)Action::Copy);
}

bool IsCapsLockOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

void EnsureCapsLockOff() {
    if (!g_settings.forceCapsLockOff || !IsCapsLockOn()) {
        return;
    }

    keybd_event(VK_CAPITAL, 0x3A, 0, 0);
    keybd_event(VK_CAPITAL, 0x3A, KEYEVENTF_KEYUP, 0);
}

void SendCtrlKey(WORD key) {
    INPUT inputs[4] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = key;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (sent != ARRAYSIZE(inputs)) {
        Wh_Log(L"SendInput failed: sent=%u error=%u", sent, GetLastError());
    }
}

void SendAction(Action action) {
    SendCtrlKey(action == Action::Copy ? 'C' : 'V');
}

void MaybeResetAfterIdle() {
    if (g_settings.resetToCopyAfterSeconds <= 0 || g_lastActionTick == 0) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG idleMs = now - g_lastActionTick;
    if (idleMs >= (ULONGLONG)g_settings.resetToCopyAfterSeconds * 1000) {
        g_nextAction.store((int)Action::Copy);
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* data =
        reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

    if (data->vkCode != VK_CAPITAL) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    // Allow injected Caps Lock events so our own EnsureCapsLockOff toggle can
    // actually correct the keyboard state.
    if (data->flags & (LLKHF_INJECTED | LLKHF_LOWER_IL_INJECTED)) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    bool isKeyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool isKeyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (isKeyUp) {
        g_capsDown.store(false);
        EnsureCapsLockOff();
        return 1;
    }

    if (!isKeyDown) {
        return CallNextHookEx(g_keyboardHook, code, wParam, lParam);
    }

    if (g_capsDown.exchange(true)) {
        return 1;
    }

    MaybeResetAfterIdle();

    Action action = (Action)g_nextAction.load();
    g_nextAction.store(action == Action::Copy ? (int)Action::Paste
                                              : (int)Action::Copy);
    g_lastActionTick = GetTickCount64();

    SendAction(action);
    EnsureCapsLockOff();

    return 1;
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

    Wh_Log(L"Caps Lock Copy/Paste hook installed");
    EnsureCapsLockOff();

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
    g_capsDown.store(false);
    g_lastActionTick = 0;

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

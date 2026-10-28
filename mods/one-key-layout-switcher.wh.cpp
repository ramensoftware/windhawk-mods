// ==WindhawkMod==
// @id              one-key-layout-switcher
// @name            One Key Keyboard Layout Switcher
// @description     Switch keyboard layout with a single key press (Win, Alt, or Menu key)
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         dwm.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Switches keyboard layout with a single key press instead of a two-key combination.
Intercepts keyboard events at the lowest possible user-mode level (WH_KEYBOARD_LL).

This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- switchMethod: both
  $name: Switch method
  $description: Choose which key combination to send for layout switching
  $options:
  - altshift: Alt+Shift
  - ctrlshift: Ctrl+Shift
  - both: Both Ctrl+Shift and Alt+Shift
- enableLeftWin: false
  $name: Enable Left Win key
  $description: Use Left Windows key to switch layout
- enableRightWin: true
  $name: Enable Right Win key
  $description: Use Right Windows key to switch layout
- enableLeftAlt: false
  $name: Enable Left Alt key
  $description: Use Left Alt key to switch layout
- enableRightAlt: false
  $name: Enable Right Alt key
  $description: Use Right Alt key to switch layout
- enableMenu: true
  $name: Enable Menu key
  $description: Use Menu (Application) key to switch layout
*/
// ==/WindhawkModSettings==

#include <windows.h>

HHOOK  g_hook     = NULL;
HANDLE g_thread   = NULL;
DWORD  g_threadId = 0;
HANDLE g_threadReadyEvent = NULL;

enum class SwitchMethod {
    AltShift,
    CtrlShift,
    Both
};

struct {
    SwitchMethod switchMethod;
    BOOL enableLeftWin;
    BOOL enableRightWin;
    BOOL enableLeftAlt;
    BOOL enableRightAlt;
    BOOL enableMenu;
} settings;

// Track modifier key down states to suppress auto-repeat
static bool s_keyDown[256] = {};

// Custom message for layout switching
constexpr UINT WM_SWITCH_LAYOUT = WM_APP + 1;

void LoadSettings() {
    PCWSTR method = Wh_GetStringSetting(L"switchMethod");
    
    if (!wcscmp(method, L"altshift")) {
        settings.switchMethod = SwitchMethod::AltShift;
    } else if (!wcscmp(method, L"ctrlshift")) {
        settings.switchMethod = SwitchMethod::CtrlShift;
    } else {
        settings.switchMethod = SwitchMethod::Both;
    }
    
    Wh_FreeStringSetting(method);

    settings.enableLeftWin  = Wh_GetIntSetting(L"enableLeftWin");
    settings.enableRightWin = Wh_GetIntSetting(L"enableRightWin");
    settings.enableLeftAlt  = Wh_GetIntSetting(L"enableLeftAlt");
    settings.enableRightAlt = Wh_GetIntSetting(L"enableRightAlt");
    settings.enableMenu     = Wh_GetIntSetting(L"enableMenu");
}

bool ShouldIntercept(DWORD vk) {
    switch (vk) {
        case VK_LWIN:  return settings.enableLeftWin;
        case VK_RWIN:  return settings.enableRightWin;
        case VK_LMENU: return settings.enableLeftAlt;
        case VK_RMENU: return settings.enableRightAlt;
        case VK_APPS:  return settings.enableMenu;
        default:       return false;
    }
}

void SendLayoutSwitch() {
    if (settings.switchMethod == SwitchMethod::CtrlShift ||
        settings.switchMethod == SwitchMethod::Both) {

        Wh_Log(L"Sending Ctrl+Shift");

        INPUT inputs[4] = {};

        inputs[0].type       = INPUT_KEYBOARD;
        inputs[0].ki.wVk     = VK_LCONTROL;
        inputs[0].ki.dwFlags = 0;

        inputs[1].type       = INPUT_KEYBOARD;
        inputs[1].ki.wVk     = VK_LSHIFT;
        inputs[1].ki.dwFlags = 0;

        inputs[2].type       = INPUT_KEYBOARD;
        inputs[2].ki.wVk     = VK_LSHIFT;
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

        inputs[3].type       = INPUT_KEYBOARD;
        inputs[3].ki.wVk     = VK_LCONTROL;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(4, inputs, sizeof(INPUT));
    }

    if (settings.switchMethod == SwitchMethod::AltShift ||
        settings.switchMethod == SwitchMethod::Both) {

        Wh_Log(L"Sending Alt+Shift");

        INPUT inputs[4] = {};

        inputs[0].type       = INPUT_KEYBOARD;
        inputs[0].ki.wVk     = VK_LMENU;
        inputs[0].ki.dwFlags = 0;

        inputs[1].type       = INPUT_KEYBOARD;
        inputs[1].ki.wVk     = VK_LSHIFT;
        inputs[1].ki.dwFlags = 0;

        inputs[2].type       = INPUT_KEYBOARD;
        inputs[2].ki.wVk     = VK_LSHIFT;
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

        inputs[3].type       = INPUT_KEYBOARD;
        inputs[3].ki.wVk     = VK_LMENU;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(4, inputs, sizeof(INPUT));
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vk = kb->vkCode;

        // Ignore injected events (from our own SendInput)
        if (kb->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_hook, nCode, wParam, lParam);
        }

        if (ShouldIntercept(vk)) {
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

            if (isKeyDown) {
                // Suppress auto-repeat
                if (!s_keyDown[vk]) {
                    s_keyDown[vk] = true;
                    Wh_Log(L"Key down intercepted: VK=0x%X", vk);
                    // Post message to worker thread instead of blocking here
                    PostThreadMessage(g_threadId, WM_SWITCH_LAYOUT, 0, 0);
                }
                // Block the key
                return 1;
            }

            if (isKeyUp) {
                s_keyDown[vk] = false;
                Wh_Log(L"Key up intercepted: VK=0x%X", vk);
                // Block the key
                return 1;
            }
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

DWORD WINAPI MsgThread(LPVOID) {
    // Prime the message queue before signaling ready
    MSG msg;
    PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE);
    
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %d", GetLastError());
        SetEvent(g_threadReadyEvent);
        return 1;
    }

    Wh_Log(L"Hook installed, entering message loop");
    
    // Signal that the thread is ready
    SetEvent(g_threadReadyEvent);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_SWITCH_LAYOUT) {
            // Handle layout switching here, outside the hook callback
            SendLayoutSwitch();
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"=== Keyboard Layout Switcher Init ===");

    LoadSettings();

    Wh_Log(L"Method=%d LWin=%d RWin=%d LAlt=%d RAlt=%d Menu=%d",
           (int)settings.switchMethod,
           settings.enableLeftWin,
           settings.enableRightWin,
           settings.enableLeftAlt,
           settings.enableRightAlt,
           settings.enableMenu);

    // Create event to signal when thread is ready
    g_threadReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!g_threadReadyEvent) {
        Wh_Log(L"CreateEvent failed: %d", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(NULL, 0, MsgThread, NULL, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %d", GetLastError());
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = NULL;
        return FALSE;
    }

    // Wait for thread to be ready before returning
    WaitForSingleObject(g_threadReadyEvent, INFINITE);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== Keyboard Layout Switcher Uninit ===");

    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        // Use bounded wait instead of INFINITE to prevent potential hang
        DWORD result = WaitForSingleObject(g_thread, 1000);
        if (result == WAIT_TIMEOUT) {
            Wh_Log(L"Warning: Worker thread did not exit within timeout");
        }
        CloseHandle(g_thread);
        g_thread   = NULL;
        g_threadId = 0;
    }

    if (g_threadReadyEvent) {
        CloseHandle(g_threadReadyEvent);
        g_threadReadyEvent = NULL;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");
    LoadSettings();
}

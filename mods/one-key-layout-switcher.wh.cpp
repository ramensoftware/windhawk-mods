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
- switchMethod: ctrlshift
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
- enableRightAlt: true
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

struct {
    WCHAR switchMethod[32];
    BOOL enableLeftWin;
    BOOL enableRightWin;
    BOOL enableLeftAlt;
    BOOL enableRightAlt;
    BOOL enableMenu;
} settings;

// Track modifier key down states to suppress auto-repeat
static bool s_keyDown[256] = {};

void LoadSettings() {
    PCWSTR method = Wh_GetStringSetting(L"switchMethod");
    wcscpy_s(settings.switchMethod, 32, method ? method : L"ctrlshift");
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
    if (wcscmp(settings.switchMethod, L"ctrlshift") == 0 ||
        wcscmp(settings.switchMethod, L"both") == 0) {

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

    if (wcscmp(settings.switchMethod, L"altshift") == 0 ||
        wcscmp(settings.switchMethod, L"both") == 0) {

        Wh_Log(L"Sending Alt+Shift");

        if (wcscmp(settings.switchMethod, L"both") == 0) {
            Sleep(100);
        }

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
                    SendLayoutSwitch();
                }
                // Block the key - do NOT call CallNextHookEx
                return 1;
            }

            if (isKeyUp) {
                s_keyDown[vk] = false;
                Wh_Log(L"Key up intercepted: VK=0x%X", vk);
                // Block the key - do NOT call CallNextHookEx
                return 1;
            }
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

DWORD WINAPI MsgThread(LPVOID) {
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %d", GetLastError());
        return 1;
    }

    Wh_Log(L"Hook installed, entering message loop");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

    Wh_Log(L"Method=%s LWin=%d RWin=%d LAlt=%d RAlt=%d Menu=%d",
           settings.switchMethod,
           settings.enableLeftWin,
           settings.enableRightWin,
           settings.enableLeftAlt,
           settings.enableRightAlt,
           settings.enableMenu);

    g_thread = CreateThread(NULL, 0, MsgThread, NULL, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %d", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== Keyboard Layout Switcher Uninit ===");

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }

    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread   = NULL;
        g_threadId = 0;
    }

    memset(s_keyDown, 0, sizeof(s_keyDown));
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");
    LoadSettings();
}

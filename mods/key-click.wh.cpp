// ==WindhawkMod==
// @id              key-click
// @name            Key Click
// @description     Produces click sound on keypress, supports autorepeat
// @version         1.2
// @author          Anixx
// @github          https://github.com/Anixx
// @compilerOptions -lwinmm
// @include         dwm.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Produces click sound on keypress, supports autorepeat.
Modifier keys (Ctrl, Shift, Alt, Win) do not produce autorepeat sounds.
Intercepts keyboard events at the lowest possible user-mode level (WH_KEYBOARD_LL),
before remappers that use SendInput/Raw Input.

 This mod needs to hook into `dwm.exe` to work. Please navigate to Windhawk's 
 Settings > Advanced settings > More advanced settings > Process inclusion list, 
 and make sure that `dwm.exe` is in the list.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <mmsystem.h>
#include <cstdio>

HWND  g_hwnd     = NULL;
HANDLE g_thread  = NULL;
DWORD  g_threadId = 0;
HINSTANCE g_hInst = NULL;
HHOOK  g_hook    = NULL;

static const wchar_t* CLASS_NAME = L"KeyClickWndClass";

static const unsigned char clickWav[] = {
    // RIFF Header
    'R','I','F','F', 38,0,0,0, 'W','A','V','E',
    // fmt
    'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
    0x40,0x1F,0,0,   // Sample Rate 8000 Hz
    0x40,0x1F,0,0,   // Byte Rate
    1,0, 8,0,        // Block Align, Bits per sample
    // data
    'd','a','t','a', 8,0,0,0,
    128, 255, 0, 255, 0, 200, 80, 128
};

void PlayClick() {
    PlaySoundA((LPCSTR)clickWav, NULL,
        SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
}

// Отслеживание состояния модификаторов для подавления автоповтора
static bool s_modifierDown[256] = {};

static bool IsModifier(DWORD vkey) {
    return (vkey == VK_SHIFT    ||
            vkey == VK_LSHIFT   ||
            vkey == VK_RSHIFT   ||
            vkey == VK_CONTROL  ||
            vkey == VK_LCONTROL ||
            vkey == VK_RCONTROL ||
            vkey == VK_MENU     ||
            vkey == VK_LMENU    ||
            vkey == VK_RMENU    ||
            vkey == VK_LWIN     ||
            vkey == VK_RWIN);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkey = kb->vkCode;

        // Фильтруем инъецированные события (от SendInput/remapper'ов)
        // LLKHF_INJECTED = 0x10
        bool isInjected = (kb->flags & LLKHF_INJECTED) != 0;

        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

        if (!isInjected) {
            if (isKeyUp) {
                if (vkey < 256 && IsModifier(vkey)) {
                    s_modifierDown[vkey] = false;
                }
            } else if (isKeyDown) {
                if (IsModifier(vkey) && vkey < 256) {
                    bool isAutoRepeat = s_modifierDown[vkey];
                    s_modifierDown[vkey] = true;
                    if (!isAutoRepeat) {
                        PlayClick();
                    }
                } else {
                    PlayClick();
                }
            }
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

DWORD WINAPI MsgThread(LPVOID) {
    // WH_KEYBOARD_LL можно ставить из любого потока,
    // но hMod должен быть NULL для глобального LL-хука
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %d", GetLastError());
        return 1;
    }

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
    g_hInst = GetModuleHandle(NULL);
    g_thread = CreateThread(NULL, 0, MsgThread, NULL, 0, &g_threadId);
    return g_thread != NULL;
}

void Wh_ModUninit() {
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
        g_thread = NULL;
    }
}

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
#include <atomic>

// ---------------------------------------------------------------------------
// Settings — written on settings-change thread, read on hook thread.
// Each field is a separate atomic so reads in the hot path are lock-free.
// ---------------------------------------------------------------------------
static std::atomic<bool> g_enableLeftWin  {false};
static std::atomic<bool> g_enableRightWin {true};
static std::atomic<bool> g_enableLeftAlt  {false};
static std::atomic<bool> g_enableRightAlt {false};
static std::atomic<bool> g_enableMenu     {true};

static void LoadSettings() {
    g_enableLeftWin .store(Wh_GetIntSetting(L"enableLeftWin")  != 0);
    g_enableRightWin.store(Wh_GetIntSetting(L"enableRightWin") != 0);
    g_enableLeftAlt .store(Wh_GetIntSetting(L"enableLeftAlt")  != 0);
    g_enableRightAlt.store(Wh_GetIntSetting(L"enableRightAlt") != 0);
    g_enableMenu    .store(Wh_GetIntSetting(L"enableMenu")      != 0);
}

// ---------------------------------------------------------------------------
// Layout switching — posts WM_INPUTLANGCHANGEREQUEST directly to the
// foreground window, so it works regardless of the user's hotkey config.
// INPUTLANGCHANGE_FORWARD (0x0002) asks for the next layout in the list.
// ---------------------------------------------------------------------------
static void SwitchLayout() {
    HWND fg = GetForegroundWindow();
    if (!fg) {
        Wh_Log(L"SwitchLayout: no foreground window");
        return;
    }

    // Get the current layout for the foreground thread
    DWORD fgThreadId = GetWindowThreadProcessId(fg, nullptr);
    HKL currentHkl   = GetKeyboardLayout(fgThreadId);

    // Ask Windows for the next layout in the list
    HKL nextHkl = (HKL)GetKeyboardLayoutList(0, nullptr);
    {
        int count = GetKeyboardLayoutList(0, nullptr);
        if (count <= 1) {
            Wh_Log(L"SwitchLayout: only one layout installed, nothing to do");
            return;
        }

        HKL* list = new HKL[count];
        GetKeyboardLayoutList(count, list);

        nextHkl = list[0]; // fallback
        for (int i = 0; i < count; i++) {
            if (list[i] == currentHkl) {
                nextHkl = list[(i + 1) % count];
                break;
            }
        }
        delete[] list;
    }

    Wh_Log(L"SwitchLayout: fg=0x%p currentHkl=0x%p nextHkl=0x%p",
           (void*)fg, (void*)currentHkl, (void*)nextHkl);

    // Post to the foreground window — same mechanism Windows itself uses
    PostMessage(fg, WM_INPUTLANGCHANGEREQUEST,
                INPUTLANGCHANGE_FORWARD,
                (LPARAM)nextHkl);
}

// ---------------------------------------------------------------------------
// Hook
// ---------------------------------------------------------------------------
static HHOOK g_hook = NULL;

// Track which intercepted keys are currently held to suppress auto-repeat
static bool s_keyDown[256] = {};

static bool ShouldIntercept(DWORD vk) {
    switch (vk) {
        case VK_LWIN:  return g_enableLeftWin .load();
        case VK_RWIN:  return g_enableRightWin.load();
        case VK_LMENU: return g_enableLeftAlt .load();
        case VK_RMENU: return g_enableRightAlt.load();
        case VK_APPS:  return g_enableMenu    .load();
        default:       return false;
    }
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        DWORD vk = kb->vkCode;

        // Ignore injected events (from SendInput etc.)
        if (kb->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_hook, nCode, wParam, lParam);
        }

        if (ShouldIntercept(vk)) {
            bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool isUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

            if (isDown) {
                if (!s_keyDown[vk]) {
                    s_keyDown[vk] = true;
                    Wh_Log(L"Key down: VK=0x%X -> switching layout", vk);
                    SwitchLayout();
                }
                return 1; // Block, do NOT call CallNextHookEx
            }

            if (isUp) {
                s_keyDown[vk] = false;
                Wh_Log(L"Key up: VK=0x%X blocked", vk);
                return 1; // Block
            }
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread — owns the hook and runs the message loop
// ---------------------------------------------------------------------------
static HANDLE g_thread   = NULL;
static DWORD  g_threadId = 0;
static HANDLE g_readyEvent = NULL; // signaled once the queue is primed

static DWORD WINAPI MsgThread(LPVOID) {
    // Prime the message queue before installing the hook so that a
    // WM_QUIT posted from another thread is never lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Signal that the queue exists and is ready
    SetEvent(g_readyEvent);

    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %d", GetLastError());
        return 1;
    }

    Wh_Log(L"Hook installed, entering message loop");

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hook);
    g_hook = NULL;

    Wh_Log(L"Message loop exited");
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"=== Keyboard Layout Switcher Init ===");

    LoadSettings();

    Wh_Log(L"Settings: LWin=%d RWin=%d LAlt=%d RAlt=%d Menu=%d",
           (int)g_enableLeftWin.load(),
           (int)g_enableRightWin.load(),
           (int)g_enableLeftAlt.load(),
           (int)g_enableRightAlt.load(),
           (int)g_enableMenu.load());

    // Create the ready event before the thread so there's no race
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed: %d", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, MsgThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %d", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Wait until the worker thread has primed its message queue
    WaitForSingleObject(g_readyEvent, INFINITE);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

    Wh_Log(L"Worker thread ready");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== Keyboard Layout Switcher Uninit ===");

    // Safe to post now because we waited for the queue to be primed
    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread   = nullptr;
        g_threadId = 0;
    }

    memset(s_keyDown, 0, sizeof(s_keyDown));
    Wh_Log(L"Uninit complete");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");
    LoadSettings();
}

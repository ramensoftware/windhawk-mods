// ==WindhawkMod==
// @id              win-e-reuse-explorer
// @name            Win+E Reuse File Explorer
// @description     Restore an existing File Explorer window with Win+E, or open one normally if none exists
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+E Reuse File Explorer

Press **Win+E** to bring the most recently active File Explorer window forward.
Minimized windows are restored. If Explorer is already focused, it stays focused.

If no Explorer window exists on the current virtual desktop, Windows handles
Win+E normally and opens a new window. Extra modifiers (Ctrl, Shift, or Alt)
are left unchanged. There are no settings.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>

static HANDLE g_thread;
static HANDLE g_ready;
static DWORD g_threadId;
static bool g_hookInstalled;
static bool g_suppressE;
static constexpr UINT kRestoreWindow = WM_APP + 1;

static BOOL CALLBACK FindExplorerWindow(HWND window, LPARAM param) {
    if (!IsWindowVisible(window) || GetWindow(window, GW_OWNER)) {
        return TRUE;
    }

    wchar_t className[32];
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (lstrcmpW(className, L"CabinetWClass") != 0 &&
         lstrcmpW(className, L"ExploreWClass") != 0)) {
        return TRUE;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED,
                                      &cloaked, sizeof(cloaked))) && cloaked) {
        return TRUE;
    }

    // EnumWindows visits top-level windows in Z order.
    *reinterpret_cast<HWND*>(param) = window;
    return FALSE;
}

static bool KeyDown(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

static LRESULT CALLBACK KeyboardHook(int code, WPARAM message, LPARAM param) {
    if (code == HC_ACTION) {
        const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(param);
        if (key->vkCode == 'E' && !(key->flags & LLKHF_INJECTED)) {
            const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
            const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
            if (g_suppressE) {
                if (up) {
                    g_suppressE = false;
                }
                return 1;
            }

            if (down && (KeyDown(VK_LWIN) || KeyDown(VK_RWIN)) &&
                !KeyDown(VK_CONTROL) && !KeyDown(VK_SHIFT) && !KeyDown(VK_MENU)) {
                HWND window = nullptr;
                EnumWindows(FindExplorerWindow, reinterpret_cast<LPARAM>(&window));
                if (window && PostThreadMessageW(g_threadId, kRestoreWindow,
                                                reinterpret_cast<WPARAM>(window), 0)) {
                    g_suppressE = true;

                    // Mark Win as used so releasing it doesn't open Start.
                    INPUT input[2] = {};
                    input[0].type = input[1].type = INPUT_KEYBOARD;
                    input[0].ki.wVk = input[1].ki.wVk = 0xFF;  // Unassigned VK.
                    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, message, param);
}

static DWORD WINAPI HookThread(void*) {
    MSG message;
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, nullptr, 0);
    g_hookInstalled = hook != nullptr;
    SetEvent(g_ready);
    if (!hook) {
        return 0;
    }

    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (message.message == kRestoreWindow) {
            HWND window = reinterpret_cast<HWND>(message.wParam);
            if (IsWindow(window)) {
                if (IsIconic(window)) {
                    ShowWindowAsync(window, SW_RESTORE);
                }
                SetForegroundWindow(window);
            }
        }
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

BOOL Wh_ModInit() {
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (g_thread) {
        WaitForSingleObject(g_ready, INFINITE);
    }
    CloseHandle(g_ready);
    g_ready = nullptr;
    if (!g_thread || !g_hookInstalled) {
        if (g_thread) {
            WaitForSingleObject(g_thread, INFINITE);
            CloseHandle(g_thread);
            g_thread = nullptr;
        }
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

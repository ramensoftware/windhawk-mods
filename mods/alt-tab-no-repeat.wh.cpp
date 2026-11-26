// ==WindhawkMod==
// @id            alt-tab-no-repeat
// @name          Alt+Tab No Repeat
// @description   Disable automatic repetition of Alt+Tab
// @version       1.0
// @author ElektroGvozd
// @github https://github.com/ElektroGvozd
// @include       explorer.exe
// ==/WindhawkMod==

#include <windows.h>

static HHOOK hook = nullptr;
static HANDLE worker = nullptr;
static HANDLE stopEvent = nullptr;
static bool tabDown = false;

LRESULT CALLBACK KeyboardProc(
    int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (key->vkCode == VK_TAB) {
            if (wParam == WM_KEYUP ||
                wParam == WM_SYSKEYUP) {
                tabDown = false;
            }

            if (wParam == WM_KEYDOWN ||
                wParam == WM_SYSKEYDOWN) {
                bool repeated = tabDown;
                tabDown = true;

                if (repeated &&
                    (key->flags & LLKHF_ALTDOWN)) {
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(hook, code, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID) {
    hook = SetWindowsHookExW(
        WH_KEYBOARD_LL, KeyboardProc,
        GetModuleHandleW(nullptr), 0);

    if (!hook)
        return 1;

    MSG msg;
    while (MsgWaitForMultipleObjects(
        1, &stopEvent, FALSE, INFINITE,
        QS_ALLINPUT) == WAIT_OBJECT_0 + 1) {
        while (PeekMessageW(
            &msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnhookWindowsHookEx(hook);
    hook = nullptr;
    return 0;
}

BOOL Wh_ModInit() {
    stopEvent = CreateEventW(
        nullptr, TRUE, FALSE, nullptr);

    if (!stopEvent)
        return FALSE;

    worker = CreateThread(
        nullptr, 0, HookThread,
        nullptr, 0, nullptr);

    if (!worker) {
        CloseHandle(stopEvent);
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    SetEvent(stopEvent);
    WaitForSingleObject(worker, INFINITE);
    CloseHandle(worker);
    CloseHandle(stopEvent);
}

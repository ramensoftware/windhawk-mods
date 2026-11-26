
// ==WindhawkMod==
// @id           alt-tab-no-repeat
// @name         Alt+Tab No Repeat
// @description  Disable automatic repetition of Alt+Tab
// @version      1.0
// @author       ElektroGvozd
// @github       https://github.com/ElektroGvozd
// @include      windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab No Repeat

Prevents automatic repetition of Alt+Tab when the keys are held down.

Holding Alt+Tab opens the window switcher without continuously cycling through windows.
Release and press Tab again to move to the next window.

Shift+Alt+Tab and Ctrl+Alt+Tab are also affected.

The keyboard hook is system-wide and suppresses repeated Tab key-down events while Alt is held, including in third-party window switchers.

The mod runs in a dedicated Windhawk process.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>

static HHOOK hook = nullptr;
static HANDLE worker = nullptr;
static HANDLE stopEvent = nullptr;
static HANDLE readyEvent = nullptr;
static bool tabDown = false;
static bool hookInstalled = false;

LRESULT CALLBACK KeyboardProc(
    int code, WPARAM wParam, LPARAM lParam) {

    if (code == HC_ACTION) {
        auto* key =
            reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        bool keyUp =
            wParam == WM_KEYUP ||
            wParam == WM_SYSKEYUP;

        bool keyDown =
            wParam == WM_KEYDOWN ||
            wParam == WM_SYSKEYDOWN;

        if (keyUp &&
            (key->vkCode == VK_LMENU ||
             key->vkCode == VK_RMENU ||
             key->vkCode == VK_MENU)) {
            tabDown = false;
        }

        if (key->vkCode == VK_TAB) {
            if (keyUp) {
                tabDown = false;
            }

            if (keyDown) {
                bool repeated = tabDown;
                tabDown = true;

                if (repeated &&
                    (key->flags & LLKHF_ALTDOWN)) {
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(
        nullptr, code, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID) {
    MSG msg;

    PeekMessageW(
        &msg, nullptr, 0, 0, PM_NOREMOVE);

    hook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandleW(nullptr),
        0);

    hookInstalled = hook != nullptr;

    if (!hookInstalled) {
        Wh_Log(
            L"SetWindowsHookExW failed: %u",
            GetLastError());

        SetEvent(readyEvent);
        return 1;
    }

    SetEvent(readyEvent);

    while (true) {
        DWORD result = MsgWaitForMultipleObjects(
            1,
            &stopEvent,
            FALSE,
            INFINITE,
            QS_ALLINPUT);

        if (result == WAIT_OBJECT_0) {
            break;
        }

        if (result != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Message wait failed");
            break;
        }

        while (PeekMessageW(
            &msg, nullptr, 0, 0, PM_REMOVE)) {

            if (msg.message == WM_QUIT ||
                WaitForSingleObject(
                    stopEvent, 0) == WAIT_OBJECT_0) {
                UnhookWindowsHookEx(hook);
                hook = nullptr;
                return 0;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnhookWindowsHookEx(hook);
    hook = nullptr;

    return 0;
}

BOOL WhTool_ModInit() {
    tabDown = false;
    hookInstalled = false;

    stopEvent = CreateEventW(
        nullptr, TRUE, FALSE, nullptr);

    if (!stopEvent) {
        Wh_Log(L"Failed to create stop event");
        return FALSE;
    }

    readyEvent = CreateEventW(
        nullptr, TRUE, FALSE, nullptr);

    if (!readyEvent) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
        return FALSE;
    }

    worker = CreateThread(
        nullptr, 0, HookThread,
        nullptr, 0, nullptr);

    if (!worker) {
        CloseHandle(readyEvent);
        CloseHandle(stopEvent);

        readyEvent = nullptr;
        stopEvent = nullptr;

        return FALSE;
    }

    HANDLE handles[] = {readyEvent, worker};

    DWORD result = WaitForMultipleObjects(
        2, handles, FALSE, INFINITE);

    if (result != WAIT_OBJECT_0 ||
        !hookInstalled) {

        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);

        CloseHandle(worker);
        CloseHandle(readyEvent);
        CloseHandle(stopEvent);

        worker = nullptr;
        readyEvent = nullptr;
        stopEvent = nullptr;

        return FALSE;
    }

    CloseHandle(readyEvent);
    readyEvent = nullptr;

    return TRUE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    if (worker) {
        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);

        CloseHandle(worker);
        worker = nullptr;
    }

    if (stopEvent) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
    }

    tabDown = false;
    hookInstalled = false;
}

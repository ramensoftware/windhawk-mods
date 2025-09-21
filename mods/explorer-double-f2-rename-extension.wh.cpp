// ==WindhawkMod==
// @id              explorer-double-f2-rename-extension
// @name            Select filename extension on double F2
// @description     When pressing F2 in Explorer to rename a file, the filename is selected as usual, but double-pressing now selects the extension.
// @version         1
// @author          Marnes <leaumar@sent.com>
// @github          https://github.com/leaumar
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
QTTabbar and some other tools provide a feature where double-pressing F2 to
rename a file first selects the entire name (existing Explorer behavior) but the
second press selects just the extension. That's handy to e.g. rename a zip file
to a cbz file. This mod implements that same feature: double-press F2
in Explorer to rename a file's extension.

This works great together with [extension-change-no-warning](https://windhawk.net/mods/extension-change-no-warning).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DoubleF2MilliSeconds: 1000
*/
// ==/WindhawkModSettings==

// I am not an experienced C++ programmer and do not know the Windows APIs.
// This code was cobbled together with a lot trial & error, using input from
// ChatGPT and the Windhawk Discord server.

#include <string>

DWORD explorerThreadId;
HHOOK keyboardHook;

static ULONGLONG lastF2Time = 0;
static bool lastWasF2 = false;

static void SelectExtension(HWND edit) {
    int length = GetWindowTextLengthW(edit);
    if (length > 0) {
        std::wstring buffer(length, L'\0');
        GetWindowTextW(edit, &buffer[0], length + 1);

        size_t dotIndex = buffer.find_last_of(L'.');
        int start = (dotIndex != std::wstring::npos && dotIndex != 0)
                        ? (int)dotIndex + 1
                        : 0;
        SendMessageW(edit, EM_SETSEL, start, length);
    }
}

static HWND(WINAPI* originalCreateWindowExW)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             LPVOID lpParam);

static LRESULT CALLBACK HandleKeyboard(int nCode,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    auto shouldProcess = nCode >= 0;
    auto isF2 = wParam == VK_F2;
    auto isKeyUp = lParam & 0x80000000;

    if (shouldProcess && isKeyUp) {
        if (isF2) {
            ULONGLONG now = GetTickCount64();
            auto doubleF2Time = (DWORD)Wh_GetIntSetting(L"DoubleF2MilliSeconds");
            bool isDouble = lastWasF2 && (now - lastF2Time <= doubleF2Time);

            lastWasF2 = true;
            lastF2Time = now;

            Wh_Log(L"F2 pressed: isDouble=%d, delta=%llu.", isDouble ? 1 : 0,
                   now - lastF2Time);

            if (isDouble) {
                HWND focus = GetFocus();
                if (focus != nullptr) {
                    wchar_t cls[32];
                    GetClassNameW(focus, cls, _countof(cls));
                    if (_wcsicmp(cls, L"Edit") == 0) {
                        Wh_Log(
                            L"Double F2 in edit control, selecting extension.");
                        SelectExtension(focus);
                        return 0;  // swallow
                    }
                }
            }
        } else {
            lastWasF2 = false;
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

static HWND WINAPI HookedCreateWindowExW(DWORD dwExStyle,
                                         LPCWSTR lpClassName,
                                         LPCWSTR lpWindowName,
                                         DWORD dwStyle,
                                         int X,
                                         int Y,
                                         int nWidth,
                                         int nHeight,
                                         HWND hWndParent,
                                         HMENU hMenu,
                                         HINSTANCE hInstance,
                                         LPVOID lpParam) {
    // always call the original first
    HWND hwnd = originalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);

    // only proceed if hwnd is valid and fully created
    if (hwnd && IsWindow(hwnd)) {
        wchar_t cls[64];
        if (GetClassNameW(hwnd, cls, _countof(cls))) {
            if (_wcsicmp(cls, L"CabinetWClass") == 0 ||
                _wcsicmp(cls, L"ExploreWClass") == 0) {
                Wh_Log(L"New Explorer window created: hwnd=0x%p class=%s.",
                       hwnd, cls);
                explorerThreadId = GetWindowThreadProcessId(hwnd, nullptr);
                keyboardHook = SetWindowsHookExW(WH_KEYBOARD, HandleKeyboard,
                                                 nullptr, explorerThreadId);
            }
        }
    }

    return hwnd;
}

void Wh_ModInit() {
    Wh_Log(L"Double F2 initializing.");
    HWND shellWnd = GetShellWindow();
    explorerThreadId = GetWindowThreadProcessId(shellWnd, nullptr);
    keyboardHook = SetWindowsHookExW(WH_KEYBOARD, HandleKeyboard, nullptr,
                                     explorerThreadId);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)HookedCreateWindowExW,
                       (void**)&originalCreateWindowExW);
}

void Wh_ModUninit() {
    Wh_Log(L"Double F2 uninitializing.");

    if (keyboardHook != nullptr) {
        BOOL ok = UnhookWindowsHookEx(keyboardHook);
        Wh_Log(L"UnhookWindowsHookEx(keyboardHook) -> %d", ok ? 1 : 0);
        keyboardHook = nullptr;
    }

    // If you ever installed additional per-window hooks when new explorer
    // windows were created, make sure you unhook them when you replace them.
    // Example: if you set a new keyboardHook in hookedCreateWindowExW, you must
    // unhook the previous one before overwriting the variable.
    //
    // Windhawk removes the function hook you installed with Wh_SetFunctionHook
    // automatically, so you don't need to explicitly restore
    // originalCreateWindowExW here.
}

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

This mod works great together with
[extension-change-no-warning](https://windhawk.net/mods/extension-change-no-warning).
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
#include <unordered_map>

static void SelectExtension(HWND editControl) {
    // typical max filename length
    std::wstring buffer(260, L'\0');
    int copied = GetWindowTextW(editControl, buffer.data(), (int)buffer.size());
    if (copied <= 0) {
        return;
    }
    // trim to actual length
    buffer.resize(copied);

    size_t dotIndex = buffer.find_last_of(L'.');
    if (dotIndex != std::wstring::npos) {
        int start = (int)dotIndex + 1;

        SendMessageW(editControl, EM_SETSEL, start, (WPARAM)buffer.size());
        std::wstring extension = buffer.substr(start);
        Wh_Log(L"Selected extension \"%s\".", extension.c_str());
    }
}

static ULONGLONG lastF2Time = 0;
static bool lastWasF2 = false;
static bool CALLBACK SelectExtensionIfDoubleF2(int nCode,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    bool shouldProcess = nCode >= 0;
    bool isKeyUp = lParam & 0x80000000;
    if (!shouldProcess || !isKeyUp) {
        return false;
    }

    bool isF2 = wParam == VK_F2;
    if (!isF2) {
        lastWasF2 = false;
        return false;
    }

    auto doubleF2Time = (DWORD)Wh_GetIntSetting(L"DoubleF2MilliSeconds");
    ULONGLONG now = GetTickCount64();

    auto delta = now - lastF2Time;
    lastF2Time = now;
    bool isDouble = lastWasF2 && (delta <= doubleF2Time);
    lastWasF2 = true;

    Wh_Log(L"F2 pressed: isDouble=%d, delta=%llu.", isDouble, delta);

    if (isDouble) {
        HWND focus = GetFocus();
        if (focus != nullptr) {
            wchar_t cls[32];
            GetClassNameW(focus, cls, _countof(cls));
            if (_wcsicmp(cls, L"Edit") == 0) {
                Wh_Log(L"Double F2 in edit control, selecting extension.");
                SelectExtension(focus);
                return true;
            }
        }
    }

    return false;
}
static LRESULT CALLBACK HandleKeyEvent(int nCode,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    bool handled = SelectExtensionIfDoubleF2(nCode, wParam, lParam);
    return handled ? 0 : CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static std::unordered_map<DWORD, HHOOK> keyboardHooks;
static bool AddKeyboardHook(DWORD threadId) {
    if (threadId == 0) {
        Wh_Log(L"Refusing keyboard hook on bad thread id.");
        return false;
    }

    if (keyboardHooks.find(threadId) != keyboardHooks.end()) {
        Wh_Log(L"Thread %u already has a keyboard hook, skipping.", threadId);
        return false;
    }

    HHOOK hook =
        SetWindowsHookExW(WH_KEYBOARD, HandleKeyEvent, nullptr, threadId);

    if (hook == nullptr) {
        Wh_Log(L"Failed to hook thread %u.", threadId);
        return false;
    }

    keyboardHooks[threadId] = hook;
    Wh_Log(L"Installed keyboard hook %p on thread %u.", hook, threadId);
    return true;
}
static void RemoveKeyboardHooks() {
    for (auto& [threadId, hook] : keyboardHooks) {
        if (hook != nullptr) {
            bool ok = UnhookWindowsHookEx(hook);
            Wh_Log(L"Unhook %p -> %d.", hook, ok);
        }
    }
    keyboardHooks.clear();
}

static void HookIfExplorerFileView(HWND windowHandle, DWORD threadId) {
    if (windowHandle != nullptr && IsWindow(windowHandle)) {
        wchar_t clazz[64];
        if (GetClassNameW(windowHandle, clazz, _countof(clazz))) {
            // legacy explorer, modern explorer, desktop
            if (_wcsicmp(clazz, L"CabinetWClass") == 0 ||
                _wcsicmp(clazz, L"ExploreWClass") == 0 ||
                _wcsicmp(clazz, L"Progman") == 0) {
                AddKeyboardHook(threadId);
                Wh_Log(L"Hooked Explorer window: hwnd=0x%p class=%s.",
                       windowHandle, clazz);
            }
        }
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
    auto threadId = GetCurrentThreadId();
    HookIfExplorerFileView(hwnd, threadId);
    return hwnd;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    bool isExplorer = processId == GetCurrentProcessId();

    if (isExplorer) {
        HookIfExplorerFileView(hwnd, threadId);
    }

    // continue enumeration
    return true;
}

void Wh_ModInit() {
    Wh_Log(L"Hooking the desktop (shell) thread.");
    DWORD shellThreadId = GetWindowThreadProcessId(GetShellWindow(), nullptr);
    AddKeyboardHook(shellThreadId);

    Wh_Log(L"Hooking already open Explorer windows.");
    EnumWindows(EnumWindowsProc, 0);

    Wh_Log(L"Hooking Explorer window creation.");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)HookedCreateWindowExW,
                       (void**)&originalCreateWindowExW);
}

void Wh_ModUninit() {
    Wh_Log(L"Removing all keyboard hooks.");
    RemoveKeyboardHooks();
}

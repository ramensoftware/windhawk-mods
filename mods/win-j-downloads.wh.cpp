// ==WindhawkMod==
// @id              win-j-downloads
// @name            Win+J to Downloads
// @description     Maps the Win+J keyboard shortcut to open the Downloads folder.
// @version         1.0
// @author          Ahmad Mozaik
// @github          https://github.com/ahmadmozaik
// @include         explorer.exe
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+J to Downloads

This mod registers `Win+J` as a global hotkey and opens the current user's
Downloads folder when the shortcut is pressed.

The shortcut is handled by a message-only window on a background thread inside
File Explorer. If another application has already registered `Win+J`, the mod
logs the conflict and doesn't start.
*/
// ==/WindhawkModReadme==

#include <shellapi.h>
#include <windows.h>

namespace {

constexpr int kHotkeyId = 1;
constexpr wchar_t kWindowClassName[] = L"Windhawk_WinJ_Downloads_Window";

HANDLE g_thread = nullptr;
HANDLE g_readyEvent = nullptr;
DWORD g_threadId = 0;
bool g_threadInitialized = false;
HMODULE g_module = nullptr;

LRESULT CALLBACK HotkeyWindowProc(HWND window,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    if (message == WM_HOTKEY && wParam == kHotkeyId) {
        Wh_Log(L"Win+J pressed. Opening Downloads.");

        HINSTANCE result = ShellExecuteW(nullptr, L"open", L"shell:Downloads",
                                         nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(result) <= 32) {
            Wh_Log(L"ShellExecuteW failed: %Id",
                   reinterpret_cast<INT_PTR>(result));
        }

        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

DWORD WINAPI HotkeyThread(void*) {
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = HotkeyWindowProc;
    windowClass.hInstance = g_module;
    windowClass.lpszClassName = kWindowClassName;

    ATOM classAtom = RegisterClassW(&windowClass);
    if (!classAtom) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        SetEvent(g_readyEvent);
        return 0;
    }

    HWND window = CreateWindowExW(0, kWindowClassName, L"", 0, 0, 0, 0, 0,
                                  HWND_MESSAGE, nullptr, g_module, nullptr);
    if (!window) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(kWindowClassName, g_module);
        SetEvent(g_readyEvent);
        return 0;
    }

    if (!RegisterHotKey(window, kHotkeyId, MOD_WIN | MOD_NOREPEAT, 'J')) {
        Wh_Log(L"RegisterHotKey for Win+J failed: %lu", GetLastError());
        DestroyWindow(window);
        UnregisterClassW(kWindowClassName, g_module);
        SetEvent(g_readyEvent);
        return 0;
    }

    g_threadInitialized = true;
    SetEvent(g_readyEvent);

    MSG message;
    int result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (result == -1) {
        Wh_Log(L"GetMessageW failed: %lu", GetLastError());
    }

    UnregisterHotKey(window, kHotkeyId);
    DestroyWindow(window);
    UnregisterClassW(kWindowClassName, g_module);
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Win+J to Downloads.");

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&Wh_ModInit), &g_module)) {
        Wh_Log(L"GetModuleHandleExW failed: %lu", GetLastError());
        return FALSE;
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);
    if (!g_threadInitialized) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        CloseHandle(g_readyEvent);
        g_thread = nullptr;
        g_readyEvent = nullptr;
        g_threadId = 0;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Win+J to Downloads.");

    if (g_thread) {
        if (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
            Wh_Log(L"PostThreadMessageW failed: %lu", GetLastError());
        }

        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    g_threadId = 0;
    g_threadInitialized = false;
}


// ==WindhawkMod==
// @id              search-auto-input-disable
// @name            Search Auto Input Disable
// @description     Disables type-ahead in the File Explorer file view.
// @version         1.2
// @author          ipEtroIlIu
// @github          https://github.com/ipEtroIlIu
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Search Auto Input Disable

Disables automatic type-ahead in the File Explorer file view.

Normally, when you type characters while the file list is focused, Windows
either selects the first matching item or automatically enters the Search Box,
depending on your Folder Options settings.

This mod disables both behaviors, preventing accidental keyboard input from
changing the selected file or interrupting navigation.

## Features

- Disables automatic selection of files and folders when typing.
- Disables automatic switching to the Search Box when typing.
- Blocks letters, numbers, printable characters, Space, and numpad operators.
- Preserves the current file selection and scroll position.
- Does not block standard keyboard shortcuts involving Ctrl, Alt, or Win.
- Does not interfere with normal text input in the address bar, Search Box,
  or file rename field.
- Does not disable manual use of the Search Box.

## Built-in Windows alternative

If you only want to prevent Windows from switching to the Search Box,
you can use the built-in setting instead:

Folder Options -> View -> When typing into list view ->
Select the typed item in the view.

This mod goes further by disabling type-ahead entirely.

## Development

This modification was developed with the assistance of artificial intelligence.

## Feedback

If you encounter any bugs, compatibility issues, or have suggestions for
improvement, please report them on the project page.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <cwchar>

struct HookRecord
{
    DWORD threadId;
    HHOOK hook;
};

static std::vector<HookRecord> g_hooks;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerThread = nullptr;

// --------------------------------------------------
// Check window class
// --------------------------------------------------

bool IsWindowClass(HWND hwnd, const wchar_t* expected)
{
    if (!hwnd || !IsWindow(hwnd))
        return false;

    wchar_t className[128] = {};

    return GetClassNameW(
               hwnd,
               className,
               ARRAYSIZE(className)) &&
           wcscmp(className, expected) == 0;
}

// --------------------------------------------------
// Check whether window belongs to file view
// --------------------------------------------------

bool IsFileViewWindow(HWND hwnd)
{
    HWND current = hwnd;
    bool hasFileView = false;

    while (current)
    {
        if (IsWindowClass(current, L"SHELLDLL_DefView"))
        {
            hasFileView = true;
            break;
        }

        current = GetAncestor(current, GA_PARENT);
    }

    if (!hasFileView)
        return false;

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root)
        return false;

    return IsWindowClass(root, L"CabinetWClass") ||
           IsWindowClass(root, L"ExploreWClass");
}

// --------------------------------------------------
// Check whether this is the rename edit control
// --------------------------------------------------

bool IsRenameEditWindow(HWND hwnd)
{
    return IsWindowClass(hwnd, L"Edit") &&
           IsFileViewWindow(hwnd);
}

// --------------------------------------------------
// Keyboard modifiers
// --------------------------------------------------

bool HasCommandModifier()
{
    return
        (GetKeyState(VK_CONTROL) & 0x8000) ||
        (GetKeyState(VK_MENU) & 0x8000) ||
        (GetKeyState(VK_LWIN) & 0x8000) ||
        (GetKeyState(VK_RWIN) & 0x8000);
}

// --------------------------------------------------
// Printable virtual keys
// --------------------------------------------------

bool IsPrintableVirtualKey(WPARAM vk)
{
    if (vk >= 'A' && vk <= 'Z')
        return true;

    if (vk >= '0' && vk <= '9')
        return true;

    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
        return true;

    if (vk >= VK_OEM_1 && vk <= VK_OEM_8)
        return true;

    switch (vk)
    {
        case VK_SPACE:
        case VK_OEM_PLUS:
        case VK_OEM_COMMA:
        case VK_OEM_MINUS:
        case VK_OEM_PERIOD:
        case VK_OEM_102:
        case VK_MULTIPLY:
        case VK_ADD:
        case VK_SEPARATOR:
        case VK_SUBTRACT:
        case VK_DECIMAL:
        case VK_DIVIDE:
            return true;

        default:
            return false;
    }
}

// --------------------------------------------------
// Printable character messages
// --------------------------------------------------

bool IsPrintableCharacter(UINT message, WPARAM value)
{
    switch (message)
    {
        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_IME_CHAR:
            return value >= 0x20 && value != 0x7F;

        case WM_UNICHAR:
            return value != UNICODE_NOCHAR &&
                   value >= 0x20 &&
                   value != 0x7F;

        default:
            return false;
    }
}

// --------------------------------------------------
// Should this message be blocked?
// --------------------------------------------------

bool ShouldBlockMessage(const MSG* msg)
{
    if (!msg || !IsFileViewWindow(msg->hwnd))
        return false;

    // Allow typing into the file rename edit control.
    if (IsRenameEditWindow(msg->hwnd))
        return false;

    if (HasCommandModifier())
        return false;

    if (msg->message == WM_KEYDOWN)
        return IsPrintableVirtualKey(msg->wParam);

    if (msg->message == WM_CHAR ||
        msg->message == WM_SYSCHAR ||
        msg->message == WM_IME_CHAR ||
        msg->message == WM_UNICHAR)
    {
        return IsPrintableCharacter(
            msg->message,
            msg->wParam
        );
    }

    return false;
}

// --------------------------------------------------
// WH_GETMESSAGE callback
// --------------------------------------------------

LRESULT CALLBACK GetMessageHook(
    int code,
    WPARAM wParam,
    LPARAM lParam)
{
    if (code >= 0 && lParam && wParam == PM_REMOVE)
    {
        MSG* msg = reinterpret_cast<MSG*>(lParam);

        if (ShouldBlockMessage(msg))
        {
            msg->message = WM_NULL;
            msg->wParam = 0;
            msg->lParam = 0;
        }
    }

    return CallNextHookEx(
        nullptr,
        code,
        wParam,
        lParam
    );
}

// --------------------------------------------------
// Thread enumeration
// --------------------------------------------------

bool IsThreadHooked(DWORD threadId)
{
    for (const auto& record : g_hooks)
    {
        if (record.threadId == threadId)
            return true;
    }

    return false;
}

bool InstallThreadHook(DWORD threadId)
{
    if (IsThreadHooked(threadId))
        return true;

    HHOOK hook = SetWindowsHookExW(
        WH_GETMESSAGE,
        GetMessageHook,
        nullptr,
        threadId
    );

    if (!hook)
        return false;

    g_hooks.push_back({ threadId, hook });

    return true;
}

void ScanThreads()
{
    DWORD processId = GetCurrentProcessId();
    DWORD currentThreadId = GetCurrentThreadId();

    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPTHREAD,
        0
    );

    if (snapshot == INVALID_HANDLE_VALUE)
        return;

    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);

    if (Thread32First(snapshot, &entry))
    {
        do
        {
            if (entry.th32OwnerProcessID == processId &&
                entry.th32ThreadID != currentThreadId)
            {
                InstallThreadHook(entry.th32ThreadID);
            }

        } while (Thread32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
}

// --------------------------------------------------
// Background thread monitor
// --------------------------------------------------

DWORD WINAPI ThreadMonitorProc(LPVOID)
{
    while (WaitForSingleObject(g_stopEvent, 1000) == WAIT_TIMEOUT)
    {
        ScanThreads();
    }

    return 0;
}

// --------------------------------------------------
// Initialization
// --------------------------------------------------

BOOL Wh_ModInit()
{
    ScanThreads();

    if (g_hooks.empty())
        return FALSE;

    g_stopEvent = CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr
    );

    if (!g_stopEvent)
        goto failed;

    g_workerThread = CreateThread(
        nullptr,
        0,
        ThreadMonitorProc,
        nullptr,
        0,
        nullptr
    );

    if (!g_workerThread)
        goto failed;

    return TRUE;

failed:

    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    for (const auto& record : g_hooks)
    {
        if (record.hook)
            UnhookWindowsHookEx(record.hook);
    }

    g_hooks.clear();

    return FALSE;
}

// --------------------------------------------------
// Uninitialization
// --------------------------------------------------

void Wh_ModUninit()
{
    if (g_stopEvent)
        SetEvent(g_stopEvent);

    if (g_workerThread)
    {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    for (const auto& record : g_hooks)
    {
        if (record.hook)
            UnhookWindowsHookEx(record.hook);
    }

    g_hooks.clear();
}

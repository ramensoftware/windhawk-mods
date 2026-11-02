
// ==WindhawkMod==
// @id              search-auto-input-disable
// @name            Search Auto Input Disable
// @description     Disables type-to-search in the File Explorer file view.
// @version         1.0
// @author          ipEtroIlIu
// @include         explorer.exe
// @compilerOptions -std=c++17
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Search Auto Input Disable

Tired of Windows File Explorer unexpectedly starting a search whenever you accidentally press a key or type something?

For example, you're scrolling through a folder looking for a specific file. You finally find it, but an accidental key press switches the view to the search field. Your current position in the file list is lost, interrupting your navigation.

**Search Auto Input Disable** solves this by disabling automatic type-to-search in the File Explorer file view.

## Features

- Disables automatic search when typing letters, numbers, or other printable characters in the file list.
- Preserves your current scroll position and file selection, preventing accidental interruptions while navigating folders.
- Does not block standard keyboard shortcuts involving Ctrl, Alt, or Win.
- Does not interfere with normal text input in the address bar, search field, or file rename field.
- Does not disable the search feature itself — you can still use it manually whenever you need it.

## Development

This modification was developed with the assistance of artificial intelligence.

## Feedback

If you encounter any bugs, compatibility issues, or have suggestions for improvement, please report them on the project page.
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

    for (int i = 0; current && i < 12; i++)
    {
        if (IsWindowClass(current, L"SHELLDLL_DefView"))
            return true;

        current = GetParent(current);
    }

    return false;
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

bool InstallThreadHook(DWORD threadId)
{
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

// --------------------------------------------------
// Initialization
// --------------------------------------------------

BOOL Wh_ModInit()
{
    DWORD processId = GetCurrentProcessId();

    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPTHREAD,
        0
    );

    if (snapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);

    if (Thread32First(snapshot, &entry))
    {
        do
        {
            if (entry.th32OwnerProcessID == processId)
                InstallThreadHook(entry.th32ThreadID);

        } while (Thread32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);

    return !g_hooks.empty();
}

// --------------------------------------------------
// Uninitialization
// --------------------------------------------------

void Wh_ModUninit()
{
    for (const auto& record : g_hooks)
    {
        if (record.hook)
            UnhookWindowsHookEx(record.hook);
    }

    g_hooks.clear();
}

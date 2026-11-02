// ==WindhawkMod==
// @id              search-auto-input-disable
// @name            Search Auto Input Disable
// @description     Disables type-ahead in the File Explorer file view.
// @version         1.1
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
#include <windhawk_utils.h>
#include <cwchar>

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

    // Preserve keyboard shortcuts.
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
// ExplorerFrame.dll accelerator hook
// --------------------------------------------------

using MayTranslateAccelerator_t =
    HRESULT(WINAPI*)(void* pThis, MSG* msg, int flags);

MayTranslateAccelerator_t MayTranslateAccelerator_Original = nullptr;

HMODULE g_explorerFrame = nullptr;

HRESULT WINAPI MayTranslateAccelerator_Hook(
    void* pThis,
    MSG* msg,
    int flags)
{
    if (ShouldBlockMessage(msg))
    {
        // Consume the message.
        return S_OK;
    }

    return MayTranslateAccelerator_Original(
        pThis,
        msg,
        flags
    );
}

// --------------------------------------------------
// Initialization
// --------------------------------------------------

BOOL Wh_ModInit()
{
    g_explorerFrame = LoadLibraryExW(
        L"ExplorerFrame.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32
    );

    if (!g_explorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] =
    {
        {
            {
                L"private: long __cdecl CShellBrowser::_MayTranslateAccelerator(struct tagMSG *,int)"
            },
            &MayTranslateAccelerator_Original,
            MayTranslateAccelerator_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            g_explorerFrame,
            explorerFrameDllHooks,
            ARRAYSIZE(explorerFrameDllHooks)))
    {
        Wh_Log(L"Failed to hook CShellBrowser::_MayTranslateAccelerator");

        FreeLibrary(g_explorerFrame);
        g_explorerFrame = nullptr;

        return FALSE;
    }

    Wh_Log(L"Successfully hooked CShellBrowser::_MayTranslateAccelerator");

    return TRUE;
}

// --------------------------------------------------
// Uninitialization
// --------------------------------------------------

void Wh_ModUninit()
{
    if (g_explorerFrame)
    {
        FreeLibrary(g_explorerFrame);
        g_explorerFrame = nullptr;
    }

    MayTranslateAccelerator_Original = nullptr;
}

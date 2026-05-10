// ==WindhawkMod==
// @id              ctrl-backspace-fix-for-win32-text-boxes
// @name            Ctrl+Backspace Fix for Win32 Text Boxes
// @description     Makes Ctrl+Backspace delete the previous word in Win32 text boxes instead of inserting a control character
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ctrl+Backspace Fix for Win32 Text Boxes
Win32 text boxes often lack previous-word deletion functionality, resulting in
the `Ctrl+Backspace` hotkey inserting the `Delete` control character instead of
deleting the previous word.

This mod resolves this behavior by adding previous-word deletion functionality
to those `Edit` controls.

It also supports wrapped `Edit` controls used in .NET WinForms and Delphi VCL
applications.

| Before |
| :----- |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/ctrl-backspace-fix-for-win32-text-boxes_before.gif) |

| After |
| :---- |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/ctrl-backspace-fix-for-win32-text-boxes_after.gif) |

## Notes
* In hotkey text boxes, pressing `Ctrl+Backspace` deletes the text instead of
  assigning the hotkey.
* In masked text boxes, pressing `Ctrl+Backspace` deletes the placeholder
  characters, breaking the input mask and preventing further input into those
  missing slots.
* Custom-drawn text boxes, such as those in Qt applications, are unaffected
  because they do not utilize standard Win32 `Edit` controls. Most custom-drawn
  UI frameworks already handle their own previous-word deletion functionality.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <cwctype>
#include <string>

enum class CharClass {
    Whitespace,
    Punctuation,
    Alphanumeric
};

// Helper: Determine the character class to calculate word boundaries
CharClass GetCharClass(WCHAR wch)
{
    // Whitespace
    if (iswspace(wch)) return CharClass::Whitespace;

    // VS Code's default word separators
    constexpr const WCHAR* szWordSeparators =
        L"`~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?";
    if (wcschr(szWordSeparators, wch) != nullptr)
    {
        return CharClass::Punctuation;
    }

    // Everything else is grouped as a word
    return CharClass::Alphanumeric;
}

// Helper: Calculate the previous word boundary and delete the text block
void DeletePreviousWord(HWND hWnd)
{
    // Retrieve the starting and ending positions of the current text selection
    DWORD dwSelectionStart, dwSelectionEnd;
    SendMessageW(hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwSelectionStart),
        reinterpret_cast<LPARAM>(&dwSelectionEnd));

    // Replace any active text selection with an empty string
    if (dwSelectionStart != dwSelectionEnd)
    {
        SendMessageW(hWnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
        return;
    }

    // Allocate a string buffer and retrieve the text to calculate the boundary
    int cchTextLen = GetWindowTextLengthW(hWnd);
    if (cchTextLen == 0 || dwSelectionStart == 0) return;

    std::wstring windowText(cchTextLen, L'\0');
    GetWindowTextW(hWnd, windowText.data(), cchTextLen + 1);
    int iPos = dwSelectionStart - 1;

    // Step back through all whitespace characters
    while (iPos >= 0 && GetCharClass(windowText[iPos]) == CharClass::Whitespace)
    {
        iPos--;
    }

    if (iPos >= 0)
    {
        // Identify the character class to delete
        CharClass targetCharClass = GetCharClass(windowText[iPos]);

        // Step back through all characters of the same class
        while (iPos >= 0 && GetCharClass(windowText[iPos]) == targetCharClass)
        {
            iPos--;
        }
    }

    // Calculate the final text selection boundary
    DWORD dwNewSelStart = iPos + 1;

    // Pause re-drawing to hide the text selection flicker
    SendMessageW(hWnd, WM_SETREDRAW, FALSE, 0);

    // Select the calculated word bounds and replace with an empty string
    SendMessageW(hWnd, EM_SETSEL, dwNewSelStart, dwSelectionEnd);
    SendMessageW(hWnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));

    // Resume re-drawing and force a re-paint to display the updated text
    SendMessageW(hWnd, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hWnd, nullptr, TRUE);
}

// Helper: Identify the target Edit control
bool IsEditControl(HWND hWnd)
{
    WCHAR szClassName[64];
    if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)))
    {
        if (_wcsicmp(szClassName, L"Edit") == 0)
        {
            return true;
        }

        // Query the UI framework to reveal a wrapped Edit control
        // Common class names for such controls:
        // - WindowsForms10.EDIT.* | .NET WinForms
        // - TEdit, TMemo, T*Edit  | Delphi VCL
        LRESULT lResult = SendMessageW(hWnd, WM_GETDLGCODE, 0, 0);
        if (lResult & DLGC_HASSETSEL)
        {
            // Exclude RichEdit controls
            _wcsupr_s(szClassName, ARRAYSIZE(szClassName));
            if (wcsstr(szClassName, L"RICHEDIT") != nullptr)
            {
                return false;
            }
            return true;
        }

        // Fallback for .NET WinForms wrapped Edit controls
        // Required for those within PropertyGrid controls that swallow the
        // WM_GETDLGCODE query.
        if (_wcsnicmp(szClassName, L"WindowsForms10.EDIT.", 20) == 0)
        {
            return true;
        }
    }
    return false;
}

// Helper: Validate the Ctrl+Backspace hotkey on a target Edit control
bool IsCtrlBackspaceOnEditControl(HWND hWnd, UINT uMsg, WPARAM wParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_BACK &&
        (GetKeyState(VK_CONTROL) & 0x8000))
    {
        LONG_PTR lStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if ((lStyle & WS_CHILD) == 0)
        {
            return false;
        }

        if (IsEditControl(hWnd))
        {
            // Ignore read-only Edit controls
            if ((lStyle & ES_READONLY) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

// Hook for DispatchMessageW
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;
LRESULT WINAPI DispatchMessageW_Hook(const MSG *lpMsg)
{
    if (lpMsg)
    {
        if (IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
                lpMsg->wParam))
        {
            DeletePreviousWord(lpMsg->hwnd);
            return 0;
        }
    }
    return DispatchMessageW_Original(lpMsg);
}

// Hook for DispatchMessageA
using DispatchMessageA_t = decltype(&DispatchMessageA);
DispatchMessageA_t DispatchMessageA_Original;
LRESULT WINAPI DispatchMessageA_Hook(const MSG *lpMsg)
{
    if (lpMsg)
    {
        if (IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
                lpMsg->wParam))
        {
            DeletePreviousWord(lpMsg->hwnd);
            return 0;
        }
    }
    return DispatchMessageA_Original(lpMsg);
}

// Hook for IsDialogMessageW
using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t IsDialogMessageW_Original;
BOOL WINAPI IsDialogMessageW_Hook(HWND hDlg, LPMSG lpMsg)
{
    if (lpMsg)
    {
        if (IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
                lpMsg->wParam))
        {
            DeletePreviousWord(lpMsg->hwnd);
            return TRUE;
        }
    }
    return IsDialogMessageW_Original(hDlg, lpMsg);
}

// Hook for IsDialogMessageA
using IsDialogMessageA_t = decltype(&IsDialogMessageA);
IsDialogMessageA_t IsDialogMessageA_Original;
BOOL WINAPI IsDialogMessageA_Hook(HWND hDlg, LPMSG lpMsg)
{
    if (lpMsg)
    {
        if (IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
                lpMsg->wParam))
        {
            DeletePreviousWord(lpMsg->hwnd);
            return TRUE;
        }
    }
    return IsDialogMessageA_Original(hDlg, lpMsg);
}

// Hook for TranslateMessage
using TranslateMessage_t = decltype(&TranslateMessage);
TranslateMessage_t TranslateMessage_Original;
BOOL WINAPI TranslateMessage_Hook(const MSG *lpMsg)
{
    if (lpMsg)
    {
        if (IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
                lpMsg->wParam))
        {
            return TRUE;
        }
    }
    return TranslateMessage_Original(lpMsg);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    WindhawkUtils::SetFunctionHook(
        DispatchMessageW,
        DispatchMessageW_Hook,
        &DispatchMessageW_Original
    );

    WindhawkUtils::SetFunctionHook(
        DispatchMessageA,
        DispatchMessageA_Hook,
        &DispatchMessageA_Original
    );

    WindhawkUtils::SetFunctionHook(
        IsDialogMessageW,
        IsDialogMessageW_Hook,
        &IsDialogMessageW_Original
    );

    WindhawkUtils::SetFunctionHook(
        IsDialogMessageA,
        IsDialogMessageA_Hook,
        &IsDialogMessageA_Original
    );

    WindhawkUtils::SetFunctionHook(
        TranslateMessage,
        TranslateMessage_Hook,
        &TranslateMessage_Original
    );

    return TRUE;
}

// ==WindhawkMod==
// @id              ctrl-backspace-fix-for-win32-text-boxes
// @name            Ctrl+Backspace Fix for Win32 Text Boxes
// @description     Makes Ctrl+Backspace delete the previous word in Win32 text boxes instead of inserting a control character
// @version         1.1.0
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

| Before |
| :----- |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/ctrl-backspace-fix-for-win32-text-boxes_before.gif) |

| After |
| :---- |
| ![](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/ctrl-backspace-fix-for-win32-text-boxes_after.gif) |

## Features & behavior
* Supports wrapped `Edit` controls used in .NET WinForms and Delphi VCL apps.
* Handles Unicode whitespace, punctuation, and symbols for word-wise deletion.
* Groups consecutive Japanese kana (Hiragana & Katakana) by script for word-wise
  deletion.
* Groups consecutive Korean Hangul (syllables & jamo) as words for word-wise
  deletion.
* Steps back one character at a time through Chinese, Japanese, and Korean
  ideographs.
* Deletes masked text before the caret as a single block in password fields to
  avoid leaking word boundaries.

## Notes & limitations
* In hotkey text boxes, pressing `Ctrl+Backspace` deletes the text instead of
  assigning the hotkey.
* In masked text boxes, pressing `Ctrl+Backspace` deletes the placeholder
  characters, breaking the input mask and preventing further input into those
  missing slots.
* In legacy ANSI apps on CJK system locales, pressing `Ctrl+Backspace` on
  multi-byte text deletes the wrong range because this mod calculates word
  boundaries using Unicode offsets.
* Custom-drawn text boxes, such as those in Qt apps, are unaffected because they
  do not utilize standard Win32 `Edit` controls. Most custom-drawn UI frameworks
  already handle their own previous-word deletion functionality.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <cwchar>
#include <string>
#include <string_view>

enum class CharClass
{
    Whitespace,
    Punctuation,
    Ideograph,
    Hiragana,
    Katakana,
    Alphanumeric
};

// Helper: Identify prolonged sound marks shared across Japanese kana
bool IsProlongedSoundMark(WCHAR wch)
{
    return wch == L'\u30FC' || wch == L'\uFF70';
}

// Helper: Determine the character class to calculate word boundaries
CharClass GetCharClass(WCHAR wch)
{
    // Treat underscores as part of a word
    if (wch == L'_')
    {
        return CharClass::Alphanumeric;
    }

    // Treat Hangul syllables and jamo as part of a word
    if ((wch >= L'\uAC00' && wch <= L'\uD7A3') ||
        (wch >= L'\u1100' && wch <= L'\u11FF') ||
        (wch >= L'\u3130' && wch <= L'\u318F'))
    {
        return CharClass::Alphanumeric;
    }

    // Treat prolonged and voiced/semi-voiced sound marks as katakana
    if (IsProlongedSoundMark(wch) || wch == L'\uFF9E' || wch == L'\uFF9F')
    {
        return CharClass::Katakana;
    }

    // Unicode whitespace and punctuation
    WORD wCharType1 = 0;
    if (GetStringTypeW(CT_CTYPE1, &wch, 1, &wCharType1))
    {
        if (wCharType1 & C1_SPACE)
        {
            return CharClass::Whitespace;
        }
        if (wCharType1 & C1_PUNCT)
        {
            return CharClass::Punctuation;
        }
    }

    // Unicode symbols, CJK ideographs, and Japanese kana
    WORD wCharType3 = 0;
    if (GetStringTypeW(CT_CTYPE3, &wch, 1, &wCharType3))
    {
        if (wCharType3 & C3_SYMBOL)
        {
            return CharClass::Punctuation;
        }
        if (wCharType3 & C3_IDEOGRAPH)
        {
            return CharClass::Ideograph;
        }
        if (wCharType3 & C3_HIRAGANA)
        {
            return CharClass::Hiragana;
        }
        if (wCharType3 & C3_KATAKANA)
        {
            return CharClass::Katakana;
        }
    }

    // Treat everything else as part of a word
    return CharClass::Alphanumeric;
}

// Helper: Resolve a prolonged sound mark to the preceding Japanese kana
CharClass GetProlongedSoundMarkClass(std::wstring_view text, int iPos)
{
    while (iPos > 0 && (IsProlongedSoundMark(text[iPos - 1]) ||
        text[iPos - 1] == L'\u3099' || text[iPos - 1] == L'\u309A'))
    {
        iPos--;
    }
    if (iPos > 0 && GetCharClass(text[iPos - 1]) == CharClass::Hiragana)
    {
        return CharClass::Hiragana;
    }
    return CharClass::Katakana;
}

// Helper: Identify selectors and joiners that attach to a base character
bool IsSelectorOrJoiner(char32_t codePoint)
{
    return (codePoint >= 0xFE00 && codePoint <= 0xFE0F) ||
        codePoint == 0x200D || codePoint == 0x20E3 ||
        codePoint == 0x3099 || codePoint == 0x309A ||
        (codePoint >= 0xE0020 && codePoint <= 0xE007F) ||
        (codePoint >= 0xE0100 && codePoint <= 0xE01EF);
}

// Helper: Determine the character class at a UTF-16 index
CharClass GetCharClassAt(std::wstring_view text, int iPos, int& cchCharLen)
{
    cchCharLen = 0;
    while (iPos >= 0)
    {
        int cchCodePointLen = 1;
        char32_t codePoint = text[iPos];
        if (iPos > 0 && IS_SURROGATE_PAIR(text[iPos - 1], text[iPos]))
        {
            cchCodePointLen = 2;
            codePoint = 0x10000 +
                ((static_cast<char32_t>(text[iPos - 1]) - 0xD800) << 10) +
                (static_cast<char32_t>(text[iPos]) - 0xDC00);
        }

        // Keep selectors and joiners attached to the preceding base character
        cchCharLen += cchCodePointLen;
        if (IsSelectorOrJoiner(codePoint) && iPos >= cchCodePointLen)
        {
            iPos -= cchCodePointLen;
            continue;
        }

        // Classify CJK ideographs and emoji outside the BMP
        if (cchCodePointLen == 2)
        {
            if (codePoint >= 0x20000 && codePoint <= 0x3FFFF)
            {
                return CharClass::Ideograph;
            }
            if (codePoint >= 0x1F000 && codePoint <= 0x1FAFF)
            {
                return CharClass::Punctuation;
            }
            return CharClass::Alphanumeric;
        }

        if (IsProlongedSoundMark(text[iPos]))
        {
            return GetProlongedSoundMarkClass(text, iPos);
        }

        return GetCharClass(text[iPos]);
    }

    return CharClass::Alphanumeric;
}

// Helper: Calculate the starting index of the previous word
DWORD FindPreviousWordStart(std::wstring_view text, DWORD dwSelStart)
{
    auto dwClampedSelStart =
        std::min<DWORD>(dwSelStart, static_cast<DWORD>(text.size()));
    auto iPos = static_cast<int>(dwClampedSelStart) - 1;

    // Step back through all whitespace characters
    while (iPos >= 0 && GetCharClass(text[iPos]) == CharClass::Whitespace)
    {
        iPos--;
    }

    if (iPos < 0)
    {
        return 0;
    }

    // Identify the character class to delete
    int cchCharLen = 1;
    CharClass targetCharClass = GetCharClassAt(text, iPos, cchCharLen);
    iPos -= cchCharLen;

    if (targetCharClass == CharClass::Ideograph)
    {
        return static_cast<DWORD>(iPos + 1);
    }

    // Step back through all characters of the same class
    while (iPos >= 0)
    {
        CharClass prevCharClass = GetCharClassAt(text, iPos, cchCharLen);
        if (prevCharClass != targetCharClass)
        {
            break;
        }
        iPos -= cchCharLen;
    }

    return static_cast<DWORD>(iPos + 1);
}

// Helper: Calculate the previous word boundary and delete the text block
void DeletePreviousWord(HWND hWnd)
{
    // Retrieve the starting and ending positions of the current text selection
    DWORD dwSelStart = 0;
    DWORD dwSelEnd = 0;
    SendMessageW(hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&dwSelStart),
        reinterpret_cast<LPARAM>(&dwSelEnd));

    // Replace any active text selection with an empty string
    if (dwSelStart != dwSelEnd)
    {
        SendMessageW(hWnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
        return;
    }

    if (dwSelStart == 0)
    {
        return;
    }

    DWORD dwNewSelStart = 0;

    // Treat masked password text as a single block to avoid leaking word
    // boundaries
    if (!SendMessageW(hWnd, EM_GETPASSWORDCHAR, 0, 0))
    {
        // Allocate a string buffer and retrieve the text to calculate the
        // boundary
        int cchTextLen = GetWindowTextLengthW(hWnd);
        if (cchTextLen <= 0)
        {
            return;
        }

        std::wstring windowText(cchTextLen, L'\0');
        int cchTextCopied = GetWindowTextW(hWnd, windowText.data(),
            cchTextLen + 1);
        if (cchTextCopied <= 0)
        {
            return;
        }

        // Calculate the final text selection boundary
        dwNewSelStart = FindPreviousWordStart(std::wstring_view(
            windowText.data(), cchTextCopied), dwSelStart);
    }

    // Pause re-drawing to hide the text selection flicker
    SendMessageW(hWnd, WM_SETREDRAW, FALSE, 0);

    // Select the calculated word bounds and replace with an empty string
    SendMessageW(hWnd, EM_SETSEL, dwNewSelStart, dwSelEnd);
    SendMessageW(hWnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));

    // Resume re-drawing and force a re-paint to display the updated text
    SendMessageW(hWnd, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hWnd, nullptr, TRUE);
}

// Helper: Identify the target Edit control
bool IsEditControl(HWND hWnd)
{
    WCHAR szClassName[64];
    if (!GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)))
    {
        return false;
    }

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
        // Exclude specific text controls from the Ctrl+Backspace fix
        _wcsupr_s(szClassName, ARRAYSIZE(szClassName));
        return (wcsstr(szClassName, L"RICHEDIT") == nullptr &&
            wcsstr(szClassName, L"SCINTILLA") == nullptr);
    }

    // Fallback for .NET WinForms wrapped Edit controls
    // Required for those within PropertyGrid controls that swallow the
    // WM_GETDLGCODE query.
    return _wcsnicmp(szClassName, L"WindowsForms10.EDIT.", 20) == 0;
}

// Helper: Validate the Ctrl+Backspace hotkey on a target Edit control
bool IsCtrlBackspaceOnEditControl(HWND hWnd, UINT uMsg, WPARAM wParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_BACK &&
        (GetKeyState(VK_CONTROL) & 0x8000))
    {
        LONG_PTR lStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);

        // Ensure it is a valid, writable Edit control
        return ((lStyle & WS_CHILD) &&
            !(lStyle & ES_READONLY) &&
            IsEditControl(hWnd));
    }
    return false;
}

// Hook for DispatchMessageW
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;
LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg)
{
    if (lpMsg && IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
            lpMsg->wParam))
    {
        DeletePreviousWord(lpMsg->hwnd);
        return 0;
    }
    return DispatchMessageW_Original(lpMsg);
}

// Hook for DispatchMessageA
using DispatchMessageA_t = decltype(&DispatchMessageA);
DispatchMessageA_t DispatchMessageA_Original;
LRESULT WINAPI DispatchMessageA_Hook(const MSG* lpMsg)
{
    if (lpMsg && IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
            lpMsg->wParam))
    {
        DeletePreviousWord(lpMsg->hwnd);
        return 0;
    }
    return DispatchMessageA_Original(lpMsg);
}

// Hook for IsDialogMessageW
using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t IsDialogMessageW_Original;
BOOL WINAPI IsDialogMessageW_Hook(HWND hDlg, LPMSG lpMsg)
{
    if (lpMsg && IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
            lpMsg->wParam))
    {
        DeletePreviousWord(lpMsg->hwnd);
        return TRUE;
    }
    return IsDialogMessageW_Original(hDlg, lpMsg);
}

// Hook for IsDialogMessageA
using IsDialogMessageA_t = decltype(&IsDialogMessageA);
IsDialogMessageA_t IsDialogMessageA_Original;
BOOL WINAPI IsDialogMessageA_Hook(HWND hDlg, LPMSG lpMsg)
{
    if (lpMsg && IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
            lpMsg->wParam))
    {
        DeletePreviousWord(lpMsg->hwnd);
        return TRUE;
    }
    return IsDialogMessageA_Original(hDlg, lpMsg);
}

// Hook for TranslateMessage
using TranslateMessage_t = decltype(&TranslateMessage);
TranslateMessage_t TranslateMessage_Original;
BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg)
{
    if (lpMsg && IsCtrlBackspaceOnEditControl(lpMsg->hwnd, lpMsg->message,
            lpMsg->wParam))
    {
        return TRUE;
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

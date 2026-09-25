// ==WindhawkMod==
// @id shorten-startallback-terminal-admin
// @name Shorten StartAllBack Terminal Admin
// @description Replaces the Russian "Терминал Windows (Администратор)" label with a shorter customizable label in StartAllBack.
// @version 1.0
// @author Murtuzoff
// @github https://github.com/Murtuzoff
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -luser32 -lgdi32
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- newText: Терминал Windows (Админ)
  $name: Replacement text
  $description: Text to display instead of "Терминал Windows (Администратор)".
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>

#include <mutex>
#include <string>
#include <string_view>
#include <utility>


// ============================================================
// Global data
// ============================================================

static std::wstring g_newText =
    L"Терминал Windows (Админ)";

static std::mutex g_settingsMutex;

static constexpr wchar_t kTarget1[] =
    L"Терминал Windows (Администратор)";


// ============================================================
// Settings
// ============================================================

static void LoadSettings()
{
    PCWSTR text = Wh_GetStringSetting(L"newText");

    std::wstring newText =
        (text && *text)
            ? text
            : L"Терминал Windows (Админ)";

    if (text)
    {
        Wh_FreeStringSetting(text);
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_newText = std::move(newText);
    }
}


static std::wstring GetNewText()
{
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_newText;
}


// ============================================================
// Text detection
// ============================================================

static bool IsTargetText(
    LPCWSTR text,
    int length)
{
    if (!text)
        return false;

    std::wstring_view value;

    if (length < 0)
    {
        value = std::wstring_view(text);
    }
    else
    {
        value = std::wstring_view(
            text,
            static_cast<size_t>(length)
        );
    }

    // Exact known Russian label.
    if (value.find(kTarget1) != std::wstring_view::npos)
    {
        return true;
    }

    // More flexible detection in case an accelerator "&"
    // or a minor formatting difference is present.
    if (value.find(L"Терминал Windows") !=
            std::wstring_view::npos &&
        value.find(L"Администратор") !=
            std::wstring_view::npos)
    {
        return true;
    }

    return false;
}


// ============================================================
// DrawTextW
// ============================================================

using DrawTextW_t = decltype(&DrawTextW);
static DrawTextW_t DrawTextW_Original = nullptr;


static int WINAPI DrawTextW_Hook(
    HDC hdc,
    LPCWSTR text,
    int count,
    LPRECT rect,
    UINT format)
{
    if (IsTargetText(text, count))
    {
        Wh_Log(
            L"DrawTextW: replacing Terminal Admin text"
        );

        std::wstring replacement = GetNewText();

        int replacementLength =
            static_cast<int>(replacement.length());

        // DrawTextW may modify the supplied string when
        // DT_MODIFYSTRING is specified. According to the Win32
        // contract, the buffer must have room for four additional
        // characters.
        if (format & DT_MODIFYSTRING)
        {
            replacement.resize(replacementLength + 4);
        }

        return DrawTextW_Original(
            hdc,
            replacement.data(),
            replacementLength,
            rect,
            format
        );
    }

    return DrawTextW_Original(
        hdc,
        text,
        count,
        rect,
        format
    );
}


// ============================================================
// DrawTextExW
// ============================================================

using DrawTextExW_t = decltype(&DrawTextExW);
static DrawTextExW_t DrawTextExW_Original = nullptr;


static int WINAPI DrawTextExW_Hook(
    HDC hdc,
    LPWSTR text,
    int count,
    LPRECT rect,
    UINT format,
    LPDRAWTEXTPARAMS params)
{
    if (IsTargetText(text, count))
    {
        Wh_Log(
            L"DrawTextExW: replacing Terminal Admin text"
        );

        std::wstring replacement = GetNewText();

        int replacementLength =
            static_cast<int>(replacement.length());

        // DrawTextExW may modify the supplied string when
        // DT_MODIFYSTRING is specified.
        if (format & DT_MODIFYSTRING)
        {
            replacement.resize(replacementLength + 4);
        }

        return DrawTextExW_Original(
            hdc,
            replacement.data(),
            replacementLength,
            rect,
            format,
            params
        );
    }

    return DrawTextExW_Original(
        hdc,
        text,
        count,
        rect,
        format,
        params
    );
}


// ============================================================
// TextOutW
// ============================================================

using TextOutW_t = decltype(&TextOutW);
static TextOutW_t TextOutW_Original = nullptr;


static BOOL WINAPI TextOutW_Hook(
    HDC hdc,
    int x,
    int y,
    LPCWSTR text,
    int count)
{
    if (IsTargetText(text, count))
    {
        Wh_Log(
            L"TextOutW: replacing Terminal Admin text"
        );

        std::wstring replacement = GetNewText();

        return TextOutW_Original(
            hdc,
            x,
            y,
            replacement.c_str(),
            static_cast<int>(replacement.length())
        );
    }

    return TextOutW_Original(
        hdc,
        x,
        y,
        text,
        count
    );
}


// ============================================================
// ExtTextOutW
// ============================================================

using ExtTextOutW_t = decltype(&ExtTextOutW);
static ExtTextOutW_t ExtTextOutW_Original = nullptr;


static BOOL WINAPI ExtTextOutW_Hook(
    HDC hdc,
    int x,
    int y,
    UINT options,
    const RECT* rect,
    LPCWSTR text,
    UINT count,
    const INT* dx)
{
    if (IsTargetText(
        text,
        static_cast<int>(count)))
    {
        Wh_Log(
            L"ExtTextOutW: replacing Terminal Admin text"
        );

        std::wstring replacement = GetNewText();

        // dx belongs to the original string. Since the replacement
        // has a different length, the old character-spacing array
        // must not be reused.
        return ExtTextOutW_Original(
            hdc,
            x,
            y,
            options,
            rect,
            replacement.c_str(),
            static_cast<UINT>(replacement.length()),
            nullptr
        );
    }

    return ExtTextOutW_Original(
        hdc,
        x,
        y,
        options,
        rect,
        text,
        count,
        dx
    );
}


// ============================================================
// GetTextExtentPoint32W
//
// This hook makes StartAllBack measure the shortened string,
// allowing the menu layout to use the shorter width.
// ============================================================

using GetTextExtentPoint32W_t =
    decltype(&GetTextExtentPoint32W);

static GetTextExtentPoint32W_t
    GetTextExtentPoint32W_Original = nullptr;


static BOOL WINAPI GetTextExtentPoint32W_Hook(
    HDC hdc,
    LPCWSTR text,
    int count,
    LPSIZE size)
{
    if (IsTargetText(text, count))
    {
        Wh_Log(
            L"GetTextExtentPoint32W: measuring shortened text"
        );

        std::wstring replacement = GetNewText();

        return GetTextExtentPoint32W_Original(
            hdc,
            replacement.c_str(),
            static_cast<int>(replacement.length()),
            size
        );
    }

    return GetTextExtentPoint32W_Original(
        hdc,
        text,
        count,
        size
    );
}


// ============================================================
// Windhawk initialization
// ============================================================

BOOL Wh_ModInit()
{
    LoadSettings();

    Wh_Log(
        L"Initializing StartAllBack Terminal Admin text mod"
    );

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(DrawTextW),
        reinterpret_cast<void*>(DrawTextW_Hook),
        reinterpret_cast<void**>(&DrawTextW_Original)))
    {
        Wh_Log(L"Failed to hook DrawTextW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(DrawTextExW),
        reinterpret_cast<void*>(DrawTextExW_Hook),
        reinterpret_cast<void**>(&DrawTextExW_Original)))
    {
        Wh_Log(L"Failed to hook DrawTextExW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(TextOutW),
        reinterpret_cast<void*>(TextOutW_Hook),
        reinterpret_cast<void**>(&TextOutW_Original)))
    {
        Wh_Log(L"Failed to hook TextOutW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(ExtTextOutW),
        reinterpret_cast<void*>(ExtTextOutW_Hook),
        reinterpret_cast<void**>(&ExtTextOutW_Original)))
    {
        Wh_Log(L"Failed to hook ExtTextOutW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        reinterpret_cast<void*>(GetTextExtentPoint32W),
        reinterpret_cast<void*>(GetTextExtentPoint32W_Hook),
        reinterpret_cast<void**>(
            &GetTextExtentPoint32W_Original)))
    {
        Wh_Log(
            L"Failed to hook GetTextExtentPoint32W"
        );
        return FALSE;
    }

    return TRUE;
}


// ============================================================
// Settings changed
// ============================================================

void Wh_ModSettingsChanged()
{
    LoadSettings();
}


// ============================================================
// Uninitialization
// ============================================================

void Wh_ModUninit()
{
    Wh_Log(
        L"Unloading StartAllBack Terminal Admin text mod"
    );
}

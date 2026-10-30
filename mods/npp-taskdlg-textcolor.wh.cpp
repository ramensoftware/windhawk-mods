// ==WindhawkMod==
// @id              npp-taskdlg-textcolor
// @name            Notepad++ Dark Dialog Fix
// @description     Readable text in Notepad++ Save and confirm dialogs when dark mode is on
// @version         1.0.1
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @license         MIT
// @include         notepad++.exe
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notepad++ Dark Dialog Fix

In dark mode, Notepad++ paints its Save and confirm dialogs dark but leaves the
text black, so you can barely read it. This mod makes that text light again.

![Notepad++ Dark Dialog Fix preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/npp-taskdlg-textcolor.png)

## Features

- **Readable dialog text** in Save, Save changes and other confirm dialogs.
- **Fixes labels and buttons** drawn with the theme's text color.
- **Only touches Notepad++.** No other app is affected, and nothing is changed
  on disk.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

using GetThemeColor_t = HRESULT(WINAPI*)(HTHEME, int, int, int, COLORREF*);
static GetThemeColor_t GetThemeColor_orig;

using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);
static GetThemeClass_t GetThemeClass_fn;

static bool IsDarkColor(COLORREF c) {
    int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    return (r * 299 + g * 587 + b * 114) / 1000 < 140;
}

static bool ShouldFixClass(HTHEME hTheme) {
    if (!GetThemeClass_fn) {
        HMODULE hUx = GetModuleHandleW(L"uxtheme.dll");
        if (!hUx) return false;
        GetThemeClass_fn =
            (GetThemeClass_t)GetProcAddress(hUx, "GetThemeClass");
        if (!GetThemeClass_fn)
            GetThemeClass_fn =
                (GetThemeClass_t)GetProcAddress(hUx, MAKEINTRESOURCEA(74));
        if (!GetThemeClass_fn) return false;
    }
    WCHAR buf[128] = {0};
    if (FAILED(GetThemeClass_fn(hTheme, buf, 127))) return false;
    buf[127] = 0;
    return wcsstr(buf, L"TaskDialog") != nullptr ||
           _wcsicmp(buf, L"Static") == 0 ||
           _wcsicmp(buf, L"Button") == 0;
}

HRESULT WINAPI GetThemeColor_hook(HTHEME hTheme, int iPartId, int iStateId,
                                  int iPropId, COLORREF* pColor) {
    HRESULT hr =
        GetThemeColor_orig(hTheme, iPartId, iStateId, iPropId, pColor);
    if (SUCCEEDED(hr) && pColor && iPropId == TMT_TEXTCOLOR &&
        IsDarkColor(*pColor) && ShouldFixClass(hTheme)) {
        *pColor = RGB(242, 242, 242);
    }
    return hr;
}

BOOL Wh_ModInit(void) {
    if (!WindhawkUtils::SetFunctionHook(GetThemeColor, GetThemeColor_hook,
                                        &GetThemeColor_orig)) {
        Wh_Log(L"GetThemeColor hook failed");
        return FALSE;
    }
    return TRUE;
}

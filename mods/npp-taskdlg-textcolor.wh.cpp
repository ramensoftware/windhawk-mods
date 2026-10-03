// ==WindhawkMod==
// @id              npp-taskdlg-textcolor
// @name            Notepad++ Save Dialog Text Color
// @description     Fixes dark-on-dark text in Notepad++ Save/confirm dialogs
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
# Notepad++ Save Dialog Text Color

Notepad++ dark mode paints Save/confirm dialogs dark while theme text stays
black. Forces readable light text for TaskDialog/Static/Button theme text
colors inside notepad++.exe only.
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

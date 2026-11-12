// ==WindhawkMod==
// @id              npp-taskdlg-textcolor
// @name            Notepad++ Dark Dialog Fix
// @description     Readable text in Notepad++ Save and confirm dialogs when dark mode is on
// @version         1.0.3
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @license         MIT
// @include         notepad++.exe
// @compilerOptions -luxtheme
// @donateUrl       https://ko-fi.com/DavidHiFi
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
- **Only in dark mode.** The mod asks Notepad++ whether dark mode is on, so
  light mode dialogs are left alone. Needs Notepad++ 8.4.1 or newer.
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

// Notepad++ plugin API, Notepad_plus_msgs.h (Notepad++ 8.4.1 and newer).
constexpr UINT NPPM_ISDARKMODEENABLED = WM_USER + 1000 + 107;

static bool IsDarkColor(COLORREF c) {
    int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    return (r * 299 + g * 587 + b * 114) / 1000 < 140;
}

// uxtheme exports GetThemeClass by ordinal only.
static GetThemeClass_t GetThemeClassFn() {
    static const GetThemeClass_t fn = []() -> GetThemeClass_t {
        HMODULE hUx = GetModuleHandleW(L"uxtheme.dll");
        if (!hUx) {
            return nullptr;
        }
        return (GetThemeClass_t)GetProcAddress(hUx, MAKEINTRESOURCEA(74));
    }();
    return fn;
}

static bool ShouldFixClass(HTHEME hTheme) {
    GetThemeClass_t getThemeClass = GetThemeClassFn();
    if (!getThemeClass) {
        return false;
    }
    WCHAR buf[128] = {0};
    if (FAILED(getThemeClass(hTheme, buf, ARRAYSIZE(buf)))) {
        return false;
    }
    buf[ARRAYSIZE(buf) - 1] = 0;
    return wcsstr(buf, L"TaskDialog") != nullptr ||
           _wcsicmp(buf, L"Static") == 0 ||
           _wcsicmp(buf, L"Button") == 0;
}

// The dialogs are modal on the Notepad++ main thread, so only that thread is
// asked. The state is read each time because it can change while running.
static bool IsNppDarkModeOnThisThread() {
    DWORD threadId = GetCurrentThreadId();
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(nullptr, hwnd, L"Notepad++", nullptr))) {
        // The thread check also skips other Notepad++ instances.
        if (GetWindowThreadProcessId(hwnd, nullptr) == threadId) {
            return SendMessageW(hwnd, NPPM_ISDARKMODEENABLED, 0, 0) != 0;
        }
    }
    return false;
}

HRESULT WINAPI GetThemeColor_hook(HTHEME hTheme, int iPartId, int iStateId,
                                  int iPropId, COLORREF* pColor) {
    HRESULT hr =
        GetThemeColor_orig(hTheme, iPartId, iStateId, iPropId, pColor);
    if (SUCCEEDED(hr) && pColor && iPropId == TMT_TEXTCOLOR &&
        IsDarkColor(*pColor) && ShouldFixClass(hTheme) &&
        IsNppDarkModeOnThisThread()) {
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

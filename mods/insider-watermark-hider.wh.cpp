// ==WindhawkMod==
// @id              insider-watermark-hider
// @name            Insider Watermark Hider
// @description     Hides the "Evaluation copy / Insider Preview" build watermark on the desktop
// @version         1.0
// @author          1uk34dd0
// @github          https://github.com/1uk34dd0
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -luxtheme -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Insider Watermark Hider

Hides the "Windows 11 Pro Insider Preview. Evaluation copy. Build ..." watermark
that Windows Insider builds draw in the bottom-right corner of the desktop.

## How it works

The mod runs inside `explorer.exe` and intercepts the text-drawing calls
(`DrawTextW`, `DrawTextExW`, `ExtTextOutW`, `TextOutW` and `DrawThemeTextEx`).
Any call whose text contains "Evaluation copy" or "Insider Preview" is skipped,
so the watermark is never painted. No system files or registry settings are
modified, and disabling the mod brings the watermark back.

## Notes

- If the watermark is still visible after enabling the mod, restart Windows
  Explorer (Task Manager -> Windows Explorer -> Restart) so the desktop repaints.
- The text matching is deliberately narrow, so normal Explorer text such as
  file names is not affected.
- Microsoft may change how the watermark is drawn in a future Insider build,
  which could stop this mod from working.
- Tested on: Windows 11 Insider build 29648.1000
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>
#include <shlwapi.h>

// Set to 1 to also log anything containing "Build" or "Windows" (noisy, for diagnosis only)
#define VERBOSE 0

static bool Inspect(const wchar_t* api, LPCWSTR text, int count) {
    if (!text || count == 0) return false;

    wchar_t buf[512];
    size_t n = 0;
    if (count < 0) { while (n < 511 && text[n]) n++; }
    else n = count < 511 ? (size_t)count : 511;
    memcpy(buf, text, n * sizeof(wchar_t));
    buf[n] = 0;

    if (StrStrIW(buf, L"Evaluation copy") || StrStrIW(buf, L"Insider Preview")) {
        Wh_Log(L"HIDE %ls: %ls", api, buf);
        return true;
    }
#if VERBOSE
    if (StrStrIW(buf, L"Build") || StrStrIW(buf, L"Windows")) {
        Wh_Log(L"seen %ls: %ls", api, buf);
    }
#endif
    return false;
}

// DrawTextW
typedef int (WINAPI *DrawTextW_t)(HDC, LPCWSTR, int, LPRECT, UINT);
DrawTextW_t DrawTextW_Original;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR text, int cch, LPRECT rc, UINT fmt) {
    if (Inspect(L"DrawTextW", text, cch)) return 0;
    return DrawTextW_Original(hdc, text, cch, rc, fmt);
}

// DrawTextExW
typedef int (WINAPI *DrawTextExW_t)(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
DrawTextExW_t DrawTextExW_Original;
int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR text, int cch, LPRECT rc, UINT fmt, LPDRAWTEXTPARAMS p) {
    if (Inspect(L"DrawTextExW", text, cch)) return 0;
    return DrawTextExW_Original(hdc, text, cch, rc, fmt, p);
}

// ExtTextOutW
typedef BOOL (WINAPI *ExtTextOutW_t)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);
ExtTextOutW_t ExtTextOutW_Original;
BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT opt, const RECT* rc, LPCWSTR text, UINT cch, const INT* dx) {
    if (!(opt & ETO_GLYPH_INDEX) && Inspect(L"ExtTextOutW", text, (int)cch)) return TRUE;
    return ExtTextOutW_Original(hdc, x, y, opt, rc, text, cch, dx);
}

// TextOutW
typedef BOOL (WINAPI *TextOutW_t)(HDC, int, int, LPCWSTR, int);
TextOutW_t TextOutW_Original;
BOOL WINAPI TextOutW_Hook(HDC hdc, int x, int y, LPCWSTR text, int cch) {
    if (Inspect(L"TextOutW", text, cch)) return TRUE;
    return TextOutW_Original(hdc, x, y, text, cch);
}

// DrawThemeTextEx
typedef HRESULT (WINAPI *DrawThemeTextEx_t)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const DTTOPTS*);
DrawThemeTextEx_t DrawThemeTextEx_Original;
HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME th, HDC hdc, int part, int state, LPCWSTR text, int cch, DWORD flags, LPRECT rc, const DTTOPTS* opts) {
    if (Inspect(L"DrawThemeTextEx", text, cch)) return S_OK;
    return DrawThemeTextEx_Original(th, hdc, part, state, text, cch, flags, rc, opts);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)DrawTextW,        (void*)DrawTextW_Hook,        (void**)&DrawTextW_Original);
    Wh_SetFunctionHook((void*)DrawTextExW,      (void*)DrawTextExW_Hook,      (void**)&DrawTextExW_Original);
    Wh_SetFunctionHook((void*)ExtTextOutW,      (void*)ExtTextOutW_Hook,      (void**)&ExtTextOutW_Original);
    Wh_SetFunctionHook((void*)TextOutW,         (void*)TextOutW_Hook,         (void**)&TextOutW_Original);
    Wh_SetFunctionHook((void*)DrawThemeTextEx,  (void*)DrawThemeTextEx_Hook,  (void**)&DrawThemeTextEx_Original);
    return TRUE;
}

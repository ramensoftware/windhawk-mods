// ==WindhawkMod==
// @id              fix-legacy-taskbar-tray-input-indicator
// @name            Fix language indicator in Win10 taskbar under Win11 24H2+
// @description     Fixes text orientation in the keyboard layout indicator in Win10 taskbar running under Win11 24H2+
// @version         1.2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix language indicator in Win10 taskbar under Win11 24H2 or later

Before:

![Before](https://i.imgur.com/k8e2zsh.png)

After:

![After](https://i.imgur.com/bsfyiYr.png)

This mod fixes the keyboard layout indicator text orientation 
when using the classic (non-modern) Windows taskbar.

**Note:** This mod only works with the classic taskbar, not the modern 
Windows 11 taskbar.
*/
// ==/WindhawkModReadme==

#include <windows.h>

static wchar_t g_text[8] = {};
static LOGFONTW g_font = {};
static int g_srcW = 0;
static int g_srcH = 0;
static bool g_pending = false;
static DWORD g_threadId = 0;
static COLORREF g_lastGoodBg = CLR_INVALID;

static bool LooksLikeLayoutText(LPCWSTR s, int len) {
    if (!s || len < 2 || len > 4) return false;
    for (int i = 0; i < len; i++) {
        if (!IsCharAlphaW(s[i])) return false;
    }
    return true;
}

static bool IsRotatedFont(HDC hdc, LOGFONTW* pLF) {
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    if (!hFont) return false;
    if (GetObjectW(hFont, sizeof(*pLF), pLF) == 0) return false;
    return (pLF->lfEscapement != 0 || pLF->lfOrientation != 0);
}

using ExtTextOutW_t = BOOL(WINAPI*)(HDC, int, int, UINT, const RECT*,
                                     LPCWSTR, UINT, const INT*);
static ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options,
                              const RECT* lprect, LPCWSTR lpString,
                              UINT c, const INT* lpDx) {
    int len = (int)c;
    LOGFONTW lf = {};

    if (LooksLikeLayoutText(lpString, len) && IsRotatedFont(hdc, &lf)) {
        HBITMAP hbm = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        BITMAP bm = {};
        if (hbm && GetObject(hbm, sizeof(bm), &bm)) {
            
            if (!g_pending) {
                wcsncpy_s(g_text, lpString, len);
                g_text[len] = 0;
                g_font = lf;
                g_srcW = bm.bmWidth;
                g_srcH = bm.bmHeight;
                g_pending = true;
                g_threadId = GetCurrentThreadId();
            }

            COLORREF fillColor = (g_lastGoodBg != CLR_INVALID) ? g_lastGoodBg : GetSysColor(COLOR_BTNFACE);
            RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
            HBRUSH hBr = CreateSolidBrush(fillColor);
            FillRect(hdc, &rc, hBr);
            DeleteObject(hBr);

            return TRUE;
        }
    }

    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_Original = nullptr;

BOOL WINAPI BitBlt_Hook(HDC hdcDest, int xDest, int yDest, int w, int h,
                         HDC hdcSrc, int xSrc, int ySrc, DWORD rop) {

    BOOL result = BitBlt_Original(hdcDest, xDest, yDest, w, h, hdcSrc, xSrc, ySrc, rop);

    if (!g_pending || g_srcW <= 0 || g_srcH <= 0) {
        return result;
    }
    
    if (GetCurrentThreadId() != g_threadId) {
        return result;
    }

    if (xDest != 0 || yDest != 0) {
        return result;
    }

    bool isSecondBlit = (w > h && w >= g_srcH && h >= g_srcW);
    
    if (!isSecondBlit) {
        return result;
    }

    COLORREF bgColor = GetPixel(hdcDest, w - 1, h - 1);
    
    if (bgColor != CLR_INVALID && bgColor != 0x000000) {
        g_lastGoodBg = bgColor;
    } else if (g_lastGoodBg != CLR_INVALID) {
        bgColor = g_lastGoodBg;
    } else {
        bgColor = GetSysColor(COLOR_BTNFACE);
    }

    RECT rc = { 0, 0, w, h };

    HBRUSH hBr = CreateSolidBrush(bgColor);
    FillRect(hdcDest, &rc, hBr);
    DeleteObject(hBr);

    LOGFONTW lfH = g_font;
    lfH.lfEscapement = 0;
    lfH.lfOrientation = 0;

    HFONT hFont = CreateFontIndirectW(&lfH);
    if (hFont) {
        HFONT hOld = (HFONT)SelectObject(hdcDest, hFont);
        SetBkMode(hdcDest, TRANSPARENT);
        SetTextColor(hdcDest, GetSysColor(COLOR_BTNTEXT));

        DrawTextW(hdcDest, g_text, -1, &rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        SelectObject(hdcDest, hOld);
        DeleteObject(hFont);
    }

    g_pending = false;

    return result;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)ExtTextOutW,
                        (void*)ExtTextOutW_Hook,
                        (void**)&ExtTextOutW_Original);

    Wh_SetFunctionHook((void*)BitBlt,
                        (void*)BitBlt_Hook,
                        (void**)&BitBlt_Original);

    return TRUE;
}

// ==WindhawkMod==
// @id              fix-legacy-taskbar-tray-input-indicator
// @name            Fix language indicator in Win10 taskbar under Win11 24H2+
// @description     Fixes text orientation in the keyboard layout indicator in Win10 taskbar running under Win11 24H2+
// @version         1.0.0
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

static wchar_t g_lastText[8] = {};
static DWORD g_captureTime = 0;
static bool g_drawn = false;

static bool LooksLikeLayoutText(LPCWSTR lpchText, int len) {
    if (!lpchText || len < 2 || len > 4) return false;
    for (int i = 0; i < len; i++) {
        if (!IsCharAlphaW(lpchText[i])) return false;
    }
    return true;
}

static bool IsRotatedFont(HDC hdc) {
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    if (!hFont) return false;
    LOGFONTW lf = {};
    if (GetObjectW(hFont, sizeof(lf), &lf) == 0) return false;
    return (lf.lfEscapement != 0 || lf.lfOrientation != 0);
}

using ExtTextOutW_t = BOOL(WINAPI*)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);
ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options, 
                              const RECT* lprect, LPCWSTR lpString, 
                              UINT c, const INT* lpDx) {
    int len = (int)c;
    
    if (LooksLikeLayoutText(lpString, len) && IsRotatedFont(hdc)) {
        wcsncpy_s(g_lastText, lpString, len);
        g_lastText[len] = 0;
        g_captureTime = GetTickCount();
        g_drawn = false;
        return TRUE;
    }

    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
BitBlt_t BitBlt_Original = nullptr;

BOOL WINAPI BitBlt_Hook(HDC hdcDest, int xDest, int yDest, int width, int height,
                         HDC hdcSrc, int xSrc, int ySrc, DWORD rop) {
    
    BOOL result = BitBlt_Original(hdcDest, xDest, yDest, width, height, hdcSrc, xSrc, ySrc, rop);
    
    DWORD elapsed = GetTickCount() - g_captureTime;
    
    // Проверяем: есть текст, прошло < 100мс, ещё не рисовали
    if (g_lastText[0] != 0 && elapsed < 100 && !g_drawn) {
        // Проверяем только размер копируемой области: ~36x20
        if (width >= 30 && width <= 50 && height >= 15 && height <= 30) {
            
            RECT rc = { xDest, yDest, xDest + width, yDest + height };
            
            HBRUSH hBrush = GetSysColorBrush(COLOR_3DFACE);
            FillRect(hdcDest, &rc, hBrush);
            
            LOGFONTW lf = {};
            lf.lfHeight = -11;
            lf.lfWeight = FW_NORMAL;
            lf.lfCharSet = DEFAULT_CHARSET;
            wcscpy_s(lf.lfFaceName, L"Tahoma");
            
            HFONT hFont = CreateFontIndirectW(&lf);
            if (hFont) {
                HFONT hOld = (HFONT)SelectObject(hdcDest, hFont);
                SetBkMode(hdcDest, TRANSPARENT);
                SetTextColor(hdcDest, GetSysColor(COLOR_BTNTEXT));
                
                DrawTextW(hdcDest, g_lastText, -1, &rc, 
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                SelectObject(hdcDest, hOld);
                DeleteObject(hFont);
            }
            
            g_drawn = true;
        }
    }
    
    return result;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutW_Hook, (void**)&ExtTextOutW_Original);
    Wh_SetFunctionHook((void*)BitBlt, (void*)BitBlt_Hook, (void**)&BitBlt_Original);
    return TRUE;
}

void Wh_ModUninit() {
}

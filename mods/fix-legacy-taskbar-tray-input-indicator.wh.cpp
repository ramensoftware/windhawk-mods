// ==WindhawkMod==
// @id              fix-legacy-taskbar-tray-input-indicator
// @name            Fix language indicator in Win10 taskbar under Win11 24H2+
// @description     Fixes text orientation in the keyboard layout indicator in Win10 taskbar running under Win11 24H2+
// @version         2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -luxtheme
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

When visual themes are enabled, the native background (including the hover
highlight) is preserved. When themes are disabled, the "Button face" color
is used.

**Note:** This mod only works with the classic taskbar, not the modern 
Windows 11 taskbar.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <uxtheme.h>

static wchar_t g_text[8] = {};
static wchar_t g_lastDrawnText[8] = {};
static LOGFONTW g_font = {};
static int g_srcW = 0;
static int g_srcH = 0;
static bool g_pending = false;
static DWORD g_threadId = 0;
static COLORREF g_lastGoodBg = CLR_INVALID;
static bool g_firstBltDone = false;
static bool g_alreadyDrawn = false;
static bool g_themed = false;   // темы были включены в момент начала отрисовки

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

static bool ThemesEnabled() {
    return IsThemeActive() != FALSE;
}

static HWND FindIndicatorWnd() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) return nullptr;
    HWND hNotify = FindWindowExW(hTray, nullptr, L"TrayNotifyWnd", nullptr);
    if (!hNotify) return nullptr;
    return FindWindowExW(hNotify, nullptr, L"TrayInputIndicatorWClass", nullptr);
}

static void FillSolid(HDC hdc, const RECT* rc, COLORREF color) {
    HBRUSH hBr = CreateSolidBrush(color);
    FillRect(hdc, rc, hBr);
    DeleteObject(hBr);
}

// Цвет обычного фона панели под индикатором (через временный битмап).
// Используется только как запасной вариант для выбора цвета текста.
static COLORREF SampleThemeBg(int w, int h) {
    COLORREF result = CLR_INVALID;

    HWND hInd = FindIndicatorWnd();
    if (hInd && w > 0 && h > 0) {
        HDC hScr = GetDC(nullptr);
        if (hScr) {
            HDC hMem = CreateCompatibleDC(hScr);
            HBITMAP hbm = CreateCompatibleBitmap(hScr, w, h);
            if (hMem && hbm) {
                HGDIOBJ hOld = SelectObject(hMem, hbm);
                RECT rc = { 0, 0, w, h };
                if (SUCCEEDED(DrawThemeParentBackground(hInd, hMem, &rc))) {
                    result = GetPixel(hMem, w / 2, h / 2);
                }
                SelectObject(hMem, hOld);
            }
            if (hbm) DeleteObject(hbm);
            if (hMem) DeleteDC(hMem);
            ReleaseDC(nullptr, hScr);
        }
    }

    if (result != CLR_INVALID) {
        g_lastGoodBg = result;
        return result;
    }
    if (g_lastGoodBg != CLR_INVALID) return g_lastGoodBg;
    return GetSysColor(COLOR_BTNFACE);
}

// Растягиваем столбец пикселей у левого края (там фон/hover правильный)
// по всей внутренней ширине индикатора. Так «дыра» в середине, закрашенная
// обычным цветом панели, получает тот же цвет, что и края (включая hover).
// Края по 3 пикселя не трогаем, чтобы сохранить рамку подсветки.
// Возвращает цвет пикселя в центре после исправления.
static COLORREF RepairBackgroundFromEdge(HDC hdc, int w, int h) {
    const int kSrcX = 2;
    const int kMargin = 3;

    if (w > 2 * kMargin + 4 && h > 0) {
        StretchBlt(hdc,
                   kMargin, 0, w - 2 * kMargin, h,
                   hdc,
                   kSrcX, 0, 1, h,
                   SRCCOPY);
    }
    return GetPixel(hdc, w / 2, h / 2);
}

static COLORREF TextColorForBg(COLORREF bg, bool themed) {
    if (!themed) {
        return GetSysColor(COLOR_BTNTEXT);
    }
    int r = GetRValue(bg), g = GetGValue(bg), b = GetBValue(bg);
    int lum = (r * 299 + g * 587 + b * 114) / 1000;
    return (lum > 140) ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

// Рисует текст горизонтально. При включённых темах и 32-битном DIB-буфере
// используем DrawThemeTextEx с DTT_COMPOSITED, чтобы корректно записался
// альфа-канал.
static void DrawIndicatorText(HDC hdc, RECT* rc, COLORREF textColor, bool themed) {
    LOGFONTW lfH = g_font;
    lfH.lfEscapement = 0;
    lfH.lfOrientation = 0;

    HFONT hFont = CreateFontIndirectW(&lfH);
    if (!hFont) return;

    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    bool done = false;

    if (themed) {
        HBITMAP hbm = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        BITMAP bm = {};
        if (hbm && GetObject(hbm, sizeof(bm), &bm) &&
            bm.bmBitsPixel == 32 && bm.bmBits) {
            HTHEME hTheme = OpenThemeData(nullptr, L"CompositedWindow::Window");
            if (hTheme) {
                DTTOPTS opts = {};
                opts.dwSize = sizeof(opts);
                opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
                opts.crText = textColor;
                HRESULT hr = DrawThemeTextEx(
                    hTheme, hdc, 0, 0, g_text, -1,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX,
                    rc, &opts);
                done = SUCCEEDED(hr);
                CloseThemeData(hTheme);
            }
        }
    }

    if (!done) {
        int oldMode = SetBkMode(hdc, TRANSPARENT);
        COLORREF oldColor = SetTextColor(hdc, textColor);

        DrawTextW(hdc, g_text, -1, rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        SetTextColor(hdc, oldColor);
        SetBkMode(hdc, oldMode);
    }

    SelectObject(hdc, hOld);
    DeleteObject(hFont);
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
        
        wchar_t newText[8] = {};
        wcsncpy_s(newText, lpString, len);
        newText[len] = 0;
        
        bool textChanged = (wcscmp(newText, g_lastDrawnText) != 0);
        
        if (textChanged) {
            g_alreadyDrawn = false;
        }
        
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
                g_firstBltDone = false;
                g_themed = ThemesEnabled();
                g_threadId = GetCurrentThreadId();
            }

            // Без тем — заливаем слой цветом BTNFACE, как раньше.
            // С темами ничего не заливаем.
            if (!g_themed) {
                RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
                FillSolid(hdc, &rc, GetSysColor(COLOR_BTNFACE));
            }

            return TRUE;
        }
    }

    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_Original = nullptr;

BOOL WINAPI BitBlt_Hook(HDC hdcDest, int xDest, int yDest, int w, int h,
                         HDC hdcSrc, int xSrc, int ySrc, DWORD rop) {

    bool ours = g_pending && g_srcW > 0 && g_srcH > 0 &&
                GetCurrentThreadId() == g_threadId &&
                xDest == 0 && yDest == 0;

    // С темами слой с текстом не копируем.
    if (ours && g_themed && w == g_srcW && h == g_srcH) {
        g_firstBltDone = true;
        return TRUE;
    }

    BOOL result = BitBlt_Original(hdcDest, xDest, yDest, w, h, hdcSrc, xSrc, ySrc, rop);

    if (!ours) {
        return result;
    }

    bool isFirstBlit = (w == g_srcW && h == g_srcH);
    if (isFirstBlit) {
        g_firstBltDone = true;
        return result;
    }
    
    if (!g_firstBltDone) {
        return result;
    }
    
    bool isIndicator = (w > g_srcW || h > g_srcH) && (w >= g_srcH);
    
    if (!isIndicator) {
        return result;
    }

    RECT rc = { 0, 0, w, h };

    if (g_themed) {
        // Убираем «дыру» в середине: протягиваем цвет от левого края
        // (обычный фон или hover — как есть у Windows).
        COLORREF bg = RepairBackgroundFromEdge(hdcDest, w, h);
        if (bg == CLR_INVALID || bg == RGB(0, 0, 0)) {
            bg = SampleThemeBg(w, h);
        } else {
            g_lastGoodBg = bg;
        }

        DrawIndicatorText(hdcDest, &rc, TextColorForBg(bg, true), true);

        wcscpy_s(g_lastDrawnText, g_text);
        g_pending = false;
        return result;
    }

    // Темы отключены — прежнее поведение с BTNFACE.
    if (g_alreadyDrawn) {
        DrawIndicatorText(hdcDest, &rc, GetSysColor(COLOR_BTNTEXT), false);
        g_pending = false;
        return result;
    }

    FillSolid(hdcDest, &rc, GetSysColor(COLOR_BTNFACE));
    DrawIndicatorText(hdcDest, &rc, GetSysColor(COLOR_BTNTEXT), false);

    wcscpy_s(g_lastDrawnText, g_text);
    g_alreadyDrawn = true;
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

// ==WindhawkMod==
// @id              classic-theme-explorer-lite
// @name            Classic Theme Explorer Lite
// @description     Classic Theme mitigations for Explorer ported from Explorer Patcher
// @version         1.1.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Classic theme fixes for Windows 10 taskbar (when running on either Windows 10 or Windows 11)
Includes ClientEdge 3D border for folders if you use SysListView32.
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {
    wchar_t wszClassName[200];
    ZeroMemory(wszClassName, 200);
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }

//  Disable this block if you don't want 3D borders in folders
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"SysListView32"))
    {
        GetClassNameW(GetParent(hWndParent), wszClassName, 200);
        if (wcscmp(wszClassName, L"Progman"))
        {
            dwExStyle |= WS_EX_CLIENTEDGE;
        }
    }

    if ( (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            dwStyle |= RBS_BANDBORDERS;
        }
    }

    HWND hWnd = CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

    return hWnd;
}
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Orig;
LONG_PTR SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    WCHAR lpClassName[200];
    ZeroMemory(lpClassName, 200);
    GetClassNameW(hWnd, lpClassName, 200);
    HWND hWndParent = GetParent(hWnd);

    if ( (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }
    if ( (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }

    if ( (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            if (nIndex == GWL_STYLE)
            {
                dwNewLong |= RBS_BANDBORDERS;
            }
        }
    }

    return SetWindowLongPtrW_Orig(hWnd, nIndex, dwNewLong);
}

HTHEME(*pOriginalOpenThemeDataForDpi)(HWND hWnd, LPCWSTR pszClassList, UINT dpi);
HTHEME OpenThemeDataForDpiHook(HWND hWnd, LPCWSTR pszClassList, UINT dpi)
{
    if ( (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"Taskband2")) {
        return (HTHEME)0xDEADBEEF;
    } else if ( (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"TrayNotifyFlyout")) {
        return (HTHEME)0xDEADBEFF;
    }

    return pOriginalOpenThemeDataForDpi(hWnd, pszClassList, dpi);
}

HRESULT(*pOriginalGetThemeMetric)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int* piVal);
HRESULT GetThemeMetricHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int* piVal) {

    const int TMT_WIDTH = 2416;
    const int TMT_HEIGHT = 2417;
    if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_WIDTH && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CXICON);
    }
    else if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_HEIGHT && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CYICON);
    }
    return S_OK;
}


HRESULT(*pOriginalGetThemeMargins)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS* pMargins);
HRESULT GetThemeMarginsHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS* pMargins) {

    const int TMT_SIZINGMARGINS = 3601;
    const int TMT_CONTENTMARGINS = 3602;
    HRESULT hr = S_OK;
    if (hTheme)
    {
        hr = pOriginalGetThemeMargins(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            iPropId,
            prc,
            pMargins
        );
    }

    if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_CONTENTMARGINS && iPartId == 5 && iStateId == 1) {
        // task list button measurements
        pMargins->cxLeftWidth = 4;
        pMargins->cyTopHeight = 3;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 3;
    } else if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_CONTENTMARGINS && iPartId == 1 && iStateId == 0) {
        // task list measurements
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 0;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 0;
    } else if (hTheme == (HTHEME)0xDEADBEEF && iPropId == TMT_SIZINGMARGINS && iPartId == 5 && iStateId == 1) {
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 10;
        pMargins->cxRightWidth = 0;
        pMargins->cyBottomHeight = 10;
    } else if (hTheme == (HTHEME)0xDEADBEFF && iPropId == TMT_CONTENTMARGINS && iPartId == 3 && iStateId == 0) {
        pMargins->cxLeftWidth = 4; // GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyTopHeight = 4; // GetSystemMetrics(SM_CYICONSPACING);
        pMargins->cxRightWidth = 4; //GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyBottomHeight = 4; // GetSystemMetrics(SM_CYICONSPACING);
    }

    HWND hShell_TrayWnd = FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShell_TrayWnd) {
        LONG dwStyle = 0;
        dwStyle = GetWindowLongW(hShell_TrayWnd, GWL_STYLE);
        dwStyle |= WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
        dwStyle &= ~WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
    }

    HWND hWnd = NULL;
    do {
        hWnd = FindWindowEx(
            NULL,
            hWnd,
            L"Shell_SecondaryTrayWnd",
            NULL
        );
        if (hWnd) {
            LONG dwStyle = 0;
            dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
            dwStyle |= WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
            dwStyle &= ~WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
        }
    } while (hWnd);

    return S_OK;
}

HRESULT(*pOriginalDrawThemeTextEx)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions);
HRESULT DrawThemeTextExHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS* pOptions) {

    COLORREF bc = GetBkColor(hdc);
    COLORREF fc = GetTextColor(hdc);
    int mode = SetBkMode(hdc, TRANSPARENT);

        wchar_t text[200];
    GetWindowTextW(GetForegroundWindow(), text, 200);

    BOOL bIsActiveUnhovered = (iPartId == 5 && iStateId == 5);
    BOOL bIsInactiveUnhovered = (iPartId == 5 && iStateId == 1);
    BOOL bIsInactiveHovered = (iPartId == 5 && iStateId == 2);
    BOOL bIsActiveHovered = bIsInactiveHovered && !wcscmp(text, pszText);

    SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

    HFONT hFont = NULL;
    if (bIsActiveUnhovered || bIsActiveHovered)
    {
        ncm.lfCaptionFont.lfWeight = FW_BOLD;
    }
    else
    {
        ncm.lfCaptionFont.lfWeight = FW_NORMAL;
    }
    hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));

    if (iPartId == 5 && iStateId == 0) // clock
    {
        pRect->top += 2;
    }

    HGDIOBJ hOldFont = SelectObject(hdc, hFont);
    DrawTextW(
        hdc,
        pszText,
        cchText, 
        pRect,
        dwTextFlags
    );
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SetBkColor(hdc, bc);
    SetTextColor(hdc, fc);
    SetBkMode(hdc, mode);

    return S_OK;
}

HRESULT(*pOriginalDrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, LPCRECT pClipRect);
HRESULT DrawThemeBackgroundHook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT pRect, LPCRECT pClipRect) {
    if (TRUE) {
        if (iPartId == 4 && iStateId == 1) {
            COLORREF bc = GetBkColor(hdc);
            COLORREF fc = GetTextColor(hdc);
            int mode = SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

            NONCLIENTMETRICSW ncm;
            ncm.cbSize = sizeof(NONCLIENTMETRICSW);
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

            HFONT hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));

            HGDIOBJ hOldFont = SelectObject(hdc, hFont);
            DWORD dwTextFlags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
            RECT rc = *pRect;
            rc.bottom -= 7;
            DrawTextW(
                hdc,
                L"\u2026",
                -1, 
                &rc,
                dwTextFlags
            );
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            SetBkColor(hdc, bc);
            SetTextColor(hdc, fc);
            SetBkMode(hdc, mode);
        }
        return S_OK;
    }

    return pOriginalDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

BOOL Wh_ModInit() {

    HMODULE hUxtheme = GetModuleHandle(L"uxtheme.dll");

    Wh_SetFunctionHook((void*)GetProcAddress(hUxtheme, "OpenThemeDataForDpi"), (void*)OpenThemeDataForDpiHook, (void**)&pOriginalOpenThemeDataForDpi);
    
    Wh_SetFunctionHook((void*)GetProcAddress(hUxtheme, "GetThemeMetric"), (void*)GetThemeMetricHook, (void**)&pOriginalGetThemeMetric);
    
    Wh_SetFunctionHook((void*)GetProcAddress(hUxtheme, "GetThemeMargins"), (void*)GetThemeMarginsHook, (void**)&pOriginalGetThemeMargins);

    Wh_SetFunctionHook((void*)GetProcAddress(hUxtheme, "DrawThemeTextEx"), (void*)DrawThemeTextExHook, (void**)&pOriginalDrawThemeTextEx);

    Wh_SetFunctionHook((void*)GetProcAddress(hUxtheme, "DrawThemeBackground"), (void*)DrawThemeBackgroundHook, (void**)&pOriginalDrawThemeBackground);

    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Orig);
    Wh_SetFunctionHook((void*)SetWindowLongPtrW,
                       (void*)SetWindowLongPtrW_Hook,
                       (void**)&SetWindowLongPtrW_Orig);

    return TRUE;
}

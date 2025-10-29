// ==WindhawkMod==
// @id              color-picker-dialog-hex-input
// @name            Color Picker Dialog Hex Input
// @description     Provides a hex input in the standard color picker dialog
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Color Picker Dialog Hex Input
Windows' native color picker dialog does not come with a way to input hex codes.
This mod adds an input that allows you to type hex colors in both the standard
6-digit format and the CSS 3-digit format (for example, `ABC` would result in `AABBCC`).

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/color-picker-dialog-hex-input.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hex_input_label: "He&x:"
  $name: Hex input label
  $description: The label text to show next to the hex input. Prefix the shortcut key with an ampersand (&).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <colordlg.h>

/*
 * We don't need 64-bit intermediate precision so we use this macro
 * instead of calling MulDiv.
 */
#define MultDiv(x, y, z)        (((INT)(x) * (INT)(y) + (INT)(z) / 2) / (INT)(z))

#define XPixFromXDU(x, cxChar)       MultDiv(x, cxChar, 4)
#define YPixFromYDU(y, cyChar)       MultDiv(y, cyChar, 8)

#define LONG2POINT(l, pt)    ((pt).x = (SHORT)LOWORD(l), (pt).y = (SHORT)HIWORD(l))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define COLOR_HEX            740
#define COLOR_HEXACCEL       741

#define COLORBOXES           64

typedef struct {
    UINT           ApiType;
    LPCHOOSECOLORW pCC;
    HANDLE         hLocal;
    HANDLE         hDialog;
    HPALETTE       hPal;
    DWORD          currentRGB;
    WORD           currentHue;
    WORD           currentSat;
    WORD           currentLum;
    WORD           nHueWidth;
    WORD           nSatHeight;
    WORD           nLumHeight;
    WORD           nCurMix;
    WORD           nCurDsp;
    WORD           nCurBox;
    WORD           nHuePos;
    WORD           nSatPos;
    WORD           nLumPos;
    RECT           rOriginal;
    RECT           rRainbow;
    RECT           rLumScroll;
    RECT           rLumPaint;
    RECT           rCurrentColor;
    RECT           rNearestPure;
    RECT           rColorSamples;
    BOOL           bFoldOut;
    DWORD          rgbBoxColor[COLORBOXES];
    LPCHOOSECOLORA pCCA;
} COLORINFO;

#define COLORPROP  (LPCTSTR) 0xA000L

typedef COLORINFO *PCOLORINFO;

int (WINAPI *GdiGetCharDimensions)(
    HDC hdc,
    TEXTMETRICW *lptm,
    LPINT lpcy);

/* Get font metrics for calculating dialog item placements. */
int GetDialogCharDimensions(HWND hDlg, LPINT lpcy)
{
    HFONT hfont = (HFONT)SendMessageW(hDlg, WM_GETFONT, 0, 0);
    HDC hdc = GetDC(hDlg);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HFONT hfontOld = (HFONT)SelectObject(hdcMem, hfont);

    int cx = GdiGetCharDimensions(hdcMem, nullptr, lpcy);

    SelectObject(hdcMem, hfontOld);
    ReleaseDC(hDlg, hdc);
    DeleteDC(hdcMem);
    return cx;
}

void MoveDialogItem(HWND hDlg, int nIDDlgItem, int dxDU, int dyDU)
{
    HWND hwndItem = GetDlgItem(hDlg, nIDDlgItem);
    if (!hwndItem)
        return;

    int iAveCharHeight;
    int iAveCharWidth = GetDialogCharDimensions(hDlg, &iAveCharHeight);

    int dx = XPixFromXDU(dxDU, iAveCharWidth);
    int dy = YPixFromYDU(dyDU, iAveCharHeight);

    RECT rc;
    GetWindowRect(hwndItem, &rc);
    MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rc, 2);
    OffsetRect(&rc, dx, dy);
    SetWindowPos(
        hwndItem,
        NULL,
        rc.left, rc.top,
        0, 0,
        SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER
    );
}

inline bool IsGoodCharacter(WCHAR ch)
{
    // Allow control characters
    if (ch < 0x20 || ch == 0x7F)
        return true;
    if (ch >= L'0' && ch <= L'9')
        return true;
    if (ch >= L'A' && ch <= L'F')
        return true;
    if (ch >= L'a' && ch <= L'f')
        return true;
    return false;
}

LRESULT CALLBACK HexInputSubclassProc(
    HWND      hwnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    UINT_PTR  uIdSubclass, 
    DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
        case WM_CHAR:
        {
            if (!IsGoodCharacter(wParam))
            {
                MessageBeep(MB_OK);
                return 0;
            }
            break;
        }
        case WM_PASTE:
            if (OpenClipboard(hwnd))
            {
                HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                if (hData)
                {
                    LPCWSTR pszText = (LPCWSTR)GlobalLock(hData);
                    if (pszText)
                    {
                        for (LPCWSTR psz = pszText; *psz; psz++)
                        {
                            if (!IsGoodCharacter(*psz))
                            {
                                MessageBeep(MB_OK);
                                CloseClipboard();
                                return 0;
                            }
                        }
                    }
                }
                CloseClipboard();
            }
            break;
        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, HexInputSubclassProc, uIdSubclass);
            break;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}
                                  
void SetHexEdit(PCOLORINFO pCI)
{
    WCHAR szColor[7];
    swprintf_s(
        szColor,
        L"%02X%02X%02X",
        GetRValue(pCI->currentRGB),
        GetGValue(pCI->currentRGB),
        GetBValue(pCI->currentRGB));
    SetDlgItemTextW((HWND)pCI->hDialog, COLOR_HEX, szColor);
}

inline BYTE ParseHexChar(WCHAR ch)
{
    if (ch >= L'0' && ch <= L'9')
        return ch - L'0';
    if (ch >= L'A' && ch <= L'F')
        return ch - (L'A' - 0xA);
    if (ch >= L'a' && ch <= L'f')
        return ch - (L'a' - 0xA);
    return ch;
}

inline BYTE ParseHexByte(LPWSTR psz)
{
    return (ParseHexChar(psz[0]) << 4)
        | (ParseHexChar(psz[1]));
}

BOOL *pbMouseCapture = nullptr;

void (__fastcall *ChangeColorSettings)(PCOLORINFO pCI);
void (__fastcall *SetRGBEdit)(SHORT nRGBEdit, PCOLORINFO pCI);
void (__fastcall *SetHLSEdit)(SHORT nHLSEdit, PCOLORINFO pCI);

/**
  * Updating the hex display makes the crosshair paint itself
  * and absolutely obliterates the rainbow when dragging the mouse
  * on it. Just make it never draw when the mouse capture is active.
  */
void (__fastcall *CrossHairPaint_orig)(HDC, SHORT, SHORT, PCOLORINFO);
void __fastcall CrossHairPaint_hook(
    HDC hDC,
    SHORT x,
    SHORT y,
    PCOLORINFO pCI)
{
    if (*pbMouseCapture)
        return;
    CrossHairPaint_orig(hDC, x, y, pCI);
}

WNDPROC ColorDlgProc_orig;
LRESULT CALLBACK ColorDlgProc_hook(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            LRESULT lRes = ColorDlgProc_orig(hwnd, uMsg, wParam, lParam);
            PCOLORINFO pCI = (PCOLORINFO)GetPropW(hwnd, COLORPROP);
            // Don't interfere with custom dialog templates
            if (!(pCI->pCC->Flags & CC_ENABLETEMPLATE))
            {
                int iAveCharHeight;
                int iAveCharWidth = GetDialogCharDimensions(hwnd, &iAveCharHeight);
                HFONT hfont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);

                // "Hex:" label
                LPCWSTR pszLabelText = Wh_GetStringSetting(L"hex_input_label");
                HWND hwndHexAccel = CreateWindowExW(
                    0,
                    WC_STATICW,
                    pszLabelText,
                    WS_CHILD | WS_VISIBLE | SS_RIGHT,
                    XPixFromXDU(231, iAveCharWidth),
                    YPixFromYDU(168, iAveCharHeight),
                    XPixFromXDU(20, iAveCharWidth),
                    YPixFromYDU(9, iAveCharHeight),
                    hwnd,
                    (HMENU)COLOR_HEXACCEL,
                    NULL,
                    nullptr
                );
                Wh_FreeStringSetting(pszLabelText);
                SendMessageW(hwndHexAccel, WM_SETFONT, (WPARAM)hfont, 0);

                // Input
                HWND hwndHexInput = CreateWindowExW(
                    WS_EX_CLIENTEDGE,
                    WC_EDITW,
                    nullptr,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_UPPERCASE,
                    XPixFromXDU(253, iAveCharWidth),
                    YPixFromYDU(166, iAveCharHeight),
                    XPixFromXDU(34, iAveCharWidth),
                    YPixFromYDU(12, iAveCharHeight),
                    hwnd,
                    (HMENU)COLOR_HEX,
                    NULL,
                    nullptr
                );
                SendMessageW(hwndHexInput, WM_SETFONT, (WPARAM)hfont, 0);
                SendMessageW(hwndHexInput, EM_SETLIMITTEXT, 6, 0);
                SetWindowSubclass(hwndHexInput, HexInputSubclassProc, 0, 0);

                // Increase dialog size to account for the hex input
                
                if (pCI->pCC->Flags & CC_FULLOPEN)
                {
                    RECT rc;
                    GetWindowRect(hwnd, &rc);
                    rc.bottom += YPixFromYDU(18, iAveCharHeight);
                    SetWindowPos(
                        hwnd,
                        NULL,
                        0, 0,
                        rc.right - rc.left,
                        rc.bottom - rc.top,
                        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
                    );
                    
                    // Move OK and Cancel buttons so they are at the bottom of the dialog again.
                    MoveDialogItem(hwnd, IDOK, 0, 18);
                    MoveDialogItem(hwnd, IDCANCEL, 0, 18);
                }
                else
                {
                    pCI->rOriginal.bottom += YPixFromYDU(18, iAveCharHeight);
                }

                // Move "Add to Custom Colors" button down to account for hex input
                MoveDialogItem(hwnd, COLOR_ADD, 0, 18);

                SetHexEdit(pCI);
            }
            
            return lRes;
        }
        case WM_MOUSEMOVE:
            if (!*pbMouseCapture)
                break;
            [[fallthrough]];
        case WM_LBUTTONDOWN:
        {
            PCOLORINFO pCI = (PCOLORINFO)GetPropW(hwnd, COLORPROP);
            COLORREF clrSave = pCI->currentRGB;
            LRESULT lRes = ColorDlgProc_orig(hwnd, uMsg, wParam, lParam);
            if (clrSave != pCI->currentRGB)
            {
                SetHexEdit(pCI);
                if (uMsg == WM_MOUSEMOVE)
                {
                    HDC hDC = GetDC(hwnd);
                    ReleaseDC(hwnd, hDC);
                }
            }
            return lRes;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case COLOR_MIX:
                {
                    PCOLORINFO pCI = (PCOLORINFO)GetPropW(hwnd, COLORPROP);
                    if (!(pCI->pCC->Flags & CC_ENABLETEMPLATE)
                    && HIWORD(wParam) == BN_CLICKED)
                    {
                        // Move OK and Cancel buttons so they are at the bottom of the dialog again.
                        MoveDialogItem(hwnd, IDOK, 0, 18);
                        MoveDialogItem(hwnd, IDCANCEL, 0, 18);
                    }
                    break;
                }
                case COLOR_HEX:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        WCHAR szText[7];
                        GetWindowTextW((HWND)lParam, szText, ARRAYSIZE(szText));
                        int nTextLen = wcslen(szText);
                        COLORREF clr = CLR_DEFAULT;

                        // Typical hex color (e.g. #ABCDEF)
                        if (nTextLen == 6)
                        {
                            clr = RGB(
                                ParseHexByte(szText),
                                ParseHexByte(szText + 2),
                                ParseHexByte(szText + 4));
                        }
                        // CSS-style 3 char hex color (e.g. #ABC -> #AABBCC)
                        else if (nTextLen == 3)
                        {
                            BYTE rBit = ParseHexChar(szText[0]);
                            BYTE gBit = ParseHexChar(szText[1]);
                            BYTE bBit = ParseHexChar(szText[2]);

                            clr = RGB(
                                rBit | (rBit << 4),
                                gBit | (gBit << 4),
                                bBit | (bBit << 4)
                            );
                        }

                        if (clr != CLR_DEFAULT)
                        {
                            PCOLORINFO pCI = (PCOLORINFO)GetPropW(hwnd, COLORPROP);
                            pCI->currentRGB = clr;

                            ChangeColorSettings(pCI);
                            SetRGBEdit(0, pCI);
                            SetHLSEdit(0, pCI);
                        }
                    }
                    break;
                case COLOR_RED:
                case COLOR_BLUE:
                case COLOR_GREEN:
                case COLOR_HUE:
                case COLOR_SAT:
                case COLOR_LUM:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        PCOLORINFO pCI = (PCOLORINFO)GetPropW(hwnd, COLORPROP);
                        COLORREF clrSave = pCI->currentRGB;
                        LRESULT lRes = ColorDlgProc_orig(hwnd, uMsg, wParam, lParam);
                        if (clrSave != pCI->currentRGB)
                        {
                            SetHexEdit(pCI);
                        }
                        return lRes;
                    }
            }
            break;
        }
    }
    return ColorDlgProc_orig(hwnd, uMsg, wParam, lParam);
}

const WindhawkUtils::SYMBOL_HOOK comdlg32DllHooks[] = {
    {
        {
#ifdef _WIN64
            L"ColorDlgProc"
#else
            L"_ColorDlgProc@16"
#endif
        },
        &ColorDlgProc_orig,
        ColorDlgProc_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"ChangeColorSettings"
#else
            L"_ChangeColorSettings@4"
#endif
        },
        &ChangeColorSettings,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"SetRGBEdit"
#else
            L"_SetRGBEdit@8"
#endif
        },
        &SetRGBEdit,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"SetHLSEdit"
#else
            L"_SetHLSEdit@8"
#endif
        },
        &SetHLSEdit,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"CrossHairPaint"
#else
            L"_CrossHairPaint@16"
#endif
        },
        &CrossHairPaint_orig,
        CrossHairPaint_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"bMouseCapture"
#else
            L"_bMouseCapture"
#endif
        },
        &pbMouseCapture,
        nullptr,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hGdi32, hComDlg32;

    BOOL fSucceeded = TRUE;
    hGdi32 = LoadLibraryExW(L"gdi32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hGdi32)
    {
        Wh_Log(L"Failed to load gdi32.dll");
        fSucceeded = FALSE;
        goto Exit;
    }

    *(void **)&GdiGetCharDimensions = (void *)GetProcAddress(hGdi32, "GdiGetCharDimensions");
    if (!GdiGetCharDimensions)
    {
        Wh_Log(L"Failed to get address of GdiGetCharDimensions");
        fSucceeded = FALSE;
        goto Exit;
    }

    hComDlg32 = LoadLibraryExW(L"comdlg32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hComDlg32)
    {
        Wh_Log(L"Failed to load comdlg32.dll");
        fSucceeded = FALSE;
        goto Exit;
    }

    if (!WindhawkUtils::HookSymbols(
        hComDlg32,
        comdlg32DllHooks,
        ARRAYSIZE(comdlg32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in comdlg32.dll");
        fSucceeded = FALSE;
        goto Exit;
    }

Exit:
    if (hGdi32)
        FreeLibrary(hGdi32);
    return fSucceeded;
}
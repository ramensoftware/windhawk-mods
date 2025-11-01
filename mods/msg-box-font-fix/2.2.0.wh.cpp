// ==WindhawkMod==
// @id              msg-box-font-fix
// @name            Message Box Fix
// @description     Reverts message boxes to their behavior from before Windows 10 1709
// @version         2.2.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -lgdi32 -lcomctl32 -DWINVER=0x0A00
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Message Box Fix
Starting with Windows 10 1709, message boxes render their font size 1pt less than the
user-defined size. This mod aims to replicate the behavior of message boxes from various
versions before Windows 10 1709.

## Styles

- **Windows 7-10 1703**: Fixes the font size
- **Windows Vista**: Same as Windows 7-10 1703, but message boxes with no icon will play
  the "Default Beep" sound
- **Windows 98-XP**: Same as Windows Vista, but with different sizes and positions, as well as
  a solid background.
- **Windows 95/NT 4.0**: Same as Windows 98-XP, but button widths are dynamic (typically smaller).

## Before

![Before](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-before.png)
![Before (classic)](https://raw.githubusercontent.com/aubymori/images/main/message-box-fix-before-classic.png)

## After

![After](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-after.png)
![After (classic)](https://raw.githubusercontent.com/aubymori/images/main/message-box-fix-after-classic.png)
![After (Windows 95/NT 4.0)](https://raw.githubusercontent.com/aubymori/images/main/message-box-fix-after-nt4.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: seven
  $name: Message box style
  $options:
  - nt4: Windows 95/NT 4.0
  - xp: Windows 98-XP
  - vista: Windows Vista
  - seven: Windows 7-10 1703
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

HMODULE g_hUser32 = NULL;
enum MESSAGEBOXSTYLE
{
    MBS_NT4,
    MBS_XP,
    MBS_VISTA,
    MBS_SEVEN,
} g_mbStyle = MBS_SEVEN;

/* Only available in Windows 10 version 1607 and greater. */
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hWnd);
WINUSERAPI BOOL WINAPI SystemParametersInfoForDpi(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);

/* Imported from win32u.dll */
typedef INT_PTR (WINAPI *NtUserCallOneParam_t)(DWORD dwParam, DWORD xpfnProc);
NtUserCallOneParam_t NtUserCallOneParam = nullptr;
typedef INT_PTR (NTAPI *NtUserCallHwnd_t)(HWND hwnd, DWORD xpfnProc);
NtUserCallHwnd_t NtUserCallHwnd = nullptr;

/* New in Windows 11 24H2. Using old method breaks. */
typedef INT_PTR (WINAPI *NtUserMessageBeep_t)(DWORD dwBeep);
NtUserMessageBeep_t NtUserMessageBeep = nullptr;
typedef INT_PTR (WINAPI *NtUserSetMsgBox_t)(HWND hwnd);
NtUserSetMsgBox_t NtUserSetMsgBox = nullptr;

HFONT (__fastcall *GetMessageBoxFontForDpi_orig)(UINT);
HFONT __fastcall GetMessageBoxFontForDpi_hook(
    UINT nDpi
)
{
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    if (SystemParametersInfoForDpi(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        0,
        nDpi
    ))
    {
        return CreateFontIndirectW(&ncm.lfMessageFont);
    }
    return NULL;
}

#pragma region "Classic style"

typedef struct _MSGBOXDATA {
    // BEGIN MSGBOXPARAMS
    UINT        cbSize;
    HWND        hwndOwner;
    HINSTANCE   hInstance;
    LPCWSTR     lpszText;
    LPCWSTR     lpszCaption;
    DWORD       dwStyle;
    LPCWSTR     lpszIcon;
    DWORD_PTR   dwContextHelpId;
    MSGBOXCALLBACK      lpfnMsgBoxCallback;
    DWORD       dwLanguageId;
    // END MSGBOXPARAMS
    HWND    HWNDOwner;
    DWORD   dwPadding;
    WORD    wLanguageId;
    const INT *pidButton;
    LPCWSTR *ppszButtonText;
    DWORD   cButtons;
    UINT    DefButton;
    UINT    CancelId;
    DWORD   dwTimeout;
    HWND *phwndList;
    DWORD   dwReserved[20];
} MSGBOXDATA, *PMSGBOXDATA, *LPMSGBOXDATA;

typedef struct _TEB64 {
	UCHAR ignored[0x0800];
	ULONG_PTR Win32ClientInfo[0x3E];
} TEB64, *PTEB64;

typedef struct _DESKTOPINFO
{
	DWORD ignored[12];
	int cntMBox;
} DESKTOPINFO, *PDESKTOPINFO;

typedef struct _CLIENTINFO64 {
	DWORD64 CI_flags;
	DWORD64 cSpins;
	DWORD dwExpWinVer;
	DWORD dwCompatFlags;
	DWORD dwCompatFlags2;
	DWORD dwTIFlags;
	PDESKTOPINFO pDeskInfo;
} CLIENTINFO64, *PCLIENTINFO64;

inline PCLIENTINFO64 GetClientInfo()
{
    PTEB64 teb = (PTEB64)NtCurrentTeb();
// This breaks on real 32-bit Windows, it's WOW64 specific.
// But literally nobody uses that anymore, so who cares.
#ifndef _WIN64
	teb = *(PTEB64 *)((LPBYTE)teb + 0x0F70);
#endif
    return (PCLIENTINFO64)teb->Win32ClientInfo;
}

static CONST WCHAR szEmpty[] = L"";

// IDs and such
#define MAX_RES_STRING 256
#define UNICODE_RLM 0x200F  // RIGHT-TO-LEFT MARK      (RLM)
#define IDUSERICON      20
#define BUTTONCODE	  0x80
#define STATICCODE	  0x82
#define SFI_PLAYEVENTSOUND 57
#define SFI_SETMSGBOX      89

// Macros
#define MAXUSHORT       (0xFFFF)
#define IS_PTR(p)       ((((ULONG_PTR)(p)) & ~MAXUSHORT) != 0)
#define PTR_TO_ID(p)    ((USHORT)(((ULONG_PTR)(p)) & MAXUSHORT))
#define SYSMET(i)               GetSystemMetrics( SM_##i )
#define XPixFromXDU(x, cxChar)       MulDiv(x, cxChar, 4)
#define YPixFromYDU(y, cyChar)       MulDiv(y, cyChar, 8)
#define XDUFromXPix(x, cxChar)       MulDiv(x, 4, cxChar)
#define YDUFromYPix(y, cyChar)       MulDiv(y, 8, cyChar)
#define NextWordBoundary(p)     ((PBYTE)(p) + ((ULONG_PTR)(p) & 1))
#define NextDWordBoundary(p)    ((PBYTE)(p) + ((ULONG_PTR)(-(LONG_PTR)(p)) & 3))
#define USERGLOBALLOCK(h, p)   p = (LPWSTR)GlobalLock((HANDLE)(h))
#define USERGLOBALUNLOCK(h)             GlobalUnlock((HANDLE)(h))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// Dialog unit dimensions
#define DU_OUTERMARGIN    7
#define DU_INNERMARGIN    10
#define DU_BTNGAP         4   // D.U. of space between buttons
#define DU_BTNHEIGHT      14  // D.U. of button height
#define DU_BTNWIDTH       50  // D.U. of minimum button width in a message box

// Custom struct and function because we can't use gpsi
typedef struct tagMSGBOXMETRICS
{
    WORD cxMsgFontChar;
    WORD cyMsgFontChar;
    WORD wMaxBtnSize;
    HFONT hCaptionFont;
    HFONT hMessageFont;
} MSGBOXMETRICS, *LPMSGBOXMETRICS;
MSGBOXMETRICS mbm;

int GetCharDimensions(HDC hdc, TEXTMETRICW *lptm, int *lpcy)
{
    TEXTMETRICW tm;

    if (!GetTextMetricsW(hdc, &tm)) // _GetTextMetricsW
    {
        // Fallback to global system font: (NT User caches this information, but we don't have access u_u)
        GetTextMetricsW(GetDC(nullptr), &tm);

        if (tm.tmAveCharWidth == 0)
        {
            // Ultimate fallback:
            tm.tmAveCharWidth = 8;
        }
    }

    if (lptm)
    {
        *lptm = tm;
    }

    if (lpcy)
    {
        *lpcy = tm.tmHeight;
    }

    // Variable-width fonts calculate a true average rather than relying on tmAveCharWidth.
    if (tm.tmPitchAndFamily & TMPF_FIXED_PITCH)
    {
        static const WCHAR wszAvgChars[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

        SIZE size;
        if (GetTextExtentPoint32W(hdc, wszAvgChars, ARRAYSIZE(wszAvgChars) - 1, &size)) // GreGetTextExtentW
        {
            // The above string is 26 * 2 characters. + 1 rounds the result.
            return ((size.cx / 26) + 1) / 2;
        }
    }

    return tm.tmAveCharWidth;
}

UINT MB_FindLongestString(HDC hdc, LPCWSTR *ppszButtonText, DWORD cButtons)
{
    UINT wRetVal;
    int i, iMaxLen = 0, iNewMaxLen;
    LPCWSTR szMaxStr;
    SIZE sizeOneChar;
    SIZE sizeMaxStr;
    WCHAR szOneChar[2] = L"0";

    for (i = 800; i <= 810; i++) {
        WCHAR szCurStr[256];
        LoadStringW(g_hUser32, i, szCurStr, 256);
        if ((iNewMaxLen = wcslen(szCurStr)) > iMaxLen) {
            iMaxLen = iNewMaxLen;
            szMaxStr = szCurStr;
        }
    }

    // Modification from original NT4 func: account for any custom strings.
    // Don't want any custom message boxes cutting off text.
    for (i = 0; i < (int)cButtons; i++)
    {
        LPCWSTR szCurStr = ppszButtonText[i];
        if ((iNewMaxLen = wcslen(szCurStr)) > iMaxLen)
        {
            iMaxLen = iNewMaxLen;
            szMaxStr = szCurStr;
        }
    }

    /*
     * Find the longest string
     */
    GetTextExtentPointW(hdc, szOneChar, 1, &sizeOneChar);
    GetTextExtentPointW(hdc, szMaxStr, iMaxLen, &sizeMaxStr);
    wRetVal = (UINT)(sizeMaxStr.cx + (sizeOneChar.cx * 2));

    return wRetVal;
}

BOOL GetMessageBoxMetrics(LPMSGBOXMETRICS pmbm, LPCWSTR *ppszButtonText, DWORD cButtons)
{  
    if (!pmbm)
        return FALSE;

    ZeroMemory(pmbm, sizeof(MSGBOXMETRICS));
    BOOL succeeded = TRUE;
    HDC hDesktopDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hDesktopDC);

    HFONT hf = GetMessageBoxFontForDpi_hook(GetDeviceCaps(hMemDC, LOGPIXELSX));
    SelectObject(hMemDC, hf);

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoForDpi(
        SPI_GETNONCLIENTMETRICS,
        sizeof(ncm),
        &ncm,
        0,
        GetDeviceCaps(hMemDC, LOGPIXELSX)
    );
    
    TEXTMETRICW tm;
    int iAverageCharHeight;
    int iAverageCharWidth = GetCharDimensions(hMemDC, nullptr, &iAverageCharHeight);
    succeeded = GetTextMetricsW(hMemDC, &tm);
    if (succeeded)
    {
        pmbm->cxMsgFontChar = iAverageCharWidth;
        pmbm->cyMsgFontChar = tm.tmHeight;
        pmbm->wMaxBtnSize = (g_mbStyle == MBS_NT4)
            ? MB_FindLongestString(hMemDC, ppszButtonText, cButtons)
            : XPixFromXDU(DU_BTNWIDTH, iAverageCharWidth);
        pmbm->hCaptionFont = CreateFontIndirectW(&ncm.lfCaptionFont);
        pmbm->hMessageFont = hf;
    }

    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDesktopDC);
    return succeeded;
}

UINT MB_GetIconOrdNum(UINT rgBits)
{
    switch (rgBits & MB_ICONMASK) {
    case MB_USERICON:
    case MB_ICONHAND:
        return PtrToUlong(IDI_HAND);

    case MB_ICONQUESTION:
        return PtrToUlong(IDI_QUESTION);

    case MB_ICONEXCLAMATION:
        return PtrToUlong(IDI_EXCLAMATION);

    case MB_ICONASTERISK:
        return PtrToUlong(IDI_ASTERISK);
    }

    return 0;
}


UINT MB_FindDlgTemplateSize(LPMSGBOXDATA lpmb)
{
    ULONG_PTR cbLen;
    UINT cbT;
    UINT i;
    UINT wCount;

    wCount = lpmb->cButtons;

    /* Start with dialog header's size */
    cbLen = (ULONG_PTR)NextWordBoundary(sizeof(DLGTEMPLATE) + sizeof(WCHAR));
    cbLen = (ULONG_PTR)NextWordBoundary(cbLen + sizeof(WCHAR));
    cbLen += wcslen(lpmb->lpszCaption) * sizeof(WCHAR) + sizeof(WCHAR);
    cbLen += sizeof(WORD);                   // Font height
    cbLen = (ULONG_PTR)NextDWordBoundary(cbLen);

    /* Check if an icon is present */
    if (lpmb->dwStyle & MB_ICONMASK)
        cbLen += (ULONG_PTR)NextDWordBoundary(sizeof(DLGITEMTEMPLATE) + 7 * sizeof(WCHAR));

    /* Find the number of buttons in the msg box */
    for (i = 0; i < wCount; i++) {
        cbLen = (ULONG_PTR)NextWordBoundary(cbLen + sizeof(DLGITEMTEMPLATE) +
            (2 * sizeof(WCHAR)));
        cbT = (wcslen(lpmb->ppszButtonText[i]) + 1) * sizeof(WCHAR);
        cbLen = (ULONG_PTR)NextWordBoundary(cbLen + cbT);
        cbLen += sizeof(WCHAR);
        cbLen = (ULONG_PTR)NextDWordBoundary(cbLen);
    }

    /* Add in the space required for the text message (if there is one) */
    if (lpmb->lpszText != NULL) {
        cbLen = (ULONG_PTR)NextWordBoundary(cbLen + sizeof(DLGITEMTEMPLATE) +
            (2 * sizeof(WCHAR)));
        cbT = (wcslen(lpmb->lpszText) + 1) * sizeof(WCHAR);
        cbLen = (ULONG_PTR)NextWordBoundary(cbLen + cbT);
        cbLen += sizeof(WCHAR);
        cbLen = (ULONG_PTR)NextDWordBoundary(cbLen);
    }

    return (UINT)cbLen;
}

LPBYTE MB_UpdateDlgHdr(
    LPDLGTEMPLATE lpDlgTmp,
    long lStyle,
    long lExtendedStyle,
    BYTE bItemCount,
    int iX,
    int iY,
    int iCX,
    int iCY,
    LPWSTR lpszCaption,
    int cchCaptionLen)
{
    LPTSTR lpStr;
    RECT rc;

    rc.left = iX + SYSMET(CXFIXEDFRAME) + SYSMET(CXPADDEDBORDER);
    rc.top = iY + SYSMET(CYFIXEDFRAME) + SYSMET(CXPADDEDBORDER);
    rc.right = iX + iCX - SYSMET(CXFIXEDFRAME) + SYSMET(CXPADDEDBORDER);
    rc.bottom = iY + iCY - SYSMET(CYFIXEDFRAME) + SYSMET(CXPADDEDBORDER);
    rc.top += SYSMET(CYCAPTION);

    lpDlgTmp->style = lStyle;
    lpDlgTmp->dwExtendedStyle = lExtendedStyle;
    lpDlgTmp->cdit = bItemCount;
    lpDlgTmp->x = XDUFromXPix(rc.left, mbm.cxMsgFontChar);
    lpDlgTmp->y = YDUFromYPix(rc.top, mbm.cyMsgFontChar);
    lpDlgTmp->cx = XDUFromXPix(rc.right - rc.left, mbm.cxMsgFontChar);
    lpDlgTmp->cy = YDUFromYPix(rc.bottom - rc.top, mbm.cyMsgFontChar);

    /*
     * Move pointer to variable length fields.  No menu resource for
     * message box, a zero window class (means dialog box class).
     */
    lpStr = (LPWSTR)(lpDlgTmp + 1);
    *lpStr++ = 0;                           // Menu
    lpStr = (LPWSTR)NextWordBoundary(lpStr);
    *lpStr++ = 0;                           // Class
    lpStr = (LPWSTR)NextWordBoundary(lpStr);

    /*
     * NOTE: iCaptionLen may be less than the length of the Caption string;
     * So, DO NOT USE lstrcpy();
     */
    RtlCopyMemory(lpStr, lpszCaption, cchCaptionLen * sizeof(WCHAR));
    lpStr += cchCaptionLen;
    *lpStr++ = TEXT('\0');

    /* Font height of 0x7FFF means use the message box font */
    *lpStr++ = 0x7FFF;

    return NextDWordBoundary(lpStr);
}

LPBYTE MB_UpdateDlgItem(
    LPDLGITEMTEMPLATE lpDlgItem,
    int iCtrlId,
    long lStyle,
    long lExtendedStyle,
    int iX,
    int iY,
    int iCX,
    int iCY,
    LPWSTR lpszText,
    UINT cchTextLen,
    int iControlClass)
{
    LPWSTR lpStr;
    BOOL fIsOrdNum;


    lpDlgItem->x = XDUFromXPix(iX, mbm.cxMsgFontChar);
    lpDlgItem->y = YDUFromYPix(iY, mbm.cyMsgFontChar);
    lpDlgItem->cx = XDUFromXPix(iCX, mbm.cxMsgFontChar);
    lpDlgItem->cy = YDUFromYPix(iCY, mbm.cyMsgFontChar);
    lpDlgItem->id = (WORD)iCtrlId;
    lpDlgItem->style = lStyle;
    lpDlgItem->dwExtendedStyle = lExtendedStyle;

    if (iControlClass == STATICCODE &&
        (((lStyle & 0x0F) == SS_LEFT) || ((lStyle & 0x0F) == SS_RIGHT)))
    {
        lpDlgItem->cx++;
        lpDlgItem->cy++;
    }

    lpStr = (LPWSTR)(lpDlgItem + 1);

    *lpStr++ = 0xFFFF;
    *lpStr++ = (BYTE)iControlClass;
    lpStr = (LPWSTR)NextWordBoundary(lpStr);

    fIsOrdNum = ((*lpszText == 0xFFFF) && (cchTextLen == sizeof(DWORD) / sizeof(WCHAR)));

    RtlCopyMemory(lpStr, lpszText, cchTextLen * sizeof(WCHAR));
    lpStr = lpStr + cchTextLen;
    if (!fIsOrdNum)
    {
        *lpStr = TEXT('\0');
        lpStr = (LPWSTR)NextWordBoundary(lpStr + 1);
    }

    *lpStr++ = 0;

    return NextDWordBoundary(lpStr);
}

LPBYTE MB_AddPushButtons(
    LPDLGITEMTEMPLATE  lpDlgTmp,
    LPMSGBOXDATA       lpmb,
    UINT               wLEdge,
    UINT               wBEdge)
{
    UINT   wYValue;
    UINT   i;
    UINT   wHeight;
    UINT   wCount = lpmb->cButtons;

    wHeight = YPixFromYDU(DU_BTNHEIGHT, mbm.cyMsgFontChar);

    wYValue = wBEdge - wHeight;         // Y coord for push buttons

    for (i = 0; i < wCount; i++) {

        lpDlgTmp = (LPDLGITEMTEMPLATE)MB_UpdateDlgItem(
            lpDlgTmp,                       /* Ptr to template */
            lpmb->pidButton[i],             /* Control Id */
            WS_TABSTOP | WS_CHILD | WS_VISIBLE | (i == 0 ? WS_GROUP : 0) |
            ((UINT)i == lpmb->DefButton ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
            0,
            wLEdge,                         /* X coord */
            wYValue,                        /* Y coord */
            mbm.wMaxBtnSize,                /* CX */
            wHeight,                        /* CY */
            (LPWSTR)lpmb->ppszButtonText[i],        /* String for button */
            (UINT)wcslen(lpmb->ppszButtonText[i]),/* Length */
            BUTTONCODE);

        /* Get the X coord for the next push button */
        wLEdge += mbm.wMaxBtnSize + XPixFromXDU(DU_BTNGAP, mbm.cxMsgFontChar);
    }

    return (LPBYTE)lpDlgTmp;
}

void MB_CopyToClipboard(HWND hwndDlg)
{
    LPCWSTR lpszRead;
    LPWSTR lpszAll, lpszWrite;
    HANDLE hData;
    static  CONST WCHAR   szLine[] = L"---------------------------\r\n";
    UINT cBufSize, i, cWrote;
    LPMSGBOXDATA lpmb;

    if (!(lpmb = (LPMSGBOXDATA)GetWindowLongPtr(hwndDlg, GWLP_USERDATA))) {
        return;
    }

    if (!OpenClipboard(hwndDlg)) {
        return;
    }

    /*
     * Calculate the buffer size:
     *      - the message text can be all \n, that will become \r\n
     *      - there are a few extra \r\n (that's why 8)
     */
    cBufSize = (lpmb->lpszCaption ? wcslen(lpmb->lpszCaption) : 0) +
        (lpmb->lpszText ? 2 * wcslen(lpmb->lpszText) : 0) +
        4 * sizeof(szLine) +
        lpmb->cButtons * mbm.wMaxBtnSize +
        8;

    cBufSize *= sizeof(WCHAR);

    if (!(hData = GlobalAlloc(LHND, (LONG)(cBufSize)))) {
        goto CloseClip;
    }

    USERGLOBALLOCK(hData, lpszAll);

    cWrote = wsprintf(lpszAll, L"%s%s\r\n%s",
        szLine,
        lpmb->lpszCaption ? lpmb->lpszCaption : L"",
        szLine);

    lpszWrite = lpszAll + cWrote;
    lpszRead = lpmb->lpszText;
    /*
     * Change \n to \r\n in the text
     */
    for (i = 0; *lpszRead; i++) {

        if (*lpszRead == L'\n')
            *lpszWrite++ = L'\r';

        *lpszWrite++ = *lpszRead++;
    }

    cWrote = wsprintf(lpszWrite, L"\r\n%s", szLine);
    lpszWrite += cWrote;

    /*
     * Remove & from the button texts
     */
    for (i = 0; i < lpmb->cButtons; i++) {

        lpszRead = lpmb->ppszButtonText[i];
        while (*lpszRead) {
            if (*lpszRead != L'&') {
                *lpszWrite++ = *lpszRead;
            }
            lpszRead++;
        }
        *lpszWrite++ = L' ';
        *lpszWrite++ = L' ';
        *lpszWrite++ = L' ';
    }
    wsprintf(lpszWrite, L"\r\n%s\0", szLine);

    USERGLOBALUNLOCK(hData);

    EmptyClipboard();

    SetClipboardData(CF_UNICODETEXT, hData);

CloseClip:
    CloseClipboard();

}

INT_PTR CALLBACK MB_DlgProc(
    HWND hwndDlg,
    UINT wMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    HWND hwndT;
    int iCount;
    LPMSGBOXDATA lpmb;
    HWND hwndOwner;
    PVOID lpfnCallback;
    BOOL bTimedOut = FALSE;

    switch (wMsg) {
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
        return DefWindowProcW(hwndDlg, WM_CTLCOLORMSGBOX,
            wParam, lParam);

    case WM_TIMER:
        if (!bTimedOut) {
            bTimedOut = TRUE;
            EndDialog(hwndDlg, IDTIMEOUT);
        }
        break;

    case WM_NCDESTROY:
        if ((lpmb = (LPMSGBOXDATA)GetWindowLongPtr(hwndDlg, GWLP_USERDATA))) {
            if (lpmb->dwTimeout != INFINITE) {
                KillTimer(hwndDlg, 0);
                lpmb->dwTimeout = INFINITE;
            }
        }
        return DefWindowProcW(hwndDlg, wMsg,
            wParam, lParam);


    case WM_INITDIALOG:
    {
        lpmb = (LPMSGBOXDATA)lParam;
        SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (ULONG_PTR)lParam);

        if (NtUserSetMsgBox)
            NtUserSetMsgBox(hwndDlg);
        else
            NtUserCallHwnd(hwndDlg, SFI_SETMSGBOX);

        if (lpmb->dwStyle & MB_TOPMOST) {
            SetWindowPos(hwndDlg,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE);
        }

        if (lpmb->dwStyle & MB_USERICON) {
            SendDlgItemMessage(hwndDlg, IDUSERICON, STM_SETICON, (WPARAM)(lpmb->lpszIcon), 0);
            iCount = ALERT_SYSTEM_WARNING;
        }
        else {
            /*
             * Generate an alert notification
             */
            switch (lpmb->dwStyle & MB_ICONMASK) {
            case MB_ICONWARNING:
                iCount = ALERT_SYSTEM_WARNING;
                break;

            case MB_ICONQUESTION:
                iCount = ALERT_SYSTEM_QUERY;
                break;

            case MB_ICONERROR:
                iCount = ALERT_SYSTEM_ERROR;
                break;

            case MB_ICONINFORMATION:
            default:
                iCount = ALERT_SYSTEM_INFORMATIONAL;
                break;
            }
        }

        NotifyWinEvent(EVENT_SYSTEM_ALERT, hwndDlg, OBJID_ALERT, iCount);

        hwndT = GetWindow(hwndDlg, GW_CHILD);
        iCount = lpmb->DefButton;
        while (iCount--)
            hwndT = GetWindow(hwndT, GW_HWNDNEXT);

        SetFocus(hwndT);

        hwndT = hwndDlg;

        if (lpmb->CancelId == 0) {
            HMENU hMenu;

            if (hMenu = GetSystemMenu(hwndDlg, FALSE)) {
                DeleteMenu(hMenu, SC_CLOSE, (UINT)MF_BYCOMMAND);
            }
        }

        if ((lpmb->dwStyle & MB_TYPEMASK) == MB_OK) {
            hwndDlg = GetDlgItem(hwndDlg, IDOK);

            if (hwndDlg != NULL) {
                SetWindowLongPtr(hwndDlg, GWLP_ID, IDCANCEL);
            }
        }

        if (lpmb->dwTimeout != INFINITE) {
            if (SetTimer(hwndT, 0, lpmb->dwTimeout, NULL) == 0) {
                /*
                 * Couldn't create the timer, so "clear" out the timeout value
                 * for future reference.
                 */
                lpmb->dwTimeout = INFINITE;
            }
        }

        // Hack because XP's positioning code is broken on 10
        // This centers it properly
        HMONITOR hm = MonitorFromWindow(hwndT, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(hm, &mi);
        RECT rc;
        GetWindowRect(hwndT, &rc);
        int x = (mi.rcWork.right - mi.rcWork.left) / 2;
        x -= (rc.right - rc.left) / 2;
        int y = (mi.rcWork.bottom - mi.rcWork.top) / 2;
        y -= (rc.bottom - rc.top) /2;

        // Tile effect
        int cntMBox = GetClientInfo()->pDeskInfo->cntMBox;
        x += cntMBox * SYSMET(CXSIZE);
        y += cntMBox * SYSMET(CYSIZE);

        // Make sure it stays in the screen
        if ((x + (rc.right - rc.left)) > mi.rcWork.right)
        {
            x = mi.rcWork.right - SYSMET(CXEDGE) - (rc.right - rc.left);
        }

        if (y + (rc.bottom - rc.top) > mi.rcWork.bottom)
        {
            y = mi.rcWork.bottom - SYSMET(CYEDGE) - (rc.bottom - rc.top);
            if (y < mi.rcWork.top)
            {
                y = mi.rcMonitor.bottom - SYSMET(CYEDGE) - (rc.bottom - rc.top);
            }
        }

        SetWindowPos(
            hwndT,
            NULL,
            x, y,
            0, 0,
            SWP_NOZORDER | SWP_NOSIZE
        );

        return FALSE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        case IDCANCEL:
            //
            // Check if a control exists with the given ID; This
            // check is needed because DlgManager returns IDCANCEL
            // blindly when ESC is pressed even if a button with
            // IDCANCEL is not present.
            //
            if (!GetDlgItem(hwndDlg, LOWORD(wParam)))
                return FALSE;


            // else FALL THRO....This is intentional.
        case IDABORT:
        case IDIGNORE:
        case IDNO:
        case IDRETRY:
        case IDYES:
        case IDTRYAGAIN:
        case IDCONTINUE:
            EndDialog(hwndDlg, LOWORD(wParam));
            break;

        default:
            return FALSE;
            break;
        }
        break;

    case WM_COPY:
        MB_CopyToClipboard(hwndDlg);
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

int SoftModalMessageBox_XP(
    LPMSGBOXDATA lpmb)
{
    LPBYTE              lpDlgTmp;
    int                 cyIcon, cxIcon;
    int                 cxButtons;
    int                 cxMBMax;
    int                 cxText, cyText, xText;
    int                 cxBox, cyBox;
    int                 cxFoo, cxCaption;
    int                 xMB, yMB;
    HDC                 hdc;
    DWORD               wIconOrdNum;
    DWORD               wCaptionLen;
    DWORD               wTextLen;
    WORD                OrdNum[2];  // Must be an array or WORDs
    RECT                rc;
    RECT                rcWork;
    HCURSOR             hcurOld;
    DWORD               dwStyleMsg, dwStyleText;
    DWORD               dwExStyleMsg = 0;
    DWORD               dwStyleDlg;
    HWND                hwndOwner;
    LPWSTR              lpsz;
    int                 iRetVal = 0;
    HICON               hIcon;
    HGLOBAL             hTemplate = NULL;
    HGLOBAL             hCaption = NULL;
    HGLOBAL             hText = NULL;
    HINSTANCE           hInstMsg = lpmb->hInstance;
    SIZE                size;
    HFONT               hFontOld = NULL;
    int                 cntMBox;
    HMONITOR            hMonitor;
    MONITORINFO         mi;

    GetMessageBoxMetrics(&mbm, lpmb->ppszButtonText, lpmb->cButtons);

    dwStyleMsg = lpmb->dwStyle;

    if (dwStyleMsg & MB_RIGHT) {
        dwExStyleMsg |= WS_EX_RIGHT;
    }

    if (!IS_PTR(lpmb->lpszCaption)) {
        /*
         * Won't ever be NULL because MessageBox sticks "Error!" on error.
         */
        if (hInstMsg && (hCaption = LocalAlloc(LPTR, MAX_RES_STRING * sizeof(WCHAR)))) {
            lpsz = (LPWSTR)hCaption;
            LoadString(hInstMsg, PTR_TO_ID(lpmb->lpszCaption), lpsz, MAX_RES_STRING);
        }
        else {
            lpsz = NULL;
        }

        lpmb->lpszCaption = lpsz ? lpsz : szEmpty;
    }

    if (!IS_PTR(lpmb->lpszText)) {
        // NULL not allowed
        if (hInstMsg && (hText = LocalAlloc(LPTR, MAX_RES_STRING * sizeof(WCHAR)))) {
            lpsz = (LPWSTR)hText;
            LoadString(hInstMsg, PTR_TO_ID(lpmb->lpszText), lpsz, MAX_RES_STRING);
        }
        else {
            lpsz = NULL;
        }

        lpmb->lpszText = lpsz ? lpsz : szEmpty;
    }

    if ((dwStyleMsg & MB_RTLREADING) ||
        (lpmb->lpszText != NULL && (lpmb->lpszText[0] == UNICODE_RLM) &&
            (lpmb->lpszText[1] == UNICODE_RLM))) {
        //
        // Set Mirroring so that MessageBox and its child controls
        // get mirrored. Otherwise, the message box and its child controls
        // are Left-To-Right.
        //
        dwExStyleMsg |= WS_EX_LAYOUTRTL;

        //
        // And turn off any conflicting flags.
        //
        dwExStyleMsg &= ~WS_EX_RIGHT;
        if (dwStyleMsg & MB_RTLREADING) {
            dwStyleMsg &= ~MB_RTLREADING;
            dwStyleMsg ^= MB_RIGHT;
        }
    }

    if ((dwStyleMsg & MB_ICONMASK) == MB_USERICON)
        hIcon = LoadIcon(hInstMsg, lpmb->lpszIcon);
    else
        hIcon = NULL;

    // For compatibility reasons, we still allow the message box to come up.
    hwndOwner = lpmb->hwndOwner;

getthedc:
    // Check if we're out of cache DCs until robustness...
    if (!(hdc = GetDCEx(NULL, NULL, DCX_WINDOW | DCX_CACHE))) {

        /*
         * The above call might fail for TIF_RESTRICTED processes
         * so check for the DC from the owner window
         */
        if (!(hdc = GetDCEx(hwndOwner, NULL, DCX_WINDOW | DCX_CACHE)))
            goto SMB_Exit;
    }

    // Figure out the types and dimensions of buttons
    cxButtons = (lpmb->cButtons * mbm.wMaxBtnSize) + ((lpmb->cButtons - 1) * XPixFromXDU(DU_BTNGAP, mbm.cxMsgFontChar));

    // Ditto for the icon, if there is one.  If not, cxIcon & cyIcon are 0.

    if (wIconOrdNum = MB_GetIconOrdNum(dwStyleMsg)) {
        cxIcon = SYSMET(CXICON) + XPixFromXDU(DU_INNERMARGIN, mbm.cxMsgFontChar);
        cyIcon = SYSMET(CYICON);
    }
    else
        cxIcon = cyIcon = 0;

    hFontOld = (HFONT)SelectObject(hdc, mbm.hCaptionFont);

    // Find the max between the caption text and the buttons
    wCaptionLen = wcslen(lpmb->lpszCaption);
    GetTextExtentPointW(hdc, lpmb->lpszCaption, wCaptionLen, &size);
    cxCaption = size.cx + 2 * SYSMET(CXSIZE);

    //
    // The max width of the message box is 5/8 of the work area for most
    // countries.  We will then try 6/8 and 7/8 if it won't fit.  Then
    // we will use whole screen.
    //
    hMonitor = MonitorFromWindow(hwndOwner, MONITOR_DEFAULTTOPRIMARY);
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(hMonitor, &mi);
    CopyRect(&rcWork, &mi.rcWork);
    cxMBMax = MulDiv(rcWork.right - rcWork.left, 5, 8);

    cxFoo = 2 * XPixFromXDU(DU_OUTERMARGIN, mbm.cxMsgFontChar);

    SelectObject(hdc, mbm.hMessageFont);

    //
    // If the text doesn't fit in 5/8, try 7/8 of the screen
    //
ReSize:
    //
    // The message box is as big as needed to hold the caption/text/buttons,
    // but not bigger than the maximum width.
    //

    cxBox = cxMBMax - 2 * SYSMET(CXFIXEDFRAME);

    // Ask DrawText for the right cx and cy
    rc.left = 0;
    rc.top = 0;
    rc.right = cxBox - cxFoo - cxIcon;
    rc.bottom = rcWork.bottom - rcWork.top;
    cyText = DrawTextExW(hdc, (LPWSTR)lpmb->lpszText, -1, &rc,
        DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS |
        DT_NOPREFIX | DT_EXTERNALLEADING | DT_EDITCONTROL, NULL);
    //
    // Make sure we have enough width to hold the buttons, in addition to
    // the icon+text.  Always force the buttons.  If they don't fit, it's
    // because the working area is small.
    //
    //
    // The buttons are centered underneath the icon/text.
    //
    cxText = rc.right - rc.left + cxIcon + cxFoo;
    cxBox = min(cxBox, max(cxText, cxCaption));
    cxBox = max(cxBox, cxButtons + cxFoo);
    cxText = cxBox - cxFoo - cxIcon;

    //
    // Now we know the text width for sure.  Really calculate how high the
    // text will be.
    //
    rc.left = 0;
    rc.top = 0;
    rc.right = cxText;
    rc.bottom = rcWork.bottom - rcWork.top;
    cyText = DrawTextExW(hdc, (LPWSTR)lpmb->lpszText, -1, &rc, DT_CALCRECT | DT_WORDBREAK
        | DT_EXPANDTABS | DT_NOPREFIX | DT_EXTERNALLEADING | DT_EDITCONTROL, NULL);

    // Find the window size.
    cxBox += 2 * SYSMET(CXFIXEDFRAME);
    cyBox = 2 * SYSMET(CYFIXEDFRAME) + SYSMET(CYCAPTION) + YPixFromYDU(2 * DU_OUTERMARGIN +
        DU_INNERMARGIN + DU_BTNHEIGHT, mbm.cyMsgFontChar);

    cyBox += max(cyIcon, cyText);

    //
    // If the message box doesn't fit on the working area, we'll try wider
    // sizes successively:  6/8 of work then 7/8 of screen.
    //
    if (cyBox > rcWork.bottom - rcWork.top) {
        int cxTemp;

        cxTemp = MulDiv(rcWork.right - rcWork.left, 6, 8);

        if (cxMBMax == MulDiv(rcWork.right - rcWork.left, 5, 8)) {
            cxMBMax = cxTemp;
            goto ReSize;
        }
        else if (cxMBMax == cxTemp) {
            // then let's try with rcMonitor
            CopyRect(&rcWork, &mi.rcMonitor);
            cxMBMax = MulDiv(rcWork.right - rcWork.left, 7, 8);
            goto ReSize;
        }
    }

    if (hFontOld) {
        SelectObject(hdc, hFontOld);
    }
    ReleaseDC(NULL, hdc);

    // Find the window position
    cntMBox = GetClientInfo()->pDeskInfo->cntMBox;

    xMB = (rcWork.left + rcWork.right - cxBox) / 2 + (cntMBox * SYSMET(CXSIZE));
    xMB = max(xMB, rcWork.left);
    yMB = (rcWork.top + rcWork.bottom - cyBox) / 2 + (cntMBox * SYSMET(CYSIZE));
    yMB = max(yMB, rcWork.top);

    //
    // Bottom, right justify if we're going off the screen--but leave a
    // little gap.
    //
    if (xMB + cxBox > rcWork.right) {
        xMB = rcWork.right - SYSMET(CXEDGE) - cxBox;
    }

    //
    // Pin to the working area.  If it won't fit, then pin to the screen
    // height.  Bottom justify it at least if too big even for that, so
    // that the buttons are visible.
    //
    if (yMB + cyBox > rcWork.bottom) {
        yMB = rcWork.bottom - SYSMET(CYEDGE) - cyBox;
        if (yMB < rcWork.top) {
            yMB = mi.rcMonitor.bottom - SYSMET(CYEDGE) - cyBox;
        }
    }

    wTextLen = wcslen(lpmb->lpszText);

    // Find out the memory required for the Dlg template and try to alloc it
    hTemplate = LocalAlloc(LPTR, MB_FindDlgTemplateSize(lpmb));
    if (!hTemplate) {
        goto SMB_Exit;
    }

    lpDlgTmp = (LPBYTE)hTemplate;

    //
    // Setup the dialog style for the message box
    //
    dwStyleDlg = WS_POPUPWINDOW | WS_CAPTION | DS_ABSALIGN | DS_NOIDLEMSG |
        DS_SETFONT | DS_3DLOOK;

    if ((dwStyleMsg & MB_MODEMASK) == MB_SYSTEMMODAL) {
        dwStyleDlg |= DS_SYSMODAL | DS_SETFOREGROUND;
    }
    else {
        dwStyleDlg |= DS_MODALFRAME | WS_SYSMENU;
    }

    if (dwStyleMsg & MB_SETFOREGROUND) {
        dwStyleDlg |= DS_SETFOREGROUND;
    }

    // Add the Header of the Dlg Template
    // BOGUS !!!  don't ADD bools
    lpDlgTmp = MB_UpdateDlgHdr((LPDLGTEMPLATE)lpDlgTmp, dwStyleDlg, dwExStyleMsg,
        (BYTE)(lpmb->cButtons + (wIconOrdNum != 0) + (lpmb->lpszText != NULL)),
        xMB, yMB, cxBox, cyBox, (LPWSTR)lpmb->lpszCaption, wCaptionLen);

    //
    // Center the buttons
    //

    cxFoo = (cxBox - 2 * SYSMET(CXFIXEDFRAME) - cxButtons) / 2;

    lpDlgTmp = MB_AddPushButtons((LPDLGITEMTEMPLATE)lpDlgTmp, lpmb, cxFoo,
        cyBox - SYSMET(CYCAPTION) - (2 * SYSMET(CYFIXEDFRAME)) -
        YPixFromYDU(DU_OUTERMARGIN, mbm.cyMsgFontChar));

    // Add Icon, if any, to the Dlg template
    //
    // The icon is always top justified.  If the text is shorter than the
    // height of the icon, we center it.  Otherwise the text will start at
    // the top.
    //
    if (wIconOrdNum) {
        OrdNum[0] = 0xFFFF;  // To indicate that an Ordinal number follows
        OrdNum[1] = (WORD)wIconOrdNum;

        lpDlgTmp = MB_UpdateDlgItem((LPDLGITEMTEMPLATE)lpDlgTmp, IDUSERICON,        // Control Id
            SS_ICON | WS_GROUP | WS_CHILD | WS_VISIBLE, 0,
            XPixFromXDU(DU_OUTERMARGIN, mbm.cxMsgFontChar),   // X co-ordinate
            YPixFromYDU(DU_OUTERMARGIN, mbm.cyMsgFontChar),   // Y co-ordinate
            0, 0,          // For Icons, CX and CY are ignored, can be zero
            (LPWSTR)OrdNum,    // Ordinal number of Icon
            ARRAYSIZE(OrdNum), // Length of OrdNum
            STATICCODE);
    }

    // Add the Text of the Message to the Dlg Template
    if (lpmb->lpszText) {
        //
        // Center the text if shorter than the icon.
        //
        if (cyText >= cyIcon)
            cxFoo = 0;
        else
            cxFoo = (cyIcon - cyText) / 2;

        dwStyleText = SS_NOPREFIX | WS_GROUP | WS_CHILD | WS_VISIBLE | SS_EDITCONTROL;
        if (dwStyleMsg & MB_RIGHT) {
            dwStyleText |= SS_RIGHT;
            xText = cxBox - (SYSMET(CXSIZE) + cxText);
        }
        else {
            dwStyleText |= SS_LEFT;
            xText = cxIcon + XPixFromXDU(DU_INNERMARGIN, mbm.cxMsgFontChar);
        }

        MB_UpdateDlgItem((LPDLGITEMTEMPLATE)lpDlgTmp, -1, dwStyleText, dwExStyleMsg, xText,
            YPixFromYDU(DU_OUTERMARGIN, mbm.cyMsgFontChar) + cxFoo,
            cxText, cyText,
            (LPWSTR)lpmb->lpszText, wTextLen, STATICCODE);
    }

    // The dialog template is ready

    //
    // Set the normal cursor
    //
    hcurOld = SetCursor(LoadCursorW(NULL, IDC_ARROW));

    lpmb->lpszIcon = (LPWSTR)hIcon;

    if (!(lpmb->dwStyle & MB_USERICON))
    {
        int wBeep = LOWORD(lpmb->dwStyle & MB_ICONMASK);
        if (NtUserMessageBeep)
            NtUserMessageBeep(wBeep);
        else
            NtUserCallOneParam(wBeep, SFI_PLAYEVENTSOUND);
    }

    iRetVal = (int)DialogBoxIndirectParamW(g_hUser32,
        (LPCDLGTEMPLATEW)hTemplate, hwndOwner,
        MB_DlgProc, (LPARAM)lpmb);

    //
    // Fix up return value
    if (iRetVal == -1)
        iRetVal = 0;                /* Messagebox should also return error */

    //
    // If the messagebox contains only OK button, then its ID is changed as
    // IDCANCEL in MB_DlgProc; So, we must change it back to IDOK irrespective
    // of whether ESC is pressed or Carriage return is pressed;
    //
    if (((dwStyleMsg & MB_TYPEMASK) == MB_OK) && iRetVal)
        iRetVal = IDOK;


    //
    // Restore the previous cursor
    //
    if (hcurOld)
        SetCursor(hcurOld);

SMB_Exit:
    if (hTemplate)
    {
        LocalFree(hTemplate);
    }

    if (hCaption)
    {
        LocalFree(hCaption);
    }

    if (hText)
    {
        LocalFree(hText);
    }

    if (mbm.hCaptionFont)
    {
        DeleteObject(mbm.hCaptionFont);
    }

    if (mbm.hMessageFont)
    {
        DeleteObject(mbm.hMessageFont);
    }

    return iRetVal;
}

int (WINAPI *SoftModalMessageBox_orig)(LPMSGBOXDATA);
int WINAPI SoftModalMessageBox_hook(LPMSGBOXDATA lpmb)
{
    if (g_mbStyle < MBS_VISTA)
    {
        return SoftModalMessageBox_XP(lpmb);
    }

    if (g_mbStyle == MBS_VISTA
    && lpmb
    && (lpmb->dwStyle & MB_ICONMASK) == 0)
    {
        if (NtUserMessageBeep)
            NtUserMessageBeep(0);
        else
            NtUserCallOneParam(0, SFI_PLAYEVENTSOUND);
    }
    return SoftModalMessageBox_orig(lpmb);
}

#pragma endregion // "Classic style"

void LoadSettings(void)
{
    LPCWSTR pszStyle = Wh_GetStringSetting(L"style");
    if (0 == wcscmp(pszStyle, L"nt4"))
    {
        g_mbStyle = MBS_NT4;
    }
    else if (0 == wcscmp(pszStyle, L"xp"))
    {
        g_mbStyle = MBS_XP;
    }
    else if (0 == wcscmp(pszStyle, L"vista"))
    {
        g_mbStyle = MBS_VISTA;
    }
    else
    {
        g_mbStyle = MBS_SEVEN;
    }
    Wh_FreeStringSetting(pszStyle);
}

const WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
    {
        {
            L"struct HFONT__ * "
#ifdef _WIN64
            L"__cdecl"
#else
            L"__stdcall"
#endif
            L" GetMessageBoxFontForDpi(unsigned int)"
        },
        &GetMessageBoxFontForDpi_orig,
        GetMessageBoxFontForDpi_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    LoadSettings();

    // Load undocumented functions for playing message box sound and incrementing message box count
    HMODULE hWin32U = GetModuleHandleW(L"win32u.dll");
    if (!hWin32U)
    {
        Wh_Log(L"Failed to get handle to win32u.dll");
        return FALSE;
    }
    NtUserCallOneParam = (NtUserCallOneParam_t)GetProcAddress(hWin32U, "NtUserCallOneParam");
    NtUserMessageBeep = (NtUserMessageBeep_t)GetProcAddress(hWin32U, "NtUserMessageBeep");
    if (!NtUserCallOneParam && !NtUserMessageBeep)
    {
        Wh_Log(L"Failed to find NtUserCallOneParam or NtUserMessageBeep in win32u.dll");
        return FALSE;
    }

    NtUserCallHwnd = (NtUserCallHwnd_t)GetProcAddress(hWin32U, "NtUserCallHwnd");
    NtUserSetMsgBox = (NtUserSetMsgBox_t)GetProcAddress(hWin32U, "NtUserSetMsgBox");
    if (!NtUserCallHwnd && !NtUserSetMsgBox)
    {
        Wh_Log(L"Failed to find NtUserCallHwnd or NtUserSetMsgBox in win32u.dll");
        return FALSE;
    }

    g_hUser32 = GetModuleHandleW(L"user32.dll");
    void *SoftModalMessageBox = (void *)GetProcAddress(g_hUser32, "SoftModalMessageBox");
    if (!SoftModalMessageBox)
    {
        Wh_Log(L"Failed to find SoftModalMessageBox in user32.dll");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        SoftModalMessageBox,
        (void *)SoftModalMessageBox_hook,
        (void **)&SoftModalMessageBox_orig
    ))
    {
        Wh_Log(L"Failed to hook SoftModalMessageBox in user32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        g_hUser32,
        user32DllHooks,
        ARRAYSIZE(user32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook GetMessageBoxFontForDpi in user32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
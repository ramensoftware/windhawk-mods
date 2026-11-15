// ==WindhawkMod==
// @id              win95-nt4-menubars
// @name            Windows 95/NT 4.0 Menubars
// @description     Restores the appearance of menubars from Windows 95/NT 4.0
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 95/NT 4.0 Menubars
This mod restores the appearance of menubars from Windows 95/NT 4.0, in which
they had no hover effect, and changed to the selection color while selected.

This mod will only work with the classic theme.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win95-nt4-menubars-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win95-nt4-menubars-after.png)
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <windhawk_utils.h>

/* User API hook messages */
#define WM_UAHDESTROYWINDOW    0x0090
#define WM_UAHDRAWMENU         0x0091
#define WM_UAHDRAWMENUITEM     0x0092
#define WM_UAHINITMENU         0x0093
#define WM_UAHMEASUREMENUITEM  0x0094
#define WM_UAHNCPAINTMENUPOPUP 0x0095

/*
 * Internal menu flags stored in pMenu->fFlags.
 * High order bits are used for public MNS_ flags defined in winuser.w
 */
#define MFISPOPUP               0x00000001
#define MFMULTIROW              0x00000002
#define MFUNDERLINE             0x00000004
#define MFWINDOWDC              0x00000008  /* Window DC vs Client area DC when drawing*/
#define MFINACTIVE              0x00000010
#define MFRTL                   0x00000020
#define MFDESKTOP               0x00000040 /* Set on the desktop menu AND its submenus */
#define MFSYSMENU               0x00000080 /* Set on desktop menu but NOT on its submenus */
#define MFAPPSYSMENU            0x00000100 /* Set on (sub)menu we return to the app via GetSystemMenu */
#define MFREADONLY              0x00000200 /* Menu cannot be modified */
#define MFLAST                  0x00000200

// describes the sizes of the menu bar or menu item
typedef union tagUAHMENUITEMMETRICS
{
	// cx appears to be 14 / 0xE less than rcItem's width!
	// cy 0x14 seems stable, i wonder if it is 4 less than rcItem's height which is always 24 atm
	SIZE rgsizeBar[2];
	SIZE rgsizePopup[4];
} UAHMENUITEMMETRICS;

// not really used in our case but part of the other structures
typedef struct tagUAHMENUPOPUPMETRICS
{
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2; // from kernel symbols, padded to full dword
} UAHMENUPOPUPMETRICS;

// hmenu is the main window menu; hdc is the context to draw in
typedef struct tagUAHMENU
{
	HMENU hmenu;
	HDC hdc;
	DWORD dwFlags; // no idea what these mean, in my testing it's either 0x00000a00 or sometimes 0x00000a10
} UAHMENU;

// menu items are always referred to by iPosition here
typedef struct tagUAHMENUITEM
{
	int iPosition; // 0-based position of menu item in menubar
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
} UAHMENUITEM;

// the DRAWITEMSTRUCT contains the states of the menu items, as well as
// the position index of the item in the menu, which is duplicated in
// the UAHMENUITEM's iPosition as well
typedef struct UAHDRAWMENUITEM
{
	DRAWITEMSTRUCT dis; // itemID looks uninitialized
	UAHMENU um;
	UAHMENUITEM umi;
} UAHDRAWMENUITEM;

// the MEASUREITEMSTRUCT is intended to be filled with the size of the item
// height appears to be ignored, but width can be modified
typedef struct tagUAHMEASUREMENUITEM
{
	MEASUREITEMSTRUCT mis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHMEASUREMENUITEM;

HFONT GetMenuFont(HDC hdc)
{
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, FALSE, dpi);
    return CreateFontIndirectW(&ncm.lfMenuFont);
}

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

bool UAHMenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plr)
{
    // Prevent NT4 menu drawing when UXTheme is enabled
    if (uMsg >= WM_UAHDRAWMENU && uMsg <= WM_UAHMEASUREMENUITEM)
    {
        HTHEME hTheme = OpenThemeData(NULL, L"Menu");
        if (hTheme)
        {
            CloseThemeData(hTheme);
            return false;
        }
    }

    switch (uMsg)
    {
        case WM_UAHINITMENU:
        {
            // Don't use UAH drawing for popups
            UAHMENU *pMenu = (UAHMENU *)lParam;
            *plr = !(pMenu->dwFlags & MFISPOPUP);
            return true;
        }
        case WM_UAHDRAWMENU:
        {
            UAHMENU *pMenu = (UAHMENU *)lParam;

            RECT rc = { 0 };
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

            RECT rcWindow;
            GetWindowRect(hWnd, &rcWindow);

            rc = mbi.rcBar;
            OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

            FillRect(pMenu->hdc, &rc, (HBRUSH)(COLOR_3DFACE + 1));
            return true;
        }
        case WM_UAHMEASUREMENUITEM:
        {
            UAHMEASUREMENUITEM *pMeasure = (UAHMEASUREMENUITEM *)lParam;
            HDC hdc = GetDC(hWnd);
            int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            HFONT hfMenu = GetMenuFont(hdc);
            HFONT hfOld = (HFONT)SelectObject(hdc, hfMenu);

            // Get menu item text
            MENUITEMINFOW mii = { sizeof(mii) };
            mii.fMask = MIIM_STRING;
            WCHAR szMenuItemText[MAX_PATH];
            // dwTypeData is a misleading member name; it is actually an LPWSTR
            mii.dwTypeData = szMenuItemText;
            mii.cch = ARRAYSIZE(szMenuItemText);
            GetMenuItemInfoW(pMeasure->um.hmenu, pMeasure->umi.iPosition, TRUE, &mii);

            // Calculate item size
            TEXTMETRICW tm;
            int cyMenuFontChar;
            GetCharDimensions(hdc, &tm, &cyMenuFontChar);
            SIZE sz = { 0, 0 };
            GetTextExtentPoint32W(hdc, mii.dwTypeData, wcslen(mii.dwTypeData), &sz);
            pMeasure->mis.itemWidth = sz.cx;
            pMeasure->mis.itemWidth += (((cyMenuFontChar + tm.tmExternalLeading + GetSystemMetricsForDpi(SM_CYBORDER, dpi)) & 0xFFFE) - 1) / 2;
            pMeasure->mis.itemHeight = GetSystemMetricsForDpi(SM_CYMENU, GetDeviceCaps(hdc, LOGPIXELSX)) - 1;

            *plr = TRUE;

            SelectObject(hdc, hfOld);
            DeleteObject(hfMenu);
            ReleaseDC(hWnd, hdc);
            return true;
        }
        case WM_UAHDRAWMENUITEM:
        {
            UAHDRAWMENUITEM *pDraw = (UAHDRAWMENUITEM *)lParam;

            // Get menu item text
            MENUITEMINFOW mii = { sizeof(mii) };
            mii.fMask = MIIM_STRING;
            WCHAR szMenuItemText[MAX_PATH];
            // dwTypeData is a misleading member name; it is actually an LPWSTR
            mii.dwTypeData = szMenuItemText;
            mii.cch = ARRAYSIZE(szMenuItemText);
            GetMenuItemInfoW(pDraw->um.hmenu, pDraw->umi.iPosition, TRUE, &mii);

            if (pDraw->dis.itemState & ODS_NOACCEL)
            {
                WCHAR *pchAmpersand = wcschr(mii.dwTypeData, L'&');
                if (pchAmpersand)
                {
                    wcscpy(pchAmpersand, pchAmpersand + 1);
                }
            }

            bool fDisabled = ((pDraw->dis.itemState & ODS_GRAYED) || (pDraw->dis.itemState & ODS_DISABLED));
            bool fSelected = !fDisabled && (pDraw->dis.itemState & ODS_SELECTED);
            int idBackground = fSelected ? COLOR_HIGHLIGHT : COLOR_MENU;
            HBRUSH hbrBackground = (HBRUSH)(idBackground + 1);
            COLORREF crText = GetSysColor(fSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT);

            HDC hdc = pDraw->um.hdc;
            HFONT hfMenu = GetMenuFont(hdc);
            HFONT hfOld = (HFONT)SelectObject(hdc, hfMenu);
            COLORREF crOld = SetTextColor(hdc, crText);
            int bkOld = SetBkMode(hdc, TRANSPARENT);

            FillRect(hdc, &pDraw->dis.rcItem, hbrBackground);
            if (fDisabled)
            {
                SetTextColor(hdc, GetSysColor(COLOR_3DHILIGHT));
                OffsetRect(&pDraw->dis.rcItem, 1, 1);
                DrawTextW(hdc, mii.dwTypeData, -1, &pDraw->dis.rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                OffsetRect(&pDraw->dis.rcItem, -1, -1);
                SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
                DrawTextW(hdc, mii.dwTypeData, -1, &pDraw->dis.rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            }
            else
            {
                DrawTextW(hdc, mii.dwTypeData, -1, &pDraw->dis.rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            }

            SetTextColor(hdc, crOld);
            SetBkMode(hdc, bkOld);
            SelectObject(hdc, hfOld);
            return true;
        }
        default:
            return false;
    }
}

WNDPROC DefWindowProcW_orig;
LRESULT CALLBACK DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;
    if (UAHMenuWndProc(hWnd, uMsg, wParam, lParam, &lr))
        return lr;
    return DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);
}

WNDPROC DefWindowProcA_orig;
LRESULT CALLBACK DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lr;
    if (UAHMenuWndProc(hWnd, uMsg, wParam, lParam, &lr))
        return lr;
    return DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit(void)
{
    if (!Wh_SetFunctionHook(
        (void *)DefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    ))
    {
        Wh_Log(L"Failed to hook DefWindowProcW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)DefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    ))
    {
        Wh_Log(L"Failed to hook DefWindowProcA");
        return FALSE;
    }


    return TRUE;
}
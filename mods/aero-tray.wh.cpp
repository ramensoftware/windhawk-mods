// ==WindhawkMod==
// @id              aero-tray
// @name            Aero Tray 
// @description     Restores Windows 7/8 tray overflow
// @version         1.0.2
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpsapi -lgdi32 -luxtheme -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aero Tray
This mod restores the tray overflow from Windows 7 and 8, including the "Customize..." link,
drawing tray icons with the system theme, and the general appearance.

# IMPORTANT: READ!
Windhawk needs to hook into `winlogon.exe` to successfully capture Explorer starting. Please
navigate to Windhawk's Settings, Advanced settings, More advanced settings, and make sure that
`winlogon.exe` is in the Process inclusion list.

## Notice
It is highly recommended you restart Explorer after making changes to the settings.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/aero-tray-before.png)

**After (Aero)**:

![After (Aero)](https://raw.githubusercontent.com/aubymori/images/main/aero-tray-after-aero.png)

**After (Aero Basic)**:

![After (Aero Basic)](https://raw.githubusercontent.com/aubymori/images/main/aero-tray-after-basic.png)

**After (Windows Classic)**:

![After (Windows Classic)](https://raw.githubusercontent.com/aubymori/images/main/aero-tray-after-classic.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- composition: default
  $name: Theme mode
  $description: Force the tray overflow to use a certain theme.
  $options:
  - default: Default
  - aero: Aero
  - basic: Aero Basic
  - classic: Windows Classic
- customizetext: Customize...
  $name: "\"Customize...\" link text"
- customizetooltip: Click here to select what icons appear on the taskbar.
  $name: "\"Customize...\" link tooltip"
*/
// ==/WindhawkModSettings==

#include <psapi.h>
#include <vsstyle.h>
#include <uxtheme.h>
#include <windowsx.h>
#include <windhawk_utils.h>

const UINT LINK_AREA_HEIGHT = 43;
const UINT LINK_AREA_PADDING = 16;

const UINT ICON_AREA_PADDING = 7;
const UINT ICON_PADDING = 18;

const UINT FLYOUT_OFFSET = 8;

#define RECTWIDTH(rect)  ((rect).right - (rect).left)
#define RECTHEIGHT(rect) ((rect).bottom - (rect).top)

typedef enum
{
    TM_DEFAULT = 0,
    TM_AERO,
    TM_BASIC,
    TM_CLASSIC
} THEMEMODE;

struct
{
    THEMEMODE                    thememode;
    WindhawkUtils::StringSetting customizetext;
    WindhawkUtils::StringSetting customizetooltip;
} settings;

HTHEME  g_hTheme              =  NULL;
HWND    g_hCustomizeLink      =  NULL;
HWND    g_hCustomizeTooltip   =  NULL;
BOOL    g_bCustomizeHovered   = FALSE;
BOOL    g_bCustomizeDown      = FALSE;
BOOL    g_bCustomizeCapturing = FALSE;
HCURSOR g_hcArrow             =  NULL;
HCURSOR g_hcHand              =  NULL;

bool UseComposition(void)
{
    switch (settings.thememode)
    {
        case TM_DEFAULT:
            return IsCompositionActive();
        case TM_AERO:
            return true;
        case TM_BASIC:
        case TM_CLASSIC:
            return false;
    }
}

/**
  * Get the menu font (for classic theme).
  * This creates a new GDI object, so be sure to destroy it once you're done!
  */
HFONT GetMenuFont(bool underline = false)
{
    NONCLIENTMETRICSW ncm = { sizeof(NONCLIENTMETRICSW) };
    if (SystemParametersInfoW(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        NULL
    ))
    {
        if (underline)
        {
            ncm.lfMenuFont.lfUnderline = TRUE;
        }
        return CreateFontIndirectW(&ncm.lfMenuFont);
    }
    return NULL;
}

/* Get the tray chevron. Needed for a hack. */
HWND GetTrayChevron(void)
{
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTaskbar)
    {
        HWND hNotify = FindWindowExW(hTaskbar, NULL, L"TrayNotifyWnd", NULL);
        if (hNotify)
        {
            return FindWindowExW(hNotify, NULL, L"Button", NULL);
        }
    }
    return NULL;
}

#define CTrayOverflow_Window(pThis) *((HWND *)pThis + 3)
#define CTrayOverflow_Toolbar(pThis) *((HWND *)pThis + 11)

/* Make the tray toolbar draw using the system theme */
LRESULT (* CTrayNotify__OnCDNotify_orig)(void *, HWND, LPNMTBCUSTOMDRAW);
LRESULT CTrayNotify__OnCDNotify_hook(
    void             *pThis,
    HWND              hWnd,
    LPNMTBCUSTOMDRAW  lpnmtbcd
)
{
    if (lpnmtbcd->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        return CDRF_NOTIFYITEMDRAW;
    }
    else if (lpnmtbcd->nmcd.dwDrawStage == 0x10001)
    {
        return TBCDRF_NOOFFSET;
    }
    return CDRF_DODEFAULT;
}

/* Apply appropriate border style */
void (* CTrayOverflow__EnsureBorder_orig)(void *, HWND);
void CTrayOverflow__EnsureBorder_hook(
    void *pThis,
    HWND  hWnd
)
{
    DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (UseComposition())
    {
        dwStyle |= (WS_THICKFRAME | WS_DLGFRAME);
    }
    else
    {
        dwStyle |= WS_BORDER;
    }
    SetWindowLongPtrW(hWnd, GWL_STYLE, dwStyle);
}

/* Adjust a window's position to be pushed away from the taskbar */
POINT AdjustWindowPosForTaskbar(HWND hWnd)
{
    HMONITOR hm = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    HDC hDC = GetDC(hWnd);
    int offset = MulDiv(FLYOUT_OFFSET, GetDeviceCaps(hDC, LOGPIXELSY), 96);

    RECT rc;
    GetWindowRect(hWnd, &rc);

    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfoW(hm, &mi);

    int dx = 0, dy = 0;
    PLONG plrc = (PLONG)&rc;
    PLONG plwrc = (PLONG)&mi.rcWork;
    for (int i = 0; i < 4; i++)
    {
        int curOffset = plwrc[i] - plrc[i];
        curOffset = (curOffset < 0) ? -curOffset : curOffset;

        if (curOffset < offset)
        {
            int *set = (i % 2 == 0) ? &dx : &dy;
            if (i > 1)
            {
                *set -= offset - curOffset;
            }
            else
            {
                *set += offset - curOffset;
            }
        }
    }
    return { rc.left + dx, rc.top + dy };
}

/* Add space between taskbar and tray overflow for Aero */
void (* CTrayOverflow__PositionWindow_orig)(void *);
void CTrayOverflow__PositionWindow_hook(
    void *pThis
)
{
    CTrayOverflow__PositionWindow_orig(pThis);

    HWND hWnd = CTrayOverflow_Window(pThis);
    if (hWnd && UseComposition())
    {
        POINT pt = AdjustWindowPosForTaskbar(hWnd);
        SetWindowPos(
            hWnd, NULL,
            pt.x, pt.y,
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER
        );
    }
}

/* To get item count */
__int64 (* CTrayItemManager__GetItemCountHelper)(void *pThis, int i1, int i2);

#define CTrayOverflow_CTrayItemManager(pThis) (void *)*((__int64 *)pThis + 5)

/* Add space for link area and add margin to icon area */
bool (* CTrayOverflow_SizeWindows_orig)(void *);
bool CTrayOverflow_SizeWindows_hook(
    void *pThis
)
{
    CTrayOverflow_SizeWindows_orig(pThis);

    HWND hWnd = CTrayOverflow_Window(pThis);
    HDC hDC = GetDC(hWnd);

    RECT rc;
    GetClientRect(hWnd, &rc);

    rc.bottom += MulDiv(LINK_AREA_HEIGHT, GetDeviceCaps(hDC, LOGPIXELSY), 96);

    HWND hToolbar = CTrayOverflow_Toolbar(pThis);
    if (hToolbar)
    {
        int nMargin = MulDiv(ICON_AREA_PADDING, GetDeviceCaps(hDC, LOGPIXELSY), 96);
        SetWindowPos(
            hToolbar, NULL,
            nMargin, nMargin,
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER
        );
        rc.right += nMargin * 2;
        rc.bottom += nMargin * 2;

        /**
          * Windows 7 and 8 always used 3 items per row on this.
          * However, Windows 10 will use two items per row whenever there is 3 or 4
          * items on it. The following code adjusts the sizes to make it always use
          * 3 items per row again.
          */
    
        void *ptim = CTrayOverflow_CTrayItemManager(pThis);
        if (ptim)
        {
            __int64 nItems = CTrayItemManager__GetItemCountHelper(
                ptim, 3, 0
            );
            if (nItems == 3 || nItems == 4)
            {
                int nLinkAreaHeight = MulDiv(LINK_AREA_HEIGHT, GetDeviceCaps(hDC, LOGPIXELSY), 96);
                int nItemSize = LOWORD(SendMessageW(hToolbar, TB_GETBUTTONSIZE, NULL, NULL));

                RECT rcToolbar = { 0 };
                switch (nItems)
                {
                    case 3:
                        rcToolbar.right = nItemSize * 3;
                        rcToolbar.bottom = nItemSize;
                        break;
                    case 4:
                        rcToolbar.right = nItemSize * 3;
                        rcToolbar.bottom = nItemSize * 2;
                        break;
                }

                SetWindowPos(
                    hToolbar, NULL,
                    0, 0,
                    RECTWIDTH(rcToolbar),
                    RECTHEIGHT(rcToolbar),
                    SWP_NOMOVE | SWP_NOZORDER
                );

                SetRectEmpty(&rc);
                rc.right = (nMargin * 2) + RECTWIDTH(rcToolbar);
                rc.bottom = (nMargin * 2) + RECTHEIGHT(rcToolbar) + nLinkAreaHeight;
            }
        }
    }

    if (g_hCustomizeLink)
    {
        RECT rcLink;
        GetWindowRect(g_hCustomizeLink, &rcLink);
        int nLinkMargin = MulDiv(LINK_AREA_PADDING, GetDeviceCaps(hDC, LOGPIXELSY), 96);
        int nLinkAreaHeight = MulDiv(LINK_AREA_HEIGHT, GetDeviceCaps(hDC, LOGPIXELSY), 96);

        if (RECTWIDTH(rc) < RECTWIDTH(rcLink) + (nLinkMargin * 2))
        {
            rc.right = RECTWIDTH(rcLink) + (nLinkMargin * 2);
        }

        SetWindowPos(
            g_hCustomizeLink,
            NULL,
            rc.left + ((RECTWIDTH(rc) / 2) - (RECTWIDTH(rcLink) / 2)),
            rc.top + (rc.bottom - nLinkAreaHeight) + ((nLinkAreaHeight / 2) - (RECTHEIGHT(rcLink) / 2)),
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER
        );

        if (hToolbar)
        {
            int nMargin = MulDiv(ICON_AREA_PADDING, GetDeviceCaps(hDC, LOGPIXELSY), 96);
            RECT rcToolbar;
            GetClientRect(hToolbar, &rcToolbar);
            if (RECTWIDTH(rc) - (nMargin * 2) > RECTWIDTH(rcToolbar))
            {
                SetWindowPos(
                    hToolbar, NULL,
                    rc.left + ((RECTWIDTH(rc) / 2) - (RECTWIDTH(rcToolbar) / 2)),
                    nMargin,
                    0, 0,
                    SWP_NOSIZE | SWP_NOZORDER
                );
            }
        }
    }

    AdjustWindowRect(
        &rc,
        GetWindowLongPtrW(hWnd, GWL_STYLE),
        FALSE
    );

    SetWindowPos(
        hWnd,
        NULL,
        0, 0,
        RECTWIDTH(rc),
        RECTHEIGHT(rc),
        SWP_NOMOVE | SWP_NOZORDER
    );

    ReleaseDC(hWnd, hDC);
    CTrayOverflow__PositionWindow_hook(pThis);
    return true;
}

/* Reduce tray button padding */
void (* CTrayOverflow_UpdateButtonMetrics_orig)(void *);
void CTrayOverflow_UpdateButtonMetrics_hook(
    void *pThis
)
{
    HWND hToolbar = CTrayOverflow_Toolbar(pThis);
    if (hToolbar)
    {
        SendMessageW(hToolbar, TB_SETPADDING, NULL, MAKELPARAM(ICON_PADDING, ICON_PADDING));
    }
}

/* Draw icon and link area backgrounds */
LRESULT (* CTrayOverflow__OnPaint_orig)(void *, HDC);
LRESULT CTrayOverflow__OnPaint_hook(
    void *pThis,
    HDC   hdcOut
)
{
    HWND hWnd = CTrayOverflow_Window(pThis);
    PAINTSTRUCT ps;
    HDC hDC;

    if (hdcOut)
    {
        hDC = hdcOut;
        if (!GetClipBox(hDC, &ps.rcPaint))
        {
            SetRectEmpty(&ps.rcPaint);
        }
        ps.fRestore = FALSE;
    }
    else
    {
        hDC = BeginPaint(hWnd, &ps);
        ps.fRestore = TRUE;
    }

    RECT rc = { 0 };
    GetClientRect(hWnd, &rc);

    int nAreaHeight = MulDiv(LINK_AREA_HEIGHT, GetDeviceCaps(hDC, LOGPIXELSY), 96);

    rc.top = rc.bottom - nAreaHeight;
    if (g_hTheme)
    {
        DrawThemeBackground(
            g_hTheme,
            hDC,
            FLYOUT_LINKAREA,
            0,
            &rc,
            NULL
        );
    }
    else
    {
        FillRect(
            hDC,
            &rc,
            GetSysColorBrush(COLOR_3DFACE)
        );
    }

    rc.bottom = rc.top;
    rc.top = 0;

    if (g_hTheme)
    {
        DrawThemeBackground(
            g_hTheme,
            hDC,
            6,
            0,
            &rc,
            NULL
        );
    }
    else
    {
        FillRect(
            hDC,
            &rc,
            GetSysColorBrush(COLOR_WINDOW)
        );
    }

    if (ps.fRestore)
    {
        EndPaint(hWnd, &ps);
    }

    return 0;
}

/* Prevent DWM composition attribute from being set */
void (* CTrayOverflow__UpdateColors_orig)(void *);
void CTrayOverflow__UpdateColors_hook(
    void *pThis
)
{
    return;
}

LRESULT PaintCustomizeLink(
    HWND hWnd,
    HDC  hDC
)
{
    BOOL bHover;

    POINT pt;
    GetCursorPos(&pt);
    RECT rc;
    GetWindowRect(hWnd, &rc);
    
    bHover = PtInRect(&rc, pt) || GetFocus() == hWnd;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    DTTOPTS opts = { sizeof(DTTOPTS) };
    if (IsCompositionActive())
    {
        opts.dwFlags = DTT_COMPOSITED;
    }

    RECT rcPaint;
    GetClipBox(hDC, &rcPaint);

    DrawThemeParentBackground(hWnd, hDC, NULL);

    if (g_hTheme)
    {
        int bkOld = SetBkMode(hDC, TRANSPARENT);
        DrawThemeTextEx(
            g_hTheme,
            hDC,
            FLYOUT_LINK,
            bHover ? FLYOUTLINK_HOVER : FLYOUTLINK_NORMAL,
            settings.customizetext.get(),
            -1,
            DT_CENTER,
            &rcClient,
            &opts
        );
        SetBkMode(hDC, bkOld);
    }
    else
    {
        HFONT hfMenu = GetMenuFont(bHover);
        HFONT hfOld = (HFONT)SelectObject(hDC, hfMenu);
        COLORREF crBkOld = SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
        COLORREF crOld = SetTextColor(hDC, GetSysColor(COLOR_HOTLIGHT));

        DrawTextW(
            hDC,
            settings.customizetext.get(),
            -1,
            &rcClient,
            NULL
        );

        SetTextColor(hDC, crOld);
        SetBkColor(hDC, crBkOld);
        SelectObject(hDC, hfOld);
        DeleteObject(hfMenu);
    }

    return 0;
}

LRESULT CALLBACK CustomizeLinkSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);
            LRESULT lr = PaintCustomizeLink(hWnd, hDC);
            EndPaint(hWnd, &ps);
            return lr;
        }
        case WM_PRINTCLIENT:
        {
            return PaintCustomizeLink(hWnd, (HDC)wParam);
        }
        /* Repaint and set cursor on hover */
        case WM_MOUSEMOVE:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            RECT rc;
            GetClientRect(hWnd, &rc);

            BOOL bHovered = g_bCustomizeHovered;
            if (IsWindowEnabled(hWnd) && PtInRect(&rc, pt))
            {
                if (!g_bCustomizeCapturing && SetCapture(hWnd))
                {
                    g_bCustomizeCapturing = TRUE;
                }
                g_bCustomizeHovered = TRUE;
            }
            else
            {
                if (g_bCustomizeCapturing && ReleaseCapture())
                {
                    g_bCustomizeCapturing = FALSE;
                }
                g_bCustomizeHovered = FALSE;
                g_bCustomizeDown = FALSE;
            }

            if (g_bCustomizeHovered != bHovered)
            {
                SetCursor(g_bCustomizeHovered ? g_hcHand : g_hcArrow);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        /* Unhover when it's hidden */
        case WM_SHOWWINDOW:
            if (!wParam && g_bCustomizeHovered)
            {
                g_bCustomizeHovered = FALSE;
                if (g_bCustomizeCapturing && ReleaseCapture())
                {
                    g_bCustomizeCapturing = FALSE;
                }
            }
            return 0;
        case WM_LBUTTONDOWN:
            g_bCustomizeDown = TRUE;
            break;
        /* Hide tray overflow and open Notification Area Icons CPL */
        case WM_LBUTTONUP:
            if (g_bCustomizeDown)
            {
                g_bCustomizeDown = FALSE;
                HWND hChevron = GetTrayChevron();
                if (hChevron)
                {
                    SendMessageW(hChevron, WM_LBUTTONUP, NULL, NULL);
                }
                ShellExecuteW(
                    NULL,
                    L"open",
                    L"explorer.exe",
                    L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{05D7B0F4-2121-4EFF-BF6B-ED3F69B894D9}",
                    NULL,
                    SW_SHOWNORMAL
                );
            }
            break;
        case WM_CAPTURECHANGED:
            g_bCustomizeCapturing = FALSE;
            g_bCustomizeHovered = FALSE;
            return 0;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LPCWSTR LINK_TEXT_FORMAT = L"<A id=\"customize\">%s</A>";

/* Create link window */
HRESULT (* CTrayOverflow_RuntimeClassInitialize_orig)(void *, HWND, void *, void *, void *);
HRESULT CTrayOverflow_RuntimeClassInitialize_hook(
    void *pThis,
    HWND  hUnused,
    void *ptoe,
    void *ptla,
    void *ptch
)
{
    HRESULT hr = CTrayOverflow_RuntimeClassInitialize_orig(
        pThis, hUnused, ptoe, ptla, ptch
    );
    if (SUCCEEDED(hr))
    {
        HWND hWnd = CTrayOverflow_Window(pThis);
        if (hWnd)
        {
            WCHAR szText[1024];
            wsprintfW(szText, LINK_TEXT_FORMAT, settings.customizetext.get());

            g_hCustomizeLink = CreateWindowExW(
                NULL,
                L"SysLink",
                szText,
                WS_CHILD | LWS_TRANSPARENT,
                0, 0,
                0, 0,
                hWnd,
                NULL,
                GetModuleHandleW(NULL),
                NULL
            );

            if (g_hCustomizeLink)
            {
                /* This gets kicked out or something?? Very odd. */
                SetParent(g_hCustomizeLink, hWnd);

                WindhawkUtils::SetWindowSubclassFromAnyThread(
                    g_hCustomizeLink, CustomizeLinkSubclassProc, NULL
                );

                SIZE sz;
                HDC hDC = GetDC(g_hCustomizeLink);
                if (g_hTheme)
                {
                    RECT rcTemp;
                    if (SUCCEEDED(GetThemeTextExtent(
                        g_hTheme,
                        hDC,
                        FLYOUT_LINK,
                        FLYOUTLINK_NORMAL,
                        settings.customizetext.get(),
                        -1,
                        NULL,
                        NULL,
                        &rcTemp
                    )))
                    {
                        sz.cx = RECTWIDTH(rcTemp);
                        sz.cy = RECTHEIGHT(rcTemp);
                    }
                }
                else
                {
                    HFONT hfMenu = GetMenuFont();
                    HFONT hfOld = (HFONT)SelectObject(hDC, hfMenu);
                    GetTextExtentPoint32W(
                        hDC,
                        settings.customizetext.get(),
                        wcslen(settings.customizetext.get()),
                        &sz
                    );
                    SelectObject(hDC, hfOld);
                    DeleteObject(hfMenu);
                }
                ReleaseDC(g_hCustomizeLink, hDC);

                SetWindowPos(
                    g_hCustomizeLink,
                    NULL,
                    0, 0,
                    sz.cx, sz.cy,
                    SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOZORDER
                );

                g_hCustomizeTooltip = CreateWindowExW(
                    WS_EX_TOPMOST,
                    TOOLTIPS_CLASSW,
                    NULL,
                    WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                    0, 0, 0, 0,
                    g_hCustomizeLink,
                    NULL, 0, NULL
                );

                if (g_hCustomizeTooltip)
                {
                    TTTOOLINFOW ti = { sizeof(TTTOOLINFOW) };
                    ti.uFlags = TTF_SUBCLASS;
                    ti.hwnd = g_hCustomizeLink;
                    ti.lpszText = (LPWSTR)settings.customizetooltip.get();
                    GetClientRect(g_hCustomizeLink, &ti.rect);

                    SendMessageW(g_hCustomizeTooltip, TTM_ADDTOOLW, NULL, (LPARAM)&ti);
                }
            }
        }
    }
    return hr;
}

/*
 * Patch StartIsBack in memory to prevent it from overriding standard explorer
 * behavior and thus breaking the mod.
 */
void PatchStartIsBack(HMODULE hModule)
{
    const wchar_t *pattern = L"NotifyIconOverflowWindow";

    MODULEINFO info = {0};
    GetModuleInformation(
        GetCurrentProcess(),
        hModule,
        &info,
        sizeof(MODULEINFO)
    );

    DWORD_PTR base = (size_t)info.lpBaseOfDll;
    size_t size = (size_t)info.SizeOfImage;
    size_t patternLen = wcslen(pattern) * 2;

    for (size_t i = 0; i < size - patternLen; i++)
    {
        bool found = true;

        for (size_t j = 0; j < patternLen; j++)
        {
            found = *((char *)pattern + j) == *(char *)(base + i + j);

            if (!found)
            {
                break;
            }
        }

        if (found)
        {
            size_t ptr = base + i;

            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery((wchar_t *)ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

            if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
            {
                return;
            }

            wcsncpy((wchar_t *)ptr, L"BYEBYEBYEBYEBYEBYEBYEBYE", patternLen);

            DWORD dwOldProtect;
            VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);

            return;
        }
    }
}

#define LoadIntSetting(NAME) settings.NAME = Wh_GetIntSetting(L ## #NAME)
#define LoadStringSetting(NAME) settings.NAME = WindhawkUtils::StringSetting::make(L ## #NAME)

void LoadSettings(void)
{
    LoadStringSetting(customizetext);
    LoadStringSetting(customizetooltip);

    LPCWSTR szThemeMode = Wh_GetStringSetting(L"composition");
    if (!wcscmp(szThemeMode, L"aero"))
    {
        settings.thememode = TM_AERO;
    }
    else if (!wcscmp(szThemeMode, L"basic"))
    {
        settings.thememode = TM_BASIC;
    }
    else if (!wcscmp(szThemeMode, L"classic"))
    {
        settings.thememode = TM_CLASSIC;
    }
    else
    {
        settings.thememode = TM_DEFAULT;
    }
    Wh_FreeStringSetting(szThemeMode);
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    /* Make the tray toolbar draw using the system theme */
    {
        {
            L"protected: __int64 __cdecl CTrayNotify::_OnCDNotify(struct HWND__ *,struct _NMTBCUSTOMDRAW *)"
        },
        &CTrayNotify__OnCDNotify_orig,
        CTrayNotify__OnCDNotify_hook,
        false
    },
    /* Apply appropriate border style */
    {
        {
            L"private: void __cdecl CTrayOverflow::_EnsureBorder(struct HWND__ *)"
        },
        &CTrayOverflow__EnsureBorder_orig,
        CTrayOverflow__EnsureBorder_hook,
        false
    },
    /* Add space between taskbar and tray overflow for Aero */
    {
        {
            L"private: void __cdecl CTrayOverflow::_PositionWindow(void)"
        },
        &CTrayOverflow__PositionWindow_orig,
        CTrayOverflow__PositionWindow_hook,
        false
    },
    {
        {
            L"private: __int64 __cdecl CTrayItemManager::_GetItemCountHelper(int,int)const "
        },
        &CTrayItemManager__GetItemCountHelper,
        nullptr,
        false
    },
    /* Add space for link area and add margin to icon area */
    {
        {
            L"public: bool __cdecl CTrayOverflow::SizeWindows(void)"
        },
        &CTrayOverflow_SizeWindows_orig,
        CTrayOverflow_SizeWindows_hook,
        false
    },
    /* Reduce tray button padding */
    {
        {
            L"private: void __cdecl CTrayOverflow::UpdateButtonMetrics(void)"
        },
        &CTrayOverflow_UpdateButtonMetrics_orig,
        CTrayOverflow_UpdateButtonMetrics_hook,
        false
    },
    /* Draw icon and link area backgrounds */
    {
        {
            L"private: __int64 __cdecl CTrayOverflow::_OnPaint(struct HDC__ *)"
        },
        &CTrayOverflow__OnPaint_orig,
        CTrayOverflow__OnPaint_hook,
        false
    },
    /* Prevent DWM composition attribute from being set */
    {
        {
            L"private: void __cdecl CTrayOverflow::_UpdateColors(void)"
        },
        &CTrayOverflow__UpdateColors_orig,
        CTrayOverflow__UpdateColors_hook,
        false
    },
    /* Create link window */
    {
        {
            L"public: long __cdecl CTrayOverflow::RuntimeClassInitialize(struct HWND__ *,class TrayOverflowEvents *,struct ITaskListAnimation *,struct ITrayComponentHost *)"
        },
        &CTrayOverflow_RuntimeClassInitialize_orig,
        CTrayOverflow_RuntimeClassInitialize_hook,
        false
    }
};

LPCWSTR SIB_PATH = L"%PROGRAMFILES(X86)%\\StartIsBack\\StartIsBack64.dll";
LPCWSTR SIB_PATH_USER = L"%LOCALAPPDATA%\\StartIsBack\\StartIsBack64.dll";

BOOL Wh_ModInit(void)
{
    LoadSettings();

    WCHAR szSIBPath[MAX_PATH];
    ExpandEnvironmentStringsW(SIB_PATH, szSIBPath, MAX_PATH);

    HMODULE hSib = LoadLibraryW(szSIBPath);
    if (!hSib)
    {
        WCHAR szUserSIBPath[MAX_PATH];
        ExpandEnvironmentStringsW(SIB_PATH_USER, szUserSIBPath, MAX_PATH);
        hSib = LoadLibraryW(szUserSIBPath);
    }

    if (hSib)
    {
        PatchStartIsBack(hSib);
    }

    if (settings.thememode != TM_CLASSIC)
    {
        g_hTheme = OpenThemeData(NULL, L"Flyout");
    }
    g_hcArrow = LoadCursorW(NULL, IDC_ARROW);
    g_hcHand = LoadCursorW(NULL, IDC_HAND);

    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }
    return TRUE;
}

BOOL Wh_ModSettingsChanged(PBOOL pbReload)
{
    *pbReload = TRUE;
    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_hCustomizeLink)
    {
        DestroyWindow(g_hCustomizeLink);
    }
}
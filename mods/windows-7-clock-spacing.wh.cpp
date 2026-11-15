// ==WindhawkMod==
// @id              windows-7-clock-spacing
// @name            Windows 7 Clock Spacing
// @description     Makes the taskbar clock in Windows 10 use the spacing from Windows 7
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Clock Spacing
Makes the taskbar clock in Windows 10 use Windows 7 sizing/spacing.

# IMPORTANT: READ!
Windhawk needs to hook into `winlogon.exe` to successfully capture Explorer starting. Please
navigate to Windhawk's Settings, Advanced settings, More advanced settings, and make sure that
`winlogon.exe` is in the Process inclusion list.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/windows-7-clock-spacing-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/windows-7-clock-spacing-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

const UINT CLOCK_VERTICAL_SPACING     =  2;
const UINT CLOCK_TEXT_VERTICAL_OFFSET =  1;
const UINT CLOCK_WIDTH                = 70;
const UINT CLOCK_LEFT_MARGIN          =  4;
const UINT CLOCK_RIGHT_MARGIN         =  2;

#define ClockButton_Window(pThis) *((HWND *)pThis + 1)

/* Reduce spacing between clock text */
int (*ClockButton_GetTextSpacingForOrientation_orig)(void *, bool, DWORD, DWORD, DWORD, DWORD);
int ClockButton_GetTextSpacingForOrientation_hook(
    void *pThis,
    bool  bHorizontal,
    DWORD dwSiteHeight,
    DWORD dwLine1Height,
    DWORD dwLine2Height,
    DWORD dwLine3Height
)
{
    if (dwLine2Height > 0 || dwLine3Height > 0)
    {
        HDC hDC = GetDC(ClockButton_Window(pThis));
        if (hDC)
        {
            int spacing = MulDiv(CLOCK_VERTICAL_SPACING, GetDeviceCaps(hDC, LOGPIXELSY), 96);
            ReleaseDC(ClockButton_Window(pThis), hDC);
            return spacing;
        }
    }

    return ClockButton_GetTextSpacingForOrientation_orig(
        pThis, bHorizontal, dwSiteHeight, dwLine1Height, dwLine2Height, dwLine3Height
    );
}

/* Increase minimum width */
LPSIZE (* ClockButton_CalculateMinimumSize_orig)(void *, LPSIZE);
LPSIZE ClockButton_CalculateMinimumSize_hook(
    void   *pThis,
    LPSIZE  lpUnused
)
{
    LPSIZE sz = ClockButton_CalculateMinimumSize_orig(
        pThis, lpUnused
    );
    if (sz)
    {
        HDC hDC = GetDC(ClockButton_Window(pThis));
        if (hDC)
        {
            sz->cx = MulDiv(CLOCK_WIDTH, GetDeviceCaps(hDC, LOGPIXELSX), 96);
            ReleaseDC(ClockButton_Window(pThis), hDC);
        }
    }
    return sz;
}

/* Draw clock text one pixel lower than 10 does, this is where 7 draws it */
void (* ClockButton_ClockButtonImage_DrawTextW_orig)(void *, HDC, LPCWSTR, int, int, int, int, int);
void ClockButton_ClockButtonImage_DrawTextW_hook(
    void    *pThis,
    HDC      hDC,
    LPCWSTR  pszText,
    int      x,
    int      y,
    int      cx,
    int      cy,
    int      iStateId
)
{
    ClockButton_ClockButtonImage_DrawTextW_orig(
        pThis, hDC, pszText, x, y + CLOCK_TEXT_VERTICAL_OFFSET, cx, cy + CLOCK_TEXT_VERTICAL_OFFSET, iStateId
    );
}

/* This is the big one; spacing between the clock and tray icons */

#define RECTWIDTH(rc) ((rc).right - (rc).left)
#define RECTHEIGHT(rc) ((rc).bottom - (rc).top)

bool IsTaskbarHorizontal(void)
{
    APPBARDATA abd = { sizeof(APPBARDATA) };
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    return (abd.uEdge == ABE_BOTTOM || abd.uEdge == ABE_TOP);
}

HWND g_hTrayNotify = NULL;

LRESULT CALLBACK TrayNotifySubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_WINDOWPOSCHANGING:
            if (IsTaskbarHorizontal())
            {
                LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

                HDC hDC = GetDC(hWnd);
                int offset = MulDiv(CLOCK_LEFT_MARGIN + CLOCK_RIGHT_MARGIN, GetDeviceCaps(hDC, LOGPIXELSX), 96);
                ReleaseDC(hWnd, hDC);

                lpwp->x -= offset;
                lpwp->cx += offset;
            }
            return 0;
        case WM_DESTROY:
            g_hTrayNotify = NULL;
    }

    return DefSubclassProc(
        hWnd, uMsg, wParam, lParam
    );
}

std::vector<HWND> g_hTaskLists;

LRESULT CALLBACK TaskListSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_WINDOWPOSCHANGING:
            if (IsTaskbarHorizontal())
            {
                LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;
                
                HDC hDC = GetDC(hWnd);
                int offset = MulDiv(CLOCK_LEFT_MARGIN + CLOCK_RIGHT_MARGIN, GetDeviceCaps(hDC, LOGPIXELSX), 96);
                ReleaseDC(hWnd, hDC);

                lpwp->cx -= offset;
            }
            return 0;
        case WM_DESTROY:
            g_hTaskLists.erase(std::remove_if(
                g_hTaskLists.begin(),
                g_hTaskLists.end(),
                [hWnd](HWND hw)
                {
                    return hw == hWnd;
                }
            ));
    }

    return DefSubclassProc(
        hWnd, uMsg, wParam, lParam
    );
}

std::vector<HWND> g_hTrayNotifyChildren;

LRESULT CALLBACK TrayNotifyChildSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_WINDOWPOSCHANGING:
            if (IsTaskbarHorizontal())
            {
                LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

                HDC hDC = GetDC(hWnd);
                int offset = MulDiv(CLOCK_LEFT_MARGIN, GetDeviceCaps(hDC, LOGPIXELSX), 96);
                if (!dwRefData)
                {
                    offset += MulDiv(CLOCK_RIGHT_MARGIN, GetDeviceCaps(hDC, LOGPIXELSX), 96);
                }
                ReleaseDC(hWnd, hDC);

                lpwp->x += offset;
            }
            return 0;
        case WM_DESTROY:
            g_hTrayNotifyChildren.erase(std::remove_if(
                g_hTrayNotifyChildren.begin(),
                g_hTrayNotifyChildren.end(),
                [hWnd](HWND hw)
                {
                    return hw == hWnd;
                }
            ));
            break;
    }

    return DefSubclassProc(
        hWnd, uMsg, wParam, lParam
    );
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    HWND hWnd = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent,
        hMenu, hInstance, lpParam
    );

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0))
    {
        if (0 == wcscmp(lpClassName, L"TrayNotifyWnd"))
        {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifySubclassProc, NULL))
            {
                g_hTrayNotify = hWnd;
            }
        }
        else if (0 == wcscmp(lpClassName, L"TrayClockWClass")
        || 0 == wcscmp(lpClassName, L"TrayButton")
        || 0 == wcscmp(lpClassName, L"TrayShowDesktopButtonWClass"))
        {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TrayNotifyChildSubclassProc, (0 == wcscmp(lpClassName, L"TrayClockWClass"))))
            {
                g_hTrayNotifyChildren.push_back(hWnd);
            }
        }
        else if (0 == wcscmp(lpClassName, L"MSTaskSwWClass")
        || 0 == wcscmp(lpClassName, L"MSTaskListWClass"))
        {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskListSubclassProc, NULL))
            {
                g_hTaskLists.push_back(hWnd);
            }
        }
        else if (0 == wcscmp(lpClassName, L"ReBarWindow32"))
        {
            WCHAR szParentClass[MAX_PATH];
            if (GetClassNameW(hWndParent, szParentClass, MAX_PATH)
            && (0 == wcscmp(szParentClass, L"Shell_TrayWnd")
            || 0 == wcscmp(szParentClass, L"Shell_SecondaryTrayWnd")))
            {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskListSubclassProc, NULL))
                {
                    g_hTaskLists.push_back(hWnd);
                }
            }
        }
    }

    return hWnd;
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    /* Reduce spacing between clock text */
    {
        {
            L"private: int __cdecl ClockButton::GetTextSpacingForOrientation(bool,int,int,int,int)"
        },
        &ClockButton_GetTextSpacingForOrientation_orig,
        ClockButton_GetTextSpacingForOrientation_hook,
        false
    },
    /* Increase minimum width */
    {
        {
            L"public: struct tagSIZE __cdecl ClockButton::CalculateMinimumSize(struct tagSIZE)"
        },
        &ClockButton_CalculateMinimumSize_orig,
        ClockButton_CalculateMinimumSize_hook,
        false
    },
    /* Draw clock text one pixel lower than 10 does, this is where 7 draws it */
    {
        {
            L"private: void __cdecl ClockButton::ClockButtonImage::DrawTextW(struct HDC__ *,unsigned short const *,int,int,int,int,int)"
        },
        &ClockButton_ClockButtonImage_DrawTextW_orig,
        ClockButton_ClockButtonImage_DrawTextW_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        CreateWindowExW,
        CreateWindowExW_hook,
        &CreateWindowExW_orig
    ))
    {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_hTrayNotify)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTrayNotify,
            TrayNotifySubclassProc
        );
    }

    size_t len = g_hTaskLists.size();
    for (size_t i = 0; i < len; i++)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTaskLists[i],
            TaskListSubclassProc
        );
    }

    len = g_hTrayNotifyChildren.size();
    for (size_t i = 0; i < len; i++)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTrayNotifyChildren[i],
            TrayNotifyChildSubclassProc
        );
    }
}
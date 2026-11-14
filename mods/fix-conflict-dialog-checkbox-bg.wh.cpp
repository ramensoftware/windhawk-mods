// ==WindhawkMod==
// @id              fix-conflict-dialog-checkbox-bg
// @name            Fix file dialogs for Classic theme
// @description     Fixes black backgrounds in shell dialogs (Classic Theme) under Win 11 24H2+
// @version         2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix File Conflict Dialog Checkbox Background

Fixes black backgrounds in Explorer shell dialogs when Windows Classic
theme is active:
- dialog background (#32770)
- "Apply to all conflicts" checkbox in the file conflict dialog
- Static/Label controls (including icon labels) in shell dialogs
  such as file deletion

Only unthemed (under Classic theme) windows are affected. Themed windows are
ignored completely.

This bug has appeared in recent cumulative updates.

Before:

![before](https://i.imgur.com/jMGJOHd.png)

![before](https://i.imgur.com/asDIOSr.png)

After:

![after](https://i.imgur.com/pK0Z0u8.png)

![after](https://i.imgur.com/wZnlaci.png)

*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <algorithm>
#include <vector>
#include <mutex>
#include <windhawk_utils.h>

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t pOriginalCreateWindowExW = nullptr;

static std::vector<HWND> g_subclassedWindows;
static std::mutex g_mutex;

static bool IsWindowClass(HWND hWnd, const wchar_t* expected)
{
    if (!hWnd)
        return false;
    wchar_t cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)))
        return false;
    return _wcsicmp(cls, expected) == 0;
}

static bool HasDialogAncestor(HWND hWnd)
{
    for (HWND hCur = hWnd; hCur; hCur = GetParent(hCur))
    {
        if (IsWindowClass(hCur, L"#32770"))
            return true;
    }
    return false;
}

static bool CheckHierarchy(HWND hWndParent)
{
    // Original v1.0: Button -> CtrlNotifySink -> DirectUIHWND -> #32770
    wchar_t cls[64];

    GetClassNameW(hWndParent, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"CtrlNotifySink"))
        return false;

    HWND hLevel2 = GetParent(hWndParent);
    if (!hLevel2)
        return false;
    GetClassNameW(hLevel2, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"DirectUIHWND"))
        return false;

    HWND hLevel3 = GetParent(hLevel2);
    if (!hLevel3)
        return false;
    GetClassNameW(hLevel3, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"#32770"))
        return false;

    return true;
}

// SS_BLACKRECT / SS_GRAYRECT / SS_WHITEFRAME are intentionally colored.
static bool IsColorRectStatic(HWND hWnd)
{
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    int type = (int)(style & 0x1F); // SS_TYPEMASK
    return type == 4 || type == 5 || type == 6;
}

static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_CTLCOLORSTATIC)
    {
        HWND hCtrl = (HWND)lParam;
        if (hCtrl)
        {
            wchar_t cls[64] = L"";
            GetClassNameW(hCtrl, cls, ARRAYSIZE(cls));
            bool isStatic = (_wcsicmp(cls, L"Static") == 0);
            bool isLabel  = (_wcsicmp(cls, L"Label") == 0);

            if ((isStatic || isLabel) && !(isStatic && IsColorRectStatic(hCtrl)))
            {
                HDC hdc = (HDC)wParam;
                SetBkMode(hdc, OPAQUE);
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
                SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
                return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
            }
        }
    }
    else if (uMsg == WM_CTLCOLORBTN)
    {
        HWND hCtrl = (HWND)lParam;
        if (hCtrl && IsWindowClass(hCtrl, L"Button"))
        {
            LONG_PTR style = GetWindowLongPtrW(hCtrl, GWL_STYLE);
            LONG_PTR type  = style & BS_TYPEMASK;
            if (type == BS_AUTOCHECKBOX || type == BS_CHECKBOX)
            {
                HDC hdc = (HDC)wParam;
                SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
                SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
                return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
            }
        }
    }
    else if (uMsg == WM_ERASEBKGND)
    {
        wchar_t cls[64] = L"";
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
        bool isDialog = (_wcsicmp(cls, L"#32770") == 0);
        bool isStatic = (_wcsicmp(cls, L"Static") == 0 || _wcsicmp(cls, L"Label") == 0);

        if (isDialog || (isStatic && !(_wcsicmp(cls, L"Static") == 0 && IsColorRectStatic(hWnd))))
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
            return 1;
        }
    }
    else if (uMsg == WM_CTLCOLORDLG)
    {
        if (IsWindowClass(hWnd, L"#32770"))
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }
    }
    else if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = std::find(g_subclassedWindows.begin(),
                            g_subclassedWindows.end(), hWnd);
        if (it != g_subclassedWindows.end())
            g_subclassedWindows.erase(it);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static bool SubclassWindowIfNeeded(HWND hWnd)
{
    if (!hWnd)
        return false;

    std::lock_guard<std::mutex> lock(g_mutex);
    if (std::find(g_subclassedWindows.begin(), g_subclassedWindows.end(), hWnd) !=
        g_subclassedWindows.end())
    {
        return false;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0))
        return false;

    g_subclassedWindows.push_back(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
    return true;
}

// Only Static/Label children — nothing else.
static BOOL CALLBACK EnumStaticProc(HWND hChild, LPARAM lParam)
{
    if (IsWindowClass(hChild, L"Static") || IsWindowClass(hChild, L"Label"))
        SubclassWindowIfNeeded(hChild);
    return TRUE;
}

static void SubclassStaticChildren(HWND hDlg)
{
    if (hDlg)
        EnumChildWindows(hDlg, EnumStaticProc, 0);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
    LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd || IsAppThemed())
        return hWnd;

    wchar_t cls[64] = L"";
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    bool isDialog = (_wcsicmp(cls, L"#32770") == 0);
    bool isStatic = (_wcsicmp(cls, L"Static") == 0 || _wcsicmp(cls, L"Label") == 0);

    // 1. Dialog background. #32770 itself bypasses CreateWindowExW,
    //    so subclass it on its first child.
    if (isDialog)
    {
        if (SubclassWindowIfNeeded(hWnd))
            SubclassStaticChildren(hWnd);
    }

    if (hWndParent && IsWindowClass(hWndParent, L"#32770"))
    {
        if (SubclassWindowIfNeeded(hWndParent))
            SubclassStaticChildren(hWndParent);
    }

    // 2. Static/Label: itself (WM_ERASEBKGND, incl. icon) + parent
    //    (WM_CTLCOLORSTATIC).
    if (isStatic && hWndParent && HasDialogAncestor(hWndParent))
    {
        SubclassWindowIfNeeded(hWnd);
        SubclassWindowIfNeeded(hWndParent);
    }

    // 3. Original v1.0 checkbox logic.
    if (hWndParent && lpClassName && !IS_INTRESOURCE(lpClassName) &&
        wcscmp(lpClassName, L"Button") == 0)
    {
        LONG_PTR type = dwStyle & BS_TYPEMASK;
        if ((type == BS_AUTOCHECKBOX || type == BS_CHECKBOX) &&
            CheckHierarchy(hWndParent))
        {
            SubclassWindowIfNeeded(hWndParent);
        }
    }

    return hWnd;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&pOriginalCreateWindowExW);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    std::vector<HWND> hwnds;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        hwnds.swap(g_subclassedWindows);
    }

    for (HWND hWnd : hwnds)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
}

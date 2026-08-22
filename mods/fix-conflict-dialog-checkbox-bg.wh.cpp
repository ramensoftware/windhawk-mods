// ==WindhawkMod==
// @id              fix-conflict-dialog-checkbox-bg
// @name            Fix File Conflict Dialog Checkbox Background
// @description     Fixes black background on the checkbox in the file conflict dialog (Classic Theme)
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix File Conflict Dialog Checkbox Background

Fixes the black background of the "Apply to all conflicts" checkbox
in the Explorer file copy/move conflict dialog when Windows Classic theme is active.

This bug has appeared in recent cumulative updates.

Before:

![before](https://i.imgur.com/jMGJOHd.png)

After:

![after](https://i.imgur.com/pK0Z0u8.png)

*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <vector>
#include <mutex>
#include <windhawk_utils.h>

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t pOriginalCreateWindowExW = nullptr;

static std::vector<HWND> g_subclassedWindows;
static std::mutex g_windowsMutex;

static bool CheckHierarchy(HWND hWndParent)
{
    // Button -> CtrlNotifySink -> DirectUIHWND -> #32770
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

static LRESULT CALLBACK ParentSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                           LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_CTLCOLORBTN || uMsg == WM_CTLCOLORSTATIC)
    {
        HWND hCtrl = (HWND)lParam;
        wchar_t szClass[32];
        GetClassNameW(hCtrl, szClass, ARRAYSIZE(szClass));

        if (!_wcsicmp(szClass, L"Button"))
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
    else if (uMsg == WM_NCDESTROY)
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = std::find(g_subclassedWindows.begin(),
                            g_subclassedWindows.end(), hWnd);
        if (it != g_subclassedWindows.end())
            g_subclassedWindows.erase(it);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
    LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd || !hWndParent)
        return hWnd;

    if (!lpClassName || IS_INTRESOURCE(lpClassName))
        return hWnd;

    if (wcscmp(lpClassName, L"Button"))
        return hWnd;

    LONG_PTR type = dwStyle & BS_TYPEMASK;
    if (type != BS_AUTOCHECKBOX && type != BS_CHECKBOX)
        return hWnd;

    if (IsAppThemed())
        return hWnd;

    if (!CheckHierarchy(hWndParent))
        return hWnd;

    std::lock_guard<std::mutex> lock(g_windowsMutex);

    bool alreadySubclassed = std::find(g_subclassedWindows.begin(),
                                       g_subclassedWindows.end(),
                                       hWndParent) != g_subclassedWindows.end();
    if (!alreadySubclassed)
    {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWndParent, ParentSubclassProc, 0))
        {
            g_subclassedWindows.push_back(hWndParent);
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
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        hwnds.swap(g_subclassedWindows);
    }

    for (HWND hWnd : hwnds)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ParentSubclassProc);
}

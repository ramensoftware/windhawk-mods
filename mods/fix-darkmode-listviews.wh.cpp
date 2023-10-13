// ==WindhawkMod==
// @id              fix-darkmode-listviews
// @name            Fix Darkmode ListViews
// @description     Fixes ListViews in dark mode
// @version         1.0
// @author          Reabstraction
// @github          https://github.com/Reabstraction
// @include         *
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Fixes ListViews in dark mode, and outside of it

Without: ![Without](https://i.imgur.com/8kmOShw.png)

With: ![With](https://i.imgur.com/PorgfmI.png)
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <commctrl.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
using SetWindowTheme_t = decltype(&SetWindowTheme);
SetWindowTheme_t SetWindowTheme_orig;

void ApplyTheme(HWND hListView)
{
    SetWindowTheme(
        hListView,
        L"Explorer",
        NULL
    );

    // HDC hDC = GetDC(hListView);
    ListView_SetTextColor(hListView, GetSysColor(COLOR_WINDOWTEXT));
    // ListView_SetTextColor(hListView, RGB(255, 255, 255));
    // DeleteObject(hDC);
    
    ListView_SetExtendedListViewStyle(
        hListView,
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER
    );
    
    SendMessageW(hListView, WM_THEMECHANGED, NULL, NULL);
}

BOOL ShouldApply(
    HWND hWndParent,
    LPCWSTR lpClassName
) 
{
    return (hWndParent != NULL
    &&      lpClassName != NULL
    &&      (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0)
    &&      0 == wcscmp(lpClassName, L"SysListView32"));
}

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
    Wh_Log(L"CreateWindowExW hook called");
    HWND hRes = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu,
        hInstance, lpParam
    );

    if (ShouldApply(hWndParent, lpClassName))
    {
        WCHAR lpPrntCls[256];
        GetClassNameW(hWndParent, lpPrntCls, 256);
        ApplyTheme(hRes);
    }

    return hRes;
}

HRESULT WINAPI SetWindowTheme_Hook(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList)
{
    HWND hWndParent = GetParent(hWnd);
    WCHAR lpClassName[256];
    GetClassNameW(hWnd, lpClassName, sizeof(lpClassName) / sizeof(WCHAR));
    if(ShouldApply(hWndParent, lpClassName))
    {
        HRESULT hRes = SetWindowTheme_orig(hWnd, L"Explorer", pszSubIdList);
        return hRes;
    }
    else
    {
        HRESULT hRes = SetWindowTheme_orig(hWnd, pszSubAppName, pszSubIdList);
        return hRes;
    }
}

// TODO: Fix this

// BOOL CALLBACK FindListViewProc(HWND hWnd, LPARAM lParam)
// {
//     DWORD pId;
//     WCHAR lpClass[256];

//     GetWindowThreadProcessId(hWnd, &pId);
//     GetClassNameW(hWnd, lpClass, 256);

//     if (pId == (DWORD)lParam)
//     {
//         HWND hListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
//         ApplyTheme(hListView);
//         return FALSE;
//     }

//     return TRUE;
// }

// BOOL FindListView(void)
// {
//     DWORD pId = GetCurrentProcessId();
//     EnumWindows(FindListViewProc, (LPARAM)pId);
//     return true;
// }


BOOL Wh_ModInit(void)
{
    Wh_Log(L"Mod loaded");
    if (!Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    ))
    {
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook(
        (void *)SetWindowTheme,
        (void *)SetWindowTheme_Hook,
        (void **)&SetWindowTheme_orig
    ))
    {
        return FALSE;
    }

    // FindListView();

    return true;
}

void Wh_ModUninit(void)
{
    // TODO
}

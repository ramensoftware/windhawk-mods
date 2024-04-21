// ==WindhawkMod==
// @id              themed-regedit-listview
// @name            Themed Regedit ListView
// @description     Makes the ListView in Regedit themed
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         regedit.exe
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Themed Regedit ListView
For a brief time during Windows 10's life, the Registry Editor had a themed
ListView. This mod restores that.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/themed-regedit-listview-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/themed-regedit-listview-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- themed: true
  $name: Themed ListView
  $description: Applies theme to the ListView
- fullrow: true
  $name: Full-row selection
  $description: Makes the ListView use full-row selection
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>
#include <commctrl.h>

HWND hListView;

struct
{
    BOOL bThemed;
    BOOL bFullrow;
} settings;

void UpdateListView(BOOL bInit)
{
    if (settings.bThemed)
    {
        SetWindowTheme(
            hListView,
            bInit ? L"Explorer" : NULL,
            NULL
        );
        SendMessageW(hListView, WM_THEMECHANGED, NULL, NULL);
    }

    if (settings.bFullrow)
    {
        ListView_SetExtendedListViewStyle(
            hListView,
            bInit
        ?   LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER
        :   ListView_GetExtendedListViewStyle(hListView) & ~(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER)
        );
    }
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
    HWND hRes = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu,
        hInstance, lpParam
    );

    if (hWndParent != NULL
    &&  lpClassName != NULL
    &&  (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0)
    &&  0 == wcscmp(lpClassName, L"SysListView32")
    )
    {
        WCHAR lpPrntCls[256];
        GetClassNameW(hWndParent, lpPrntCls, 256);
        if (0 == wcscmp(lpPrntCls, L"RegEdit_RegEdit"))
        {
            hListView = hRes;
            UpdateListView(TRUE);
        }
    }

    return hRes;
}

void LoadSettings(void)
{
    settings.bThemed = Wh_GetIntSetting(L"themed");
    settings.bFullrow = Wh_GetIntSetting(L"fullrow");
}

BOOL CALLBACK FindListViewProc(HWND hWnd, LPARAM lParam)
{
    DWORD pId;
    WCHAR lpClass[256];

    GetWindowThreadProcessId(hWnd, &pId);
    GetClassNameW(hWnd, lpClass, 256);

    if (pId == (DWORD)lParam
    &&  0 == wcscmp(lpClass, L"RegEdit_RegEdit"))
    {
        hListView = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        return FALSE;
    }

    return TRUE;
}

BOOL FindListView(void)
{
    DWORD pId = GetCurrentProcessId();
    EnumWindows(FindListViewProc, (LPARAM)pId);
    return (hListView != NULL);
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    if (FindListView())
    {
        UpdateListView(TRUE);
    }

    if (!Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    ))
    {
        return FALSE;
    }

    return true;
}

void Wh_ModUninit(void)
{
    UpdateListView(FALSE);
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}
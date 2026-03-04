// ==WindhawkMod==
// @id              native-titlebars-uwp-lite
// @name            Remove UWP titlebars Lite
// @description     Enables native titlebars in UWP apps
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         ApplicationFrameHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Replaces the UWP titlebars with native Win32 titlebars.

This mod is focused on the Classic theme, so may produce sub-optimal results in other cases.

![Screenshot](https://i.imgur.com/Pf6RQTk.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <CommCtrl.h>
#include <dwmapi.h>

typedef INT64 (*CTitleBar__CreateTitleBarWindow_t)(void *);
CTitleBar__CreateTitleBarWindow_t CTitleBar__CreateTitleBarWindow_orig;
INT64 __fastcall CTitleBar__CreateTitleBarWindow_hook(void *pThis)
{
    return 0;
}

typedef INT64 (*CTitleBar__PaintButton_t)(INT64, INT64, INT64, UINT, int *, DWORD);
CTitleBar__PaintButton_t CTitleBar__PaintButton_orig;
INT64 __fastcall CTitleBar__PaintButton_hook(
    INT64 a1,
    INT64 a2,
    INT64 a3,
    UINT a4,
    int *a5,
    DWORD a6)
{
    return 0;
}

LRESULT CALLBACK SubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, SubclassProc, uIdSubclass);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    
    if (uMsg == WM_NCPAINT || uMsg == WM_NCCALCSIZE)
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (*CreateWindowInBandEx_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_orig;
HWND WINAPI CreateWindowInBandEx_hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam,
    DWORD dwBand,
    DWORD dwTypeFlags)
{
    dwExStyle |= WS_EX_DLGMODALFRAME;
    dwExStyle &= ~0x00200000L; // WS_EX_NOREDIRECTIONBITMAP
    dwStyle = WS_OVERLAPPEDWINDOW | WS_DLGFRAME;

    HWND res = CreateWindowInBandEx_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    
    if (res != NULL)
    {
        SetWindowSubclass(res, SubclassProc, 1, 0);
    }
    
    return res;
}

typedef HRESULT (WINAPI *DwmExtendFrameIntoClientArea_t)(HWND, const MARGINS *);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_orig;
HRESULT WINAPI DwmExtendFrameIntoClientArea_hook(
    HWND hWnd,
    const MARGINS *pMarInset)
{
    WCHAR wszClassName[32] = {0};
    GetClassNameW(hWnd, wszClassName, ARRAYSIZE(wszClassName));
    
    if (wcscmp(wszClassName, L"ApplicationFrameWindow") == 0)
    {
        return 0x80263001; // DWM_E_COMPOSITIONDISABLED
    }
    
    return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
}

BOOL Wh_ModInit()
{

    Wh_SetFunctionHook((void *)GetProcAddress(LoadLibraryW(L"user32.dll"), "CreateWindowInBandEx"), (void *)CreateWindowInBandEx_hook, (void **)&CreateWindowInBandEx_orig);

    Wh_SetFunctionHook((void *)DwmExtendFrameIntoClientArea, (void *)DwmExtendFrameIntoClientArea_hook, (void **)&DwmExtendFrameIntoClientArea_orig);

    WindhawkUtils::SYMBOL_HOOK ApplicationFrame_dll_hooks[] = {
        {
            {L"private: long __cdecl CTitleBar::_CreateTitleBarWindow(void)"},
            (void **)&CTitleBar__CreateTitleBarWindow_orig,
            (void *)CTitleBar__CreateTitleBarWindow_hook,
            false
        },
        {
            {L"private: long __cdecl CTitleBar::_PaintButton(struct IDCompositionSurface *,enum CTitleBar::TitleBarControl,enum TITLE_BAR_BITMAP_TYPE,struct tagRECT const &,struct tagRECT const &)"},
            (void **)&CTitleBar__PaintButton_orig,
            (void *)CTitleBar__PaintButton_hook,
            false
        },
    };

    return WindhawkUtils::HookSymbols(LoadLibraryW(L"ApplicationFrame.dll"), ApplicationFrame_dll_hooks, ARRAYSIZE(ApplicationFrame_dll_hooks));
}

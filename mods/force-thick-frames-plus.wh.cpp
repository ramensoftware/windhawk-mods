// ==WindhawkMod==
// @id              force-thick-frames-plus
// @name            Force thick frames PLUS
// @description     Force windows to have thick frames like in Windows Vista/7
// @version         3.1
// @author          teknixstuff
// @github          https://github.com/teknixstuff2
// @include         *
// @compilerOptions -lcomdlg32 -lcomctl32 -lgdi32 -luxtheme -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Force windows to have thick frames like in Windows Vista/7. This theme should already have thick borders (so like, not the default Windows 10 or 11 theme).

![Screenshot](https://raw.githubusercontent.com/arukateru/ThickFramer/main/251890469-a04311e7-bbed-449f-9c92-79c642c36005.png)
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winuser.h>
#include <Windowsx.h>

using PUNICODE_STRING = PVOID;

LRESULT CALLBACK antiResizeProc(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam,
  DWORD_PTR dwRefData
)
{
    if (uMsg == WM_NCHITTEST)
    {
        LRESULT lr = DefSubclassProc(
            hWnd, uMsg, wParam, lParam
        );
        switch (lr)
        {
            case HTTOP:
            case HTTOPRIGHT:
            case HTTOPLEFT:
                return HTCAPTION;
            case HTRIGHT:
            case HTLEFT:
                RECT pos;
                GetWindowRect(hWnd, &pos);
                if (GET_Y_LPARAM(lParam) > (pos.top + 30)) {
                    return HTBORDER;
                } else {
                    return HTCAPTION;
                }
            case HTBOTTOMRIGHT:
            case HTBOTTOM:
            case HTBOTTOMLEFT:
                return HTBORDER;
            default:
                return lr;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// https://github.com/sandboxie-plus/Sandboxie/blob/294966c7d6e99cd153ede87ad09aa39ef29e34c3/Sandboxie/core/dll/Win32.c#L25
using NtUserCreateWindowEx_t =
    HWND(WINAPI*)(DWORD dwExStyle,
                      PUNICODE_STRING UnsafeClassName,
                      LPCWSTR VersionedClass,
                      PUNICODE_STRING UnsafeWindowName,
                      DWORD dwStyle,
                      LONG x,
                      LONG y,
                      LONG nWidth,
                      LONG nHeight,
                      HWND hWndParent,
                      HMENU hMenu,
                      HINSTANCE hInstance,
                      LPVOID lpParam,
                      DWORD dwShowMode,
                      DWORD dwUnknown1,
                      DWORD dwUnknown2,
                      VOID* qwUnknown3);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;
HWND WINAPI NtUserCreateWindowEx_Hook(DWORD dwExStyle,
                                          PUNICODE_STRING UnsafeClassName,
                                          LPCWSTR VersionedClass,
                                          PUNICODE_STRING UnsafeWindowName,
                                          DWORD dwStyle,
                                          LONG x,
                                          LONG y,
                                          LONG nWidth,
                                          LONG nHeight,
                                          HWND hWndParent,
                                          HMENU hMenu,
                                          HINSTANCE hInstance,
                                          LPVOID lpParam,
                                          DWORD dwShowMode,
                                          DWORD dwUnknown1,
                                          DWORD dwUnknown2,
                                          VOID* qwUnknown3) {
    BOOL canMod = false;
    if ((dwStyle & WS_CAPTION) == WS_CAPTION) {
        if ((dwStyle & WS_THICKFRAME) == 0) {
            dwStyle |= WS_THICKFRAME;
            canMod = true;
        }
    }

    HWND ret = NtUserCreateWindowEx_Original(
        dwExStyle, UnsafeClassName, VersionedClass, UnsafeWindowName, dwStyle,
        x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
        dwShowMode, dwUnknown1, dwUnknown2, qwUnknown3);
    if (canMod && ret != NULL) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(ret, &antiResizeProc, 0);
    }
    return ret;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE hModule = GetModuleHandle(L"win32u.dll");
    if (!hModule) {
        return FALSE;
    }

    NtUserCreateWindowEx_t pNtUserCreateWindowEx =
        (NtUserCreateWindowEx_t)GetProcAddress(hModule, "NtUserCreateWindowEx");
    if (!pNtUserCreateWindowEx) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)pNtUserCreateWindowEx,
                       (void*)NtUserCreateWindowEx_Hook,
                       (void**)&NtUserCreateWindowEx_Original);

    return TRUE;
}

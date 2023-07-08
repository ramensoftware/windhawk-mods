// ==WindhawkMod==
// @id              force-thick-frames-clock
// @name            Force thick frames-clock
// @description     Force windows to have thick frames like in Windows Vista/7. 
// @version         2
// @author          Alcatel
// @github          https://github.com/arukateru
// @include         *
// @exclude         notepad++.exe
// ==/WindhawkMod==

 // ==WindhawkModReadme== 
 /*  
 Force windows to have thick frames like in Windows Vista/7. This theme should already have thick borders (so like, not the default Windows 10 or 11 theme)
 ![Screenshot](https://user-images.githubusercontent.com/84914212/251890469-a04311e7-bbed-449f-9c92-79c642c36005.png) 
 */ 
 // ==/WindhawkModReadme== 
 
#include <Windows.h>

// Define function pointer types
using NtUserCreateWindowEx_t = NTSTATUS(WINAPI*)(DWORD dwExStyle,
                                                 PVOID UnsafeClassName,
                                                 LPCWSTR VersionedClass,
                                                 PVOID UnsafeWindowName,
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
                                                 PVOID qwUnknown3);
NtUserCreateWindowEx_t NtUserCreateWindowEx_Original;

// Hook function
NTSTATUS WINAPI NtUserCreateWindowEx_Hook(DWORD dwExStyle,
                                          PVOID UnsafeClassName,
                                          LPCWSTR VersionedClass,
                                          PVOID UnsafeWindowName,
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
                                          PVOID qwUnknown3) {
    // Call the original function
    NTSTATUS result = NtUserCreateWindowEx_Original(dwExStyle, UnsafeClassName, VersionedClass,
                                                   UnsafeWindowName, dwStyle, x, y, nWidth,
                                                   nHeight, hWndParent, hMenu, hInstance,
                                                   lpParam, dwShowMode, dwUnknown1, dwUnknown2,
                                                   qwUnknown3);

    // Check if the window being created is the taskbar clock flyout
    if (VersionedClass && wcscmp(VersionedClass, L"Shell_TrayWnd") == 0) {
        HWND hWndFlyout = FindWindowExW(hWndParent, nullptr, L"ClockFlyoutWindow", nullptr);
        if (hWndFlyout != nullptr) {
            // Add the border style to the flyout window
            LONG updatedStyle = GetWindowLong(hWndFlyout, GWL_STYLE);
            updatedStyle |= WS_THICKFRAME;
            SetWindowLong(hWndFlyout, GWL_STYLE, updatedStyle);
            SetWindowPos(hWndFlyout, nullptr, x, y, nWidth, nHeight, SWP_FRAMECHANGED);
        }
    }

    return result;
}

BOOL Wh_ModInit() {
    HMODULE hModule = GetModuleHandle(L"win32u.dll");
    if (!hModule) {
        return FALSE;
    }

    // Get the address of NtUserCreateWindowEx
    NtUserCreateWindowEx_t pNtUserCreateWindowEx =
        (NtUserCreateWindowEx_t)GetProcAddress(hModule, "NtUserCreateWindowEx");
    if (!pNtUserCreateWindowEx) {
        return FALSE;
    }

    // Set the hook
    Wh_SetFunctionHook((PVOID*)pNtUserCreateWindowEx, (PVOID*)NtUserCreateWindowEx_Hook,
                       (PVOID**)&NtUserCreateWindowEx_Original);

    return TRUE;
}

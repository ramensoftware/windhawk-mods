// ==WindhawkMod==
// @id              force-thick-frames
// @name            Force thick frames
// @description     Force windows to have thick frames like in Windows Vista/7
// @version         2
// @author          Alcatel
// @github          https://github.com/arukateru
// @include         *
// @exclude         notepad++.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Force windows to have thick frames like in Windows Vista/7. This theme should already have thick borders (so like, not the default Windows 10 or 11 theme).

![Screenshot](https://raw.githubusercontent.com/arukateru/ThickFramer/main/251890469-a04311e7-bbed-449f-9c92-79c642c36005.png)
*/
// ==/WindhawkModReadme==

using PUNICODE_STRING = PVOID;

// https://github.com/sandboxie-plus/Sandboxie/blob/294966c7d6e99cd153ede87ad09aa39ef29e34c3/Sandboxie/core/dll/Win32.c#L25
using NtUserCreateWindowEx_t =
    NTSTATUS(WINAPI*)(DWORD dwExStyle,
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
NTSTATUS WINAPI NtUserCreateWindowEx_Hook(DWORD dwExStyle,
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
    if ((dwStyle & WS_CAPTION) == WS_CAPTION) {
        dwStyle |= WS_THICKFRAME;
    }

    return NtUserCreateWindowEx_Original(
        dwExStyle, UnsafeClassName, VersionedClass, UnsafeWindowName, dwStyle,
        x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
        dwShowMode, dwUnknown1, dwUnknown2, qwUnknown3);
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

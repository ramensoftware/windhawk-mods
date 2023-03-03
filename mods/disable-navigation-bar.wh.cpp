// ==WindhawkMod==
// @id              disable-navigation-bar
// @name            Disable Navigation Bar
// @description     Disables the navigation bar in file explorer
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Navigation Bar
This mod disables the navigation bar in file explorer.

Note: This mod applies to explorer folders as well as save/open dialogs in all programs.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/disable-navigation-bar.png)
*/
// ==/WindhawkModReadme==

static HWND(__stdcall *ExplorerFrame_SHCreateWorkerWindow)(
    WNDPROC     wndProc,
    HWND        hWndParent,
    DWORD       dwExStyle,
    DWORD       dwStyle,
    HMENU       hMenu,
    LONG_PTR    wnd_extra
);

HWND WINAPI ExplorerFrame_SHCreateWorkerWindowHook(
    WNDPROC     wndProc,
    HWND        hWndParent,
    DWORD       dwExStyle,
    DWORD       dwStyle,
    HMENU       hMenu,
    LONG_PTR    wnd_extra
) {
    HWND result;

    if (dwExStyle == 0x10000 && dwStyle == 1174405120) return 0;

    return ExplorerFrame_SHCreateWorkerWindow(
        wndProc,
        hWndParent,
        dwExStyle,
        dwStyle,
        hMenu,
        wnd_extra
    );
}



BOOL Wh_ModInit() {
    HMODULE hShcore = GetModuleHandle(L"shcore.dll");

    void* origFunc = (void*)GetProcAddress(hShcore, (LPCSTR)188);
    Wh_SetFunctionHook(origFunc, (void*)ExplorerFrame_SHCreateWorkerWindowHook, (void**)&ExplorerFrame_SHCreateWorkerWindow);

    return TRUE;
}

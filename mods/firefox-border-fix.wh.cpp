// ==WindhawkMod==
// @id              firefox-border-fix
// @name            Firefox border fix for Classic theme 
// @description     Mitigates Firefox bug 1950145 (glitched window borders in Classic theme)
// @version         1.5
// @author          anixx
// @github          https://github.com/Anixx
// @include         firefox.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Workaround for the Firefox bug [1950145](https://bugzilla.mozilla.org/show_bug.cgi?id=1950145) (glitched window borders in Classic theme).
It affects Firefox builds `129.0a1` and above.

*/
// ==/WindhawkModReadme==

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Orig = nullptr;

// Thread procedure: apply frame refresh once
DWORD WINAPI ShowWindowFixThread(LPVOID param) {
    HWND hwnd = (HWND)param;

    // Trigger non-client refresh
    SendMessage(hwnd, WM_NCACTIVATE, FALSE, 0);
    SendMessage(hwnd, WM_NCACTIVATE, TRUE, 0);

    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOACTIVATE | SWP_FRAMECHANGED);

    RedrawWindow(hwnd, NULL, NULL,
                 RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);

    return 0;
}

// Hooked ShowWindow
BOOL WINAPI ShowWindow_Hook(HWND hwnd, int nCmdShow) {
    BOOL ret = ShowWindow_Orig(hwnd, nCmdShow); // call original first

    if (IsWindowVisible(hwnd)) {

        WCHAR cls[64] = {0};
        GetClassNameW(hwnd, cls, _countof(cls));

        if (!wcscmp(cls, L"MozillaWindowClass")) {
            CreateThread(NULL, 0, ShowWindowFixThread, (LPVOID)hwnd, 0, NULL);
        }

    }
    return ret;
}

BOOL Wh_ModInit(void) {
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);
    return TRUE;
}

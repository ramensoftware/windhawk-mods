// ==WindhawkMod==
// @id              firefox-border-fix
// @name            Firefox border fix for Classic theme 
// @description     Mitigates Firefox bug 1950145 (glitched window borders in Classic theme)
// @version         1.0.1
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
ShowWindow_t ShowWindow_Orig;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {

    WCHAR lpClassName[50];
    GetClassName(hWnd, lpClassName, 50);

    if (!wcscmp(lpClassName, L"MozillaWindowClass"))
        SendMessage(hWnd, WM_THEMECHANGED, NULL, NULL);

    return ShowWindow_Orig(hWnd, nCmdShow);
}

BOOL Wh_ModInit(void)
{

        Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);

    return TRUE;
}

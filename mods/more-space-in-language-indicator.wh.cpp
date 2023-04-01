// ==WindhawkMod==
// @id           more-space-in-language-indicator
// @name         More space in language indicator
// @description  Enables to see two lines in the language indicator with small taskbar icons (Windows 10)
// @version      1.0
// @author       m417z
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# More space in language indicator

Enables to see two lines in the language indicator with small taskbar icons
(Windows 10).

Example:

![Screenshot](https://i.imgur.com/VztpH9B.png)

By default, Windows only shows a single line (ENG) with small taskbar buttons.
*/
// ==/WindhawkModReadme==

HWND g_hTrayInputIndicator;

using DeferWindowPos_t = decltype(&DeferWindowPos);
DeferWindowPos_t DeferWindowPos_Original;
HDWP WINAPI DeferWindowPos_Hook(HDWP hWinPosInfo,
                                HWND hWnd,
                                HWND hWndInsertAfter,
                                int x,
                                int y,
                                int cx,
                                int cy,
                                UINT uFlags) {
    if (!g_hTrayInputIndicator) {
        WCHAR szClassName[32];
        GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName));
        if (_wcsicmp(szClassName, L"TrayInputIndicatorWClass") == 0) {
            g_hTrayInputIndicator = hWnd;
        }
    }

    if (g_hTrayInputIndicator && hWnd == g_hTrayInputIndicator && cy < 32) {
        cy = 32;
    }

    return DeferWindowPos_Original(hWinPosInfo, hWnd, hWndInsertAfter, x, y, cx,
                                   cy, uFlags);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    Wh_SetFunctionHook((void*)DeferWindowPos, (void*)DeferWindowPos_Hook,
                       (void**)&DeferWindowPos_Original);

    return TRUE;
}

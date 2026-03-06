// ==WindhawkMod==
// @id              sndvol-minimize
// @name            Volume Mixer Minimize Button
// @description     Adds a Minimize button to the Volume Mixer
// @version         1
// @author          Jevil7452
// @github          https://github.com/Jevil7452
// @include         sndvol.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod adds the Minimize button to the Volume Mixer and Volume Control Options dialogs.

Before:

![1](https://i.imgur.com/MG6eeLL.png)
![2](https://i.imgur.com/2ZCGB2t.png)

After:

![3](https://i.imgur.com/1pNiTNi.png)
![4](https://i.imgur.com/cu6j0JR.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;
    if (pwp->message == WM_CREATE) {
        LONG_PTR style = GetWindowLongPtrW(pwp->hwnd, GWL_STYLE);
        SetWindowLongPtrW(pwp->hwnd, GWL_STYLE, style | WS_MINIMIZEBOX);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL Wh_ModInit(void) {
    SetWindowsHookExW(WH_CALLWNDPROC, CallWndProc, NULL, GetCurrentThreadId());
    return TRUE;
}
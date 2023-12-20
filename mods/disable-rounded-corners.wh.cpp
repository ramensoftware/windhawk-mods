// ==WindhawkMod==
// @id              disable-rounded-corners
// @name            Disable rounded corners in Windows 11
// @description     A simple mod to disable window rounded corners in Windows 11
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Disable rounded corners in Windows 11

A simple mod to disable window rounded corners in Windows 11.

Based on the
[Win11DisableRoundedCorners](https://github.com/valinet/Win11DisableRoundedCorners)
project by Valentin Radu.

![Screenshot](https://i.imgur.com/ez0jyuW.png)

## ⚠ Important usage note ⚠

In order to use this mod, you must allow Windhawk to inject into the **dwm.exe**
system process. To do so, add it to the process inclusion list in the advanced
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

int (*WINAPI GetEffectiveCornerStyle_Original)();
int WINAPI GetEffectiveCornerStyle_Hook() {
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            (void**)&GetEffectiveCornerStyle_Original,
            (void*)GetEffectiveCornerStyle_Hook,
        },
    };

    return HookSymbols(udwm, symbolHooks, ARRAYSIZE(symbolHooks));
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

// ==WindhawkMod==
// @id              w11-dwm-fix
// @name            Bring Back the Borders!
// @description     Restores borders and corners
// @version         1.1.0
// @author          teknixstuff
// @github          https://github.com/teknixstuff
// @twitter         https://twitter.com/teknixstuff
// @homepage        https://teknixstuff.github.io/
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The MIT License.
// Email teknixstuff@gmail.com for any help or info

// ==WindhawkModReadme==
/*
# Bring back the borders!

A simple mod to restore borders and corners in Windows 11.

Based on the
[Disable rounded corners in Windows 11](https://windhawk.net/mods/disable-rounded-corners)
project by m417z.

Based on the
[Win11DisableRoundedCorners](https://github.com/valinet/Win11DisableRoundedCorners)
project by Valentin Radu.

## ⚠ Important usage note ⚠

In order to use this mod, you must allow Windhawk to inject into the **dwm.exe**
system process. To do so, add it to the process inclusion list in the advanced
settings. If you do not do this, it will silently fail to inject.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

bool HighContrastNow = true;

int (*WINAPI GetEffectiveCornerStyle_Original)();
int WINAPI GetEffectiveCornerStyle_Hook() {
    return 0;
}

bool (*WINAPI IsHighContrastMode_Original)();
bool WINAPI IsHighContrastMode_Hook() {
    return HighContrastNow;
}

bool (*WINAPI SystemParametersInfoW_Original)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
bool WINAPI SystemParametersInfoW_Hook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    bool retval = SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);
    if (uiAction == SPI_GETHIGHCONTRAST) {
        HIGHCONTRASTW* contrast;
        contrast = (HIGHCONTRASTW*)pvParam;
        contrast->dwFlags |= HCF_HIGHCONTRASTON;
    }
    return retval;
}

void* (*WINAPI GetBorderRect_Original)(bool);
void* WINAPI GetBorderRect_Hook(bool p) {
    HighContrastNow = false;
    void* retval = GetBorderRect_Original(p);
    HighContrastNow = true;
    return retval;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }
    HMODULE user32 = GetModuleHandle(L"user32.dll");
    if (!user32) {
        Wh_Log(L"user32.dll isn't loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooksUser32[] = {
        {
            {LR"(int __cdecl RealSystemParametersInfoW(unsigned int,unsigned int,void *,unsigned int))"},
            (void**)&SystemParametersInfoW_Original,
            (void*)SystemParametersInfoW_Hook,
        },
    };

    HookSymbols(user32, symbolHooksUser32, ARRAYSIZE(symbolHooksUser32));

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            (void**)&GetEffectiveCornerStyle_Original,
            (void*)GetEffectiveCornerStyle_Hook,
        },
        {
            {LR"(public: static bool __cdecl CDesktopManager::IsHighContrastMode(void))"},
            (void**)&IsHighContrastMode_Original,
            (void*)IsHighContrastMode_Hook,
        },
        {
            {LR"(public: struct tagRECT __cdecl CTopLevelWindow::GetBorderRect(bool)const )"},
            (void**)&GetBorderRect_Original,
            (void*)GetBorderRect_Hook,
        },
    };

    return HookSymbols(udwm, symbolHooks, ARRAYSIZE(symbolHooks));
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

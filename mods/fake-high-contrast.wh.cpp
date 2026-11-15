// ==WindhawkMod==
// @id              fake-high-contrast
// @name            Fake High Contrast
// @description     Fake High Contrast status for apps
// @version         1.0.0
// @author          Ingan121
// @github          https://github.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         *
// @exclude         explorer.exe
// @exclude         dwm.exe
// ==/WindhawkMod==
// hooking explorer makes it change the real HiContrast status every time user switches back to current desktop (e.g. after UAC or Ctrl+Alt+Del)
// hooking dwm adds undesirable HiContrast borders

// ==WindhawkModReadme==
/*
# Fake High Contrast
* Makes applications think as if high contrast mode is enabled (disabled), even if it's not.
* Useful if you want more applications to follow your Win32 msstyles / classic schemes. Even UWP apps!
## Known issues
* UIRibbon is not affected at all.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fakeoff: false
  $name: Fake high contrast is disabled
  $description: Enable this if you use a real high contrast theme. This will make apps think that high contrast is disabled.
*/
// ==/WindhawkModSettings==

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t pOriginalSystemParametersInfoW;
BOOL WINAPI SystemParametersInfoW_Hook(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
)
{
    if (uiAction == SPI_GETHIGHCONTRAST)
    {
        DWORD flag = Wh_GetIntSetting(L"fakeoff") ? HCF_AVAILABLE : HCF_HIGHCONTRASTON;
        HIGHCONTRAST info = { .cbSize = sizeof(info), .dwFlags = flag };
        *(HIGHCONTRAST*)pvParam = info;
        return TRUE;
    }

    return pOriginalSystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)SystemParametersInfoW, (void*)SystemParametersInfoW_Hook, (void**)&pOriginalSystemParametersInfoW);
    return TRUE;
}

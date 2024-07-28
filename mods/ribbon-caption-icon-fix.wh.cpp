// ==WindhawkMod==
// @id              ribbon-caption-icon-fix
// @name            UIRibbon Caption Icon Fix
// @description     Fixes the position of the caption icon in UIRibbon.
// @version         1.0.1
// @author          Taniko Yamamoto
// @author:ja       山本たにこ
// @github          https://github.com/YukisCoffee
// @include         mspaint.exe
// @include         explorer.exe
// @include         wordpad.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UIRibbon Caption Icon Fix

*Copyright (C) 2024 Taniko Yamamoto. All rights reserved.* Please do not use in your projects without express
permission of the copyright holder.

Fixes the position of the caption icon in UIRibbon, which is ordinarily broken since Windows 10, due to
window frames becoming only 1 pixel wide. Whenever ribbons are displayed on a custom theme with wider
borders, the icon will have extra indentation, which is not desirable.

![Preview](https://raw.githubusercontent.com/YukisCoffee/images/main/uiribbon%20fix.png)
*/
// ==/WindhawkModReadme==

#include <libloaderapi.h>

int (WINAPI *GetSystemMetrics_orig)(int);
int WINAPI GetSystemMetrics_hook(int nIndex)
{
    switch (nIndex)
    {
        case SM_CXFRAME:
            return 0;
    }

    return GetSystemMetrics_orig(nIndex);
}

int (WINAPI *GetSystemMetricsForDpi_orig)(int, int);
int WINAPI GetSystemMetricsForDpi_hook(int nIndex, int dpi)
{
    switch (nIndex)
    {
        case SM_CXFRAME:
            return 0;
    }

    return GetSystemMetricsForDpi_orig(nIndex, dpi);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    if (!user32)
    {
        Wh_Log(L"Failed to load user32 module!");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)GetProcAddress(user32, "GetSystemMetrics"),
        (void *)GetSystemMetrics_hook,
        (void **)&GetSystemMetrics_orig
    ))
    {
        Wh_Log(L"Failed to hook GetSystemMetrics");
    }

    if (!Wh_SetFunctionHook(
        (void *)GetProcAddress(user32, "GetSystemMetricsForDpi"),
        (void *)GetSystemMetricsForDpi_hook,
        (void **)&GetSystemMetricsForDpi_orig
    ))
    {
        Wh_Log(L"Failed to hook GetSystemMetricsForDpi");
    }

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

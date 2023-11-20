// ==WindhawkMod==
// @id              ribbon-caption-icon-fix
// @name            UIRibbon Caption Icon Fix
// @description     Fixes the position of the caption icon in UIRibbon.
// @version         1.0
// @author          Taniko Yamamoto
// @author:ja       山本たにこ
// @github          https://github.com/YukisCoffee
// @include         mspaint.exe
// @include         explorer.exe
// @include         wordpad.exe
// ==/WindhawkMod==

#include <libloaderapi.h>

int (*GetSystemMetrics_orig)(int);
int GetSystemMetrics_hook(int nIndex)
{
    switch (nIndex)
    {
        case SM_CXFRAME:
            return 0;
    }

    return GetSystemMetrics_orig(nIndex);
}

int (*GetSystemMetricsForDpi_orig)(int, int);
int GetSystemMetricsForDpi_hook(int nIndex, int dpi)
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

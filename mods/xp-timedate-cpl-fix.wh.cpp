// ==WindhawkMod==
// @id              xp-timedate-cpl-fix
// @name            XP Time/Date CPL Fix
// @description     Fixes the Windows XP Time/Date CPL's clock display on modern systems
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         rundll32.exe
// @compilerOptions -lgdi32
// @license         GPL-3.0
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# XP Time/Date CPL Fix
The clock in Windows XP's timedate.cpl is bugged and will look different depending
on the screen resolution you use. This mod will make it always look how it does on
640x480 (likely what the Windows 95 developers were testing on when they developed
timedate.cpl).

**Before (2560x1440)**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/xp-timedate-cpl-fix/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/xp-timedate-cpl-fix/after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

thread_local bool g_fInClockCreate = false;

using GetDeviceCaps_t = decltype(&GetDeviceCaps);
GetDeviceCaps_t GetDeviceCaps_orig;
int WINAPI GetDeviceCaps_hook(
    HDC hdc,
    int index
)
{
    if (g_fInClockCreate)
    {
        switch (index)
        {
            case HORZRES:
                return 640;
            case VERTRES:
                return 480;
            case HORZSIZE:
                return 169;
            case VERTSIZE:
                return 127;
        }
    }
    return GetDeviceCaps_orig(hdc, index);
}

void (*ClockCreate_orig)(HWND, struct CLOCKSTR *);
void ClockCreate_hook(
    HWND             hWnd,
    struct CLOCKSTR *np
)
{
    g_fInClockCreate = true;
    ClockCreate_orig(hWnd, np);
    g_fInClockCreate = false;
}

const WindhawkUtils::SYMBOL_HOOK timedateCplHooks[] = {
    {
        {
            L"ClockCreate"
        },
        &ClockCreate_orig,
        ClockCreate_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hTimeDate = LoadLibraryExW(L"timedate.cpl", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTimeDate)
    {
        Wh_Log(L"Failed to load timedate.cpl");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hTimeDate,
        timedateCplHooks,
        ARRAYSIZE(timedateCplHooks)
    ))
    {
        Wh_Log(L"Failed to hook ClockCreate");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)GetDeviceCaps,
        (void *)GetDeviceCaps_hook,
        (void **)&GetDeviceCaps_orig
    ))
    {
        Wh_Log(L"Failed to hook GetDeviceCaps");
        return FALSE;
    }

    return TRUE;
}
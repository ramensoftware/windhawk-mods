// ==WindhawkMod==
// @id              msg-box-font-fix
// @name            Message Box Font Fix
// @description     Fixes the MessageBox font size in 1709+
// @version         1.2
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -lgdi32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# IMPORTANT
Due to a bug with Windhawk's symbols system, this mod will only work on 64-bit applications for now.

# Message Box Font Fix
Starting with Windows 10 1709, message boxes render their font size 1pt less than the
user-defined size.\* You cannot just set this size higher, as many applications still query
it, and will show up with bigger fonts.

This mod fixes that.

**Before:**

![Before](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-before.png)

**After:**

![After](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-after.png)

*\*Microsoft changed the way the font size was calculator for Per-Monitor V2 DPI awareness. It ALWAYS gets
1pt below the font size, even when on a higher DPI. This is because Microsoft decided to do some weird math
instead of just using `SystemParametersInfoW` like a normal person.*
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef HFONT (*GetMessageBoxFontForDpi_t)(UINT);
GetMessageBoxFontForDpi_t GetMessageBoxFontForDpi_orig;
HFONT GetMessageBoxFontForDpi_hook(
    UINT nDpi
)
{
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);

    SystemParametersInfoW(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        0
    );

    return CreateFontIndirectW(&(ncm.lfMessageFont));
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Initializing Message Box Font Fix");

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");

    if (!hUser32)
    {
        MessageBoxW(
            NULL,
            L"Failed to load user32.dll. There is something seriously wrong with your Windows install or Windhawk.",
            L"Windhawk: Message Box Font Fix",
            MB_ICONERROR
        );
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                L"struct HFONT__ * __cdecl GetMessageBoxFontForDpi(unsigned int)"
            },
            (void **)&GetMessageBoxFontForDpi_orig,
            (void *)GetMessageBoxFontForDpi_hook
        }
    };

    bool bHookSuccess = HookSymbols(
        hUser32,
        symbolHooks,
        ARRAYSIZE(symbolHooks)
    );

    if (!bHookSuccess)
    {
        // Many applications will always fail to hook this.
        // Coincidentally, none of these ever use MessageBox.
        // At least for me. Anyways, silently fail to log,
        // we don't want to bombard the user with a million
        // error message boxes.
        Wh_Log(L"Failed to hook GetMessageBoxFontForDpi");
        return FALSE;
    }

    Wh_Log(L"Done initializing Message Box Font Fix");
    return TRUE;
}
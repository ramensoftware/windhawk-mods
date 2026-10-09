// ==WindhawkMod==
// @id              no-taskbar-item-glow
// @name            No Taskbar Item Glow
// @description     Disables the glow hover effect on Windows 7 and 8
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No Taskbar Item Glow
This mod disables the glow effect that is shown when hovering taskbar items
in Windows 7, 8, and 8.1. This is primarily useful if you're going for a
Windows Vista look.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/no-taskbar-item-glow-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/no-taskbar-item-glow-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

HRESULT (*DrawRadialGradient_orig)(HDC, struct tag_TASKBAND_RADIALGRADIENT *);
HRESULT __cdecl DrawRadialGradient_hook(
    HDC hdc,
    struct tag_TASKBAND_RADIALGRADIENT *pGradient
)
{
    return S_OK;
}

const WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
    {
        {
            L"long __cdecl DrawRadialGradient(struct HDC__ *,struct tag_TASKBAND_RADIALGRADIENT *)",
        },
        &DrawRadialGradient_orig,
        DrawRadialGradient_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook DrawRadialGradient");
        return FALSE;
    }

    return TRUE;
}
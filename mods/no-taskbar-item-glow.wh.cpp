// ==WindhawkMod==
// @id              no-taskbar-item-glow
// @name            No Taskbar Item Glow
// @description     Disables the glow hover effect on Windows 7 and 8
// @version         1.0.0
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

#include <versionhelpers.h>
#include <windhawk_utils.h>

typedef void (* CTaskBtnGroup__DrawHotTrackingLight_t)(HDC, void *, const RECT *, int, int, int, int, int, unsigned char);
CTaskBtnGroup__DrawHotTrackingLight_t CTaskBtnGroup__DrawHotTrackingLight_orig;
void __cdecl CTaskBtnGroup__DrawHotTrackingLight_hook(
    HDC           hDC,
    void         *pTaskItem,
    const RECT   *lprc,
    int           i1,
    int           i2,
    int           i3,
    int           i4,
    int           i5,
    unsigned char c
)
{
    return;
}

BOOL Wh_ModInit(void)
{
    if (IsWindows10OrGreater())
    {
        Wh_Log(L"This mod is designed for Windows 8.1 and below");
        return FALSE;
    }

    HMODULE hExplorer = GetModuleHandleW(NULL);
    if (!hExplorer)
    {
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"private: void __cdecl CTaskBtnGroup::_DrawHotTrackingLight(struct HDC__ *,struct ITaskItem *,struct tagRECT const *,int,int,int,int,int,unsigned char)",
        },
        (void **)&CTaskBtnGroup__DrawHotTrackingLight_orig,
        (void *)CTaskBtnGroup__DrawHotTrackingLight_hook
    };

    if (!HookSymbols(
        hExplorer,
        &hook,
        1
    ))
    {
        Wh_Log(L"Failed to hook CTaskBtnGroup::_DrawHotTrackingLight");
        return FALSE;
    }

    return TRUE;
}
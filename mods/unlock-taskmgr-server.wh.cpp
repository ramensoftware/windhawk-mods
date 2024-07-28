// ==WindhawkMod==
// @id              unlock-taskmgr-server
// @name            Unlock Task Manager for Server
// @description     Re-enable features that are normally disabled in Task Manager on Windows Server
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         Taskmgr.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Unlock Task Manager for Server
Some features in Task Manager, such as certain columns in the Processes tab and
the Startup tab are completely disabled in Windows Server. This mod re-enables
those features.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/unlock-taskmgr-server-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/unlock-taskmgr-server-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

bool (THISCALL *RunTimeSettings_IsServer_orig)(void *);
bool THISCALL RunTimeSettings_IsServer_hook(
    void *pThis
)
{
    return false;
}

BOOL Wh_ModInit(void)
{
    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"public: bool "
            STHISCALL
            L" RunTimeSettings::IsServer(void)"
        },
        &RunTimeSettings_IsServer_orig,
        RunTimeSettings_IsServer_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        &hook,
        1
    ))
    {
        Wh_Log(L"Failed to hook RunTimeSettings::IsServer");
        return FALSE;
    }

    return TRUE;
}
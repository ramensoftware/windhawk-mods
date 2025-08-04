// ==WindhawkMod==
// @id              win11-old-taskmgr
// @name            Windows 11 Old Task Manager
// @description     Always use the Windows 8/10 styled Task Manager on Windows 11
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         taskmgr.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Old Task Manager
This mod restores the old Task Manager UI from Windows 8/10 in Windows 11.

![Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win11-old-taskmgr.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

bool (*TaskManager_Core_Settings_IsRejuvenatedViewEnabled_orig)(void *pThis);
bool TaskManager_Core_Settings_IsRejuvenatedViewEnabled_hook(void *pThis)
{
    return false;
}

bool (*TaskManager_Core_Settings_IsDarkModeEnabled_orig)(void *pThis);
bool TaskManager_Core_Settings_IsDarkModeEnabled_hook(void *pThis)
{
    return false;
}

bool (*IsRejuvenatedViewEnabledHelper_orig)(void);
bool IsRejuvenatedViewEnabledHelper_hook(void)
{
    return false;
}

bool (*ShouldTaskManagerUseDarkMode_orig)(void);
bool ShouldTaskManagerUseDarkMode_hook(void)
{
    return false;
}

const WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
    {
        {
            L"public: bool __cdecl TaskManager::Core::Settings::IsRejuvenatedViewEnabled(void)const "
        },
        &TaskManager_Core_Settings_IsRejuvenatedViewEnabled_orig,
        TaskManager_Core_Settings_IsRejuvenatedViewEnabled_hook,
        false
    },
    {
        {
            L"public: bool __cdecl TaskManager::Core::Settings::IsDarkModeEnabled(void)const "
        },
        &TaskManager_Core_Settings_IsDarkModeEnabled_orig,
        TaskManager_Core_Settings_IsDarkModeEnabled_hook,
        false
    },
    {
        {
            L"bool __cdecl IsRejuvenatedViewEnabledHelper(void)"
        },
        &IsRejuvenatedViewEnabledHelper_orig,
        IsRejuvenatedViewEnabledHelper_hook,
        false
    },
    {
        {
            L"bool __cdecl ShouldTaskManagerUseDarkMode(void)"
        },
        &ShouldTaskManagerUseDarkMode_orig,
        ShouldTaskManagerUseDarkMode_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        taskmgrExeHooks,
        ARRAYSIZE(taskmgrExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in taskmgr.exe");
        return FALSE;
    }

    return TRUE;
}
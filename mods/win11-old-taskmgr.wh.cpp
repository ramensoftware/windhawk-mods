// ==WindhawkMod==
// @id              win11-old-taskmgr
// @name            Windows 11 Old Task Manager
// @description     Always use the Windows 8/10 styled Task Manager on Windows 11
// @version         1.1.0
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

#include <strsafe.h>
#include <windhawk_utils.h>

#define DECLARE_HOOK_FUNCTION(RETURN_TYPE, ATTRIBUTES, NAME, ...) \
    RETURN_TYPE (ATTRIBUTES *NAME ## _orig)(__VA_ARGS__);         \
    RETURN_TYPE ATTRIBUTES NAME ## _hook(__VA_ARGS__)

DECLARE_HOOK_FUNCTION(int, WINAPI, wWinMain,
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow
)
{
    size_t cchCommandLine = wcslen(lpCmdLine) + sizeof("/d ");
    LPWSTR pszCommandLine = (LPWSTR)LocalAlloc(LPTR, cchCommandLine * sizeof(WCHAR));
    if (!pszCommandLine)
    {
        return wWinMain_orig(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    StringCchCopyW(pszCommandLine, cchCommandLine, L"/d ");
    StringCchCatW(pszCommandLine, cchCommandLine, lpCmdLine);

    int nRet = wWinMain_orig(hInstance, hPrevInstance, pszCommandLine, nCmdShow);
    LocalFree(pszCommandLine);
    return nRet;
}

const WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
    {
        {
            L"wWinMain"
        },
        &wWinMain_orig,
        wWinMain_hook,
        false
    },
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
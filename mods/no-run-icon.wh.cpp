// ==WindhawkMod==
// @id              no-run-icon
// @name            No Run Icon
// @description     Removes the window icon from the Run dialog, like Windows XP and before.
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No Run Icon
Removes the window icon from the Run dialog, like Windows XP and before.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/no-run-icon-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/no-run-icon-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

DLGPROC RunDlgProc_orig;
INT_PTR CALLBACK RunDlgProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (uMsg == WM_INITDIALOG)
    {
        RunDlgProc_orig(hWnd, uMsg, wParam, lParam);
        SendMessageW(hWnd, WM_SETICON, 0, NULL);
        return 0;
    }
    return RunDlgProc_orig(hWnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
#ifdef _WIN64
            L"__int64 __cdecl RunDlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"int __stdcall RunDlgProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &RunDlgProc_orig,
        RunDlgProc_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hShell32, &hook, 1))
    {
        Wh_Log(L"Failed to hook RunDlgProc");
        return FALSE;
    }

    return TRUE;
}
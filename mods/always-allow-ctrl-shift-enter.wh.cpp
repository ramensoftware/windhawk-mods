// ==WindhawkMod==
// @id              always-allow-ctrl-shift-enter
// @name            Always Allow CTRL+SHIFT+ENTER
// @description     Always allow CTRL+SHIFT+ENTER in the Run dialog
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @license         BSD-3-Clause
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always Allow CTRL+SHIFT+ENTER
The system run dialog (most commonly WIN+R) allows you to press CTRL+SHIFT+ENTER
to run the command as administrator. However, this feature needs to be
explicitly enabled by the program.

This mod makes it always enabled, so that it works everywhere, including but not
limited to:
- Explorer7
- Old versions of Task Manager
*/
// ==/WindhawkModReadme==

// Made up name
#define RFD_CONSENTHOTKEY   0x00000100

int (WINAPI *RunFileDlg_orig)(HWND, HICON, LPCWSTR, LPCWSTR, LPCWSTR, DWORD);
int WINAPI RunFileDlg_hook(
    HWND hwndParent,
    HICON hIcon,
    LPCWSTR pszWorkingDir,
    LPCWSTR pszTitle,
    LPCWSTR pszPrompt,
    DWORD dwFlags)
{
    return RunFileDlg_orig(
        hwndParent,
        hIcon,
        pszWorkingDir,
        pszTitle,
        pszPrompt,
        dwFlags | RFD_CONSENTHOTKEY);
}

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    void *pfnRunFileDlg = (void *)GetProcAddress(hShell32, (LPCSTR)61);
    if (!pfnRunFileDlg)
    {
        Wh_Log(L"Failed to find RunFileDlg");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)pfnRunFileDlg,
        (void *)RunFileDlg_hook,
        (void **)&RunFileDlg_orig))
    {
        Wh_Log(L"Failed to hook RunFileDlg");
        return false;
    }

    return TRUE;
}
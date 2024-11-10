// ==WindhawkMod==
// @id              custom-shutdown-dialog
// @name            Custom Shutdown Dialog
// @description     Override the classic shutdown dialog in Explorer with your own
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Shutdown Dialog
Override the classic shutdown dialog in Explorer
which is invoked with `ALT`+`F4` with your own program.

This also lets you override the action of the "Log Off" button
in Windows XP's Explorer, which on Windows 10 normally instantly logs
the user out.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- exe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Shutdown executable
  $description: Path to executable to open instead of shutdown dialog.
- args: /style classic
  $name: Shutdown arguments
  $description: Arguments to pass to the shutdown executable, if any.
- logoffexe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Logoff executable
  $description: Path to executable to open instead of logoff dialog.
- logoffargs: /logoff /style classic
  $name: Logoff arguments
  $description: Arguments to pass to the logoff executable, if any.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_szShutdownExe, g_szShutdownArgs;
WindhawkUtils::StringSetting g_szLogoffExe, g_szLogoffArgs;

__int64 (*_ShutdownDialogEx_orig)(HWND, int, int, UINT);
__int64 _ShutdownDialogEx_hook(
    HWND hWndParent,
    int  i1,
    int  i2,
    UINT i3
)
{
    ShellExecuteW(
        hWndParent,
        L"open",
        g_szShutdownExe,
        g_szShutdownArgs,
        NULL,
        SW_NORMAL
    );
    return 0;
}

void (*LogoffWindowsDialog_orig)(HWND);
void LogoffWindowsDialog_hook(HWND hWndParent)
{
    ShellExecuteW(
        hWndParent,
        L"open",
        g_szLogoffExe,
        g_szLogoffArgs,
        NULL,
        SW_NORMAL
    );
}

WindhawkUtils::SYMBOL_HOOK shutdownuxDllHooks[] = {
    {
        {
            L"static  _ShutdownDialogEx()"
        },
        &_ShutdownDialogEx_orig,
        _ShutdownDialogEx_hook,
        false
    }
};\

WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"LogoffWindowsDialog"
        },
        &LogoffWindowsDialog_orig,
        LogoffWindowsDialog_hook,
        false
    }
};

void LoadSettings(void)
{
    g_szShutdownExe = WindhawkUtils::StringSetting::make(L"exe");
    g_szShutdownArgs = WindhawkUtils::StringSetting::make(L"args");
    g_szLogoffExe = WindhawkUtils::StringSetting::make(L"logoffexe");
    g_szLogoffArgs = WindhawkUtils::StringSetting::make(L"logoffargs");
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hShutdownUx = LoadLibraryW(L"shutdownux.dll");
    if (!hShutdownUx)
    {
        Wh_Log(L"Failed to load shutdownux.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShutdownUx,
        shutdownuxDllHooks,
        ARRAYSIZE(shutdownuxDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook _ShutdownDialogEx");
        return FALSE;
    }

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook LogoffWindowsDialog");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
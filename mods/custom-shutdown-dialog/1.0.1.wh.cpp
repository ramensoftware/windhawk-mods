// ==WindhawkMod==
// @id              custom-shutdown-dialog
// @name            Custom Shutdown Dialog
// @description     Override the classic shutdown dialog in Explorer with your own
// @version         1.0.1
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
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- exe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Executable
  $description: Path to executable to open instead of dialog.
- args: /style classic
  $name: Arguments
  $description: Arguments to pass to the executable, if any.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_szExe, g_szArgs;

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
        g_szExe,
        g_szArgs,
        NULL,
        SW_NORMAL
    );
    return 0;
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
};

void LoadSettings(void)
{
    g_szExe = WindhawkUtils::StringSetting::make(L"exe");
    g_szArgs = WindhawkUtils::StringSetting::make(L"args");
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

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
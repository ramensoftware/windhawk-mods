// ==WindhawkMod==
// @id              custom-shutdown-dialog
// @name            Custom Shutdown Dialog
// @description     Override the classic shutdown dialog in Explorer with your own
// @version         1.0.0
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

**This mod will only work on Windows 10 or greater and Windhawk v1.4 or greater.**
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
#include <versionhelpers.h>

WindhawkUtils::StringSetting g_szExe, g_szArgs;

typedef __int64 (* _ShutdownDialogEx_t)(HWND, int, int, UINT);
_ShutdownDialogEx_t _ShutdownDialogEx_orig;
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

void LoadSettings(void)
{
    g_szExe = WindhawkUtils::StringSetting::make(L"exe");
    g_szArgs = WindhawkUtils::StringSetting::make(L"args");
}

BOOL Wh_ModInit(void)
{
    if (!IsWindows10OrGreater())
    {
        Wh_Log(L"This mod was designed for Windows 10 and up.");
        return FALSE;
    }

    LoadSettings();

    HMODULE hShutdownUx = LoadLibraryW(L"shutdownux.dll");
    if (!hShutdownUx)
    {
        Wh_Log(L"Failed to load shutdownux.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"static  _ShutdownDialogEx()"
        },
        &_ShutdownDialogEx_orig,
        _ShutdownDialogEx_hook
    };

    if (!HookSymbols(hShutdownUx, &hook, 1))
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
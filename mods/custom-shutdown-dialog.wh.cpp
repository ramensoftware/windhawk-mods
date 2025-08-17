// ==WindhawkMod==
// @id              custom-shutdown-dialog
// @name            Custom Shutdown Dialog
// @description     Override the classic shutdown dialog in Explorer with your own
// @version         1.2.0
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
- args: ""
  $name: Shutdown arguments
  $description: Arguments to pass to the shutdown executable, if any.
- logoffexe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Logoff executable
  $description: Path to executable to open instead of logoff dialog.
- logoffargs: /logoff
  $name: Logoff arguments
  $description: Arguments to pass to the logoff executable, if any.
- disconnectexe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Disconnect executable
  $description: Path to executable to open instead of disconnect dialog.
- disconnectargs: /disconnect
  $name: Disconnect arguments
  $description: Arguments to pass to the disconnect executable, if any.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_spszShutdownExe,   g_spszShutdownArgs;
WindhawkUtils::StringSetting g_spszLogoffExe,     g_spszLogoffArgs;
WindhawkUtils::StringSetting g_spszDisconnectExe, g_spszDisconnectArgs;

void (*ExitWindowsDialog_orig)(HWND);
void ExitWindowsDialog_hook(HWND hwndParent)
{
    ShellExecuteW(
        hwndParent,
        L"open",
        g_spszShutdownExe,
        g_spszShutdownArgs,
        NULL,
        SW_NORMAL
    );
}

void (*LogoffWindowsDialog_orig)(HWND);
void LogoffWindowsDialog_hook(HWND hwndParent)
{
    ShellExecuteW(
        hwndParent,
        L"open",
        g_spszLogoffExe,
        g_spszLogoffArgs,
        NULL,
        SW_NORMAL
    );
}

void (*DisconnectWindowsDialog_orig)(HWND);
void DisconnectWindowsDialog_hook(HWND hwndParent)
{
    ShellExecuteW(
        hwndParent,
        L"open",
        g_spszDisconnectExe,
        g_spszDisconnectArgs,
        NULL,
        SW_NORMAL
    );
}

void LoadSettings(void)
{
    g_spszShutdownExe    = WindhawkUtils::StringSetting::make(L"exe");
    g_spszShutdownArgs   = WindhawkUtils::StringSetting::make(L"args");
    g_spszLogoffExe      = WindhawkUtils::StringSetting::make(L"logoffexe");
    g_spszLogoffArgs     = WindhawkUtils::StringSetting::make(L"logoffargs");
    g_spszDisconnectExe  = WindhawkUtils::StringSetting::make(L"disconnectexe");
    g_spszDisconnectArgs = WindhawkUtils::StringSetting::make(L"disconnectargs");
}

#define HOOK(NAME, ORDINAL)                                   \
    FARPROC NAME = GetProcAddress(hShell32, (LPCSTR)ORDINAL); \
    if (!NAME)                                                \
    {                                                         \
        Wh_Log(L"Failed to get address of %s", L ## #NAME);   \
        return FALSE;                                         \
    }                                                         \
                                                              \
    if (!Wh_SetFunctionHook(                                  \
        (void *)NAME,                                         \
        (void *)NAME ## _hook,                                \
        (void **)&NAME ## _orig                               \
    ))                                                        \
    {                                                         \
        Wh_Log(L"Failed to hook %s", L ## #NAME);             \
        return FALSE;                                         \
    }

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    HOOK(ExitWindowsDialog,        60);
    HOOK(LogoffWindowsDialog,      54);
    HOOK(DisconnectWindowsDialog, 254);

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
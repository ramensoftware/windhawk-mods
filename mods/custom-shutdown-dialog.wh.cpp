// ==WindhawkMod==
// @id              custom-shutdown-dialog
// @name            Custom Shutdown Dialog
// @description     Override the classic shutdown dialog in Explorer with your own
// @version         1.2.1
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

## Safe Launch Feature (Rectify11 Support)
If you are using **Rectify11** or a similar modern dialog, you might notice the window closes immediately upon opening. 
Enable the **Safe Launch** setting in the mod options. This makes the mod wait until you release `ALT`+`F4` before launching the executable, preventing the self-close bug.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- exe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Shutdown executable
  $description: Path to executable to open instead of shutdown dialog.
- args: ""
  $name: Shutdown arguments
- logoffexe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Logoff executable
- logoffargs: /logoff
  $name: Logoff arguments
- disconnectexe: C:\Classic\ClassicShutdown\ClassicShutdown.exe
  $name: Disconnect executable
- disconnectargs: /disconnect
  $name: Disconnect arguments
- safeLaunch: false
  $name: Safe Launch (Wait for Alt+F4 release)
  $description: Prevents the custom dialog from closing immediately by waiting for keys to be released. Useful for Rectify11.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <string>

WindhawkUtils::StringSetting g_spszShutdownExe,   g_spszShutdownArgs;
WindhawkUtils::StringSetting g_spszLogoffExe,     g_spszLogoffArgs;
WindhawkUtils::StringSetting g_spszDisconnectExe, g_spszDisconnectArgs;
bool g_bSafeLaunch;

void LaunchExecutable(PCWSTR exePath, PCWSTR args)
{
    if (!exePath || !*exePath) return;

    if (g_bSafeLaunch)
    {
        // Wait as long as Alt (VK_MENU) or F4 is pressed
        while ((GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_F4) & 0x8000))
        {
            Sleep(50);
        }
        Sleep(100); // Small buffer wait
    }

    // Set working directory (to prevent missing icons and resources)
    std::wstring sExe = exePath;
    std::wstring sDir = L"";
    size_t found = sExe.find_last_of(L"\\");
    if (found != std::wstring::npos) {
        sDir = sExe.substr(0, found);
    }

    ShellExecuteW(
        NULL,
        L"open",
        exePath,
        args,
        sDir.empty() ? NULL : sDir.c_str(),
        SW_NORMAL
    );
}

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------

void (*ExitWindowsDialog_orig)(HWND);
void ExitWindowsDialog_hook(HWND hwndParent)
{
    LaunchExecutable(g_spszShutdownExe, g_spszShutdownArgs);
}

void (*LogoffWindowsDialog_orig)(HWND);
void LogoffWindowsDialog_hook(HWND hwndParent)
{
    LaunchExecutable(g_spszLogoffExe, g_spszLogoffArgs);
}

void (*DisconnectWindowsDialog_orig)(HWND);
void DisconnectWindowsDialog_hook(HWND hwndParent)
{
    LaunchExecutable(g_spszDisconnectExe, g_spszDisconnectArgs);
}

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void LoadSettings(void)
{
    g_spszShutdownExe    = WindhawkUtils::StringSetting::make(L"exe");
    g_spszShutdownArgs   = WindhawkUtils::StringSetting::make(L"args");
    g_spszLogoffExe      = WindhawkUtils::StringSetting::make(L"logoffexe");
    g_spszLogoffArgs     = WindhawkUtils::StringSetting::make(L"logoffargs");
    g_spszDisconnectExe  = WindhawkUtils::StringSetting::make(L"disconnectexe");
    g_spszDisconnectArgs = WindhawkUtils::StringSetting::make(L"disconnectargs");
    g_bSafeLaunch        = (bool)Wh_GetIntSetting(L"safeLaunch");
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

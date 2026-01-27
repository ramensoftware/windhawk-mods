// ==WindhawkMod==
// @id              custom-shutdown-dialog-rectify11
// @name            Custom Shutdown Dialog (Rectify11)
// @description     Replaces the classic Windows shutdown dialogs with a custom executable. Specifically optimized for Rectify11 to fix the "immediate self-close" bug by waiting for Alt+F4 release.
// @version         1.1.0
// @author          aubymori & osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Shutdown Dialog (Safe Launch)

This mod redirects the classic Windows shutdown, logoff, and disconnect dialogs to a custom executable. It is essential for users of **Rectify11 Modern Shutdown**.

## The Problem with Rectify11 / Modern Shutdown
When you trigger the shutdown dialog using `Alt+F4`, the new modern dialog often launches and immediately closes itself. This happens because the custom application detects that you are still holding `Alt+F4` (which is the command to close a window) and closes itself instantly.

## The Solution (Safe Launch)
This mod includes a **Key Wait** mechanism. When you press `Alt+F4`:
1.  The mod intercepts the command.
2.  It actively **waits** until you release the `Alt` and `F4` keys.
3.  Only after the keys are released, it launches the custom shutdown dialog.
4.  It also sets the correct working directory to prevent missing icons/resources.

## Configuration
Go to the **Settings** tab and point the `exe` paths to your Rectify11 installation (usually in `C:\Windows\System32\ModernShutDownWindows.exe` or similar).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shutdownSettings:
  - exe: C:\Windows\System32\ModernShutDownWindows.exe
    $name: Shutdown executable path
    $description: Path to your custom app (e.g., Rectify11's ModernShutdown.exe).
  - args: ""
    $name: Shutdown arguments
- logoffSettings:
  - exe: C:\Windows\System32\ModernShutDownWindows.exe
    $name: Logoff executable path
  - args: /logoff
    $name: Logoff arguments
- disconnectSettings:
  - exe: C:\Windows\System32\ModernShutDownWindows.exe
    $name: Disconnect executable path
  - args: /disconnect
    $name: Disconnect arguments
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <string>
#include <shellapi.h>

WindhawkUtils::StringSetting g_shutdownExe, g_shutdownArgs;
WindhawkUtils::StringSetting g_logoffExe, g_logoffArgs;
WindhawkUtils::StringSetting g_disconnectExe, g_disconnectArgs;

/**
 * Waits for the user to release Alt/F4 keys and then launches the executable.
 * This prevents the target app from receiving the close signal immediately.
 */
void SafeLaunch(PCWSTR exePath, PCWSTR args)
{
    // Safety check: ensure path is valid
    if (!exePath || !*exePath) return;

    // STEP 1: Wait for keys to be released
    // 0x8000 bit indicates the key is currently down.
    while ((GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_F4) & 0x8000))
    {
        Sleep(50); 
    }

    // STEP 2: Small buffer to ensure input state is clear
    Sleep(100);

    // STEP 3: Determine working directory
    std::wstring sExe = exePath;
    std::wstring sDir = L"";
    size_t found = sExe.find_last_of(L"\\");
    if (found != std::wstring::npos) {
        sDir = sExe.substr(0, found);
    }

    // STEP 4: Launch the application
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
    SafeLaunch(g_shutdownExe.get(), g_shutdownArgs.get());
}

void (*LogoffWindowsDialog_orig)(HWND);
void LogoffWindowsDialog_hook(HWND hwndParent)
{
    SafeLaunch(g_logoffExe.get(), g_logoffArgs.get());
}

void (*DisconnectWindowsDialog_orig)(HWND);
void DisconnectWindowsDialog_hook(HWND hwndParent)
{
    SafeLaunch(g_disconnectExe.get(), g_disconnectArgs.get());
}

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void LoadSettings(void)
{
    g_shutdownExe    = WindhawkUtils::StringSetting::make(L"shutdownSettings.exe");
    g_shutdownArgs   = WindhawkUtils::StringSetting::make(L"shutdownSettings.args");
    
    g_logoffExe      = WindhawkUtils::StringSetting::make(L"logoffSettings.exe");
    g_logoffArgs     = WindhawkUtils::StringSetting::make(L"logoffSettings.args");
    
    g_disconnectExe  = WindhawkUtils::StringSetting::make(L"disconnectSettings.exe");
    g_disconnectArgs = WindhawkUtils::StringSetting::make(L"disconnectSettings.args");
}

#define HOOK_ORDINAL(NAME, ORDINAL)                                   \
    FARPROC NAME = GetProcAddress(hShell32, (LPCSTR)ORDINAL);         \
    if (!NAME)                                                        \
    {                                                                 \
        Wh_Log(L"Failed to get address of %s", L ## #NAME);           \
        return FALSE;                                                 \
    }                                                                 \
    if (!Wh_SetFunctionHook(                                          \
        (void *)NAME,                                                 \
        (void *)NAME ## _hook,                                        \
        (void **)&NAME ## _orig                                       \
    ))                                                                \
    {                                                                 \
        Wh_Log(L"Failed to hook %s", L ## #NAME);                     \
        return FALSE;                                                 \
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

    // Ordinal 60: ExitWindowsDialog (Shutdown)
    HOOK_ORDINAL(ExitWindowsDialog, 60);
    
    // Ordinal 54: LogoffWindowsDialog (Logoff)
    HOOK_ORDINAL(LogoffWindowsDialog, 54);
    
    // Ordinal 254: DisconnectWindowsDialog (Disconnect User)
    HOOK_ORDINAL(DisconnectWindowsDialog, 254);

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
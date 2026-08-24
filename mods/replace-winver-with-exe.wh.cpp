// ==WindhawkMod==
// @id              replace-winver-with-exe
// @name            Replace Winver With EXE
// @description     Replaces the Winver dialog with a custom executable
// @version         1.0
// @author          winui64
// @github          https://github.com/winui64
// @include         winver.exe
// @compilerOptions -lshell32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Redirect Winver to an .exe file.
This is a simple mod that intercepts the traditional Winver dialog and replaces it with a custom .exe file of your choice. This can be a Winver replacement like Fluver or Modern Winver for example. 

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replacement_exe: ""
  $name: Replacement executable
  $description: Absolute path to the executable to launch instead of the Winver dialog.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_replacementExe;

using ShellAboutW_t = decltype(&ShellAboutW);
ShellAboutW_t ShellAboutW_orig = nullptr;

void LoadSettings()
{
    g_replacementExe =
        WindhawkUtils::StringSetting::make(L"replacement_exe");
}

bool IsWinverProcess()
{
    WCHAR szPath[MAX_PATH];

    if (!GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath)))
        return false;

    LPCWSTR fileName = PathFindFileNameW(szPath);

    if (!fileName)
        return false;

    return _wcsicmp(fileName, L"winver.exe") == 0;
}

INT WINAPI ShellAboutW_hook(
    HWND hWnd,
    LPCWSTR szApp,
    LPCWSTR szOtherStuff,
    HICON hIcon)
{

    const wchar_t* exePath = g_replacementExe.get();

    // If no EXE configured, fall back to original behavior
    if (!exePath || !exePath[0])
    {
        return ShellAboutW_orig(
            hWnd,
            szApp,
            szOtherStuff,
            hIcon
        );
    }

    DWORD attrib = GetFileAttributesW(exePath);

    if (attrib == INVALID_FILE_ATTRIBUTES ||
        (attrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        MessageBoxW(
            hWnd,
            L"The configured executable path is invalid.",
            L"Replace Winver With EXE",
            MB_ICONERROR
        );

        return ShellAboutW_orig(
            hWnd,
            szApp,
            szOtherStuff,
            hIcon
        );
    }

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = hWnd;
    sei.lpFile = exePath;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei))
    {
        WCHAR szError[512];

        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            GetLastError(),
            0,
            szError,
            ARRAYSIZE(szError),
            nullptr
        );

        MessageBoxW(
            hWnd,
            szError,
            L"Failed to launch replacement executable",
            MB_ICONERROR
        );

        return ShellAboutW_orig(
            hWnd,
            szApp,
            szOtherStuff,
            hIcon
        );
    }

    if (sei.hProcess)
    {
        CloseHandle(sei.hProcess);
    }

    // Prevent original Winver dialog
    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    if (!Wh_SetFunctionHook(
        (void*)ShellAboutW,
        (void*)ShellAboutW_hook,
        (void**)&ShellAboutW_orig))
    {
        Wh_Log(L"Failed to hook ShellAboutW");
        return FALSE;
    }

    Wh_Log(L"Hook installed successfully");

    return TRUE;
}

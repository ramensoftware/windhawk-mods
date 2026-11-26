// ==WindhawkMod==
// @id              auto-square-corners
// @name            Auto Square Corners
// @description     Makes apps use square corners continuously.
// @version         1.3
// @author          James Talion
// @include         *.exe
// @include         windhawk.exe
// @exclude         fontdrvhost.exe
// @exclude         sihost.exe
// @exclude         runtimebroker.exe
// @exclude         securityhealthservice.exe
// @exclude         securityhealthsystray.exe
// @exclude         shellexperiencehost.exe
// @exclude         searchhost.exe
// @exclude         searchindexer.exe
// @exclude         textinputhost.exe
// @exclude         lockapp.exe
// @exclude         applicationframehost.exe
// @exclude         taskhostw.exe
// @exclude         wermgr.exe
// @exclude         werfault.exe
// @exclude         dllhost.exe
// @exclude         winlogon.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==

/*
# Auto Square Corners

Automatically changes supported application windows to use square corners.

## Features

- Automatically applies square corners.
- Uses the official DWM API.
- Excludes critical system processes.

## Known limitations

- Existing windows keep their current corner style when the mod is disabled.
- Restart applications or Explorer to restore the default rounded corners.
*/

// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

void SetSquareCorners(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return;

    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_DONOTROUND;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &pref,
        sizeof(pref)
    );
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM)
{
    if (!IsWindowVisible(hwnd))
        return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == GetCurrentProcessId())
    {
        SetSquareCorners(hwnd);
    }

    return TRUE;
}

void ApplyCorners()
{
    EnumWindows(EnumWindowsProc, 0);
}

DWORD WINAPI CornerLoop(LPVOID)
{
    while (true)
    {
        ApplyCorners();

        // Check every half a second
        Sleep(500);
    }

    return 0;
}

BOOL Wh_ModInit()
{
    CreateThread(
        nullptr,
        0,
        CornerLoop,
        nullptr,
        0,
        nullptr
    );

    return TRUE;
}

void Wh_ModAfterInit()
{
    ApplyCorners();
}

void Wh_ModUninit()
{
}

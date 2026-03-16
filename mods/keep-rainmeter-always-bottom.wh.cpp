// ==WindhawkMod==
// @id              keep-rainmeter-always-bottom
// @name            Keep Rainmeter Always on Desktop
// @description     Keeps Rainmeter windows to stay on desktop.
// @version         1.0
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         explorer.exe
// @compilerOptions -luser32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Rainmeter Always on Desktop

Keeps Rainmeter windows to stay on desktop.

## Compatibility
- Windows 10 and Windows 11
- Targets `explorer.exe` only
*/
// ==/WindhawkModReadme==

#include <minwindef.h>
#include <windef.h>
#include <winuser.h>

HWINEVENTHOOK g_eventHook = nullptr;

static DWORD GetRainmeterPid()
{
    HWND hw = FindWindowW(L"RainmeterMeterWindow", nullptr);
    if (!hw) hw = FindWindowW(L"RainmeterTrayClass", nullptr);
    if (!hw) return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(hw, &pid);
    return pid;
}

static DWORD g_rainmeterPid = 0;

static BOOL CALLBACK PushWindowToBottom(HWND hw, LPARAM)
{
    DWORD wpid = 0;
    GetWindowThreadProcessId(hw, &wpid);
    if (wpid == g_rainmeterPid)
        SetWindowPos(hw, HWND_BOTTOM, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    return TRUE;
}

static void PushRainmeterToBottom()
{
    g_rainmeterPid = GetRainmeterPid();
    if (!g_rainmeterPid) return;
    EnumWindows(PushWindowToBottom, 0);
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK, DWORD event, HWND hwnd,
    LONG, LONG, DWORD, DWORD)
{
    if (event == EVENT_SYSTEM_FOREGROUND)
    {
        PushRainmeterToBottom();
    }
}

BOOL Wh_ModInit()
{

    g_eventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT);

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_eventHook)
    {
        UnhookWinEvent(g_eventHook);
        g_eventHook = nullptr;
    }
}

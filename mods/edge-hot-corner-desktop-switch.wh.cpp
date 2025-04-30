// ==WindhawkMod==
// @id              edge-hot-corner-desktop-switch
// @name            Edge Hot-Corners Desktop Switch
// @description     Switches virtual desktops when the mouse hovers at the left or right screen edge for a moment.
// @version         0.2
// @author          Sanskar Prasad
// @github          https://github.com/sanskarprasad
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- thresholdMs: 1000
  $name: Time threshold at edge (ms)
  $description: >
    Milliseconds the cursor must stay in the hot zone before
    switching desktops.

- edgeWidth: 5
  $name: Detection edge width (px)
  $description: >
    Width in pixels of the left/right screen hot zone.
*/
// ==/WindhawkModSettings==

#include <Windows.h>

// #include <windhawk-sdk.h>

struct {
    int thresholdMs;
    int edgeWidth;
} settings;

volatile bool g_Running = false;
HANDLE g_hThread = nullptr;

//--------------------------------------------------------------------------------
// CALLBACK for EnumWindows to find the taskbar window in this process
//--------------------------------------------------------------------------------
static BOOL CALLBACK EnumTaskbarWndProc(HWND hWnd, LPARAM lParam)
{
    DWORD pid = 0;
    WCHAR className[32] = { 0 };
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() &&
        GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0)
    {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE; // stop enumeration
    }
    return TRUE; // continue
}

static HWND FindCurrentProcessTaskbarWnd()
{
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumTaskbarWndProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

void LoadSettings() {
    settings.thresholdMs = Wh_GetIntSetting(L"thresholdMs");
    settings.edgeWidth   = Wh_GetIntSetting(L"edgeWidth");
}

void SimulateDesktopSwitch(int direction) {
    // Force-focus the taskbar so SendInput will be accepted
    HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd) {
        SetForegroundWindow(hTaskbarWnd);
    }

    WORD vkDir = (direction < 0 ? VK_LEFT : VK_RIGHT);
    INPUT inputs[6] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_CONTROL;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = vkDir;
    inputs[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = vkDir;
    inputs[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    inputs[4].type = INPUT_KEYBOARD;
    inputs[4].ki.wVk = VK_CONTROL;
    inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[5].type = INPUT_KEYBOARD;
    inputs[5].ki.wVk = VK_LWIN;
    inputs[5].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    UINT sent = SendInput(_countof(inputs), inputs, sizeof(INPUT));
    Wh_Log(
        L"EdgeHotCorner: SimulateDesktopSwitch(%s): SendInput injected %u events",
        (direction < 0 ? L"Left" : L"Right"), sent);
}

DWORD WINAPI MonitorThread(LPVOID) {
    POINT pt = { 0 }, lastPt = { -1, -1 };
    int zone = 0;          // 0 = none, 1 = left, 2 = right
    DWORD enterTime = 0;

    while (g_Running) {
        Sleep(zone == 0 ? 100 : 20);

        if (!GetCursorPos(&pt))
            continue;

        if (zone == 0 && pt.x == lastPt.x && pt.y == lastPt.y)
            continue;
        lastPt = pt;

        int vsX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vsW = GetSystemMetrics(SM_CXVIRTUALSCREEN);

        if (pt.x <= vsX + 100 || pt.x >= vsX + vsW - 100) {
            Wh_Log(
                L"EdgeHotCorner: Cursor x = %d, virtualScreenStartX = %d, width = %d",
                pt.x, vsX, vsW);
        }

        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
        if (!hMon) {
            zone = 0;
            continue;
        }

        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(hMon, &mi);
        bool atLeft  = (pt.x <= mi.rcMonitor.left + settings.edgeWidth);
        bool atRight = (pt.x >= mi.rcMonitor.right - settings.edgeWidth);
        int newZone  = atLeft ? 1 : (atRight ? 2 : 0);

        if (newZone != 0) {
            if (zone != newZone) {
                zone = newZone;
                enterTime = GetTickCount();
            }
            else if (GetTickCount() - enterTime >= (DWORD)settings.thresholdMs) {
                Wh_Log(
                    L"EdgeHotCorner: Switching desktop: %s",
                    (zone == 1 ? L"Previous" : L"Next"));
                SimulateDesktopSwitch(zone == 1 ? -1 : +1);
                zone = 0;
            }
        } else {
            zone = 0;
        }
    }

    return 0;
}

BOOL Wh_ModInit() {
    if (!FindCurrentProcessTaskbarWnd()) {
        HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (hTaskbarWnd) {
            // The taskbar exists, but it's not owned by the current process.
            return FALSE;
        }
    }

    Wh_Log(L"EdgeHotCorner: Initializing mod");
    LoadSettings();

    g_Running = true;
    g_hThread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"EdgeHotCorner: Failed to create monitor thread");
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"EdgeHotCorner: Uninitializing mod");
    g_Running = false;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    Wh_Log(L"EdgeHotCorner: Mod shut down");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"EdgeHotCorner: Settings changed; reloading");
    LoadSettings();
}

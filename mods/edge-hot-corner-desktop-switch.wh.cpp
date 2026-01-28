// ==WindhawkMod==
// @id              edge-hot-corner-desktop-switch
// @name            Edge Hot-Corners Desktop Switch
// @description     Switch virtual desktops by hovering at screen edges (left/right). Includes center height limit to avoid accidental triggers.
// @version         1.3
// @author          Sanskar Prasad
// @github          https://github.com/sanskarprasad
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- thresholdMs: 700
  $name: Time threshold at edge (ms)
- edgeWidth: 5
  $name: Detection edge width (px)
- centerHeight: 400
  $name: Active center height (px)
  $description: Height of the vertical active band for hot corners.
*/
// ==/WindhawkModSettings==

#include <Windows.h>
#include <Shobjidl.h>

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------
struct {
    int thresholdMs;
    int edgeWidth;
    int centerHeight;
} settings;

volatile bool g_Running = false;
HANDLE g_hThread = nullptr;

// -----------------------------------------------------------------------------
// Settings Loader
// -----------------------------------------------------------------------------
void LoadSettings() {
    settings.thresholdMs = Wh_GetIntSetting(L"thresholdMs");
    settings.edgeWidth   = Wh_GetIntSetting(L"edgeWidth");
    settings.centerHeight = Wh_GetIntSetting(L"centerHeight");
}

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------
bool IsInFullScreenMode() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (FAILED(SHQueryUserNotificationState(&state)))
        return false;

    return (state == QUNS_RUNNING_D3D_FULL_SCREEN);
}

// -----------------------------------------------------------------------------
// Simulate desktop switch (Ctrl + Win + Arrow)
// -----------------------------------------------------------------------------
void SimulateDesktopSwitch(int direction) {
    if (IsInFullScreenMode()) return;

    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) SetForegroundWindow(hTaskbar);

    INPUT inputs[6] = {};
    WORD vkArrow = (direction < 0 ? VK_LEFT : VK_RIGHT);

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_CONTROL;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = vkArrow;
    inputs[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = vkArrow;
    inputs[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    inputs[4].type = INPUT_KEYBOARD;
    inputs[4].ki.wVk = VK_CONTROL;
    inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[5].type = INPUT_KEYBOARD;
    inputs[5].ki.wVk = VK_LWIN;
    inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(_countof(inputs), inputs, sizeof(INPUT));
}

// -----------------------------------------------------------------------------
// Monitor Thread
// -----------------------------------------------------------------------------
DWORD WINAPI MonitorThread(LPVOID) {
    const int vsLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vsRight = vsLeft + vsWidth;
    const int vsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    const int vsTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

    POINT pt;
    int zone = 0;
    DWORD enterTime = 0;

    while (g_Running) {
        Sleep(25);
        if (!GetCursorPos(&pt)) continue;

        int centerTop = vsTop + (vsHeight - settings.centerHeight) / 2;
        int centerBottom = centerTop + settings.centerHeight;

        bool inVerticalCenter = (pt.y >= centerTop && pt.y <= centerBottom);
        bool atLeft = (pt.x <= vsLeft + settings.edgeWidth);
        bool atRight = (pt.x >= vsRight - settings.edgeWidth);
        int newZone = (inVerticalCenter && atLeft) ? 1 : (inVerticalCenter && atRight ? 2 : 0);

        if (newZone != 0) {
            if (zone != newZone) {
                zone = newZone;
                enterTime = GetTickCount();
            } else if (GetTickCount() - enterTime >= (DWORD)settings.thresholdMs) {
                SimulateDesktopSwitch(zone == 1 ? -1 : +1);
                zone = 0;
            }
        } else {
            zone = 0;
        }
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Init / Uninit
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings();

    g_Running = true;
    g_hThread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_Running = false;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// ==WindhawkMod==
// @id              edge-hot-corner-desktop-switch
// @name            Edge Hot-Corners Desktop Switch
// @description     Switches virtual desktops when the mouse hovers at the left or right screen edge for a moment.
// @version         0.1
// @author          You
// @github          https://github.com/sanskarprasad
// @homepage        https://your-personal-homepage.example.com/edge-hot-corner-desktop-switch
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

void LoadSettings() {
    settings.thresholdMs = Wh_GetIntSetting(L"thresholdMs");
    settings.edgeWidth = Wh_GetIntSetting(L"edgeWidth");
}

void SimulateDesktopSwitch(int direction) {
    // direction: -1 = previous (Win+Ctrl+Left), +1 = next (Win+Ctrl+Right)
    WORD vkDir = (direction < 0 ? VK_LEFT : VK_RIGHT);
    INPUT inputs[6] = {};

    // Win down (extended)
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    // Ctrl down (not extended)
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_CONTROL;

    // Arrow down (extended)
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = vkDir;
    inputs[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    // Arrow up  (extended + key-up)
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = vkDir;
    inputs[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    // Ctrl up
    inputs[4].type = INPUT_KEYBOARD;
    inputs[4].ki.wVk = VK_CONTROL;
    inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;

    // Win up   (extended + key-up)
    inputs[5].type = INPUT_KEYBOARD;
    inputs[5].ki.wVk = VK_LWIN;
    inputs[5].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    // Send them all in one go
    UINT sent = SendInput(_countof(inputs), inputs, sizeof(INPUT));
    Wh_Log(L"EdgeHotCorner",
           L"SimulateDesktopSwitch(%s): SendInput injected %u events",
           (direction < 0 ? L"Left" : L"Right"), sent);
}

DWORD WINAPI MonitorThread(LPVOID) {
    POINT pt;
    int zone = 0;  // 0 = none, 1 = left, 2 = right
    DWORD enterTime = 0;

    while (g_Running) {
        Sleep(20);

        if (!GetCursorPos(&pt))
            continue;

        int vsX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vsW = GetSystemMetrics(SM_CXVIRTUALSCREEN);

        // Only log if near an edge (within 100 pixels from left or right virtual screen)
        if (pt.x <= vsX + 100 || pt.x >= (vsX + vsW - 100)) {
            Wh_Log(L"EdgeHotCorner",
                   L"Cursor x = %d, virtualScreenStartX = %d, width = %d",
                   pt.x, vsX, vsW);
        }

        // Get the monitor under the cursor
        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
        if (!hMon) {
            zone = 0;
            continue;
        }

        // Query its full bounds
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hMon, &mi);
        int left = mi.rcMonitor.left;
        int right = mi.rcMonitor.right;

        bool atLeft = (pt.x <= left + settings.edgeWidth);
        bool atRight = (pt.x >= right - settings.edgeWidth);

        int newZone = atLeft ? 1 : (atRight ? 2 : 0);

        if (newZone != 0) {
            if (zone != newZone) {
                // Just entered a hot zone
                zone = newZone;
                enterTime = GetTickCount();
            } else if (GetTickCount() - enterTime >= (DWORD)settings.thresholdMs) {
                // Held in zone long enough: trigger switch
                Wh_Log(L"EdgeHotCorner", L"Switching desktop: %s",
                       (zone == 1 ? L"Previous" : L"Next"));
                SimulateDesktopSwitch(zone == 1 ? -1 : +1);
                // Reset so we don’t retrigger immediately
                zone = 0;
            }
        } else {
            // Left the hot zone; reset
            zone = 0;
        }
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"EdgeHotCorner", L"Initializing mod");
    LoadSettings();

    // Start the monitoring thread
    g_Running = true;
    g_hThread = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"EdgeHotCorner", L"Failed to create monitor thread");
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"EdgeHotCorner", L"Uninitializing mod");
    g_Running = false;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    Wh_Log(L"EdgeHotCorner", L"Mod shut down");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"EdgeHotCorner", L"Settings changed; reloading");
    LoadSettings();
}

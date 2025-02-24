// ==WindhawkMod==
// @id              hide-taskbar-monitor
// @name            Hide Taskbar on a Specific Monitor
// @description     Hides the taskbar only on a user-specified monitor.
// @version         1.0.1
// @author          Repilee
// @github          https://github.com/Repilee
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar on a Specific Monitor
This Windhawk mod hides the Windows taskbar **only** on the specified monitor while keeping it visible on others. Great for displaying Rainmeter on display info purposes, hiding the useless taskbar.

## Features:
- **Hides taskbar on a single monitor** while leaving it visible on others.
- **Supports Windows 10, and Windows 11**. Older than Windows 10 systems *should* theoretically work but not guaranteed.
- **Restores the taskbar when the mod is disabled**.

## How to Use:
1. Install and enable this mod.
2. Configure the settings:
   - **Monitor Index**: Select which monitor should have the taskbar hidden.
3. The taskbar on the selected monitor will disappear.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitorIndex: 1
  $name: Monitor Index
  $description: >-
    Choose which monitor should have the taskbar hidden.
    1 = Primary monitor, 2 = Secondary monitor, etc.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>

int g_monitorIndex = 1;
std::wstring g_taskbarMode = L"24H2";
bool g_running = true; // Background thread control

// Struct to store monitor info and assigned taskbar
struct MonitorData {
    MONITORINFOEX info;
    HWND taskbarHandle;
    bool isPrimary;
};

// Global storage for monitors
std::vector<MonitorData> g_monitors;

// Function to wait for Explorer and Taskbar to load
void WaitForExplorer() {
    int attempts = 0;
    while (attempts < 30) {  // Wait up to 30 seconds for Explorer
        HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
        if (taskbar) {
            Wh_Log(L"Explorer is ready, proceeding...");
            return;
        }
        Wh_Log(L"Explorer not ready yet, retrying...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        attempts++;
    }
    Wh_Log(L"Explorer did not load within 30 seconds, exiting.");
}

// Callback function for `EnumDisplayMonitors` to get all monitors
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM) {
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMonitor, &mi)) {
        MonitorData md;
        md.info = mi;
        md.taskbarHandle = NULL;
        md.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0; // Check if primary monitor
        g_monitors.push_back(md);
    }
    return TRUE;
}

// Find the closest monitor to a taskbar based on position
size_t FindClosestMonitorIndex(RECT taskbarRect) {
    size_t bestMatch = 0;
    int smallestDistance = INT_MAX;

    for (size_t i = 0; i < g_monitors.size(); i++) {
        RECT monitorRect = g_monitors[i].info.rcMonitor;
        int dx = std::abs((monitorRect.left + monitorRect.right) / 2 - (taskbarRect.left + taskbarRect.right) / 2);
        int dy = std::abs((monitorRect.top + monitorRect.bottom) / 2 - (taskbarRect.top + taskbarRect.bottom) / 2);
        int distance = dx + dy;

        if (distance < smallestDistance) {
            smallestDistance = distance;
            bestMatch = i;
        }
    }

    return bestMatch;
}

// Get all monitors and correctly assign taskbars
void GetMonitorsAndTaskbars() {
    g_monitors.clear();
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

    // Sort so primary monitor is always first (index = 1 in settings)
    std::sort(g_monitors.begin(), g_monitors.end(), [](const MonitorData& a, const MonitorData& b) {
        return a.isPrimary > b.isPrimary;
    });

    // Find primary taskbar
    HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (taskbar) {
        RECT taskbarRect;
        GetWindowRect(taskbar, &taskbarRect);
        size_t monitorIndex = FindClosestMonitorIndex(taskbarRect);
        g_monitors[monitorIndex].taskbarHandle = taskbar;
    }

    // Find secondary taskbars and assign them correctly
    HWND hwnd = NULL;
    while ((hwnd = FindWindowEx(NULL, hwnd, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        RECT taskbarRect;
        GetWindowRect(hwnd, &taskbarRect);
        size_t monitorIndex = FindClosestMonitorIndex(taskbarRect);
        g_monitors[monitorIndex].taskbarHandle = hwnd;
    }

    // Debug logs
    for (size_t i = 0; i < g_monitors.size(); i++) {
        Wh_Log(L"Monitor %d (Primary: %s) - Left: %ld, Top: %ld, Taskbar Handle: %p", 
            i + 1, g_monitors[i].isPrimary ? L"YES" : L"NO", 
            g_monitors[i].info.rcMonitor.left, g_monitors[i].info.rcMonitor.top, g_monitors[i].taskbarHandle);
    }
}

// Convert Windows Display Settings' monitor index (1-based) to API index (0-based)
int GetSelectedMonitorIndex() {
    GetMonitorsAndTaskbars();
    int adjustedIndex = g_monitorIndex - 1; // Convert to zero-based index

    if (adjustedIndex >= 0 && adjustedIndex < static_cast<int>(g_monitors.size())) {
        return adjustedIndex;
    }
    return -1; // Invalid index
}

// Hide taskbar only on the selected monitor
void UpdateTaskbarVisibility() {
    int selectedMonitor = GetSelectedMonitorIndex();
    if (selectedMonitor == -1) {
        Wh_Log(L"Invalid monitor selection.");
        return;
    }

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (g_monitors[i].taskbarHandle) {
            if (i == static_cast<size_t>(selectedMonitor)) {
                Wh_Log(L"Hiding taskbar on monitor index: %d", selectedMonitor + 1);
                ShowWindow(g_monitors[i].taskbarHandle, SW_HIDE);
            } else {
                ShowWindow(g_monitors[i].taskbarHandle, SW_SHOW);
            }
        }
    }
}

// Background thread: Monitors taskbars after login or restart
void TaskbarMonitorThread() {
    WaitForExplorer(); // Wait until Explorer is ready
    while (g_running) {
        UpdateTaskbarVisibility();
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Check every 3 seconds, sometimes Explorer shows for whatever reason
    }
}

// Hook for monitor changes
void CALLBACK OnMonitorChange(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    UpdateTaskbarVisibility();
}

HWINEVENTHOOK g_hook = NULL;
std::thread g_taskbarThread;

// Load Windhawk settings
void LoadSettings() {
    g_monitorIndex = Wh_GetIntSetting(L"monitorIndex");
    g_taskbarMode = Wh_GetStringSetting(L"taskbarMode");
}

// Initialize the mod
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Hide Taskbar on Specific Monitor mod...");
    LoadSettings();
    g_hook = SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, NULL, OnMonitorChange, 0, 0, WINEVENT_OUTOFCONTEXT);
    
    // Start the taskbar monitoring thread
    g_running = true;
    g_taskbarThread = std::thread(TaskbarMonitorThread);
    
    UpdateTaskbarVisibility();
    return TRUE;
}

// Unload mod
void Wh_ModUninit() {
    Wh_Log(L"Unloading mod...");
    if (g_hook) UnhookWinEvent(g_hook);
    
    // Stop the taskbar monitoring thread
    g_running = false;
    if (g_taskbarThread.joinable()) g_taskbarThread.join();

    // Restore all taskbars
    for (auto& monitor : g_monitors) {
        if (monitor.taskbarHandle) ShowWindow(monitor.taskbarHandle, SW_SHOW);
    }
}

// Apply new settings when changed
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed.");
    LoadSettings();
    UpdateTaskbarVisibility();
}

// ==WindhawkMod==
// @id              crystaldiskinfo-smart-auto-refresh
// @name            CrystalDiskInfo Smart Auto-Refresh
// @description     Temporarily pauses the Auto-Refresh function whenever the application's main window is visible
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         DiskInfo32.exe
// @include         DiskInfo64.exe
// @include         DiskInfoA64.exe
// @include         DiskInfo32A.exe
// @include         DiskInfo64A.exe
// @include         DiskInfoA64A.exe
// @include         DiskInfo32K.exe
// @include         DiskInfo64K.exe
// @include         DiskInfoA64K.exe
// @include         DiskInfo32S.exe
// @include         DiskInfo64S.exe
// @include         DiskInfoA64S.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# CrystalDiskInfo Smart Auto-Refresh
CrystalDiskInfo includes an optional Auto-Refresh feature that updates disk
information at specified intervals, which is useful for background monitoring
while the application is minimized to the system tray. However, when the
application's main window is visible, automatic refreshes can interfere with the
analysis of current disk status, as shifting attribute values make tracking
specific metrics more difficult.

This mod temporarily pauses the Auto-Refresh function by blocking its disk
polling cycle whenever the application's main window is visible, allowing for an
uninterrupted analysis of current disk status.

The Auto-Refresh function resumes once the application is minimized to the
taskbar or system tray.

**Note:** If CrystalDiskInfo is already running when the mod is loaded, pick one
of the following options to activate it:
* Restart the application completely.
* Select an interval in **Function** → **Auto Refresh**, including the one
  currently set.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::mutex g_timerMutex;
std::unordered_map<HWND, std::unordered_set<UINT_PTR>> g_timerMap;

// Subclass procedure
LRESULT CALLBACK AutoRefreshSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam, DWORD_PTR dwRefData)
{
    if (uMsg == WM_TIMER)
    {
        bool isAutoRefreshTimer = false;
        {
            std::lock_guard<std::mutex> lock(g_timerMutex);
            auto it = g_timerMap.find(hWnd);
            if (it != g_timerMap.end() && it->second.count(wParam))
            {
                isAutoRefreshTimer = true;
            }
        }

        if (isAutoRefreshTimer)
        {
            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            if (!hRootWnd)
            {
                hRootWnd = hWnd;
            }

            if (IsWindowVisible(hRootWnd) && !IsIconic(hRootWnd))
            {
                return 0;
            }
        }
    }
    else if (uMsg == WM_NCDESTROY)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
            AutoRefreshSubclassProc);
        std::lock_guard<std::mutex> lock(g_timerMutex);
        g_timerMap.erase(hWnd);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Hook for SetTimer
using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse,
    TIMERPROC lpTimerFunc)
{
    // CrystalDiskInfo's minimum Auto-Refresh interval is 1 minute
    // This threshold avoids interfering with unrelated functions by filtering
    // out high-frequency timers, ensuring only the application's disk polling
    // cycle is targeted.
    if (hWnd && uElapse >= 60000)
    {
        bool isSubclassRequired = false;
        {
            std::lock_guard<std::mutex> lock(g_timerMutex);
            auto [it, inserted] = g_timerMap.try_emplace(hWnd);
            if (inserted)
            {
                isSubclassRequired = true;
            }
            it->second.insert(nIDEvent);
        }

        if (isSubclassRequired)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                AutoRefreshSubclassProc, 0);
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// Hook for KillTimer
using KillTimer_t = decltype(&KillTimer);
KillTimer_t KillTimer_Original;
BOOL WINAPI KillTimer_Hook(HWND hWnd, UINT_PTR nIDEvent)
{
    if (hWnd)
    {
        std::lock_guard<std::mutex> lock(g_timerMutex);
        auto it = g_timerMap.find(hWnd);
        if (it != g_timerMap.end())
        {
            it->second.erase(nIDEvent);
        }
    }
    return KillTimer_Original(hWnd, nIDEvent);
}

// Mod initialization
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    WindhawkUtils::SetFunctionHook(
        SetTimer,
        SetTimer_Hook,
        &SetTimer_Original
    );

    WindhawkUtils::SetFunctionHook(
        KillTimer,
        KillTimer_Hook,
        &KillTimer_Original
    );

    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    std::vector<HWND> windowsToDetach;
    {
        std::lock_guard<std::mutex> lock(g_timerMutex);
        for (const auto& timerEntry : g_timerMap)
        {
            windowsToDetach.push_back(timerEntry.first);
        }
        g_timerMap.clear();
    }

    for (HWND hWnd : windowsToDetach)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
            AutoRefreshSubclassProc);
    }
}

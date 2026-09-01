// ==WindhawkMod==
// @id              uwp-clean-safe
// @name            Safe UWP Process Cleanup
// @description     Safely clean up idle ApplicationFrameHost processes without interfering with app launching.
// @version         1.1
// @author          Abhinav Chitrey
// @github          https://github.com/chitreyAbhinav
// @include         ApplicationFrameHost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Safe UWP Process Cleanup

Safely cleans up unused ApplicationFrameHost.exe processes.

## How it works

- Only targets ApplicationFrameHost.exe.
- Checks the process once when the mod loads.
- Checks again when a window belonging to the process is closed.
- Never terminates the process while it owns a visible window.
- Performs a final safety check immediately before cleanup.

## Processes NOT targeted

- RuntimeBroker.exe
- TextInputHost.exe
- ctfmon.exe
- SystemSettingsAdminFlows.exe

The conservative design is intended to avoid interfering with
Windows applications while reducing leftover ApplicationFrameHost
processes.

## Credits

Inspired by the original "Clean up UWP processes" Windhawk mod
by Alcatel.

This version uses a substantially different and more conservative
cleanup approach and targets only ApplicationFrameHost.exe.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <atomic>

// ============================================================
// GLOBAL VARIABLES
// ============================================================

std::atomic<bool> g_stopWorker{ false };

HANDLE g_stopEvent = nullptr;
HANDLE g_checkEvent = nullptr;
HANDLE g_workerThread = nullptr;

// ============================================================
// ORIGINAL FUNCTION
// ============================================================

using DestroyWindow_t = BOOL(WINAPI*)(HWND hWnd);

DestroyWindow_t DestroyWindow_Original = nullptr;

// ============================================================
// WINDOW ENUMERATION DATA
// ============================================================

struct WindowCheckData
{
    DWORD processId;
    bool hasVisibleWindow;
};

// ============================================================
// ENUMERATE WINDOWS
// ============================================================

BOOL CALLBACK EnumWindowsProc(
    HWND hwnd,
    LPARAM lParam)
{
    WindowCheckData* data =
        reinterpret_cast<WindowCheckData*>(lParam);

    DWORD processId = 0;

    GetWindowThreadProcessId(
        hwnd,
        &processId);

    // Ignore windows belonging to other processes.
    if (processId != data->processId)
        return TRUE;

    // If this process owns a visible window,
    // it is currently being used.
    if (IsWindowVisible(hwnd))
    {
        data->hasVisibleWindow = true;

        // We already found a visible window.
        return FALSE;
    }

    return TRUE;
}

// ============================================================
// CHECK WHETHER CURRENT PROCESS HAS A VISIBLE WINDOW
// ============================================================

bool CurrentProcessHasVisibleWindow()
{
    WindowCheckData data{};

    data.processId = GetCurrentProcessId();
    data.hasVisibleWindow = false;

    EnumWindows(
        EnumWindowsProc,
        reinterpret_cast<LPARAM>(&data));

    return data.hasVisibleWindow;
}

// ============================================================
// CHECK WHETHER CLEANUP IS SAFE
// ============================================================

bool IsSafeToCleanup()
{
    // If ApplicationFrameHost currently owns a visible window,
    // it is still being used by a UWP application.
    if (CurrentProcessHasVisibleWindow())
        return false;

    return true;
}

// ============================================================
// REQUEST CLEANUP CHECK
// ============================================================

void RequestCleanupCheck()
{
    if (g_checkEvent)
    {
        SetEvent(g_checkEvent);
    }
}

// ============================================================
// DESTROYWINDOW HOOK
// ============================================================

BOOL WINAPI DestroyWindow_Hook(HWND hWnd)
{
    // Let Windows perform the actual window destruction first.
    BOOL result =
        DestroyWindow_Original(hWnd);

    // Only request a check after the window has actually
    // been destroyed.
    //
    // The worker performs the safety check outside of the
    // hooked function to avoid terminating the process from
    // inside DestroyWindow().
    RequestCleanupCheck();

    return result;
}

// ============================================================
// WORKER THREAD
// ============================================================

DWORD WINAPI WorkerThread(LPVOID)
{
    HANDLE events[2] =
    {
        g_stopEvent,
        g_checkEvent
    };

    while (!g_stopWorker)
    {
        DWORD result =
            WaitForMultipleObjects(
                2,
                events,
                FALSE,
                INFINITE);

        // Mod was disabled/unloaded.
        if (result == WAIT_OBJECT_0)
            break;

        // Cleanup check requested.
        if (result == WAIT_OBJECT_0 + 1)
        {
            // Reset the event before performing the check.
            ResetEvent(g_checkEvent);

            // ------------------------------------------------
            // First safety check.
            // ------------------------------------------------

            if (!IsSafeToCleanup())
                continue;

            // ------------------------------------------------
            // Final safety check.
            //
            // This reduces the chance of cleaning up the
            // process just as another window is appearing.
            // ------------------------------------------------

            if (!IsSafeToCleanup())
                continue;

            // ------------------------------------------------
            // No visible window exists.
            //
            // This mod is loaded only into
            // ApplicationFrameHost.exe.
            // ------------------------------------------------

            ExitProcess(0);
        }
    }

    return 0;
}

// ============================================================
// WINDHAWK INITIALIZATION
// ============================================================

BOOL Wh_ModInit()
{
    g_stopWorker = false;

    // Create event used to stop the worker.
    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_stopEvent)
        return FALSE;

    // Create event used to request a cleanup check.
    g_checkEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_checkEvent)
    {
        CloseHandle(g_stopEvent);

        g_stopEvent = nullptr;

        return FALSE;
    }

    // Start the worker thread.
    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThread,
            nullptr,
            0,
            nullptr);

    if (!g_workerThread)
    {
        CloseHandle(g_checkEvent);
        CloseHandle(g_stopEvent);

        g_checkEvent = nullptr;
        g_stopEvent = nullptr;

        return FALSE;
    }

    // ========================================================
    // INITIAL CHECK
    // ========================================================

    // Check the current state once when the mod loads.
    RequestCleanupCheck();

    // ========================================================
    // INSTALL DESTROYWINDOW HOOK
    // ========================================================

    if (!Wh_SetFunctionHook(
            (void*)DestroyWindow,
            (void*)DestroyWindow_Hook,
            (void**)&DestroyWindow_Original))
    {
        g_stopWorker = true;

        SetEvent(g_stopEvent);

        WaitForSingleObject(
            g_workerThread,
            3000);

        CloseHandle(g_workerThread);
        CloseHandle(g_checkEvent);
        CloseHandle(g_stopEvent);

        g_workerThread = nullptr;
        g_checkEvent = nullptr;
        g_stopEvent = nullptr;

        return FALSE;
    }

    return TRUE;
}

// ============================================================
// WINDHAWK UNINITIALIZATION
// ============================================================

void Wh_ModUninit()
{
    // Tell the worker to stop.
    g_stopWorker = true;

    // Wake the worker immediately.
    if (g_stopEvent)
    {
        SetEvent(g_stopEvent);
    }

    // Wait for the worker to finish.
    if (g_workerThread)
    {
        WaitForSingleObject(
            g_workerThread,
            3000);

        CloseHandle(g_workerThread);

        g_workerThread = nullptr;
    }

    // Close the check event.
    if (g_checkEvent)
    {
        CloseHandle(g_checkEvent);

        g_checkEvent = nullptr;
    }

    // Close the stop event.
    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);

        g_stopEvent = nullptr;
    }
}

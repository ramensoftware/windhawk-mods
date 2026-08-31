// ==WindhawkMod==
// @id              uwp-clean-safe
// @name            Safe UWP Process Cleanup
// @description     Safely clean up idle ApplicationFrameHost processes without interfering with app launching.
// @version         1.0
// @author          Abhinav Chitrey
// @github          https://github.com/chitreyAbhinav
// @include         ApplicationFrameHost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Safe UWP Process Cleanup

Safely cleans up idle ApplicationFrameHost.exe processes.

## How it works

- Only targets ApplicationFrameHost.exe.
- Waits two minutes after the process starts.
- Checks every 15 seconds.
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
// ==WindhawkModReadme==

#include <Windows.h>

#include <atomic>


// ============================================================
// CONFIGURATION
// ============================================================

// Wait this long after ApplicationFrameHost.exe starts before
// cleanup is considered.
//
// 120000 ms = 2 minutes.
constexpr DWORD GRACE_PERIOD_MS = 120000;


// Check every 15 seconds after the grace period.
constexpr DWORD CHECK_INTERVAL_MS = 15000;


// ============================================================
// GLOBAL VARIABLES
// ============================================================

std::atomic<bool> g_stopWorker{ false };

HANDLE g_stopEvent = nullptr;

HANDLE g_workerThread = nullptr;


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


    // If this process owns a visible window, it is currently
    // being used and must not be terminated.
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
    // --------------------------------------------------------
    // Safety check:
    //
    // If ApplicationFrameHost currently owns a visible window,
    // it is being used by an application.
    // --------------------------------------------------------

    if (CurrentProcessHasVisibleWindow())
        return false;


    return true;
}


// ============================================================
// WORKER THREAD
// ============================================================

DWORD WINAPI WorkerThread(LPVOID)
{
    // Record when the mod started inside this process.
    const DWORD startTime = GetTickCount();


    // ========================================================
    // GRACE PERIOD
    // ========================================================

    while (!g_stopWorker)
    {
        DWORD elapsed =
            GetTickCount() - startTime;


        if (elapsed >= GRACE_PERIOD_MS)
            break;


        DWORD remaining =
            GRACE_PERIOD_MS - elapsed;


        DWORD waitTime =
            (remaining < CHECK_INTERVAL_MS)
                ? remaining
                : CHECK_INTERVAL_MS;


        DWORD result =
            WaitForSingleObject(
                g_stopEvent,
                waitTime);


        // Mod was disabled/unloaded.
        if (result == WAIT_OBJECT_0)
            return 0;
    }


    // ========================================================
    // CLEANUP LOOP
    // ========================================================

    while (!g_stopWorker)
    {
        // Wait before checking again.
        //
        // Using an event rather than a normal Sleep() allows
        // the thread to stop immediately when the mod is
        // disabled.
        DWORD result =
            WaitForSingleObject(
                g_stopEvent,
                CHECK_INTERVAL_MS);


        // Mod was disabled/unloaded.
        if (result == WAIT_OBJECT_0)
            break;


        // ----------------------------------------------------
        // First safety check.
        // ----------------------------------------------------

        if (!IsSafeToCleanup())
            continue;


        // ----------------------------------------------------
        // FINAL SAFETY CHECK.
        //
        // This reduces the chance of terminating the process
        // just as an application is opening a window.
        // ----------------------------------------------------

        if (!IsSafeToCleanup())
            continue;


        // ----------------------------------------------------
        // No visible window was found.
        //
        // Terminate ONLY this ApplicationFrameHost process.
        // ----------------------------------------------------

        ExitProcess(0);
    }


    return 0;
}


// ============================================================
// WINDHAWK INITIALIZATION
// ============================================================

BOOL Wh_ModInit()
{
    g_stopWorker = false;


    // Create a manual-reset event used to stop the worker.
    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);


    if (!g_stopEvent)
        return FALSE;


    // Start the worker thread.
    //
    // Wh_ModInit() is used instead of DllMain(), avoiding
    // thread creation from inside DLL_PROCESS_ATTACH.
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
        CloseHandle(g_stopEvent);

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


    // Close the stop event.
    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);

        g_stopEvent = nullptr;
    }
}

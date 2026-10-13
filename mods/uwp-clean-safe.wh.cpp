// ==WindhawkMod==
// @id              uwp-clean-safe
// @name            Safe UWP Process Cleanup
// @description     Conservatively cleans up idle ApplicationFrameHost processes without interfering with app launching.
// @version         1.2
// @author          Abhinav Chitrey
// @github          https://github.com/chitreyAbhinav
// @include         ApplicationFrameHost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Safe UWP Process Cleanup

Conservatively cleans up unused ApplicationFrameHost.exe processes.

## How it works

- Only targets ApplicationFrameHost.exe.
- Does not terminate the process when the mod is first loaded.
- Waits for an ApplicationFrameWindow to be destroyed before considering cleanup.
- Waits 30 seconds after the window destruction.
- Checks that no ApplicationFrameWindow remains.
- Checks that no other top-level window belonging to the process remains.
- Performs an additional 5-second safety check before cleanup.
- Cancels cleanup if further window activity is detected.
- Never terminates the process from inside the hooked function.
- Stops the worker before Windhawk removes the hook.

The implementation deliberately favors leaving an idle
ApplicationFrameHost.exe process running over terminating a
process that may still be needed.

## Processes NOT targeted

- RuntimeBroker.exe
- TextInputHost.exe
- ctfmon.exe
- SystemSettingsAdminFlows.exe

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
// CONFIGURATION
// ============================================================

// Time to wait after an ApplicationFrameWindow is destroyed
// before performing the first cleanup evaluation.
//
// A long delay is intentional. False negatives (leaving an idle
// process alive) are preferable to false positives.
constexpr DWORD QUIET_PERIOD_MS = 30000;

// Additional delay before the final safety check.
constexpr DWORD FINAL_CHECK_DELAY_MS = 5000;

// ============================================================
// GLOBAL STATE
// ============================================================

std::atomic<bool> g_stopWorker{false};
std::atomic<bool> g_unloading{false};

// Becomes true after an ApplicationFrameWindow has been
// observed in this process.
std::atomic<bool> g_frameWindowObserved{false};

// Incremented whenever an ApplicationFrameWindow is destroyed.
//
// The worker uses this as a generation counter so that multiple
// rapid window events cannot overwrite a single stored HWND.
std::atomic<unsigned long long> g_windowGeneration{0};

HANDLE g_stopEvent = nullptr;
HANDLE g_checkEvent = nullptr;
HANDLE g_workerThread = nullptr;

// ============================================================
// WINDOW ENUMERATION DATA
// ============================================================

struct WindowSearchData
{
    DWORD processId;
    bool hasApplicationFrameWindow;
    bool hasAnyTopLevelWindow;
};

// ============================================================
// ENUMERATE WINDOWS
// ============================================================

BOOL CALLBACK EnumWindowsProc(
    HWND hwnd,
    LPARAM lParam)
{
    WindowSearchData* data =
        reinterpret_cast<WindowSearchData*>(lParam);

    DWORD processId = 0;

    GetWindowThreadProcessId(
        hwnd,
        &processId);

    // Ignore windows belonging to other processes.
    if (processId != data->processId)
    {
        return TRUE;
    }

    // Any top-level window is treated conservatively as evidence
    // that the process may still be in use.
    data->hasAnyTopLevelWindow = true;

    wchar_t className[256] = {};

    int length =
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className));

    if (length <= 0)
    {
        return TRUE;
    }

    if (wcscmp(
            className,
            L"ApplicationFrameWindow") == 0)
    {
        data->hasApplicationFrameWindow = true;

        // We already know everything needed.
        return FALSE;
    }

    return TRUE;
}

// ============================================================
// GET CURRENT WINDOW STATE
// ============================================================

WindowSearchData GetCurrentWindowState()
{
    WindowSearchData data{};

    data.processId = GetCurrentProcessId();
    data.hasApplicationFrameWindow = false;
    data.hasAnyTopLevelWindow = false;

    EnumWindows(
        EnumWindowsProc,
        reinterpret_cast<LPARAM>(&data));

    if (data.hasApplicationFrameWindow)
    {
        g_frameWindowObserved = true;
    }

    return data;
}

// ============================================================
// CHECK WHETHER CLEANUP IS SAFE
// ============================================================

bool IsSafeToCleanup()
{
    // Do not clean up a process for which we have never observed
    // an ApplicationFrameWindow.
    if (!g_frameWindowObserved)
    {
        return false;
    }

    WindowSearchData state =
        GetCurrentWindowState();

    // A current ApplicationFrameWindow means the host is still
    // associated with an application frame.
    if (state.hasApplicationFrameWindow)
    {
        return false;
    }

    // Be conservative with ANY remaining top-level window.
    //
    // This intentionally does not use IsWindowVisible().
    // Hidden or transitional windows are still treated as
    // evidence that the process may be in use.
    if (state.hasAnyTopLevelWindow)
    {
        return false;
    }

    return true;
}

// ============================================================
// REQUEST CLEANUP CHECK
// ============================================================

void RequestCleanupCheck()
{
    if (g_unloading)
    {
        return;
    }

    HANDLE checkEvent = g_checkEvent;

    if (!checkEvent)
    {
        return;
    }

    SetEvent(checkEvent);
}

// ============================================================
// DESTROYWINDOW HOOK
// ============================================================

using DestroyWindow_t = decltype(&DestroyWindow);

DestroyWindow_t DestroyWindow_Original = nullptr;

BOOL WINAPI DestroyWindow_Hook(HWND hWnd)
{
    // Identify the window before destroying it.
    wchar_t className[256] = {};

    int length =
        GetClassNameW(
            hWnd,
            className,
            ARRAYSIZE(className));

    bool isApplicationFrameWindow =
        length > 0 &&
        wcscmp(
            className,
            L"ApplicationFrameWindow") == 0;

    // Always call the original function first.
    BOOL result =
        DestroyWindow_Original(hWnd);

    // Only a successfully destroyed ApplicationFrameWindow
    // starts a cleanup evaluation.
    //
    // Cleanup itself is never performed from this hook.
    if (result &&
        isApplicationFrameWindow &&
        !g_unloading)
    {
        g_frameWindowObserved = true;

        g_windowGeneration.fetch_add(
            1,
            std::memory_order_acq_rel);

        RequestCleanupCheck();
    }

    return result;
}

// ============================================================
// WAIT FOR STOP OR TIMEOUT
// ============================================================

bool WaitForStopOrTimeout(DWORD milliseconds)
{
    HANDLE stopEvent = g_stopEvent;

    if (!stopEvent)
    {
        return true;
    }

    DWORD result =
        WaitForSingleObject(
            stopEvent,
            milliseconds);

    return result == WAIT_OBJECT_0;
}

// ============================================================
// CHECK WHETHER WORKER SHOULD STOP
// ============================================================

bool StopRequested()
{
    if (g_stopWorker ||
        g_unloading)
    {
        return true;
    }

    HANDLE stopEvent = g_stopEvent;

    if (!stopEvent)
    {
        return true;
    }

    return WaitForSingleObject(
        stopEvent,
        0) == WAIT_OBJECT_0;
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

    while (!StopRequested())
    {
        DWORD result =
            WaitForMultipleObjects(
                ARRAYSIZE(events),
                events,
                FALSE,
                INFINITE);

        // Stop event.
        if (result == WAIT_OBJECT_0)
        {
            break;
        }

        // Unexpected result.
        if (result != WAIT_OBJECT_0 + 1)
        {
            continue;
        }

        // Consume the cleanup request.
        ResetEvent(g_checkEvent);

        if (StopRequested())
        {
            break;
        }

        // ----------------------------------------------------
        // Capture current lifecycle generation.
        // ----------------------------------------------------

        unsigned long long generationAtStart =
            g_windowGeneration.load(
                std::memory_order_acquire);

        // ----------------------------------------------------
        // QUIET PERIOD
        // ----------------------------------------------------

        if (WaitForStopOrTimeout(QUIET_PERIOD_MS))
        {
            break;
        }

        if (StopRequested())
        {
            break;
        }

        // ----------------------------------------------------
        // Check whether another ApplicationFrameWindow was
        // destroyed while we were waiting.
        // ----------------------------------------------------

        unsigned long long generationAfterWait =
            g_windowGeneration.load(
                std::memory_order_acquire);

        if (generationAfterWait != generationAtStart)
        {
            // New activity occurred. Start over using the
            // newest cleanup notification.
            continue;
        }

        // ----------------------------------------------------
        // FIRST SAFETY CHECK
        // ----------------------------------------------------

        if (!IsSafeToCleanup())
        {
            continue;
        }

        // ----------------------------------------------------
        // FINAL DELAY
        // ----------------------------------------------------

        if (WaitForStopOrTimeout(
                FINAL_CHECK_DELAY_MS))
        {
            break;
        }

        if (StopRequested())
        {
            break;
        }

        // ----------------------------------------------------
        // Check again for lifecycle activity.
        // ----------------------------------------------------

        unsigned long long generationBeforeFinalCheck =
            g_windowGeneration.load(
                std::memory_order_acquire);

        if (generationBeforeFinalCheck !=
            generationAtStart)
        {
            continue;
        }

        // ----------------------------------------------------
        // FINAL SAFETY CHECK
        // ----------------------------------------------------

        if (!IsSafeToCleanup())
        {
            continue;
        }

        // ----------------------------------------------------
        // FINAL GENERATION CHECK
        // ----------------------------------------------------

        unsigned long long finalGeneration =
            g_windowGeneration.load(
                std::memory_order_acquire);

        if (finalGeneration != generationAtStart)
        {
            continue;
        }

        // ----------------------------------------------------
        // FINAL UNLOAD CHECK
        // ----------------------------------------------------

        if (StopRequested())
        {
            break;
        }

        // ----------------------------------------------------
        // CLEANUP
        // ----------------------------------------------------
        //
        // The mod is loaded only into ApplicationFrameHost.exe.
        //
        // Cleanup is reached only after:
        //
        // 1. An ApplicationFrameWindow was observed.
        // 2. Its destruction triggered the evaluation.
        // 3. The process remained quiet for 30 seconds.
        // 4. No ApplicationFrameWindow exists.
        // 5. No other top-level window exists.
        // 6. An additional 5-second verification passed.
        // 7. No new relevant window destruction was observed.
        // 8. The mod is not being unloaded.
        //
        // If any condition is uncertain, cleanup is skipped.
        //

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
    g_unloading = false;
    g_frameWindowObserved = false;
    g_windowGeneration = 0;

    // --------------------------------------------------------
    // CREATE STOP EVENT
    // --------------------------------------------------------

    g_stopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_stopEvent)
    {
        return FALSE;
    }

    // --------------------------------------------------------
    // CREATE CLEANUP EVENT
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // START WORKER
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // INITIAL STATE OBSERVATION
    // --------------------------------------------------------
    //
    // If the mod is loaded into an already-running process,
    // observe its current state.
    //
    // IMPORTANT:
    // This does NOT request cleanup.
    //

    GetCurrentWindowState();

    // --------------------------------------------------------
    // INSTALL DESTROYWINDOW HOOK
    // --------------------------------------------------------

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(DestroyWindow),
            reinterpret_cast<void*>(DestroyWindow_Hook),
            reinterpret_cast<void**>(
                &DestroyWindow_Original)))
    {
        g_unloading = true;
        g_stopWorker = true;

        SetEvent(g_stopEvent);

        WaitForSingleObject(
            g_workerThread,
            INFINITE);

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
// WINDHAWK BEFORE UNINITIALIZATION
// ============================================================

void Wh_ModBeforeUninit()
{
    // Windhawk calls this before removing our hooks.
    //
    // Stop the worker first so it cannot continue executing
    // mod code while Windhawk removes the hook.
    g_unloading = true;
    g_stopWorker = true;

    HANDLE stopEvent = g_stopEvent;

    if (stopEvent)
    {
        SetEvent(stopEvent);
    }

    HANDLE workerThread = g_workerThread;

    if (workerThread)
    {
        WaitForSingleObject(
            workerThread,
            INFINITE);
    }
}

// ============================================================
// WINDHAWK UNINITIALIZATION
// ============================================================

void Wh_ModUninit()
{
    if (g_workerThread)
    {
        CloseHandle(g_workerThread);

        g_workerThread = nullptr;
    }

    if (g_checkEvent)
    {
        CloseHandle(g_checkEvent);

        g_checkEvent = nullptr;
    }

    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);

        g_stopEvent = nullptr;
    }

    g_frameWindowObserved = false;
    g_windowGeneration = 0;
}

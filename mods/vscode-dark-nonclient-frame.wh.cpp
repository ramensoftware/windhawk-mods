// ==WindhawkMod==
// @id              vscode-dark-nonclient-frame
// @name            VS Code Dark Non-client Frame
// @description     Forces VS Code's non-client frame into dark mode to remove the light 2px bottom edge in Windows light mode.
// @version         1.0
// @author          v1b3s
// @github          https://github.com/v1b3s0
// @license         MIT
// @include         Code.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VS Code Dark Non-client Frame

Forces VS Code's Windows non-client frame into immersive dark mode.

This fixes the 2px light gray/white bottom edge that can appear when VS Code is maximized with the custom title bar, especially when using a hidden or transparent taskbar in Windows light mode.

The mod targets only `Code.exe`.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>

static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerThread = nullptr;

constexpr DWORD REAPPLY_INTERVAL_MS = 1000;

// Official modern value.
// On current Windows 10/11 builds, this is the dark non-client-frame switch.
constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE = 20;

BOOL ApplyDarkFrameToWindow(HWND hwnd, BOOL enabled) {
    BOOL value = enabled;

    HRESULT hr = DwmSetWindowAttribute(
        hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE_VALUE,
        &value,
        sizeof(value)
    );

    return SUCCEEDED(hr);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD targetPid = GetCurrentProcessId();

    DWORD windowPid = 0;
    GetWindowThreadProcessId(hwnd, &windowPid);

    if (windowPid != targetPid) {
        return TRUE;
    }

    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    BOOL enabled = static_cast<BOOL>(lParam);
    ApplyDarkFrameToWindow(hwnd, enabled);

    return TRUE;
}

void ApplyDarkFrameToCurrentProcessWindows(BOOL enabled) {
    EnumWindows(EnumWindowsProc, static_cast<LPARAM>(enabled));
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    while (WaitForSingleObject(g_stopEvent, REAPPLY_INTERVAL_MS) == WAIT_TIMEOUT) {
        ApplyDarkFrameToCurrentProcessWindows(TRUE);
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"VS Code dark non-client frame mod initialized");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"Failed to create stop event");
        return FALSE;
    }

    // Apply immediately in case VS Code already has a visible window.
    ApplyDarkFrameToCurrentProcessWindows(TRUE);

    // Keep reapplying so new/reloaded/remaximized VS Code windows get fixed too.
    g_workerThread = CreateThread(
        nullptr,
        0,
        WorkerThreadProc,
        nullptr,
        0,
        nullptr
    );

    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"VS Code dark non-client frame mod unloading");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    // Restore default behavior when the mod is disabled/unloaded.
    ApplyDarkFrameToCurrentProcessWindows(FALSE);

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
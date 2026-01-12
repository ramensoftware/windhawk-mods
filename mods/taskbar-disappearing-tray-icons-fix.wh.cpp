// ==WindhawkMod==
// @id              taskbar-disappearing-tray-icons-fix
// @name            Disappearing Tray Icons Fix
// @description     Fixes missing system tray icons by broadcasting TaskbarCreated when the taskbar initializes.
// @version         1.0
// @author          Alchemy
// @github          https://github.com/alchemyyy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disappearing Tray Icons Fix

Fixes system tray icons going missing in Windows 11 by rebroadcasting the
`TaskbarCreated` window message.

## The Problem

Windows 11 has a known issue where system tray icons can randomly vanish (not
just hide in the overflow area). This happens because:

1. Apps register tray icons by calling `Shell_NotifyIcon`
2. When `explorer.exe` restarts or the shell initializes, it broadcasts
   `TaskbarCreated` to tell apps to re-register their icons
3. Some apps miss this message due to race conditions, sleep/wake cycles, or
   timing issues
4. Result: icons vanish completely with no way to recover except restarting
   the app

## The Solution

This mod detects when the taskbar is created and rebroadcasts the `TaskbarCreated`
message after a defined delay, giving apps another chance to register their tray
icons. This mod can also run a passive rebroadcast to catch bizarre instances of this
occurring while the taskbar is running.

Only the explorer.exe instance that owns the taskbar will activate this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- initialDelaySeconds: 5
  $name: Initial delay (seconds)
  $description: How long to wait after taskbar init before first broadcast
- disablePassiveBroadcast: false
  $name: Disable passive broadcast
  $description: Stop broadcasting periodically after initial broadcast
- intervalSeconds: 5
  $name: Broadcast interval (seconds)
  $description: How often to broadcast in passive mode
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>

struct {
    std::atomic<int> initialDelaySeconds;
    std::atomic<bool> disablePassiveBroadcast;
    std::atomic<int> intervalSeconds;
} g_settings;

UINT g_taskbarCreatedMsg = 0;
HANDLE g_workerThread = nullptr;
DWORD g_taskbarThreadId = 0;

void BroadcastTaskbarCreated() {
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"ERROR: TaskbarCreated message not registered");
        return;
    }

    Wh_Log(L"Broadcasting TaskbarCreated (msg=%u)", g_taskbarCreatedMsg);
    PostMessageW(HWND_BROADCAST, g_taskbarCreatedMsg, 0, 0);
}

void LoadSettings() {
    int initialDelay = Wh_GetIntSetting(L"initialDelaySeconds");
    int interval = Wh_GetIntSetting(L"intervalSeconds");
    
    g_settings.initialDelaySeconds.store(initialDelay < 1 ? 5 : initialDelay);
    g_settings.intervalSeconds.store(interval < 1 ? 5 : interval);
    g_settings.disablePassiveBroadcast.store(Wh_GetIntSetting(L"disablePassiveBroadcast") != 0);
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    // Poll for taskbar creation
    HWND hTaskbarWnd = nullptr;
    while (!hTaskbarWnd) {
        hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!hTaskbarWnd) {
            Sleep(1000);
            continue;
        }

        // Check if we own it
        DWORD dwTaskbarProcessId;
        GetWindowThreadProcessId(hTaskbarWnd, &dwTaskbarProcessId);

        if (dwTaskbarProcessId != GetCurrentProcessId()) {
            Wh_Log(L"Taskbar owned by PID %lu, not us - terminating", dwTaskbarProcessId);
            return 0;
        }

        g_taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
        Wh_Log(L"Found taskbar: %08X (thread %lu)", 
               (DWORD)(ULONG_PTR)hTaskbarWnd, g_taskbarThreadId);
    }

    // Initial delay before first broadcast
    int initialDelayMs = g_settings.initialDelaySeconds.load() * 1000;
    Wh_Log(L"Waiting %dms before initial broadcast", initialDelayMs);
    Sleep(initialDelayMs);

    BroadcastTaskbarCreated();

    // Passive broadcast loop
    if (g_settings.disablePassiveBroadcast.load()) {
        Wh_Log(L"Passive broadcast disabled");
        return 0;
    }

    int intervalMs = g_settings.intervalSeconds.load() * 1000;
    Wh_Log(L"Starting passive broadcast every %dms", intervalMs);

    MSG msg;
    UINT_PTR timerId = SetTimer(nullptr, 0, intervalMs, nullptr);

    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        if (msg.message == WM_TIMER && msg.wParam == timerId) {
            BroadcastTaskbarCreated();
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (timerId) {
        KillTimer(nullptr, timerId);
    }

    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing...");

    LoadSettings();

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"Failed to register TaskbarCreated message (error: %lu)", GetLastError());
        return FALSE;
    }

    // Check if taskbar already exists and owned by another process
    HWND hTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd) {
        DWORD dwTaskbarProcessId;
        GetWindowThreadProcessId(hTaskbarWnd, &dwTaskbarProcessId);

        if (dwTaskbarProcessId != GetCurrentProcessId()) {
            Wh_Log(L"Taskbar owned by PID %lu, not us - unloading", dwTaskbarProcessId);
            return FALSE;
        }
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread (error: %lu)", GetLastError());
        return FALSE;
    }

    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModUninit() {
    if (g_workerThread) {
        PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    Wh_Log(L"Uninitialized");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

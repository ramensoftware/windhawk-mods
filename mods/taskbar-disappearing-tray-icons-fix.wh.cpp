// ==WindhawkMod==
// @id              taskbar-disappearing-tray-icons-fix
// @name            Disappearing Tray Icons Fix
// @description     Fixes missing system tray icons by broadcasting TaskbarCreated when the taskbar initializes.
// @version         1.1
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

## GDI Leak Fix

When passive broadcast is enabled, this mod hooks icon creation APIs to track and
destroy icons created during each broadcast cycle, preventing GDI handle leaks that
would otherwise crash explorer.exe.

Only the explorer.exe instance that owns the taskbar will activate this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- initialDelaySeconds: 5
  $name: Initial delay (seconds)
  $description: How long to wait after taskbar init before first broadcast
- passiveBroadcast: false
  $name: Enable passive broadcast
  $description: Continue broadcasting periodically after initial broadcast
- intervalSeconds: 5
  $name: Broadcast interval (seconds)
  $description: How often to broadcast in passive mode
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <vector>
#include <mutex>

struct {
    std::atomic<int> initialDelaySeconds;
    std::atomic<bool> passiveBroadcast;
    std::atomic<int> intervalSeconds;
} g_settings;

UINT g_taskbarCreatedMsg = 0;
HANDLE g_workerThread = nullptr;
DWORD g_taskbarThreadId = 0;

// ============================================================================
// GDI Leak Prevention (active only when passive broadcast is enabled)
// ============================================================================

enum class IconSource : int {
    LoadImageW = 0,
    CreateIconIndirect = 1,
    CopyIcon = 2,
    CreateIconFromResourceEx = 3
};

struct TrackedIcon {
    HICON hIcon;
    IconSource source;
};

std::mutex g_iconMutex;
std::vector<TrackedIcon> g_currentIcons;
std::vector<TrackedIcon> g_previousIcons;
std::atomic<bool> g_trackingEnabled{false};

using LoadImageW_t = HANDLE(WINAPI*)(HINSTANCE, LPCWSTR, UINT, int, int, UINT);
LoadImageW_t LoadImageW_Original = nullptr;

HANDLE WINAPI LoadImageW_Hook(HINSTANCE hInst, LPCWSTR name, UINT type,
                               int cx, int cy, UINT fuLoad) {
    HANDLE result = LoadImageW_Original(hInst, name, type, cx, cy, fuLoad);
    if (result && type == IMAGE_ICON && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({(HICON)result, IconSource::LoadImageW});
    }
    return result;
}

using CreateIconIndirect_t = HICON(WINAPI*)(PICONINFO);
CreateIconIndirect_t CreateIconIndirect_Original = nullptr;

HICON WINAPI CreateIconIndirect_Hook(PICONINFO piconinfo) {
    HICON result = CreateIconIndirect_Original(piconinfo);
    if (result && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CreateIconIndirect});
    }
    return result;
}

using CopyIcon_t = HICON(WINAPI*)(HICON);
CopyIcon_t CopyIcon_Original = nullptr;

HICON WINAPI CopyIcon_Hook(HICON hIcon) {
    HICON result = CopyIcon_Original(hIcon);
    if (result && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CopyIcon});
    }
    return result;
}

using CreateIconFromResourceEx_t = HICON(WINAPI*)(PBYTE, DWORD, BOOL, DWORD, int, int, UINT);
CreateIconFromResourceEx_t CreateIconFromResourceEx_Original = nullptr;

HICON WINAPI CreateIconFromResourceEx_Hook(PBYTE presbits, DWORD dwResSize,
                                            BOOL fIcon, DWORD dwVer,
                                            int cxDesired, int cyDesired, UINT Flags) {
    HICON result = CreateIconFromResourceEx_Original(presbits, dwResSize, fIcon,
                                                      dwVer, cxDesired, cyDesired, Flags);
    if (result && fIcon && g_trackingEnabled.load()) {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        g_currentIcons.push_back({result, IconSource::CreateIconFromResourceEx});
    }
    return result;
}

void DestroyTrackedIcons() {
    std::vector<TrackedIcon> toDestroy;
    {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        toDestroy = std::move(g_previousIcons);
        g_previousIcons = std::move(g_currentIcons);
        g_currentIcons.clear();
    }

    int destroyed[4] = {0, 0, 0, 0};
    for (const auto& ti : toDestroy) {
        if (ti.hIcon) {
            DestroyIcon(ti.hIcon);
            destroyed[(int)ti.source]++;
        }
    }

    if (destroyed[0] || destroyed[1] || destroyed[2] || destroyed[3]) {
        Wh_Log(L"Destroyed icons: LoadImage=%d CreateIconIndirect=%d CopyIcon=%d CreateIconFromRes=%d",
               destroyed[0], destroyed[1], destroyed[2], destroyed[3]);
    }
}

// ============================================================================

void BroadcastTaskbarCreated() {
    if (!g_taskbarCreatedMsg) {
        Wh_Log(L"ERROR: TaskbarCreated message not registered");
        return;
    }

    if (g_trackingEnabled.load()) {
        DestroyTrackedIcons();
    }

    Wh_Log(L"Broadcasting TaskbarCreated (msg=%u)", g_taskbarCreatedMsg);
    PostMessageW(HWND_BROADCAST, g_taskbarCreatedMsg, 0, 0);
}

void LoadSettings() {
    int initialDelay = Wh_GetIntSetting(L"initialDelaySeconds");
    int interval = Wh_GetIntSetting(L"intervalSeconds");

    g_settings.initialDelaySeconds.store(initialDelay < 1 ? 5 : initialDelay);
    g_settings.intervalSeconds.store(interval < 1 ? 5 : interval);
    g_settings.passiveBroadcast.store(Wh_GetIntSetting(L"passiveBroadcast") != 0);
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
    if (!g_settings.passiveBroadcast.load()) {
        Wh_Log(L"Passive broadcast disabled");
        return 0;
    }

    g_trackingEnabled.store(true);

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

    g_trackingEnabled.store(false);
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

    // Hook icon APIs only if passive broadcast is enabled (to fix GDI leaks)
    if (g_settings.passiveBroadcast.load()) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* p;

            if ((p = (void*)GetProcAddress(hUser32, "LoadImageW"))) {
                Wh_SetFunctionHook(p, (void*)LoadImageW_Hook, (void**)&LoadImageW_Original);
                Wh_Log(L"Hooked LoadImageW");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CreateIconIndirect"))) {
                Wh_SetFunctionHook(p, (void*)CreateIconIndirect_Hook, (void**)&CreateIconIndirect_Original);
                Wh_Log(L"Hooked CreateIconIndirect");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CopyIcon"))) {
                Wh_SetFunctionHook(p, (void*)CopyIcon_Hook, (void**)&CopyIcon_Original);
                Wh_Log(L"Hooked CopyIcon");
            }

            if ((p = (void*)GetProcAddress(hUser32, "CreateIconFromResourceEx"))) {
                Wh_SetFunctionHook(p, (void*)CreateIconFromResourceEx_Hook, (void**)&CreateIconFromResourceEx_Original);
                Wh_Log(L"Hooked CreateIconFromResourceEx");
            }
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
    g_trackingEnabled.store(false);

    if (g_workerThread) {
        PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    // Destroy any remaining tracked icons
    {
        std::lock_guard<std::mutex> lock(g_iconMutex);
        for (const auto& ti : g_currentIcons) {
            if (ti.hIcon) DestroyIcon(ti.hIcon);
        }
        for (const auto& ti : g_previousIcons) {
            if (ti.hIcon) DestroyIcon(ti.hIcon);
        }
        g_currentIcons.clear();
        g_previousIcons.clear();
    }

    Wh_Log(L"Uninitialized");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}

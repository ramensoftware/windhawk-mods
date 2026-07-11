// ==WindhawkMod==
// @id              lid-sleep-delay
// @name            Lid Close Sleep Delay
// @description     Delays system sleep after laptop lid is closed by a configurable time (30s, 1min, 5min, 10min)
// @version         1.0
// @author          Ansh Raj
// @github          https://github.com/remixansh
// @include         explorer.exe
// @compilerOptions -lpowrprof -luser32 -ladvapi32 -lole32 -loleaut32 -lwbemuuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lid Close Sleep Delay

This mod adds a configurable delay before your laptop goes to sleep when the
lid is closed. It works dynamically on **any Windows system** — laptops,
convertibles, and tablets with a lid sensor. On desktops or systems without
a lid switch, the mod detects this at startup and gracefully disables itself.

## Compatibility

- **Architectures**: x86 (32-bit), x86-64 (64-bit), and ARM64
- **Windows versions**: Windows 7, 8, 8.1, 10, and 11
- **System types**: Automatically detects laptops vs desktops

## How it works

1. On startup, the mod checks if the system has a **lid switch** (battery
   presence, chassis type, and power capabilities are all checked).
2. If a lid switch is detected, it listens for **lid state change** power
   notifications.
3. When the lid is closed, it keeps the system awake and starts a countdown.
4. If you reopen the lid before the timer expires, sleep is cancelled.
5. If the timer expires with the lid still closed, the system goes to sleep.
6. On desktops without a lid, the mod logs a message and exits cleanly.

## Prerequisites

**Set your Windows lid-close action to "Do Nothing":**

1. Open **Control Panel** → **Power Options** → **Choose what closing the lid
   does**
2. Set **"When I close the lid"** to **"Do nothing"** for both **On battery**
   and **Plugged in**
3. Click **Save changes**

## Options

| Option      | Delay           |
|-------------|-----------------|
| **30sec**   | 30 seconds      |
| **1min**    | 1 minute (default) |
| **5min**    | 5 minutes       |
| **10min**   | 10 minutes      |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- SleepDelay: 1min
  $name: Sleep delay after lid close
  $description: Select how long to wait before sleeping after the lid is closed. Changes apply immediately on save.
  $options:
    - 30sec: 30 seconds
    - 1min: 1 minute (default)
    - 5min: 5 minutes
    - 10min: 10 minutes
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <powrprof.h>
#include <batclass.h>
#include <comdef.h>
#include <wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

// ─── GUIDs ───────────────────────────────────────────────────────────────────

// {BA3E0F4D-B817-4094-A2D1-D56379E6A0F3}
static const GUID GUID_LIDSWITCH_STATE_CHANGE_LOCAL = {
    0xBA3E0F4D, 0xB817, 0x4094,
    {0xA2, 0xD1, 0xD5, 0x63, 0x79, 0xE6, 0xA0, 0xF3}
};

// ─── Global State ────────────────────────────────────────────────────────────

static volatile int g_sleepDelaySeconds = 60;
static volatile BOOL g_bLidClosed = FALSE;
static volatile BOOL g_bTimerActive = FALSE;
static volatile BOOL g_bRunning = FALSE;

static HANDLE g_hThread = NULL;
static DWORD g_dwThreadId = 0;
static HANDLE g_hMutex = NULL;
static HWND g_hWnd = NULL;
static HPOWERNOTIFY g_hPowerNotify = NULL;

static const UINT_PTR SLEEP_TIMER_ID = 0xD00D;
// Custom message to tell the thread to reload settings
static const UINT WM_RELOAD_SETTINGS = WM_USER + 100;
// Custom message to tell the thread to quit
static const UINT WM_QUIT_THREAD = WM_USER + 101;

// ─── System Detection ────────────────────────────────────────────────────────

// Check if the system has a battery (strong indicator of laptop/tablet)
static BOOL HasBattery() {
    SYSTEM_POWER_STATUS sps;
    if (!GetSystemPowerStatus(&sps)) {
        Wh_Log(L"GetSystemPowerStatus failed: %lu", GetLastError());
        return FALSE;
    }

    // BatteryFlag == 128 means "No system battery"
    // ACLineStatus == 255 means "Unknown" (possible on some desktops)
    if (sps.BatteryFlag == 128) {
        Wh_Log(L"No system battery detected (BatteryFlag=128)");
        return FALSE;
    }

    Wh_Log(L"Battery detected: ACLine=%d, BatteryFlag=%d, BatteryLife=%d%%",
           sps.ACLineStatus, sps.BatteryFlag, sps.BatteryLifePercent);
    return TRUE;
}

// Check the system's power capabilities for lid switch support
static BOOL HasLidSwitchCapability() {
    // Use CallNtPowerInformation to check SYSTEM_POWER_CAPABILITIES
    typedef struct _SYSTEM_POWER_CAPABILITIES_FULL {
        BOOLEAN PowerButtonPresent;
        BOOLEAN SleepButtonPresent;
        BOOLEAN LidPresent;
        BOOLEAN SystemS1;
        BOOLEAN SystemS2;
        BOOLEAN SystemS3;
        BOOLEAN SystemS4;
        BOOLEAN SystemS5;
        // ... more fields follow but we only need LidPresent
    } SYSTEM_POWER_CAPABILITIES_FULL;

    SYSTEM_POWER_CAPABILITIES_FULL caps = {};
    NTSTATUS status = CallNtPowerInformation(
        SystemPowerCapabilities,
        NULL, 0,
        &caps, sizeof(caps)
    );

    if (status != 0) {
        Wh_Log(L"CallNtPowerInformation failed: 0x%08X", status);
        return FALSE;
    }

    Wh_Log(L"Power capabilities: LidPresent=%d, PowerButton=%d, SleepButton=%d",
           caps.LidPresent, caps.PowerButtonPresent, caps.SleepButtonPresent);

    return caps.LidPresent;
}

// Query WMI for chassis type to determine if laptop/portable
// Chassis types: 8=Portable, 9=Laptop, 10=Notebook, 14=Sub-Notebook,
//                30=Tablet, 31=Convertible, 32=Detachable
static BOOL IsPortableChassis() {
    HRESULT hr;
    BOOL result = FALSE;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", hr);
        return FALSE;
    }
    BOOL bComInitialized = SUCCEEDED(hr);

    // Set security levels
    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    // Ignore error if already initialized

    IWbemLocator* pLoc = NULL;
    hr = CoCreateInstance(
        CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc
    );
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance WbemLocator failed: 0x%08X", hr);
        if (bComInitialized) CoUninitialize();
        return FALSE;
    }

    IWbemServices* pSvc = NULL;
    hr = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), NULL, NULL, NULL, 0, NULL, NULL, &pSvc
    );
    if (FAILED(hr)) {
        Wh_Log(L"ConnectServer failed: 0x%08X", hr);
        pLoc->Release();
        if (bComInitialized) CoUninitialize();
        return FALSE;
    }

    // Set proxy security
    hr = CoSetProxyBlanket(
        pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE
    );

    IEnumWbemClassObject* pEnum = NULL;
    hr = pSvc->ExecQuery(
        _bstr_t(L"WQL"),
        _bstr_t(L"SELECT ChassisTypes FROM Win32_SystemEnclosure"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnum
    );

    if (SUCCEEDED(hr)) {
        IWbemClassObject* pObj = NULL;
        ULONG uReturn = 0;

        while (pEnum->Next(WBEM_INFINITE, 1, &pObj, &uReturn) == S_OK) {
            VARIANT vtProp;
            hr = pObj->Get(L"ChassisTypes", 0, &vtProp, NULL, NULL);
            if (SUCCEEDED(hr) && vtProp.vt == (VT_ARRAY | VT_I4)) {
                SAFEARRAY* psa = vtProp.parray;
                LONG lBound, uBound;
                SafeArrayGetLBound(psa, 1, &lBound);
                SafeArrayGetUBound(psa, 1, &uBound);

                for (LONG i = lBound; i <= uBound; i++) {
                    LONG val;
                    SafeArrayGetElement(psa, &i, &val);
                    Wh_Log(L"Chassis type: %ld", val);

                    // Portable chassis types
                    if (val == 8 || val == 9 || val == 10 || val == 14 ||
                        val == 30 || val == 31 || val == 32) {
                        result = TRUE;
                    }
                }
            }
            VariantClear(&vtProp);
            pObj->Release();
        }
        pEnum->Release();
    }

    pSvc->Release();
    pLoc->Release();
    if (bComInitialized) CoUninitialize();

    Wh_Log(L"IsPortableChassis: %s", result ? L"YES" : L"NO");
    return result;
}

// Combined detection: is this system capable of lid-close events?
static BOOL IsLidCapableSystem() {
    // Primary check: does the system report a lid switch?
    if (HasLidSwitchCapability()) {
        Wh_Log(L"System reports LidPresent=TRUE via power capabilities");
        return TRUE;
    }

    // Secondary check: does it have a battery AND a portable chassis?
    if (HasBattery() && IsPortableChassis()) {
        Wh_Log(L"System has battery + portable chassis — assuming lid capable");
        return TRUE;
    }

    Wh_Log(L"System does NOT appear to have a lid switch");
    return FALSE;
}

// ─── Sleep Privilege ─────────────────────────────────────────────────────────

static BOOL EnableShutdownPrivilege() {
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        Wh_Log(L"OpenProcessToken failed: %lu", GetLastError());
        return FALSE;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                               &tp.Privileges[0].Luid)) {
        Wh_Log(L"LookupPrivilegeValue failed: %lu", GetLastError());
        CloseHandle(hToken);
        return FALSE;
    }

    BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
    DWORD err = GetLastError();
    CloseHandle(hToken);

    if (!result || err == ERROR_NOT_ALL_ASSIGNED) {
        Wh_Log(L"AdjustTokenPrivileges failed: %lu", err);
        return FALSE;
    }

    Wh_Log(L"SE_SHUTDOWN_NAME privilege enabled successfully");
    return TRUE;
}

// ─── Window Proc (runs on our dedicated thread) ─────────────────────────────

static VOID CALLBACK SleepTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent,
                                     DWORD dwTime)
{
    Wh_Log(L"*** TIMER CALLBACK FIRED ***");
    KillTimer(g_hWnd, SLEEP_TIMER_ID);
    g_bTimerActive = FALSE;

    if (!g_bLidClosed) {
        Wh_Log(L"Timer fired but lid is now OPEN — sleep cancelled");
        SetThreadExecutionState(ES_CONTINUOUS);
        return;
    }

    Wh_Log(L"==> TIMER EXPIRED, lid still CLOSED — SLEEPING NOW <==");

    // Release keep-awake
    SetThreadExecutionState(ES_CONTINUOUS);
    Sleep(200);

    // Put system to sleep
    BOOLEAN res = SetSuspendState(FALSE, TRUE, FALSE);
    Wh_Log(L"SetSuspendState returned: %d (error: %lu)",
           (int)res, GetLastError());
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam)
{
    switch (uMsg) {
        case WM_POWERBROADCAST: {
            if (wParam == PBT_POWERSETTINGCHANGE) {
                PPOWERBROADCAST_SETTING pSetting =
                    (PPOWERBROADCAST_SETTING)lParam;

                if (IsEqualGUID(pSetting->PowerSetting,
                                GUID_LIDSWITCH_STATE_CHANGE_LOCAL)) {
                    DWORD lidState = *(DWORD*)(pSetting->Data);

                    if (lidState == 0) {
                        // ── Lid Closed ──
                        g_bLidClosed = TRUE;
                        int delay = g_sleepDelaySeconds;
                        Wh_Log(L">>> LID CLOSED — will sleep in %d seconds",
                               delay);

                        // Keep system awake
                        SetThreadExecutionState(ES_CONTINUOUS |
                                                ES_SYSTEM_REQUIRED);

                        // Kill any existing timer first
                        if (g_bTimerActive) {
                            KillTimer(g_hWnd, SLEEP_TIMER_ID);
                        }

                        // Start timer with callback
                        g_bTimerActive = TRUE;
                        UINT_PTR tid = SetTimer(g_hWnd, SLEEP_TIMER_ID,
                                                (UINT)(delay * 1000),
                                                SleepTimerProc);
                        Wh_Log(L"Timer set: ID=%llu, delay=%d ms",
                               (unsigned long long)tid, delay * 1000);
                    } else {
                        // ── Lid Opened ──
                        g_bLidClosed = FALSE;
                        Wh_Log(L">>> LID OPENED");

                        if (g_bTimerActive) {
                            KillTimer(g_hWnd, SLEEP_TIMER_ID);
                            g_bTimerActive = FALSE;
                            Wh_Log(L"    Timer cancelled — sleep aborted");
                        }

                        SetThreadExecutionState(ES_CONTINUOUS);
                    }
                }
            }
            return 0;
        }

        case WM_RELOAD_SETTINGS: {
            Wh_Log(L"Thread received WM_RELOAD_SETTINGS");
            // Settings are already updated in g_sleepDelaySeconds
            // If timer is active, restart it with new delay
            if (g_bTimerActive) {
                KillTimer(g_hWnd, SLEEP_TIMER_ID);
                int delay = g_sleepDelaySeconds;
                SetTimer(g_hWnd, SLEEP_TIMER_ID, (UINT)(delay * 1000),
                         SleepTimerProc);
                Wh_Log(L"Timer restarted with new delay: %d seconds", delay);
            }
            return 0;
        }

        case WM_QUIT_THREAD: {
            Wh_Log(L"Thread received WM_QUIT_THREAD");
            if (g_bTimerActive) {
                KillTimer(g_hWnd, SLEEP_TIMER_ID);
                g_bTimerActive = FALSE;
            }
            SetThreadExecutionState(ES_CONTINUOUS);
            PostQuitMessage(0);
            return 0;
        }

        case WM_DESTROY: {
            if (g_hPowerNotify) {
                UnregisterPowerSettingNotification(g_hPowerNotify);
                g_hPowerNotify = NULL;
            }
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ─── Dedicated Message-Pumping Thread ────────────────────────────────────────
// This thread creates the window, registers for power notifications,
// and runs its own message loop. This ensures WM_POWERBROADCAST and
// WM_TIMER are always dispatched regardless of which thread Windhawk
// loaded us on.

static DWORD WINAPI LidMonitorThread(LPVOID lpParam) {
    Wh_Log(L"[Thread] LidMonitorThread started (TID: %lu)",
           GetCurrentThreadId());

    // Enable shutdown privilege on this thread too
    EnableShutdownPrivilege();

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkLidSleepDelay_v14";

    ATOM atom = RegisterClassEx(&wc);
    if (!atom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"[Thread] RegisterClassEx failed: %lu", GetLastError());
        return 1;
    }

    // Create a regular hidden window (NOT HWND_MESSAGE)
    g_hWnd = CreateWindowEx(
        0,
        L"WindhawkLidSleepDelay_v14",
        L"LidSleepDelay",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!g_hWnd) {
        Wh_Log(L"[Thread] CreateWindowEx failed: %lu", GetLastError());
        return 1;
    }

    ShowWindow(g_hWnd, SW_HIDE);
    Wh_Log(L"[Thread] Window created: HWND=%p", (void*)g_hWnd);

    // Register for lid switch power notifications
    g_hPowerNotify = RegisterPowerSettingNotification(
        g_hWnd,
        &GUID_LIDSWITCH_STATE_CHANGE_LOCAL,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    if (!g_hPowerNotify) {
        Wh_Log(L"[Thread] RegisterPowerSettingNotification failed: %lu",
               GetLastError());
        DestroyWindow(g_hWnd);
        g_hWnd = NULL;
        return 1;
    }

    Wh_Log(L"[Thread] Power notification registered — listening for lid "
           L"events");
    Wh_Log(L"[Thread] Current delay: %d seconds", g_sleepDelaySeconds);

    // ── Message Loop ──
    // This is the key: our own message loop ensures WM_POWERBROADCAST
    // and timer callbacks are always dispatched
    g_bRunning = TRUE;
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"[Thread] Message loop exited");

    // Cleanup
    if (g_hPowerNotify) {
        UnregisterPowerSettingNotification(g_hPowerNotify);
        g_hPowerNotify = NULL;
    }
    if (g_hWnd) {
        DestroyWindow(g_hWnd);
        g_hWnd = NULL;
    }
    UnregisterClass(L"WindhawkLidSleepDelay_v14", GetModuleHandle(NULL));

    g_bRunning = FALSE;
    Wh_Log(L"[Thread] LidMonitorThread exiting");
    return 0;
}

// ─── Settings ────────────────────────────────────────────────────────────────

static int ParseDelayOption(PCWSTR option) {
    if (wcscmp(option, L"30sec") == 0)  return 30;
    if (wcscmp(option, L"1min") == 0)   return 60;
    if (wcscmp(option, L"5min") == 0)   return 300;
    if (wcscmp(option, L"10min") == 0)  return 600;
    return 60;
}

void LoadSettings() {
    PCWSTR delayStr = Wh_GetStringSetting(L"SleepDelay");
    g_sleepDelaySeconds = ParseDelayOption(delayStr);
    Wh_FreeStringSetting(delayStr);
    Wh_Log(L"Settings loaded: sleepDelaySeconds = %d", g_sleepDelaySeconds);
}

// ─── Windhawk Callbacks ──────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"=== Lid Close Sleep Delay v1.5: Init (TID: %lu) ===",
           GetCurrentThreadId());

    // ── Check Windows version ──
    // The APIs we use (RegisterPowerSettingNotification, SetSuspendState)
    // require Windows Vista or later. All modern Windows versions are fine.
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    // Use RtlGetVersion for accurate results (GetVersionEx is shimmed)
    typedef NTSTATUS(WINAPI* RtlGetVersionFunc)(PRTL_OSVERSIONINFOW);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionFunc pRtlGetVersion =
            (RtlGetVersionFunc)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            pRtlGetVersion((PRTL_OSVERSIONINFOW)&osvi);
            Wh_Log(L"Windows version: %lu.%lu.%lu",
                   osvi.dwMajorVersion, osvi.dwMinorVersion,
                   osvi.dwBuildNumber);
        }
    }

    // ── Detect if this system has a lid ──
    if (!IsLidCapableSystem()) {
        Wh_Log(L"This system does not have a lid switch (desktop/server). "
               L"Mod will not activate.");
        return FALSE;
    }
    Wh_Log(L"Lid-capable system detected — proceeding with initialization");

    // Only run in one explorer.exe instance
    g_hMutex = CreateMutex(NULL, TRUE,
                            L"WindhawkLidSleepDelay_SingleInstance");
    if (g_hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Another instance already running — skipping");
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
        return FALSE;
    }

    LoadSettings();

    // Enable shutdown privilege on the init thread
    EnableShutdownPrivilege();

    // Launch the dedicated message-pumping thread
    g_hThread = CreateThread(NULL, 0, LidMonitorThread, NULL, 0,
                              &g_dwThreadId);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }

    // Wait a moment for the thread to initialize
    Sleep(200);

    if (!g_bRunning) {
        Wh_Log(L"Thread failed to start");
        return FALSE;
    }

    Wh_Log(L"=== Init complete. Thread TID: %lu, Delay: %d sec ===",
           g_dwThreadId, g_sleepDelaySeconds);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== Lid Close Sleep Delay: Uninit ===");

    // Tell the thread to quit
    if (g_dwThreadId && g_hWnd) {
        PostMessage(g_hWnd, WM_QUIT_THREAD, 0, 0);

        // Wait for thread to finish (max 3 seconds)
        if (g_hThread) {
            DWORD waitResult = WaitForSingleObject(g_hThread, 3000);
            if (waitResult == WAIT_TIMEOUT) {
                Wh_Log(L"Thread did not exit in time — terminating");
                TerminateThread(g_hThread, 0);
            }
            CloseHandle(g_hThread);
            g_hThread = NULL;
        }
    }

    SetThreadExecutionState(ES_CONTINUOUS);

    if (g_hMutex) {
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
        g_hMutex = NULL;
    }

    Wh_Log(L"=== Uninit complete ===");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");
    int oldDelay = g_sleepDelaySeconds;
    LoadSettings();
    Wh_Log(L"Delay: %d -> %d seconds", oldDelay, g_sleepDelaySeconds);

    // Tell the thread to reload
    if (g_hWnd) {
        PostMessage(g_hWnd, WM_RELOAD_SETTINGS, 0, 0);
    }
}

// ==WindhawkMod==
// @id              lid-sleep-delay
// @name            Lid Close Sleep Delay
// @description     Delays system sleep after laptop lid is closed by a configurable time (30s, 1min, 5min, 10min)
// @version         1.1
// @author          Ansh Raj
// @github          https://github.com/remixansh
// @include         windhawk.exe
// @compilerOptions -lpowrprof -luser32 -ladvapi32 -lole32 -loleaut32 -lwbemuuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lid Close Sleep Delay

Ever wished your laptop wouldn't immediately go to sleep the moment you close the lid? This mod adds a customizable delay before suspending your system, giving you a grace period to grab your laptop, move to another room, or simply change your mind without interrupting your workflow or breaking active network connections.

Designed with stability in mind, it operates securely as a **dedicated-process tool mod**. This means it runs isolated in its own Windhawk process rather than injecting code into `explorer.exe` or other critical system components, guaranteeing a zero-crash impact on your desktop shell.

## 💻 Compatibility & Features
- **Smart Detection:** Dynamically recognizes whether your system is a laptop, convertible, or tablet with a physical lid switch. On desktops, it safely detects the absence of a lid and disables itself to consume zero resources.
- **Universal Support:** Works seamlessly across x86 (32-bit), x86-64 (64-bit), and ARM64 architectures.
- **OS Versions:** Compatible with Windows 7, 8, 8.1, 10, and 11.

## ⚙️ How it Works
1. When you close the lid, the mod kicks in, keeping your system awake while a countdown begins.
2. If you reopen the lid before the timer runs out, the sleep action is seamlessly aborted, and you can resume work instantly.
3. If the timer expires while the lid is still closed, it gently puts your system to sleep.

## ⚠️ Important Prerequisites
For this mod to take control of the sleep delay, you must first disable the default Windows instant-sleep behavior:

1. Open **Control Panel** → **Power Options** → **Choose what closing the lid does** (on the left panel).
2. Change the setting for **"When I close the lid"** to **"Do nothing"** for both *On battery* and *Plugged in*.
3. Click **Save changes**.

> **Note:** Because this relies on your native Windows power settings, uninstalling or disabling this mod will *not* automatically restore the original instant-sleep behavior. You will need to manually revert the setting back to **"Sleep"** in your Power Options if you decide to stop using the mod.

## ⏱️ Configuration Options
You can configure the exact delay duration directly in the mod settings. Changes apply instantly on save.

| Delay Option | Description |
|---|---|
| **30sec** | 30 seconds |
| **1min** | 1 minute (default) |
| **5min** | 5 minutes |
| **10min** | 10 minutes |
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
#include <atomic>

// ─── GUIDs ───────────────────────────────────────────────────────────────────

// {BA3E0F4D-B817-4094-A2D1-D56379E6A0F3}
static const GUID GUID_LIDSWITCH_STATE_CHANGE_LOCAL = {
    0xBA3E0F4D, 0xB817, 0x4094,
    {0xA2, 0xD1, 0xD5, 0x63, 0x79, 0xE6, 0xA0, 0xF3}
};

// ─── Global State ────────────────────────────────────────────────────────────

// Cross-thread: WhTool_ModSettingsChanged runs on a Windhawk thread while the
// entry-point thread reads this value.
static std::atomic<int> g_sleepDelaySeconds{60};

// Entry-point thread only — no synchronization needed.
static BOOL g_bLidClosed = FALSE;
static BOOL g_bTimerActive = FALSE;

static HWND g_hWnd = NULL;
static HPOWERNOTIFY g_hPowerNotify = NULL;

static const UINT_PTR SLEEP_TIMER_ID = 0xD00D;
// Custom message to tell the entry point to reload settings
static const UINT WM_RELOAD_SETTINGS = WM_USER + 100;
// Custom message to tell the entry point to quit
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
    // Use the full SYSTEM_POWER_CAPABILITIES from <powrprof.h> so
    // CallNtPowerInformation receives a buffer large enough to succeed.
    SYSTEM_POWER_CAPABILITIES caps = {};
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

// ─── Window Proc (runs on the entry-point thread) ───────────────────────────

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
                        int delay = g_sleepDelaySeconds.load();
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
            Wh_Log(L"Received WM_RELOAD_SETTINGS");
            // If timer is active, restart it with new delay
            if (g_bTimerActive) {
                KillTimer(g_hWnd, SLEEP_TIMER_ID);
                int delay = g_sleepDelaySeconds.load();
                SetTimer(g_hWnd, SLEEP_TIMER_ID, (UINT)(delay * 1000),
                         SleepTimerProc);
                Wh_Log(L"Timer restarted with new delay: %d seconds", delay);
            }
            return 0;
        }

        case WM_QUIT_THREAD: {
            Wh_Log(L"Received WM_QUIT_THREAD");
            if (g_bTimerActive) {
                KillTimer(g_hWnd, SLEEP_TIMER_ID);
                g_bTimerActive = FALSE;
            }
            SetThreadExecutionState(ES_CONTINUOUS);
            DestroyWindow(hWnd);
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

// ─── Settings ────────────────────────────────────────────────────────────────

static int ParseDelayOption(PCWSTR option) {
    if (wcscmp(option, L"30sec") == 0)  return 30;
    if (wcscmp(option, L"1min") == 0)   return 60;
    if (wcscmp(option, L"5min") == 0)   return 300;
    if (wcscmp(option, L"10min") == 0)  return 600;
    return 60;
}

static void LoadSettings() {
    PCWSTR delayStr = Wh_GetStringSetting(L"SleepDelay");
    g_sleepDelaySeconds = ParseDelayOption(delayStr);
    Wh_FreeStringSetting(delayStr);
    Wh_Log(L"Settings loaded: sleepDelaySeconds = %d",
           g_sleepDelaySeconds.load());
}

// ─── Windhawk Tool-Mod Callbacks ─────────────────────────────────────────────
//
// This mod runs as a dedicated-process "tool mod": it targets windhawk.exe and
// uses WhTool_* callbacks instead of Wh_*.  The Windhawk engine launches a
// dedicated process, calls WhTool_ModInit, then WhTool_ModEntryPoint (which
// blocks).  Settings changes and uninit are dispatched on a separate thread.
//
// Benefits over explorer.exe injection:
//   • A crash here cannot destabilize the shell.
//   • Single-instance is handled automatically (no hand-rolled mutex).
//   • On unload the entire process exits — no TerminateThread needed.

BOOL WhTool_ModInit() {
    Wh_Log(L"=== Lid Close Sleep Delay v1.1: Init ===");

    // ── Detect if this system has a lid ──
    if (!IsLidCapableSystem()) {
        Wh_Log(L"This system does not have a lid switch (desktop/server). "
               L"Mod will not activate.");
        return FALSE;
    }
    Wh_Log(L"Lid-capable system detected — proceeding with initialization");

    LoadSettings();
    EnableShutdownPrivilege();

    Wh_Log(L"=== Init complete. Delay: %d sec ===",
           g_sleepDelaySeconds.load());
    return TRUE;
}

void WhTool_ModEntryPoint() {
    Wh_Log(L"[EntryPoint] Starting (TID: %lu)", GetCurrentThreadId());

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkLidSleepDelay";

    ATOM atom = RegisterClassEx(&wc);
    if (!atom) {
        Wh_Log(L"[EntryPoint] RegisterClassEx failed: %lu", GetLastError());
        return;
    }

    // Create a hidden window to receive WM_POWERBROADCAST and timer messages
    g_hWnd = CreateWindowEx(
        0,
        L"WindhawkLidSleepDelay",
        L"LidSleepDelay",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!g_hWnd) {
        Wh_Log(L"[EntryPoint] CreateWindowEx failed: %lu", GetLastError());
        UnregisterClass(L"WindhawkLidSleepDelay", GetModuleHandle(NULL));
        return;
    }

    ShowWindow(g_hWnd, SW_HIDE);
    Wh_Log(L"[EntryPoint] Window created: HWND=%p", (void*)g_hWnd);

    // Register for lid switch power notifications
    g_hPowerNotify = RegisterPowerSettingNotification(
        g_hWnd,
        &GUID_LIDSWITCH_STATE_CHANGE_LOCAL,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    if (!g_hPowerNotify) {
        Wh_Log(L"[EntryPoint] RegisterPowerSettingNotification failed: %lu",
               GetLastError());
        DestroyWindow(g_hWnd);
        g_hWnd = NULL;
        UnregisterClass(L"WindhawkLidSleepDelay", GetModuleHandle(NULL));
        return;
    }

    Wh_Log(L"[EntryPoint] Power notification registered — listening for lid "
           L"events");
    Wh_Log(L"[EntryPoint] Current delay: %d seconds",
           g_sleepDelaySeconds.load());

    // ── Message Loop ──
    // This blocks until PostQuitMessage is called (via WM_QUIT_THREAD →
    // DestroyWindow → WM_DESTROY → PostQuitMessage).
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"[EntryPoint] Message loop exited — cleaning up");

    // Final cleanup (WM_DESTROY already unregistered the power notification)
    UnregisterClass(L"WindhawkLidSleepDelay", GetModuleHandle(NULL));
    g_hWnd = NULL;

    Wh_Log(L"[EntryPoint] Exiting");
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");
    int oldDelay = g_sleepDelaySeconds.load();
    LoadSettings();
    Wh_Log(L"Delay: %d -> %d seconds", oldDelay, g_sleepDelaySeconds.load());

    // Tell the entry-point thread to reload (it reads g_sleepDelaySeconds
    // atomically; posting the message restarts any active timer with the new
    // delay).
    if (g_hWnd) {
        PostMessage(g_hWnd, WM_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"=== Lid Close Sleep Delay: Uninit ===");

    // Ask the entry-point thread to stop.  If it doesn't exit in time, the
    // tool-mod framework will ExitProcess the dedicated process — which is
    // safe and clean (no host process to corrupt).
    if (g_hWnd) {
        PostMessage(g_hWnd, WM_QUIT_THREAD, 0, 0);
    }

    Wh_Log(L"=== Uninit complete ===");
}

// ─── Windhawk Tool Mod Boilerplate ───────────────────────────────────────────
//
// Implementation for mods which don't need to inject to other processes.
// Context: https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    WhTool_ModEntryPoint();
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

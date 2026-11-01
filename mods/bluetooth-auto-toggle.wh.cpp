// ==WindhawkMod==
// @id              bluetooth-auto-toggle
// @name            Bluetooth Auto Toggle
// @description     Automatically cycles Bluetooth off and back on in milliseconds when a connected device is detected at startup
// @version         1.1.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bluetooth Auto Toggle

A Windhawk tool mod that automatically cycles Windows Bluetooth off and back on (in milliseconds) whenever Windhawk boots with your system and a connected Bluetooth device is detected.

### Why this mod?
Some Bluetooth hardware adapters, drivers, and wireless peripherals (e.g., Bluetooth headphones, earbuds, mice, and keyboards) enter an unresponsive or degraded state on fresh system boots—even when Windows displays them as "connected"—causing audio silence, latency, or connection drops unless Bluetooth is power-cycled. This mod automates that process by waiting until a device is connected, pausing a configurable few seconds for the driver stack to settle, and cleanly cycling the Bluetooth radio off and then back on in the background.

### Key Features
- **Device-Aware Activation:** By default, only cycles Bluetooth when a connected Bluetooth device (headset, mouse, keyboard, etc.) is found.
- **Precise Millisecond Timing:** Configurable startup delay, post-connection delay, and off-to-on toggle duration in milliseconds.
- **Dedicated Tool Mod Architecture:** Runs in an isolated `windhawk.exe` background tool process without injecting into or hooking `explorer.exe` or any other critical system process.
- **Cold Boot Hardware Retry:** Automatically retries radio enumeration if the Bluetooth service (`bthserv`) or adapter driver is still initializing during Windows boot.
- **State-Safe Cycling:** Only toggles radios that are already powered ON (leaving radios in Airplane mode or turned off alone).
- **Safe Power Restoration:** Minimizes the off window and ensures radios are commanded back on with retries even if the mod is unloaded mid-cycle.
- **Session-Safe:** Remembers the current Windows logon session across tool process restarts using Windows Terminal Services (WTS) logon timestamps.
- **Graceful Lifecycle & Live Testing:** Responsive to settings updates, mod reload, and instant cancellation on shutdown.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- onlyWhenDeviceConnected: true
  $name: Only When Device Connected
  $description: If enabled, Bluetooth is only cycled after a connected Bluetooth device (headset, mouse, keyboard, etc.) is detected. If disabled, Bluetooth cycles on startup regardless of connected devices.

- delayAfterDeviceConnectedMs: 2000
  $name: Delay After Device Found (ms)
  $description: Milliseconds to wait after a connected device is detected before executing the off-and-on cycle (minimum 0 ms).

- deviceWaitTimeoutSec: 60
  $name: Device Connection Wait Timeout (seconds)
  $description: Maximum seconds to wait for a Bluetooth device to connect after startup (0 for indefinite/infinite wait until a device connects).

- startupDelayMs: 2000
  $name: Startup Settle Delay (ms)
  $description: Milliseconds to wait after Windhawk starts before beginning checks (allows the driver stack to settle, minimum 0 ms).

- toggleDelayMs: 500
  $name: Toggle Off-to-On Duration (ms)
  $description: Duration in milliseconds the Bluetooth radio remains OFF before turning back ON (minimum 50 ms).

- maxRetries: 10
  $name: Radio Detection Retries
  $description: Number of attempts to locate the Bluetooth radio if not immediately enumerated on cold boot (minimum 1).

- retryIntervalMs: 500
  $name: Radio Retry Interval (ms)
  $description: Milliseconds to wait between radio discovery retry attempts (minimum 50 ms).

- onlyOncePerSession: true
  $name: Toggle Only Once Per Logon Session
  $description: Prevents re-cycling Bluetooth if Windhawk is restarted or updated during the same Windows logon session.

- forceToggleOnSettingsChange: false
  $name: Re-toggle on Settings Save
  $description: If enabled, saving settings in Windhawk will immediately trigger a Bluetooth off-on toggle test (bypassing session check, startup delay, and connected device check).
*/
// ==/WindhawkModSettings==

// clang-format off
#include <windows.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <utility>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Radios.h>
// clang-format on

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Radios;

struct Settings {
    bool onlyWhenDeviceConnected;
    int delayAfterDeviceConnectedMs;
    int deviceWaitTimeoutSec;
    int startupDelayMs;
    int toggleDelayMs;
    int maxRetries;
    int retryIntervalMs;
    bool onlyOncePerSession;
    bool forceToggleOnSettingsChange;
};

static Settings g_settings;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static HANDLE g_hWorkerThread = NULL;
static HANDLE g_hStopEvent = NULL;
static HANDLE g_hTriggerEvent = NULL;
static std::atomic<bool> g_forceNextRun{false};

static void LoadSettings() {
    AcquireSRWLockExclusive(&g_settingsLock);

    g_settings.onlyWhenDeviceConnected = Wh_GetIntSetting(L"onlyWhenDeviceConnected") != 0;

    g_settings.delayAfterDeviceConnectedMs =
        std::max(Wh_GetIntSetting(L"delayAfterDeviceConnectedMs"), 0);

    g_settings.deviceWaitTimeoutSec =
        std::max(Wh_GetIntSetting(L"deviceWaitTimeoutSec"), 0);

    g_settings.startupDelayMs =
        std::max(Wh_GetIntSetting(L"startupDelayMs"), 0);

    g_settings.toggleDelayMs =
        std::max(Wh_GetIntSetting(L"toggleDelayMs"), 50);

    g_settings.maxRetries =
        std::max(Wh_GetIntSetting(L"maxRetries"), 1);

    g_settings.retryIntervalMs =
        std::max(Wh_GetIntSetting(L"retryIntervalMs"), 50);

    g_settings.onlyOncePerSession = Wh_GetIntSetting(L"onlyOncePerSession") != 0;
    g_settings.forceToggleOnSettingsChange = Wh_GetIntSetting(L"forceToggleOnSettingsChange") != 0;

    ReleaseSRWLockExclusive(&g_settingsLock);

    Wh_Log(L"Settings: onlyWhenConnected=%d, delayAfterConn=%dms, waitTimeout=%ds, startupDelay=%dms, toggleDelay=%dms, onlyOnce=%d",
           (int)g_settings.onlyWhenDeviceConnected, g_settings.delayAfterDeviceConnectedMs,
           g_settings.deviceWaitTimeoutSec, g_settings.startupDelayMs,
           g_settings.toggleDelayMs, (int)g_settings.onlyOncePerSession);
}

static bool GetLogonMarker(WCHAR (&valueName)[64], LONGLONG* logonTime) {
    DWORD sessionId;
    WTSINFOW* info;
    DWORD bytes;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) ||
        !WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, sessionId,
                                     WTSSessionInfo, (LPWSTR*)&info, &bytes)) {
        return false;
    }
    *logonTime = info->LogonTime.QuadPart;
    WTSFreeMemory(info);
    swprintf_s(valueName, L"toggledLogon_%u", sessionId);
    return true;
}

static bool HasToggledInThisSession() {
    WCHAR valueName[64];
    LONGLONG logonTime, stored = 0;
    return GetLogonMarker(valueName, &logonTime) &&
           Wh_GetBinaryValue(valueName, &stored, sizeof(stored)) == sizeof(stored) &&
           stored == logonTime;
}

static void MarkToggledInThisSession() {
    WCHAR valueName[64];
    LONGLONG logonTime;
    if (GetLogonMarker(valueName, &logonTime)) {
        Wh_SetBinaryValue(valueName, &logonTime, sizeof(logonTime));
    }
}

static bool GetFirstConnectedBluetoothDevice(std::wstring& outName) {
    try {
        // 1. Check Classic Bluetooth connected devices
        auto aqsClassic = BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
        auto devicesClassic = DeviceInformation::FindAllAsync(aqsClassic).get();
        if (devicesClassic.Size() > 0) {
            outName = devicesClassic.GetAt(0).Name().c_str();
            return true;
        }

        // 2. Check BLE connected devices
        auto aqsBle = BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
        auto devicesBle = DeviceInformation::FindAllAsync(aqsBle).get();
        if (devicesBle.Size() > 0) {
            outName = devicesBle.GetAt(0).Name().c_str();
            return true;
        }
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Exception checking connected devices: 0x%08X (%s)",
               (unsigned int)ex.code(), ex.message().c_str());
    }
    return false;
}

static void PerformBluetoothToggle(bool isManualTest = false) {
    Settings cfg;
    AcquireSRWLockShared(&g_settingsLock);
    cfg = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);

    if (!isManualTest && cfg.onlyOncePerSession && HasToggledInThisSession()) {
        Wh_Log(L"Bluetooth has already been toggled in this logon session. Skipping.");
        return;
    }

    if (!isManualTest && cfg.startupDelayMs > 0) {
        Wh_Log(L"Waiting initial startup delay of %d ms...", cfg.startupDelayMs);
        if (WaitForSingleObject(g_hStopEvent, cfg.startupDelayMs) == WAIT_OBJECT_0) {
            Wh_Log(L"Stop event signaled during startup delay.");
            return;
        }
    }

    // If configured to only toggle when a device is connected (bypassed on manual test):
    if (!isManualTest && cfg.onlyWhenDeviceConnected) {
        Wh_Log(L"Monitoring for connected Bluetooth device (timeout: %d s)...", cfg.deviceWaitTimeoutSec);
        std::wstring connectedDeviceName;
        DWORD startTick = GetTickCount();

        while (true) {
            if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
                return;
            }

            if (GetFirstConnectedBluetoothDevice(connectedDeviceName)) {
                Wh_Log(L"Connected Bluetooth device detected: \"%s\"", connectedDeviceName.c_str());
                break;
            }

            if (cfg.deviceWaitTimeoutSec > 0) {
                DWORD elapsedSec = (GetTickCount() - startTick) / 1000;
                if (elapsedSec >= (DWORD)cfg.deviceWaitTimeoutSec) {
                    Wh_Log(L"No Bluetooth device connected within timeout window (%d s). Skipping toggle.",
                           cfg.deviceWaitTimeoutSec);
                    return;
                }
            }

            // Wait 1 second before re-checking, or until stop event
            if (WaitForSingleObject(g_hStopEvent, 1000) == WAIT_OBJECT_0) {
                return;
            }
        }

        // Wait configured settle delay after device is found (the "within seconds" settle time)
        if (cfg.delayAfterDeviceConnectedMs > 0) {
            Wh_Log(L"Waiting %d ms settle time after device connection before toggling...",
                   cfg.delayAfterDeviceConnectedMs);
            if (WaitForSingleObject(g_hStopEvent, cfg.delayAfterDeviceConnectedMs) == WAIT_OBJECT_0) {
                return;
            }
        }
    }

    // Locate Bluetooth radio(s) with retries
    std::vector<Radio> targetRadios;
    for (int attempt = 1; attempt <= cfg.maxRetries; ++attempt) {
        if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
            return;
        }

        try {
            RadioAccessStatus access = Radio::RequestAccessAsync().get();
            if (access == RadioAccessStatus::Allowed) {
                auto radios = Radio::GetRadiosAsync().get();
                for (const auto& r : radios) {
                    if (r.Kind() == RadioKind::Bluetooth) {
                        targetRadios.push_back(r);
                    }
                }
            } else {
                Wh_Log(L"Radio::RequestAccessAsync returned status: %d (attempt %d/%d)",
                       (int)access, attempt, cfg.maxRetries);
            }
        } catch (const winrt::hresult_error& ex) {
            Wh_Log(L"Exception during radio discovery (attempt %d): 0x%08X (%s)",
                   attempt, (unsigned int)ex.code(), ex.message().c_str());
        }

        if (!targetRadios.empty()) {
            Wh_Log(L"Found %u Bluetooth radio(s) on attempt %d.", (unsigned int)targetRadios.size(), attempt);
            break;
        }

        if (attempt < cfg.maxRetries) {
            Wh_Log(L"Bluetooth radio not found yet (attempt %d/%d). Retrying in %d ms...",
                   attempt, cfg.maxRetries, cfg.retryIntervalMs);
            if (WaitForSingleObject(g_hStopEvent, cfg.retryIntervalMs) == WAIT_OBJECT_0) {
                return;
            }
        }
    }

    if (targetRadios.empty()) {
        Wh_Log(L"No Bluetooth radio found after %d detection attempts.", cfg.maxRetries);
        return;
    }

    // Step 1: Turn OFF only radios that are currently ON
    std::vector<std::pair<Radio, std::wstring>> cycledRadios;
    for (const auto& radio : targetRadios) {
        try {
            std::wstring name = radio.Name().c_str();
            RadioState state = radio.State();
            if (state != RadioState::On) {
                Wh_Log(L"Adapter \"%s\" is not ON (state: %d). Leaving as-is.", name.c_str(), (int)state);
                continue;
            }

            Wh_Log(L"Adapter \"%s\" is ON. Commanding OFF...", name.c_str());
            RadioAccessStatus res = radio.SetStateAsync(RadioState::Off).get();
            if (res == RadioAccessStatus::Allowed) {
                cycledRadios.emplace_back(radio, name);
                Wh_Log(L"Adapter \"%s\" commanded OFF successfully.", name.c_str());
            } else {
                Wh_Log(L"Adapter \"%s\" -> SetStateAsync(Off) status: %d", name.c_str(), (int)res);
            }
        } catch (const winrt::hresult_error& ex) {
            Wh_Log(L"Error commanding Bluetooth OFF: 0x%08X (%s)",
                   (unsigned int)ex.code(), ex.message().c_str());
        }
    }

    if (cycledRadios.empty()) {
        Wh_Log(L"No active Bluetooth radios to cycle.");
        return;
    }

    // Step 2: Wait the configured milliseconds between OFF and ON.
    // On stop event, skip the remaining wait, but NEVER exit before powering radios back on.
    Wh_Log(L"Waiting %d ms before turning Bluetooth back ON...", cfg.toggleDelayMs);
    WaitForSingleObject(g_hStopEvent, cfg.toggleDelayMs);

    // Step 3: Turn cycled radios back ON (with retry on failure)
    for (const auto& [radio, name] : cycledRadios) {
        Wh_Log(L"Adapter \"%s\". Commanding back ON...", name.c_str());

        bool turnedOn = false;
        for (int retry = 0; retry < 3 && !turnedOn; ++retry) {
            try {
                turnedOn = radio.SetStateAsync(RadioState::On).get() ==
                           RadioAccessStatus::Allowed;
            } catch (...) {
            }
            if (!turnedOn) {
                Sleep(100);
            }
        }

        try {
            Wh_Log(L"Adapter \"%s\" final state: %d (success=%d)", name.c_str(),
                   (int)radio.State(), (int)turnedOn);
        } catch (...) {
            Wh_Log(L"Adapter \"%s\" state read failed after turn ON (success=%d)",
                   name.c_str(), (int)turnedOn);
        }
    }

    if (!isManualTest) {
        MarkToggledInThisSession();
    }
    Wh_Log(L"Bluetooth automatic toggle cycle completed successfully.");
}

static DWORD WINAPI BluetoothWorkerThread(LPVOID lpParam) {
    Wh_Log(L"Bluetooth worker thread started.");

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Failed to initialize WinRT apartment: 0x%08X", (unsigned int)ex.code());
        return 1;
    }

    HANDLE waitHandles[] = { g_hStopEvent, g_hTriggerEvent };

    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
        if (waitRes == WAIT_OBJECT_0) {
            Wh_Log(L"Worker thread stop event signaled.");
            break;
        } else if (waitRes == WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Toggle trigger event signaled.");
            bool isManual = g_forceNextRun.exchange(false);
            PerformBluetoothToggle(isManual);
        } else {
            break;
        }
    }

    try {
        winrt::uninit_apartment();
    } catch (...) {
    }

    Wh_Log(L"Bluetooth worker thread exiting.");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Bluetooth Auto Toggle tool mod initializing...");

    LoadSettings();

    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) {
        Wh_Log(L"CreateEvent (stop) failed: %d", GetLastError());
        return FALSE;
    }

    // Auto-reset event, initially signaled to trigger on startup
    g_hTriggerEvent = CreateEventW(NULL, FALSE, TRUE, NULL);
    if (!g_hTriggerEvent) {
        Wh_Log(L"CreateEvent (trigger) failed: %d", GetLastError());
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    g_hWorkerThread = CreateThread(NULL, 0, BluetoothWorkerThread, NULL, 0, NULL);
    if (!g_hWorkerThread) {
        Wh_Log(L"CreateThread failed: %d", GetLastError());
        CloseHandle(g_hTriggerEvent);
        g_hTriggerEvent = NULL;
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    Wh_Log(L"Bluetooth Auto Toggle tool mod initialized successfully.");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Bluetooth Auto Toggle tool mod uninitializing...");

    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    if (g_hWorkerThread) {
        // Wait up to 15 seconds for the worker thread to finish the turn-on step and exit cleanly
        WaitForSingleObject(g_hWorkerThread, 15000);
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = NULL;
    }

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    if (g_hTriggerEvent) {
        CloseHandle(g_hTriggerEvent);
        g_hTriggerEvent = NULL;
    }

    Wh_Log(L"Bluetooth Auto Toggle tool mod uninitialized.");
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed notification received.");
    LoadSettings();

    AcquireSRWLockShared(&g_settingsLock);
    bool force = g_settings.forceToggleOnSettingsChange;
    ReleaseSRWLockShared(&g_settingsLock);

    if (force && g_hTriggerEvent) {
        Wh_Log(L"forceToggleOnSettingsChange enabled, triggering immediate test toggle...");
        g_forceNextRun.store(true);
        SetEvent(g_hTriggerEvent);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
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
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS *ntHeaders =
            (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
    case 0:
    case ARRAYSIZE(currentProcessPath):
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
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

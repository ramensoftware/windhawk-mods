// ==WindhawkMod==
// @id              energy-saver-power-plan-link
// @name            Energy Saver Power Plan Link
// @description     Links Windows 11 Energy Saver with the Power Saver power plan and restores the previous plan when disabled.
// @version         1.0.0
// @author          Yusseter
// @github          https://github.com/Yusseter
// @homepage        https://github.com/Yusseter/energy-saver-power-plan-link
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lpowrprof -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Energy Saver Power Plan Link

When Windows Energy Saver is enabled:

- Saves the currently active power plan.
- Switches to the standard Power Saver plan.

When Energy Saver is disabled:

- Restores the saved plan only if Power Saver is still active.
- If the user manually selected another plan, that selection is preserved.

Disabling the mod also restores the previous plan when appropriate.

> On some Modern Standby systems, the classic Power Saver plan might not be
> available. In that case, Windows can't switch to that plan and the mod leaves
> the current plan unchanged.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <powrprof.h>
#include <powersetting.h>
#include <shellapi.h>
#include <stdio.h>

#include <cstring>
#include <mutex>

// Some SDK versions might not define this value.
#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 2
#endif

using DeviceNotifyCallbackRoutine =
    ULONG(CALLBACK*)(PVOID context, ULONG type, PVOID setting);

struct DeviceNotifySubscribeParameters {
    DeviceNotifyCallbackRoutine Callback;
    PVOID Context;
};

namespace {

// GUID_ENERGY_SAVER_STATUS
// 550E8400-E29B-41D4-A716-446655440000
constexpr GUID kEnergySaverStatusGuid = {
    0x550E8400,
    0xE29B,
    0x41D4,
    {0xA7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00}
};

// Standard Windows Power Saver plan
// A1841308-3541-4FAB-BC81-F71556F20B4A
constexpr GUID kPowerSaverSchemeGuid = {
    0xA1841308,
    0x3541,
    0x4FAB,
    {0xBC, 0x81, 0xF7, 0x15, 0x56, 0xF2, 0x0B, 0x4A}
};

constexpr wchar_t kPreviousSchemeValueName[] = L"PreviousPowerScheme";

HPOWERNOTIFY g_notificationHandle = nullptr;
HANDLE g_keepAliveEvent = nullptr;
HANDLE g_keepAliveThread = nullptr;
std::mutex g_stateMutex;

bool GetActivePowerScheme(GUID* scheme) {
    GUID* activeScheme = nullptr;

    DWORD error = PowerGetActiveScheme(nullptr, &activeScheme);
    if (error != ERROR_SUCCESS || !activeScheme) {
        Wh_Log(L"PowerGetActiveScheme failed: %lu", error);
        return false;
    }

    *scheme = *activeScheme;
    LocalFree(activeScheme);
    return true;
}

bool LoadPreviousPowerScheme(GUID* scheme) {
    size_t bytesRead = Wh_GetBinaryValue(
        kPreviousSchemeValueName,
        scheme,
        sizeof(*scheme)
    );

    return bytesRead == sizeof(*scheme);
}

bool SavePreviousPowerScheme(const GUID& scheme) {
    if (!Wh_SetBinaryValue(
            kPreviousSchemeValueName,
            &scheme,
            sizeof(scheme))) {
        Wh_Log(L"Failed to save the previous power scheme");
        return false;
    }

    return true;
}

void ClearPreviousPowerScheme() {
    Wh_DeleteValue(kPreviousSchemeValueName);
}

void EnableLinkedPowerSaver() {
    GUID activeScheme{};

    if (!GetActivePowerScheme(&activeScheme)) {
        return;
    }

    // The user was already using Power Saver before Energy Saver was enabled.
    // In this case there is nothing to save or change.
    if (InlineIsEqualGUID(activeScheme, kPowerSaverSchemeGuid)) {
        Wh_Log(L"Energy Saver enabled; Power Saver is already active");
        return;
    }

    GUID storedPreviousScheme{};

    if (LoadPreviousPowerScheme(&storedPreviousScheme)) {
        /*
        A previous plan is already stored, but the current plan isn't
        Power Saver. This normally means that the user manually changed
        the plan while Energy Saver was active.

        Don't override that manual selection.
        */
        Wh_Log(
            L"Energy Saver enabled, but the active plan was manually "
            L"changed; leaving it unchanged"
        );
        return;
    }

    if (!SavePreviousPowerScheme(activeScheme)) {
        return;
    }

    DWORD error = PowerSetActiveScheme(
        nullptr,
        &kPowerSaverSchemeGuid
    );

    if (error != ERROR_SUCCESS) {
        Wh_Log(L"PowerSetActiveScheme Power Saver failed: %lu", error);

        // Don't leave an invalid restoration record behind.
        ClearPreviousPowerScheme();
        return;
    }

    Wh_Log(L"Energy Saver enabled; switched to Power Saver");
}

void RestorePreviousPowerScheme() {
    GUID previousScheme{};

    if (!LoadPreviousPowerScheme(&previousScheme)) {
        return;
    }

    GUID activeScheme{};

    if (!GetActivePowerScheme(&activeScheme)) {
        // Keep the saved value so restoration can be retried later.
        return;
    }

    /*
    Only restore when Power Saver is still active.

    If another plan is active, the user changed it manually while
    Energy Saver was enabled. Preserve that manual selection.
    */
    if (!InlineIsEqualGUID(activeScheme, kPowerSaverSchemeGuid)) {
        Wh_Log(
            L"Power plan was manually changed; previous plan won't "
            L"be restored"
        );

        ClearPreviousPowerScheme();
        return;
    }

    DWORD error = PowerSetActiveScheme(
        nullptr,
        &previousScheme
    );

    if (error != ERROR_SUCCESS) {
        Wh_Log(L"Restoring the previous power scheme failed: %lu", error);

        // Keep the saved plan for a later retry.
        return;
    }

    ClearPreviousPowerScheme();
    Wh_Log(L"Energy Saver disabled; restored the previous power plan");
}

ULONG CALLBACK EnergySaverNotificationCallback(
    PVOID context,
    ULONG type,
    PVOID setting
) {
    UNREFERENCED_PARAMETER(context);

    if (type != PBT_POWERSETTINGCHANGE || !setting) {
        return ERROR_SUCCESS;
    }

    const auto* powerSetting =
        static_cast<const POWERBROADCAST_SETTING*>(setting);

    if (!InlineIsEqualGUID(
            powerSetting->PowerSetting,
            kEnergySaverStatusGuid)) {
        return ERROR_SUCCESS;
    }

    if (powerSetting->DataLength < sizeof(DWORD)) {
        Wh_Log(L"Unexpected Energy Saver notification data");
        return ERROR_SUCCESS;
    }

    DWORD energySaverStatus = 0;
    std::memcpy(
        &energySaverStatus,
        powerSetting->Data,
        sizeof(energySaverStatus)
    );

    /*
    ENERGY_SAVER_OFF          = 0
    ENERGY_SAVER_STANDARD     = 1
    ENERGY_SAVER_HIGH_SAVINGS = 2

    Both non-zero modes are treated as enabled.
    */
    std::lock_guard<std::mutex> lock(g_stateMutex);

    if (energySaverStatus == 0) {
        RestorePreviousPowerScheme();
    } else {
        EnableLinkedPowerSaver();
    }

    return ERROR_SUCCESS;
}

DWORD WINAPI KeepAliveThreadProc(LPVOID) {
    WaitForSingleObject(g_keepAliveEvent, INFINITE);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    g_keepAliveEvent = CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr
    );

    if (!g_keepAliveEvent) {
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        return FALSE;
    }

    g_keepAliveThread = CreateThread(
        nullptr,
        0,
        KeepAliveThreadProc,
        nullptr,
        0,
        nullptr
    );

    if (!g_keepAliveThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());

        CloseHandle(g_keepAliveEvent);
        g_keepAliveEvent = nullptr;
        return FALSE;
    }

    DeviceNotifySubscribeParameters parameters{};
    parameters.Callback = EnergySaverNotificationCallback;
    parameters.Context = nullptr;

    DWORD error = PowerSettingRegisterNotification(
        &kEnergySaverStatusGuid,
        DEVICE_NOTIFY_CALLBACK,
        reinterpret_cast<HANDLE>(&parameters),
        &g_notificationHandle
    );

    if (error != ERROR_SUCCESS) {
        Wh_Log(
            L"PowerSettingRegisterNotification failed: %lu",
            error
        );

        g_notificationHandle = nullptr;

        SetEvent(g_keepAliveEvent);
        WaitForSingleObject(g_keepAliveThread, 2000);

        CloseHandle(g_keepAliveThread);
        CloseHandle(g_keepAliveEvent);

        g_keepAliveThread = nullptr;
        g_keepAliveEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Energy Saver power-plan listener registered");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // This mod has no configurable settings.
}

void WhTool_ModUninit() {
    HPOWERNOTIFY notificationHandle = g_notificationHandle;
    g_notificationHandle = nullptr;

    if (notificationHandle) {
        DWORD error =
            PowerSettingUnregisterNotification(notificationHandle);

        if (error != ERROR_SUCCESS) {
            Wh_Log(
                L"PowerSettingUnregisterNotification failed: %lu",
                error
            );
        }
    }

    /*
    When the user disables or removes the mod, don't leave the computer
    stuck on Power Saver because of this mod.
    */
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        RestorePreviousPowerScheme();
    }

    if (g_keepAliveEvent) {
        SetEvent(g_keepAliveEvent);
    }

    if (g_keepAliveThread) {
        WaitForSingleObject(g_keepAliveThread, 2000);
        CloseHandle(g_keepAliveThread);
        g_keepAliveThread = nullptr;
    }

    if (g_keepAliveEvent) {
        CloseHandle(g_keepAliveEvent);
        g_keepAliveEvent = nullptr;
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
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
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

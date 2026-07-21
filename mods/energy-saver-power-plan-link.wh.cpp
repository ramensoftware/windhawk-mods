// ==WindhawkMod==
// @id              energy-saver-power-plan-link
// @name            Energy Saver Power Plan Link
// @description     Links Windows 11 Energy Saver with the Power Saver power plan and restores the previous plan when disabled.
// @version         1.0.0
// @author          Yusseter
// @github          https://github.com/Yusseter
// @homepage        https://github.com/Yusseter/energy-saver-power-plan-link
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpowrprof
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
*/
// ==/WindhawkModReadme==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <powrprof.h>
#include <powersetting.h>

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

}  // namespace

BOOL Wh_ModInit() {
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
        return FALSE;
    }

    Wh_Log(L"Energy Saver power-plan listener registered");
    return TRUE;
}

void Wh_ModBeforeUninit() {
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
    std::lock_guard<std::mutex> lock(g_stateMutex);
    RestorePreviousPowerScheme();
}

// ==WindhawkMod==
// @id              screen-off-no-sleep
// @name            Turn Off Display Without Sleep
// @description     Turns the display off without entering Modern Standby sleep, keeping background audio playback, downloads, and mobile hotspots alive.
// @version         1.0.0
// @author          mirukurusan
// @github          https://github.com/mirukurusan
// @homepage        https://github.com/mirukurusan/screen-off-no-sleep
// @license         MIT
// @include         windhawk-mod-uiaccess.exe
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -lpowrprof -luser32 -lhid -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Turn Off Display Without Sleep

On Modern Standby (S0ix) laptops, user-initiated actions—such as clicking "Sleep"
in the Start menu, pressing a power button configured for sleep, or invoking
display-off shortcuts—instruct the Windows Power Dependency Coordinator (PDC) to
transition to sleep. This suspends desktop apps (DAM) and disconnects mobile hotspots.

However, when Windows turns the display off through a **natural idle timeout**, Modern
Standby keeps audio streams, downloads, and mobile hotspots running indefinitely.

**Turn Off Display Without Sleep** triggers this natural idle path on demand:

1. **Instant Pitch-Black Screen**: Creates an instant fullscreen topmost blackout
   overlay across all monitors so the screen appears turned off immediately.
2. **Dynamic 1-Second Timeout**: Saves your current timeouts and temporarily injects
   a 1-second display idle timeout (`VIDEOIDLE`) into the active power scheme. Windows
   naturally powers down the monitors via the idle path within ~1–2 seconds without
   suspending background tasks.
3. **Automatic Restoration**: When hardware display-off (`GUID_CONSOLE_DISPLAY_STATE == 0`)
   is detected, the original display timeout is immediately restored, and the overlay
   is dismissed.
4. **Direct-to-Desktop Wake or Optional Lock**:
   - By default, your workstation stays unlocked so you return directly to your active
     desktop upon waking the screen—no PIN or Windows Hello delay.
   - If desired, enable **Lock workstation after display turns off** to silently lock the
     session in the dark once the screen is powered down.
5. **Failsafe Watchdog**: If the display does not power down within 3 seconds (e.g. due
   to mouse movement or an application holding `ES_DISPLAY_REQUIRED` / active video playback),
   the overlay is removed and timeouts are restored automatically.

### What happens on an unclean exit?
The 1-second timeout only exists in the power plan for the 1–3 seconds between trigger
and display power-down. If the tool process is forcefully terminated during those few
seconds (system crash, power loss, or process kill), the 1-second timeout remains in the
active power plan until the mod runs again and automatically restores it from storage.
If the screen turns off after one second of idle while the mod is disabled or uninstalled,
simply re-adjust "Turn off my screen after" in Windows Settings.

---

## Prerequisites

To allow this mod to take control of lid-close or power button events, Windows native actions 
must be set to "Do nothing" (0) so Windows does not trigger its built-in sleep transition.

Run this in an elevated Command Prompt:
```cmd
powercfg /setacvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 0
powercfg /setdcvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 0
powercfg /setacvalueindex SCHEME_CURRENT SUB_BUTTONS LIDACTION 0
powercfg /setdcvalueindex SCHEME_CURRENT SUB_BUTTONS LIDACTION 0
powercfg /setactive SCHEME_CURRENT
```

> **Warning**: With the lid action set to "Do nothing", closing the lid and putting the laptop
> into a backpack or bag will keep the system running (until the regular system sleep timeout).
> Make sure to explicitly sleep or shut down your laptop before bagging it.

### Reverting prerequisites
If you disable or uninstall this mod and want to restore the native sleep behavior:
```cmd
powercfg /setacvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 1
powercfg /setdcvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 1
powercfg /setacvalueindex SCHEME_CURRENT SUB_BUTTONS LIDACTION 1
powercfg /setdcvalueindex SCHEME_CURRENT SUB_BUTTONS LIDACTION 1
powercfg /setactive SCHEME_CURRENT
```

---

## Triggers

- **Keyboard Shortcut**: Default is `Win+Alt+L`. Customizable in settings.
- **Lid Close**: Triggered automatically when closing the laptop lid. Requires native lid 
  close action set to "Do nothing".
- **Power Button**: Supported on devices where the power button is exposed via HID System Control
  (Usage Page `0x01`, Usages `0x80`/`0x81`/`0x82` or `0x0D`, such as Microsoft Surface Pro).
  Requires native power button action set to "Do nothing".
  Not detectable on laptops with ACPI-only power buttons, use Trigger hotkey or Lid close instead.
- **External Window Message**: Broadcast `WindhawkScreenOffNoSleep_Trigger` from external scripts or taskbar shortcuts.

---

## Settings

- **Lock workstation after display turns off**: Silently lock in the dark after screen is off.
- **Trigger hotkey**: Global hotkey to turn off display (default `Win+Alt+L`).
- **Turn off display on lid close**: Trigger when laptop lid closes.
- **Turn off display on power button (HID devices)**: Capture HID power button events.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LockOnScreenOff: false
  $name: Lock workstation after display turns off
  $description: "If enabled, locks the workstation silently in the dark after the display powers off (requiring PIN/Windows Hello on wake). If disabled, waking returns directly to your active desktop."
- TriggerHotkey: "Win+Alt+L"
  $name: Trigger hotkey
  $description: "Global keyboard shortcut to turn off display (e.g. Win+Alt+L). Leave empty to disable."
- HandleLidClose: false
  $name: Turn off display on lid close
  $description: "Trigger display-off immediately when laptop lid closes. Requires native lid action set to 'Do nothing'."
- HandlePowerButton: false
  $name: Turn off display on power button (HID devices)
  $description: "Trigger display-off when pressing the hardware power button on devices that expose it via HID. Requires native power button action set to 'Do nothing'. Note: Not detectable on laptops with ACPI-only power buttons, use Trigger hotkey or Lid close instead."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <powrprof.h>
#include <hidsdi.h>
#include <windhawk_utils.h>
#include <atomic>
#include <cstring>
#include <string>
#include <vector>
#include <wchar.h>

// ─── GUIDs ───────────────────────────────────────────────────────────────────

// {7516b95f-f776-4464-8c53-06167f40cc99}
static const GUID GUID_VIDEO_SUBGROUP_LOCAL = {
    0x7516b95f, 0xf776, 0x4464,
    {0x8c, 0x53, 0x06, 0x16, 0x7f, 0x40, 0xcc, 0x99}
};

// {3c0bc021-c8a8-4e07-a973-6b14cbcb2b7e} — VIDEOIDLE (Turn off display after)
static const GUID GUID_VIDEO_IDLE_TIMEOUT_LOCAL = {
    0x3c0bc021, 0xc8a8, 0x4e07,
    {0xa9, 0x73, 0x6b, 0x14, 0xcb, 0xcb, 0x2b, 0x7e}
};

// {6fe69556-704a-47a0-8f24-c28d936fda47}
static const GUID GUID_CONSOLE_DISPLAY_STATE_LOCAL = {
    0x6fe69556, 0x704a, 0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}
};

// {ba3e0f4d-b817-4094-a2d1-d56379e6a0f3}
static const GUID GUID_LIDSWITCH_STATE_CHANGE_LOCAL = {
    0xba3e0f4d, 0xb817, 0x4094,
    {0xa2, 0xd1, 0xd5, 0x63, 0x79, 0xe6, 0xa0, 0xf3}
};

// {4f971e89-eebd-4455-a8de-9e59040e7347}
static const GUID GUID_SYSTEM_BUTTON_SUBGROUP_LOCAL = {
    0x4f971e89, 0xeebd, 0x4455,
    {0xa8, 0xde, 0x9e, 0x59, 0x04, 0x0e, 0x73, 0x47}
};

// {5ca83367-6e45-459f-a27b-476b1d01c936}
static const GUID GUID_LIDCLOSE_ACTION_LOCAL = {
    0x5ca83367, 0x6e45, 0x459f,
    {0xa2, 0x7b, 0x47, 0x6b, 0x1d, 0x01, 0xc9, 0x36}
};

// {7648efa3-dd9c-4e3e-b566-50f929386280}
static const GUID GUID_POWERBUTTON_ACTION_LOCAL = {
    0x7648efa3, 0xdd9c, 0x4e3e,
    {0xb5, 0x66, 0x50, 0xf9, 0x29, 0x38, 0x62, 0x80}
};

// ─── Constants ───────────────────────────────────────────────────────────────

static const UINT_PTR WATCHDOG_TIMER_ID = 0xD00E;
static const int TRIGGER_HOTKEY_ID = 0x4D49;

// A display-off notification normally arrives ~1-2 s after VIDEOIDLE=1s.
// 3000 ms leaves room for a slow power-manager round trip.
static const UINT WATCHDOG_TIMEOUT_MS = 3000;

// Ignore a power-button or hotkey report that arrives together with a display-on
// transition; it is the gesture that woke the device.
static const ULONGLONG WAKE_GUARD_MS = 1500;

static const wchar_t* MOD_WINDOW_CLASS = L"WindhawkScreenOffNoSleep";
static const wchar_t* BLACKOUT_WINDOW_CLASS = L"WindhawkScreenOffNoSleepBlackout";
static const wchar_t* TRIGGER_MESSAGE_NAME = L"WindhawkScreenOffNoSleep_Trigger";

static const UINT WM_RELOAD_SETTINGS = WM_USER + 100;
static const UINT WM_QUIT_THREAD = WM_USER + 101;

// ─── Global State ────────────────────────────────────────────────────────────

#pragma pack(push, 1)
struct SavedPowerState {
    GUID scheme;
    DWORD acTimeout;
    DWORD dcTimeout;
};
#pragma pack(pop)

static const wchar_t* SAVED_POWER_STATE_VALUE = L"SavedPowerState";

// Settings
static std::atomic<bool> g_lockOnScreenOff{false};
static std::atomic<bool> g_handleLidClose{false};
static std::atomic<bool> g_handlePowerButton{false};
static std::atomic<bool> g_shuttingDown{false};

// Power-scheme serialization
static CRITICAL_SECTION g_powerLock;
static bool g_powerLockInitialized = false;

// Flow state: holds the snapshot taken right before 1-second injection
static std::atomic<bool> g_flowActive{false};
static SavedPowerState g_flowSavedState = {};

// Entry-thread state
static HANDLE g_hThread = NULL;
static HANDLE g_hInitEvent = NULL;
static HWND g_hWnd = NULL;
static HWND g_hBlackoutWnd = NULL;
static HPOWERNOTIFY g_lidNotify = NULL;
static HPOWERNOTIFY g_displayNotify = NULL;
static UINT g_triggerMessage = 0;

static bool g_lidClosed = false;
static bool g_lidStateInitialized = false;
static std::atomic<bool> g_watchdogActive{false};
static bool g_hotkeyRegistered = false;

// Last known console display state (0 = off, 1 = on, 2 = dimmed).
static bool g_displayOn = true;
static ULONGLONG g_lastDisplayOnTick = 0;

// ─── Utilities ───────────────────────────────────────────────────────────────

static bool GuidEqual(const GUID& a, const GUID& b) {
    return memcmp(&a, &b, sizeof(GUID)) == 0;
}

static DWORD ReadPowerSettingDword(const POWERBROADCAST_SETTING* setting) {
    DWORD value = 0;
    DWORD bytes = setting->DataLength;
    if (bytes > sizeof(value)) {
        bytes = sizeof(value);
    }
    memcpy(&value, setting->Data, bytes);
    return value;
}

static bool GetActiveSchemeGuidNoLock(GUID* scheme) {
    GUID* active = nullptr;
    DWORD err = PowerGetActiveScheme(nullptr, &active);
    if (err != ERROR_SUCCESS || !active) {
        Wh_Log(L"PowerGetActiveScheme failed: %lu", err);
        return false;
    }

    *scheme = *active;
    LocalFree(active);
    return true;
}

struct WriteResult {
    bool acOk;
    bool dcOk;
};

// Must be called with g_powerLock held.
static WriteResult WriteTimeoutValuesNoLock(const GUID& scheme, DWORD acSeconds,
                                            DWORD dcSeconds) {
    WriteResult result = {};

    DWORD err = PowerWriteACValueIndex(
        nullptr, &scheme, &GUID_VIDEO_SUBGROUP_LOCAL,
        &GUID_VIDEO_IDLE_TIMEOUT_LOCAL, acSeconds);
    result.acOk = (err == ERROR_SUCCESS);
    if (!result.acOk) {
        Wh_Log(L"PowerWriteACValueIndex(%lu) failed: %lu", acSeconds, err);
    }

    err = PowerWriteDCValueIndex(
        nullptr, &scheme, &GUID_VIDEO_SUBGROUP_LOCAL,
        &GUID_VIDEO_IDLE_TIMEOUT_LOCAL, dcSeconds);
    result.dcOk = (err == ERROR_SUCCESS);
    if (!result.dcOk) {
        Wh_Log(L"PowerWriteDCValueIndex(%lu) failed: %lu", dcSeconds, err);
    }

    return result;
}

// Must be called with g_powerLock held.
static void ApplySchemeIfActiveNoLock(const GUID& scheme) {
    GUID active = {};
    if (!GetActiveSchemeGuidNoLock(&active)) {
        return;
    }

    if (!GuidEqual(active, scheme)) {
        return;
    }

    DWORD err = PowerSetActiveScheme(nullptr, &scheme);
    if (err != ERROR_SUCCESS) {
        Wh_Log(L"PowerSetActiveScheme failed: %lu", err);
    }
}


// ─── Blackout Overlay Window ─────────────────────────────────────────────────

static LRESULT CALLBACK BlackoutWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
    switch (uMsg) {
        case WM_SETCURSOR:
            SetCursor(NULL);
            return TRUE;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCHITTEST:
            return HTCLIENT;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void ShowBlackoutWindow() {
    if (g_hBlackoutWnd) {
        return;
    }

    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    g_hBlackoutWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        BLACKOUT_WINDOW_CLASS,
        L"",
        WS_POPUP,
        x, y, cx, cy,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (g_hBlackoutWnd) {
        SetWindowPos(g_hBlackoutWnd, HWND_TOPMOST, x, y, cx, cy,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE);
        UpdateWindow(g_hBlackoutWnd);
        Wh_Log(L"Blackout overlay displayed (instant visual blackout)");
    }
}

static void DestroyBlackoutWindow() {
    if (g_hBlackoutWnd) {
        DestroyWindow(g_hBlackoutWnd);
        g_hBlackoutWnd = NULL;
        Wh_Log(L"Blackout overlay destroyed");
    }
}

// ─── Settings ────────────────────────────────────────────────────────────────

static void LoadSettings() {
    g_lockOnScreenOff.store(Wh_GetIntSetting(L"LockOnScreenOff") != 0);
    g_handleLidClose.store(Wh_GetIntSetting(L"HandleLidClose") != 0);
    g_handlePowerButton.store(Wh_GetIntSetting(L"HandlePowerButton") != 0);

    Wh_Log(L"Settings: LockOnScreenOff=%d, HandleLidClose=%d, HandlePowerButton=%d",
           (int)g_lockOnScreenOff.load(), (int)g_handleLidClose.load(),
           (int)g_handlePowerButton.load());
}

// ─── Power Scheme Recovery & Flow State ──────────────────────────────────────

// Must be called with g_powerLock held.
static void ClearPowerStateRecordNoLock() {
    Wh_DeleteValue(SAVED_POWER_STATE_VALUE);
}

// Check for and recover any leftover 1-second timeout from a previous unclean shutdown / crash.
static void RecoverSavedPowerState() {
    EnterCriticalSection(&g_powerLock);

    SavedPowerState state = {};
    if (Wh_GetBinaryValue(SAVED_POWER_STATE_VALUE, &state, sizeof(state)) == sizeof(SavedPowerState)) {
        Wh_Log(L"Found leftover power state from unclean shutdown; restoring AC=%lu s, DC=%lu s",
               state.acTimeout, state.dcTimeout);

        WriteResult result = WriteTimeoutValuesNoLock(state.scheme, state.acTimeout, state.dcTimeout);
        if (result.acOk || result.dcOk) {
            ApplySchemeIfActiveNoLock(state.scheme);
            ClearPowerStateRecordNoLock();
        } else {
            DWORD dummy = 0;
            DWORD checkErr = PowerReadACValueIndex(
                nullptr, &state.scheme, &GUID_VIDEO_SUBGROUP_LOCAL,
                &GUID_VIDEO_IDLE_TIMEOUT_LOCAL, &dummy);
            if (checkErr != ERROR_SUCCESS) {
                Wh_Log(L"Recorded power scheme no longer exists (err=%lu); dropping stale record", checkErr);
                ClearPowerStateRecordNoLock();
            } else {
                Wh_Log(L"Failed to restore leftover power state; keeping record for future recovery");
            }
        }
    }

    LeaveCriticalSection(&g_powerLock);
}

static bool InjectOneSecondTimeout() {
    EnterCriticalSection(&g_powerLock);

    if (g_shuttingDown.load()) {
        LeaveCriticalSection(&g_powerLock);
        Wh_Log(L"Refusing to inject timeout: mod is shutting down");
        return false;
    }
    if (g_flowActive.load()) {
        LeaveCriticalSection(&g_powerLock);
        Wh_Log(L"Refusing to inject timeout: flow already active");
        return false;
    }

    GUID active = {};
    if (!GetActiveSchemeGuidNoLock(&active)) {
        LeaveCriticalSection(&g_powerLock);
        return false;
    }

    SavedPowerState state = {};
    bool hasExistingRecord = (Wh_GetBinaryValue(SAVED_POWER_STATE_VALUE, &state, sizeof(state)) == sizeof(SavedPowerState));

    if (hasExistingRecord && !GuidEqual(state.scheme, active)) {
        Wh_Log(L"Saved power state belongs to another scheme; attempting restoration of old scheme");
        WriteResult oldRes = WriteTimeoutValuesNoLock(state.scheme, state.acTimeout, state.dcTimeout);
        if (oldRes.acOk || oldRes.dcOk) {
            ApplySchemeIfActiveNoLock(state.scheme);
        }
        ClearPowerStateRecordNoLock();
        hasExistingRecord = false;
    }

    if (!hasExistingRecord) {
        DWORD currentAc = 0;
        DWORD currentDc = 0;
        DWORD acErr = PowerReadACValueIndex(
            nullptr, &active, &GUID_VIDEO_SUBGROUP_LOCAL,
            &GUID_VIDEO_IDLE_TIMEOUT_LOCAL, &currentAc);
        DWORD dcErr = PowerReadDCValueIndex(
            nullptr, &active, &GUID_VIDEO_SUBGROUP_LOCAL,
            &GUID_VIDEO_IDLE_TIMEOUT_LOCAL, &currentDc);

        if (acErr != ERROR_SUCCESS || dcErr != ERROR_SUCCESS) {
            Wh_Log(L"Failed to read current VIDEOIDLE (AC err=%lu, DC err=%lu); aborting injection",
                   acErr, dcErr);
            LeaveCriticalSection(&g_powerLock);
            return false;
        }

        state.scheme = active;
        state.acTimeout = currentAc;
        state.dcTimeout = currentDc;

        // Persist to mod storage before writing 1 second so an unclean exit can recover.
        if (!Wh_SetBinaryValue(SAVED_POWER_STATE_VALUE, &state, sizeof(state))) {
            Wh_Log(L"Failed to record power state to mod storage; aborting injection");
            LeaveCriticalSection(&g_powerLock);
            return false;
        }
    } else {
        Wh_Log(L"Using existing recorded power state: AC=%lu s, DC=%lu s",
               state.acTimeout, state.dcTimeout);
    }

    WriteResult result = WriteTimeoutValuesNoLock(active, 1, 1);
    if (!result.acOk && !result.dcOk) {
        if (!hasExistingRecord) {
            ClearPowerStateRecordNoLock();
        }
        Wh_Log(L"Failed to write the 1-second display-off timeout");
        LeaveCriticalSection(&g_powerLock);
        return false;
    }

    ApplySchemeIfActiveNoLock(active);

    g_flowSavedState = state;
    g_flowActive.store(true);

    LeaveCriticalSection(&g_powerLock);

    Wh_Log(L"Injected VIDEOIDLE=1 s (saved AC=%lu s, DC=%lu s, AC ok=%d, DC ok=%d)",
           state.acTimeout, state.dcTimeout, result.acOk, result.dcOk);
    return true;
}

// Must be called with g_powerLock held.
static void RestoreFlowSafeTimeoutNoLock() {
    if (!g_flowActive.load()) {
        return;
    }

    SavedPowerState state = g_flowSavedState;
    g_flowActive.store(false);

    WriteResult result = WriteTimeoutValuesNoLock(state.scheme, state.acTimeout, state.dcTimeout);
    if (result.acOk || result.dcOk) {
        ApplySchemeIfActiveNoLock(state.scheme);
        ClearPowerStateRecordNoLock();
    } else {
        Wh_Log(L"Failed to restore VIDEOIDLE; keeping saved record for startup recovery");
    }

    Wh_Log(L"Restored VIDEOIDLE: AC=%lu s, DC=%lu s (AC ok=%d, DC ok=%d)",
           state.acTimeout, state.dcTimeout, result.acOk, result.dcOk);
}

static void RestoreFlowSafeTimeout() {
    if (g_watchdogActive.exchange(false)) {
        KillTimer(g_hWnd, WATCHDOG_TIMER_ID);
    }

    EnterCriticalSection(&g_powerLock);
    RestoreFlowSafeTimeoutNoLock();
    LeaveCriticalSection(&g_powerLock);
}

static void RestoreOnUninit() {
    EnterCriticalSection(&g_powerLock);

    if (g_flowActive.load()) {
        Wh_Log(L"Uninit during active flow; restoring saved power state");
        RestoreFlowSafeTimeoutNoLock();
    }

    LeaveCriticalSection(&g_powerLock);
}

// ─── Raw Input (power button HID reports) ───────────────────────────────────

static bool RawInputIsPowerButton(RAWINPUT* raw) {
    if (!raw || raw->header.dwType != RIM_TYPEHID) {
        return false;
    }

    UINT preparsedSize = 0;
    if (GetRawInputDeviceInfoW(raw->header.hDevice, RIDI_PREPARSEDDATA, nullptr,
                               &preparsedSize) == (UINT)-1 ||
        preparsedSize == 0) {
        return false;
    }

    std::vector<BYTE> preparsedBuffer(preparsedSize);
    UINT copied = GetRawInputDeviceInfoW(
        raw->header.hDevice, RIDI_PREPARSEDDATA, preparsedBuffer.data(),
        &preparsedSize);
    if (copied == (UINT)-1 || copied == 0) {
        return false;
    }

    PHIDP_PREPARSED_DATA preparsed =
        reinterpret_cast<PHIDP_PREPARSED_DATA>(preparsedBuffer.data());

    HIDP_CAPS caps = {};
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return false;
    }

    // Query active usages across candidate usage pages:
    // 0x01: Generic Desktop (0x81 = Power Down, 0x82 = Sleep)
    // 0x0C: Consumer (0x30 = Power, 0x32 = Sleep)
    // 0x09: Button Page (1 = Button 1 / Power Button on Portable Device Control)
    USAGE usagesGeneric[16] = {};
    ULONG countGeneric = ARRAYSIZE(usagesGeneric);
    NTSTATUS statusGen = HidP_GetUsages(
        HidP_Input, 0x01, 0, usagesGeneric, &countGeneric, preparsed,
        reinterpret_cast<PCHAR>(raw->data.hid.bRawData),
        raw->data.hid.dwSizeHid);
    if (statusGen != HIDP_STATUS_SUCCESS) {
        countGeneric = 0;
    }

    USAGE usagesConsumer[16] = {};
    ULONG countConsumer = ARRAYSIZE(usagesConsumer);
    NTSTATUS statusCon = HidP_GetUsages(
        HidP_Input, 0x0C, 0, usagesConsumer, &countConsumer, preparsed,
        reinterpret_cast<PCHAR>(raw->data.hid.bRawData),
        raw->data.hid.dwSizeHid);
    if (statusCon != HIDP_STATUS_SUCCESS) {
        countConsumer = 0;
    }

    USAGE usagesButton[16] = {};
    ULONG countButton = ARRAYSIZE(usagesButton);
    NTSTATUS statusBtn = HidP_GetUsages(
        HidP_Input, 0x09, 0, usagesButton, &countButton, preparsed,
        reinterpret_cast<PCHAR>(raw->data.hid.bRawData),
        raw->data.hid.dwSizeHid);
    if (statusBtn != HIDP_STATUS_SUCCESS) {
        countButton = 0;
    }

    // Format raw bytes for diagnostic logging
    wchar_t rawHex[128] = {0};
    DWORD byteCount = raw->data.hid.dwSizeHid * raw->data.hid.dwCount;
    DWORD dumpCount = (byteCount > 16) ? 16 : byteCount;
    for (DWORD i = 0; i < dumpCount; i++) {
        wsprintfW(rawHex + wcslen(rawHex), L"%02X ",
                  (BYTE)raw->data.hid.bRawData[i]);
    }

    Wh_Log(L"[HID-Debug] TLC: Page=0x%02X Usage=0x%02X | Active counts: Gen=%lu Con=%lu Btn=%lu | Bytes(%lu)=[%s]",
           caps.UsagePage, caps.Usage, countGeneric, countConsumer, countButton, byteCount, rawHex);

    for (ULONG i = 0; i < countGeneric; i++) {
        Wh_Log(L"  -> Generic Desktop Usage: 0x%02X", usagesGeneric[i]);
    }
    for (ULONG i = 0; i < countConsumer; i++) {
        Wh_Log(L"  -> Consumer Usage: 0x%02X", usagesConsumer[i]);
    }
    for (ULONG i = 0; i < countButton; i++) {
        Wh_Log(L"  -> Button Usage: %lu", (ULONG)usagesButton[i]);
    }

    // 1. Generic Desktop Power Down (0x81) or Sleep (0x82)
    for (ULONG i = 0; i < countGeneric; i++) {
        if (usagesGeneric[i] == 0x81 || usagesGeneric[i] == 0x82) {
            Wh_Log(L"Matched Generic Desktop Power/Sleep Usage: 0x%02X", usagesGeneric[i]);
            return true;
        }
    }

    // 2. Consumer Power (0x30) or Sleep (0x32)
    for (ULONG i = 0; i < countConsumer; i++) {
        if (usagesConsumer[i] == 0x30 || usagesConsumer[i] == 0x32) {
            Wh_Log(L"Matched Consumer Power/Sleep Usage: 0x%02X", usagesConsumer[i]);
            return true;
        }
    }

    // 3. Portable Device Control (UsagePage 0x01, Usage 0x0D):
    // On Surface Button Array / Button Converter, Button 1 is the Power Button
    if (caps.UsagePage == 0x01 && caps.Usage == 0x0D) {
        for (ULONG i = 0; i < countButton; i++) {
            if (usagesButton[i] == 1) {
                Wh_Log(L"Matched Surface Portable Device Control Power Button (Button 1)");
                return true;
            }
        }
    }

    // 4. Direct TLC match if the collection itself is Power Down or Sleep and has payload
    if (caps.UsagePage == 0x01 && (caps.Usage == 0x81 || caps.Usage == 0x82)) {
        if (byteCount > 0 && raw->data.hid.bRawData[0] != 0) {
            Wh_Log(L"Matched TLC direct power/sleep usage 0x%02X with non-zero payload", caps.Usage);
            return true;
        }
    }

    return false;
}

static void RegisterPowerButtonRawInput() {
    struct CollectionDesc {
        USHORT page;
        USHORT usage;
        const wchar_t* desc;
    };

    static const CollectionDesc kCollections[] = {
        {0x01, 0x80, L"Generic Desktop: System Control (0x01/0x80)"},
        {0x01, 0x0D, L"Generic Desktop: Portable Device Control (Surface/Slate 0x01/0x0D)"},
        {0x01, 0x81, L"Generic Desktop: System Power Down (0x01/0x81)"},
        {0x01, 0x82, L"Generic Desktop: System Sleep (0x01/0x82)"},
        {0x0C, 0x01, L"Consumer: Consumer Control (0x0C/0x01)"},
    };

    for (const auto& item : kCollections) {
        RAWINPUTDEVICE rid = {};
        rid.usUsagePage = item.page;
        rid.usUsage = item.usage;
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = g_hWnd;

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            Wh_Log(L"Registered raw input: Page 0x%02X, Usage 0x%02X (%s)",
                   item.page, item.usage, item.desc);
        } else {
            Wh_Log(L"RegisterRawInputDevices(Page 0x%02X, Usage 0x%02X - %s) failed: %lu",
                   item.page, item.usage, item.desc, GetLastError());
        }
    }
}

static void UnregisterPowerButtonRawInput() {
    static const USHORT kPages[] = {0x01, 0x01, 0x01, 0x01, 0x0C};
    static const USHORT kUsages[] = {0x80, 0x0D, 0x81, 0x82, 0x01};

    for (size_t i = 0; i < ARRAYSIZE(kPages); i++) {
        RAWINPUTDEVICE rid = {};
        rid.usUsagePage = kPages[i];
        rid.usUsage = kUsages[i];
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }
}

static bool ShouldIgnorePowerButtonAsWake() {
    if (!g_displayOn) {
        Wh_Log(L"Power button while display is off: wake gesture, ignored");
        return true;
    }

    ULONGLONG now = GetTickCount64();
    if (g_lastDisplayOnTick != 0 &&
        now - g_lastDisplayOnTick < WAKE_GUARD_MS) {
        Wh_Log(L"Power button within display-on guard window: wake gesture, ignored");
        return true;
    }

    return false;
}

static bool IsWakeGuardActive() {
    if (!g_displayOn) {
        return true;
    }

    return g_lastDisplayOnTick != 0 &&
           GetTickCount64() - g_lastDisplayOnTick < WAKE_GUARD_MS;
}

// ─── Trigger Hotkey ─────────────────────────────────────────────────────────

static wchar_t LowerAscii(wchar_t c) {
    return (c >= L'A' && c <= L'Z') ? (wchar_t)(c + 32) : c;
}

static bool IsAsciiAlphaNumeric(wchar_t c) {
    return (c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9');
}

static std::wstring TrimHotkeyPart(const std::wstring& text) {
    size_t first = text.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return L"";
    }
    size_t last = text.find_last_not_of(L" \t");
    return text.substr(first, last - first + 1);
}

static bool ParseHotkeyKey(const std::wstring& part, UINT* vk) {
    if (part.empty()) {
        return false;
    }

    if (part.find_first_not_of(L"0123456789") == std::wstring::npos) {
        wchar_t* end = nullptr;
        unsigned long value = wcstoul(part.c_str(), &end, 10);
        if (end && *end == L'\0' && value > 0 && value <= 0xFF) {
            *vk = (UINT)value;
            return true;
        }
        return false;
    }

    if (part.size() > 2 && part[0] == L'0' && part[1] == L'x') {
        wchar_t* end = nullptr;
        unsigned long value = wcstoul(part.c_str(), &end, 16);
        if (end && *end == L'\0' && value > 0 && value <= 0xFF) {
            *vk = (UINT)value;
            return true;
        }
        return false;
    }

    if (part.size() == 1 && IsAsciiAlphaNumeric(part[0])) {
        *vk = (UINT)(part[0] >= L'a' ? part[0] - 32 : part[0]);
        return true;
    }

    if (part[0] == L'f' && part.size() <= 3 &&
        part.find_first_not_of(L"0123456789", 1) == std::wstring::npos) {
        wchar_t* end = nullptr;
        unsigned long number = wcstoul(part.c_str() + 1, &end, 10);
        if (end && *end == L'\0' && number >= 1 && number <= 24) {
            *vk = VK_F1 + (UINT)number - 1;
            return true;
        }
        return false;
    }

    struct NamedKey {
        const wchar_t* name;
        UINT vk;
    };
    static const NamedKey kNamedKeys[] = {
        {L"space", VK_SPACE},         {L"tab", VK_TAB},
        {L"enter", VK_RETURN},        {L"return", VK_RETURN},
        {L"esc", VK_ESCAPE},          {L"escape", VK_ESCAPE},
        {L"backspace", VK_BACK},      {L"insert", VK_INSERT},
        {L"ins", VK_INSERT},          {L"delete", VK_DELETE},
        {L"del", VK_DELETE},          {L"home", VK_HOME},
        {L"end", VK_END},             {L"pageup", VK_PRIOR},
        {L"pgup", VK_PRIOR},          {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT},           {L"up", VK_UP},
        {L"down", VK_DOWN},           {L"left", VK_LEFT},
        {L"right", VK_RIGHT},         {L"pause", VK_PAUSE},
        {L"printscreen", VK_SNAPSHOT}, {L"prtsc", VK_SNAPSHOT},
        {L"backquote", VK_OEM_3},     {L"`", VK_OEM_3},
        {L"plus", VK_OEM_PLUS},
        {L"minus", VK_OEM_MINUS},     {L"-", VK_OEM_MINUS},
        {L"comma", VK_OEM_COMMA},     {L",", VK_OEM_COMMA},
        {L"period", VK_OEM_PERIOD},   {L".", VK_OEM_PERIOD},
        {L"slash", VK_OEM_2},         {L"/", VK_OEM_2},
        {L"semicolon", VK_OEM_1},     {L";", VK_OEM_1},
        {L"quote", VK_OEM_7},         {L"'", VK_OEM_7},
        {L"leftbracket", VK_OEM_4},   {L"[", VK_OEM_4},
        {L"rightbracket", VK_OEM_6},  {L"]", VK_OEM_6},
        {L"backslash", VK_OEM_5},     {L"\\", VK_OEM_5},
    };
    for (const auto& key : kNamedKeys) {
        if (part == key.name) {
            *vk = key.vk;
            return true;
        }
    }

    return false;
}

static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;

    std::wstring lowered;
    lowered.reserve(text.size());
    for (wchar_t c : text) {
        lowered.push_back(LowerAscii(c));
    }

    bool gotKey = false;
    size_t start = 0;
    while (start <= lowered.size()) {
        size_t plus = lowered.find(L'+', start);
        std::wstring part = TrimHotkeyPart(lowered.substr(
            start, plus == std::wstring::npos ? std::wstring::npos
                                              : plus - start));
        bool last = (plus == std::wstring::npos);

        if (!part.empty()) {
            if (part == L"ctrl" || part == L"control") {
                *mods |= MOD_CONTROL;
            } else if (part == L"alt") {
                *mods |= MOD_ALT;
            } else if (part == L"shift") {
                *mods |= MOD_SHIFT;
            } else if (part == L"win" || part == L"windows") {
                *mods |= MOD_WIN;
            } else if (last && !gotKey) {
                gotKey = ParseHotkeyKey(part, vk);
                if (!gotKey) {
                    return false;
                }
            } else {
                return false;
            }
        }

        if (last) {
            break;
        }
        start = plus + 1;
    }

    return gotKey && *vk != 0;
}

static void UpdateTriggerHotkey() {
    if (!g_hWnd) {
        return;
    }

    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_hWnd, TRIGGER_HOTKEY_ID);
        g_hotkeyRegistered = false;
    }

    auto hotkeySetting = WindhawkUtils::StringSetting::make(L"TriggerHotkey");
    std::wstring text = hotkeySetting.get();

    if (text.empty()) {
        Wh_Log(L"Trigger hotkey disabled (empty setting)");
        return;
    }

    UINT mods = 0;
    UINT vk = 0;
    if (!ParseHotkey(text, &mods, &vk)) {
        Wh_Log(L"Invalid TriggerHotkey '%ls'; use e.g. Win+Alt+L or win+alt+76",
               text.c_str());
        return;
    }

    if (!RegisterHotKey(g_hWnd, TRIGGER_HOTKEY_ID, mods | MOD_NOREPEAT, vk)) {
        DWORD err = GetLastError();
        if (err == ERROR_HOTKEY_ALREADY_REGISTERED) {
            Wh_Log(L"Cannot register TriggerHotkey '%ls': already used by another program",
                   text.c_str());
        } else {
            Wh_Log(L"Cannot register TriggerHotkey '%ls': RegisterHotKey failed (%lu)",
                   text.c_str(), err);
        }
        return;
    }

    g_hotkeyRegistered = true;
    Wh_Log(L"Trigger hotkey registered: %ls (modifiers=0x%X, vk=0x%02X)",
           text.c_str(), mods, vk);
}

// ─── Flow State Machine ─────────────────────────────────────────────────────

static VOID CALLBACK WatchdogTimerProc(HWND hwnd, UINT, UINT_PTR, DWORD) {
    g_watchdogActive.store(false);
    KillTimer(hwnd, WATCHDOG_TIMER_ID);

    if (g_flowActive.load()) {
        Wh_Log(L"Watchdog: no display-off event within %u ms (user activity detected or an application is holding ES_DISPLAY_REQUIRED / video playback); restoring safe timeout and dismissing blackout",
               WATCHDOG_TIMEOUT_MS);
        RestoreFlowSafeTimeout();
        DestroyBlackoutWindow();
    }
}

// Source: 0 = lid, 1 = power button, 2 = external message, 3 = hotkey.
static void TriggerFlow(int source) {
    if (g_shuttingDown.load()) {
        return;
    }

    DWORD currentSessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &currentSessionId) ||
        currentSessionId != WTSGetActiveConsoleSessionId()) {
        Wh_Log(L"Trigger (%d) ignored: not in active console session", source);
        return;
    }

    if (g_flowActive.load()) {
        Wh_Log(L"Trigger (%d) ignored: flow already active", source);
        return;
    }

    switch (source) {
        case 0:
            Wh_Log(L"Starting screen-off flow: lid close");
            break;
        case 1:
            Wh_Log(L"Starting screen-off flow: power button");
            break;
        case 2:
            Wh_Log(L"Starting screen-off flow: external message");
            break;
        case 3:
            Wh_Log(L"Starting screen-off flow: hotkey");
            break;
        default:
            Wh_Log(L"Starting screen-off flow: source %d", source);
            break;
    }

    // 1. Immediately present the fullscreen blackout overlay
    ShowBlackoutWindow();

    // 2. Inject temporary 1-second VIDEOIDLE timeout
    if (!InjectOneSecondTimeout()) {
        Wh_Log(L"Flow aborted before timeout injection");
        DestroyBlackoutWindow();
        return;
    }

    if (g_shuttingDown.load()) {
        Wh_Log(L"Shutdown started during injection; aborting flow");
        RestoreFlowSafeTimeout();
        DestroyBlackoutWindow();
        return;
    }

    // 3. Start the watchdog timer
    g_watchdogActive.store(true);
    SetTimer(g_hWnd, WATCHDOG_TIMER_ID, WATCHDOG_TIMEOUT_MS,
             WatchdogTimerProc);

    Wh_Log(L"VIDEOIDLE=1 s injected with blackout overlay; waiting for GUID_CONSOLE_DISPLAY_STATE=0");
}

static void HandleLidState(bool isClosed) {
    if (!g_lidStateInitialized) {
        g_lidClosed = isClosed;
        g_lidStateInitialized = true;
        Wh_Log(L"Initial lid state notification received: %s (ignoring trigger)",
               isClosed ? L"closed" : L"open");
        return;
    }

    if (isClosed) {
        if (g_lidClosed) {
            return;
        }
        g_lidClosed = true;

        if (!g_handleLidClose.load()) {
            Wh_Log(L"Lid closed, but HandleLidClose is disabled");
            return;
        }

        Wh_Log(L"Lid closed transition detected; triggering immediately");
        TriggerFlow(0);
        return;
    }

    if (!g_lidClosed) {
        return;
    }
    g_lidClosed = false;
    Wh_Log(L"Lid opened");

    if (g_flowActive.load()) {
        Wh_Log(L"Lid re-opened during flow; aborting and restoring");
        RestoreFlowSafeTimeout();
        DestroyBlackoutWindow();
    }
}

static void HandleDisplayState(DWORD state) {
    bool wasOn = g_displayOn;
    g_displayOn = (state != 0);

    if (!wasOn && g_displayOn) {
        g_lastDisplayOnTick = GetTickCount64();
        // Screen woke up; ensure blackout overlay is removed
        DestroyBlackoutWindow();
    }

    if (state == 0) {
        if (g_flowActive.load()) {
            Wh_Log(L"Display off notification received; restoring safe timeout");
            RestoreFlowSafeTimeout();

            // Perform silent lock in the dark if configured
            if (g_lockOnScreenOff.load()) {
                Wh_Log(L"LockOnScreenOff is enabled; locking workstation silently in the dark");
                LockWorkStation();
            } else {
                Wh_Log(L"LockOnScreenOff is disabled; keeping active desktop for wake");
            }

            DestroyBlackoutWindow();
        }
    } else if (state == 2) {
        Wh_Log(L"Display dimmed");
    }
}

// ─── Window Procedure ───────────────────────────────────────────────────────

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
    if (uMsg == g_triggerMessage && g_triggerMessage != 0) {
        Wh_Log(L"External trigger message received");
        TriggerFlow(2);
        return 0;
    }

    switch (uMsg) {
        case WM_POWERBROADCAST: {
            switch (wParam) {
                case PBT_POWERSETTINGCHANGE: {
                    PPOWERBROADCAST_SETTING setting =
                        (PPOWERBROADCAST_SETTING)lParam;
                    if (!setting) {
                        break;
                    }

                    if (GuidEqual(setting->PowerSetting,
                                  GUID_LIDSWITCH_STATE_CHANGE_LOCAL)) {
                        // GUID_LIDSWITCH_STATE_CHANGE: 0 = closed, 1 = open
                        DWORD rawVal = ReadPowerSettingDword(setting);
                        HandleLidState(rawVal == 0);
                    } else if (GuidEqual(setting->PowerSetting,
                                         GUID_CONSOLE_DISPLAY_STATE_LOCAL)) {
                        HandleDisplayState(ReadPowerSettingDword(setting));
                    }
                    return 0;
                }

                case PBT_APMRESUMEAUTOMATIC:
                case PBT_APMRESUMESUSPEND: {
                    Wh_Log(L"Resume from sleep");
                    if (g_flowActive.load()) {
                        Wh_Log(L"Flow still active after resume; restoring");
                        RestoreFlowSafeTimeout();
                        DestroyBlackoutWindow();
                    }
                    return TRUE;
                }
            }
            break;
        }

        case WM_INPUT: {
            if (!g_handlePowerButton.load() || g_shuttingDown.load()) {
                break;
            }

            UINT size = 0;
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size,
                                sizeof(RAWINPUTHEADER)) != 0 ||
                size == 0 || size > 64 * 1024) {
                break;
            }

            std::vector<BYTE> buffer(size);
            UINT copied = GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
                                          buffer.data(), &size,
                                          sizeof(RAWINPUTHEADER));
            if (copied != size || size < sizeof(RAWINPUTHEADER)) {
                break;
            }

            RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());
            if (raw->header.dwType != RIM_TYPEHID) {
                break;
            }

            if (RawInputIsPowerButton(raw) && !ShouldIgnorePowerButtonAsWake()) {
                Wh_Log(L"Power button HID report received");
                TriggerFlow(1);
            }
            break;
        }

        case WM_HOTKEY: {
            if (wParam != (WPARAM)TRIGGER_HOTKEY_ID) {
                break;
            }
            if (g_shuttingDown.load()) {
                break;
            }
            if (IsWakeGuardActive()) {
                Wh_Log(L"Hotkey pressed while display is off or just turned on; ignored");
                return 0;
            }
            Wh_Log(L"Trigger hotkey pressed");
            TriggerFlow(3);
            return 0;
        }

        case WM_RELOAD_SETTINGS: {
            Wh_Log(L"Reloading settings");
            LoadSettings();
            UpdateTriggerHotkey();
            return 0;
        }

        case WM_QUIT_THREAD: {
            Wh_Log(L"Received WM_QUIT_THREAD");
            if (g_flowActive.load()) {
                RestoreFlowSafeTimeout();
            }
            DestroyBlackoutWindow();
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_DESTROY: {
            UnregisterPowerButtonRawInput();
            if (g_hotkeyRegistered) {
                UnregisterHotKey(hWnd, TRIGGER_HOTKEY_ID);
                g_hotkeyRegistered = false;
            }
            if (g_lidNotify) {
                UnregisterPowerSettingNotification(g_lidNotify);
                g_lidNotify = NULL;
            }
            if (g_displayNotify) {
                UnregisterPowerSettingNotification(g_displayNotify);
                g_displayNotify = NULL;
            }
            DestroyBlackoutWindow();
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ─── Power Action Diagnostics ───────────────────────────────────────────────

static void LogNativeActionWarnings() {
    EnterCriticalSection(&g_powerLock);

    GUID scheme = {};
    if (!GetActiveSchemeGuidNoLock(&scheme)) {
        LeaveCriticalSection(&g_powerLock);
        return;
    }

    DWORD lidAc = 0;
    DWORD lidDc = 0;
    DWORD powerAc = 0;
    DWORD powerDc = 0;

    DWORD lidAcErr = PowerReadACValueIndex(
        nullptr, &scheme, &GUID_SYSTEM_BUTTON_SUBGROUP_LOCAL,
        &GUID_LIDCLOSE_ACTION_LOCAL, &lidAc);
    DWORD lidDcErr = PowerReadDCValueIndex(
        nullptr, &scheme, &GUID_SYSTEM_BUTTON_SUBGROUP_LOCAL,
        &GUID_LIDCLOSE_ACTION_LOCAL, &lidDc);
    DWORD powerAcErr = PowerReadACValueIndex(
        nullptr, &scheme, &GUID_SYSTEM_BUTTON_SUBGROUP_LOCAL,
        &GUID_POWERBUTTON_ACTION_LOCAL, &powerAc);
    DWORD powerDcErr = PowerReadDCValueIndex(
        nullptr, &scheme, &GUID_SYSTEM_BUTTON_SUBGROUP_LOCAL,
        &GUID_POWERBUTTON_ACTION_LOCAL, &powerDc);

    if (lidAcErr == ERROR_SUCCESS && lidDcErr == ERROR_SUCCESS) {
        Wh_Log(L"Native lid close action: AC=%lu, DC=%lu", lidAc, lidDc);
        if (lidAc != 0 || lidDc != 0) {
            Wh_Log(L"WARNING: lid close action is not 'Do nothing'. Windows may sleep before this mod acts.");
        }
    }

    if (powerAcErr == ERROR_SUCCESS && powerDcErr == ERROR_SUCCESS) {
        Wh_Log(L"Native power button action: AC=%lu, DC=%lu", powerAc, powerDc);
        if (powerAc != 0 || powerDc != 0) {
            Wh_Log(L"WARNING: power button action is not 'Do nothing'. Windows may sleep before this mod acts.");
        }
    }

    LeaveCriticalSection(&g_powerLock);
}

// ─── Windhawk Tool-Mod Worker Thread & Callbacks ─────────────────────────────

static DWORD WINAPI ToolModThreadProc(LPVOID lpParam) {
    Wh_Log(L"[Thread] Starting (TID: %lu)", GetCurrentThreadId());

    // 1. Register Blackout Window Class
    WNDCLASSEX bwc = {};
    bwc.cbSize = sizeof(WNDCLASSEX);
    bwc.lpfnWndProc = BlackoutWndProc;
    bwc.hInstance = GetModuleHandle(NULL);
    bwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    bwc.hCursor = NULL;
    bwc.lpszClassName = BLACKOUT_WINDOW_CLASS;

    if (!RegisterClassEx(&bwc)) {
        Wh_Log(L"[Thread] RegisterClassEx (Blackout) failed: %lu", GetLastError());
    }

    // 2. Register Mod Control Window Class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = MOD_WINDOW_CLASS;

    ATOM atom = RegisterClassEx(&wc);
    if (!atom) {
        Wh_Log(L"[Thread] RegisterClassEx (Mod) failed: %lu", GetLastError());
        UnregisterClass(BLACKOUT_WINDOW_CLASS, GetModuleHandle(NULL));
        if (g_hInitEvent) SetEvent(g_hInitEvent);
        return 1;
    }

    g_hWnd = CreateWindowEx(
        0, MOD_WINDOW_CLASS, L"ScreenOffNoSleep", WS_OVERLAPPED,
        0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!g_hWnd) {
        Wh_Log(L"[Thread] CreateWindowEx failed: %lu", GetLastError());
        UnregisterClass(MOD_WINDOW_CLASS, GetModuleHandle(NULL));
        UnregisterClass(BLACKOUT_WINDOW_CLASS, GetModuleHandle(NULL));
        if (g_hInitEvent) SetEvent(g_hInitEvent);
        return 1;
    }

    ShowWindow(g_hWnd, SW_HIDE);
    Wh_Log(L"[Thread] Hidden window created: HWND=%p", (void*)g_hWnd);

    g_triggerMessage = RegisterWindowMessageW(TRIGGER_MESSAGE_NAME);
    if (g_triggerMessage) {
        Wh_Log(L"[Thread] External trigger message registered: 0x%04X (%ls)",
               g_triggerMessage, TRIGGER_MESSAGE_NAME);
        ChangeWindowMessageFilterEx(g_hWnd, g_triggerMessage, MSGFLT_ALLOW, nullptr);
    }

    g_lidNotify = RegisterPowerSettingNotification(
        g_hWnd, &GUID_LIDSWITCH_STATE_CHANGE_LOCAL,
        DEVICE_NOTIFY_WINDOW_HANDLE);

    g_displayNotify = RegisterPowerSettingNotification(
        g_hWnd, &GUID_CONSOLE_DISPLAY_STATE_LOCAL,
        DEVICE_NOTIFY_WINDOW_HANDLE);

    if (!g_displayNotify) {
        Wh_Log(L"[Thread] Display state notification registration failed: %lu; aborting",
               GetLastError());
        DestroyWindow(g_hWnd);
        g_hWnd = NULL;
        UnregisterClass(MOD_WINDOW_CLASS, GetModuleHandle(NULL));
        UnregisterClass(BLACKOUT_WINDOW_CLASS, GetModuleHandle(NULL));
        if (g_hInitEvent) SetEvent(g_hInitEvent);
        return 1;
    }

    RegisterPowerButtonRawInput();
    UpdateTriggerHotkey();

    Wh_Log(L"[Thread] Listening. Hotkey, Lid, PowerButton and external message are active.");

    if (g_hInitEvent) {
        SetEvent(g_hInitEvent);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"[Thread] Message loop exited; cleaning up");

    DestroyBlackoutWindow();

    if (g_lidNotify) {
        UnregisterPowerSettingNotification(g_lidNotify);
        g_lidNotify = NULL;
    }
    if (g_displayNotify) {
        UnregisterPowerSettingNotification(g_displayNotify);
        g_displayNotify = NULL;
    }

    UnregisterClass(MOD_WINDOW_CLASS, GetModuleHandle(NULL));
    UnregisterClass(BLACKOUT_WINDOW_CLASS, GetModuleHandle(NULL));
    g_hWnd = NULL;

    Wh_Log(L"[Thread] Exiting");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"=== Turn Off Display Without Sleep: Init ===");

    InitializeCriticalSection(&g_powerLock);
    g_powerLockInitialized = true;

    LoadSettings();

    // Check if an earlier run ended mid-flow and recover original power scheme timeout.
    RecoverSavedPowerState();

    LogNativeActionWarnings();

    g_lastDisplayOnTick = GetTickCount64();
    g_displayOn = true;
    g_lidClosed = false;
    g_lidStateInitialized = false;

    g_hInitEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hInitEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, ToolModThreadProc, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_hInitEvent);
        g_hInitEvent = NULL;
        return FALSE;
    }

    WaitForSingleObject(g_hInitEvent, INFINITE);
    CloseHandle(g_hInitEvent);
    g_hInitEvent = NULL;

    if (!g_hWnd) {
        Wh_Log(L"Worker thread initialization failed");
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        return FALSE;
    }

    Wh_Log(L"=== Init complete ===");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"=== Settings Changed ===");

    if (g_hWnd) {
        PostMessage(g_hWnd, WM_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"=== Turn Off Display Without Sleep: Uninit ===");

    g_shuttingDown.store(true);

    if (g_powerLockInitialized) {
        RestoreOnUninit();
    }

    if (g_hWnd) {
        PostMessage(g_hWnd, WM_QUIT_THREAD, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }

    if (g_powerLockInitialized) {
        DeleteCriticalSection(&g_powerLock);
        g_powerLockInitialized = false;
    }

    Wh_Log(L"=== Uninit complete ===");
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

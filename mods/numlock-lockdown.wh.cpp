// ==WindhawkMod==
// @id              numlock-lockdown
// @name            Num Lock Lockdown
// @description     Keeps Num Lock permanently ON, with a modifier key for temporary override
// @version         1.0.8
// @author          Tony Thompson
// @github          https://github.com/tonythethompson
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Num Lock Lockdown

Keeps Num Lock ON with almost no background work. A low-level keyboard hook
watches the Num Lock key and the override modifier. There is no continuous
polling. An optional slow safety timer (off, or every few seconds) covers the
few cases the hook cannot see.

## Behavior

| Situation | Result |
| --- | --- |
| Normal use | Num Lock stays ON. The Num Lock key does nothing (default). |
| Hold Shift + press Num Lock | Normal toggle is allowed. |
| Release Shift | Num Lock is forced back ON if it is off. |
| Sleep/wake, RDP, or another program turns it off | Forced back ON on the next event, or on the safety check. |
| Mod disabled | Normal Windows Num Lock behavior is restored. |

Hold **Left Shift** or **Right Shift** to temporarily turn Num Lock off (for
example when you need the numpad as arrow keys). As soon as you release Shift,
Num Lock is turned back on.

## Why a tool mod?

The mod is loaded into a dedicated `windhawk.exe` process (`@include
windhawk.exe`). It does not inject into Explorer or every running app. That
keeps overhead low and avoids tying Num Lock to the shell process.

## Notes

- The keyboard hook cannot see keystrokes sent to a higher-integrity
  (elevated) window because of UIPI. If Windhawk is not running as
  administrator, enable the safety check so an elevated app that turns Num
  Lock off is corrected within a few seconds.
- The hook also cannot see some Remote Desktop / KVM paths. The safety check
  and session-unlock / resume handlers cover those.
- Synthetic Num Lock presses this mod sends are tagged and ignored by the
  hook, so the force-on path cannot fight itself.
- In "allow" mode, holding Num Lock without the modifier can leave it off
  until you release the key. Auto-repeat is swallowed so the LED does not
  flicker; force-on runs on key-up.
- The Win key is never blocked. Only Num Lock is filtered. If you hold Win and
  press numpad 1/3/5/9, Windows may type those digits because Win+Numpad1 is
  not the same shortcut as Win+1 on the number row. That is OS behavior.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- overrideModifier: shift
  $name: Temporary override modifier
  $description: >-
    Hold this key to toggle Num Lock normally. When you release it, Num Lock
    is forced back ON. Left and right keys both count.
  $options:
    - shift: Shift (left or right)
    - ctrl: Ctrl (left or right)
    - alt: Alt (left or right)
    - win: Win (left or right)
    - none: None (always force ON, no override)
- numLockKeyMode: block
  $name: Num Lock key without modifier
  $description: >-
    What happens when Num Lock is pressed without the override modifier.
  $options:
    - block: Block the key (does nothing)
    - allow: Allow the press, then force Num Lock back ON on key-up. Holding the key can leave it off until release.
- safetyCheckSeconds: 10
  $name: Safety check interval (seconds)
  $description: >-
    Optional fallback that re-checks Num Lock in case it was turned off by
    sleep, RDP, or an elevated app the hook cannot see. 0 disables the timer.
    10 seconds is a light default.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <wchar.h>

#include <windows.h>
#include <wtsapi32.h>

#include <windhawk_api.h>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// Posted from the hook (and from settings/power/session handlers) so SendInput
// never runs inside the low-level hook callback. An LL hook blocks all
// keyboard input until it returns.
constexpr UINT WM_APP_FORCE_ON = WM_APP + 1;
constexpr UINT WM_APP_UPDATE_TIMER = WM_APP + 2;
constexpr UINT WM_APP_QUIT = WM_APP + 3;

constexpr UINT_PTR kSafetyTimerId = 1;
constexpr UINT_PTR kDeferredForceTimerId = 2;

// dwExtraInfo stamped on every synthetic Num Lock we send, so the hook can
// let our own keystrokes through without treating them as user input.
constexpr ULONG_PTR kInjectedNumLockExtraInfo = 0x4E4C4F4B;  // 'NLOK'

constexpr wchar_t kWorkerClass[] = L"Windhawk.ForceNumLock.Worker";

// ---------------------------------------------------------------------------
// Settings (written on the Windhawk / worker thread, read from the hook)
// ---------------------------------------------------------------------------

enum class OverrideModifier : int {
    None = 0,
    Shift = 1,
    Ctrl = 2,
    Alt = 3,
    Win = 4,
};

std::atomic<int> g_overrideModifier{static_cast<int>(OverrideModifier::Shift)};
std::atomic<bool> g_blockNumLockKey{true};
std::atomic<int> g_safetyCheckSeconds{10};

// ---------------------------------------------------------------------------
// Live input state, updated only from the hook (cheap atomics)
// ---------------------------------------------------------------------------

std::atomic<bool> g_leftModDown{false};
std::atomic<bool> g_rightModDown{false};

// True while a physical Num Lock key is down that we allowed through because
// the override modifier was held at key-down. The matching key-up must also
// be allowed, even if Shift was released in between, so the key cannot stick
// and so a pending toggle cannot race our force-on pulse.
std::atomic<bool> g_allowedNumLockDown{false};

// True after we swallowed a Num Lock key-down (block mode). Matching key-up
// and auto-repeat downs are swallowed too, so the key cannot stick.
std::atomic<bool> g_swallowedNumLockDown{false};

// ---------------------------------------------------------------------------
// Worker thread / window / hook
// ---------------------------------------------------------------------------

// Hook thread publishes these; other threads only load. Uninit may unhook
// via exchange so a wedged hook thread cannot leave the hook installed.
std::atomic<HHOOK> g_keyboardHook{nullptr};
std::atomic<HMODULE> g_hookModule{nullptr};
std::atomic<HWND> g_workerHwnd{nullptr};
HANDLE g_workerThread = nullptr;
HANDLE g_hookThread = nullptr;
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<DWORD> g_hookThreadId{0};
std::atomic<bool> g_hookInstalled{false};
std::atomic<bool> g_hookQuit{false};
HANDLE g_workerReadyEvent = nullptr;
HANDLE g_hookReadyEvent = nullptr;
HANDLE g_hookStopEvent = nullptr;
HPOWERNOTIFY g_suspendResumeNotify = nullptr;
bool g_sessionNotifyRegistered = false;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

OverrideModifier CurrentModifier() {
    return static_cast<OverrideModifier>(
        g_overrideModifier.load(std::memory_order_relaxed));
}

bool IsOverrideHeld() {
    return g_leftModDown.load(std::memory_order_relaxed) ||
           g_rightModDown.load(std::memory_order_relaxed);
}

bool IsNumLockOn() {
    return (GetKeyState(VK_NUMLOCK) & 1) != 0;
}

bool ParseModifier(PCWSTR value, OverrideModifier* out) {
    if (!value || !out) {
        return false;
    }
    if (_wcsicmp(value, L"none") == 0) {
        *out = OverrideModifier::None;
        return true;
    }
    if (_wcsicmp(value, L"ctrl") == 0) {
        *out = OverrideModifier::Ctrl;
        return true;
    }
    if (_wcsicmp(value, L"alt") == 0) {
        *out = OverrideModifier::Alt;
        return true;
    }
    if (_wcsicmp(value, L"win") == 0) {
        *out = OverrideModifier::Win;
        return true;
    }
    if (_wcsicmp(value, L"shift") == 0) {
        *out = OverrideModifier::Shift;
        return true;
    }
    return false;
}

void LoadSettings() {
    OverrideModifier modifier = OverrideModifier::Shift;
    PCWSTR modifierSetting = Wh_GetStringSetting(L"overrideModifier");
    if (modifierSetting) {
        OverrideModifier parsed = OverrideModifier::Shift;
        if (ParseModifier(modifierSetting, &parsed)) {
            modifier = parsed;
        }
        Wh_FreeStringSetting(modifierSetting);
    }
    g_overrideModifier.store(static_cast<int>(modifier),
                             std::memory_order_relaxed);

    bool blockKey = true;
    PCWSTR keyMode = Wh_GetStringSetting(L"numLockKeyMode");
    if (keyMode) {
        if (_wcsicmp(keyMode, L"allow") == 0) {
            blockKey = false;
        }
        Wh_FreeStringSetting(keyMode);
    }
    g_blockNumLockKey.store(blockKey, std::memory_order_relaxed);

    int seconds = Wh_GetIntSetting(L"safetyCheckSeconds");
    if (seconds < 0) {
        seconds = 0;
    } else if (seconds > 3600) {
        seconds = 3600;
    }
    g_safetyCheckSeconds.store(seconds, std::memory_order_relaxed);
}

bool IsOverrideVk(DWORD vk) {
    switch (CurrentModifier()) {
        case OverrideModifier::Shift:
            return vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT;
        case OverrideModifier::Ctrl:
            return vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL;
        case OverrideModifier::Alt:
            return vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU;
        case OverrideModifier::Win:
            return vk == VK_LWIN || vk == VK_RWIN;
        case OverrideModifier::None:
            return false;
    }
    return false;
}

void SetModifierDown(DWORD vk, bool down) {
    switch (vk) {
        case VK_LSHIFT:
        case VK_LCONTROL:
        case VK_LMENU:
        case VK_LWIN:
            g_leftModDown.store(down, std::memory_order_relaxed);
            break;
        case VK_RSHIFT:
        case VK_RCONTROL:
        case VK_RMENU:
        case VK_RWIN:
            g_rightModDown.store(down, std::memory_order_relaxed);
            break;
        case VK_SHIFT:
        case VK_CONTROL:
        case VK_MENU:
            // Generic VK_* (no L/R). Treat as the left key so a single
            // flag still covers "modifier is held".
            g_leftModDown.store(down, std::memory_order_relaxed);
            break;
        default:
            break;
    }
}

void SeedModifierState() {
    g_leftModDown.store(false, std::memory_order_relaxed);
    g_rightModDown.store(false, std::memory_order_relaxed);

    switch (CurrentModifier()) {
        case OverrideModifier::Shift:
            g_leftModDown.store(GetAsyncKeyState(VK_LSHIFT) < 0,
                                std::memory_order_relaxed);
            g_rightModDown.store(GetAsyncKeyState(VK_RSHIFT) < 0,
                                 std::memory_order_relaxed);
            break;
        case OverrideModifier::Ctrl:
            g_leftModDown.store(GetAsyncKeyState(VK_LCONTROL) < 0,
                                std::memory_order_relaxed);
            g_rightModDown.store(GetAsyncKeyState(VK_RCONTROL) < 0,
                                 std::memory_order_relaxed);
            break;
        case OverrideModifier::Alt:
            g_leftModDown.store(GetAsyncKeyState(VK_LMENU) < 0,
                                std::memory_order_relaxed);
            g_rightModDown.store(GetAsyncKeyState(VK_RMENU) < 0,
                                 std::memory_order_relaxed);
            break;
        case OverrideModifier::Win:
            g_leftModDown.store(GetAsyncKeyState(VK_LWIN) < 0,
                                std::memory_order_relaxed);
            g_rightModDown.store(GetAsyncKeyState(VK_RWIN) < 0,
                                 std::memory_order_relaxed);
            break;
        case OverrideModifier::None:
            break;
    }
}

// ---------------------------------------------------------------------------
// Force Num Lock ON (worker thread only)
// ---------------------------------------------------------------------------

void RequestForceOn() {
    HWND hwnd = g_workerHwnd.load(std::memory_order_acquire);
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_FORCE_ON, 0, 0);
    }
}

// SendInput runs on the worker thread. The LL hook lives on a dedicated
// hook thread that is sitting in GetMessage, so Windows can deliver the
// callback there without this thread unhooking. No observation gap.
void SendNumLockPulse() {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_NUMLOCK;
    inputs[0].ki.wScan = 0x45;
    inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    inputs[0].ki.dwExtraInfo = kInjectedNumLockExtraInfo;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_NUMLOCK;
    inputs[1].ki.wScan = 0x45;
    inputs[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
    inputs[1].ki.dwExtraInfo = kInjectedNumLockExtraInfo;

    if (SendInput(2, inputs, sizeof(INPUT)) != 2) {
        Wh_Log(L"SendInput(VK_NUMLOCK) failed: %lu", GetLastError());
    }
}

enum class ForceReason {
    Startup,
    Event,
    Timer,
    Settings,
};

// Must run on the worker thread, never inside the LL hook.
void ForceNumLockOn(ForceReason reason) {
    if (IsOverrideHeld()) {
        return;
    }
    if (IsNumLockOn()) {
        return;
    }
    if (reason == ForceReason::Timer) {
        Wh_Log(L"Safety check: Num Lock was off, forcing ON");
    }
    SendNumLockPulse();
}

void ArmDeferredForceOn(HWND hwnd) {
    SetTimer(hwnd, kDeferredForceTimerId, 0, nullptr);
}

void UninstallKeyboardHook() {
    HHOOK hook = g_keyboardHook.exchange(nullptr, std::memory_order_acq_rel);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    g_hookInstalled.store(false, std::memory_order_release);
}

void UpdateSafetyTimer() {
    HWND hwnd = g_workerHwnd.load(std::memory_order_acquire);
    if (!hwnd) {
        return;
    }

    KillTimer(hwnd, kSafetyTimerId);

    int seconds = g_safetyCheckSeconds.load(std::memory_order_relaxed);
    if (seconds <= 0) {
        return;
    }

    const UINT intervalMs = static_cast<UINT>(seconds) * 1000;

    // Prefer a coalescable timer so a 10s safety check can share a wakeup
    // with other timers instead of forcing its own interrupt. 1000 ms
    // tolerance is enough for a fallback check.
    using SetCoalescableTimer_t = UINT_PTR(WINAPI*)(HWND, UINT_PTR, UINT,
                                                    TIMERPROC, ULONG);
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pSetCoalescableTimer =
            reinterpret_cast<SetCoalescableTimer_t>(
                GetProcAddress(user32, "SetCoalescableTimer"));
        if (pSetCoalescableTimer) {
            if (pSetCoalescableTimer(hwnd, kSafetyTimerId, intervalMs, nullptr,
                                     1000)) {
                return;
            }
        }
    }

    if (!SetTimer(hwnd, kSafetyTimerId, intervalMs, nullptr)) {
        Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }
}

// ---------------------------------------------------------------------------
// Low-level keyboard hook (keep this path tiny)
// ---------------------------------------------------------------------------

bool IsOwnInjectedNumLock(const KBDLLHOOKSTRUCT* info) {
    return (info->flags & LLKHF_INJECTED) &&
           info->dwExtraInfo == kInjectedNumLockExtraInfo;
}

// hhk is ignored on modern Windows. Passing null avoids reading g_keyboardHook
// from the callback, including during unload.
LRESULT Pass(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return Pass(nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* info =
        reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    if (!info) {
        return Pass(nCode, wParam, lParam);
    }

    const DWORD vk = info->vkCode;
    const bool isDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    // Our own force-on pulse must always reach the OS.
    if (vk == VK_NUMLOCK && IsOwnInjectedNumLock(info)) {
        return Pass(nCode, wParam, lParam);
    }

    // Track the override modifier first so a Shift-down that arrives in this
    // same callback sequence is visible to a following Num Lock press.
    if (IsOverrideVk(vk)) {
        SetModifierDown(vk, isDown && !isUp);

        // Shift released: restore Num Lock unless a user Num Lock key-up is
        // still outstanding (that up would toggle after our pulse).
        if (isUp && !IsOverrideHeld() &&
            !g_allowedNumLockDown.load(std::memory_order_relaxed)) {
            RequestForceOn();
        }

        return Pass(nCode, wParam, lParam);
    }

    if (vk != VK_NUMLOCK) {
        return Pass(nCode, wParam, lParam);
    }

    if (isDown) {
        if (IsOverrideHeld()) {
            g_allowedNumLockDown.store(true, std::memory_order_relaxed);
            g_swallowedNumLockDown.store(false, std::memory_order_relaxed);
            return Pass(nCode, wParam, lParam);
        }

        g_allowedNumLockDown.store(false, std::memory_order_relaxed);

        if (g_blockNumLockKey.load(std::memory_order_relaxed)) {
            g_swallowedNumLockDown.store(true, std::memory_order_relaxed);
            return 1;  // Swallow: key does nothing, including auto-repeat.
        }

        // Allow the first down through, swallow repeats so the LED does not
        // flicker on every typematic repeat before we force ON at key-up.
        if (g_swallowedNumLockDown.exchange(true, std::memory_order_relaxed)) {
            return 1;
        }
        return Pass(nCode, wParam, lParam);
    }

    if (isUp) {
        const bool allowed =
            g_allowedNumLockDown.exchange(false, std::memory_order_relaxed);
        const bool swallowed =
            g_swallowedNumLockDown.exchange(false, std::memory_order_relaxed);

        if (allowed) {
            // Deliver the matching up first so any toggle settles, then
            // force ON if the override is no longer held. Posting before
            // CallNextHookEx can run the pulse, then lose to this key-up.
            const LRESULT result =
                Pass(nCode, wParam, lParam);
            if (!IsOverrideHeld()) {
                RequestForceOn();
            }
            return result;
        }

        if (swallowed && g_blockNumLockKey.load(std::memory_order_relaxed)) {
            return 1;
        }

        if (IsOverrideHeld()) {
            return Pass(nCode, wParam, lParam);
        }

        if (g_blockNumLockKey.load(std::memory_order_relaxed)) {
            return 1;
        }

        const LRESULT result =
            Pass(nCode, wParam, lParam);
        RequestForceOn();
        return result;
    }

    return Pass(nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker window (timers, session/power, marshaled force-on)
// ---------------------------------------------------------------------------

void UnregisterPowerAndSession(HWND hwnd) {
    if (g_sessionNotifyRegistered) {
        WTSUnRegisterSessionNotification(hwnd);
        g_sessionNotifyRegistered = false;
    }

    if (g_suspendResumeNotify) {
        using UnregisterSuspendResumeNotification_t =
            BOOL(WINAPI*)(HPOWERNOTIFY);
        if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
            auto pUnregister = reinterpret_cast<
                UnregisterSuspendResumeNotification_t>(
                GetProcAddress(user32, "UnregisterSuspendResumeNotification"));
            if (pUnregister) {
                pUnregister(g_suspendResumeNotify);
            }
        }
        g_suspendResumeNotify = nullptr;
    }
}

LRESULT CALLBACK WorkerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_APP_FORCE_ON:
            ForceNumLockOn(ForceReason::Event);
            ArmDeferredForceOn(hwnd);
            return 0;

        case WM_APP_UPDATE_TIMER:
            SeedModifierState();
            UpdateSafetyTimer();
            ForceNumLockOn(ForceReason::Settings);
            return 0;

        case WM_APP_QUIT:
            PostQuitMessage(0);
            return 0;

        case WM_TIMER:
            if (wParam == kSafetyTimerId) {
                ForceNumLockOn(ForceReason::Timer);
            } else if (wParam == kDeferredForceTimerId) {
                KillTimer(hwnd, kDeferredForceTimerId);
                ForceNumLockOn(ForceReason::Event);
            }
            return 0;

        case WM_WTSSESSION_CHANGE:
            switch (wParam) {
                case WTS_SESSION_UNLOCK:
                case WTS_CONSOLE_CONNECT:
                case WTS_REMOTE_CONNECT:
                    ForceNumLockOn(ForceReason::Event);
                    break;
                default:
                    break;
            }
            return 0;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMRESUMEAUTOMATIC:
                case PBT_APMRESUMESUSPEND:
#ifdef PBT_APMRESUMECRITICAL
                case PBT_APMRESUMECRITICAL:
#endif
                    ForceNumLockOn(ForceReason::Event);
                    break;
                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&LowLevelKeyboardProc),
                       &module);
    return module;
}

bool RegisterPowerAndSession(HWND hwnd) {
    if (WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION)) {
        g_sessionNotifyRegistered = true;
    } else {
        Wh_Log(L"WTSRegisterSessionNotification failed: %lu", GetLastError());
    }

    using RegisterSuspendResumeNotification_t =
        HPOWERNOTIFY(WINAPI*)(HANDLE, DWORD);
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pRegister =
            reinterpret_cast<RegisterSuspendResumeNotification_t>(
                GetProcAddress(user32, "RegisterSuspendResumeNotification"));
        if (pRegister) {
            g_suspendResumeNotify =
                pRegister(hwnd, DEVICE_NOTIFY_WINDOW_HANDLE);
            if (!g_suspendResumeNotify) {
                Wh_Log(L"RegisterSuspendResumeNotification failed: %lu",
                       GetLastError());
            }
        }
    }

    return true;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    g_workerThreadId.store(GetCurrentThreadId(), std::memory_order_release);

    MSG msg;
    // Create the thread queue before we signal ready so Uninit's
    // PostThreadMessage(WM_QUIT) cannot be dropped during early unload.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HMODULE module = GetCurrentModuleHandle();

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = WorkerWndProc;
    wc.hInstance = module;
    wc.lpszClassName = kWorkerClass;
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        g_workerThreadId.store(0, std::memory_order_release);
        return 1;
    }

    HWND hwnd =
        CreateWindowExW(0, kWorkerClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                        nullptr, module, nullptr);
    g_workerHwnd.store(hwnd, std::memory_order_release);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        g_workerThreadId.store(0, std::memory_order_release);
        return 1;
    }

    RegisterPowerAndSession(hwnd);

    SeedModifierState();
    ForceNumLockOn(ForceReason::Startup);
    UpdateSafetyTimer();

    if (g_workerReadyEvent) {
        SetEvent(g_workerReadyEvent);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    hwnd = g_workerHwnd.exchange(nullptr, std::memory_order_acq_rel);
    if (hwnd) {
        KillTimer(hwnd, kSafetyTimerId);
        KillTimer(hwnd, kDeferredForceTimerId);
        UnregisterPowerAndSession(hwnd);
        DestroyWindow(hwnd);
    }

    UnregisterClassW(kWorkerClass, module);
    g_workerThreadId.store(0, std::memory_order_release);
    return 0;
}

// Dedicated hook thread: owns WH_KEYBOARD_LL and nothing else. SendInput
// stays on the worker so the hook never has to come down for a pulse.
DWORD WINAPI HookThreadProc(LPVOID) {
    g_hookThreadId.store(GetCurrentThreadId(), std::memory_order_release);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hookModule.store(GetCurrentModuleHandle(), std::memory_order_release);

    DWORD delayMs = 50;
    bool signaledReady = false;
    bool loggedPersistentFailure = false;
    unsigned attempts = 0;

    while (!g_hookQuit.load(std::memory_order_relaxed) &&
           !g_keyboardHook.load(std::memory_order_acquire)) {
        ++attempts;
        HHOOK hook = SetWindowsHookExW(
            WH_KEYBOARD_LL, LowLevelKeyboardProc,
            g_hookModule.load(std::memory_order_acquire), 0);
        if (g_hookQuit.load(std::memory_order_acquire)) {
            if (hook) {
                UnhookWindowsHookEx(hook);
            }
            break;
        }
        g_keyboardHook.store(hook, std::memory_order_release);
        if (hook) {
            g_hookInstalled.store(true, std::memory_order_release);
            if (attempts > 1) {
                Wh_Log(L"Keyboard hook installed on retry %u", attempts);
            }
            break;
        }

        Wh_Log(L"SetWindowsHookExW(WH_KEYBOARD_LL) failed: %lu (retry in %u ms)",
               GetLastError(), delayMs);
        if (delayMs >= 2000 && !loggedPersistentFailure) {
            Wh_Log(L"Keyboard hook still not installed after %u attempts; "
                   L"retrying every 2s. Safety timer covers Num Lock until then.",
                   attempts);
            loggedPersistentFailure = true;
        }

        if (!signaledReady && g_hookReadyEvent) {
            SetEvent(g_hookReadyEvent);
            signaledReady = true;
        }

        if (g_hookStopEvent) {
            WaitForSingleObject(g_hookStopEvent, delayMs);
        }
        if (delayMs < 2000) {
            delayMs *= 2;
        }
    }

    if (!signaledReady && g_hookReadyEvent) {
        SetEvent(g_hookReadyEvent);
    }

    if (!g_keyboardHook.load(std::memory_order_acquire)) {
        g_hookInstalled.store(false, std::memory_order_release);
        g_hookThreadId.store(0, std::memory_order_release);
        return 1;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UninstallKeyboardHook();
    g_hookModule.store(nullptr, std::memory_order_release);
    g_hookThreadId.store(0, std::memory_order_release);
    return 0;
}

// ---------------------------------------------------------------------------
// Tool-mod lifecycle
// ---------------------------------------------------------------------------

void WhTool_ModUninit();

BOOL WhTool_ModInit() {
    LoadSettings();
    g_hookQuit.store(false, std::memory_order_relaxed);

    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent || !g_hookReadyEvent || !g_hookStopEvent) {
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }

    if (WaitForSingleObject(g_workerReadyEvent, 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"Worker thread did not become ready in time");
    }

    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread (hook) failed: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }

    if (WaitForSingleObject(g_hookReadyEvent, 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"Hook thread did not become ready in time");
    }

    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
    if (g_hookReadyEvent) {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
    }

    if (!g_workerHwnd.load(std::memory_order_acquire)) {
        Wh_Log(L"Worker window was not created");
        WhTool_ModUninit();
        return FALSE;
    }

    if (!g_hookInstalled.load(std::memory_order_acquire)) {
        Wh_Log(L"Keyboard hook not installed yet; retrying in the background");
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();

    HWND hwnd = g_workerHwnd.load(std::memory_order_acquire);
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_UPDATE_TIMER, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_hookQuit.store(true, std::memory_order_release);
    if (g_hookStopEvent) {
        SetEvent(g_hookStopEvent);
    }

    // Unhook from this thread so a stuck hook callback cannot keep the
    // filter installed after disable. The hook thread's own unhook is then
    // a no-op (exchange already cleared the handle).
    UninstallKeyboardHook();

    DWORD hookThreadId = g_hookThreadId.load(std::memory_order_acquire);
    if (hookThreadId) {
        PostThreadMessageW(hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        if (WaitForSingleObject(g_hookThread, 2000) != WAIT_OBJECT_0) {
            Wh_Log(L"Hook thread did not exit in time; process exit will "
                   L"tear it down");
        }
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    HWND hwnd = g_workerHwnd.load(std::memory_order_acquire);
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_QUIT, 0, 0);
    } else {
        DWORD threadId = g_workerThreadId.load(std::memory_order_acquire);
        if (threadId) {
            PostThreadMessageW(threadId, WM_QUIT, 0, 0);
        }
    }

    if (g_workerThread) {
        if (WaitForSingleObject(g_workerThread, 2000) != WAIT_OBJECT_0) {
            Wh_Log(L"Worker thread did not exit in time; process exit will "
                   L"tear it down");
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
    if (g_hookReadyEvent) {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
    }
    if (g_hookStopEvent) {
        CloseHandle(g_hookStopEvent);
        g_hookStopEvent = nullptr;
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
    swprintf(commandLine, ARRAYSIZE(commandLine), L"\"%s\" -tool-mod \"%s\"",
             currentProcessPath, WH_MOD_ID);

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

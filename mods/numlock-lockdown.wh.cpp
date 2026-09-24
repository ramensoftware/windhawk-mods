// ==WindhawkMod==
// @id              numlock-lockdown
// @name            Num Lock Lockdown
// @description     Keeps Num Lock permanently ON, with a modifier key for temporary override
// @version         1.1.9
// @author          tonythethompson
// @github          https://github.com/tonythethompson
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Num Lock Lockdown

Keeps Num Lock ON with almost no background work. A low-level keyboard hook
watches the Num Lock key and the override modifier. There is no fast polling.
An optional slow safety check (off, or every few seconds; 10 s by default)
covers the few cases the hook cannot see.

## Behavior

| Situation | Result |
| --- | --- |
| Normal use | Num Lock stays ON. The Num Lock key does nothing (default). |
| Hold Shift + press Num Lock | Normal toggle is allowed. |
| Release Shift | Num Lock is forced back ON if it is off. |
| Sleep/wake, RDP, or another program turns it off | Forced back ON on the next event, on unlock/resume/focus change, or on the safety check. |
| Mod disabled | The Num Lock *key* behaves normally again. The LED is left ON (the last forced state), not restored to whatever it was before the mod ran. |

Hold **Left Shift** or **Right Shift** and press Num Lock to toggle it
normally for as long as you keep Shift down. While Shift is held, numpad
arrows are Shift+Arrow in most apps (extend selection), not plain arrows.
Ctrl, Alt, and Win have the same kind of extra meaning. Release the
override and Num Lock is turned back on.

**Block** (the default) swallows Num Lock for every process, so a game or app
that uses Num Lock as a hotkey will not see the key. Use **Allow** if you need
those binds; the mod still forces Num Lock back ON on key-up.

## Why a tool mod?

The mod is loaded into a dedicated `windhawk.exe` process (`@include
windhawk.exe`). It does not inject into Explorer or every running app. That
keeps overhead low and avoids tying Num Lock to the shell process.

## Notes

- The keyboard hook cannot see keystrokes sent to a higher-integrity
  (elevated) window because of UIPI. `SendInput` is also subject to UIPI:
  when an elevated window is focused, the injected Num Lock pulse is
  dropped and neither the return value nor `GetLastError` reports it. Run
  Windhawk as administrator if you need the force-on to work while an
  elevated app is in front. When focus moves back to a normal window, a
  foreground-change hook retries the pulse. The safety check is a slower
  fallback for cases with no focus change.
- The hook also cannot see some Remote Desktop / KVM paths. Session-unlock
  / resume handlers and the safety check cover those.
- If the hook misses a modifier key-up (elevated window or secure desktop),
  the next Num Lock event re-reads the real modifier state from the OS, and
  the worker does the same before skipping a force-on.
- **Block** also swallows the ToggleKeys accessibility shortcut (hold Num
  Lock for 5 seconds) and the MouseKeys shortcut (Left Alt + Left Shift +
  Num Lock) whenever your override modifier is not Shift or Alt. Use
  **Allow** if you need those shortcuts.
- Synthetic Num Lock presses this mod sends are tagged and ignored by
  *this* hook, so the force-on path cannot fight itself. Other apps still
  see those injected presses (the pulse has to reach the OS to move the
  LED). A game bound to Num Lock can therefore see a synthetic press when
  a force-on fires, even in **Block** mode.
- In "allow" mode, holding Num Lock without the modifier can leave it off
  until you release the key. Auto-repeat is swallowed so the LED does not
  flicker; force-on runs on key-up. If the safety check is on (10 s by
  default), it can still force Num Lock back ON while the key is held.
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
    Hold this key and press Num Lock to toggle it. While the key is down,
    numpad arrows keep that modifier (Shift+Arrow extends selection in most
    apps). When you release it, Num Lock is turned back on. Left and right
    keys both count.
  $options:
    - shift: Shift (left or right)
    - ctrl: Ctrl (left or right)
    - alt: Alt (left or right)
    - win: Win (left or right)
    - none: None (always force ON, no override)
- numLockKeyMode: block
  $name: Pressing Num Lock alone
  $description: >-
    What should happen when you press Num Lock without holding Shift (or
    whichever override you picked). In allow mode, holding the key can leave
    Num Lock off until you release it; the safety check may still turn it
    back on before then.
  $options:
    - block: Ignore it. Games and other apps will not see the key either.
    - allow: Let it toggle, then turn Num Lock back on when you let go.
- safetyCheckSeconds: 10
  $name: Safety check interval (seconds)
  $description: >-
    Optional fallback that re-checks Num Lock in case it was turned off by
    sleep, RDP, or an elevated app the hook cannot see. Focus change, unlock,
    and resume already retry without this timer. 0 disables it. 10 seconds
    is a light default. Values above 3600 seconds are treated as 3600.
*/
// ==/WindhawkModSettings==

#include <atomic>

#include <windows.h>
#include <wtsapi32.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// Posted from the hook (and from settings/power/session handlers) so SendInput
// never runs inside the low-level hook callback. An LL hook blocks all
// keyboard input until it returns.
constexpr UINT WM_APP_FORCE_ON = WM_APP + 1;
constexpr UINT WM_APP_UPDATE_TIMER = WM_APP + 2;
constexpr UINT WM_APP_QUIT = WM_APP + 3;
// Posted to the hook thread (hwnd is null). Install a fresh hook first, then
// drop the old one, so a timeout-dropped or failed hook can be recovered
// without a gap if SetWindowsHookEx fails.
constexpr UINT WM_APP_REHOOK = WM_APP + 4;

constexpr UINT_PTR kSafetyTimerId = 1;
// Retries a missing keyboard hook when the safety check is disabled.
constexpr UINT_PTR kHookRetryTimerId = 2;
constexpr UINT kHookRetryIntervalMs = 60 * 1000;
// SendInput is asynchronous; wait for the injected toggle to land before
// sending another pulse or Num Lock can flip back OFF.
constexpr ULONGLONG kPulseDebounceMs = 300;

// dwExtraInfo stamped on every synthetic Num Lock we send, so the hook can
// let our own keystrokes through without treating them as user input.
constexpr ULONG_PTR kInjectedNumLockExtraInfo = 0x4E4C4F4B;  // 'NLOK'

constexpr wchar_t kWorkerClass[] = L"Windhawk.NumLockLockdown.Worker";

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
// Live input state (atomics). The hook updates these on key events;
// the worker also re-seeds from GetAsyncKeyState when verifying a hold.
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

// Set when a Num Lock press was allowed because the override was held.
// Modifier key-up only asks for a force-on after that, so ordinary Shift
// taps during typing do not wake the worker.
std::atomic<bool> g_overrideUsedThisHold{false};

// ---------------------------------------------------------------------------
// Worker thread / window / hook
// ---------------------------------------------------------------------------

// Hook thread publishes these; other threads only load. Uninit may unhook
// via exchange so a wedged hook thread cannot leave the hook installed.
std::atomic<HHOOK> g_keyboardHook{nullptr};
std::atomic<HWND> g_workerHwnd{nullptr};
HANDLE g_workerThread = nullptr;
HANDLE g_hookThread = nullptr;
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<DWORD> g_hookThreadId{0};
std::atomic<bool> g_hookQuit{false};
HANDLE g_workerReadyEvent = nullptr;
HANDLE g_hookReadyEvent = nullptr;
HPOWERNOTIFY g_suspendResumeNotify = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
bool g_sessionNotifyRegistered = false;
// Worker thread only. Last time we sent a Num Lock pulse.
ULONGLONG g_lastPulseMs = 0;

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
    if (!out) {
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
    WindhawkUtils::StringSetting modifierSetting =
        WindhawkUtils::StringSetting::make(L"overrideModifier");
    OverrideModifier parsed = OverrideModifier::Shift;
    if (ParseModifier(modifierSetting.get(), &parsed)) {
        modifier = parsed;
    }
    g_overrideModifier.store(static_cast<int>(modifier),
                             std::memory_order_relaxed);

    bool blockKey = true;
    WindhawkUtils::StringSetting keyMode =
        WindhawkUtils::StringSetting::make(L"numLockKeyMode");
    if (_wcsicmp(keyMode.get(), L"allow") == 0) {
        blockKey = false;
    }
    g_blockNumLockKey.store(blockKey, std::memory_order_relaxed);

    int seconds = Wh_GetIntSetting(L"safetyCheckSeconds");
    if (seconds < 0) {
        seconds = 0;
    } else if (seconds > 3600) {
        seconds = 3600;
    }
    g_safetyCheckSeconds.store(seconds, std::memory_order_relaxed);
    g_overrideUsedThisHold.store(false, std::memory_order_relaxed);
}

bool IsOverrideVk(DWORD vk) {
    switch (CurrentModifier()) {
        case OverrideModifier::Shift:
            return vk == VK_LSHIFT || vk == VK_RSHIFT;
        case OverrideModifier::Ctrl:
            return vk == VK_LCONTROL || vk == VK_RCONTROL;
        case OverrideModifier::Alt:
            return vk == VK_LMENU || vk == VK_RMENU;
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
        default:
            break;
    }
}

void SeedModifierState() {
    bool left = false;
    bool right = false;

    switch (CurrentModifier()) {
        case OverrideModifier::Shift:
            left = GetAsyncKeyState(VK_LSHIFT) < 0;
            right = GetAsyncKeyState(VK_RSHIFT) < 0;
            break;
        case OverrideModifier::Ctrl:
            left = GetAsyncKeyState(VK_LCONTROL) < 0;
            right = GetAsyncKeyState(VK_RCONTROL) < 0;
            break;
        case OverrideModifier::Alt:
            left = GetAsyncKeyState(VK_LMENU) < 0;
            right = GetAsyncKeyState(VK_RMENU) < 0;
            break;
        case OverrideModifier::Win:
            left = GetAsyncKeyState(VK_LWIN) < 0;
            right = GetAsyncKeyState(VK_RWIN) < 0;
            break;
        case OverrideModifier::None:
            break;
    }

    g_leftModDown.store(left, std::memory_order_relaxed);
    g_rightModDown.store(right, std::memory_order_relaxed);
}

void ClearStaleKeyFlags() {
    g_allowedNumLockDown.store(false, std::memory_order_relaxed);
    g_swallowedNumLockDown.store(false, std::memory_order_relaxed);
    g_overrideUsedThisHold.store(false, std::memory_order_relaxed);
}

// Worker thread only: a cached "held" can be stale if the hook missed a
// key-up (elevated window / secure desktop), so confirm against the OS.
bool IsOverrideHeldVerified() {
    if (IsOverrideHeld()) {
        SeedModifierState();
    }
    if (IsOverrideHeld()) {
        return true;
    }
    // Modifier is not held. If Num Lock is also up, a missed key-up can
    // leave the swallow / allow flags stuck; clear them.
    if (GetAsyncKeyState(VK_NUMLOCK) >= 0) {
        ClearStaleKeyFlags();
    }
    return false;
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

void UninstallKeyboardHook() {
    HHOOK hook = g_keyboardHook.exchange(nullptr, std::memory_order_acq_rel);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }
}

// Worker thread only. Unlock/resume only.
void RequestRehook() {
    DWORD threadId = g_hookThreadId.load(std::memory_order_acquire);
    if (!threadId) {
        return;
    }
    if (!PostThreadMessageW(threadId, WM_APP_REHOOK, 0, 0)) {
        Wh_Log(L"PostThreadMessageW(WM_APP_REHOOK) failed: %lu", GetLastError());
    }
}

void RequestRehookIfMissing() {
    if (!g_keyboardHook.load(std::memory_order_acquire)) {
        RequestRehook();
    }
}

// Must run on the worker thread, never inside the LL hook.
void ForceNumLockOn(bool fromSafetyCheck) {
    if (IsOverrideHeldVerified() || IsNumLockOn()) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (g_lastPulseMs != 0 && now - g_lastPulseMs < kPulseDebounceMs) {
        return;
    }

    if (fromSafetyCheck) {
        Wh_Log(L"Safety check: Num Lock was off, forcing ON");
    }

    g_lastPulseMs = now;
    SendNumLockPulse();
}

void UpdateHookRetryTimer(HWND hwnd) {
    KillTimer(hwnd, kHookRetryTimerId);

    int seconds = g_safetyCheckSeconds.load(std::memory_order_relaxed);
    if (seconds > 0) {
        return;
    }

    if (!SetTimer(hwnd, kHookRetryTimerId, kHookRetryIntervalMs, nullptr)) {
        Wh_Log(L"SetTimer (hook retry) failed: %lu", GetLastError());
    }
}

void UpdateSafetyTimer() {
    HWND hwnd = g_workerHwnd.load(std::memory_order_acquire);
    if (!hwnd) {
        return;
    }

    KillTimer(hwnd, kSafetyTimerId);
    UpdateHookRetryTimer(hwnd);

    int seconds = g_safetyCheckSeconds.load(std::memory_order_relaxed);
    if (seconds <= 0) {
        return;
    }

    const UINT intervalMs = static_cast<UINT>(seconds) * 1000;
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
        SetModifierDown(vk, isDown);

        // Only wake the worker after an override was actually used. Ordinary
        // Shift taps during typing stay on this thread.
        if (isUp && !IsOverrideHeld()) {
            const bool used =
                g_overrideUsedThisHold.exchange(false, std::memory_order_relaxed);
            if (used && !g_allowedNumLockDown.load(std::memory_order_relaxed)) {
                RequestForceOn();
            }
        }

        return Pass(nCode, wParam, lParam);
    }

    if (vk != VK_NUMLOCK) {
        return Pass(nCode, wParam, lParam);
    }

    // Num Lock is rare. Re-read the modifier from the OS so a missed
    // key-up (UIPI / secure desktop) cannot leave the cache stuck "held".
    SeedModifierState();

    if (isDown) {
        if (IsOverrideHeld()) {
            g_allowedNumLockDown.store(true, std::memory_order_relaxed);
            g_swallowedNumLockDown.store(false, std::memory_order_relaxed);
            g_overrideUsedThisHold.store(true, std::memory_order_relaxed);
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
            // ask the worker to force ON. The worker re-verifies the hold
            // and no-ops if the override is still actually down.
            const LRESULT result = Pass(nCode, wParam, lParam);
            RequestForceOn();
            return result;
        }

        if (swallowed && g_blockNumLockKey.load(std::memory_order_relaxed)) {
            return 1;
        }

        if (IsOverrideHeld()) {
            // Allow mode: Num Lock went down alone, then Shift was held
            // before key-up. Honour the hold, and mark it used so Shift-up
            // still forces ON.
            g_overrideUsedThisHold.store(true, std::memory_order_relaxed);
            return Pass(nCode, wParam, lParam);
        }

        if (g_blockNumLockKey.load(std::memory_order_relaxed)) {
            return 1;
        }

        const LRESULT result = Pass(nCode, wParam, lParam);
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
        UnregisterSuspendResumeNotification(g_suspendResumeNotify);
        g_suspendResumeNotify = nullptr;
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
}

LRESULT CALLBACK WorkerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_APP_FORCE_ON:
            ForceNumLockOn(false);
            return 0;

        case WM_APP_UPDATE_TIMER:
            SeedModifierState();
            UpdateSafetyTimer();
            ForceNumLockOn(false);
            return 0;

        case WM_APP_QUIT:
            PostQuitMessage(0);
            return 0;

        case WM_TIMER:
            if (wParam == kSafetyTimerId) {
                RequestRehookIfMissing();
                ForceNumLockOn(true);
            } else if (wParam == kHookRetryTimerId) {
                RequestRehookIfMissing();
            }
            return 0;

        case WM_WTSSESSION_CHANGE:
            switch (wParam) {
                case WTS_SESSION_UNLOCK:
                case WTS_CONSOLE_CONNECT:
                case WTS_REMOTE_CONNECT:
                    RequestRehook();
                    ForceNumLockOn(false);
                    break;
                default:
                    break;
            }
            return 0;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMRESUMEAUTOMATIC:
                case PBT_APMRESUMESUSPEND:
                    RequestRehook();
                    ForceNumLockOn(false);
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

void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD,
                                  DWORD) {
    // Same thread that called SetWinEventHook (the worker).
    ForceNumLockOn(false);
}

void RegisterForegroundHook() {
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook) {
        Wh_Log(L"SetWinEventHook(EVENT_SYSTEM_FOREGROUND) failed: %lu",
               GetLastError());
    }
}

void RegisterPowerAndSession(HWND hwnd) {
    if (WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION)) {
        g_sessionNotifyRegistered = true;
    } else {
        Wh_Log(L"WTSRegisterSessionNotification failed: %lu", GetLastError());
    }

    g_suspendResumeNotify =
        RegisterSuspendResumeNotification(hwnd, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!g_suspendResumeNotify) {
        Wh_Log(L"RegisterSuspendResumeNotification failed: %lu", GetLastError());
    }
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
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        g_workerThreadId.store(0, std::memory_order_release);
        return 1;
    }

    // Hidden top-level window (not HWND_MESSAGE). WTSRegisterSessionNotification
    // does not deliver WM_WTSSESSION_CHANGE to message-only windows.
    // WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE keeps it out of Alt+Tab; never shown.
    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWorkerClass, L"", WS_OVERLAPPED, 0,
        0, 0, 0, nullptr, nullptr, module, nullptr);
    g_workerHwnd.store(hwnd, std::memory_order_release);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(kWorkerClass, module);
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        g_workerThreadId.store(0, std::memory_order_release);
        return 1;
    }

    RegisterPowerAndSession(hwnd);
    RegisterForegroundHook();

    SeedModifierState();
    ForceNumLockOn(false);
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
        KillTimer(hwnd, kHookRetryTimerId);
        UnregisterPowerAndSession(hwnd);
        DestroyWindow(hwnd);
    }

    UnregisterClassW(kWorkerClass, module);
    g_workerThreadId.store(0, std::memory_order_release);
    return 0;
}

void TryInstallKeyboardHook() {
    if (g_hookQuit.load(std::memory_order_acquire)) {
        return;
    }
    if (g_keyboardHook.load(std::memory_order_acquire)) {
        return;
    }

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                   GetCurrentModuleHandle(), 0);
    if (g_hookQuit.load(std::memory_order_acquire)) {
        if (hook) {
            UnhookWindowsHookEx(hook);
        }
        return;
    }

    g_keyboardHook.store(hook, std::memory_order_release);
    if (!hook) {
        Wh_Log(L"SetWindowsHookExW(WH_KEYBOARD_LL) failed: %lu",
               GetLastError());
    }
}

// Install a new hook first, then drop the previous handle. If SetWindowsHookEx
// fails, a still-working hook is left in place. If Windows already dropped the
// old hook, the stored handle is stale and this puts a live one back.
void ReinstallKeyboardHook() {
    if (g_hookQuit.load(std::memory_order_acquire)) {
        return;
    }

    HHOOK fresh = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                    GetCurrentModuleHandle(), 0);
    if (g_hookQuit.load(std::memory_order_acquire)) {
        if (fresh) {
            UnhookWindowsHookEx(fresh);
        }
        return;
    }
    if (!fresh) {
        Wh_Log(L"SetWindowsHookExW(WH_KEYBOARD_LL) failed: %lu",
               GetLastError());
        return;
    }

    HHOOK old = g_keyboardHook.exchange(fresh, std::memory_order_acq_rel);
    if (old) {
        UnhookWindowsHookEx(old);
    }
}

// Dedicated hook thread: owns WH_KEYBOARD_LL and nothing else. SendInput
// stays on the worker so the hook never has to come down for a pulse.
DWORD WINAPI HookThreadProc(LPVOID) {
    g_hookThreadId.store(GetCurrentThreadId(), std::memory_order_release);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    TryInstallKeyboardHook();

    if (g_hookReadyEvent) {
        SetEvent(g_hookReadyEvent);
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_REHOOK) {
            ReinstallKeyboardHook();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UninstallKeyboardHook();
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
    if (!g_workerReadyEvent || !g_hookReadyEvent) {
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
        WhTool_ModUninit();
        return FALSE;
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
        WhTool_ModUninit();
        return FALSE;
    }

    if (!g_workerHwnd.load(std::memory_order_acquire)) {
        Wh_Log(L"Worker window was not created");
        WhTool_ModUninit();
        return FALSE;
    }

    if (!g_keyboardHook.load(std::memory_order_acquire)) {
        Wh_Log(L"Keyboard hook failed to install; will retry on unlock/resume");
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

void CloseEventHandle(HANDLE* event) {
    if (*event) {
        CloseHandle(*event);
        *event = nullptr;
    }
}

void WhTool_ModUninit() {
    g_hookQuit.store(true, std::memory_order_release);

    // Unhook from this thread so a stuck hook callback cannot keep the
    // filter installed after disable. The hook thread's own unhook is then
    // a no-op (exchange already cleared the handle).
    UninstallKeyboardHook();

    DWORD hookThreadId = g_hookThreadId.load(std::memory_order_acquire);
    if (hookThreadId) {
        PostThreadMessageW(hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        const bool joined =
            WaitForSingleObject(g_hookThread, 2000) == WAIT_OBJECT_0;
        if (!joined) {
            Wh_Log(L"Hook thread did not exit in time; process exit will "
                   L"tear it down");
        }
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        if (joined) {
            CloseEventHandle(&g_hookReadyEvent);
        }
    } else {
        CloseEventHandle(&g_hookReadyEvent);
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
        const bool joined =
            WaitForSingleObject(g_workerThread, 2000) == WAIT_OBJECT_0;
        if (!joined) {
            Wh_Log(L"Worker thread did not exit in time; process exit will "
                   L"tear it down");
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        if (joined) {
            CloseEventHandle(&g_workerReadyEvent);
        }
    } else {
        CloseEventHandle(&g_workerReadyEvent);
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

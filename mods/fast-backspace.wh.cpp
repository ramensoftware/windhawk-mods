// ==WindhawkMod==
// @id              fast-backspace
// @name            Fast Backspace
// @description     Delete faster while holding Backspace, with a configurable repeat multiplier
// @version         1.0
// @author          Enrique Bos
// @github          https://github.com/enriquebos
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Fast Backspace

Increase the speed of held Backspace without changing the repeat speed of other
keys. The first press and the Windows keyboard repeat delay stay unchanged.

## Installation and settings

Install **Fast Backspace** in Windhawk, then open its **Settings** tab. Set
**Backspace speed multiplier** to an integer from **1** to **10**. The default is
**2**; **1** keeps normal Windows speed. Values outside this range are clamped.
Settings take effect without recompiling or restarting Explorer.

While reviewing an unpublished copy, use **Create a new mod** in Windhawk, replace
the template with this complete source file, then compile and enable it.

Each Windows-generated repeat can produce up to the selected number of Backspace
key-downs in total. Actual deletion speed depends on the application. Use a low
multiplier first: a high value can delete more text than intended.

## Behavior

- Tapping Backspace still sends one ordinary press and release.
- Acceleration starts only when Windows repeats a held physical Backspace key.
- Ctrl, Alt, Shift, and Windows-key combinations are not accelerated. Pressing
  another key during a hold cancels acceleration until Backspace is released.
- A repeat observed in a different foreground window cancels acceleration for
  that hold.
- Disabling the mod stops pending work, removes the hook, and joins its thread.
  Input already delivered to an application cannot be undone.

## How it works

A dedicated thread in one Explorer process per Windows session installs a global
`WH_KEYBOARD_LL` hook. It observes physical Backspace repeats and posts work to its
own message loop. `SendInput` runs after the keyboard callback has returned.
Only one repeat batch can be pending; delayed batches are dropped rather than
accumulated. No timer, continuous polling, or injection into every app is needed.

Extra events are tagged key-downs. The physical key-up remains responsible for
releasing Backspace; synthetic key-up/key-down pairs would release a key that is
still physically held. The hook rejects its own stale input after a release, a
new hold, a shortcut, a foreground-window change, or shutdown. Other injected
Backspace events do not start acceleration.

## Compatibility and limitations

Designed for Windows 10 and 11. Programs that accept ordinary synthetic keyboard
input may work, including editors, browsers, and terminals, but each app needs
testing. Games, raw-input applications, keyboard remappers, accessibility tools,
and Remote Desktop can handle repeats or injected input differently.

- Windows integrity restrictions can prevent Explorer from sending extra input
  to elevated applications. Ordinary physical input is always passed through;
  the mod does not bypass this restriction or elevate Explorer.
- The hook operates on Explorer's input desktop, not the secure desktop. Missed
  events across desktop/session switches can affect hold detection; release
  Backspace before continuing. Events marked as injected are not accelerated.
- Backspace is accelerated wherever it is handled, including password fields
  and non-text actions such as navigation. The global hook cannot reliably
  identify text fields. Disable the mod where this behavior is undesirable.
- Foreground-window checks cannot detect every change of focus inside one window
  or an application-specific mode change. Release Backspace before changing focus.
- Other hooks can block injected input. Windows can also silently remove a
  low-level hook if its thread times out; disable and re-enable the mod to retry.
- Acceleration is unavailable while its Explorer host is stopped. Windhawk can
  load the mod again when Explorer restarts. Each Windows session is independent.

## Troubleshooting and verification

If Backspace is not faster, check that the multiplier exceeds 1, release and hold
the key again without modifiers, and try an ordinary, non-elevated text editor.
Disable and re-enable the mod if necessary. Windhawk logs initialization and
input-insertion failures when logging is enabled. Failed batches are not retried.

Before relying on the mod, test disposable text in your editors, browser inputs,
terminal, and VS Code. Verify single taps, rapid taps, long holds at 1/2/10,
settings changes while held, release during a repeat, left/right modifier
shortcuts, foreground/focus changes, disable/re-enable, and an Explorer restart.
Check elevated apps, remote sessions, and games separately if you use them.

## Changelog

### 1.0

- Initial submission, based on the author's locally tested Fast Backspace mod.
- Configurable 1-10 multiplier with a default of 2 and live settings.
- Preserve the physical release; generate bounded, cancellable extra key-downs.
- Check hook startup and clean up the worker and session guard on shutdown.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RepeatMultiplier: 2
  $name: Backspace speed multiplier
  $description: >-
    Integer from 1 (normal speed) to 10. Default: 2. Only physical Backspace
    repeats are accelerated; the first press and repeat delay stay unchanged.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>

#include <algorithm>
#include <atomic>

namespace {

constexpr UINT kRepeatMessage = WM_APP + 1;
constexpr int kMaxMultiplier = 10;
constexpr ULONG_PTR kInputTag = 0xFA570000;
constexpr ULONG_PTR kGenerationMask = 0xFFFF;
constexpr ULONG_PTR kInputTagMask = ~kGenerationMask;

std::atomic<int> g_multiplier{2};
std::atomic<bool> g_stopping{true};
std::atomic<bool> g_hookInstalled{false};

HANDLE g_mutex = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_ready = nullptr;
HANDLE g_stop = nullptr;

// Hook-thread state. Generations invalidate queued work and injected input.
bool g_backspaceDown = false;
bool g_accelerateHold = false;
bool g_repeatPending = false;
HWND g_holdWindow = nullptr;
ULONG_PTR g_generation = 0;

void LoadSettings() {
    int value =
        std::clamp(Wh_GetIntSetting(L"RepeatMultiplier"), 1, kMaxMultiplier);
    g_multiplier.store(value);
    Wh_Log(L"Backspace multiplier: %d", value);
}

bool ModifiersPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_MENU) & 0x8000) ||
           (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

void CancelAcceleration() {
    g_accelerateHold = false;
    g_repeatPending = false;
    ++g_generation;
}

bool CanAccelerate() {
    return !g_stopping.load() && g_backspaceDown && g_accelerateHold &&
           g_holdWindow && GetForegroundWindow() == g_holdWindow &&
           (GetAsyncKeyState(VK_BACK) & 0x8000) && !ModifiersPressed();
}

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    bool injected = (key->flags & LLKHF_INJECTED) != 0;
    if (injected && key->vkCode == VK_BACK &&
        (key->dwExtraInfo & kInputTagMask) == kInputTag) {
        // Recheck at delivery: injected input can outlive its physical hold.
        if ((key->dwExtraInfo & kGenerationMask) !=
                (g_generation & kGenerationMask) ||
            !CanAccelerate()) {
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    if (g_stopping.load()) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool keyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (key->vkCode != VK_BACK) {
        // Also observe injected modifiers/shortcuts used by remapping tools.
        if (keyDown && g_backspaceDown) {
            CancelAcceleration();
        }
    } else if (!injected) {
        if (keyUp) {
            g_backspaceDown = false;
            g_holdWindow = nullptr;
            CancelAcceleration();
        } else if (keyDown) {
            // The callback precedes the async-state update. An up state here
            // also recovers from a release missed on another input desktop.
            if (!g_backspaceDown || !(GetAsyncKeyState(VK_BACK) & 0x8000)) {
                g_backspaceDown = true;
                g_holdWindow = GetForegroundWindow();
                ++g_generation;
                g_repeatPending = false;
                g_accelerateHold = g_holdWindow && !ModifiersPressed() &&
                                   !(key->flags & LLKHF_ALTDOWN);
            } else if (!CanAccelerate() || (key->flags & LLKHF_ALTDOWN)) {
                CancelAcceleration();
            } else if (g_multiplier.load() > 1 && !g_repeatPending) {
                g_repeatPending = true;
                if (!PostThreadMessageW(GetCurrentThreadId(), kRepeatMessage,
                                        g_generation, 0)) {
                    g_repeatPending = false;
                }
            }
        }
    }

    // Never suppress physical input, even if queuing or injection fails.
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void ProcessRepeat(ULONG_PTR generation) {
    if (generation != g_generation || !g_repeatPending) {
        return;
    }
    g_repeatPending = false;

    if (!CanAccelerate()) {
        CancelAcceleration();
        return;
    }

    int extra = g_multiplier.load() - 1;
    if (extra <= 0) {
        return;
    }

    INPUT inputs[kMaxMultiplier - 1]{};
    for (int i = 0; i < extra; ++i) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = VK_BACK;
        inputs[i].ki.dwExtraInfo = kInputTag | (generation & kGenerationMask);
    }

    // Key-downs extend the physical hold. Its physical key-up releases the key.
    UINT count = static_cast<UINT>(extra);
    UINT sent = SendInput(count, inputs, sizeof(INPUT));
    if (sent != count) {
        DWORD error = GetLastError();
        CancelAcceleration();
        Wh_Log(
            L"SendInput inserted %u of %u events (error %lu); "
            L"acceleration cancelled until release",
            sent, count, error);
    }
}

DWORD WINAPI HookThread(void*) {
    g_backspaceDown = false;
    g_holdWindow = nullptr;
    CancelAcceleration();

    MSG message;
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&KeyboardHook),
                            &module)) {
        Wh_Log(L"GetModuleHandleEx failed: %lu", GetLastError());
        SetEvent(g_ready);
        return 1;
    }

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, module, 0);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
        SetEvent(g_ready);
        return 1;
    }

    g_hookInstalled.store(true);
    SetEvent(g_ready);
    Wh_Log(L"Global keyboard hook installed");

    bool quit = false;
    while (!quit && !g_stopping.load()) {
        DWORD result =
            MsgWaitForMultipleObjects(1, &g_stop, FALSE, INFINITE, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Hook message wait failed: %lu", GetLastError());
            break;
        }

        while (!g_stopping.load() &&
               PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                quit = true;
                break;
            }
            if (message.message == kRepeatMessage) {
                ProcessRepeat(message.wParam);
            } else {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    g_stopping.store(true);
    CancelAcceleration();
    UnhookWindowsHookEx(hook);
    g_hookInstalled.store(false);
    return 0;
}

void RequestStop() {
    g_stopping.store(true);
    if (g_stop) {
        SetEvent(g_stop);
    }
}

void Cleanup() {
    RequestStop();
    if (g_thread) {
        // The DLL must remain loaded until the callback and thread have exited.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    for (HANDLE* handle : {&g_ready, &g_stop, &g_mutex}) {
        if (*handle) {
            CloseHandle(*handle);
            *handle = nullptr;
        }
    }
}

}  // namespace

BOOL Wh_ModInit() {
    g_stopping.store(true);
    g_hookInstalled.store(false);
    LoadSettings();

    g_mutex = CreateMutexW(nullptr, FALSE, L"Local\\WindhawkFastBackspaceHook");
    if (!g_mutex) {
        Wh_Log(L"CreateMutex failed: %lu", GetLastError());
        return FALSE;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Another Explorer instance already hosts the hook");
        Cleanup();
        return FALSE;
    }

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        Wh_Log(L"Failed to create ready event: %lu", GetLastError());
        Cleanup();
        return FALSE;
    }

    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop) {
        Wh_Log(L"Failed to create stop event: %lu", GetLastError());
        Cleanup();
        return FALSE;
    }

    g_stopping.store(false);
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        Cleanup();
        return FALSE;
    }

    HANDLE handles[] = {g_ready, g_thread};
    DWORD result =
        WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, 5000);
    if (result != WAIT_OBJECT_0 || !g_hookInstalled.load()) {
        Wh_Log(L"Keyboard hook startup failed (wait result %lu)", result);
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    RequestStop();
}

void Wh_ModUninit() {
    Cleanup();
}

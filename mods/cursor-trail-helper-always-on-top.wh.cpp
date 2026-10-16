// ==WindhawkMod==
// @id              cursor-trail-helper-always-on-top
// @name            Simple Cursor Trail helper - Always on top
// @description     Places the Simple Cursor Trail overlay above the taskbar and Start menu by moving it to a higher Z-order band
// @version         1.2
// @author          Ulrizza
// @github          https://github.com/Ulrizza
// @license         MIT
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Cursor Trail helper - always on top

Companion mod for **Simple Cursor Trail**. It makes the trail visible above the
taskbar and Start menu.

It exists only to solve a Windows limitation: starting with Windows 8, windows
live in fixed Z-order *bands* (`ZBID`), and a window in a lower band can never be
drawn above a window in a higher band, no matter how `SetWindowPos`/`WS_EX_TOPMOST`
is used.

The `Simple Cursor Trail` overlay is a normal desktop-band window, while the
Windows 11 taskbar sits in `ZBID_IMMERSIVE_MOGO` (6) and the Start menu and
notification center in other immersive bands above the desktop band. This mod
moves the overlay into `ZBID_SYSTEM_TOOLS` (16), the same band Task Manager uses
for "Always on top", which is above all of them.

## How it works

This mod is injected into `explorer.exe` (which has the privileges required for
band manipulation) and:

1. Finds the overlay window by its class name (`CursorTrailOverlayClass`).
2. Calls the undocumented `SetWindowBand(hwnd, NULL, 16)`.
3. If that is denied, it hooks `NtUserEnableIAMAccess` to capture the IAM access
   key the shell uses, then retries with that key. Capturing the key requires
   pressing the **Win key** (or otherwise opening a shell surface) once after
   the mod loads.

The band is re-applied automatically if the overlay is recreated or the band is
reset.

## Notes

- Uses undocumented Windows APIs (`SetWindowBand`, `NtUserEnableIAMAccess`).
- Places the trail above the taskbar, Start menu, Task Manager and Alt-Tab.
  The overlay is click-through, so it never steals input.
- Requires the **Simple Cursor Trail** mod
  ([catalog](https://windhawk.net/mods/cursor-trail)) to be installed and
  enabled.

## Licence

This project is licensed under the MIT License. You're free to use, modify, and
redistribute the code as long as you keep the original copyright notice and
credit the author (Ulrizza). See
[LICENSE](https://github.com/Ulrizza/WindHawk-CursorTrail/blob/main/LICENSE) for
the full text.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>

#include <atomic>

// --- Z-order bands (ZBID) -------------------------------------------------
enum ZBID {
    ZBID_DESKTOP = 1,
    ZBID_SYSTEM_TOOLS = 16,
};

// --- Undocumented user32 APIs --------------------------------------------
typedef BOOL(WINAPI* SetWindowBand_t)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL(WINAPI* GetWindowBand_t)(HWND hWnd, PDWORD pdwBand);
typedef BOOL(WINAPI* NtUserEnableIAMAccess_t)(ULONG64 key, BOOL enable);

static SetWindowBand_t pSetWindowBand;
static GetWindowBand_t pGetWindowBand;
static NtUserEnableIAMAccess_t pNtUserEnableIAMAccess;
static NtUserEnableIAMAccess_t pNtUserEnableIAMAccessOriginal;

// --- State ----------------------------------------------------------------
// Must match the overlay window class created by the main Simple Cursor Trail mod.
static const wchar_t* kOverlayClass = L"CursorTrailOverlayClass";
static const DWORD kTargetBand = ZBID_SYSTEM_TOOLS;

static std::atomic<ULONG64> g_iamKey{0};
static HANDLE g_stopEvent;
static HANDLE g_workerThread;
static HWND g_lastBanded;
static std::atomic<bool> g_hookInstalled{false};
static std::atomic<bool> g_unhookPending{false};

// Poll quickly while looking for the overlay or applying the band, then back
// off to a slow interval once the band is confirmed, to keep wakeups low.
static const DWORD kActivePollMs = 1000;
static const DWORD kStablePollMs = 5000;

// Captures the IAM access key when the shell calls NtUserEnableIAMAccess.
// The hook is removed by the worker thread (not from here) once captured.
static BOOL WINAPI NtUserEnableIAMAccessHook(ULONG64 key, BOOL enable) {
    BOOL result = pNtUserEnableIAMAccessOriginal(key, enable);
    if (result && g_iamKey.load() == 0 && key != 0) {
        g_iamKey.store(key);
        g_unhookPending.store(true);
        Wh_Log(L"IAM access key captured");
    }
    return result;
}

// Installs the IAM access key hook if it isn't already installed. Called once
// at load, and again if a captured key turns out to be stale.
static void ArmIamHook(bool applyOperations) {
    if (g_hookInstalled.load() || !pNtUserEnableIAMAccess) {
        return;
    }

    if (WindhawkUtils::SetFunctionHook(pNtUserEnableIAMAccess,
                                       NtUserEnableIAMAccessHook,
                                       &pNtUserEnableIAMAccessOriginal)) {
        g_hookInstalled.store(true);
        if (applyOperations) {
            // Hooks added at load are applied implicitly after Wh_ModInit;
            // a re-arm at runtime has to apply them itself.
            Wh_ApplyHookOperations();
        }
        Wh_Log(L"NtUserEnableIAMAccess hooked");
    } else {
        Wh_Log(L"Failed to hook NtUserEnableIAMAccess");
    }
}

static bool ApplyBand(HWND hwnd) {
    ULONG64 iamKey = g_iamKey.load();
    if (iamKey) {
        pNtUserEnableIAMAccess(iamKey, TRUE);
        BOOL ok = pSetWindowBand(hwnd, nullptr, kTargetBand);
        DWORD err = ok ? ERROR_SUCCESS : GetLastError();
        pNtUserEnableIAMAccess(iamKey, FALSE);
        if (!ok) {
            Wh_Log(L"SetWindowBand (key) failed: %u", err);
            // The key may have gone stale; drop it and re-arm the hook so a
            // fresh key can be captured if the shell enables IAM access again.
            g_iamKey.store(0);
            ArmIamHook(true);
        }
        return ok != FALSE;
    }

    // The shell may already have IAM access enabled; try without a key first.
    SetLastError(ERROR_SUCCESS);
    BOOL ok = pSetWindowBand(hwnd, nullptr, kTargetBand);
    if (!ok) {
        DWORD err = GetLastError();
        if (err != ERROR_ACCESS_DENIED) {
            Wh_Log(L"SetWindowBand (direct) failed: %u", err);
        }
        return false;
    }
    return true;
}

static DWORD WINAPI BandWorkerThread(LPVOID) {
    DWORD waitMs = kActivePollMs;
    for (;;) {
        if (WaitForSingleObject(g_stopEvent, waitMs) != WAIT_TIMEOUT) {
            break;
        }

        if (g_unhookPending.load() && g_hookInstalled.load() && pNtUserEnableIAMAccess) {
            Wh_RemoveFunctionHook((void*)pNtUserEnableIAMAccess);
            Wh_ApplyHookOperations();
            g_hookInstalled.store(false);
            g_unhookPending.store(false);
            Wh_Log(L"NtUserEnableIAMAccess unhooked");
        }

        HWND hwnd = FindWindowW(kOverlayClass, nullptr);
        if (!hwnd) {
            g_lastBanded = nullptr;
            waitMs = kActivePollMs;
            continue;
        }

        DWORD band = 0;
        if (pGetWindowBand && pGetWindowBand(hwnd, &band)) {
            if (band == kTargetBand) {
                g_lastBanded = hwnd;
                waitMs = kStablePollMs;
                continue;
            }
        } else if (hwnd == g_lastBanded) {
            waitMs = kStablePollMs;
            continue;
        }

        if (ApplyBand(hwnd)) {
            g_lastBanded = hwnd;
            waitMs = kStablePollMs;
            Wh_Log(L"Overlay moved to band %u", kTargetBand);
        } else {
            waitMs = kActivePollMs;
        }
    }
    return 0;
}

static bool ResolveApis() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"user32.dll not loaded");
        return false;
    }

    pSetWindowBand = (SetWindowBand_t)GetProcAddress(user32, "SetWindowBand");
    pGetWindowBand = (GetWindowBand_t)GetProcAddress(user32, "GetWindowBand");
    pNtUserEnableIAMAccess =
        (NtUserEnableIAMAccess_t)GetProcAddress(user32, MAKEINTRESOURCEA(2510));

    if (!pSetWindowBand) {
        Wh_Log(L"SetWindowBand not found");
        return false;
    }
    return true;
}

BOOL Wh_ModInit() {
    if (!ResolveApis()) {
        return FALSE;
    }

    ArmIamHook(false);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, BandWorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    // Best effort: return the overlay to the desktop band while it still exists.
    HWND hwnd = FindWindowW(kOverlayClass, nullptr);
    if (hwnd && pSetWindowBand) {
        ULONG64 iamKey = g_iamKey.load();
        if (iamKey) {
            pNtUserEnableIAMAccess(iamKey, TRUE);
            pSetWindowBand(hwnd, nullptr, ZBID_DESKTOP);
            pNtUserEnableIAMAccess(iamKey, FALSE);
        } else {
            pSetWindowBand(hwnd, nullptr, ZBID_DESKTOP);
        }
    }
}

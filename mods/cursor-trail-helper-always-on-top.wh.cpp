// ==WindhawkMod==
// @id              cursor-trail-helper-always-on-top
// @name            Simple Cursor Trail helper - Always on top
// @description     Places the Simple Cursor Trail overlay above the taskbar and Start menu by moving it to a higher Z-order band
// @version         1.3
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

Without this mod, the trail is drawn behind the taskbar and Start menu:

![Simple Cursor Trail drawn behind the taskbar and Start menu](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/AlwaysOnTopBefore.gif)

With this mod, it is drawn above them:

![Simple Cursor Trail drawn above the taskbar and Start menu](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/AlwaysOnTopAfter.gif)

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

1. Finds the overlay window: a click-through top-level window with the class
   name `CursorTrailOverlayClass`, owned by `windhawk.exe` (`windhawk-mod.exe`
   on Windhawk 2.0).
2. Calls the undocumented `SetWindowBand(hwnd, NULL, 16)`.
3. Also hooks `NtUserEnableIAMAccess` to record the IAM access key the shell
   uses, and uses that key when the direct call is denied. The key is recorded
   once the shell opens one of its surfaces (e.g. press the **Win key**) after
   the mod loads.

The band is re-applied automatically if the overlay is recreated or the band is
reset.

## Notes

- Uses undocumented Windows APIs (`SetWindowBand`, `NtUserEnableIAMAccess`).
- The `NtUserEnableIAMAccess` ordinal is only used after it is verified to
  match `win32u.dll`'s named export; otherwise the IAM hook is disabled.
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

// Poll quickly while looking for the overlay or applying the band, then back
// off to a slow interval once the band is confirmed, to keep wakeups low.
static const DWORD kActivePollMs = 1000;
static const DWORD kStablePollMs = 5000;

// Records the last IAM access key the shell used. The hook stays installed for
// the whole lifetime of the mod: the shell only calls this when it opens one of
// its surfaces, so the cost is negligible, and a fresh key overwrites a stale
// one by itself. Removing/re-adding hooks at runtime would need
// Wh_ApplyHookOperations, which must not run after Wh_ModBeforeUninit returns.
static BOOL WINAPI NtUserEnableIAMAccessHook(ULONG64 key, BOOL enable) {
    BOOL result = pNtUserEnableIAMAccessOriginal(key, enable);
    if (result && key) {
        g_iamKey.store(key);
    }
    return result;
}

static bool ApplyBand(HWND hwnd) {
    ULONG64 iamKey = g_iamKey.load();
    if (iamKey && pNtUserEnableIAMAccess) {
        pNtUserEnableIAMAccess(iamKey, TRUE);
        BOOL ok = pSetWindowBand(hwnd, nullptr, kTargetBand);
        DWORD err = ok ? ERROR_SUCCESS : GetLastError();
        pNtUserEnableIAMAccess(iamKey, FALSE);
        if (!ok) {
            Wh_Log(L"SetWindowBand (key) failed: %u", err);
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

// True when this explorer.exe process hosts the shell taskbar. Folder windows
// running in a separate explorer.exe don't, have no IAM key of their own, and
// would just poll for nothing. If the taskbar doesn't exist yet (the mod loaded
// during shell startup), assume this is the shell process and proceed.
static bool IsShellProcess() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) {
        return true;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(tray, &pid);
    return pid == GetCurrentProcessId();
}

// Only raise the actual trail overlay: a click-through window owned by the
// Windhawk process. This keeps an unrelated top-level window that happens to
// use the same class name, or a sandboxed window, from being lifted above the
// shell UI.
static bool IsTrailOverlay(HWND hwnd) {
    if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);
    if (!ok) {
        return false;
    }

    const wchar_t* name = path;
    for (const wchar_t* p = path; *p; ++p) {
        if (*p == L'\\') {
            name = p + 1;
        }
    }

    return lstrcmpiW(name, L"windhawk.exe") == 0 ||
           lstrcmpiW(name, L"windhawk-mod.exe") == 0;
}

// Finds the actual trail overlay, skipping any other top-level window that
// happens to use the same class name.
static HWND FindTrailOverlay() {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(nullptr, hwnd, kOverlayClass, nullptr)) != nullptr) {
        if (IsTrailOverlay(hwnd)) {
            return hwnd;
        }
    }
    return nullptr;
}

static DWORD WINAPI BandWorkerThread(LPVOID) {
    if (!IsShellProcess()) {
        Wh_Log(L"Not the shell explorer process, nothing to do");
        return 0;
    }

    DWORD waitMs = kActivePollMs;
    for (;;) {
        if (WaitForSingleObject(g_stopEvent, waitMs) != WAIT_TIMEOUT) {
            break;
        }

        HWND hwnd = FindTrailOverlay();
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

// Confirms that a tiny user32 export forwards to target (win32u's named
// export) through a call/jmp [rip+disp32] import slot. Prevents hooking an
// unrelated function if ordinal 2510 ever maps elsewhere on another build.
static bool ThunkCallsTarget(const void* thunk, const void* target) {
    const BYTE* p = (const BYTE*)thunk;
    for (int i = 0; i + 6 <= 32; ++i) {
        if (p[i] != 0xFF || (p[i + 1] != 0x15 && p[i + 1] != 0x25)) {
            continue;
        }
#ifdef _WIN64
        INT32 disp = *(const INT32*)(p + i + 2);
        const void* slot = p + i + 6 + disp;
#else
        const void* slot = *(const void* const*)(p + i + 2);
#endif
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(slot, &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) {
            continue;
        }
        return *(void* const*)slot == target;
    }
    return false;
}

static bool ResolveApis() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"user32.dll not loaded");
        return false;
    }

    pSetWindowBand = (SetWindowBand_t)GetProcAddress(user32, "SetWindowBand");
    pGetWindowBand = (GetWindowBand_t)GetProcAddress(user32, "GetWindowBand");

    // user32 exports NtUserEnableIAMAccess by ordinal only; on modern builds it
    // is a thunk into win32u's named export. Only hook it when that matches, so
    // a changed ordinal can never make us hook an unrelated function.
    HMODULE win32u = GetModuleHandleW(L"win32u.dll");
    void* byOrdinal = (void*)GetProcAddress(user32, MAKEINTRESOURCEA(2510));
    void* byName = win32u
        ? (void*)GetProcAddress(win32u, "NtUserEnableIAMAccess") : nullptr;
    if (!win32u) {
        pNtUserEnableIAMAccess = nullptr;
        Wh_Log(L"win32u.dll not loaded, IAM hook disabled");
    } else if (!byOrdinal) {
        pNtUserEnableIAMAccess = nullptr;
        Wh_Log(L"NtUserEnableIAMAccess ordinal 2510 not found, IAM hook disabled");
    } else if (!byName) {
        pNtUserEnableIAMAccess = nullptr;
        Wh_Log(L"win32u!NtUserEnableIAMAccess not found, IAM hook disabled");
    } else if (byOrdinal != byName && !ThunkCallsTarget(byOrdinal, byName)) {
        pNtUserEnableIAMAccess = nullptr;
        Wh_Log(L"Ordinal 2510 is not win32u!NtUserEnableIAMAccess, "
               L"IAM hook disabled");
    } else {
        pNtUserEnableIAMAccess = (NtUserEnableIAMAccess_t)byOrdinal;
        Wh_Log(L"NtUserEnableIAMAccess verified (ordinal 2510)");
    }

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

    if (pNtUserEnableIAMAccess) {
        if (WindhawkUtils::SetFunctionHook(pNtUserEnableIAMAccess,
                                           NtUserEnableIAMAccessHook,
                                           &pNtUserEnableIAMAccessOriginal)) {
            Wh_Log(L"NtUserEnableIAMAccess hook installed");
        } else {
            Wh_Log(L"Failed to hook NtUserEnableIAMAccess");
        }
    }

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

    if (!IsShellProcess()) {
        return;
    }

    // Best effort: return the overlay to the desktop band while it still exists.
    HWND hwnd = FindTrailOverlay();
    if (hwnd && pSetWindowBand) {
        ULONG64 iamKey = g_iamKey.load();
        if (iamKey && pNtUserEnableIAMAccess) {
            pNtUserEnableIAMAccess(iamKey, TRUE);
            pSetWindowBand(hwnd, nullptr, ZBID_DESKTOP);
            pNtUserEnableIAMAccess(iamKey, FALSE);
        } else {
            pSetWindowBand(hwnd, nullptr, ZBID_DESKTOP);
        }
    }
}

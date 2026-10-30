// ==WindhawkMod==
// @id              explorer-no-white-flash
// @name            Explorer No White Flash
// @description     Hides new File Explorer windows until their content has painted, instead of showing an empty white frame
// @version         1.0
// @author          Chr1s
// @github          https://github.com/chr1sx
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
**Tested on Windows 10 22H2 (build 19045), 64-bit Explorer with animations
disabled.**

When you open a folder in Explorer, the window is composited before its contents
have been drawn, so you see an empty white frame for a fraction of a second.

With window animations enabled, DWM fades the window in and that frame is
invisible. With animations disabled (or on slow machines) the white frame is
clearly visible and very distracting.

This mod hides a newly created Explorer window from the compositor until its
content view has actually painted, then reveals it. The window is never visible
while empty, so there is no white frame.

This covers **new windows** (Win+E, double-clicking a folder, the taskbar). It
does not affect new tabs inside an already open window - for those, see the
`fix-explorer-white-flash` mod.

The safety timeout guarantees the window is always revealed even if no paint is
detected, so a window can never get stuck invisible.

Windows 11 can create a folder window while it is still hidden and show it
later. If that happens and the window is not shown before the safety timeout,
the mod reveals it while it is hidden, so it can flash once when it is shown -
the same as without the mod. Keeping it hidden instead would tie up the mod
indefinitely, which is the worse trade.

## Settings

- **Reveal delay (ms)** - how long to wait after the content has painted before
  showing the window. Default `120`. Lower feels faster, but too low may show a
  partly drawn window. `0` means "use the default".
- **Safety timeout (ms)** - reveal the window anyway after this long, even if no
  paint was detected. Default `2500`. This is the guard that stops a window from
  ever staying invisible; lower it if you prefer a faster worst case, but keep it
  above the time your machine normally needs to draw a folder. `0` means "use the
  default".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- revealDelay: 120
  $name: Reveal delay (ms)
  $description: How long to wait after the content has painted before showing the window. Lower feels faster, too low may show a partly drawn window. 0 means "use the default".
- safetyTimeout: 2500
  $name: Safety timeout (ms)
  $description: Reveal the window anyway after this long, even if no paint was detected. Prevents a window from ever staying invisible. 0 means "use the default".
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#include <atomic>

#define MAX_TRACKED 64

static std::atomic<int> g_revealDelay{120};
static std::atomic<int> g_safetyTimeout{2500};

static void LoadSettings() {
    // A setting which has no value in the registry reads as 0 (the engine uses
    // value_or(0)), so 0 means "use the default".
    int delay = (int)Wh_GetIntSetting(L"revealDelay");
    if (delay <= 0)
        delay = 120;
    if (delay > 2000)
        delay = 2000;

    int timeout = (int)Wh_GetIntSetting(L"safetyTimeout");
    if (timeout <= 0)
        timeout = 2500;
    if (timeout > 10000)
        timeout = 10000;

    g_revealDelay.store(delay, std::memory_order_relaxed);
    g_safetyTimeout.store(timeout, std::memory_order_relaxed);
}

static bool ClassIs(PCWSTR a, PCWSTR b) {
    return a && b && !_wcsicmp(a, b);
}

static bool WindowClassIs(HWND hWnd, PCWSTR want) {
    wchar_t cls[128];
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)))
        return false;
    return ClassIs(cls, want);
}

struct Entry {
    HWND hWnd;
    DWORD cloakTick;
    DWORD revealAt;
    bool pending;
};

static Entry g_entries[MAX_TRACKED];
static CRITICAL_SECTION g_lock;
static std::atomic<int> g_pendingCount{0};

static HANDLE g_revealThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_wakeEvent = nullptr;

static Entry* FindEntry(HWND hWnd) {
    for (int i = 0; i < MAX_TRACKED; i++)
        if (g_entries[i].pending && g_entries[i].hWnd == hWnd)
            return &g_entries[i];
    return nullptr;
}

static void Uncloak(HWND hWnd) {
    // A window that was destroyed in the meantime must not be touched: its
    // handle may already belong to another window.
    if (!IsWindow(hWnd))
        return;
    BOOL off = FALSE;
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &off, sizeof(off));
}

static DWORD WINAPI RevealThread(LPVOID) {
    HANDLE handles[2] = {g_stopEvent, g_wakeEvent};

    for (;;) {
        // Only poll while a window is waiting; otherwise sleep until one is
        // tracked, so an idle Explorer is not woken 25 times a second.
        DWORD timeout = g_pendingCount.load(std::memory_order_relaxed) > 0 ? 40 : INFINITE;
        if (WaitForMultipleObjects(2, handles, FALSE, timeout) == WAIT_OBJECT_0)
            return 0;

        HWND due[MAX_TRACKED];
        int dueCount = 0;
        DWORD now = GetTickCount();

        EnterCriticalSection(&g_lock);
        for (int i = 0; i < MAX_TRACKED; i++) {
            Entry& e = g_entries[i];
            if (!e.pending)
                continue;

            bool drop = false;
            bool reveal = false;
            if (e.revealAt != 0) {
                drop = reveal = (LONG)(now - e.revealAt) >= 0;
            } else if ((DWORD)(now - e.cloakTick) > (DWORD)g_safetyTimeout.load()) {
                // Nothing painted in time. Drop the entry and reveal the
                // window, even if it is still hidden (a window Explorer
                // pre-created ahead of time): keeping it pending would keep
                // this thread polling and hold the slot for the rest of the
                // session, which is worse than the flash it prevents. If the
                // window was destroyed, there is nothing left to reveal.
                drop = true;
                reveal = IsWindow(e.hWnd) != FALSE;
            }

            if (drop) {
                if (reveal)
                    due[dueCount++] = e.hWnd;
                e.pending = false;
                e.hWnd = nullptr;
                g_pendingCount.fetch_sub(1, std::memory_order_relaxed);
            }
        }
        LeaveCriticalSection(&g_lock);

        // DWM is called outside the lock: the BitBlt hook on Explorer's UI
        // threads takes the same lock while painting.
        for (int i = 0; i < dueCount; i++)
            Uncloak(due[i]);
    }
}

static void TrackWindow(HWND hWnd) {
    // Cloak before publishing the entry, so the reveal thread can never
    // uncloak the window and then be followed by this cloak.
    BOOL on = TRUE;
    if (FAILED(DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &on, sizeof(on))))
        return;

    EnterCriticalSection(&g_lock);
    bool tracked = false;
    for (int i = 0; i < MAX_TRACKED; i++) {
        if (!g_entries[i].pending) {
            g_entries[i].hWnd = hWnd;
            g_entries[i].cloakTick = GetTickCount();
            g_entries[i].revealAt = 0;
            g_entries[i].pending = true;
            g_pendingCount.fetch_add(1, std::memory_order_relaxed);
            tracked = true;
            break;
        }
    }
    LeaveCriticalSection(&g_lock);

    if (!tracked) {
        // No free slot, so nothing would ever reveal this window.
        Uncloak(hWnd);
        return;
    }

    SetEvent(g_wakeEvent);
}

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int,
                                        int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                 int nWidth, int nHeight, HWND hWndParent,
                                 HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                         hMenu, hInstance, lpParam);

    if (hWnd && (WindowClassIs(hWnd, L"CabinetWClass") ||
                 WindowClassIs(hWnd, L"ExploreWClass")))
        TrackWindow(hWnd);

    return hWnd;
}

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_Original;

// The content view is the DirectUIHWND inside the shell view, i.e. one that has
// SHELLDLL_DefView or DUIViewWndClassName among its ancestors. Other
// DirectUIHWNDs in the window (the search box, for example) are excluded by
// that, and by the minimum size.
static bool IsContentViewBlit(HWND hWnd, int cx, int cy) {
    if (cx < 100 || cy < 100)
        return false;

    HWND parent = GetParent(hWnd);
    for (int i = 0; i < 8 && parent; i++) {
        if (WindowClassIs(parent, L"SHELLDLL_DefView") ||
            WindowClassIs(parent, L"DUIViewWndClassName"))
            return true;
        parent = GetParent(parent);
    }
    return false;
}

BOOL WINAPI BitBlt_Hook(HDC hdcDest, int x, int y, int cx, int cy, HDC hdcSrc,
                        int x1, int y1, DWORD rop) {
    if (rop == SRCCOPY && cx > 0 && cy > 0 &&
        g_pendingCount.load(std::memory_order_relaxed) > 0) {
        HWND hWnd = WindowFromDC(hdcDest);
        if (hWnd && WindowClassIs(hWnd, L"DirectUIHWND") &&
            IsContentViewBlit(hWnd, cx, cy)) {
            HWND root = GetAncestor(hWnd, GA_ROOT);
            if (root) {
                DWORD revealAt = GetTickCount() + (DWORD)g_revealDelay.load();
                EnterCriticalSection(&g_lock);
                Entry* e = FindEntry(root);
                if (e && e->revealAt == 0)
                    e->revealAt = revealAt;
                LeaveCriticalSection(&g_lock);
            }
        }
    }
    return BitBlt_Original(hdcDest, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

BOOL Wh_ModInit() {
    LoadSettings();

    InitializeCriticalSection(&g_lock);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stopEvent && g_wakeEvent)
        g_revealThread = CreateThread(nullptr, 0, RevealThread, nullptr, 0, nullptr);

    if (!g_revealThread) {
        // Never cloak windows without a thread that can reveal them.
        if (g_stopEvent)
            CloseHandle(g_stopEvent);
        if (g_wakeEvent)
            CloseHandle(g_wakeEvent);
        g_stopEvent = nullptr;
        g_wakeEvent = nullptr;
        DeleteCriticalSection(&g_lock);
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(BitBlt, BitBlt_Hook, &BitBlt_Original);

    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_revealThread) {
        WaitForSingleObject(g_revealThread, INFINITE);
        CloseHandle(g_revealThread);
        g_revealThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }

    // Reveal whatever is still cloaked, so no window is left invisible when
    // the mod is disabled, updated or removed.
    for (int i = 0; i < MAX_TRACKED; i++) {
        if (g_entries[i].pending) {
            Uncloak(g_entries[i].hWnd);
            g_entries[i].pending = false;
            g_entries[i].hWnd = nullptr;
        }
    }
    g_pendingCount.store(0, std::memory_order_relaxed);

    DeleteCriticalSection(&g_lock);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

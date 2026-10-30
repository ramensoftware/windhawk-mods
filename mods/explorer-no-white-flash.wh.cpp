// ==WindhawkMod==
// @id              explorer-no-white-flash
// @name            Explorer No White Flash
// @description     Stops Explorer windows and folders flashing white before their content paints
// @version         1.0
// @author          Chr1s
// @github          https://github.com/chr1sx
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

## Settings

- **Reveal delay (ms)** - how long to wait after the content has painted before
  showing the window. Default `120`. Lower feels faster, but too low may show a
  partly drawn window.
- **Safety timeout (ms)** - reveal the window anyway after this long, even if no
  paint was detected. Default `2500`. This is the guard that stops a window from
  ever staying invisible; lower it if you prefer a faster worst case, but keep it
  above the time your machine normally needs to draw a folder.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- revealDelay: 120
  $name: Reveal delay (ms)
  $description: How long to wait after the content has painted before showing the window. Lower feels faster, too low may show a partly drawn window.
- safetyTimeout: 2500
  $name: Safety timeout (ms)
  $description: Reveal the window anyway after this long, even if no paint was detected. Prevents a window from ever staying invisible.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#define MAX_TRACKED 64

static volatile LONG g_revealDelay = 120;
static volatile LONG g_safetyTimeout = 2500;

static void LoadSettings() {
    // Unset settings read as 0, so 0 means "use the default".
    LONG delay = Wh_GetIntSetting(L"revealDelay");
    if (delay <= 0)
        delay = 120;
    if (delay > 2000)
        delay = 2000;

    LONG timeout = Wh_GetIntSetting(L"safetyTimeout");
    if (timeout <= 0)
        timeout = 2500;
    if (timeout > 10000)
        timeout = 10000;

    g_revealDelay = delay;
    g_safetyTimeout = timeout;
}

static bool ClassIs(PCWSTR a, PCWSTR b) {
    return a && b && !_wcsicmp(a, b);
}

struct Entry {
    HWND hWnd;
    DWORD cloakTick;
    DWORD revealAt;
    bool pending;
};

static Entry g_entries[MAX_TRACKED];
static CRITICAL_SECTION g_lock;
static bool g_lockInit = false;

static Entry* FindEntry(HWND hWnd) {
    for (int i = 0; i < MAX_TRACKED; i++)
        if (g_entries[i].pending && g_entries[i].hWnd == hWnd)
            return &g_entries[i];
    return nullptr;
}

static DWORD WINAPI RevealThread(LPVOID) {
    for (;;) {
        Sleep(40);
        DWORD now = GetTickCount();
        EnterCriticalSection(&g_lock);
        for (int i = 0; i < MAX_TRACKED; i++) {
            Entry& e = g_entries[i];
            if (!e.pending)
                continue;
            bool due = false;
            if (e.revealAt != 0)
                due = (LONG)(now - e.revealAt) >= 0;
            else if (now - e.cloakTick > (DWORD)g_safetyTimeout)
                due = true;  // safety net
            if (due) {
                BOOL off = FALSE;
                DwmSetWindowAttribute(e.hWnd, DWMWA_CLOAK, &off, sizeof(off));
                e.pending = false;
                e.hWnd = nullptr;
            }
        }
        LeaveCriticalSection(&g_lock);
    }
    return 0;
}

static void TrackWindow(HWND hWnd) {
    if (!g_lockInit)
        return;
    EnterCriticalSection(&g_lock);
    Entry* slot = nullptr;
    for (int i = 0; i < MAX_TRACKED; i++) {
        if (!g_entries[i].pending) {
            slot = &g_entries[i];
            break;
        }
    }
    if (slot) {
        slot->hWnd = hWnd;
        slot->cloakTick = GetTickCount();
        slot->revealAt = 0;
        slot->pending = true;
        BOOL on = TRUE;
        DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &on, sizeof(on));
    }
    LeaveCriticalSection(&g_lock);
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

    if (hWnd) {
        wchar_t cls[128];
        if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
            if (ClassIs(cls, L"CabinetWClass") || ClassIs(cls, L"ExploreWClass"))
                TrackWindow(hWnd);
        }
    }

    return hWnd;
}

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_Original;

// The content view is the big DirectUIHWND. Small siblings (ribbon, address bar,
// search box) also blit "their whole surface", so measure against the top-level
// window instead of the child, and require a genuinely large area.
static bool IsContentViewBlit(HWND hWnd, int cx, int cy) {
    if (cx < 300 || cy < 200)
        return false;
    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (!root)
        return false;
    RECT rc;
    if (!GetClientRect(root, &rc) || rc.right <= 0 || rc.bottom <= 0)
        return false;
    long long blit = (long long)cx * (long long)cy;
    long long area = (long long)rc.right * (long long)rc.bottom;
    return blit * 100 >= area * 45;
}

BOOL WINAPI BitBlt_Hook(HDC hdcDest, int x, int y, int cx, int cy, HDC hdcSrc,
                        int x1, int y1, DWORD rop) {
    if (rop == SRCCOPY && cx > 0 && cy > 0 && g_lockInit) {
        HWND hWnd = WindowFromDC(hdcDest);
        if (hWnd) {
            wchar_t cls[128];
            if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
                ClassIs(cls, L"DirectUIHWND") && IsContentViewBlit(hWnd, cx, cy)) {
                HWND root = GetAncestor(hWnd, GA_ROOT);
                EnterCriticalSection(&g_lock);
                Entry* e = FindEntry(root);
                if (e && e->revealAt == 0)
                    e->revealAt = GetTickCount() + (DWORD)g_revealDelay;
                LeaveCriticalSection(&g_lock);
            }
        }
    }
    return BitBlt_Original(hdcDest, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

BOOL Wh_ModInit() {
    LoadSettings();

    InitializeCriticalSection(&g_lock);
    g_lockInit = true;

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(BitBlt, BitBlt_Hook, &BitBlt_Original);

    HANDLE h = CreateThread(nullptr, 0, RevealThread, nullptr, 0, nullptr);
    if (h)
        CloseHandle(h);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

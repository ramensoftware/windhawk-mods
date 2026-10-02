// ==WindhawkMod==
// @id              notification-center-position
// @name            Notification Center Position
// @description     Move the Windows 11 Notification Center to different taskbar positions.
// @version         1.0.5
// @author          MaestroSky
// @github          MaestroSky
// @include         ShellExperienceHost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notification Center Position

Repositions the Windows 11 Notification Center panel relative to the taskbar.

## Positions
- **right** — Default Windows behavior (bottom-right corner)
- **center** — Horizontally centered above the taskbar
- **left_tray** — Aligned to the left edge of the system tray area
- **left** — Aligned to the far left edge of the screen

## Settings
- **position**: `right` / `center` / `left_tray` / `left`
- **edgeGap**: Gap in pixels from screen/taskbar edges (default: 12)
- **offsetX**: Fine horizontal offset in pixels added on top of the preset
- **offsetY**: Fine vertical offset in pixels added on top of the preset
- **pollIntervalMs**: Watcher poll interval in milliseconds (default: 50)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: right
  $name: Panel position
  $description: Where to show the Notification Center panel
  $options:
  - right: Right (default)
  - center: Center of taskbar
  - left_tray: Left of tray area
  - left: Far left
- edgeGap: 12
  $name: Edge gap (px)
  $description: Gap in pixels between panel and screen/taskbar edges
- offsetX: 0
  $name: Horizontal offset (px)
  $description: Fine-tune horizontal position in pixels, positive moves right, negative moves left
- offsetY: 0
  $name: Vertical offset (px)
  $description: Fine-tune vertical position in pixels, positive moves down, negative moves up
- pollIntervalMs: 50
  $name: Poll interval (ms)
  $description: How often the watcher checks for the panel in milliseconds
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <string>
#include <atomic>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    std::wstring position;
    int edgeGap;
    int offsetX;
    int offsetY;
    int pollIntervalMs;
};

static Settings          g_settings;
static std::atomic<bool> g_running{ false };
static HANDLE            g_watchThread = nullptr;

void LoadSettings() {
    WindhawkUtils::StringSetting pos(Wh_GetStringSetting(L"position"));
    g_settings.position       = pos.get() ? pos.get() : L"right";
    g_settings.edgeGap        = Wh_GetIntSetting(L"edgeGap");
    g_settings.offsetX        = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY        = Wh_GetIntSetting(L"offsetY");
    g_settings.pollIntervalMs = Wh_GetIntSetting(L"pollIntervalMs");
    if (g_settings.edgeGap        < 0)   g_settings.edgeGap        = 0;
    if (g_settings.pollIntervalMs < 10)  g_settings.pollIntervalMs = 10;
    if (g_settings.pollIntervalMs > 500) g_settings.pollIntervalMs = 500;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static int GetTrayLeftX() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) return 0;
    HWND trayNotify = FindWindowExW(tray, nullptr, L"TrayNotifyWnd", nullptr);
    if (!trayNotify) return 0;
    RECT rc = {};
    GetWindowRect(trayNotify, &rc);
    return rc.left;
}

static int ComputeNewX(int winW) {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int gap      = g_settings.edgeGap;
    if (g_settings.position == L"right")
        return screenW - winW - gap;
    if (g_settings.position == L"center")
        return (screenW - winW) / 2;
    if (g_settings.position == L"left_tray") {
        int x = GetTrayLeftX() - winW - gap;
        return (x < gap) ? gap : x;
    }
    return gap; // left
}

// ---------------------------------------------------------------------------
// Window finder
// The NC panel lives permanently off-screen (x >= screenW) when "closed".
// We consider it "open" only when it's within the visible screen area.
// ---------------------------------------------------------------------------

struct FindAcCtx {
    HWND result;
    DWORD pid;
    int screenW;
    int screenH;
};

static BOOL CALLBACK FindAcProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<FindAcCtx*>(lParam);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid) return TRUE;

    // Must be a CoreWindow
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) return TRUE;

    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    int w = rc.right  - rc.left;
    int h = rc.bottom - rc.top;

    // Size filter: NC panel is tall and moderate width
    if (w < 200 || w > ctx->screenW - 50) return TRUE;
    if (h < 400) return TRUE;

    // KEY CHECK: panel must be on-screen (not parked off to the right)
    // When "closed", Windows parks it at x = screenW or beyond.
    if (rc.left >= ctx->screenW) return TRUE;
    // Also reject if positioned above screen
    if (rc.bottom < 0) return TRUE;

    ctx->result = hwnd;
    return FALSE;
}

static HWND FindActionCenter() {
    FindAcCtx ctx{};
    ctx.pid     = GetCurrentProcessId();
    ctx.screenW = GetSystemMetrics(SM_CXSCREEN);
    ctx.screenH = GetSystemMetrics(SM_CYSCREEN);
    EnumWindows(FindAcProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.result;
}

// ---------------------------------------------------------------------------
// Reposition
// ---------------------------------------------------------------------------

static void RepositionWindow(HWND hwnd) {
    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    int winW = rc.right  - rc.left;
    int winH = rc.bottom - rc.top;

    RECT work = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);

    int newX = ComputeNewX(winW) + g_settings.offsetX;
    int newY = work.bottom - winH - g_settings.edgeGap + g_settings.offsetY;

    // Only move if position actually differs
    if (rc.left == newX && rc.top == newY) return;

    Wh_Log(L"[NC-Move] (%d,%d)->(%d,%d) size=%dx%d pos=%s",
           rc.left, rc.top, newX, newY, winW, winH,
           g_settings.position.c_str());

    SetWindowPos(hwnd, nullptr, newX, newY, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// ---------------------------------------------------------------------------
// Watcher thread
// ---------------------------------------------------------------------------

static DWORD WINAPI WatcherThread(LPARAM) {
    Wh_Log(L"[NC-Watcher] Thread started");
    HWND lastHwnd = nullptr;

    while (g_running.load()) {
        HWND ac = FindActionCenter();

        if (ac) {
            // Reposition every tick while open — handles animations that
            // briefly move the window back, then settle.
            RepositionWindow(ac);
            if (!lastHwnd)
                Wh_Log(L"[NC-Watcher] Panel opened");
            lastHwnd = ac;
        } else if (lastHwnd) {
            Wh_Log(L"[NC-Watcher] Panel closed");
            lastHwnd = nullptr;
        }

        Sleep(static_cast<DWORD>(g_settings.pollIntervalMs));
    }

    Wh_Log(L"[NC-Watcher] Thread stopped");
    return 0;
}

static void StartWatcher() {
    if (g_watchThread) return;
    g_running     = true;
    g_watchThread = CreateThread(nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(WatcherThread),
        nullptr, 0, nullptr);
    if (!g_watchThread)
        Wh_Log(L"[NC-Watcher] Failed to create thread!");
}

static void StopWatcher() {
    g_running = false;
    if (g_watchThread) {
        WaitForSingleObject(g_watchThread, 2000);
        CloseHandle(g_watchThread);
        g_watchThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[NC-Position] Init v1.0.5");
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[NC-Position] AfterInit — starting watcher");
    StartWatcher();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[NC-Position] BeforeUninit — stopping watcher");
    StopWatcher();
}

void Wh_ModUninit() {
    Wh_Log(L"[NC-Position] Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[NC-Position] Settings changed, reloading");
    StopWatcher();
    LoadSettings();
    StartWatcher();
}

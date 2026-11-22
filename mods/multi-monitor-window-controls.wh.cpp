// ==WindhawkMod==
// @id              multi-monitor-window-controls
// @name            Multi-Monitor Window Controls
// @description     Keyboard shortcuts to move windows between monitors, minimize and restore them one at a time, and switch focus between monitors (double tap Right Shift) with a colored outline flash.
// @version         1.12.1
// @author          Momin
// @github          https://github.com/mominmansoor
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Multi-Monitor Window Controls

Hold **Right Ctrl** and press **Left** or **Right Arrow** to send the active
(focused) window to the neighbouring monitor on that side. The window stays
focused, so you can keep pressing the keys to bounce it between monitors.

Hold **Right Ctrl + Right Shift** and press **Left** or **Right Arrow** to send
**all** windows on the active monitor (the one holding the focused window, or
the one under the mouse if nothing is focused) to the neighbouring monitor.
Minimized windows, windows on other virtual desktops, tool windows, the desktop
and the taskbar are left alone.

Hold **Right Ctrl** and press **Down Arrow** to minimize the top window on the
active monitor, so the one under it shows. Press again to minimize the next one.
Press **Right Ctrl + Up Arrow** to bring them back one at a time, the most
recently minimized first, each one on top of the last.

Hold **Right Ctrl + Right Shift** and press **Up** or **Down Arrow** to move
keyboard focus to the next or previous monitor (in left to right order,
wrapping around, so with two monitors either key jumps to the other one). The
top window on that monitor gets focus. Monitors with no windows are skipped.

You can also **double tap Right Shift** (tap and release it twice, quickly, with no
other key in between) to jump focus to the next monitor. Using Right Shift as part
of another shortcut or to type a capital letter, or holding it, doesn't count as a tap.
Whenever focus switches, a short outline flashes around the monitor that got it (white by default, color is a setting).

- Maximized (or borderless fullscreen) windows stay maximized/fullscreen on the
  new monitor.
- Other windows keep their size (scaled for the monitor's DPI) and are centered
  on the new monitor. If a window is bigger than the new monitor it is shrunk
  to fit.
- Monitors are ordered left to right by their position in Windows' display
  settings. Pressing a direction with no monitor on that side does nothing.
- The desktop and taskbar are ignored.

Based on the "Move Window to Monitor" mod by TomberWolf. Credits to sultansaad-dev as well.

The shortcuts above are written with Right Ctrl, which is the default. If your
keyboard has no Right Ctrl, or you use it for something else, pick Left Ctrl,
Right Alt or Left Alt in the mod settings and every shortcut uses that key instead.

Note: this mod uses a low-level keyboard hook so it can tell Right Ctrl from
Left Ctrl. It only reacts to the shortcuts above; while their modifier keys are held,
the matching arrow keys are not passed on to other programs.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifier: rctrl
  $name: Shortcut modifier key
  $description: The key you hold for all the shortcuts. Right Ctrl is the default; pick another if your keyboard has no Right Ctrl. Right Alt can clash with AltGr on some layouts, and tapping an Alt key alone may open an app's menu bar.
  $options:
  - rctrl: Right Ctrl
  - lctrl: Left Ctrl
  - ralt: Right Alt
  - lalt: Left Alt
- outlineColor: white
  $name: Focus outline color
  $description: Color of the outline that flashes around the monitor that gets focus.
  $options:
  - white: White
  - red: Red
  - orange: Orange
  - yellow: Yellow
  - lime: Lime
  - green: Green
  - cyan: Cyan
  - blue: Blue
  - purple: Purple
  - pink: Pink
  - black: Black
- outlineOpacity: 100
  $name: Focus outline opacity (%)
  $description: How solid the outline is, from 10 (faint) to 100 (fully solid).
- outlineWidth: 3
  $name: Focus outline width (pixels)
  $description: Thickness of the outline. Recommended 1 to 5.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <vector>
#include <atomic>
#include <algorithm>

// Posted from the keyboard hook to the message window so the (potentially
// slow) window moving never runs inside the hook callback.
// wParam: -1 = move left, +1 = move right
#define WM_APP_MOVE_WINDOW (WM_APP + 1)
#define WM_APP_MOVE_ALL    (WM_APP + 2)
#define WM_APP_MINIMIZE_OTHERS   (WM_APP + 3)
#define WM_APP_RESTORE_MINIMIZED (WM_APP + 4)
#define WM_APP_FOCUS_MONITOR     (WM_APP + 5)

struct MonitorInfo {
    HMONITOR hMon;
    RECT     rcMon;
    RECT     rcWork;
    UINT     dpi;
};

static HANDLE            g_hThread  = nullptr;
static DWORD             g_threadId = 0;
static HWND              g_hMsgWnd  = nullptr;
static HHOOK             g_hHook    = nullptr;
static DWORD             g_swallowedKey = 0;

// Double tap Right Shift detection (only touched from the keyboard hook).
static bool  g_rshiftDown      = false;
static bool  g_rshiftDirty     = false;  // another key was pressed during the tap
static DWORD g_rshiftPressTick = 0;
static DWORD g_lastTapTick    = 0;
static const DWORD kTapMaxHoldMs = 250;  // longest press that still counts as a tap
static const DWORD kDoubleTapMs  = 400;  // max time between the two taps
static std::atomic<bool> g_running{false};

// ── monitors ──────────────────────────────────────────────────────────────────

static UINT GetMonitorDpi(HMONITOR hMon) {
    typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitorFn fn = []() -> GetDpiForMonitorFn {
        HMODULE h = LoadLibraryW(L"shcore.dll");
        return h ? reinterpret_cast<GetDpiForMonitorFn>(
                       GetProcAddress(h, "GetDpiForMonitor"))
                 : nullptr;
    }();
    UINT x = 96, y = 96;
    if (fn) fn(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &x, &y);
    return x ? x : 96;
}

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return TRUE;

    MonitorInfo info;
    info.hMon   = hMon;
    info.rcMon  = mi.rcMonitor;
    info.rcWork = mi.rcWork;
    info.dpi    = GetMonitorDpi(hMon);
    list->push_back(info);
    return TRUE;
}

// Sorted left to right (then top to bottom).
static std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&list));
    std::sort(list.begin(), list.end(),
              [](const MonitorInfo& a, const MonitorInfo& b) {
                  if (a.rcMon.left != b.rcMon.left)
                      return a.rcMon.left < b.rcMon.left;
                  return a.rcMon.top < b.rcMon.top;
              });
    return list;
}

static int FindMonitorIndex(const std::vector<MonitorInfo>& monitors,
                            HMONITOR hMon) {
    for (int i = 0; i < (int)monitors.size(); i++)
        if (monitors[i].hMon == hMon) return i;
    return -1;
}

// WINDOWPLACEMENT uses "workspace" coordinates, which are offset from screen
// coordinates by the primary monitor's work area origin (e.g. a taskbar on the
// left or top). Returns that offset.
static POINT GetWorkspaceOffset() {
    POINT origin = {0, 0};
    HMONITOR hPrimary = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    POINT off = {0, 0};
    if (GetMonitorInfo(hPrimary, &mi)) {
        off.x = mi.rcWork.left;
        off.y = mi.rcWork.top;
    }
    return off;
}

// ── active window ─────────────────────────────────────────────────────────────

// Store (UWP) apps like WhatsApp draw into a CoreWindow that lives inside an
// ApplicationFrameWindow. The frame is the window that can actually be moved
// and minimized, so map the inner window to its frame.
static BOOL CALLBACK FindFrameProc(HWND frame, LPARAM lParam) {
    auto* pair = reinterpret_cast<HWND*>(lParam);  // [0] = core, [1] = result
    wchar_t cls[64] = {};
    GetClassNameW(frame, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"ApplicationFrameWindow") != 0) return TRUE;
    if (FindWindowExW(frame, nullptr, L"Windows.UI.Core.CoreWindow", nullptr) ==
        pair[0]) {
        pair[1] = frame;
        return FALSE;
    }
    return TRUE;
}

static HWND GetActiveTopLevelWindow() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return nullptr;

    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd) return nullptr;

    {
        wchar_t c[64] = {};
        GetClassNameW(hwnd, c, ARRAYSIZE(c));
        if (wcscmp(c, L"Windows.UI.Core.CoreWindow") == 0) {
            HWND pair[2] = {hwnd, nullptr};
            EnumWindows(FindFrameProc, reinterpret_cast<LPARAM>(pair));
            if (pair[1]) hwnd = pair[1];
        }
    }
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return nullptr;

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    Wh_Log(L"Active window class: %s", cls);
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return nullptr;
    }
    return hwnd;
}

// ── moving ────────────────────────────────────────────────────────────────────

// bulk = true when many windows are moved at once: windows are raised without
// being activated, and moved asynchronously so one hung app can't stall the rest.
static void MoveWindowToMonitor(HWND hwnd, const MonitorInfo& src,
                                const MonitorInfo& dst, bool bulk = false) {
    const int dstW = dst.rcWork.right  - dst.rcWork.left;
    const int dstH = dst.rcWork.bottom - dst.rcWork.top;

    WINDOWPLACEMENT wp = { sizeof(wp) };
    if (!GetWindowPlacement(hwnd, &wp)) return;

    RECT rcWin = {};
    GetWindowRect(hwnd, &rcWin);

    const bool maximized = IsZoomed(hwnd) != FALSE;

    // Borderless fullscreen (F11 browser, video player...): cover the new monitor.
    if (!maximized && EqualRect(&rcWin, &src.rcMon)) {
        SetWindowPos(hwnd, nullptr, dst.rcMon.left, dst.rcMon.top,
                     dst.rcMon.right - dst.rcMon.left,
                     dst.rcMon.bottom - dst.rcMon.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    // Size to carry over: the restored size if maximized, else the current size.
    const RECT rcSize = maximized ? wp.rcNormalPosition : rcWin;
    int w = rcSize.right  - rcSize.left;
    int h = rcSize.bottom - rcSize.top;

    if (src.dpi > 0 && dst.dpi > 0 && src.dpi != dst.dpi) {
        w = MulDiv(w, (int)dst.dpi, (int)src.dpi);
        h = MulDiv(h, (int)dst.dpi, (int)src.dpi);
    }

    // Try to fit.
    w = std::min(w, dstW);
    h = std::min(h, dstH);

    // Center on the new monitor's work area.
    const int x = dst.rcWork.left + (dstW - w) / 2;
    const int y = dst.rcWork.top  + (dstH - h) / 2;

    if (maximized) {
        // Place the restored rect on the destination monitor, then maximize:
        // Windows maximizes onto the monitor that holds the restored rect.
        const POINT off = GetWorkspaceOffset();
        wp.rcNormalPosition =
            RECT{x - off.x, y - off.y, x - off.x + w, y - off.y + h};

        wp.showCmd = SW_SHOWNORMAL;
        SetWindowPlacement(hwnd, &wp);

        wp.showCmd = SW_SHOWMAXIMIZED;
        SetWindowPlacement(hwnd, &wp);
    } else {
        // Applied twice: apps that react to a DPI change can resize themselves
        // right after the first move.
        HWND insertAfter = bulk ? HWND_TOP : nullptr;
        UINT flags = SWP_NOACTIVATE | (bulk ? SWP_ASYNCWINDOWPOS : SWP_NOZORDER);
        SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
        SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
    }
}

static void MoveActiveWindow(int direction /* -1 left, +1 right */) {
    HWND hwnd = GetActiveTopLevelWindow();
    if (!hwnd) {
        Wh_Log(L"No movable active window");
        return;
    }

    auto monitors = GetAllMonitors();
    HMONITOR hCur = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    int curIdx = FindMonitorIndex(monitors, hCur);
    if (curIdx < 0) return;

    int dstIdx = curIdx + direction;
    if (dstIdx < 0 || dstIdx >= (int)monitors.size()) {
        Wh_Log(L"No monitor in that direction (current index %d of %d)",
               curIdx, (int)monitors.size());
        return;
    }

    Wh_Log(L"Moving window from monitor %d to %d", curIdx, dstIdx);
    MoveWindowToMonitor(hwnd, monitors[curIdx], monitors[dstIdx]);
}

// ── move all windows on a monitor ────────────────────────────────────────────

static bool IsWindowCloaked(HWND hwnd) {
    // Windows on other virtual desktops (and suspended UWP windows) are cloaked.
    typedef HRESULT(WINAPI * DwmGetWindowAttributeFn)(HWND, DWORD, PVOID, DWORD);
    static DwmGetWindowAttributeFn fn = []() -> DwmGetWindowAttributeFn {
        HMODULE h = LoadLibraryW(L"dwmapi.dll");
        return h ? reinterpret_cast<DwmGetWindowAttributeFn>(
                       GetProcAddress(h, "DwmGetWindowAttribute"))
                 : nullptr;
    }();
    DWORD cloaked = 0;
    if (fn && SUCCEEDED(fn(hwnd, 14 /*DWMWA_CLOAKED*/, &cloaked, sizeof(cloaked))))
        return cloaked != 0;
    return false;
}

static bool IsBulkMovableWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    if (IsWindowCloaked(hwnd)) return false;
    if (GetWindowTextLengthW(hwnd) == 0) return false;

    const LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) && !(ex & WS_EX_APPWINDOW)) return false;
    if (ex & WS_EX_NOACTIVATE) return false;

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }
    return true;
}

struct EnumContext {
    HMONITOR             src;
    std::vector<HWND>    windows;
};

static BOOL CALLBACK EnumWindowsCollectProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);
    if (!IsBulkMovableWindow(hwnd)) return TRUE;
    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) != ctx->src)
        return TRUE;
    ctx->windows.push_back(hwnd);
    return TRUE;
}

static void MoveAllWindows(int direction /* -1 left, +1 right */) {
    // Source monitor: the one with the focused window, else the one under the
    // mouse cursor.
    HMONITOR hSrc = nullptr;
    if (HWND active = GetActiveTopLevelWindow()) {
        hSrc = MonitorFromWindow(active, MONITOR_DEFAULTTONEAREST);
    } else {
        POINT pt;
        if (GetCursorPos(&pt))
            hSrc = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    if (!hSrc) return;

    auto monitors = GetAllMonitors();
    int srcIdx = FindMonitorIndex(monitors, hSrc);
    if (srcIdx < 0) return;

    int dstIdx = srcIdx + direction;
    if (dstIdx < 0 || dstIdx >= (int)monitors.size()) {
        Wh_Log(L"No monitor in that direction (current index %d of %d)", srcIdx,
               (int)monitors.size());
        return;
    }

    EnumContext ctx;
    ctx.src = hSrc;
    EnumWindows(EnumWindowsCollectProc, reinterpret_cast<LPARAM>(&ctx));

    Wh_Log(L"Moving %d window(s) from monitor %d to %d",
           (int)ctx.windows.size(), srcIdx, dstIdx);

    // EnumWindows lists top to bottom; move bottom to top so the windows keep
    // their stacking order (the previously active window ends up on top).
    for (auto it = ctx.windows.rbegin(); it != ctx.windows.rend(); ++it) {
        MoveWindowToMonitor(*it, monitors[srcIdx], monitors[dstIdx], true);
    }
}

// ── minimize the top window one press at a time, and bring them back ──────────────

// Windows minimized by focus mode, in top-to-bottom stacking order.
// Only touched from the message window's thread.
static std::vector<HWND> g_minimizedByFocus;

static void ActivateWindow(HWND hwnd);

// Minimizes the top window only, so the one underneath shows through.
// Press again to minimize the next one. Windows are remembered in a stack.
static void MinimizeTopWindow() {
    HWND active = GetActiveTopLevelWindow();
    if (!active) {
        Wh_Log(L"No active window to minimize");
        return;
    }

    EnumContext ctx;
    ctx.src = MonitorFromWindow(active, MONITOR_DEFAULTTONEAREST);
    EnumWindows(EnumWindowsCollectProc, reinterpret_cast<LPARAM>(&ctx));

    // Next window down in the stack on this monitor (EnumWindows is top to bottom).
    HWND next = nullptr;
    for (HWND hwnd : ctx.windows) {
        if (hwnd != active) { next = hwnd; break; }
    }

    wchar_t title[80] = {};
    GetWindowTextW(active, title, ARRAYSIZE(title));
    ShowWindow(active, SW_MINIMIZE);
    if (IsIconic(active)) {
        g_minimizedByFocus.push_back(active);
        Wh_Log(L"Minimized \"%s\"", title);
        if (next) ActivateWindow(next);
    } else {
        Wh_Log(L"Could not minimize \"%s\" (elevated?)", title);
    }
}

// One window per press. The list is in top-to-bottom stacking order, so taking
// from the end brings windows back bottom to top, each landing on top of the
// previous one (the window that was on top ends up on top again).
static void RestoreNextMinimizedWindow() {
    while (!g_minimizedByFocus.empty()) {
        HWND hwnd = g_minimizedByFocus.back();
        g_minimizedByFocus.pop_back();

        // Skip windows that were closed or restored by hand in the meantime.
        if (!IsWindow(hwnd) || !IsIconic(hwnd)) continue;

        ShowWindow(hwnd, SW_RESTORE);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ActivateWindow(hwnd);
        Wh_Log(L"Restored a window (%d left to restore)",
               (int)g_minimizedByFocus.size());
        return;
    }
    Wh_Log(L"Nothing to restore");
}

// ── switch focus to another monitor ──────────────────────────────────────────

static void ActivateWindow(HWND hwnd) {
    // Windows only lets the process that last received input take focus. A
    // synthetic tap of an unused key (F24) counts as input from this process.
    INPUT in[2] = {};
    in[0].type       = INPUT_KEYBOARD;
    in[0].ki.wVk     = VK_F24;
    in[1].type       = INPUT_KEYBOARD;
    in[1].ki.wVk     = VK_F24;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));

    SetForegroundWindow(hwnd);

    if (GetForegroundWindow() != hwnd) {
        // Fallback: temporarily share input state with the current foreground
        // window's thread.
        HWND fg = GetForegroundWindow();
        DWORD fgThread = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
        DWORD myThread = GetCurrentThreadId();
        if (fgThread && fgThread != myThread &&
            AttachThreadInput(myThread, fgThread, TRUE)) {
            BringWindowToTop(hwnd);
            SetForegroundWindow(hwnd);
            AttachThreadInput(myThread, fgThread, FALSE);
        }
    }
}

// ── white outline flash on the monitor that got focus ────────────────────────

static const wchar_t kFlashClass[] = L"WH_MultiMonitorControls_Flash";
static bool g_flashClassRegistered = false;
static int  g_flashTick = 0;
static std::atomic<COLORREF> g_outlineColor{RGB(255, 255, 255)};
// Background of the flash window; this exact color is made see-through.
static std::atomic<int> g_outlineAlpha{255};
static std::atomic<int> g_outlineWidth{3};
// 0 = Right Ctrl, 1 = Left Ctrl, 2 = Right Alt, 3 = Left Alt
static std::atomic<int> g_modifier{0};
static const COLORREF kFlashKey = RGB(1, 2, 3);

static void LoadSettings() {
    struct Named { const wchar_t* name; COLORREF color; };
    static const Named table[] = {
        {L"white",  RGB(255, 255, 255)}, {L"red",    RGB(255, 40, 40)},
        {L"orange", RGB(255, 140, 0)},   {L"yellow", RGB(255, 230, 0)},
        {L"lime",   RGB(160, 255, 0)},   {L"green",  RGB(0, 200, 70)},
        {L"cyan",   RGB(0, 230, 255)},   {L"blue",   RGB(40, 100, 255)},
        {L"purple", RGB(160, 70, 255)},  {L"pink",   RGB(255, 100, 190)},
        {L"black",  RGB(0, 0, 0)},
    };
    {
        PCWSTR m = Wh_GetStringSetting(L"modifier");
        int mod = 0;
        if (m) {
            if (wcscmp(m, L"lctrl") == 0) mod = 1;
            else if (wcscmp(m, L"ralt") == 0) mod = 2;
            else if (wcscmp(m, L"lalt") == 0) mod = 3;
            Wh_FreeStringSetting(m);
        }
        g_modifier = mod;
    }

    COLORREF c = RGB(255, 255, 255);
    PCWSTR v = Wh_GetStringSetting(L"outlineColor");
    if (v) {
        for (const auto& n : table) {
            if (wcscmp(v, n.name) == 0) { c = n.color; break; }
        }
        Wh_FreeStringSetting(v);
    }
    g_outlineColor = c;

    int pct = Wh_GetIntSetting(L"outlineOpacity");
    if (pct < 10) pct = 10;
    if (pct > 100) pct = 100;
    g_outlineAlpha = 255 * pct / 100;

    int w = Wh_GetIntSetting(L"outlineWidth");
    if (w < 1) w = 1;
    if (w > 20) w = 20;
    g_outlineWidth = w;
}

static LRESULT CALLBACK FlashWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            // kFlashKey is see-through, the chosen color is the outline.
            HBRUSH keyBrush = CreateSolidBrush(kFlashKey);
            FillRect(dc, &rc, keyBrush);
            DeleteObject(keyBrush);
            HBRUSH white = CreateSolidBrush(g_outlineColor.load());
            const int t = g_outlineWidth.load();
            RECT top = {rc.left, rc.top, rc.right, rc.top + t};
            RECT bot = {rc.left, rc.bottom - t, rc.right, rc.bottom};
            RECT lef = {rc.left, rc.top, rc.left + t, rc.bottom};
            RECT rig = {rc.right - t, rc.top, rc.right, rc.bottom};
            FillRect(dc, &top, white);
            FillRect(dc, &bot, white);
            FillRect(dc, &lef, white);
            FillRect(dc, &rig, white);
            DeleteObject(white);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_TIMER: {
            // Hold for about 150 ms, then fade out over about 250 ms.
            const int hold = 9, fade = 15;
            g_flashTick++;
            if (g_flashTick > hold + fade) {
                KillTimer(hwnd, 1);
                DestroyWindow(hwnd);
                return 0;
            }
            if (g_flashTick > hold) {
                int top = g_outlineAlpha.load();
                BYTE a = (BYTE)(top - top * (g_flashTick - hold) / fade);
                SetLayeredWindowAttributes(hwnd, kFlashKey, a, LWA_COLORKEY | LWA_ALPHA);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void FlashMonitorOutline(const RECT& rc) {
    if (!g_flashClassRegistered) {
        WNDCLASSEX wc = {};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = FlashWndProc;
        wc.hInstance     = GetModuleHandle(nullptr);
        wc.lpszClassName = kFlashClass;
        if (!RegisterClassEx(&wc)) return;
        g_flashClassRegistered = true;
    }
    // Only one flash at a time.
    if (HWND old = FindWindowW(kFlashClass, nullptr)) DestroyWindow(old);

    HWND w = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kFlashClass, nullptr, WS_POPUP, rc.left, rc.top, rc.right - rc.left,
        rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr),
        nullptr);
    if (!w) return;
    g_flashTick = 0;
    SetLayeredWindowAttributes(w, kFlashKey, (BYTE)g_outlineAlpha.load(),
                               LWA_COLORKEY | LWA_ALPHA);
    ShowWindow(w, SW_SHOWNOACTIVATE);
    SetWindowPos(w, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    UpdateWindow(w);
    SetTimer(w, 1, 16, nullptr);
}

static void FocusAdjacentMonitor(int direction /* +1 next, -1 previous */) {
    auto monitors = GetAllMonitors();
    const int n = (int)monitors.size();
    if (n < 2) return;

    HMONITOR hCur = nullptr;
    if (HWND active = GetActiveTopLevelWindow()) {
        hCur = MonitorFromWindow(active, MONITOR_DEFAULTTONEAREST);
    } else {
        POINT pt;
        if (GetCursorPos(&pt))
            hCur = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    int curIdx = FindMonitorIndex(monitors, hCur);
    if (curIdx < 0) return;

    // Go around the monitors until one has a window to focus.
    for (int step = 1; step < n; step++) {
        int idx = ((curIdx + direction * step) % n + n) % n;

        EnumContext ctx;
        ctx.src = monitors[idx].hMon;
        EnumWindows(EnumWindowsCollectProc, reinterpret_cast<LPARAM>(&ctx));
        if (ctx.windows.empty()) continue;

        // EnumWindows lists top to bottom, so the first one is the top window.
        Wh_Log(L"Focusing monitor %d", idx);
        ActivateWindow(ctx.windows.front());
        FlashMonitorOutline(monitors[idx].rcMon);
        return;
    }
    Wh_Log(L"No other monitor has a window to focus");
}

// ── keyboard hook ─────────────────────────────────────────────────────────────

static bool KeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

// True when the chosen modifier is held, its twin on the other side is not,
// and no other modifier family (Ctrl / Alt / Win) is mixed in.
static bool ModifierHeld() {
    switch (g_modifier.load()) {
        case 1:  // Left Ctrl
            return KeyDown(VK_LCONTROL) && !KeyDown(VK_RCONTROL) &&
                   !KeyDown(VK_MENU) && !KeyDown(VK_LWIN) && !KeyDown(VK_RWIN);
        case 2:  // Right Alt (AltGr layouts also hold Left Ctrl, so allow it)
            return KeyDown(VK_RMENU) && !KeyDown(VK_LMENU) &&
                   !KeyDown(VK_RCONTROL) && !KeyDown(VK_LWIN) &&
                   !KeyDown(VK_RWIN);
        case 3:  // Left Alt
            return KeyDown(VK_LMENU) && !KeyDown(VK_RMENU) &&
                   !KeyDown(VK_CONTROL) && !KeyDown(VK_LWIN) &&
                   !KeyDown(VK_RWIN);
        default:  // Right Ctrl
            return KeyDown(VK_RCONTROL) && !KeyDown(VK_LCONTROL) &&
                   !KeyDown(VK_MENU) && !KeyDown(VK_LWIN) && !KeyDown(VK_RWIN);
    }
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam,
                                             LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

        // Double tap Right Shift -> switch monitor focus.
        if (!(kb->flags & LLKHF_INJECTED)) {
            const bool evDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            const bool evUp   = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

            if (kb->vkCode == VK_RSHIFT) {
                if (evDown) {
                    if (!g_rshiftDown) {  // ignore auto-repeat
                        g_rshiftDown      = true;
                        g_rshiftDirty     = false;
                        g_rshiftPressTick = GetTickCount();
                    }
                } else if (evUp) {
                    g_rshiftDown = false;
                    const DWORD now = GetTickCount();
                    const bool cleanTap =
                        !g_rshiftDirty && (now - g_rshiftPressTick) <= kTapMaxHoldMs;
                    if (cleanTap) {
                        if (g_lastTapTick && (now - g_lastTapTick) <= kDoubleTapMs) {
                            g_lastTapTick = 0;
                            if (g_hMsgWnd)
                                PostMessage(g_hMsgWnd, WM_APP_FOCUS_MONITOR,
                                            (WPARAM)1, 0);
                        } else {
                            g_lastTapTick = now;
                        }
                    } else {
                        g_lastTapTick = 0;
                    }
                }
            } else if (evDown) {
                // Any other key cancels a tap in progress or a pending first tap.
                if (g_rshiftDown) g_rshiftDirty = true;
                g_lastTapTick = 0;
            }
        }

        if (!(kb->flags & LLKHF_INJECTED) &&
            (kb->vkCode == VK_LEFT || kb->vkCode == VK_RIGHT ||
             kb->vkCode == VK_UP || kb->vkCode == VK_DOWN)) {
            const bool isDown =
                wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            const bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

            if (isDown) {
                // The chosen modifier (Right Ctrl by default).
                const bool baseMods = ModifierHeld();
                // Modifier              -> move the active window
                // Modifier + R.Shift  -> move all windows on the monitor
                // Modifier + Down -> minimize the top window
                // Modifier + Up   -> bring one back
                const bool horizontal =
                    kb->vkCode == VK_LEFT || kb->vkCode == VK_RIGHT;
                UINT msgId = 0;
                if (horizontal && baseMods && !KeyDown(VK_SHIFT)) {
                    msgId = WM_APP_MOVE_WINDOW;
                } else if (horizontal && baseMods && KeyDown(VK_RSHIFT) &&
                           !KeyDown(VK_LSHIFT)) {
                    msgId = WM_APP_MOVE_ALL;
                } else if (!horizontal && baseMods && !KeyDown(VK_SHIFT)) {
                    msgId = kb->vkCode == VK_DOWN ? WM_APP_MINIMIZE_OTHERS
                                                  : WM_APP_RESTORE_MINIMIZED;
                } else if (!horizontal && baseMods && KeyDown(VK_RSHIFT) &&
                           !KeyDown(VK_LSHIFT)) {
                    // Modifier + Right Shift + Up/Down -> switch monitor focus
                    msgId = WM_APP_FOCUS_MONITOR;
                }

                if (msgId) {
                    // Ignore auto-repeat of a key we already handled.
                    if (g_swallowedKey != kb->vkCode) {
                        g_swallowedKey = kb->vkCode;
                        if (g_hMsgWnd) {
                            PostMessage(g_hMsgWnd, msgId,
                                        (kb->vkCode == VK_LEFT ||
                                         kb->vkCode == VK_DOWN)
                                            ? (WPARAM)-1
                                            : (WPARAM)1,
                                        0);
                        }
                    }
                    return 1;  // swallow
                }
                // This key-down is passed on (e.g. Right Ctrl was released
                // while the arrow auto-repeats), so pass on its key-up too.
                if (g_swallowedKey == kb->vkCode) {
                    g_swallowedKey = 0;
                }
            } else if (isUp && g_swallowedKey == kb->vkCode) {
                g_swallowedKey = 0;
                return 1;  // swallow the matching key-up
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ── hook thread ───────────────────────────────────────────────────────────────

static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (msg == WM_APP_MOVE_WINDOW) {
        MoveActiveWindow((int)(intptr_t)wParam);
        return 0;
    }
    if (msg == WM_APP_MOVE_ALL) {
        MoveAllWindows((int)(intptr_t)wParam);
        return 0;
    }
    if (msg == WM_APP_MINIMIZE_OTHERS) {
        Wh_Log(L"Minimize shortcut received");
        MinimizeTopWindow();
        return 0;
    }
    if (msg == WM_APP_FOCUS_MONITOR) {
        FocusAdjacentMonitor((int)(intptr_t)wParam);
        return 0;
    }
    if (msg == WM_APP_RESTORE_MINIMIZED) {
        Wh_Log(L"Restore shortcut received");
        RestoreNextMinimizedWindow();
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void HookThreadProc() {
    // Use real (physical) pixel coordinates regardless of the host process'
    // DPI awareness. -4 = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContextFn)(HANDLE);
    auto setDpiCtx = reinterpret_cast<SetThreadDpiAwarenessContextFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "SetThreadDpiAwarenessContext"));
    if (setDpiCtx) setDpiCtx((HANDLE)-4);

    const wchar_t CLASS_NAME[] = L"WH_MultiMonitorControls_MsgWnd";
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MsgWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetModuleHandle(nullptr),
                               nullptr);
    if (!hwnd) {
        Wh_Log(L"Failed to create message window");
        return;
    }
    g_hMsgWnd = hwnd;

    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                               GetModuleHandle(nullptr), 0);
    if (!g_hHook) {
        Wh_Log(L"Failed to install keyboard hook (error %lu)", GetLastError());
    } else {
        auto monitors = GetAllMonitors();
        Wh_Log(L"Ready: %d monitor(s) detected (left to right):",
               (int)monitors.size());
        for (int i = 0; i < (int)monitors.size(); i++) {
            const auto& m = monitors[i];
            Wh_Log(L"  %d: pos (%ld,%ld) size %ldx%ld DPI %u", i, m.rcMon.left,
                   m.rcMon.top, m.rcMon.right - m.rcMon.left,
                   m.rcMon.bottom - m.rcMon.top, m.dpi);
        }
    }

    MSG msg;
    while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
    DestroyWindow(hwnd);
    g_hMsgWnd = nullptr;
    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
    if (g_flashClassRegistered) {
        if (HWND old = FindWindowW(kFlashClass, nullptr)) DestroyWindow(old);
        UnregisterClass(kFlashClass, GetModuleHandle(nullptr));
        g_flashClassRegistered = false;
    }
}

static DWORD WINAPI HookThreadEntry(LPVOID) {
    HookThreadProc();
    return 0;
}

// ── Tool mod callbacks ────────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"Multi-Monitor Window Controls: Init");
    LoadSettings();
    g_running = true;
    g_hThread = CreateThread(nullptr, 0, HookThreadEntry, nullptr, 0, &g_threadId);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_threadId) PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

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

    WCHAR commandLine[MAX_PATH + 2 +
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

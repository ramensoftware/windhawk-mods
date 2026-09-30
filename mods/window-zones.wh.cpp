// ==WindhawkMod==
// @id              window-zones
// @name            Window Zones (Space around zones)
// @description     Multiple zone layouts with space around and between zones, each shown by its own drag modifier, inspired by PowerToys FancyZones
// @version         1.0
// @author          Anilson Lopes
// @github          https://github.com/anilsonlopes
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Zones
Places windows into zones with **space around zones**, like PowerToys
FancyZones. Windows are positioned by the mod itself, so they never enter the
Windows "snapped" state (which forces square corners).

## Layouts and drag modifiers
You can define up to 4 layouts. Each one has a **drag modifier**: hold that
modifier while dragging a window by its title bar to see the layout's zones,
and release over a zone to place the window in it. Example with the defaults:
hold **Shift** to get a 2x1 layout, hold **Ctrl** to get a 4x4 layout.

A layout is either a grid `COLUMNSxROWS` (for example `2x1`, `4x4`) or a list
of zones separated by `;`. Each zone is `x,y,width,height` in percent of the
screen work area:

- Two thirds + one third: `0,0,66.67,100; 66.67,0,33.33,100`
- Big left, two stacked on the right: `0,0,50,100; 50,0,50,50; 50,50,50,50`
- Center focus: `0,0,25,100; 25,0,50,100; 75,0,25,100`

Zones should touch each other exactly (for example `66.67` next to `66.67`)
so the space between them is uniform. Leave a layout empty to disable it. An
invalid layout is ignored and the reason is written to the log.

## Hotkeys
Hotkey modifier (Ctrl+Alt by default) plus:
- **Arrow keys:** move the active window to the neighboring zone. If the window
  is not in a zone it goes to the closest one.
- **Space:** switch the layout used by the arrow keys (the zones flash on the
  screen).

## Notes
- For best results turn off Windows snapping (Settings > System > Multitasking >
  Snap windows), otherwise Win+Arrow and edge dragging still use native snap.
- Windows of elevated apps cannot be moved by this mod.
- Zones are computed on the monitor the window is on. Moving between monitors
  with the hotkeys is not supported yet.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout1: 2x1
  $name: Layout 1
  $description: 'Grid like 2x1, or zones as "x,y,w,h" in percent separated by ";". Empty disables it.'
- layout1Modifier: shift
  $name: Layout 1 drag modifier
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt: Ctrl + Alt
  - shift_alt: Shift + Alt
- layout2: 4x4
  $name: Layout 2
  $description: 'Grid like 2x1, or zones as "x,y,w,h" in percent separated by ";". Empty disables it.'
- layout2Modifier: ctrl
  $name: Layout 2 drag modifier
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt: Ctrl + Alt
  - shift_alt: Shift + Alt
- layout3: ""
  $name: Layout 3
  $description: 'Grid like 2x1, or zones as "x,y,w,h" in percent separated by ";". Empty disables it.'
- layout3Modifier: alt
  $name: Layout 3 drag modifier
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt: Ctrl + Alt
  - shift_alt: Shift + Alt
- layout4: ""
  $name: Layout 4
  $description: 'Grid like 2x1, or zones as "x,y,w,h" in percent separated by ";". Empty disables it.'
- layout4Modifier: ctrl_shift
  $name: Layout 4 drag modifier
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - ctrl_shift: Ctrl + Shift
  - ctrl_alt: Ctrl + Alt
  - shift_alt: Shift + Alt
- outerGap: 10
  $name: Space around zones (px)
  $description: Space between the zones and the screen edge.
- innerGap: 10
  $name: Space between zones (px)
  $description: Space between two adjacent zones.
- dragOverlay: true
  $name: Drag overlay
  $description: Show the zones while dragging a window with a layout modifier held, and place it in the zone under the cursor on release.
- modifier: ctrl_alt
  $name: Hotkey modifier
  $description: Modifier keys used with the arrow keys and Space.
  $options:
  - ctrl_alt: Ctrl + Alt
  - win_alt: Win + Alt
  - ctrl_win: Ctrl + Win
  - ctrl_shift_alt: Ctrl + Shift + Alt
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <vector>

struct {
    std::atomic<int> outerGap;
    std::atomic<int> innerGap;
    std::atomic<UINT> modifiers;
    std::atomic<bool> dragOverlay;
} g_settings;

// A zone in percent of the work area.
struct Zone {
    double x, y, w, h;
};

struct Layout {
    std::vector<Zone> zones;
    UINT dragMask;  // MOD_CONTROL | MOD_SHIFT | MOD_ALT combination
};

// Only touched from the worker thread (LoadSettings runs there after startup).
// Never empty after LoadSettings.
std::vector<Layout> g_layouts;
int g_active = 0;  // layout used by the hotkeys

HANDLE g_thread;
DWORD g_threadId;
HANDLE g_ready;

constexpr UINT kMsgReload = WM_APP;
constexpr int kHotkeyLeft = 1;
constexpr int kHotkeyRight = 2;
constexpr int kHotkeyUp = 3;
constexpr int kHotkeyDown = 4;
constexpr int kHotkeyCycle = 5;
constexpr int kAlignTolerance = 8;
constexpr size_t kMaxZones = 64;
constexpr int kLayoutSlots = 4;
constexpr ULONGLONG kFlashMs = 900;

// Pixel rect of a zone on the work area wa, including the gaps: zones touching
// the work area edge get the outer gap, shared edges get half the inner gap on
// each side.
RECT ZoneRect(const RECT& wa, const Zone& z) {
    int outer = g_settings.outerGap;
    int inner = g_settings.innerGap;
    const double eps = 1e-6;

    double width = wa.right - wa.left;
    double height = wa.bottom - wa.top;

    RECT rc;
    rc.left = wa.left + (LONG)std::lround(width * z.x / 100.0);
    rc.right = wa.left + (LONG)std::lround(width * (z.x + z.w) / 100.0);
    rc.top = wa.top + (LONG)std::lround(height * z.y / 100.0);
    rc.bottom = wa.top + (LONG)std::lround(height * (z.y + z.h) / 100.0);

    rc.left += z.x < eps ? outer : inner - inner / 2;
    rc.right -= z.x + z.w > 100.0 - eps ? outer : inner / 2;
    rc.top += z.y < eps ? outer : inner - inner / 2;
    rc.bottom -= z.y + z.h > 100.0 - eps ? outer : inner / 2;

    if (rc.right <= rc.left) rc.right = rc.left + 1;
    if (rc.bottom <= rc.top) rc.bottom = rc.top + 1;
    return rc;
}

bool IsBlank(const wchar_t* p) {
    while (*p == L' ' || *p == L'\t' || *p == L'\r' || *p == L'\n') p++;
    return *p == 0;
}

// "COLSxROWS" -> equal grid.
bool ParseGrid(PCWSTR s, std::vector<Zone>& out) {
    wchar_t* end;
    long cols = wcstol(s, &end, 10);
    if (end == s || (*end != L'x' && *end != L'X')) {
        return false;
    }
    const wchar_t* p = end + 1;
    long rows = wcstol(p, &end, 10);
    if (end == p || !IsBlank(end) || cols < 1 || rows < 1 || cols > 12 ||
        rows > 12) {
        return false;
    }

    out.clear();
    for (long r = 0; r < rows; r++) {
        for (long c = 0; c < cols; c++) {
            out.push_back({100.0 * c / cols, 100.0 * r / rows, 100.0 / cols,
                           100.0 / rows});
        }
    }
    return true;
}

// "x,y,w,h; x,y,w,h; ..." (percent).
bool ParseZones(PCWSTR s, std::vector<Zone>& out) {
    out.clear();
    const wchar_t* p = s;
    while (*p) {
        while (*p == L' ' || *p == L';' || *p == L'\n' || *p == L'\r' ||
               *p == L'\t') {
            p++;
        }
        if (!*p) {
            break;
        }

        double v[4];
        for (int i = 0; i < 4; i++) {
            wchar_t* end;
            v[i] = wcstod(p, &end);
            if (end == p) {
                return false;
            }
            p = end;
            while (*p == L' ') p++;
            if (i < 3) {
                if (*p != L',') {
                    return false;
                }
                p++;
            }
        }

        if (v[0] < 0 || v[1] < 0 || v[2] <= 0 || v[3] <= 0 ||
            v[0] + v[2] > 100.001 || v[1] + v[3] > 100.001) {
            return false;
        }
        out.push_back({v[0], v[1], v[2], v[3]});
        if (out.size() > kMaxZones) {
            return false;
        }
    }
    return !out.empty();
}

bool ParseLayout(PCWSTR s, std::vector<Zone>& out) {
    return ParseGrid(s, out) || ParseZones(s, out);
}

UINT ParseDragMask(PCWSTR value) {
    if (!wcscmp(value, L"ctrl")) return MOD_CONTROL;
    if (!wcscmp(value, L"alt")) return MOD_ALT;
    if (!wcscmp(value, L"ctrl_shift")) return MOD_CONTROL | MOD_SHIFT;
    if (!wcscmp(value, L"ctrl_alt")) return MOD_CONTROL | MOD_ALT;
    if (!wcscmp(value, L"shift_alt")) return MOD_SHIFT | MOD_ALT;
    return MOD_SHIFT;
}

UINT CurrentDragMask() {
    UINT mask = 0;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) mask |= MOD_CONTROL;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) mask |= MOD_SHIFT;
    if (GetAsyncKeyState(VK_MENU) & 0x8000) mask |= MOD_ALT;
    return mask;
}

// Layout whose drag modifier is exactly the pressed modifiers, or -1.
int LayoutForMask(UINT mask) {
    for (size_t i = 0; i < g_layouts.size(); i++) {
        if (g_layouts[i].dragMask == mask) {
            return (int)i;
        }
    }
    return -1;
}

bool IsManageable(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd) ||
        GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
            !wcscmp(cls, L"Shell_TrayWnd") ||
            !wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
            return false;
        }
    }
    return true;
}

// Moves/resizes so that the visible frame (without invisible borders) matches
// the target rect.
bool PlaceWindow(HWND hwnd, const RECT& target) {
    if (IsZoomed(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }

    RECT wr, fr;
    if (!GetWindowRect(hwnd, &wr) ||
        FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &fr,
                                     sizeof(fr)))) {
        return false;
    }

    int x = target.left - (fr.left - wr.left);
    int y = target.top - (fr.top - wr.top);
    int cx = (target.right - target.left) +
             ((wr.right - wr.left) - (fr.right - fr.left));
    int cy = (target.bottom - target.top) +
             ((wr.bottom - wr.top) - (fr.bottom - fr.top));

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (!(GetWindowLongW(hwnd, GWL_STYLE) & WS_THICKFRAME)) {
        flags |= SWP_NOSIZE;  // Not resizable: only move.
    }

    if (!SetWindowPos(hwnd, nullptr, x, y, cx, cy, flags)) {
        Wh_Log(L"SetWindowPos failed: %u", GetLastError());
        return false;
    }
    return true;
}

// Closest zone (by center) to a window frame, and whether the frame is already
// aligned with it.
int FindCurrentZone(const std::vector<Zone>& zones,
                    const RECT& wa,
                    const RECT& fr,
                    bool* aligned) {
    LONG cx = (fr.left + fr.right) / 2;
    LONG cy = (fr.top + fr.bottom) / 2;
    int best = 0;
    long long bestDist = -1;
    *aligned = false;
    for (size_t i = 0; i < zones.size(); i++) {
        RECT z = ZoneRect(wa, zones[i]);
        long long zx = (z.left + z.right) / 2 - cx;
        long long zy = (z.top + z.bottom) / 2 - cy;
        long long dist = zx * zx + zy * zy;
        if (bestDist < 0 || dist < bestDist) {
            bestDist = dist;
            best = (int)i;
            *aligned = std::abs(fr.left - z.left) <= kAlignTolerance &&
                       std::abs(fr.right - z.right) <= kAlignTolerance &&
                       std::abs(fr.top - z.top) <= kAlignTolerance &&
                       std::abs(fr.bottom - z.bottom) <= kAlignTolerance;
        }
    }
    return best;
}

// Neighboring zone of `cur` in the direction (dx, dy); `cur` if there is none.
int FindNeighbor(const std::vector<Zone>& zones,
                 const RECT& wa,
                 int cur,
                 int dx,
                 int dy) {
    RECT c = ZoneRect(wa, zones[cur]);
    double cx = (c.left + c.right) / 2.0;
    double cy = (c.top + c.bottom) / 2.0;

    int best = cur;
    double bestScore = -1;
    for (size_t i = 0; i < zones.size(); i++) {
        if ((int)i == cur) {
            continue;
        }
        RECT z = ZoneRect(wa, zones[i]);
        double ddx = (z.left + z.right) / 2.0 - cx;
        double ddy = (z.top + z.bottom) / 2.0 - cy;

        bool ok = dx < 0   ? ddx < -1
                  : dx > 0 ? ddx > 1
                  : dy < 0 ? ddy < -1
                           : ddy > 1;
        if (!ok) {
            continue;
        }

        double primary = std::abs(dx ? ddx : ddy);
        double secondary = std::abs(dx ? ddy : ddx);
        double score = primary + 2 * secondary;
        if (bestScore < 0 || score < bestScore) {
            bestScore = score;
            best = (int)i;
        }
    }
    return best;
}

// ---- Overlay ----

constexpr WCHAR kOverlayClass[] = L"WindhawkWindowZonesOverlay";
constexpr COLORREF kKeyColor = RGB(255, 0, 255);
constexpr UINT kTimerMs = 30;
constexpr ULONGLONG kDropDelayMs = 40;

HWND g_overlay;
bool g_overlayVisible;
RECT g_overlayMon;
RECT g_overlayWork;
int g_overlayLayout = 0;
int g_activeZone = -1;
ULONGLONG g_flashUntil;

HWND g_dragHwnd;
HWND g_dropHwnd;
RECT g_dropRect;
ULONGLONG g_dropTick;

COLORREF AccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    return RGB(0, 120, 215);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC mem = CreateCompatibleDC(hdc);
            HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right - rc.left,
                                                 rc.bottom - rc.top);
            HGDIOBJ oldBmp = SelectObject(mem, bmp);

            HBRUSH keyBrush = CreateSolidBrush(kKeyColor);
            FillRect(mem, &rc, keyBrush);
            DeleteObject(keyBrush);

            COLORREF accent = AccentColor();
            HPEN pen = CreatePen(PS_SOLID, 3, accent);
            HGDIOBJ oldPen = SelectObject(mem, pen);
            HBRUSH inactiveBrush = CreateSolidBrush(RGB(90, 90, 90));
            HBRUSH activeBrush = CreateSolidBrush(accent);

            if (g_overlayLayout >= 0 && g_overlayLayout < (int)g_layouts.size()) {
                const auto& zones = g_layouts[g_overlayLayout].zones;
                for (size_t i = 0; i < zones.size(); i++) {
                    RECT z = ZoneRect(g_overlayWork, zones[i]);
                    OffsetRect(&z, -g_overlayMon.left, -g_overlayMon.top);
                    HGDIOBJ oldBrush = SelectObject(
                        mem,
                        (int)i == g_activeZone ? activeBrush : inactiveBrush);
                    RoundRect(mem, z.left, z.top, z.right, z.bottom, 16, 16);
                    SelectObject(mem, oldBrush);
                }
            }

            BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, mem, 0, 0,
                   SRCCOPY);

            SelectObject(mem, oldPen);
            DeleteObject(pen);
            DeleteObject(inactiveBrush);
            DeleteObject(activeBrush);
            SelectObject(mem, oldBmp);
            DeleteObject(bmp);
            DeleteDC(mem);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateOverlay() {
    HINSTANCE inst = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        return false;
    }

    g_overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT |
            WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        kOverlayClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst,
        nullptr);
    if (!g_overlay) {
        Wh_Log(L"CreateWindow failed: %u", GetLastError());
        return false;
    }

    SetLayeredWindowAttributes(g_overlay, kKeyColor, 140,
                               LWA_COLORKEY | LWA_ALPHA);
    return true;
}

void HideOverlay() {
    if (g_overlay && g_overlayVisible) {
        ShowWindow(g_overlay, SW_HIDE);
    }
    g_overlayVisible = false;
    g_activeZone = -1;
}

// Shows the overlay over the given monitor. Returns true if it changed.
bool ShowOverlayOn(const MONITORINFO& mi) {
    if (g_overlayVisible && EqualRect(&g_overlayMon, &mi.rcMonitor) &&
        EqualRect(&g_overlayWork, &mi.rcWork)) {
        return false;
    }

    g_overlayMon = mi.rcMonitor;
    g_overlayWork = mi.rcWork;
    SetWindowPos(g_overlay, HWND_TOPMOST, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_overlayVisible = true;
    return true;
}

void UpdateOverlay() {
    int layout = LayoutForMask(CurrentDragMask());
    if (!g_overlay || !IsWindow(g_dragHwnd) || layout < 0) {
        HideOverlay();
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
        return;
    }

    bool changed = ShowOverlayOn(mi);
    if (layout != g_overlayLayout) {
        g_overlayLayout = layout;
        changed = true;
    }

    // Zone closest to the cursor (distance 0 when the cursor is inside).
    const auto& zones = g_layouts[layout].zones;
    int best = 0;
    long long bestDist = -1;
    for (size_t i = 0; i < zones.size(); i++) {
        RECT z = ZoneRect(g_overlayWork, zones[i]);
        long long dx = std::max<long long>(
            {(long long)z.left - pt.x, 0LL, (long long)pt.x - z.right});
        long long dy = std::max<long long>(
            {(long long)z.top - pt.y, 0LL, (long long)pt.y - z.bottom});
        long long dist = dx * dx + dy * dy;
        if (bestDist < 0 || dist < bestDist) {
            bestDist = dist;
            best = (int)i;
        }
    }

    if (best != g_activeZone) {
        g_activeZone = best;
        changed = true;
    }

    if (changed) {
        InvalidateRect(g_overlay, nullptr, FALSE);
    }
}

// Briefly shows the active hotkey layout on the foreground window's monitor.
void FlashActiveLayout() {
    if (!g_overlay || g_dragHwnd) {
        return;
    }

    HWND fg = GetForegroundWindow();
    MONITORINFO mi = {sizeof(mi)};
    if (!fg || !GetMonitorInfoW(MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY),
                                &mi)) {
        return;
    }

    ShowOverlayOn(mi);
    g_overlayLayout = g_active;
    g_activeZone = -1;
    g_flashUntil = GetTickCount64() + kFlashMs;
    InvalidateRect(g_overlay, nullptr, FALSE);
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG,
                           DWORD,
                           DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW) {
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        if (!g_settings.dragOverlay || !IsManageable(hwnd)) {
            return;
        }

        // Ignore resizing: only show zones when moving (not dragging an edge).
        POINT pt;
        GetCursorPos(&pt);
        DWORD_PTR hit = 0;
        if (SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0,
                                MAKELPARAM((short)pt.x, (short)pt.y),
                                SMTO_ABORTIFHUNG, 100, &hit) &&
            hit >= HTLEFT && hit <= HTBOTTOMRIGHT) {
            return;
        }
        g_dragHwnd = hwnd;
        g_flashUntil = 0;
    } else if (event == EVENT_SYSTEM_MOVESIZEEND && hwnd == g_dragHwnd) {
        if (g_overlayVisible && g_activeZone >= 0 &&
            g_overlayLayout < (int)g_layouts.size() &&
            g_activeZone < (int)g_layouts[g_overlayLayout].zones.size()) {
            g_dropHwnd = hwnd;
            g_dropRect = ZoneRect(g_overlayWork,
                                  g_layouts[g_overlayLayout].zones[g_activeZone]);
            g_dropTick = GetTickCount64();
        }
        HideOverlay();
        g_dragHwnd = nullptr;
    }
}

void OnTimer() {
    ULONGLONG now = GetTickCount64();

    if (g_dropHwnd && now - g_dropTick >= kDropDelayMs) {
        HWND hwnd = g_dropHwnd;
        g_dropHwnd = nullptr;
        if (IsWindow(hwnd)) {
            PlaceWindow(hwnd, g_dropRect);
        }
    }

    if (g_dragHwnd) {
        UpdateOverlay();
    } else if (g_overlayVisible && now >= g_flashUntil) {
        HideOverlay();
    }
}

// ---- Hotkeys ----

void OnHotkey(int dx, int dy) {
    HWND hwnd = GetForegroundWindow();
    if (!IsManageable(hwnd)) {
        return;
    }

    RECT fr;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &fr,
                                     sizeof(fr)))) {
        return;
    }

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                         &mi)) {
        return;
    }

    const auto& zones = g_layouts[g_active].zones;
    bool aligned;
    int zone = FindCurrentZone(zones, mi.rcWork, fr, &aligned);
    if (aligned) {
        zone = FindNeighbor(zones, mi.rcWork, zone, dx, dy);
    }

    Wh_Log(L"Layout %d zone %d aligned=%d", g_active + 1, zone, (int)aligned);
    PlaceWindow(hwnd, ZoneRect(mi.rcWork, zones[zone]));
}

void OnCycleLayout() {
    g_active = (g_active + 1) % (int)g_layouts.size();
    Wh_Log(L"Active layout: %d", g_active + 1);
    FlashActiveLayout();
}

bool g_hotkeysRegistered = false;

void UnregisterHotkeys() {
    if (!g_hotkeysRegistered) {
        return;
    }
    for (int id = kHotkeyLeft; id <= kHotkeyCycle; id++) {
        UnregisterHotKey(nullptr, id);
    }
    g_hotkeysRegistered = false;
}

void RegisterHotkeys() {
    UnregisterHotkeys();

    UINT mods = g_settings.modifiers | MOD_NOREPEAT;
    struct {
        int id;
        UINT vk;
    } keys[] = {{kHotkeyLeft, VK_LEFT},
                {kHotkeyRight, VK_RIGHT},
                {kHotkeyUp, VK_UP},
                {kHotkeyDown, VK_DOWN},
                {kHotkeyCycle, VK_SPACE}};
    for (const auto& key : keys) {
        if (!RegisterHotKey(nullptr, key.id, mods, key.vk)) {
            Wh_Log(L"RegisterHotKey failed for vk=%u: %u", key.vk,
                   GetLastError());
        }
    }
    g_hotkeysRegistered = true;
}

UINT ParseHotkeyModifiers(PCWSTR value) {
    if (!wcscmp(value, L"win_alt")) return MOD_WIN | MOD_ALT;
    if (!wcscmp(value, L"ctrl_win")) return MOD_CONTROL | MOD_WIN;
    if (!wcscmp(value, L"ctrl_shift_alt"))
        return MOD_CONTROL | MOD_SHIFT | MOD_ALT;
    return MOD_CONTROL | MOD_ALT;
}

// Called before the worker thread starts, and afterwards only from it.
void LoadSettings() {
    g_settings.outerGap = std::max(Wh_GetIntSetting(L"outerGap"), 0);
    g_settings.innerGap = std::max(Wh_GetIntSetting(L"innerGap"), 0);
    g_settings.dragOverlay = Wh_GetIntSetting(L"dragOverlay") != 0;

    PCWSTR modifier = Wh_GetStringSetting(L"modifier");
    g_settings.modifiers = ParseHotkeyModifiers(modifier);
    Wh_FreeStringSetting(modifier);

    g_layouts.clear();
    for (int i = 1; i <= kLayoutSlots; i++) {
        PCWSTR spec = Wh_GetStringSetting(L"layout%d", i);
        PCWSTR mod = Wh_GetStringSetting(L"layout%dModifier", i);

        if (spec && !IsBlank(spec)) {
            Layout layout;
            layout.dragMask = ParseDragMask(mod);
            if (!ParseLayout(spec, layout.zones)) {
                Wh_Log(L"Layout %d is invalid, ignored", i);
            } else if (LayoutForMask(layout.dragMask) >= 0) {
                Wh_Log(L"Layout %d reuses a drag modifier, ignored", i);
            } else {
                Wh_Log(L"Layout %d: %d zones", i, (int)layout.zones.size());
                g_layouts.push_back(std::move(layout));
            }
        }

        Wh_FreeStringSetting(spec);
        Wh_FreeStringSetting(mod);
    }

    if (g_layouts.empty()) {
        Wh_Log(L"No valid layout, using 2x1 with Shift");
        Layout layout;
        layout.dragMask = MOD_SHIFT;
        ParseGrid(L"2x1", layout.zones);
        g_layouts.push_back(std::move(layout));
    }
    g_active = 0;
    g_overlayLayout = 0;
}

DWORD WINAPI WorkerThread(LPVOID) {
    // Physical pixels, same space as the monitor work area.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_ready);

    RegisterHotkeys();
    CreateOverlay();

    const DWORD hookFlags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS;
    HWINEVENTHOOK hook =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                        nullptr, WinEventProc, 0, 0, hookFlags);
    UINT_PTR timer = SetTimer(nullptr, 0, kTimerMs, nullptr);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER && msg.hwnd == nullptr &&
            msg.wParam == timer) {
            OnTimer();
        } else if (msg.message == WM_HOTKEY) {
            switch (msg.wParam) {
                case kHotkeyLeft:
                    OnHotkey(-1, 0);
                    break;
                case kHotkeyRight:
                    OnHotkey(1, 0);
                    break;
                case kHotkeyUp:
                    OnHotkey(0, -1);
                    break;
                case kHotkeyDown:
                    OnHotkey(0, 1);
                    break;
                case kHotkeyCycle:
                    OnCycleLayout();
                    break;
            }
        } else if (msg.message == kMsgReload && msg.hwnd == nullptr) {
            HideOverlay();
            LoadSettings();
            RegisterHotkeys();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    KillTimer(nullptr, timer);
    if (hook) UnhookWinEvent(hook);
    if (g_overlay) {
        DestroyWindow(g_overlay);
        g_overlay = nullptr;
    }
    UnregisterClassW(kOverlayClass, GetModuleHandleW(nullptr));
    UnregisterHotkeys();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        return FALSE;
    }
    WaitForSingleObject(g_ready, 5000);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    // Reloaded on the worker thread, which owns the layout list.
    if (g_thread) {
        PostThreadMessageW(g_threadId, kMsgReload, 0, 0);
    }
}

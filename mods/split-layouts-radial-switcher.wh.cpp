// ==WindhawkMod==
// @id              split-layouts-radial-switcher
// @name            Split Layouts & Radial Switcher
// @description     Drag windows to the top or the screen edges to arrange them in split layouts, and switch between windows and split groups with a radial Alt+Tab wheel
// @version         1.0
// @author          khorkovenko
// @github          https://github.com/khorkovenko
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshcore -lgdi32 -lgdiplus -lshell32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Split Layouts & Radial Switcher
Two features that work together. Each one can be turned off in the settings.

## Split layouts
Drag a window to the top center of the screen. A layouts bar appears with five
layouts: top/bottom halves, left/right halves, two thirds + one third, half +
two quarters and four quarters. The top half is lightly highlighted as a hint.
Hover a zone to select it: a preview shows the area the window will be
stretched to, and releasing the mouse places the window there. Releasing it
without a selected zone leaves the window where it was dropped.

Moving a window to a screen edge or corner also opens the bar and selects the
matching zone: the left or right edge selects the left or right half, the top or
bottom edge the top or bottom half, and a corner the matching quarter.

After the first window is placed, the next empty zone of the same layout shows
your other open windows. Click one to place it there. The X on a window closes
that window. Press Esc, click the X in the top right corner, click empty space
or switch to another window to stop choosing; all windows stay as they are.

With "Turn off Windows edge snapping while dragging" enabled, Windows doesn't
maximize or snap windows dragged to the screen edges, and doesn't show its gray
previews. The mod turns this off for the current session only, with
`SystemParametersInfo(SPI_SETDOCKMOVING)` without saving it, and turns it back
on when the setting is turned off or the mod is disabled. Nothing is written to
the registry.

The built-in Windows snap layouts bar appears in the same place when a window
is dragged to the top of the screen. To use only this mod's bar, turn off "Show
snap layouts when I drag a window to the top of my screen" in Settings > System
> Multitasking > Snap windows. The mod doesn't change this Windows setting.

## Radial Alt+Tab
Replaces the Alt+Tab switcher with a full-screen view split into two halves:

- **Top half:** a large live preview of the selected window, with its number
  and full title.
- **Bottom half:** a wheel with the icon and number of each open window, and its
  title outside the wheel. Each window keeps its place on the wheel; only the
  numbers change, following the Alt+Tab order. A thumbstick in the center moves
  with the mouse and springs back to the center when the mouse stops, and a
  pointer marks the selected window.

Windows that fill one of the split layouts together, for example two windows
side by side or one above the other, appear as **one item**: the wheel shows
their icons side by side, the titles are joined with `|`, the preview shows the
whole arrangement, and switching to the item brings all of them back to the
front, also with a quick Alt+Tab. Groups are recognized from the window
positions, so they work however the windows were arranged, and a group splits
up as soon as one of its windows is moved, resized or minimized.

- Hold **Alt** and press **Tab**. If you release Alt quickly, you switch to the
  previous window without seeing the wheel, like the default Alt+Tab.
- Keep holding Alt and the view opens. While it's open, the mouse cursor is
  hidden and doesn't move. Push the mouse toward a window to select it.
- Release **Alt** to switch to the selected window. No click is needed.
- Tab / Shift+Tab, the arrow keys and the mouse wheel also change the selection.
  Other keys don't reach the app while Alt+Tab is held.
- Left click switches right away. Right click closes the selected window (or all
  windows of a group); if a window has unsaved changes, the app asks first.
  **Esc** cancels.

The bar, the previews and the Alt+Tab view appear instantly, without animations.
Dragging feedback and the thumbstick are updated once per screen refresh, at the
refresh rate of the monitor in use (for example 60, 120 or 144 Hz).

## Compared with Windows and other mods
- Windows 11 snap layouts offer a top/bottom split only on portrait monitors.
  This mod offers it on every monitor, together with the other layouts, and
  also from the screen edges and corners. Its chooser can close windows, too.
- Windows 11 shows snap groups in its own Alt+Tab only for windows snapped by
  Windows. The radial view here recognizes any windows that fill one of the
  layouts above, however they were arranged, and restores the whole arrangement.
- Compared with the Radial Window Switcher mod, the radial view is a full-screen
  split view with a large preview, keeps every window in a fixed place on the
  wheel with Alt+Tab numbers, is steered with the mouse like a thumbstick and
  treats split groups as one item.

## Limitations
Windows doesn't let programs move or close windows of programs running as
administrator, so such windows may not be placed by the split layouts or closed
with a right click. When an administrator window is in the foreground, Windows
may not pass the keyboard to this mod, and the default Alt+Tab appears instead.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- splitLayouts: true
  $name: Split layouts
  $description: Show the layouts bar when a window is dragged to the top or the screen edges.
- snapAssist: true
  $name: Offer windows for the remaining zones
- noEdgeSnap: true
  $name: Turn off Windows edge snapping while dragging
  $description: >-
    Stops Windows from maximizing or snapping windows dragged to the screen
    edges, and from showing its previews. Applies to the current session only
    and is turned back on when this setting is off or the mod is disabled.
- radialSwitcher: true
  $name: Radial Alt+Tab
  $description: Replace the Alt+Tab switcher with the radial view.
- groupSplitWindows: true
  $name: Show split windows as one Alt+Tab item
- openDelay: 150
  $name: Delay before the Alt+Tab view opens (ms)
  $description: >-
    Releasing Alt before this delay switches to the previous window without
    showing the view, like the default Alt+Tab. 0 opens the view immediately.
- maxWindows: 12
  $name: Maximum number of items on the wheel
  $description: From 2 to 16. Larger values use 16.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <dwmapi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <shellscalingapi.h>
#include <gdiplus.h>
#include <sddl.h>

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

namespace common {

struct Zone {
    BYTE x0, y0, x1, y1;
};

struct Layout {
    int n;
    Zone z[4];
};

const Layout g_layouts[] = {
    {2, {{0, 0, 6, 3}, {0, 3, 6, 6}}},
    {2, {{0, 0, 3, 6}, {3, 0, 6, 6}}},
    {2, {{0, 0, 4, 6}, {4, 0, 6, 6}}},
    {3, {{0, 0, 3, 6}, {3, 0, 6, 3}, {3, 3, 6, 6}}},
    {4, {{0, 0, 3, 3}, {3, 0, 6, 3}, {0, 3, 3, 6}, {3, 3, 6, 6}}},
};
constexpr int NL = sizeof(g_layouts) / sizeof(g_layouts[0]);

RECT ZoneIn(RECT a, Zone z) {
    int w = a.right - a.left, h = a.bottom - a.top;
    return {a.left + w * z.x0 / 6, a.top + h * z.y0 / 6, a.left + w * z.x1 / 6,
            a.top + h * z.y1 / 6};
}

int MonitorDpi(HMONITOR mon) {
    UINT dx, dy;
    return SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) ? dx : 96;
}

bool StartThread(LPTHREAD_START_ROUTINE proc, HANDLE& thread, DWORD& threadId) {
    thread = CreateThread(nullptr, 0, proc, nullptr, 0, &threadId);
    return thread != nullptr;
}

void StopThread(HANDLE thread, DWORD threadId) {
    while (!PostThreadMessage(threadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(thread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

int RefreshRate(HMONITOR mon) {
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    DEVMODEW dm{};
    dm.dmSize = sizeof(dm);
    if (GetMonitorInfoW(mon, &mi) &&
        EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
        dm.dmDisplayFrequency > 1)
        return dm.dmDisplayFrequency;
    return 60;
}

struct FrameClock {
    HANDLE timer = nullptr;
    LONGLONG interval = 10000000 / 60;
    bool active = false;

    void Create() {
        timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                       TIMER_ALL_ACCESS);
        if (!timer)
            timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }

    void Destroy() {
        Stop();
        if (timer)
            CloseHandle(timer);
        timer = nullptr;
    }

    void Arm() {
        LARGE_INTEGER due;
        due.QuadPart = -interval;
        if (timer)
            SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE);
    }

    void Start(HMONITOR mon) {
        interval = 10000000 / RefreshRate(mon);
        active = true;
        Arm();
    }

    void Stop() {
        active = false;
        if (timer)
            CancelWaitableTimer(timer);
    }
};

template <typename Callback>
void RunMessageLoop(FrameClock& clock, Callback onFrame) {
    for (;;) {
        DWORD r = MsgWaitForMultipleObjectsEx(clock.timer ? 1 : 0, &clock.timer, INFINITE,
                                              QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (clock.timer && r == WAIT_OBJECT_0 && clock.active) {
            clock.Arm();
            onFrame();
        }
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

}  // namespace common

constexpr int MAXMEMBERS = 4;

namespace split {

using namespace common;

constexpr int MAXWIN = 12;
constexpr DWORD WINDOW_CORNER_PREFERENCE = 33;
constexpr int CORNER_ROUND = 2;

struct {
    std::atomic<bool> enabled;
    std::atomic<bool> noEdgeSnap;
    std::atomic<bool> assist;
} settings;

HANDLE g_thread;
DWORD g_threadId;
HWND g_bar, g_preview, g_chooser, g_hwnd;
BOOL g_oldDock = TRUE;
bool g_dockChanged;
HFONT g_font;
RECT g_work;
POINT g_start;
UINT_PTR g_timer;
bool g_tracking;
FrameClock g_frames;
int g_ticks, g_dpi = 96, g_hl = -1, g_hz = -1, g_layout, g_zone, g_done, g_nused;
int g_count, g_hover = -1;
DWORD g_shownAt;
bool g_force;
HWND g_used[MAXWIN], g_wins[MAXWIN];
HTHUMBNAIL g_thumbs[MAXWIN];
RECT g_tiles[MAXWIN];

int Sc(int v) {
    return MulDiv(v, g_dpi, 96);
}

RECT GroupRect(int i) {
    int p = Sc(10), gw = Sc(96), gh = Sc(60), x = p + i * (gw + p);
    return {x, p, x + gw, p + gh};
}

RECT CellRect(int l, int z) {
    RECT g = GroupRect(l);
    InflateRect(&g, -Sc(3), -Sc(3));
    RECT c = ZoneIn(g, g_layouts[l].z[z]);
    InflateRect(&c, -Sc(2), -Sc(2));
    return c;
}

void Fill(HDC dc, RECT r, COLORREF c) {
    HBRUSH b = CreateSolidBrush(c);
    FillRect(dc, &r, b);
    DeleteObject(b);
}

bool Place(HWND hwnd, RECT z) {
    if (IsHungAppWindow(hwnd))
        return false;
    if (IsZoomed(hwnd) || IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);
    RECT wr, fr;
    GetWindowRect(hwnd, &wr);
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &fr, sizeof(fr))))
        fr = wr;
    int l = fr.left - wr.left, t = fr.top - wr.top;
    int r = wr.right - fr.right, b = wr.bottom - fr.bottom;
    RECT before = wr;
    SetWindowPos(hwnd, nullptr, z.left - l, z.top - t, z.right - z.left + l + r,
                 z.bottom - z.top + t + b, SWP_NOZORDER | SWP_NOACTIVATE);
    GetWindowRect(hwnd, &wr);
    return !EqualRect(&wr, &before) || EqualRect(&fr, &z);
}

void HideChooser() {
    UnregisterHotKey(g_chooser, 1);
    for (int i = 0; i < g_count; i++)
        if (g_thumbs[i])
            DwmUnregisterThumbnail(g_thumbs[i]);
    g_count = 0;
    g_hover = -1;
    ShowWindow(g_chooser, SW_HIDE);
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    for (int i = 0; i < g_nused; i++)
        if (hwnd == g_used[i])
            return TRUE;
    WCHAR cls[32] = L"";
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    GetClassName(hwnd, cls, 32);
    if (!IsWindowVisible(hwnd) || cloaked || IsHungAppWindow(hwnd) || GetWindow(hwnd, GW_OWNER) ||
        (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        !GetWindowTextLength(hwnd) || !lstrcmpW(cls, L"Progman"))
        return TRUE;
    g_wins[g_count++] = hwnd;
    return g_count < MAXWIN;
}

void ShowChooser(RECT b) {
    HideChooser();
    EnumWindows(EnumProc, 0);
    if (!g_count)
        return;

    g_dpi = MonitorDpi(MonitorFromRect(&g_work, MONITOR_DEFAULTTONEAREST));
    if (g_font)
        DeleteObject(g_font);
    g_font = CreateFont(-Sc(14), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                        CLEARTYPE_QUALITY, 0, L"Segoe UI");

    int w = b.right - b.left, h = b.bottom - b.top, p = Sc(24), top = Sc(56);
    int cols = 1, tw = 0;
    for (int c = 1; c <= g_count; c++) {
        int r = (g_count + c - 1) / c;
        int byW = (w - p * (c + 1)) / c, byH = (h - top - p * (r + 1)) / r * 16 / 10;
        int s = byW < byH ? byW : byH;
        if (s > tw) {
            tw = s;
            cols = c;
        }
    }
    int rows = (g_count + cols - 1) / cols, th = tw * 10 / 16;
    int x0 = (w - cols * tw - (cols - 1) * p) / 2;
    int y0 = top + (h - top - rows * th - (rows - 1) * p) / 2;

    SetWindowPos(g_chooser, HWND_TOPMOST, b.left, b.top, w, h, SWP_NOACTIVATE);

    for (int i = 0; i < g_count; i++) {
        int x = x0 + i % cols * (tw + p), y = y0 + i / cols * (th + p);
        g_tiles[i] = {x, y, x + tw, y + th};
        g_thumbs[i] = nullptr;
        if (FAILED(DwmRegisterThumbnail(g_chooser, g_wins[i], &g_thumbs[i])))
            continue;
        RECT t{x + Sc(8), y + Sc(36), x + tw - Sc(8), y + th - Sc(8)};
        int aw = t.right - t.left, ah = t.bottom - t.top;
        SIZE s{};
        DwmQueryThumbnailSourceSize(g_thumbs[i], &s);
        if (s.cx > 0 && s.cy > 0 && s.cx * ah > s.cy * aw) {
            int nh = aw * s.cy / s.cx;
            t.top += (ah - nh) / 2;
            t.bottom = t.top + nh;
        } else if (s.cx > 0 && s.cy > 0) {
            int nw = ah * s.cx / s.cy;
            t.left += (aw - nw) / 2;
            t.right = t.left + nw;
        }
        DWM_THUMBNAIL_PROPERTIES tp{};
        tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE;
        tp.rcDestination = t;
        tp.fVisible = TRUE;
        DwmUpdateThumbnailProperties(g_thumbs[i], &tp);
    }

    g_shownAt = GetTickCount();
    RegisterHotKey(g_chooser, 1, MOD_NOREPEAT, VK_ESCAPE);
    ShowWindow(g_chooser, SW_SHOW);
    SetForegroundWindow(g_chooser);
    InvalidateRect(g_chooser, nullptr, TRUE);
}

void NextChooser() {
    const Layout& L = g_layouts[g_layout];
    for (g_zone = 0; g_zone < L.n && (g_done >> g_zone & 1); g_zone++) {
    }
    if (g_zone < L.n)
        ShowChooser(ZoneIn(g_work, L.z[g_zone]));
}

RECT CloseRect(int i) {
    if (i < 0) {
        RECT rc;
        GetClientRect(g_chooser, &rc);
        int s = Sc(32), m = Sc(12);
        return {rc.right - m - s, m, rc.right - m, m + s};
    }
    int s = Sc(24), m = Sc(6);
    RECT& t = g_tiles[i];
    return {t.right - m - s, t.top + m, t.right - m, t.top + m + s};
}

int HitTile(LPARAM lp) {
    POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
    RECT r = CloseRect(-1);
    if (PtInRect(&r, pt))
        return 2 * MAXWIN;
    for (int i = 0; i < g_count; i++) {
        r = CloseRect(i);
        if (PtInRect(&r, pt))
            return MAXWIN + i;
        if (PtInRect(&g_tiles[i], pt))
            return i;
    }
    return -1;
}

void DrawClose(HDC dc, RECT r, bool hot) {
    Fill(dc, r, hot ? RGB(196, 43, 28) : RGB(72, 72, 72));
    HPEN pen = CreatePen(PS_SOLID, Sc(2), RGB(255, 255, 255));
    HGDIOBJ old = SelectObject(dc, pen);
    int d = (r.right - r.left) * 3 / 10;
    MoveToEx(dc, r.left + d, r.top + d, nullptr);
    LineTo(dc, r.right - d, r.bottom - d);
    MoveToEx(dc, r.right - d, r.top + d, nullptr);
    LineTo(dc, r.left + d, r.bottom - d);
    SelectObject(dc, old);
    DeleteObject(pen);
}

void PaintBar(HDC dc, RECT rc) {
    Fill(dc, rc, RGB(243, 243, 243));
    for (int l = 0; l < NL; l++) {
        Fill(dc, GroupRect(l), RGB(228, 228, 228));
        for (int z = 0; z < g_layouts[l].n; z++)
            Fill(dc, CellRect(l, z),
                 l == g_hl && z == g_hz            ? RGB(0, 120, 215)
                 : g_hl < 0 && l == 0 && z == 0 ? RGB(160, 200, 235)
                                                   : RGB(200, 200, 200));
    }
}

void PaintChooser(HDC dc, RECT rc) {
    Fill(dc, rc, RGB(32, 32, 32));
    SelectObject(dc, g_font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    for (int i = 0; i < g_count; i++) {
        Fill(dc, g_tiles[i], i == g_hover ? RGB(0, 95, 184) : RGB(56, 56, 56));
        WCHAR title[256] = L"";
        GetWindowText(g_wins[i], title, 256);
        RECT tr = g_tiles[i];
        tr.left += Sc(10);
        tr.right -= Sc(36);
        tr.bottom = tr.top + Sc(36);
        DrawText(dc, title, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        DrawClose(dc, CloseRect(i), g_hover == MAXWIN + i);
    }
    DrawClose(dc, CloseRect(-1), g_hover == 2 * MAXWIN);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (hwnd == g_bar)
                PaintBar(dc, rc);
            else
                PaintChooser(dc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEMOVE: {
            int i = HitTile(lp);
            if (i != g_hover) {
                g_hover = i;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            int i = HitTile(lp);
            if (i >= MAXWIN && i < 2 * MAXWIN) {
                HWND victim = g_wins[i - MAXWIN];
                PostMessage(victim, WM_CLOSE, 0, 0);
                if (g_nused < MAXWIN)
                    g_used[g_nused++] = victim;
                ShowChooser(ZoneIn(g_work, g_layouts[g_layout].z[g_zone]));
                return 0;
            }
            HWND pick = i >= 0 && i < MAXWIN ? g_wins[i] : nullptr;
            HideChooser();
            if (!pick)
                return 0;
            RECT zone = ZoneIn(g_work, g_layouts[g_layout].z[g_zone]);
            if (Place(pick, zone))
                g_done |= 1 << g_zone;
            if (g_nused < MAXWIN)
                g_used[g_nused++] = pick;
            NextChooser();
            if (!IsWindowVisible(g_chooser))
                SetForegroundWindow(pick);
            return 0;
        }
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE)
                HideChooser();
            return 0;
        case WM_HOTKEY:
            HideChooser();
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE)
                HideChooser();
            break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void CALLBACK ApplyProc(HWND, UINT, UINT_PTR, DWORD) {
    bool zoomed = IsZoomed(g_hwnd);
    ++g_ticks;
    if (g_force ? g_ticks < 6 : !zoomed && g_ticks < 20)
        return;
    KillTimer(nullptr, g_timer);
    g_timer = 0;
    RECT zone = ZoneIn(g_work, g_layouts[g_layout].z[g_zone]);
    if ((!g_force && !zoomed) || !Place(g_hwnd, zone))
        return;
    g_done = 1 << g_zone;
    g_used[0] = g_hwnd;
    g_nused = 1;
    if (settings.assist)
        NextChooser();
}

RECT g_barRect, g_previewRect;
bool g_barShown, g_previewShown;

void ShowAt(HWND hwnd, HWND after, RECT r, RECT& last, bool& shown) {
    if (shown && EqualRect(&r, &last))
        return;
    SetWindowPos(hwnd, after, r.left, r.top, r.right - r.left, r.bottom - r.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    last = r;
    shown = true;
}

void HideAt(HWND hwnd, bool& shown) {
    if (shown)
        ShowWindow(hwnd, SW_HIDE);
    shown = false;
}

void TrackFrame() {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(mon, &mi);
    g_dpi = MonitorDpi(mon);
    g_work = mi.rcWork;

    RECT last = GroupRect(NL - 1);
    int bw = last.right + Sc(10), bh = last.bottom + Sc(10);
    int bx = (mi.rcMonitor.left + mi.rcMonitor.right - bw) / 2, by = mi.rcWork.top + Sc(8);
    int dx = pt.x - g_start.x, dy = pt.y - g_start.y, d = GetSystemMetrics(SM_CXDRAG);
    bool moved = dx * dx + dy * dy >= d * d;
    RECT m = mi.rcMonitor;
    int e = Sc(6), corner = Sc(40);
    auto free = [&](int ox, int oy) {
        return moved && !MonitorFromPoint({pt.x + ox, pt.y + oy}, MONITOR_DEFAULTTONULL);
    };
    bool L = pt.x <= m.left + e && free(-2 * e, 0);
    bool R = pt.x >= m.right - 1 - e && free(2 * e, 0);
    bool T = pt.y <= m.top + e && free(0, -2 * e);
    bool B = pt.y >= m.bottom - 1 - e && free(0, 2 * e);
    int h = L ? 0 : R ? 1 : -1, v = T ? 0 : B ? 1 : -1;
    if (h < 0 && v >= 0)
        h = pt.x <= m.left + corner ? 0 : pt.x >= m.right - corner ? 1 : -1;
    if (v < 0 && h >= 0 && (L || R))
        v = pt.y <= m.top + corner ? 0 : pt.y >= m.bottom - corner ? 1 : -1;

    int hl = -1, hz = -1;
    if (h >= 0 && v >= 0) {
        hl = NL - 1;
        hz = v * 2 + h;
    } else if (h >= 0) {
        hl = 1;
        hz = h;
    } else if (v >= 0) {
        hl = 0;
        hz = v;
    }
    bool show = hl >= 0 ||
                (pt.y < by + bh + Sc(80) && pt.x > bx - Sc(100) && pt.x < bx + bw + Sc(100));
    for (int l = 0; show && l < NL; l++)
        for (int z = 0; z < g_layouts[l].n; z++) {
            RECT c = CellRect(l, z);
            OffsetRect(&c, bx, by);
            InflateRect(&c, Sc(2), Sc(2));
            if (PtInRect(&c, pt)) {
                hl = l;
                hz = z;
            }
        }

    if (show)
        ShowAt(g_bar, HWND_TOPMOST, {bx, by, bx + bw, by + bh}, g_barRect, g_barShown);
    else
        HideAt(g_bar, g_barShown);

    if (hl != g_hl || hz != g_hz) {
        g_hl = hl;
        g_hz = hz;
        InvalidateRect(g_bar, nullptr, FALSE);
    }

    if (hl >= 0)
        ShowAt(g_preview, g_hwnd, ZoneIn(g_work, g_layouts[hl].z[hz]), g_previewRect,
               g_previewShown);
    else
        HideAt(g_preview, g_previewShown);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                           LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd)
        return;

    if (event == EVENT_SYSTEM_FOREGROUND) {
        if (hwnd != g_chooser && GetTickCount() - g_shownAt > 400)
            HideChooser();
        return;
    }

    POINT pt;
    GetCursorPos(&pt);

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        if (!settings.enabled)
            return;
        DWORD_PTR hit = HTCAPTION;
        SendMessageTimeout(hwnd, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y),
                           SMTO_ABORTIFHUNG, 50, &hit);
        if (hit >= HTLEFT && hit <= HTBOTTOMRIGHT)
            return;
        HideChooser();
        if (g_timer) {
            KillTimer(nullptr, g_timer);
            g_timer = 0;
        }
        g_hwnd = hwnd;
        g_start = pt;
        g_hl = g_hz = -1;
        InvalidateRect(g_bar, nullptr, FALSE);
        g_tracking = true;
        g_frames.Start(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
        return;
    }

    if (!g_tracking || hwnd != g_hwnd)
        return;
    g_frames.Stop();
    g_tracking = false;
    HideAt(g_bar, g_barShown);
    HideAt(g_preview, g_previewShown);

    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    int dx = pt.x - g_start.x, dy = pt.y - g_start.y, d = GetSystemMetrics(SM_CXDRAG);
    bool edge = pt.y <= mi.rcMonitor.top + 1 && dx * dx + dy * dy >= d * d;
    g_force = g_hl >= 0 && dx * dx + dy * dy >= d * d;
    if (!g_force && !edge)
        return;

    g_work = mi.rcWork;
    g_layout = g_force ? g_hl : 0;
    g_zone = g_force ? g_hz : 0;
    g_ticks = 0;
    g_timer = SetTimer(nullptr, 0, 25, ApplyProc);
}

DWORD WINAPI ThreadProc(LPVOID) {
    HINSTANCE inst;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)ThreadProc, &inst);
    HBRUSH brush = CreateSolidBrush(RGB(0, 120, 215));
    WNDCLASS pc{0, DefWindowProc, 0, 0, inst,
                nullptr, nullptr, brush, nullptr, L"SplitLayoutsPreview"};
    WNDCLASS uc{0, WndProc, 0, 0, inst,
                nullptr, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, L"SplitLayoutsUi"};
    RegisterClass(&pc);
    RegisterClass(&uc);

    g_preview = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        pc.lpszClassName, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    SetLayeredWindowAttributes(g_preview, 0, 80, LWA_ALPHA);

    g_bar = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        uc.lpszClassName, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    g_chooser = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, uc.lpszClassName, nullptr,
                               WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    SetLayeredWindowAttributes(g_bar, 0, 245, LWA_ALPHA);
    int round = CORNER_ROUND;
    DwmSetWindowAttribute(g_bar, WINDOW_CORNER_PREFERENCE, &round, sizeof(round));
    DwmSetWindowAttribute(g_preview, WINDOW_CORNER_PREFERENCE, &round, sizeof(round));
    BOOL noAnim = TRUE;
    HWND windows[] = {g_bar, g_preview, g_chooser};
    for (HWND w : windows)
        DwmSetWindowAttribute(w, DWMWA_TRANSITIONS_FORCEDISABLED, &noAnim, sizeof(noAnim));

    HWINEVENTHOOK moveHook =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK fgHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    g_frames.Create();
    RunMessageLoop(g_frames, TrackFrame);

    g_frames.Destroy();
    if (g_timer)
        KillTimer(nullptr, g_timer);
    g_timer = 0;
    g_tracking = false;
    g_barShown = g_previewShown = false;
    UnhookWinEvent(moveHook);
    UnhookWinEvent(fgHook);
    HideChooser();
    DestroyWindow(g_preview);
    DestroyWindow(g_bar);
    DestroyWindow(g_chooser);
    UnregisterClass(pc.lpszClassName, inst);
    UnregisterClass(uc.lpszClassName, inst);
    DeleteObject(brush);
    if (g_font)
        DeleteObject(g_font);
    g_font = nullptr;
    return 0;
}

void UserValueName(PCWSTR name, WCHAR* out, int len) {
    static WCHAR sid[100];
    if (!sid[0]) {
        HANDLE token;
        BYTE buf[256];
        DWORD size;
        LPWSTR str = nullptr;
        lstrcpynW(sid, L"default", 100);
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &size) &&
                ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &str)) {
                lstrcpynW(sid, str, 100);
                LocalFree(str);
            }
            CloseHandle(token);
        }
    }
    _snwprintf_s(out, len, _TRUNCATE, L"%s_%s", name, sid);
}

int GetUserInt(PCWSTR name, int def) {
    WCHAR full[160];
    UserValueName(name, full, 160);
    return Wh_GetIntValue(full, def);
}

void SetUserInt(PCWSTR name, int value) {
    WCHAR full[160];
    UserValueName(name, full, 160);
    Wh_SetIntValue(full, value);
}

void RestoreDocking(BOOL value) {
    BOOL current = TRUE;
    SystemParametersInfo(SPI_GETDOCKMOVING, 0, &current, 0);
    if (!current)
        SystemParametersInfo(SPI_SETDOCKMOVING, 0, (PVOID)(INT_PTR)value, 0);
    SetUserInt(L"dockingOff", 0);
}

void RecoverDocking() {
    if (GetUserInt(L"dockingOff", 0))
        RestoreDocking(GetUserInt(L"savedDocking", TRUE));
}

void SetDocking(bool off) {
    if (off && !g_dockChanged) {
        SystemParametersInfo(SPI_GETDOCKMOVING, 0, &g_oldDock, 0);
        SetUserInt(L"savedDocking", g_oldDock ? 1 : 0);
        SetUserInt(L"dockingOff", 1);
        SystemParametersInfo(SPI_SETDOCKMOVING, 0, (PVOID)FALSE, 0);
        g_dockChanged = true;
    } else if (!off && g_dockChanged) {
        RestoreDocking(g_oldDock);
        g_dockChanged = false;
    }
}

void LoadSettings() {
    settings.enabled = Wh_GetIntSetting(L"splitLayouts");
    settings.assist = Wh_GetIntSetting(L"snapAssist");
    settings.noEdgeSnap = Wh_GetIntSetting(L"noEdgeSnap");
    SetDocking(settings.enabled && settings.noEdgeSnap);
}

bool Start() {
    return StartThread(ThreadProc, g_thread, g_threadId);
}

void Stop() {
    StopThread(g_thread, g_threadId);
    SetDocking(false);
}

}  // namespace split

namespace radial {

using namespace common;

constexpr int MAXWIN = 16;
constexpr float PI = 3.14159265f;
constexpr WORD VK_MASK = 0xE8;
constexpr UINT WM_APP_START = WM_APP + 1;
constexpr UINT WM_APP_STEP = WM_APP + 2;
constexpr UINT WM_APP_END = WM_APP + 3;
constexpr UINT WM_APP_JOY = WM_APP + 4;
constexpr UINT WM_APP_MOUSEHOOK = WM_APP + 5;

struct {
    std::atomic<bool> enabled;
    std::atomic<bool> group;
    std::atomic<int> delay;
    std::atomic<int> maxWindows;
} settings;

struct Item {
    int n, primary;
    HWND w[MAXMEMBERS];
    HICON icon[MAXMEMBERS];
    RECT r[MAXMEMBERS];
};

HANDLE g_thread, g_hookThread;
DWORD g_threadId, g_hookThreadId;
HINSTANCE g_inst;
HWND g_wnd;
std::atomic<bool> g_session, g_open;
int g_count, g_sel, g_dpi = 96;
constexpr int MAXRAW = 48;
constexpr int TITLE_LEN = MAXMEMBERS * 205;
HWND g_raw[MAXRAW];
HICON g_rawIcons[MAXRAW];
int g_rawCount;
Item g_items[MAXWIN];
HTHUMBNAIL g_thumbs[MAXMEMBERS];
HWND g_order[MAXWIN];
int g_orderCount, g_slot[MAXWIN], g_slotOf[MAXWIN];
DWORD g_lastJoy;
FrameClock g_frames;
LARGE_INTEGER g_lastFrame;
bool g_dirty;
RECT g_mon, g_preview;
POINT g_center, g_savedPos;
float g_ro, g_ri, g_wx, g_wy, g_jx, g_jy;
HDC g_mem;
HBITMAP g_bmp;
SIZE g_bmpSize;
bool g_swallowed[256], g_unhookPending;
WPARAM g_upToSwallow;

float Sc(float v) {
    return v * g_dpi / 96.f;
}

void SendKey(WORD vk, bool up) {
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    SendInput(1, &in, sizeof(INPUT));
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    WCHAR cls[32] = L"";
    DWORD cloaked = 0;
    LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    GetClassName(hwnd, cls, 32);
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (hwnd == g_wnd || !IsWindowVisible(hwnd) || cloaked || !GetWindowTextLength(hwnd) ||
        (!(ex & WS_EX_APPWINDOW) && (GetWindow(hwnd, GW_OWNER) || (ex & WS_EX_TOOLWINDOW))) ||
        !lstrcmpW(cls, L"Progman"))
        return TRUE;
    DWORD_PTR icon = 0;
    SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 20, &icon);
    if (!icon)
        SendMessageTimeout(hwnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG, 20, &icon);
    if (!icon)
        icon = GetClassLongPtr(hwnd, GCLP_HICON);
    if (!icon)
        icon = GetClassLongPtr(hwnd, GCLP_HICONSM);
    g_rawIcons[g_rawCount] = (HICON)icon;
    g_raw[g_rawCount++] = hwnd;
    return g_rawCount < MAXRAW;
}

void Activate(HWND hwnd) {
    if (IsIconic(hwnd))
        ShowWindowAsync(hwnd, SW_RESTORE);
    INPUT in{};
    SendInput(1, &in, sizeof(INPUT));
    SetForegroundWindow(hwnd);
}

bool AltReleased() {
    return GetAsyncKeyState(VK_MENU) >= 0;
}

HWND g_pendingPrimary, g_pendingOthers[MAXMEMBERS];
int g_pendingCount, g_pendingTicks;

void ActivateItem(const Item& it) {
    Activate(it.w[it.primary]);
    g_pendingCount = 0;
    for (int m = 0; m < it.n; m++)
        if (m != it.primary)
            g_pendingOthers[g_pendingCount++] = it.w[m];
    if (!g_pendingCount)
        return;
    g_pendingPrimary = it.w[it.primary];
    g_pendingTicks = 0;
    SetTimer(g_wnd, 3, 20, nullptr);
}

void ItemTitle(int i, WCHAR* buf, int len) {
    buf[0] = 0;
    for (int m = 0; m < g_items[i].n; m++) {
        WCHAR t[200] = L"";
        GetWindowText(g_items[i].w[m], t, 200);
        int used = (int)wcslen(buf);
        _snwprintf_s(buf + used, len - used, _TRUNCATE, m ? L"  |  %s" : L"%s", t);
    }
}

bool FrameMatches(HWND hwnd, RECT r) {
    RECT f;
    if (IsIconic(hwnd) ||
        FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &f, sizeof(f))))
        return false;
    return abs(f.left - r.left) <= 20 && abs(f.top - r.top) <= 20 &&
           abs(f.right - r.right) <= 20 && abs(f.bottom - r.bottom) <= 20;
}

void BuildItems() {
    HWND fg = GetAncestor(GetForegroundWindow(), GA_ROOTOWNER);
    for (int i = 1; i < g_rawCount; i++) {
        if (g_raw[i] != fg)
            continue;
        HWND w = g_raw[i];
        HICON icon = g_rawIcons[i];
        for (; i > 0; i--) {
            g_raw[i] = g_raw[i - 1];
            g_rawIcons[i] = g_rawIcons[i - 1];
        }
        g_raw[0] = w;
        g_rawIcons[0] = icon;
        break;
    }

    struct Group {
        int n;
        int idx[MAXMEMBERS];
        RECT r[MAXMEMBERS];
    } groups[MAXRAW / 2];
    int ng = 0, groupOf[MAXRAW];
    for (int i = 0; i < g_rawCount; i++)
        groupOf[i] = -1;
    for (int i = 0; settings.group && i < g_rawCount; i++) {
        if (groupOf[i] >= 0 || IsIconic(g_raw[i]) || IsZoomed(g_raw[i]))
            continue;
        HMONITOR mon = MonitorFromWindow(g_raw[i], MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfo(mon, &mi);
        for (int l = NL - 1; l >= 0; l--) {
            const Layout& L = g_layouts[l];
            Group gr{};
            gr.n = L.n;
            bool ok = true;
            for (int z = 0; z < L.n && ok; z++) {
                gr.r[z] = ZoneIn(mi.rcWork, L.z[z]);
                gr.idx[z] = -1;
                for (int j = 0; j < g_rawCount && gr.idx[z] < 0; j++) {
                    bool used = groupOf[j] >= 0;
                    for (int y = 0; y < z; y++)
                        used |= gr.idx[y] == j;
                    if (!used && MonitorFromWindow(g_raw[j], MONITOR_DEFAULTTONEAREST) == mon &&
                        !IsZoomed(g_raw[j]) && FrameMatches(g_raw[j], gr.r[z]))
                        gr.idx[z] = j;
                }
                ok = gr.idx[z] >= 0;
            }
            bool hasSeed = false;
            for (int z = 0; ok && z < L.n; z++)
                hasSeed |= gr.idx[z] == i;
            if (!ok || !hasSeed)
                continue;
            for (int z = 0; z < L.n; z++)
                groupOf[gr.idx[z]] = ng;
            groups[ng++] = gr;
            break;
        }
    }

    bool emitted[MAXRAW / 2] = {};
    g_count = 0;
    for (int i = 0; i < g_rawCount && g_count < settings.maxWindows; i++) {
        int gi = groupOf[i];
        if (gi >= 0 && emitted[gi])
            continue;
        Item& it = g_items[g_count++];
        if (gi < 0) {
            it.n = 1;
            it.primary = 0;
            it.w[0] = g_raw[i];
            it.icon[0] = g_rawIcons[i];
            continue;
        }
        emitted[gi] = true;
        it.n = groups[gi].n;
        for (int m = 0; m < it.n; m++) {
            it.w[m] = g_raw[groups[gi].idx[m]];
            it.r[m] = groups[gi].r[m];
            it.icon[m] = g_rawIcons[groups[gi].idx[m]];
            if (groups[gi].idx[m] == i)
                it.primary = m;
        }
    }
}

void UpdatePreview() {
    for (HTHUMBNAIL& t : g_thumbs) {
        if (t)
            DwmUnregisterThumbnail(t);
        t = nullptr;
    }
    int w = g_mon.right - g_mon.left, h = g_mon.bottom - g_mon.top;
    float m = Sc(40), titleH = Sc(48);
    float bx = m, by = m + titleH, bw = w - 2 * m, bh = h / 2.f - by - Sc(24);
    const Item& it = g_items[g_sel];
    RECT dest[MAXMEMBERS];

    if (it.n == 1) {
        float tw = bw, th = bh;
        if (SUCCEEDED(DwmRegisterThumbnail(g_wnd, it.w[0], &g_thumbs[0]))) {
            SIZE s{};
            DwmQueryThumbnailSourceSize(g_thumbs[0], &s);
            if (s.cx > 0 && s.cy > 0) {
                if (tw * s.cy > th * s.cx)
                    tw = th * s.cx / s.cy;
                else
                    th = tw * s.cy / s.cx;
            }
        } else {
            g_thumbs[0] = nullptr;
        }
        g_preview = {(LONG)(bx + (bw - tw) / 2), (LONG)by, (LONG)(bx + (bw + tw) / 2),
                     (LONG)(by + th)};
        dest[0] = g_preview;
    } else {
        RECT u = it.r[0];
        for (int k = 1; k < it.n; k++)
            UnionRect(&u, &u, &it.r[k]);
        float uw = (float)(u.right - u.left), uh = (float)(u.bottom - u.top);
        float s = fminf(bw / uw, bh / uh), ox = bx + (bw - uw * s) / 2, gap = Sc(3);
        g_preview = {(LONG)ox, (LONG)by, (LONG)(ox + uw * s), (LONG)(by + uh * s)};
        for (int k = 0; k < it.n; k++) {
            const RECT& r = it.r[k];
            dest[k] = {(LONG)(ox + (r.left - u.left) * s + gap), (LONG)(by + (r.top - u.top) * s + gap),
                       (LONG)(ox + (r.right - u.left) * s - gap),
                       (LONG)(by + (r.bottom - u.top) * s - gap)};
            if (FAILED(DwmRegisterThumbnail(g_wnd, it.w[k], &g_thumbs[k])))
                g_thumbs[k] = nullptr;
        }
    }

    for (int k = 0; k < it.n; k++) {
        if (!g_thumbs[k])
            continue;
        DWM_THUMBNAIL_PROPERTIES tp{};
        tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE;
        tp.rcDestination = dest[k];
        tp.fVisible = TRUE;
        DwmUpdateThumbnailProperties(g_thumbs[k], &tp);
    }
}

void BuildSlots() {
    HWND next[MAXWIN];
    int n = 0;
    for (int k = 0; k < g_orderCount; k++)
        for (int i = 0; i < g_count; i++)
            if (g_items[i].w[0] == g_order[k]) {
                next[n++] = g_order[k];
                break;
            }
    for (int i = 0; i < g_count; i++) {
        bool known = false;
        for (int k = 0; k < n; k++)
            known |= next[k] == g_items[i].w[0];
        if (!known)
            next[n++] = g_items[i].w[0];
    }
    for (int k = 0; k < n; k++) {
        g_order[k] = next[k];
        for (int i = 0; i < g_count; i++)
            if (g_items[i].w[0] == next[k]) {
                g_slot[k] = i;
                g_slotOf[i] = k;
            }
    }
    g_orderCount = n;
}

void Select(int sel) {
    if (sel == g_sel)
        return;
    g_sel = sel;
    if (g_open)
        UpdatePreview();
    InvalidateRect(g_wnd, nullptr, FALSE);
}

void Open() {
    GetCursorPos(&g_savedPos);
    HMONITOR mon = MonitorFromPoint(g_savedPos, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(mon, &mi);
    g_dpi = MonitorDpi(mon);
    g_mon = mi.rcMonitor;
    int w = g_mon.right - g_mon.left, h = g_mon.bottom - g_mon.top;
    g_center = {g_mon.left + w / 2, g_mon.top + h / 2};
    g_ro = fminf(w, h / 2.f) * 0.34f;
    g_ri = g_ro * 0.42f;
    g_wx = w / 2.f;
    g_wy = h * 0.75f;
    g_jx = g_jy = 0;
    g_dirty = false;

    SetWindowPos(g_wnd, HWND_TOPMOST, g_mon.left, g_mon.top, w, h, SWP_NOACTIVATE);
    g_open = true;
    UpdatePreview();
    ShowWindow(g_wnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_wnd);
    SetCursorPos(g_center.x, g_center.y);
    SetCursor(nullptr);
    g_lastFrame.QuadPart = 0;
    g_frames.Start(mon);
    PostThreadMessage(g_hookThreadId, WM_APP_MOUSEHOOK, 1, 0);
}

void Close() {
    if (!g_open)
        return;
    g_open = false;
    g_frames.Stop();
    PostThreadMessage(g_hookThreadId, WM_APP_MOUSEHOOK, 0, 0);
    for (HTHUMBNAIL& t : g_thumbs) {
        if (t)
            DwmUnregisterThumbnail(t);
        t = nullptr;
    }
    ShowWindow(g_wnd, SW_HIDE);
    SetCursorPos(g_savedPos.x, g_savedPos.y);
}

void AddRoundRect(Gdiplus::GraphicsPath& p, Gdiplus::RectF r, float rad) {
    float d = rad * 2;
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

float IconSize() {
    float rm = (g_ri + g_ro) / 2, band = g_ro - g_ri;
    float chord = g_count > 1 ? 2 * rm * sinf(PI / g_count) : 2 * rm;
    float s = fminf(chord * 0.38f, band * 0.4f);
    return fmaxf(Sc(20), fminf(s, Sc(40)));
}

void Paint(HDC dc, int w, int h) {
    using namespace Gdiplus;
    float span = 360.f / (g_count ? g_count : 1), rm = (g_ri + g_ro) / 2, rh = g_ri - Sc(16);
    float isz = IconSize(), headerIconX = 0, headerIconY = 0, hsz = Sc(28);
    {
        Graphics g(dc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        SolidBrush top(Color(255, 22, 22, 22)), bottom(Color(255, 16, 16, 16));
        g.FillRectangle(&top, 0, 0, w, h / 2);
        g.FillRectangle(&bottom, 0, h / 2, w, h - h / 2);
        Pen divider(Color(255, 54, 54, 54), Sc(1));
        g.DrawLine(&divider, 0.f, h / 2.f, (float)w, h / 2.f);
        if (!g_count)
            return;

        SolidBrush white(Color(255, 255, 255, 255)), dim(Color(255, 185, 185, 185));
        Color accent(255, 0, 150, 255);
        StringFormat center;
        center.SetAlignment(StringAlignmentCenter);
        center.SetLineAlignment(StringAlignmentCenter);
        center.SetTrimming(StringTrimmingEllipsisCharacter);
        center.SetFormatFlags(StringFormatFlagsNoWrap);

        WCHAR title[TITLE_LEN], text[TITLE_LEN + 20];
        ItemTitle(g_sel, title, TITLE_LEN);
        _snwprintf_s(text, _TRUNCATE, L"%d  \u00B7  %s", g_sel + 1, title);
        Font titleFont(L"Segoe UI Semibold", Sc(20), FontStyleRegular, UnitPixel);
        RectF titleRect(Sc(80), Sc(40), w - Sc(160), Sc(40)), measured;
        g.MeasureString(text, -1, &titleFont, titleRect, &center, &measured);
        headerIconX = measured.X - hsz - Sc(10);
        headerIconY = titleRect.Y + (titleRect.Height - hsz) / 2;
        g.DrawString(text, -1, &titleFont, titleRect, &center, &white);

        SolidBrush frame(accent);
        GraphicsPath framePath;
        AddRoundRect(framePath,
                     RectF(g_preview.left - Sc(4), g_preview.top - Sc(4),
                           g_preview.right - g_preview.left + Sc(8),
                           g_preview.bottom - g_preview.top + Sc(8)),
                     Sc(10));
        g.FillPath(&frame, &framePath);

        SolidBrush slice(Color(255, 40, 40, 40)), selFill(Color(255, 0, 95, 184));
        float cr = Sc(8), gap = Sc(4), ro = g_ro - cr, ri = g_ri + cr;
        float go = (gap / 2 + cr) / ro * 180 / PI, gi = (gap / 2 + cr) / ri * 180 / PI;
        int fs = g_count <= 6 ? 14 : g_count <= 10 ? 12 : 11;
        Font labelFont(L"Segoe UI", Sc(fs), FontStyleRegular, UnitPixel);
        Font labelBold(L"Segoe UI Semibold", Sc(fs), FontStyleRegular, UnitPixel);
        Font numFont(L"Segoe UI", Sc(11), FontStyleRegular, UnitPixel);
        StringFormat wrap;
        wrap.SetAlignment(StringAlignmentCenter);
        wrap.SetLineAlignment(StringAlignmentCenter);
        wrap.SetTrimming(StringTrimmingEllipsisWord);
        float lr = g_ro + Sc(10), lh = Sc(fs) * 2.7f;
        float lw = fmaxf(Sc(70), fminf(2 * PI * lr / g_count * 0.92f, Sc(200)));

        for (int k = 0; k < g_count; k++) {
            int i = g_slot[k];
            SolidBrush& fill = i == g_sel ? selFill : slice;
            GraphicsPath p;
            float a0 = -90 + k * span - span / 2;
            if (g_count == 1) {
                p.AddEllipse(RectF(g_wx - g_ro, g_wy - g_ro, 2 * g_ro, 2 * g_ro));
                p.AddEllipse(RectF(g_wx - g_ri, g_wy - g_ri, 2 * g_ri, 2 * g_ri));
                g.FillPath(&fill, &p);
            } else {
                p.AddArc(RectF(g_wx - ro, g_wy - ro, 2 * ro, 2 * ro), a0 + go, span - 2 * go);
                p.AddArc(RectF(g_wx - ri, g_wy - ri, 2 * ri, 2 * ri), a0 + span - gi,
                         -(span - 2 * gi));
                p.CloseFigure();
                Pen round(&fill, 2 * cr);
                round.SetLineJoin(LineJoinRound);
                g.FillPath(&fill, &p);
                g.DrawPath(&round, &p);
            }

            float a = (-90 + k * span) * PI / 180, c = cosf(a), sn = sinf(a);
            WCHAR num[8];
            swprintf_s(num, L"%d", i + 1);
            g.DrawString(num, -1, &numFont,
                         RectF(g_wx + rm * c - Sc(20), g_wy + rm * sn + isz / 2, Sc(40), Sc(16)),
                         &center, i == g_sel ? &white : &dim);

            WCHAR label[TITLE_LEN];
            ItemTitle(i, label, TITLE_LEN);
            float lx = g_wx + lr * c + c * lw / 2, ly = g_wy + lr * sn + sn * lh / 2;
            g.DrawString(label, -1, i == g_sel ? &labelBold : &labelFont,
                         RectF(lx - lw / 2, ly - lh / 2, lw, lh), &wrap,
                         i == g_sel ? &white : &dim);
        }

        int ks = g_slotOf[g_sel];
        float ta = (-90 + ks * span) * PI / 180, tc = cosf(ta), ts = sinf(ta);
        float tip = g_ri - Sc(3), base = rh + Sc(3), half = Sc(7);
        PointF tri[3] = {
            PointF(g_wx + tip * tc, g_wy + tip * ts),
            PointF(g_wx + base * tc - half * ts, g_wy + base * ts + half * tc),
            PointF(g_wx + base * tc + half * ts, g_wy + base * ts - half * tc),
        };
        SolidBrush pointer(accent);
        g.FillPolygon(&pointer, tri, 3);

        SolidBrush rim(Color(255, 72, 72, 72)), disc(Color(255, 122, 122, 122));
        SolidBrush well(Color(255, 98, 98, 98)), knob(Color(255, 16, 58, 250));
        float rd = rh * 0.9f, rw = rh * 0.62f, kr = rh * 0.26f;
        g.FillEllipse(&rim, g_wx - rh, g_wy - rh, 2 * rh, 2 * rh);
        g.FillEllipse(&disc, g_wx - rd, g_wy - rd, 2 * rd, 2 * rd);
        g.FillEllipse(&well, g_wx - rw, g_wy - rw, 2 * rw, 2 * rw);

        float R = Sc(80), travel = rh - kr * 0.55f;
        float kx = g_wx + g_jx / R * travel, ky = g_wy + g_jy / R * travel;
        Pen shaft(Color(110, 210, 210, 210), Sc(3));
        shaft.SetStartCap(LineCapRound);
        shaft.SetEndCap(LineCapRound);
        g.DrawLine(&shaft, g_wx, g_wy, kx, ky);
        if (g_jx * g_jx + g_jy * g_jy >= R * R * 0.9f) {
            SolidBrush halo(Color(70, 255, 255, 255));
            float hr = kr * 1.6f;
            g.FillEllipse(&halo, kx - hr, ky - hr, 2 * hr, 2 * hr);
        }
        g.FillEllipse(&knob, kx - kr, ky - kr, 2 * kr, 2 * kr);
    }

    if (!g_count)
        return;
    for (int k = 0; k < g_count; k++) {
        const Item& it = g_items[g_slot[k]];
        float a = (-90 + k * span) * PI / 180, s = it.n > 1 ? isz * 0.7f : isz, sp = Sc(4);
        float x = g_wx + rm * cosf(a) - (it.n * s + (it.n - 1) * sp) / 2;
        float y = g_wy + rm * sinf(a) - s / 2 - Sc(6);
        for (int m = 0; m < it.n; m++)
            if (it.icon[m])
                DrawIconEx(dc, (int)(x + m * (s + sp)), (int)y, it.icon[m], (int)s, (int)s, 0,
                           nullptr, DI_NORMAL);
    }
    const Item& sel = g_items[g_sel];
    if (sel.icon[sel.primary])
        DrawIconEx(dc, (int)headerIconX, (int)headerIconY, sel.icon[sel.primary], (int)hsz,
                   (int)hsz, 0, nullptr, DI_NORMAL);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    auto* k = (KBDLLHOOKSTRUCT*)lp;
    if (code != HC_ACTION || (k->flags & LLKHF_INJECTED))
        return CallNextHookEx(nullptr, code, wp, lp);

    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    DWORD vk = k->vkCode & 0xFF;
    if (!down && g_swallowed[vk]) {
        g_swallowed[vk] = false;
        return 1;
    }
    if (vk == VK_TAB && down && (g_session || (settings.enabled && (k->flags & LLKHF_ALTDOWN)))) {
        bool shift = GetAsyncKeyState(VK_SHIFT) < 0;
        g_swallowed[vk] = true;
        if (!g_session) {
            g_session = true;
            PostMessage(g_wnd, WM_APP_START, shift, 0);
        } else {
            PostMessage(g_wnd, WM_APP_STEP, shift ? -1 : 1, 0);
        }
        return 1;
    }
    if (!g_session)
        return CallNextHookEx(nullptr, code, wp, lp);

    if (vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU) {
        if (!down) {
            g_session = false;
            PostMessage(g_wnd, WM_APP_END, 1, 0);
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    if (!down || vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT)
        return CallNextHookEx(nullptr, code, wp, lp);

    g_swallowed[vk] = true;
    if (vk == VK_ESCAPE) {
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, 0, 0);
    } else if (vk == VK_LEFT || vk == VK_UP || vk == VK_RIGHT || vk == VK_DOWN) {
        PostMessage(g_wnd, WM_APP_STEP, vk == VK_LEFT || vk == VK_UP ? -1 : 1, 0);
    }
    return 1;
}

LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION)
        return CallNextHookEx(nullptr, code, wp, lp);
    if (g_upToSwallow && wp == g_upToSwallow) {
        g_upToSwallow = 0;
        if (g_unhookPending)
            PostThreadMessage(GetCurrentThreadId(), WM_APP_MOUSEHOOK, 0, 0);
        return 1;
    }
    if (!g_open)
        return CallNextHookEx(nullptr, code, wp, lp);
    auto* m = (MSLLHOOKSTRUCT*)lp;
    if (wp == WM_MOUSEMOVE) {
        POINT cur;
        GetCursorPos(&cur);
        int dx = m->pt.x - cur.x, dy = m->pt.y - cur.y;
        if (!dx && !dy)
            return CallNextHookEx(nullptr, code, wp, lp);
        PostMessage(g_wnd, WM_APP_JOY, (WPARAM)dx, (LPARAM)dy);
    } else if (wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN) {
        g_upToSwallow = wp == WM_LBUTTONDOWN ? WM_LBUTTONUP : WM_RBUTTONUP;
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, wp == WM_LBUTTONDOWN ? 1 : 2, 0);
    } else if (wp == WM_MOUSEWHEEL) {
        PostMessage(g_wnd, WM_APP_STEP, (short)HIWORD(m->mouseData) > 0 ? -1 : 1, 0);
    }
    return 1;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    HHOOK kb = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, g_inst, 0), mouse = nullptr;
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != WM_APP_MOUSEHOOK)
            continue;
        g_unhookPending = false;
        if (msg.wParam && !mouse) {
            mouse = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, g_inst, 0);
        } else if (!msg.wParam && mouse && g_upToSwallow) {
            g_unhookPending = true;
        } else if (!msg.wParam && mouse) {
            UnhookWindowsHookEx(mouse);
            mouse = nullptr;
        }
    }
    if (mouse)
        UnhookWindowsHookEx(mouse);
    UnhookWindowsHookEx(kb);
    return 0;
}

void OnFrame() {
    LARGE_INTEGER now, freq;
    QueryPerformanceCounter(&now);
    QueryPerformanceFrequency(&freq);
    float dt = g_lastFrame.QuadPart ? (now.QuadPart - g_lastFrame.QuadPart) * 1000.f / freq.QuadPart
                                    : 0;
    g_lastFrame = now;
    if (GetTickCount() - g_lastJoy > 90 && (g_jx || g_jy)) {
        float k = powf(0.72f, dt / 15.f);
        g_jx *= k;
        g_jy *= k;
        if (g_jx * g_jx + g_jy * g_jy < 1)
            g_jx = g_jy = 0;
        g_dirty = true;
    }
    if (g_dirty) {
        g_dirty = false;
        RedrawWindow(g_wnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_APP_START:
            SendKey(VK_MASK, false);
            SendKey(VK_MASK, true);
            g_rawCount = 0;
            EnumWindows(EnumProc, 0);
            BuildItems();
            if (!g_count) {
                g_session = false;
                return 0;
            }
            BuildSlots();
            g_sel = g_count < 2 ? 0 : wp ? g_count - 1 : 1;
            if (settings.delay > 0)
                SetTimer(hwnd, 1, settings.delay, nullptr);
            else
                Open();
            return 0;
        case WM_TIMER:
            if (wp == 3) {
                if (GetForegroundWindow() != g_pendingPrimary && ++g_pendingTicks < 15)
                    return 0;
                KillTimer(hwnd, 3);
                for (int k = 0; k < g_pendingCount; k++)
                    SetWindowPos(g_pendingOthers[k], g_pendingPrimary, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
                return 0;
            }
            KillTimer(hwnd, 1);
            if (g_session && AltReleased()) {
                g_session = false;
                PostMessage(hwnd, WM_APP_END, 0, 0);
            } else if (g_session) {
                Open();
            }
            return 0;
        case WM_APP_STEP:
            if (g_session && AltReleased()) {
                g_session = false;
                PostMessage(hwnd, WM_APP_END, 0, 0);
            } else if (g_count)
                Select((g_sel + (int)(INT_PTR)wp + g_count) % g_count);
            return 0;
        case WM_APP_JOY: {
            if (!g_open)
                return 0;
            if (g_session && AltReleased()) {
                g_session = false;
                PostMessage(hwnd, WM_APP_END, 0, 0);
                return 0;
            }
            g_jx += (int)(INT_PTR)wp;
            g_jy += (int)(INT_PTR)lp;
            g_lastJoy = GetTickCount();
            g_dirty = true;
            float len = sqrtf(g_jx * g_jx + g_jy * g_jy), r = Sc(80);
            if (len > r) {
                g_jx *= r / len;
                g_jy *= r / len;
            }
            if (len < Sc(20))
                return 0;
            float deg = atan2f(g_jy, g_jx) * 180 / PI;
            int k = ((int)lroundf((deg + 90) / (360.f / g_count)) % g_count + g_count) % g_count;
            Select(g_slot[k]);
            return 0;
        }
        case WM_APP_END:
            KillTimer(hwnd, 1);
            Close();
            if (wp && g_sel < g_count) {
                ActivateItem(g_items[g_sel]);
                for (int m = 0; wp == 2 && m < g_items[g_sel].n; m++)
                    PostMessage(g_items[g_sel].w[m], WM_CLOSE, 0, 0);
            }
            g_count = 0;
            return 0;
        case WM_SETCURSOR:
            SetCursor(nullptr);
            return TRUE;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (!g_mem)
                g_mem = CreateCompatibleDC(dc);
            if (rc.right != g_bmpSize.cx || rc.bottom != g_bmpSize.cy) {
                HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
                SelectObject(g_mem, bmp);
                if (g_bmp)
                    DeleteObject(g_bmp);
                g_bmp = bmp;
                g_bmpSize = {rc.right, rc.bottom};
            }
            Paint(g_mem, rc.right, rc.bottom);
            BitBlt(dc, 0, 0, rc.right, rc.bottom, g_mem, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD WINAPI ThreadProc(LPVOID) {
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gsi, nullptr);
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)ThreadProc, &g_inst);

    WNDCLASS wc{0, WndProc, 0, 0, g_inst, nullptr, nullptr, nullptr, nullptr, L"RadialSwitcher"};
    RegisterClass(&wc);
    g_wnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName,
                           nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_inst, nullptr);
    BOOL noAnim = TRUE;
    DwmSetWindowAttribute(g_wnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noAnim, sizeof(noAnim));
    StartThread(HookThreadProc, g_hookThread, g_hookThreadId);

    g_frames.Create();
    RunMessageLoop(g_frames, OnFrame);

    Close();
    g_frames.Destroy();
    StopThread(g_hookThread, g_hookThreadId);
    DestroyWindow(g_wnd);
    if (g_mem)
        DeleteDC(g_mem);
    if (g_bmp)
        DeleteObject(g_bmp);
    g_mem = nullptr;
    g_bmp = nullptr;
    g_bmpSize = {};
    UnregisterClass(wc.lpszClassName, g_inst);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

void LoadSettings() {
    settings.enabled = Wh_GetIntSetting(L"radialSwitcher");
    settings.group = Wh_GetIntSetting(L"groupSplitWindows");
    int delay = Wh_GetIntSetting(L"openDelay");
    int maxWindows = Wh_GetIntSetting(L"maxWindows");
    settings.delay = delay < 0 ? 0 : delay;
    settings.maxWindows = maxWindows < 2 ? 2 : maxWindows > MAXWIN ? MAXWIN : maxWindows;
}

bool Start() {
    g_session = false;
    return StartThread(ThreadProc, g_thread, g_threadId);
}

void Stop() {
    StopThread(g_thread, g_threadId);
}

}  // namespace radial

bool g_splitRunning, g_radialRunning;

void LoadSettings() {
    split::LoadSettings();
    radial::LoadSettings();
    if (split::settings.enabled && !g_splitRunning) {
        g_splitRunning = split::Start();
    } else if (!split::settings.enabled && g_splitRunning) {
        split::Stop();
        g_splitRunning = false;
    }
    if (radial::settings.enabled && !g_radialRunning) {
        g_radialRunning = radial::Start();
    } else if (!radial::settings.enabled && g_radialRunning) {
        radial::Stop();
        g_radialRunning = false;
    }
}

BOOL WhTool_ModInit() {
    split::RecoverDocking();
    LoadSettings();
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_splitRunning)
        split::Stop();
    if (g_radialRunning)
        radial::Stop();
    split::SetDocking(false);
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
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

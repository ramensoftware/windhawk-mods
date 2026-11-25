// ==WindhawkMod==
// @id              top-drag-split-layouts
// @name            Top Drag Split Layouts
// @description     Own snap layouts bar at the top center with a top/bottom split, plus a window chooser for the remaining zones
// @version         1.0
// @author          khorkovenko
// @github          https://github.com/khorkovenko
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshcore -lgdi32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Top Drag Split Layouts
Drag a window to the top center of the screen. A layouts bar appears with six
layouts: top/bottom halves, left/right halves, thirds, two thirds + one third,
half + two quarters and four quarters. The top half is lightly highlighted as a
hint. Hover a zone to select it: a preview shows the area the window will be
stretched to, and releasing the mouse places the window there. Releasing it
without a selected zone leaves the window where it was dropped. The bar and the
preview appear instantly, without animations.

After the first window is placed, the next empty zone of the same layout shows
your other open windows. Click one to place it there. The X on a window closes
that window. Press Esc, click the X in the top right corner, click empty space
or switch to another window to stop choosing; all windows stay as they are.

Moving a window to a screen edge or corner also opens the bar and selects the
matching zone: the left or right edge selects the left or right half, the top or
bottom edge the top or bottom half, and a corner the matching quarter. The
preview shows the area, and dropping the window there places it in that zone.

With "Turn off Windows edge snapping while dragging" enabled, Windows doesn't
maximize or snap windows dragged to the screen edges, and doesn't show its gray
full-screen or half-screen previews. The mod turns this off for the current
session only, with `SystemParametersInfo(SPI_SETDOCKMOVING)` without saving it,
and turns it back on when the setting is turned off or the mod is disabled.
Nothing is written to the registry; after signing out and in again, Windows
uses its saved setting.

The built-in Windows snap layouts bar appears in the same place when a window
is dragged to the top of the screen. To use only this mod's bar, turn off "Show
snap layouts when I drag a window to the top of my screen" in Settings > System
> Multitasking > Snap windows. The mod doesn't change this Windows setting.

Windows does not allow programs to move windows of programs running as
administrator, so such windows may not be placed by this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- snapAssist: true
  $name: Offer windows for the remaining zones
- noEdgeSnap: true
  $name: Turn off Windows edge snapping while dragging
  $description: >-
    Stops Windows from maximizing or snapping windows dragged to the screen
    edges, and from showing its previews. Applies to the current session only
    and is turned back on when this setting is off or the mod is disabled.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <stdio.h>
#include <shellscalingapi.h>

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
    {3, {{0, 0, 2, 6}, {2, 0, 4, 6}, {4, 0, 6, 6}}},
    {2, {{0, 0, 4, 6}, {4, 0, 6, 6}}},
    {3, {{0, 0, 3, 6}, {3, 0, 6, 3}, {3, 3, 6, 6}}},
    {4, {{0, 0, 3, 3}, {3, 0, 6, 3}, {0, 3, 3, 6}, {3, 3, 6, 6}}},
};
constexpr int NL = sizeof(g_layouts) / sizeof(g_layouts[0]);
constexpr int MAXWIN = 12;

struct {
    bool noEdgeSnap;
    bool assist;
} settings;

HANDLE g_thread;
DWORD g_threadId;
HWND g_bar, g_preview, g_chooser, g_hwnd;
BOOL g_oldDock = TRUE;
bool g_dockChanged;
HFONT g_font;
RECT g_work;
POINT g_start;
UINT_PTR g_track, g_timer;
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

int MonitorDpi(HMONITOR mon) {
    UINT dx, dy;
    return SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) ? dx : 96;
}

RECT ZoneIn(RECT a, Zone z) {
    int w = a.right - a.left, h = a.bottom - a.top;
    return {a.left + w * z.x0 / 6, a.top + h * z.y0 / 6, a.left + w * z.x1 / 6,
            a.top + h * z.y1 / 6};
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
    if (!IsWindowVisible(hwnd) || cloaked || GetWindow(hwnd, GW_OWNER) ||
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
            if (Place(pick, ZoneIn(g_work, g_layouts[g_layout].z[g_zone])))
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
    if ((!g_force && !zoomed) || !Place(g_hwnd, ZoneIn(g_work, g_layouts[g_layout].z[g_zone])))
        return;
    g_done = 1 << g_zone;
    g_used[0] = g_hwnd;
    g_nused = 1;
    if (settings.assist)
        NextChooser();
}

void CALLBACK TrackProc(HWND, UINT, UINT_PTR, DWORD) {
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
    int e = Sc(6), c = Sc(40);
    auto free = [&](int ox, int oy) {
        return moved && !MonitorFromPoint({pt.x + ox, pt.y + oy}, MONITOR_DEFAULTTONULL);
    };
    bool L = pt.x <= m.left + e && free(-2 * e, 0);
    bool R = pt.x >= m.right - 1 - e && free(2 * e, 0);
    bool T = pt.y <= m.top + e && free(0, -2 * e);
    bool B = pt.y >= m.bottom - 1 - e && free(0, 2 * e);
    int h = L ? 0 : R ? 1 : -1, v = T ? 0 : B ? 1 : -1;
    if (h < 0 && v >= 0)
        h = pt.x <= m.left + c ? 0 : pt.x >= m.right - c ? 1 : -1;
    if (v < 0 && h >= 0 && (L || R))
        v = pt.y <= m.top + c ? 0 : pt.y >= m.bottom - c ? 1 : -1;

    int hl = -1, hz = -1;
    if (h >= 0 && v >= 0) {
        hl = 5;
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
        SetWindowPos(g_bar, HWND_TOPMOST, bx, by, bw, bh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    else
        ShowWindow(g_bar, SW_HIDE);

    if (hl != g_hl || hz != g_hz) {
        g_hl = hl;
        g_hz = hz;
        InvalidateRect(g_bar, nullptr, FALSE);
    }

    if (hl >= 0) {
        RECT z = ZoneIn(g_work, g_layouts[hl].z[hz]);
        SetWindowPos(g_preview, g_hwnd, z.left, z.top, z.right - z.left, z.bottom - z.top,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        ShowWindow(g_preview, SW_HIDE);
    }
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
        g_track = SetTimer(nullptr, g_track, 15, TrackProc);
        return;
    }

    if (!g_track || hwnd != g_hwnd)
        return;
    KillTimer(nullptr, g_track);
    g_track = 0;
    ShowWindow(g_bar, SW_HIDE);
    ShowWindow(g_preview, SW_HIDE);

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
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HINSTANCE inst;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)ThreadProc, &inst);
    HBRUSH brush = CreateSolidBrush(RGB(0, 120, 215));
    WNDCLASS pc{0, DefWindowProc, 0, 0, inst,
                nullptr, nullptr, brush, nullptr, L"TopDragSplitPreview"};
    WNDCLASS uc{0, WndProc, 0, 0, inst,
                nullptr, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, L"TopDragSplitUi"};
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
    int round = 2;
    DwmSetWindowAttribute(g_bar, 33, &round, sizeof(round));
    DwmSetWindowAttribute(g_preview, 33, &round, sizeof(round));
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
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_track)
        KillTimer(nullptr, g_track);
    if (g_timer)
        KillTimer(nullptr, g_timer);
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
    return 0;
}

void SetDocking(bool off) {
    if (off && !g_dockChanged) {
        SystemParametersInfo(SPI_GETDOCKMOVING, 0, &g_oldDock, 0);
        SystemParametersInfo(SPI_SETDOCKMOVING, 0, (PVOID)FALSE, 0);
        g_dockChanged = true;
    } else if (!off && g_dockChanged) {
        SystemParametersInfo(SPI_SETDOCKMOVING, 0, (PVOID)(INT_PTR)g_oldDock, 0);
        g_dockChanged = false;
    }
}

void LoadSettings() {
    settings.assist = Wh_GetIntSetting(L"snapAssist");
    settings.noEdgeSnap = Wh_GetIntSetting(L"noEdgeSnap");
    SetDocking(settings.noEdgeSnap);
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModUninit() {
    while (!PostThreadMessage(g_threadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(g_thread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    SetDocking(false);
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

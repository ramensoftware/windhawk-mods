// ==WindhawkMod==
// @id              split-layouts-radial-switcher
// @name            Split Layouts & Radial Switcher
// @description     Drag windows to the top or the screen edges to arrange them in split layouts, and switch between windows and split groups with a radial Alt+Tab wheel
// @version         1.0
// @author          khorkovenko
// @github          https://github.com/khorkovenko
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshcore -lgdi32 -lgdiplus -lshell32 -ladvapi32 -ldxgi
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
  windows of a group) and keeps the view open, so you can close several windows
  in a row; the wheel updates as soon as the window is gone. If a window asks a
  question before closing, for example about unsaved changes, the view closes
  and switches to that question. With "Force-close windows that ask before
  closing" enabled, such an app is ended without asking instead. Only a
  question that opens as its own dialog window after the right click counts: an
  app that just takes long to close, asks inside its own window, or is a
  Microsoft Store app whose own process can't be told apart is never ended.
  **Esc** cancels and leaves all windows as they are.

The bar, the previews and the Alt+Tab view appear instantly, without animations.
Dragging feedback and the thumbstick are drawn once per screen refresh, in sync
with the vertical blank of the monitor in use, whatever its refresh rate (for
example 60, 144 or 240 Hz).

## Scrolling split windows together
When the mouse is over a window that fills a split layout together with other
windows, hold **Ctrl** and turn the mouse wheel to scroll all of them at once.
Hold **Ctrl + Shift** to scroll them horizontally (tilting the wheel with Ctrl
held works too). Each window is scrolled at the same relative position as the
mouse, and the apps don't zoom.

Ctrl can be pressed before or after the mouse reaches the split, and it works
as usual until the wheel is turned over a split. Only from that moment the mod
hides the key from the apps, so they scroll instead of zooming. With any other
key, a click, or once the mouse leaves the split, the apps get Ctrl back, so
shortcuts like Ctrl+C and Ctrl+click keep working. Over other windows Ctrl +
mouse wheel keeps its usual meaning, such as zooming in a browser. The mod
watches the mouse only while a Ctrl key is down.

The window under the mouse gets the wheel as usual. For the other windows the
mod repeats the wheel at the matching position: the mouse pointer jumps there
and back in one step, which normally isn't visible. This needs "Scroll inactive
windows when I hover over them" in Settings > Bluetooth & devices > Mouse, which
is on by default.

With the "Enabled, without moving the pointer" option, or when that Windows
setting is off, the mod sends the scroll messages straight to the other windows
instead. The pointer never moves, but some apps, such as Firefox and Microsoft
Store apps, ignore these messages and only the window under the mouse scrolls.

## Floating windows
Floating always-on-top windows, such as the Google Meet or video
picture-in-picture window, can be placed in a zone like any other window, are
offered by the chooser, and count as part of a split arrangement even when the
app keeps them smaller than the zone (at least 40% of it). On their own they
don't appear in Alt+Tab, like in Windows.

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
- Scrolling the windows of a split arrangement together isn't offered by
  Windows or by another mod. It uses the same detection of split windows as the
  Alt+Tab grouping.

## Limitations
Windows doesn't let programs move or close windows of programs running as
administrator, so such windows may not be placed by the split layouts or closed
with a right click. When an administrator window is in the foreground, Windows
may not pass the keyboard to this mod, and the default Alt+Tab appears instead.
For the same reason, split windows aren't scrolled together while an
administrator window is in the foreground.
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
- syncScroll: enabled
  $name: Scroll split windows together
  $description: >-
    Ctrl + mouse wheel scrolls all windows of a split arrangement at once;
    Ctrl + Shift + mouse wheel scrolls them horizontally. Elsewhere Ctrl + mouse
    wheel works as usual (for example, zoom). Without moving the pointer, the
    scrolling is sent to the windows as messages, which some apps (Firefox,
    Microsoft Store apps) ignore.
  $options:
  - enabled: Enabled
  - messages: Enabled, without moving the pointer (works in fewer apps)
  - disabled: Disabled
- forceClose: false
  $name: Force-close windows that ask before closing
  $description: >-
    When a window closed with a right click asks a question, for example about
    unsaved changes, end its app without asking. This ends the whole app: all
    of its windows close and unsaved work is lost. File Explorer is never
    forced, and neither is an app that doesn't open a question dialog (one
    that is slow to close or asks inside its own window). When off, the Alt+Tab
    view closes and switches to the question.
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
#include <dxgi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <shellscalingapi.h>
#include <gdiplus.h>
#include <sddl.h>
#include <windhawk_utils.h>

#ifndef SPI_GETMOUSEWHEELROUTING
#define SPI_GETMOUSEWHEELROUTING 0x201C
#endif

namespace common {

// Marks the input sent by this mod, so that its own hooks leave it alone. Input
// injected by other programs (mouse and touchpad drivers, remote desktop, key
// remappers) is handled like input from the user.
constexpr ULONG_PTR INPUT_TAG = 0x53504C54;
constexpr WORD VK_MASK = 0xE8;

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

// Every thread of the mod works in physical pixels, like the window frames
// reported by DWM and the points reported to low-level mouse hooks.
void InitThread() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

bool StartThread(LPTHREAD_START_ROUTINE proc, HANDLE& thread, DWORD& threadId) {
    thread = CreateThread(nullptr, 0, proc, nullptr, 0, &threadId);
    return thread != nullptr;
}

// Waits until the thread is gone: a new instance of a feature must never start
// while the old thread still uses the same globals.
void StopThread(HANDLE thread, DWORD threadId) {
    // Posting fails until the thread has created its message queue.
    while (!PostThreadMessage(threadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(thread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

bool IsFloating(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    RECT r;
    return (style & WS_THICKFRAME) && (ex & WS_EX_TOPMOST) && GetWindowRect(hwnd, &r) &&
           r.right - r.left >= 160 && r.bottom - r.top >= 120;
}

bool IsCandidate(HWND hwnd, bool* floating) {
    WCHAR cls[32] = L"";
    DWORD cloaked = 0;
    if (!IsWindowVisible(hwnd))
        return false;
    LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    GetClassName(hwnd, cls, 32);
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked || !GetWindowTextLength(hwnd) || (ex & WS_EX_TRANSPARENT) ||
        !lstrcmpW(cls, L"Progman"))
        return false;
    bool normal = (ex & WS_EX_APPWINDOW) || (!GetWindow(hwnd, GW_OWNER) && !(ex & WS_EX_TOOLWINDOW));
    bool isFloating = !normal && IsFloating(hwnd);
    if (floating)
        *floating = isFloating;
    return normal || isFloating;
}

bool FrameMatches(HWND hwnd, RECT r) {
    RECT f;
    if (IsIconic(hwnd) ||
        FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &f, sizeof(f))))
        return false;
    if (abs(f.left - r.left) <= 20 && abs(f.top - r.top) <= 20 &&
        abs(f.right - r.right) <= 20 && abs(f.bottom - r.bottom) <= 20)
        return true;
    if (!IsFloating(hwnd) || f.left < r.left - 20 || f.top < r.top - 20 ||
        f.right > r.right + 20 || f.bottom > r.bottom + 20)
        return false;
    LONGLONG area = (LONGLONG)(f.right - f.left) * (f.bottom - f.top);
    return area * 10 >= (LONGLONG)(r.right - r.left) * (r.bottom - r.top) * 4;
}

struct WindowList {
    int n;
    HWND w[128];
};

BOOL CALLBACK CollectProc(HWND hwnd, LPARAM lp) {
    auto* list = (WindowList*)lp;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsZoomed(hwnd) ||
        !IsCandidate(hwnd, nullptr))
        return TRUE;
    list->w[list->n++] = hwnd;
    return list->n < 128;
}

HWND RootAt(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    return hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
}

// Finds the windows that fill a split layout together with the window at pt.
// It is called from low-level hooks, so the common case, a window that isn't in
// any zone, returns after a few calls, without listing the open windows.
int FindSplitGroup(POINT pt, HWND* members, RECT* zones) {
    HWND root = RootAt(pt);
    if (!root || IsIconic(root) || IsZoomed(root) || !IsCandidate(root, nullptr))
        return 0;
    HMONITOR mon = MonitorFromWindow(root, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi))
        return 0;
    WindowList list{};
    bool listed = false;
    for (int l = NL - 1; l >= 0; l--) {
        const Layout& L = g_layouts[l];
        int rootZone = -1;
        for (int z = 0; z < L.n; z++) {
            zones[z] = ZoneIn(mi.rcWork, L.z[z]);
            if (rootZone < 0 && FrameMatches(root, zones[z]))
                rootZone = z;
        }
        if (rootZone < 0)
            continue;
        if (!listed) {
            EnumWindows(CollectProc, (LPARAM)&list);
            listed = true;
        }
        bool ok = true;
        for (int z = 0; z < L.n && ok; z++) {
            members[z] = z == rootZone ? root : nullptr;
            for (int i = 0; i < list.n && !members[z]; i++) {
                bool used = list.w[i] == root;
                for (int y = 0; y < z; y++)
                    used |= members[y] == list.w[i];
                if (!used && MonitorFromWindow(list.w[i], MONITOR_DEFAULTTONEAREST) == mon &&
                    FrameMatches(list.w[i], zones[z]))
                    members[z] = list.w[i];
            }
            ok = members[z] != nullptr;
        }
        if (ok)
            return L.n;
    }
    return 0;
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

IDXGIOutput* FindOutput(HMONITOR mon) {
    IDXGIFactory1* factory = nullptr;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
        return nullptr;
    IDXGIOutput* found = nullptr;
    IDXGIAdapter1* adapter = nullptr;
    for (UINT a = 0; !found && SUCCEEDED(factory->EnumAdapters1(a, &adapter)); a++) {
        IDXGIOutput* output = nullptr;
        for (UINT o = 0; !found && SUCCEEDED(adapter->EnumOutputs(o, &output)); o++) {
            DXGI_OUTPUT_DESC desc;
            if (SUCCEEDED(output->GetDesc(&desc)) && desc.Monitor == mon)
                found = output;
            else
                output->Release();
        }
        adapter->Release();
    }
    factory->Release();
    return found;
}

// Paces the frames of an animation with the vertical blank of one monitor, so
// each screen refresh shows exactly one new frame at any refresh rate. Falls
// back to the DWM composition clock, then to a plain sleep.
struct FrameClock {
    IDXGIOutput* output = nullptr;
    DWORD interval = 16;
    int shortWaits = 0;
    bool active = false;

    void Create() {}

    void Destroy() { Stop(); }

    void Start(HMONITOR mon) {
        Stop();
        output = FindOutput(mon);
        interval = 1000 / RefreshRate(mon);
        if (interval < 1)
            interval = 1;
        shortWaits = 0;
        active = true;
    }

    void Stop() {
        active = false;
        if (output)
            output->Release();
        output = nullptr;
    }

    void Wait() {
        LARGE_INTEGER t0, t1, freq;
        QueryPerformanceCounter(&t0);
        if (!(output && SUCCEEDED(output->WaitForVBlank())) && FAILED(DwmFlush()))
            Sleep(interval);
        QueryPerformanceCounter(&t1);
        QueryPerformanceFrequency(&freq);
        // A display that is off or remote can report a blank right away; don't
        // let the loop spin then.
        bool isShort = (t1.QuadPart - t0.QuadPart) * 4000 < freq.QuadPart;
        shortWaits = isShort ? shortWaits + 1 : 0;
        if (shortWaits >= 3)
            Sleep(interval);
    }
};

// While the clock is active: wait for the vertical blank, handle the input that
// arrived meanwhile, draw one frame.
template <typename Callback>
void RunMessageLoop(FrameClock& clock, Callback onFrame) {
    for (;;) {
        if (clock.active)
            clock.Wait();
        else
            MsgWaitForMultipleObjectsEx(0, nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (clock.active)
            onFrame();
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
    if (!IsWindow(hwnd) || IsHungAppWindow(hwnd))
        return false;
    if (IsZoomed(hwnd) || IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);
    RECT wr, fr;
    if (!GetWindowRect(hwnd, &wr))
        return false;
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
    if (IsHungAppWindow(hwnd) || !IsCandidate(hwnd, nullptr))
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
        if (FAILED(DwmRegisterThumbnail(g_chooser, g_wins[i], &g_thumbs[i]))) {
            g_thumbs[i] = nullptr;
            continue;
        }
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
    HGDIOBJ oldFont = SelectObject(dc, g_font);
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
    SelectObject(dc, oldFont);
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
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            int i = HitTile(lp);
            if (i != g_hover) {
                g_hover = i;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_hover != -1) {
                g_hover = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
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

void StopTracking() {
    g_frames.Stop();
    g_tracking = false;
    HideAt(g_bar, g_barShown);
    HideAt(g_preview, g_previewShown);
}

void TrackFrame() {
    // A window closed while it is dragged never reports the end of the drag.
    if (!g_tracking || !IsWindow(g_hwnd)) {
        StopTracking();
        return;
    }
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
    StopTracking();

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
    InitThread();
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
constexpr UINT WM_APP_START = WM_APP + 1;
constexpr UINT WM_APP_STEP = WM_APP + 2;
constexpr UINT WM_APP_END = WM_APP + 3;
constexpr UINT WM_APP_JOY = WM_APP + 4;
constexpr UINT WM_APP_MOUSEHOOK = WM_APP + 5;
constexpr UINT WM_APP_CLOSEITEM = WM_APP + 6;

struct {
    std::atomic<bool> enabled;
    std::atomic<bool> group;
    std::atomic<bool> force;
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
bool g_rawFloating[MAXRAW];
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
// g_mem holds the scene without the thumbstick; it is redrawn only when the
// items or the selection change. Each frame copies the thumbstick area from it
// to g_frame, draws the stick there and copies that area to the screen.
HDC g_mem, g_frame;
HBITMAP g_bmp, g_frameBmp;
SIZE g_bmpSize;
bool g_sceneDirty;
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
    in.ki.dwExtraInfo = INPUT_TAG;
    SendInput(1, &in, sizeof(INPUT));
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    bool floating = false;
    if (hwnd == g_wnd || !IsCandidate(hwnd, &floating))
        return TRUE;
    g_rawFloating[g_rawCount] = floating;
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
    in.mi.dwExtraInfo = INPUT_TAG;
    SendInput(1, &in, sizeof(INPUT));
    SetForegroundWindow(hwnd);
}

bool AltReleased() {
    return GetAsyncKeyState(VK_MENU) >= 0;
}

HWND g_pendingPrimary, g_pendingOthers[MAXMEMBERS];
int g_pendingCount, g_pendingTicks;

// Brings a window to the top without activating it. A plain z-order change is
// ignored when it comes from a process that isn't in the foreground; switching
// "always on top" on and off is not.
void Raise(HWND hwnd) {
    if (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
        return;
    UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, flags);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, flags);
}

bool IsOnTop(HWND hwnd) {
    RECT f;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &f, sizeof(f))))
        return true;
    return RootAt({(f.left + f.right) / 2, (f.top + f.bottom) / 2}) == hwnd;
}

// Shows all windows of the item: the others are raised first, then the primary
// one is activated. A timer checks afterwards that none of them stayed behind.
void ActivateItem(const Item& it) {
    g_pendingCount = 0;
    for (int m = 0; m < it.n; m++)
        if (m != it.primary) {
            if (IsIconic(it.w[m]))
                ShowWindowAsync(it.w[m], SW_SHOWNOACTIVATE);
            Raise(it.w[m]);
            g_pendingOthers[g_pendingCount++] = it.w[m];
        }
    Activate(it.w[it.primary]);
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

void BuildItems() {
    HWND fg = GetAncestor(GetForegroundWindow(), GA_ROOTOWNER);
    for (int i = 1; i < g_rawCount; i++) {
        if (g_raw[i] != fg)
            continue;
        HWND w = g_raw[i];
        HICON icon = g_rawIcons[i];
        bool floating = g_rawFloating[i];
        for (; i > 0; i--) {
            g_raw[i] = g_raw[i - 1];
            g_rawIcons[i] = g_rawIcons[i - 1];
            g_rawFloating[i] = g_rawFloating[i - 1];
        }
        g_raw[0] = w;
        g_rawIcons[0] = icon;
        g_rawFloating[0] = floating;
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
        if (!GetMonitorInfo(mon, &mi))
            continue;
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
        if ((gi >= 0 && emitted[gi]) || (gi < 0 && g_rawFloating[i]))
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
        it.primary = 0;
        for (int m = 0; m < it.n; m++) {
            it.w[m] = g_raw[groups[gi].idx[m]];
            it.r[m] = groups[gi].r[m];
            it.icon[m] = g_rawIcons[groups[gi].idx[m]];
            if (groups[gi].idx[m] == i)
                it.primary = m;
        }
    }
}

// Whether the first item is the window in use. When it isn't, for example with
// the desktop in the foreground, Alt+Tab starts at the first item, not the
// second one.
bool FirstItemIsForeground() {
    HWND fg = GetAncestor(GetForegroundWindow(), GA_ROOTOWNER);
    for (int m = 0; g_count && m < g_items[0].n; m++)
        if (g_items[0].w[m] == fg)
            return true;
    return false;
}

void UpdatePreview() {
    for (HTHUMBNAIL& t : g_thumbs) {
        if (t)
            DwmUnregisterThumbnail(t);
        t = nullptr;
    }
    if (g_sel < 0 || g_sel >= g_count)
        return;
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

void Refresh() {
    HWND key = g_sel < g_count ? g_items[g_sel].w[0] : nullptr;
    int oldSel = g_sel;
    g_rawCount = 0;
    EnumWindows(EnumProc, 0);
    BuildItems();
    if (!g_count) {
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, 0, 0);
        return;
    }
    BuildSlots();
    g_sel = oldSel < g_count ? oldSel : g_count - 1;
    for (int i = 0; i < g_count; i++)
        if (g_items[i].w[0] == key)
            g_sel = i;
    if (g_open)
        UpdatePreview();
    g_sceneDirty = true;
    InvalidateRect(g_wnd, nullptr, FALSE);
}

void Select(int sel) {
    if (sel == g_sel)
        return;
    g_sel = sel;
    if (g_open)
        UpdatePreview();
    g_sceneDirty = true;
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
    g_sceneDirty = true;

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

// Windows 11 dark theme: neutral dark surfaces, soft white text and the light
// blue accent of dark mode. Nothing is pure black or saturated, so the view is
// easy on the eyes at night.
namespace theme {
using Gdiplus::Color;
const Color base(255, 32, 32, 32);          // #202020, window background
const Color baseLow(255, 28, 28, 28);       // #1C1C1C, lower band
const Color divider(255, 48, 48, 48);
const Color card(255, 44, 44, 44);          // #2C2C2C, cards and wheel slices
const Color cardStroke(255, 62, 62, 62);
const Color selected(255, 57, 84, 97);      // accent at 25% over a card
const Color text(255, 255, 255, 255);
const Color textSecondary(255, 204, 204, 204);
const Color textTertiary(255, 154, 154, 154);
const Color accent(255, 96, 205, 255);      // #60CDFF
const Color stickRim(255, 44, 44, 44);
const Color stickDisc(255, 52, 52, 52);
const Color stickWell(255, 38, 38, 38);
}  // namespace theme

// Segoe UI Variable is the Windows 11 font; Windows 10 doesn't have it.
const WCHAR* FontName(const WCHAR* preferred, const WCHAR* fallback) {
    Gdiplus::FontFamily family(preferred);
    return family.IsAvailable() ? preferred : fallback;
}

void Paint(HDC dc, int w, int h) {
    using namespace Gdiplus;
    float span = 360.f / (g_count ? g_count : 1), rm = (g_ri + g_ro) / 2, rh = g_ri - Sc(16);
    float isz = IconSize(), headerIconX = 0, headerIconY = 0, hsz = Sc(28);
    {
        Graphics g(dc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        SolidBrush top(theme::base), bottom(theme::baseLow);
        g.FillRectangle(&top, 0, 0, w, h / 2);
        g.FillRectangle(&bottom, 0, h / 2, w, h - h / 2);
        Pen divider(theme::divider, Sc(1));
        g.DrawLine(&divider, 0.f, h / 2.f, (float)w, h / 2.f);
        if (!g_count || g_sel < 0 || g_sel >= g_count)
            return;

        SolidBrush white(theme::text), dim(theme::textSecondary), faint(theme::textTertiary);
        const WCHAR* regular = FontName(L"Segoe UI Variable Text", L"Segoe UI");
        const WCHAR* semibold = FontName(L"Segoe UI Variable Text Semibold", L"Segoe UI Semibold");
        StringFormat center;
        center.SetAlignment(StringAlignmentCenter);
        center.SetLineAlignment(StringAlignmentCenter);
        center.SetTrimming(StringTrimmingEllipsisCharacter);
        center.SetFormatFlags(StringFormatFlagsNoWrap);

        WCHAR title[TITLE_LEN], text[TITLE_LEN + 20];
        ItemTitle(g_sel, title, TITLE_LEN);
        _snwprintf_s(text, _TRUNCATE, L"%d  \u00B7  %s", g_sel + 1, title);
        Font titleFont(FontName(L"Segoe UI Variable Display Semib", L"Segoe UI Semibold"), Sc(20),
                       FontStyleRegular, UnitPixel);
        RectF titleRect(Sc(80), Sc(40), w - Sc(160), Sc(40)), measured;
        g.MeasureString(text, -1, &titleFont, titleRect, &center, &measured);
        headerIconX = measured.X - hsz - Sc(10);
        headerIconY = titleRect.Y + (titleRect.Height - hsz) / 2;
        g.DrawString(text, -1, &titleFont, titleRect, &center, &white);

        // The preview sits on a card, like the surfaces of the Start menu.
        SolidBrush frame(theme::card);
        Pen frameStroke(theme::cardStroke, Sc(1));
        GraphicsPath framePath;
        AddRoundRect(framePath,
                     RectF(g_preview.left - Sc(4), g_preview.top - Sc(4),
                           g_preview.right - g_preview.left + Sc(8),
                           g_preview.bottom - g_preview.top + Sc(8)),
                     Sc(10));
        g.FillPath(&frame, &framePath);
        g.DrawPath(&frameStroke, &framePath);

        SolidBrush slice(theme::card), selFill(theme::selected);
        float cr = Sc(8), gap = Sc(4), ro = g_ro - cr, ri = g_ri + cr;
        float go = (gap / 2 + cr) / ro * 180 / PI, gi = (gap / 2 + cr) / ri * 180 / PI;
        int fs = g_count <= 6 ? 14 : g_count <= 10 ? 12 : 11;
        Font labelFont(regular, Sc(fs), FontStyleRegular, UnitPixel);
        Font labelBold(semibold, Sc(fs), FontStyleRegular, UnitPixel);
        Font numFont(regular, Sc(11), FontStyleRegular, UnitPixel);
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
                         &center, i == g_sel ? &white : &faint);

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
        SolidBrush pointer(theme::accent);
        g.FillPolygon(&pointer, tri, 3);

        SolidBrush rim(theme::stickRim), disc(theme::stickDisc);
        SolidBrush well(theme::stickWell);
        float rd = rh * 0.9f, rw = rh * 0.62f;
        g.FillEllipse(&rim, g_wx - rh, g_wy - rh, 2 * rh, 2 * rh);
        g.FillEllipse(&disc, g_wx - rd, g_wy - rd, 2 * rd, 2 * rd);
        g.FillEllipse(&well, g_wx - rw, g_wy - rw, 2 * rw, 2 * rw);
    }

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

// The area the thumbstick can reach, with its halo.
RECT StickRect() {
    float r = (g_ri - Sc(16)) * 1.3f + Sc(4);
    return {(LONG)(g_wx - r), (LONG)(g_wy - r), (LONG)(g_wx + r) + 1, (LONG)(g_wy + r) + 1};
}

void PaintStick(HDC dc) {
    using namespace Gdiplus;
    Graphics g(dc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    float rh = g_ri - Sc(16), kr = rh * 0.26f;
    float R = Sc(80), travel = rh - kr * 0.55f;
    float kx = g_wx + g_jx / R * travel, ky = g_wy + g_jy / R * travel;
    Pen shaft(Color(90, 255, 255, 255), Sc(3));
    shaft.SetStartCap(LineCapRound);
    shaft.SetEndCap(LineCapRound);
    g.DrawLine(&shaft, g_wx, g_wy, kx, ky);
    if (g_jx * g_jx + g_jy * g_jy >= R * R * 0.9f) {
        SolidBrush halo(Color(60, 96, 205, 255));
        float hr = kr * 1.6f;
        g.FillEllipse(&halo, kx - hr, ky - hr, 2 * hr, 2 * hr);
    }
    SolidBrush knob(theme::accent);
    g.FillEllipse(&knob, kx - kr, ky - kr, 2 * kr, 2 * kr);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    auto* k = (KBDLLHOOKSTRUCT*)lp;
    if (code != HC_ACTION || k->dwExtraInfo == INPUT_TAG)
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
    } else if (wp == WM_LBUTTONDOWN) {
        g_upToSwallow = WM_LBUTTONUP;
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, 1, 0);
    } else if (wp == WM_RBUTTONDOWN) {
        g_upToSwallow = WM_RBUTTONUP;
        PostMessage(g_wnd, WM_APP_CLOSEITEM, 0, 0);
    } else if (wp == WM_MOUSEWHEEL) {
        PostMessage(g_wnd, WM_APP_STEP, (short)HIWORD(m->mouseData) > 0 ? -1 : 1, 0);
    }
    return 1;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    InitThread();
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
    if (kb)
        UnhookWindowsHookEx(kb);
    return 0;
}

struct Closing {
    HWND w;
    DWORD start;
    bool forced;
    HWND popup;    // last active popup before the window was asked to close
    bool enabled;  // whether the window took input before that
};

Closing g_closing[MAXRAW];
int g_closingCount;

bool ForceClose(HWND hwnd) {
    // The frame of a Microsoft Store app belongs to ApplicationFrameHost.exe,
    // which hosts the frames of all such apps. The app itself owns the core
    // window inside the frame.
    WCHAR cls[64] = L"";
    GetClassName(hwnd, cls, 64);
    if (!lstrcmpW(cls, L"ApplicationFrameWindow")) {
        hwnd = FindWindowEx(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        if (!hwnd)
            return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == GetCurrentProcessId())
        return false;
    HANDLE process = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process)
        return false;
    WCHAR path[MAX_PATH];
    DWORD len = MAX_PATH;
    bool ok = false;
    if (QueryFullProcessImageNameW(process, 0, path, &len)) {
        PCWSTR name = wcsrchr(path, L'\\');
        name = name ? name + 1 : path;
        if (lstrcmpiW(name, L"explorer.exe") && lstrcmpiW(name, L"windhawk.exe") &&
            lstrcmpiW(name, L"ApplicationFrameHost.exe"))
            ok = TerminateProcess(process, 1);
    }
    CloseHandle(process);
    return ok;
}

void PointTo(HWND hwnd) {
    HWND popup = GetLastActivePopup(hwnd);
    if (g_open) {
        g_session = false;
        KillTimer(g_wnd, 1);
        Close();
        g_count = 0;
    }
    Activate(popup ? popup : hwnd);
}

void CheckClosing() {
    DWORD now = GetTickCount();
    bool changed = false;
    for (int i = 0; i < g_closingCount;) {
        Closing& c = g_closing[i];
        DWORD cloaked = 0;
        if (IsWindow(c.w))
            DwmGetWindowAttribute(c.w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        bool gone = !IsWindow(c.w) || !IsWindowVisible(c.w) || cloaked;
        HWND popup = gone ? nullptr : GetLastActivePopup(c.w);
        bool blocked = !gone && !IsWindowEnabled(c.w);
        bool asks = !gone && (blocked || popup != c.w);
        // Only a question that appeared because of the close request ends the
        // app: a dialog window that wasn't there before, or the window being
        // blocked by one since then. A popup that was already open, such as a
        // Find dialog, or a window that is just slow to close doesn't.
        bool question = asks && ((blocked && c.enabled) ||
                                 (popup != c.w && popup != c.popup && IsWindowVisible(popup)));
        DWORD age = now - c.start;
        bool act = !c.forced && ((asks && age > 300) || (!asks && age > 5000));
        if (act && question && settings.force && ForceClose(c.w)) {
            c.forced = true;
            c.start = now;
            i++;
            continue;
        }
        if (act && asks) {
            HWND w = c.w;
            g_closingCount = 0;
            PointTo(w);
            break;
        }
        if (gone || act || (c.forced && age > 3000)) {
            g_closing[i] = g_closing[--g_closingCount];
            changed |= gone;
            continue;
        }
        i++;
    }
    if (changed && g_open)
        Refresh();
    if (!g_closingCount)
        KillTimer(g_wnd, 4);
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
        RECT stick = StickRect();
        RedrawWindow(g_wnd, &stick, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
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
            g_sel = g_count < 2 ? 0 : wp ? g_count - 1 : FirstItemIsForeground() ? 1 : 0;
            if (settings.delay > 0)
                SetTimer(hwnd, 1, settings.delay, nullptr);
            else
                Open();
            return 0;
        case WM_TIMER:
            if (wp == 4) {
                CheckClosing();
                return 0;
            }
            if (wp == 3) {
                if (GetForegroundWindow() != g_pendingPrimary && ++g_pendingTicks < 15)
                    return 0;
                KillTimer(hwnd, 3);
                for (int k = 0; k < g_pendingCount; k++)
                    if (!IsOnTop(g_pendingOthers[k]))
                        Raise(g_pendingOthers[k]);
                return 0;
            }
            if (wp != 1)
                return 0;
            KillTimer(hwnd, 1);
            if (g_session && AltReleased()) {
                g_session = false;
                PostMessage(hwnd, WM_APP_END, 0, 0);
            } else if (g_session && g_count) {
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
            // The view can stay open for a moment with no items left, after the
            // last window was closed with a right click.
            if (!g_open || !g_count)
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
        case WM_APP_CLOSEITEM:
            if (g_open && g_sel < g_count) {
                for (int m = 0; m < g_items[g_sel].n; m++) {
                    HWND w = g_items[g_sel].w[m];
                    bool known = false;
                    for (int k = 0; k < g_closingCount; k++)
                        known |= g_closing[k].w == w;
                    if (!known && g_closingCount < MAXRAW)
                        g_closing[g_closingCount++] = {w, GetTickCount(), false,
                                                       GetLastActivePopup(w),
                                                       IsWindowEnabled(w) != FALSE};
                    PostMessage(w, WM_CLOSE, 0, 0);
                }
                SetTimer(hwnd, 4, 100, nullptr);
            }
            return 0;
        case WM_APP_END:
            KillTimer(hwnd, 1);
            Close();
            if (wp && g_sel < g_count)
                ActivateItem(g_items[g_sel]);
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
            if (!g_mem) {
                g_mem = CreateCompatibleDC(dc);
                g_frame = CreateCompatibleDC(dc);
            }
            if (rc.right != g_bmpSize.cx || rc.bottom != g_bmpSize.cy) {
                HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
                HBITMAP frameBmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
                SelectObject(g_mem, bmp);
                SelectObject(g_frame, frameBmp);
                if (g_bmp)
                    DeleteObject(g_bmp);
                if (g_frameBmp)
                    DeleteObject(g_frameBmp);
                g_bmp = bmp;
                g_frameBmp = frameBmp;
                g_bmpSize = {rc.right, rc.bottom};
                g_sceneDirty = true;
            }
            if (g_sceneDirty) {
                g_sceneDirty = false;
                Paint(g_mem, rc.right, rc.bottom);
            }
            const RECT& p = ps.rcPaint;
            int pw = p.right - p.left, ph = p.bottom - p.top;
            BitBlt(g_frame, p.left, p.top, pw, ph, g_mem, p.left, p.top, SRCCOPY);
            if (g_count && g_sel >= 0 && g_sel < g_count)
                PaintStick(g_frame);
            BitBlt(dc, p.left, p.top, pw, ph, g_frame, p.left, p.top, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD WINAPI ThreadProc(LPVOID) {
    InitThread();
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
    bool hooked = StartThread(HookThreadProc, g_hookThread, g_hookThreadId);

    g_frames.Create();
    RunMessageLoop(g_frames, OnFrame);

    Close();
    g_frames.Destroy();
    if (hooked)
        StopThread(g_hookThread, g_hookThreadId);
    KillTimer(g_wnd, 1);
    KillTimer(g_wnd, 3);
    KillTimer(g_wnd, 4);
    g_closingCount = 0;
    g_count = 0;
    DestroyWindow(g_wnd);
    if (g_mem)
        DeleteDC(g_mem);
    if (g_frame)
        DeleteDC(g_frame);
    if (g_bmp)
        DeleteObject(g_bmp);
    if (g_frameBmp)
        DeleteObject(g_frameBmp);
    g_mem = g_frame = nullptr;
    g_bmp = g_frameBmp = nullptr;
    g_bmpSize = {};
    UnregisterClass(wc.lpszClassName, g_inst);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

void LoadSettings() {
    settings.enabled = Wh_GetIntSetting(L"radialSwitcher");
    settings.group = Wh_GetIntSetting(L"groupSplitWindows");
    settings.force = Wh_GetIntSetting(L"forceClose");
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

namespace scroll {

using namespace common;

enum { MODE_OFF, MODE_INPUT, MODE_MESSAGES };
constexpr UINT WM_APP_SCROLL = WM_APP + 1;
constexpr UINT WM_APP_MOUSEHOOK = WM_APP + 2;
constexpr WORD CTRL_KEYS[2] = {VK_LCONTROL, VK_RCONTROL};

struct Job {
    int n, src, delta;
    bool horizontal, shift;
    POINT pt;
    HWND w[MAXMEMBERS];
};

std::atomic<int> g_mode;
HANDLE g_thread, g_worker;
DWORD g_threadId, g_workerId;
HINSTANCE g_inst;

// Used by the hook thread only.
// g_ctrl: the Ctrl keys that are down, as the keyboard hook saw them.
// g_held: the key is down, but the apps were told that it is up (HideCtrl).
// The mouse hook is installed only while a Ctrl key is down.
bool g_ctrl[2], g_held[2], g_wantMouse;
HHOOK g_mouse;
DWORD g_lastCheck;
int g_n;
HWND g_members[MAXMEMBERS];
RECT g_zones[MAXMEMBERS], g_area;

bool Down(int vk) {
    return GetAsyncKeyState(vk) < 0;
}

bool AnyHeld() {
    return g_held[0] || g_held[1];
}

void AddKey(INPUT* in, int& k, WORD vk, bool up, bool extended) {
    in[k] = {};
    in[k].type = INPUT_KEYBOARD;
    in[k].ki.wVk = vk;
    in[k].ki.dwExtraInfo = INPUT_TAG;
    in[k++].ki.dwFlags = (up ? KEYEVENTF_KEYUP : 0) | (extended ? KEYEVENTF_EXTENDEDKEY : 0);
}

LONG Normalize(LONG v, int origin, int size) {
    if (size < 1)
        size = 1;
    return (LONG)(((LONGLONG)(v - origin) * 65536 + size - 1) / size);
}

void AddMouse(INPUT* in, int& k, POINT p, DWORD flags, int data) {
    in[k] = {};
    in[k].type = INPUT_MOUSE;
    in[k].mi.dx = Normalize(p.x, GetSystemMetrics(SM_XVIRTUALSCREEN),
                            GetSystemMetrics(SM_CXVIRTUALSCREEN));
    in[k].mi.dy = Normalize(p.y, GetSystemMetrics(SM_YVIRTUALSCREEN),
                            GetSystemMetrics(SM_CYVIRTUALSCREEN));
    in[k].mi.mouseData = (DWORD)data;
    in[k].mi.dwExtraInfo = INPUT_TAG;
    in[k++].mi.dwFlags =
        flags | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
}

// Gives the held keys back to the apps. SendInput is processed before the hook
// that calls it returns, so the apps get Ctrl before the event that follows.
void ShowCtrl() {
    INPUT in[2];
    int k = 0;
    for (int i = 0; i < 2; i++)
        if (g_held[i]) {
            AddKey(in, k, CTRL_KEYS[i], false, i == 1);
            g_held[i] = false;
        }
    if (k)
        SendInput(k, in, sizeof(INPUT));
}

// Takes the Ctrl keys the apps already know about away from them. The mask key
// keeps it from looking like a single Ctrl tap.
bool HideCtrl() {
    INPUT in[4];
    int k = 0;
    AddKey(in, k, VK_MASK, false, false);
    AddKey(in, k, VK_MASK, true, false);
    for (int i = 0; i < 2; i++)
        if (Down(CTRL_KEYS[i])) {
            AddKey(in, k, CTRL_KEYS[i], true, i == 1);
            g_held[i] = true;
        }
    if (!AnyHeld())
        return false;
    SendInput(k, in, sizeof(INPUT));
    return true;
}

bool FindGroup(POINT pt) {
    g_n = FindSplitGroup(pt, g_members, g_zones);
    if (g_n < 2) {
        g_n = 0;
        return false;
    }
    g_area = g_zones[0];
    for (int z = 1; z < g_n; z++)
        UnionRect(&g_area, &g_area, &g_zones[z]);
    return true;
}

// The zone of the known group that has the window at pt, or -1. With check, the
// group also has to be still in place.
int SourceZone(POINT pt, bool check) {
    HWND root = RootAt(pt);
    int src = -1;
    for (int z = 0; z < g_n; z++) {
        if (check && !FrameMatches(g_members[z], g_zones[z]))
            return -1;
        if (g_members[z] == root)
            src = z;
    }
    return src;
}

HWND ChildAt(HWND root, POINT screen) {
    HWND cur = root;
    for (int depth = 0; depth < 32; depth++) {
        POINT c = screen;
        ScreenToClient(cur, &c);
        HWND next = ChildWindowFromPointEx(
            cur, c, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
        if (!next || next == cur)
            break;
        cur = next;
    }
    return cur;
}

bool Frame(HWND hwnd, RECT* r) {
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, r, sizeof(*r))) &&
           r->right > r->left && r->bottom > r->top;
}

// Worker thread: repeats the wheel in the other windows of the group. The
// window under the mouse got the original wheel event.
void Scroll(const Job& job) {
    // The apps got Ctrl back in the meantime; the wheel would zoom them.
    if (Down(VK_CONTROL))
        return;
    RECT s;
    if (!Frame(job.w[job.src], &s))
        return;
    float rx = (float)(job.pt.x - s.left) / (s.right - s.left);
    float ry = (float)(job.pt.y - s.top) / (s.bottom - s.top);
    rx = fminf(fmaxf(rx, 0.02f), 0.98f);
    ry = fminf(fmaxf(ry, 0.02f), 0.98f);

    UINT routing = 2;
    SystemParametersInfo(SPI_GETMOUSEWHEELROUTING, 0, &routing, 0);
    bool messages = g_mode == MODE_MESSAGES || routing == 0;
    DWORD flags = job.horizontal ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL;
    UINT msg = job.horizontal || job.shift ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL;
    int msgDelta = job.shift && !job.horizontal ? -job.delta : job.delta;

    INPUT in[MAXMEMBERS + 1];
    int k = 0;
    for (int z = 0; z < job.n; z++) {
        RECT r;
        if (z == job.src || !Frame(job.w[z], &r))
            continue;
        POINT t{r.left + (LONG)(rx * (r.right - r.left)), r.top + (LONG)(ry * (r.bottom - r.top))};
        // Real input goes to whatever is at the point, so it is used only when
        // that is the window to scroll.
        if (!messages && RootAt(t) == job.w[z])
            AddMouse(in, k, t, flags, job.delta);
        else
            PostMessage(ChildAt(job.w[z], t), msg, MAKEWPARAM(0, msgDelta),
                        MAKELPARAM(t.x, t.y));
    }
    if (!k)
        return;
    POINT cur, now;
    if (!GetCursorPos(&cur))
        return;
    AddMouse(in, k, cur, 0, 0);
    SendInput(k, in, sizeof(INPUT));
    if (GetCursorPos(&now) && (now.x != cur.x || now.y != cur.y) && abs(now.x - cur.x) <= 1 &&
        abs(now.y - cur.y) <= 1)
        SetCursorPos(cur.x, cur.y);
}

bool SameTarget(const Job& a, const Job& b) {
    if (a.n != b.n || a.src != b.src || a.horizontal != b.horizontal || a.shift != b.shift)
        return false;
    for (int z = 0; z < a.n; z++)
        if (a.w[z] != b.w[z])
            return false;
    return true;
}

int ClampDelta(int delta) {
    return delta > 32000 ? 32000 : delta < -32000 ? -32000 : delta;
}

DWORD WINAPI WorkerProc(LPVOID) {
    InitThread();
    MSG msg;
    bool quit = false;
    while (!quit && GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != WM_APP_SCROLL)
            continue;
        Job* job = (Job*)msg.lParam;
        // When wheel events arrive faster than they are repeated, the waiting
        // ones are merged, so the scrolling never falls behind.
        MSG next;
        while (PeekMessage(&next, nullptr, WM_APP_SCROLL, WM_APP_SCROLL, PM_REMOVE)) {
            if (next.message != WM_APP_SCROLL) {
                quit = next.message == WM_QUIT;
                break;
            }
            Job* later = (Job*)next.lParam;
            if (SameTarget(*job, *later)) {
                later->delta = ClampDelta(later->delta + job->delta);
            } else {
                Scroll(*job);
            }
            delete job;
            job = later;
        }
        Scroll(*job);
        delete job;
    }
    while (PeekMessage(&msg, nullptr, WM_APP_SCROLL, WM_APP_SCROLL, PM_REMOVE) &&
           msg.message == WM_APP_SCROLL)
        delete (Job*)msg.lParam;
    return 0;
}

void PostJob(const MSLLHOOKSTRUCT* m, bool horizontal, int src) {
    Job* job = new Job{};
    job->n = g_n;
    job->src = src;
    job->delta = (short)HIWORD(m->mouseData);
    job->horizontal = horizontal;
    job->shift = Down(VK_SHIFT);
    job->pt = m->pt;
    for (int z = 0; z < g_n; z++)
        job->w[z] = g_members[z];
    if (!PostThreadMessage(g_workerId, WM_APP_SCROLL, 0, (LPARAM)job))
        delete job;
}

void WantMouseHook(bool on) {
    if (on == g_wantMouse)
        return;
    g_wantMouse = on;
    PostThreadMessage(GetCurrentThreadId(), WM_APP_MOUSEHOOK, 0, 0);
}

// The hooks only decide and hand the scrolling over to the worker thread, so
// they return quickly. Ctrl itself always reaches the apps: it is taken away
// from them (HideCtrl) only at the first Ctrl + wheel over a split.
LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    auto* kb = (KBDLLHOOKSTRUCT*)lp;
    if (code != HC_ACTION || kb->dwExtraInfo == INPUT_TAG)
        return CallNextHookEx(nullptr, code, wp, lp);
    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    int ctrl = kb->vkCode == VK_LCONTROL ? 0 : kb->vkCode == VK_RCONTROL ? 1 : -1;

    if (ctrl >= 0) {
        g_ctrl[ctrl] = down;
        WantMouseHook(g_ctrl[0] || g_ctrl[1]);
        if (!g_held[ctrl])
            return CallNextHookEx(nullptr, code, wp, lp);
        // The apps already think the key is up: its key repeats and its
        // release aren't for them.
        if (!down)
            g_held[ctrl] = false;
        return 1;
    }

    bool shift = kb->vkCode == VK_LSHIFT || kb->vkCode == VK_RSHIFT || kb->vkCode == VK_SHIFT;
    if (down && !shift && AnyHeld())
        ShowCtrl();
    return CallNextHookEx(nullptr, code, wp, lp);
}

LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp) {
    auto* m = (MSLLHOOKSTRUCT*)lp;
    if (code != HC_ACTION || m->dwExtraInfo == INPUT_TAG)
        return CallNextHookEx(nullptr, code, wp, lp);

    bool held = AnyHeld();
    if (!held && !Down(VK_CONTROL)) {
        // No Ctrl key is down after all, for example because its release went
        // to the lock screen: stop watching the mouse.
        if (GetTickCount() - g_lastCheck > 1000) {
            g_lastCheck = GetTickCount();
            g_ctrl[0] = g_ctrl[1] = false;
            WantMouseHook(false);
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    if (wp == WM_MOUSEWHEEL || wp == WM_MOUSEHWHEEL) {
        // Not with Alt or Win: that includes AltGr, which Windows reports as
        // Ctrl + Alt.
        if (radial::g_open || (!held && (Down(VK_MENU) || Down(VK_LWIN) || Down(VK_RWIN))))
            return CallNextHookEx(nullptr, code, wp, lp);
        int src = held ? SourceZone(m->pt, true) : -1;
        if (src < 0 && FindGroup(m->pt))
            src = SourceZone(m->pt, false);
        if (src < 0 || (!held && !HideCtrl())) {
            ShowCtrl();
            return CallNextHookEx(nullptr, code, wp, lp);
        }
        PostJob(m, wp == WM_MOUSEHWHEEL, src);
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    if (!held)
        return CallNextHookEx(nullptr, code, wp, lp);
    bool button = wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN || wp == WM_MBUTTONDOWN ||
                  wp == WM_XBUTTONDOWN;
    if (button || (wp == WM_MOUSEMOVE && !PtInRect(&g_area, m->pt)))
        ShowCtrl();
    return CallNextHookEx(nullptr, code, wp, lp);
}

DWORD WINAPI ThreadProc(LPVOID) {
    InitThread();
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)ThreadProc, &g_inst);
    g_ctrl[0] = g_ctrl[1] = g_held[0] = g_held[1] = g_wantMouse = false;
    g_mouse = nullptr;
    g_n = 0;
    HHOOK keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, g_inst, 0);
    Wh_Log(L"Sync scroll keyboard hook %s", keyboard ? L"installed" : L"failed");
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != WM_APP_MOUSEHOOK)
            continue;
        if (g_wantMouse && !g_mouse) {
            g_mouse = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, g_inst, 0);
        } else if (!g_wantMouse && g_mouse) {
            UnhookWindowsHookEx(g_mouse);
            g_mouse = nullptr;
        }
    }
    if (g_mouse)
        UnhookWindowsHookEx(g_mouse);
    g_mouse = nullptr;
    if (keyboard)
        UnhookWindowsHookEx(keyboard);
    ShowCtrl();
    return 0;
}

void LoadSettings() {
    auto value = WindhawkUtils::StringSetting::make(L"syncScroll");
    g_mode = !lstrcmpW(value.get(), L"disabled")   ? MODE_OFF
             : !lstrcmpW(value.get(), L"messages") ? MODE_MESSAGES
                                                   : MODE_INPUT;
}

bool Start() {
    if (!StartThread(WorkerProc, g_worker, g_workerId))
        return false;
    if (StartThread(ThreadProc, g_thread, g_threadId))
        return true;
    StopThread(g_worker, g_workerId);
    return false;
}

void Stop() {
    StopThread(g_thread, g_threadId);
    StopThread(g_worker, g_workerId);
}

}  // namespace scroll

bool g_splitRunning, g_radialRunning, g_scrollRunning;

void LoadSettings() {
    split::LoadSettings();
    radial::LoadSettings();
    scroll::LoadSettings();
    bool scrollOn = scroll::g_mode != scroll::MODE_OFF;
    if (scrollOn && !g_scrollRunning) {
        g_scrollRunning = scroll::Start();
    } else if (!scrollOn && g_scrollRunning) {
        scroll::Stop();
        g_scrollRunning = false;
    }
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
    Wh_Log(L"Started: split layouts %d, radial Alt+Tab %d, sync scroll %d", g_splitRunning,
           g_radialRunning, g_scrollRunning);
    return TRUE;
}

void WhTool_ModUninit() {
    split::SetDocking(false);
    if (g_scrollRunning)
        scroll::Stop();
    if (g_radialRunning)
        radial::Stop();
    if (g_splitRunning)
        split::Stop();
    Wh_Log(L"Stopped");
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

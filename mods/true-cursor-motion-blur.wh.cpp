// ==WindhawkMod==
// @id              true-cursor-motion-blur
// @name            True Cursor Motion Blur
// @description     A real motion blur for the mouse pointer: fading copies of your own cursor along its path
// @version         1.0
// @author          sidlikesgrapess
// @github          https://github.com/sidlikesgrapess
// @homepage        https://github.com/sidlikesgrapess/CursorMotionBlur
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32 -lshcore -lwinmm
// @license         Unlicense
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# True Cursor Motion Blur
A live motion blur for the mouse pointer, everywhere in Windows. While the
mouse moves, fading copies of your actual cursor are drawn along its recent
path, so it looks like a camera motion blur rather than a trail. Unlike the
smear ribbon of [Cursor Motion Blur](https://windhawk.net/mods/cursor-motion-blur)
or the particles of [Mouse Trail](https://windhawk.net/mods/mouse-trail), the
blur is made of your own cursor picture.

![Demo](https://raw.githubusercontent.com/sidlikesgrapess/CursorMotionBlur/main/assets/demo.gif)

## Features
- Uses your real cursor: your scheme, size and colour, and whatever shape it
  has right now (arrow, text, hand...).
- Right size on every monitor, including monitors with different scaling.
- Optionally hides the real pointer during very fast movement so only the blur
  shows; it comes back about 25 ms after you slow down.
- Pauses itself while a fullscreen app or game is in front.
- Runs in its own small Windhawk process: nothing is injected into other
  programs.

## Settings
- **Trail opacity**: how solid the blur is right behind the pointer. It always
  fades to nothing at the end of the trail.
- **Trail length**: how far back in time the blur reaches, in ms. Longer means
  a longer streak.
- **Hide the real cursor when moving very fast** (off by default): while you
  move fast, the system cursors are swapped for an invisible one, and your own
  cursors are put back as soon as you slow down and when the mod is disabled.
- **Hide above speed**: the pointer speed that hides the real cursor, in cm/s
  as measured on your main monitor. It is the same hand speed on every
  monitor.
- **Pause in fullscreen apps and games**: no blur while a fullscreen app is in
  front.

## Resource use
- No CPU while the mouse is still; typically well under 1% while it moves, and
  about 1.5 MB of memory at the default settings.
- While an administrator window (Task Manager, Windhawk...) is in front,
  Windows gives the mod no mouse input, so it checks the cursor on a timer.

## Troubleshooting
- **The cursor stays invisible** (only possible with hiding on, e.g. if the
  process was killed mid-flick). Disable and re-enable the mod, or open
  Settings > Bluetooth & devices > Mouse > Additional mouse settings > Pointers
  and click OK.
- **Another tool's custom cursors get reset.** Restoring after hiding reloads
  the Windows cursor scheme, so turn hiding off if you use a tool that swaps
  the system cursors, such as the Shake to Find Cursor or macOS magnifying
  cursor mods.
- **No blur over the Start menu, Search or Task View.** Windows keeps those
  above every app's windows, including the blur.
- **No blur in a game or video.** That is the fullscreen pause; turn off
  "Pause in fullscreen apps and games" if you want it there.
- **The cursor hides too often or not at all.** Raise or lower "Hide above
  speed", or turn hiding off.
- **The blur seems slightly behind the pointer at high speed.** Windows draws
  the real pointer more directly than any window can be drawn, so every
  overlay is 1-2 frames behind. Hiding the real cursor when fast hides this.
- A standalone app version (no Windhawk needed) is on the
  [homepage](https://github.com/sidlikesgrapess/CursorMotionBlur).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacity: 92
  $name: Trail opacity (%)
  $description: 1-100. How solid the blur is right behind the pointer.
- trailMs: 30
  $name: Trail length (ms)
  $description: 10-150. How far back in time the blur reaches.
- hideWhenFast: false
  $name: Hide the real cursor when moving very fast
  $description: >-
    Swaps the system cursors for an invisible one while you move fast, and
    restores your cursor scheme afterwards.
- hideSpeed: 40
  $name: Hide above speed (cm/s)
  $description: 20-300. Measured on the main monitor; the same hand speed on every monitor.
- pauseInFullscreen: true
  $name: Pause in fullscreen apps and games
*/
// ==/WindhawkModSettings==

// How it works. Three threads:
//   sampler  reads the cursor position on every raw mouse report and decides when the real cursor should be hidden
//   hider    swaps the system cursors for an invisible one and back (restoring takes ~20 ms, so it runs on its own)
//   render   works out the fading copies along the recent path, blends them by hand into one bitmap and shows it in a
//            click-through layered window; it owns that window, which also receives the raw mouse input

#include <windows.h>
#include <atomic>
#include <mmsystem.h>
#include <shellapi.h>
#include <sddl.h>
#include <shellscalingapi.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define HOLD_MS        10      // how long after the last fast moment the cursor may come back
#define SHOW_WINDOW_MS 12      // the speed that brings the cursor back is measured over just this many ms
// ~px of travel one copy covers at cursor size 32 (keeps the opacity independent of copy density)
#define COVER_PX       10.0
#define MAX_SAMPLES    512
#define MAX_COPIES     100     // the most copies spread along the trail in one frame
#define COPY_BUF       (MAX_SAMPLES + MAX_COPIES)
#define MAX_SPRITES    16      // cursor pictures kept before the cache is emptied
#define WM_APP_PAUSED  (WM_APP + 1)

struct Sample { int x, y; LONGLONG t; };
struct Sprite { int w, h, hx, hy, id; UINT32* px; };   // cursor picture at its own size: premultiplied BGRA, hotspot
struct Copy { int x, y; float a; };                   // one copy: top-left relative to the frame, opacity 0-1

// settings (written by WhTool_ModSettingsChanged, read by the threads)
static std::atomic<int> g_opacity, g_trailMs, g_hideWhenFast, g_hideSpeedCm, g_pauseInFullscreen;

// shared between the threads (guarded by g_gate)
static CRITICAL_SECTION g_gate;
static Sample g_hist[MAX_SAMPLES];
static int g_histN;
static HCURSOR g_curHandle;
static bool g_curVisible, g_hideWanted;
static int g_monCursor = 32;          // cursor width on the monitor the cursor is on
static std::atomic<int> g_monGap{7};     // ms between frames: half a refresh of the monitor the cursor is on
static std::atomic<bool> g_blankActive;  // the system cursors are currently swapped for an invisible one
static std::atomic<bool> g_drawFailed;   // the overlay could not draw: then never hide the real cursor
static std::atomic<bool> g_monStale, g_spritesStale;   // display or cursor settings changed
static std::atomic<bool> g_recheck;      // settings or the foreground window changed: check for fullscreen apps now
static std::atomic<bool> g_paused;       // a fullscreen app is in front
// the window in front runs with more rights: Windows sends us no raw input then
static std::atomic<bool> g_adminFront;
static std::atomic<bool> g_stop;
static std::atomic<HWND> g_wnd;          // the overlay window (owned by the render thread)

static HANDLE g_moved, g_mouseWake, g_hideChanged;   // auto-reset events
static HANDLE g_ready;                               // set when the render thread has set up (or failed to)
static HANDLE g_threads[3];
static LARGE_INTEGER g_freq, g_start;

static LONGLONG Now() {
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (t.QuadPart - g_start.QuadPart) * 1000 / g_freq.QuadPart;
}

static double Dist(Sample a, Sample b) {
    double dx = b.x - a.x, dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

static void LoadSettings() {
    g_opacity = Clamp(Wh_GetIntSetting(L"opacity"), 1, 100);
    g_trailMs = Clamp(Wh_GetIntSetting(L"trailMs"), 10, 150);
    g_hideWhenFast = Wh_GetIntSetting(L"hideWhenFast") != 0;
    g_hideSpeedCm = Clamp(Wh_GetIntSetting(L"hideSpeed"), 20, 300);
    g_pauseInFullscreen = Wh_GetIntSetting(L"pauseInFullscreen") != 0;
}

// A monitor's refresh rate.
static int RefreshRate(HMONITOR mon) {
    MONITORINFOEXW mi;
    mi.cbSize = sizeof(mi);
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (GetMonitorInfoW(mon, &mi) && EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
        dm.dmDisplayFrequency > 1)
        return dm.dmDisplayFrequency;
    return 60;
}

// A monitor's physical pixel density, from the size Windows reports for it (its DPI setting if that looks wrong).
static double PixelsPerCm(HMONITOR mon, UINT dpi) {
    double fallback = dpi / 2.54 < 20 ? 20 : dpi / 2.54;
    MONITORINFOEXW mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) return fallback;
    HDC dc = CreateDCW(L"DISPLAY", mi.szDevice, nullptr, nullptr);
    if (!dc) return fallback;
    int mm = GetDeviceCaps(dc, HORZSIZE), px = GetDeviceCaps(dc, HORZRES);
    DeleteDC(dc);
    return mm >= 150 && mm <= 2500 && px >= 320 ? px / (mm / 10.0) : fallback;
}

static bool FullscreenAppRunning() {
    QUERY_USER_NOTIFICATION_STATE s;
    return SHQueryUserNotificationState(&s) == S_OK &&
           (s == QUNS_BUSY || s == QUNS_RUNNING_D3D_FULL_SCREEN || s == QUNS_PRESENTATION_MODE);
}

// Width of the arrow as Windows loaded it: at the system DPI, and with the Accessibility pointer size applied.
static int ArrowSize() {
    int size = 0;
    ICONINFO ii;
    if (GetIconInfo(LoadCursorW(nullptr, IDC_ARROW), &ii)) {
        BITMAP bm;
        if (GetObjectW(ii.hbmColor ? ii.hbmColor : ii.hbmMask, sizeof(bm), &bm)) size = bm.bmWidth;
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        DeleteObject(ii.hbmMask);
    }
    return size > 0 ? size : GetSystemMetrics(SM_CXCURSOR);
}

static void ReloadCursors() { SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0); }

// Whether our invisible cursor is in place, also kept in the mod's storage: if the process is ever killed while hiding,
// the next start knows to put the user's cursors back (and otherwise leaves the cursor scheme alone). That is a storage
// write per hide and per restore, only while hiding is switched on: a deliberate trade for being able to recover.
// The flag is per user, as the storage is shared by everyone signed in on the machine.
static WCHAR g_hiddenKey[256] = L"cursorsHidden";

static void SetBlank(bool on) {
    g_blankActive = on;
    Wh_SetIntValue(g_hiddenKey, on);
}

// ---- sampler ----

static DWORD WINAPI SampleThread(void*) {
    HMONITOR lastMon = nullptr;
    int arrow = 32;
    double hidePxPerCm = 38;   // turns the hide speed (cm/s on the main monitor) into px/s on the monitor in use
    LONGLONG lastMove = 0, lastFast = 0, lastFullscreenCheck = -1000;
    bool paused = false, timerHigh = false;
    while (!g_stop) {
        if (g_recheck.exchange(false)) lastFullscreenCheck = -1000;
        LONGLONG checkedAt = Now();
        if (checkedAt - lastFullscreenCheck >= 500) {
            lastFullscreenCheck = checkedAt;
            bool was = paused;
            paused = g_pauseInFullscreen && FullscreenAppRunning();
            g_paused = paused;
            if (paused != was) {
                Wh_Log(L"Paused for a fullscreen app: %d", paused);
                // stop listening to the mouse during the game
                if (g_wnd) PostMessageW(g_wnd, WM_APP_PAUSED, paused, 0);
            }
        }
        if (paused) {
            EnterCriticalSection(&g_gate);
            g_curVisible = false;
            g_histN = 0;
            if (g_hideWanted) { g_hideWanted = false; SetEvent(g_hideChanged); }
            LeaveCriticalSection(&g_gate);
            if (timerHigh) { timeEndPeriod(1); timerHigh = false; }
            // woken when the foreground window changes; the timeout catches an app leaving fullscreen in place
            WaitForSingleObject(g_mouseWake, 1000);
            continue;
        }

        CURSORINFO ci;
        ci.cbSize = sizeof(ci);
        bool showing = GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING);
        // Crossing to another monitor: measure it here, outside the lock, as some of these calls are slow.
        HMONITOR mon = showing ? MonitorFromPoint(ci.ptScreenPos, MONITOR_DEFAULTTONEAREST) : lastMon;
        bool stale = showing && g_monStale.exchange(false);
        bool newMon = showing && (mon != lastMon || stale);
        int monCursor = 32;
        if (newMon) {
            lastMon = mon;
            UINT dpi = 96, mainDpi = 96, dy;
            GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpi, &dy);
            if (!g_blankActive) arrow = ArrowSize();   // while hiding, the arrow is our invisible one
            monCursor = MulDiv(arrow, dpi, GetDpiForSystem());
            int hz = RefreshRate(mon);
            // The hide speed is set in cm/s as measured on the main monitor. Windows moves the pointer further on a
            // monitor with more scaling (twice the pixels at 200%), so scale it by the DPI to hide at the same hand
            // speed on every monitor.
            POINT origin = {0, 0};
            HMONITOR mainMon = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
            GetDpiForMonitor(mainMon, MDT_EFFECTIVE_DPI, &mainDpi, &dy);
            hidePxPerCm = PixelsPerCm(mainMon, mainDpi) * dpi / mainDpi;
            // Half a refresh: frames are not timed to the screen's refreshes, so a whole one would leave the blur
            // fresh on some refreshes and almost a refresh old on others, and it would jitter against the pointer.
            g_monGap = 500 / hz > 3 ? 500 / hz : 3;
        }

        EnterCriticalSection(&g_gate);
        if (!showing) {
            g_curVisible = false;
            g_histN = 0;
            if (g_hideWanted) { g_hideWanted = false; SetEvent(g_hideChanged); }
        } else {
            g_curVisible = true;
            if (!g_blankActive) g_curHandle = ci.hCursor;   // keep the real cursor's picture while it is hidden
            bool changed = g_histN == 0 || g_hist[g_histN - 1].x != ci.ptScreenPos.x ||
                           g_hist[g_histN - 1].y != ci.ptScreenPos.y;

            if (newMon) {   // drop the trail so it doesn't smear across the gap between monitors
                g_histN = 0;
                g_monCursor = monCursor;
            }

            LONGLONG now = Now();
            if (changed && g_histN && g_hist[g_histN - 1].t == now) {   // several reports within a ms: keep the newest
                g_hist[g_histN - 1].x = ci.ptScreenPos.x;
                g_hist[g_histN - 1].y = ci.ptScreenPos.y;
            } else if (changed || g_histN == 0 || now - g_hist[g_histN - 1].t >= 4) {
                if (g_histN == MAX_SAMPLES) {
                    memmove(g_hist, g_hist + 1, (MAX_SAMPLES - 1) * sizeof(Sample));
                    g_histN--;
                }
                g_hist[g_histN++] = {ci.ptScreenPos.x, ci.ptScreenPos.y, now};
            }
            if (changed) { lastMove = now; SetEvent(g_moved); }   // new mouse position: draw now
            int drop = 0;
            while (g_histN - drop > 1 && now - g_hist[drop].t > g_trailMs) drop++;
            if (drop) {
                memmove(g_hist, g_hist + drop, (g_histN - drop) * sizeof(Sample));
                g_histN -= drop;
            }

            // speed over the trail window -> hide the real cursor when very fast
            LONGLONG dt = g_hist[g_histN - 1].t - g_hist[0].t;
            bool wasHidden = g_hideWanted;
            if (!g_hideWhenFast || g_drawFailed) {
                g_hideWanted = false;
            } else if (dt >= 8) {
                // Hiding looks at the whole trail window (steady). Bringing the cursor back looks at only the last few ms
                // (from sample j on), so it returns as soon as the mouse stops or slows, not a whole trail later.
                int j = g_histN - 1;
                while (j > 0 && now - g_hist[j - 1].t <= SHOW_WINDOW_MS) j--;
                double path = 0, recentPath = 0;
                for (int i = 1; i < g_histN; i++) {
                    double d = Dist(g_hist[i - 1], g_hist[i]);
                    path += d;
                    if (i > j) recentPath += d;
                }
                double speed = path * 1000.0 / dt;
                double hide = g_hideSpeedCm * hidePxPerCm;   // px/s on this monitor, the same hand speed everywhere
                LONGLONG recentDt = now - g_hist[j].t;
                double recent = recentDt >= 4 ? recentPath * 1000.0 / recentDt : speed;
                if (speed > hide) {
                    g_hideWanted = true;
                    lastFast = now;
                } else if (g_hideWanted && recent < hide * 0.5 && now - lastFast > HOLD_MS) {
                    g_hideWanted = false;
                }
            }
            if (g_hideWanted && now - lastMove > HOLD_MS + SHOW_WINDOW_MS) g_hideWanted = false;   // stopped
            if (g_hideWanted != wasHidden) SetEvent(g_hideChanged);
        }
        bool visible = g_curVisible, hidden = g_hideWanted;
        LeaveCriticalSection(&g_gate);

        if (Now() - lastMove < g_trailMs + 40 || hidden) {   // never sleep while the real cursor is hidden
            if (!timerHigh) { timeBeginPeriod(1); timerHigh = true; }
            // look again on every mouse report; the timeout keeps the path ageing after a stop, and checks every 2 ms
            // while the real cursor is hidden so it comes back quickly
            WaitForSingleObject(g_mouseWake, hidden ? 2 : 10);
        } else {
            if (timerHigh) { timeEndPeriod(1); timerHigh = false; }
            // Sleep until the mouse moves; while an app hides the cursor, just look once per report. While a window with more
            // rights is in front no mouse reports arrive, so look every 10 ms instead, and every 100 ms once the mouse has
            // been still for a second.
            DWORD poll = INFINITE;
            if (g_adminFront) poll = Now() - lastMove < 1000 ? 10 : 100;
            if (WaitForSingleObject(g_mouseWake, poll) == WAIT_OBJECT_0 && visible) lastMove = Now();
        }
    }
    if (timerHigh) timeEndPeriod(1);
    return 0;
}

// ---- hider ----

static DWORD WINAPI HideThread(void*) {
    // static system cursors blanked while the mouse is fast (the animated wait/app-starting ones are left alone)
    static const UINT ids[] = {32512, 32513, 32515, 32516, 32642, 32643, 32644,
                               32645, 32646, 32648, 32649, 32651, 32671, 32672};
    BYTE andMask[128], xorMask[128];
    memset(andMask, 0xFF, sizeof(andMask));
    memset(xorMask, 0, sizeof(xorMask));
    while (!g_stop) {
        EnterCriticalSection(&g_gate);
        bool want = g_hideWanted;
        LeaveCriticalSection(&g_gate);
        if (want && !g_blankActive) {
            SetBlank(true);
            for (UINT id : ids) {
                HCURSOR c = CreateCursor(nullptr, 0, 0, 32, 32, andMask, xorMask);
                if (c && !SetSystemCursor(c, id)) DestroyCursor(c);   // on success the system owns it
            }
        } else if (!want && g_blankActive) {
            ReloadCursors();
            SetBlank(false);
        }
        WaitForSingleObject(g_hideChanged, INFINITE);
    }
    if (g_blankActive) {   // never leave the user with an invisible cursor
        ReloadCursors();
        SetBlank(false);
    }
    return 0;
}

// ---- cursor pictures (render thread only) ----

static struct { HCURSOR h; Sprite* sp; } g_sprites[MAX_SPRITES];
static int g_nSprites, g_nextId;
// Cursors an app made invisible on purpose: no blur for them. Checked again after a while, because another tool that
// hides the cursor the way this mod does (swapping the system cursors) would otherwise switch the blur off for good.
static struct { HCURSOR h; LONGLONG t; } g_empties[MAX_SPRITES];
static int g_nEmpties;
static Sprite* g_lastSprite;             // the last real picture, drawn while the real cursor is hidden

// Takes the cursor's picture by drawing it on black and on white: what shows through gives the opacity.
static Sprite* Grab(HCURSOR h, bool* empty) {
    *empty = false;
    ICONINFO ii;
    if (!GetIconInfo(h, &ii)) return nullptr;
    BITMAP bm = {};
    GetObjectW(ii.hbmColor ? ii.hbmColor : ii.hbmMask, sizeof(bm), &bm);
    // a monochrome mask holds AND and XOR halves
    int w = bm.bmWidth, ht = ii.hbmColor ? bm.bmHeight : bm.bmHeight / 2;
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (w <= 0 || ht <= 0 || w > 256 || ht > 256) return nullptr;

    int n = w * ht;
    BITMAPINFO bi = {};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), w, -ht, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
    void* bits;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP dib = dc ? CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    Sprite* sp = dib ? (Sprite*)malloc(sizeof(Sprite) + n * 4) : nullptr;
    if (sp) {
        HGDIOBJ old = SelectObject(dc, dib);
        UINT32 *px = (UINT32*)(sp + 1), *onWhite = (UINT32*)bits;
        memset(bits, 0, n * 4);
        DrawIconEx(dc, 0, 0, h, w, ht, 0, nullptr, DI_NORMAL);
        GdiFlush();
        memcpy(px, bits, n * 4);   // on black
        memset(bits, 0xFF, n * 4);
        DrawIconEx(dc, 0, 0, h, w, ht, 0, nullptr, DI_NORMAL);
        GdiFlush();
        bool any = false;
        for (int i = 0; i < n; i++) {
            UINT32 b = px[i];
            // how much of the background shows through
            int a = 255 - ((int)(onWhite[i] >> 8 & 255) - (int)(b >> 8 & 255));
            a = Clamp(a, 0, 255);
            // on black = premultiplied colour
            UINT32 r = b >> 16 & 255, g = b >> 8 & 255, bl = b & 255, ua = (UINT32)a;
            px[i] = ua << 24 | (r < ua ? r : ua) << 16 | (g < ua ? g : ua) << 8 | (bl < ua ? bl : ua);
            any |= a != 0;
        }
        SelectObject(dc, old);
        *sp = {w, ht, (int)ii.xHotspot, (int)ii.yHotspot, ++g_nextId, px};
        if (!any) { free(sp); sp = nullptr; *empty = true; }
    }
    if (dib) DeleteObject(dib);
    if (dc) DeleteDC(dc);
    return sp;
}

static void FreeSprites() {
    for (int i = 0; i < g_nSprites; i++) free(g_sprites[i].sp);
    g_nSprites = g_nEmpties = 0;
    g_lastSprite = nullptr;
}

static bool IsEmpty(HCURSOR h) {
    for (int i = 0; i < g_nEmpties; i++)
        if (g_empties[i].h == h) {
            if (Now() - g_empties[i].t < 2000) return true;
            g_empties[i] = g_empties[--g_nEmpties];   // look at it again
            return false;
        }
    return false;
}

static Sprite* GetSprite(HCURSOR h) {
    for (int i = 0; i < g_nSprites; i++)
        if (g_sprites[i].h == h) return g_sprites[i].sp;
    if (IsEmpty(h)) return nullptr;
    bool hiddenBefore = g_blankActive, empty;
    Sprite* sp = Grab(h, &empty);
    // Only keep a picture taken while the real cursor is showing: blanking replaces the content under the same handle.
    if (sp && g_blankActive) { free(sp); sp = nullptr; }
    if (sp) {
        if (g_nSprites == MAX_SPRITES) FreeSprites();
        g_sprites[g_nSprites++] = {h, sp};
    }
    // Empty while our own blanking was off the whole time: the app itself shows an invisible cursor.
    if (empty && !hiddenBefore && !g_blankActive) {
        if (g_nEmpties == MAX_SPRITES) g_nEmpties = 0;
        g_empties[g_nEmpties++] = {h, Now()};
    }
    return sp;
}

// ---- drawing (render thread only) ----


static HDC g_memDc;
static HBITMAP g_dib;
static HGDIOBJ g_dibOld;
// the canvas: premultiplied BGRA, top-down, g_canvasW wide; frames use its top-left corner
static UINT32* g_bits;
static int g_canvasW, g_canvasH;
static UINT32* g_pic;          // the cursor picture at the drawn size
static int* g_span;            // per row of g_pic: first and one-past-last visible pixel
static int g_picId, g_picW, g_picH;
static bool g_shown;

static void ReleaseCanvas() {
    if (g_dib) {
        SelectObject(g_memDc, g_dibOld);
        DeleteObject(g_dib);
        g_dib = nullptr;
    }
    g_canvasW = g_canvasH = 0;
}

static void EnsureCanvas(int w, int h) {   // grows only, in 128 px steps
    if (g_dib && w <= g_canvasW && h <= g_canvasH) return;
    int nw = (w + 127) / 128 * 128, nh = (h + 127) / 128 * 128;
    if (nw < g_canvasW) nw = g_canvasW;
    if (nh < g_canvasH) nh = g_canvasH;
    ReleaseCanvas();
    BITMAPINFO bi = {};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), nw, -nh, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
    g_dib = CreateDIBSection(g_memDc, &bi, DIB_RGB_COLORS, (void**)&g_bits, nullptr, 0);
    if (!g_dib) return;
    g_dibOld = SelectObject(g_memDc, g_dib);
    g_canvasW = nw;
    g_canvasH = nh;
}

// One pixel of the cursor picture resized to w x h, on premultiplied colours: the average of the pixels it covers when
// shrinking, bilinear when growing.
static UINT32 Pixel(const Sprite* sp, int x, int y, int w, int h) {
    float acc[4] = {}, n = 0;
    if (sp->w > w) {
        int x0 = x * sp->w / w, x1 = (x + 1) * sp->w / w, y0 = y * sp->h / h, y1 = (y + 1) * sp->h / h;
        if (x1 <= x0) x1 = x0 + 1;
        if (y1 <= y0) y1 = y0 + 1;
        for (int yy = y0; yy < y1; yy++)
            for (int xx = x0; xx < x1; xx++, n++)
                for (int s = 0; s < 4; s++) acc[s] += sp->px[yy * sp->w + xx] >> (8 * s) & 255;
    } else {
        float fx = (x + 0.5f) * sp->w / w - 0.5f, fy = (y + 0.5f) * sp->h / h - 0.5f;
        if (fx < 0) fx = 0;
        if (fy < 0) fy = 0;
        int x0 = (int)fx, y0 = (int)fy, x1 = x0 + 1 < sp->w ? x0 + 1 : x0, y1 = y0 + 1 < sp->h ? y0 + 1 : y0;
        float ax = fx - x0, ay = fy - y0, wt[4] = {(1 - ax) * (1 - ay), ax * (1 - ay), (1 - ax) * ay, ax * ay};
        UINT32 p[4] = {sp->px[y0 * sp->w + x0], sp->px[y0 * sp->w + x1],
                       sp->px[y1 * sp->w + x0], sp->px[y1 * sp->w + x1]};
        for (int i = 0; i < 4; i++)
            for (int s = 0; s < 4; s++) acc[s] += (p[i] >> (8 * s) & 255) * wt[i];
        n = 1;
    }
    UINT32 out = 0;
    for (int s = 0; s < 4; s++) out |= (UINT32)(acc[s] / n + 0.5f) << (8 * s);
    return out;
}

// The cursor picture at the drawn size, made again only when the cursor or the size changes.
static bool Resize(const Sprite* sp, int w, int h) {
    if (g_pic && g_picId == sp->id && g_picW == w && g_picH == h) return true;
    free(g_pic);
    g_pic = (UINT32*)malloc(w * h * 4 + h * 2 * sizeof(int));
    if (!g_pic) return false;
    g_span = (int*)(g_pic + w * h);
    g_picId = sp->id;
    g_picW = w;
    g_picH = h;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            g_pic[y * w + x] = w == sp->w && h == sp->h ? sp->px[y * w + x] : Pixel(sp, x, y, w, h);
    // most of a cursor is transparent: blending only touches the visible part of each row
    for (int y = 0; y < h; y++) {
        int a = 0, b = w;
        while (a < w && !g_pic[y * w + a]) a++;
        while (b > a && !g_pic[y * w + b - 1]) b--;
        g_span[2 * y] = a;
        g_span[2 * y + 1] = b;
    }
    return true;
}

// Draws the picture at (ox, oy) at opacity a (1-255): premultiplied "source over", two channels per multiplication.
static void Blend(int a, int ox, int oy) {
    UINT32 k = (UINT32)a + 1;   // 2-256, so full opacity is exact
    for (int y = 0; y < g_picH; y++) {
        UINT32* row = g_bits + (oy + y) * g_canvasW + ox;
        const UINT32* src = g_pic + y * g_picW;
        for (int x = g_span[2 * y]; x < g_span[2 * y + 1]; x++) {
            UINT32 s = src[x];
            if (!s) continue;
            s = ((s & 0xFF00FF) * k >> 8 & 0xFF00FF) | ((s >> 8 & 0xFF00FF) * k & 0xFF00FF00);
            UINT32 inv = 256 - (s >> 24), d = row[x];
            row[x] = s + (((d & 0xFF00FF) * inv >> 8 & 0xFF00FF) | ((d >> 8 & 0xFF00FF) * inv & 0xFF00FF00));
        }
    }
}

static bool Draw(const Sprite* sp, int sw, int sh, const Copy* c, int n, int x, int y, int w, int h) {
    if (!Resize(sp, sw, sh)) return false;
    EnsureCanvas(w, h);
    if (!g_dib) return false;
    for (int r = 0; r < h; r++) memset(g_bits + r * g_canvasW, 0, w * 4);
    for (int i = 0; i < n; i++) {
        int a = (int)(c[i].a * 255 + 0.5f);
        if (a > 0 && c[i].x >= 0 && c[i].y >= 0 && c[i].x + sw <= w && c[i].y + sh <= h) Blend(a, c[i].x, c[i].y);
    }
    // The window is resized to every frame: keeping its size and only moving it is a little cheaper, but Windows can then
    // show the move a frame before the new picture, and the blur jitters.
    POINT dst = {x, y}, src = {0, 0};
    SIZE size = {w, h};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &size, g_memDc, &src, 0, &bf, ULW_ALPHA);
    if (!IsWindowVisible(g_wnd))
        SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    return true;
}

static bool Hide() {
    if (g_shown) {
        ShowWindow(g_wnd, SW_HIDE);
        g_shown = false;
    }
    return false;
}

// Draws one frame of the blur; false if there is nothing to draw (then the overlay is hidden).
static bool Render() {
    static Sample pts[MAX_SAMPLES];
    static Copy copies[COPY_BUF];
    int np, size;
    HCURSOR handle;
    bool visible;
    EnterCriticalSection(&g_gate);
    visible = g_curVisible;
    handle = g_curHandle;
    size = g_monCursor;
    np = g_histN;
    memcpy(pts, g_hist, np * sizeof(Sample));
    LeaveCriticalSection(&g_gate);
    if (!visible || np < 2) return Hide();
    if (g_spritesStale && !g_blankActive) {   // the cursor scheme, size or colour may have changed
        g_spritesStale = 0;
        FreeSprites();
    }

    Sprite* sp;
    if (g_blankActive) {
        sp = g_lastSprite;   // the real cursor is hidden: keep drawing the picture taken before
    } else {
        if (IsEmpty(handle)) return Hide();   // the app hid its cursor: don't blur the previous one
        sp = GetSprite(handle);
        if (sp) g_lastSprite = sp;
        else sp = g_lastSprite;
    }
    if (!sp) return Hide();

    Sample last = pts[np - 1];
    bool anyMove = false;
    for (int i = 0; i < np && !anyMove; i++) anyMove = pts[i].x != last.x || pts[i].y != last.y;
    if (!anyMove) return Hide();   // stationary: nothing to blur

    // the picture at the size Windows draws the cursor on this monitor
    double f = (double)size / sp->w;
    int sw = size, sh = (int)lround(sp->h * f), hx = (int)lround(sp->hx * f), hy = (int)lround(sp->hy * f);

    int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
    double total = 0;
    for (int i = 0; i < np; i++) {
        if (pts[i].x - hx < minX) minX = pts[i].x - hx;
        if (pts[i].y - hy < minY) minY = pts[i].y - hy;
        if (pts[i].x - hx + sw > maxX) maxX = pts[i].x - hx + sw;
        if (pts[i].y - hy + sh > maxY) maxY = pts[i].y - hy + sh;
        if (i) total += Dist(pts[i - 1], pts[i]);
    }
    double step = total / MAX_COPIES > 1 ? total / MAX_COPIES : 1;   // dense, faint copies read as a smooth blur
    double cover = COVER_PX * size / 32.0, peak = g_opacity / 100.0, trail = g_trailMs;
    LONGLONG now = Now();
    int n = 0;
    for (int i = 1; i < np && n < COPY_BUF; i++) {
        Sample a = pts[i - 1], b = pts[i];
        double dx = b.x - a.x, dy = b.y - a.y, d = sqrt(dx * dx + dy * dy);
        if (d < 0.5) continue;
        int k = (int)(d / step);
        if (k < 1) k = 1;
        for (int c = 0; c < k && n < COPY_BUF; c++) {
            double t = (double)c / k;
            double life = 1.0 - (now - (a.t + (b.t - a.t) * t)) / trail;
            if (life <= 0) continue;
            life *= life;   // steeper fade
            double alpha = peak * life * step / cover;   // copies overlap by about cover/step: each gets that share
            if (alpha * 255 < 0.5) continue;
            copies[n++] = {(int)lround(a.x + dx * t) - hx - minX, (int)lround(a.y + dy * t) - hy - minY,
                           alpha > 1 ? 1.0f : (float)alpha};
        }
    }
    if (!n) return Hide();
    g_drawFailed = !Draw(sp, sw, sh, copies, n, minX, minY, maxX - minX, maxY - minY);
    if (g_drawFailed) return Hide();
    g_shown = true;
    return true;
}

// Raw mouse input: while on, Windows tells the overlay about every mouse report (even while other apps are active), so
// the sampler can sleep until the mouse really moves. Off while paused for a fullscreen game.
static bool ListenToMouse(bool on) {
    // generic desktop / mouse
    RAWINPUTDEVICE mouse = {1, 2, on ? (DWORD)RIDEV_INPUTSINK : (DWORD)RIDEV_REMOVE, on ? g_wnd.load() : nullptr};
    if (RegisterRawInputDevices(&mouse, 1, sizeof(mouse))) return true;
    Wh_Log(L"RegisterRawInputDevices(%d) failed: %lu", on, GetLastError());
    return false;
}

// Whether a window belongs to a process with more rights than this one (an administrator app such as Task Manager or
// Windhawk itself): this process may not open its token, and Windows does not send it raw mouse input meanwhile.
static bool HigherRights(HWND w) {
    DWORD pid = 0;
    if (!w || !GetWindowThreadProcessId(w, &pid)) return false;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return true;
    HANDLE token = nullptr;
    bool denied = !OpenProcessToken(p, TOKEN_QUERY, &token);
    if (token) CloseHandle(token);
    CloseHandle(p);
    return denied;
}

static void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND w, LONG, LONG, DWORD, DWORD) {
    bool admin = HigherRights(w);
    if (admin != g_adminFront) Wh_Log(L"Administrator window in front: %d", admin);
    g_adminFront = admin;
    g_recheck = true;   // a fullscreen app may also have come to the front or left it
    SetEvent(g_mouseWake);
}

static LRESULT CALLBACK OverlayProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_INPUT: {   // wake the sampler for movement only (not for wheel or button reports)
            RAWINPUT ri;
            UINT size = sizeof(ri);
            if (GetRawInputData((HRAWINPUT)lp, RID_INPUT, &ri, &size, sizeof(RAWINPUTHEADER)) != (UINT)-1 &&
                ri.header.dwType == RIM_TYPEMOUSE &&
                (ri.data.mouse.lLastX || ri.data.mouse.lLastY || (ri.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)))
                SetEvent(g_mouseWake);
            break;   // then DefWindowProc, which cleans up after the input
        }
        case WM_APP_PAUSED:
            ListenToMouse(!g_paused);
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:   // monitors, scaling or cursor scheme changed
            g_monStale = 1;
            g_spritesStale = 1;
            break;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// Draws when the mouse moves, at most once per half refresh of the monitor, and keeps drawing until the trail has faded
// after a stop. Idle, it wakes once more to give back memory, then sleeps until the mouse moves.
static DWORD WINAPI RenderThread(void*) {
    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = inst;
    wc.lpszClassName = L"TrueCursorMotionBlur";
    RegisterClassW(&wc);
    g_wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                            wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, inst, nullptr);
    g_memDc = CreateCompatibleDC(nullptr);
    HWINEVENTHOOK hook = nullptr;
    if (!g_wnd || !g_memDc || !ListenToMouse(true)) {
        Wh_Log(L"Could not set up the overlay window");
        if (g_wnd) DestroyWindow(g_wnd);
        g_wnd = nullptr;
    } else {
        hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundChanged, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
        g_adminFront = HigherRights(GetForegroundWindow());
    }
    SetEvent(g_ready);

    LONGLONG lastDraw = -1000;
    bool trimmed = true, pending = false;   // pending: the mouse moved, but too soon after the last frame
    while (!g_stop && g_wnd) {
        LONGLONG wait = g_monGap - (Now() - lastDraw);
        DWORD timeout = pending || g_shown ? (wait > 0 ? (DWORD)wait : 0) : trimmed ? INFINITE : 1500;
        DWORD r = MsgWaitForMultipleObjects(1, &g_moved, FALSE, timeout, QS_ALLINPUT);
        if (g_stop) break;
        if (r == WAIT_OBJECT_0 + 1) {
            MSG m;
            while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&m);
            continue;
        }
        if (r == WAIT_OBJECT_0) pending = true;
        if (!pending && !g_shown) {   // idle for 1.5 s: give back the memory that can be rebuilt
            ReleaseCanvas();
            free(g_pic);
            g_pic = nullptr;
            trimmed = true;
            continue;
        }
        if (Now() - lastDraw < g_monGap) continue;
        pending = false;
        if (Render()) {
            lastDraw = Now();
            trimmed = false;
        }
    }

    if (hook) UnhookWinEvent(hook);
    if (g_wnd) {
        ListenToMouse(false);
        DestroyWindow(g_wnd);
        g_wnd = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, inst);
    ReleaseCanvas();
    if (g_memDc) DeleteDC(g_memDc);
    g_memDc = nullptr;
    free(g_pic);
    g_pic = nullptr;
    FreeSprites();
    return 0;
}

// ---- mod lifecycle ----

static LPTOP_LEVEL_EXCEPTION_FILTER g_prevFilter;

// This process exists only for this mod: if it ever crashes while the real cursor is hidden, give the cursor back.
static LONG WINAPI CrashFilter(EXCEPTION_POINTERS* e) {
    if (g_blankActive) ReloadCursors();   // the stored flag stays set, so the next start checks again
    return g_prevFilter ? g_prevFilter(e) : EXCEPTION_CONTINUE_SEARCH;
}

// Signals every thread to finish, waits for them, and makes sure the user's cursors are back.
static void StopThreads() {
    g_stop = true;
    SetEvent(g_mouseWake);
    SetEvent(g_moved);
    SetEvent(g_hideChanged);
    for (HANDLE& t : g_threads)
        if (t) {
            WaitForSingleObject(t, 5000);
            CloseHandle(t);
            t = nullptr;
        }
    if (g_blankActive) {
        ReloadCursors();
        SetBlank(false);
    }
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    QueryPerformanceFrequency(&g_freq);
    QueryPerformanceCounter(&g_start);
    InitializeCriticalSection(&g_gate);
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        BYTE buf[SECURITY_MAX_SID_SIZE + sizeof(TOKEN_USER)];
        DWORD len;
        LPWSTR sid;
        if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &len) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &sid)) {
            swprintf_s(g_hiddenKey, L"cursorsHidden_%s", sid);
            LocalFree(sid);
        }
        CloseHandle(token);
    }
    if (Wh_GetIntValue(g_hiddenKey, 0)) {   // an earlier run was killed while hiding the cursor
        Wh_Log(L"Restoring the cursors after an earlier run");
        ReloadCursors();
        Wh_SetIntValue(g_hiddenKey, 0);
    }
    g_prevFilter = SetUnhandledExceptionFilter(CrashFilter);

    // Windows 11 ignores a finer timer for a process with no visible window; the short waits here need it while moving.
    PROCESS_POWER_THROTTLING_STATE pt = {PROCESS_POWER_THROTTLING_CURRENT_VERSION,
                                         PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION, 0};
    SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &pt, sizeof(pt));

    g_moved = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mouseWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_hideChanged = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_moved || !g_mouseWake || !g_hideChanged || !g_ready) return FALSE;

    // The render thread first: it creates the overlay window that receives the mouse input, and the others need it.
    LPTHREAD_START_ROUTINE procs[3] = {RenderThread, SampleThread, HideThread};
    for (int i = 0; i < 3; i++) {
        g_threads[i] = CreateThread(nullptr, 0, procs[i], nullptr, 0, nullptr);
        if (!g_threads[i]) {
            Wh_Log(L"CreateThread failed");
            StopThreads();
            return FALSE;
        }
        if (i < 2) SetThreadPriority(g_threads[i], THREAD_PRIORITY_ABOVE_NORMAL);
        if (i == 0 && (WaitForSingleObject(g_ready, 5000) != WAIT_OBJECT_0 || !g_wnd)) {
            StopThreads();
            return FALSE;
        }
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    g_recheck = true;
    SetEvent(g_mouseWake);
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    StopThreads();
    SetUnhandledExceptionFilter(g_prevFilter);
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

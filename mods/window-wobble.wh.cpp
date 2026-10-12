// ==WindhawkMod==
// @id              window-wobble
// @name            Window Wobble
// @description     KDE-style spring-physics window dragging, without injecting into dwm.exe
// @version         1.0.0
// @author          HeyJeay
// @github          https://github.com/heyjeay
// @homepage        https://github.com/heyjeay/window-wobble
// @license         GPL-3.0-or-later
// @include         *
// @exclude         sihost.exe
// @exclude         RuntimeBroker.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         TextInputHost.exe
// @exclude         ApplicationFrameHost.exe
// @exclude         NVDisplay.Container.exe
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Wobble

KDE-style spring-physics window dragging. Grab a title bar and the window
follows your cursor like jelly - the title bar keeps up, the body trails
behind, and it settles with a wobble when you let go.

![Window Wobble in action](https://raw.githubusercontent.com/heyjeay/window-wobble/main/docs/preview.gif)

Hold **Shift** (configurable) while grabbing to move a window normally.

Licensed GPL-3.0-or-later: you are free to use, study, modify and share it,
and any version you distribute must stay open under the same terms.

## How this differs from the Wobbly Windows mod

[Wobbly Windows](https://windhawk.net/mods/wobbly-windows) does the same
effect from inside `dwm.exe`, using private uDWM symbols. That is the better
place to do it: the content stays live, and it can wobble on snapping and
resizing too, not just dragging.

This mod does **not** touch `dwm.exe`. It runs in the application's own
process and draws the effect on its own Direct3D overlay. Pick whichever
trade you prefer:

**Reasons to use this one**
- No changes to Windhawk's process inclusion list - it works once enabled
- Nothing is injected into the compositor, so a fault here cannot take the
  desktop down with it; at worst one application is affected
- No dependency on private, undocumented DWM symbols, so a Windows update
  changing uDWM internals cannot break it

**Reasons to use the other one**
- Window content stays live; here it is a snapshot, so a playing video
  appears frozen for the length of the drag
- It wobbles on snap and resize as well as drag
- One copy in the compositor, rather than a small overlay per application

**Questions, problems or ideas?** Join the Discord
(https://discord.gg/8wwfPxfMWA) or open an issue
(https://github.com/heyjeay/window-wobble/issues).

## How it works

### In plain terms

1. You press on a title bar, and the mod quietly takes a photograph of the
   whole window.
2. The real window is made invisible. Not minimised, not moved - just not
   drawn. Its taskbar button stays exactly where it was.
3. The photograph is pinned to an invisible grid, like a sheet of rubber, and
   shown in the window's place. The top edge of the sheet is stiff and keeps
   up with your cursor; further down it gets floppier. That is the whole
   trick - the body lags behind the title bar because the bottom of the sheet
   is softer than the top.
4. All the while you are dragging, the mod is just moving that rubber sheet
   around and letting the springs do what springs do.
5. When you let go, the real window is moved to where you dropped it while it
   is still invisible, then made visible again *underneath* the photograph.
   Only once it is safely back does the photograph disappear, so you never
   catch the swap happening.

Two things follow from this, and they are the reason for the bypass key. The
window content is a photograph for as long as you hold it, so a playing video
looks frozen. And because the mod is doing the dragging instead of Windows,
Windows' own dragging features do not happen - see the section above.

### Under the hood

A title-bar press is caught at `DispatchMessage`, with `DefWindowProc` as a
backup and `WM_SYSCOMMAND`/`SC_MOVE` alongside for apps that draw their own
title bars. The mod then runs its own move loop in place of the one Windows
would start: the window is captured with `PrintWindow(PW_RENDERFULLCONTENT)`,
cloaked so it leaves composition entirely, and a spring mesh of that capture
is drawn on a Direct3D 11 + DirectComposition overlay until you let go.

The mesh is two layers of springs: a control spring pulling each vertex back
to its rest position, with the stiffness falling off down the rows, and
springs between neighbouring vertices so the grid holds together instead of
turning to taffy. On release the control springs are switched to a stiff,
overdamped pair so it snaps back rather than ringing.

The drag runs on one persistent worker thread per process, never in the
message pump: `PrintWindow(PW_RENDERFULLCONTENT)` renders through DWM and
needs the window's own thread pumping, so doing it inline deadlocks against
the very loop that invoked it.

Only top-level windows are touched, and only ones DWM will cloak. Anything
else is handed back to Windows, which then runs its own move loop as usual.

## What you give up during a wobble drag

The mod replaces Windows' move loop, so for the windows it animates these do
not happen:

- Aero Snap (dragging to a screen edge or the top)
- the Snap Layouts bar at the top of the screen
- restoring a snapped window's previous size when you drag it out
- shake to minimize
- `WM_ENTERSIZEMOVE` / `WM_MOVING` / `WM_EXITSIZEMOVE`, which docking UIs
  such as Visual Studio's tool windows use to show drop targets

Hold the bypass key (**Shift** by default) while grabbing to get all of that
back for a drag.

## Excluded processes

Shell and UI-host processes are excluded, since none of them own a window you
would drag: `sihost.exe`, `RuntimeBroker.exe`, `StartMenuExperienceHost.exe`,
`ShellExperienceHost.exe`, `SearchHost.exe`, `TextInputHost.exe`,
`ApplicationFrameHost.exe` and `NVDisplay.Container.exe`.

Windhawk never injects into the critical system processes (`csrss.exe`,
`dwm.exe`, `lsass.exe` and the rest), so those need no entry here.

## Notes

Window content is a snapshot for the duration of the drag, so a playing video
appears frozen while you hold it. This is inherent to the approach - capturing
live content per frame is far more expensive than one `PrintWindow`.

The Direct3D device and the overlay are released after a few seconds without
a drag, so an application you dragged once does not hold them for the rest of
its life. They are rebuilt on the next drag, which costs a few milliseconds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- stiffness: 200
  $name: Stiffness
  $description: Spring strength pulling the window to the cursor. Higher is snappier.
- bottomSoftness: 65
  $name: Body softness (%)
  $description: How much softer the bottom edge is than the title bar. This is what makes the body trail behind. 100 removes the wave entirely.
- clothRigidity: 30
  $name: Cloth rigidity (x10)
  $description: Resistance to stretching between neighbouring mesh points. 30 means 3.0.
- damping: 140
  $name: Friction (x10)
  $description: Higher settles sooner with fewer rings. 140 means 14.0.
- impulse: 12
  $name: Kick strength (x100)
  $description: Impulse applied on direction changes. 12 means 0.12.
- meshSize: 16
  $name: Mesh resolution
  $description: Grid cells per side. Higher is smoother but costs more per frame.
- revealHoldMs: 90
  $name: Reveal hold (ms)
  $description: How long the settled overlay stays up while the real window repaints underneath. Raise it if you see a flash on landing.
- dragThreshold: 4
  $name: Drag threshold (px)
  $description: How far the pointer must move before the wobble takes over. Below this a title-bar click behaves normally.
- idleReleaseSec: 5
  $name: Release GPU resources after (seconds)
  $description: Idle time after a drag before the overlay and the Direct3D device are freed. Lower keeps memory down, higher makes an occasional drag start faster. 0 keeps them for the life of the process.
- bypassKey: shift
  $name: Bypass key
  $description: Hold this while grabbing a title bar to move the window normally, with Aero Snap and everything else Windows does.
  $options:
  - shift: Shift
  - ctrl: Ctrl
  - alt: Alt
  - none: Disabled
*/
// ==/WindhawkModSettings==

// Window Wobble - spring-physics window dragging for Windows.
// Copyright (C) 2026 heyjeay
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details <https://www.gnu.org/licenses/>.

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <optional>
#include <vector>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define MOD_HINSTANCE (reinterpret_cast<HINSTANCE>(&__ImageBase))

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------
// Windhawk validates settings as integers or strings; $float is not available
// here. Values that are conceptually fractional are stored scaled and divided
// on load.

struct Settings {
    float springK      = 200.0f;
    float bottomRatio  = 0.65f;
    float meshK        = 3.0f;
    float damping      = 14.0f;
    float impulse      = 0.12f;
    // Fixed, not user-tunable: critical damping is d = 2*sqrt(k), and the
    // pair has to move together. ~14ms time constant at a 4ms frame, which
    // lands in roughly 100ms from any realistic drag.
    float settleK       = 12000.0f;
    float settleD       = 210.0f;
    int   mesh          = 16;
    int   bypassVk      = VK_SHIFT;
    int   threshold     = 4;
    int   revealHoldMs  = 90;
    int   idleReleaseMs = 5000;
};

// Written only by Wh_ModSettingsChanged, on Windhawk's thread. The worker
// takes a copy for the whole of each drag rather than reading this directly:
// a mesh size that changed between Mesh::Setup and Overlay::Draw would size
// the vertex buffer for one grid and fill it from another, which overflows
// the buffer when the size went down.
static Settings g_cfg;

static void LoadSettings() {
    g_cfg.springK     = (float)Wh_GetIntSetting(L"stiffness");
    g_cfg.bottomRatio = Wh_GetIntSetting(L"bottomSoftness") / 100.0f;
    g_cfg.meshK       = Wh_GetIntSetting(L"clothRigidity") / 10.0f;
    g_cfg.damping     = Wh_GetIntSetting(L"damping") / 10.0f;
    g_cfg.impulse     = Wh_GetIntSetting(L"impulse") / 100.0f;
    g_cfg.mesh        = Wh_GetIntSetting(L"meshSize");

    // An unknown value falls back to Shift rather than to "disabled", so a
    // typo cannot silently remove the escape hatch.
    WindhawkUtils::StringSetting bypass =
        WindhawkUtils::StringSetting::make(L"bypassKey");
    PCWSTR b = bypass.get();
    g_cfg.bypassVk = !wcscmp(b, L"ctrl") ? VK_CONTROL
                   : !wcscmp(b, L"alt")  ? VK_MENU
                   : !wcscmp(b, L"none") ? 0
                                         : VK_SHIFT;

    g_cfg.revealHoldMs = Wh_GetIntSetting(L"revealHoldMs");
    if (g_cfg.revealHoldMs < 0 || g_cfg.revealHoldMs > 1000) g_cfg.revealHoldMs = 90;
    g_cfg.threshold = Wh_GetIntSetting(L"dragThreshold");
    if (g_cfg.threshold < 1) g_cfg.threshold = 4;

    int idle = Wh_GetIntSetting(L"idleReleaseSec");
    if (idle < 0)   idle = 5;
    if (idle > 600) idle = 600;
    g_cfg.idleReleaseMs = idle * 1000;

    if (g_cfg.springK <= 0.0f) g_cfg.springK = 200.0f;
    if (g_cfg.damping <= 0.0f) g_cfg.damping = 14.0f;
    if (g_cfg.mesh < 4)  g_cfg.mesh = 4;
    if (g_cfg.mesh > 40) g_cfg.mesh = 40;

    Wh_Log(L"settings: k=%d ratio=%d%% meshK=%d damp=%d mesh=%d bypass=0x%02X idle=%dms",
           (int)g_cfg.springK, (int)(g_cfg.bottomRatio * 100), (int)g_cfg.meshK,
           (int)g_cfg.damping, g_cfg.mesh, g_cfg.bypassVk, g_cfg.idleReleaseMs);
}

// -----------------------------------------------------------------------------
// Small helpers
// -----------------------------------------------------------------------------

template <class T>
struct Com {
    T* p = nullptr;
    ~Com() { reset(); }
    Com() = default;
    Com(const Com&) = delete;
    Com& operator=(const Com&) = delete;
    T** operator&() { reset(); return &p; }
    T* operator->() const { return p; }
    operator T*() const { return p; }
    void reset() { if (p) { p->Release(); p = nullptr; } }
};

struct Vec2 { float x, y; };

static double NowSeconds() {
    static LARGE_INTEGER freq = [] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER c; QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)freq.QuadPart;
}

// Raised to retire the worker. Every loop that can run for more than a frame
// checks it, so an unload never has to wait out a 120-second drag.
static HANDLE g_quitSignal = nullptr;

static bool QuitRequested() {
    return g_quitSignal &&
           WaitForSingleObject(g_quitSignal, 0) == WAIT_OBJECT_0;
}

// "Switch primary and secondary buttons" swaps which physical button reports
// as VK_LBUTTON. GetAsyncKeyState reads the physical one, so polling
// VK_LBUTTON unconditionally meant a left-handed user's drag ended the
// instant it began - and the press had already been swallowed by the hook.
static int PrimaryButtonVk() {
    return GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
}

static bool PrimaryButtonDown() {
    return (GetAsyncKeyState(PrimaryButtonVk()) & 0x8000) != 0;
}

// GetWindowRect includes an invisible DWM resize border - 8-9px per side on
// Windows 11. Capturing or positioning against it bakes an empty black frame
// into the texture and offsets the drop. This is the real visible rectangle.
static RECT VisibleRect(HWND hwnd) {
    RECT r{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))))
        GetWindowRect(hwnd, &r);
    return r;
}

// GetWindowRect updates the moment a window is resized; DWM's extended frame
// bounds update asynchronously and briefly report the OLD geometry. Capturing
// in that window crops the bitmap to the previous size and builds the mesh
// there, so the overlay appears at the old size and position while the real
// window sits at the new one - a duplicate, reproducible by resizing and then
// immediately dragging.
//
// The extended frame is always the window rect minus a small invisible
// border, so a large disagreement means DWM has not caught up yet. Wait
// briefly for it to agree, and fall back to the window rect if it never does.
//
// Both rectangles must be read in the same coordinate space for this
// comparison to mean anything. Extended frame bounds are always physical
// pixels, so the caller has to be per-monitor DPI aware - see WorkerProc.
static RECT VisibleRectSettled(HWND hwnd) {
    for (int attempt = 0; attempt < 40; ++attempt) {      // <= ~40ms
        RECT outer{};
        if (!GetWindowRect(hwnd, &outer)) break;
        const RECT vis = VisibleRect(hwnd);

        const LONG dl = vis.left     - outer.left;
        const LONG dt = vis.top      - outer.top;
        const LONG dr = outer.right  - vis.right;
        const LONG db = outer.bottom - vis.bottom;

        // Plausible: inset on every side, and never by more than a border.
        if (dl >= 0 && dt >= 0 && dr >= 0 && db >= 0 &&
            dl <= 32 && dt <= 32 && dr <= 32 && db <= 32)
            return vis;

        if (QuitRequested()) break;
        Sleep(1);
    }
    RECT outer{};
    GetWindowRect(hwnd, &outer);
    Wh_Log(L"frame bounds never settled; using window rect");
    return outer;
}

// -----------------------------------------------------------------------------
// Capture
// -----------------------------------------------------------------------------

struct Bitmap32 {
    int w = 0, h = 0;
    std::vector<uint32_t> px;   // BGRA, premultiplied on upload
};

// PrintWindow's alpha is unreliable: GDI paint calls leave the alpha byte at
// zero, so a classic app comes back fully transparent while a compositor-drawn
// one comes back opaque. We ignore the captured alpha, force it opaque, and
// carve the Windows 11 rounded corners ourselves.
static void RoundCorners(Bitmap32& bmp, float radius) {
    if (radius < 1.0f) return;
    const int r = (int)radius;
    if (r * 2 > bmp.w || r * 2 > bmp.h) return;

    for (int cy = 0; cy < r; ++cy) {
        for (int cx = 0; cx < r; ++cx) {
            const float dx = (float)r - (float)cx - 0.5f;
            const float dy = (float)r - (float)cy - 0.5f;
            const float d  = sqrtf(dx * dx + dy * dy);
            // One-pixel smoothstep across the arc keeps the curve from aliasing.
            float a = radius - d + 0.5f;
            a = a < 0.0f ? 0.0f : (a > 1.0f ? 1.0f : a);
            const uint32_t alpha = (uint32_t)(a * 255.0f + 0.5f);

            const int xs[4] = { cx, bmp.w - 1 - cx, cx, bmp.w - 1 - cx };
            const int ys[4] = { cy, cy, bmp.h - 1 - cy, bmp.h - 1 - cy };
            for (int i = 0; i < 4; ++i) {
                uint32_t& p = bmp.px[(size_t)ys[i] * bmp.w + xs[i]];
                p = (p & 0x00FFFFFF) | (alpha << 24);
            }
        }
    }
}

// Windows 10 draws square window corners, so carving rounded ones into the
// snapshot would make the corners visibly change at the handoff. The corner
// preference attribute is not a reliable probe for this - it is documented
// write-only and the read fails on some Windows 11 builds too - so ask the
// OS version directly. RtlGetVersion is used because GetVersionEx lies to
// unmanifested callers.
static bool SystemRoundsCorners() {
    static const bool rounded = [] {
        struct VerInfo {
            ULONG size, major, minor, build, platform;
            WCHAR csd[128];
        } vi{};
        vi.size = sizeof(vi);
        HMODULE nt = GetModuleHandleW(L"ntdll.dll");
        if (!nt) return true;
        auto get = (LONG(WINAPI*)(VerInfo*))GetProcAddress(nt, "RtlGetVersion");
        if (!get || get(&vi) != 0) return true;   // unknown: keep rounding
        return vi.build >= 22000;                 // Windows 11
    }();
    return rounded;
}

static bool CaptureWindow(HWND hwnd, Bitmap32& out) {
    RECT outer{};
    if (!GetWindowRect(hwnd, &outer)) return false;
    const int ow = outer.right - outer.left, oh = outer.bottom - outer.top;
    if (ow < 1 || oh < 1) return false;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = ow;
    bi.bmiHeader.biHeight      = -oh;      // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HBITMAP hbm = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!hbm) return false;

    HDC mem = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(mem, hbm);
    // PW_RENDERFULLCONTENT renders through DWM, so it works even when the
    // window is occluded or already transparent.
    PrintWindow(hwnd, mem, PW_RENDERFULLCONTENT);
    SelectObject(mem, old);
    DeleteDC(mem);

    const RECT vis = VisibleRectSettled(hwnd);
    const int dx = vis.left - outer.left, dy = vis.top - outer.top;
    const int vw = vis.right - vis.left,  vh = vis.bottom - vis.top;
    if (vw < 1 || vh < 1) { DeleteObject(hbm); return false; }

    out.w = vw; out.h = vh;
    out.px.assign((size_t)vw * vh, 0);
    const uint32_t* src = (const uint32_t*)bits;
    for (int y = 0; y < vh; ++y) {
        const int sy = y + dy;
        if (sy < 0 || sy >= oh) continue;
        for (int x = 0; x < vw; ++x) {
            const int sx = x + dx;
            if (sx < 0 || sx >= ow) continue;
            out.px[(size_t)y * vw + x] = src[(size_t)sy * ow + sx] | 0xFF000000u;
        }
    }
    DeleteObject(hbm);

    if (!IsZoomed(hwnd) && SystemRoundsCorners()) {
        UINT dpi = GetDpiForWindow(hwnd);
        if (!dpi) dpi = 96;
        RoundCorners(out, 8.0f * dpi / 96.0f);
    }
    return true;
}

// -----------------------------------------------------------------------------
// Spring mesh
// -----------------------------------------------------------------------------
// Two layers, same as the original: a control spring pulling each vertex to its
// rest position with stiffness falling off down the rows (the title bar is
// stiff, the bottom soft - this is what makes the body lag), and mesh springs
// between neighbours so the grid holds together instead of turning to taffy.

struct Mesh {
    int nx = 0, ny = 0;
    std::vector<Vec2> rest, pos, vel;
    std::vector<Vec2> force;            // scratch, reused every frame
    bool  settled = true;
    float bandK = 0.0f, bandD = 0.0f;   // non-zero during settle
    Settings cfg;                       // snapshot taken at Setup

    void Setup(const Settings& c, int cells, float x, float y, float w, float h) {
        cfg = c;
        nx = ny = cells + 1;
        rest.resize((size_t)nx * ny);
        pos.resize(rest.size());
        vel.assign(rest.size(), Vec2{0, 0});
        force.resize(rest.size());
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i) {
                Vec2 p{ x + w * i / (nx - 1), y + h * j / (ny - 1) };
                rest[(size_t)j * nx + i] = p;
                pos[(size_t)j * nx + i]  = p;
            }
        settled = false;
        bandK = bandD = 0.0f;
    }

    void MoveRest(float x, float y) {
        const float dx = x - rest[0].x, dy = y - rest[0].y;
        for (auto& p : rest) { p.x += dx; p.y += dy; }
        settled = false;
    }

    void Kick(float vx, float vy) {
        const float speed = sqrtf(vx * vx + vy * vy);
        if (speed < 2.0f) return;
        const float norm = fabsf(vx) + fabsf(vy) + 1e-6f;
        for (int j = 0; j < ny; ++j) {
            const float fy = (float)j / (ny - 1);
            for (int i = 0; i < nx; ++i) {
                const float fx = (float)i / (nx - 1);
                // Trailing edge gets kicked most: the side opposite travel.
                const float wx = (vx > 0) ? (1.0f - fx) : fx;
                const float wy = (vy > 0) ? (1.0f - fy) : fy;
                const float w  = (wx * fabsf(vx) + wy * fabsf(vy)) / norm;
                Vec2& v = vel[(size_t)j * nx + i];
                v.x -= vx * w * cfg.impulse;
                v.y -= vy * w * cfg.impulse;
            }
        }
        settled = false;
    }

    void SettleFast() {
        bandK = cfg.settleK;
        bandD = cfg.settleD;
        settled = false;
    }

    bool Update(float dt) {
        if (settled) return false;
        const bool band = bandK > 0.0f;
        const float kBase = band ? bandK : cfg.springK;
        const float d     = band ? bandD : cfg.damping;

        std::vector<Vec2>& f = force;
        float maxDisp = 0.0f;

        for (int j = 0; j < ny; ++j) {
            // Uniform while settling; row-graded while dragging.
            const float t = (ny > 1) ? (float)j / (ny - 1) : 0.0f;
            const float k = band ? kBase
                                 : kBase * (1.0f + (cfg.bottomRatio - 1.0f) * t);
            for (int i = 0; i < nx; ++i) {
                const size_t n = (size_t)j * nx + i;
                const float ex = pos[n].x - rest[n].x;
                const float ey = pos[n].y - rest[n].y;
                maxDisp = fmaxf(maxDisp, fmaxf(fabsf(ex), fabsf(ey)));
                f[n] = Vec2{ -k * ex, -k * ey };
            }
        }

        if (cfg.meshK > 0.0f) {
            const float km = cfg.meshK;
            for (int j = 0; j < ny; ++j)
                for (int i = 0; i + 1 < nx; ++i) {
                    const size_t a = (size_t)j * nx + i, b = a + 1;
                    const float dxv = (pos[b].x - pos[a].x) - (rest[b].x - rest[a].x);
                    const float dyv = (pos[b].y - pos[a].y) - (rest[b].y - rest[a].y);
                    f[a].x += km * dxv; f[a].y += km * dyv;
                    f[b].x -= km * dxv; f[b].y -= km * dyv;
                }
            for (int j = 0; j + 1 < ny; ++j)
                for (int i = 0; i < nx; ++i) {
                    const size_t a = (size_t)j * nx + i, b = a + nx;
                    const float dxv = (pos[b].x - pos[a].x) - (rest[b].x - rest[a].x);
                    const float dyv = (pos[b].y - pos[a].y) - (rest[b].y - rest[a].y);
                    f[a].x += km * dxv; f[a].y += km * dyv;
                    f[b].x -= km * dxv; f[b].y -= km * dyv;
                }
        }

        float maxVel = 0.0f;
        for (size_t n = 0; n < pos.size(); ++n) {
            f[n].x -= d * vel[n].x;
            f[n].y -= d * vel[n].y;
            vel[n].x += f[n].x * dt;
            vel[n].y += f[n].y * dt;
            pos[n].x += vel[n].x * dt;
            pos[n].y += vel[n].y * dt;
            maxVel = fmaxf(maxVel, fmaxf(fabsf(vel[n].x), fabsf(vel[n].y)));
        }

        // Sub-pixel exactness is invisible, and demanding it kept the mesh
        // running its whole budget after every long drag.
        if (maxDisp < 2.5f && maxVel < 20.0f) {
            pos = rest;
            std::fill(vel.begin(), vel.end(), Vec2{0, 0});
            bandK = bandD = 0.0f;
            settled = true;
        }
        return !settled;
    }
};

// -----------------------------------------------------------------------------
// Renderer: D3D11 + DirectComposition
// -----------------------------------------------------------------------------
// The mod is injected into every process, so it must not drag the graphics
// stack in with it. d3d11.dll and dcomp.dll are loaded on the first drag
// rather than linked, which keeps a console tool or a background helper from
// paying for a feature it will never use. The shaders are compiled offline
// with fxc and embedded as bytecode, so d3dcompiler is not needed at all.

struct D3DApi {
    using PFN_D3D11CreateDevice = HRESULT(WINAPI*)(
        IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*,
        UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
    using PFN_DCompositionCreateDevice =
        HRESULT(WINAPI*)(IDXGIDevice*, REFIID, void**);

    HMODULE d3d11 = nullptr;
    HMODULE dcomp = nullptr;
    PFN_D3D11CreateDevice          CreateDevice      = nullptr;
    PFN_DCompositionCreateDevice   CreateCompDevice  = nullptr;
    bool tried = false, ok = false;

    bool Load() {
        if (tried) return ok;
        tried = true;
        // SEARCH_SYSTEM32: never pick up a DLL planted beside the host exe.
        d3d11 = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        dcomp = LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!d3d11 || !dcomp) {
            Wh_Log(L"d3d11/dcomp load failed (%p/%p)", (void*)d3d11, (void*)dcomp);
            return false;
        }
        CreateDevice = (PFN_D3D11CreateDevice)
            GetProcAddress(d3d11, "D3D11CreateDevice");
        CreateCompDevice = (PFN_DCompositionCreateDevice)
            GetProcAddress(dcomp, "DCompositionCreateDevice");
        ok = CreateDevice && CreateCompDevice;
        if (!ok) Wh_Log(L"d3d11/dcomp entry points missing");
        return ok;
    }

    // Without this each enable/disable cycle leaks a reference and the DLLs
    // stay mapped after the mod is gone. Only safe once the worker has been
    // joined, so every device built from them is already released.
    void Free() {
        if (d3d11) FreeLibrary(d3d11);
        if (dcomp) FreeLibrary(dcomp);
        d3d11 = dcomp = nullptr;
        CreateDevice = nullptr;
        CreateCompDevice = nullptr;
        tried = ok = false;
    }
};
static D3DApi g_api;

struct Vertex { float x, y, u, v; };

// Embedding the compiled bytecode means d3dcompiler is neither linked nor
// loaded, and no shader is compiled at run time. The source is reproduced
// here in full so the arrays below can be rebuilt and compared byte for
// byte; save it as wobble.hlsl and run:
//
//   fxc /nologo /T vs_4_0 /E VS /O3 /Fo vs.cso wobble.hlsl
//   fxc /nologo /T ps_4_0 /E PS /O3 /Fo ps.cso wobble.hlsl
//
// ---------------------------------------------------------------------------
// cbuffer CB : register(b0) { float2 origin; float2 size; };
// struct VSIn  { float2 pos : POSITION; float2 uv : TEXCOORD; };
// struct VSOut { float4 pos : SV_POSITION; float2 uv : TEXCOORD; };
// Texture2D    tex : register(t0);
// SamplerState smp : register(s0);
//
// VSOut VS(VSIn i) {
//     VSOut o;
//     // Surface pixels -> NDC. origin is the surface's top-left in screen
//     // coordinates; mesh vertices are in screen coordinates.
//     float2 n = (i.pos - origin) / size * 2.0 - 1.0;
//     o.pos = float4(n.x, -n.y, 0, 1);
//     o.uv  = i.uv;
//     return o;
// }
// float4 PS(VSOut i) : SV_TARGET {
//     float4 c = tex.Sample(smp, i.uv);
//     return float4(c.rgb * c.a, c.a);   // swapchain is premultiplied alpha
// }
// ---------------------------------------------------------------------------

static const BYTE kVS[] = {
    0x44, 0x58, 0x42, 0x43, 0xD1, 0x8F, 0x64, 0x25, 0xE8, 0x85, 0x89, 0x72,
    0x41, 0x8B, 0x95, 0x92, 0x23, 0xF9, 0xF8, 0xEA, 0x01, 0x00, 0x00, 0x00,
    0x74, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
    0x0C, 0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0xB8, 0x01, 0x00, 0x00,
    0xF8, 0x02, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0xD0, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x1C, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFE, 0xFF, 0x00, 0x81, 0x00, 0x00,
    0xA5, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x43, 0x42, 0x00, 0xAB, 0x3C, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x58, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x6F, 0x72, 0x69, 0x67, 0x69, 0x6E, 0x00, 0xAB,
    0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x73, 0x69, 0x7A, 0x65, 0x00, 0x4D, 0x69, 0x63,
    0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48,
    0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43,
    0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x31, 0x30, 0x2E, 0x31,
    0x00, 0xAB, 0xAB, 0xAB, 0x49, 0x53, 0x47, 0x4E, 0x4C, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x50, 0x4F, 0x53, 0x49,
    0x54, 0x49, 0x4F, 0x4E, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4F, 0x4F, 0x52,
    0x44, 0x00, 0xAB, 0xAB, 0x4F, 0x53, 0x47, 0x4E, 0x50, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x50,
    0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00, 0x54, 0x45, 0x58, 0x43,
    0x4F, 0x4F, 0x52, 0x44, 0x00, 0xAB, 0xAB, 0xAB, 0x53, 0x48, 0x44, 0x52,
    0x38, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x4E, 0x00, 0x00, 0x00,
    0x59, 0x00, 0x00, 0x04, 0x46, 0x8E, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xF2, 0x20, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03,
    0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x32, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x46, 0x80, 0x20, 0x80, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x08, 0x32, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xE6, 0x8A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x32, 0x00, 0x00, 0x0F, 0x32, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x46, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0xBF,
    0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x36, 0x00, 0x00, 0x06, 0x22, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1A, 0x00, 0x10, 0x80, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x36, 0x00, 0x00, 0x05, 0x12, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08,
    0xC2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x3F, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x3E, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x74, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const BYTE kPS[] = {
    0x44, 0x58, 0x42, 0x43, 0x98, 0x5F, 0xF3, 0x3F, 0x5A, 0x0B, 0x12, 0x53,
    0xBF, 0x21, 0xC8, 0xB8, 0xFE, 0x5C, 0xDF, 0x29, 0x01, 0x00, 0x00, 0x00,
    0x74, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
    0xC8, 0x00, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x54, 0x01, 0x00, 0x00,
    0xF8, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x8C, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x1C, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0x00, 0x81, 0x00, 0x00,
    0x64, 0x00, 0x00, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x73, 0x6D, 0x70, 0x00,
    0x74, 0x65, 0x78, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66,
    0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53,
    0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C,
    0x65, 0x72, 0x20, 0x31, 0x30, 0x2E, 0x31, 0x00, 0x49, 0x53, 0x47, 0x4E,
    0x50, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
    0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,
    0x53, 0x56, 0x5F, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00,
    0x54, 0x45, 0x58, 0x43, 0x4F, 0x4F, 0x52, 0x44, 0x00, 0xAB, 0xAB, 0xAB,
    0x4F, 0x53, 0x47, 0x4E, 0x2C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x54, 0x41, 0x52, 0x47, 0x45,
    0x54, 0x00, 0xAB, 0xAB, 0x53, 0x48, 0x44, 0x52, 0x9C, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x5A, 0x00, 0x00, 0x03,
    0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04,
    0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
    0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x65, 0x00, 0x00, 0x03, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x09,
    0xF2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x46, 0x7E, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07,
    0x72, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF6, 0x0F, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x46, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x36, 0x00, 0x00, 0x05, 0x82, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3A, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x01,
    0x53, 0x54, 0x41, 0x54, 0x74, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

// The device and shaders are size-independent and expensive to build. They
// live as a local of the worker thread, not as a global: Windhawk does not
// call Wh_ModUninit when the host process exits, so a global's destructor
// would run from DllMain, under the loader lock, with every other thread
// already killed. Releasing a DXGI swapchain or a DirectComposition device
// there is exactly what Microsoft warns against.
struct Device {
    Com<ID3D11Device>        dev;
    Com<ID3D11DeviceContext> ctx;
    Com<ID3D11VertexShader>  vs;
    Com<ID3D11PixelShader>   ps;
    Com<ID3D11InputLayout>   layout;
    Com<ID3D11SamplerState>  sampler;
    Com<ID3D11BlendState>    blend;
    Com<ID3D11Buffer>        cb;
    bool ok = false;
    bool warmed = false;

    bool Init() {
        if (ok) return true;
        if (!g_api.Load()) return false;

        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL got{};
        HRESULT hr = g_api.CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                        flags, nullptr, 0, D3D11_SDK_VERSION,
                                        &dev, &got, &ctx);
        if (FAILED(hr)) {
            // Some hosts cannot get a hardware device; WARP still composites.
            Wh_Log(L"D3D11 hardware device failed hr=0x%08X, trying WARP", hr);
            hr = g_api.CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                                    flags, nullptr, 0, D3D11_SDK_VERSION,
                                    &dev, &got, &ctx);
        }
        if (FAILED(hr)) { Wh_Log(L"D3D11CreateDevice failed hr=0x%08X", hr); return false; }

        if (FAILED(dev->CreateVertexShader(kVS, sizeof(kVS), nullptr, &vs))) return false;
        if (FAILED(dev->CreatePixelShader(kPS, sizeof(kPS), nullptr, &ps)))  return false;

        D3D11_INPUT_ELEMENT_DESC el[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (FAILED(dev->CreateInputLayout(el, 2, kVS, sizeof(kVS), &layout)))
            return false;

        D3D11_SAMPLER_DESC sd{};
        sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        if (FAILED(dev->CreateSamplerState(&sd, &sampler))) return false;

        D3D11_BLEND_DESC bd{};
        bd.RenderTarget[0].BlendEnable           = TRUE;
        bd.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;   // premultiplied
        bd.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(dev->CreateBlendState(&bd, &blend))) return false;

        D3D11_BUFFER_DESC cbd{};
        cbd.ByteWidth      = 16;
        cbd.Usage          = D3D11_USAGE_DYNAMIC;
        cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(dev->CreateBuffer(&cbd, nullptr, &cb))) return false;

        ok = true;
        return true;
    }

    bool Lost() const {
        return dev.p && FAILED(dev->GetDeviceRemovedReason());
    }
};

enum class DrawResult { Ok, DeviceLost, Failed };

// The swapchain spans the whole virtual desktop and never moves.
//
// A window-sized swapchain repositioned each frame with
// IDCompositionVisual::SetOffsetX/Y is far cheaper in memory, and was tried:
// ~29 MB for one 1440p screen against ~130 MB for two 4K ones. It had to be
// reverted. A DirectComposition commit is applied by DWM on its own
// schedule while the pixels arrive through Present, so when the two land on
// different frames the entire image shifts by one frame of cursor travel.
// The wobble still looked right in isolation, but the window felt like it
// lagged the pointer, which for an effect whose whole purpose is how it
// feels is not a trade worth making.
//
// The per-process cost is instead bounded by releasing the device and the
// swapchain after an idle period - see WorkerProc. A desktop-sized surface
// exists only while a drag is actually happening.

struct Overlay {
    Device* d = nullptr;
    HWND hwnd = nullptr;
    Com<IDXGISwapChain1>          swap;
    Com<ID3D11RenderTargetView>   rtv;
    Com<IDCompositionDevice>      dcomp;
    Com<IDCompositionTarget>      target;
    Com<IDCompositionVisual>      visual;
    Com<ID3D11Texture2D>          tex;
    Com<ID3D11ShaderResourceView> srv;
    Com<ID3D11Buffer>             vb;

    int vx = 0, vy = 0, vw = 0, vh = 0;     // virtual desktop, = surface bounds
    int vertCount = 0;
    bool presentWarmed = false;             // per surface, not per press
    std::vector<Vertex> verts;

    ~Overlay() { Destroy(); }

    bool EnsureWindow();
    bool EnsureSurface();
    bool DisplayChanged() const;
    void WarmPipeline();
    void WarmPresent();
    bool SetContent(const Bitmap32& bmp, int cells);
    void Show();
    void Hide();
    void Destroy();
    DrawResult Draw(const Mesh& m);
};

static const wchar_t kOverlayClass[] = L"WobblyWindowsOverlay";
static std::atomic<bool> g_classRegistered{false};

// Registered against the mod's own module, not the host exe, and unregistered
// on unload. Registering under the host would leave the class behind after a
// disable or an update, so the next load would silently reuse the old one -
// harmless only for as long as lpfnWndProc stays DefWindowProcW, and a
// dangling window procedure the moment it does not.
static bool EnsureClass() {
    if (g_classRegistered.load()) return true;
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = DefWindowProcW;
    wc.hInstance     = MOD_HINSTANCE;
    wc.lpszClassName = kOverlayClass;
    // ERROR_CLASS_ALREADY_EXISTS is not treated as success. Registration is
    // under this module and Wh_ModUninit unregisters, so the only way the
    // class can already exist is that a previous instance left it behind -
    // and reusing that one is the dangling lpfnWndProc case, harmless only
    // for as long as the procedure stays DefWindowProcW. Failing here just
    // hands the drag back to Windows.
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"overlay: RegisterClassEx failed err=%u", GetLastError());
        return false;
    }
    g_classRegistered.store(true);
    return true;
}

bool Overlay::DisplayChanged() const {
    return vx != GetSystemMetrics(SM_XVIRTUALSCREEN) ||
           vy != GetSystemMetrics(SM_YVIRTUALSCREEN) ||
           vw != GetSystemMetrics(SM_CXVIRTUALSCREEN) ||
           vh != GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

bool Overlay::EnsureWindow() {
    if (hwnd && !DisplayChanged()) return true;
    if (hwnd) {
        // A monitor was added, removed or rescaled. Everything below is sized
        // against the old desktop, so it all has to go.
        Wh_Log(L"overlay: display geometry changed, rebuilding");
        Destroy();
    }
    if (!EnsureClass()) return false;

    vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw < 1 || vh < 1) return false;

    // NOREDIRECTIONBITMAP: no GDI surface, DirectComposition supplies the
    // content. TRANSPARENT|NOACTIVATE keep it click-through and unfocusable.
    hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_POPUP,
        vx, vy, vw, vh, nullptr, nullptr, MOD_HINSTANCE, nullptr);
    if (!hwnd) {
        Wh_Log(L"overlay: CreateWindowEx failed err=%u", GetLastError());
        return false;
    }
    return true;
}

bool Overlay::EnsureSurface() {
    if (!d || !d->Init()) { Wh_Log(L"overlay: D3D device init failed"); return false; }
    if (!EnsureWindow()) return false;      // rebuilds if the desktop changed
    if (swap.p) return true;
    presentWarmed = false;                  // a new surface needs warming

    const bool built = [&] {
        Com<IDXGIDevice> dxgiDev;
        if (FAILED(d->dev->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev)))
            return false;
        Com<IDXGIAdapter> adapter;
        if (FAILED(dxgiDev->GetAdapter(&adapter))) return false;
        Com<IDXGIFactory2> factory;
        if (FAILED(adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory)))
            return false;

        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width            = vw;
        sd.Height           = vh;
        sd.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount      = 2;
        sd.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;
        HRESULT hr = factory->CreateSwapChainForComposition(d->dev, &sd, nullptr, &swap);
        if (FAILED(hr)) {
            Wh_Log(L"overlay: swapchain failed hr=0x%08X (%dx%d)", hr, vw, vh);
            return false;
        }

        Com<ID3D11Texture2D> back;
        if (FAILED(swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back)))
            return false;
        if (FAILED(d->dev->CreateRenderTargetView(back, nullptr, &rtv))) return false;

        if (!dcomp.p) {
            hr = g_api.CreateCompDevice(dxgiDev, __uuidof(IDCompositionDevice),
                                        (void**)&dcomp);
            if (FAILED(hr)) {
                Wh_Log(L"overlay: DCompositionCreateDevice failed hr=0x%08X", hr);
                return false;
            }
            hr = dcomp->CreateTargetForHwnd(hwnd, TRUE, &target);
            if (FAILED(hr)) {
                Wh_Log(L"overlay: CreateTargetForHwnd failed hr=0x%08X", hr);
                return false;
            }
            if (FAILED(dcomp->CreateVisual(&visual))) return false;
            target->SetRoot(visual);
        }
        visual->SetContent(swap);
        dcomp->Commit();                // once, at build time; never per frame
        return true;
    }();

    // Leave nothing half-built behind: the next press would otherwise create
    // a second window and leak this one.
    if (!built) Destroy();
    return built;
}

void Overlay::WarmPipeline() {
    if (!d || !d->ok || d->warmed) return;
    d->warmed = true;     // once per device, not once per title-bar click

    // Presenting to the swapchain is useless here: the overlay window is
    // still hidden, so DXGI treats the frame as occluded and the driver never
    // does the work. Render into a small offscreen target instead, which does
    // not care about window visibility, and block on a query until the GPU
    // has actually finished - otherwise the driver's shader work is merely
    // deferred to the first real frame, which is the stall being removed.
    const UINT kSide = 64;

    D3D11_TEXTURE2D_DESC rt{};
    rt.Width = rt.Height = kSide;
    rt.MipLevels = rt.ArraySize = 1;
    rt.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    rt.SampleDesc.Count = 1;
    rt.Usage            = D3D11_USAGE_DEFAULT;
    rt.BindFlags        = D3D11_BIND_RENDER_TARGET;
    Com<ID3D11Texture2D> rtTex;
    Com<ID3D11RenderTargetView> rtView;
    if (FAILED(d->dev->CreateTexture2D(&rt, nullptr, &rtTex)))             return;
    if (FAILED(d->dev->CreateRenderTargetView(rtTex, nullptr, &rtView)))   return;

    const uint32_t pixel = 0xFF808080;
    D3D11_TEXTURE2D_DESC td{};
    td.Width = td.Height = 1;
    td.MipLevels = td.ArraySize = 1;
    td.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage            = D3D11_USAGE_IMMUTABLE;
    td.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA srd{}; srd.pSysMem = &pixel; srd.SysMemPitch = 4;
    Com<ID3D11Texture2D> t;
    Com<ID3D11ShaderResourceView> v;
    if (FAILED(d->dev->CreateTexture2D(&td, &srd, &t)))           return;
    if (FAILED(d->dev->CreateShaderResourceView(t, nullptr, &v))) return;

    // A triangle that genuinely covers the target, so the pixel shader runs.
    const Vertex tri[3] = { {0,0,0,0}, {(float)kSide,0,1,0}, {0,(float)kSide,0,1} };
    D3D11_BUFFER_DESC vbd{};
    vbd.ByteWidth = sizeof(tri);
    vbd.Usage     = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vsd{}; vsd.pSysMem = tri;
    Com<ID3D11Buffer> b;
    if (FAILED(d->dev->CreateBuffer(&vbd, &vsd, &b))) return;

    auto* ctx = d->ctx.p;

    struct CB { float ox, oy, sw, sh; } cbv{ 0, 0, (float)kSide, (float)kSide };
    D3D11_MAPPED_SUBRESOURCE map{};
    if (SUCCEEDED(ctx->Map(d->cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) {
        memcpy(map.pData, &cbv, sizeof(cbv));
        ctx->Unmap(d->cb, 0);
    }

    const float clear[4] = { 0, 0, 0, 0 };
    ID3D11RenderTargetView* rts[] = { rtView.p };
    ctx->OMSetRenderTargets(1, rts, nullptr);
    ctx->ClearRenderTargetView(rtView, clear);
    D3D11_VIEWPORT vp{ 0, 0, (float)kSide, (float)kSide, 0, 1 };
    ctx->RSSetViewports(1, &vp);
    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer* vbs[] = { b.p };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetInputLayout(d->layout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(d->vs, nullptr, 0);
    ID3D11Buffer* cbs[] = { d->cb.p };
    ctx->VSSetConstantBuffers(0, 1, cbs);
    ctx->PSSetShader(d->ps, nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = { v.p };
    ctx->PSSetShaderResources(0, 1, srvs);
    ID3D11SamplerState* smps[] = { d->sampler.p };
    ctx->PSSetSamplers(0, 1, smps);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(d->blend, bf, 0xFFFFFFFF);
    ctx->Draw(3, 0);

    // Block until the GPU is done, so the work happens here and not later.
    D3D11_QUERY_DESC qd{}; qd.Query = D3D11_QUERY_EVENT;
    Com<ID3D11Query> q;
    if (SUCCEEDED(d->dev->CreateQuery(&qd, &q))) {
        ctx->End(q);
        ctx->Flush();
        BOOL done = FALSE;
        const double limit = NowSeconds() + 1.0;
        while (ctx->GetData(q, &done, sizeof(done), 0) != S_OK &&
               NowSeconds() < limit && !QuitRequested())
            Sleep(0);
    } else {
        ctx->Flush();
    }
}

void Overlay::WarmPresent() {
    if (!swap.p || !hwnd || !d || presentWarmed) return;
    presentWarmed = true;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    const float clear[4] = { 0, 0, 0, 0 };          // fully transparent
    ID3D11RenderTargetView* rts[] = { rtv.p };
    d->ctx->OMSetRenderTargets(1, rts, nullptr);
    d->ctx->ClearRenderTargetView(rtv, clear);
    swap->Present(0, 0);
    ShowWindow(hwnd, SW_HIDE);
}

bool Overlay::SetContent(const Bitmap32& bmp, int cells) {
    if (!d || !d->ok) return false;

    srv.reset();
    tex.reset();

    // Window snapshot -> texture
    D3D11_TEXTURE2D_DESC td{};
    td.Width            = bmp.w;
    td.Height           = bmp.h;
    td.MipLevels        = 1;
    td.ArraySize        = 1;
    td.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage            = D3D11_USAGE_IMMUTABLE;
    td.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA srd{};
    srd.pSysMem     = bmp.px.data();
    srd.SysMemPitch = bmp.w * 4;
    HRESULT hr = d->dev->CreateTexture2D(&td, &srd, &tex);
    if (FAILED(hr)) {
        Wh_Log(L"overlay: texture %dx%d failed hr=0x%08X", bmp.w, bmp.h, hr);
        return false;
    }
    if (FAILED(d->dev->CreateShaderResourceView(tex, nullptr, &srv))) return false;

    const int need = cells * cells * 6;
    if (need != vertCount) {
        vb.reset();
        vertCount = need;
        verts.resize(vertCount);
    }
    if (vb.p) return true;     // buffer already sized correctly
    D3D11_BUFFER_DESC vbd{};
    vbd.ByteWidth      = (UINT)(sizeof(Vertex) * vertCount);
    vbd.Usage          = D3D11_USAGE_DYNAMIC;
    vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(d->dev->CreateBuffer(&vbd, nullptr, &vb))) return false;

    return true;
}

void Overlay::Show() {
    if (hwnd) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
}

void Overlay::Hide() {
    if (hwnd) ShowWindow(hwnd, SW_HIDE);
}

void Overlay::Destroy() {
    srv.reset(); tex.reset(); vb.reset(); rtv.reset();
    visual.reset(); target.reset(); dcomp.reset(); swap.reset();
    if (hwnd) { DestroyWindow(hwnd); hwnd = nullptr; }
    presentWarmed = false;
    vertCount = 0;
    verts.clear();
}

DrawResult Overlay::Draw(const Mesh& m) {
    if (!d || !d->ok || !swap.p || !rtv.p) return DrawResult::Failed;

    const int cells = m.nx - 1;
    int n = 0;
    for (int j = 0; j < cells; ++j) {
        for (int i = 0; i < cells; ++i) {
            const size_t a = (size_t)j * m.nx + i,       b = a + 1;
            const size_t c = a + m.nx,                   dd = c + 1;
            const float u0 = (float)i / cells,  u1 = (float)(i + 1) / cells;
            const float v0 = (float)j / cells,  v1 = (float)(j + 1) / cells;
            const Vertex quad[6] = {
                { m.pos[a].x,  m.pos[a].y,  u0, v0 },
                { m.pos[b].x,  m.pos[b].y,  u1, v0 },
                { m.pos[c].x,  m.pos[c].y,  u0, v1 },
                { m.pos[b].x,  m.pos[b].y,  u1, v0 },
                { m.pos[dd].x, m.pos[dd].y, u1, v1 },
                { m.pos[c].x,  m.pos[c].y,  u0, v1 },
            };
            for (const auto& vtx : quad) verts[n++] = vtx;
        }
    }

    auto* ctx = d->ctx.p;
    D3D11_MAPPED_SUBRESOURCE map{};
    if (SUCCEEDED(ctx->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) {
        memcpy(map.pData, verts.data(), sizeof(Vertex) * vertCount);
        ctx->Unmap(vb, 0);
    }

    struct CB { float ox, oy, sw, sh; } cbv{
        (float)vx, (float)vy, (float)vw, (float)vh };
    if (SUCCEEDED(ctx->Map(d->cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) {
        memcpy(map.pData, &cbv, sizeof(cbv));
        ctx->Unmap(d->cb, 0);
    }

    const float clear[4] = { 0, 0, 0, 0 };
    ID3D11RenderTargetView* rts[] = { rtv.p };
    ctx->OMSetRenderTargets(1, rts, nullptr);
    ctx->ClearRenderTargetView(rtv, clear);

    D3D11_VIEWPORT vp{ 0, 0, (float)vw, (float)vh, 0, 1 };
    ctx->RSSetViewports(1, &vp);

    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer* vbs[] = { vb.p };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetInputLayout(d->layout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(d->vs, nullptr, 0);
    ID3D11Buffer* cbs[] = { d->cb.p };
    ctx->VSSetConstantBuffers(0, 1, cbs);
    ctx->PSSetShader(d->ps, nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = { srv.p };
    ctx->PSSetShaderResources(0, 1, srvs);
    ID3D11SamplerState* smps[] = { d->sampler.p };
    ctx->PSSetSamplers(0, 1, smps);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(d->blend, bf, 0xFFFFFFFF);
    ctx->Draw(vertCount, 0);

    // A removed device (driver update, TDR, GPU switch on a hybrid laptop, an
    // RDP reconnect) used to go unnoticed: every later drag cloaked the window
    // and drew nothing, leaving it invisible until the button came up.
    const HRESULT hr = swap->Present(1, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Wh_Log(L"overlay: device lost on present hr=0x%08X", hr);
        return DrawResult::DeviceLost;
    }
    if (FAILED(hr)) {
        Wh_Log(L"overlay: present failed hr=0x%08X", hr);
        return DrawResult::Failed;
    }
    return DrawResult::Ok;
}

// -----------------------------------------------------------------------------
// Hiding the real window
// -----------------------------------------------------------------------------
// SW_HIDE would strip the taskbar button. Cloaking removes the window from
// composition entirely - including DirectComposition content, which a layered
// alpha does not - without moving it, so there is no DWM animation to produce
// a ghost and the taskbar is untouched. In-process we are always allowed to
// do this; cross-process it is E_ACCESSDENIED, which is why the standalone
// version of this effect cannot use it.

struct HideState {
    bool  cloaked   = false;
    bool  hadCorner = false;   // corner preference was readable
    DWORD corner    = 0;       // and this is what it was
};

// DWMWA_WINDOW_CORNER_PREFERENCE is documented as write-only, and on some
// builds the read does fail - hadCorner then stays false and the restore is
// skipped. It is kept because where the read does work it fixes a reported
// bug: a window with rounded corners came back square after a drag, because
// the cloak cycle dropped the preference.
static void SaveCorners(HWND hwnd, HideState& st) {
    DWORD corner = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                        &corner, sizeof(corner)))) {
        st.hadCorner = true;
        st.corner    = corner;
    }
}

static void RestoreCorners(HWND hwnd, const HideState& st) {
    if (!st.hadCorner) return;
    DWORD corner = st.corner;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &corner, sizeof(corner));
}

static bool HideWindowInPlace(HWND hwnd, HideState& st) {
    SaveCorners(hwnd, st);
    BOOL on = TRUE;
    if (FAILED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &on, sizeof(on))))
        return false;
    st.cloaked = true;
    return true;
}

static void ShowWindowInPlace(HWND hwnd, const HideState& st) {
    if (!st.cloaked) return;
    BOOL off = FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &off, sizeof(off));
    RestoreCorners(hwnd, st);
}

// Whether DWM will cloak this window at all. A window it refuses would stay
// on screen under the overlay, showing two of itself, so we would rather not
// start than animate it unreliably.
static bool CanHideReliably(HWND hwnd) {
    BOOL off = FALSE;    // a no-op write; we only care whether DWM accepts it
    return SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &off, sizeof(off)));
}

// -----------------------------------------------------------------------------
// The drag
// -----------------------------------------------------------------------------

struct DragRequest { HWND hwnd; };

static std::atomic<bool> g_dragging{false};
static std::atomic<bool> g_shuttingDown{false};
static HANDLE      g_worker     = nullptr;
static HANDLE      g_dragSignal = nullptr;   // raised to start a drag
static DragRequest g_pending    = {};

// A press we swallowed but then decided not to animate has to go back to
// Windows, or the user's drag simply does nothing. The hook lets exactly one
// SC_MOVE through for this window instead of taking it again.
static std::atomic<HWND> g_handBack{nullptr};

static void HandBackToWindows(HWND hwnd) {
    if (!PrimaryButtonDown()) return;     // nothing left to hand back
    g_handBack.store(hwnd);
    PostMessageW(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
}

static bool BypassHeld() {
    return g_cfg.bypassVk && (GetAsyncKeyState(g_cfg.bypassVk) & 0x8000);
}

static bool Eligible(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;

    // Top-level only. A child window's SetWindowPos coordinates are relative
    // to its parent while GetWindowRect reports screen coordinates, so moving
    // one by screen position lands it at parent + offset - the drop was
    // overshooting by exactly the parent's origin. Child windows are also not
    // independently composited, so DWM cloaking fails on them, leaving the
    // real window on screen underneath the overlay. That is the duplicate.
    if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;

    const RECT r = VisibleRect(hwnd);
    return (r.right - r.left) >= 100 && (r.bottom - r.top) >= 60;
}

static void PumpMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) { PostQuitMessage((int)msg.wParam); return; }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

// Returns false if the press should be handed back to Windows.
static bool DoWobbleDrag(Device& dev, Overlay& ov, HWND hwnd) {
    // Settings are snapshotted for the whole drag. Reading g_cfg as we go
    // would let a settings change between Mesh::Setup and Overlay::Draw size
    // the vertex buffer for one grid and fill it from another.
    const Settings cfg = g_cfg;

    // The cursor is read here, on the worker, rather than in the hook: this
    // thread is per-monitor DPI aware, so its coordinates are physical
    // pixels and agree with DWMWA_EXTENDED_FRAME_BOUNDS. A DPI-unaware app's
    // own thread would have reported scaled ones.
    POINT down{};
    GetCursorPos(&down);

    // Warm the GPU while the window is still visible and the pointer has not
    // moved, so the one-time cost lands where nothing is on screen waiting.
    {
        if (!ov.EnsureSurface()) {
            Wh_Log(L"abort: overlay surface unavailable");
            return false;
        }
        ov.WarmPipeline();
        // Show and present once, then hide again. The overlay is cleared to
        // fully transparent so nothing appears, but DWM builds its
        // composition target here rather than during the first real drag.
        ov.WarmPresent();
    }

    bool moved = false;
    int  spins = 0;
    const double waitStart = NowSeconds();
    while (PrimaryButtonDown()) {
        if (QuitRequested()) { Wh_Log(L"skip: unloading"); return false; }
        if (NowSeconds() - waitStart > 10.0) {
            Wh_Log(L"skip: button held 10s without moving - giving up");
            return false;
        }
        if (BypassHeld()) { Wh_Log(L"skip: bypass pressed during wait"); return false; }
        POINT p{};
        GetCursorPos(&p);
        if (abs(p.x - down.x) > cfg.threshold ||
            abs(p.y - down.y) > cfg.threshold) { moved = true; break; }
        // Sleep(1) actually sleeps ~15ms at the default timer resolution, so
        // the old wait sampled the pointer only ~6 times across an 83ms press.
        // Yield instead, and pump occasionally so the app stays responsive.
        if ((spins & 7) == 0) PumpMessages();
        Sleep(1);
        ++spins;
    }
    if (!moved) {
        // A plain click, not a drag. Activation has already happened during
        // input processing, before DispatchMessage, so nothing is owed here -
        // but an app that acts on the caption press itself will not see it.
        Wh_Log(L"skip: no movement after %d ms (%d polls)",
               (int)((NowSeconds() - waitStart) * 1000.0), spins);
        return false;
    }

    // Checking this in the hook was pointless: that runs on the window's own
    // thread, which is by definition answering at that moment. Here it is
    // worth something, because everything below is a cross-thread call that
    // blocks for as long as the app stays hung - which an unload cannot wait
    // out.
    if (IsHungAppWindow(hwnd)) {
        Wh_Log(L"skip: window stopped responding");
        return false;
    }

    const bool wasZoomed = IsZoomed(hwnd) != 0;

    // Un-maximize under the cursor, the way Windows does when you drag a
    // maximized title bar - the pointer keeps its relative spot on the bar.
    if (wasZoomed) {
        const RECT mx = VisibleRect(hwnd);
        const float rx = (float)(down.x - mx.left) /
                         (float)std::max<LONG>(1, mx.right - mx.left);
        const int offY = down.y - mx.top;
        ShowWindow(hwnd, SW_RESTORE);
        const RECT nr = VisibleRectSettled(hwnd);
        const int nw = nr.right - nr.left, nh = nr.bottom - nr.top;
        RECT outer{}; GetWindowRect(hwnd, &outer);
        const int tx = (int)(down.x - rx * nw);
        const int ty = down.y - std::min(offY, std::max(nh / 3, 1));
        SetWindowPos(hwnd, nullptr,
                     tx - (nr.left - outer.left), ty - (nr.top - outer.top),
                     0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    // Capture BEFORE hiding. Hiding first left PrintWindow nothing to render,
    // and every drag showed a solid black rectangle.
    Bitmap32 bmp;
    if (!CaptureWindow(hwnd, bmp)) {
        Wh_Log(L"abort: capture failed");
        return false;
    }

    // Measure while the window is still composed: extended frame bounds on a
    // cloaked window are not dependable, and a wrong border offset is exactly
    // what makes the overlay and the real window disagree about position.
    const RECT start = VisibleRectSettled(hwnd);
    RECT outerRc{}; GetWindowRect(hwnd, &outerRc);
    const int frameDx = start.left - outerRc.left;
    const int frameDy = start.top  - outerRc.top;

    // Put the overlay up BEFORE hiding the window, showing the captured image
    // at exactly the window's own position. Hiding first left a gap where
    // neither was on screen - short, but a visible blink at the start of
    // every drag. This is the same handoff used when landing, in reverse.
    Mesh mesh;
    mesh.Setup(cfg, cfg.mesh, (float)start.left, (float)start.top,
               (float)(start.right - start.left),
               (float)(start.bottom - start.top));

    if (!ov.SetContent(bmp, cfg.mesh)) {
        Wh_Log(L"abort: overlay setup failed");
        return false;
    }
    if (ov.Draw(mesh) != DrawResult::Ok) {   // pixel-identical to the window
        Wh_Log(L"abort: first frame failed - leaving the drag to Windows");
        return false;
    }
    ov.Show();

    HideState hide;
    if (!HideWindowInPlace(hwnd, hide)) {
        Wh_Log(L"abort: cannot cloak window");
        ov.Hide();
        return false;
    }

    // From here the window is invisible, which is the worst thing to leave
    // behind. Reveal on every exit path - early return, exception, anything.
    struct Reveal {
        HWND h; const HideState& st; bool armed = true;
        ~Reveal() { if (armed) ShowWindowInPlace(h, st); }
    } reveal{hwnd, hide};

    // Re-baseline the grab to wherever the pointer is now. Everything above
    // takes time - ~130ms on the first drag in a process - and without this
    // the window would leap by whatever distance the pointer covered
    // meanwhile. Windows' own drag does not jump by the threshold either.
    POINT at{};
    GetCursorPos(&at);
    down = at;
    int winX = start.left, winY = start.top;

    double last = NowSeconds();
    float  prevVx = 0, prevVy = 0;
    POINT  lastPt = at;
    bool   cancelled = false;
    bool   lostDevice = false;

    const double dragDeadline = NowSeconds() + 120.0;
    while (PrimaryButtonDown()) {
        if (QuitRequested()) { cancelled = true; break; }
        if (NowSeconds() > dragDeadline) {
            Wh_Log(L"drag exceeded 120s - ending it");
            break;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { cancelled = true; break; }
        PumpMessages();

        const double now = NowSeconds();
        float dt = (float)(now - last);
        if (dt > 0.05f) dt = 0.05f;
        last = now;

        POINT pt{}; GetCursorPos(&pt);
        if (pt.x != lastPt.x || pt.y != lastPt.y) {
            const float idt = dt > 1e-4f ? dt : 1e-4f;
            const float rawVx = (pt.x - lastPt.x) / idt;
            const float rawVy = (pt.y - lastPt.y) / idt;
            const float a = 0.35f;
            const float vxs = a * rawVx + (1 - a) * prevVx;
            const float vys = a * rawVy + (1 - a) * prevVy;

            winX = start.left + (pt.x - down.x);
            winY = start.top  + (pt.y - down.y);
            mesh.MoveRest((float)winX, (float)winY);

            if (prevVx * vxs < 0 || prevVy * vys < 0)
                mesh.Kick(vxs, vys);
            else if (sqrtf(vxs * vxs + vys * vys) > 80.0f)
                mesh.Kick(vxs * 0.4f, vys * 0.4f);

            prevVx = vxs; prevVy = vys;
            lastPt = pt;
        }

        mesh.Update(dt);
        if (ov.Draw(mesh) == DrawResult::DeviceLost) { lostDevice = true; break; }
    }

    if (cancelled) { winX = start.left; winY = start.top; }

    mesh.Kick(prevVx * 0.5f, prevVy * 0.5f);

    // Move first, while still invisible, then ask where it actually went.
    // SWP_ASYNCWINDOWPOS: the target owns this window on another thread, and
    // a synchronous call would block the worker for as long as that thread
    // takes to answer - indefinitely if it has hung mid-drag.
    SetWindowPos(hwnd, nullptr, winX - frameDx, winY - frameDy, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_ASYNCWINDOWPOS);

    // SetWindowPos on a window owned by another thread is asynchronous: it
    // queues the change and returns before the window has moved. Reading the
    // rect straight back gives the OLD position, and uncloaking at that point
    // leaves the overlay at the new spot while the window is still at the old
    // one - which is the duplicate. Wait for the move to actually land.
    const double moveDeadline = NowSeconds() + 0.25;
    RECT landed = VisibleRect(hwnd);
    while (NowSeconds() < moveDeadline && !QuitRequested()) {
        landed = VisibleRect(hwnd);
        if (labs(landed.left - winX) <= 2 && labs(landed.top - winY) <= 2) break;
        Sleep(1);
    }
    if (labs(landed.left - winX) > 2 || labs(landed.top - winY) > 2) {
        // The app genuinely refused or snapped the position. Put the mesh
        // where the window really is so the two still coincide.
        Wh_Log(L"window settled at (%d,%d), asked (%d,%d) - realigning mesh",
               (int)landed.left, (int)landed.top, winX, winY);
        winX = (int)landed.left;
        winY = (int)landed.top;
    }

    mesh.MoveRest((float)winX, (float)winY);
    mesh.SettleFast();

    last = NowSeconds();
    const double settleDeadline = NowSeconds() + 0.30;
    while (!lostDevice && NowSeconds() < settleDeadline && !QuitRequested()) {
        PumpMessages();
        const double now = NowSeconds();
        float dt = (float)(now - last);
        if (dt > 0.05f) dt = 0.05f;
        last = now;
        const bool moving = mesh.Update(dt);
        if (ov.Draw(mesh) == DrawResult::DeviceLost) { lostDevice = true; break; }
        if (!moving) break;
    }

    // Reveal underneath the still-visible overlay, let the app repaint out of
    // sight, then drop the overlay. Hiding it on the same frame as the reveal
    // is what makes a landing flash.
    if (!lostDevice) ov.Draw(mesh);   // one frame at exact rest, as a stand-in
    ShowWindowInPlace(hwnd, hide);    // window returns underneath the overlay
    reveal.armed = false;             // done deliberately; guard must not repeat

    const double holdUntil = NowSeconds() + cfg.revealHoldMs / 1000.0;
    while (!lostDevice && NowSeconds() < holdUntil && !QuitRequested()) {
        PumpMessages();
        if (ov.Draw(mesh) != DrawResult::Ok) break;   // vsync-paced
    }

    ov.Hide();
    return true;
}

// -----------------------------------------------------------------------------
// Worker thread
// -----------------------------------------------------------------------------
// A window belongs to the thread that created it, and Windows destroys every
// window a thread owns when that thread exits. Creating the overlay on a
// per-drag worker therefore destroyed it the moment the drag finished, which
// left the cached swapchain presenting into a dead HWND and nothing on screen.
//
// So: one worker per process, created on first use and kept alive. It owns
// the device and the overlay as locals, which is what keeps them off the
// CRT's exit path: Windhawk does not call Wh_ModUninit when the host process
// exits, so a global's destructor would release DXGI and DirectComposition
// objects from DllMain, under the loader lock, after every other thread has
// been killed.
//
// While idle it waits with MsgWaitForMultipleObjects rather than a plain
// wait, so the overlay's message queue keeps being serviced between drags -
// an unpumped window stalls anyone who sends it a message - and so the wait
// can time out and release the GPU resources.

static DWORD WINAPI WorkerProc(LPVOID) {
    // Everything this thread touches is in physical pixels:
    // DWMWA_EXTENDED_FRAME_BOUNDS always is, while GetWindowRect,
    // GetCursorPos and SetWindowPos follow the process's DPI awareness. In a
    // DPI-unaware app on a scaled monitor the two disagree, which made the
    // frame-bounds check spin its whole budget and the mesh realign onto
    // physical coordinates inside a logical overlay.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    std::optional<Device>  dev;
    std::optional<Overlay> ov;
    double idleSince = 0.0;

    HANDLE waits[2] = { g_dragSignal, g_quitSignal };
    for (;;) {
        DWORD timeout = INFINITE;
        const int idleMs = g_cfg.idleReleaseMs;
        if (ov && idleMs > 0) {
            const double elapsed = (NowSeconds() - idleSince) * 1000.0;
            timeout = elapsed >= idleMs ? 0 : (DWORD)(idleMs - elapsed);
        }

        const DWORD r = MsgWaitForMultipleObjects(2, waits, FALSE, timeout,
                                                  QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) {
            const HWND hwnd = g_pending.hwnd;
            if (!dev) dev.emplace();
            if (!ov) { ov.emplace(); ov->d = &*dev; }

            const bool took = DoWobbleDrag(*dev, *ov, hwnd);
            g_dragging.store(false);
            if (!took) HandBackToWindows(hwnd);

            // A lost device cannot be reused; drop it so the next drag
            // builds a fresh one instead of silently drawing nothing.
            if (dev->Lost()) {
                Wh_Log(L"worker: device lost, releasing");
                ov.reset();
                dev.reset();
            }
            idleSince = NowSeconds();
        } else if (r == WAIT_OBJECT_0 + 1) {
            break;
        } else if (r == WAIT_TIMEOUT) {
            // Idle long enough: give back the swapchain and the device. An
            // app dragged once should not hold them for the rest of its life.
            if (ov || dev) Wh_Log(L"worker: idle, releasing GPU resources");
            ov.reset();
            dev.reset();
        } else {
            PumpMessages();        // keep the overlay's queue alive
        }
    }

    // On the thread that owns them, while the module is still loaded.
    ov.reset();
    dev.reset();
    return 0;
}

static bool EnsureWorker() {
    if (g_worker) return true;
    if (!g_dragSignal || !g_quitSignal) return false;
    g_worker = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    return g_worker != nullptr;
}

// Called from inside the message pump, so it must not block: running the drag
// here froze the window's own thread, and PrintWindow(PW_RENDERFULLCONTENT)
// renders through DWM and needs that thread pumping - it deadlocked against
// the very loop that invoked it. Validate cheaply, hand the work over, return.
//
// Everything that can say "not this window" has to be decided here, before
// the hook returns 0. Once it has, Windows will not start its own move loop,
// so a later refusal means the user's drag does nothing at all.
static bool BeginWobbleDrag(HWND hwnd) {
    if (g_shuttingDown.load()) return false;

    // No button down means this is a keyboard or window-menu Move
    // (Alt+Space, M), which has no cursor to follow. Let Windows have it.
    if (!PrimaryButtonDown()) return false;

    if (BypassHeld()) return false;
    if (!Eligible(hwnd)) return false;
    if (!CanHideReliably(hwnd)) return false;

    bool expected = false;
    if (!g_dragging.compare_exchange_strong(expected, true)) return false;

    if (!EnsureWorker()) {
        g_dragging.store(false);
        Wh_Log(L"skip: no worker thread");
        return false;
    }

    g_pending.hwnd = hwnd;
    SetEvent(g_dragSignal);
    return true;
}

// Lets a press we handed back reach Windows instead of being taken again.
static bool ShouldPassThrough(HWND hwnd) {
    if (g_handBack.load() != hwnd) return false;
    g_handBack.store(nullptr);
    return true;
}

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------
// WM_NCLBUTTONDOWN/HTCAPTION reaching DefWindowProc is exactly where Windows
// would start its own modal move loop. Running ours instead and returning 0
// means Windows never moves the window.
//
// WM_NCLBUTTONDOWN is a POSTED message, so it passes through DispatchMessage
// on its way to the window procedure. Catching it there works no matter what
// the application does with it afterwards - which matters because apps with
// custom chrome (XAML, Electron, Chromium) frequently handle it themselves
// and never reach DefWindowProc at all. DefWindowProc is kept as a second net
// for messages that are sent rather than posted, and for WM_SYSCOMMAND/SC_MOVE
// which is how a custom title bar usually asks for a move.

using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageW_t DispatchMessageW_orig;

static LRESULT WINAPI DispatchMessageW_hook(const MSG* msg) {
    if (msg && msg->message == WM_NCLBUTTONDOWN && msg->wParam == HTCAPTION &&
        !g_dragging.load() && BeginWobbleDrag(msg->hwnd))
        return 0;
    return DispatchMessageW_orig(msg);
}

using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageA_t DispatchMessageA_orig;

static LRESULT WINAPI DispatchMessageA_hook(const MSG* msg) {
    if (msg && msg->message == WM_NCLBUTTONDOWN && msg->wParam == HTCAPTION &&
        !g_dragging.load() && BeginWobbleDrag(msg->hwnd))
        return 0;
    return DispatchMessageA_orig(msg);
}

using DefWindowProc_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
static DefWindowProc_t DefWindowProcW_orig;
static DefWindowProc_t DefWindowProcA_orig;

static bool ShouldTake(HWND hWnd, UINT msg, WPARAM wp) {
    if (g_dragging.load()) return false;
    if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_MOVE) {
        if (ShouldPassThrough(hWnd)) return false;
        return BeginWobbleDrag(hWnd);
    }
    if (msg == WM_NCLBUTTONDOWN && wp == HTCAPTION)
        return BeginWobbleDrag(hWnd);
    return false;
}

static LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (ShouldTake(hWnd, msg, wp)) return 0;
    return DefWindowProcW_orig(hWnd, msg, wp, lp);
}

static LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (ShouldTake(hWnd, msg, wp)) return 0;
    return DefWindowProcA_orig(hWnd, msg, wp, lp);
}

// -----------------------------------------------------------------------------
// Entry points
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    g_dragSignal = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_quitSignal = CreateEventW(nullptr, TRUE,  FALSE, nullptr);
    if (!g_dragSignal || !g_quitSignal) {
        Wh_Log(L"init: could not create events");
        return FALSE;
    }

    const BOOL w  = WindhawkUtils::SetFunctionHook(
        DefWindowProcW, DefWindowProcW_hook, &DefWindowProcW_orig);
    const BOOL a  = WindhawkUtils::SetFunctionHook(
        DefWindowProcA, DefWindowProcA_hook, &DefWindowProcA_orig);
    const BOOL dw = WindhawkUtils::SetFunctionHook(
        DispatchMessageW, DispatchMessageW_hook, &DispatchMessageW_orig);
    const BOOL da = WindhawkUtils::SetFunctionHook(
        DispatchMessageA, DispatchMessageA_hook, &DispatchMessageA_orig);

    Wh_Log(L"init: hooks defW=%d defA=%d dispW=%d dispA=%d", w, a, dw, da);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    // Stop new drags from starting. The hooks are still live at this point,
    // so without this a caption press could create a worker that nothing
    // joins. Signal the existing one, but do not wait here: the wait belongs
    // in Wh_ModUninit, where the hooks are already gone.
    g_shuttingDown.store(true);
    if (g_quitSignal) SetEvent(g_quitSignal);
}

void Wh_ModUninit() {
    // The hooks are removed by now, so no new worker can appear. Every loop
    // in the worker checks the quit event, so this returns promptly; waiting
    // without a timeout is what makes the unload safe, because the module is
    // unmapped the moment this function returns and a worker still running
    // inside it would fault on its next instruction.
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_dragSignal) { CloseHandle(g_dragSignal); g_dragSignal = nullptr; }
    if (g_quitSignal) { CloseHandle(g_quitSignal); g_quitSignal = nullptr; }

    // The worker destroyed the overlay window before exiting, so the class
    // has no windows left and can go. Leaving it registered would make the
    // next load silently reuse it.
    if (g_classRegistered.exchange(false))
        UnregisterClassW(kOverlayClass, MOD_HINSTANCE);

    g_api.Free();

    Wh_Log(L"uninit: done");
}

// ==WindhawkMod==
// @id              window-wobble
// @name            Window Wobble
// @description     KDE-style spring-physics window dragging, without injecting into dwm.exe
// @version         1.0.0
// @author          heyjeay
// @github          https://github.com/heyjeay
// @homepage        https://github.com/heyjeay/wobblywin
// @license         GPL-3.0-or-later
// @include         *
// @exclude         dwm.exe
// @exclude         sihost.exe
// @exclude         RuntimeBroker.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         TextInputHost.exe
// @exclude         ApplicationFrameHost.exe
// @exclude         NVDisplay.Container.exe
// @exclude         csrss.exe
// @exclude         winlogon.exe
// @exclude         lsass.exe
// @exclude         services.exe
// @exclude         smss.exe
// @exclude         LogonUI.exe
// @architecture    x86-64
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ld3dcompiler -ldwmapi -lgdi32 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -DNTDDI_VERSION=0x0A000006
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Wobble

KDE-style spring-physics window dragging. Grab a title bar and the window
follows your cursor like jelly - the title bar keeps up, the body trails
behind, and it settles with a wobble when you let go.

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

**Questions, problems or ideas? Join the Discord:**
https://discord.gg/8wwfPxfMWA

## How it works

A title-bar press is caught at `DispatchMessage`, with `DefWindowProc` as a
backup and `WM_SYSCOMMAND`/`SC_MOVE` alongside for apps that draw their own
title bars. The mod then runs its own move loop in place of the one Windows
would start: the window is snapshotted, cloaked so it leaves composition
entirely, and a spring mesh of the snapshot is drawn on a Direct3D 11 +
DirectComposition overlay until you let go.

Only top-level windows are touched, and only ones DWM will cloak - anything
else is handed straight back to Windows rather than animated unreliably.

## Excluded processes

System and shell processes are excluded: `dwm.exe`, `csrss.exe`,
`winlogon.exe`, `lsass.exe`, `services.exe`, `smss.exe`, `LogonUI.exe`,
`sihost.exe`, `RuntimeBroker.exe`, `StartMenuExperienceHost.exe`,
`ShellExperienceHost.exe`, `SearchHost.exe`, `TextInputHost.exe` and
`ApplicationFrameHost.exe`.

None of them own a window you would drag, so the mod could only add risk
there - and `dwm.exe` composites everything on screen, so a fault in it is
not a recoverable one.

## Notes

Window content is a snapshot for the duration of the drag, so a playing video
appears frozen while you hold it. This is inherent to the approach - capturing
live content per frame is far more expensive than one `PrintWindow`.
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
- diagnosticLog: 0
  $name: Write a diagnostic log
  $description: Appends to %TEMP%\wobbly-windhawk.log. Off unless you are chasing a problem; it writes on every drag.
- revealHoldMs: 90
  $name: Reveal hold (ms)
  $description: How long the settled overlay stays up while the real window repaints underneath. Raise it if you see a flash on landing.
- dragThreshold: 4
  $name: Drag threshold (px)
  $description: How far the pointer must move before the wobble takes over. Below this a title-bar click behaves normally.
- bypassKey: shift
  $name: Bypass key
  $description: Hold this while grabbing a title bar to move the window normally.
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

#include <windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <d3dcompiler.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <vector>

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------
// Windhawk 1.7.x validates settings as integers; $float is a 2.0 annotation and
// fails here. Values that are conceptually fractional are stored scaled and
// divided on load.

struct Settings {
    float springK      = 200.0f;
    float bottomRatio  = 0.65f;
    float meshK        = 3.0f;
    float damping      = 14.0f;
    float impulse      = 0.12f;
    // Fixed, not user-tunable: critical damping is d = 2*sqrt(k), and the
    // pair has to move together. ~14ms time constant at a 4ms frame, which
    // lands in roughly 100ms from any realistic drag.
    float settleK      = 12000.0f;
    float settleD      = 210.0f;
    int   mesh         = 16;
    int   bypassVk     = VK_SHIFT;
    int   threshold    = 4;
    int   revealHoldMs = 90;
    int   debugLog     = 0;
};
static Settings g_cfg;

// -----------------------------------------------------------------------------
// Diagnostics
// -----------------------------------------------------------------------------
// Wh_Log goes to Windhawk's log channel, which its engine owns at a higher
// integrity level than an ordinary process can attach to. For diagnosis the
// mod mirrors its log to %TEMP%\wobbly-windhawk.log, which anyone can read.
// Mods load into ~100 processes, so a named mutex serialises appends.

static void FileLog(PCWSTR fmt, ...) {
    if (!g_cfg.debugLog) return;

    wchar_t line[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = _vsnwprintf(line, 1000, fmt, ap);
    va_end(ap);
    if (n < 0) return;
    line[n < 1000 ? n : 1000] = 0;

    wchar_t path[MAX_PATH];
    DWORD len = GetTempPathW(MAX_PATH, path);
    if (!len || len > MAX_PATH - 32) return;
    wcscpy(path + len, L"wobbly-windhawk.log");

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t full[1200];
    _snwprintf(full, 1199, L"%02d:%02d:%02d.%03d [%lu] %s\r\n",
               st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
               GetCurrentProcessId(), line);
    full[1199] = 0;

    char utf8[2400];
    int bytes = WideCharToMultiByte(CP_UTF8, 0, full, -1, utf8, sizeof(utf8),
                                    nullptr, nullptr);
    if (bytes <= 1) return;

    // FILE_APPEND_DATA (without FILE_WRITE_DATA) makes each small write an
    // atomic append, so concurrent writers need no lock between them.
    HANDLE fh = CreateFileW(path, FILE_APPEND_DATA,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fh != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(fh, utf8, (DWORD)(bytes - 1), &written, nullptr);
        CloseHandle(fh);
    }
}

// Mirror every diagnostic to both sinks.
#define WLOG(...) do { Wh_Log(__VA_ARGS__); FileLog(__VA_ARGS__); } while (0)

// Startup timings are recorded whatever the log setting says: they are one
// line per process and are needed exactly when someone reports a slow first
// drag, which is the moment logging is likely to be off.


// Windhawk 2.0 rejects integer-keyed $options, so the bypass key is stored as
// a name and mapped here. An unknown value falls back to Shift rather than to
// "disabled", so a typo cannot silently remove the escape hatch.
static int ReadBypassVk() {
    PCWSTR name = Wh_GetStringSetting(L"bypassKey");
    int vk = VK_SHIFT;
    if (!wcscmp(name, L"ctrl"))      vk = VK_CONTROL;
    else if (!wcscmp(name, L"alt"))  vk = VK_MENU;
    else if (!wcscmp(name, L"none")) vk = 0;
    Wh_FreeStringSetting(name);
    return vk;
}

static void LoadSettings() {
    g_cfg.springK     = (float)Wh_GetIntSetting(L"stiffness");
    g_cfg.bottomRatio = Wh_GetIntSetting(L"bottomSoftness") / 100.0f;
    g_cfg.meshK       = Wh_GetIntSetting(L"clothRigidity") / 10.0f;
    g_cfg.damping     = Wh_GetIntSetting(L"damping") / 10.0f;
    g_cfg.impulse     = Wh_GetIntSetting(L"impulse") / 100.0f;
    g_cfg.mesh        = Wh_GetIntSetting(L"meshSize");
    g_cfg.bypassVk    = ReadBypassVk();
    g_cfg.debugLog    = Wh_GetIntSetting(L"diagnosticLog");
    g_cfg.revealHoldMs = Wh_GetIntSetting(L"revealHoldMs");
    if (g_cfg.revealHoldMs < 0 || g_cfg.revealHoldMs > 1000) g_cfg.revealHoldMs = 90;
    g_cfg.threshold   = Wh_GetIntSetting(L"dragThreshold");
    if (g_cfg.threshold < 1) g_cfg.threshold = 4;

    if (g_cfg.springK <= 0.0f) g_cfg.springK = 200.0f;
    if (g_cfg.damping <= 0.0f) g_cfg.damping = 14.0f;
    if (g_cfg.mesh < 4)  g_cfg.mesh = 4;
    if (g_cfg.mesh > 40) g_cfg.mesh = 40;

    Wh_Log(L"settings: k=%d ratio=%d%% meshK=%d damp=%d settleK=%d mesh=%d bypass=%d",
           (int)g_cfg.springK, (int)(g_cfg.bottomRatio * 100), (int)g_cfg.meshK,
           (int)g_cfg.damping, (int)g_cfg.settleK, g_cfg.mesh, g_cfg.bypassVk);
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
static RECT VisibleRectSettled(HWND hwnd) {
    for (int attempt = 0; attempt < 40; ++attempt) {      // <= ~40ms
        RECT outer{};
        if (!GetWindowRect(hwnd, &outer)) break;
        const RECT vis = VisibleRect(hwnd);

        const LONG dl = vis.left   - outer.left;
        const LONG dt = vis.top    - outer.top;
        const LONG dr = outer.right  - vis.right;
        const LONG db = outer.bottom - vis.bottom;

        // Plausible: inset on every side, and never by more than a border.
        if (dl >= 0 && dt >= 0 && dr >= 0 && db >= 0 &&
            dl <= 32 && dt <= 32 && dr <= 32 && db <= 32)
            return vis;

        Sleep(1);
    }
    RECT outer{};
    GetWindowRect(hwnd, &outer);
    WLOG(L"frame bounds never settled; using window rect");
    return outer;
}

// -----------------------------------------------------------------------------
// Capture
// -----------------------------------------------------------------------------

struct Bitmap32 {
    int w = 0, h = 0;
    std::vector<uint32_t> px;   // BGRA, premultiplied on upload
    bool valid() const { return w > 0 && h > 0 && !px.empty(); }
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

    if (!IsZoomed(hwnd)) {
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
    bool  settled = true;
    float bandK = 0.0f, bandD = 0.0f;   // non-zero during settle

    void Setup(int cells, float x, float y, float w, float h) {
        nx = ny = cells + 1;
        rest.resize((size_t)nx * ny);
        pos.resize(rest.size());
        vel.assign(rest.size(), Vec2{0, 0});
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
                v.x -= vx * w * g_cfg.impulse;
                v.y -= vy * w * g_cfg.impulse;
            }
        }
        settled = false;
    }

    void SettleFast() {
        bandK = g_cfg.settleK;
        bandD = g_cfg.settleD;
        settled = false;
    }

    bool Update(float dt) {
        if (settled) return false;
        const bool band = bandK > 0.0f;
        const float kBase = band ? bandK : g_cfg.springK;
        const float d     = band ? bandD : g_cfg.damping;

        std::vector<Vec2> f(pos.size());
        float maxDisp = 0.0f;

        for (int j = 0; j < ny; ++j) {
            // Uniform while settling; row-graded while dragging.
            const float t = (ny > 1) ? (float)j / (ny - 1) : 0.0f;
            const float k = band ? kBase
                                 : kBase * (1.0f + (g_cfg.bottomRatio - 1.0f) * t);
            for (int i = 0; i < nx; ++i) {
                const size_t n = (size_t)j * nx + i;
                const float ex = pos[n].x - rest[n].x;
                const float ey = pos[n].y - rest[n].y;
                maxDisp = fmaxf(maxDisp, fmaxf(fabsf(ex), fabsf(ey)));
                f[n] = Vec2{ -k * ex, -k * ey };
            }
        }

        if (g_cfg.meshK > 0.0f) {
            const float km = g_cfg.meshK;
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

static const char kShaderSrc[] = R"(
cbuffer CB : register(b0) { float2 origin; float2 size; };
struct VSIn  { float2 pos : POSITION; float2 uv : TEXCOORD; };
struct VSOut { float4 pos : SV_POSITION; float2 uv : TEXCOORD; };
Texture2D    tex : register(t0);
SamplerState smp : register(s0);

VSOut VS(VSIn i) {
    VSOut o;
    // Screen pixels -> NDC. The overlay spans the whole virtual desktop, whose
    // origin is negative when a monitor sits left of or above the primary.
    float2 n = (i.pos - origin) / size * 2.0 - 1.0;
    o.pos = float4(n.x, -n.y, 0, 1);
    o.uv  = i.uv;
    return o;
}
float4 PS(VSOut i) : SV_TARGET {
    float4 c = tex.Sample(smp, i.uv);
    return float4(c.rgb * c.a, c.a);   // swapchain is premultiplied alpha
}
)";

struct Vertex { float x, y, u, v; };

// The device and shaders are size-independent and expensive to build, so they
// are made once per process and kept. Only the swapchain and texture, which
// depend on the window being dragged, are rebuilt per drag.
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

    bool Init() {
        if (ok) return true;
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL got{};
        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                       flags, nullptr, 0, D3D11_SDK_VERSION,
                                       &dev, &got, &ctx);
        if (FAILED(hr)) {
            // Some hosts cannot get a hardware device; WARP still composites.
            WLOG(L"D3D11 hardware device failed hr=0x%08X, trying WARP", hr);
            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                                   flags, nullptr, 0, D3D11_SDK_VERSION,
                                   &dev, &got, &ctx);
        }
        if (FAILED(hr)) { WLOG(L"D3D11CreateDevice failed hr=0x%08X", hr); return false; }

        Com<ID3DBlob> vsb, psb, err;
        hr = D3DCompile(kShaderSrc, sizeof(kShaderSrc) - 1, nullptr, nullptr,
                        nullptr, "VS", "vs_4_0", 0, 0, &vsb, &err);
        if (FAILED(hr)) {
            WLOG(L"VS compile failed hr=0x%08X: %S", hr,
                   err.p ? (const char*)err->GetBufferPointer() : "?");
            return false;
        }
        hr = D3DCompile(kShaderSrc, sizeof(kShaderSrc) - 1, nullptr, nullptr,
                        nullptr, "PS", "ps_4_0", 0, 0, &psb, &err);
        if (FAILED(hr)) {
            WLOG(L"PS compile failed hr=0x%08X: %S", hr,
                   err.p ? (const char*)err->GetBufferPointer() : "?");
            return false;
        }
        if (FAILED(dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &vs)))
            return false;
        if (FAILED(dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &ps)))
            return false;

        D3D11_INPUT_ELEMENT_DESC el[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (FAILED(dev->CreateInputLayout(el, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), &layout)))
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
};
static Device g_dev;

struct Overlay {
    HWND hwnd = nullptr;
    Com<IDXGISwapChain1>        swap;
    Com<ID3D11RenderTargetView> rtv;
    Com<IDCompositionDevice>    dcomp;
    Com<IDCompositionTarget>    target;
    Com<IDCompositionVisual>    visual;
    Com<ID3D11Texture2D>        tex;
    Com<ID3D11ShaderResourceView> srv;
    Com<ID3D11Buffer>           vb;
    int vx = 0, vy = 0, vw = 0, vh = 0;
    int vertCount = 0;
    std::vector<Vertex> verts;

    bool EnsureSurface();                      // once per process
    void WarmPipeline();                       // force the driver to compile
    void WarmPresent();                        // make DWM build its target
    bool SetContent(const Bitmap32& bmp, int cells);  // per drag
    void Show();
    void Hide();
    void Destroy();
    void Draw(const Mesh& m);
};

// Built once per process and reused; see Overlay::Hide.
static Overlay g_overlay;

static const wchar_t kOverlayClass[] = L"WobblyWindowsOverlay";

static void EnsureClass() {
    static bool done = false;
    if (done) return;
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = DefWindowProcW;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.lpszClassName = kOverlayClass;
    RegisterClassExW(&wc);
    done = true;
}

bool Overlay::EnsureSurface() {
    if (swap.p) return true;   // already built
    if (!g_dev.Init()) { WLOG(L"overlay: D3D device init failed"); return false; }
    EnsureClass();

    vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // NOREDIRECTIONBITMAP: no GDI surface, DirectComposition supplies the
    // content. TRANSPARENT|NOACTIVATE keep it click-through and unfocusable.
    hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_POPUP,
        vx, vy, vw, vh, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) { WLOG(L"overlay: CreateWindowEx failed err=%u", GetLastError()); return false; }

    Com<IDXGIDevice> dxgiDev;
    if (FAILED(g_dev.dev->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev))) return false;
    Com<IDXGIAdapter> adapter;
    if (FAILED(dxgiDev->GetAdapter(&adapter))) return false;
    Com<IDXGIFactory2> factory;
    if (FAILED(adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory))) return false;

    DXGI_SWAP_CHAIN_DESC1 sd{};
    sd.Width            = vw;
    sd.Height           = vh;
    sd.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount      = 2;
    sd.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;
    HRESULT hr = factory->CreateSwapChainForComposition(g_dev.dev, &sd, nullptr, &swap);
    if (FAILED(hr)) { WLOG(L"overlay: swapchain failed hr=0x%08X (%dx%d)", hr, vw, vh); return false; }

    Com<ID3D11Texture2D> back;
    if (FAILED(swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back))) return false;
    if (FAILED(g_dev.dev->CreateRenderTargetView(back, nullptr, &rtv))) return false;

    hr = DCompositionCreateDevice(dxgiDev, __uuidof(IDCompositionDevice), (void**)&dcomp);
    if (FAILED(hr)) { WLOG(L"overlay: DCompositionCreateDevice failed hr=0x%08X", hr); return false; }
    hr = dcomp->CreateTargetForHwnd(hwnd, TRUE, &target);
    if (FAILED(hr)) { WLOG(L"overlay: CreateTargetForHwnd failed hr=0x%08X", hr); return false; }
    if (FAILED(dcomp->CreateVisual(&visual))) return false;
    visual->SetContent(swap);
    target->SetRoot(visual);
    dcomp->Commit();

    return true;
}

void Overlay::WarmPipeline() {
    if (!g_dev.ok) return;

    // Presenting to the swapchain is useless here: the overlay window is
    // still hidden, so DXGI treats the frame as occluded and the driver never
    // does the work. Render into a small offscreen target instead, which does
    // not care about window visibility, and block on a query until the GPU
    // has actually finished - otherwise the shader compilation is merely
    // deferred to the first real frame, which is the stall being removed.
    const UINT kSide = 64;

    D3D11_TEXTURE2D_DESC rt{};
    rt.Width = rt.Height = kSide;
    rt.MipLevels = rt.ArraySize = 1;
    rt.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
    rt.SampleDesc.Count = 1;
    rt.Usage            = D3D11_USAGE_DEFAULT;
    rt.BindFlags        = D3D11_BIND_RENDER_TARGET;
    Com<ID3D11Texture2D> target;
    Com<ID3D11RenderTargetView> targetView;
    if (FAILED(g_dev.dev->CreateTexture2D(&rt, nullptr, &target)))           return;
    if (FAILED(g_dev.dev->CreateRenderTargetView(target, nullptr, &targetView))) return;

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
    if (FAILED(g_dev.dev->CreateTexture2D(&td, &srd, &t)))           return;
    if (FAILED(g_dev.dev->CreateShaderResourceView(t, nullptr, &v))) return;

    // A triangle that genuinely covers the target, so the pixel shader runs.
    const Vertex tri[3] = { {0,0,0,0}, {(float)kSide,0,1,0}, {0,(float)kSide,0,1} };
    D3D11_BUFFER_DESC vbd{};
    vbd.ByteWidth = sizeof(tri);
    vbd.Usage     = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vsd{}; vsd.pSysMem = tri;
    Com<ID3D11Buffer> b;
    if (FAILED(g_dev.dev->CreateBuffer(&vbd, &vsd, &b))) return;

    auto* ctx = g_dev.ctx.p;

    struct CB { float ox, oy, sw, sh; } cbv{ 0, 0, (float)kSide, (float)kSide };
    D3D11_MAPPED_SUBRESOURCE map{};
    if (SUCCEEDED(ctx->Map(g_dev.cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
        memcpy(map.pData, &cbv, sizeof(cbv));
    ctx->Unmap(g_dev.cb, 0);

    const float clear[4] = { 0, 0, 0, 0 };
    ID3D11RenderTargetView* rts[] = { targetView.p };
    ctx->OMSetRenderTargets(1, rts, nullptr);
    ctx->ClearRenderTargetView(targetView, clear);
    D3D11_VIEWPORT vp{ 0, 0, (float)kSide, (float)kSide, 0, 1 };
    ctx->RSSetViewports(1, &vp);
    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer* vbs[] = { b.p };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetInputLayout(g_dev.layout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(g_dev.vs, nullptr, 0);
    ID3D11Buffer* cbs[] = { g_dev.cb.p };
    ctx->VSSetConstantBuffers(0, 1, cbs);
    ctx->PSSetShader(g_dev.ps, nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = { v.p };
    ctx->PSSetShaderResources(0, 1, srvs);
    ID3D11SamplerState* smps[] = { g_dev.sampler.p };
    ctx->PSSetSamplers(0, 1, smps);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(g_dev.blend, bf, 0xFFFFFFFF);
    ctx->Draw(3, 0);

    // Block until the GPU is done, so the compile happens here and not later.
    D3D11_QUERY_DESC qd{}; qd.Query = D3D11_QUERY_EVENT;
    Com<ID3D11Query> q;
    if (SUCCEEDED(g_dev.dev->CreateQuery(&qd, &q))) {
        ctx->End(q);
        ctx->Flush();
        BOOL done = FALSE;
        const double limit = NowSeconds() + 1.0;
        while (ctx->GetData(q, &done, sizeof(done), 0) != S_OK &&
               NowSeconds() < limit)
            Sleep(0);
    } else {
        ctx->Flush();
    }
}

void Overlay::WarmPresent() {
    if (!swap.p || !hwnd) return;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    const float clear[4] = { 0, 0, 0, 0 };          // fully transparent
    ID3D11RenderTargetView* rts[] = { rtv.p };
    g_dev.ctx->OMSetRenderTargets(1, rts, nullptr);
    g_dev.ctx->ClearRenderTargetView(rtv, clear);
    swap->Present(0, 0);
    ShowWindow(hwnd, SW_HIDE);
}

bool Overlay::SetContent(const Bitmap32& bmp, int cells) {
    if (!EnsureSurface()) return false;
    HRESULT hr = S_OK;

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
    hr = g_dev.dev->CreateTexture2D(&td, &srd, &tex);
    if (FAILED(hr)) { WLOG(L"overlay: texture %dx%d failed hr=0x%08X", bmp.w, bmp.h, hr); return false; }
    if (FAILED(g_dev.dev->CreateShaderResourceView(tex, nullptr, &srv))) return false;

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
    if (FAILED(g_dev.dev->CreateBuffer(&vbd, nullptr, &vb))) return false;

    return true;
}

void Overlay::Show() {
    if (hwnd) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
}

void Overlay::Hide() {
    // Hidden, not destroyed: rebuilding the swapchain per drag is what makes
    // the app hitch when the drag ends.
    if (hwnd) ShowWindow(hwnd, SW_HIDE);
}

void Overlay::Destroy() {
    srv.reset(); tex.reset(); vb.reset(); rtv.reset();
    visual.reset(); target.reset(); dcomp.reset(); swap.reset();
    if (hwnd) { DestroyWindow(hwnd); hwnd = nullptr; }
}

void Overlay::Draw(const Mesh& m) {
    const int cells = m.nx - 1;
    int n = 0;
    for (int j = 0; j < cells; ++j) {
        for (int i = 0; i < cells; ++i) {
            const size_t a = (size_t)j * m.nx + i,       b = a + 1;
            const size_t c = a + m.nx,                   d = c + 1;
            const float u0 = (float)i / cells,  u1 = (float)(i + 1) / cells;
            const float v0 = (float)j / cells,  v1 = (float)(j + 1) / cells;
            const Vertex quad[6] = {
                { m.pos[a].x, m.pos[a].y, u0, v0 },
                { m.pos[b].x, m.pos[b].y, u1, v0 },
                { m.pos[c].x, m.pos[c].y, u0, v1 },
                { m.pos[b].x, m.pos[b].y, u1, v0 },
                { m.pos[d].x, m.pos[d].y, u1, v1 },
                { m.pos[c].x, m.pos[c].y, u0, v1 },
            };
            for (const auto& vtx : quad) verts[n++] = vtx;
        }
    }

    auto* ctx = g_dev.ctx.p;
    D3D11_MAPPED_SUBRESOURCE map{};
    if (SUCCEEDED(ctx->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
        memcpy(map.pData, verts.data(), sizeof(Vertex) * vertCount);
    ctx->Unmap(vb, 0);

    struct CB { float ox, oy, sw, sh; } cbv{ (float)vx, (float)vy, (float)vw, (float)vh };
    if (SUCCEEDED(ctx->Map(g_dev.cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map)))
        memcpy(map.pData, &cbv, sizeof(cbv));
    ctx->Unmap(g_dev.cb, 0);

    const float clear[4] = { 0, 0, 0, 0 };
    ID3D11RenderTargetView* rts[] = { rtv.p };
    ctx->OMSetRenderTargets(1, rts, nullptr);
    ctx->ClearRenderTargetView(rtv, clear);

    D3D11_VIEWPORT vp{ 0, 0, (float)vw, (float)vh, 0, 1 };
    ctx->RSSetViewports(1, &vp);

    UINT stride = sizeof(Vertex), offset = 0;
    ID3D11Buffer* vbs[] = { vb.p };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetInputLayout(g_dev.layout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(g_dev.vs, nullptr, 0);
    ID3D11Buffer* cbs[] = { g_dev.cb.p };
    ctx->VSSetConstantBuffers(0, 1, cbs);
    ctx->PSSetShader(g_dev.ps, nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = { srv.p };
    ctx->PSSetShaderResources(0, 1, srvs);
    ID3D11SamplerState* smps[] = { g_dev.sampler.p };
    ctx->PSSetSamplers(0, 1, smps);
    const float bf[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(g_dev.blend, bf, 0xFFFFFFFF);
    ctx->Draw(vertCount, 0);

    swap->Present(1, 0);
}

// -----------------------------------------------------------------------------
// Hiding the real window
// -----------------------------------------------------------------------------
// SW_HIDE would strip the taskbar button. WS_EX_LAYERED + alpha=0 makes the
// window invisible in place without moving it, so there is no DWM animation to
// produce a ghost, and the taskbar is untouched. In-process we are always
// allowed to do this - no UIPI involved.
//
// Apps driving their own per-pixel alpha through UpdateLayeredWindow must be
// left alone: SetLayeredWindowAttributes would permanently switch their blend
// mode. GetLayeredWindowAttributes failing is how we detect them.

enum class HideVia { None, Cloak, Layered };

struct HideState {
    HideVia via        = HideVia::None;
    bool    hadLayered = false;
    BYTE    keepAlpha  = 255;
    DWORD   keepFlags  = LWA_ALPHA;
    bool    hadCorner  = false;   // corner preference was readable
    DWORD   corner     = 0;       // and this is what it was
};

static constexpr DWORD DWMWA_CLOAK_ATTR  = 13;
static constexpr DWORD DWMWA_CORNER_ATTR = 33;   // WINDOW_CORNER_PREFERENCE

static bool HideWindowInPlace(HWND hwnd, HideState& st) {
    DWORD corner = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CORNER_ATTR,
                                        &corner, sizeof(corner)))) {
        st.hadCorner = true;
        st.corner    = corner;
    }

    // Preferred: ask DWM to cloak the window. This is what Windows itself uses
    // for virtual desktops - it removes the window from composition entirely,
    // including DirectComposition content, which a layered alpha does not.
    {
        BOOL on = TRUE;
        if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK_ATTR,
                                            &on, sizeof(on)))) {
            st.via = HideVia::Cloak;
            return true;
        }
    }

    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    st.hadLayered = (ex & WS_EX_LAYERED) != 0;

    if (st.hadLayered) {
        COLORREF key = 0; BYTE a = 255; DWORD fl = 0;
        if (!GetLayeredWindowAttributes(hwnd, &key, &a, &fl))
            return false;                       // per-pixel alpha - do not touch
        st.keepAlpha = a;
        st.keepFlags = fl;
    } else {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    }

    if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
        if (!st.hadLayered) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex);
        return false;
    }
    st.via = HideVia::Layered;
    return true;
}

// Whether cloaking is available tells us whether hiding will actually work:
// a layered alpha leaves DirectComposition content visible, so for a window
// DWM will not cloak we would rather not start at all than show two of it.
static bool CanHideReliably(HWND hwnd) {
    BOOL on = FALSE;    // a no-op write; we only care whether DWM accepts it
    return SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK_ATTR,
                                           &on, sizeof(on)));
}

static void RestoreCorners(HWND hwnd, const HideState& st) {
    if (!st.hadCorner) return;
    DWORD corner = st.corner;
    DwmSetWindowAttribute(hwnd, DWMWA_CORNER_ATTR, &corner, sizeof(corner));
}

static void ShowWindowInPlace(HWND hwnd, const HideState& st) {
    if (st.via == HideVia::None) return;

    if (st.via == HideVia::Cloak) {
        BOOL off = FALSE;
        DwmSetWindowAttribute(hwnd, DWMWA_CLOAK_ATTR, &off, sizeof(off));
        RestoreCorners(hwnd, st);
        return;
    }

    if (st.hadLayered) {
        SetLayeredWindowAttributes(hwnd, 0, st.keepAlpha,
                                   st.keepFlags ? st.keepFlags : LWA_ALPHA);
    } else {
        // Opaque first, so the window comes back from DWM's existing surface
        // with no repaint; only then drop the style bit we added. The other
        // order makes the style change itself the thing that reveals it, which
        // forces a full repaint exactly when it becomes visible.
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        RestoreCorners(hwnd, st);
    }
}

// -----------------------------------------------------------------------------
// The drag
// -----------------------------------------------------------------------------

static std::atomic<bool> g_dragging{false};

static bool BypassHeld() {
    return g_cfg.bypassVk && (GetAsyncKeyState(g_cfg.bypassVk) & 0x8000);
}

static bool Eligible(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;

    // Top-level only. A child window's SetWindowPos coordinates are relative
    // to its parent while GetWindowRect reports screen coordinates, so moving
    // one by screen position lands it at parent + offset - the drop was
    // overshooting by exactly the parent's origin. Child windows are also not
    // independently composited, so DWM cloaking fails on them and the layered
    // fallback cannot hide DirectComposition content, leaving the real window
    // on screen underneath the overlay. That is the duplicate.
    if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    // The drag runs on a worker thread and polls the pointer, so it needs no
    // mouse capture and no particular thread affinity.
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

static void DoWobbleDrag(HWND hwnd, POINT down) {

    // Wait for the pointer to actually move before taking over. Returning
    // TRUE from the hook means Windows never sees the click, so without this
    // a plain title-bar click - the ordinary way to focus and raise a window -
    // got swallowed, and paid for a hide, a D3D device and a settle to do it.
    // Warm the GPU resources while the window is still visible and before the
    // pointer has moved, so the one-time cost lands where nothing is on
    // screen waiting for it.
    {
        if (!g_overlay.EnsureSurface()) {
            WLOG(L"abort: overlay surface unavailable");
            return;
        }
        g_overlay.WarmPipeline();

        // Show and present once, then hide again. The overlay is cleared to
        // fully transparent so nothing appears, but DWM builds its
        // composition target here rather than during the first real drag.
        g_overlay.WarmPresent();

    }

    bool moved = false;
    int  spins = 0;
    const double waitStart = NowSeconds();
    while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (NowSeconds() - waitStart > 10.0) {
            WLOG(L"skip: button held 10s without moving - giving up");
            return;
        }
        POINT p{};
        GetCursorPos(&p);
        if (BypassHeld()) {
            WLOG(L"skip: bypass pressed during wait");
            return;
        }
        if (abs(p.x - down.x) > g_cfg.threshold ||
            abs(p.y - down.y) > g_cfg.threshold) { moved = true; break; }
        // Sleep(1) actually sleeps ~15ms at the default timer resolution, so
        // the old wait sampled the pointer only ~6 times across an 83ms press.
        // Yield instead, and pump occasionally so the app stays responsive.
        if ((spins & 7) == 0) PumpMessages();
        Sleep(1);
        ++spins;
    }
    if (!moved) {
        WLOG(L"skip: no movement after %d ms (%d polls, button down=%d)",
             (int)((NowSeconds() - waitStart) * 1000.0), spins,
             (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0);
        return;
    }
    WLOG(L"threshold passed after %d ms",
         (int)((NowSeconds() - waitStart) * 1000.0));

    const bool wasZoomed = IsZoomed(hwnd) != 0;

    // Un-maximize under the cursor, the way Windows does when you drag a
    // maximized title bar - the pointer keeps its relative spot on the bar.
    // This happens while the window is still visible, which costs one frame
    // of snap, exactly as the native behaviour does.
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

    // Capture BEFORE hiding. Hiding first made the window a fully transparent
    // layered surface, and PrintWindow then had nothing to render - every
    // drag showed a solid black rectangle.
    Bitmap32 bmp;
    if (!CaptureWindow(hwnd, bmp)) {
        WLOG(L"abort: capture failed");
        return;
    }
    WLOG(L"captured %dx%d", bmp.w, bmp.h);

    // Measure while the window is still composed: extended frame bounds on a
    // cloaked window are not dependable, and a wrong border offset is exactly
    // what makes the overlay and the real window disagree about position.
    const RECT startRc = VisibleRectSettled(hwnd);
    RECT outerRc{}; GetWindowRect(hwnd, &outerRc);
    const int frameDx0 = startRc.left - outerRc.left;
    const int frameDy0 = startRc.top  - outerRc.top;
    WLOG(L"start: visible (%d,%d) %dx%d border (%d,%d) | captured %dx%d",
         (int)startRc.left, (int)startRc.top,
         (int)(startRc.right - startRc.left),
         (int)(startRc.bottom - startRc.top),
         frameDx0, frameDy0, bmp.w, bmp.h);

    if (!CanHideReliably(hwnd)) {
        WLOG(L"abort: DWM will not cloak this window - leaving it to Windows");
        return;
    }

    const RECT start   = startRc;
    const int  frameDx = frameDx0;
    const int  frameDy = frameDy0;

    // Put the overlay up BEFORE hiding the window, showing the captured image
    // at exactly the window's own position. Hiding first left a gap where
    // neither was on screen - short, but a visible blink at the start of
    // every drag. This is the same handoff used when landing, in reverse.
    Mesh mesh;
    mesh.Setup(g_cfg.mesh, (float)start.left, (float)start.top,
               (float)(start.right - start.left),
               (float)(start.bottom - start.top));

    Overlay& ov = g_overlay;
    if (!ov.SetContent(bmp, g_cfg.mesh)) {
        WLOG(L"abort: overlay setup failed");
        return;
    }
    ov.Draw(mesh);          // pixel-identical to the window beneath it
    ov.Show();

    HideState hide;
    if (!HideWindowInPlace(hwnd, hide)) {
        // Per-pixel alpha app, or the call was refused - leave it to Windows.
        WLOG(L"abort: cannot hide window in place");
        ov.Hide();
        return;
    }
    WLOG(L"hidden via %s (corner pref %s)",
         hide.via == HideVia::Cloak ? L"DWM cloak" : L"layered alpha",
         hide.hadCorner ? L"saved" : L"NOT READABLE - rounding may be lost");

    // From here the window is invisible, which is the worst thing to leave
    // behind. Reveal on every exit path - early return, exception, anything.
    struct Reveal {
        HWND h; const HideState& st; bool armed = true;
        ~Reveal() { if (armed) ShowWindowInPlace(h, st); }
    } reveal{hwnd, hide};

    WLOG(L"overlay up, entering drag loop");

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

    const double dragDeadline = NowSeconds() + 120.0;
    while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        if (NowSeconds() > dragDeadline) {
            WLOG(L"drag exceeded 120s - ending it");
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
        ov.Draw(mesh);
    }

    if (cancelled) { winX = start.left; winY = start.top; }

    mesh.Kick(prevVx * 0.5f, prevVy * 0.5f);

    // Move first, while still invisible, then ask where it actually went.
    SetWindowPos(hwnd, nullptr, winX - frameDx, winY - frameDy, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

    // SetWindowPos on a window owned by another thread is asynchronous: it
    // queues the change and returns before the window has moved. Reading the
    // rect straight back gives the OLD position, and uncloaking at that point
    // leaves the overlay at the new spot while the window is still at the old
    // one - which is the duplicate. Wait for the move to actually land.
    const double moveDeadline = NowSeconds() + 0.25;
    RECT landed = VisibleRect(hwnd);
    while (NowSeconds() < moveDeadline) {
        landed = VisibleRect(hwnd);
        if (labs(landed.left - winX) <= 2 && labs(landed.top - winY) <= 2)
            break;
        Sleep(1);
    }
    if (labs(landed.left - winX) > 2 || labs(landed.top - winY) > 2) {
        // The app genuinely refused or snapped the position. Put the mesh
        // where the window really is so the two still coincide.
        WLOG(L"window settled at (%d,%d), asked (%d,%d) - realigning mesh",
             (int)landed.left, (int)landed.top, winX, winY);
        winX = (int)landed.left;
        winY = (int)landed.top;
    }

    mesh.MoveRest((float)winX, (float)winY);
    mesh.SettleFast();

    last = NowSeconds();
    const double settleStart    = NowSeconds();
    const double settleDeadline = settleStart + 0.30;
    int  settleFrames = 0;
    bool settledCleanly = false;
    while (NowSeconds() < settleDeadline) {
        PumpMessages();
        const double now = NowSeconds();
        float dt = (float)(now - last);
        if (dt > 0.05f) dt = 0.05f;
        last = now;
        const bool moving = mesh.Update(dt);
        ov.Draw(mesh);
        ++settleFrames;
        if (!moving) { settledCleanly = true; break; }
    }
    WLOG(L"settle: %d ms, %d frames, converged=%d",
         (int)((NowSeconds() - settleStart) * 1000.0), settleFrames,
         settledCleanly ? 1 : 0);

    // Reveal underneath the still-visible overlay, let the app repaint out of
    // sight, then drop the overlay. Hiding it on the same frame as the reveal
    // is what makes a landing flash.
    {
        const RECT shown = VisibleRect(hwnd);
        const int meshW = (int)(mesh.rest[(size_t)mesh.nx - 1].x - mesh.rest[0].x);
        const int meshH = (int)(mesh.rest[(size_t)(mesh.ny - 1) * mesh.nx].y
                                - mesh.rest[0].y);
        WLOG(L"reveal: mesh (%d,%d) %dx%d | window (%d,%d) %dx%d | captured %dx%d",
             (int)mesh.rest[0].x, (int)mesh.rest[0].y, meshW, meshH,
             (int)shown.left, (int)shown.top,
             (int)(shown.right - shown.left), (int)(shown.bottom - shown.top),
             bmp.w, bmp.h);
    }

    ov.Draw(mesh);                 // one frame at exact rest, as a stand-in
    ShowWindowInPlace(hwnd, hide); // window returns underneath the overlay
    reveal.armed = false;          // done deliberately; guard must not repeat it

    const double holdUntil = NowSeconds() + g_cfg.revealHoldMs / 1000.0;
    while (NowSeconds() < holdUntil) {
        PumpMessages();
        ov.Draw(mesh);             // vsync-paced, so this is a few frames
    }

    ov.Hide();
    WLOG(L"drag done at (%d,%d)", winX, winY);
    return;
}

// A window belongs to the thread that created it, and Windows destroys every
// window a thread owns when that thread exits. Creating the overlay on a
// per-drag worker therefore destroyed it the moment the drag finished, which
// left the cached swapchain presenting into a dead HWND and nothing on screen.
//
// So: one worker per process, created on first use and kept alive. It owns the
// overlay for the life of the process. While idle it waits with
// MsgWaitForMultipleObjects rather than a plain wait, so the overlay's message
// queue keeps being serviced between drags - an unpumped window stalls anyone
// who sends it a message.

struct DragRequest { HWND hwnd; POINT down; };

static HANDLE      g_worker      = nullptr;
static HANDLE      g_dragSignal  = nullptr;   // raised to start a drag
static HANDLE      g_quitSignal  = nullptr;   // raised to retire the worker
static DragRequest g_pending     = {};

static DWORD WINAPI WorkerProc(LPVOID) {
    HANDLE waits[2] = { g_dragSignal, g_quitSignal };
    for (;;) {
        const DWORD r = MsgWaitForMultipleObjects(2, waits, FALSE, INFINITE,
                                                  QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) {
            DoWobbleDrag(g_pending.hwnd, g_pending.down);
            g_dragging.store(false);
        } else if (r == WAIT_OBJECT_0 + 1) {
            break;
        } else {
            PumpMessages();        // keep the overlay's queue alive
        }
    }
    g_overlay.Destroy();           // on the thread that owns it
    return 0;
}

static bool EnsureWorker() {
    if (g_worker) return true;
    if (!g_dragSignal) g_dragSignal = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_quitSignal) g_quitSignal = CreateEventW(nullptr, TRUE,  FALSE, nullptr);
    if (!g_dragSignal || !g_quitSignal) return false;
    g_worker = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    return g_worker != nullptr;
}

// Called from inside the message pump, so it must not block: running the drag
// here froze the window's own thread, and PrintWindow(PW_RENDERFULLCONTENT)
// renders through DWM and needs that thread pumping - it deadlocked against
// the very loop that invoked it. Validate cheaply, hand the work over, return.
static bool BeginWobbleDrag(HWND hwnd) {
    if (BypassHeld()) {
        WLOG(L"skip: bypass vk=0x%02X held (shift=%d ctrl=%d alt=%d)",
             g_cfg.bypassVk,
             (GetAsyncKeyState(VK_SHIFT)   & 0x8000) ? 1 : 0,
             (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? 1 : 0,
             (GetAsyncKeyState(VK_MENU)    & 0x8000) ? 1 : 0);
        return false;
    }
    if (!Eligible(hwnd)) {
        WLOG(L"skip: not eligible (visible=%d iconic=%d)",
             IsWindowVisible(hwnd), IsIconic(hwnd));
        return false;
    }
    bool expected = false;
    if (!g_dragging.compare_exchange_strong(expected, true)) return false;

    if (!EnsureWorker()) {
        g_dragging.store(false);
        WLOG(L"skip: no worker thread");
        return false;
    }

    g_pending.hwnd = hwnd;
    GetCursorPos(&g_pending.down);
    SetEvent(g_dragSignal);

    WLOG(L"drag start hwnd=%p (bypass vk=0x%02X state=0x%04X shift=%d)",
         (void*)hwnd, g_cfg.bypassVk,
         (unsigned short)GetAsyncKeyState(g_cfg.bypassVk),
         (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 1 : 0);
    return true;
}

// -----------------------------------------------------------------------------
// Hook
// -----------------------------------------------------------------------------
// WM_NCLBUTTONDOWN/HTCAPTION reaching DefWindowProc is exactly where Windows
// would start its own modal move loop. Running ours instead and returning 0
// means Windows never moves the window, and we inherit its semantics for free:
// the caller is already blocked for the duration of a drag.

// WM_NCLBUTTONDOWN is a POSTED message, so it passes through DispatchMessage
// on its way to the window procedure. Catching it there works no matter what
// the application does with it afterwards - which matters because apps with
// custom chrome (XAML, Electron, Chromium) frequently handle it themselves and
// never reach DefWindowProc at all. DefWindowProc is kept as a second net for
// messages that are sent rather than posted.

// Every message pump retrieves posted messages through GetMessage or
// PeekMessage before anything else sees them, so this is the broadest net
// available and does not depend on how the app dispatches afterwards.
// Rewriting the retrieved message to WM_NULL consumes it, which is what stops
// Windows from starting its own move loop once we have taken the drag.


using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageW_t DispatchMessageW_orig;

static LRESULT WINAPI DispatchMessageW_hook(const MSG* msg) {
    if (msg && msg->message == WM_NCLBUTTONDOWN && !g_dragging.load()) {
        WLOG(L"dispW: NCLBUTTONDOWN hit=%d hwnd=%p", (int)msg->wParam,
               (void*)msg->hwnd);
        if (msg->wParam == HTCAPTION && BeginWobbleDrag(msg->hwnd)) return 0;
    }
    return DispatchMessageW_orig(msg);
}

using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageA_t DispatchMessageA_orig;

static LRESULT WINAPI DispatchMessageA_hook(const MSG* msg) {
    if (msg && msg->message == WM_NCLBUTTONDOWN &&
        msg->wParam == HTCAPTION && !g_dragging.load()) {
        if (BeginWobbleDrag(msg->hwnd)) return 0;
    }
    return DispatchMessageA_orig(msg);
}

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
static DefWindowProcW_t DefWindowProcW_orig;

static LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_MOVE && !g_dragging.load()) {
        WLOG(L"defW: SC_MOVE hwnd=%p", (void*)hWnd);
        if (BeginWobbleDrag(hWnd)) return 0;
    }
    if (msg == WM_NCLBUTTONDOWN && !g_dragging.load()) {
        WLOG(L"defW: NCLBUTTONDOWN hit=%d hwnd=%p", (int)wp, (void*)hWnd);
        if (wp == HTCAPTION && BeginWobbleDrag(hWnd)) return 0;
    }
    return DefWindowProcW_orig(hWnd, msg, wp, lp);
}

using DefWindowProcA_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
static DefWindowProcA_t DefWindowProcA_orig;

static LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_MOVE && !g_dragging.load()) {
        WLOG(L"defA: SC_MOVE hwnd=%p", (void*)hWnd);
        if (BeginWobbleDrag(hWnd)) return 0;
    }
    if (msg == WM_NCLBUTTONDOWN && wp == HTCAPTION && !g_dragging.load()) {
        if (BeginWobbleDrag(hWnd)) return 0;
    }
    return DefWindowProcA_orig(hWnd, msg, wp, lp);
}

BOOL Wh_ModInit() {
    LoadSettings();

    // Resolve the targets out of user32 explicitly rather than taking the
    // address of the imported name. Depending on how the import is declared,
    // `(void*)DefWindowProcW` can be the address of a thunk inside this DLL
    // rather than the real function - hooking that would only affect calls the
    // mod itself makes, which is silently useless.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) { Wh_Log(L"init: no user32"); return FALSE; }

    void* pW = (void*)GetProcAddress(user32, "DefWindowProcW");
    void* pA = (void*)GetProcAddress(user32, "DefWindowProcA");
    Wh_Log(L"init: user32 DefWindowProcW=%p (import-name resolves to %p)",
           pW, (void*)DefWindowProcW);
    if (!pW || !pA) { Wh_Log(L"init: GetProcAddress failed"); return FALSE; }

    const BOOL w = Wh_SetFunctionHook(pW, (void*)DefWindowProcW_hook,
                                      (void**)&DefWindowProcW_orig);
    const BOOL a = Wh_SetFunctionHook(pA, (void*)DefWindowProcA_hook,
                                      (void**)&DefWindowProcA_orig);


    void* pDW = (void*)GetProcAddress(user32, "DispatchMessageW");
    void* pDA = (void*)GetProcAddress(user32, "DispatchMessageA");
    const BOOL dw = pDW && Wh_SetFunctionHook(pDW, (void*)DispatchMessageW_hook,
                                              (void**)&DispatchMessageW_orig);
    const BOOL da = pDA && Wh_SetFunctionHook(pDA, (void*)DispatchMessageA_hook,
                                              (void**)&DispatchMessageA_orig);

    Wh_Log(L"init: hooks defW=%d defA=%d dispW=%d dispA=%d", w, a, dw, da);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"after-init: hooks applied");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    // The worker must be gone before the module is, or it faults the instant
    // it next executes an instruction. This runs earlier than Wh_ModUninit
    // and while the DLL is still guaranteed loaded, which makes it the safe
    // place to join; waiting here also lets a drag in flight finish first.
    if (g_quitSignal) SetEvent(g_quitSignal);
    if (g_worker && WaitForSingleObject(g_worker, 5000) == WAIT_TIMEOUT)
        Wh_Log(L"worker did not exit in time");
}

void Wh_ModUninit() {
    // Give a drag in flight a moment to finish, but never block unload on it:
    // unloading is the remedy when a mod misbehaves, so it must always work.
    for (int i = 0; i < 200 && g_dragging.load(); ++i) Sleep(10);   // <= 2s

    // Drop the cached device before the DLL goes away. Leaving COM objects
    // owned by this module alive past unload crashes the host the next time
    // anything touches them.
    if (g_worker) {                       // signalled and joined above
        CloseHandle(g_worker); g_worker = nullptr;
    }
    if (g_dragSignal) { CloseHandle(g_dragSignal); g_dragSignal = nullptr; }
    if (g_quitSignal) { CloseHandle(g_quitSignal); g_quitSignal = nullptr; }

    g_dev.cb.reset();      g_dev.blend.reset();   g_dev.sampler.reset();
    g_dev.layout.reset();  g_dev.ps.reset();      g_dev.vs.reset();
    g_dev.ctx.reset();     g_dev.dev.reset();
    g_dev.ok = false;
    Wh_Log(L"uninit: released D3D device");
}

// ==WindhawkMod==
// @id              genie-and-friends-minimize-animation
// @name            Genie + Friends minimize animation pack
// @description     GPU-accelerated minimise/restore effects that lock each animation to the correct taskbar icon. Genie (true mesh bend), Vacuum, Glide, Pop, Slide, Free Fall, Warp, Squash, Roll-Up & Swirl. Windows 10 + 11.
// @version         2.3.0
// @author          akilluminati47
// @github          https://github.com/akilluminati47
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld3d11 -ldxgi -ldcomp -ld3dcompiler -lole32 -loleaut32 -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Genie + Friends minimize animation pack

A standalone pack that reproduces the classic minimize/restore effects of
macOS and the Compiz-era Linux desktops, rendered on the GPU. Pick one from
the **Animation style** dropdown in the mod's Settings tab; that dropdown is
your toggle between Genie and the nine other reproductions. Each style has
its own **Duration** slider so you can tune the feel of every effect
independently, and there's a master **Enable animations** switch to fall back
to stock Windows without disabling the mod.

## The effects

- **Genie (Magic Lamp)**: the macOS classic. The window stretches and pours
  down into the taskbar like a genie into a lamp.
- **Vacuum**: the whole window shrinks and accelerates as it gets sucked into
  the taskbar icon.
- **Glide**: GNOME-style shrink + fade in place. Understated and clean.
- **Pop**: the window swells slightly and vanishes. Snappy and modern.
- **Slide**: KDE-style straight drop off the bottom edge.
- **Free Fall**: gravity takes over; the window accelerates, stretches, sways,
  and tumbles off the bottom of the screen.
- **Warp**: Star Trek transporter. The window squeezes into a thin vertical
  beam of light, then the beam shoots up and dematerializes.
- **Squash**: the window is flattened like a pancake onto the taskbar.
- **Roll-Up**: the window rolls up into its own title bar like a window blind.
- **Swirl**: a whirlpool. The window spins side-to-side while shrinking down
  into the taskbar like water down a drain.

Restore plays every effect in reverse, so windows *un-genie*, *un-roll*, drop
back in, etc.

## How it renders

The window snapshot is handed to the GPU compositor (DirectComposition) once,
and each frame only pushes a transform + opacity. This stays smooth on
high-refresh displays (120/144/165/180+ Hz) and uses a fraction of the CPU a
per-frame `StretchBlt` renderer would. Genie goes further: the snapshot is
rendered as a tessellated mesh warped every frame by a small GPU shader, so
the window body curves into a thin neck that pours into the taskbar (the real
Magic Lamp look). The other nine effects use the cheap single-transform GPU
path. If the GPU/shader path can't initialize in a given process, the mod
silently falls back to a GDI renderer, so nothing breaks.

## Changelog

- **v2.3.0**: minimise/restore animation now locks to the exact taskbar icon
  position (horizontal centre + button width) rather than the cursor click
  point, so every effect converges on the correct icon. Full taskbar button
  enumeration with owner-chain, root-ancestor, and process-ID matching ensures
  grouped and child windows find their icon. Removed cursor-based fallback.
  Fixes a grey flash on minimise/restore (double-animation guard, DwmFlush
  before ShowWindow, restore cloak/uncloak sequencing, ghost-window background
  brush). Genie mesh neck width and per-effect min scale now match the icon
  button's pixel width. Windows 11 22H2+ are supported, with fallback scanning
  paths for every known taskbar layout (ReBar, WorkerW, MSTaskSwWClass).
- **v2.2.3**: ghost windows no longer render DWM drop shadows, preventing
  shadow stacking or phantom shadows when the real window has shadows disabled.
- **v2.2.2**: the pack stands on its own as `genie-and-friends-minimize-animation`.
- **v2.2.1**: first-frame anti-flash. The minimize/restore hook waits (up to
  40ms) for the ghost's first frame to actually be on screen before the real
  window changes, so the window can't blink out a few ms before the animation
  appears when GPU setup is momentarily slow.
- **v2.2**: true Genie bend via tessellated mesh + GPU warp shader, replacing
  the shrink + slide approximation.
- **v2.1**: GPU rendering via DirectComposition, with GDI fallback.
- Windows' own animation API is ancient, so a couple of effects (Warp
  especially) are clever fakes rather than true 3D, but they read great in
  motion.

## Credits

The effect selection and the original Genie math originate from **lolstijl**'s
multi-effect pack, which itself built on the original Genie Animation Mod.
This pack is maintained by **akilluminati47**, who wrote the GPU
(DirectComposition) renderer, the mesh-warped Genie bend, and the anti-flash
timing fix. Development of v2.3.0 (icon-targeted animation, grey-flash
resolved, Windows 11 support) was assisted by Claude.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable animations
  $description: >-
    Master switch. Turn this off to fall back to Windows' default minimize/restore
    behavior without having to disable the whole mod.

- style: genie
  $name: Animation style
  $description: >-
    Which effect plays when you minimize or restore a window. This is your toggle
    between the classic Genie and the other reproduced effects. Restore always
    plays the same effect in reverse.
  $options:
  - genie: Genie - Magic Lamp (pours into the taskbar)
  - vacuum: Vacuum - sucked into the taskbar icon
  - glide: Glide - GNOME-style shrink & fade
  - pop: Pop - swell and vanish
  - slide: Slide - straight drop off the bottom (KDE)
  - fall: Free Fall - gravity tumble off screen
  - warp: Warp - Star Trek transporter beam
  - squash: Squash - flattened onto the taskbar
  - rollup: Roll-Up - rolls up like a window blind
  - swirl: Swirl - whirlpool down the drain

- duration_genie: 450
  $name: Duration - Genie (ms)
  $description: Clamped to 50-3000. Lower is snappier, higher is more deliberate.

- duration_vacuum: 380
  $name: Duration - Vacuum (ms)
  $description: Clamped to 50-3000.

- duration_glide: 300
  $name: Duration - Glide (ms)
  $description: Clamped to 50-3000.

- duration_pop: 260
  $name: Duration - Pop (ms)
  $description: Clamped to 50-3000.

- duration_slide: 340
  $name: Duration - Slide (ms)
  $description: Clamped to 50-3000.

- duration_fall: 620
  $name: Duration - Free Fall (ms)
  $description: Clamped to 50-3000.

- duration_warp: 520
  $name: Duration - Warp (ms)
  $description: Clamped to 50-3000.

- duration_squash: 400
  $name: Duration - Squash (ms)
  $description: Clamped to 50-3000.

- duration_rollup: 380
  $name: Duration - Roll-Up (ms)
  $description: Clamped to 50-3000.

- duration_swirl: 700
  $name: Duration - Swirl (ms)
  $description: Clamped to 50-3000.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <math.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <string>
#include <ole2.h>
#include <uiautomation.h>
#include <shellapi.h>

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

enum AnimMode {
    MODE_GENIE = 0,
    MODE_VACUUM,
    MODE_GLIDE,
    MODE_POP,
    MODE_SLIDE,
    MODE_FALL,
    MODE_WARP,
    MODE_SQUASH,
    MODE_ROLLUP,
    MODE_SWIRL,
    MODE_COUNT
};

static const wchar_t* kModeKeys[MODE_COUNT] = {
    L"genie", L"vacuum", L"glide", L"pop", L"slide",
    L"fall",  L"warp",   L"squash", L"rollup", L"swirl"
};
static const wchar_t* kDurKeys[MODE_COUNT] = {
    L"duration_genie", L"duration_vacuum", L"duration_glide", L"duration_pop",
    L"duration_slide", L"duration_fall",   L"duration_warp",  L"duration_squash",
    L"duration_rollup", L"duration_swirl"
};

struct GhostAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;
    RECT targetRect;
    int width;
    int height;
    int targetDockX;
    int iconButtonWidth;
    BOOL isRising;
    int mode;
    int durationMs;
    LONG_PTR originalExStyle;
    BOOL wasCloaked;
    HANDLE hReady;
};

std::unordered_map<HWND, HBITMAP> g_SnapshotCache;
std::mutex g_CacheMutex;

std::atomic<bool> g_enabled{true};
std::atomic<int>  g_mode{MODE_GENIE};
std::atomic<int>  g_durations[MODE_COUNT];

void LoadSettings() {
    g_enabled.store(Wh_GetIntSetting(L"enabled") != 0, std::memory_order_relaxed);
    for (int i = 0; i < MODE_COUNT; i++) {
        int ms = Wh_GetIntSetting(kDurKeys[i]);
        if (ms < 50) ms = 50;
        if (ms > 3000) ms = 3000;
        g_durations[i].store(ms, std::memory_order_relaxed);
    }
    int mode = MODE_GENIE;
    PCWSTR style = Wh_GetStringSetting(L"style");
    if (style) {
        for (int i = 0; i < MODE_COUNT; i++) {
            if (wcscmp(style, kModeKeys[i]) == 0) { mode = i; break; }
        }
        Wh_FreeStringSetting(style);
    }
    g_mode.store(mode, std::memory_order_relaxed);
}

void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// Disable DWM drop-shadow on the ghost so we never add, remove, or change
// shadows during the animation. The snapshot only contains the window rect
// (shadows live outside that rect), so the ghost must not render its own.
static void DisableGhostShadow(HWND hGhost) {
    if (!hGhost) return;
    DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(hGhost, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
}

template <class T> static inline void SafeRelease(T*& p) {
    if (p) { p->Release(); p = nullptr; }
}

static std::mutex        g_gpuInitMutex;
static std::mutex        g_gpuCtxMutex;
static bool              g_gpuInitTried = false;
static bool              g_gpuAvailable = false;
static ID3D11Device*     g_d3dDevice    = nullptr;
static IDXGIFactory2*    g_dxgiFactory  = nullptr;

static bool EnsureGpuDevice() {
    std::lock_guard<std::mutex> lock(g_gpuInitMutex);
    if (g_gpuInitTried) return g_gpuAvailable;
    g_gpuInitTried = true;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL fl;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, &fl, nullptr);
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
            nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, &fl, nullptr);
    }
    if (FAILED(hr) || !g_d3dDevice) return false;

    IDXGIDevice* dxgiDev = nullptr;
    IDXGIAdapter* adapter = nullptr;
    if (SUCCEEDED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev)) &&
        SUCCEEDED(dxgiDev->GetAdapter(&adapter)) &&
        SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&g_dxgiFactory))) {
        g_gpuAvailable = true;
    }
    SafeRelease(adapter);
    SafeRelease(dxgiDev);

    if (!g_gpuAvailable) SafeRelease(g_d3dDevice);
    return g_gpuAvailable;
}

static void ReleaseGpuDevice() {
    std::lock_guard<std::mutex> lock(g_gpuInitMutex);
    SafeRelease(g_dxgiFactory);
    SafeRelease(g_d3dDevice);
    g_gpuAvailable = false;
    g_gpuInitTried = false;
}

static const int  GENIE_GX = 8;
static const int  GENIE_GY = 64;

struct GenieVertex { float x, y; float u, v; };

static std::mutex             g_genieMutex;
static bool                   g_genieTried = false;
static bool                   g_genieOk    = false;
static ID3D11VertexShader*    g_gVS       = nullptr;
static ID3D11PixelShader*     g_gPS       = nullptr;
static ID3D11InputLayout*     g_gLayout   = nullptr;
static ID3D11Buffer*          g_gIndexBuf = nullptr;
static ID3D11Buffer*          g_gConstBuf = nullptr;
static ID3D11SamplerState*    g_gSampler  = nullptr;
static ID3D11RasterizerState* g_gRaster   = nullptr;
static UINT                   g_gIndexCount = 0;

static const char* kGenieVS =
    "struct VIn  { float2 pos:POSITION; float2 tex:TEXCOORD; };\n"
    "struct VOut { float4 pos:SV_POSITION; float2 tex:TEXCOORD; };\n"
    "VOut main(VIn i){ VOut o; o.pos=float4(i.pos,0,1); o.tex=i.tex; return o; }\n";

static const char* kGeniePS =
    "Texture2D gTex:register(t0); SamplerState gSmp:register(s0);\n"
    "cbuffer C:register(b0){ float gAlpha; float3 pad; };\n"
    "struct VOut { float4 pos:SV_POSITION; float2 tex:TEXCOORD; };\n"
    "float4 main(VOut i):SV_TARGET{\n"
    "  float4 c = gTex.Sample(gSmp, i.tex);\n"
    "  return float4(c.rgb * gAlpha, gAlpha);\n"
    "}\n";

static bool EnsureGenieGpuResources() {
    std::lock_guard<std::mutex> lock(g_genieMutex);
    if (g_genieTried) return g_genieOk;
    g_genieTried = true;
    if (!g_d3dDevice) return false;

    ID3DBlob* vsb = nullptr;
    ID3DBlob* psb = nullptr;
    ID3DBlob* err = nullptr;
    HRESULT hr = D3DCompile(kGenieVS, strlen(kGenieVS), "genieVS", nullptr, nullptr,
                            "main", "vs_4_0", 0, 0, &vsb, &err);
    SafeRelease(err);
    if (SUCCEEDED(hr))
        hr = D3DCompile(kGeniePS, strlen(kGeniePS), "geniePS", nullptr, nullptr,
                        "main", "ps_4_0", 0, 0, &psb, &err);
    SafeRelease(err);
    if (FAILED(hr)) { SafeRelease(vsb); SafeRelease(psb); return false; }

    bool ok =
        SUCCEEDED(g_d3dDevice->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &g_gVS)) &&
        SUCCEEDED(g_d3dDevice->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &g_gPS));

    if (ok) {
        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        ok = SUCCEEDED(g_d3dDevice->CreateInputLayout(il, 2, vsb->GetBufferPointer(),
                                                      vsb->GetBufferSize(), &g_gLayout));
    }
    SafeRelease(vsb);
    SafeRelease(psb);
    if (!ok) return false;

    {
        const int cols = GENIE_GX + 1;
        UINT* idx = (UINT*)malloc((size_t)GENIE_GX * GENIE_GY * 6 * sizeof(UINT));
        if (!idx) return false;
        UINT n = 0;
        for (int j = 0; j < GENIE_GY; j++) {
            for (int i = 0; i < GENIE_GX; i++) {
                UINT tl = (UINT)(j * cols + i);
                UINT tr = tl + 1;
                UINT bl = tl + cols;
                UINT br = bl + 1;
                idx[n++] = tl; idx[n++] = bl; idx[n++] = tr;
                idx[n++] = tr; idx[n++] = bl; idx[n++] = br;
            }
        }
        g_gIndexCount = n;
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.ByteWidth = (UINT)(n * sizeof(UINT));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.pSysMem = idx;
        HRESULT ihr = g_d3dDevice->CreateBuffer(&bd, &sd, &g_gIndexBuf);
        free(idx);
        if (FAILED(ihr)) return false;
    }

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.ByteWidth = 16;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_d3dDevice->CreateBuffer(&bd, nullptr, &g_gConstBuf))) return false;
    }

    {
        D3D11_SAMPLER_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        if (FAILED(g_d3dDevice->CreateSamplerState(&sd, &g_gSampler))) return false;
    }

    {
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));
        rd.FillMode = D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_NONE;
        if (FAILED(g_d3dDevice->CreateRasterizerState(&rd, &g_gRaster))) return false;
    }

    g_genieOk = true;
    return true;
}

static void ReleaseGenieGpuResources() {
    std::lock_guard<std::mutex> lock(g_genieMutex);
    SafeRelease(g_gRaster);
    SafeRelease(g_gSampler);
    SafeRelease(g_gConstBuf);
    SafeRelease(g_gIndexBuf);
    SafeRelease(g_gLayout);
    SafeRelease(g_gPS);
    SafeRelease(g_gVS);
    g_genieOk = false;
    g_genieTried = false;
}

static inline float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
static inline float easeInCubic(float t)  { return t * t * t; }
static inline float easeOutCubic(float t) { float u = 1.0f - t; return 1.0f - u * u * u; }
static inline float easeInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static void SolveFrame(const GhostAnimData* d, float t,
                       int SW, int SH, float taskbarY,
                       int capW, int capH,
                       int* outW, int* outH, int* outX, int* outY, float* outAlpha) {
    const int   w = d->width;
    const int   h = d->height;
    const int   L = d->targetRect.left;
    const int   T = d->targetRect.top;
    const float cx = L + w * 0.5f;
    const float cy = T + h * 0.5f;
    const int   dockX = d->targetDockX;

    float minScaleX = 0.0f;
    if (d->iconButtonWidth > 0 && w > 0) {
        minScaleX = (float)d->iconButtonWidth / (float)w;
        if (minScaleX >= 1.0f) minScaleX = 0.99f;
    }

    float scaleX = 1.0f, scaleY = 1.0f;
    float fx = cx, fy = cy;
    float alpha = 1.0f;
    bool  anchorTopLeft = false;

    switch (d->mode) {

    case MODE_GENIE: {
        float invT  = 1.0f - t;
        float moveX = 1.0f - (invT * invT * invT * invT * invT * invT);
        float moveY = (0.70f * (t * t)) + (0.10f * t);
        scaleX = 1.0f - (0.95f * (1.8f * t));
        { float ms = 0.05f > minScaleX ? 0.05f : minScaleX; if (scaleX < ms) scaleX = ms; }
        scaleY = 1.0f - (0.70f * (t * t));

        float startCenterX = cx;
        float startY       = (float)T;
        float targetDockY  = taskbarY + h;
        float curCenterX   = startCenterX + ((float)dockX - startCenterX) * moveX;
        float curTopY      = startY + (targetDockY - startY) * moveY;
        anchorTopLeft = true;
        fx = curCenterX;
        fy = curTopY;
        if (t > 0.6f) alpha = 1.0f - ((t - 0.6f) / 0.4f);
        break;
    }

    case MODE_VACUUM: {
        float e = easeInCubic(t);
        float s = 1.0f - 0.97f * e;
        { float ms = 0.03f > minScaleX ? 0.03f : minScaleX; if (s < ms) s = ms; }
        scaleX = scaleY = s;
        fx = cx + (dockX - cx) * e;
        fy = cy + (taskbarY - cy) * e;
        if (t > 0.75f) alpha = 1.0f - ((t - 0.75f) / 0.25f);
        break;
    }

    case MODE_GLIDE: {
        float e = easeOutCubic(t);
        scaleX = scaleY = 1.0f - 0.12f * e;
        alpha = 1.0f - e;
        break;
    }

    case MODE_POP: {
        float e = easeOutCubic(t);
        scaleX = scaleY = 1.0f + 0.18f * e;
        alpha = 1.0f - e;
        break;
    }

    case MODE_SLIDE: {
        float e = easeInCubic(t);
        anchorTopLeft = true;
        fx = (float)L;
        fy = T + (SH - T + 5) * e;
        if (t > 0.70f) alpha = 1.0f - ((t - 0.70f) / 0.30f);
        break;
    }

    case MODE_FALL: {
        float e = t * t;
        scaleX = 1.0f - 0.10f * t;
        scaleY = 1.0f + 0.20f * t;
        float drift = (w * 0.15f) * sinf(t * 3.0f);
        anchorTopLeft = true;
        fx = L + drift;
        fy = T + (SH - T + h) * e;
        if (t > 0.60f) alpha = 1.0f - ((t - 0.60f) / 0.40f);
        break;
    }

    case MODE_WARP: {
        float ramp = t / 0.6f; if (ramp > 1.0f) ramp = 1.0f;
        scaleX = 1.0f - 0.96f * ramp;
        { float ms = 0.04f > minScaleX ? 0.04f : minScaleX; if (scaleX < ms) scaleX = ms; }
        scaleY = 1.0f;
        anchorTopLeft = true;
        fx = cx;
        fy = (float)T;
        if (t > 0.6f) {
            float up  = (t - 0.6f) / 0.4f;
            float upE = easeInCubic(up);
            scaleY = 1.0f - 0.40f * up;
            fy = T - (cy + h) * upE;
            alpha = 1.0f - up;
        }
        break;
    }

    case MODE_SQUASH: {
        float e = easeInCubic(t);
        scaleY = 1.0f - 0.97f * e;
        scaleX = 1.0f + 0.10f * e;
        anchorTopLeft = true;
        fx = cx;
        fy = T + (taskbarY - T) * e;
        if (t > 0.75f) alpha = 1.0f - ((t - 0.75f) / 0.25f);
        break;
    }

    case MODE_ROLLUP: {
        float e = easeInOutCubic(t);
        scaleY = 1.0f - e;
        scaleX = 1.0f;
        anchorTopLeft = true;
        fx = (float)L;
        fy = (float)T;
        if (t > 0.85f) alpha = 1.0f - ((t - 0.85f) / 0.15f);
        break;
    }

    case MODE_SWIRL: {
        float e = easeInCubic(t);
        float s = 1.0f - 0.95f * e;
        { float ms = 0.05f > minScaleX ? 0.05f : minScaleX; if (s < ms) s = ms; }
        scaleX = scaleY = s;
        float baseCX = cx + (dockX - cx) * e;
        float baseCY = cy + (taskbarY - cy) * e;
        float amp = (w * 0.40f) * (1.0f - t);
        fx = baseCX + amp * sinf(t * 18.0f);
        fy = baseCY + amp * 0.5f * cosf(t * 18.0f);
        if (t > 0.70f) alpha = 1.0f - ((t - 0.70f) / 0.30f);
        break;
    }

    default:
        break;
    }

    int newW = (int)(w * scaleX);
    int newH = (int)(h * scaleY);
    if (newW < 2) newW = 2;
    if (newH < 1) newH = 1;
    if (newW > capW) newW = capW;
    if (newH > capH) newH = capH;

    int px, py;
    if (anchorTopLeft) {
        if (d->mode == MODE_GENIE || d->mode == MODE_WARP || d->mode == MODE_SQUASH)
            px = (int)(fx - newW / 2.0f);
        else
            px = (int)fx;
        py = (int)fy;
    } else {
        px = (int)(fx - newW / 2.0f);
        py = (int)(fy - newH / 2.0f);
    }

    *outW = newW;
    *outH = newH;
    *outX = px;
    *outY = py;
    *outAlpha = clampf(alpha, 0.0f, 1.0f);
}

static void FinalizeRealWindow(GhostAnimData* data) {
    if (data->isRising) {
        if (data->wasCloaked) {
            BOOL cloaked = FALSE;
            DwmSetWindowAttribute(data->hRealWnd, DWMWA_CLOAK, &cloaked, sizeof(cloaked));
        }
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
        }
    }
    SetDwmTransitions(data->hRealWnd, TRUE);
}

static void RunCpuAnim(GhostAnimData* data, HWND hGhost,
                       int screenWidth, int screenHeight, float taskbarY) {
    int capW = (int)(data->width  * 1.30f) + 4;
    int capH = (int)(data->height * 1.30f) + 4;

    HDC hScreenDC = GetDC(NULL);
    HDC hOrigDC   = CreateCompatibleDC(hScreenDC);
    HDC hScaledDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hScaledBitmap = CreateCompatibleBitmap(hScreenDC, capW, capH);
    HBITMAP hOldOrig   = (HBITMAP)SelectObject(hOrigDC, data->hBitmap);
    HBITMAP hOldScaled = (HBITMAP)SelectObject(hScaledDC, hScaledBitmap);
    SetStretchBltMode(hScaledDC, HALFTONE);
    SetBrushOrgEx(hScaledDC, 0, 0, NULL);

    const double totalMs = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float t = data->isRising ? (1.0f - progress) : progress;

        int newW, newH, currentX, currentY;
        float alphaFloat;
        SolveFrame(data, t, screenWidth, screenHeight, taskbarY, capW, capH,
                   &newW, &newH, &currentX, &currentY, &alphaFloat);
        BYTE alpha = (BYTE)(255.0f * alphaFloat);

        StretchBlt(hScaledDC, 0, 0, newW, newH,
                   hOrigDC, 0, 0, data->width, data->height, SRCCOPY);

        POINT ptDst = { currentX, currentY };
        SIZE  sz    = { newW, newH };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha;
        bf.AlphaFormat = 0;
        UpdateLayeredWindow(hGhost, NULL, &ptDst, &sz, hScaledDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (firstFrame) {
            DwmFlush();
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
            if (data->hReady) SetEvent(data->hReady);
        }

        if (lastFrame) break;
        DwmFlush();
    }

    FinalizeRealWindow(data);

    SelectObject(hScaledDC, hOldScaled);
    SelectObject(hOrigDC, hOldOrig);
    DeleteObject(hScaledBitmap);
    DeleteDC(hScaledDC);
    DeleteDC(hOrigDC);
    ReleaseDC(NULL, hScreenDC);
}

static bool RunGpuAnim(GhostAnimData* data, HWND hGhost,
                       int screenX, int screenY, int screenWidth, int screenHeight, float taskbarY) {
    const int w = data->width;
    const int h = data->height;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    const size_t stride = (size_t)w * 4;
    BYTE* bits = (BYTE*)malloc(stride * (size_t)h);
    if (!bits) return false;

    HDC hScreenDC = GetDC(NULL);
    int scan = GetDIBits(hScreenDC, data->hBitmap, 0, h, bits, &bmi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hScreenDC);
    if (scan == 0) { free(bits); return false; }
    for (size_t i = 0; i < (size_t)w * (size_t)h; i++) bits[i * 4 + 3] = 0xFF;

    IDXGISwapChain1*             swapChain   = nullptr;
    IDXGIDevice*                 dxgiDev     = nullptr;
    IDCompositionDesktopDevice*  dcompDevice = nullptr;
    IDCompositionTarget*         dcompTarget = nullptr;
    IDCompositionVisual2*        visual      = nullptr;
    IDCompositionEffectGroup*    effectGroup = nullptr;
    IDCompositionMatrixTransform* xform      = nullptr;
    bool ok = false;

    {
        std::lock_guard<std::mutex> lock(g_gpuCtxMutex);

        D3D11_TEXTURE2D_DESC td;
        ZeroMemory(&td, sizeof(td));
        td.Width  = w;
        td.Height = h;
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_IMMUTABLE;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA srd;
        ZeroMemory(&srd, sizeof(srd));
        srd.pSysMem = bits;
        srd.SysMemPitch = (UINT)stride;

        ID3D11Texture2D* srcTex = nullptr;
        HRESULT hr = g_d3dDevice->CreateTexture2D(&td, &srd, &srcTex);
        if (SUCCEEDED(hr) && srcTex) {
            DXGI_SWAP_CHAIN_DESC1 scd;
            ZeroMemory(&scd, sizeof(scd));
            scd.Width  = w;
            scd.Height = h;
            scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            scd.SampleDesc.Count = 1;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.BufferCount = 2;
            scd.Scaling    = DXGI_SCALING_STRETCH;
            scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            scd.AlphaMode  = DXGI_ALPHA_MODE_PREMULTIPLIED;

            hr = g_dxgiFactory->CreateSwapChainForComposition(g_d3dDevice, &scd, nullptr, &swapChain);
            if (SUCCEEDED(hr) && swapChain) {
                ID3D11Texture2D* backBuf = nullptr;
                if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuf)) && backBuf) {
                    ID3D11DeviceContext* ctx = nullptr;
                    g_d3dDevice->GetImmediateContext(&ctx);
                    if (ctx) {
                        ctx->CopyResource(backBuf, srcTex);
                        ctx->Release();
                    }
                    backBuf->Release();
                }
                DXGI_PRESENT_PARAMETERS pp;
                ZeroMemory(&pp, sizeof(pp));
                swapChain->Present1(0, 0, &pp);
            }
        }
        SafeRelease(srcTex);

        if (swapChain &&
            SUCCEEDED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev)) &&
            SUCCEEDED(DCompositionCreateDevice2(dxgiDev, __uuidof(IDCompositionDesktopDevice), (void**)&dcompDevice)) &&
            SUCCEEDED(dcompDevice->CreateTargetForHwnd(hGhost, TRUE, &dcompTarget)) &&
            SUCCEEDED(dcompDevice->CreateVisual(&visual)) &&
            SUCCEEDED(dcompDevice->CreateMatrixTransform(&xform)) &&
            SUCCEEDED(dcompDevice->CreateEffectGroup(&effectGroup))) {
            visual->SetContent(swapChain);
            visual->SetTransform(xform);
            visual->SetEffect(effectGroup);
            dcompTarget->SetRoot(visual);
            ok = true;
        }
        SafeRelease(dxgiDev);
    }

    free(bits);

    if (!ok) {
        SafeRelease(xform);
        SafeRelease(effectGroup);
        SafeRelease(visual);
        SafeRelease(dcompTarget);
        SafeRelease(dcompDevice);
        SafeRelease(swapChain);
        return false;
    }

    dcompDevice->Commit();
    DwmFlush();

    const int capW = (int)(w * 1.30f) + 4;
    const int capH = (int)(h * 1.30f) + 4;
    const double totalMs = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float t = data->isRising ? (1.0f - progress) : progress;

        int newW, newH, curX, curY;
        float alphaFloat;
        SolveFrame(data, t, screenWidth, screenHeight, taskbarY, capW, capH,
                   &newW, &newH, &curX, &curY, &alphaFloat);

        float sx = (w > 0) ? (float)newW / (float)w : 1.0f;
        float sy = (h > 0) ? (float)newH / (float)h : 1.0f;
        D2D_MATRIX_3X2_F m;
        m._11 = sx;          m._12 = 0.0f;
        m._21 = 0.0f;        m._22 = sy;
        m._31 = (float)(curX - screenX); m._32 = (float)(curY - screenY);

        xform->SetMatrix(m);
        effectGroup->SetOpacity(clampf(alphaFloat, 0.0f, 1.0f));
        dcompDevice->Commit();

        if (firstFrame) {
            DwmFlush();
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
            if (data->hReady) SetEvent(data->hReady);
        }

        if (lastFrame) break;
        DwmFlush();
    }

    FinalizeRealWindow(data);

    SafeRelease(xform);
    SafeRelease(effectGroup);
    SafeRelease(visual);
    SafeRelease(dcompTarget);
    SafeRelease(dcompDevice);
    SafeRelease(swapChain);
    return true;
}

static bool RunGpuGenieAnim(GhostAnimData* data, HWND hGhost,
                            int screenX, int screenY, int screenWidth, int screenHeight, float taskbarY) {
    if (!EnsureGenieGpuResources()) return false;

    const int w = data->width;
    const int h = data->height;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    const size_t stride = (size_t)w * 4;
    BYTE* bits = (BYTE*)malloc(stride * (size_t)h);
    if (!bits) return false;
    HDC hScreenDC = GetDC(NULL);
    int scan = GetDIBits(hScreenDC, data->hBitmap, 0, h, bits, &bmi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hScreenDC);
    if (scan == 0) { free(bits); return false; }
    for (size_t i = 0; i < (size_t)w * (size_t)h; i++) bits[i * 4 + 3] = 0xFF;

    const int vertCount = (GENIE_GX + 1) * (GENIE_GY + 1);
    const UINT vbBytes  = (UINT)(vertCount * sizeof(GenieVertex));

    ID3D11Texture2D*             srcTex      = nullptr;
    ID3D11ShaderResourceView*    srv         = nullptr;
    IDXGISwapChain1*             swapChain   = nullptr;
    ID3D11RenderTargetView*      rtv         = nullptr;
    ID3D11Buffer*                vbuf        = nullptr;
    ID3D11DeviceContext*         ctx         = nullptr;
    IDXGIDevice*                 dxgiDev     = nullptr;
    IDCompositionDesktopDevice*  dcompDevice = nullptr;
    IDCompositionTarget*         dcompTarget = nullptr;
    IDCompositionVisual2*        visual      = nullptr;
    bool ok = false;

    {
        std::lock_guard<std::mutex> lock(g_gpuCtxMutex);

        D3D11_TEXTURE2D_DESC td;
        ZeroMemory(&td, sizeof(td));
        td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_IMMUTABLE;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA srd;
        ZeroMemory(&srd, sizeof(srd));
        srd.pSysMem = bits;
        srd.SysMemPitch = (UINT)stride;

        if (SUCCEEDED(g_d3dDevice->CreateTexture2D(&td, &srd, &srcTex)) &&
            SUCCEEDED(g_d3dDevice->CreateShaderResourceView(srcTex, nullptr, &srv))) {

            DXGI_SWAP_CHAIN_DESC1 scd;
            ZeroMemory(&scd, sizeof(scd));
            scd.Width  = screenWidth;
            scd.Height = screenHeight;
            scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            scd.SampleDesc.Count = 1;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.BufferCount = 2;
            scd.Scaling    = DXGI_SCALING_STRETCH;
            scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            scd.AlphaMode  = DXGI_ALPHA_MODE_PREMULTIPLIED;

            if (SUCCEEDED(g_dxgiFactory->CreateSwapChainForComposition(g_d3dDevice, &scd, nullptr, &swapChain))) {
                ID3D11Texture2D* backBuf = nullptr;
                if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuf)) && backBuf) {
                    g_d3dDevice->CreateRenderTargetView(backBuf, nullptr, &rtv);
                    backBuf->Release();
                }
            }

            D3D11_BUFFER_DESC vbd;
            ZeroMemory(&vbd, sizeof(vbd));
            vbd.ByteWidth = vbBytes;
            vbd.Usage = D3D11_USAGE_DYNAMIC;
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            g_d3dDevice->CreateBuffer(&vbd, nullptr, &vbuf);
        }

        if (rtv && vbuf && srv &&
            SUCCEEDED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev)) &&
            SUCCEEDED(DCompositionCreateDevice2(dxgiDev, __uuidof(IDCompositionDesktopDevice), (void**)&dcompDevice)) &&
            SUCCEEDED(dcompDevice->CreateTargetForHwnd(hGhost, TRUE, &dcompTarget)) &&
            SUCCEEDED(dcompDevice->CreateVisual(&visual))) {
            g_d3dDevice->GetImmediateContext(&ctx);
            visual->SetContent(swapChain);
            dcompTarget->SetRoot(visual);
            dcompDevice->Commit();
            DwmFlush();
            ok = (ctx != nullptr);
        }
        SafeRelease(dxgiDev);
    }

    free(bits);

    GenieVertex* verts = ok ? (GenieVertex*)malloc(vbBytes) : nullptr;
    if (!ok || !verts) {
        if (verts) free(verts);
        SafeRelease(ctx);
        SafeRelease(visual);
        SafeRelease(dcompTarget);
        SafeRelease(dcompDevice);
        SafeRelease(vbuf);
        SafeRelease(rtv);
        SafeRelease(swapChain);
        SafeRelease(srv);
        SafeRelease(srcTex);
        return false;
    }

    const float LEAD     = 1.4f;
    const float neckW    = (data->iconButtonWidth > 0) ? ((float)data->iconButtonWidth > 10.0f ? (float)data->iconButtonWidth : 10.0f) : (w * 0.05f > 10.0f ? w * 0.05f : 10.0f);
    const float sourceCX = data->targetRect.left + w * 0.5f;
    const float dockX    = (float)data->targetDockX;
    const float dockY    = taskbarY;
    const float srcTop   = (float)data->targetRect.top;

    const double totalMs = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);
        float p = data->isRising ? (1.0f - progress) : progress;

        int vi = 0;
        for (int j = 0; j <= GENIE_GY; j++) {
            float v    = (float)j / (float)GENIE_GY;
            float rowP = clampf(p * (1.0f + LEAD) - (1.0f - v) * LEAD, 0.0f, 1.0f);
            float pinch   = rowP * rowP * (3.0f - 2.0f * rowP);
            float descend = rowP * rowP * rowP;
            float rowW  = w + (neckW - w) * pinch;
            float rowCX = sourceCX + (dockX - sourceCX) * pinch;
            float rowY  = (srcTop + v * h) + (dockY - (srcTop + v * h)) * descend;
            for (int i = 0; i <= GENIE_GX; i++) {
                float u = (float)i / (float)GENIE_GX;
                float xpx = rowCX + (u - 0.5f) * rowW;
                verts[vi].x = (xpx - (float)screenX) / (float)screenWidth * 2.0f - 1.0f;
                verts[vi].y = 1.0f - (rowY - (float)screenY) / (float)screenHeight * 2.0f;
                verts[vi].u = u;
                verts[vi].v = v;
                vi++;
            }
        }
        float alpha = (p < 0.75f) ? 1.0f : clampf(1.0f - (p - 0.75f) / 0.25f, 0.0f, 1.0f);

        {
            std::lock_guard<std::mutex> lock(g_gpuCtxMutex);

            D3D11_MAPPED_SUBRESOURCE ms;
            if (SUCCEEDED(ctx->Map(vbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                memcpy(ms.pData, verts, vbBytes);
                ctx->Unmap(vbuf, 0);
            }
            if (SUCCEEDED(ctx->Map(g_gConstBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                float c[4] = { alpha, 0.0f, 0.0f, 0.0f };
                memcpy(ms.pData, c, sizeof(c));
                ctx->Unmap(g_gConstBuf, 0);
            }

            UINT strideV = sizeof(GenieVertex), offsetV = 0;
            ctx->IASetInputLayout(g_gLayout);
            ctx->IASetVertexBuffers(0, 1, &vbuf, &strideV, &offsetV);
            ctx->IASetIndexBuffer(g_gIndexBuf, DXGI_FORMAT_R32_UINT, 0);
            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            ctx->VSSetShader(g_gVS, nullptr, 0);
            ctx->PSSetShader(g_gPS, nullptr, 0);
            ctx->PSSetShaderResources(0, 1, &srv);
            ctx->PSSetSamplers(0, 1, &g_gSampler);
            ctx->PSSetConstantBuffers(0, 1, &g_gConstBuf);
            ctx->RSSetState(g_gRaster);
            D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight, 0.0f, 1.0f };
            ctx->RSSetViewports(1, &vp);
            float clearCol[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            ctx->OMSetRenderTargets(1, &rtv, nullptr);
            ctx->ClearRenderTargetView(rtv, clearCol);
            ctx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
            ctx->DrawIndexed(g_gIndexCount, 0, 0);

            DXGI_PRESENT_PARAMETERS pp;
            ZeroMemory(&pp, sizeof(pp));
            swapChain->Present1(0, 0, &pp);
        }
        if (firstFrame) {
            DwmFlush();
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
            firstFrame = FALSE;
            if (data->hReady) SetEvent(data->hReady);
        }

        if (lastFrame) break;
        DwmFlush();
    }

    FinalizeRealWindow(data);

    free(verts);
    SafeRelease(ctx);
    SafeRelease(visual);
    SafeRelease(dcompTarget);
    SafeRelease(dcompDevice);
    SafeRelease(vbuf);
    SafeRelease(rtv);
    SafeRelease(swapChain);
    SafeRelease(srv);
    SafeRelease(srcTex);
    return true;
}

// --- Persistent taskbar icon cache (by HWND and by process+monitor) ---
struct TbIconPos {
    int x, y, width;
};
static std::unordered_map<HWND, TbIconPos> g_TaskbarIconCache;
static std::unordered_map<std::wstring, TbIconPos> g_ProcessIconCache;
static std::mutex g_TbCacheMutex;

// Guard: only one animation at a time per window.
static std::unordered_set<HWND> g_animInProgress;
static std::mutex g_animMtx;

static bool AnimIsInProgress(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_animMtx);
    return g_animInProgress.count(hWnd) != 0;
}
static void AnimSetInProgress(HWND hWnd, bool set) {
    std::lock_guard<std::mutex> lock(g_animMtx);
    if (set) g_animInProgress.insert(hWnd);
    else     g_animInProgress.erase(hWnd);
}

static HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", NULL);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) return hMainTray;
    HWND hSecTray = NULL;
    while ((hSecTray = FindWindowExW(NULL, hSecTray, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon)
            return hSecTray;
    }
    return hMainTray;
}

// Resolve the taskbar icon centre (X) and the actual taskbar top edge (Y) for
// hWnd.  Uses UI Automation on both Win10 and Win11, falling back to the
// ToolbarWindow32 / TB_GETBUTTON path when UIA is unavailable.  Returns FALSE
// only when no taskbar or button can be found.
static BOOL GetTaskbarIconCenter(HWND hWnd, int* outX, int* outY, int* outWidth) {
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // Quick cache hit by HWND.
    {
        std::lock_guard<std::mutex> lock(g_TbCacheMutex);
        auto it = g_TaskbarIconCache.find(hWnd);
        if (it != g_TaskbarIconCache.end()) {
            *outX = it->second.x;
            *outY = it->second.y;
            if (outWidth) *outWidth = it->second.width;
            return TRUE;
        }
    }

    // Build a per-process+monitor key for grouped-button cache.
    std::wstring procName;
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hProc) {
                WCHAR path[MAX_PATH];
                DWORD len = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, path, &len)) {
                    WCHAR* name = wcsrchr(path, L'\\');
                    if (name) procName = name + 1;
                }
                CloseHandle(hProc);
            }
        }
    }
    std::wstring processKey = procName + L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
    {
        std::lock_guard<std::mutex> lock(g_TbCacheMutex);
        auto it = g_ProcessIconCache.find(processKey);
        if (it != g_ProcessIconCache.end()) {
            *outX = it->second.x;
            *outY = it->second.y;
            if (outWidth) *outWidth = it->second.width;
            g_TaskbarIconCache[hWnd] = it->second;
            return TRUE;
        }
    }

    int targetX = 0;
    int targetY = 0;
    int targetW = 0;
    bool found = false;

    // --- Primary method: UI Automation (Win10 + Win11) ---
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);
    IUIAutomation* pAutomation = NULL;
    HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER,
                                     __uuidof(IUIAutomation), (void**)&pAutomation);
    if (FAILED(hrUia))
        hrUia = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                                 __uuidof(IUIAutomation), (void**)&pAutomation);
    if (SUCCEEDED(hrUia) && pAutomation) {
        HWND hTray = FindTaskbarForMonitor(hMon);
        if (hTray) {
            IUIAutomationElement* pTrayElement = NULL;
            if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTrayElement)) && pTrayElement) {
                WCHAR titleW[512] = {0};
                GetWindowTextW(hWnd, titleW, 512);
                std::wstring titleLower = titleW;
                for (auto& c : titleLower) c = (WCHAR)towlower(c);
                std::wstring procLower = procName;
                for (auto& c : procLower) c = (WCHAR)towlower(c);
                size_t dot = procLower.find(L'.');
                if (dot != std::wstring::npos) procLower = procLower.substr(0, dot);

                IUIAutomationCondition* pButtonCond = NULL;
                IUIAutomationCondition* pListItemCond = NULL;
                IUIAutomationCondition* pOrCond = NULL;
                VARIANT varBtn; varBtn.vt = VT_I4; varBtn.lVal = UIA_ButtonControlTypeId;
                pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &pButtonCond);
                VARIANT varList; varList.vt = VT_I4; varList.lVal = UIA_ListItemControlTypeId;
                pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pListItemCond);
                if (pButtonCond && pListItemCond)
                    pAutomation->CreateOrCondition(pButtonCond, pListItemCond, &pOrCond);

                IUIAutomationElementArray* pArray = NULL;
                if (pOrCond && SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants, pOrCond, &pArray)) && pArray) {
                    int length = 0;
                    pArray->get_Length(&length);
                    MONITORINFO mi = {};
                    mi.cbSize = sizeof(mi);
                    GetMonitorInfoW(hMon, &mi);
                    int monRight = mi.rcMonitor.right;
                    int bestScore = 0;
                    for (int i = 0; i < length; i++) {
                        IUIAutomationElement* pItem = NULL;
                        if (SUCCEEDED(pArray->GetElement(i, &pItem)) && pItem) {
                            BSTR name;
                            if (SUCCEEDED(pItem->get_CurrentName(&name)) && name) {
                                std::wstring uiaName = name;
                                for (auto& c : uiaName) c = (WCHAR)towlower(c);
                                if (!uiaName.empty()) {
                                    int score = 0;
                                    if (titleLower == uiaName) score += 1000;
                                    else {
                                        if (!titleLower.empty() && titleLower.find(uiaName) != std::wstring::npos)
                                            score += 500;
                                        if (!uiaName.empty() && uiaName.find(titleLower) != std::wstring::npos)
                                            score += 500;
                                    }
                                    if (!procLower.empty() && uiaName.find(procLower) != std::wstring::npos)
                                        score += 400;
                                    if (uiaName.find(L"start") != std::wstring::npos)    score -= 500;
                                    if (uiaName.find(L"search") != std::wstring::npos)   score -= 500;
                                    if (uiaName.find(L"task view") != std::wstring::npos) score -= 500;
                                    if (score > bestScore) {
                                        RECT bRect;
                                        if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&bRect)) &&
                                            bRect.right > bRect.left && bRect.left < monRight - 50) {
                                            bestScore = score;
                                            targetX = bRect.left + (bRect.right - bRect.left) / 2;
                                            targetY = bRect.top;
                                            targetW = bRect.right - bRect.left;
                                            found   = true;
                                        }
                                    }
                                }
                                SysFreeString(name);
                            }
                            pItem->Release();
                        }
                    }
                    pArray->Release();
                }
                if (pButtonCond)    pButtonCond->Release();
                if (pListItemCond)  pListItemCond->Release();
                if (pOrCond)        pOrCond->Release();
                pTrayElement->Release();
            }
        }
        pAutomation->Release();
    }
    if (coInit) CoUninitialize();

    // --- Fallback: ToolbarWindow32 / TB_GETBUTTON (Win10) ---
    if (!found) {
        for (int pass = 0; pass < 2 && !found; pass++) {
            HWND hTaskbar = FindWindowW(pass == 0 ? L"Shell_TrayWnd" : L"Shell_SecondaryTrayWnd", NULL);
            if (!hTaskbar) continue;
            RECT tbRect;
            GetWindowRect(hTaskbar, &tbRect);
            if (MonitorFromRect(&tbRect, MONITOR_DEFAULTTONULL) != hMon) continue;

            HWND hToolbar = NULL;
            HWND hRebar = FindWindowEx(hTaskbar, NULL, L"ReBarWindow32", NULL);
            if (hRebar) {
                HWND hMSTask = FindWindowEx(hRebar, NULL, L"MSTaskSwWClass", NULL);
                if (hMSTask)
                    hToolbar = FindWindowEx(hMSTask, NULL, L"ToolbarWindow32", NULL);
            }
            if (!hToolbar) {
                HWND hWorkerW = FindWindowEx(hTaskbar, NULL, L"WorkerW", NULL);
                if (hWorkerW) {
                    HWND hReBarW = FindWindowEx(hWorkerW, NULL, L"ReBarWindow32", NULL);
                    if (hReBarW) {
                        HWND hMSTask = FindWindowEx(hReBarW, NULL, L"MSTaskSwWClass", NULL);
                        if (hMSTask)
                            hToolbar = FindWindowEx(hMSTask, NULL, L"ToolbarWindow32", NULL);
                    }
                }
            }
            if (!hToolbar)
                hToolbar = FindWindowEx(hTaskbar, NULL, L"ToolbarWindow32", NULL);

            if (hToolbar) {
                struct { HWND hWnd; int x; int y; int w; } tbCache[64];
                int tbCount = 0;
                int btnCount = (int)SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
                for (int i = 0; i < btnCount && tbCount < 64; i++) {
                    TBBUTTON btn;
                    ZeroMemory(&btn, sizeof(btn));
                    if (SendMessage(hToolbar, TB_GETBUTTON, i, (LPARAM)&btn)) {
                        RECT r;
                        if (SendMessage(hToolbar, TB_GETRECT, btn.idCommand, (LPARAM)&r)) {
                            MapWindowPoints(hToolbar, NULL, (POINT*)&r, 2);
                            tbCache[tbCount].hWnd = (HWND)btn.dwData;
                            tbCache[tbCount].x = (r.left + r.right) / 2;
                            tbCache[tbCount].y = r.top; // top edge of the button
                            tbCache[tbCount].w = r.right - r.left;
                            tbCount++;
                        }
                    }
                }
                auto tryMatch = [&](HWND h) -> bool {
                    for (int i = 0; i < tbCount; i++)
                        if (tbCache[i].hWnd == h) {
                            targetX = tbCache[i].x;
                            targetY = tbCache[i].y;
                            targetW = tbCache[i].w;
                            return true;
                        }
                    return false;
                };
                if (tryMatch(hWnd)) found = true;
                else for (HWND hCur = GetWindow(hWnd, GW_OWNER); hCur && !found; hCur = GetWindow(hCur, GW_OWNER))
                    found = tryMatch(hCur);
                if (!found) { HWND hRoot = GetAncestor(hWnd, GA_ROOT); if (hRoot != hWnd) found = tryMatch(hRoot); }
                if (!found) {
                    DWORD targetPid;
                    GetWindowThreadProcessId(hWnd, &targetPid);
                    if (targetPid) {
                        int bestIdx = -1, bestDist = INT_MAX;
                        RECT wr; GetWindowRect(hWnd, &wr);
                        int cx = (wr.left + wr.right) / 2;
                        for (int i = 0; i < tbCount; i++) {
                            DWORD pid;
                            GetWindowThreadProcessId(tbCache[i].hWnd, &pid);
                            if (pid == targetPid) {
                                int d = abs(tbCache[i].x - cx);
                                if (d < bestDist) { bestDist = d; bestIdx = i; }
                            }
                        }
                        if (bestIdx >= 0) {
                            targetX = tbCache[bestIdx].x;
                            targetY = tbCache[bestIdx].y;
                            targetW = tbCache[bestIdx].w;
                            found   = true;
                        }
                    }
                }
            }
        }
    }

    if (!found) return FALSE;

    // Cache the result.
    {
        std::lock_guard<std::mutex> lock(g_TbCacheMutex);
        TbIconPos pos = { targetX, targetY, targetW };
        g_TaskbarIconCache[hWnd] = pos;
        if (!processKey.empty()) g_ProcessIconCache[processKey] = pos;
    }
    *outX = targetX;
    *outY = targetY;
    if (outWidth) *outWidth = targetW;
    return TRUE;
}

DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    int screenX      = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY      = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HMONITOR hMon = MonitorFromWindow(data->hRealWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);
    float taskbarY = (float)mi.rcWork.bottom;

    // Lock the animation target to the taskbar icon centre (X) and the top
    // edge of the icon (Y).  When the icon cannot be found, the caller's
    // monitor-centre default (from StartGenieAnim) is kept for X, and
    // rcWork.bottom is used for Y.
    int iconX, iconY;
    data->iconButtonWidth = 0;
    if (GetTaskbarIconCenter(data->hRealWnd, &iconX, &iconY, &data->iconButtonWidth)) {
        data->targetDockX = iconX;
        taskbarY = (float)iconY;
    }

    bool useGpu = EnsureGpuDevice();
    HWND hGhost = NULL;

    if (useGpu) {
        hGhost = CreateWindowExW(
            WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
            L"STATIC", NULL, WS_POPUP,
            screenX, screenY, screenWidth, screenHeight, NULL, NULL, NULL, NULL);

        if (hGhost) {
            SetClassLongPtr(hGhost, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(NULL_BRUSH));
        }

        DisableGhostShadow(hGhost);

        bool ran = false;
        if (hGhost) {
            ran = (data->mode == MODE_GENIE)
                    ? RunGpuGenieAnim(data, hGhost, screenX, screenY, screenWidth, screenHeight, taskbarY)
                    : RunGpuAnim(data, hGhost, screenX, screenY, screenWidth, screenHeight, taskbarY);
        }
        if (!ran) {
            useGpu = false;
            if (hGhost) { DestroyWindow(hGhost); hGhost = NULL; }
        }
    }

    if (!useGpu) {
        hGhost = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
            L"STATIC", NULL, WS_POPUP,
            data->targetRect.left, data->targetRect.top, data->width, data->height,
            NULL, NULL, NULL, NULL);

        if (hGhost) {
            SetClassLongPtr(hGhost, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(NULL_BRUSH));
        }

        DisableGhostShadow(hGhost);

        RunCpuAnim(data, hGhost, screenWidth, screenHeight, taskbarY);
    }

    if (hGhost) DestroyWindow(hGhost);
    DeleteObject(data->hBitmap);
    if (data->hReady) CloseHandle(data->hReady);
    AnimSetInProgress(data->hRealWnd, false);
    delete data;
    return 0;
}

void StartGenieAnim(HWND hWnd, BOOL rising) {
    if (AnimIsInProgress(hWnd)) return;
    AnimSetInProgress(hWnd, true);

    RECT rect;
    GetWindowRect(hWnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (w <= 0 || h <= 0) return;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);

    GhostAnimData* data = new GhostAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->wasCloaked = rising;
    // Default dock X is the monitor centre; the animation thread will
    // overwrite this with the actual taskbar-icon centre via
    // GetTaskbarIconCenter.
    data->targetDockX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
    data->mode = g_mode.load(std::memory_order_relaxed);
    data->durationMs = g_durations[data->mode].load(std::memory_order_relaxed);
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    data->hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, data->hBitmap);

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp);
                DeleteDC(hCacheDC);

                DeleteObject(g_SnapshotCache[hWnd]);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            // The window may be cloaked (the restore hook cloaks before
            // calling us).  Temporarily uncloak so PrintWindow captures real
            // content instead of black.
            BOOL wasCloaked = FALSE;
            DwmGetWindowAttribute(hWnd, DWMWA_CLOAK, &wasCloaked, sizeof(wasCloaked));
            if (wasCloaked) {
                BOOL uncloak = FALSE;
                DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &uncloak, sizeof(uncloak));
                DwmFlush();
            }
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
            if (wasCloaked) {
                BOOL recloak = TRUE;
                DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &recloak, sizeof(recloak));
                DwmFlush();
            }
        }
    } else {
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, rect.left, rect.top, SRCCOPY);

        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd]);
        }
        g_SnapshotCache[hWnd] = CreateCompatibleBitmap(hScreenDC, w, h);
        HDC hCacheDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
        BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hCacheDC, hOldCacheBmp);
        DeleteDC(hCacheDC);
    }

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    HANDLE hReady = CreateEventW(NULL, TRUE, FALSE, NULL);
    data->hReady = hReady;
    HANDLE hThread = CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
    if (!hThread) {
        if (hReady) CloseHandle(hReady);
        DeleteObject(data->hBitmap);
        delete data;
        return;
    }
    CloseHandle(hThread);
    if (hReady) WaitForSingleObject(hReady, 40);
}

// Per-window re-entrancy guard so that when DefWindowProcW_Hook handles
// SC_MINIMIZE/SC_RESTORE (which internally calls ShowWindow), the nested
// call through ShowWindow_Hook does not start a second animation.
static std::unordered_map<HWND, DWORD> g_inSysCmd;
static std::mutex                      g_sysCmdMtx;

static bool IsInSysCmdOnThisThread(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_sysCmdMtx);
    auto it = g_inSysCmd.find(hWnd);
    return it != g_inSysCmd.end() && it->second == GetCurrentThreadId();
}

static void SetSysCmdGuard(HWND hWnd, bool set) {
    std::lock_guard<std::mutex> lock(g_sysCmdMtx);
    if (set)
        g_inSysCmd[hWnd] = GetCurrentThreadId();
    else
        g_inSysCmd.erase(hWnd);
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_enabled.load(std::memory_order_relaxed)) {
        if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
            if (!IsInSysCmdOnThisThread(hWnd) && IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
                SetDwmTransitions(hWnd, FALSE);
                StartGenieAnim(hWnd, FALSE);
            }
            return ShowWindow_Original(hWnd, nCmdShow);
        }
        else if (nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) {
            if (IsIconic(hWnd) && !IsInSysCmdOnThisThread(hWnd)) {
                SetDwmTransitions(hWnd, FALSE);
                BOOL cloaked = TRUE;
                DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloaked, sizeof(cloaked));
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                BOOL res = ShowWindow_Original(hWnd, nCmdShow);
                StartGenieAnim(hWnd, TRUE);
                return res;
            }
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        {
            std::lock_guard<std::mutex> lock(g_sysCmdMtx);
            g_inSysCmd.erase(hWnd);
        }
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                DeleteObject(g_SnapshotCache[hWnd]);
                g_SnapshotCache.erase(hWnd);
            }
        }
    }

    if (g_enabled.load(std::memory_order_relaxed) && Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            SetSysCmdGuard(hWnd, true);
            SetDwmTransitions(hWnd, FALSE);
            StartGenieAnim(hWnd, FALSE);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            SetSysCmdGuard(hWnd, false);
            return res;
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd)) {
            SetSysCmdGuard(hWnd, true);
            SetDwmTransitions(hWnd, FALSE);
            BOOL cloaked = TRUE;
            DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloaked, sizeof(cloaked));
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            StartGenieAnim(hWnd, TRUE);
            SetSysCmdGuard(hWnd, false);
            return res;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    // Eagerly initialize GPU resources so the first animation doesn't have to
    // wait for D3D device creation and shader compilation.
    if (EnsureGpuDevice()) {
        EnsureGenieGpuResources();
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        for (auto& pair : g_SnapshotCache) {
            DeleteObject(pair.second);
        }
        g_SnapshotCache.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_TbCacheMutex);
        g_TaskbarIconCache.clear();
        g_ProcessIconCache.clear();
    }
    ReleaseGenieGpuResources();
    ReleaseGpuDevice();
}

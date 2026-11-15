// ==WindhawkMod==
// @id              fluid-wallpaper
// @name            Fluid Wallpaper
// @description     Animated GPU fluid simulation wallpaper with autonomous motion, mouse interaction and per-monitor rendering
// @version         1.0.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ldwmapi -lwtsapi32 -lgdi32 -luser32 -lshell32 -luuid
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Fluid Wallpaper
![Screenshot](https://i.imgur.com/rIzdlWv.jpeg)
[Watch the overview video in full quality](https://i.imgur.com/yXW45Vs.mp4)

An animated, interactive fluid simulation rendered on the GPU behind your
desktop icons. The shaders are a Direct3D port of
[WebGL Fluid Simulation](https://github.com/PavelDoGreat/WebGL-Fluid-Simulation)
by Pavel Dobryakov (MIT license).

## Features

- One independent simulation per monitor, drawn behind the desktop icons.
- Autonomous color trails and periodic color bursts keep the fluid moving.
- Moving the mouse over the desktop stirs the fluid. No clicks are needed and
  no mouse hooks are installed; clicks always reach the desktop.
- Bloom, sun rays and shading effects, all configurable.
- Pauses automatically when the desktop can't be seen: when a monitor's
  desktop is fully covered by windows (maximized, full-screen or several
  windows side by side), when the session is locked or switched away, and when
  the display is turned off.
- Optional lower frame rate or pause on battery power and Energy Saver.

To see the wallpaper, minimize your windows. Settings are applied live;
changing a setting restarts the simulation.

## How it works

Each monitor gets a child window of Explorer's wallpaper host, without a
redirection surface. The simulation runs in Direct3D 11 and its final pass is
drawn straight into a DirectComposition swap chain attached to that window.
Frames never leave the GPU. The same path is used on the classic desktop
layout and on the Windows 11 24H2+ layout.

The mod doesn't change your wallpaper settings, the registry or the order of
Explorer's own windows. Disabling it removes its windows and reveals your
normal wallpaper.

## Performance tips

- Lighter: Maximum FPS 30, Dye resolution 512, Trails 2, Bloom off.
- Swirlier: Vorticity 50, Dye dissipation 60, Trail force 4500.

## Troubleshooting

If the wallpaper doesn't appear, open the mod log in Windhawk: the lines
starting with "Desktop layout", "Direct3D" and "Monitor" describe what was
found and created. Please include them when reporting a problem, together with
your Windows build number.

If the animation stays frozen while the desktop is visible, a window that
looks hidden is probably reported as visible and covering it. Disable **Pause
when the desktop is covered** in that case.

## Requirements and limitations

- Windows 10 or 11 and a GPU with Direct3D feature level 10.0 or later.
- Relies on the undocumented WorkerW desktop layout of Windows, which differs
  between Windows 10, Windows 11 and Windows 11 24H2 and later. Future Windows
  builds may change it.
- Below 30 FPS (including the battery option) the motion also slows down,
  because each simulation step is limited in length to stay stable.
- The transparent background and screenshot features of the web demo are not
  included.
- Some settings use integer scales; for example, 80 means 0.8.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - fps: 30
    $name: Maximum FPS
    $description: "15-120. Higher values look smoother but use more GPU."
  - pause: false
    $name: Pause simulation
  - pauseWhenCovered: true
    $name: Pause when the desktop is covered
    $description: "Pause a monitor while its desktop is hidden by windows: maximized, full-screen, or several windows side by side."
  - onBattery: halfFps
    $name: On battery or Energy Saver
    $options:
    - normal: Run normally
    - halfFps: Half frame rate
    - pause: Pause
  - primaryOnly: false
    $name: Primary monitor only
  $name: General
- mouse:
  - enabled: true
    $name: Mouse interaction
    $description: "Moving the mouse stirs the fluid. No clicks needed."
  - desktopOnly: true
    $name: Only over the desktop
    $description: "Disable to also react to mouse movement over other windows."
  - force: 6000
    $name: Force
    $description: "100-15000"
  $name: Mouse
- simulation:
  - simResolution: 128
    $name: Simulation resolution
    $description: "64-512"
  - dyeResolution: 1024
    $name: Dye resolution
    $description: "256-2048. Lower values reduce GPU memory and load."
  - densityDissipation: 100
    $name: Dye dissipation ×100
    $description: "0-1000. 100 means 1.0. Higher values make colors fade faster."
  - velocityDissipation: 20
    $name: Velocity dissipation ×100
    $description: "0-1000. 20 means 0.2."
  - pressure: 80
    $name: Pressure ×100
    $description: "0-100"
  - pressureIterations: 20
    $name: Pressure iterations
    $description: "5-60"
  - curl: 30
    $name: Vorticity
    $description: "0-100. Higher values create more swirls."
  - splatRadius: 25
    $name: Splat radius ×100
    $description: "1-200. 25 means 0.25, the default of the web demo."
  $name: Simulation
- colors:
  - colorful: true
    $name: Changing colors
    $description: "Cycle through hues. When disabled, the fixed color below is used."
  - colorSpeed: 10
    $name: Color change speed
    $description: "0-100"
  - fixedRed: 30
    $name: "Fixed color: red"
    $description: "0-255"
  - fixedGreen: 160
    $name: "Fixed color: green"
    $description: "0-255"
  - fixedBlue: 255
    $name: "Fixed color: blue"
    $description: "0-255"
  - backgroundRed: 0
    $name: "Background: red"
    $description: "0-255"
  - backgroundGreen: 0
    $name: "Background: green"
    $description: "0-255"
  - backgroundBlue: 0
    $name: "Background: blue"
    $description: "0-255"
  - shading: true
    $name: Shading
  $name: Colors
- autoMotion:
  - enabled: true
    $name: Autonomous motion
  - trails: 3
    $name: Trails
    $description: "1-6"
  - speed: 35
    $name: Speed
    $description: "1-100"
  - force: 3000
    $name: Trail force
    $description: "100-15000"
  - colorAmount: 55
    $name: Color amount ×100
    $description: "1-300"
  - bursts: true
    $name: Color bursts
  - burstSeconds: 12
    $name: Seconds between bursts
    $description: "2-120"
  $name: Autonomous motion
- bloom:
  - enabled: true
    $name: Enabled
  - resolution: 256
    $name: Resolution
    $description: "64-512"
  - levels: 8
    $name: Levels
    $description: "2-8"
  - intensity: 80
    $name: Intensity ×100
    $description: "0-300"
  - threshold: 60
    $name: Threshold ×100
    $description: "0-300"
  - softKnee: 70
    $name: Soft knee ×100
    $description: "0-100"
  $name: Bloom
- sunrays:
  - enabled: true
    $name: Enabled
  - resolution: 196
    $name: Resolution
    $description: "64-512"
  - intensity: 100
    $name: Intensity ×100
    $description: "0-300"
  $name: Sun rays
*/
// ==/WindhawkModSettings==
// clang-format on

/*
Shaders adapted from PavelDoGreat/WebGL-Fluid-Simulation.

MIT License

Copyright (c) 2017 Pavel Dobryakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

////////////////////////////////////////////////////////////////////////////////
// Shaders (HLSL port of the WebGL shaders; see the license above)

const char kShaderSource[] = R"HLSL(
cbuffer Params : register(b0) {
    float2 texelSize;
    float2 splatPoint;
    float3 color;
    float radius;
    float3 curve;
    float aspectRatio;
    float3 background;
    float dt;
    float dissipation;
    float curl;
    float value;
    float threshold;
    float intensity;
    float weight;
    float2 padding;
};

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
SamplerState linearClamp : register(s0);

struct Varyings {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float2 l : TEXCOORD1;
    float2 r : TEXCOORD2;
    float2 t : TEXCOORD3;
    float2 b : TEXCOORD4;
};

// The math keeps the original WebGL convention (v grows upwards). Direct3D
// stores rows top-down, so v is flipped on every read. Rendering and sampling
// then agree, and v = 1 is the top of the screen, as in the original.
float4 Tex(Texture2D source, float2 uv) {
    return source.SampleLevel(linearClamp, float2(uv.x, 1.0 - uv.y), 0);
}

// One triangle covering the target; no vertex buffer needed.
Varyings FullscreenTriangle(uint id) {
    float2 uv = float2((id << 1) & 2, id & 2);
    Varyings o;
    o.position = float4(uv * 2.0 - 1.0, 0.0, 1.0);
    o.uv = uv;
    o.l = uv;
    o.r = uv;
    o.t = uv;
    o.b = uv;
    return o;
}

Varyings BaseVS(uint id : SV_VertexID) {
    Varyings o = FullscreenTriangle(id);
    o.l = o.uv - float2(texelSize.x, 0.0);
    o.r = o.uv + float2(texelSize.x, 0.0);
    o.t = o.uv + float2(0.0, texelSize.y);
    o.b = o.uv - float2(0.0, texelSize.y);
    return o;
}

Varyings BlurVS(uint id : SV_VertexID) {
    Varyings o = FullscreenTriangle(id);
    float offset = 1.33333333;
    o.l = o.uv - texelSize * offset;
    o.r = o.uv + texelSize * offset;
    return o;
}

float4 BlurPS(Varyings i) : SV_Target {
    float4 sum = Tex(tex0, i.uv) * 0.29411764;
    sum += Tex(tex0, i.l) * 0.35294117;
    sum += Tex(tex0, i.r) * 0.35294117;
    return sum;
}

float4 ClearPS(Varyings i) : SV_Target {
    return value * Tex(tex0, i.uv);
}

float4 SplatPS(Varyings i) : SV_Target {
    float2 p = i.uv - splatPoint;
    p.x *= aspectRatio;
    float3 splat = exp(-dot(p, p) / radius) * color;
    float3 base = Tex(tex0, i.uv).xyz;
    return float4(base + splat, 1.0);
}

// tex0: velocity, tex1: advected quantity.
float4 AdvectionPS(Varyings i) : SV_Target {
    float2 coord = i.uv - dt * Tex(tex0, i.uv).xy * texelSize;
    float4 result = Tex(tex1, coord);
    float decay = 1.0 + dissipation * dt;
    return result / decay;
}

float4 DivergencePS(Varyings i) : SV_Target {
    float L = Tex(tex0, i.l).x;
    float R = Tex(tex0, i.r).x;
    float T = Tex(tex0, i.t).y;
    float B = Tex(tex0, i.b).y;

    float2 C = Tex(tex0, i.uv).xy;
    if (i.l.x < 0.0) { L = -C.x; }
    if (i.r.x > 1.0) { R = -C.x; }
    if (i.t.y > 1.0) { T = -C.y; }
    if (i.b.y < 0.0) { B = -C.y; }

    float div = 0.5 * (R - L + T - B);
    return float4(div, 0.0, 0.0, 1.0);
}

float4 CurlPS(Varyings i) : SV_Target {
    float L = Tex(tex0, i.l).y;
    float R = Tex(tex0, i.r).y;
    float T = Tex(tex0, i.t).x;
    float B = Tex(tex0, i.b).x;
    float vorticity = R - L - T + B;
    return float4(0.5 * vorticity, 0.0, 0.0, 1.0);
}

// tex0: velocity, tex1: curl.
float4 VorticityPS(Varyings i) : SV_Target {
    float L = Tex(tex1, i.l).x;
    float R = Tex(tex1, i.r).x;
    float T = Tex(tex1, i.t).x;
    float B = Tex(tex1, i.b).x;
    float C = Tex(tex1, i.uv).x;

    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= curl * C;
    force.y *= -1.0;

    float2 velocity = Tex(tex0, i.uv).xy;
    return float4(velocity + force * dt, 0.0, 1.0);
}

// tex0: pressure, tex1: divergence.
float4 PressurePS(Varyings i) : SV_Target {
    float L = Tex(tex0, i.l).x;
    float R = Tex(tex0, i.r).x;
    float T = Tex(tex0, i.t).x;
    float B = Tex(tex0, i.b).x;
    float divergence = Tex(tex1, i.uv).x;
    float pressure = (L + R + B + T - divergence) * 0.25;
    return float4(pressure, 0.0, 0.0, 1.0);
}

// tex0: pressure, tex1: velocity.
float4 GradientSubtractPS(Varyings i) : SV_Target {
    float L = Tex(tex0, i.l).x;
    float R = Tex(tex0, i.r).x;
    float T = Tex(tex0, i.t).x;
    float B = Tex(tex0, i.b).x;
    float2 velocity = Tex(tex1, i.uv).xy;
    velocity -= float2(R - L, T - B);
    return float4(velocity, 0.0, 1.0);
}

float4 BloomPrefilterPS(Varyings i) : SV_Target {
    float3 c = Tex(tex0, i.uv).rgb;
    float br = max(c.r, max(c.g, c.b));
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    c *= max(rq, br - threshold) / max(br, 0.0001);
    return float4(c, 0.0);
}

float4 BloomBlurPS(Varyings i) : SV_Target {
    float4 sum = Tex(tex0, i.l) + Tex(tex0, i.r) + Tex(tex0, i.t) +
                 Tex(tex0, i.b);
    return sum * 0.25;
}

float4 BloomFinalPS(Varyings i) : SV_Target {
    float4 sum = Tex(tex0, i.l) + Tex(tex0, i.r) + Tex(tex0, i.t) +
                 Tex(tex0, i.b);
    return sum * 0.25 * intensity;
}

float4 SunraysMaskPS(Varyings i) : SV_Target {
    float4 c = Tex(tex0, i.uv);
    float br = max(c.r, max(c.g, c.b));
    c.a = 1.0 - min(max(br * 20.0, 0.0), 0.8);
    return c;
}

float4 SunraysPS(Varyings i) : SV_Target {
    const float density = 0.3;
    const float decay = 0.95;
    const float exposure = 0.7;

    float2 coord = i.uv;
    float2 dir = (i.uv - 0.5) * (density / 16.0);
    float illuminationDecay = 1.0;
    float rays = Tex(tex0, i.uv).a;

    [unroll] for (int k = 0; k < 16; k++) {
        coord -= dir;
        rays += Tex(tex0, coord).a * illuminationDecay * weight;
        illuminationDecay *= decay;
    }
    return float4(rays * exposure, 0.0, 0.0, 1.0);
}

float3 LinearToGamma(float3 c) {
    c = max(c, float3(0.0, 0.0, 0.0));
    return max(1.055 * pow(c, float3(0.416666667, 0.416666667, 0.416666667)) - 0.055,
               float3(0.0, 0.0, 0.0));
}

// tex0: dye, tex1: bloom, tex2: sun rays.
float4 DisplayPS(Varyings i) : SV_Target {
    float3 c = Tex(tex0, i.uv).rgb;

#ifdef SHADING
    float3 lc = Tex(tex0, i.l).rgb;
    float3 rc = Tex(tex0, i.r).rgb;
    float3 tc = Tex(tex0, i.t).rgb;
    float3 bc = Tex(tex0, i.b).rgb;

    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);

    float3 n = normalize(float3(dx, dy, length(texelSize)));
    float diffuse = clamp(dot(n, float3(0.0, 0.0, 1.0)) + 0.7, 0.7, 1.0);
    c *= diffuse;
#endif

#ifdef BLOOM
    float3 bloom = Tex(tex1, i.uv).rgb;
#endif

#ifdef SUNRAYS
    float sunrays = Tex(tex2, i.uv).r;
    c *= sunrays;
#ifdef BLOOM
    bloom *= sunrays;
#endif
#endif

#ifdef BLOOM
    // Hash noise replaces the demo's dithering texture.
    float noise = frac(sin(dot(i.uv, float2(12.9898, 78.233))) * 43758.5453);
    noise = noise * 2.0 - 1.0;
    bloom += noise / 255.0;
    bloom = LinearToGamma(bloom);
    c += bloom;
#endif

    float a = max(c.r, max(c.g, c.b));
    return float4(c + background * (1.0 - saturate(a)), 1.0);
}
)HLSL";

// Must match the cbuffer layout above (HLSL packs into 16-byte registers).
struct alignas(16) ShaderParams {
    float texelSize[2];
    float splatPoint[2];
    float color[3];
    float radius;
    float curve[3];
    float aspectRatio;
    float background[3];
    float dt;
    float dissipation;
    float curl;
    float value;
    float threshold;
    float intensity;
    float weight;
    float padding[2];
};
static_assert(sizeof(ShaderParams) == 96, "ShaderParams must match cbuffer");

////////////////////////////////////////////////////////////////////////////////
// Small helpers

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : p_(std::exchange(other.p_, nullptr)) {}
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            p_ = std::exchange(other.p_, nullptr);
        }
        return *this;
    }
    ~ComPtr() { Reset(); }

    void Reset() {
        if (p_) {
            p_->Release();
            p_ = nullptr;
        }
    }
    T* Get() const { return p_; }
    T** Put() {
        Reset();
        return &p_;
    }
    T* operator->() const { return p_; }
    explicit operator bool() const { return p_ != nullptr; }

   private:
    T* p_ = nullptr;
};

class HResultError : public std::runtime_error {
   public:
    HResultError(const char* what, HRESULT hr)
        : std::runtime_error(Format(what, hr)), hr_(hr) {}
    HRESULT hr() const { return hr_; }

   private:
    static std::string Format(const char* what, HRESULT hr) {
        char buffer[160];
        snprintf(buffer, sizeof(buffer), "%s failed (0x%08lX)", what,
                 static_cast<unsigned long>(hr));
        return buffer;
    }
    HRESULT hr_;
};

void Check(HRESULT hr, const char* what) {
    if (FAILED(hr)) {
        throw HResultError(what, hr);
    }
}

bool IsDeviceLost(HRESULT hr) {
    return hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET ||
           hr == DXGI_ERROR_DEVICE_HUNG;
}

void PumpMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class BatteryMode { Normal, HalfFps, Pause };

struct Settings {
    int fps;
    bool pause;
    bool pauseWhenCovered;
    BatteryMode onBattery;
    bool primaryOnly;

    bool mouse;
    bool desktopOnly;
    float mouseForce;

    int simResolution;
    int dyeResolution;
    float densityDissipation;
    float velocityDissipation;
    float pressure;
    int pressureIterations;
    float curl;
    float splatRadius;

    bool colorful;
    float colorSpeed;
    float fixedColor[3];
    float background[3];
    bool shading;

    bool autoMotion;
    int emitters;
    float autoSpeed;
    float autoForce;
    float autoColor;
    bool bursts;
    int burstSeconds;

    bool bloom;
    int bloomResolution;
    int bloomLevels;
    float bloomIntensity;
    float bloomThreshold;
    float bloomKnee;

    bool sunrays;
    int sunraysResolution;
    float sunraysWeight;
};

int IntSetting(const wchar_t* key, int lo, int hi) {
    return std::clamp(Wh_GetIntSetting(key), lo, hi);
}

bool BoolSetting(const wchar_t* key) {
    return Wh_GetIntSetting(key) != 0;
}

BatteryMode ReadBatteryMode() {
    PCWSTR value = Wh_GetStringSetting(L"general.onBattery");
    BatteryMode mode = BatteryMode::HalfFps;
    if (value) {
        if (wcscmp(value, L"normal") == 0) {
            mode = BatteryMode::Normal;
        } else if (wcscmp(value, L"pause") == 0) {
            mode = BatteryMode::Pause;
        }
        Wh_FreeStringSetting(value);
    }
    return mode;
}

Settings ReadSettings() {
    Settings s{};
    s.fps = IntSetting(L"general.fps", 15, 120);
    s.pause = BoolSetting(L"general.pause");
    s.pauseWhenCovered = BoolSetting(L"general.pauseWhenCovered");
    s.onBattery = ReadBatteryMode();
    s.primaryOnly = BoolSetting(L"general.primaryOnly");

    s.mouse = BoolSetting(L"mouse.enabled");
    s.desktopOnly = BoolSetting(L"mouse.desktopOnly");
    s.mouseForce = float(IntSetting(L"mouse.force", 100, 15000));

    s.simResolution = IntSetting(L"simulation.simResolution", 64, 512);
    s.dyeResolution = IntSetting(L"simulation.dyeResolution", 256, 2048);
    s.densityDissipation =
        IntSetting(L"simulation.densityDissipation", 0, 1000) / 100.f;
    s.velocityDissipation =
        IntSetting(L"simulation.velocityDissipation", 0, 1000) / 100.f;
    s.pressure = IntSetting(L"simulation.pressure", 0, 100) / 100.f;
    s.pressureIterations = IntSetting(L"simulation.pressureIterations", 5, 60);
    s.curl = float(IntSetting(L"simulation.curl", 0, 100));
    // The setting mirrors the demo's SPLAT_RADIUS (25 = 0.25). The demo then
    // divides it by 100 before passing it to the shader, hence 10000 here.
    s.splatRadius = IntSetting(L"simulation.splatRadius", 1, 200) / 10000.f;

    s.colorful = BoolSetting(L"colors.colorful");
    s.colorSpeed = float(IntSetting(L"colors.colorSpeed", 0, 100));
    s.fixedColor[0] = IntSetting(L"colors.fixedRed", 0, 255) / 255.f;
    s.fixedColor[1] = IntSetting(L"colors.fixedGreen", 0, 255) / 255.f;
    s.fixedColor[2] = IntSetting(L"colors.fixedBlue", 0, 255) / 255.f;
    s.background[0] = IntSetting(L"colors.backgroundRed", 0, 255) / 255.f;
    s.background[1] = IntSetting(L"colors.backgroundGreen", 0, 255) / 255.f;
    s.background[2] = IntSetting(L"colors.backgroundBlue", 0, 255) / 255.f;
    s.shading = BoolSetting(L"colors.shading");

    s.autoMotion = BoolSetting(L"autoMotion.enabled");
    s.emitters = IntSetting(L"autoMotion.trails", 1, 6);
    s.autoSpeed = IntSetting(L"autoMotion.speed", 1, 100) / 100.f;
    s.autoForce = float(IntSetting(L"autoMotion.force", 100, 15000));
    s.autoColor = IntSetting(L"autoMotion.colorAmount", 1, 300) / 100.f;
    s.bursts = BoolSetting(L"autoMotion.bursts");
    s.burstSeconds = IntSetting(L"autoMotion.burstSeconds", 2, 120);

    s.bloom = BoolSetting(L"bloom.enabled");
    s.bloomResolution = IntSetting(L"bloom.resolution", 64, 512);
    s.bloomLevels = IntSetting(L"bloom.levels", 2, 8);
    s.bloomIntensity = IntSetting(L"bloom.intensity", 0, 300) / 100.f;
    s.bloomThreshold = IntSetting(L"bloom.threshold", 0, 300) / 100.f;
    s.bloomKnee = IntSetting(L"bloom.softKnee", 0, 100) / 100.f;

    s.sunrays = BoolSetting(L"sunrays.enabled");
    s.sunraysResolution = IntSetting(L"sunrays.resolution", 64, 512);
    s.sunraysWeight = IntSetting(L"sunrays.intensity", 0, 300) / 100.f;
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// Globals

HINSTANCE g_instance;
HANDLE g_stopEvent = nullptr;
HANDLE g_updateEvent = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_worker = nullptr;
bool g_startupOK = false;

constexpr wchar_t kWindowClass[] = L"WindhawkFluidWallpaperWindow";
constexpr wchar_t kControlClass[] = L"WindhawkFluidWallpaperControl";

constexpr float kTau = 6.283185f;
constexpr double kIdleIntervalMs = 200;
// A monitor counts as covered when less than this share of its work area
// remains visible (tolerates hairline gaps between snapped windows).
constexpr double kCoveredVisibleFraction = 0.005;

// Power setting GUIDs (WinNT.h; not all are in every SDK version).
constexpr GUID kConsoleDisplayState = {
    0x6fe69556,
    0x704a,
    0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};
constexpr GUID kAcDcPowerSource = {
    0x5d3e9a59,
    0xe9d5,
    0x4b00,
    {0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48}};
constexpr GUID kPowerSavingStatus = {  // Windows 10 battery saver.
    0xe00958c0,
    0xc213,
    0x4ace,
    {0xac, 0x77, 0xfe, 0xcc, 0xed, 0x2e, 0xee, 0xa5}};
constexpr GUID kEnergySaverStatus = {  // Windows 11 Energy Saver.
    0x550e8400,
    0xe29b,
    0x41d4,
    {0xa7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00}};

// Written by the control window procedure, read by the render loop. Both run
// on the render thread.
bool g_sessionLocked = false;
bool g_sessionDisconnected = false;  // Fast user switching, RDP disconnect.
bool g_displayOff = false;
bool g_onBattery = false;
bool g_batterySaver = false;
bool g_energySaver = false;

////////////////////////////////////////////////////////////////////////////////
// Desktop layout

// Explorer's process. The mod runs in its own process, so desktop windows are
// matched against the shell's process.
DWORD ShellProcessId() {
    DWORD process = 0;
    if (HWND shell = GetShellWindow()) {
        GetWindowThreadProcessId(shell, &process);
    }
    return process;
}

DWORD WindowProcessId(HWND w) {
    DWORD process = 0;
    GetWindowThreadProcessId(w, &process);
    return process;
}

struct IconHostSearch {
    DWORD shellProcess;
    HWND result;
};

BOOL CALLBACK FindIconHost(HWND w, LPARAM param) {
    auto search = reinterpret_cast<IconHostSearch*>(param);
    if (WindowProcessId(w) == search->shellProcess && IsWindowVisible(w) &&
        FindWindowExW(w, nullptr, L"SHELLDLL_DefView", nullptr)) {
        search->result = w;
        return FALSE;
    }
    return TRUE;
}

// Returns the window to parent the wallpaper to. On the Windows 11 24H2+
// layout, that's Progman, and `iconView` receives the icon view to stay
// below. On the classic layout, it's the WorkerW behind the icons and
// `iconView` is null.
HWND FindWallpaperHost(HWND& iconView) {
    iconView = nullptr;
    DWORD shellProcess = ShellProcessId();
    if (!shellProcess) {
        return nullptr;
    }

    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman && WindowProcessId(progman) == shellProcess) {
        HWND view =
            FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
        HWND worker = FindWindowExW(progman, nullptr, L"WorkerW", nullptr);
        if (view && worker &&
            (GetWindowLongPtrW(progman, GWL_EXSTYLE) &
             WS_EX_NOREDIRECTIONBITMAP) &&
            (GetWindowLongPtrW(view, GWL_EXSTYLE) & WS_EX_LAYERED)) {
            iconView = view;
            return progman;
        }
    }

    IconHostSearch search{shellProcess, nullptr};
    EnumWindows(FindIconHost, reinterpret_cast<LPARAM>(&search));
    if (!search.result) {
        return nullptr;
    }
    HWND host = FindWindowExW(nullptr, search.result, L"WorkerW", nullptr);
    if (!host || !IsWindowVisible(host) ||
        WindowProcessId(host) != shellProcess ||
        FindWindowExW(host, nullptr, L"SHELLDLL_DefView", nullptr)) {
        return nullptr;
    }
    return host;
}

HWND FindOrCreateWallpaperHost(HWND& iconView) {
    HWND host = FindWallpaperHost(iconView);
    if (host) {
        return host;
    }
    HWND progman = FindWindowW(L"Progman", nullptr);
    DWORD shellProcess = ShellProcessId();
    if (progman && shellProcess && WindowProcessId(progman) == shellProcess) {
        // Undocumented: ask Explorer to create the WorkerW wallpaper layer,
        // the same message used by other wallpaper engines.
        DWORD_PTR result = 0;
        SendMessageTimeoutW(progman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000,
                            &result);
        host = FindWallpaperHost(iconView);
    }
    return host;
}

bool ClassIsAnyOf(HWND w, std::initializer_list<const wchar_t*> names) {
    wchar_t cls[80]{};
    if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) {
        return false;
    }
    for (const wchar_t* name : names) {
        if (wcscmp(cls, name) == 0) {
            return true;
        }
    }
    return false;
}

bool IsShellOrOwnWindow(HWND w) {
    return ClassIsAnyOf(
        w, {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
            kWindowClass, kControlClass});
}

bool DesktopAt(POINT p) {
    HWND w = WindowFromPoint(p);
    for (int i = 0; w && i < 8; i++, w = GetParent(w)) {
        if (ClassIsAnyOf(w, {L"SHELLDLL_DefView", L"Progman", L"WorkerW",
                             kWindowClass})) {
            return true;
        }
    }
    return false;
}

struct MonitorInfo {
    HMONITOR monitor;
    RECT rect;
    RECT work;
    bool primary;
};

BOOL CALLBACK CollectMonitor(HMONITOR h, HDC, LPRECT, LPARAM param) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(h, &mi)) {
        reinterpret_cast<std::vector<MonitorInfo>*>(param)->push_back(
            {h, mi.rcMonitor, mi.rcWork,
             (mi.dwFlags & MONITORINFOF_PRIMARY) != 0});
    }
    return TRUE;
}

std::vector<MonitorInfo> Monitors(bool primaryOnly) {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitor,
                        reinterpret_cast<LPARAM>(&list));
    if (primaryOnly) {
        list.erase(
            std::remove_if(list.begin(), list.end(),
                           [](const MonitorInfo& m) { return !m.primary; }),
            list.end());
    }
    return list;
}

bool SameMonitors(const std::vector<MonitorInfo>& a,
                  const std::vector<MonitorInfo>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].monitor != b[i].monitor ||
            !EqualRect(&a[i].rect, &b[i].rect)) {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Desktop visibility: subtract every visible window from each monitor's work
// area; a monitor is covered when (almost) nothing is left. This handles
// maximized and full-screen windows as well as tiled or snapped windows.

bool IsCloaked(HWND w) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

// Layered windows can be partly or fully transparent. Only those with a
// known constant full opacity count as covering.
bool IsOpaqueLayered(HWND w) {
    COLORREF key = 0;
    BYTE alpha = 0;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(w, &key, &alpha, &flags)) {
        return false;  // UpdateLayeredWindow: per-pixel alpha, unknown.
    }
    return !(flags & LWA_COLORKEY) && (!(flags & LWA_ALPHA) || alpha == 255);
}

struct CoverRegion {
    HMONITOR monitor;
    RECT work;
    HRGN visible;
};

// Cheap checks first: this runs over every top-level window 4 times a second.
BOOL CALLBACK SubtractWindow(HWND w, LPARAM param) {
    if (!IsWindowVisible(w) || IsIconic(w)) {
        return TRUE;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(w, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TRANSPARENT) ||
        ((exStyle & WS_EX_LAYERED) && !IsOpaqueLayered(w))) {
        return TRUE;
    }
    // The visible frame, without the invisible resize borders.
    RECT r{};
    if (FAILED(DwmGetWindowAttribute(w, DWMWA_EXTENDED_FRAME_BOUNDS, &r,
                                     sizeof(r))) &&
        !GetWindowRect(w, &r)) {
        return TRUE;
    }
    auto& regions = *reinterpret_cast<std::vector<CoverRegion>*>(param);
    bool touches = false;
    for (const auto& region : regions) {
        RECT overlap;
        if (IntersectRect(&overlap, &r, &region.work)) {
            touches = true;
            break;
        }
    }
    if (!touches || IsShellOrOwnWindow(w) || IsCloaked(w)) {
        return TRUE;
    }
    HRGN windowRegion = CreateRectRgnIndirect(&r);
    if (windowRegion) {
        for (auto& region : regions) {
            CombineRgn(region.visible, region.visible, windowRegion, RGN_DIFF);
        }
        DeleteObject(windowRegion);
    }
    return TRUE;
}

LONGLONG RegionArea(HRGN region) {
    DWORD size = GetRegionData(region, 0, nullptr);
    if (!size) {
        return LLONG_MAX;  // Unknown: treat as visible.
    }
    std::vector<BYTE> buffer(size);
    auto data = reinterpret_cast<RGNDATA*>(buffer.data());
    if (!GetRegionData(region, size, data)) {
        return LLONG_MAX;
    }
    auto rects = reinterpret_cast<const RECT*>(data->Buffer);
    LONGLONG area = 0;
    for (DWORD i = 0; i < data->rdh.nCount; i++) {
        area += LONGLONG(rects[i].right - rects[i].left) *
                (rects[i].bottom - rects[i].top);
    }
    return area;
}

std::vector<HMONITOR> CoveredMonitors(
    const std::vector<MonitorInfo>& monitors) {
    std::vector<CoverRegion> regions;
    for (const auto& mi : monitors) {
        if (HRGN rgn = CreateRectRgnIndirect(&mi.work)) {
            regions.push_back({mi.monitor, mi.work, rgn});
        }
    }
    EnumWindows(SubtractWindow, reinterpret_cast<LPARAM>(&regions));

    std::vector<HMONITOR> covered;
    for (auto& region : regions) {
        LONGLONG total = LONGLONG(region.work.right - region.work.left) *
                         (region.work.bottom - region.work.top);
        if (RegionArea(region.visible) <= total * kCoveredVisibleFraction) {
            covered.push_back(region.monitor);
        }
        DeleteObject(region.visible);
    }
    return covered;
}

////////////////////////////////////////////////////////////////////////////////
// Direct3D 11

// d3dcompiler_47.dll ships with Windows 10 and 11. It's loaded from System32
// at runtime, so the mod doesn't depend on an import library for it.
class ShaderCompiler {
   public:
    ShaderCompiler() {
        module_ = LoadLibraryExW(L"d3dcompiler_47.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module_) {
            compile_ = reinterpret_cast<pD3DCompile>(
                reinterpret_cast<void*>(GetProcAddress(module_, "D3DCompile")));
        }
    }
    ~ShaderCompiler() {
        if (module_) {
            FreeLibrary(module_);
        }
    }
    ShaderCompiler(const ShaderCompiler&) = delete;
    ShaderCompiler& operator=(const ShaderCompiler&) = delete;

    ComPtr<ID3DBlob> Compile(const char* entry,
                             const char* profile,
                             const D3D_SHADER_MACRO* defines = nullptr) {
        if (!compile_) {
            throw std::runtime_error("d3dcompiler_47.dll is unavailable");
        }
        ComPtr<ID3DBlob> code, errors;
        HRESULT hr = compile_(kShaderSource, sizeof(kShaderSource) - 1,
                              "fluid.hlsl", defines, nullptr, entry, profile,
                              D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, code.Put(),
                              errors.Put());
        if (FAILED(hr)) {
            std::string message = std::string("Shader ") + entry + " failed";
            if (errors) {
                message += ": ";
                message.append(
                    static_cast<const char*>(errors->GetBufferPointer()),
                    errors->GetBufferSize());
            }
            throw std::runtime_error(message);
        }
        return code;
    }

   private:
    HMODULE module_ = nullptr;
    pD3DCompile compile_ = nullptr;
};

struct RenderTarget {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    int w = 0;
    int h = 0;
};

struct DoubleTarget {
    RenderTarget read;
    RenderTarget write;
    void swap() { std::swap(read, write); }
};

// The device, shaders and fixed pipeline state, shared by all monitors.
class Gpu {
   public:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGIFactory2> factory;
    ComPtr<IDCompositionDevice> dcomp;
    int maxTextureSize = 8192;

    ComPtr<ID3D11VertexShader> baseVS, blurVS;
    ComPtr<ID3D11PixelShader> blurPS, clearPS, splatPS, advectionPS;
    ComPtr<ID3D11PixelShader> divergencePS, curlPS, vorticityPS, pressurePS;
    ComPtr<ID3D11PixelShader> gradientPS, bloomPrefilterPS, bloomBlurPS;
    ComPtr<ID3D11PixelShader> bloomFinalPS, sunraysMaskPS, sunraysPS;
    ComPtr<ID3D11PixelShader> displayPS;

    // Uniforms for the next Pass(); uploaded on every draw.
    ShaderParams params{};

    void Create(const Settings& s, ShaderCompiler& compiler) {
        CreateDevice();
        CreateShaders(s, compiler);
        CreateState();
    }

    void CheckDevice() const {
        HRESULT reason = device->GetDeviceRemovedReason();
        if (FAILED(reason)) {
            throw HResultError("GPU device", reason);
        }
    }

    RenderTarget CreateTarget(int w, int h) {
        RenderTarget t;
        t.w = w;
        t.h = h;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = UINT(w);
        desc.Height = UINT(h);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        Check(device->CreateTexture2D(&desc, nullptr, t.texture.Put()),
              "CreateTexture2D");
        Check(device->CreateRenderTargetView(t.texture.Get(), nullptr,
                                             t.rtv.Put()),
              "CreateRenderTargetView");
        Check(device->CreateShaderResourceView(t.texture.Get(), nullptr,
                                               t.srv.Put()),
              "CreateShaderResourceView");
        const float zero[4]{};
        context->ClearRenderTargetView(t.rtv.Get(), zero);
        return t;
    }

    void SetTexelSize(const RenderTarget& t) {
        params.texelSize[0] = 1.f / t.w;
        params.texelSize[1] = 1.f / t.h;
    }

    // Draws one full-screen triangle into `target`. The target is bound
    // before the inputs, so a texture that was just written can be read.
    void Pass(ID3D11PixelShader* ps,
              ID3D11RenderTargetView* target,
              int w,
              int h,
              std::initializer_list<const RenderTarget*> inputs,
              bool additive = false,
              ID3D11VertexShader* vs = nullptr) {
        context->OMSetRenderTargets(1, &target, nullptr);
        D3D11_VIEWPORT viewport{0, 0, float(w), float(h), 0, 1};
        context->RSSetViewports(1, &viewport);

        ID3D11ShaderResourceView* views[3]{};
        size_t slot = 0;
        for (const RenderTarget* input : inputs) {
            if (slot < ARRAYSIZE(views)) {
                views[slot++] = input ? input->srv.Get() : nullptr;
            }
        }
        context->PSSetShaderResources(0, ARRAYSIZE(views), views);

        D3D11_MAPPED_SUBRESOURCE mapped{};
        Check(context->Map(paramsBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
                           &mapped),
              "Map");
        memcpy(mapped.pData, &params, sizeof(params));
        context->Unmap(paramsBuffer_.Get(), 0);

        context->VSSetShader(vs ? vs : baseVS.Get(), nullptr, 0);
        context->PSSetShader(ps, nullptr, 0);
        context->OMSetBlendState(additive ? additive_.Get() : nullptr, nullptr,
                                 0xffffffff);
        context->Draw(3, 0);
    }

    void Pass(ID3D11PixelShader* ps,
              const RenderTarget& target,
              std::initializer_list<const RenderTarget*> inputs,
              bool additive = false,
              ID3D11VertexShader* vs = nullptr) {
        Pass(ps, target.rtv.Get(), target.w, target.h, inputs, additive, vs);
    }

   private:
    ComPtr<ID3D11SamplerState> sampler_;
    ComPtr<ID3D11BlendState> additive_;
    ComPtr<ID3D11RasterizerState> rasterizer_;
    ComPtr<ID3D11Buffer> paramsBuffer_;

    void CreateDevice() {
        const D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
        D3D_FEATURE_LEVEL level{};
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        HRESULT hr =
            D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                              levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                              device.Put(), &level, context.Put());
        if (hr == E_INVALIDARG) {  // Runtimes without 11.1 reject the list.
            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   flags, levels + 1, ARRAYSIZE(levels) - 1,
                                   D3D11_SDK_VERSION, device.Put(), &level,
                                   context.Put());
        }
        Check(hr, "D3D11CreateDevice");
        maxTextureSize = level >= D3D_FEATURE_LEVEL_11_0 ? 16384 : 8192;

        ComPtr<IDXGIDevice1> dxgiDevice;
        Check(device->QueryInterface(IID_PPV_ARGS(dxgiDevice.Put())),
              "IDXGIDevice1");
        // Don't let the CPU queue frames far ahead of the GPU.
        dxgiDevice->SetMaximumFrameLatency(2);

        ComPtr<IDXGIAdapter> adapter;
        Check(dxgiDevice->GetAdapter(adapter.Put()), "GetAdapter");
        Check(adapter->GetParent(IID_PPV_ARGS(factory.Put())), "IDXGIFactory2");
        DXGI_ADAPTER_DESC desc{};
        adapter->GetDesc(&desc);
        Wh_Log(L"Direct3D: %s, feature level %x", desc.Description,
               unsigned(level));

        Check(DCompositionCreateDevice(dxgiDevice.Get(),
                                       IID_PPV_ARGS(dcomp.Put())),
              "DCompositionCreateDevice");
    }

    // Compiling takes a moment; messages are pumped in between because the
    // wallpaper windows are children of Explorer's window, which ties the
    // two threads' input together.
    void CreateShaders(const Settings& s, ShaderCompiler& compiler) {
        auto vertex = [&](const char* entry, ComPtr<ID3D11VertexShader>& out) {
            auto code = compiler.Compile(entry, "vs_4_0");
            Check(device->CreateVertexShader(code->GetBufferPointer(),
                                             code->GetBufferSize(), nullptr,
                                             out.Put()),
                  "CreateVertexShader");
            PumpMessages();
        };
        auto pixel = [&](const char* entry, ComPtr<ID3D11PixelShader>& out,
                         const D3D_SHADER_MACRO* defines = nullptr) {
            auto code = compiler.Compile(entry, "ps_4_0", defines);
            Check(device->CreatePixelShader(code->GetBufferPointer(),
                                            code->GetBufferSize(), nullptr,
                                            out.Put()),
                  "CreatePixelShader");
            PumpMessages();
        };

        vertex("BaseVS", baseVS);
        vertex("BlurVS", blurVS);
        pixel("BlurPS", blurPS);
        pixel("ClearPS", clearPS);
        pixel("SplatPS", splatPS);
        pixel("AdvectionPS", advectionPS);
        pixel("DivergencePS", divergencePS);
        pixel("CurlPS", curlPS);
        pixel("VorticityPS", vorticityPS);
        pixel("PressurePS", pressurePS);
        pixel("GradientSubtractPS", gradientPS);
        pixel("BloomPrefilterPS", bloomPrefilterPS);
        pixel("BloomBlurPS", bloomBlurPS);
        pixel("BloomFinalPS", bloomFinalPS);
        pixel("SunraysMaskPS", sunraysMaskPS);
        pixel("SunraysPS", sunraysPS);

        std::vector<D3D_SHADER_MACRO> defines;
        if (s.shading) {
            defines.push_back({"SHADING", "1"});
        }
        if (s.bloom) {
            defines.push_back({"BLOOM", "1"});
        }
        if (s.sunrays) {
            defines.push_back({"SUNRAYS", "1"});
        }
        defines.push_back({nullptr, nullptr});
        pixel("DisplayPS", displayPS, defines.data());
    }

    void CreateState() {
        D3D11_SAMPLER_DESC sampler{};
        sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampler.MaxLOD = D3D11_FLOAT32_MAX;
        Check(device->CreateSamplerState(&sampler, sampler_.Put()),
              "CreateSamplerState");

        D3D11_BLEND_DESC blend{};
        auto& rt = blend.RenderTarget[0];
        rt.BlendEnable = TRUE;
        rt.SrcBlend = D3D11_BLEND_ONE;
        rt.DestBlend = D3D11_BLEND_ONE;
        rt.BlendOp = D3D11_BLEND_OP_ADD;
        rt.SrcBlendAlpha = D3D11_BLEND_ONE;
        rt.DestBlendAlpha = D3D11_BLEND_ONE;
        rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        Check(device->CreateBlendState(&blend, additive_.Put()),
              "CreateBlendState");

        // The full-screen triangle's winding isn't relevant; don't cull.
        D3D11_RASTERIZER_DESC raster{};
        raster.FillMode = D3D11_FILL_SOLID;
        raster.CullMode = D3D11_CULL_NONE;
        raster.DepthClipEnable = TRUE;
        Check(device->CreateRasterizerState(&raster, rasterizer_.Put()),
              "CreateRasterizerState");

        D3D11_BUFFER_DESC buffer{};
        buffer.ByteWidth = sizeof(ShaderParams);
        buffer.Usage = D3D11_USAGE_DYNAMIC;
        buffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        Check(device->CreateBuffer(&buffer, nullptr, paramsBuffer_.Put()),
              "CreateBuffer");

        // Fixed state for every pass: no vertex buffers, one sampler, one
        // constant buffer.
        context->IASetInputLayout(nullptr);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->RSSetState(rasterizer_.Get());
        ID3D11SamplerState* samplers[] = {sampler_.Get()};
        context->PSSetSamplers(0, 1, samplers);
        ID3D11Buffer* buffers[] = {paramsBuffer_.Get()};
        context->VSSetConstantBuffers(0, 1, buffers);
        context->PSSetConstantBuffers(0, 1, buffers);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Wallpaper: one window, swap chain and simulation per monitor

class Wallpaper {
   public:
    Wallpaper(Gpu& gpu, const MonitorInfo& info, const Settings& settings)
        : gpu_(gpu), info_(info), s_(settings) {}
    Wallpaper(const Wallpaper&) = delete;
    Wallpaper& operator=(const Wallpaper&) = delete;

    ~Wallpaper() {
        // Release composition objects before the window they target.
        visual_.Reset();
        compositionTarget_.Reset();
        if (window_ && IsWindow(window_)) {
            DestroyWindow(window_);  // No WM_PARENTNOTIFY: see CreateHost.
        }
    }

    HWND window() const { return window_; }
    HMONITOR monitor() const { return info_.monitor; }

    void Initialize(HWND parent, HWND iconView) {
        phase_ = Rand() * kTau;
        parent_ = parent;
        iconView_ = iconView;
        CreateHost();
        CreateSwapChain();
        CreateTargets();

        for (int i = 0; i < s_.emitters; i++) {
            emitters_.push_back(EmitterPosition(i, 0));
        }
        for (int i = 0; i < 8; i++) {
            float c[3];
            Color(c, float(i));
            Splat(Rand(), Rand(), (Rand() - .5f) * 1000, (Rand() - .5f) * 1000,
                  c);
        }
        Render();  // Shows the window after the first frame is presented.
        Wh_Log(L"Monitor %ld,%ld %dx%d: window=%p", info_.rect.left,
               info_.rect.top, Width(), Height(), window_);
    }

    // On the 24H2+ layout, Explorer can reorder its children; keep the
    // wallpaper directly below the icon view and above the shell's WorkerW.
    void EnsureLayerOrder() {
        if (!iconView_) {
            return;
        }
        for (HWND previous = GetWindow(window_, GW_HWNDPREV); previous;
             previous = GetWindow(previous, GW_HWNDPREV)) {
            if (previous == iconView_) {
                return;
            }
            if (ClassIsAnyOf(previous, {L"WorkerW"})) {
                break;
            }
        }
        Position(0);
    }

    // Returns true if a frame was simulated and presented.
    bool Step(float dt, POINT mouse, bool mouseActive, bool paused) {
        if (paused) {
            hadMouse_ = false;
            wasPaused_ = true;
            return false;
        }
        if (wasPaused_) {
            dt = std::min(dt, 1.f / 60);
            wasPaused_ = false;
        }
        clock_ += dt;
        burstClock_ += dt;

        Emit(dt);
        ApplyMouse(mouse, mouseActive);
        Simulate(dt);
        Render();
        return true;
    }

   private:
    Gpu& gpu_;
    MonitorInfo info_;
    Settings s_;

    HWND window_ = nullptr;
    HWND parent_ = nullptr;
    HWND iconView_ = nullptr;
    bool shown_ = false;
    ComPtr<IDXGISwapChain1> swapChain_;
    ComPtr<ID3D11RenderTargetView> backBuffer_;
    ComPtr<IDCompositionTarget> compositionTarget_;
    ComPtr<IDCompositionVisual> visual_;

    DoubleTarget velocity_, dye_, pressure_;
    RenderTarget divergence_, curl_, bloom_, sunMask_, sun_, sunTemp_;
    std::vector<RenderTarget> bloomLevels_;

    std::mt19937 random_{std::random_device{}()};
    float clock_ = 0;
    float burstClock_ = 0;
    float phase_ = 0;
    POINT previousMouse_{};
    bool hadMouse_ = false;
    bool wasPaused_ = false;

    struct Emitter {
        float x, y;
    };
    std::vector<Emitter> emitters_;

    int Width() const { return info_.rect.right - info_.rect.left; }
    int Height() const { return info_.rect.bottom - info_.rect.top; }
    float Aspect() const { return float(Width()) / Height(); }
    float Rand() {
        return std::uniform_real_distribution<float>(0, 1)(random_);
    }

    ////////////////////////////////////////////////////////////////////////
    // Setup

    void CreateHost() {
        // A child of Explorer's wallpaper host without a redirection surface:
        // content comes only from DirectComposition. WS_EX_NOPARENTNOTIFY
        // avoids synchronous WM_PARENTNOTIFY calls into Explorer when the
        // window is created or destroyed (Explorer might be hung).
        window_ = CreateWindowExW(
            WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW |
                WS_EX_NOPARENTNOTIFY | WS_EX_TRANSPARENT,
            kWindowClass, L"Fluid Wallpaper", WS_CHILD | WS_CLIPSIBLINGS, 0, 0,
            0, 0, parent_, nullptr, g_instance, nullptr);
        if (!window_) {
            throw std::runtime_error("Cannot create wallpaper window");
        }
        Position(0);  // Shown after the first frame.
    }

    void Position(UINT flags) {
        POINT origin{info_.rect.left, info_.rect.top};
        MapWindowPoints(nullptr, parent_, &origin, 1);
        SetWindowPos(window_, iconView_ ? iconView_ : HWND_BOTTOM, origin.x,
                     origin.y, Width(), Height(), SWP_NOACTIVATE | flags);
    }

    void CreateSwapChain() {
        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = UINT(Width());
        desc.Height = UINT(Height());
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        Check(gpu_.factory->CreateSwapChainForComposition(
                  gpu_.device.Get(), &desc, nullptr, swapChain_.Put()),
              "CreateSwapChainForComposition");

        // With flip-model swap chains in D3D11, buffer 0 always refers to the
        // current back buffer, so one view is enough.
        ComPtr<ID3D11Texture2D> buffer;
        Check(swapChain_->GetBuffer(0, IID_PPV_ARGS(buffer.Put())),
              "GetBuffer");
        Check(gpu_.device->CreateRenderTargetView(buffer.Get(), nullptr,
                                                  backBuffer_.Put()),
              "CreateRenderTargetView");

        Check(gpu_.dcomp->CreateTargetForHwnd(window_, TRUE,
                                              compositionTarget_.Put()),
              "CreateTargetForHwnd");
        Check(gpu_.dcomp->CreateVisual(visual_.Put()), "CreateVisual");
        Check(visual_->SetContent(swapChain_.Get()), "SetContent");
        Check(compositionTarget_->SetRoot(visual_.Get()), "SetRoot");
        Check(gpu_.dcomp->Commit(), "Commit");
    }

    // A target whose shorter side is `base`, matching the monitor's aspect.
    RenderTarget MakeScaledTarget(int base) {
        float a = Aspect();
        if (a >= 1) {
            return gpu_.CreateTarget(int(std::round(base * a)), base);
        }
        return gpu_.CreateTarget(base, int(std::round(base / a)));
    }

    DoubleTarget MakeScaledPair(int base) {
        return {MakeScaledTarget(base), MakeScaledTarget(base)};
    }

    void CreateTargets() {
        // Keep aspect-ratio-scaled allocations within the hardware limit.
        float aspect = std::max(Aspect(), 1.f / Aspect());
        s_.dyeResolution =
            std::min(s_.dyeResolution, int(gpu_.maxTextureSize / aspect));

        velocity_ = MakeScaledPair(s_.simResolution);
        dye_ = MakeScaledPair(s_.dyeResolution);
        pressure_ = MakeScaledPair(s_.simResolution);
        divergence_ = MakeScaledTarget(s_.simResolution);
        curl_ = MakeScaledTarget(s_.simResolution);

        if (s_.bloom) {
            bloom_ = MakeScaledTarget(s_.bloomResolution);
            int w = bloom_.w, h = bloom_.h;
            for (int i = 0; i < s_.bloomLevels; i++) {
                w /= 2;
                h /= 2;
                if (w < 2 || h < 2) {
                    break;
                }
                bloomLevels_.push_back(gpu_.CreateTarget(w, h));
            }
        }
        if (s_.sunrays) {
            sunMask_ = MakeScaledTarget(s_.sunraysResolution);
            sun_ = MakeScaledTarget(s_.sunraysResolution);
            sunTemp_ = MakeScaledTarget(s_.sunraysResolution);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Simulation

    Emitter EmitterPosition(int i, float time) const {
        float p = phase_ + i * 2.39996f;  // Golden angle spreads the trails.
        float t = time * s_.autoSpeed;
        return {.5f + .38f * std::sin(t * (.63f + i * .09f) + p) *
                          std::cos(t * .23f + p * .7f),
                .5f + .38f * std::cos(t * (.49f + i * .07f) + p) *
                          std::sin(t * .31f + p)};
    }

    void Color(float out[3], float offset = 0) const {
        if (!s_.colorful) {
            for (int i = 0; i < 3; i++) {
                out[i] = s_.fixedColor[i] * .15f;
            }
            return;
        }
        // HSV hue cycle with saturation and value 1, scaled by 0.15 like the
        // demo's generateColor().
        float h = std::fmod(
            phase_ / kTau + clock_ * s_.colorSpeed * .025f + offset * .17f,
            1.f);
        for (int i = 0; i < 3; i++) {
            float x = std::fmod(h * 6.f + (i == 0   ? 0.f
                                           : i == 1 ? 4.f
                                                    : 2.f),
                                6.f);
            out[i] = .15f * std::clamp(std::abs(x - 3.f) - 1.f, 0.f, 1.f);
        }
    }

    void Splat(float x, float y, float dx, float dy, const float c[3]) {
        ShaderParams& p = gpu_.params;
        p.aspectRatio = Aspect();
        p.splatPoint[0] = x;
        p.splatPoint[1] = y;
        p.radius = s_.splatRadius * std::max(1.f, Aspect());

        p.color[0] = dx;
        p.color[1] = dy;
        p.color[2] = 0;
        gpu_.Pass(gpu_.splatPS.Get(), velocity_.write, {&velocity_.read});
        velocity_.swap();

        p.color[0] = c[0];
        p.color[1] = c[1];
        p.color[2] = c[2];
        gpu_.Pass(gpu_.splatPS.Get(), dye_.write, {&dye_.read});
        dye_.swap();
    }

    void Emit(float dt) {
        if (!s_.autoMotion) {
            return;
        }
        for (int i = 0; i < s_.emitters; i++) {
            Emitter next = EmitterPosition(i, clock_);
            Emitter old = emitters_[i];
            float c[3];
            Color(c, float(i));
            float amount = s_.autoColor * dt * 60.f;
            for (float& v : c) {
                v *= amount;
            }
            // Momentum comes from the path delta, so it is frame-rate
            // independent.
            Splat(next.x, next.y, (next.x - old.x) * s_.autoForce,
                  (next.y - old.y) * s_.autoForce, c);
            emitters_[i] = next;
        }
        if (s_.bursts && burstClock_ >= s_.burstSeconds) {
            burstClock_ = 0;
            for (int i = 0; i < 4; i++) {
                float c[3];
                Color(c, float(i) + Rand());
                for (float& v : c) {
                    v *= 4;
                }
                Splat(Rand(), Rand(), (Rand() - .5f) * s_.autoForce,
                      (Rand() - .5f) * s_.autoForce, c);
            }
        }
    }

    void ApplyMouse(POINT mouse, bool mouseActive) {
        if (!mouseActive || !PtInRect(&info_.rect, mouse)) {
            hadMouse_ = false;
            previousMouse_ = mouse;
            return;
        }
        bool moved = mouse.x != previousMouse_.x || mouse.y != previousMouse_.y;
        if (hadMouse_ && moved && PtInRect(&info_.rect, previousMouse_)) {
            float w = float(Width()), h = float(Height());
            float dx =
                std::clamp((mouse.x - previousMouse_.x) / w, -.15f, .15f);
            float dy =
                std::clamp((previousMouse_.y - mouse.y) / h, -.15f, .15f);
            // Like the demo, scale the delta on the longer axis so the
            // impulse stays circular.
            float aspect = w / h;
            if (aspect < 1) {
                dx *= aspect;
            }
            if (aspect > 1) {
                dy /= aspect;
            }
            float c[3];
            Color(c);
            Splat((mouse.x - info_.rect.left) / w,
                  1.f - (mouse.y - info_.rect.top) / h, dx * s_.mouseForce,
                  dy * s_.mouseForce, c);
        }
        hadMouse_ = true;
        previousMouse_ = mouse;
    }

    void Simulate(float dt) {
        ShaderParams& p = gpu_.params;
        gpu_.SetTexelSize(velocity_.read);
        p.dt = dt;

        gpu_.Pass(gpu_.curlPS.Get(), curl_, {&velocity_.read});

        p.curl = s_.curl;
        gpu_.Pass(gpu_.vorticityPS.Get(), velocity_.write,
                  {&velocity_.read, &curl_});
        velocity_.swap();

        gpu_.Pass(gpu_.divergencePS.Get(), divergence_, {&velocity_.read});

        p.value = s_.pressure;
        gpu_.Pass(gpu_.clearPS.Get(), pressure_.write, {&pressure_.read});
        pressure_.swap();

        for (int i = 0; i < s_.pressureIterations; i++) {
            gpu_.Pass(gpu_.pressurePS.Get(), pressure_.write,
                      {&pressure_.read, &divergence_});
            pressure_.swap();
        }

        gpu_.Pass(gpu_.gradientPS.Get(), velocity_.write,
                  {&pressure_.read, &velocity_.read});
        velocity_.swap();

        p.dissipation = s_.velocityDissipation;
        gpu_.Pass(gpu_.advectionPS.Get(), velocity_.write,
                  {&velocity_.read, &velocity_.read});
        velocity_.swap();

        // The dye is advected with the simulation texel size, as in the demo.
        p.dissipation = s_.densityDissipation;
        gpu_.Pass(gpu_.advectionPS.Get(), dye_.write,
                  {&velocity_.read, &dye_.read});
        dye_.swap();
    }

    ////////////////////////////////////////////////////////////////////////
    // Rendering

    void ApplyBloom() {
        ShaderParams& p = gpu_.params;
        float knee = s_.bloomThreshold * s_.bloomKnee + .0001f;
        p.curve[0] = s_.bloomThreshold - knee;
        p.curve[1] = knee * 2;
        p.curve[2] = .25f / knee;
        p.threshold = s_.bloomThreshold;
        gpu_.Pass(gpu_.bloomPrefilterPS.Get(), bloom_, {&dye_.read});

        const RenderTarget* last = &bloom_;
        for (const RenderTarget& level : bloomLevels_) {
            gpu_.SetTexelSize(*last);
            gpu_.Pass(gpu_.bloomBlurPS.Get(), level, {last});
            last = &level;
        }
        for (int i = int(bloomLevels_.size()) - 2; i >= 0; i--) {
            gpu_.SetTexelSize(*last);
            gpu_.Pass(gpu_.bloomBlurPS.Get(), bloomLevels_[i], {last},
                      /*additive=*/true);
            last = &bloomLevels_[i];
        }

        gpu_.SetTexelSize(*last);
        p.intensity = s_.bloomIntensity;
        gpu_.Pass(gpu_.bloomFinalPS.Get(), bloom_, {last});
    }

    void ApplySunrays() {
        ShaderParams& p = gpu_.params;
        gpu_.Pass(gpu_.sunraysMaskPS.Get(), sunMask_, {&dye_.read});

        p.weight = s_.sunraysWeight;
        gpu_.Pass(gpu_.sunraysPS.Get(), sun_, {&sunMask_});

        p.texelSize[0] = 1.f / sun_.w;
        p.texelSize[1] = 0;
        gpu_.Pass(gpu_.blurPS.Get(), sunTemp_, {&sun_}, false,
                  gpu_.blurVS.Get());
        p.texelSize[0] = 0;
        p.texelSize[1] = 1.f / sun_.h;
        gpu_.Pass(gpu_.blurPS.Get(), sun_, {&sunTemp_}, false,
                  gpu_.blurVS.Get());
    }

    void Render() {
        if (s_.bloom && bloomLevels_.size() >= 2) {
            ApplyBloom();
        }
        if (s_.sunrays) {
            ApplySunrays();
        }

        ShaderParams& p = gpu_.params;
        p.texelSize[0] = 1.f / Width();
        p.texelSize[1] = 1.f / Height();
        for (int i = 0; i < 3; i++) {
            p.background[i] = s_.background[i];
        }
        gpu_.Pass(gpu_.displayPS.Get(), backBuffer_.Get(), Width(), Height(),
                  {&dye_.read, s_.bloom ? &bloom_ : nullptr,
                   s_.sunrays ? &sun_ : nullptr});

        Check(swapChain_->Present(0, 0), "Present");
        if (!shown_) {
            ShowWindow(window_, SW_SHOWNA);
            shown_ = true;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Window procedures

LRESULT CALLBACK WallpaperWindowProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    switch (m) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            // No redirection surface: DirectComposition supplies the content.
            ValidateRect(w, nullptr);
            return 0;
        case WM_CLOSE:
            return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

DWORD PowerSettingValue(const POWERBROADCAST_SETTING* setting) {
    return setting->DataLength >= sizeof(DWORD)
               ? *reinterpret_cast<const DWORD*>(setting->Data)
               : 0;
}

// Hidden window that receives session and power notifications.
LRESULT CALLBACK ControlWindowProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    if (m == WM_WTSSESSION_CHANGE) {
        switch (wp) {
            case WTS_SESSION_LOCK:
                g_sessionLocked = true;
                break;
            case WTS_SESSION_UNLOCK:
                g_sessionLocked = false;
                break;
            case WTS_CONSOLE_DISCONNECT:
            case WTS_REMOTE_DISCONNECT:
                g_sessionDisconnected = true;
                break;
            case WTS_CONSOLE_CONNECT:
            case WTS_REMOTE_CONNECT:
                g_sessionDisconnected = false;
                break;
        }
        return 0;
    }
    if (m == WM_POWERBROADCAST && wp == PBT_POWERSETTINGCHANGE) {
        auto setting = reinterpret_cast<const POWERBROADCAST_SETTING*>(lp);
        if (setting) {
            DWORD value = PowerSettingValue(setting);
            const GUID& id = setting->PowerSetting;
            if (IsEqualGUID(id, kConsoleDisplayState)) {
                g_displayOff = value == 0;  // 0 off, 1 on, 2 dimmed.
            } else if (IsEqualGUID(id, kAcDcPowerSource)) {
                g_onBattery = value != 0;  // 0 AC, 1 battery, 2 UPS.
            } else if (IsEqualGUID(id, kPowerSavingStatus)) {
                g_batterySaver = value != 0;
            } else if (IsEqualGUID(id, kEnergySaverStatus)) {
                g_energySaver = value != 0;  // 0 off, 1 standard, 2 high.
            }
        }
        return TRUE;
    }
    if (m == WM_CLOSE) {
        return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

bool RegisterWindowClasses() {
    WNDCLASSEXW cls{};
    cls.cbSize = sizeof(cls);
    cls.lpfnWndProc = WallpaperWindowProc;
    cls.hInstance = g_instance;
    cls.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&cls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        return false;
    }

    WNDCLASSEXW controlCls{};
    controlCls.cbSize = sizeof(controlCls);
    controlCls.lpfnWndProc = ControlWindowProc;
    controlCls.hInstance = g_instance;
    controlCls.lpszClassName = kControlClass;
    if (!RegisterClassExW(&controlCls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        UnregisterClassW(kWindowClass, g_instance);
        return false;
    }
    return true;
}

void UnregisterWindowClasses() {
    UnregisterClassW(kControlClass, g_instance);
    UnregisterClassW(kWindowClass, g_instance);
}

////////////////////////////////////////////////////////////////////////////////
// Render thread

// A high-resolution waitable timer (Windows 10 1803+) gives accurate frame
// pacing without changing the system timer resolution. Older systems fall back
// to a regular timer with ~15.6 ms granularity.
HANDLE CreateFrameTimer() {
    HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr,
                                          CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_ALL_ACCESS);
    if (!timer) {
        timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }
    return timer;
}

// Waits until `intervalMs` after `frameStart`, dispatching messages meanwhile.
// Returns true if the stop event was signaled.
bool WaitForNextFrame(HANDLE timer,
                      const LARGE_INTEGER& frameStart,
                      double intervalMs,
                      const LARGE_INTEGER& frequency) {
    for (;;) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        double elapsed = double(now.QuadPart - frameStart.QuadPart) * 1000 /
                         frequency.QuadPart;
        double remaining = intervalMs - elapsed;
        if (remaining <= 0) {
            return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
        }

        HANDLE handles[2] = {g_stopEvent, timer};
        DWORD count = 1;
        DWORD timeout = DWORD(std::ceil(remaining));
        LARGE_INTEGER due;
        due.QuadPart = -LONGLONG(remaining * 10000);  // Relative, 100 ns units.
        if (timer &&
            SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE)) {
            count = 2;
            timeout += 1000;  // Safety net; the timer fires first.
        }

        DWORD result = MsgWaitForMultipleObjectsEx(
            count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0) {
            return true;
        }
        if (result == WAIT_OBJECT_0 + count) {
            PumpMessages();
            continue;
        }
        return false;  // Timer, timeout, or failure (don't spin).
    }
}

struct Renderer {
    ShaderCompiler compiler;
    std::unique_ptr<Gpu> gpu;
    // Declared after `gpu`, so they're destroyed first.
    std::vector<std::unique_ptr<Wallpaper>> wallpapers;
    std::vector<MonitorInfo> monitors;
    HWND parent = nullptr;
    HWND iconView = nullptr;

    void Clear() {
        wallpapers.clear();
        gpu.reset();
    }

    // Returns true if at least one wallpaper is running.
    bool Build(const Settings& settings) {
        Clear();
        monitors = Monitors(settings.primaryOnly);
        parent = FindOrCreateWallpaperHost(iconView);
        if (!parent) {
            Wh_Log(L"Desktop window not found (Explorer not ready?)");
            return false;
        }
        Wh_Log(L"Desktop layout: %s, host=%p",
               iconView ? L"layered (Progman)" : L"classic (WorkerW)", parent);

        gpu = std::make_unique<Gpu>();
        gpu->Create(settings, compiler);
        for (const auto& mi : monitors) {
            // Keep Explorer responsive: see Gpu::CreateShaders.
            PumpMessages();
            auto w = std::make_unique<Wallpaper>(*gpu, mi, settings);
            try {
                w->Initialize(parent, iconView);
                wallpapers.push_back(std::move(w));
            } catch (const HResultError& e) {
                if (IsDeviceLost(e.hr())) {
                    throw;
                }
                Wh_Log(L"Monitor initialization failed: %S", e.what());
            } catch (const std::exception& e) {
                Wh_Log(L"Monitor initialization failed: %S", e.what());
            }
        }
        return !wallpapers.empty();
    }

    // Periodic check; returns true if everything must be rebuilt.
    bool NeedsRebuild(const Settings& settings) {
        if (!gpu || wallpapers.empty()) {
            return true;
        }
        gpu->CheckDevice();  // Throws on device loss.
        HWND currentIcons = nullptr;
        if (FindWallpaperHost(currentIcons) != parent ||
            currentIcons != iconView) {
            return true;  // Explorer restarted or the layout changed.
        }
        if (!SameMonitors(monitors, Monitors(settings.primaryOnly))) {
            return true;
        }
        for (auto& w : wallpapers) {
            if (!IsWindow(w->window())) {
                return true;
            }
            w->EnsureLayerOrder();
        }
        return false;
    }
};

void RunRenderLoop(HANDLE frameTimer) {
    Renderer renderer;
    Settings settings = ReadSettings();
    std::vector<HMONITOR> covered;

    bool rebuild = true;
    int failures = 0;
    ULONGLONG nextRebuild = 0, nextHealthCheck = 0, nextCoverCheck = 0;

    // Exponential backoff: 2, 4, 8, 16, 32 seconds.
    auto scheduleRetry = [&](ULONGLONG now) {
        failures = std::min(failures + 1, 5);
        ULONGLONG delay = 2000ull << (failures - 1);
        nextRebuild = now + delay;
        Wh_Log(L"No wallpaper active; retrying in %llu s", delay / 1000);
    };

    LARGE_INTEGER frequency, last;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last);

    for (;;) {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);
        // Each step is limited to 1/30 s to keep the simulation stable.
        float dt =
            std::clamp(float(double(frameStart.QuadPart - last.QuadPart) /
                             frequency.QuadPart),
                       .001f, 1.f / 30);
        last = frameStart;
        bool anyActive = false;
        bool powerSaving = g_onBattery || g_batterySaver || g_energySaver;

        PumpMessages();

        try {
            if (WaitForSingleObject(g_updateEvent, 0) == WAIT_OBJECT_0) {
                settings = ReadSettings();
                rebuild = true;
                failures = 0;
                nextRebuild = 0;
            }

            ULONGLONG now = GetTickCount64();
            if (now >= nextHealthCheck) {
                nextHealthCheck = now + 2000;
                if (!rebuild && renderer.NeedsRebuild(settings)) {
                    rebuild = true;
                }
            }

            if (rebuild && now >= nextRebuild) {
                rebuild = false;
                if (renderer.Build(settings)) {
                    failures = 0;
                    nextRebuild = 0;
                } else {
                    renderer.Clear();
                    scheduleRetry(now);
                }
                nextCoverCheck = 0;
                QueryPerformanceCounter(&last);
                dt = 1.f / settings.fps;
            }

            if (now >= nextCoverCheck) {
                nextCoverCheck = now + 250;
                covered.clear();
                if (settings.pauseWhenCovered && !renderer.wallpapers.empty()) {
                    covered = CoveredMonitors(renderer.monitors);
                }
            }

            bool globalPause =
                settings.pause || g_sessionLocked || g_sessionDisconnected ||
                g_displayOff ||
                (powerSaving && settings.onBattery == BatteryMode::Pause);

            POINT mouse{LONG_MIN, LONG_MIN};
            bool mouseActive = false;
            if (!globalPause && settings.mouse && GetCursorPos(&mouse)) {
                mouseActive = !settings.desktopOnly || DesktopAt(mouse);
            }

            for (auto& w : renderer.wallpapers) {
                bool paused =
                    globalPause || std::find(covered.begin(), covered.end(),
                                             w->monitor()) != covered.end();
                if (w->Step(dt, mouse, mouseActive, paused)) {
                    anyActive = true;
                }
            }
        } catch (const std::exception& e) {
            // For example a lost GPU device (driver update, TDR): start over.
            Wh_Log(L"Renderer error: %S", e.what());
            renderer.Clear();
            rebuild = true;
            scheduleRetry(GetTickCount64());
            anyActive = false;
        }

        int fps = settings.fps;
        if (powerSaving && settings.onBattery == BatteryMode::HalfFps) {
            fps = std::max(15, fps / 2);
        }
        double intervalMs = anyActive ? 1000.0 / fps : kIdleIntervalMs;
        if (WaitForNextFrame(frameTimer, frameStart, intervalMs, frequency)) {
            break;
        }
    }

    renderer.Clear();
}

DWORD WINAPI RenderThread(void*) {
    // This thread owns all windows, GPU resources and settings.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!RegisterWindowClasses()) {
        SetEvent(g_readyEvent);
        return 1;
    }

    HWND control =
        CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kControlClass,
                        L"Fluid Wallpaper control", WS_POPUP, 0, 0, 0, 0,
                        nullptr, nullptr, g_instance, nullptr);
    std::vector<HPOWERNOTIFY> powerNotifications;
    bool sessionNotify = false;
    if (control) {
        // Each registration immediately reports the current value.
        for (const GUID* id : {&kConsoleDisplayState, &kAcDcPowerSource,
                               &kPowerSavingStatus, &kEnergySaverStatus}) {
            // Older Windows versions don't know every GUID; that's fine.
            if (HPOWERNOTIFY notify = RegisterPowerSettingNotification(
                    control, id, DEVICE_NOTIFY_WINDOW_HANDLE)) {
                powerNotifications.push_back(notify);
            }
        }
        sessionNotify =
            WTSRegisterSessionNotification(control, NOTIFY_FOR_THIS_SESSION);
    } else {
        Wh_Log(
            L"Control window creation failed: %lu; session and power "
            L"detection disabled",
            GetLastError());
    }

    // The desktop may not exist yet (e.g. at logon); the render loop keeps
    // retrying, so startup itself succeeds here.
    g_startupOK = true;
    SetEvent(g_readyEvent);

    HANDLE frameTimer = CreateFrameTimer();
    RunRenderLoop(frameTimer);
    if (frameTimer) {
        CloseHandle(frameTimer);
    }

    if (sessionNotify) {
        WTSUnRegisterSessionNotification(control);
    }
    for (HPOWERNOTIFY notify : powerNotifications) {
        UnregisterPowerSettingNotification(notify);
    }
    if (control) {
        DestroyWindow(control);
    }
    UnregisterWindowClasses();
    return 0;
}

void CloseEvents() {
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_updateEvent) {
        CloseHandle(g_updateEvent);
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
    }
    g_stopEvent = g_updateEvent = g_readyEvent = nullptr;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Fluid Wallpaper initializing");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&RenderThread), &g_instance);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_updateEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_updateEvent || !g_readyEvent) {
        CloseEvents();
        return FALSE;
    }

    g_worker = CreateThread(nullptr, 0, RenderThread, nullptr, 0, nullptr);
    if (!g_worker) {
        CloseEvents();
        return FALSE;
    }

    if (WaitForSingleObject(g_readyEvent, 10000) != WAIT_OBJECT_0 ||
        !g_startupOK) {
        Wh_Log(L"Startup failed");
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
        CloseEvents();
        return FALSE;
    }

    Wh_Log(L"Fluid Wallpaper started");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_updateEvent) {
        SetEvent(g_updateEvent);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Stopping Fluid Wallpaper");
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    CloseEvents();
    Wh_Log(L"Wallpaper windows and GPU resources removed");
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

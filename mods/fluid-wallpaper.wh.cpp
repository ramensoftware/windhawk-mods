// ==WindhawkMod==
// @id              fluid-wallpaper
// @name            Fluid Wallpaper
// @description     Animated GPU fluid simulation wallpaper with autonomous motion, mouse interaction and per-monitor rendering
// @version         1.0.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -lopengl32 -lgdi32 -luser32 -lshell32 -ldwmapi -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluid Wallpaper
![Screenshot](https://i.imgur.com/rIzdlWv.jpeg)
[Watch the overview video in full quality](https://i.imgur.com/yXW45Vs.mp4)

An animated, interactive fluid simulation rendered on the GPU behind your
desktop icons. It is a native OpenGL port of the shaders from
[WebGL Fluid Simulation](https://github.com/PavelDoGreat/WebGL-Fluid-Simulation)
by Pavel Dobryakov (MIT license).

## Features

- One independent simulation per monitor, drawn behind the desktop icons.
- Autonomous color trails and periodic color bursts keep the fluid moving.
- Moving the mouse over the desktop stirs the fluid. No clicks are needed and
  no mouse hooks are installed; clicks always reach the desktop.
- Bloom, sun rays and shading effects, all configurable.
- Pauses automatically when the desktop can't be seen: when a monitor is
  covered by a maximized or full-screen window, when the session is locked and
  when the display is turned off.

To see the wallpaper, minimize your windows. Settings are applied live;
changing a setting restarts the simulation.

## Performance

On Windows 11 24H2 and later the desktop is hosted differently, and frames are
copied from the GPU to a layered window (read back at up to 1536 pixels on the
longest edge and scaled up). This uses noticeably more CPU than the direct
OpenGL path used on older builds.

- Lighter profile: Maximum FPS 30, Dye resolution 512, Autonomous trails 2.
- Swirlier profile: Vorticity 50, Dye dissipation 60, Autonomous trail force
  4500.

## Troubleshooting

If the wallpaper doesn't appear, open the mod log in Windhawk: the lines
starting with "Desktop layout", "OpenGL" and "Layered frame" describe what was
found and created. Please include them when reporting a problem, together with
your Windows build number.

If the animation stays frozen while the desktop is visible, a window on that
monitor is probably reported as maximized while hidden. Disable **Pause when
covered** in that case.

## Requirements and limitations

- A GPU driver with OpenGL 3.0 and floating-point framebuffer support.
- Relies on the undocumented WorkerW desktop layout of Windows, which differs
  between Windows 10, Windows 11 and Windows 11 24H2 and later. Future Windows
  builds may change it.
- May not work over Remote Desktop or with software OpenGL renderers.
- No registry writes and no changes to your wallpaper settings. Disabling the
  mod removes its windows and reveals your normal wallpaper.
- The transparent background and screenshot features of the web demo are not
  included.
- Some settings use integer scales; for example, 80 means 0.8.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fps: 30
  $name: Maximum FPS
  $description: "15-120. Higher values look smoother but use more CPU and GPU. Below 30, the motion also slows down."
- pause: false
  $name: Pause simulation
- pauseWhenCovered: true
  $name: Pause when covered
  $description: "Pause a monitor's simulation while a maximized or full-screen window covers it."
- primaryOnly: false
  $name: Primary monitor only
- mouse: true
  $name: Mouse interaction
  $description: "Moving the mouse stirs the fluid. No clicks needed."
- desktopOnly: true
  $name: Mouse only over the desktop
  $description: "Disable to also react to mouse movement over other windows."
- force: 6000
  $name: Mouse force
  $description: "100-15000"
- radius: 25
  $name: Splat radius ×100
  $description: "1-200. 25 means 0.25, the default of the web demo."
- simResolution: 128
  $name: Simulation resolution
  $description: "64-512"
- dyeResolution: 1024
  $name: Dye resolution
  $description: "256-2048. Lower values reduce GPU memory and load."
- density: 100
  $name: Dye dissipation ×100
  $description: "0-1000. 100 means 1.0. Higher values make colors fade faster."
- velocity: 20
  $name: Velocity dissipation ×100
  $description: "0-1000. 20 means 0.2."
- pressure: 80
  $name: Pressure ×100
  $description: "0-100"
- iterations: 20
  $name: Pressure iterations
  $description: "5-60"
- curl: 30
  $name: Vorticity
  $description: "0-100. Higher values create more swirls."
- shading: true
  $name: Shading
- colorful: true
  $name: Changing colors
  $description: "Cycle through hues. When disabled, the fixed color below is used."
- colorSpeed: 10
  $name: Color change speed
  $description: "0-100"
- red: 30
  $name: "Fixed color: red"
  $description: "0-255"
- green: 160
  $name: "Fixed color: green"
  $description: "0-255"
- blue: 255
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
- autoMotion: true
  $name: Autonomous motion
- emitters: 3
  $name: Autonomous trails
  $description: "1-6"
- autoSpeed: 35
  $name: Autonomous motion speed
  $description: "1-100"
- autoForce: 3000
  $name: Autonomous trail force
  $description: "100-15000"
- autoColor: 55
  $name: Autonomous color amount ×100
  $description: "1-300"
- bursts: true
  $name: Color bursts
- burstSeconds: 12
  $name: Seconds between bursts
  $description: "2-120"
- bloom: true
  $name: Bloom
- bloomResolution: 256
  $name: Bloom resolution
  $description: "64-512"
- bloomIterations: 8
  $name: Bloom levels
  $description: "2-8"
- bloomIntensity: 80
  $name: Bloom intensity ×100
  $description: "0-300"
- bloomThreshold: 60
  $name: Bloom threshold ×100
  $description: "0-300"
- bloomKnee: 70
  $name: Bloom soft knee ×100
  $description: "0-100"
- sunrays: true
  $name: Sun rays
- sunResolution: 196
  $name: Sun rays resolution
  $description: "64-512"
- sunWeight: 100
  $name: Sun rays intensity ×100
  $description: "0-300"
*/
// ==/WindhawkModSettings==

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

#include <dwmapi.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <GL/gl.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Shaders

const char* kBaseVertexShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform vec2 texelSize;

void main () {
    vUv = gl_Vertex.xy * 0.5 + 0.5;
    vL = vUv - vec2(texelSize.x, 0.0);
    vR = vUv + vec2(texelSize.x, 0.0);
    vT = vUv + vec2(0.0, texelSize.y);
    vB = vUv - vec2(0.0, texelSize.y);
    gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
}
)GLSL";

const char* kBlurVertexShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
uniform vec2 texelSize;

void main () {
    vUv = gl_Vertex.xy * 0.5 + 0.5;
    float offset = 1.33333333;
    vL = vUv - texelSize * offset;
    vR = vUv + texelSize * offset;
    gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
}
)GLSL";

const char* kBlurShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
uniform sampler2D uTexture;

void main () {
    vec4 sum = texture2D(uTexture, vUv) * 0.29411764;
    sum += texture2D(uTexture, vL) * 0.35294117;
    sum += texture2D(uTexture, vR) * 0.35294117;
    gl_FragColor = sum;
}
)GLSL";

const char* kClearShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uTexture;
uniform float value;

void main () {
    gl_FragColor = value * texture2D(uTexture, vUv);
}
)GLSL";

const char* kSplatShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uTarget;
uniform float aspectRatio;
uniform vec3 color;
uniform vec2 point;
uniform float radius;

void main () {
    vec2 p = vUv - point.xy;
    p.x *= aspectRatio;
    vec3 splat = exp(-dot(p, p) / radius) * color;
    vec3 base = texture2D(uTarget, vUv).xyz;
    gl_FragColor = vec4(base + splat, 1.0);
}
)GLSL";

const char* kAdvectionShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uVelocity;
uniform sampler2D uSource;
uniform vec2 texelSize;
uniform float dt;
uniform float dissipation;

void main () {
    vec2 coord = vUv - dt * texture2D(uVelocity, vUv).xy * texelSize;
    vec4 result = texture2D(uSource, coord);
    float decay = 1.0 + dissipation * dt;
    gl_FragColor = result / decay;
}
)GLSL";

const char* kDivergenceShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uVelocity;

void main () {
    float L = texture2D(uVelocity, vL).x;
    float R = texture2D(uVelocity, vR).x;
    float T = texture2D(uVelocity, vT).y;
    float B = texture2D(uVelocity, vB).y;

    vec2 C = texture2D(uVelocity, vUv).xy;
    if (vL.x < 0.0) { L = -C.x; }
    if (vR.x > 1.0) { R = -C.x; }
    if (vT.y > 1.0) { T = -C.y; }
    if (vB.y < 0.0) { B = -C.y; }

    float div = 0.5 * (R - L + T - B);
    gl_FragColor = vec4(div, 0.0, 0.0, 1.0);
}
)GLSL";

const char* kCurlShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uVelocity;

void main () {
    float L = texture2D(uVelocity, vL).y;
    float R = texture2D(uVelocity, vR).y;
    float T = texture2D(uVelocity, vT).x;
    float B = texture2D(uVelocity, vB).x;
    float vorticity = R - L - T + B;
    gl_FragColor = vec4(0.5 * vorticity, 0.0, 0.0, 1.0);
}
)GLSL";

const char* kVorticityShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uVelocity;
uniform sampler2D uCurl;
uniform float curl;
uniform float dt;

void main () {
    float L = texture2D(uCurl, vL).x;
    float R = texture2D(uCurl, vR).x;
    float T = texture2D(uCurl, vT).x;
    float B = texture2D(uCurl, vB).x;
    float C = texture2D(uCurl, vUv).x;

    vec2 force = 0.5 * vec2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= curl * C;
    force.y *= -1.0;

    vec2 vel = texture2D(uVelocity, vUv).xy;
    gl_FragColor = vec4(vel + force * dt, 0.0, 1.0);
}
)GLSL";

const char* kPressureShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uPressure;
uniform sampler2D uDivergence;

void main () {
    float L = texture2D(uPressure, vL).x;
    float R = texture2D(uPressure, vR).x;
    float T = texture2D(uPressure, vT).x;
    float B = texture2D(uPressure, vB).x;
    float divergence = texture2D(uDivergence, vUv).x;
    float pressure = (L + R + B + T - divergence) * 0.25;
    gl_FragColor = vec4(pressure, 0.0, 0.0, 1.0);
}
)GLSL";

const char* kGradientSubtractShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uPressure;
uniform sampler2D uVelocity;

void main () {
    float L = texture2D(uPressure, vL).x;
    float R = texture2D(uPressure, vR).x;
    float T = texture2D(uPressure, vT).x;
    float B = texture2D(uPressure, vB).x;
    vec2 velocity = texture2D(uVelocity, vUv).xy;
    velocity.xy -= vec2(R - L, T - B);
    gl_FragColor = vec4(velocity, 0.0, 1.0);
}
)GLSL";

const char* kBloomPrefilterShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uTexture;
uniform vec3 curve;
uniform float threshold;

void main () {
    vec3 c = texture2D(uTexture, vUv).rgb;
    float br = max(c.r, max(c.g, c.b));
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    c *= max(rq, br - threshold) / max(br, 0.0001);
    gl_FragColor = vec4(c, 0.0);
}
)GLSL";

const char* kBloomBlurShader = R"GLSL(
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uTexture;

void main () {
    vec4 sum = vec4(0.0);
    sum += texture2D(uTexture, vL);
    sum += texture2D(uTexture, vR);
    sum += texture2D(uTexture, vT);
    sum += texture2D(uTexture, vB);
    sum *= 0.25;
    gl_FragColor = sum;
}
)GLSL";

const char* kBloomFinalShader = R"GLSL(
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uTexture;
uniform float intensity;

void main () {
    vec4 sum = vec4(0.0);
    sum += texture2D(uTexture, vL);
    sum += texture2D(uTexture, vR);
    sum += texture2D(uTexture, vT);
    sum += texture2D(uTexture, vB);
    sum *= 0.25;
    gl_FragColor = sum * intensity;
}
)GLSL";

const char* kSunraysMaskShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uTexture;

void main () {
    vec4 c = texture2D(uTexture, vUv);
    float br = max(c.r, max(c.g, c.b));
    c.a = 1.0 - min(max(br * 20.0, 0.0), 0.8);
    gl_FragColor = c;
}
)GLSL";

const char* kSunraysShader = R"GLSL(
varying vec2 vUv;
uniform sampler2D uTexture;
uniform float weight;

#define ITERATIONS 16

void main () {
    float Density = 0.3;
    float Decay = 0.95;
    float Exposure = 0.7;

    vec2 coord = vUv;
    vec2 dir = vUv - 0.5;

    dir *= 1.0 / float(ITERATIONS) * Density;
    float illuminationDecay = 1.0;

    float color = texture2D(uTexture, vUv).a;

    for (int i = 0; i < ITERATIONS; i++) {
        coord -= dir;
        float col = texture2D(uTexture, coord).a;
        color += col * illuminationDecay * weight;
        illuminationDecay *= Decay;
    }

    gl_FragColor = vec4(color * Exposure, 0.0, 0.0, 1.0);
}
)GLSL";

const char* kDisplayShader = R"GLSL(
varying vec2 vUv;
varying vec2 vL;
varying vec2 vR;
varying vec2 vT;
varying vec2 vB;
uniform sampler2D uTexture;
uniform sampler2D uBloom;
uniform sampler2D uSunrays;
uniform vec3 background;
uniform vec2 texelSize;

vec3 linearToGamma (vec3 color) {
    color = max(color, vec3(0));
    return max(1.055 * pow(color, vec3(0.416666667)) - 0.055, vec3(0));
}

void main () {
    vec3 c = texture2D(uTexture, vUv).rgb;

#ifdef SHADING
    vec3 lc = texture2D(uTexture, vL).rgb;
    vec3 rc = texture2D(uTexture, vR).rgb;
    vec3 tc = texture2D(uTexture, vT).rgb;
    vec3 bc = texture2D(uTexture, vB).rgb;

    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);

    vec3 n = normalize(vec3(dx, dy, length(texelSize)));
    vec3 l = vec3(0.0, 0.0, 1.0);

    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    c *= diffuse;
#endif

#ifdef BLOOM
    vec3 bloom = texture2D(uBloom, vUv).rgb;
#endif

#ifdef SUNRAYS
    float sunrays = texture2D(uSunrays, vUv).r;
    c *= sunrays;
#ifdef BLOOM
    bloom *= sunrays;
#endif
#endif

#ifdef BLOOM
    // Hash noise replaces the demo's dithering texture.
    float noise = fract(sin(dot(vUv, vec2(12.9898, 78.233))) * 43758.5453);
    noise = noise * 2.0 - 1.0;
    bloom += noise / 255.0;
    bloom = linearToGamma(bloom);
    c += bloom;
#endif

    float a = max(c.r, max(c.g, c.b));
    gl_FragColor = vec4(c + background * (1.0 - clamp(a, 0.0, 1.0)), 1.0);
}
)GLSL";

////////////////////////////////////////////////////////////////////////////////
// OpenGL entry points
//
// Functions beyond the OpenGL 1.1 exports of opengl32.dll. On Windows these
// pointers are only valid for the context they were queried with, so each
// wallpaper keeps its own table (monitors can be driven by different GPUs).

constexpr GLenum kGlFramebuffer = 0x8D40;
constexpr GLenum kGlColorAttachment0 = 0x8CE0;
constexpr GLenum kGlFramebufferComplete = 0x8CD5;
constexpr GLenum kGlRgba16f = 0x881A;
constexpr GLenum kGlRgba8 = 0x8058;
constexpr GLenum kGlTexture0 = 0x84C0;
constexpr GLenum kGlClampToEdge = 0x812F;
constexpr GLenum kGlBgra = 0x80E1;
constexpr GLenum kGlFragmentShader = 0x8B30;
constexpr GLenum kGlVertexShader = 0x8B31;
constexpr GLenum kGlCompileStatus = 0x8B81;
constexpr GLenum kGlLinkStatus = 0x8B82;

#define FLUID_GL_FUNCTIONS(X)                                                \
    X(GLuint, CreateShader, GLenum)                                          \
    X(void, ShaderSource, GLuint, GLsizei, const char* const*, const GLint*) \
    X(void, CompileShader, GLuint)                                           \
    X(void, GetShaderiv, GLuint, GLenum, GLint*)                             \
    X(void, GetShaderInfoLog, GLuint, GLsizei, GLsizei*, char*)              \
    X(void, DeleteShader, GLuint)                                            \
    X(GLuint, CreateProgram)                                                 \
    X(void, AttachShader, GLuint, GLuint)                                    \
    X(void, LinkProgram, GLuint)                                             \
    X(void, GetProgramiv, GLuint, GLenum, GLint*)                            \
    X(void, GetProgramInfoLog, GLuint, GLsizei, GLsizei*, char*)             \
    X(void, DeleteProgram, GLuint)                                           \
    X(void, UseProgram, GLuint)                                              \
    X(GLint, GetUniformLocation, GLuint, const char*)                        \
    X(void, Uniform1i, GLint, GLint)                                         \
    X(void, Uniform1f, GLint, GLfloat)                                       \
    X(void, Uniform2f, GLint, GLfloat, GLfloat)                              \
    X(void, Uniform3f, GLint, GLfloat, GLfloat, GLfloat)                     \
    X(void, ActiveTexture, GLenum)                                           \
    X(void, GenFramebuffers, GLsizei, GLuint*)                               \
    X(void, BindFramebuffer, GLenum, GLuint)                                 \
    X(void, FramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint)     \
    X(GLenum, CheckFramebufferStatus, GLenum)                                \
    X(void, DeleteFramebuffers, GLsizei, const GLuint*)

const char* GLString(GLenum name) {
    auto value = reinterpret_cast<const char*>(glGetString(name));
    return value ? value : "(unknown)";
}

void* GLAddress(const char* name) {
    PROC proc = wglGetProcAddress(name);
    auto value = reinterpret_cast<intptr_t>(proc);
    // Some drivers return small sentinel values instead of nullptr.
    if (!proc || value == 1 || value == 2 || value == 3 || value == -1) {
        proc = GetProcAddress(GetModuleHandleW(L"opengl32.dll"), name);
    }
    return reinterpret_cast<void*>(proc);
}

struct GLApi {
#define X(ret, name, ...) ret(APIENTRY* name)(__VA_ARGS__) = nullptr;
    FLUID_GL_FUNCTIONS(X)
#undef X

    void Load() {
#define X(ret, name, ...)                                             \
    name = reinterpret_cast<decltype(name)>(GLAddress("gl" #name));   \
    if (!name) {                                                      \
        throw std::runtime_error("Missing OpenGL function gl" #name); \
    }
        FLUID_GL_FUNCTIONS(X)
#undef X
    }
};

// Per-context state: entry points, bound program and cached uniform locations.
struct GLContext {
    GLApi api;
    GLuint program = 0;
    // Uniform names are always string literals, so their address is a stable
    // key. Two literals with the same text just produce two valid entries.
    std::map<std::pair<GLuint, const char*>, GLint> uniforms;
};

// The context current on the render thread. Only touched by that thread.
GLContext* g_gl = nullptr;

GLApi& GL() {
    return g_gl->api;
}

struct Target {
    GLuint texture = 0;
    GLuint fbo = 0;
    int w = 0;
    int h = 0;
};

struct DoubleTarget {
    Target read;
    Target write;
    void swap() { std::swap(read, write); }
};

GLuint BuildShader(GLenum type, const std::string& source) {
    GLuint id = GL().CreateShader(type);
    const char* text = source.c_str();
    GL().ShaderSource(id, 1, &text, nullptr);
    GL().CompileShader(id);
    GLint ok = 0;
    GL().GetShaderiv(id, kGlCompileStatus, &ok);
    if (!ok) {
        char log[4096]{};
        GL().GetShaderInfoLog(id, sizeof(log), nullptr, log);
        GL().DeleteShader(id);
        throw std::runtime_error(std::string("Shader compilation failed: ") +
                                 log);
    }
    return id;
}

GLuint BuildProgram(const char* fragment,
                    const std::string& defines,
                    const char* vertex) {
    GLuint vs = 0, fs = 0, program = 0;
    try {
        vs = BuildShader(kGlVertexShader,
                         std::string("#version 120\n") + vertex);
        fs = BuildShader(kGlFragmentShader,
                         std::string("#version 120\n") + defines + fragment);
        program = GL().CreateProgram();
        GL().AttachShader(program, vs);
        GL().AttachShader(program, fs);
        GL().LinkProgram(program);
        GLint ok = 0;
        GL().GetProgramiv(program, kGlLinkStatus, &ok);
        if (!ok) {
            char log[4096]{};
            GL().GetProgramInfoLog(program, sizeof(log), nullptr, log);
            throw std::runtime_error(std::string("Program link failed: ") +
                                     log);
        }
    } catch (...) {
        if (vs) {
            GL().DeleteShader(vs);
        }
        if (fs) {
            GL().DeleteShader(fs);
        }
        if (program) {
            GL().DeleteProgram(program);
        }
        throw;
    }
    GL().DeleteShader(vs);
    GL().DeleteShader(fs);
    return program;
}

void UseProgram(GLuint program) {
    g_gl->program = program;
    GL().UseProgram(program);
}

GLint UniformLocation(const char* name) {
    auto key = std::make_pair(g_gl->program, name);
    auto it = g_gl->uniforms.find(key);
    if (it != g_gl->uniforms.end()) {
        return it->second;
    }
    GLint location = GL().GetUniformLocation(g_gl->program, name);
    g_gl->uniforms.emplace(key, location);
    return location;
}

void SetFloat(const char* name, float v) {
    GL().Uniform1f(UniformLocation(name), v);
}

void SetVec2(const char* name, float x, float y) {
    GL().Uniform2f(UniformLocation(name), x, y);
}

void SetVec3(const char* name, float x, float y, float z) {
    GL().Uniform3f(UniformLocation(name), x, y, z);
}

void SetTexture(const char* name, const Target& t, int unit = 0) {
    GL().ActiveTexture(kGlTexture0 + unit);
    glBindTexture(GL_TEXTURE_2D, t.texture);
    GL().Uniform1i(UniformLocation(name), unit);
}

void SetTexelSize(const Target& t) {
    SetVec2("texelSize", 1.f / t.w, 1.f / t.h);
}

void DrawQuad() {
    glBegin(GL_QUADS);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();
}

void DrawTo(const Target& t) {
    GL().BindFramebuffer(kGlFramebuffer, t.fbo);
    glViewport(0, 0, t.w, t.h);
    DrawQuad();
}

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int fps;
    int simResolution;
    int dyeResolution;
    int pressureIterations;
    int emitters;
    int bloomResolution;
    int bloomLevels;
    int sunraysResolution;
    int burstSeconds;
    float densityDissipation;
    float velocityDissipation;
    float pressure;
    float curl;
    float splatRadius;
    float mouseForce;
    float colorSpeed;
    float autoSpeed;
    float autoForce;
    float autoColor;
    float bloomIntensity;
    float bloomThreshold;
    float bloomKnee;
    float sunraysWeight;
    bool shading;
    bool colorful;
    bool autoMotion;
    bool bursts;
    bool mouse;
    bool desktopOnly;
    bool bloom;
    bool sunrays;
    bool pause;
    bool pauseWhenCovered;
    bool primaryOnly;
    float fixedColor[3];
    float background[3];
};

int IntSetting(const wchar_t* key, int lo, int hi) {
    return std::clamp(Wh_GetIntSetting(key), lo, hi);
}

bool BoolSetting(const wchar_t* key) {
    return Wh_GetIntSetting(key) != 0;
}

Settings ReadSettings() {
    Settings s{};
    s.fps = IntSetting(L"fps", 15, 120);
    s.simResolution = IntSetting(L"simResolution", 64, 512);
    s.dyeResolution = IntSetting(L"dyeResolution", 256, 2048);
    s.pressureIterations = IntSetting(L"iterations", 5, 60);
    s.densityDissipation = IntSetting(L"density", 0, 1000) / 100.f;
    s.velocityDissipation = IntSetting(L"velocity", 0, 1000) / 100.f;
    s.pressure = IntSetting(L"pressure", 0, 100) / 100.f;
    s.curl = float(IntSetting(L"curl", 0, 100));
    // The setting mirrors the demo's SPLAT_RADIUS (25 = 0.25). The demo then
    // divides it by 100 before passing it to the shader, hence 10000 here.
    s.splatRadius = IntSetting(L"radius", 1, 200) / 10000.f;
    s.mouseForce = float(IntSetting(L"force", 100, 15000));
    s.colorSpeed = float(IntSetting(L"colorSpeed", 0, 100));
    s.emitters = IntSetting(L"emitters", 1, 6);
    s.autoSpeed = IntSetting(L"autoSpeed", 1, 100) / 100.f;
    s.autoForce = float(IntSetting(L"autoForce", 100, 15000));
    s.autoColor = IntSetting(L"autoColor", 1, 300) / 100.f;
    s.burstSeconds = IntSetting(L"burstSeconds", 2, 120);
    s.bloomResolution = IntSetting(L"bloomResolution", 64, 512);
    s.bloomLevels = IntSetting(L"bloomIterations", 2, 8);
    s.bloomIntensity = IntSetting(L"bloomIntensity", 0, 300) / 100.f;
    s.bloomThreshold = IntSetting(L"bloomThreshold", 0, 300) / 100.f;
    s.bloomKnee = IntSetting(L"bloomKnee", 0, 100) / 100.f;
    s.sunraysResolution = IntSetting(L"sunResolution", 64, 512);
    s.sunraysWeight = IntSetting(L"sunWeight", 0, 300) / 100.f;

    s.shading = BoolSetting(L"shading");
    s.colorful = BoolSetting(L"colorful");
    s.autoMotion = BoolSetting(L"autoMotion");
    s.bursts = BoolSetting(L"bursts");
    s.mouse = BoolSetting(L"mouse");
    s.desktopOnly = BoolSetting(L"desktopOnly");
    s.bloom = BoolSetting(L"bloom");
    s.sunrays = BoolSetting(L"sunrays");
    s.pause = BoolSetting(L"pause");
    s.pauseWhenCovered = BoolSetting(L"pauseWhenCovered");
    s.primaryOnly = BoolSetting(L"primaryOnly");

    s.fixedColor[0] = IntSetting(L"red", 0, 255) / 255.f;
    s.fixedColor[1] = IntSetting(L"green", 0, 255) / 255.f;
    s.fixedColor[2] = IntSetting(L"blue", 0, 255) / 255.f;
    s.background[0] = IntSetting(L"backgroundRed", 0, 255) / 255.f;
    s.background[1] = IntSetting(L"backgroundGreen", 0, 255) / 255.f;
    s.background[2] = IntSetting(L"backgroundBlue", 0, 255) / 255.f;
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
constexpr wchar_t kLayerClass[] = L"WindhawkFluidWallpaperLayer";
constexpr wchar_t kControlClass[] = L"WindhawkFluidWallpaperControl";

constexpr float kTau = 6.283185f;
constexpr double kIdleIntervalMs = 200;
constexpr int kMaxPresentEdge = 1536;

// {6FE69556-704A-47A0-8F24-C28D936FDA47}
constexpr GUID kConsoleDisplayState = {
    0x6fe69556,
    0x704a,
    0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

// Written by the control window procedure, read by the render loop. Both run
// on the render thread.
bool g_sessionLocked = false;
bool g_sessionDisconnected = false;  // Fast user switching, RDP disconnect.
bool g_displayOff = false;

////////////////////////////////////////////////////////////////////////////////
// Desktop layout

HWND g_wallpaperParent = nullptr;
HWND g_iconHost = nullptr;
HWND g_iconView = nullptr;
bool g_raisedDesktop = false;
HWND g_backgroundWindow = nullptr;
HWND g_savedBackground = nullptr;
HWND g_savedBackgroundPrevious = nullptr;

// On raised desktops, keep the shell's background WorkerW below our window.
void EnsureDesktopOrder() {
    if (!g_raisedDesktop || !g_backgroundWindow ||
        !IsWindow(g_backgroundWindow)) {
        return;
    }
    if (g_savedBackground != g_backgroundWindow) {
        g_savedBackground = g_backgroundWindow;
        g_savedBackgroundPrevious = GetWindow(g_backgroundWindow, GW_HWNDPREV);
    }
    SetWindowPos(g_backgroundWindow, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void RestoreDesktopOrder() {
    bool previousValid =
        !g_savedBackgroundPrevious ||
        (IsWindow(g_savedBackgroundPrevious) &&
         GetParent(g_savedBackgroundPrevious) == GetParent(g_savedBackground));
    if (g_savedBackground && IsWindow(g_savedBackground) && previousValid) {
        SetWindowPos(
            g_savedBackground,
            g_savedBackgroundPrevious ? g_savedBackgroundPrevious : HWND_TOP, 0,
            0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    g_savedBackground = nullptr;
    g_savedBackgroundPrevious = nullptr;
}

BOOL CALLBACK FindIconView(HWND top, LPARAM) {
    HWND icons = FindWindowExW(top, nullptr, L"SHELLDLL_DefView", nullptr);
    if (icons && !g_iconView) {
        g_iconHost = top;
        g_iconView = icons;
    }
    return TRUE;
}

HWND FindWallpaperParent() {
    g_wallpaperParent = nullptr;
    g_iconHost = nullptr;
    g_iconView = nullptr;
    g_raisedDesktop = false;

    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman) {
        return nullptr;
    }

    // Undocumented: ask Progman to create the WorkerW used for animated
    // wallpapers (the same message used by other wallpaper engines).
    DWORD_PTR result = 0;
    SendMessageTimeoutW(progman, 0x052C, 0xD, 0, SMTO_ABORTIFHUNG, 1000,
                        &result);
    SendMessageTimeoutW(progman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000,
                        &result);

    EnumWindows(FindIconView, 0);
    if (!g_iconView) {
        Wh_Log(L"Desktop SHELLDLL_DefView not found");
        return nullptr;
    }

    HWND childWorker = FindWindowExW(g_iconHost, nullptr, L"WorkerW", nullptr);
    g_backgroundWindow = childWorker;
    g_raisedDesktop = (GetWindowLongPtrW(g_iconHost, GWL_EXSTYLE) &
                       WS_EX_NOREDIRECTIONBITMAP) != 0 ||
                      childWorker != nullptr;

    if (g_raisedDesktop) {
        // Windows 11 24H2+: the wallpaper is a sibling below DefView and above
        // the shell's background WorkerW. Do not place it inside that WorkerW.
        g_wallpaperParent = g_iconHost;
    } else {
        g_wallpaperParent =
            FindWindowExW(nullptr, g_iconHost, L"WorkerW", nullptr);
        if (g_wallpaperParent && FindWindowExW(g_wallpaperParent, nullptr,
                                               L"SHELLDLL_DefView", nullptr)) {
            g_wallpaperParent = nullptr;
        }
    }

    RECT client{};
    if (g_wallpaperParent) {
        GetClientRect(g_wallpaperParent, &client);
    }
    Wh_Log(
        L"Desktop layout: raised=%d parent=%p icons=%p backgroundChild=%p "
        L"client=%ldx%ld visible=%d",
        g_raisedDesktop, g_wallpaperParent, g_iconView, childWorker,
        client.right, client.bottom,
        g_wallpaperParent ? IsWindowVisible(g_wallpaperParent) : 0);
    return g_wallpaperParent;
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
            kWindowClass, kLayerClass, kControlClass});
}

bool DesktopAt(POINT p) {
    HWND w = WindowFromPoint(p);
    for (int i = 0; w && i < 8; i++, w = GetParent(w)) {
        if (ClassIsAnyOf(w, {L"SHELLDLL_DefView", L"Progman", L"WorkerW",
                             kWindowClass, kLayerClass})) {
            return true;
        }
    }
    return false;
}

bool IsCloaked(HWND w) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

// Collects monitors hidden behind a maximized or full-screen window. Hidden
// windows, windows on other virtual desktops (cloaked) and click-through
// overlays are ignored.
// Cheap checks first: this runs over every top-level window 4 times a second.
BOOL CALLBACK CollectCoveredMonitor(HWND w, LPARAM param) {
    if (!IsWindowVisible(w) || IsIconic(w)) {
        return TRUE;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(w, GWL_EXSTYLE);
    if (exStyle & WS_EX_TRANSPARENT) {
        return TRUE;
    }
    HMONITOR monitor = MonitorFromWindow(w, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return TRUE;
    }
    bool covers = IsZoomed(w) != FALSE;
    if (!covers && !(exStyle & WS_EX_TOOLWINDOW)) {
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        RECT wr{};
        covers = GetMonitorInfoW(monitor, &mi) && GetWindowRect(w, &wr) &&
                 wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
                 wr.right >= mi.rcMonitor.right &&
                 wr.bottom >= mi.rcMonitor.bottom;
    }
    if (covers && !IsShellOrOwnWindow(w) && !IsCloaked(w)) {
        auto& list = *reinterpret_cast<std::vector<HMONITOR>*>(param);
        if (std::find(list.begin(), list.end(), monitor) == list.end()) {
            list.push_back(monitor);
        }
    }
    return TRUE;
}

std::vector<HMONITOR> CoveredMonitors() {
    std::vector<HMONITOR> list;
    EnumWindows(CollectCoveredMonitor, reinterpret_cast<LPARAM>(&list));
    return list;
}

struct MonitorInfo {
    HMONITOR monitor;
    RECT rect;
    bool primary;
};

BOOL CALLBACK CollectMonitor(HMONITOR h, HDC, LPRECT, LPARAM param) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(h, &mi)) {
        reinterpret_cast<std::vector<MonitorInfo>*>(param)->push_back(
            {h, mi.rcMonitor, (mi.dwFlags & MONITORINFOF_PRIMARY) != 0});
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
// Wallpaper: one window, OpenGL context and simulation per monitor

struct Wallpaper {
    MonitorInfo info{};
    Settings s{};

    HWND window = nullptr;
    HWND renderWindow = nullptr;  // Hidden WGL drawable for layered windows.
    HDC dc = nullptr;
    HGLRC context = nullptr;
    GLContext gl;
    bool layered = false;
    bool loggedFrame = false;
    DWORD lastPresentError = 0;

    // Layered presentation: the frame is rendered into an RGBA8 target, read
    // back at reduced size into frameDC, and scaled into the window by
    // WM_PAINT.
    Target presentation;
    int presentW = 0;
    int presentH = 0;
    HDC frameDC = nullptr;
    HBITMAP frameBitmap = nullptr;
    HGDIOBJ oldFrameBitmap = nullptr;
    void* readback = nullptr;

    std::vector<Target> targets;
    std::vector<GLuint> programs;
    DoubleTarget velocity, dye, pressure;
    Target divergence, curl, bloom, sunMask, sun, sunTemp;
    std::vector<Target> bloomLevels;
    GLuint pSplat = 0, pAdvection = 0, pCurl = 0, pVorticity = 0;
    GLuint pDivergence = 0, pPressure = 0, pGradient = 0, pClear = 0;
    GLuint pBloomPrefilter = 0, pBloomBlur = 0, pBloomFinal = 0;
    GLuint pSunraysMask = 0, pSunrays = 0, pBlur = 0, pDisplay = 0;

    std::mt19937 random{std::random_device{}()};
    float clock = 0;
    float burstClock = 0;
    float phase = 0;
    POINT previousMouse{};
    bool hadMouse = false;
    bool wasPaused = false;

    struct Emitter {
        float x, y;
    };
    std::vector<Emitter> emitters;

    Wallpaper() = default;
    Wallpaper(const Wallpaper&) = delete;
    Wallpaper& operator=(const Wallpaper&) = delete;

    int Width() const { return info.rect.right - info.rect.left; }
    int Height() const { return info.rect.bottom - info.rect.top; }
    float Aspect() const { return float(Width()) / Height(); }
    float Rand() { return std::uniform_real_distribution<float>(0, 1)(random); }

    bool MakeCurrent() {
        if (!wglMakeCurrent(dc, context)) {
            return false;
        }
        g_gl = &gl;
        return true;
    }

    // Called from WM_PAINT of the desktop layered window.
    void Paint(HDC output) const {
        if (!frameDC) {
            return;
        }
        SetStretchBltMode(output, COLORONCOLOR);
        StretchBlt(output, 0, 0, Width(), Height(), frameDC, 0, 0, presentW,
                   presentH, SRCCOPY);
    }

    ////////////////////////////////////////////////////////////////////////
    // Initialization

    void Initialize(const MonitorInfo& mi,
                    const Settings& settings,
                    HWND parent) {
        info = mi;
        s = settings;
        phase = Rand() * kTau;

        CreateWindows(parent);
        CreateContext();
        CreatePrograms();
        CreateTargets();
        if (layered) {
            CreatePresentation();
        }

        for (int i = 0; i < s.emitters; i++) {
            emitters.push_back(EmitterPosition(i, 0));
        }
        for (int i = 0; i < 8; i++) {
            float c[3];
            Color(c, float(i));
            Splat(Rand(), Rand(), (Rand() - .5f) * 1000, (Rand() - .5f) * 1000,
                  c);
        }

        Render();
        ShowWindow(window, SW_SHOWNOACTIVATE);
        EnsureDesktopOrder();
        SetWindowPos(window, layered ? g_iconView : HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        loggedFrame = false;
        Render();
        Wh_Log(L"Wallpaper shown: window=%p layered=%d visible=%d", window,
               layered, IsWindowVisible(window));
    }

    void CreateWindows(HWND parent) {
        layered = g_raisedDesktop;

        POINT origin{info.rect.left, info.rect.top};
        MapWindowPoints(nullptr, parent, &origin, 1);

        DWORD exStyle = WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
        if (layered) {
            exStyle |= WS_EX_LAYERED;
        }
        DWORD style = WS_CHILD;
        if (!layered) {
            style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        }

        window = CreateWindowExW(exStyle, layered ? kLayerClass : kWindowClass,
                                 L"Fluid Wallpaper", style, origin.x, origin.y,
                                 Width(), Height(), parent, nullptr, g_instance,
                                 nullptr);
        if (!window) {
            throw std::runtime_error("Cannot create wallpaper window");
        }
        SetWindowPos(window, layered ? g_iconView : HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        if (layered) {
            // Layered windows cannot use a CS_OWNDC class. Keep WGL on a
            // separate hidden drawable and transfer only the finished frame.
            renderWindow = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass,
                L"Fluid Wallpaper GPU renderer", WS_POPUP, 0, 0, 16, 16,
                nullptr, nullptr, g_instance, nullptr);
            if (!renderWindow) {
                throw std::runtime_error("Cannot create hidden GPU drawable");
            }
            // A layered child at constant opacity uses redirected painting,
            // so WM_PAINT can draw into it like a normal window.
            if (!SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA)) {
                throw std::runtime_error(
                    "Fixed-opacity layered window setup failed");
            }
        }
    }

    void CreateContext() {
        dc = GetDC(layered ? renderWindow : window);
        if (!dc) {
            throw std::runtime_error("GetDC failed");
        }

        PIXELFORMATDESCRIPTOR pfd{};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags =
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;
        int format = ChoosePixelFormat(dc, &pfd);
        if (!format || !SetPixelFormat(dc, format, &pfd)) {
            throw std::runtime_error("OpenGL pixel format unavailable");
        }

        context = wglCreateContext(dc);
        if (!context || !MakeCurrent()) {
            throw std::runtime_error("OpenGL context unavailable");
        }
        gl.api.Load();

        Wh_Log(L"OpenGL: %S / %S; monitor %ld,%ld %dx%d", GLString(GL_VERSION),
               GLString(GL_RENDERER), info.rect.left, info.rect.top, Width(),
               Height());

        GLint maxTexture = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexture);
        if (maxTexture < 2048) {
            throw std::runtime_error("GPU texture size limit too low");
        }
        // Keep aspect-ratio-scaled allocations within the hardware limit.
        float aspect = std::max(Aspect(), 1.f / Aspect());
        s.dyeResolution = std::min(s.dyeResolution, int(maxTexture / aspect));

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Frame pacing is done by the render loop, not by vsync.
        using SwapInterval_t = BOOL(WINAPI*)(int);
        if (auto swapInterval = reinterpret_cast<SwapInterval_t>(
                GLAddress("wglSwapIntervalEXT"))) {
            swapInterval(0);
        }
    }

    GLuint MakeProgram(const char* fragment,
                       const std::string& defines = "",
                       const char* vertex = kBaseVertexShader) {
        GLuint p = BuildProgram(fragment, defines, vertex);
        programs.push_back(p);
        return p;
    }

    void CreatePrograms() {
        pSplat = MakeProgram(kSplatShader);
        pAdvection = MakeProgram(kAdvectionShader);
        pCurl = MakeProgram(kCurlShader);
        pVorticity = MakeProgram(kVorticityShader);
        pDivergence = MakeProgram(kDivergenceShader);
        pPressure = MakeProgram(kPressureShader);
        pGradient = MakeProgram(kGradientSubtractShader);
        pClear = MakeProgram(kClearShader);
        pBloomPrefilter = MakeProgram(kBloomPrefilterShader);
        pBloomBlur = MakeProgram(kBloomBlurShader);
        pBloomFinal = MakeProgram(kBloomFinalShader);
        pSunraysMask = MakeProgram(kSunraysMaskShader);
        pSunrays = MakeProgram(kSunraysShader);
        pBlur = MakeProgram(kBlurShader, "", kBlurVertexShader);

        std::string defines;
        if (s.shading) {
            defines += "#define SHADING\n";
        }
        if (s.bloom) {
            defines += "#define BLOOM\n";
        }
        if (s.sunrays) {
            defines += "#define SUNRAYS\n";
        }
        pDisplay = MakeProgram(kDisplayShader, defines);
    }

    // Simulation targets are RGBA16F. The presentation target is RGBA8: it
    // halves readback bandwidth and BGRA8 readback is a driver fast path.
    Target MakeTarget(int w,
                      int h,
                      GLenum internalFormat = kGlRgba16f,
                      GLenum type = GL_FLOAT) {
        Target t{};
        t.w = w;
        t.h = h;
        glGenTextures(1, &t.texture);
        GL().GenFramebuffers(1, &t.fbo);
        targets.push_back(t);

        GL().ActiveTexture(kGlTexture0);
        glBindTexture(GL_TEXTURE_2D, t.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, kGlClampToEdge);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, kGlClampToEdge);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, GL_RGBA, type,
                     nullptr);

        GL().BindFramebuffer(kGlFramebuffer, t.fbo);
        GL().FramebufferTexture2D(kGlFramebuffer, kGlColorAttachment0,
                                  GL_TEXTURE_2D, t.texture, 0);
        if (GL().CheckFramebufferStatus(kGlFramebuffer) !=
            kGlFramebufferComplete) {
            throw std::runtime_error(
                "Framebuffer format unsupported or GPU memory exhausted");
        }
        glViewport(0, 0, w, h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        return t;
    }

    // A target whose shorter side is `base`, matching the monitor's aspect.
    Target MakeScaledTarget(int base) {
        float a = Aspect();
        if (a >= 1) {
            return MakeTarget(int(std::round(base * a)), base);
        }
        return MakeTarget(base, int(std::round(base / a)));
    }

    DoubleTarget MakeScaledPair(int base) {
        return {MakeScaledTarget(base), MakeScaledTarget(base)};
    }

    void CreateTargets() {
        velocity = MakeScaledPair(s.simResolution);
        dye = MakeScaledPair(s.dyeResolution);
        pressure = MakeScaledPair(s.simResolution);
        divergence = MakeScaledTarget(s.simResolution);
        curl = MakeScaledTarget(s.simResolution);

        if (s.bloom) {
            bloom = MakeScaledTarget(s.bloomResolution);
            int w = bloom.w, h = bloom.h;
            for (int i = 0; i < s.bloomLevels; i++) {
                w /= 2;
                h /= 2;
                if (w < 2 || h < 2) {
                    break;
                }
                bloomLevels.push_back(MakeTarget(w, h));
            }
        }
        if (s.sunrays) {
            sunMask = MakeScaledTarget(s.sunraysResolution);
            sun = MakeScaledTarget(s.sunraysResolution);
            sunTemp = MakeScaledTarget(s.sunraysResolution);
        }
    }

    HBITMAP CreateFrameBitmap(int w, int h, void** pixels) {
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = h;  // Bottom-up, like glReadPixels.
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        return CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, pixels, nullptr,
                                0);
    }

    void CreatePresentation() {
        float scale =
            std::min(1.f, float(kMaxPresentEdge) / std::max(Width(), Height()));
        presentW = std::max(1, int(std::round(Width() * scale)));
        presentH = std::max(1, int(std::round(Height() * scale)));
        presentation =
            MakeTarget(presentW, presentH, kGlRgba8, GL_UNSIGNED_BYTE);

        frameDC = CreateCompatibleDC(nullptr);
        frameBitmap = CreateFrameBitmap(presentW, presentH, &readback);
        if (!frameDC || !frameBitmap || !readback) {
            throw std::runtime_error("Presentation buffer allocation failed");
        }
        oldFrameBitmap = SelectObject(frameDC, frameBitmap);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(this));
        Wh_Log(L"Layered presentation: %dx%d readback -> %dx%d, window=%p",
               presentW, presentH, Width(), Height(), window);
    }

    ////////////////////////////////////////////////////////////////////////
    // Simulation

    Emitter EmitterPosition(int i, float time) const {
        float p = phase + i * 2.39996f;  // Golden angle spreads the trails.
        float t = time * s.autoSpeed;
        return {.5f + .38f * std::sin(t * (.63f + i * .09f) + p) *
                          std::cos(t * .23f + p * .7f),
                .5f + .38f * std::cos(t * (.49f + i * .07f) + p) *
                          std::sin(t * .31f + p)};
    }

    void Color(float out[3], float offset = 0) const {
        if (!s.colorful) {
            for (int i = 0; i < 3; i++) {
                out[i] = s.fixedColor[i] * .15f;
            }
            return;
        }
        // HSV hue cycle with saturation and value 1, scaled by 0.15 like the
        // demo's generateColor().
        float h = std::fmod(
            phase / kTau + clock * s.colorSpeed * .025f + offset * .17f, 1.f);
        for (int i = 0; i < 3; i++) {
            float x = std::fmod(h * 6.f + (i == 0   ? 0.f
                                           : i == 1 ? 4.f
                                                    : 2.f),
                                6.f);
            out[i] = .15f * std::clamp(std::abs(x - 3.f) - 1.f, 0.f, 1.f);
        }
    }

    void Splat(float x, float y, float dx, float dy, const float c[3]) {
        UseProgram(pSplat);
        SetTexture("uTarget", velocity.read);
        SetFloat("aspectRatio", Aspect());
        SetVec2("point", x, y);
        SetFloat("radius", s.splatRadius * std::max(1.f, Aspect()));
        SetVec3("color", dx, dy, 0);
        DrawTo(velocity.write);
        velocity.swap();

        SetTexture("uTarget", dye.read);
        SetVec3("color", c[0], c[1], c[2]);
        DrawTo(dye.write);
        dye.swap();
    }

    // Returns true if a frame was simulated and presented.
    bool Step(float dt, POINT mouse, bool mouseActive, bool paused) {
        if (paused) {
            hadMouse = false;
            wasPaused = true;
            return false;
        }
        if (!MakeCurrent()) {
            throw std::runtime_error("wglMakeCurrent failed");
        }
        if (wasPaused) {
            dt = std::min(dt, 1.f / 60);
            wasPaused = false;
        }
        clock += dt;
        burstClock += dt;

        glDisable(GL_BLEND);
        Emit(dt);
        ApplyMouse(mouse, mouseActive);
        Simulate(dt);
        Render();
        return true;
    }

    void Emit(float dt) {
        if (!s.autoMotion) {
            return;
        }
        for (int i = 0; i < s.emitters; i++) {
            Emitter next = EmitterPosition(i, clock);
            Emitter old = emitters[i];
            float c[3];
            Color(c, float(i));
            float amount = s.autoColor * dt * 60.f;
            for (float& v : c) {
                v *= amount;
            }
            // Momentum comes from the path delta, so it is frame-rate
            // independent.
            Splat(next.x, next.y, (next.x - old.x) * s.autoForce,
                  (next.y - old.y) * s.autoForce, c);
            emitters[i] = next;
        }
        if (s.bursts && burstClock >= s.burstSeconds) {
            burstClock = 0;
            for (int i = 0; i < 4; i++) {
                float c[3];
                Color(c, float(i) + Rand());
                for (float& v : c) {
                    v *= 4;
                }
                Splat(Rand(), Rand(), (Rand() - .5f) * s.autoForce,
                      (Rand() - .5f) * s.autoForce, c);
            }
        }
    }

    void ApplyMouse(POINT mouse, bool mouseActive) {
        if (!mouseActive || !PtInRect(&info.rect, mouse)) {
            hadMouse = false;
            previousMouse = mouse;
            return;
        }
        bool moved = mouse.x != previousMouse.x || mouse.y != previousMouse.y;
        if (hadMouse && moved && PtInRect(&info.rect, previousMouse)) {
            float w = float(Width()), h = float(Height());
            float dx = std::clamp((mouse.x - previousMouse.x) / w, -.15f, .15f);
            float dy = std::clamp((previousMouse.y - mouse.y) / h, -.15f, .15f);
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
            Splat((mouse.x - info.rect.left) / w,
                  1.f - (mouse.y - info.rect.top) / h, dx * s.mouseForce,
                  dy * s.mouseForce, c);
        }
        hadMouse = true;
        previousMouse = mouse;
    }

    void Simulate(float dt) {
        UseProgram(pCurl);
        SetTexelSize(velocity.read);
        SetTexture("uVelocity", velocity.read);
        DrawTo(curl);

        UseProgram(pVorticity);
        SetTexelSize(velocity.read);
        SetTexture("uVelocity", velocity.read);
        SetTexture("uCurl", curl, 1);
        SetFloat("curl", s.curl);
        SetFloat("dt", dt);
        DrawTo(velocity.write);
        velocity.swap();

        UseProgram(pDivergence);
        SetTexelSize(velocity.read);
        SetTexture("uVelocity", velocity.read);
        DrawTo(divergence);

        UseProgram(pClear);
        SetTexture("uTexture", pressure.read);
        SetFloat("value", s.pressure);
        DrawTo(pressure.write);
        pressure.swap();

        UseProgram(pPressure);
        SetTexelSize(velocity.read);
        SetTexture("uDivergence", divergence);
        for (int i = 0; i < s.pressureIterations; i++) {
            SetTexture("uPressure", pressure.read, 1);
            DrawTo(pressure.write);
            pressure.swap();
        }

        UseProgram(pGradient);
        SetTexelSize(velocity.read);
        SetTexture("uPressure", pressure.read);
        SetTexture("uVelocity", velocity.read, 1);
        DrawTo(velocity.write);
        velocity.swap();

        UseProgram(pAdvection);
        SetTexelSize(velocity.read);
        SetTexture("uVelocity", velocity.read);
        SetTexture("uSource", velocity.read);
        SetFloat("dt", dt);
        SetFloat("dissipation", s.velocityDissipation);
        DrawTo(velocity.write);
        velocity.swap();

        SetTexture("uVelocity", velocity.read);
        SetTexture("uSource", dye.read, 1);
        SetFloat("dissipation", s.densityDissipation);
        DrawTo(dye.write);
        dye.swap();
    }

    ////////////////////////////////////////////////////////////////////////
    // Rendering

    void ApplyBloom() {
        UseProgram(pBloomPrefilter);
        SetTexture("uTexture", dye.read);
        float knee = s.bloomThreshold * s.bloomKnee + .0001f;
        SetVec3("curve", s.bloomThreshold - knee, knee * 2, .25f / knee);
        SetFloat("threshold", s.bloomThreshold);
        DrawTo(bloom);

        UseProgram(pBloomBlur);
        Target last = bloom;
        for (const Target& level : bloomLevels) {
            SetTexelSize(last);
            SetTexture("uTexture", last);
            DrawTo(level);
            last = level;
        }
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
        for (int i = int(bloomLevels.size()) - 2; i >= 0; i--) {
            SetTexelSize(last);
            SetTexture("uTexture", last);
            DrawTo(bloomLevels[i]);
            last = bloomLevels[i];
        }
        glDisable(GL_BLEND);

        UseProgram(pBloomFinal);
        SetTexelSize(last);
        SetTexture("uTexture", last);
        SetFloat("intensity", s.bloomIntensity);
        DrawTo(bloom);
    }

    void ApplySunrays() {
        UseProgram(pSunraysMask);
        SetTexture("uTexture", dye.read);
        DrawTo(sunMask);

        UseProgram(pSunrays);
        SetTexture("uTexture", sunMask);
        SetFloat("weight", s.sunraysWeight);
        DrawTo(sun);

        UseProgram(pBlur);
        SetVec2("texelSize", 1.f / sun.w, 0);
        SetTexture("uTexture", sun);
        DrawTo(sunTemp);
        SetVec2("texelSize", 0, 1.f / sun.h);
        SetTexture("uTexture", sunTemp);
        DrawTo(sun);
    }

    void Render() {
        glDisable(GL_BLEND);
        if (s.bloom && bloomLevels.size() >= 2) {
            ApplyBloom();
        }
        if (s.sunrays) {
            ApplySunrays();
        }

        int outputW = layered ? presentW : Width();
        int outputH = layered ? presentH : Height();
        UseProgram(pDisplay);
        SetVec2("texelSize", 1.f / outputW, 1.f / outputH);
        SetTexture("uTexture", dye.read);
        if (s.bloom) {
            SetTexture("uBloom", bloom, 1);
        }
        if (s.sunrays) {
            SetTexture("uSunrays", sun, 2);
        }
        SetVec3("background", s.background[0], s.background[1],
                s.background[2]);

        if (layered) {
            PresentLayered();
        } else {
            PresentWgl();
        }
    }

    void PresentWgl() {
        GL().BindFramebuffer(kGlFramebuffer, 0);
        glViewport(0, 0, Width(), Height());
        DrawQuad();
        BOOL ok = SwapBuffers(dc);
        if (!loggedFrame) {
            Wh_Log(L"WGL frame: swap=%d GL=0x%X window=%p", ok, glGetError(),
                   window);
            loggedFrame = true;
        }
    }

    void PresentLayered() {
        DrawTo(presentation);
        GdiFlush();  // Finish earlier GDI reads before overwriting the DIB.
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0, 0, presentW, presentH, kGlBgra, GL_UNSIGNED_BYTE,
                     readback);

        // WM_PAINT scales the readback straight into the redirection surface:
        // a single copy per frame.
        BOOL ok = RedrawWindow(window, nullptr, nullptr,
                               RDW_INVALIDATE | RDW_UPDATENOW);

        DWORD error = ok ? 0 : GetLastError();
        if (!loggedFrame || error != lastPresentError) {
            LogLayeredFrame(ok, error);
            loggedFrame = true;
            lastPresentError = error;
        }
    }

    void LogLayeredFrame(BOOL ok, DWORD error) {
        unsigned maxValue = 0;
        uint64_t sum = 0;
        unsigned samples = 0;
        auto pixels = static_cast<const unsigned char*>(readback);
        for (int i = 0; i < presentW * presentH; i += 97) {
            for (int c = 0; c < 3; c++) {
                unsigned value = pixels[4 * i + c];
                maxValue = std::max(maxValue, value);
                sum += value;
                samples++;
            }
        }
        RECT actual{};
        GetWindowRect(window, &actual);
        Wh_Log(
            L"Layered frame: ok=%d error=%lu GL=0x%X mean=%.2f max=%u "
            L"rect=%ld,%ld %ldx%ld visible=%d prev=%p next=%p",
            ok, error, glGetError(), samples ? double(sum) / samples : 0.,
            maxValue, actual.left, actual.top, actual.right - actual.left,
            actual.bottom - actual.top, IsWindowVisible(window),
            GetWindow(window, GW_HWNDPREV), GetWindow(window, GW_HWNDNEXT));
    }

    ////////////////////////////////////////////////////////////////////////
    // Cleanup

    ~Wallpaper() {
        if (window && IsWindow(window)) {
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            ShowWindow(window, SW_HIDE);
        }
        if (context && MakeCurrent()) {
            if (gl.api.DeleteProgram) {
                for (GLuint p : programs) {
                    gl.api.DeleteProgram(p);
                }
            }
            for (const Target& t : targets) {
                if (gl.api.DeleteFramebuffers && t.fbo) {
                    gl.api.DeleteFramebuffers(1, &t.fbo);
                }
                if (t.texture) {
                    glDeleteTextures(1, &t.texture);
                }
            }
            wglMakeCurrent(nullptr, nullptr);
        }
        if (g_gl == &gl) {
            g_gl = nullptr;
        }
        if (context) {
            wglDeleteContext(context);
        }
        if (dc) {
            ReleaseDC(layered ? renderWindow : window, dc);
        }
        if (frameDC && oldFrameBitmap) {
            SelectObject(frameDC, oldFrameBitmap);
        }
        if (frameBitmap) {
            DeleteObject(frameBitmap);
        }
        if (frameDC) {
            DeleteDC(frameDC);
        }
        if (renderWindow) {
            DestroyWindow(renderWindow);
        }
        if (window && IsWindow(window)) {
            DestroyWindow(window);
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
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC output = BeginPaint(w, &ps);
            auto wallpaper = reinterpret_cast<const Wallpaper*>(
                GetWindowLongPtrW(w, GWLP_USERDATA));
            if (wallpaper) {
                wallpaper->Paint(output);
            }
            EndPaint(w, &ps);
            return 0;
        }
        case WM_CLOSE:
            return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

// Hidden window that receives session lock and display power notifications.
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
        if (setting &&
            IsEqualGUID(setting->PowerSetting, kConsoleDisplayState) &&
            setting->DataLength >= sizeof(DWORD)) {
            // 0 = off, 1 = on, 2 = dimmed.
            g_displayOff = *reinterpret_cast<const DWORD*>(setting->Data) == 0;
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
    cls.style = CS_OWNDC;
    cls.lpfnWndProc = WallpaperWindowProc;
    cls.hInstance = g_instance;
    cls.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&cls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        return false;
    }

    // Layered windows cannot use CS_OWNDC.
    WNDCLASSEXW layerCls = cls;
    layerCls.style = 0;
    layerCls.lpszClassName = kLayerClass;
    if (!RegisterClassExW(&layerCls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        UnregisterClassW(kWindowClass, g_instance);
        return false;
    }

    WNDCLASSEXW controlCls{};
    controlCls.cbSize = sizeof(controlCls);
    controlCls.lpfnWndProc = ControlWindowProc;
    controlCls.hInstance = g_instance;
    controlCls.lpszClassName = kControlClass;
    if (!RegisterClassExW(&controlCls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        UnregisterClassW(kLayerClass, g_instance);
        UnregisterClassW(kWindowClass, g_instance);
        return false;
    }
    return true;
}

void UnregisterWindowClasses() {
    UnregisterClassW(kControlClass, g_instance);
    UnregisterClassW(kLayerClass, g_instance);
    UnregisterClassW(kWindowClass, g_instance);
}

////////////////////////////////////////////////////////////////////////////////
// Render thread

void PumpMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

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
            timeout = timeout + 1000;  // Safety net; the timer fires first.
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

void RunRenderLoop(HANDLE frameTimer) {
    std::vector<std::unique_ptr<Wallpaper>> wallpapers;
    std::vector<MonitorInfo> monitors;
    std::vector<HMONITOR> covered;
    Settings settings = ReadSettings();
    HWND parent = nullptr;

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
        float dt =
            std::clamp(float(double(frameStart.QuadPart - last.QuadPart) /
                             frequency.QuadPart),
                       .001f, 1.f / 30);
        last = frameStart;
        bool anyActive = false;

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
                if (!wallpapers.empty()) {
                    EnsureDesktopOrder();
                }
                if (!SameMonitors(monitors, Monitors(settings.primaryOnly))) {
                    rebuild = true;
                }
                // Also covers Explorer restarts: the parent window disappears.
                if (!parent || !IsWindow(parent) || wallpapers.empty()) {
                    rebuild = true;
                }
                for (auto& w : wallpapers) {
                    if (!IsWindow(w->window)) {
                        rebuild = true;
                    }
                }
            }

            if (rebuild && now >= nextRebuild) {
                rebuild = false;
                wallpapers.clear();
                RestoreDesktopOrder();
                monitors = Monitors(settings.primaryOnly);
                parent = FindWallpaperParent();
                if (parent) {
                    EnsureDesktopOrder();
                    for (const auto& mi : monitors) {
                        auto w = std::make_unique<Wallpaper>();
                        try {
                            w->Initialize(mi, settings, parent);
                            wallpapers.push_back(std::move(w));
                        } catch (const std::exception& e) {
                            Wh_Log(L"Monitor initialization failed: %S",
                                   e.what());
                        }
                    }
                } else {
                    Wh_Log(L"Desktop window not found (Explorer not ready?)");
                }
                if (wallpapers.empty()) {
                    scheduleRetry(now);
                } else {
                    failures = 0;
                    nextRebuild = 0;
                }
                QueryPerformanceCounter(&last);
                dt = 1.f / settings.fps;
            }

            if (now >= nextCoverCheck) {
                nextCoverCheck = now + 250;
                covered.clear();
                if (settings.pauseWhenCovered) {
                    covered = CoveredMonitors();
                }
            }

            bool globalPause = settings.pause || g_sessionLocked ||
                               g_sessionDisconnected || g_displayOff;

            POINT mouse{LONG_MIN, LONG_MIN};
            bool mouseActive = false;
            if (!globalPause && settings.mouse && GetCursorPos(&mouse)) {
                mouseActive = !settings.desktopOnly || DesktopAt(mouse);
            }

            for (auto& w : wallpapers) {
                bool paused =
                    globalPause || std::find(covered.begin(), covered.end(),
                                             w->info.monitor) != covered.end();
                if (w->Step(dt, mouse, mouseActive, paused)) {
                    anyActive = true;
                }
            }
        } catch (const std::exception& e) {
            // For example a lost GPU context: start over after a delay.
            Wh_Log(L"Renderer error: %S", e.what());
            wallpapers.clear();
            RestoreDesktopOrder();
            rebuild = true;
            scheduleRetry(GetTickCount64());
        }

        double intervalMs = anyActive ? 1000.0 / settings.fps : kIdleIntervalMs;
        if (WaitForNextFrame(frameTimer, frameStart, intervalMs, frequency)) {
            break;
        }
    }

    wallpapers.clear();
    RestoreDesktopOrder();
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
    HPOWERNOTIFY powerNotify = nullptr;
    bool sessionNotify = false;
    if (control) {
        powerNotify = RegisterPowerSettingNotification(
            control, &kConsoleDisplayState, DEVICE_NOTIFY_WINDOW_HANDLE);
        sessionNotify =
            WTSRegisterSessionNotification(control, NOTIFY_FOR_THIS_SESSION);
    } else {
        Wh_Log(
            L"Control window creation failed: %lu; lock and display-off "
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
    if (powerNotify) {
        UnregisterPowerSettingNotification(powerNotify);
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

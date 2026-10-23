// ==WindhawkMod==
// @id              realtime-cast-shadow-wallpaper
// @name            Real-time Cast Shadow Wallpaper
// @description     A light moves over your desktop and every open window casts a soft, real-time shadow on the wallpaper
// @version         1.0.3
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -lopengl32 -lgdi32 -luser32 -lshell32 -ldwmapi -lgdiplus -lole32 -luuid -lwtsapi32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Real-time Cast Shadow Wallpaper

Turns the desktop into a lit surface: a light source moves over your wallpaper
and every visible window casts a projected shadow on it, updated in real time
as you move, resize and drag windows.

![Preview](https://i.imgur.com/hfuF3Na.gif)

[Watch the full-quality video](https://i.imgur.com/5f4m5cO.mp4)

## Features
- **Light motion**: follow the mouse, wander randomly, orbit the screen, sweep
  across it like the sun, follow the real time of day, or stay fixed.
- **Light presets**: warm lamp, daylight, flickering candle, moonlight, neon,
  or fully custom color, intensity, radius and height.
- **Shadow presets**: crisp, soft, diffuse, long (dramatic), subtle, or custom
  opacity, softness and length.
- **Anchored shadows**: the shadow stays attached to the window base and
  stretches away from the light, with a sharp contact edge and a soft tail.
- **Contact shadow**: a soft darkening right around each window that grounds
  it on the desktop, independent of the light.
- **Shadow color**: neutral, cool, warm or violet shadows.
- **Shadows on other windows** (optional): windows also shade the
  applications below them, following the real window stacking order.
- **Background**: your current Windows wallpaper (per monitor, slideshows
  included), a custom image, or a plain dark surface; optional vignette.
- Multi-monitor support, pause on fullscreen apps, smooth tracking while
  dragging windows.

## Light motion
| Motion | Behavior |
|---|---|
| Follow the mouse | The light follows the cursor across all monitors. |
| Wander | Smooth random paths, independent on each monitor. |
| Orbit | A slow elliptical orbit around the center of each monitor. |
| Sun arc | Rises on the left, crosses the top, sets on the right, then repeats. Shadows are long and the light warm near the horizon. |
| Time of day | Like the sun arc, but driven by your clock: sunrise at 6:00, noon at the top, sunset at 20:00, then a dim moonlight arc at night. |
| Fixed point | Pinned to a position in % of the monitor; values below 0 or above 100 place it off-screen for long, raking shadows. |

"Speed" applies to wander, orbit and sun arc.

## Presets
Choosing a light or shadow preset overrides the matching custom values; pick
"Custom" to use your own. Shadow presets express length relative to the light
height, so they look consistent with every light preset.

## Notes
- Windows are approximated as rounded rectangles with a common virtual height:
  shadows don't follow transparency or irregular window shapes.
- Shadows are drawn on the wallpaper behind the desktop icons. Shadows on
  other windows use a click-through overlay that never covers the window
  casting them; the taskbar, menus and tooltips are left untouched. This
  option adds some CPU/GPU load.
- Rendering uses OpenGL (GLSL 1.20). On current Windows 11 desktops, frames
  are only redrawn when the light, the windows or the background change, so a
  still desktop costs almost nothing (the flickering candle redraws
  continuously). On older desktop layouts every frame is drawn, up to the FPS
  limit.
- Rendering pauses while the session is locked, the display is off, or every
  monitor shows a fullscreen app. If the desktop isn't ready yet (at sign-in,
  or while Explorer restarts), the mod waits and attaches when it appears.
- The Windows wallpaper setting is never modified, Explorer's own windows are
  never reordered, and nothing is written to the registry. Disabling the mod
  removes everything it created.
- Disable other animated-wallpaper mods while using this one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- light:
  - motion: mouse
    $name: Motion
    $options:
    - mouse: Follow the mouse
    - wander: Wander
    - orbit: Orbit
    - sunArc: Sun arc
    - clock: Time of day
    - fixed: Fixed point
  - preset: warmLamp
    $name: Light preset
    $description: Overrides the custom color, intensity, radius and height below
    $options:
    - warmLamp: Warm lamp
    - daylight: Daylight
    - candle: Candle (flickering)
    - moonlight: Moonlight
    - neon: Neon
    - custom: Custom
  - speed: 10
    $name: Speed (1-100)
    $description: For wander, orbit and sun arc
  - fixedX: 50
    $name: Fixed point X (% of monitor width)
    $description: -100 to 200; outside 0-100 the light is off-screen
  - fixedY: 25
    $name: Fixed point Y (% of monitor height)
    $description: -100 to 200; outside 0-100 the light is off-screen
  - fixedScope: primary
    $name: Fixed point placement
    $options:
    - primary: One light on the primary monitor, shared by all monitors
    - each: A separate light on each monitor
  - showSource: false
    $name: Show the light point
  - ambient: 25
    $name: Ambient light (0-100)
    $description: Brightness of the surface outside the light
  - intensity: 120
    $name: "Custom: intensity (×100)"
  - radius: 1000
    $name: "Custom: radius in pixels"
  - height: 900
    $name: "Custom: height above the desktop (virtual pixels)"
    $description: Lower lights cast longer shadows
  - red: 255
    $name: "Custom: red (0-255)"
  - green: 225
    $name: "Custom: green (0-255)"
  - blue: 185
    $name: "Custom: blue (0-255)"
  $name: Light
- shadows:
  - preset: custom
    $name: Shadow preset
    $description: Overrides the custom opacity, softness and window height below; Long always uses anchored shadows
    $options:
    - crisp: Crisp
    - soft: Soft
    - diffuse: Diffuse
    - long: Long (dramatic)
    - subtle: Subtle
    - custom: Custom
  - anchored: false
    $name: Anchored shadows
    $description: Keep the shadow attached to the window base, stretching away from the light
  - contact: 20
    $name: Contact shadow (0-100)
    $description: Soft darkening right around each window, independent of the light
  - color: neutral
    $name: Shadow color
    $options:
    - neutral: Neutral
    - cool: Cool (blue)
    - warm: Warm (brown)
    - violet: Violet
  - cornerRadius: 12
    $name: Window corner radius in pixels
  - onWindows: false
    $name: Cast shadows onto windows below
    $description: Uses a click-through overlay; adds some CPU/GPU load
  - onWindowsStrength: 60
    $name: Shadow strength on windows (0-100)
  - opacity: 85
    $name: "Custom: opacity (0-100)"
  - softness: 80
    $name: "Custom: softness in pixels"
  - windowHeight: 120
    $name: "Custom: window height above the desktop (virtual pixels)"
    $description: Higher windows cast longer shadows; kept at least 80 below the light
  $name: Shadows
- background:
  - mode: solid
    $name: Background
    $options:
    - windows: Current Windows wallpaper
    - custom: Custom image
    - solid: Plain surface
  - path: ""
    $name: Custom image path
    $description: Full path to a JPG, PNG or BMP file; environment variables are expanded
  - fit: fill
    $name: Custom image fit
    $options:
    - fill: Fill
    - fit: Fit
    - stretch: Stretch
    - center: Center
    - tile: Tile
  - brightness: 85
    $name: Image brightness (0-150)
  - vignette: 0
    $name: Vignette (0-100)
    $description: Darkens the screen edges
  - red: 35
    $name: "Plain surface: red (0-255)"
  - green: 39
    $name: "Plain surface: green (0-255)"
  - blue: 48
    $name: "Plain surface: blue (0-255)"
  $name: Background
- performance:
  - fps: 60
    $name: Maximum frames per second (15-120)
  - quality: native
    $name: Render quality
    $description: Native draws at the monitor's full resolution; lower levels render smaller and upscale smoothly, using less GPU
    $options:
    - native: Native (sharpest)
    - high: High (up to 2560 px)
    - balanced: Balanced (up to 1920 px)
    - fast: Fast (up to 1280 px)
  - maxWindows: 48
    $name: Maximum number of windows (8-64)
  - pauseFullscreen: true
    $name: Pause on monitors running a fullscreen app
  - smoothDrag: true
    $name: Smooth shadows while dragging
    $description: Places a dragged window from the cursor and syncs frames to screen refresh during the drag
  - primaryOnly: false
    $name: Primary monitor only
  $name: Performance and advanced
*/
// ==/WindhawkModSettings==
// clang-format on

// clang-format off
#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <wtsapi32.h>
#include <GL/gl.h>
// clang-format on
#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// clang-format off
const char* baseVertexShader = R"GLSL(varying vec2 vUv;
void main(){vUv=gl_Vertex.xy*.5+.5;gl_Position=vec4(gl_Vertex.xy,0.,1.);}
)GLSL";

// One fragment shader for both passes:
// - desktop pass: lit wallpaper with shadows from every tracked window;
// - overlay pass: alpha-only shadows on windows, cast by windows above them.
const char* lightShader = R"GLSL(varying vec2 vUv;
uniform vec2 viewOrigin,viewSize,lightPoint;
uniform float projectionScale,shadowOpacity,shadowSoftness,cornerRadius,anchoredShadows,contactShadow;
uniform float lightRadius,intensity,ambient,showSource,vignette;
uniform vec3 lightColor,floorColor,shadowTint,overlayShadowColor;
uniform sampler2D backgroundTexture;
uniform float hasBackground,backgroundBrightness;
uniform int count;
uniform vec4 windows[64];
uniform float overlayPass,windowShadowStrength;
// Overlay pass: 1 = application (casts and receives), 0 = other surface (mask).
uniform float surfaceFlags[64];

float roundedBox(vec2 p,vec2 center,vec2 halfSize,float radius){
    radius=min(radius,min(halfSize.x,halfSize.y));
    vec2 q=abs(p-center)-halfSize+vec2(radius);
    return length(max(q,vec2(0.)))+min(max(q.x,q.y),0.)-radius;
}
// Anchored shadow = convex hull of the window footprint and its projection.
// For two axis-aligned rounded boxes that hull is exactly the union of the
// boxes interpolated between them (t=0 footprint, t=1 projection), so its
// distance is min over t of a rounded-box distance. That function is convex
// in t outside the shape, so a golden-section search finds it exactly.
float sweptBox(vec2 p,vec2 center,vec2 halfSize,vec2 projected,float scale,float t){
    float s=mix(1.,scale,t);
    return roundedBox(p,mix(center,projected,t),halfSize*s,cornerRadius*s);
}
// Returns (distance, t of the closest slice) so blur can grow along the shadow.
vec2 anchoredDistance(vec2 p,vec2 center,vec2 halfSize,vec2 projected,float scale){
    float a=0.,b=1.,x1=.381966,x2=.618034;
    float f1=sweptBox(p,center,halfSize,projected,scale,x1);
    float f2=sweptBox(p,center,halfSize,projected,scale,x2);
    for(int k=0;k<14;k++){
        if(f1<f2){b=x2;x2=x1;f2=f1;x1=a+.381966*(b-a);f1=sweptBox(p,center,halfSize,projected,scale,x1);}
        else{a=x1;x1=x2;f1=f2;x2=a+.618034*(b-a);f2=sweptBox(p,center,halfSize,projected,scale,x2);}
    }
    vec2 best=f1<f2?vec2(f1,x1):vec2(f2,x2);
    float d0=sweptBox(p,center,halfSize,projected,scale,0.);
    float d1=sweptBox(p,center,halfSize,projected,scale,1.);
    if(d0<best.x)best=vec2(d0,0.);
    if(d1<best.x)best=vec2(d1,1.);
    return best;
}
// Shadow coverage at p from the windows before `limit` (above the receiver).
float shadowAt(vec2 p,int limit){
    float shadow=0.;
    for(int i=0;i<64;i++){
        if(i>=limit)break;
        if(overlayPass>.5 && surfaceFlags[i]<.5)continue;
        vec4 r=windows[i];
        vec2 center=(r.xy+r.zw)*.5,footHalf=(r.zw-r.xy)*.5;
        // Contact shadow: grounds the window, independent of the light.
        if(contactShadow>0.){
            float d=roundedBox(p,center,footHalf,cornerRadius);
            if(d>0.)shadow=max(shadow,contactShadow*exp(-d/18.));
        }
        if(shadowOpacity<=0.)continue;
        vec2 halfSize=footHalf*projectionScale;
        vec2 projected=lightPoint+(center-lightPoint)*projectionScale;
        float distance;
        float edge=max(1.,shadowSoftness*(.6+.4*projectionScale));
        if(anchoredShadows>.5){
            // Bounding box of the hull: a lower bound, so far pixels skip the search.
            vec2 lo=min(r.xy,projected-halfSize),hi=max(r.zw,projected+halfSize);
            vec2 axis=max(lo-p,p-hi);
            if(max(axis.x,axis.y)>max(1.,shadowSoftness*(.45+.55*projectionScale)))continue;
            vec2 hit=anchoredDistance(p,center,footHalf,projected,projectionScale);
            distance=hit.x;
            // Contact hardening: crisp at the window base, softer toward the far end.
            edge=max(1.,shadowSoftness*(.45+.55*mix(1.,projectionScale,hit.y)));
        }
        else distance=roundedBox(p,projected,halfSize,cornerRadius*projectionScale);
        shadow=max(shadow,(1.-smoothstep(-edge*.25,edge,distance))*shadowOpacity);
        if(shadow>.999)break;
    }
    return shadow;
}
void main(){
    vec2 p=viewOrigin+vec2(vUv.x,1.-vUv.y)*viewSize;
    if(overlayPass>.5){
        // The topmost surface under p receives shadows only from windows above it.
        int receiver=-1;
        for(int i=0;i<64;i++){
            if(i>=count)break;
            vec4 r=windows[i];
            float radius=surfaceFlags[i]>.5?cornerRadius:0.;
            if(roundedBox(p,(r.xy+r.zw)*.5,(r.zw-r.xy)*.5,radius)<=0.){receiver=i;break;}
        }
        if(receiver<0 || surfaceFlags[receiver]<.5){gl_FragColor=vec4(0.);return;}
        float alpha=clamp(shadowAt(p,receiver)*windowShadowStrength,0.,.95);
        // Premultiplied: a dark, optionally tinted shade.
        gl_FragColor=vec4(overlayShadowColor*alpha,alpha);return;
    }
    // `shadow` already includes opacity (0 = lit, 1 = fully shadowed).
    float shadow=shadowAt(p,count);
    float d=length(p-lightPoint);
    float diffuse=exp(-d*d/(lightRadius*lightRadius*.6))*intensity;
    float visibility=1.-shadow;
    vec3 c=floorColor*ambient+(floorColor+vec3(.20))*lightColor*diffuse*visibility;
    if(hasBackground>.5){
        vec3 base=texture2D(backgroundTexture,vec2(vUv.x,1.-vUv.y)).rgb;
        c=base*backgroundBrightness*(1.-shadow*.75)+base*lightColor*diffuse*.35*visibility;
    }
    // Shadow color shifts the hue of shaded areas only (neutral = 1,1,1).
    c*=mix(vec3(1.),shadowTint,shadow);
    if(showSource>.5)c+=lightColor*(exp(-d*d/28.)*1.2+exp(-d*d/1100.)*.09);
    if(vignette>0.){
        vec2 v=vUv*2.-1.;
        c*=1.-vignette*smoothstep(.45,2.,dot(v,v));
    }
    gl_FragColor=vec4(clamp(c,0.,1.),1.);
}
)GLSL";
// clang-format on

// Functions beyond Windows' OpenGL 1.1 exports, resolved only on the render thread.
#define GLPROC(ret, name, ...)                    \
    using name##_t = ret(APIENTRY*)(__VA_ARGS__); \
    name##_t name = nullptr
GLPROC(GLuint, CreateShader, GLenum);
GLPROC(void, ShaderSource, GLuint, GLsizei, const char* const*, const GLint*);
GLPROC(void, CompileShader, GLuint);
GLPROC(void, GetShaderiv, GLuint, GLenum, GLint*);
GLPROC(void, GetShaderInfoLog, GLuint, GLsizei, GLsizei*, char*);
GLPROC(void, DeleteShader, GLuint);
GLPROC(GLuint, CreateProgram);
GLPROC(void, AttachShader, GLuint, GLuint);
GLPROC(void, LinkProgram, GLuint);
GLPROC(void, GetProgramiv, GLuint, GLenum, GLint*);
GLPROC(void, GetProgramInfoLog, GLuint, GLsizei, GLsizei*, char*);
GLPROC(void, DeleteProgram, GLuint);
GLPROC(void, UseProgram, GLuint);
GLPROC(GLint, GetUniformLocation, GLuint, const char*);
GLPROC(void, Uniform1i, GLint, GLint);
GLPROC(void, Uniform1f, GLint, GLfloat);
GLPROC(void, Uniform2f, GLint, GLfloat, GLfloat);
GLPROC(void, Uniform3f, GLint, GLfloat, GLfloat, GLfloat);
GLPROC(void, Uniform4fv, GLint, GLsizei, const GLfloat*);
GLPROC(void, Uniform1fv, GLint, GLsizei, const GLfloat*);
GLPROC(void, ActiveTexture, GLenum);
GLPROC(void, GenFramebuffers, GLsizei, GLuint*);
GLPROC(void, BindFramebuffer, GLenum, GLuint);
GLPROC(void, FramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);
GLPROC(GLenum, CheckFramebufferStatus, GLenum);
GLPROC(void, DeleteFramebuffers, GLsizei, const GLuint*);
#undef GLPROC

void* GLAddress(const char* name) {
    auto p = wglGetProcAddress(name);
    auto value = reinterpret_cast<intptr_t>(p);
    if (!p || value == 1 || value == 2 || value == 3 || value == -1)
        p = GetProcAddress(GetModuleHandleW(L"opengl32.dll"), name);
    return reinterpret_cast<void*>(p);
}
void LoadGL() {
#define LOAD(name)                                                \
    do {                                                          \
        name = reinterpret_cast<name##_t>(GLAddress("gl" #name)); \
        if (!name)                                                \
            throw std::runtime_error("Missing gl" #name);         \
    } while (0)
    LOAD(CreateShader);
    LOAD(ShaderSource);
    LOAD(CompileShader);
    LOAD(GetShaderiv);
    LOAD(GetShaderInfoLog);
    LOAD(DeleteShader);
    LOAD(CreateProgram);
    LOAD(AttachShader);
    LOAD(LinkProgram);
    LOAD(GetProgramiv);
    LOAD(GetProgramInfoLog);
    LOAD(DeleteProgram);
    LOAD(UseProgram);
    LOAD(GetUniformLocation);
    LOAD(Uniform1i);
    LOAD(Uniform1f);
    LOAD(Uniform2f);
    LOAD(Uniform3f);
    LOAD(Uniform4fv);
    LOAD(Uniform1fv);
    LOAD(ActiveTexture);
    LOAD(GenFramebuffers);
    LOAD(BindFramebuffer);
    LOAD(FramebufferTexture2D);
    LOAD(CheckFramebufferStatus);
    LOAD(DeleteFramebuffers);
#undef LOAD
}
constexpr GLenum Framebuffer = 0x8D40, Attachment = 0x8CE0, Complete = 0x8CD5;
constexpr GLenum Texture0 = 0x84C0, ClampEdge = 0x812F, BGRA = 0x80E1;
// A frame is redrawn when its inputs change, and at least this often as a safety net.
constexpr ULONGLONG SafetyRefreshMs = 1000;

enum class Motion { Mouse, Wander, Orbit, SunArc, Clock, Fixed };

struct Settings {
    // Light (preset values already resolved).
    Motion motion;
    float speed, fixedX, fixedY, ambient;
    bool fixedPerMonitor, showSource, flicker;
    float intensity, radius, lightHeight, light[3];
    // Shadows (preset values already resolved).
    float opacity, softness, windowHeight, corners, contact;
    bool anchored, overWindows;
    float windowShadowStrength, tint[3], overlayTint[3];
    // Background.
    std::wstring backgroundMode, backgroundPath, backgroundFit;
    float backgroundBrightness, vignette, floor[3];
    // Performance and advanced.
    // Longest side of the internal image; 0 = native monitor resolution.
    int fps, renderWidth, maxWindows;
    bool fullscreen, dragPrediction, primary;
    bool NeedsOverlay() const {
        return overWindows && (opacity > 0 || contact > 0) &&
               windowShadowStrength > 0;
    }
};
int Setting(const wchar_t* key, int lo, int hi) {
    return std::clamp(Wh_GetIntSetting(key), lo, hi);
}
bool Flag(const wchar_t* key) {
    return Wh_GetIntSetting(key) != 0;
}
std::wstring StringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}
struct LightPreset {
    const wchar_t* name;
    float red, green, blue, intensity, radius, height;
    bool flicker;
};
constexpr LightPreset LightPresets[] = {
    {L"warmLamp", 255, 214, 170, 1.25f, 1000, 900, false},
    {L"daylight", 255, 250, 240, 1.10f, 1600, 1400, false},
    {L"candle", 255, 170, 90, 1.35f, 750, 600, true},
    {L"moonlight", 150, 180, 255, 0.75f, 1400, 1300, false},
    {L"neon", 120, 220, 255, 1.40f, 800, 700, false},
};
// Shadow length is relative to the light height, so presets look alike with every light.
struct ShadowPreset {
    const wchar_t* name;
    float opacity, softness, heightRatio;
    // Long shadows read as shadows only when attached to the window.
    bool anchored;
};
constexpr ShadowPreset ShadowPresets[] = {
    {L"crisp", .90f, 6, .12f, false},    {L"soft", .85f, 25, .13f, false},
    {L"diffuse", .65f, 70, .15f, false}, {L"long", .85f, 22, .40f, true},
    {L"subtle", .45f, 40, .09f, false},
};
// Shade multiplier on the wallpaper and premultiplied overlay color on windows.
struct ShadowColor {
    const wchar_t* name;
    float tint[3], overlay[3];
};
constexpr ShadowColor ShadowColors[] = {
    {L"neutral", {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}},
    {L"cool", {.72f, .86f, 1.30f}, {.02f, .05f, .16f}},
    {L"warm", {1.25f, 1.f, .75f}, {.14f, .07f, .02f}},
    {L"violet", {1.12f, .78f, 1.30f}, {.10f, .02f, .14f}},
};
template <typename T, size_t N>
const T* FindPreset(const T (&presets)[N], const std::wstring& name) {
    for (const T& preset : presets)
        if (name == preset.name)
            return &preset;
    return nullptr;
}
Settings ReadSettings() {
    Settings s{};
    std::wstring motion = StringSetting(L"light.motion");
    s.motion = motion == L"wander"   ? Motion::Wander
               : motion == L"orbit"  ? Motion::Orbit
               : motion == L"sunArc" ? Motion::SunArc
               : motion == L"clock"  ? Motion::Clock
               : motion == L"fixed"  ? Motion::Fixed
                                     : Motion::Mouse;
    s.speed = Setting(L"light.speed", 1, 100);
    s.fixedX = Setting(L"light.fixedX", -100, 200) / 100.f;
    s.fixedY = Setting(L"light.fixedY", -100, 200) / 100.f;
    s.fixedPerMonitor = StringSetting(L"light.fixedScope") == L"each";
    s.showSource = Flag(L"light.showSource");
    s.ambient = Setting(L"light.ambient", 0, 100) / 100.f;
    if (auto preset =
            FindPreset(LightPresets, StringSetting(L"light.preset"))) {
        s.light[0] = preset->red / 255.f;
        s.light[1] = preset->green / 255.f;
        s.light[2] = preset->blue / 255.f;
        s.intensity = preset->intensity;
        s.radius = preset->radius;
        s.lightHeight = preset->height;
        s.flicker = preset->flicker;
    } else {
        s.light[0] = Setting(L"light.red", 0, 255) / 255.f;
        s.light[1] = Setting(L"light.green", 0, 255) / 255.f;
        s.light[2] = Setting(L"light.blue", 0, 255) / 255.f;
        s.intensity = Setting(L"light.intensity", 0, 400) / 100.f;
        s.radius = Setting(L"light.radius", 100, 5000);
        s.lightHeight = Setting(L"light.height", 100, 5000);
    }
    const ShadowPreset* shadowPreset =
        FindPreset(ShadowPresets, StringSetting(L"shadows.preset"));
    if (auto preset = shadowPreset) {
        s.opacity = preset->opacity;
        s.softness = preset->softness;
        s.windowHeight = preset->heightRatio * s.lightHeight;
    } else {
        s.opacity = Setting(L"shadows.opacity", 0, 100) / 100.f;
        s.softness = Setting(L"shadows.softness", 0, 200);
        s.windowHeight = Setting(L"shadows.windowHeight", 0, 1000);
    }
    // The light must stay above the windows; this also caps magnification.
    s.lightHeight = std::max(s.lightHeight, s.windowHeight + 80.f);
    s.anchored =
        Flag(L"shadows.anchored") || (shadowPreset && shadowPreset->anchored);
    s.contact = Setting(L"shadows.contact", 0, 100) / 100.f;
    s.corners = Setting(L"shadows.cornerRadius", 0, 80);
    auto color = FindPreset(ShadowColors, StringSetting(L"shadows.color"));
    if (!color)
        color = &ShadowColors[0];
    std::copy_n(color->tint, 3, s.tint);
    std::copy_n(color->overlay, 3, s.overlayTint);
    s.overWindows = Flag(L"shadows.onWindows");
    s.windowShadowStrength =
        Setting(L"shadows.onWindowsStrength", 0, 100) / 100.f;

    s.backgroundMode = StringSetting(L"background.mode");
    s.backgroundPath = StringSetting(L"background.path");
    if (s.backgroundPath.size() > 1 && s.backgroundPath.front() == L'"' &&
        s.backgroundPath.back() == L'"')
        s.backgroundPath =
            s.backgroundPath.substr(1, s.backgroundPath.size() - 2);
    s.backgroundFit = StringSetting(L"background.fit");
    s.backgroundBrightness = Setting(L"background.brightness", 0, 150) / 100.f;
    s.vignette = Setting(L"background.vignette", 0, 100) / 100.f;
    s.floor[0] = Setting(L"background.red", 0, 255) / 255.f;
    s.floor[1] = Setting(L"background.green", 0, 255) / 255.f;
    s.floor[2] = Setting(L"background.blue", 0, 255) / 255.f;

    s.fps = Setting(L"performance.fps", 15, 120);
    std::wstring quality = StringSetting(L"performance.quality");
    s.renderWidth = quality == L"high"       ? 2560
                    : quality == L"balanced" ? 1920
                    : quality == L"fast"     ? 1280
                                             : 0;
    s.maxWindows = Setting(L"performance.maxWindows", 8, 64);
    s.fullscreen = Flag(L"performance.pauseFullscreen");
    s.dragPrediction = Flag(L"performance.smoothDrag");
    s.primary = Flag(L"performance.primaryOnly");
    return s;
}

struct Target {
    GLuint texture = 0, fbo = 0;
    int w = 0, h = 0;
};
GLuint Shader(GLenum type, const std::string& source) {
    GLuint id = CreateShader(type);
    const char* p = source.c_str();
    ShaderSource(id, 1, &p, nullptr);
    CompileShader(id);
    GLint ok = 0;
    GetShaderiv(id, 0x8B81, &ok);
    if (!ok) {
        char msg[4096]{};
        GetShaderInfoLog(id, sizeof(msg), nullptr, msg);
        DeleteShader(id);
        throw std::runtime_error(msg);
    }
    return id;
}
GLuint Program(const char* fragment, const char* vertex = baseVertexShader) {
    GLuint vs = 0, fs = 0, id = 0;
    try {
        vs = Shader(0x8B31, std::string("#version 120\n") + vertex);
        fs = Shader(0x8B30, std::string("#version 120\n") + fragment);
        id = CreateProgram();
        AttachShader(id, vs);
        AttachShader(id, fs);
        LinkProgram(id);
        GLint ok = 0;
        GetProgramiv(id, 0x8B82, &ok);
        if (!ok) {
            char msg[4096]{};
            GetProgramInfoLog(id, sizeof(msg), nullptr, msg);
            throw std::runtime_error(msg);
        }
    } catch (...) {
        if (vs)
            DeleteShader(vs);
        if (fs)
            DeleteShader(fs);
        if (id)
            DeleteProgram(id);
        throw;
    }
    DeleteShader(vs);
    DeleteShader(fs);
    return id;
}
// Uniform locations are looked up once per program instead of every frame.
// Keys are string literals, so their addresses are stable.
struct UniformCache {
    GLuint program = 0;
    std::unordered_map<const char*, GLint> locations;
};
UniformCache* activeUniforms = nullptr;
GLint Loc(const char* n) {
    auto found = activeUniforms->locations.find(n);
    if (found != activeUniforms->locations.end())
        return found->second;
    GLint location = GetUniformLocation(activeUniforms->program, n);
    activeUniforms->locations.emplace(n, location);
    return location;
}
void Bind(UniformCache& cache) {
    activeUniforms = &cache;
    UseProgram(cache.program);
}
void F(const char* n, float v) {
    Uniform1f(Loc(n), v);
}
void I(const char* n, int v) {
    Uniform1i(Loc(n), v);
}
void V2(const char* n, float x, float y) {
    Uniform2f(Loc(n), x, y);
}
void V3(const char* n, float x, float y, float z) {
    Uniform3f(Loc(n), x, y, z);
}
void Quad() {
    glBegin(GL_QUADS);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();
}
void Draw(const Target& t) {
    BindFramebuffer(Framebuffer, t.fbo);
    glViewport(0, 0, t.w, t.h);
    Quad();
}
void CreateTarget(Target& t, const char* what) {
    glGenTextures(1, &t.texture);
    GenFramebuffers(1, &t.fbo);
    ActiveTexture(Texture0);
    glBindTexture(GL_TEXTURE_2D, t.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t.w, t.h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    BindFramebuffer(Framebuffer, t.fbo);
    FramebufferTexture2D(Framebuffer, Attachment, GL_TEXTURE_2D, t.texture, 0);
    if (CheckFramebufferStatus(Framebuffer) != Complete)
        throw std::runtime_error(what);
}
void ReleaseTarget(Target& t) {
    if (t.fbo && DeleteFramebuffers)
        DeleteFramebuffers(1, &t.fbo);
    if (t.texture)
        glDeleteTextures(1, &t.texture);
    t = Target{};
}
// Scale from monitor pixels to the internal image (1 = native resolution).
float RenderScale(int maxSide, int width, int height) {
    if (maxSide <= 0)
        return 1.f;
    return std::min(1.f, float(maxSide) / std::max(width, height));
}
HBITMAP CreateDib(int w, int h, void** bits) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = h;
    return CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, bits, nullptr, 0);
}

HINSTANCE instance;
HANDLE stopEvent = nullptr, updateEvent = nullptr, readyEvent = nullptr,
       worker = nullptr;
bool startupOK = false;
constexpr wchar_t WindowClass[] = L"WindhawkCastShadowRenderer";
constexpr wchar_t LayerClass[] = L"WindhawkCastShadowLayer";
HWND wallpaperParent = nullptr, iconHost = nullptr, iconView = nullptr;
HWND draggedWindow = nullptr;
RECT draggedInsets{};
bool draggedInsetsValid = false;
// Drag prediction: window top-left relative to the cursor, and window size,
// measured when the move starts (the window is still at rest then).
bool dragPrediction = true, dragAnchorValid = false;
POINT dragOffset{};
SIZE dragSize{};
void CALLBACK
DragEvent(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        draggedWindow = hwnd;
        draggedInsetsValid = false;
        RECT outer{}, visible{};
        if (hwnd && GetWindowRect(hwnd, &outer) &&
            SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                            &visible, sizeof(visible)))) {
            draggedInsets = {visible.left - outer.left, visible.top - outer.top,
                             outer.right - visible.right,
                             outer.bottom - visible.bottom};
            draggedInsetsValid = true;
        }
        POINT cursor{};
        dragAnchorValid = false;
        if (hwnd && (outer.right > outer.left || GetWindowRect(hwnd, &outer)) &&
            GetCursorPos(&cursor)) {
            dragOffset = {outer.left - cursor.x, outer.top - cursor.y};
            dragSize = {outer.right - outer.left, outer.bottom - outer.top};
            dragAnchorValid = true;
        }
    } else if (event == EVENT_SYSTEM_MOVESIZEEND) {
        draggedWindow = nullptr;
        draggedInsetsValid = false;
        dragAnchorValid = false;
    }
}
struct DragHook {
    HWINEVENTHOOK handle =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART,
                        EVENT_SYSTEM_MOVESIZEEND,
                        nullptr,
                        DragEvent,
                        0,
                        0,
                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    DragHook() { Wh_Log(L"Window drag tracking hook=%p", handle); }
    ~DragHook() {
        if (handle)
            UnhookWinEvent(handle);
        draggedWindow = nullptr;
        draggedInsetsValid = false;
        dragAnchorValid = false;
    }
};
bool raisedDesktop = false;
HWND backgroundWindow = nullptr;
// Raised desktop layout: our layer must sit between SHELLDLL_DefView (icons)
// and Explorer's background WorkerW. Only our own window is ever moved.
void EnsureLayerOrder(HWND layer) {
    if (!raisedDesktop || !iconView || !IsWindow(iconView) || !IsWindow(layer))
        return;
    for (HWND w = GetWindow(iconView, GW_HWNDNEXT); w;
         w = GetWindow(w, GW_HWNDNEXT)) {
        if (w == layer)
            return;  // Already below the icons and above the background.
        if (w == backgroundWindow)
            break;
    }
    SetWindowPos(layer, iconView, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
// The reduced-resolution frame a desktop window paints from (scaled once, in WM_PAINT).
struct PaintSource {
    HDC dc = nullptr;
    int w = 0, h = 0;
};
LRESULT CALLBACK WindowProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    if (m == WM_NCHITTEST)
        return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE)
        return MA_NOACTIVATE;
    if (m == WM_ERASEBKGND)
        return 1;
    if (m == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC output = BeginPaint(w, &ps);
        auto source = reinterpret_cast<const PaintSource*>(
            GetWindowLongPtrW(w, GWLP_USERDATA));
        if (source && source->dc) {
            RECT r{};
            GetClientRect(w, &r);
            if (r.right == source->w && r.bottom == source->h) {
                BitBlt(output, 0, 0, r.right, r.bottom, source->dc, 0, 0,
                       SRCCOPY);
            } else {
                // Reduced quality: filter instead of repeating pixels.
                SetStretchBltMode(output, HALFTONE);
                SetBrushOrgEx(output, 0, 0, nullptr);
                StretchBlt(output, 0, 0, r.right, r.bottom, source->dc, 0, 0,
                           source->w, source->h, SRCCOPY);
            }
        }
        EndPaint(w, &ps);
        return 0;
    }
    if (m == WM_CLOSE)
        return 0;
    return DefWindowProcW(w, m, wp, lp);
}
DWORD ShellProcessId() {
    DWORD pid = 0;
    if (HWND shell = GetShellWindow())
        GetWindowThreadProcessId(shell, &pid);
    return pid;
}
// lParam: the shell's process id, so another app's SHELLDLL_DefView is ignored.
BOOL CALLBACK FindDesktop(HWND top, LPARAM shellPid) {
    DWORD pid = 0;
    GetWindowThreadProcessId(top, &pid);
    if (DWORD(shellPid) && pid != DWORD(shellPid))
        return TRUE;
    HWND icons = FindWindowExW(top, nullptr, L"SHELLDLL_DefView", nullptr);
    if (icons && !iconView) {
        iconHost = top;
        iconView = icons;
    }
    return TRUE;
}
HWND FindWallpaperParent() {
    wallpaperParent = nullptr;
    iconHost = nullptr;
    iconView = nullptr;
    raisedDesktop = false;
    backgroundWindow = nullptr;
    HWND prog = GetShellWindow();
    if (!prog)
        prog = FindWindowW(L"Progman", nullptr);
    if (!prog)
        return nullptr;
    DWORD_PTR result = 0;
    SendMessageTimeoutW(prog, 0x052C, 0xD, 0, SMTO_ABORTIFHUNG, 1000, &result);
    SendMessageTimeoutW(prog, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000, &result);
    EnumWindows(FindDesktop, LPARAM(ShellProcessId()));
    if (!iconView) {
        Wh_Log(L"Desktop SHELLDLL_DefView not found");
        return nullptr;
    }
    HWND childWorker = FindWindowExW(iconHost, nullptr, L"WorkerW", nullptr);
    backgroundWindow = childWorker;
    raisedDesktop = (GetWindowLongPtrW(iconHost, GWL_EXSTYLE) &
                     WS_EX_NOREDIRECTIONBITMAP) != 0 ||
                    childWorker != nullptr;
    if (raisedDesktop) {
        // The wallpaper is a sibling BELOW DefView and ABOVE the shell WorkerW.
        // Do not place it inside the shell's background WorkerW.
        wallpaperParent = iconHost;
    } else {
        wallpaperParent = FindWindowExW(nullptr, iconHost, L"WorkerW", nullptr);
        if (wallpaperParent && FindWindowExW(wallpaperParent, nullptr,
                                             L"SHELLDLL_DefView", nullptr))
            wallpaperParent = nullptr;
    }
    RECT client{};
    if (wallpaperParent)
        GetClientRect(wallpaperParent, &client);
    Wh_Log(
        L"Desktop layout: raised=%d parent=%p icons=%p backgroundChild=%p "
        L"client=%ldx%ld visible=%d",
        raisedDesktop, wallpaperParent, iconView, childWorker, client.right,
        client.bottom, wallpaperParent ? IsWindowVisible(wallpaperParent) : 0);
    return wallpaperParent;
}
bool FullscreenOn(HMONITOR monitor, const RECT& r) {
    HWND w = GetForegroundWindow();
    if (!w || IsIconic(w))
        return false;
    wchar_t cls[80]{};
    GetClassNameW(w, cls, 80);
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        wcscmp(cls, WindowClass) == 0 || wcscmp(cls, LayerClass) == 0)
        return false;
    if (MonitorFromWindow(w, MONITOR_DEFAULTTONULL) != monitor)
        return false;
    RECT wr{};
    if (!GetWindowRect(w, &wr))
        return false;
    return wr.left <= r.left && wr.top <= r.top && wr.right >= r.right &&
           wr.bottom >= r.bottom;
}
struct MonitorInfo {
    HMONITOR monitor;
    RECT rect;
    bool primary;
};
BOOL CALLBACK CollectMonitor(HMONITOR h, HDC, LPRECT, LPARAM param) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(h, &mi))
        reinterpret_cast<std::vector<MonitorInfo>*>(param)->push_back(
            {h, mi.rcMonitor, (mi.dwFlags & MONITORINFOF_PRIMARY) != 0});
    return TRUE;
}
std::vector<MonitorInfo> Monitors(bool primaryOnly) {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitor,
                        reinterpret_cast<LPARAM>(&list));
    if (primaryOnly)
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [](auto& m) { return !m.primary; }),
                   list.end());
    return list;
}
bool SameMonitors(const std::vector<MonitorInfo>& a,
                  const std::vector<MonitorInfo>& b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++)
        if (a[i].monitor != b[i].monitor || !EqualRect(&a[i].rect, &b[i].rect))
            return false;
    return true;
}
RECT PrimaryMonitorRect() {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY),
                        &mi))
        return mi.rcMonitor;
    return RECT{0, 0, GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN)};
}

// Screen-space rectangles, owned and refreshed solely by the rendering thread.
using Rects = std::vector<std::array<float, 4>>;
Rects trackedWindows, overlayWindows;
std::vector<float> overlayFlags;
bool collectOverlay = false;
int windowLimit = 48;
struct LiveBounds {
    RECT previous{}, insets{};
    DWORD pid = 0;
    ULONGLONG lastDwm = 0;
    unsigned stable = 0;
    bool initialized = false;
};
std::unordered_map<HWND, LiveBounds> liveBounds;
ULONGLONG nextBoundsPrune = 0, frameTick = 0;
// During a move the window follows the cursor rigidly, but the position Windows
// reports trails the cursor by a frame or so. Placing it from the cursor removes
// that lag without extrapolation, so it never overshoots when the mouse stops.
void PredictDraggedWindow(RECT& outer) {
    if (!dragPrediction || !dragAnchorValid)
        return;
    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return;
    LONG w = outer.right - outer.left, h = outer.bottom - outer.top;
    LONG x = cursor.x + dragOffset.x, y = cursor.y + dragOffset.y;
    // Resizing, restoring from maximized, or held back (top edge, app limits):
    // re-anchor to where the window really is and use its real position.
    if (w != dragSize.cx || h != dragSize.cy ||
        std::abs(x - outer.left) > 120 || std::abs(y - outer.top) > 120) {
        dragOffset = {outer.left - cursor.x, outer.top - cursor.y};
        dragSize = {w, h};
        return;
    }
    outer = {x, y, x + w, y + h};
}
bool CurrentVisibleBounds(HWND window, DWORD pid, RECT& result) {
    RECT outer{};
    if (!GetWindowRect(window, &outer))
        return false;
    auto& state = liveBounds[window];
    if (state.pid != pid) {
        state = LiveBounds{};
        state.pid = pid;
    }
    bool stationary = state.initialized && EqualRect(&outer, &state.previous) &&
                      window != draggedWindow;
    state.stable = stationary ? std::min(3u, state.stable + 1) : 0;
    if (!state.initialized ||
        (state.stable >= 2 && frameTick - state.lastDwm >= 1000)) {
        RECT visible{};
        if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS,
                                            &visible, sizeof(visible))))
            state.insets = {visible.left - outer.left, visible.top - outer.top,
                            outer.right - visible.right,
                            outer.bottom - visible.bottom};
        state.lastDwm = frameTick;
        state.initialized = true;
    }
    state.previous = outer;
    if (window == draggedWindow)
        PredictDraggedWindow(outer);
    RECT insets = window == draggedWindow && draggedInsetsValid ? draggedInsets
                                                                : state.insets;
    result = {outer.left + insets.left, outer.top + insets.top,
              outer.right - insets.right, outer.bottom - insets.bottom};
    return true;
}
BOOL CALLBACK CollectWindow(HWND window, LPARAM) {
    if (int(trackedWindows.size()) >= windowLimit &&
        (!collectOverlay || overlayWindows.size() >= 64))
        return FALSE;
    if (!IsWindowVisible(window) || IsIconic(window))
        return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid == GetCurrentProcessId())
        return TRUE;
    LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE),
             ex = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (style & WS_CHILD)
        return TRUE;
    wchar_t cls[64]{};
    GetClassNameW(window, cls, 64);
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0)
        return TRUE;
    bool taskbar = wcscmp(cls, L"Shell_TrayWnd") == 0 ||
                   wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
    bool app =
        !taskbar && !(ex & WS_EX_TOOLWINDOW) &&
        ((style & (WS_CAPTION | WS_THICKFRAME)) || (ex & WS_EX_APPWINDOW));
    // Without the overlay only application windows matter: skip the DWM query for the rest.
    if (!collectOverlay && !app)
        return TRUE;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked)
        return TRUE;
    RECT r{};
    if (!CurrentVisibleBounds(window, pid, r))
        return TRUE;
    if (r.right <= r.left || r.bottom <= r.top ||
        !MonitorFromRect(&r, MONITOR_DEFAULTTONULL))
        return TRUE;
    std::array<float, 4> bounds{float(r.left), float(r.top), float(r.right),
                                float(r.bottom)};
    // Overlay: apps cast and receive; taskbar, menus and tooltips only mask.
    if (collectOverlay && overlayWindows.size() < 64) {
        overlayWindows.push_back(bounds);
        overlayFlags.push_back(app ? 1.f : 0.f);
    }
    if (app && r.right - r.left >= 64 && r.bottom - r.top >= 40 &&
        int(trackedWindows.size()) < windowLimit)
        trackedWindows.push_back(bounds);
    return TRUE;
}
void RefreshWindows(int limit, bool includeOverlay) {
    frameTick = GetTickCount64();
    if (frameTick >= nextBoundsPrune) {
        for (auto it = liveBounds.begin(); it != liveBounds.end();)
            if (!IsWindow(it->first))
                it = liveBounds.erase(it);
            else
                ++it;
        nextBoundsPrune = frameTick + 2000;
    }
    windowLimit = limit;
    collectOverlay = includeOverlay;
    trackedWindows.clear();
    overlayWindows.clear();
    overlayFlags.clear();
    EnumWindows(CollectWindow, 0);
}

struct ImageRuntime {
    ULONG_PTR token = 0;
    bool comInitialized = false;
    ImageRuntime() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        comInitialized = SUCCEEDED(hr);
        Gdiplus::GdiplusStartupInput input;
        if (Gdiplus::GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok) {
            token = 0;
            Wh_Log(L"GDI+ image decoding unavailable");
        }
    }
    ~ImageRuntime() {
        if (token)
            Gdiplus::GdiplusShutdown(token);
        if (comInitialized)
            CoUninitialize();
    }
};
struct BackgroundInfo {
    std::wstring path;
    DESKTOP_WALLPAPER_POSITION position = DWPOS_FILL;
    RECT region{};
    COLORREF color = RGB(0, 0, 0);
    bool valid = true;
};
BackgroundInfo BackgroundFor(const MonitorInfo& mi, const Settings& settings) {
    BackgroundInfo bg;
    bg.region = mi.rect;
    bg.color = RGB(int(settings.floor[0] * 255), int(settings.floor[1] * 255),
                   int(settings.floor[2] * 255));
    if (settings.backgroundMode == L"solid")
        return bg;
    if (settings.backgroundMode == L"custom") {
        bg.path = settings.backgroundPath;
        if (!bg.path.empty()) {
            DWORD length =
                ExpandEnvironmentStringsW(bg.path.c_str(), nullptr, 0);
            if (length && length < 32768) {
                std::vector<wchar_t> expanded(length);
                if (ExpandEnvironmentStringsW(bg.path.c_str(), expanded.data(),
                                              length))
                    bg.path = expanded.data();
            }
        }
        bg.valid = !bg.path.empty();
        if (settings.backgroundFit == L"fit")
            bg.position = DWPOS_FIT;
        else if (settings.backgroundFit == L"stretch")
            bg.position = DWPOS_STRETCH;
        else if (settings.backgroundFit == L"center")
            bg.position = DWPOS_CENTER;
        else if (settings.backgroundFit == L"tile")
            bg.position = DWPOS_TILE;
        return bg;
    }
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(mi.monitor, &info))
        bg.region = info.rcMonitor;
    IDesktopWallpaper* api = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr,
                                   CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&api)))) {
        api->GetPosition(&bg.position);
        api->GetBackgroundColor(&bg.color);
        UINT count = 0;
        api->GetMonitorDevicePathCount(&count);
        bool matched = false;
        for (UINT i = 0; i < count; i++) {
            LPWSTR id = nullptr;
            if (FAILED(api->GetMonitorDevicePathAt(i, &id)) || !id)
                continue;
            RECT r{};
            if (SUCCEEDED(api->GetMonitorRECT(id, &r)) &&
                EqualRect(&r, &bg.region)) {
                LPWSTR path = nullptr;
                if (SUCCEEDED(api->GetWallpaper(id, &path)) && path) {
                    bg.path = path;
                    CoTaskMemFree(path);
                }
                matched = true;
                CoTaskMemFree(id);
                break;
            }
            CoTaskMemFree(id);
        }
        if (!matched || bg.path.empty()) {
            LPWSTR path = nullptr;
            if (SUCCEEDED(api->GetWallpaper(nullptr, &path)) && path) {
                bg.path = path;
                CoTaskMemFree(path);
            }
        }
        api->Release();
    } else {
        std::vector<wchar_t> path(32768);
        if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, DWORD(path.size()),
                                  path.data(), 0))
            bg.path = path.data();
        bg.valid = !bg.path.empty();
    }
    if (bg.position == DWPOS_SPAN) {
        bg.region.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        bg.region.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        bg.region.right = bg.region.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        bg.region.bottom = bg.region.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    return bg;
}
std::wstring BackgroundKey(const BackgroundInfo& bg) {
    WIN32_FILE_ATTRIBUTE_DATA file{};
    ULONGLONG modified = 0;
    if (!bg.path.empty() &&
        GetFileAttributesExW(bg.path.c_str(), GetFileExInfoStandard, &file))
        modified = (ULONGLONG(file.ftLastWriteTime.dwHighDateTime) << 32) |
                   file.ftLastWriteTime.dwLowDateTime;
    return bg.path + L"|" + std::to_wstring(int(bg.position)) + L"|" +
           std::to_wstring(bg.color) + L"|" + std::to_wstring(modified) + L"|" +
           std::to_wstring(bg.region.left) + L"," +
           std::to_wstring(bg.region.top) + L"," +
           std::to_wstring(bg.region.right) + L"," +
           std::to_wstring(bg.region.bottom) + L"|" + std::to_wstring(bg.valid);
}
struct BackgroundPixels {
    int w = 1, h = 1;
    std::vector<unsigned char> pixels;
};
BackgroundPixels RenderBackground(const BackgroundInfo& bg,
                                  const RECT& viewport,
                                  int maxWidth) {
    if (!bg.valid)
        throw std::runtime_error(
            "Wallpaper path unavailable; set a valid image path");
    BackgroundPixels result;
    if (bg.path.empty()) {
        result.pixels = {GetBValue(bg.color), GetGValue(bg.color),
                         GetRValue(bg.color), 255};
        return result;
    }
    std::unique_ptr<Gdiplus::Bitmap> source(
        Gdiplus::Bitmap::FromFile(bg.path.c_str(), TRUE));
    if (!source || source->GetLastStatus() != Gdiplus::Ok ||
        !source->GetWidth() || !source->GetHeight())
        throw std::runtime_error(
            "Cannot decode wallpaper image (JPG/PNG/BMP supported)");
    int width = viewport.right - viewport.left,
        height = viewport.bottom - viewport.top;
    float scale = RenderScale(maxWidth, width, height);
    result.w = std::max(1, int(std::round(width * scale)));
    result.h = std::max(1, int(std::round(height * scale)));
    Gdiplus::Bitmap output(result.w, result.h, PixelFormat32bppARGB);
    if (output.GetLastStatus() != Gdiplus::Ok)
        throw std::runtime_error("Background bitmap allocation failed");
    {
        Gdiplus::Graphics g(&output);
        g.Clear(Gdiplus::Color(255, GetRValue(bg.color), GetGValue(bg.color),
                               GetBValue(bg.color)));
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        float sx = float(result.w) / width, sy = float(result.h) / height;
        Gdiplus::Matrix transform(sx, 0, 0, sy, -viewport.left * sx,
                                  -viewport.top * sy);
        g.SetTransform(&transform);
        float rw = float(bg.region.right - bg.region.left),
              rh = float(bg.region.bottom - bg.region.top);
        float iw = float(source->GetWidth()), ih = float(source->GetHeight());
        if (bg.position == DWPOS_TILE) {
            Gdiplus::TextureBrush brush(source.get(), Gdiplus::WrapModeTile);
            Gdiplus::Matrix origin(1, 0, 0, 1, float(bg.region.left),
                                   float(bg.region.top));
            brush.SetTransform(&origin);
            if (g.FillRectangle(&brush, Gdiplus::RectF(float(bg.region.left),
                                                       float(bg.region.top), rw,
                                                       rh)) != Gdiplus::Ok)
                throw std::runtime_error("Background tile rendering failed");
        } else {
            float dw = iw, dh = ih;
            if (bg.position == DWPOS_STRETCH) {
                dw = rw;
                dh = rh;
            } else if (bg.position != DWPOS_CENTER) {
                float fit = bg.position == DWPOS_FIT
                                ? std::min(rw / iw, rh / ih)
                                : std::max(rw / iw, rh / ih);
                dw = iw * fit;
                dh = ih * fit;
            }
            Gdiplus::RectF destination(bg.region.left + (rw - dw) * .5f,
                                       bg.region.top + (rh - dh) * .5f, dw, dh);
            if (g.DrawImage(source.get(), destination, 0, 0, iw, ih,
                            Gdiplus::UnitPixel) != Gdiplus::Ok)
                throw std::runtime_error("Background image rendering failed");
        }
    }
    Gdiplus::BitmapData data{};
    Gdiplus::Rect region(0, 0, result.w, result.h);
    if (output.LockBits(&region, Gdiplus::ImageLockModeRead,
                        PixelFormat32bppARGB, &data) != Gdiplus::Ok)
        throw std::runtime_error("Background bitmap lock failed");
    try {
        result.pixels.resize(size_t(result.w) * result.h * 4);
        for (int y = 0; y < result.h; y++) {
            auto row = static_cast<unsigned char*>(data.Scan0) +
                       ptrdiff_t(y) * data.Stride;
            std::copy_n(row, size_t(result.w) * 4,
                        result.pixels.data() + size_t(y) * result.w * 4);
        }
    } catch (...) {
        output.UnlockBits(&data);
        throw;
    }
    output.UnlockBits(&data);
    return result;
}

struct Wallpaper {
    HWND window = nullptr, renderWindow = nullptr;
    HDC dc = nullptr, smallDC = nullptr;
    HGLRC context = nullptr;
    HBITMAP smallBitmap = nullptr;
    HGDIOBJ oldSmall = nullptr;
    void* pixels = nullptr;
    PaintSource paint;
    Target frame;
    UniformCache shader;
    HWND shadowOverlay = nullptr;
    HDC overlayDC = nullptr;
    HBITMAP overlayBitmap = nullptr;
    HGDIOBJ oldOverlay = nullptr;
    Target overlayFrame;
    std::vector<uint32_t> overlayReadback;
    uint32_t* overlayDib = nullptr;
    std::vector<int> overlayX, overlayY;
    bool overlayLogged = false;
    DWORD overlayError = 0;
    Settings s{};
    MonitorInfo info{};
    bool layered = false, logged = false;
    GLuint backgroundTexture = 0;
    bool hasBackground = false;
    std::wstring backgroundKey;
    ULONGLONG nextBackgroundCheck = 0;
    int outputW = 0, outputH = 0;
    // Light position driven by the motion; what is rendered adds flicker on top.
    float lightX = 0, lightY = 0, fixedX = 0, fixedY = 0;
    // Seconds since start, in double so animation stays smooth after days of uptime.
    double time = 0;
    // Light as rendered this frame: x, y, height, intensity, r, g, b.
    using LightKey = std::array<float, 7>;
    LightKey current{};
    // What was last presented, to skip frames whose inputs are unchanged.
    bool desktopDirty = true, overlayDirty = true;
    LightKey desktopLight{}, overlayLight{};
    Rects desktopWindows, overlayPresented;
    std::vector<float> overlayPresentedFlags;
    ULONGLONG desktopTick = 0, overlayTick = 0, nextTopmost = 0;
    std::mt19937 random{std::random_device{}()};
    struct PointF {
        float x, y;
    };
    PointF start{}, finish{};
    float segmentTime = 0, segmentDuration = 12;
    float Rand() { return std::uniform_real_distribution<float>(0, 1)(random); }
    PointF RandomPoint() { return {.1f + .8f * Rand(), .1f + .8f * Rand()}; }
    // Sub-pixel motion and tiny color/intensity changes are invisible.
    static bool Changed(const LightKey& a, const LightKey& b) {
        for (size_t i = 0; i < a.size(); i++)
            if (!(std::fabs(a[i] - b[i]) <= (i < 3 ? .05f : .002f)))
                return true;
        return false;
    }
    void ComputeFixedPoint() {
        // "Primary" shares one light across monitors in global coordinates, like the mouse light.
        RECT base = s.fixedPerMonitor ? info.rect : PrimaryMonitorRect();
        fixedX = base.left + s.fixedX * (base.right - base.left);
        fixedY = base.top + s.fixedY * (base.bottom - base.top);
    }
    void Initialize(const MonitorInfo& mi,
                    const Settings& settings,
                    HWND parent) {
        s = settings;
        info = mi;
        const RECT& r = info.rect;
        outputW = r.right - r.left;
        outputH = r.bottom - r.top;
        layered = raisedDesktop;
        POINT origin{r.left, r.top};
        MapWindowPoints(nullptr, parent, &origin, 1);
        // WS_EX_NOPARENTNOTIFY: creating/destroying a child otherwise sends a
        // synchronous WM_PARENTNOTIFY to Explorer, which blocks if it hangs.
        window = CreateWindowExW(
            WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY |
                (layered ? WS_EX_LAYERED : 0),
            layered ? LayerClass : WindowClass,
            L"Real-time Cast Shadow Wallpaper",
            WS_CHILD | (layered ? 0 : WS_CLIPSIBLINGS | WS_CLIPCHILDREN),
            origin.x, origin.y, outputW, outputH, parent, nullptr, instance,
            nullptr);
        if (!window)
            throw std::runtime_error("Wallpaper window creation failed");
        PlaceLayer();
        if (layered) {
            renderWindow = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, WindowClass,
                L"Cast shadow GPU renderer", WS_POPUP, 0, 0, 16, 16, nullptr,
                nullptr, instance, nullptr);
            if (!renderWindow)
                throw std::runtime_error("Hidden GPU drawable creation failed");
        }
        dc = GetDC(layered ? renderWindow : window);
        if (!dc)
            throw std::runtime_error("GetDC failed");
        PIXELFORMATDESCRIPTOR pf{};
        pf.nSize = sizeof(pf);
        pf.nVersion = 1;
        pf.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pf.iPixelType = PFD_TYPE_RGBA;
        pf.cColorBits = 32;
        int format = ChoosePixelFormat(dc, &pf);
        if (!format || !SetPixelFormat(dc, format, &pf))
            throw std::runtime_error("OpenGL pixel format failed");
        context = wglCreateContext(dc);
        if (!context || !wglMakeCurrent(dc, context))
            throw std::runtime_error("OpenGL context failed");
        LoadGL();
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        using SwapInterval_t = BOOL(WINAPI*)(int);
        if (auto fn = reinterpret_cast<SwapInterval_t>(
                GLAddress("wglSwapIntervalEXT")))
            fn(0);
        shader.program = Program(lightShader);
        if (layered) {
            float scale = RenderScale(s.renderWidth, outputW, outputH);
            frame.w = std::max(1, int(std::round(outputW * scale)));
            frame.h = std::max(1, int(std::round(outputH * scale)));
            CreateTarget(frame, "Presentation framebuffer failed");
            smallDC = CreateCompatibleDC(nullptr);
            smallBitmap = CreateDib(frame.w, frame.h, &pixels);
            if (!smallDC || !smallBitmap || !pixels)
                throw std::runtime_error("Presentation DIB allocation failed");
            oldSmall = SelectObject(smallDC, smallBitmap);
            if (!SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA))
                throw std::runtime_error("Fixed opacity desktop setup failed");
            paint = {smallDC, frame.w, frame.h};
            SetWindowLongPtrW(window, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(&paint));
        }
        if (s.NeedsOverlay()) {
            try {
                InitializeOverlay();
            } catch (const std::exception& e) {
                Wh_Log(L"Window shadow overlay unavailable: %S", e.what());
                ReleaseOverlay();
            }
        }
        UpdateBackground(true);
        ComputeFixedPoint();
        start = RandomPoint();
        finish = RandomPoint();
        segmentDuration = (8.f + 8.f * Rand()) * 10.f / s.speed;
        lightX = r.left + start.x * outputW;
        lightY = r.top + start.y * outputH;
        UpdateLight(0.f, POINT{LONG_MIN, LONG_MIN});
        Wh_Log(
            L"Wallpaper: OpenGL=%S monitor=%ld,%ld %dx%d layered=%d motion=%d",
            glGetString(GL_VERSION), r.left, r.top, outputW, outputH, layered,
            int(s.motion));
        Render(true);
        ShowWindow(window, SW_SHOWNOACTIVATE);
        PlaceLayer();
        logged = false;
        Render(true);
    }
    // Raised layout: between the icons and Explorer's background. Classic
    // layout: at the bottom of the WorkerW we are a child of.
    void PlaceLayer() {
        if (layered)
            EnsureLayerOrder(window);
        else
            SetWindowPos(window, HWND_BOTTOM, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    // Forces a full redraw on the next frame (e.g. after a pause).
    void Invalidate() { desktopDirty = overlayDirty = true; }
    // Returns false while this monitor is paused for a fullscreen app.
    bool Step(float dt, POINT mouse) {
        if (s.fullscreen && FullscreenOn(info.monitor, info.rect)) {
            if (shadowOverlay)
                ShowWindow(shadowOverlay, SW_HIDE);
            Invalidate();  // Redraw everything when the pause ends.
            return false;
        }
        UpdateLight(dt, mouse);
        wglMakeCurrent(dc, context);
        UpdateBackground(false);
        Render(false);
        return true;
    }
    // Places the light on an arc across the monitor (f: 0 = left horizon,
    // 1 = right horizon) and returns its elevation (0 horizon .. 1 top).
    float SkyPosition(float f) {
        f = std::clamp(f, 0.f, 1.f);
        float elevation = std::sin(3.14159265f * f);
        lightX = info.rect.left + outputW * (.06f + .88f * f);
        lightY = info.rect.top + outputH * (.95f - .85f * elevation);
        return elevation;
    }
    void UpdateLight(float dt, POINT mouse) {
        time += dt;
        float elevation = 1.f;
        bool sky = false, night = false;
        switch (s.motion) {
            case Motion::Fixed:
                lightX = fixedX;
                lightY = fixedY;
                break;
            case Motion::Mouse:
                if (mouse.x != LONG_MIN) {
                    float follow = 1.f - std::exp(-dt * 18.f);
                    lightX += (mouse.x - lightX) * follow;
                    lightY += (mouse.y - lightY) * follow;
                }
                break;
            case Motion::Wander: {
                segmentTime += dt;
                if (segmentTime >= segmentDuration) {
                    segmentTime = 0;
                    start = finish;
                    finish = RandomPoint();
                    segmentDuration = (8.f + 8.f * Rand()) * 10.f / s.speed;
                }
                float t = std::clamp(segmentTime / segmentDuration, 0.f, 1.f);
                t = t * t * t * (t * (t * 6.f - 15.f) + 10.f);
                lightX = info.rect.left +
                         (start.x + (finish.x - start.x) * t) * outputW;
                lightY = info.rect.top +
                         (start.y + (finish.y - start.y) * t) * outputH;
                break;
            }
            case Motion::Orbit: {
                // Speed 10 = one turn per minute.
                float angle =
                    float(std::fmod(time * s.speed / 600., 1.) * 6.2831853);
                lightX =
                    info.rect.left + outputW * (.5f + .32f * std::cos(angle));
                lightY =
                    info.rect.top + outputH * (.5f + .30f * std::sin(angle));
                break;
            }
            case Motion::SunArc:
                // Speed 10 = one sunrise-to-sunset sweep every three minutes.
                sky = true;
                elevation =
                    SkyPosition(float(std::fmod(time * s.speed / 1800., 1.)));
                break;
            case Motion::Clock: {
                // Day arc 6:00-20:00, moonlight arc 20:00-6:00.
                SYSTEMTIME now;
                GetLocalTime(&now);
                float hour =
                    now.wHour + now.wMinute / 60.f + now.wSecond / 3600.f;
                night = hour < 6.f || hour >= 20.f;
                sky = true;
                elevation =
                    SkyPosition(night ? std::fmod(hour + 4.f, 24.f) / 10.f
                                      : (hour - 6.f) / 14.f);
                break;
            }
        }
        float height = s.lightHeight, brightness = s.intensity,
              color[3]{s.light[0], s.light[1], s.light[2]};
        if (sky) {
            // A low sun casts longer shadows and a warmer, dimmer light.
            height *= .3f + .7f * elevation;
            brightness *= .6f + .4f * elevation;
            constexpr float horizon[3]{1.f, .62f, .38f},
                moon[3]{.62f, .72f, 1.f};
            for (int i = 0; i < 3; i++)
                color[i] = night ? s.light[i] * .15f + moon[i] * .85f
                                 : color[i] * (horizon[i] +
                                               (1.f - horizon[i]) * elevation);
            if (night)
                brightness *= .45f;
        }
        float x = lightX, y = lightY;
        if (s.flicker) {
            // Incommensurate sines: an irregular flame that never visibly loops.
            float n = float(.55 * std::sin(time * 7.3) +
                            .30 * std::sin(time * 13.1 + 1.7) +
                            .15 * std::sin(time * 23.9 + .4));
            brightness *= 1.f + .10f * n;
            x += float(2.5 * std::sin(time * 9.7));
            y += float(2. * std::sin(time * 11.3 + .8));
        }
        // Keep the light above the windows whatever the elevation, and cap the
        // projection at 2.25x so low suns give long but still readable shadows.
        height =
            std::max({height, s.windowHeight + 80.f, s.windowHeight * 1.8f});
        current = {x, y, height, brightness, color[0], color[1], color[2]};
    }
    void UpdateBackground(bool force) {
        if (!force && draggedWindow)
            return;  // Avoid image/COM work during interactive dragging.
        ULONGLONG now = GetTickCount64();
        if (!force && now < nextBackgroundCheck)
            return;
        nextBackgroundCheck = now + 2000;
        BackgroundInfo bg = BackgroundFor(info, s);
        std::wstring key = BackgroundKey(bg);
        if (!force && key == backgroundKey)
            return;
        backgroundKey = key;
        hasBackground = false;
        desktopDirty = true;
        if (s.backgroundMode == L"solid")
            return;
        try {
            auto image = RenderBackground(bg, info.rect, s.renderWidth);
            if (!backgroundTexture)
                glGenTextures(1, &backgroundTexture);
            ActiveTexture(Texture0);
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ClampEdge);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ClampEdge);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.w, image.h, 0, BGRA,
                         GL_UNSIGNED_BYTE, image.pixels.data());
            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
                throw std::runtime_error("Background GPU upload failed");
            hasBackground = true;
            Wh_Log(L"Background loaded: mode=%s %dx%d position=%d",
                   s.backgroundMode.c_str(), image.w, image.h,
                   int(bg.position));
        } catch (const std::exception& e) {
            Wh_Log(L"Background unavailable, using original surface: %S",
                   e.what());
        }
    }
    void ReleaseOverlay() {
        if (shadowOverlay)
            ShowWindow(shadowOverlay, SW_HIDE);
        ReleaseTarget(overlayFrame);
        if (overlayDC && oldOverlay)
            SelectObject(overlayDC, oldOverlay);
        if (overlayBitmap)
            DeleteObject(overlayBitmap);
        if (overlayDC)
            DeleteDC(overlayDC);
        if (shadowOverlay)
            DestroyWindow(shadowOverlay);
        shadowOverlay = nullptr;
        overlayDC = nullptr;
        overlayBitmap = nullptr;
        oldOverlay = nullptr;
        overlayDib = nullptr;
    }
    void InitializeOverlay() {
        shadowOverlay = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
                WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            LayerClass, L"Cast shadows on windows", WS_POPUP, info.rect.left,
            info.rect.top, outputW, outputH, nullptr, nullptr, instance,
            nullptr);
        if (!shadowOverlay)
            throw std::runtime_error("Shadow overlay window creation failed");
        float scale = RenderScale(s.renderWidth, outputW, outputH);
        overlayFrame.w = std::max(1, int(std::round(outputW * scale)));
        overlayFrame.h = std::max(1, int(std::round(outputH * scale)));
        CreateTarget(overlayFrame, "Shadow overlay framebuffer failed");
        overlayReadback.resize(size_t(overlayFrame.w) * overlayFrame.h);
        overlayDC = CreateCompatibleDC(nullptr);
        void* data = nullptr;
        overlayBitmap = CreateDib(outputW, outputH, &data);
        overlayDib = static_cast<uint32_t*>(data);
        if (!overlayDC || !overlayBitmap || !overlayDib)
            throw std::runtime_error("Shadow overlay bitmap allocation failed");
        oldOverlay = SelectObject(overlayDC, overlayBitmap);
        overlayX.resize(outputW);
        overlayY.resize(outputH);
        for (int x = 0; x < outputW; x++)
            overlayX[x] = std::min(overlayFrame.w - 1,
                                   int((x + .5) * overlayFrame.w / outputW));
        for (int y = 0; y < outputH; y++)
            overlayY[y] = std::min(overlayFrame.h - 1,
                                   int((y + .5) * overlayFrame.h / outputH));
        Wh_Log(L"Window shadows initialized: %dx%d GPU, %dx%d overlay",
               overlayFrame.w, overlayFrame.h, outputW, outputH);
    }
    void RenderOverlay(bool force) {
        // Sample geometry again after desktop GPU readback/GDI presentation,
        // so overlay edges don't reuse the older beginning-of-frame positions.
        RefreshWindows(s.maxWindows, true);
        ULONGLONG now = GetTickCount64();
        if (now >= nextTopmost) {
            // A clicked topmost window can rise above the overlay; stay on top.
            // Surfaces above us are still masked out by the shader.
            nextTopmost = now + 1000;
            SetWindowPos(
                shadowOverlay, HWND_TOPMOST, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        }
        bool dirty = force || overlayDirty ||
                     now - overlayTick >= SafetyRefreshMs ||
                     Changed(current, overlayLight) ||
                     overlayWindows != overlayPresented ||
                     overlayFlags != overlayPresentedFlags;
        if (!dirty)
            return;
        F("overlayPass", 1.f);
        F("windowShadowStrength", s.windowShadowStrength);
        I("count", GLint(overlayWindows.size()));
        if (!overlayWindows.empty()) {
            Uniform4fv(Loc("windows[0]"), GLsizei(overlayWindows.size()),
                       overlayWindows.front().data());
            Uniform1fv(Loc("surfaceFlags[0]"), GLsizei(overlayFlags.size()),
                       overlayFlags.data());
        }
        Draw(overlayFrame);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0, 0, overlayFrame.w, overlayFrame.h, BGRA,
                     GL_UNSIGNED_BYTE, overlayReadback.data());
        GdiFlush();
        // Preserve alpha explicitly. GDI stretching can discard the alpha channel.
        // OpenGL and the positive-height DIB both store the bottom row first.
        for (int y = 0; y < outputH; y++) {
            uint32_t* destination = overlayDib + size_t(y) * outputW;
            if (y > 0 && overlayY[y] == overlayY[y - 1])
                std::copy_n(destination - outputW, outputW, destination);
            else {
                const uint32_t* source = overlayReadback.data() +
                                         size_t(overlayY[y]) * overlayFrame.w;
                for (int x = 0; x < outputW; x++)
                    destination[x] = source[overlayX[x]];
            }
        }
        POINT dest{info.rect.left, info.rect.top}, source{0, 0};
        SIZE size{outputW, outputH};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        BOOL ok = UpdateLayeredWindow(shadowOverlay, nullptr, &dest, &size,
                                      overlayDC, &source, 0, &blend, ULW_ALPHA);
        DWORD error = ok ? 0 : GetLastError();
        if (ok) {
            if (!IsWindowVisible(shadowOverlay))
                ShowWindow(shadowOverlay, SW_SHOWNOACTIVATE);
        } else
            ShowWindow(shadowOverlay, SW_HIDE);
        if (!overlayLogged || error != overlayError) {
            Wh_Log(L"Window shadow overlay: ok=%d error=%lu surfaces=%u", ok,
                   error, unsigned(overlayWindows.size()));
            overlayLogged = true;
            overlayError = error;
        }
        overlayDirty = !ok;
        overlayTick = now;
        overlayLight = current;
        overlayPresented = overlayWindows;
        overlayPresentedFlags = overlayFlags;
    }
    void SetFrameUniforms() {
        Bind(shader);
        F("overlayPass", 0.f);
        V2("viewOrigin", float(info.rect.left), float(info.rect.top));
        V2("viewSize", float(outputW), float(outputH));
        // Similar triangles: a window at height h under a light at height H
        // projects scaled by H/(H-h) from the light's ground point.
        V2("lightPoint", current[0], current[1]);
        F("projectionScale",
          std::min(4.f, current[2] / (current[2] - s.windowHeight)));
        F("anchoredShadows", s.anchored ? 1.f : 0.f);
        F("shadowOpacity", s.opacity);
        F("shadowSoftness", s.softness);
        F("cornerRadius", s.corners);
        F("contactShadow", s.contact);
        V3("shadowTint", s.tint[0], s.tint[1], s.tint[2]);
        V3("overlayShadowColor", s.overlayTint[0], s.overlayTint[1],
           s.overlayTint[2]);
        F("lightRadius", s.radius);
        F("intensity", current[3]);
        F("ambient", s.ambient);
        F("showSource", s.showSource ? 1.f : 0.f);
        F("vignette", s.vignette);
        V3("lightColor", current[4], current[5], current[6]);
        V3("floorColor", s.floor[0], s.floor[1], s.floor[2]);
        F("hasBackground", hasBackground ? 1.f : 0.f);
        F("backgroundBrightness", s.backgroundBrightness);
        ActiveTexture(Texture0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        I("backgroundTexture", 0);
        I("count", GLint(trackedWindows.size()));
        if (!trackedWindows.empty())
            Uniform4fv(Loc("windows[0]"), GLsizei(trackedWindows.size()),
                       trackedWindows.front().data());
    }
    void Render(bool force) {
        SetFrameUniforms();
        ULONGLONG now = GetTickCount64();
        // The non-layered path draws straight to a GL window with no retained copy, so it always renders.
        bool dirty = force || !layered || desktopDirty ||
                     now - desktopTick >= SafetyRefreshMs ||
                     Changed(current, desktopLight) ||
                     trackedWindows != desktopWindows;
        if (dirty) {
            BOOL ok = TRUE;
            if (layered) {
                Draw(frame);
                GdiFlush();
                glPixelStorei(GL_PACK_ALIGNMENT, 4);
                glReadPixels(0, 0, frame.w, frame.h, BGRA, GL_UNSIGNED_BYTE,
                             pixels);
                // WM_PAINT copies the frame straight onto the window.
                InvalidateRect(window, nullptr, FALSE);
                UpdateWindow(window);
            } else {
                BindFramebuffer(Framebuffer, 0);
                glViewport(0, 0, outputW, outputH);
                Quad();
                ok = SwapBuffers(dc);
            }
            if (!logged || !ok) {
                Wh_Log(
                    L"Light frame: ok=%d error=%lu GL=0x%X windows=%u "
                    L"visible=%d",
                    ok, ok ? 0 : GetLastError(), glGetError(),
                    unsigned(trackedWindows.size()), IsWindowVisible(window));
                logged = true;
            }
            desktopDirty = !ok;
            desktopTick = now;
            desktopLight = current;
            desktopWindows = trackedWindows;
        }
        if (shadowOverlay)
            RenderOverlay(force);
    }
    ~Wallpaper() {
        if (shadowOverlay)
            ShowWindow(shadowOverlay, SW_HIDE);
        if (window && IsWindow(window)) {
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            ShowWindow(window, SW_HIDE);
        }
        if (context && wglMakeCurrent(dc, context)) {
            if (shader.program && DeleteProgram)
                DeleteProgram(shader.program);
            if (backgroundTexture)
                glDeleteTextures(1, &backgroundTexture);
            ReleaseTarget(overlayFrame);
            ReleaseTarget(frame);
            wglMakeCurrent(nullptr, nullptr);
        }
        if (context)
            wglDeleteContext(context);
        if (dc)
            ReleaseDC(layered ? renderWindow : window, dc);
        if (smallDC && oldSmall)
            SelectObject(smallDC, oldSmall);
        if (smallBitmap)
            DeleteObject(smallBitmap);
        if (smallDC)
            DeleteDC(smallDC);
        if (overlayDC && oldOverlay)
            SelectObject(overlayDC, oldOverlay);
        if (overlayBitmap)
            DeleteObject(overlayBitmap);
        if (overlayDC)
            DeleteDC(overlayDC);
        if (shadowOverlay)
            DestroyWindow(shadowOverlay);
        if (renderWindow)
            DestroyWindow(renderWindow);
        if (window && IsWindow(window))
            DestroyWindow(window);
    }
};

// Session lock and display state, updated by the control window's messages.
// Rendering pauses while nobody can see the desktop.
bool sessionLocked = false, displayOff = false;
LRESULT CALLBACK ControlProc(HWND w, UINT m, WPARAM wp, LPARAM lp) {
    if (m == WM_WTSSESSION_CHANGE) {
        if (wp == WTS_SESSION_LOCK)
            sessionLocked = true;
        else if (wp == WTS_SESSION_UNLOCK)
            sessionLocked = false;
        return 0;
    }
    if (m == WM_POWERBROADCAST && wp == PBT_POWERSETTINGCHANGE) {
        auto setting = reinterpret_cast<const POWERBROADCAST_SETTING*>(lp);
        if (setting &&
            IsEqualGUID(setting->PowerSetting, GUID_CONSOLE_DISPLAY_STATE) &&
            setting->DataLength >= sizeof(DWORD)) {
            // 0 = off, 1 = on, 2 = dimmed (still visible).
            displayOff = *reinterpret_cast<const DWORD*>(setting->Data) == 0;
        }
        return TRUE;
    }
    return DefWindowProcW(w, m, wp, lp);
}
constexpr wchar_t ControlClass[] = L"WindhawkCastShadowControl";
// Hidden window receiving session-lock and display-state notifications.
struct PowerWatcher {
    HWND window = nullptr;
    HPOWERNOTIFY power = nullptr;
    bool session = false;
    PowerWatcher() {
        WNDCLASSEXW cls{};
        cls.cbSize = sizeof(cls);
        cls.lpfnWndProc = ControlProc;
        cls.hInstance = instance;
        cls.lpszClassName = ControlClass;
        if (!RegisterClassExW(&cls))
            return;
        window = CreateWindowExW(WS_EX_TOOLWINDOW, ControlClass,
                                 L"Cast shadow control", WS_POPUP, 0, 0, 0, 0,
                                 nullptr, nullptr, instance, nullptr);
        if (!window)
            return;
        session =
            WTSRegisterSessionNotification(window, NOTIFY_FOR_THIS_SESSION);
        power = RegisterPowerSettingNotification(
            window, &GUID_CONSOLE_DISPLAY_STATE, DEVICE_NOTIFY_WINDOW_HANDLE);
        Wh_Log(L"Pause notifications: session=%d display=%d", session,
               power != nullptr);
    }
    ~PowerWatcher() {
        if (power)
            UnregisterPowerSettingNotification(power);
        if (session)
            WTSUnRegisterSessionNotification(window);
        if (window)
            DestroyWindow(window);
        UnregisterClassW(ControlClass, instance);
        sessionLocked = displayOff = false;
    }
};

DWORD WINAPI RenderThread(void*) {
    // No shared settings or GPU resources: this thread owns the complete renderer.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSEXW cls{};
    cls.cbSize = sizeof(cls);
    cls.style = CS_OWNDC;
    cls.lpfnWndProc = WindowProc;
    cls.hInstance = instance;
    cls.lpszClassName = WindowClass;
    if (!RegisterClassExW(&cls)) {
        Wh_Log(L"RegisterClass failed: %lu", GetLastError());
        SetEvent(readyEvent);
        return 1;
    }
    WNDCLASSEXW layerCls = cls;
    layerCls.style = 0;
    layerCls.lpszClassName = LayerClass;
    if (!RegisterClassExW(&layerCls)) {
        UnregisterClassW(WindowClass, instance);
        SetEvent(readyEvent);
        return 1;
    }
    ImageRuntime imageRuntime;
    DragHook dragHook;
    PowerWatcher powerWatcher;
    // The desktop may not exist yet (e.g. at sign-in, or while Explorer
    // restarts); the loop below keeps retrying, so startup succeeds here.
    startupOK = true;
    SetEvent(readyEvent);

    std::vector<std::unique_ptr<Wallpaper>> wallpapers;
    Settings settings = ReadSettings();
    std::vector<MonitorInfo> monitors;
    dragPrediction = settings.dragPrediction;
    HWND parent = nullptr;
    bool rebuild = true, paused = false;
    // Failed builds are retried with a growing delay (2 s .. 60 s), so a
    // missing desktop or OpenGL driver doesn't cost work every 2 seconds.
    ULONGLONG nextCheck = 0, retryDelay = 2000;
    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    try {
        while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
            LARGE_INTEGER frameStart;
            QueryPerformanceCounter(&frameStart);
            float dt =
                std::clamp(float(double(frameStart.QuadPart - last.QuadPart) /
                                 freq.QuadPart),
                           .001f, 1.f / 30);
            last = frameStart;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (WaitForSingleObject(updateEvent, 0) == WAIT_OBJECT_0) {
                settings = ReadSettings();
                rebuild = true;
                retryDelay = 2000;
                dragPrediction = settings.dragPrediction;
            }
            HANDLE events[]{stopEvent};
            // Locked session or display off: nothing is visible, so do no
            // work at all and wake rarely; redraw everything on resume.
            if (sessionLocked || displayOff) {
                if (!paused)
                    Wh_Log(L"Paused (locked=%d displayOff=%d)", sessionLocked,
                           displayOff);
                paused = true;
                if (MsgWaitForMultipleObjects(1, events, FALSE, 1000,
                                              QS_ALLINPUT) == WAIT_OBJECT_0)
                    break;
                continue;
            }
            if (paused) {
                paused = false;
                Wh_Log(L"Resumed");
                for (auto& w : wallpapers)
                    w->Invalidate();
                QueryPerformanceCounter(&last);
                continue;
            }
            ULONGLONG now = GetTickCount64();
            if (now >= nextCheck) {
                nextCheck = now + 2000;
                auto list = Monitors(settings.primary);
                if (!SameMonitors(monitors, list))
                    rebuild = true;
                if (!parent || !IsWindow(parent) || wallpapers.empty())
                    rebuild = true;
                for (auto& w : wallpapers) {
                    if (!IsWindow(w->window))
                        rebuild = true;
                    else
                        w->PlaceLayer();
                }
            }
            if (rebuild) {
                wallpapers.clear();
                parent = FindWallpaperParent();
                monitors = Monitors(settings.primary);
                if (parent) {
                    for (auto& mi : monitors) {
                        auto w = std::make_unique<Wallpaper>();
                        try {
                            w->Initialize(mi, settings, parent);
                            wallpapers.push_back(std::move(w));
                        } catch (const std::exception& e) {
                            Wh_Log(L"Monitor initialization failed: %S",
                                   e.what());
                        }
                    }
                }
                rebuild = false;
                if (wallpapers.empty()) {
                    Wh_Log(L"Desktop not ready; retrying in %llu s",
                           retryDelay / 1000);
                    nextCheck = GetTickCount64() + retryDelay;
                    retryDelay = std::min<ULONGLONG>(retryDelay * 2, 60000);
                } else {
                    retryDelay = 2000;
                }
                QueryPerformanceCounter(&last);
                dt = 1.f / settings.fps;
            }
            // Window geometry is only needed when something will be drawn.
            bool anyActive = false;
            if (!wallpapers.empty()) {
                // Desktop shadows only need application windows; the overlay
                // pass re-samples the full Z order (masking surfaces) itself.
                RefreshWindows(settings.maxWindows, false);
                POINT mouse{};
                bool gotMouse = GetCursorPos(&mouse) != 0;
                for (auto& w : wallpapers)
                    anyActive |= w->Step(
                        dt, gotMouse ? mouse : POINT{LONG_MIN, LONG_MIN});
            }
            if (anyActive && draggedWindow && settings.dragPrediction) {
                // While dragging, wake right after each composition so window
                // positions are sampled fresh and presented by the next one.
                if (FAILED(DwmFlush()))
                    Sleep(1);
                continue;
            }
            // Nothing to draw (no desktop yet, or every monitor is paused for
            // a fullscreen app): check back a few times per second only.
            DWORD wait = 250;
            if (anyActive) {
                QueryPerformanceCounter(&frameStart);
                double elapsed = double(frameStart.QuadPart - last.QuadPart) *
                                 1000 / freq.QuadPart;
                wait = DWORD(std::max(1., 1000. / settings.fps - elapsed));
            }
            if (MsgWaitForMultipleObjects(1, events, FALSE, wait,
                                          QS_ALLINPUT) == WAIT_OBJECT_0)
                break;
        }
    } catch (const std::exception& e) {
        Wh_Log(L"Renderer stopped: %S", e.what());
    }
    wallpapers.clear();
    UnregisterClassW(LayerClass, instance);
    UnregisterClassW(WindowClass, instance);
    return 0;
}
void CloseEvents() {
    if (stopEvent)
        CloseHandle(stopEvent);
    if (updateEvent)
        CloseHandle(updateEvent);
    if (readyEvent)
        CloseHandle(readyEvent);
    stopEvent = updateEvent = readyEvent = nullptr;
}
BOOL WhTool_ModInit() {
    Wh_Log(L"Real-time Cast Shadow Wallpaper initializing");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&RenderThread), &instance);
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    updateEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent || !updateEvent || !readyEvent) {
        CloseEvents();
        return FALSE;
    }
    worker = CreateThread(nullptr, 0, RenderThread, nullptr, 0, nullptr);
    if (!worker) {
        CloseEvents();
        return FALSE;
    }
    if (WaitForSingleObject(readyEvent, 15000) != WAIT_OBJECT_0 || !startupOK) {
        Wh_Log(L"Startup failed: the render thread could not be set up.");
        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
        worker = nullptr;
        CloseEvents();
        return FALSE;
    }
    Wh_Log(L"Real-time Cast Shadow Wallpaper started");
    return TRUE;
}
void WhTool_ModSettingsChanged() {
    if (updateEvent)
        SetEvent(updateEvent);
}
void WhTool_ModUninit() {
    Wh_Log(L"Stopping Real-time Cast Shadow Wallpaper");
    if (stopEvent)
        SetEvent(stopEvent);
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
        worker = nullptr;
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

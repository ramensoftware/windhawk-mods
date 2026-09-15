// ==WindhawkMod==
// @id              vector-screen-holder
// @name            Vector Screen Holder
// @description     Fills a display you choose with generative line art and keeps the PC from idling while it runs
// @version         1.0.6
// @author          akilluminati47
// @github          https://github.com/akilluminati47
// @homepage        https://vector.akilluminati47.pages.dev/
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -ladvapi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vector Screen Holder

Fills a display of your choosing with generative line art, and holds the screen
awake for as long as it runs. Every frame is drawn as strokes through Direct2D
on the GPU. There are no images, no video file, and no fixed resolution, so
the artwork is
drawn for whatever size the display you pick actually is: a 1080p side monitor
and a 4K portrait panel each get correctly proportioned art.

Downloading a large file, or running a long build, render or backup, and you
need the machine not to sign you out or drop to idle? Start the Screen Holder,
put it on a side monitor next to your task monitor, and go to lunch. The work
keeps running, the screen stays awake, and you can still see progress at a
glance from across the room.

![The overlay running on a portrait monitor while a note is typed in Notepad](https://raw.githubusercontent.com/akilluminati47/vector-screen-holder/main/assets/typing.gif)

Above: a note being written on the side monitor while the art keeps running
underneath it. The overlay is below the window, not over it.

## Controls

The overlay is clean when it opens: no labels, no chrome, nothing on screen
but the art. When you change something, a single line appears along the bottom
naming the style, its parameter value and the amount notch, then fades after
about two seconds. That line is the only text the mod ever draws.

| Input | What it does |
| --- | --- |
| **Esc** | Close the overlay (focus it first, or see below) |
| **Left click** | Cycle to the next enabled style |
| **Right click** | Step the amount: how much information is on screen |
| **Mouse wheel** | Adjust the current style's parameter |
| **Hold Space** | Slide the colour around the hue wheel |
| **Ctrl+Alt+H** | Toggle the overlay on and off (configurable below) |

All of these need the overlay focused. Click it once and it takes them,
without ever coming to the front. **Ctrl+Alt+H works from anywhere**, so you
can always close the overlay even when something else has focus.

If you would rather **Esc** and **Space** reached the overlay from any
application, turn on **Global Esc and Space** in the settings. It is off by
default on purpose: Esc is a heavily used key, and a reflexive press meant for
a dialog or a search box in another window would end the session and release
the keep-awake with nothing on screen to say it had happened.

The overlay sits above your wallpaper but *below* your windows: anything you
open covers it normally, and it never steals focus by itself or appears in
Alt+Tab.

It does cover the desktop icons on the display it runs on, and a click there
goes to the overlay rather than the desktop. That is inherent to sitting above
the wallpaper, and it is why the mod is built around holding a monitor you are
not working on. The **Display** setting defaults to your primary screen, so
pick the side monitor if you would rather keep your icons reachable. It only goes away for good when you press Esc or toggle it off.

## The four styles

Each has its own **parameter** (the wheel) and its own **amount** (right click).
The parameter changes the character of the art; the amount changes how much of
it there is. Amount has five notches: minimal, sparse, balanced, dense, maximal.

| Style | Parameter (wheel) | Amount (right click) |
| --- | --- | --- |
| **Flow field** | turbulence of the underlying field | how tightly ribbons pack: a few broad ones through to many fine ones |
| **Contours** | relief (how rough the terrain is) | number of contour levels, 6 through 46 |
| **Differential growth** | vigor (how hard the colony pushes outward) | number of colonies, 1 through 6 |
| **Harmonograph** | tempo (how fast the figure is drawn) | number of overlaid figures, 1 through 6 |

![The four styles: flow field, contours, differential growth, harmonograph](https://raw.githubusercontent.com/akilluminati47/vector-screen-holder/main/assets/styles.png)

Left to right: flow field, contours, differential growth, harmonograph.

Flow field, growth and harmonograph draw themselves in progressively, hold the
finished piece, fade out and begin a new one. Contours redraw continuously and
drift.

## Keeping the PC awake

While the overlay is up the mod calls `SetThreadExecutionState` with
`ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED`, which is the
supported way to tell Windows the display and system should stay on. The flags
are cleared the moment you close it.

It does **not** fake keystrokes or mouse movement. Some corporate presence
tools (Teams, Slack) track real input rather than display state and will still
mark you away.

## Launching it from a shortcut

The mod listens on a named event, so a shortcut can toggle it. Save this as
`toggle-screen-holder.vbs` anywhere:

```vbs
CreateObject("WScript.Shell").Run "powershell -nop -w hidden -c ""[Threading.EventWaitHandle]::OpenExisting('Local\WindhawkVectorScreenHolderToggle').Set()""", 0, False
```

Then make an ordinary Windows shortcut to that `.vbs` and give it whatever icon
you like. Running it toggles the overlay. `wscript.exe` opens no console, so
nothing flashes on screen.

The event lives in the `Local\` (per-session) namespace, so a second
logged-in user cannot toggle your overlay. It grants `EVENT_MODIFY_STATE` and
`SYNCHRONIZE` only. `SetEvent` needs the first, and `OpenExisting` asks for
both, so the one-liner above fails with "Access to the path is denied" without
it. It is still far short of full access: the event cannot be deleted, nor its
permissions or owner changed. The integrity label is Medium rather than Low:
an ordinary shortcut runs at Medium, and stopping there keeps sandboxed
processes such as browser renderers from reaching it.

## Performance

The overlay is real work on the GPU and CPU, and the whole point is to run it
*while* something else is busy. The heaviest combination by far is **contours
at maximal amount**, which marches 46 iso levels over the sampling grid and
rebuilds 46 path geometries every frame.

If you are holding the screen for a long build or render and want the mod to
stay further out of its way:

- drop **Frames per second** to 30, since motion is paced against the clock
  so it stays smooth rather than becoming choppy;
- or step the **amount** down a notch or two with right click;
- or pick flow field or harmonograph, which draw themselves and then idle,
  rather than contours, which redraw continuously.

## Source, screenshots and the prototype

The mod lives at
[github.com/akilluminati47/vector-screen-holder](https://github.com/akilluminati47/vector-screen-holder),
along with the browser prototype the four algorithms were developed in before
the Direct2D port. There is a short showcase of all four styles at
[vector.akilluminati47.pages.dev](https://vector.akilluminati47.pages.dev/).

## Notes

- Runs as a Windhawk *tool mod* in its own dedicated `windhawk.exe` process, so
  it is never injected into your applications.
- The display numbers come from the `\\.\DISPLAYn` device names. These
  normally line up with the numbers Windows Settings shows, but the two are
  produced by different parts of Windows and can disagree after displays are
  re-arranged. The list of connected displays, with number, resolution,
  position and device name, is written to the mod log when it loads. Use that,
  and
  the resolution in particular, to confirm which display is which.
- Parameter, amount and the current style are remembered across restarts.

## Credits

Originally created by **akilluminati47**, developed with the help of the AI
pair-programmers Claude and Big-Pickle (opencode).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: Ctrl+Alt+H
  $name: Toggle hotkey
  $description: >-
    Global hotkey to show and hide the overlay. Modifiers are Ctrl, Alt, Shift
    and Win, joined with "+". Leave empty to disable.
- monitor: primary
  $name: Display
  $description: >-
    Which display to hold. Any resolution and orientation works, because the
    art is
    generated to fit whatever the display actually is. The numbered choices
    are the display numbers Windows Settings shows (1, 2, 3, ...). The full
    list of connected displays with their Windows numbers and device names is
    written to the mod log when the mod loads.
  $options:
  - primary: Primary display
  - all: All displays
  - "1": Display 1
  - "2": Display 2
  - "3": Display 3
  - "4": Display 4
  - "5": Display 5
  - "6": Display 6
  - "7": Display 7
  - "8": Display 8
- fps: 60
  $name: Frames per second
  $description: >-
    Render rate of the overlay. Higher is smoother; the artwork itself is paced
    by wall-clock time, so raising this makes the motion finer without making
    anything draw faster. Clamped to 10-240.
- enableFlow: true
  $name: "Style: flow field"
  $description: Include this style when cycling with a click or on the rotation timer.
- enableContour: true
  $name: "Style: contours"
- enableGrowth: true
  $name: "Style: differential growth"
- enableHarmonograph: true
  $name: "Style: harmonograph"
- rotate: false
  $name: Rotate through styles
  $description: Automatically move to the next enabled style on a timer.
- rotateSeconds: 300
  $name: Rotation interval (seconds)
  $description: Clamped to 10-7200.
- amount: balanced
  $name: Amount
  $description: >-
    Starting amount notch, meaning how much information is on screen. Right
    click the
    overlay to step it.
  $options:
  - minimal: Minimal
  - sparse: Sparse
  - balanced: Balanced
  - dense: Dense
  - maximal: Maximal
- parameter: 50
  $name: Parameter (%)
  $description: >-
    Starting value for the parameter the mouse wheel adjusts. It is a single
    value carried across the styles rather than one per style, so the notch you
    set for one style is the notch the next one starts from. Clamped to 0-100.
- palette: aurora
  $name: Palette
  $options:
  - aurora: Aurora (teal / blue / violet)
  - ember: Ember (orange / red / gold)
  - ocean: Ocean (cyan / blue)
  - neon: Neon (magenta / cyan / lime)
  - forest: Forest (green / lime)
  - mono: Monochrome
  - custom: Custom (see below)
- customColors: "#29d0a5,#3aa0ff,#7b5cff,#ff5ec4,#e8f3ff"
  $name: Custom colors
  $description: Two to six #rrggbb values, comma separated. Used when the palette is Custom.
- customBackground: "#05070d"
  $name: Custom background
  $description: Background #rrggbb. Used when the palette is Custom.
- colorRamp: false
  $name: Automatic colour ramp
  $description: Continuously rotate the hue of the artwork.
- rampSpeed: 12
  $name: Ramp speed (degrees/sec)
  $description: >-
    Speed of the automatic ramp. Independent of the Space key, so it can be set
    slower or faster than holding Space. Clamped to 1-360.
- spaceSpeed: 90
  $name: Space key speed (degrees/sec)
  $description: How fast holding Space slides the colour. Clamped to 1-720.
- opacity: 100
  $name: Opacity (%)
  $description: Below 100 the desktop shows through the overlay. Clamped to 10-100.
- globalKeys: false
  $name: Global Esc and Space
  $description: >-
    Let Esc close the overlay and Space slide the colour from any application,
    not just when the overlay has focus. Off by default: Esc is a heavily used
    key, and a reflexive press in another window would end the session and
    release the keep-awake without any visible sign. The toggle hotkey above
    always works regardless of this setting.
- keepAwake: true
  $name: Keep the PC awake
  $description: >-
    Hold the display and system out of idle while the overlay is up, via
    SetThreadExecutionState.
- startActive: false
  $name: Start active
  $description: >-
    Show the overlay as soon as the mod loads. Ticking it here does not open
    the overlay straight away; it applies the next time the mod loads. Use the
    hotkey or the shortcut to open it now.
- workAreaOnly: false
  $name: Stay inside the work area
  $description: >-
    Keep the overlay inside each display's work area instead of covering the
    whole display, so it never draws over the taskbar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <windhawk_utils.h>
#include <sddl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Constants and small helpers
// ---------------------------------------------------------------------------

// Declared by hand so the mod never depends on __uuidof or on the import
// library exporting the GUID. Both are toolchain sensitive.
static const GUID kIID_ID2D1Factory = {
    0x06152247, 0x6f50, 0x465a,
    {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};

static const GUID kIID_IDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b,
    {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

static IDWriteFactory* g_dwrite = nullptr;

// Global hue offset in degrees, shared by every overlay so the displays stay
// in step with one another.
static float g_hue = 0;

static const WCHAR kWindowClass[] = L"WindhawkVectorScreenHolderWnd";
static const WCHAR kEventLocal[] = L"Local\\WindhawkVectorScreenHolderToggle";

// Thread hot keys must use 0x0000-0xBFFF; 0xC000+ is reserved for atoms.
static const int kHotkeyId = 0x4A21;
static const float kPi = 3.14159265358979f;

enum StyleId {
    kStyleFlow = 0,
    kStyleContour,
    kStyleGrowth,
    kStyleHarmonograph,
    kStyleCount,
};

static const int kAmountCount = 5;

static const wchar_t* kStyleNames[kStyleCount] = {
    L"flow field", L"contours", L"differential growth", L"harmonograph"};
static const wchar_t* kStyleParams[kStyleCount] = {L"turbulence", L"relief",
                                                   L"vigor", L"tempo"};
static const wchar_t* kAmountNames[kAmountCount] = {
    L"minimal", L"sparse", L"balanced", L"dense", L"maximal"};

template <typename T>
static T ClampT(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

template <typename T>
static void SafeRelease(T** pp) {
    if (*pp) {
        (*pp)->Release();
        *pp = nullptr;
    }
}

// Deterministic LCG, so each display gets its own scene without <random>.
struct Rng {
    unsigned s;
    explicit Rng(unsigned seed) : s(seed ? seed : 1u) {}
    float Next() {
        s = s * 1664525u + 1013904223u;
        return (float)(s >> 8) * (1.0f / 16777216.0f);
    }
    float Range(float a, float b) { return a + (b - a) * Next(); }
    int Int(int a, int b) {
        if (b <= a) {
            return a;
        }
        return a + (int)(Next() * (float)(b - a + 1)) % (b - a + 1);
    }
    float Sign() { return Next() < 0.5f ? -1.0f : 1.0f; }
};

// ---------------------------------------------------------------------------
// Perlin noise (Ken Perlin's improved noise), identical to the prototype
// ---------------------------------------------------------------------------
struct Noise {
    unsigned char p[512];

    void Seed(unsigned seed) {
        unsigned char t[256];
        for (int i = 0; i < 256; i++) {
            t[i] = (unsigned char)i;
        }
        Rng r(seed);
        for (int i = 255; i > 0; i--) {
            int j = r.Int(0, i);
            unsigned char tmp = t[i];
            t[i] = t[j];
            t[j] = tmp;
        }
        for (int i = 0; i < 512; i++) {
            p[i] = t[i & 255];
        }
    }

    static float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    static float Lerp(float t, float a, float b) { return a + t * (b - a); }

    static float Grad(int h, float x, float y, float z) {
        switch (h & 15) {
            case 0: return x + y;
            case 1: return -x + y;
            case 2: return x - y;
            case 3: return -x - y;
            case 4: return x + z;
            case 5: return -x + z;
            case 6: return x - z;
            case 7: return -x - z;
            case 8: return y + z;
            case 9: return -y + z;
            case 10: return y - z;
            case 11: return -y - z;
            case 12: return y + x;
            case 13: return -y + z;
            case 14: return y - x;
            default: return -y - z;
        }
    }

    float N3(float x, float y, float z) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        int Z = (int)std::floor(z) & 255;
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);
        float u = Fade(x), v = Fade(y), w = Fade(z);
        int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
        int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
        return Lerp(w,
                    Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
                                 Grad(p[BA], x - 1, y, z)),
                         Lerp(u, Grad(p[AB], x, y - 1, z),
                              Grad(p[BB], x - 1, y - 1, z))),
                    Lerp(v,
                         Lerp(u, Grad(p[AA + 1], x, y, z - 1),
                              Grad(p[BA + 1], x - 1, y, z - 1)),
                         Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
                              Grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }

    float Fbm(float x, float y, float z, int oct) const {
        float a = 0.5f, f = 1.0f, sum = 0.0f, norm = 0.0f;
        for (int i = 0; i < oct; i++) {
            sum += a * N3(x * f, y * f, z * f);
            norm += a;
            a *= 0.5f;
            f *= 2.0f;
        }
        return norm > 0 ? sum / norm : 0.0f;
    }
};

// ---------------------------------------------------------------------------
// Colour
// ---------------------------------------------------------------------------
struct Rgb {
    float r, g, b;
};

static Rgb RgbFromHex(unsigned v) {
    Rgb c;
    c.r = (float)((v >> 16) & 0xFF) / 255.0f;
    c.g = (float)((v >> 8) & 0xFF) / 255.0f;
    c.b = (float)(v & 0xFF) / 255.0f;
    return c;
}

// Hue rotation applied at stroke time. Direct2D 1.0 has no hue-rotate effect,
// and doing it per stroke is the better look anyway: styles that redraw every
// frame slide through colour wholesale, while styles that accumulate lay a
// gradient through the artwork as the hue drifts.
static Rgb ShiftHue(const Rgb& in, float deg) {
    if (deg == 0.0f) {
        return in;
    }
    float mx = std::max(in.r, std::max(in.g, in.b));
    float mn = std::min(in.r, std::min(in.g, in.b));
    float l = (mx + mn) * 0.5f;
    float h = 0.0f, s = 0.0f;
    if (mx != mn) {
        float d = mx - mn;
        s = l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == in.r) {
            h = (in.g - in.b) / d + (in.g < in.b ? 6.0f : 0.0f);
        } else if (mx == in.g) {
            h = (in.b - in.r) / d + 2.0f;
        } else {
            h = (in.r - in.g) / d + 4.0f;
        }
        h /= 6.0f;
    }
    h += deg / 360.0f;
    h -= std::floor(h);

    Rgb out;
    if (s == 0.0f) {
        out.r = out.g = out.b = l;
        return out;
    }
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    float ch[3] = {h + 1.0f / 3.0f, h, h - 1.0f / 3.0f};
    float res[3];
    for (int i = 0; i < 3; i++) {
        float t = ch[i];
        if (t < 0) t += 1.0f;
        if (t > 1) t -= 1.0f;
        if (t < 1.0f / 6.0f) {
            res[i] = p + (q - p) * 6.0f * t;
        } else if (t < 0.5f) {
            res[i] = q;
        } else if (t < 2.0f / 3.0f) {
            res[i] = p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        } else {
            res[i] = p;
        }
    }
    out.r = res[0];
    out.g = res[1];
    out.b = res[2];
    return out;
}

struct Palette {
    Rgb bg;
    std::vector<Rgb> ink;
};

static D2D1_COLOR_F ToColorF(const Rgb& c, float a) {
    D2D1_COLOR_F o;
    o.r = c.r;
    o.g = c.g;
    o.b = c.b;
    o.a = a;
    return o;
}

static D2D1_POINT_2F Pt(float x, float y) {
    D2D1_POINT_2F p;
    p.x = x;
    p.y = y;
    return p;
}

// ---------------------------------------------------------------------------
// Direct2D geometry helpers. Struct parameters are always passed by pointer
// and points by value, which is the intersection of the MSVC and mingw-w64
// declarations of these interfaces.
// ---------------------------------------------------------------------------
static ID2D1PathGeometry* MakePolyline(ID2D1Factory* factory,
                                       const D2D1_POINT_2F* pts,
                                       UINT32 n,
                                       bool closed) {
    if (!factory || n < 2) {
        return nullptr;
    }
    ID2D1PathGeometry* geom = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geom)) || !geom) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geom->Open(&sink)) || !sink) {
        geom->Release();
        return nullptr;
    }
    sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_HOLLOW);
    for (UINT32 i = 1; i < n; i++) {
        sink->AddLine(pts[i]);
    }
    sink->EndFigure(closed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
    sink->Close();
    sink->Release();
    return geom;
}

// One geometry holding many disconnected two-point figures. Used by the
// contour renderer so a whole iso level is a single DrawGeometry call.
static ID2D1PathGeometry* MakeSegments(ID2D1Factory* factory,
                                       const std::vector<D2D1_POINT_2F>& pts) {
    if (!factory || pts.size() < 2) {
        return nullptr;
    }
    ID2D1PathGeometry* geom = nullptr;
    if (FAILED(factory->CreatePathGeometry(&geom)) || !geom) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geom->Open(&sink)) || !sink) {
        geom->Release();
        return nullptr;
    }
    for (size_t i = 0; i + 1 < pts.size(); i += 2) {
        sink->BeginFigure(pts[i], D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddLine(pts[i + 1]);
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }
    sink->Close();
    sink->Release();
    return geom;
}

// ---------------------------------------------------------------------------
// Scene interface
// ---------------------------------------------------------------------------
struct SceneCtx {
    float w = 0;
    float h = 0;
    ID2D1Factory* factory = nullptr;
    ID2D1RenderTarget* target = nullptr;      // accumulation buffer
    ID2D1SolidColorBrush* brush = nullptr;
    float hue = 0;
    float param = 0.5f;                       // 0..1, the wheel
    int amount = 2;                           // 0..4, right click
    float dt = 1.0f / 60.0f;                  // seconds since the last frame
};

// Converts a per-second rate into whole steps, carrying the remainder across
// frames so the simulation advances at the same speed at any frame rate.
class StepClock {
   public:
    int Take(float ratePerSec, float dt) {
        acc_ += ratePerSec * dt;
        if (acc_ > 240.0f) {
            acc_ = 240.0f;   // never spiral after a stall
        }
        int n = (int)acc_;
        acc_ -= (float)n;
        return n;
    }

   private:
    float acc_ = 0;
};

class Scene {
   public:
    virtual ~Scene() {}
    // Advance the simulation and paint into ctx.target. Returns true when the
    // composition is finished and should be held.
    virtual bool Step(SceneCtx& ctx) = 0;
    // Optional bright overlay drawn straight to the window each frame.
    virtual void PaintCrisp(SceneCtx& ctx, ID2D1RenderTarget* rt) {
        (void)ctx;
        (void)rt;
    }
    // When true the artwork keeps animating through the hold phase. When
    // false the finished composition fades out immediately instead of
    // freezing, so the screen is never a static picture.
    virtual bool Continuous() const { return false; }
};

static void SetInk(SceneCtx& ctx, const Rgb& base, float alpha) {
    Rgb c = ShiftHue(base, ctx.hue);
    D2D1_COLOR_F col = ToColorF(c, alpha);
    ctx.brush->SetColor(&col);
}

// ---------------------------------------------------------------------------
// Style 1: flow field ribbons
//
// Curves are traced up front against a packing grid; a curve that dies before
// a minimum length has the cells it claimed rolled back and is discarded, so
// the field never fills with short stubs. Accepted curves are then drawn in
// progressively as parallel ribbons of fine lines.
// ---------------------------------------------------------------------------
class FlowScene : public Scene {
   public:
    FlowScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed) {
        noise_.Seed(seed);
        w_ = w;
        h_ = h;
        diag_ = std::sqrt(w * w + h * h);

        static const float kSepMul[kAmountCount] = {2.20f, 1.50f, 1.00f, 0.72f,
                                                    0.54f};
        int amt = ClampT(amount, 0, kAmountCount - 1);

        scale_ = rng_.Range(0.0016f, 0.0032f);
        zt_ = rng_.Range(0.0f, 100.0f);
        turns_ = 2.0f;
        sep_ = diag_ * rng_.Range(0.010f, 0.021f) * kSepMul[amt];
        step_ = std::max(2.0f, diag_ * 0.0035f);
        minSteps_ = 32;
        cell_ = sep_ * 0.5f;

        gw_ = (int)(w_ / cell_) + 3;
        gh_ = (int)(h_ / cell_) + 3;
        grid_.assign((size_t)gw_ * gh_, -1);

        // Seed lattice scales with the packing density, so the number of
        // candidate start points is right for any display size.
        int nx = ClampT((int)(w_ / (sep_ * 0.8f)), 16, 200);
        int ny = ClampT((int)(h_ / (sep_ * 0.8f)), 16, 200);
        seeds_.reserve((size_t)nx * ny);
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                D2D1_POINT_2F p =
                    Pt(((float)i + rng_.Range(0.1f, 0.9f)) / nx * w_,
                       ((float)j + rng_.Range(0.1f, 0.9f)) / ny * h_);
                seeds_.push_back(p);
            }
        }
        for (size_t i = seeds_.size(); i > 1; i--) {
            size_t j = (size_t)rng_.Int(0, (int)i - 1);
            std::swap(seeds_[i - 1], seeds_[j]);
        }
        maxCurves_ = ClampT((int)(seeds_.size() / 3), 40, 1200);
        pal_ = &pal;
    }

    bool Step(SceneCtx& ctx) override {
        turns_ = 1.0f + ctx.param * 3.5f;
        // the 30x keeps the wall-clock pacing identical to the prototype
        int steps = clock_.Take((1.0f + ctx.param * 4.0f) * 30.0f, ctx.dt);
        for (int i = 0; i < steps; i++) {
            AdvanceAll(ctx);
        }
        return done_;
    }

   private:
    struct Curve {
        std::vector<D2D1_POINT_2F> spine;
        size_t at = 1;
        int nLines = 3;
        float width = 4;
        float lw = 1;
        float alpha = 0.9f;
        Rgb color;
        std::vector<D2D1_POINT_2F> prev;
        bool hasPrev = false;
    };

    float AngleAt(float x, float y) const {
        return noise_.Fbm(x * scale_, y * scale_, zt_, 3) * kPi * 2.0f * turns_;
    }

    bool Markable(float x, float y, int id) const {
        int cx = (int)(x / cell_) + 1;
        int cy = (int)(y / cell_) + 1;
        if (cx < 1 || cy < 1 || cx >= gw_ - 1 || cy >= gh_ - 1) {
            return false;
        }
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                int v = grid_[(size_t)(cy + j) * gw_ + (cx + i)];
                if (v != -1 && v != id) {
                    return false;
                }
            }
        }
        return true;
    }

    bool Spawn() {
        while (spawnCursor_ < seeds_.size()) {
            D2D1_POINT_2F s = seeds_[spawnCursor_++];
            int id = (int)curves_.size();
            if (!Markable(s.x, s.y, id)) {
                continue;
            }
            float dir = rng_.Sign();
            int maxLife = rng_.Int(260, 900);

            std::vector<D2D1_POINT_2F> spine;
            std::vector<int> claimed;
            spine.push_back(s);
            float x = s.x, y = s.y;
            for (int i = 0; i < maxLife; i++) {
                float a = AngleAt(x, y);
                float nx = x + std::cos(a) * step_ * dir;
                float ny = y + std::sin(a) * step_ * dir;
                if (!Markable(nx, ny, id)) {
                    break;
                }
                int cx = (int)(nx / cell_) + 1;
                int cy = (int)(ny / cell_) + 1;
                int idx = cy * gw_ + cx;
                if (grid_[idx] == -1) {
                    claimed.push_back(idx);
                }
                grid_[idx] = id;
                spine.push_back(Pt(nx, ny));
                x = nx;
                y = ny;
            }
            if ((int)spine.size() < minSteps_) {
                for (size_t i = 0; i < claimed.size(); i++) {
                    grid_[claimed[i]] = -1;   // roll back, try another seed
                }
                continue;
            }

            Curve c;
            c.spine.swap(spine);
            c.nLines = rng_.Int(2, 4);
            c.width = sep_ * rng_.Range(0.55f, 0.92f);
            c.lw = rng_.Range(0.75f, 1.25f);
            c.alpha = rng_.Range(0.72f, 1.0f);
            c.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            curves_.push_back(c);
            active_.push_back((int)curves_.size() - 1);
            return true;
        }
        return false;
    }

    void AdvanceAll(SceneCtx& ctx) {
        while ((int)active_.size() < 26 && (int)curves_.size() < maxCurves_) {
            if (!Spawn()) {
                break;
            }
        }
        if (active_.empty()) {
            done_ = true;
            return;
        }
        for (int k = (int)active_.size() - 1; k >= 0; k--) {
            Curve& c = curves_[active_[k]];
            if (c.at >= c.spine.size()) {
                active_.erase(active_.begin() + k);
                continue;
            }
            D2D1_POINT_2F n = c.spine[c.at];
            D2D1_POINT_2F p = c.spine[c.at - 1];
            float dx = n.x - p.x, dy = n.y - p.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-4f) {
                len = 1.0f;
            }
            float ox = -dy / len, oy = dx / len;
            float taper =
                std::sin(((float)c.at / (float)c.spine.size()) * kPi);
            float halfW = c.width * (0.35f + 0.65f * taper);

            std::vector<D2D1_POINT_2F> pts;
            pts.reserve(c.nLines);
            for (int i = 0; i < c.nLines; i++) {
                float t = c.nLines == 1
                              ? 0.0f
                              : ((float)i / (c.nLines - 1) - 0.5f) * 2.0f;
                pts.push_back(Pt(n.x + ox * halfW * t, n.y + oy * halfW * t));
            }

            if (c.hasPrev) {
                SetInk(ctx, c.color, c.alpha);
                for (size_t i = 0; i < pts.size() && i < c.prev.size(); i++) {
                    ctx.target->DrawLine(c.prev[i], pts[i], ctx.brush, c.lw,
                                         nullptr);
                }
            }
            c.prev = pts;
            c.hasPrev = true;
            c.at++;
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0, diag_ = 0;
    float scale_ = 0, zt_ = 0, turns_ = 2, sep_ = 10, step_ = 3, cell_ = 5;
    int minSteps_ = 32;
    int gw_ = 0, gh_ = 0;
    int maxCurves_ = 400;
    std::vector<int> grid_;
    std::vector<D2D1_POINT_2F> seeds_;
    size_t spawnCursor_ = 0;
    std::vector<Curve> curves_;
    std::vector<int> active_;
    StepClock clock_;
    bool done_ = false;
};

// ---------------------------------------------------------------------------
// Style 2: topographic contours (marching squares over a warped fbm field)
//
// The sampling grid is area budgeted rather than tied to pixel count, so the
// per-frame cost is the same on a 1080p panel and a 4K one; only the cell size
// changes. The field is resampled every third frame because it drifts slowly.
// ---------------------------------------------------------------------------
class ContourScene : public Scene {
   public:
    static int LevelsFor(int amount) {
        static const int kLevels[kAmountCount] = {6, 12, 21, 32, 46};
        return kLevels[ClampT(amount, 0, kAmountCount - 1)];
    }

    ContourScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed ^ 0x9e37u) {
        noise_.Seed(seed ^ 0x9e37u);
        w_ = w;
        h_ = h;
        pal_ = &pal;

        // Fixed sample budget -> resolution independent cost.
        const float kBudget = 18000.0f;
        float aspect = (h > 0) ? (w / h) : 1.0f;
        cols_ = ClampT((int)std::sqrt(kBudget * aspect), 40, 260);
        rows_ = ClampT((int)(kBudget / std::max(1, cols_)), 30, 200);
        cw_ = w_ / cols_;
        ch_ = h_ / rows_;
        field_.assign((size_t)(cols_ + 1) * (rows_ + 1), 0.0f);

        levels_ = LevelsFor(amount);
        scale_ = rng_.Range(2.2f, 3.6f);
        z_ = rng_.Range(0.0f, 50.0f);
        warp_ = 0.3f;

        fieldA_.assign(field_.size(), 0.0f);
        fieldB_.assign(field_.size(), 0.0f);
        zA_ = z_;
        zB_ = z_ + kDz;
        SampleInto(fieldA_, zA_);
        SampleInto(fieldB_, zB_);
        blend_ = 0.0f;
    }

    bool Step(SceneCtx& ctx) override {
        warp_ = 0.10f + ctx.param * 0.80f;
        levels_ = LevelsFor(ctx.amount);

        Advance(ctx.dt);
        age_ += ctx.dt;

        // Contours are a full redraw each frame.
        D2D1_COLOR_F clear = ToColorF(pal_->bg, 0.0f);
        ctx.target->Clear(&clear);

        const size_t inkN = pal_->ink.size();
        for (int k = 0; k < levels_; k++) {
            float t = levels_ > 1 ? (float)k / (levels_ - 1) : 0.0f;
            float level = -0.42f + t * 0.84f;
            bool emph = (k % 5 == 0);

            // blend across the palette by depth so the map reads as one system
            float fi = t * (float)(inkN - 1);
            int i0 = ClampT((int)fi, 0, (int)inkN - 1);
            int i1 = ClampT(i0 + 1, 0, (int)inkN - 1);
            float ft = fi - i0;
            Rgb c;
            c.r = pal_->ink[i0].r + (pal_->ink[i1].r - pal_->ink[i0].r) * ft;
            c.g = pal_->ink[i0].g + (pal_->ink[i1].g - pal_->ink[i0].g) * ft;
            c.b = pal_->ink[i0].b + (pal_->ink[i1].b - pal_->ink[i0].b) * ft;

            segs_.clear();
            March(level);
            if (segs_.empty()) {
                continue;
            }
            SetInk(ctx, c, emph ? 0.95f : 0.38f);
            ID2D1PathGeometry* g = MakeSegments(ctx.factory, segs_);
            if (g) {
                ctx.target->DrawGeometry(g, ctx.brush, emph ? 1.15f : 0.6f,
                                         nullptr);
                g->Release();
            }
        }
        return age_ > kLifeSecs;
    }

    bool Continuous() const override { return true; }

   private:
    void SampleInto(std::vector<float>& dst, float z) {
        float s = scale_;
        for (int j = 0; j <= rows_; j++) {
            float v = (float)j / rows_;
            for (int i = 0; i <= cols_; i++) {
                float u = (float)i / cols_;
                float wx = noise_.Fbm(u * s + 5.2f, v * s + 1.3f, z, 2) * warp_;
                float wy = noise_.Fbm(u * s + 9.7f, v * s + 4.1f, z, 2) * warp_;
                dst[(size_t)j * (cols_ + 1) + i] =
                    noise_.Fbm(u * s + wx, v * s + wy, z, 4);
            }
        }
    }

    // Blend between the two sampled fields every frame, rolling forward when
    // the blend completes. Noise is evaluated twice a second; the contours
    // themselves move continuously.
    void Advance(float dt) {
        blend_ += dt / kCycleSecs;
        while (blend_ >= 1.0f) {
            blend_ -= 1.0f;
            fieldA_.swap(fieldB_);
            zA_ = zB_;
            zB_ = zA_ + kDz;
            SampleInto(fieldB_, zB_);
        }
        float t = ClampT(blend_, 0.0f, 1.0f);
        for (size_t i = 0; i < field_.size(); i++) {
            field_[i] = fieldA_[i] + (fieldB_[i] - fieldA_[i]) * t;
        }
    }

    float At(int i, int j) const {
        return field_[(size_t)j * (cols_ + 1) + i];
    }

    void Emit(float ax, float ay, float bx, float by) {
        segs_.push_back(Pt(ax, ay));
        segs_.push_back(Pt(bx, by));
    }

    void March(float level) {
        for (int j = 0; j < rows_; j++) {
            for (int i = 0; i < cols_; i++) {
                float v0 = At(i, j), v1 = At(i + 1, j);
                float v2 = At(i + 1, j + 1), v3 = At(i, j + 1);
                int code = 0;
                if (v0 > level) code |= 1;
                if (v1 > level) code |= 2;
                if (v2 > level) code |= 4;
                if (v3 > level) code |= 8;
                if (code == 0 || code == 15) {
                    continue;
                }
                float x0 = i * cw_, y0 = j * ch_;
                float x1 = x0 + cw_, y1 = y0 + ch_;
                // linear interpolation along each crossed edge
                float tT = (level - v0) / ((v1 - v0) != 0 ? (v1 - v0) : 1e-6f);
                float tR = (level - v1) / ((v2 - v1) != 0 ? (v2 - v1) : 1e-6f);
                float tB = (level - v3) / ((v2 - v3) != 0 ? (v2 - v3) : 1e-6f);
                float tL = (level - v0) / ((v3 - v0) != 0 ? (v3 - v0) : 1e-6f);
                float Tx = x0 + (x1 - x0) * tT, Ty = y0;
                float Rx = x1, Ry = y0 + (y1 - y0) * tR;
                float Bx = x0 + (x1 - x0) * tB, By = y1;
                float Lx = x0, Ly = y0 + (y1 - y0) * tL;
                switch (code) {
                    case 1: case 14: Emit(Lx, Ly, Tx, Ty); break;
                    case 2: case 13: Emit(Tx, Ty, Rx, Ry); break;
                    case 3: case 12: Emit(Lx, Ly, Rx, Ry); break;
                    case 4: case 11: Emit(Rx, Ry, Bx, By); break;
                    case 6: case 9:  Emit(Tx, Ty, Bx, By); break;
                    case 7: case 8:  Emit(Lx, Ly, Bx, By); break;
                    case 5:
                        Emit(Lx, Ly, Tx, Ty);
                        Emit(Rx, Ry, Bx, By);
                        break;
                    case 10:
                        Emit(Tx, Ty, Rx, Ry);
                        Emit(Lx, Ly, Bx, By);
                        break;
                    default: break;
                }
            }
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0, cw_ = 1, ch_ = 1;
    int cols_ = 0, rows_ = 0, levels_ = 21;
    float scale_ = 3, z_ = 0, warp_ = 0.3f;
    float age_ = 0;
    static constexpr float kLifeSecs = 300.0f;   // wall clock, not frames
    static constexpr float kCycleSecs = 0.5f;
    static constexpr float kDz = 0.015f;
    float zA_ = 0, zB_ = 0, blend_ = 0;
    std::vector<float> field_, fieldA_, fieldB_;
    std::vector<D2D1_POINT_2F> segs_;
};

// ---------------------------------------------------------------------------
// Style 3: differential growth
//
// Closed loops of nodes that attract along the curve and repel through a
// shared spatial hash, subdividing as they stretch. Every step is stamped into
// the buffer at low alpha so the history of the growth accumulates, with the
// live outline stroked bright on top.
// ---------------------------------------------------------------------------
class GrowthScene : public Scene {
   public:
    static int LoopsFor(int amount) {
        static const int kLoops[kAmountCount] = {1, 2, 3, 4, 6};
        return kLoops[ClampT(amount, 0, kAmountCount - 1)];
    }

    GrowthScene(float w, float h, unsigned seed, const Palette& pal, int amount)
        : rng_(seed ^ 0x51edu) {
        noise_.Seed(seed ^ 0x51edu);
        w_ = w;
        h_ = h;
        pal_ = &pal;
        float m = std::min(w, h);

        maxLen_ = m * 0.0085f;
        baseRepel_ = maxLen_ * 2.4f;
        repel_ = baseRepel_;
        cellSize_ = baseRepel_ * 1.6f;
        zt_ = rng_.Range(0.0f, 40.0f);

        int nLoops = LoopsFor(amount);
        // Budget per colony, so one colony is a small clean form rather than
        // spending the whole budget in one place and saturating.
        maxNodes_ = std::min(7000, 1500 * nLoops);

        for (int k = 0; k < nLoops; k++) {
            float cx = 0, cy = 0;
            float minSep = m * (0.62f / std::max(1, nLoops - 1));
            for (int tries = 0; tries < 40; tries++) {
                cx = rng_.Range(w_ * 0.16f, w_ * 0.84f);
                cy = rng_.Range(h_ * 0.18f, h_ * 0.82f);
                bool ok = true;
                for (size_t i = 0; i < loops_.size(); i++) {
                    float dx = cx - loops_[i].cx, dy = cy - loops_[i].cy;
                    if (std::sqrt(dx * dx + dy * dy) < minSep) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    break;
                }
            }
            float rad = m * rng_.Range(0.045f, 0.075f);
            int n = std::max(24, (int)std::ceil(2 * kPi * rad /
                                                (maxLen_ * 1.35f)));
            Loop L;
            L.cx = cx;
            L.cy = cy;
            L.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            L.lw = rng_.Range(0.5f, 0.9f);
            L.nodes.resize(n);
            for (int i = 0; i < n; i++) {
                float a = (float)i / n * 2 * kPi;
                L.nodes[i].x = cx + std::cos(a) * rad;
                L.nodes[i].y = cy + std::sin(a) * rad;
                L.nodes[i].vx = 0;
                L.nodes[i].vy = 0;
            }
            loops_.push_back(L);
        }
        gw_ = std::max(1, (int)(w_ / cellSize_) + 2);
        gh_ = std::max(1, (int)(h_ / cellSize_) + 2);
    }

    bool Step(SceneCtx& ctx) override {
        repel_ = baseRepel_ * (0.70f + ctx.param * 1.20f);
        int steps = clock_.Take((1.0f + ctx.param * 4.0f) * 30.0f, ctx.dt);
        for (int i = 0; i < steps; i++) {
            Simulate();
        }
        // Stamp the trail on the step budget rather than once per frame, so
        // the accumulated density is the same at any frame rate.
        if (steps > 0) {
            Paint(ctx, ctx.target, 0.05f * (float)steps / 5.0f);
        }
        return Total() >= maxNodes_;
    }

    void PaintCrisp(SceneCtx& ctx, ID2D1RenderTarget* rt) override {
        Paint(ctx, rt, 0.85f);
    }

    bool Continuous() const override { return true; }

   private:
    struct Node {
        float x, y, vx, vy;
    };
    struct Loop {
        std::vector<Node> nodes;
        float cx = 0, cy = 0, lw = 0.7f;
        Rgb color;
    };

    int Total() const {
        int t = 0;
        for (size_t i = 0; i < loops_.size(); i++) {
            t += (int)loops_[i].nodes.size();
        }
        return t;
    }

    void Simulate() {
        // Rebuild a shared bucket grid so loops repel each other as well as
        // themselves. Linked-list buckets avoid per-step allocation.
        int total = Total();
        flat_.clear();
        flat_.reserve(total);
        for (size_t li = 0; li < loops_.size(); li++) {
            for (size_t ni = 0; ni < loops_[li].nodes.size(); ni++) {
                flat_.push_back(&loops_[li].nodes[ni]);
            }
        }
        head_.assign((size_t)gw_ * gh_, -1);
        next_.assign(flat_.size(), -1);
        for (size_t i = 0; i < flat_.size(); i++) {
            int cx = ClampT((int)(flat_[i]->x / cellSize_), 0, gw_ - 1);
            int cy = ClampT((int)(flat_[i]->y / cellSize_), 0, gh_ - 1);
            int c = cy * gw_ + cx;
            next_[i] = head_[c];
            head_[c] = (int)i;
        }

        float r2 = repel_ * repel_;
        for (size_t li = 0; li < loops_.size(); li++) {
            std::vector<Node>& N = loops_[li].nodes;
            int n = (int)N.size();
            if (n < 3) {
                continue;
            }
            for (int i = 0; i < n; i++) {
                Node& a = N[i];
                const Node& prev = N[(i - 1 + n) % n];
                const Node& nx = N[(i + 1) % n];
                float fx = (prev.x + nx.x - 2 * a.x) * 0.20f;
                float fy = (prev.y + nx.y - 2 * a.y) * 0.20f;

                int bx = ClampT((int)(a.x / cellSize_), 0, gw_ - 1);
                int by = ClampT((int)(a.y / cellSize_), 0, gh_ - 1);
                for (int dy = -1; dy <= 1; dy++) {
                    int yy = by + dy;
                    if (yy < 0 || yy >= gh_) {
                        continue;
                    }
                    for (int dx = -1; dx <= 1; dx++) {
                        int xx = bx + dx;
                        if (xx < 0 || xx >= gw_) {
                            continue;
                        }
                        for (int idx = head_[(size_t)yy * gw_ + xx]; idx != -1;
                             idx = next_[idx]) {
                            const Node* o = flat_[idx];
                            if (o == &a) {
                                continue;
                            }
                            float ox = a.x - o->x, oy = a.y - o->y;
                            float d2 = ox * ox + oy * oy;
                            if (d2 < 1e-6f || d2 > r2) {
                                continue;
                            }
                            float d = std::sqrt(d2);
                            float f = (1.0f - d / repel_) * 1.35f;
                            fx += ox / d * f;
                            fy += oy / d * f;
                        }
                    }
                }
                float nz = noise_.Fbm(a.x * 0.0045f, a.y * 0.0045f, zt_, 2);
                fx += std::cos(nz * 14.0f) * 0.16f;
                fy += std::sin(nz * 14.0f) * 0.16f;
                a.vx = (a.vx + fx) * 0.60f;
                a.vy = (a.vy + fy) * 0.60f;
            }
            for (int i = 0; i < n; i++) {
                N[i].x = ClampT(N[i].x + N[i].vx, 6.0f, w_ - 6.0f);
                N[i].y = ClampT(N[i].y + N[i].vy, 6.0f, h_ - 6.0f);
            }
        }

        if (Total() < maxNodes_) {
            int grew = 0;
            for (size_t li = 0; li < loops_.size(); li++) {
                std::vector<Node>& N = loops_[li].nodes;
                for (int i = (int)N.size() - 1; i >= 0; i--) {
                    const Node& a = N[i];
                    const Node& b = N[(i + 1) % (int)N.size()];
                    float dx = b.x - a.x, dy = b.y - a.y;
                    if (std::sqrt(dx * dx + dy * dy) > maxLen_) {
                        Node m;
                        m.x = (a.x + b.x) * 0.5f;
                        m.y = (a.y + b.y) * 0.5f;
                        m.vx = 0;
                        m.vy = 0;
                        N.insert(N.begin() + i + 1, m);
                        grew++;
                    }
                }
            }
            // Never let a colony sit at equilibrium and stall: force the
            // longest segment open if nothing subdivided this step.
            if (grew == 0) {
                for (size_t li = 0; li < loops_.size(); li++) {
                    std::vector<Node>& N = loops_[li].nodes;
                    if (N.size() < 3) {
                        continue;
                    }
                    int best = -1;
                    float bestD = -1;
                    for (int i = 0; i < (int)N.size(); i++) {
                        const Node& a = N[i];
                        const Node& b = N[(i + 1) % (int)N.size()];
                        float dx = b.x - a.x, dy = b.y - a.y;
                        float d = std::sqrt(dx * dx + dy * dy);
                        if (d > bestD) {
                            bestD = d;
                            best = i;
                        }
                    }
                    if (best >= 0) {
                        const Node& a = N[best];
                        const Node& b = N[(best + 1) % (int)N.size()];
                        Node m;
                        m.x = (a.x + b.x) * 0.5f;
                        m.y = (a.y + b.y) * 0.5f;
                        m.vx = 0;
                        m.vy = 0;
                        N.insert(N.begin() + best + 1, m);
                    }
                }
            }
        }
        zt_ += 0.0025f;
    }

    void Paint(SceneCtx& ctx, ID2D1RenderTarget* rt, float alpha) {
        for (size_t li = 0; li < loops_.size(); li++) {
            const std::vector<Node>& N = loops_[li].nodes;
            if (N.size() < 3) {
                continue;
            }
            pts_.clear();
            pts_.reserve(N.size());
            for (size_t i = 0; i < N.size(); i++) {
                pts_.push_back(Pt(N[i].x, N[i].y));
            }
            SetInk(ctx, loops_[li].color, alpha);
            ID2D1PathGeometry* g =
                MakePolyline(ctx.factory, pts_.data(), (UINT32)pts_.size(), true);
            if (g) {
                rt->DrawGeometry(g, ctx.brush, loops_[li].lw, nullptr);
                g->Release();
            }
        }
    }

    Rng rng_;
    Noise noise_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0;
    float maxLen_ = 4, repel_ = 8, baseRepel_ = 8, cellSize_ = 12, zt_ = 0;
    int maxNodes_ = 4500;
    int gw_ = 1, gh_ = 1;
    std::vector<Loop> loops_;
    std::vector<Node*> flat_;
    std::vector<int> head_, next_;
    std::vector<D2D1_POINT_2F> pts_;
    StepClock clock_;
};

// ---------------------------------------------------------------------------
// Style 4: harmonograph
//
// Two decaying pendulums per axis. Integer frequency ratios read as deliberate
// figures; a single shared decay per figure makes it spiral inward
// self-similarly instead of collapsing to a flat lens.
// ---------------------------------------------------------------------------
class HarmonographScene : public Scene {
   public:
    static int FiguresFor(int amount) {
        static const int kFigs[kAmountCount] = {1, 2, 3, 4, 6};
        return kFigs[ClampT(amount, 0, kAmountCount - 1)];
    }

    HarmonographScene(float w,
                      float h,
                      unsigned seed,
                      const Palette& pal,
                      int amount)
        : rng_(seed ^ 0x2f19u) {
        w_ = w;
        h_ = h;
        pal_ = &pal;
        float m = std::min(w, h);
        int count = FiguresFor(amount);
        for (int k = 0; k < count; k++) {
            Fig f;
            int fx = rng_.Int(1, 5);
            int fy = rng_.Int(1, 5);
            if (fy == fx) {
                fy = (fx % 5) + 1;   // equal ratios collapse to a flat lens
            }
            float detune = rng_.Range(0.002f, 0.010f) * rng_.Sign();
            f.a[0] = rng_.Range(0.26f, 0.40f) * m;
            f.a[1] = rng_.Range(0.10f, 0.22f) * m;
            f.a[2] = rng_.Range(0.26f, 0.40f) * m;
            f.a[3] = rng_.Range(0.10f, 0.22f) * m;
            f.f[0] = (float)fx;
            f.f[1] = (float)fx + detune;
            f.f[2] = (float)fy;
            f.f[3] = (float)fy + detune * rng_.Sign();
            for (int i = 0; i < 4; i++) {
                f.p[i] = rng_.Range(0.0f, 2 * kPi);
            }
            float dd = rng_.Range(0.0006f, 0.0012f);
            for (int i = 0; i < 4; i++) {
                f.d[i] = dd;
            }
            f.color = pal_->ink[(size_t)rng_.Int(0, (int)pal_->ink.size() - 1)];
            f.lw = rng_.Range(0.45f, 0.75f);
            f.alpha = rng_.Range(0.22f, 0.42f);
            f.t = 0;
            f.tEnd = rng_.Range(2000.0f, 2800.0f);
            f.hasLast = false;
            figs_.push_back(f);
        }
    }

    bool Step(SceneCtx& ctx) override {
        int steps = clock_.Take((4.0f + ctx.param * 60.0f) * 30.0f, ctx.dt);
        int alive = 0;
        for (size_t k = 0; k < figs_.size(); k++) {
            Fig& f = figs_[k];
            if (f.t >= f.tEnd) {
                continue;
            }
            alive++;
            pts_.clear();
            if (f.hasLast) {
                pts_.push_back(f.last);
            }
            for (int i = 0; i < steps && f.t < f.tEnd; i++, f.t += 0.22f) {
                D2D1_POINT_2F p = PointAt(f, f.t);
                pts_.push_back(p);
                f.last = p;
                f.hasLast = true;
            }
            if (pts_.size() < 2) {
                continue;
            }
            SetInk(ctx, f.color, f.alpha);
            ID2D1PathGeometry* g = MakePolyline(ctx.factory, pts_.data(),
                                                (UINT32)pts_.size(), false);
            if (g) {
                ctx.target->DrawGeometry(g, ctx.brush, f.lw, nullptr);
                g->Release();
            }
        }
        return alive == 0;
    }

   private:
    struct Fig {
        float a[4], f[4], p[4], d[4];
        Rgb color;
        float lw, alpha, t, tEnd;
        D2D1_POINT_2F last;
        bool hasLast;
    };

    D2D1_POINT_2F PointAt(const Fig& f, float t) const {
        float x = f.a[0] * std::sin(f.f[0] * t * 0.05f + f.p[0]) *
                      std::exp(-f.d[0] * t) +
                  f.a[1] * std::sin(f.f[1] * t * 0.05f + f.p[1]) *
                      std::exp(-f.d[1] * t);
        float y = f.a[2] * std::sin(f.f[2] * t * 0.05f + f.p[2]) *
                      std::exp(-f.d[2] * t) +
                  f.a[3] * std::sin(f.f[3] * t * 0.05f + f.p[3]) *
                      std::exp(-f.d[3] * t);
        return Pt(w_ * 0.5f + x, h_ * 0.5f + y);
    }

    Rng rng_;
    const Palette* pal_ = nullptr;
    float w_ = 0, h_ = 0;
    std::vector<Fig> figs_;
    std::vector<D2D1_POINT_2F> pts_;
    StepClock clock_;
};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct Settings {
    std::wstring monitor = L"primary";
    bool enable[kStyleCount] = {true, true, true, true};
    bool rotate = false;
    int rotateSeconds = 300;
    int amount = 2;
    int parameter = 50;
    std::wstring palette = L"aurora";
    std::wstring customColors;
    std::wstring customBackground;
    bool colorRamp = false;
    int rampSpeed = 12;
    int spaceSpeed = 90;
    int fps = 60;
    int opacity = 100;
    bool globalKeys = false;
    bool keepAwake = true;
    bool startActive = false;
    bool workAreaOnly = false;
    std::wstring hotkey = L"Ctrl+Alt+H";
};

static Settings g_settings;
static Palette g_palette;

static unsigned ParseHex(const std::wstring& s) {
    unsigned v = 0;
    for (size_t i = 0; i < s.size(); i++) {
        wchar_t c = s[i];
        int d = -1;
        if (c >= L'0' && c <= L'9') d = c - L'0';
        else if (c >= L'a' && c <= L'f') d = 10 + (c - L'a');
        else if (c >= L'A' && c <= L'F') d = 10 + (c - L'A');
        else continue;
        v = v * 16 + d;
    }
    return v;
}

static void BuildPalette() {
    struct Preset {
        const wchar_t* name;
        unsigned bg;
        unsigned ink[5];
    };
    static const Preset kPresets[] = {
        {L"aurora", 0x05070d, {0x7fe7cf, 0x5fb3ff, 0xa68bff, 0xff7fd0, 0xe8f3ff}},
        {L"ember",  0x0d0603, {0xffb066, 0xff6a3d, 0xffd98a, 0xe0503a, 0xfff0d8}},
        {L"ocean",  0x02080f, {0x4fd1f5, 0x59a5ff, 0x8fe9ff, 0x3b6fd4, 0xdff6ff}},
        {L"neon",   0x05010a, {0xff2ec4, 0x00f0ff, 0xb026ff, 0x39ff14, 0xffffff}},
        {L"forest", 0x030a06, {0x34d399, 0xa3e635, 0x059669, 0xd9f99d, 0xecfccb}},
        {L"mono",   0x07070a, {0xe8e9ee, 0xb9bcc6, 0x8a8f9c, 0x5e636f, 0xffffff}},
    };

    g_palette.ink.clear();

    if (g_settings.palette == L"custom") {
        g_palette.bg = RgbFromHex(g_settings.customBackground.empty()
                                      ? 0x05070d
                                      : ParseHex(g_settings.customBackground));
        std::wstring s = g_settings.customColors;
        size_t start = 0;
        while (start <= s.size() && g_palette.ink.size() < 6) {
            size_t comma = s.find(L',', start);
            std::wstring tok = s.substr(
                start, comma == std::wstring::npos ? std::wstring::npos
                                                   : comma - start);
            if (!tok.empty()) {
                g_palette.ink.push_back(RgbFromHex(ParseHex(tok)));
            }
            if (comma == std::wstring::npos) {
                break;
            }
            start = comma + 1;
        }
        if (g_palette.ink.size() >= 2) {
            return;
        }
        // fall through to the default if the custom list was unusable
        g_palette.ink.clear();
    }

    const Preset* chosen = &kPresets[0];
    for (size_t i = 0; i < sizeof(kPresets) / sizeof(kPresets[0]); i++) {
        if (g_settings.palette == kPresets[i].name) {
            chosen = &kPresets[i];
            break;
        }
    }
    g_palette.bg = RgbFromHex(chosen->bg);
    for (int i = 0; i < 5; i++) {
        g_palette.ink.push_back(RgbFromHex(chosen->ink[i]));
    }
}

// ---------------------------------------------------------------------------
// Monitors
// ---------------------------------------------------------------------------
struct MonitorEntry {
    RECT rect;              // full monitor bounds
    RECT work;              // bounds minus the taskbar / work area
    bool primary;
    std::wstring device;        // \\.\DISPLAY2
    std::wstring deviceName;    // friendly name, e.g. DELL U2720Q
    int winNum = 0;             // the number in \\.\DISPLAY<n>, as Windows shows it
};

static int DisplayNumberFromDevice(const std::wstring& dev) {
    // "\\.\DISPLAY2" -> 2; the number Windows Settings shows.
    size_t p = dev.rfind(L"DISPLAY");
    if (p == std::wstring::npos) {
        return 0;
    }
    return _wtoi(dev.c_str() + p + 7);
}

static BOOL CALLBACK EnumMonProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    std::vector<MonitorEntry>* out = (std::vector<MonitorEntry>*)lParam;
    MONITORINFOEXW mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorEntry e;
        e.rect = mi.rcMonitor;
        e.work = mi.rcWork;
        e.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        e.device = mi.szDevice;
        e.winNum = DisplayNumberFromDevice(e.device);
        DISPLAY_DEVICEW dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        if (EnumDisplayDevicesW(mi.szDevice, 0, &dd, 0)) {
            e.deviceName = dd.DeviceString;
        }
        out->push_back(e);
    }
    return TRUE;
}

static std::vector<MonitorEntry> EnumerateMonitors() {
    std::vector<MonitorEntry> list;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, (LPARAM)&list);
    std::sort(list.begin(), list.end(),
              [](const MonitorEntry& a, const MonitorEntry& b) {
                  if (a.rect.left != b.rect.left) {
                      return a.rect.left < b.rect.left;
                  }
                  return a.rect.top < b.rect.top;
              });
    return list;
}

// ---------------------------------------------------------------------------
// Overlay: one per held display
// ---------------------------------------------------------------------------
enum Phase { kPhaseIn, kPhaseBuild, kPhaseHold, kPhaseOut };

// How long the readout stays up, and how much of that is the fade.
static const float kHudSecs = 2.2f;
static const float kHudFade = 0.6f;

class Overlay {
   public:
    Overlay(ID2D1Factory* factory, const RECT& rc, unsigned seed)
        : factory_(factory), rect_(rc), seed_(seed) {}

    ~Overlay() {
        Destroy();
        SafeRelease(&hudFormat_);
    }

    bool Create();
    void Destroy();
    void Render(float dtSec);
    void NewScene();
    // Briefly show what just changed. The overlay is otherwise completely
    // clean, and this is the only text it ever draws.
    void FlashHud();

    int style = kStyleFlow;
    int amount = 2;
    float param = 0.5f;

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

   private:
    bool CreateDeviceResources();
    void DiscardDeviceResources();

    ID2D1Factory* factory_ = nullptr;
    HWND hwnd_ = nullptr;
    RECT rect_;
    unsigned seed_ = 1;

    ID2D1HwndRenderTarget* rt_ = nullptr;
    ID2D1BitmapRenderTarget* buf_ = nullptr;
    ID2D1Bitmap* bufBitmap_ = nullptr;
    ID2D1SolidColorBrush* brush_ = nullptr;

    std::unique_ptr<Scene> scene_;
    IDWriteTextFormat* hudFormat_ = nullptr;
    std::wstring hudText_;
    float hudT_ = 0;

    Phase phase_ = kPhaseIn;
    float phaseT_ = 0;
    float artAlpha_ = 0;
    float holdT_ = 0;
};

// forward declarations for the controller hooks the window proc calls into
static void Controller_CycleStyle(Overlay* ov);
static void Controller_StepAmount(Overlay* ov);
static void Controller_Wheel(Overlay* ov, int delta);
static void Controller_RequestRebuild();
static void Controller_RequestClose();
static void Controller_SetSpace(bool down);

bool Overlay::Create() {
    // Bottom of the z-order: it sits above the wallpaper but under every
    // application window. Focusable on click so keyboard input reaches it, but
    // it is never raised.
    hwnd_ = CreateWindowExW(
        WS_EX_TOOLWINDOW, kWindowClass, L"",
        WS_POPUP, rect_.left, rect_.top, rect_.right - rect_.left,
        rect_.bottom - rect_.top, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!hwnd_) {
        Wh_Log(L"CreateWindowEx failed (%u)", GetLastError());
        return false;
    }
    SetWindowLongPtrW(hwnd_, GWLP_USERDATA, (LONG_PTR)this);

    if (g_settings.opacity < 100) {
        SetWindowLongPtrW(hwnd_, GWL_EXSTYLE,
                          GetWindowLongPtrW(hwnd_, GWL_EXSTYLE) |
                              WS_EX_LAYERED);
        BYTE a = (BYTE)(255 * g_settings.opacity / 100);
        SetLayeredWindowAttributes(hwnd_, 0, a, LWA_ALPHA);
    }

    if (!CreateDeviceResources()) {
        return false;
    }
    NewScene();

    ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    SetWindowPos(hwnd_, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return true;
}

void Overlay::Destroy() {
    DiscardDeviceResources();
    scene_.reset();
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

bool Overlay::CreateDeviceResources() {
    if (rt_ && buf_ && brush_) {
        return true;
    }
    DiscardDeviceResources();   // never leave a half-built set behind
    D2D1_RENDER_TARGET_PROPERTIES props;
    ZeroMemory(&props, sizeof(props));
    props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    // 96 dpi so that one device independent pixel is one physical pixel,
    // whatever the display's scaling is.
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    D2D1_HWND_RENDER_TARGET_PROPERTIES hprops;
    ZeroMemory(&hprops, sizeof(hprops));
    hprops.hwnd = hwnd_;
    hprops.pixelSize.width = (UINT32)(rect_.right - rect_.left);
    hprops.pixelSize.height = (UINT32)(rect_.bottom - rect_.top);
    hprops.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

    if (FAILED(factory_->CreateHwndRenderTarget(&props, &hprops, &rt_))) {
        Wh_Log(L"CreateHwndRenderTarget failed");
        DiscardDeviceResources();
        return false;
    }
    rt_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    // The accumulation buffer must carry alpha even though the window does
    // not, so the artwork can be composited at a fade opacity.
    D2D1_PIXEL_FORMAT fmt;
    fmt.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    fmt.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    if (FAILED(rt_->CreateCompatibleRenderTarget(
            nullptr, nullptr, &fmt,
            D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &buf_))) {
        Wh_Log(L"CreateCompatibleRenderTarget failed");
        DiscardDeviceResources();
        return false;
    }
    buf_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    // GetBitmap hands back the same object with an AddRef each time, so hold
    // one reference for the life of the buffer rather than one per frame.
    if (FAILED(buf_->GetBitmap(&bufBitmap_))) {
        Wh_Log(L"GetBitmap failed");
        DiscardDeviceResources();
        return false;
    }

    // hudFormat_ deliberately outlives a device loss: IDWriteTextFormat is
    // device independent, so it is built once per overlay and released in the
    // destructor rather than rebuilt on every recreate.
    if (g_dwrite && !hudFormat_) {
        float px = std::max(15.0f, (float)(rect_.bottom - rect_.top) * 0.018f);
        if (FAILED(g_dwrite->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"",
                &hudFormat_))) {
            hudFormat_ = nullptr;   // readout is optional, never fatal
        }
    }

    D2D1_COLOR_F white = {1, 1, 1, 1};
    if (FAILED(rt_->CreateSolidColorBrush(&white, nullptr, &brush_))) {
        Wh_Log(L"CreateSolidColorBrush failed");
        DiscardDeviceResources();
        return false;
    }
    return true;
}

void Overlay::DiscardDeviceResources() {
    SafeRelease(&brush_);
    SafeRelease(&bufBitmap_);
    SafeRelease(&buf_);
    SafeRelease(&rt_);
}

void Overlay::NewScene() {
    float w = (float)(rect_.right - rect_.left);
    float h = (float)(rect_.bottom - rect_.top);
    seed_ = seed_ * 1664525u + 1013904223u;

    switch (style) {
        case kStyleContour:
            scene_.reset(new ContourScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleGrowth:
            scene_.reset(new GrowthScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleHarmonograph:
            scene_.reset(
                new HarmonographScene(w, h, seed_, g_palette, amount));
            break;
        case kStyleFlow:
        default:
            scene_.reset(new FlowScene(w, h, seed_, g_palette, amount));
            break;
    }
    phase_ = kPhaseIn;
    phaseT_ = 0;
    artAlpha_ = 0;
    holdT_ = 0;
    if (buf_) {
        D2D1_COLOR_F clear = {0, 0, 0, 0};
        buf_->BeginDraw();
        buf_->Clear(&clear);
        buf_->EndDraw(nullptr, nullptr);
    }
}

void Overlay::FlashHud() {
    int st = ClampT(style, 0, kStyleCount - 1);
    int am = ClampT(amount, 0, kAmountCount - 1);
    WCHAR buf[160];
    swprintf_s(buf, ARRAYSIZE(buf), L"%s     %s %d%%     amount %s",
               kStyleNames[st], kStyleParams[st],
               (int)(param * 100.0f + 0.5f), kAmountNames[am]);
    hudText_ = buf;
    hudT_ = kHudSecs;
}

static float EaseInOut(float t) {
    return t < 0.5f ? 2 * t * t : 1 - (2 - 2 * t) * (2 - 2 * t) * 0.5f;
}

void Overlay::Render(float dtSec) {
    if (!rt_ || !buf_ || !brush_) {
        if (!CreateDeviceResources()) {
            return;
        }
        // The accumulation buffer went with the device while the scene kept
        // its progress, so resuming would paint only the strokes that were
        // still to come onto an empty buffer. Start the piece again. This is
        // the single place every recreate path passes through.
        NewScene();
    }

    const float kFadeIn = 0.75f, kFadeOut = 2.2f;
    // A fixed hold. This used to borrow rotateSeconds when rotation was on,
    // giving two independent timers the same period so they raced each other
    // around the rotation boundary.
    const float holdSecs = 8.0f;

    SceneCtx ctx;
    ctx.w = (float)(rect_.right - rect_.left);
    ctx.h = (float)(rect_.bottom - rect_.top);
    ctx.factory = factory_;
    ctx.target = buf_;
    ctx.brush = brush_;
    ctx.hue = g_hue;
    ctx.param = param;
    ctx.amount = amount;
    ctx.dt = dtSec;

    buf_->BeginDraw();
    bool done = false;
    // Step in every phase, fade-out included, so contours and growth keep
    // moving all the way through the transition instead of freezing.
    if (scene_) {
        done = scene_->Step(ctx);
    } else {
        done = true;
    }
    if (buf_->EndDraw(nullptr, nullptr) == D2DERR_RECREATE_TARGET) {
        DiscardDeviceResources();
        return;
    }

    phaseT_ += dtSec;
    if (phase_ == kPhaseIn) {
        artAlpha_ = EaseInOut(ClampT(phaseT_ / kFadeIn, 0.0f, 1.0f));
        if (phaseT_ >= kFadeIn) {
            phase_ = kPhaseBuild;
            phaseT_ = 0;
            artAlpha_ = 1;
        }
    } else if (phase_ == kPhaseBuild) {
        if (done) {
            if (scene_ && scene_->Continuous()) {
                // Contours and growth keep animating through the hold so they
                // never sit still.
                phase_ = kPhaseHold;
                phaseT_ = 0;
            } else {
                // One-shot styles fade away the moment they fill the screen.
                phase_ = kPhaseOut;
                phaseT_ = 0;
            }
        }
    } else if (phase_ == kPhaseHold) {
        if (phaseT_ >= holdSecs) {
            phase_ = kPhaseOut;
            phaseT_ = 0;
        }
    } else {
        artAlpha_ = 1 - EaseInOut(ClampT(phaseT_ / kFadeOut, 0.0f, 1.0f));
        if (phaseT_ >= kFadeOut) {
            NewScene();
            return;
        }
    }

    // Composite: background, then the accumulation buffer at the fade opacity.
    // A true linear fade. Repeatedly blending a translucent background over
    // the artwork instead plateaus once the per-frame delta rounds below one
    // 8-bit step.
    rt_->BeginDraw();
    D2D1_COLOR_F bg = ToColorF(g_palette.bg, 1.0f);
    rt_->Clear(&bg);

    if (bufBitmap_) {
        D2D1_RECT_F dst;
        dst.left = 0;
        dst.top = 0;
        dst.right = ctx.w;
        dst.bottom = ctx.h;
        rt_->DrawBitmap(bufBitmap_, &dst, artAlpha_,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
    }

    if (scene_ && phase_ != kPhaseOut) {
        scene_->PaintCrisp(ctx, rt_);
    }

    // The readout, drawn last so it sits over the art.
    if (hudT_ > 0.0f) {
        hudT_ -= dtSec;
        if (hudFormat_ && !hudText_.empty()) {
            float a = ClampT(hudT_ / kHudFade, 0.0f, 1.0f);
            float margin = std::max(24.0f, ctx.h * 0.035f);
            D2D1_RECT_F box;
            box.left = margin;
            box.top = ctx.h - margin * 2.0f;
            box.right = ctx.w - margin;
            box.bottom = ctx.h - margin * 0.5f;

            const Rgb& fg = g_palette.ink[g_palette.ink.size() - 1];
            Rgb shifted = ShiftHue(fg, g_hue);

            // a soft dark pass underneath keeps it legible over pale art
            D2D1_COLOR_F shadow = {0, 0, 0, 0.55f * a};
            brush_->SetColor(&shadow);
            D2D1_RECT_F sbox = box;
            sbox.left += 1.5f;
            sbox.top += 1.5f;
            rt_->DrawTextW(hudText_.c_str(), (UINT32)hudText_.size(),
                           hudFormat_, &sbox, brush_,
                           D2D1_DRAW_TEXT_OPTIONS_NONE,
                           DWRITE_MEASURING_MODE_NATURAL);

            D2D1_COLOR_F col = ToColorF(shifted, 0.92f * a);
            brush_->SetColor(&col);
            rt_->DrawTextW(hudText_.c_str(), (UINT32)hudText_.size(),
                           hudFormat_, &box, brush_,
                           D2D1_DRAW_TEXT_OPTIONS_NONE,
                           DWRITE_MEASURING_MODE_NATURAL);
        }
    }

    HRESULT hr = rt_->EndDraw(nullptr, nullptr);
    if (hr == D2DERR_RECREATE_TARGET) {
        // The next frame rebuilds and restarts through the entry path.
        DiscardDeviceResources();
    }
}

LRESULT CALLBACK Overlay::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Overlay* self = (Overlay*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_WINDOWPOSCHANGING: {
            // Re-sink on every position change. ORing SWP_NOZORDER here would
            // also neutralise our own HWND_BOTTOM call and leave the overlay
            // above the user's windows.
            WINDOWPOS* wpos = (WINDOWPOS*)lp;
            wpos->hwndInsertAfter = HWND_BOTTOM;
            wpos->flags &= ~SWP_NOZORDER;
            return 0;
        }
        case WM_SETTINGCHANGE:
            // The taskbar can move or resize without any display change, so
            // the work area shifts under a window sized to it.
            if (g_settings.workAreaOnly && wp == SPI_SETWORKAREA) {
                Controller_RequestRebuild();
            }
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            // Monitor geometry moved under us; rebuild every overlay.
            Controller_RequestRebuild();
            return 0;
        case WM_MOUSEACTIVATE:
            // Focusable on click, but never raised.
            return MA_ACTIVATE;
        case WM_LBUTTONUP:
            if (self) {
                Controller_CycleStyle(self);
            }
            return 0;
        case WM_RBUTTONUP:
            if (self) {
                Controller_StepAmount(self);
            }
            return 0;
        case WM_CONTEXTMENU:
            return 0;
        case WM_MOUSEWHEEL:
            if (self) {
                Controller_Wheel(self, GET_WHEEL_DELTA_WPARAM(wp));
            }
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) {
                // Post rather than calling HideOverlays directly: that deletes
                // this very Overlay while its WndProc is on the stack.
                Controller_RequestClose();
                return 0;
            }
            if (wp == VK_SPACE) {
                Controller_SetSpace(true);
                return 0;
            }
            break;
        case WM_KEYUP:
            if (wp == VK_SPACE) {
                Controller_SetSpace(false);
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            // No WM_KEYUP arrives if the overlay is alt-tabbed away mid-hold,
            // which would leave the hue sliding indefinitely.
            Controller_SetSpace(false);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------
static std::vector<Overlay*> g_overlays;
static std::atomic<bool> g_active{false};
static std::atomic<bool> g_spaceDown{false};
static HANDLE g_toggleEvent = nullptr;
static std::atomic<DWORD> g_workerThreadId{0};
static HHOOK g_kbdHook = nullptr;
static bool g_hotkeyRegistered = false;
static float g_rotateTimer = 0;

static const UINT WM_VSH_SETTINGS = WM_APP + 1;
static const UINT WM_VSH_QUIT = WM_APP + 2;

static const UINT WM_VSH_CLOSE = WM_APP + 3;
static const UINT WM_VSH_REBUILD = WM_APP + 4;

static int NextEnabledStyle(int from) {
    for (int i = 1; i <= kStyleCount; i++) {
        int c = (from + i) % kStyleCount;
        if (g_settings.enable[c]) {
            return c;
        }
    }
    return from;
}

static int FirstEnabledStyle() {
    for (int i = 0; i < kStyleCount; i++) {
        if (g_settings.enable[i]) {
            return i;
        }
    }
    return kStyleFlow;
}

// A wheel notch used to write three values straight through, so a fast
// scroll hammered the store dozens of times a second. Mark dirty instead and
// flush on hide and teardown.
static std::atomic<bool> g_stateDirty{false};
static int g_pendingStyle = 0, g_pendingAmount = 2, g_pendingParam = 500;

static void SaveState(const Overlay* ov) {
    g_pendingStyle = ov->style;
    g_pendingAmount = ov->amount;
    g_pendingParam = (int)(ov->param * 1000.0f);
    g_stateDirty = true;
}

static void FlushState() {
    if (!g_stateDirty.exchange(false)) {
        return;
    }
    Wh_SetIntValue(L"state.style", g_pendingStyle);
    Wh_SetIntValue(L"state.amount", g_pendingAmount);
    Wh_SetIntValue(L"state.param", g_pendingParam);
    // Stamp which setting values this state was derived from.
    Wh_SetIntValue(L"state.amountFrom", g_settings.amount);
    Wh_SetIntValue(L"state.paramFrom", g_settings.parameter);
}

// Clicks fan out to every overlay, the way the rotation timer already does,
// so multiple displays stay in step instead of drifting apart.
static void Controller_CycleStyle(Overlay* ov) {
    int next = NextEnabledStyle(ov->style);
    for (size_t i = 0; i < g_overlays.size(); i++) {
        g_overlays[i]->style = next;
        g_overlays[i]->NewScene();
        g_overlays[i]->FlashHud();
    }
    SaveState(ov);
}

static void Controller_StepAmount(Overlay* ov) {
    int next = (ov->amount + 1) % kAmountCount;
    for (size_t i = 0; i < g_overlays.size(); i++) {
        Overlay* o = g_overlays[i];
        o->amount = next;
        // Contours re-level in place; the others are structural, so rebuild.
        if (o->style != kStyleContour) {
            o->NewScene();
        }
        o->FlashHud();
    }
    SaveState(ov);
}

static void Controller_Wheel(Overlay* ov, int delta) {
    float d = (delta > 0) ? 0.04f : -0.04f;
    float next = ClampT(ov->param + d, 0.0f, 1.0f);
    for (size_t i = 0; i < g_overlays.size(); i++) {
        g_overlays[i]->param = next;
        g_overlays[i]->FlashHud();
    }
    SaveState(ov);
}

static void Controller_SetSpace(bool down) {
    g_spaceDown = down;
}



static void Controller_RequestClose() {
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_CLOSE, 0, 0)) {
        Wh_Log(L"PostThreadMessage(CLOSE) failed (%u)", GetLastError());
    }
}

// Monitor geometry changed; ask the worker to tear the overlays down and
// re-run the display selection from scratch.
static std::atomic<bool> g_rebuildQueued{false};

static void Controller_RequestRebuild() {
    // Every overlay gets its own WM_DISPLAYCHANGE, and each rebuild tears the
    // hook thread down and back up; collapse the burst into one.
    if (g_rebuildQueued.exchange(true)) {
        return;
    }
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_REBUILD, 0, 0)) {
        g_rebuildQueued = false;
        Wh_Log(L"PostThreadMessage(REBUILD) failed (%u)", GetLastError());
    }
}

// A global low-level keyboard hook so Space and Esc work even when no overlay
// owns the keyboard focus. It only watches the two keys and never swallows
// anything, so normal typing is completely unaffected.
static LRESULT CALLBACK LowLevelKbdProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_active) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        if (k->vkCode == VK_ESCAPE) {
            if (down) {
                Controller_RequestClose();
            }
        } else if (k->vkCode == VK_SPACE) {
            if (down || up) {
                Controller_SetSpace(down);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static HANDLE g_hookThread = nullptr;
static HANDLE g_hookReady = nullptr;
static std::atomic<DWORD> g_hookThreadId{0};

static DWORD WINAPI KbdHookThread(LPVOID) {
    // Force the message queue into existence before publishing the id:
    // PostThreadMessageW fails until the thread owns one.
    MSG seed;
    PeekMessageW(&seed, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hookThreadId = GetCurrentThreadId();
    if (g_hookReady) {
        SetEvent(g_hookReady);   // id published and queue exists
    }

    HMODULE mod = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&LowLevelKbdProc, &mod);
    g_kbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbdProc, mod, 0);
    if (!g_kbdHook) {
        Wh_Log(L"SetWindowsHookEx failed (%u)", GetLastError());
    }

    // Nothing but the pump lives on this thread, so the hook is always
    // serviced immediately no matter how long a frame takes.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_VSH_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_kbdHook) {
        UnhookWindowsHookEx(g_kbdHook);
        g_kbdHook = nullptr;
    }
    g_hookThreadId = 0;
    return 0;
}

static void InstallKbdHook() {
    if (g_hookThread) {
        return;
    }
    g_hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReady) {
        // Without it the uninstall path cannot know when the id is published,
        // so it could post nothing and then wait forever. Better to run
        // without the global keys than to risk hanging teardown.
        Wh_Log(L"CreateEvent failed (%u); global keys disabled",
               GetLastError());
        return;
    }
    g_hookThread = CreateThread(nullptr, 0, KbdHookThread, nullptr, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"Could not start the keyboard hook thread (%u)",
               GetLastError());
        CloseHandle(g_hookReady);
        g_hookReady = nullptr;
    }
}

static void UninstallKbdHook() {
    if (!g_hookThread) {
        return;
    }
    // Wait for the thread to publish its id before posting; otherwise the
    // post is skipped and the wait below never ends.
    if (g_hookReady) {
        WaitForSingleObject(g_hookReady, INFINITE);
    }
    if (!PostThreadMessageW(g_hookThreadId.load(), WM_VSH_QUIT, 0, 0)) {
        Wh_Log(L"PostThreadMessage to the hook thread failed (%u)",
               GetLastError());
    }
    // No timeout: abandoning the thread would leave the hook installed with
    // g_hookThread nulled, so the next show would install a second one. Its
    // remaining work is just UnhookWindowsHookEx and return.
    WaitForSingleObject(g_hookThread, INFINITE);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    if (g_hookReady) {
        CloseHandle(g_hookReady);
        g_hookReady = nullptr;
    }
}

static void ApplyExecutionState() {
    if (g_active && g_settings.keepAwake) {
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED |
                                ES_DISPLAY_REQUIRED);
    } else {
        SetThreadExecutionState(ES_CONTINUOUS);
    }
}

static ID2D1Factory* g_factory = nullptr;

static void ShowOverlays() {
    if (g_active) {
        return;
    }
    if (!g_factory) {
        D2D1_FACTORY_OPTIONS opts;
        ZeroMemory(&opts, sizeof(opts));
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                     kIID_ID2D1Factory, &opts,
                                     (void**)&g_factory))) {
            Wh_Log(L"D2D1CreateFactory failed");
            return;
        }
    }
    if (!g_dwrite) {
        // Optional: without it the art still runs, just with no readout.
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                       kIID_IDWriteFactory,
                                       (IUnknown**)&g_dwrite))) {
            Wh_Log(L"DWriteCreateFactory failed; readout disabled");
            g_dwrite = nullptr;
        }
    }

    std::vector<MonitorEntry> mons = EnumerateMonitors();
    if (mons.empty()) {
        return;
    }

    std::vector<RECT> targets;
    auto targetRect = [&](const MonitorEntry& m) -> RECT {
        return g_settings.workAreaOnly ? m.work : m.rect;
    };
    if (g_settings.monitor == L"all") {
        for (size_t i = 0; i < mons.size(); i++) {
            targets.push_back(targetRect(mons[i]));
        }
    } else if (g_settings.monitor == L"primary") {
        for (size_t i = 0; i < mons.size(); i++) {
            if (mons[i].primary) {
                targets.push_back(targetRect(mons[i]));
                break;
            }
        }
        if (targets.empty()) {
            targets.push_back(targetRect(mons[0]));
        }
    } else {
        int win = _wtoi(g_settings.monitor.c_str());
        for (size_t i = 0; i < mons.size(); i++) {
            if (mons[i].winNum == win) {
                targets.push_back(targetRect(mons[i]));
                break;
            }
        }
        if (targets.empty()) {
            Wh_Log(L"Windows display %d not connected, falling back to primary",
                   win);
            for (size_t i = 0; i < mons.size(); i++) {
                if (mons[i].primary) {
                    targets.push_back(targetRect(mons[i]));
                    break;
                }
            }
            if (targets.empty()) {
                targets.push_back(targetRect(mons[0]));
            }
        }
    }

    int style = ClampT(Wh_GetIntValue(L"state.style", FirstEnabledStyle()), 0,
                       kStyleCount - 1);
    if (!g_settings.enable[style]) {
        style = FirstEnabledStyle();
    }
    // If the setting changed since the state was saved, the setting wins;
    // otherwise "Starting amount notch" and "Starting parameter" would be
    // silently ignored forever after the first scroll or right click.
    int amount = g_settings.amount;
    if (Wh_GetIntValue(L"state.amountFrom", -1) == g_settings.amount) {
        amount = Wh_GetIntValue(L"state.amount", g_settings.amount);
    }
    amount = ClampT(amount, 0, kAmountCount - 1);

    int paramMilli = g_settings.parameter * 10;
    if (Wh_GetIntValue(L"state.paramFrom", -1) == g_settings.parameter) {
        paramMilli = Wh_GetIntValue(L"state.param", paramMilli);
    }
    float param = ClampT(paramMilli, 0, 1000) / 1000.0f;

    unsigned seed = (unsigned)GetTickCount64();
    for (size_t i = 0; i < targets.size(); i++) {
        Overlay* ov = new Overlay(g_factory, targets[i], seed + (unsigned)i * 7919u);
        ov->style = style;
        ov->amount = amount;
        ov->param = param;
        if (ov->Create()) {
            g_overlays.push_back(ov);
        } else {
            delete ov;
        }
    }

    if (g_overlays.empty()) {
        return;
    }
    g_active = true;
    g_spaceDown = false;   // a hide while Space was held must not stick
    g_rotateTimer = 0;
    if (g_settings.globalKeys) {
        InstallKbdHook();
    }
    ApplyExecutionState();
    Wh_Log(L"Screen Holder shown on %d display(s)", (int)g_overlays.size());
}

static void HideOverlays() {
    if (!g_active) {
        return;
    }
    UninstallKbdHook();
    FlushState();
    for (size_t i = 0; i < g_overlays.size(); i++) {
        delete g_overlays[i];
    }
    g_overlays.clear();
    g_active = false;
    g_spaceDown = false;
    ApplyExecutionState();
    Wh_Log(L"Screen Holder hidden");
}

static void ToggleOverlays() {
    if (g_active) {
        HideOverlays();
    } else {
        ShowOverlays();
    }
}

// ---------------------------------------------------------------------------
// Hotkey parsing
// ---------------------------------------------------------------------------
static bool ParseHotkey(const std::wstring& s, UINT* mods, UINT* vk) {
    if (s.empty()) {
        return false;
    }
    UINT m = 0;
    UINT key = 0;
    size_t start = 0;
    while (start <= s.size()) {
        size_t plus = s.find(L'+', start);
        std::wstring tok = s.substr(
            start, plus == std::wstring::npos ? std::wstring::npos
                                              : plus - start);
        // trim
        while (!tok.empty() && (tok.front() == L' ' || tok.front() == L'\t')) {
            tok.erase(tok.begin());
        }
        while (!tok.empty() && (tok.back() == L' ' || tok.back() == L'\t')) {
            tok.pop_back();
        }
        std::wstring low;
        for (size_t i = 0; i < tok.size(); i++) {
            low.push_back((wchar_t)towlower(tok[i]));
        }

        if (low == L"ctrl" || low == L"control") {
            m |= MOD_CONTROL;
        } else if (low == L"alt") {
            m |= MOD_ALT;
        } else if (low == L"shift") {
            m |= MOD_SHIFT;
        } else if (low == L"win" || low == L"windows") {
            m |= MOD_WIN;
        } else if (low.size() == 1) {
            wchar_t c = low[0];
            if (c >= L'a' && c <= L'z') {
                key = (UINT)(L'A' + (c - L'a'));
            } else if (c >= L'0' && c <= L'9') {
                key = (UINT)c;
            }
        } else if (low.size() >= 2 && low[0] == L'f') {
            int n = _wtoi(low.c_str() + 1);
            if (n >= 1 && n <= 24) {
                key = VK_F1 + (n - 1);
            }
        }

        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    if (!key) {
        return false;
    }
    *mods = m | MOD_NOREPEAT;
    *vk = key;
    return true;
}

// ---------------------------------------------------------------------------
// Named toggle event, so a desktop shortcut can drive the mod
// ---------------------------------------------------------------------------
static HANDLE CreateToggleEvent() {
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    PSECURITY_DESCRIPTOR psd = nullptr;

    // EVENT_MODIFY_STATE | SYNCHRONIZE (0x100002). SetEvent alone only needs
    // EVENT_MODIFY_STATE, but the documented shortcut calls
    // EventWaitHandle.OpenExisting(name), which asks for Modify | Synchronize
    // and is refused without the latter. Still far short of GENERIC_ALL, which
    // would also hand out DELETE / WRITE_DAC / WRITE_OWNER. The Medium
    // integrity label still lets an ordinary shortcut signal it when Windhawk
    // is elevated, while keeping sandboxed processes below that level out.
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:(A;;0x100002;;;WD)S:(ML;;NW;;;ME)", SDDL_REVISION_1, &psd,
            nullptr)) {
        sa.lpSecurityDescriptor = psd;
    }

    // Local\ (per session) so a second logged-in user's shortcut cannot
    // toggle this session's overlay.
    HANDLE h = CreateEventW(psd ? &sa : nullptr, FALSE, FALSE, kEventLocal);
    if (psd) {
        LocalFree(psd);
    }
    return h;
}

// ---------------------------------------------------------------------------
// Settings loading
// ---------------------------------------------------------------------------
static std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

// The amount notch is stored by name so the setting can carry $options.
static int AmountFromName(const std::wstring& name) {
    static const wchar_t* kNames[kAmountCount] = {L"minimal", L"sparse",
                                                  L"balanced", L"dense",
                                                  L"maximal"};
    for (int i = 0; i < kAmountCount; i++) {
        if (name == kNames[i]) {
            return i;
        }
    }
    return 2;   // balanced
}

static void LoadSettings() {
    g_settings.monitor = GetStringSetting(L"monitor");
    g_settings.enable[kStyleFlow] = Wh_GetIntSetting(L"enableFlow") != 0;
    g_settings.enable[kStyleContour] = Wh_GetIntSetting(L"enableContour") != 0;
    g_settings.enable[kStyleGrowth] = Wh_GetIntSetting(L"enableGrowth") != 0;
    g_settings.enable[kStyleHarmonograph] =
        Wh_GetIntSetting(L"enableHarmonograph") != 0;
    bool any = false;
    for (int i = 0; i < kStyleCount; i++) {
        any = any || g_settings.enable[i];
    }
    if (!any) {
        g_settings.enable[kStyleFlow] = true;   // never leave nothing to draw
    }

    g_settings.rotate = Wh_GetIntSetting(L"rotate") != 0;
    g_settings.rotateSeconds =
        ClampT(Wh_GetIntSetting(L"rotateSeconds"), 10, 7200);
    g_settings.amount = AmountFromName(GetStringSetting(L"amount"));
    g_settings.parameter = ClampT(Wh_GetIntSetting(L"parameter"), 0, 100);
    g_settings.palette = GetStringSetting(L"palette");
    g_settings.customColors = GetStringSetting(L"customColors");
    g_settings.customBackground = GetStringSetting(L"customBackground");
    g_settings.colorRamp = Wh_GetIntSetting(L"colorRamp") != 0;
    g_settings.rampSpeed = ClampT(Wh_GetIntSetting(L"rampSpeed"), 1, 360);
    g_settings.spaceSpeed = ClampT(Wh_GetIntSetting(L"spaceSpeed"), 1, 720);
    g_settings.fps = ClampT(Wh_GetIntSetting(L"fps"), 10, 240);
    g_settings.opacity = ClampT(Wh_GetIntSetting(L"opacity"), 10, 100);
    g_settings.globalKeys = Wh_GetIntSetting(L"globalKeys") != 0;
    g_settings.keepAwake = Wh_GetIntSetting(L"keepAwake") != 0;
    g_settings.startActive = Wh_GetIntSetting(L"startActive") != 0;
    g_settings.workAreaOnly = Wh_GetIntSetting(L"workAreaOnly") != 0;
    g_settings.hotkey = GetStringSetting(L"hotkey");

    BuildPalette();
}

static void RegisterHotkeyFromSettings() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    UINT mods = 0, vk = 0;
    if (!g_settings.hotkey.empty() &&
        !ParseHotkey(g_settings.hotkey, &mods, &vk)) {
        Wh_Log(L"Could not parse the hotkey '%s'; no hotkey is registered",
               g_settings.hotkey.c_str());
    }
    if (ParseHotkey(g_settings.hotkey, &mods, &vk)) {
        if (RegisterHotKey(nullptr, kHotkeyId, mods, vk)) {
            g_hotkeyRegistered = true;
            Wh_Log(L"Hotkey registered: %s", g_settings.hotkey.c_str());
        } else {
            Wh_Log(L"RegisterHotKey failed for '%s' (%u)",
                   g_settings.hotkey.c_str(), GetLastError());
        }
    }
}

// ---------------------------------------------------------------------------
// Worker thread: owns the windows, the render loop and the execution state
// ---------------------------------------------------------------------------
static std::atomic<bool> g_running{true};

static DWORD WINAPI WorkerThread(LPVOID) {
    // Same as the hook thread: the queue must exist before anyone can post.
    MSG seed;
    PeekMessageW(&seed, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_workerThreadId = GetCurrentThreadId();

    // Per-monitor DPI awareness on this thread only, so monitor rectangles are
    // reported in physical pixels regardless of the host's awareness.
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContext_t)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        SetThreadDpiAwarenessContext_t fn =
            (SetThreadDpiAwarenessContext_t)GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (fn) {
            fn((HANDLE)-4);   // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        }
    }

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = Overlay::WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed (%u); no overlay can be created",
               GetLastError());
    }

    LoadSettings();

    std::vector<MonitorEntry> mons = EnumerateMonitors();
    for (size_t i = 0; i < mons.size(); i++) {
        Wh_Log(L"Windows display %d: %s [%s], %dx%d at (%d,%d)%s",
               mons[i].winNum,
               mons[i].deviceName.empty() ? L"Unknown" : mons[i].deviceName.c_str(),
               mons[i].device.c_str(),
               (int)(mons[i].rect.right - mons[i].rect.left),
               (int)(mons[i].rect.bottom - mons[i].rect.top),
               (int)mons[i].rect.left, (int)mons[i].rect.top,
               mons[i].primary ? L" [primary]" : L"");
    }

    g_toggleEvent = CreateToggleEvent();
    if (!g_toggleEvent) {
        Wh_Log(L"Could not create the toggle event (%u)", GetLastError());
    }

    RegisterHotkeyFromSettings();

    if (g_settings.startActive) {
        ShowOverlays();
    }

    LARGE_INTEGER freq, lastRender;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastRender);

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
    // High resolution needs Windows 10 1803; fall back to an ordinary timer,
    // which is still far better than a quantized timeout.
    HANDLE frameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_ALL_ACCESS);
    if (!frameTimer) {
        frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }
    if (!frameTimer) {
        Wh_Log(L"CreateWaitableTimer failed (%u); pacing will be coarse",
               GetLastError());
    }

    while (g_running) {
        HANDLE handles[2];
        DWORD count = 0;
        DWORD toggleIdx = (DWORD)-1;
        if (g_toggleEvent) {
            toggleIdx = count;
            handles[count++] = g_toggleEvent;
        }
        if (g_active && frameTimer) {
            LARGE_INTEGER nowW;
            QueryPerformanceCounter(&nowW);
            float sinceW = (float)(nowW.QuadPart - lastRender.QuadPart) /
                           (float)freq.QuadPart;
            float remain = 1.0f / (float)g_settings.fps - sinceW;
            LARGE_INTEGER due;
            // negative is relative, in 100 ns units
            due.QuadPart = remain <= 0.0f
                               ? -1LL
                               : -(LONGLONG)(remain * 10000000.0f);
            SetWaitableTimer(frameTimer, &due, 0, nullptr, nullptr, FALSE);
            // The wall-clock gate below decides whether to draw; the timer is
            // only here to make the wait end at the right moment.
            handles[count++] = frameTimer;
        }

        // Without a timer the wait must still end on its own, or the overlay
        // would advance only when a message happened to arrive.
        DWORD waitMs = (g_active && !frameTimer) ? 1 : INFINITE;
        DWORD r = MsgWaitForMultipleObjects(count, count ? handles : nullptr,
                                            FALSE, waitMs, QS_ALLINPUT);

        if (toggleIdx != (DWORD)-1 && r == WAIT_OBJECT_0 + toggleIdx) {
            ToggleOverlays();
        } else if (r == WAIT_OBJECT_0 + count) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                    ToggleOverlays();
                    continue;
                }
                if (msg.message == WM_VSH_SETTINGS) {
                    bool wasActive = g_active;
                    HideOverlays();
                    LoadSettings();
                    RegisterHotkeyFromSettings();
                    if (wasActive) {
                        ShowOverlays();
                    }
                    continue;
                }
                if (msg.message == WM_VSH_QUIT) {
                    g_running = false;
                    break;
                }
                if (msg.message == WM_VSH_CLOSE) {
                    HideOverlays();
                    continue;
                }
                if (msg.message == WM_VSH_REBUILD) {
                    g_rebuildQueued = false;
                    // Displays added, removed or resized. Rebuild so the
                    // overlay follows the new geometry rather than sitting at
                    // the old size on a display that may no longer exist.
                    if (g_active) {
                        HideOverlays();
                        ShowOverlays();
                    }
                    continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        if (!g_active) {
            QueryPerformanceCounter(&lastRender);
            continue;
        }

        // Only render when the frame clock says so. MsgWaitForMultipleObjects
        // wakes for input as well as for the timeout, so rendering on every
        // wake-up ignored the fps setting entirely and spun the GPU whenever
        // the mouse crossed the overlay.
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float since = (float)(now.QuadPart - lastRender.QuadPart) /
                      (float)freq.QuadPart;
        if (since < 1.0f / (float)g_settings.fps) {
            continue;
        }
        lastRender = now;
        float dt = since > 0.25f ? 0.25f : since;

        // hue: the automatic ramp and the Space key are independent, so the
        // ramp can be set slower or faster than holding Space
        if (g_spaceDown) {
            g_hue += (float)g_settings.spaceSpeed * dt;
        }
        if (g_settings.colorRamp) {
            g_hue += (float)g_settings.rampSpeed * dt;
        }
        g_hue = std::fmod(g_hue, 360.0f);
        if (g_hue < 0) {
            g_hue += 360.0f;
        }

        if (g_settings.rotate) {
            g_rotateTimer += dt;
            if (g_rotateTimer >= (float)g_settings.rotateSeconds) {
                g_rotateTimer = 0;
                for (size_t i = 0; i < g_overlays.size(); i++) {
                    g_overlays[i]->style =
                        NextEnabledStyle(g_overlays[i]->style);
                    g_overlays[i]->NewScene();
                    g_overlays[i]->FlashHud();
                }
                if (!g_overlays.empty()) {
                    SaveState(g_overlays[0]);
                }
            }
        }

        for (size_t i = 0; i < g_overlays.size(); i++) {
            g_overlays[i]->Render(dt);
        }
    }

    HideOverlays();
    if (frameTimer) {
        CancelWaitableTimer(frameTimer);
        CloseHandle(frameTimer);
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (g_toggleEvent) {
        CloseHandle(g_toggleEvent);
        g_toggleEvent = nullptr;
    }
    SafeRelease(&g_dwrite);
    SafeRelease(&g_factory);
    SetThreadExecutionState(ES_CONTINUOUS);
    return 0;
}

static HANDLE g_workerThread = nullptr;

BOOL WhTool_ModInit() {
    Wh_Log(L"Vector Screen Holder starting");
    g_running = true;
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    DWORD tid = g_workerThreadId.load();
    if (!tid || !PostThreadMessageW(tid, WM_VSH_SETTINGS, 0, 0)) {
        Wh_Log(L"Could not deliver the settings change (%u)", GetLastError());
    }
}

void WhTool_ModUninit() {
    g_running = false;
    DWORD tid = g_workerThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_VSH_QUIT, 0, 0)) {
        Wh_Log(L"PostThreadMessage(QUIT) failed (%u)", GetLastError());
    }
    // Wh_ModUninit calls ExitProcess right after this. Without the join that
    // would terminate the worker inside Direct2D, which can wedge DLL
    // teardown and leave the tool-mod mutex held, making the next enable
    // fail with "Tool mod already running".
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
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

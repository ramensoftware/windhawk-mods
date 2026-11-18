// ==WindhawkMod==
// @id              desktop-snowfall-enhanced
// @name            Desktop Snowfall Enhanced
// @description     Snow, cherry blossom, autumn leaves and rain over the desktop, with wind, mouse sweeping, snow piling up on windows and a gust on weather change.
// @version         1.2.0
// @author          damir
// @github          https://github.com/hiol09
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwmapi -lgdi32 -lole32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Snowfall Enhanced

Animated weather rendered over the whole desktop: snow, cherry blossom
(sakura) petals, autumn leaves and rain. Several kinds can be mixed
together. Direct2D renders into a transparent layered window, so the
desktop and all windows stay fully interactive.

This is a tool mod: it runs in its own dedicated process instead of
inside Explorer.

## Features

- Four weather kinds: snow, cherry blossom, autumn leaves, rain.
- Mixable: "Winter + spring", "Autumn + rain" or "All together".
- Wind affects drift, slant of the rain and the direction of the gust.
- On a weather change a cartoon wind sweeps across the screen, carries the
  old weather away, and the new one drifts in gradually from the top.
- Snow settles on the top edges of windows and the taskbar; leaves and
  petals may stay lying there too, a few at a time.
- Snow on a window top follows the window as it moves. Drag a window
  sideways and the snow that runs out of surface falls off; shrink a
  window from the top and the snow drops as well.
- The mouse can sweep snow, leaves and petals off window tops.
- Cartoon wind sweep can also run on a timer to clean everything up.
- Optional cartoon-style gray streaks for snow.
- The overlay hides itself over fullscreen apps and pauses while the
  session is locked or the display is off.
- Particles are small, rare and slow by design, so the overlay stays
  lightweight and looks calm rather than busy.

## Notes

- v1 renders on the primary monitor only.
- FPS is uncapped by default (matched to the monitor refresh);
  set a cap in the settings if you prefer a fixed budget.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WeatherMode: snow
  $name: Weather mode
  $description: >-
    Which weather to render. The mixed options combine two or more kinds
    with balanced proportions.
  $options:
    - snow: Snow
    - sakura: Cherry blossom
    - autumn: Autumn leaves
    - rain: Rain
    - winterspring: Winter + spring (snow + petals)
    - autumnrain: Autumn + rain (leaves + rain)
    - all: All together

- Intensity: 100
  $name: Intensity (%)
  $description: >-
    Overall amount of particles, in percent. Higher means denser weather
    (0-200).

- Size: 100
  $name: Size (%)
  $description: >-
    Particle size, in percent (50-200).

- Speed: 100
  $name: Fall speed (%)
  $description: >-
    Fall speed, in percent (20-200).

- Wind: 15
  $name: Wind
  $description: >-
    Horizontal wind, from -30 (left) to 30 (right). Large values also pick
    the gust direction; values near zero randomize it.

- FpsCap: 0
  $name: FPS limit
  $description: >-
    Frames per second limit, 0-240. 0 means uncapped (follows the monitor
    refresh).

- GustOnChange: true
  $name: Gust on weather change
  $description: >-
    When the weather mode is changed, a wind front sweeps across the screen,
    carries the old weather away, and the new one starts drifting in from the
    top. Uncheck to switch weather instantly instead.

- SweepMouse: true
  $name: Sweep with the mouse
  $description: >-
    Moving the mouse along the top edge of a window brushes the snow, leaves
    and petals off it, the way a hand would.

- SnowOnWindows: true
  $name: Snow on windows
  $description: >-
    Snow settles and accumulates on the top edges of windows and the taskbar,
    like in the cartoons.

- LeavesOnWindows: true
  $name: Leaves and petals on windows
  $description: >-
    Falling leaves and petals sometimes stay lying on the top edges of
    windows, a few at a time.

- StreakSnow: false
  $name: Cartoon snow streaks
  $description: >-
    Draw snow as slanted gray streaks (like wind-blown snow in cartoons)
    instead of round flakes.

- AutoWind: 0
  $name: Cartoon wind sweep (seconds)
  $description: >-
    Every N seconds a cartoon wind sweeps across the screen and clears the
    weather and the snow on windows. 0 = only on weather change.
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dxgiformat.h>
#include <shellapi.h>

#include <vector>
#include <cmath>
#include <algorithm>


// ─────────────────────────────────────────────────────────────────────────────
// Engine constants, tuned in my own C# prototype of the same effect. No
// third-party code is used.
// ─────────────────────────────────────────────────────────────────────────────

static const float kPi2          = 6.2832f;
static const float kGustLength   = 2.2f;   // how long the wind blows, seconds
static const float kGustSweep    = 0.7f;   // how long the front takes to cross

static const float kSnowWeight  = 1.0f;
static const float kPetalWeight = 0.55f;
static const float kLeafWeight  = 0.55f;
static const float kRainWeight  = 11.0f;

enum PKind { kSnow = 0, kPetal, kLeaf, kRain };

struct Particle {
    PKind  kind;
    int    tint;        // petal: 0..3 color, leaf: 0..5 color, variant tint&3
    float  x, y;
    float  size;
    float  fallSpeed;
    float  sway;
    float  alpha;
    float  rot;
    float  rotSpeed;
    float  phase;
    float  flutter;
    float  blown;       // gust carry direction once caught, 0 = not caught
    float  vx = 0.0f;   // extra horizontal kick (cursor sweep, window moves)
    bool   debris = false; // knocked off a window top: falls once, never respawns
};

// Weather flags, comparable so a settings change can be detected.
struct WeatherSet {
    bool snow, petal, leaf, rain;
    bool operator==(const WeatherSet& o) const {
        return snow == o.snow && petal == o.petal && leaf == o.leaf && rain == o.rain;
    }
    bool operator!=(const WeatherSet& o) const { return !(*this == o); }
};

static void ParseWeatherMode(const wchar_t* mode, WeatherSet& w) {
    w = { true, false, false, false }; // default: snow
    if (!mode || !*mode) return;
    if (wcscmp(mode, L"sakura") == 0)       w = { false, true,  false, false };
    else if (wcscmp(mode, L"autumn") == 0)  w = { false, false, true,  false };
    else if (wcscmp(mode, L"rain") == 0)    w = { false, false, false, true  };
    else if (wcscmp(mode, L"winterspring") == 0) w = { true,  true,  false, false };
    else if (wcscmp(mode, L"autumnrain") == 0)   w = { false, false, true,  true  };
    else if (wcscmp(mode, L"all") == 0)          w = { true,  true,  true,  true  };
    // "snow" and anything unknown fall back to the default above
}

// ─────────────────────────────────────────────────────────────────────────────
// Snow caps and litter on window tops
// ─────────────────────────────────────────────────────────────────────────────

static const float kColWidth    = 5.0f;   // cap column width, px
static const float kCapMaxH     = 10.0f;  // max snow height on a window top
static const float kMeltPerSec  = 0.35f;  // slow melt so caps don't grow forever
static const float kMinCapH     = 0.3f;   // below this the snow is not drawn at all

struct WinLitter {
    float offX;   // offset from the window's left edge
    float size;
    float rot;    // small tilt, the item lies almost flat
    int   kind;   // PKind (petal or leaf)
    int   tint;
};

struct WinCap {
    HWND hwnd;
    RECT rc;                       // window rect on screen
    std::vector<float> cols;       // snow height per column
    std::vector<WinLitter> litter;
    float slideAcc = 0.0f;         // sub-column horizontal offset
    int z = 0x7fffffff;            // z-order, 0 = topmost
    std::vector<uint8_t> vis;      // per column: 1 = not hidden by another window
};

// Owned by the overlay thread only.
static std::vector<WinCap> g_caps;

static bool g_loggedFirstLanding = false;

// Cartoon wind sweep visuals, shown while the gust clears the screen.
struct SweepFx {
    float yFrac;   // 0..1, vertical position of the streak
    float delay;   // stagger, seconds behind the front
};
static std::vector<SweepFx> g_sweeps;

// ─────────────────────────────────────────────────────────────────────────────
// Weather engine
// ─────────────────────────────────────────────────────────────────────────────

struct Engine {
    uint32_t rng    = 0x12345678u;
    float    time   = 0.0f;
    float    mixCursor = 0.0f;

    WeatherSet weather = { true, false, false, false };
    int   intensity = 100;
    int   sizePct   = 100;
    int   speedPct  = 100;
    int   wind      = 15;   // raw setting; formulas use wind*0.6

    bool  gustActive = false;
    float gustStart  = -100.0f;
    float gustDir    = 1.0f;

    bool snowOnWindows   = true;
    bool leavesOnWindows = true;
    bool streakSnow      = false;  // cartoon-style gray streaks instead of flakes
    int  autoWind        = 0;      // cartoon wind sweep interval, seconds (0 = off)

    // Cursor sweep: the mouse brushes snow and leaves off window tops.
    bool  sweepWithCursor = true;
    POINT cursorLast      = {};
    bool  cursorValid     = false;

    std::vector<Particle> flakes;

    int width  = 1920;
    int height = 1080;

    void Seed(uint32_t seed) { rng = seed ? seed : 0x12345678u; }

    float Rand() {
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;
        return (rng & 0xFFFFFF) / 16777216.0f;
    }

    float WeatherWeight() const {
        float total = 0.0f;
        if (weather.snow)  total += kSnowWeight;
        if (weather.petal) total += kPetalWeight;
        if (weather.leaf)  total += kLeafWeight;
        if (weather.rain)  total += kRainWeight;
        return total;
    }

    // Per-kind density compensation so mixing several weathers keeps each
    // layer visually balanced (rain is naturally sparser per particle).
    float WeatherDensity() const {
        float total = WeatherWeight();
        if (total <= 0.0f) return 1.0f;
        float sum = 0.0f;
        if (weather.snow)  sum += kSnowWeight  * 1.0f;
        if (weather.petal) sum += kPetalWeight * 0.9f;
        if (weather.leaf)  sum += kLeafWeight  * 0.9f;
        if (weather.rain)  sum += kRainWeight  * 1.3f;
        return sum / total;
    }

    // Low-discrepancy (golden ratio) mixing keeps the layers evenly spread
    // over time instead of clustering.
    PKind PickKind() {
        float total = WeatherWeight();
        if (total <= 0.0f) return kSnow;

        mixCursor += 0.6180339887f;
        if (mixCursor >= 1.0f) mixCursor -= 1.0f;

        float r = mixCursor * total;
        if (weather.snow)  { r -= kSnowWeight;  if (r < 0.0f) return kSnow;  }
        if (weather.petal) { r -= kPetalWeight; if (r < 0.0f) return kPetal; }
        if (weather.leaf)  { r -= kLeafWeight;  if (r < 0.0f) return kLeaf;  }
        if (weather.rain)  { r -= kRainWeight;  if (r < 0.0f) return kRain;  }
        return kSnow;
    }

    void SpawnFlake(Particle& f, bool scatter) {
        float depth = Rand();
        f.kind = PickKind();
        f.tint = (int)(Rand() * 6.0f) % 6;
        f.rot  = Rand() * kPi2;
        f.flutter = 1.0f + Rand() * 2.0f;
        f.blown = 0.0f;
        f.vx    = 0.0f;
        f.debris = false;

        float sizeMul = sizePct / 100.0f;

        switch (f.kind) {
            case kPetal:
                f.size      = (2.0f + depth * 0.8f) * sizeMul;
                f.fallSpeed = 20.0f + depth * 22.0f + Rand() * 8.0f;
                f.sway      = 22.0f + Rand() * 20.0f;
                f.alpha     = 0.62f + 0.32f * Rand();
                f.rotSpeed  = (Rand() - 0.5f) * 1.8f;
                break;

            case kLeaf:
                // Small but detailed: slow descent, a wide unhurried sweep
                // and a slow turn, the way a real leaf glides down.
                f.size      = (2.1f + depth * 0.9f) * sizeMul;
                f.fallSpeed = 26.0f + depth * 30.0f + Rand() * 10.0f;
                f.sway      = 20.0f + Rand() * 16.0f;
                f.alpha     = 0.8f + 0.2f * Rand();
                f.rotSpeed  = (Rand() - 0.5f) * 1.0f;
                break;

            case kRain:
                f.size      = (0.45f + depth * 0.55f) * sizeMul;
                f.fallSpeed = 420.0f + depth * 380.0f;
                f.sway      = 2.0f;
                f.alpha     = 0.26f + 0.24f * Rand();
                f.rotSpeed  = 0.0f;
                break;

            default: // Snow
                f.size      = (0.8f + depth * depth * 2.7f) * sizeMul;
                // Fast enough to read as falling snow: a flake crosses the
                // screen in a few seconds, not half a minute.
                f.fallSpeed = 62.0f + f.size * 30.0f;
                f.sway      = 6.0f + Rand() * 14.0f;
                f.alpha     = 0.45f + 0.5f * Rand();
                f.rotSpeed  = 0.0f;
                break;
        }

        f.phase = Rand() * kPi2;
        f.x = Rand() * width;
        f.y = scatter ? Rand() * height : -f.size * 2.0f - Rand() * 40.0f;
    }

    void BeginWeatherGust() {
        if (gustActive) return; // the running gust clears the screen for both changes

        // Nothing falling and nothing lying about: nothing to clear.
        if (flakes.empty() && CapsAreClear()) return;

        gustDir = wind > 1 ? 1.0f : wind < -1 ? -1.0f : (Rand() < 0.5f ? -1.0f : 1.0f);
        gustActive = true;
        gustStart = time;

        // The gust is drawn as cartoon wind streaks chasing the front.
        g_sweeps.clear();
        for (int i = 0; i < 3; i++)
            g_sweeps.push_back({ 0.12f + 0.72f * Rand(), i * 0.22f });
    }

    bool CapsAreClear() {
        for (auto& c : g_caps) {
            for (float h : c.cols)
                if (h > 0.05f) return false;
            if (!c.litter.empty()) return false;
        }
        return true;
    }

    void AddSnowTo(WinCap& c, const Particle& f) {
        int n = (int)c.cols.size();
        if (n == 0) return;
        int col = (int)((f.x - (float)c.rc.left) / kColWidth);
        if (col < 0 || col >= n) return;
        // Amount proportional to the flake, smoothed across neighbors.
        float amount = f.size * 1.8f * 0.9f;
        for (int i = col - 2; i <= col + 2; i++) {
            if (i < 0 || i >= n) continue;
            float w = 1.0f - 0.25f * (float)(i - col < 0 ? col - i : i - col);
            float next = c.cols[i] + amount * w;
            c.cols[i] = next > kCapMaxH ? kCapMaxH : next;
        }
    }

    void AddLitterTo(WinCap& c, const Particle& f) {
        int w = (int)c.rc.right - (int)c.rc.left;
        if (w <= 0) return;
        int maxLitter = w / 240 + 2;
        if ((int)c.litter.size() >= maxLitter)
            c.litter.erase(c.litter.begin()); // drop the oldest

        WinLitter l;
        l.offX = f.x - (float)c.rc.left;
        if (l.offX < 4.0f) l.offX = 4.0f;
        if (l.offX > (float)w - 4.0f) l.offX = (float)w - 4.0f;
        l.size = f.size;
        l.rot = f.rot * 0.2f;   // lie almost flat
        l.kind = (int)f.kind;
        l.tint = f.tint;
        c.litter.push_back(l);
    }

    // ─── Cursor sweep ───
    // The mouse is polled instead of hooked: the overlay window is
    // click-through, so it never receives mouse messages itself.
    void SweepWithCursor(float dt) {
        POINT pt;
        if (!GetCursorPos(&pt)) return;
        POINT last = cursorLast;
        cursorLast = pt;
        if (!cursorValid) {
            cursorValid = true;
            return;
        }
        if (!sweepWithCursor) return;
        if (pt.x == last.x && pt.y == last.y) return;

        float dtSafe = dt > 0.001f ? dt : 0.016f;
        float vx = (pt.x - last.x) / dtSafe;
        if (vx > 2200.0f) vx = 2200.0f;
        if (vx < -2200.0f) vx = -2200.0f;

        const float radius = 24.0f;
        float x0 = (float)(last.x < pt.x ? last.x : pt.x) - radius;
        float x1 = (float)(last.x < pt.x ? pt.x : last.x) + radius;
        if (x1 - x0 > 360.0f) {
            x0 = (float)pt.x - radius;
            x1 = (float)pt.x + radius;
        }

        int debrisBudget = 16;

        for (auto& c : g_caps) {
            float top = (float)c.rc.top;
            // Only the top edge of a window is swept; the hand passes over it.
            if ((float)pt.y > top + 10.0f || (float)pt.y < top - 40.0f) continue;
            if (x1 < (float)c.rc.left || x0 > (float)c.rc.right) continue;

            int n = (int)c.cols.size();
            if (n == 0) continue;

            // Lying leaves and petals are flicked off first, so the snow
            // underneath is revealed and then cleared in turn.
            for (int j = (int)c.litter.size() - 1; j >= 0; j--) {
                int ci = (int)(c.litter[j].offX / kColWidth);
                if (!c.vis.empty() &&
                    (ci < 0 || ci >= (int)c.vis.size() || !c.vis[ci]))
                    continue; // hidden behind a higher window

                float lx = (float)c.rc.left + c.litter[j].offX;
                if (lx < x0 || lx > x1) continue;

                if (debrisBudget > 0 && flakes.size() < 1100) {
                    Particle p = {};
                    p.kind      = (PKind)c.litter[j].kind;
                    p.tint      = c.litter[j].tint;
                    p.size      = c.litter[j].size;
                    p.rot       = c.litter[j].rot;
                    p.rotSpeed  = (Rand() - 0.5f) * 3.0f;
                    p.alpha     = 0.95f;
                    p.flutter   = 2.0f;
                    p.phase     = Rand() * kPi2;
                    p.sway      = 12.0f;
                    p.fallSpeed = 35.0f + Rand() * 35.0f;
                    p.vx        = vx * 0.45f + (Rand() - 0.5f) * 70.0f;
                    p.x         = lx;
                    p.y         = top - 2.0f;
                    p.debris    = true;
                    flakes.push_back(p);
                    debrisBudget--;
                }
                c.litter.erase(c.litter.begin() + j);
            }

            // Snow swept by the cursor is knocked loose as falling flakes.
            int from = (int)((x0 - (float)c.rc.left) / kColWidth);
            int to   = (int)((x1 - (float)c.rc.left) / kColWidth) + 1;
            if (from < 0) from = 0;
            if (to > n) to = n;
            for (int i = from; i < to; i++) {
                float h = c.cols[i];
                if (h < 0.6f) continue; // a hairline is not snow
                if (!c.vis.empty() && !c.vis[i]) continue;

                if (debrisBudget > 0 && flakes.size() < 1100) {
                    Particle p = {};
                    p.kind      = kSnow;
                    p.size      = 0.7f + h * 0.25f;
                    p.fallSpeed = 70.0f + h * 8.0f;
                    p.sway      = 8.0f;
                    p.alpha     = 0.9f;
                    p.flutter   = 1.0f;
                    p.vx        = vx * 0.3f + (Rand() - 0.5f) * 40.0f;
                    p.x         = (float)c.rc.left + (i + 0.5f) * kColWidth;
                    p.y         = top - h * 0.6f;
                    p.debris    = true;
                    flakes.push_back(p);
                    debrisBudget--;
                }
                c.cols[i] = 0.0f;
            }
        }

        // Leaves already in the air get pushed along by the passing hand.
        for (auto& f : flakes) {
            if (f.kind != kLeaf && f.kind != kPetal) continue;
            if (fabsf(f.x - (float)pt.x) > 26.0f) continue;
            if (fabsf(f.y - (float)pt.y) > 26.0f) continue;
            f.vx += vx * 0.25f;
            if (f.vx > 900.0f)  f.vx = 900.0f;
            if (f.vx < -900.0f) f.vx = -900.0f;
        }
    }

    float GustFront() const {
        float age = (time - gustStart) / kGustSweep;
        if (age < 0.0f) age = 0.0f;
        if (age > 1.0f) age = 1.0f;
        float span = width + 140.0f;
        return gustDir > 0
            ? -70.0f + span * age
            : width + 70.0f - span * age;
    }

    void ApplyMode(const WeatherSet& next, bool gustOnChange) {
        if (next == weather) return;
        if (gustOnChange) BeginWeatherGust();
        weather = next;
    }

    void Update(float dt) {
        time += dt;

        // Mouse sweep comes first: a hand passing over a window top knocks
        // the snow and litter loose before physics runs on them.
        SweepWithCursor(dt);

        // ─── WEATHER-SWITCH GUST ───
        // While it blows, nothing new spawns; the front catches particle
        // after particle and carries them off the screen, staggered so they
        // don't go as one straight line.
        if (gustActive && time - gustStart >= kGustLength) {
            gustActive = false;
            flakes.clear();
        }
        bool gust = gustActive;
        float gustFront = gust ? GustFront() : 0.0f;

        int target = gust ? 0
            : (int)(intensity * 1.5f * WeatherDensity() * (width * (float)height) / (1920.0f * 1080.0f));
        if (target > 900) target = 900;

        // Refill gradually so the new weather drifts in from the top
        // instead of popping onto the screen all at once. Debris knocked
        // off window tops is not part of the weather budget.
        int ambient = 0;
        for (const auto& f : flakes)
            if (!f.debris) ambient++;

        int spawnBudget = 4;
        while (ambient < target && spawnBudget-- > 0) {
            Particle f = {};
            SpawnFlake(f, false);
            flakes.push_back(f);
            ambient++;
        }
        if (!gust && ambient > target) {
            int excess = ambient - target;
            size_t w = 0;
            for (size_t r = 0; r < flakes.size(); r++) {
                if (excess > 0 && !flakes[r].debris) {
                    excess--;
                    continue;
                }
                if (w != r) flakes[w] = flakes[r];
                w++;
            }
            flakes.resize(w);
        }

        float speedScale = speedPct / 100.0f;
        float windVal = wind * 0.6f;

        // ─── WINDOW CAPS: melt, gust clearing, litter blown away ───
        float front = gust ? GustFront() : 0.0f;
        for (auto& c : g_caps) {
            if (gust) {
                int n = (int)c.cols.size();
                for (int i = 0; i < n; i++) {
                    float x = (float)c.rc.left + (i + 0.5f) * kColWidth;
                    if ((front - x) * gustDir > 0.0f) c.cols[i] = 0.0f;
                }
                for (int j = (int)c.litter.size() - 1; j >= 0; j--) {
                    float x = (float)c.rc.left + c.litter[j].offX;
                    if ((front - x) * gustDir > 0.0f) {
                        if (flakes.size() < 1000) {
                            Particle p = {};
                            p.kind = (PKind)c.litter[j].kind;
                            p.tint = c.litter[j].tint;
                            p.size = c.litter[j].size;
                            p.rot = c.litter[j].rot;
                            p.alpha = 0.9f;
                            p.flutter = 1.5f;
                            p.phase = Rand() * kPi2;
                            p.rotSpeed = (Rand() - 0.5f) * 2.0f;
                            p.blown = gustDir;
                            p.x = x;
                            p.y = (float)c.rc.top - 2.0f;
                            p.debris = true;
                            flakes.push_back(p);
                        }
                        c.litter.erase(c.litter.begin() + j);
                    }
                }
            } else {
                for (auto& h : c.cols) {
                    if (h <= 0.0f) continue;
                    h -= kMeltPerSec * dt;
                    if (h < 0.15f) h = 0.0f;
                }
            }
        }

        size_t write = 0;
        for (size_t i = 0; i < flakes.size(); i++) {
            Particle f = flakes[i];
            float oldY = f.y;

            if (gust) {
                if (f.blown == 0.0f) {
                    float stagger = (f.phase / kPi2) * 160.0f;
                    if ((gustFront - f.x) * gustDir > stagger) f.blown = gustDir;
                }
                if (f.blown != 0.0f) {
                    f.x += f.blown * (650.0f + 900.0f * (f.phase / kPi2)) * dt;
                    f.y -= 60.0f * dt;
                    f.rot += f.rotSpeed * 4.0f * dt;
                    if (f.blown > 0 ? f.x > width + 60.0f : f.x < -60.0f) continue;
                    flakes[write++] = f;
                    continue;
                }
            }

            switch (f.kind) {
                case kPetal:
                    f.rot += f.rotSpeed * dt;
                    f.y += f.fallSpeed * speedScale * dt;
                    f.x += (windVal * 1.35f
                            + sinf(time * 0.9f + f.phase) * f.sway
                            + sinf(time * 2.3f + f.phase * 1.7f) * f.sway * 0.35f) * dt;
                    break;

                case kLeaf:
                    f.rot += f.rotSpeed * dt;
                    // Close to a snowflake's steady descent, with a little
                    // lift and side sway so it still reads as a leaf.
                    f.y += f.fallSpeed * speedScale * (0.92f + 0.14f * sinf(time * 1.9f + f.phase)) * dt;
                    f.x += (windVal * 1.25f + sinf(time * 1.1f + f.phase) * f.sway) * dt;
                    break;

                case kRain:
                    f.y += f.fallSpeed * (speedScale < 0.4f ? 0.4f : speedScale) * dt;
                    f.x += windVal * 1.15f * dt;
                    break;

                default: // Snow
                    f.y += f.fallSpeed * speedScale * dt;
                    f.x += (windVal * (0.6f + 0.4f * f.size / 3.5f)
                            + sinf(time * 1.3f + f.phase) * f.sway) * dt;
                    break;
            }

            // A kick from the cursor or from a moving window: a short
            // sideways toss that fades out.
            if (f.vx != 0.0f) {
                f.x += f.vx * dt;
                float decay = 1.0f - 2.6f * dt;
                if (decay < 0.0f) decay = 0.0f;
                f.vx *= decay;
            }

            if (f.x > width + 10.0f) f.x -= width + 20.0f;
            else if (f.x < -10.0f)   f.x += width + 20.0f;

            // Landing on window tops: snow accumulates, leaves and petals
            // sometimes stay lying there for a while.
            if (!gust && f.kind != kRain) {
                bool wantsLanding =
                    (f.kind == kSnow && snowOnWindows) ||
                    (f.kind != kSnow && leavesOnWindows);
                if (wantsLanding) {
                    bool landed = false;
                    for (auto& c : g_caps) {
                        float top = (float)c.rc.top;
                        if (oldY < top && f.y >= top &&
                            f.x >= (float)c.rc.left && f.x < (float)c.rc.right) {
                            if (f.kind == kSnow) {
                                AddSnowTo(c, f);
                            } else if (Rand() < 0.4f) {
                                AddLitterTo(c, f);
                            } else {
                                break; // flies on past the window
                            }
                            landed = true;
                            break;
                        }
                    }
                    if (landed) {
                        if (!g_loggedFirstLanding) {
                            g_loggedFirstLanding = true;
                            Wh_Log(L"first landing at %.0f,%.0f (windows: %d)",
                                   f.x, f.y, (int)g_caps.size());
                        }
                        if (f.debris) continue; // it settled; that was its last fall
                        SpawnFlake(f, false); // loop back to the top
                        flakes[write++] = f;
                        continue;
                    }
                }
            }

            if (f.kind == kRain) {
                if (f.y > height + 5.0f) {
                    if (!gust) {
                        SpawnFlake(f, false);
                        flakes[write++] = f;
                    }
                    continue; // during the gust the old weather is just dropped
                }
                flakes[write++] = f;
                continue;
            }

            if (f.y > height + 10.0f) {
                if (!gust && !f.debris) {
                    SpawnFlake(f, false); // loop back to the top
                    flakes[write++] = f;
                }
                continue;
            }

            flakes[write++] = f;
        }
        flakes.resize(write);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Renderer: Direct2D → memory DIB → UpdateLayeredWindow
// ─────────────────────────────────────────────────────────────────────────────

static const IID kIID_ID2D1Factory =
    { 0x06152247, 0x6f50, 0x465a, { 0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07 } };

static const float kPetalColors[4][3] = {
    { 255, 205, 222 }, { 249, 175, 205 }, { 244, 143, 177 }, { 252, 228, 236 },
};
static const float kLeafColors[6][3] = {
    { 216, 67, 21 }, { 245, 124, 0 }, { 183, 28, 28 },
    { 249, 168, 37 }, { 141, 110, 99 }, { 191, 54, 12 },
};
static const float kRainColor[3]   = { 200, 224, 247 };
static const float kSnowColor[3]   = { 247, 251, 255 };
static const float kStreakColor[3] = { 205, 212, 222 }; // cartoon gray streaks
static const float kDarkSideMul    = 0.72f;   // edge-on leaves/petals read darker

static const D2D1_MATRIX_3X2_F kIdentity = { 1, 0, 0, 1, 0, 0 };

static ID2D1Factory*         g_factory    = nullptr;
static ID2D1DCRenderTarget*  g_rt         = nullptr;
static ID2D1SolidColorBrush* g_brush      = nullptr;
static ID2D1PathGeometry*    g_petalGeo   = nullptr;
static ID2D1PathGeometry*    g_leafGeo[4] = {};

static HDC      g_memDC = nullptr;
static HBITMAP  g_dib   = nullptr;
static void*    g_bits  = nullptr;

static int g_width  = 1920;
static int g_height = 1080;

template <class T> static void SafeRelease(T** pp) {
    if (*pp) { (*pp)->Release(); *pp = nullptr; }
}

static D2D1_COLOR_F Col(const float rgb[3], float a) {
    D2D1_COLOR_F c;
    c.r = rgb[0] / 255.0f; c.g = rgb[1] / 255.0f; c.b = rgb[2] / 255.0f; c.a = a;
    return c;
}

// Unit-space figures, transformed per particle at draw time.
static void BuildPetalsAndLeaves() {
    struct Builder {
        static void Lines(ID2D1GeometrySink* s, const D2D1_POINT_2F* pts, int n) {
            s->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_FILLED);
            s->AddLines(&pts[1], n - 1);
            s->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
        static void QuadChain(ID2D1GeometrySink* s, const D2D1_POINT_2F* start,
                              const D2D1_QUADRATIC_BEZIER_SEGMENT* segs, int n) {
            s->BeginFigure(*start, D2D1_FIGURE_BEGIN_FILLED);
            s->AddQuadraticBeziers(segs, n);
            s->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
    };

    auto makeGeo = [](ID2D1PathGeometry** out, void (*fill)(ID2D1GeometrySink*)) {
        if (FAILED(g_factory->CreatePathGeometry(out))) { *out = nullptr; return; }
        ID2D1GeometrySink* sink = nullptr;
        if (FAILED((*out)->Open(&sink))) { SafeRelease(out); return; }
        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        fill(sink);
        if (FAILED(sink->Close())) { sink->Release(); SafeRelease(out); return; }
        sink->Release();
    };

    // Sakura petal: rounded body with a small notch at the outer tip.
    makeGeo(&g_petalGeo, [](ID2D1GeometrySink* s) {
        const D2D1_POINT_2F start = { 0.0f, 0.85f };
        const D2D1_QUADRATIC_BEZIER_SEGMENT segs[] = {
            { { -0.85f,  0.35f }, { -0.72f, -0.30f } },
            { { -0.62f, -0.78f }, { -0.12f, -0.58f } },
            { {  0.00f, -0.50f }, {  0.12f, -0.58f } },
            { {  0.62f, -0.78f }, {  0.72f, -0.30f } },
            { {  0.85f,  0.35f }, {  0.00f,  0.85f } },
        };
        Builder::QuadChain(s, &start, segs, 5);
    });

    // Maple: five jagged lobes.
    makeGeo(&g_leafGeo[0], [](ID2D1GeometrySink* s) {
        const D2D1_POINT_2F p[] = {
            { 0.0f, -1.0f }, { 0.18f, -0.62f }, { 0.52f, -0.78f }, { 0.40f, -0.36f },
            { 0.95f, -0.30f }, { 0.62f, 0.02f }, { 0.82f, 0.42f }, { 0.36f, 0.38f },
            { 0.20f, 0.72f }, { 0.0f, 0.62f }, { -0.20f, 0.72f }, { -0.36f, 0.38f },
            { -0.82f, 0.42f }, { -0.62f, 0.02f }, { -0.95f, -0.30f }, { -0.40f, -0.36f },
            { -0.52f, -0.78f }, { -0.18f, -0.62f },
        };
        Builder::Lines(s, p, 18);
    });

    // Oak: rounded lobes.
    makeGeo(&g_leafGeo[1], [](ID2D1GeometrySink* s) {
        const D2D1_POINT_2F start = { 0.0f, -1.0f };
        const D2D1_QUADRATIC_BEZIER_SEGMENT segs[] = {
            { {  0.38f, -0.72f }, {  0.30f, -0.45f } },
            { {  0.62f, -0.38f }, {  0.56f, -0.10f } },
            { {  0.88f, -0.02f }, {  0.62f,  0.28f } },
            { {  0.74f,  0.52f }, {  0.38f,  0.58f } },
            { {  0.28f,  0.84f }, {  0.00f,  0.78f } },
            { { -0.28f,  0.84f }, { -0.38f,  0.58f } },
            { { -0.74f,  0.52f }, { -0.62f,  0.28f } },
            { { -0.88f, -0.02f }, { -0.56f, -0.10f } },
            { { -0.62f, -0.38f }, { -0.30f, -0.45f } },
            { { -0.38f, -0.72f }, {  0.00f, -1.00f } },
        };
        Builder::QuadChain(s, &start, segs, 10);
    });

    // Birch: narrow serrated.
    makeGeo(&g_leafGeo[2], [](ID2D1GeometrySink* s) {
        const D2D1_POINT_2F p[] = {
            { 0.0f, -1.0f }, { 0.34f, -0.45f }, { 0.42f, -0.28f }, { 0.30f, -0.14f },
            { 0.38f, 0.0f }, { 0.26f, 0.16f }, { 0.32f, 0.30f }, { 0.18f, 0.44f },
            { 0.22f, 0.60f }, { 0.06f, 0.72f }, { 0.0f, 0.80f },
            { -0.06f, 0.72f }, { -0.22f, 0.60f }, { -0.18f, 0.44f }, { -0.32f, 0.30f },
            { -0.26f, 0.16f }, { -0.38f, 0.0f }, { -0.30f, -0.14f }, { -0.42f, -0.28f },
            { -0.34f, -0.45f },
        };
        Builder::Lines(s, p, 19);
    });

    // Willow: smooth lanceolate.
    makeGeo(&g_leafGeo[3], [](ID2D1GeometrySink* s) {
        const D2D1_POINT_2F start = { 0.0f, -1.0f };
        const D2D1_QUADRATIC_BEZIER_SEGMENT segs[] = {
            { {  0.52f, -0.42f }, {  0.40f,  0.18f } },
            { {  0.28f,  0.60f }, {  0.00f,  0.82f } },
            { { -0.28f,  0.60f }, { -0.40f,  0.18f } },
            { { -0.52f, -0.42f }, {  0.00f, -1.00f } },
        };
        Builder::QuadChain(s, &start, segs, 4);
    });
}

static HRESULT CreateRenderTargetResources() {
    SafeRelease(&g_brush);
    SafeRelease(&g_rt);

    D2D1_RENDER_TARGET_PROPERTIES rtProps = {};
    rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rtProps.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    rtProps.usage    = D2D1_RENDER_TARGET_USAGE_NONE;
    rtProps.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    rtProps.dpiX     = 0.0f;
    rtProps.dpiY     = 0.0f;

    HRESULT hr = g_factory->CreateDCRenderTarget(&rtProps, &g_rt);
    if (FAILED(hr) || !g_rt) return hr;

    hr = g_rt->CreateSolidColorBrush(Col(kSnowColor, 1.0f), &g_brush);
    return hr;
}

static bool InitD2D() {
    D2D1_FACTORY_OPTIONS fo = {};
    fo.debugLevel = D2D1_DEBUG_LEVEL_NONE;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                   kIID_ID2D1Factory, &fo, (void**)&g_factory);
    if (FAILED(hr) || !g_factory) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08lX", hr);
        return false;
    }

    hr = CreateRenderTargetResources();
    if (FAILED(hr) || !g_rt || !g_brush) {
        Wh_Log(L"Creating the D2D render target failed: 0x%08lX", hr);
        return false;
    }

    BuildPetalsAndLeaves();
    return g_petalGeo && g_leafGeo[0] && g_leafGeo[1] && g_leafGeo[2] && g_leafGeo[3];
}

static void CleanupD2D() {
    for (auto& g : g_leafGeo) SafeRelease(&g);
    SafeRelease(&g_petalGeo);
    SafeRelease(&g_brush);
    SafeRelease(&g_rt);
    SafeRelease(&g_factory);
}

static void DrawWindowCaps(ID2D1RenderTarget* rt) {
    rt->SetTransform(kIdentity);
    for (auto& c : g_caps) {
        int n = (int)c.cols.size();
        if (n == 0) continue;
        float left = (float)c.rc.left, top = (float)c.rc.top;
        float right = (float)c.rc.right;

        // ─── snow cap: smoothed white layer along the top edge ───
        bool any = false;
        for (int i = 0; i < n; i++) {
            if ((c.vis.empty() || c.vis[i]) && c.cols[i] >= kMinCapH) {
                any = true;
                break;
            }
        }

        if (any) {
            ID2D1PathGeometry* geo = nullptr;
            ID2D1GeometrySink* sink = nullptr;
            if (SUCCEEDED(g_factory->CreatePathGeometry(&geo)) &&
                SUCCEEDED(geo->Open(&sink))) {
                sink->SetFillMode(D2D1_FILL_MODE_WINDING);
                // The baseline is the top edge itself: with no snow the
                // polygon has no area, so no hairline is drawn along windows.
                D2D1_POINT_2F start = { left, top };
                sink->BeginFigure(start, D2D1_FIGURE_BEGIN_FILLED);
                for (int i = 0; i < n; i++) {
                    float h = 0.0f;
                    if (c.vis.empty() || c.vis[i]) {
                        h = c.cols[i] * 2.0f;
                        if (i > 0 && (c.vis.empty() || c.vis[i - 1]))
                            h += c.cols[i - 1];
                        if (i + 1 < n && (c.vis.empty() || c.vis[i + 1]))
                            h += c.cols[i + 1];
                        h *= 0.25f;
                        if (h < kMinCapH) h = 0.0f;
                    }
                    D2D1_POINT_2F pt = { left + (float)i * kColWidth, top - h };
                    sink->AddLine(pt);
                }
                D2D1_POINT_2F end = { right, top };
                sink->AddLine(end);
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                sink->Release();
                g_brush->SetColor(Col(kSnowColor, 0.95f));
                rt->FillGeometry(geo, g_brush);
                geo->Release();
            }
        }

        // ─── litter: petals and leaves lying on the edge ───
        for (auto& l : c.litter) {
            int ci = (int)(l.offX / kColWidth);
            if (!c.vis.empty() &&
                (ci < 0 || ci >= (int)c.vis.size() || !c.vis[ci]))
                continue; // hidden behind a higher window

            ID2D1PathGeometry* geo =
                l.kind == kPetal ? g_petalGeo : g_leafGeo[l.tint & 3];
            if (!geo) continue;
            const float* rgb = l.kind == kPetal
                ? kPetalColors[l.tint & 3] : kLeafColors[l.tint % 6];
            float scale = l.size * 2.3f;
            float cos_ = cosf(l.rot), sin_ = sinf(l.rot);
            D2D1_MATRIX_3X2_F m;
            m._11 = scale * cos_;    m._12 = scale * sin_;
            m._21 = -scale * sin_;   m._22 = scale * cos_;
            m.dx = left + l.offX;
            m.dy = top - scale * 0.30f;
            rt->SetTransform(m);
            g_brush->SetColor(Col(rgb, 0.9f));
            rt->FillGeometry(geo, g_brush);
        }
    }
    rt->SetTransform(kIdentity);
}

// Cartoon wind streaks chasing the gust front, like in animations.
static void DrawSweeps(ID2D1RenderTarget* rt, const Engine& eng) {
    if (!eng.gustActive || g_sweeps.empty()) return;
    float front = eng.GustFront();
    float age = eng.time - eng.gustStart;
    float dir = eng.gustDir;

    rt->SetTransform(kIdentity);
    for (auto& s : g_sweeps) {
        float t = age - s.delay;
        if (t < -0.25f || t > kGustLength - 0.35f) continue;
        float fadeIn = t < 0.0f ? (t + 0.25f) / 0.25f : 1.0f;
        if (fadeIn < 0.0f) fadeIn = 0.0f;
        float fadeOut = (kGustLength - 0.35f - t) / 0.4f;
        if (fadeOut > 1.0f) fadeOut = 1.0f;
        float a = fadeIn * (fadeOut > 0.0f ? fadeOut : 0.0f);
        if (a <= 0.0f) continue;

        float y = s.yFrac * (float)eng.height;
        float head = front - dir * s.delay * 220.0f;

        g_brush->SetColor(Col(kSnowColor, 0.5f * a));
        // Wavy trail behind the head.
        float len = 250.0f;
        D2D1_POINT_2F prev = { head, y };
        for (int i = 1; i <= 8; i++) {
            float f = (float)i / 8.0f;
            D2D1_POINT_2F pt = { head - dir * len * f,
                                 y + sinf(t * 4.0f + f * 5.0f) * 14.0f * f };
            rt->DrawLine(prev, pt, g_brush, 3.5f);
            prev = pt;
        }
        // Curl at the head.
        D2D1_ELLIPSE curl = { { head - dir * 4.0f, y - 9.0f }, 8.0f, 6.0f };
        rt->DrawEllipse(&curl, g_brush, 2.5f);
    }
}

static void DrawScene(ID2D1RenderTarget* rt, const Engine& eng) {
    ID2D1SolidColorBrush* b = g_brush;

    // Snow caps and litter sit on the windows, under the falling weather.
    DrawWindowCaps(rt);

    // Pass 1, identity transform: snow and rain.
    rt->SetTransform(kIdentity);

    for (const Particle& f : eng.flakes) {
        if (f.kind != kSnow) continue;
        if (eng.streakSnow) {
            // Cartoon-style: slanted gray streaks, wind-blown snow.
            float slant = -(4.0f + eng.wind * 0.25f +
                            sinf(eng.time * 1.3f + f.phase) * 2.5f);
            float len = 4.0f + f.size * 3.0f;
            b->SetColor(Col(kStreakColor, f.alpha * 0.85f));
            rt->DrawLine({ f.x, f.y }, { f.x - slant, f.y - len }, b, 1.6f);
        } else {
            b->SetColor(Col(kSnowColor, f.alpha));
            float r = f.size * 1.8f;
            D2D1_ELLIPSE el = { { f.x, f.y }, r, r };
            rt->FillEllipse(&el, b);
        }
    }

    float slant = -(float)eng.wind * 0.14f;
    for (const Particle& f : eng.flakes) {
        if (f.kind != kRain) continue;
        float len = f.fallSpeed * 0.045f;
        if (len < 6.0f) len = 6.0f;
        if (len > 26.0f) len = 26.0f;
        b->SetColor(Col(kRainColor, f.alpha));
        rt->DrawLine({ f.x, f.y }, { f.x - slant, f.y - len }, b, 1.2f);
    }

    // Pass 2, per-particle transform: petals and leaves with 3D tumble.
    for (const Particle& f : eng.flakes) {
        ID2D1PathGeometry* geo = nullptr;
        const float* rgb = nullptr;
        float scale = 0.0f;

        if (f.kind == kPetal) {
            geo = g_petalGeo;
            rgb = kPetalColors[f.tint & 3];
            scale = f.size * 2.3f;
        } else if (f.kind == kLeaf) {
            geo = g_leafGeo[f.tint & 3];
            rgb = kLeafColors[f.tint % 6];
            scale = f.size * 2.3f;
        } else {
            continue;
        }
        if (!geo) continue;

        // Tumbling flip around the leaf's long axis; past half-tumble the
        // face turns edge-on and reads darker, like the shaded side.
        float tumble = 0.35f + 0.65f * fabsf(cosf(eng.time * f.flutter * 1.7f + f.phase));

        float mul = tumble < 0.5f ? kDarkSideMul : 1.0f;
        float darkened[3] = { rgb[0] * mul, rgb[1] * mul, rgb[2] * mul };
        b->SetColor(Col(darkened, f.alpha));

        float cos = cosf(f.rot), sin = sinf(f.rot);
        float ax = scale * tumble; // local x compresses with the tumble
        D2D1_MATRIX_3X2_F m;
        m._11 = ax * cos;  m._12 = ax * sin;
        m._21 = -scale * sin; m._22 = scale * cos;
        m.dx = f.x; m.dy = f.y;
        rt->SetTransform(m);

        rt->FillGeometry(geo, b);
    }

    rt->SetTransform(kIdentity);
}

static bool RenderFrame(HWND hwnd, const Engine& eng) {
    if (!g_rt || !g_memDC) return false;

    RECT rc = { 0, 0, g_width, g_height };
    HRESULT hr = g_rt->BindDC(g_memDC, &rc);
    if (FAILED(hr)) return false;
    g_rt->BeginDraw();
    g_rt->Clear(nullptr);
    DrawScene(g_rt, eng);
    DrawSweeps(g_rt, eng);
    hr = g_rt->EndDraw();

    // A GPU reset or driver update invalidates the device. Without this the
    // DIB would keep showing the last frame forever.
    if (hr == D2DERR_RECREATE_TARGET) {
        if (FAILED(CreateRenderTargetResources()))
            Wh_Log(L"Recreating the D2D render target failed");
        return false;
    }
    if (FAILED(hr)) return false;

    GdiFlush();

    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    POINT ptSrc = { 0, 0 };
    SIZE size = { g_width, g_height };
    HDC screen = GetDC(nullptr);
    BOOL ok = UpdateLayeredWindow(hwnd, screen, nullptr, &size,
                                  g_memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
    return ok != FALSE;
}

// ─────────────────────────────────────────────────────────────────────────────
// Overlay window (owned by a dedicated thread in the tool mod process)
// ─────────────────────────────────────────────────────────────────────────────

static const wchar_t* kWndClass = L"DesktopWeatherOverlayWnd";

static HWND g_wnd = nullptr; // used by the enumeration below

// ─── Window enumeration for snow caps ───

static Engine g_engine;                // overlay thread only
static std::vector<WinCap> g_capsEnum; // temp, overlay thread only

static BOOL CALLBACK EnumWndProc(HWND h, LPARAM) {
    if (h == g_wnd) return TRUE;
    if (!IsWindowVisible(h) || IsIconic(h)) return TRUE;

    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if (ex & WS_EX_TRANSPARENT) return TRUE;

    wchar_t cls[64];
    GetClassNameW(h, cls, 64);
    bool shell = wcsncmp(cls, L"Shell_", 6) == 0;   // taskbar and friends
    if ((ex & WS_EX_TOOLWINDOW) && !shell) return TRUE;

    DWORD cloaked = 0;
    DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return TRUE;

    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return TRUE;
    if (!wcscmp(cls, L"DesktopWeatherOverlayWnd")) return TRUE;

    RECT rc;
    if (!GetWindowRect(h, &rc)) return TRUE;
    RECT area = { 0, 0, g_width, g_height };
    RECT inter;
    if (!IntersectRect(&inter, &rc, &area)) return TRUE;
    if (rc.right - rc.left < 60 || rc.bottom - rc.top < 24) return TRUE;

    WinCap cap = {};
    cap.hwnd = h;
    cap.rc = rc;
    g_capsEnum.push_back(std::move(cap));
    return TRUE;
}

// ─── Cap debris: loose snow and litter that leaves a window top ───

static void SpawnCapFlake(float x, float y, float h, float vx) {
    if (h < 0.25f) return;
    if (g_engine.flakes.size() >= 1100) return;
    Particle p = {};
    p.kind      = kSnow;
    p.size      = 0.8f + h * 0.22f;
    p.fallSpeed = 70.0f + h * 8.0f;
    p.sway      = 8.0f;
    p.alpha     = 0.9f;
    p.flutter   = 1.0f;
    p.vx        = vx * 0.25f + (g_engine.Rand() - 0.5f) * 30.0f;
    p.x         = x;
    p.y         = y;
    p.debris    = true;
    g_engine.flakes.push_back(p);
}

static void SpawnLitterFlake(float x, float y, const WinLitter& l, float vx) {
    if (g_engine.flakes.size() >= 1100) return;
    Particle p = {};
    p.kind      = (PKind)l.kind;
    p.tint      = l.tint;
    p.size      = l.size;
    p.rot       = l.rot;
    p.rotSpeed  = (g_engine.Rand() - 0.5f) * 2.0f;
    p.alpha     = 0.95f;
    p.flutter   = 2.0f;
    p.phase     = g_engine.Rand() * kPi2;
    p.sway      = 12.0f;
    p.fallSpeed = 35.0f + g_engine.Rand() * 30.0f;
    p.vx        = vx * 0.3f + (g_engine.Rand() - 0.5f) * 40.0f;
    p.x         = x;
    p.y         = y;
    p.debris    = true;
    g_engine.flakes.push_back(p);
}

// Breaks a whole cap loose: the snow and the litter stop following the
// window and fall where they were.
static void DetachCap(WinCap& cap, float vx) {
    int n = (int)cap.cols.size();
    for (int i = 0; i < n; i++) {
        if (cap.cols[i] < 0.25f) continue;
        SpawnCapFlake((float)cap.rc.left + (i + 0.5f) * kColWidth,
                      (float)cap.rc.top - cap.cols[i] * 0.6f,
                      cap.cols[i], vx);
        cap.cols[i] = 0.0f;
    }

    for (auto& l : cap.litter) {
        SpawnLitterFlake((float)cap.rc.left + l.offX,
                         (float)cap.rc.top - 2.0f, l, vx);
    }
    cap.litter.clear();
}

// Starts a cap over: fresh columns for the current rectangle, nothing on top.
static void RebuildCap(WinCap& cap, const RECT& rc) {
    int n = (rc.right - rc.left) / (int)kColWidth;
    if (n < 1) n = 1;
    if (n > 400) n = 400;
    cap.cols.assign(n, 0.0f);
    cap.litter.clear();
    cap.slideAcc = 0.0f;
    cap.rc = rc;
}

// The snow strip keeps its screen position while the window slides under
// it, like snow with no grip on the surface. A sideways drag therefore
// scrapes snow off the trailing edge, and it falls.
static void SlideCap(WinCap& cap, float dx) {
    if (dx == 0.0f || cap.cols.empty()) return;

    cap.slideAcc += dx;

    while (cap.slideAcc >= kColWidth) {
        cap.slideAcc -= kColWidth;
        if (cap.cols[0] >= 0.25f) {
            SpawnCapFlake((float)cap.rc.left + kColWidth * 0.5f,
                          (float)cap.rc.top - cap.cols[0] * 0.6f,
                          cap.cols[0], 0.0f);
        }
        cap.cols.erase(cap.cols.begin());
        cap.cols.push_back(0.0f);
    }
    while (cap.slideAcc <= -kColWidth) {
        cap.slideAcc += kColWidth;
        size_t last = cap.cols.size() - 1;
        if (cap.cols[last] >= 0.25f) {
            SpawnCapFlake((float)cap.rc.left + (last + 0.5f) * kColWidth,
                          (float)cap.rc.top - cap.cols[last] * 0.6f,
                          cap.cols[last], 0.0f);
        }
        cap.cols.pop_back();
        cap.cols.insert(cap.cols.begin(), 0.0f);
    }

    // Litter stays in screen space as well; whatever runs past an edge falls.
    float w = (float)(cap.rc.right - cap.rc.left);
    for (auto& l : cap.litter) l.offX -= dx;
    for (size_t j = 0; j < cap.litter.size();) {
        WinLitter& l = cap.litter[j];
        if (l.offX < 0.0f || l.offX >= w) {
            SpawnLitterFlake((float)cap.rc.left + l.offX,
                             (float)cap.rc.top - 2.0f, l, 0.0f);
            cap.litter.erase(cap.litter.begin() + j);
        } else {
            j++;
        }
    }
}

// Brings one cap up to date with its window's current rectangle.
static void MoveCap(WinCap& cap, const RECT& rc) {
    int oldW = cap.rc.right - cap.rc.left;
    int oldH = cap.rc.bottom - cap.rc.top;
    int newW = rc.right - rc.left;
    int newH = rc.bottom - rc.top;
    int dx = rc.left - cap.rc.left;
    int dy = rc.top  - cap.rc.top;

    if (dx == 0 && dy == 0 && oldW == newW && oldH == newH) return;

    // A jump (snap, maximise, a fast fling): the snow cannot follow and
    // falls where it was.
    if (abs(dx) > 120 || abs(dy) > 120) {
        DetachCap(cap, (float)dx);
        RebuildCap(cap, rc);
        return;
    }

    if (oldW == newW && oldH == newH) {
        // A plain move: the cap follows immediately, while sideways travel
        // scrapes snow off the trailing edge.
        SlideCap(cap, (float)dx);
        cap.rc = rc;
        return;
    }

    // Resized. Moving the top edge means the snow lost its support (the
    // window shrank from the top) or would end up buried inside the window
    // (it grew upwards), so it falls.
    if (dy != 0) {
        DetachCap(cap, (float)dx);
        RebuildCap(cap, rc);
        return;
    }

    // Width changed, top stayed put. Keep the snow that is still over the
    // window; the part that ran out of surface falls where it was.
    int n = newW / (int)kColWidth;
    if (n < 1) n = 1;
    if (n > 400) n = 400;
    std::vector<float> cols((size_t)n, 0.0f);
    int nOld = (int)cap.cols.size();
    for (int i = 0; i < nOld; i++) {
        float h = cap.cols[i];
        if (h < 0.25f) continue;
        float x = (float)cap.rc.left + (i + 0.5f) * kColWidth;
        if (x < (float)rc.left || x >= (float)rc.right) {
            SpawnCapFlake(x, (float)cap.rc.top - h * 0.6f, h, 0.0f);
            continue;
        }
        int j = (int)((x - (float)rc.left) / kColWidth);
        if (j < 0) j = 0;
        if (j >= n) j = n - 1;
        if (h > cols[j]) cols[j] = h;
    }
    cap.cols.swap(cols);

    for (auto& l : cap.litter) l.offX -= (float)dx;
    float w = (float)newW;
    for (size_t j = 0; j < cap.litter.size();) {
        if (cap.litter[j].offX < 0.0f || cap.litter[j].offX >= w)
            cap.litter.erase(cap.litter.begin() + j);
        else
            j++;
    }
    cap.slideAcc = 0.0f;
    cap.rc = rc;
}

// Picks up new top-level windows and forgets the ones that closed.
static void RefreshWindowList() {
    g_capsEnum.clear();
    EnumWindows(EnumWndProc, 0);

    for (size_t i = 0; i < g_caps.size();) {
        bool found = false;
        for (auto& e : g_capsEnum) {
            if (e.hwnd == g_caps[i].hwnd) { found = true; break; }
        }
        if (!found) g_caps.erase(g_caps.begin() + i);
        else i++;
    }

    for (auto& e : g_capsEnum) {
        bool known = false;
        for (auto& c : g_caps) {
            if (c.hwnd == e.hwnd) { known = true; break; }
        }
        if (known) continue;

        WinCap cap = {};
        cap.hwnd = e.hwnd;
        RebuildCap(cap, e.rc);
        g_caps.push_back(std::move(cap));
    }
}

// Runs every frame: a dragged window must not leave its snow in mid-air.
static void TrackWindowCaps() {
    for (size_t i = 0; i < g_caps.size();) {
        WinCap& c = g_caps[i];
        RECT rc;
        if (!IsWindow(c.hwnd) || !IsWindowVisible(c.hwnd) || IsIconic(c.hwnd) ||
            !GetWindowRect(c.hwnd, &rc)) {
            g_caps.erase(g_caps.begin() + i);
            continue;
        }
        MoveCap(c, rc);
        i++;
    }
}

// Z-order of the tracked windows, 0 = topmost. The overlay is a topmost
// window of its own, so only the relative order of the tracked windows
// matters here.
static void UpdateCapZOrder() {
    for (auto& c : g_caps) c.z = 0x7fffffff;

    int z = 0;
    for (HWND h = GetTopWindow(nullptr); h; h = GetWindow(h, GW_HWNDNEXT), z++) {
        for (auto& c : g_caps) {
            if (c.hwnd == h) { c.z = z; break; }
        }
    }
}

// Marks the columns of every cap that are hidden behind a higher window, so
// the snow of a window that is behind another one is not drawn on top of it.
static void UpdateCapVisibility() {
    for (auto& c : g_caps) {
        int n = (int)c.cols.size();
        c.vis.assign((size_t)n, 1);
        if (n == 0) continue;

        for (auto& o : g_caps) {
            if (o.hwnd == c.hwnd || o.z >= c.z) continue;
            // Only a window that starts at or above this top edge and reaches
            // into the thin strip above it can hide the snow.
            if (o.rc.top > c.rc.top) continue;
            if (o.rc.bottom <= c.rc.top - (int)kCapMaxH - 2) continue;

            int i0 = (int)ceilf(((float)o.rc.left - (float)c.rc.left) / kColWidth - 0.5f);
            int i1 = (int)floorf(((float)o.rc.right - (float)c.rc.left) / kColWidth - 0.5f);
            if (i1 < 0 || i0 >= n) continue;
            if (i0 < 0) i0 = 0;
            if (i1 >= n) i1 = n - 1;
            for (int i = i0; i <= i1; i++) c.vis[i] = 0;
        }
    }
}

static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;   // manual-reset, tells the thread to exit
static volatile LONG g_settingsDirty = 0;

static int  g_fpsCap = 0;
static UINT g_timerIntervalMs = 10;
static bool g_gustOnChange = true;

static LARGE_INTEGER g_qpcFreq = {};
static LARGE_INTEGER g_lastTick = {};

// Overlay thread only.
static bool g_paused = false;
static bool g_displayOff = false;
static HPOWERNOTIFY g_powerNotify = nullptr;

static int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : v > hi ? hi : v;
}

static void ApplySettingsToEngine() {
    WeatherSet next;
    PCWSTR mode = Wh_GetStringSetting(L"WeatherMode");
    ParseWeatherMode(mode, next);
    Wh_FreeStringSetting(mode);

    g_engine.intensity = ClampInt(Wh_GetIntSetting(L"Intensity"), 0, 200);
    g_engine.sizePct   = ClampInt(Wh_GetIntSetting(L"Size"), 50, 200);
    g_engine.speedPct  = ClampInt(Wh_GetIntSetting(L"Speed"), 20, 200);
    g_engine.wind      = ClampInt(Wh_GetIntSetting(L"Wind"), -30, 30);
    g_gustOnChange     = Wh_GetIntSetting(L"GustOnChange") != 0;
    g_engine.snowOnWindows   = Wh_GetIntSetting(L"SnowOnWindows") != 0;
    g_engine.leavesOnWindows = Wh_GetIntSetting(L"LeavesOnWindows") != 0;
    g_engine.streakSnow      = Wh_GetIntSetting(L"StreakSnow") != 0;
    g_engine.sweepWithCursor = Wh_GetIntSetting(L"SweepMouse") != 0;
    g_engine.autoWind        = ClampInt(Wh_GetIntSetting(L"AutoWind"), 0, 3600);

    g_engine.ApplyMode(next, g_gustOnChange);

    int fpsCap = ClampInt(Wh_GetIntSetting(L"FpsCap"), 0, 240);
    if (fpsCap != g_fpsCap) {
        g_fpsCap = fpsCap;
        g_timerIntervalMs = g_fpsCap > 0 ? (UINT)((1000 + g_fpsCap - 1) / g_fpsCap) : 10;
        if (g_wnd && !g_paused) SetTimer(g_wnd, 1, g_timerIntervalMs, nullptr);
    }
}

static void Tick(HWND hwnd) {
    if (InterlockedExchange(&g_settingsDirty, 0)) {
        ApplySettingsToEngine();
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = g_lastTick.QuadPart
        ? (float)((double)(now.QuadPart - g_lastTick.QuadPart) / (double)g_qpcFreq.QuadPart)
        : 0.016f;
    g_lastTick = now;
    if (dt > 0.05f) dt = 0.05f;   // clamp, e.g. after a sleep
    if (dt < 0.0f) dt = 0.0f;

    // Keep the caps glued to their windows and their occlusion up to date
    // before physics runs, so the cursor sweep cannot brush invisible snow.
    // The full window list is re-enumerated rarely, as that is the costly
    // part; rectangles and z-order are cheap and are updated every frame.
    static float enumTimer = 0.0f;
    enumTimer += dt;
    if (enumTimer >= 0.5f) {
        enumTimer = 0.0f;
        RefreshWindowList();
    }
    UpdateCapZOrder();
    TrackWindowCaps();
    UpdateCapVisibility();

    g_engine.Update(dt);

    // Cartoon wind sweep on a timer (0 = off).
    if (g_engine.autoWind > 0) {
        static float autoWindTimer = 0.0f;
        autoWindTimer += dt;
        if (autoWindTimer >= (float)g_engine.autoWind) {
            autoWindTimer = 0.0f;
            g_engine.BeginWeatherGust();
        }
    }

    RenderFrame(hwnd, g_engine);
}

// ─── Fullscreen, lock screen and display-off handling ───

// A foreground window that covers the whole primary monitor counts as a
// fullscreen app: a game, a video, a slideshow.
static bool IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_wnd) return false;

    wchar_t cls[64] = {};
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Shell_TrayWnd"))
        return false;

    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return false;

    // The overlay covers the primary monitor, which always starts at (0, 0).
    if (mi.rcMonitor.left != 0 || mi.rcMonitor.top != 0) return false;

    RECT rc;
    if (!GetWindowRect(fg, &rc)) return false;

    return rc.left  <= mi.rcMonitor.left && rc.top    <= mi.rcMonitor.top &&
           rc.right >= mi.rcMonitor.right && rc.bottom >= mi.rcMonitor.bottom;
}

// While the session is locked, the input desktop is not accessible.
static bool IsSessionLocked() {
    HDESK desk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (desk) {
        CloseDesktop(desk);
        return false;
    }
    return GetLastError() == ERROR_ACCESS_DENIED;
}

// Hides the overlay and stops rendering while it would only get in the way.
static void CheckPause(HWND hwnd) {
    bool pause = g_displayOff || IsSessionLocked() || IsFullscreenAppActive();
    if (pause == g_paused) return;
    g_paused = pause;

    if (pause) {
        KillTimer(hwnd, 1);
        ShowWindow(hwnd, SW_HIDE);
    } else {
        g_lastTick = {};
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        SetTimer(hwnd, 1, g_timerIntervalMs, nullptr);
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HDC screen = GetDC(nullptr);
            g_memDC = CreateCompatibleDC(screen);

            BITMAPINFO bi = {};
            bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth       = g_width;
            bi.bmiHeader.biHeight      = -g_height; // top-down
            bi.bmiHeader.biPlanes      = 1;
            bi.bmiHeader.biBitCount    = 32;
            bi.bmiHeader.biCompression = BI_RGB;

            g_dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &g_bits, nullptr, 0);
            ReleaseDC(nullptr, screen);

            if (!g_memDC || !g_dib || !InitD2D()) {
                Wh_Log(L"Overlay initialization failed");
                return -1;
            }
            SelectObject(g_memDC, g_dib);

            // The watchdog timer keeps running while rendering is paused, so
            // the overlay can come back by itself.
            SetTimer(hwnd, 2, 500, nullptr);
            g_powerNotify = RegisterPowerSettingNotification(
                hwnd, &GUID_CONSOLE_DISPLAY_STATE, DEVICE_NOTIFY_WINDOW_HANDLE);
            return 0;
        }

        case WM_TIMER:
            if (wParam == 1) {
                Tick(hwnd);
            } else if (wParam == 2) {
                CheckPause(hwnd);
            }
            return 0;

        case WM_POWERBROADCAST:
            if (wParam == PBT_POWERSETTINGCHANGE && lParam) {
                auto* ps = (POWERBROADCAST_SETTING*)lParam;
                if (IsEqualGUID(ps->PowerSetting, GUID_CONSOLE_DISPLAY_STATE) &&
                    ps->DataLength >= sizeof(DWORD)) {
                    g_displayOff = *(DWORD*)ps->Data == 0;
                    CheckPause(hwnd);
                }
            }
            return TRUE;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            KillTimer(hwnd, 2);
            if (g_powerNotify) {
                UnregisterPowerSettingNotification(g_powerNotify);
                g_powerNotify = nullptr;
            }
            if (g_dib) { DeleteObject(g_dib); g_dib = nullptr; }
            if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
            CleanupD2D();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI OverlayThreadProc(LPVOID) {
    g_engine.Seed(0x12345678u ^ GetTickCount());
    g_engine.width = g_width;
    g_engine.height = g_height;
    Wh_Log(L"overlay thread: %dx%d", g_width, g_height);

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWndClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return 1;
    }

    ApplySettingsToEngine();
    RefreshWindowList(); // snow caps appear right away, not after a second

    // Initial weather covers the screen right away.
    {
        int target = (int)(g_engine.intensity * 1.5f * g_engine.WeatherDensity() *
                           (g_width * (float)g_height) / (1920.0f * 1080.0f));
        if (target > 900) target = 900;
        while ((int)g_engine.flakes.size() < target) {
            Particle f = {};
            g_engine.SpawnFlake(f, true);
            g_engine.flakes.push_back(f);
        }
    }

    HWND wnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        kWndClass, L"DesktopWeatherOverlay", WS_POPUP,
        0, 0, g_width, g_height,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!wnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(kWndClass, wc.hInstance);
        return 1;
    }
    g_wnd = wnd;

    ShowWindow(wnd, SW_SHOWNOACTIVATE);
    g_lastTick = {};
    SetTimer(wnd, 1, g_timerIntervalMs, nullptr);

    // The message loop owns the window, but it must also wake up when the
    // mod is being unloaded, even before the window exists.
    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                            QS_ALLINPUT);
        if (r != WAIT_OBJECT_0 + 1) break; // stop event or an error

        MSG msg;
        bool quit = false;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) break;
    }

    if (IsWindow(wnd)) DestroyWindow(wnd);
    g_wnd = nullptr;
    UnregisterClassW(kWndClass, wc.hInstance);
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Tool mod lifecycle: the mod runs in its own windhawk.exe process, not in
// Explorer or any other host process.
// ─────────────────────────────────────────────────────────────────────────────

static void SetDpiAwareness() {
    // Physical pixels, so the overlay lines up with window rectangles on
    // scaled displays.
    using SetProcessDpiAwarenessContext_t = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return;
    auto setCtx = (SetProcessDpiAwarenessContext_t)GetProcAddress(
        user32, "SetProcessDpiAwarenessContext");
    if (setCtx) setCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Tool init");

    SetDpiAwareness();

    g_width  = GetSystemMetrics(SM_CXSCREEN);
    g_height = GetSystemMetrics(SM_CYSCREEN);
    if (g_width <= 0)  g_width = 1920;
    if (g_height <= 0) g_height = 1080;

    QueryPerformanceFrequency(&g_qpcFreq);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return FALSE;
    }

    g_settingsDirty = 0;
    g_thread = CreateThread(nullptr, 0, OverlayThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    InterlockedExchange(&g_settingsDirty, 1);
}

void WhTool_ModUninit() {
    Wh_Log(L"Tool uninit");

    if (g_stopEvent) SetEvent(g_stopEvent);

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
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

// ==WindhawkMod==
// @id              desktop-weather-overlay
// @name            Desktop Weather Overlay
// @description     Snow, cherry blossom, autumn leaves and rain over the desktop, with wind and a gust on weather change.
// @version         1.1.3
// @author          damir
// @include         explorer.exe
// @compilerOptions -ld2d1 -ldwmapi -lgdi32 -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Weather Overlay

Animated weather rendered over the whole desktop: snow, cherry blossom
(sakura) petals, autumn leaves and rain. Several kinds can be mixed
together. Direct2D renders into a per-monitor transparent layered window,
so the desktop and all windows stay fully interactive.

## Features

- Four weather kinds: snow, cherry blossom, autumn leaves, rain.
- Mixable: "Winter + spring", "Autumn + rain" or "All together".
- Wind affects drift, slant of the rain and the direction of the gust.
- On a weather change a cartoon wind sweeps across the screen, carries the
  old weather away, and the new one drifts in gradually from the top.
- Snow settles on the top edges of windows and the taskbar; leaves and
  petals may stay lying there too, a few at a time.
- The mouse can sweep snow, leaves and petals off window tops.
- Snow that cannot follow a moving window comes loose and falls.
- Cartoon wind sweep can also run on a timer to clean everything up.
- Optional cartoon-style gray streaks for snow.
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

#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <algorithm>


// ─────────────────────────────────────────────────────────────────────────────
// Engine constants (ported from the SnowDesktop physics)
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
static const float kMeltPerSec  = 0.05f;  // slow melt so caps don't grow forever

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
};

// Owned by the overlay thread only.
static std::vector<WinCap> g_caps;

static bool g_loggedFirstLanding = false;

static void LogToFile(const wchar_t* fmt, ...) {
    wchar_t path[MAX_PATH];
    if (GetEnvironmentVariableW(L"USERPROFILE", path, MAX_PATH)) {
        wcscat(path, L"\\Desktop\\weather_debug.txt");
        FILE* f = _wfopen(path, L"a, csc=UTF-16LE");
        if (!f) f = _wfopen(L"C:\\SnowLog.txt", L"a, csc=UTF-16LE");
        if (f) {
            SYSTEMTIME st;
            GetLocalTime(&st);
            fwprintf(f, L"[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
            va_list args;
            va_start(args, fmt);
            vfwprintf(f, fmt, args);
            va_end(args);
            fwprintf(f, L"\n");
            fclose(f);
        }
    }
    // Also log to Windhawk's internal log
    va_list args2;
    va_start(args2, fmt);
    wchar_t buf[512];
    vswprintf_s(buf, fmt, args2);
    Wh_Log(L"%s", buf);
    va_end(args2);
}

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
                f.fallSpeed = 35.0f + f.size * 22.0f;
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
                if (h < 0.4f) continue;

                if (debrisBudget > 0 && flakes.size() < 1100) {
                    Particle p = {};
                    p.kind      = kSnow;
                    p.size      = 0.7f + h * 0.25f;
                    p.fallSpeed = 45.0f + h * 6.0f;
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
                for (auto& h : c.cols)
                    if (h > 0.0f) h -= kMeltPerSec * dt;
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

static bool InitD2D() {
    D2D1_FACTORY_OPTIONS fo = {};
    fo.debugLevel = D2D1_DEBUG_LEVEL_NONE;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                   kIID_ID2D1Factory, &fo, (void**)&g_factory);
    if (FAILED(hr) || !g_factory) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08lX", hr);
        return false;
    }

    D2D1_RENDER_TARGET_PROPERTIES rtProps = {};
    rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rtProps.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    rtProps.usage    = D2D1_RENDER_TARGET_USAGE_NONE;
    rtProps.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    rtProps.dpiX     = 0.0f;
    rtProps.dpiY     = 0.0f;

    hr = g_factory->CreateDCRenderTarget(&rtProps, &g_rt);
    if (FAILED(hr) || !g_rt) {
        Wh_Log(L"CreateDCRenderTarget failed: 0x%08lX", hr);
        return false;
    }

    hr = g_rt->CreateSolidColorBrush(Col(kSnowColor, 1.0f), &g_brush);
    if (FAILED(hr) || !g_brush) {
        Wh_Log(L"CreateSolidColorBrush failed: 0x%08lX", hr);
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
        for (float h : c.cols)
            if (h > 0.05f) { any = true; break; }

        if (any) {
            ID2D1PathGeometry* geo = nullptr;
            ID2D1GeometrySink* sink = nullptr;
            if (SUCCEEDED(g_factory->CreatePathGeometry(&geo)) &&
                SUCCEEDED(geo->Open(&sink))) {
                sink->SetFillMode(D2D1_FILL_MODE_WINDING);
                D2D1_POINT_2F start = { left, top + 1.0f };
                sink->BeginFigure(start, D2D1_FIGURE_BEGIN_FILLED);
                for (int i = 0; i < n; i++) {
                    float h = c.cols[i] * 2.0f;
                    if (i > 0)     h += c.cols[i - 1];
                    if (i + 1 < n) h += c.cols[i + 1];
                    h *= 0.25f;
                    D2D1_POINT_2F pt = { left + (float)i * kColWidth, top - h };
                    sink->AddLine(pt);
                }
                D2D1_POINT_2F end = { right, top + 1.0f };
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
    static bool loggedFirstFrame = false;
    if (!loggedFirstFrame) {
        loggedFirstFrame = true;
        LogToFile(L"RenderFrame: first frame attempt");
    }

    if (!g_rt || !g_memDC) return false;

    RECT rc = { 0, 0, g_width, g_height };
    g_rt->BindDC(g_memDC, &rc);
    g_rt->BeginDraw();
    g_rt->Clear(nullptr);
    DrawScene(g_rt, eng);
    DrawSweeps(g_rt, eng);
    g_rt->EndDraw();

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
// Overlay window (owned by a dedicated thread inside explorer.exe)
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
    p.fallSpeed = 40.0f + h * 6.0f;
    p.sway      = 8.0f;
    p.alpha     = 0.9f;
    p.flutter   = 1.0f;
    p.vx        = vx * 0.25f + (g_engine.Rand() - 0.5f) * 30.0f;
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
        if (g_engine.flakes.size() >= 1100) break;
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
        p.x         = (float)cap.rc.left + l.offX;
        p.y         = (float)cap.rc.top - 2.0f;
        p.debris    = true;
        g_engine.flakes.push_back(p);
    }
    cap.litter.clear();
}

static void UpdateWindowList() {
    g_capsEnum.clear();
    EnumWindows(EnumWndProc, 0);

    // Merge with the previous list, keeping snow columns and litter.
    std::vector<WinCap> merged;
    merged.reserve(g_capsEnum.size());
    for (auto& nu : g_capsEnum) {
        WinCap c = std::move(nu);
        int n = (c.rc.right - c.rc.left) / (int)kColWidth;
        if (n < 1) n = 1;
        if (n > 400) n = 400;
        c.cols.assign(n, 0.0f);

        for (auto& old : g_caps) {
            if (old.hwnd != c.hwnd) continue;

            int dx = (int)(c.rc.left - old.rc.left);
            int dy = (int)(c.rc.top  - old.rc.top);

            if (dx == 0 && dy == 0) {
                // The window is standing still: snow and litter stay put.
                c.litter = old.litter;
                size_t m = old.cols.size() < (size_t)n ? old.cols.size() : (size_t)n;
                for (size_t i = 0; i < m; i++) c.cols[i] = old.cols[i];
                break;
            }

            if (dy < -4 || dx < -60 || dx > 60 || dy > 60) {
                // The window jumped (snap, maximise, or pulled up faster
                // than the snow can follow). The cap comes loose and falls
                // where it was instead of sticking to the new position.
                WinCap moved = old;
                DetachCap(moved, (float)dx);
                break;
            }

            // A slow drag: the cap rides along with the window. Columns
            // scraped past a side edge slide off and fall to the ground.
            int shift = dx >= 0
                ? (dx + (int)kColWidth / 2) / (int)kColWidth
                : -((-dx + (int)kColWidth / 2) / (int)kColWidth);
            int nOld = (int)old.cols.size();
            int lo = shift > 0 ? 0 : nOld + shift;
            int hi = shift > 0 ? shift : nOld;
            if (lo < 0) lo = 0;
            if (hi > nOld) hi = nOld;
            for (int i = lo; i < hi; i++) {
                SpawnCapFlake((float)old.rc.left + (i + 0.5f) * kColWidth,
                              (float)old.rc.top - old.cols[i] * 0.6f,
                              old.cols[i], 0.0f);
            }

            for (int i = 0; i < n; i++) {
                int src = i + shift;
                if (src < 0 || src >= nOld) continue;
                c.cols[i] = old.cols[src];
            }

            c.litter = old.litter;
            for (auto& l : c.litter) l.offX -= (float)dx;
            c.litter.erase(std::remove_if(c.litter.begin(), c.litter.end(),
                                          [n](const WinLitter& l) {
                                              return l.offX < 0.0f ||
                                                     l.offX >= (float)n * kColWidth;
                                          }),
                           c.litter.end());
            break;
        }
        merged.push_back(std::move(c));
    }
    g_caps = std::move(merged);
}

static HANDLE g_thread = nullptr;
static volatile LONG g_settingsDirty = 0;

static int  g_fpsCap = 0;
static UINT g_timerIntervalMs = 10;
static bool g_gustOnChange = true;

static LARGE_INTEGER g_qpcFreq = {};

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
        if (g_wnd) SetTimer(g_wnd, 1, g_timerIntervalMs, nullptr);
    }
    LogToFile(L"Settings updated: intensity=%d, size=%d, speed=%d, wind=%d, gust=%d, snowWin=%d, leafWin=%d, streak=%d, autowind=%d, fps=%d",
        Wh_GetIntSetting(L"Intensity"), g_engine.sizePct, g_engine.speedPct, g_engine.wind, g_gustOnChange, g_engine.snowOnWindows, g_engine.leavesOnWindows, g_engine.streakSnow, g_engine.autoWind, g_fpsCap);
}

static void Tick(HWND hwnd) {
    if (InterlockedExchange(&g_settingsDirty, 0)) {
        ApplySettingsToEngine();
    }

    static LARGE_INTEGER last = {};
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = last.QuadPart
        ? (float)((double)(now.QuadPart - last.QuadPart) / (double)g_qpcFreq.QuadPart)
        : 0.016f;
    last = now;
    if (dt > 0.05f) dt = 0.05f;   // clamp, e.g. after a sleep
    if (dt < 0.0f) dt = 0.0f;

    g_engine.Update(dt);

    // Window list refresh: often enough that a dragged window does not leave
    // its snow hanging in mid-air.
    static float capsTimer = 0.2f;
    capsTimer += dt;
    if (capsTimer >= 0.2f) {
        capsTimer = 0.0f;
        UpdateWindowList();
    }

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
            return 0;
        }

        case WM_TIMER:
            if (wParam == 1) Tick(hwnd);
            return 0;

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
            if (g_dib) { DeleteObject(g_dib); g_dib = nullptr; }
            if (g_memDC) { DeleteDC(g_memDC); g_memDC = nullptr; }
            CleanupD2D();
            PostQuitMessage(0);
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
    LogToFile(L"overlay thread started: %dx%d", g_width, g_height);

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWndClass;
    if (!RegisterClassExW(&wc)) {
        DWORD err = GetLastError();
        if (err == ERROR_CLASS_ALREADY_EXISTS) {
            LogToFile(L"Class already exists, re-registering...");
            UnregisterClassW(kWndClass, wc.hInstance);
            if (!RegisterClassExW(&wc)) {
                LogToFile(L"Re-registration failed: %lu", GetLastError());
                return 1;
            }
        } else {
            LogToFile(L"RegisterClassExW failed: %lu", err);
            return 1;
        }
    }

    ApplySettingsToEngine();
    UpdateWindowList(); // snow caps appear right away, not after 2s

    // Initial weather covers the screen right away.
    {
        WeatherSet w;
        PCWSTR mode = Wh_GetStringSetting(L"WeatherMode");
        ParseWeatherMode(mode, w);
        Wh_FreeStringSetting(mode);
        int target = (int)(g_engine.intensity * 1.5f * g_engine.WeatherDensity() *
                           (g_width * (float)g_height) / (1920.0f * 1080.0f));
        if (target > 900) target = 900;
        while ((int)g_engine.flakes.size() < target) {
            Particle f = {};
            g_engine.SpawnFlake(f, true);
            g_engine.flakes.push_back(f);
        }
    }

    g_wnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        kWndClass, L"DesktopWeatherOverlay", WS_POPUP,
        0, 0, g_width, g_height,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_wnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        LogToFile(L"CreateWindowExW failed: %lu", GetLastError());
        return 1;
    }
    LogToFile(L"Window created: %p", g_wnd);

    ShowWindow(g_wnd, SW_SHOWNOACTIVATE);
    SetTimer(g_wnd, 1, g_timerIntervalMs, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_wnd = nullptr;
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk mod lifecycle
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    g_width  = GetSystemMetrics(SM_CXSCREEN);
    g_height = GetSystemMetrics(SM_CYSCREEN);
    if (g_width <= 0)  g_width = 1920;
    if (g_height <= 0) g_height = 1080;

    QueryPerformanceFrequency(&g_qpcFreq);

    g_settingsDirty = 0;
    g_thread = CreateThread(nullptr, 0, OverlayThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
    }
}

BOOL Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");

    if (g_wnd) {
        PostMessageW(g_wnd, WM_CLOSE, 0, 0);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    InterlockedExchange(&g_settingsDirty, 1);
}

// ==WindhawkMod==
// @id              winduo
// @name            WinDuo
// @description     Lean the screen back as the laptop lid closes, using the webcam instead of a hinge sensor
// @version         0.1.0
// @author          Basel Ashraf
// @github          https://github.com/BaselAshraf81
// @homepage        https://baselashraf.com/winduo
// @include         windhawk.exe
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ld3dcompiler_47 -lmfplat -lmfreadwrite -lmf -lmfuuid -lole32 -luuid -lwtsapi32
// @license         Apache-2.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinDuo

As the laptop lid closes, the picture on screen leans back, blurs and dims, as
though it were a sheet hinged at the bottom of the display that stays put in the
room while the glass turns away from it. The top of the sheet tips forward a
little, so content near the top edge stays legible for longer.

There is no hinge sensor on most laptops, so the lid angle comes from the
webcam: closing the lid pitches the camera down, the scene slides up the frame,
and the size of that slide is the angle.

This is the Windhawk edition of [WinDuo](https://github.com/BaselAshraf81/winduo).
Its geometry, shader and state machine are ported from
[Mac Duo](https://github.com/sumimakito/Mac-Duo) (Apache 2.0, Copyright 2026
Makito).
It runs as its own process (a Windhawk *tool mod*), injects into nothing, and
hooks nothing.

![The screen leaning back as the lid closes](https://raw.githubusercontent.com/BaselAshraf81/winduo/main/docs/windhawk-preview.gif)

## What to expect

- **The camera stays on while you are using the laptop**, so its privacy light
  stays lit. It has to be watching before the lid starts to move. It turns off
  while the lid is shut, the display is off, the session is locked, or the
  built-in screen is not in use. Frames are measured in memory and never stored
  or sent anywhere. Disable the mod to turn the camera off entirely.
- **Battery.** A streaming webcam costs roughly as much as a video call's
  camera does, which on most laptops is well under a watt but is not nothing.
- **Other camera apps.** On many laptops, especially on Windows 10, only one
  app can use the camera at a time. While the mod is streaming, a video call or
  the Camera app started afterwards may report that the camera is in use:
  disable the mod for the call. If the other app has the camera first, the mod
  backs off and tries again later (every few seconds at first, then up to once
  a minute) until the camera is free.
- **Built-in screen only.** The effect runs on the laptop's own panel, even
  when an external monitor is the main display, and does nothing while the
  laptop is docked with the lid shut.
- **Camera permission.** The mod reads the camera as an ordinary desktop app
  (it runs inside `windhawk.exe`), so Windows shows no prompt. It needs
  *Settings > Privacy & security > Camera > Let desktop apps access your
  camera* turned on, and `windhawk.exe` then appears in that list while the
  mod runs. If the setting is off, the mod log says so.
- It needs Windows 10 version 2004 or later, so the overlay can keep itself out
  of its own screen capture.
- Accuracy depends on the room: a lit room with some detail in front of the
  camera tracks well; a dark room or a blank wall does not, and the effect then
  declines to run rather than guessing.

## Calibration

The default assumes a typical laptop webcam (about a 75 degree field of view).
If the effect starts too early or too late, adjust **Degrees per pixel**: raise
it if a small close already triggers the effect, lower it if a large one does
not. The desktop app has a calibration wizard that measures this exactly.

## Trying it without closing the lid

Turn on **Play a preview** and save. The effect plays one scripted close on
whatever is on screen, then the switch can be turned back off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TriggerTravel: 10
  $name: Start after (degrees of closing)
- FullEffectTravel: 75
  $name: Full strength after (further degrees)
- ReleaseHysteresis: 4
  $name: Release margin (degrees)
- TiltLimit: 55
  $name: Tilt limit (degrees)
  $description: The most the picture tilts, however far the lid closes. 0 follows the lid all the way.
- TopLean: 60
  $name: Top lean (percent)
  $description: How far the top of the picture tips toward you while the bottom stays put.
- ViewingDistance: 30
  $name: Viewing distance (tenths of a screen height)
  $description: Smaller means stronger perspective. 30 is a laptop at arm's length.
- MaxBlurRadius: 90
  $name: Blur at the far edge (pixels)
- MaxDim: 85
  $name: Dimming at the far edge (percent)
- DimReach: 50
  $name: Dimming reach (percent of the height)
- HingeGlow: 50
  $name: Hinge highlight (percent)
- Reflection: 50
  $name: Reflection band (percent)
- LivePicture: true
  $name: Keep the picture live
  $description: Off holds the frame from the moment the effect starts.
- DegreesPerPixel: 269
  $name: Degrees per pixel (thousandths)
  $description: Lid degrees per pixel of camera shift at 320 pixels wide. See the readme.
- NeutralAngle: 100
  $name: Usual lid angle (degrees)
- CameraIndex: 0
  $name: Camera number
- ConfidenceFloor: 35
  $name: Required tracking confidence (percent)
- Preview: false
  $name: Play a preview
  $description: Plays one scripted close when saved with this on.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wtsapi32.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

// Per-monitor-v2 makes the overlay and Desktop Duplication agree on physical
// pixels on a scaled display.
static void UsePhysicalPixels() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

template <typename T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

static double Clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
static double Clamp01(double v) { return Clamp(v, 0.0, 1.0); }

static double Now() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return double(c.QuadPart) / double(freq.QuadPart);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    double triggerTravel = 10;
    double fullEffectTravel = 75;
    double releaseHysteresis = 4;
    double tiltLimit = 55;
    double topLean = 0.6;
    double viewingDistance = 3.0;
    double maxBlurRadius = 90;
    double maxDim = 0.85;
    double dimReach = 0.5;
    double hingeGlow = 0.5;
    double reflection = 0.5;
    bool livePicture = true;
    double degreesPerPixel = 0.269;
    double neutralAngle = 100;
    int cameraIndex = 0;
    double confidenceFloor = 0.35;
    bool preview = false;
};

static std::mutex g_settingsLock;
static Settings g_settings;
static std::atomic<int> g_settingsVersion{0};
static std::atomic<bool> g_previewRequested{false};

// `changed` is false on the first load, so a Preview switch left on does not
// replay the preview at every sign-in.
static void LoadSettings(bool changed) {
    Settings s;
    s.triggerTravel = Clamp(Wh_GetIntSetting(L"TriggerTravel"), 2, 40);
    s.fullEffectTravel = Clamp(Wh_GetIntSetting(L"FullEffectTravel"), 5, 90);
    s.releaseHysteresis = Clamp(Wh_GetIntSetting(L"ReleaseHysteresis"), 1, 15);
    s.tiltLimit = Clamp(Wh_GetIntSetting(L"TiltLimit"), 0, 90);
    s.topLean = Clamp(Wh_GetIntSetting(L"TopLean") / 100.0, 0, 1);
    s.viewingDistance = Clamp(Wh_GetIntSetting(L"ViewingDistance") / 10.0, 1, 6);
    s.maxBlurRadius = Clamp(Wh_GetIntSetting(L"MaxBlurRadius"), 10, 110);
    s.maxDim = Clamp(Wh_GetIntSetting(L"MaxDim") / 100.0, 0, 1);
    s.dimReach = Clamp(Wh_GetIntSetting(L"DimReach") / 100.0, 0.2, 1);
    s.hingeGlow = Clamp(Wh_GetIntSetting(L"HingeGlow") / 100.0, 0, 1);
    s.reflection = Clamp(Wh_GetIntSetting(L"Reflection") / 100.0, 0, 1);
    s.livePicture = Wh_GetIntSetting(L"LivePicture") != 0;
    int dpp = Wh_GetIntSetting(L"DegreesPerPixel");
    s.degreesPerPixel = Clamp((dpp > 0 ? dpp : 269) / 1000.0, 0.02, 2.0);
    int neutral = Wh_GetIntSetting(L"NeutralAngle");
    s.neutralAngle = Clamp(neutral > 0 ? neutral : 100, 30, 170);
    s.cameraIndex = (int)Clamp(Wh_GetIntSetting(L"CameraIndex"), 0, 16);
    s.confidenceFloor = Clamp(Wh_GetIntSetting(L"ConfidenceFloor") / 100.0, 0, 0.95);
    s.preview = Wh_GetIntSetting(L"Preview") != 0;

    bool startPreview;
    {
        std::lock_guard<std::mutex> lock(g_settingsLock);
        startPreview = changed && s.preview && !g_settings.preview;
        g_settings = s;
    }
    if (startPreview) {
        g_previewRequested = true;
    }
    g_settingsVersion++;
}

static Settings CurrentSettings() {
    std::lock_guard<std::mutex> lock(g_settingsLock);
    return g_settings;
}

// ---------------------------------------------------------------------------
// Geometry. A port of winduo/effect/geometry.py (DepthGeometry.profile).
//
// The picture is a sheet hinged to the bottom edge of the screen. Near the
// hinge it stays fixed in the room while the glass turns under it; above
// kLeanStart it curls forward toward the glass by `topLean`. A bent sheet is
// not a homography, so the result is a per-row lookup: entry i covers screen
// row i / (kProfileSamples - 1) * end, and holds the picture row (as a
// fraction of the height) and the horizontal scale about the centre there.
// ---------------------------------------------------------------------------

constexpr int kProfileSamples = 128;
constexpr double kLeanStart = 0.35;
constexpr double kMaxSeparation = 88.0;
constexpr double kMinDepthFraction = 0.1;
constexpr double kPi = 3.14159265358979323846;

struct Profile {
    float table[kProfileSamples][2];
    float end;
    float slope;
};

static double Smoothstep(double e0, double e1, double x) {
    double t = Clamp01((x - e0) / (e1 - e0));
    return t * t * (3 - 2 * t);
}

static bool BuildProfile(double startAngle, double currentAngle,
                         double viewingDistance, double topLean, double width,
                         double height, Profile& out) {
    if (width <= 0 || height <= 0) {
        return false;
    }
    double start = startAngle * kPi / 180;
    double current = currentAngle * kPi / 180;
    double travel = std::max(startAngle - currentAngle, 0.0);
    double separation = std::min(travel, kMaxSeparation) * kPi / 180;
    double lean = Clamp01(topLean);

    double reach = height * viewingDistance + height / 2 * std::cos(start);
    double rise = height / 2 * std::sin(start);
    double along = reach * std::cos(current) + rise * std::sin(current);
    double depth = std::max(reach * std::sin(current) - rise * std::cos(current),
                            height * kMinDepthFraction);

    const int steps = kProfileSamples * 4;
    double ds = height / steps;
    double u = 0, b = 0;
    std::vector<double> arc(steps + 1), rows(steps + 1), scales(steps + 1);
    arc[0] = 0;
    rows[0] = 0;
    scales[0] = 1;
    for (int i = 0; i < steps; i++) {
        double mid = (i + 0.5) * ds / height;
        double angle = separation * (1 - lean * Smoothstep(kLeanStart, 1, mid));
        u += std::cos(angle) * ds;
        b += std::sin(angle) * ds;
        double scale = depth / (depth + b);
        arc[i + 1] = (i + 1) * ds / height;
        rows[i + 1] = std::max(along + (u - along) * scale, rows[i]);
        scales[i + 1] = scale;
    }
    double end = rows[steps];
    if (!(end > 0) || !std::isfinite(end)) {
        return false;
    }

    int j = 0;
    for (int i = 0; i < kProfileSamples; i++) {
        double target = end * i / (kProfileSamples - 1);
        while (j < steps - 1 && rows[j + 1] < target) {
            j++;
        }
        double span = rows[j + 1] - rows[j];
        double t = span <= 1e-12 ? 0 : Clamp01((target - rows[j]) / span);
        out.table[i][0] = float(arc[j] + (arc[j + 1] - arc[j]) * t);
        out.table[i][1] = float(scales[j] + (scales[j + 1] - scales[j]) * t);
    }
    double last = rows[steps] - rows[steps - 1];
    out.end = float(end);
    out.slope = float(last > 1e-12 ? (arc[steps] - arc[steps - 1]) / last : 0);
    return true;
}

static double TiltKnee(double travel, double limit) {
    if (travel <= 0) {
        return 0;
    }
    if (limit <= 0) {
        return travel;
    }
    return limit * std::tanh(travel / limit);
}

// Curves from winduo/effect/gradient.py.
static double BlurCurve(double p) { return std::pow(Clamp01(p), 1.6); }
static double DimCurve(double p) { return std::pow(Clamp01(p), 0.7); }
static double TurnCurve(double p) { return std::pow(Clamp01(p), 1.8); }
constexpr double kDimHingeFloor = 0.2;
static double MotionBoost(double velocity) {
    double speed = std::fabs(velocity);
    if (speed <= 30) {
        return 0;
    }
    return Clamp01((speed - 30) / 190) * 0.35;
}

// ---------------------------------------------------------------------------
// Estimates handed from the camera thread to the render thread.
// ---------------------------------------------------------------------------

struct AngleSample {
    double travel = 0;       // degrees closed from neutral, positive = closing
    double velocity = 0;     // degrees per second, positive = closing
    double confidence = 0;   // 0..1
    double timestamp = 0;
    bool valid = false;
};

// ---------------------------------------------------------------------------
// Controller. A port of winduo/effect/controller.py.
// ---------------------------------------------------------------------------

constexpr double kTriggerSpeed = 4.0;
constexpr double kClosingMemory = 1.2;
constexpr double kPredictionSpeedFloor = 30.0;
constexpr double kPredictionLatency = 0.05;
constexpr double kMinimumDuration = 0.3;
constexpr double kSettleEpsilon = 0.05;
constexpr double kClosingOutTimeout = 1.2;

struct Frame {
    double startAngle;
    double currentAngle;
    double progress;
    double velocity;
    bool isFinal;
};

class Controller {
   public:
    enum class Phase { Idle, Running, ClosingOut };

    void Observe(const AngleSample& s, double now) {
        if (!s.valid) {
            confidence_ = 0;
            velocity_ = 0;
            return;
        }
        rawTravel_ = s.travel;
        velocity_ = s.velocity;
        confidence_ = s.confidence;
        sampleTime_ = s.timestamp;
        if (s.velocity >= kTriggerSpeed) {
            lastClosing_ = now;
        }
    }

    void LidShut() { Abandon(); }

    bool Step(const Settings& st, double now, Frame& out) {
        double dt = Delta(now);
        bool wanted = Wants(st, now);
        if (phase_ == Phase::Idle && wanted) {
            phase_ = Phase::Running;
            startedAt_ = now;
            springValue_ = std::max(rawTravel_, 0.0);
            springVelocity_ = 0;
            Wh_Log(L"effect on: travel %.1f, %.0f deg/s, confidence %.2f",
                   rawTravel_, velocity_, confidence_);
        } else if (phase_ == Phase::Running && !wanted) {
            phase_ = Phase::ClosingOut;
            closingOutSince_ = now;
        }
        if (phase_ == Phase::Idle) {
            return false;
        }

        double target = phase_ == Phase::ClosingOut ? 0 : std::max(rawTravel_, 0.0);
        // Critically damped spring, semi-implicit Euler, 16 rad/s.
        const double f = 16.0;
        double accel = f * f * (target - springValue_) - 2 * f * springVelocity_;
        springVelocity_ += accel * dt;
        springValue_ += springVelocity_ * dt;

        if (phase_ != Phase::ClosingOut) {
            out = MakeFrame(st, springValue_, false);
            return true;
        }
        bool settled = springValue_ <= kSettleEpsilon;
        bool timedOut = now - closingOutSince_ > kClosingOutTimeout;
        if (!(settled || timedOut)) {
            out = MakeFrame(st, springValue_, false);
            return true;
        }
        springValue_ = 0;
        springVelocity_ = 0;
        phase_ = Phase::Idle;
        out = MakeFrame(st, 0, true);
        return true;
    }

    bool Active() const { return phase_ != Phase::Idle; }
    double Travel() const { return rawTravel_; }
    double Velocity() const { return velocity_; }

    void Abandon() {
        phase_ = Phase::Idle;
        springValue_ = 0;
        springVelocity_ = 0;
        lastClosing_ = -1e9;
    }

   private:
    bool Wants(const Settings& st, double now) const {
        if (confidence_ < st.confidenceFloor) {
            return false;
        }
        if (phase_ == Phase::Running) {
            if (now - startedAt_ <= kMinimumDuration) {
                return true;
            }
            return rawTravel_ > st.triggerTravel - st.releaseHysteresis;
        }
        bool closingRecently = now - lastClosing_ < kClosingMemory;
        return closingRecently && Predicted(now) >= st.triggerTravel;
    }

    double Predicted(double now) const {
        if (velocity_ < kPredictionSpeedFloor) {
            return rawTravel_;
        }
        double staleness = Clamp(now - sampleTime_, 0, 0.12);
        return rawTravel_ + velocity_ * (staleness + kPredictionLatency);
    }

    Frame MakeFrame(const Settings& st, double travel, bool isFinal) const {
        double span = std::max(st.fullEffectTravel, 1.0);
        double past = std::max(travel - st.triggerTravel, 0.0);
        double progress = std::min(past / span, 1.0);
        double held = travel <= st.triggerTravel
                          ? travel
                          : st.triggerTravel + TiltKnee(std::min(past, span), st.tiltLimit);
        return Frame{st.neutralAngle - st.triggerTravel, st.neutralAngle - held,
                     progress, velocity_, isFinal};
    }

    double Delta(double now) {
        double last = lastFrame_;
        lastFrame_ = now;
        if (last <= 0) {
            return 1.0 / 60;
        }
        return Clamp(now - last, 1.0 / 240, 1.0 / 20);
    }

    Phase phase_ = Phase::Idle;
    double rawTravel_ = 0, velocity_ = 0, confidence_ = 0, sampleTime_ = 0;
    double lastClosing_ = -1e9, startedAt_ = 0, closingOutSince_ = 0, lastFrame_ = 0;
    double springValue_ = 0, springVelocity_ = 0;
};

// ---------------------------------------------------------------------------
// Tracker. Measures vertical image shift between consecutive camera frames.
//
// The camera sits in the lid, so lid rotation is pure camera pitch: the whole
// image slides vertically. The desktop app uses 2D phase correlation; here the
// same quantity comes from correlating row profiles (each row's mean
// brightness, high-passed), which needs no FFT and costs a few microseconds.
// Horizontal drift and left/right disagreement (roll) are measured the same way
// and veto the reading, because the hinge can neither pan nor roll: those mean
// the whole laptop moved.
// ---------------------------------------------------------------------------

constexpr int kTrackWidth = 320;

struct ShiftReading {
    double shift = 0;  // positive = closing (content moved up)
    double drift = 0;
    double roll = 0;
    double response = 0;
    double confidence = 0;
    bool degenerate = false;
};

static double Ramp(double v, double lo, double hi) {
    if (hi <= lo) {
        return v >= hi ? 1 : 0;
    }
    return Clamp01((v - lo) / (hi - lo));
}

static void HighPass(std::vector<double>& p) {
    int n = (int)p.size();
    const int half = 12;
    std::vector<double> prefix(n + 1, 0);
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + p[i];
    }
    std::vector<double> out(n);
    for (int i = 0; i < n; i++) {
        int a = std::max(0, i - half), b = std::min(n, i + half + 1);
        out[i] = p[i] - (prefix[b] - prefix[a]) / (b - a);
    }
    p.swap(out);
}

// Finds s where cur[i] ~= prev[i + s]. Returns the sub-pixel shift and the
// normalised correlation at the peak.
static bool Correlate(const std::vector<double>& prev,
                      const std::vector<double>& cur, int maxShift,
                      double& shift, double& peak) {
    int n = (int)cur.size();
    if (n < 16 || (int)prev.size() != n) {
        return false;
    }
    maxShift = std::min(maxShift, n / 2);
    std::vector<double> score(2 * maxShift + 1, -2);
    int best = 0;
    double bestScore = -2;
    for (int s = -maxShift; s <= maxShift; s++) {
        int lo = std::max(0, -s), hi = std::min(n, n - s);
        double sab = 0, saa = 0, sbb = 0;
        for (int i = lo; i < hi; i++) {
            double a = prev[i + s], b = cur[i];
            sab += a * b;
            saa += a * a;
            sbb += b * b;
        }
        double d = std::sqrt(saa * sbb);
        double c = d > 1e-9 ? sab / d : 0;
        score[s + maxShift] = c;
        if (c > bestScore) {
            bestScore = c;
            best = s;
        }
    }
    double sub = 0;
    if (best > -maxShift && best < maxShift) {
        double l = score[best - 1 + maxShift], m = score[best + maxShift],
               r = score[best + 1 + maxShift];
        double den = l - 2 * m + r;
        if (std::fabs(den) > 1e-12) {
            sub = Clamp(0.5 * (l - r) / den, -0.5, 0.5);
        }
    }
    shift = best + sub;
    peak = bestScore;
    return true;
}

class ShiftTracker {
   public:
    bool Feed(const std::vector<float>& img, int w, int h, ShiftReading& r) {
        std::vector<double> rows(h, 0), left(h, 0), right(h, 0), cols(w, 0);
        double sum = 0, sq = 0;
        int half = w / 2;
        for (int y = 0; y < h; y++) {
            const float* row = &img[size_t(y) * w];
            double rs = 0, ls = 0;
            for (int x = 0; x < w; x++) {
                double v = row[x];
                rs += v;
                if (x < half) {
                    ls += v;
                }
                cols[x] += v;
                sum += v;
                sq += v * v;
            }
            rows[y] = rs / w;
            left[y] = ls / half;
            right[y] = (rs - ls) / (w - half);
        }
        for (auto& c : cols) {
            c /= h;
        }
        HighPass(rows);
        HighPass(left);
        HighPass(right);
        HighPass(cols);

        bool first = prevRows_.empty() || (int)prevRows_.size() != h ||
                     (int)prevCols_.size() != w;
        std::swap(prevRows_, rows);
        std::swap(prevLeft_, left);
        std::swap(prevRight_, right);
        std::swap(prevCols_, cols);
        if (first) {
            return false;
        }

        double n = double(w) * h;
        double mean = sum / n;
        double contrast = std::sqrt(std::max(sq / n - mean * mean, 0.0));
        r = ShiftReading{};
        if (contrast < 3.0) {
            r.degenerate = true;
            return true;
        }

        // Now "prev*" hold the current frame and the locals the previous one.
        double shift, peak, drift, driftPeak, ls, lp, rs, rp;
        if (!Correlate(rows, prevRows_, h / 4, shift, peak) ||
            !Correlate(cols, prevCols_, w / 4, drift, driftPeak)) {
            r.degenerate = true;
            return true;
        }
        double roll = 0;
        if (Correlate(left, prevLeft_, h / 4, ls, lp) &&
            Correlate(right, prevRight_, h / 4, rs, rp)) {
            roll = std::fabs(ls - rs);
        }

        double magnitude = std::fabs(shift);
        double peakScore = Ramp(peak, 0.55, 0.9);
        double allowance = std::max(magnitude * 0.6, 1.5);
        double pan = 1 - Ramp(std::fabs(drift), allowance, allowance * 3);
        double tilt = 1 - Ramp(roll, std::max(magnitude * 0.5, 1.2),
                               std::max(magnitude * 2.0, 5.0));
        double plausible = 1 - Ramp(magnitude, h * 0.2, h * 0.35);

        r.shift = shift;
        r.drift = drift;
        r.roll = roll;
        r.response = peak;
        r.confidence = Clamp01(peakScore * pan * tilt * plausible);
        return true;
    }

    void Reset() {
        prevRows_.clear();
        prevCols_.clear();
    }

   private:
    std::vector<double> prevRows_, prevLeft_, prevRight_, prevCols_;
};

// ---------------------------------------------------------------------------
// Estimator. A port of winduo/angle/estimator.py: shift to degrees, with
// neutral re-zeroed whenever the lid holds still, so drift never accumulates
// for longer than one lid movement.
// ---------------------------------------------------------------------------

class Estimator {
   public:
    AngleSample Feed(const ShiftReading& r, double now, const Settings& st) {
        BlendConfidence(r, now);
        if (!r.degenerate && r.confidence > 0) {
            total_ += r.shift;
        }
        if (!haveNeutral_) {
            neutral_ = total_;
            haveNeutral_ = true;
        }
        UpdateRest(now, st);
        double travel = (total_ - neutral_) * st.degreesPerPixel;
        double velocity = Velocity(travel, now);
        last_ = AngleSample{travel, velocity, confidence_, now, true};
        return last_;
    }

    AngleSample Dropped(double now) {
        confidence_ = std::max(0.0, confidence_ - 0.15);
        last_.velocity = 0;
        last_.confidence = confidence_;
        last_.timestamp = now;
        return last_;
    }

    void Reset() {
        total_ = 0;
        haveNeutral_ = false;
        history_.clear();
        window_.clear();
        confidence_ = 0;
        last_ = AngleSample{};
    }

   private:
    void BlendConfidence(const ShiftReading& r, double now) {
        double target = r.degenerate ? 0 : r.confidence;
        double lastTime = last_.valid ? last_.timestamp : now;
        double dt = Clamp(now - lastTime, 1e-3, 0.5);
        double rate = target > confidence_ ? 4.0 : 12.0;
        double step = rate * dt;
        double delta = target - confidence_;
        confidence_ = std::fabs(delta) <= step ? target
                                               : confidence_ + (delta > 0 ? step : -step);
    }

    void UpdateRest(double now, const Settings& st) {
        const double settle = 0.4, tolerance = 1.5;
        window_.push_back({now, total_});
        while (window_.size() > 2 && window_.front().first < now - settle) {
            window_.pop_front();
        }
        if (confidence_ < 0.2) {
            window_.clear();
            window_.push_back({now, total_});
            return;
        }
        if (window_.size() < 3 || now - window_.front().first < settle * 0.9) {
            return;
        }
        double lo = 1e18, hi = -1e18, sum = 0;
        for (auto& p : window_) {
            lo = std::min(lo, p.second);
            hi = std::max(hi, p.second);
            sum += p.second;
        }
        if (hi - lo > tolerance / st.degreesPerPixel) {
            return;
        }
        neutral_ = sum / window_.size();
    }

    double Velocity(double travel, double now) {
        history_.push_back({now, travel});
        if (history_.size() > 5) {
            history_.pop_front();
        }
        if (history_.size() < 3) {
            return 0;
        }
        double mt = 0, mv = 0;
        for (auto& p : history_) {
            mt += p.first;
            mv += p.second;
        }
        mt /= history_.size();
        mv /= history_.size();
        double num = 0, den = 0;
        for (auto& p : history_) {
            num += (p.first - mt) * (p.second - mv);
            den += (p.first - mt) * (p.first - mt);
        }
        return den < 1e-12 ? 0 : num / den;
    }

    double total_ = 0, neutral_ = 0, confidence_ = 0;
    bool haveNeutral_ = false;
    std::deque<std::pair<double, double>> history_, window_;
    AngleSample last_;
};

// ---------------------------------------------------------------------------
// Camera thread. Media Foundation source reader, converted to RGB32 by the
// reader, reduced to 320-wide grayscale, measured, and published.
// ---------------------------------------------------------------------------

// Machine state the camera and render threads both read. Written by the
// overlay window's message handler.
static std::atomic<bool> g_lidShut{false};         // one-shot: the lid just shut
static std::atomic<bool> g_displayChanged{false};  // one-shot: rebuild the overlay
static std::atomic<bool> g_lidOpen{true};
static std::atomic<bool> g_displayOn{true};
static std::atomic<bool> g_sessionLocked{false};
static std::atomic<bool> g_overlayReady{false};

static bool CameraWanted() {
    return g_overlayReady && g_lidOpen && g_displayOn && !g_sessionLocked;
}

static std::mutex g_sampleLock;
static AngleSample g_latestSample;
static std::atomic<bool> g_stopping{false};

static void Publish(const AngleSample& s) {
    std::lock_guard<std::mutex> lock(g_sampleLock);
    g_latestSample = s;
}

static AngleSample Latest(double now) {
    std::lock_guard<std::mutex> lock(g_sampleLock);
    AngleSample s = g_latestSample;
    if (!s.valid || now - s.timestamp > 0.4) {
        s.valid = false;
    }
    return s;
}

static IMFSourceReader* OpenCamera(int index, UINT32& width, UINT32& height,
                                   LONG& stride) {
    IMFAttributes* attrs = nullptr;
    IMFActivate** devices = nullptr;
    UINT32 count = 0;
    IMFMediaSource* source = nullptr;
    IMFSourceReader* reader = nullptr;
    IMFAttributes* readerAttrs = nullptr;
    IMFMediaType* type = nullptr;
    IMFMediaType* actual = nullptr;

    if (FAILED(MFCreateAttributes(&attrs, 1)) ||
        FAILED(attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID)) ||
        FAILED(MFEnumDeviceSources(attrs, &devices, &count)) || count == 0) {
        Wh_Log(L"no camera found");
        goto done;
    }
    if (index >= (int)count) {
        index = 0;
    }
    if (HRESULT hr = devices[index]->ActivateObject(IID_PPV_ARGS(&source)); FAILED(hr)) {
        if (hr == E_ACCESSDENIED) {
            Wh_Log(L"camera access is off: turn on Settings > Privacy > Camera > "
                   L"\"Let desktop apps access your camera\"");
        } else {
            Wh_Log(L"camera %d would not open (0x%08X); another app may be using it",
                   index, (unsigned)hr);
        }
        goto done;
    }
    if (FAILED(MFCreateAttributes(&readerAttrs, 1))) {
        goto done;
    }
    readerAttrs->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    if (FAILED(MFCreateSourceReaderFromMediaSource(source, readerAttrs, &reader))) {
        Wh_Log(L"camera reader failed");
        goto done;
    }
    if (FAILED(MFCreateMediaType(&type))) {
        SafeRelease(reader);
        goto done;
    }
    type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    MFSetAttributeSize(type, MF_MT_FRAME_SIZE, 640, 480);
    if (FAILED(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                           nullptr, type))) {
        type->DeleteItem(MF_MT_FRAME_SIZE);
        if (FAILED(reader->SetCurrentMediaType(
                MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, type))) {
            Wh_Log(L"camera will not deliver RGB32");
            SafeRelease(reader);
            goto done;
        }
    }
    if (SUCCEEDED(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                              &actual))) {
        MFGetAttributeSize(actual, MF_MT_FRAME_SIZE, &width, &height);
        UINT32 s = 0;
        if (SUCCEEDED(actual->GetUINT32(MF_MT_DEFAULT_STRIDE, &s))) {
            stride = (LONG)s;
        } else {
            stride = (LONG)width * 4;
        }
    }
    Wh_Log(L"camera %d open at %ux%u, stride %ld", index, width, height, stride);

done:
    SafeRelease(actual);
    SafeRelease(type);
    SafeRelease(readerAttrs);
    SafeRelease(source);
    if (devices) {
        for (UINT32 i = 0; i < count; i++) {
            devices[i]->Release();
        }
        CoTaskMemFree(devices);
    }
    SafeRelease(attrs);
    return reader;
}

static void CameraThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MFStartup(MF_VERSION);

    ShiftTracker tracker;
    Estimator estimator;
    std::vector<float> gray;
    int openedVersion = -1, openedIndex = -1;
    IMFSourceReader* reader = nullptr;
    UINT32 width = 0, height = 0;
    LONG stride = 0;

    // Seconds to wait before the next open attempt. Grows while the camera
    // keeps failing to open, which is usually another app holding it, so the
    // mod does not keep knocking on a camera a video call is using.
    double backoff = 0;
    double retryAt = 0;
    // Starts paused: the overlay, which reports lid and display state, is
    // still being built. Only later pauses are worth a log line.
    bool paused = true;
    bool started = false;

    while (!g_stopping) {
        Settings st = CurrentSettings();
        if (openedVersion != g_settingsVersion) {
            openedVersion = g_settingsVersion;
            if (st.cameraIndex != openedIndex) {
                SafeRelease(reader);
                openedIndex = -1;
                backoff = 0;
                retryAt = 0;
            }
        }

        // Nothing can be shown with the lid shut, the display off, the
        // session locked, or the built-in panel inactive, so the camera (and
        // its privacy light) goes off until that changes.
        if (!CameraWanted()) {
            if (!paused) {
                paused = true;
                SafeRelease(reader);
                estimator.Reset();
                Publish(AngleSample{});
                Wh_Log(L"camera off: lid shut, display off, locked, or panel inactive");
            }
            Sleep(200);
            continue;
        }
        if (paused) {
            paused = false;
            backoff = 0;
            retryAt = 0;
            if (started) {
                Wh_Log(L"camera back on");
            }
            started = true;
        }

        if (!reader) {
            if (Now() < retryAt) {
                Sleep(200);
                continue;
            }
            reader = OpenCamera(st.cameraIndex, width, height, stride);
            if (!reader || width == 0 || height == 0) {
                SafeRelease(reader);
                Publish(estimator.Dropped(Now()));
                backoff = backoff <= 0 ? 3 : std::min(backoff * 2, 60.0);
                retryAt = Now() + backoff;
                continue;
            }
            backoff = 0;
            openedIndex = st.cameraIndex;
            tracker.Reset();
            estimator.Reset();
        }

        DWORD flags = 0;
        LONGLONG ts = 0;
        IMFSample* sample = nullptr;
        HRESULT hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0,
                                        nullptr, &flags, &ts, &sample);
        double now = Now();
        if (FAILED(hr) || (flags & (MF_SOURCE_READERF_ERROR |
                                    MF_SOURCE_READERF_ENDOFSTREAM))) {
            Wh_Log(L"camera read failed (0x%08X); reopening", (unsigned)hr);
            SafeRelease(sample);
            SafeRelease(reader);
            Publish(estimator.Dropped(now));
            backoff = backoff <= 0 ? 3 : std::min(backoff * 2, 60.0);
            retryAt = now + backoff;
            continue;
        }
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) {
            // Width, height and stride all came from the old format.
            SafeRelease(sample);
            SafeRelease(reader);
            continue;
        }
        if (!sample) {
            Publish(estimator.Dropped(now));
            continue;
        }

        IMFMediaBuffer* buffer = nullptr;
        BYTE* data = nullptr;
        DWORD length = 0;
        if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buffer)) &&
            SUCCEEDED(buffer->Lock(&data, nullptr, &length))) {
            LONG absStride = std::labs(stride);
            if (absStride < (LONG)width * 4) {
                absStride = (LONG)width * 4;
            }
            if (length >= (DWORD)absStride * height) {
                int outW = kTrackWidth;
                int outH = std::max(8, int(double(height) * outW / width + 0.5));
                gray.assign(size_t(outW) * outH, 0.f);
                for (int y = 0; y < outH; y++) {
                    int y0 = int(double(y) * height / outH);
                    int y1 = std::max(y0 + 1, int(double(y + 1) * height / outH));
                    for (int x = 0; x < outW; x++) {
                        int x0 = int(double(x) * width / outW);
                        int x1 = std::max(x0 + 1, int(double(x + 1) * width / outW));
                        double acc = 0;
                        int n = 0;
                        for (int sy = y0; sy < y1; sy++) {
                            // A negative stride means the rows are stored
                            // bottom-up; reading them top-down here keeps the
                            // sign of the shift right.
                            int row = stride < 0 ? int(height) - 1 - sy : sy;
                            const BYTE* p = data + size_t(row) * absStride;
                            for (int sx = x0; sx < x1; sx++) {
                                const BYTE* px = p + sx * 4;
                                acc += 0.114 * px[0] + 0.587 * px[1] + 0.299 * px[2];
                                n++;
                            }
                        }
                        gray[size_t(y) * outW + x] = float(acc / n);
                    }
                }
                ShiftReading reading;
                if (tracker.Feed(gray, outW, outH, reading)) {
                    Publish(estimator.Feed(reading, now, st));
                }
            }
            buffer->Unlock();
        }
        SafeRelease(buffer);
        SafeRelease(sample);
    }

    SafeRelease(reader);
    MFShutdown();
    CoUninitialize();
}

// ---------------------------------------------------------------------------
// Renderer. One full-screen pixel shader over a padded, mip-mapped copy of the
// desktop. Blur is a mip level, not a convolution, so its cost does not grow
// with the radius. The overlay is a DirectComposition visual on a click-through
// topmost window that excludes itself from capture, so Desktop Duplication
// keeps seeing the real desktop underneath it.
// ---------------------------------------------------------------------------

static const char kShader[] = R"(
cbuffer Params : register(b0) {
    float4 uProfile[64];
    float2 uScreenSize; float uProfileEnd; float uProfileSlope;
    float2 uPaddedSize; float uPadding; float uMaxRadius;
    float uBlurStrength; float uBlurFloor; float uMaxLevel; float uMaxDim;
    float uDimStrength; float uDimFloor; float uDimReach; float uHingeGlow;
    float uReflection; float uTurn; float uOpacity; float uUnused;
};
Texture2D uPicture : register(t0);
SamplerState uSampler : register(s0);

float4 vs(uint id : SV_VertexID) : SV_Position {
    float2 c = float2(id == 2 ? 3.0 : -1.0, id == 0 ? -3.0 : 1.0);
    return float4(c, 0.0, 1.0);
}

float2 Entry(int i) {
    float4 v = uProfile[i >> 1];
    return (i & 1) ? v.zw : v.xy;
}

float3 LinearToSrgb(float3 c) {
    float3 lo = c * 12.92;
    float3 hi = 1.055 * pow(max(c, 0.0), 1.0 / 2.4) - 0.055;
    return lerp(lo, hi, step(0.0031308, c));
}

float4 ps(float4 pos : SV_Position) : SV_Target {
    // y up from the bottom, the convention the geometry uses.
    float2 p = float2(pos.x, uScreenSize.y - pos.y);

    float2 e;
    if (p.y >= uProfileEnd) {
        e = Entry(127);
        e.x += (p.y - uProfileEnd) * uProfileSlope;
    } else {
        float f = max(p.y, 0.0) / uProfileEnd * 127.0;
        int i = min(int(f), 126);
        e = lerp(Entry(i), Entry(i + 1), f - float(i));
    }
    float centre = uScreenSize.x * 0.5;
    float2 pic = float2(centre + (p.x - centre) / max(e.y, 1e-4), e.x * uScreenSize.y);

    float2 texel = float2(pic.x + uPadding, uScreenSize.y - pic.y + uPadding);
    float2 uv = texel / uPaddedSize;
    if (any(uv < 0.0) || any(uv > 1.0)) {
        return float4(0, 0, 0, uOpacity);
    }

    float2 unit = pic / uScreenSize;
    float height = saturate(unit.y);
    float onPicture = all(unit >= 0.0) && all(unit <= 1.0) ? 1.0 : 0.0;

    float blur = uBlurStrength * (uBlurFloor + (1.0 - uBlurFloor) * height);
    float level = clamp(log2(max(blur * uMaxRadius, 1.0)), 0.0, uMaxLevel);
    float3 colour = uPicture.SampleLevel(uSampler, uv, level).rgb;

    float spread = smoothstep(0.0, max(uDimReach, 0.02), height);
    float fade = uDimStrength * (uDimFloor + (1.0 - uDimFloor) * spread);
    colour *= pow(1.0 - uMaxDim * fade, 2.2);

    float hinge = exp(-pow(height / 0.05, 2.0)) * uTurn * onPicture;
    colour += float3(0.92, 0.95, 0.97) * hinge * uHingeGlow * 0.006;
    float band = exp(-pow((height - 0.65) / 0.28, 2.0)) * uTurn * onPicture;
    colour += float3(0.85, 0.88, 0.9) * band * uReflection * 0.004;

    colour = saturate(colour);
    // Premultiplied alpha, so the fade is one multiply.
    return float4(LinearToSrgb(colour) * uOpacity, uOpacity);
}
)";

struct alignas(16) ShaderParams {
    float profile[kProfileSamples][2];
    float screenSize[2];
    float profileEnd;
    float profileSlope;
    float paddedSize[2];
    float padding;
    float maxRadius;
    float blurStrength;
    float blurFloor;
    float maxLevel;
    float maxDim;
    float dimStrength;
    float dimFloor;
    float dimReach;
    float hingeGlow;
    float reflection;
    float turn;
    float opacity;
    float unused;
};
static_assert(sizeof(ShaderParams) % 16 == 0, "constant buffer alignment");

constexpr int kPadding = 120;
constexpr double kFadeIn = 0.07;
constexpr double kFadeOut = 0.22;
constexpr double kPrewarmLinger = 2.0;

// {BA3E0F4D-B817-4094-A2D1-D56379E6A0F3}
static const GUID kLidSwitchGuid = {
    0xba3e0f4d, 0xb817, 0x4094, {0xa2, 0xd1, 0xd5, 0x63, 0x79, 0xe6, 0xa0, 0xf3}};
// GUID_CONSOLE_DISPLAY_STATE {6FE69556-704A-47A0-8F24-C28D936FDA47}
static const GUID kDisplayStateGuid = {
    0x6fe69556, 0x704a, 0x47a0, {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

// The GDI device name (\\.\DISPLAYn) of the active built-in panel, if any.
static bool FindInternalPanel(wchar_t (&name)[32]) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) !=
        ERROR_SUCCESS) {
        return false;
    }
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount,
                           modes.data(), nullptr) != ERROR_SUCCESS) {
        return false;
    }
    for (UINT32 i = 0; i < pathCount; i++) {
        auto tech = paths[i].targetInfo.outputTechnology;
        bool internal = tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                        tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                        tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED;
        if (!internal) {
            continue;
        }
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = paths[i].sourceInfo.adapterId;
        source.header.id = paths[i].sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS) {
            wcsncpy_s(name, source.viewGdiDeviceName, _TRUNCATE);
            return true;
        }
    }
    return false;
}

class Overlay {
   public:
    bool Init() {
        IDXGIFactory1* factory = nullptr;
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
            return false;
        }
        // The lid belongs to the built-in panel, which is not necessarily the
        // primary display: a docked laptop usually has an external primary.
        // So the effect goes on the internal panel, and on nothing if that
        // panel is off (lid-closed docking, "second screen only").
        wchar_t panel[32] = {};
        if (!FindInternalPanel(panel)) {
            SafeRelease(factory);
            Wh_Log(L"the built-in display is not active; the effect is idle");
            return false;
        }
        IDXGIAdapter1* adapter = nullptr;
        IDXGIOutput* output = nullptr;
        // The adapter that drives that panel, so duplication and rendering
        // share one device.
        for (UINT a = 0; !output && factory->EnumAdapters1(a, &adapter) == S_OK; a++) {
            for (UINT o = 0; adapter->EnumOutputs(o, &output) == S_OK; o++) {
                DXGI_OUTPUT_DESC desc;
                output->GetDesc(&desc);
                if (wcscmp(desc.DeviceName, panel) == 0) {
                    break;
                }
                SafeRelease(output);
            }
            if (!output) {
                SafeRelease(adapter);
            }
        }
        SafeRelease(factory);
        if (!output) {
            Wh_Log(L"no DXGI output for %s", panel);
            return false;
        }
        output->QueryInterface(IID_PPV_ARGS(&output1_));
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        SafeRelease(output);
        rect_ = desc.DesktopCoordinates;
        width_ = rect_.right - rect_.left;
        height_ = rect_.bottom - rect_.top;

        D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1};
        HRESULT hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                                       D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 2,
                                       D3D11_SDK_VERSION, &device_, nullptr, &context_);
        SafeRelease(adapter);
        if (FAILED(hr) || !output1_) {
            Wh_Log(L"D3D11 device failed (0x%08X)", (unsigned)hr);
            return false;
        }
        return CreateWindowAndChain() && CreateShaders() && CreatePicture();
    }

    void Destroy() {
        StopCapture();
        SafeRelease(picture_);
        SafeRelease(pictureSrv_);
        SafeRelease(pictureRtv_);
        SafeRelease(targetRtv_);
        SafeRelease(cbuffer_);
        SafeRelease(sampler_);
        SafeRelease(vs_);
        SafeRelease(ps_);
        SafeRelease(visual_);
        SafeRelease(target_);
        SafeRelease(dcomp_);
        SafeRelease(swapchain_);
        SafeRelease(context_);
        SafeRelease(device_);
        SafeRelease(output1_);
        if (lidNotify_) {
            UnregisterPowerSettingNotification(lidNotify_);
            lidNotify_ = nullptr;
        }
        if (displayNotify_) {
            UnregisterPowerSettingNotification(displayNotify_);
            displayNotify_ = nullptr;
        }
        if (sessionNotify_ && hwnd_) {
            WTSUnRegisterSessionNotification(hwnd_);
            sessionNotify_ = false;
        }
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        UnregisterClass(kClassName, GetModuleHandle(nullptr));
    }

    HWND Window() const { return hwnd_; }
    double Width() const { return width_; }
    double Height() const { return height_; }
    bool HasPicture() const { return hasPicture_; }

    bool StartCapture(double now) {
        if (duplication_) {
            return true;
        }
        // A failure (a UAC prompt on the secure desktop, for one) tends to
        // last a while; retrying every frame would only fill the log.
        if (now < captureRetryAt_) {
            return false;
        }
        HRESULT hr = output1_->DuplicateOutput(device_, &duplication_);
        if (FAILED(hr)) {
            Wh_Log(L"DuplicateOutput failed (0x%08X)", (unsigned)hr);
            captureRetryAt_ = now + 1.0;
            return false;
        }
        return true;
    }

    // Shows or hides our visual in the composition tree.
    void Attach(bool attached) {
        if (!target_ || attached == attached_) {
            return;
        }
        target_->SetRoot(attached ? visual_ : nullptr);
        dcomp_->Commit();
        attached_ = attached;
    }

    void StopCapture() { SafeRelease(duplication_); }

    // Copies the newest desktop frame into the picture, if there is one.
    void Grab() {
        if (!duplication_) {
            return;
        }
        DXGI_OUTDUPL_FRAME_INFO info;
        IDXGIResource* resource = nullptr;
        HRESULT hr = duplication_->AcquireNextFrame(0, &info, &resource);
        if (hr == DXGI_ERROR_ACCESS_LOST) {
            StopCapture();
            StartCapture(Now());
            return;
        }
        if (FAILED(hr)) {
            return;
        }
        ID3D11Texture2D* frame = nullptr;
        if (info.LastPresentTime.QuadPart != 0 &&
            SUCCEEDED(resource->QueryInterface(IID_PPV_ARGS(&frame)))) {
            D3D11_BOX box{0, 0, 0, UINT(width_), UINT(height_), 1};
            context_->CopySubresourceRegion(picture_, 0, kPadding, kPadding, 0,
                                            frame, 0, &box);
            context_->GenerateMips(pictureSrv_);
            hasPicture_ = true;
        }
        SafeRelease(frame);
        SafeRelease(resource);
        duplication_->ReleaseFrame();
    }

    void ForgetPicture() { hasPicture_ = false; }

    // False when the device was lost and the overlay has to be rebuilt.
    bool Draw(const ShaderParams& params) {
        if (!targetRtv_) {
            return true;
        }
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (cbuffer_ && SUCCEEDED(context_->Map(cbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            std::memcpy(mapped.pData, &params, sizeof(params));
            context_->Unmap(cbuffer_, 0);
        }
        const float clear[4] = {0, 0, 0, 0};
        context_->OMSetRenderTargets(1, &targetRtv_, nullptr);
        context_->ClearRenderTargetView(targetRtv_, clear);
        if (params.opacity > 0 && hasPicture_ && ps_ && cbuffer_) {
            D3D11_VIEWPORT vp{0, 0, float(width_), float(height_), 0, 1};
            context_->RSSetViewports(1, &vp);
            context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context_->IASetInputLayout(nullptr);
            context_->VSSetShader(vs_, nullptr, 0);
            context_->PSSetShader(ps_, nullptr, 0);
            context_->PSSetConstantBuffers(0, 1, &cbuffer_);
            context_->PSSetShaderResources(0, 1, &pictureSrv_);
            context_->PSSetSamplers(0, 1, &sampler_);
            context_->Draw(3, 0);
            ID3D11ShaderResourceView* none = nullptr;
            context_->PSSetShaderResources(0, 1, &none);
        }
        HRESULT hr = swapchain_->Present(1, 0);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            Wh_Log(L"graphics device lost (0x%08X); rebuilding", (unsigned)hr);
            g_displayChanged = true;
            return false;
        }
        return true;
    }

    void Raise() {
        SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    float MaxLevel() const { return maxLevel_; }
    float PaddedW() const { return float(width_ + 2 * kPadding); }
    float PaddedH() const { return float(height_ + 2 * kPadding); }

   private:
    static constexpr const wchar_t* kClassName = L"WinDuoWindhawkOverlay";

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    bool CreateWindowAndChain() {
        WNDCLASS wc{};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = kClassName;
        RegisterClass(&wc);
        // One pixel taller than the display. A window exactly the size of the
        // monitor can be promoted to an independent flip, which bypasses the
        // composition that capture exclusion depends on, and duplication then
        // feeds the overlay back into itself.
        hwnd_ = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                   WS_EX_TRANSPARENT | WS_EX_LAYERED |
                                   WS_EX_NOREDIRECTIONBITMAP,
                               kClassName, L"WinDuo", WS_POPUP, rect_.left, rect_.top,
                               width_, height_ + 1, nullptr, nullptr,
                               GetModuleHandle(nullptr), nullptr);
        if (!hwnd_) {
            return false;
        }
        SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);
        if (!SetWindowDisplayAffinity(hwnd_, WDA_EXCLUDEFROMCAPTURE)) {
            Wh_Log(L"capture exclusion unavailable (%lu); holding a single frame",
                   GetLastError());
            excluded_ = false;
        }
        // Each registration delivers the current state straight away, so the
        // camera starts correctly paused if the display is already off.
        lidNotify_ = RegisterPowerSettingNotification(hwnd_, &kLidSwitchGuid,
                                                      DEVICE_NOTIFY_WINDOW_HANDLE);
        displayNotify_ = RegisterPowerSettingNotification(hwnd_, &kDisplayStateGuid,
                                                          DEVICE_NOTIFY_WINDOW_HANDLE);
        sessionNotify_ = WTSRegisterSessionNotification(hwnd_, NOTIFY_FOR_THIS_SESSION);

        IDXGIDevice* dxgiDevice = nullptr;
        IDXGIAdapter* adapter = nullptr;
        IDXGIFactory2* factory = nullptr;
        if (FAILED(device_->QueryInterface(IID_PPV_ARGS(&dxgiDevice))) ||
            FAILED(dxgiDevice->GetAdapter(&adapter)) ||
            FAILED(adapter->GetParent(IID_PPV_ARGS(&factory)))) {
            SafeRelease(adapter);
            SafeRelease(dxgiDevice);
            Wh_Log(L"could not reach the DXGI factory");
            return false;
        }
        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width = width_;
        sd.Height = height_;
        sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 2;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        HRESULT hr = factory ? factory->CreateSwapChainForComposition(
                                   device_, &sd, nullptr, &swapchain_)
                             : E_FAIL;
        if (SUCCEEDED(hr)) {
            hr = DCompositionCreateDevice(dxgiDevice, IID_PPV_ARGS(&dcomp_));
        }
        if (SUCCEEDED(hr)) {
            hr = dcomp_->CreateTargetForHwnd(hwnd_, TRUE, &target_);
        }
        if (SUCCEEDED(hr)) {
            hr = dcomp_->CreateVisual(&visual_);
        }
        if (SUCCEEDED(hr)) {
            visual_->SetContent(swapchain_);
            target_->SetRoot(visual_);
            hr = dcomp_->Commit();
        }
        SafeRelease(factory);
        SafeRelease(adapter);
        SafeRelease(dxgiDevice);
        if (FAILED(hr)) {
            Wh_Log(L"composition setup failed (0x%08X)", (unsigned)hr);
            return false;
        }
        ID3D11Texture2D* back = nullptr;
        if (FAILED(swapchain_->GetBuffer(0, IID_PPV_ARGS(&back))) ||
            FAILED(device_->CreateRenderTargetView(back, nullptr, &targetRtv_))) {
            SafeRelease(back);
            Wh_Log(L"could not create the overlay's render target");
            return false;
        }
        SafeRelease(back);

        // Shown once, fully transparent, and never hidden. Showing a topmost
        // full-screen window invalidates duplication and costs a stall, which
        // is better paid now than while the lid is moving.
        ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
        ShaderParams blank{};
        Draw(blank);
        Attach(false);
        return true;
    }

    bool CreateShaders() {
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;
        ID3DBlob* errors = nullptr;
        HRESULT hr = D3DCompile(kShader, sizeof(kShader) - 1, "winduo", nullptr,
                                nullptr, "vs", "vs_4_0", 0, 0, &vsBlob, &errors);
        if (SUCCEEDED(hr)) {
            hr = D3DCompile(kShader, sizeof(kShader) - 1, "winduo", nullptr, nullptr,
                            "ps", "ps_4_0", 0, 0, &psBlob, &errors);
        }
        if (FAILED(hr)) {
            if (errors) {
                Wh_Log(L"shader: %S", (const char*)errors->GetBufferPointer());
            }
            SafeRelease(errors);
            SafeRelease(vsBlob);
            return false;
        }
        SafeRelease(errors);
        device_->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                    nullptr, &vs_);
        device_->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                   nullptr, &ps_);
        SafeRelease(vsBlob);
        SafeRelease(psBlob);

        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(ShaderParams);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        device_->CreateBuffer(&bd, nullptr, &cbuffer_);

        D3D11_SAMPLER_DESC sd{};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        device_->CreateSamplerState(&sd, &sampler_);
        return vs_ && ps_ && cbuffer_ && sampler_;
    }

    bool CreatePicture() {
        D3D11_TEXTURE2D_DESC td{};
        td.Width = width_ + 2 * kPadding;
        td.Height = height_ + 2 * kPadding;
        td.MipLevels = 0;
        td.ArraySize = 1;
        // Typeless, so duplication's UNORM frames can be copied in while the
        // shader reads the same bytes as sRGB and blurs in linear light.
        td.Format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        td.SampleDesc.Count = 1;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        if (FAILED(device_->CreateTexture2D(&td, nullptr, &picture_))) {
            return false;
        }
        D3D11_SHADER_RESOURCE_VIEW_DESC sv{};
        sv.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        sv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        sv.Texture2D.MipLevels = UINT(-1);
        D3D11_RENDER_TARGET_VIEW_DESC rv{};
        rv.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        rv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        if (FAILED(device_->CreateShaderResourceView(picture_, &sv, &pictureSrv_)) ||
            FAILED(device_->CreateRenderTargetView(picture_, &rv, &pictureRtv_))) {
            return false;
        }
        // The margin is black once and stays black; only the interior is
        // overwritten per frame.
        const float black[4] = {0, 0, 0, 1};
        context_->ClearRenderTargetView(pictureRtv_, black);
        maxLevel_ = std::floor(std::log2(double(std::max(td.Width, td.Height))));
        return true;
    }

    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    IDXGIOutput1* output1_ = nullptr;
    IDXGIOutputDuplication* duplication_ = nullptr;
    IDXGISwapChain1* swapchain_ = nullptr;
    IDCompositionDevice* dcomp_ = nullptr;
    IDCompositionTarget* target_ = nullptr;
    IDCompositionVisual* visual_ = nullptr;
    ID3D11RenderTargetView* targetRtv_ = nullptr;
    ID3D11Texture2D* picture_ = nullptr;
    ID3D11ShaderResourceView* pictureSrv_ = nullptr;
    ID3D11RenderTargetView* pictureRtv_ = nullptr;
    ID3D11Buffer* cbuffer_ = nullptr;
    ID3D11SamplerState* sampler_ = nullptr;
    ID3D11VertexShader* vs_ = nullptr;
    ID3D11PixelShader* ps_ = nullptr;
    HPOWERNOTIFY lidNotify_ = nullptr;
    HPOWERNOTIFY displayNotify_ = nullptr;
    bool sessionNotify_ = false;
    HWND hwnd_ = nullptr;
    RECT rect_{};
    int width_ = 0, height_ = 0;
    float maxLevel_ = 0;
    bool hasPicture_ = false;
    bool excluded_ = true;
    bool attached_ = true;
    double captureRetryAt_ = 0;

   public:
    bool Excluded() const { return excluded_; }
};

LRESULT CALLBACK Overlay::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_POWERBROADCAST && wp == PBT_POWERSETTINGCHANGE) {
        auto* setting = reinterpret_cast<POWERBROADCAST_SETTING*>(lp);
        if (setting && setting->DataLength >= 1) {
            if (IsEqualGUID(setting->PowerSetting, kLidSwitchGuid)) {
                bool open = setting->Data[0] != 0;
                g_lidOpen = open;
                if (!open) {
                    g_lidShut = true;
                }
            } else if (IsEqualGUID(setting->PowerSetting, kDisplayStateGuid)) {
                g_displayOn = setting->Data[0] != 0;  // 0 off, 1 on, 2 dimmed
            }
        }
        return TRUE;
    }
    if (msg == WM_WTSSESSION_CHANGE) {
        if (wp == WTS_SESSION_LOCK) {
            g_sessionLocked = true;
        } else if (wp == WTS_SESSION_UNLOCK) {
            g_sessionLocked = false;
        }
        return 0;
    }
    if (msg == WM_DISPLAYCHANGE) {
        g_displayChanged = true;  // the render loop rebuilds the overlay
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// The loop. One thread owns the window, the device and the controller, and
// re-decides everything from the newest camera sample every tick, so a stalled
// camera can never leave the effect stuck on screen.
// ---------------------------------------------------------------------------

// A scripted close, so the effect can be judged without moving the lid.
struct PreviewSweep {
    double startedAt = -1;
    double deep = 0, closing = 1.4, hold = 1.0, opening = 0.6;

    bool Active() const { return startedAt >= 0; }

    void Start(const Settings& st, double now) {
        startedAt = now;
        deep = st.triggerTravel + st.fullEffectTravel * 1.15;
    }

    bool Sample(double now, AngleSample& out) {
        double t = now - startedAt;
        double travel;
        if (t < closing) {
            travel = deep * t / closing;
        } else if (t < closing + hold) {
            travel = deep;
        } else if (t < closing + hold + opening) {
            travel = deep * (1 - (t - closing - hold) / opening);
        } else {
            startedAt = -1;
            return false;
        }
        out = AngleSample{travel, deep / closing, 1.0, now, true};
        return true;
    }
};

static void FillParams(const Settings& st, const Frame& f, const Profile& profile,
                       const Overlay& overlay, ShaderParams& p) {
    std::memcpy(p.profile, profile.table, sizeof(p.profile));
    p.screenSize[0] = float(overlay.Width());
    p.screenSize[1] = float(overlay.Height());
    p.profileEnd = profile.end;
    p.profileSlope = profile.slope;
    p.paddedSize[0] = overlay.PaddedW();
    p.paddedSize[1] = overlay.PaddedH();
    p.padding = float(kPadding);
    p.maxRadius = float(st.maxBlurRadius);
    double boost = MotionBoost(f.velocity) * f.progress;
    p.blurStrength = float(std::min(BlurCurve(f.progress) + boost, 1.0));
    p.blurFloor = 0;
    p.maxLevel = overlay.MaxLevel();
    p.maxDim = float(st.maxDim);
    p.dimStrength = float(DimCurve(f.progress));
    p.dimFloor = float(kDimHingeFloor);
    p.dimReach = float(st.dimReach);
    p.hingeGlow = float(st.hingeGlow);
    p.reflection = float(st.reflection);
    p.turn = float(TurnCurve(f.progress));
}

static void RenderThread() {
    UsePhysicalPixels();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool loggedFailure = false;
    while (!g_stopping) {
        Overlay overlay;
        if (!overlay.Init()) {
            // Usually the built-in panel being off (docked with the lid shut).
            // Checked again every few seconds, quietly after the first time.
            if (!loggedFailure) {
                Wh_Log(L"the overlay could not start; checking again every 5 s");
                loggedFailure = true;
            }
            overlay.Destroy();
            for (int i = 0; i < 50 && !g_stopping; i++) {
                Sleep(100);
            }
            continue;
        }
        loggedFailure = false;
        Wh_Log(L"overlay ready at %.0fx%.0f", overlay.Width(), overlay.Height());
        g_displayChanged = false;
        g_overlayReady = true;

        Controller controller;
        PreviewSweep preview;
        ShaderParams params{};
        bool haveParams = false;
        bool shown = false;
        double opacity = 0, fadeFrom = 0, fadeTarget = 0, fadeStart = 0, fadeLength = 1;
        double prewarmUntil = 0;

        auto fadeTo = [&](double target, double length, double now) {
            fadeFrom = opacity;
            fadeTarget = target;
            fadeStart = now;
            fadeLength = length;
        };

        while (!g_stopping && !g_displayChanged) {
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            double now = Now();
            Settings st = CurrentSettings();

            // Left pending while an effect runs, rather than dropped.
            if (!controller.Active() && g_previewRequested.exchange(false)) {
                overlay.StartCapture(now);
                prewarmUntil = now + 8;
                preview.Start(st, now);
            }
            if (g_lidShut.exchange(false)) {
                controller.LidShut();
                preview.startedAt = -1;
                // The controller goes straight to idle with no final frame, so
                // nothing else would ever fade this out. Hide it on this tick.
                if (shown) {
                    fadeTo(0, 0, now);
                }
            }

            AngleSample sample;
            if (preview.Active() && preview.Sample(now, sample)) {
                controller.Observe(sample, now);
            } else {
                controller.Observe(Latest(now), now);
            }

            // Start the capture as soon as the lid is heading down at all:
            // duplication takes long enough to start that doing it at the
            // trigger would stall the frame loop through the movement.
            if (controller.Travel() > st.triggerTravel * 0.35 || controller.Active()) {
                prewarmUntil = now + kPrewarmLinger;
            }
            if (now < prewarmUntil) {
                overlay.StartCapture(now);
            } else if (!shown) {
                overlay.StopCapture();
            }

            Frame frame;
            bool drawing = controller.Step(st, now, frame);
            if (drawing) {
                if (!shown) {
                    shown = true;
                    overlay.ForgetPicture();
                    overlay.Attach(true);
                    overlay.Raise();
                    opacity = 0;
                    fadeTo(0, 0, now);
                }
                bool live = st.livePicture && overlay.Excluded();
                if (live || !overlay.HasPicture()) {
                    overlay.Grab();
                }
                Profile profile;
                if (BuildProfile(frame.startAngle, frame.currentAngle,
                                 st.viewingDistance, st.topLean, overlay.Width(),
                                 overlay.Height(), profile)) {
                    FillParams(st, frame, profile, overlay, params);
                    haveParams = true;
                }
                // Reveal only once there is a picture, or the fade would flash
                // a transparent full-screen window over the desktop.
                if (overlay.HasPicture() && fadeTarget < 1 && !frame.isFinal) {
                    fadeTo(1, kFadeIn, now);
                }
                if (frame.isFinal) {
                    fadeTo(0, kFadeOut, now);
                    prewarmUntil = now + kPrewarmLinger;
                }
            }

            if (shown) {
                double t = fadeLength <= 0 ? 1 : Clamp01((now - fadeStart) / fadeLength);
                opacity = fadeFrom + (fadeTarget - fadeFrom) * t;
                if (haveParams) {
                    params.opacity = float(opacity);
                    // Present(1) paces this loop at the refresh rate.
                    if (!overlay.Draw(params)) {
                        break;  // device lost; rebuild
                    }
                }
                if (!controller.Active() && fadeTarget == 0 && t >= 1) {
                    shown = false;
                    haveParams = false;
                    params.opacity = 0;
                    overlay.Draw(params);
                    overlay.ForgetPicture();
                    // Detached while idle, so the compositor has nothing of
                    // ours to blend over full-screen video or games.
                    overlay.Attach(false);
                }
                continue;
            }

            bool moving = std::fabs(controller.Velocity()) > 1 || controller.Travel() > 1 ||
                          preview.Active();
            MsgWaitForMultipleObjects(0, nullptr, FALSE, moving ? 16 : 125, QS_ALLINPUT);
        }

        g_overlayReady = false;
        overlay.Destroy();
    }

    CoUninitialize();
}

// ---------------------------------------------------------------------------
// Tool mod entry points.
// ---------------------------------------------------------------------------

// Optional and never destroyed, so an exit that skips WhTool_ModUninit does not
// run ~thread() on a joinable thread and terminate the process.
[[clang::no_destroy]] static std::optional<std::thread> g_cameraThread;
[[clang::no_destroy]] static std::optional<std::thread> g_renderThread;

BOOL WhTool_ModInit() {
    LoadSettings(false);
    g_stopping = false;
    g_cameraThread.emplace(CameraThread);
    g_renderThread.emplace(RenderThread);
    return TRUE;
}

void WhTool_ModSettingsChanged() { LoadSettings(true); }

void WhTool_ModUninit() {
    g_stopping = true;
    for (auto* thread : {&g_renderThread, &g_cameraThread}) {
        if (*thread && (*thread)->joinable()) {
            (*thread)->join();
        }
        thread->reset();
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

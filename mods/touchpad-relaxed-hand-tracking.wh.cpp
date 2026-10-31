// ==WindhawkMod==
// @id              touchpad-relaxed-hand-tracking
// @name            Touchpad Relaxed Hand Tracking
// @description     Enables 5-2 fingers mouse pointer tracking for improved hand ergonomics (excludes admin windows, like Windhawk)
// @version         1.0.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         windhawk.exe
// @compilerOptions -lhid -lwtsapi32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Touchpad Relaxed Hand Tracking

Move the pointer with more fingers resting on the precision touchpad, as if gliding a weightless mouse. Your hand can relax with more natural finger movements, instead of holding a single digit position over the pad.

Hint: glide with as much fingers you wish, but use the thumb to click. It is further apart thus clearly detected.

## How it works

- **Trigger:** with 5 to as much contacts configured on the touchpad.
- **Disarm:** after all fingers leave the touchpad. Lifting some keeps the glide going.
- **Movement:** the pointer follows the finger which moves the most, from one to five resting. With fewer fingers movement or left touching, the lower multiplier applies; with more moving together, the higher one.
- **Click:** you don't need to lift all fingers. With the hand resting, same as native - single finger (double) tap to (double) click, tap and hold to drag, tap-tap and hold to select word by word. A hold lasts until the whole hand leaves the pad. **Tap to click** option controls its sensitivity.
- **Scrolling:** Windows 2-finger scroll is ignored while more-fingers-gliding. **Both will get activated if you start with 2 and rest more on the touchpad.**

## 4 and 3 fingers detection

The options to start with four or three fingers conflict with the respective Windows swipes. **Disable them in Settings → Bluetooth & devices → Touchpad → Four-finger gestures (or Three) → Swipes: Nothing**. Otherwise Windows simultaneously triggers the swipe. Four and three-finger taps can keep their actions. If the Three Finger Drag mod is installed and set to the same finger count, both mods respond to the same swipe, so give them different counts.

## 2 fingers detection

When active, two fingers do everything: glide, right click (tap) and scroll (tap-drag). Windows' own two-finger gestures have to be disabled first, in **Settings → Bluetooth & devices → Touchpad**: uncheck **Scroll & zoom → Drag two fingers to scroll** and **Taps → Tap with two fingers to right-click**. Otherwise both commands respond. With two-finger detection on, programs running as administrator get no two-finger scroll at all, since Windows' will be disabled and the mod can't reach them.

## Speed

The mod learns the touchpad's native speed and acceleration by watching ordinary one-finger use: for each range of finger speed, it compares how far the finger moved with how far Windows moved the pointer. The glide reproduces that curve, and the multipliers apply on top of it: **1.0 is the same as native**, 1.5 is 50% faster. When the touchpad speed is changed in the Windows settings, the curve adjusts with use.

Right after installing, before anything is learned, the glide uses a default speed. Using the touchpad normally for a few minutes, with slow and fast movements, is enough for it to learn.

## Limitations

- Requires a precision touchpad.
- Windows gestures can't be blocked by the mod. Windows picks the gesture by the fingers that came down last: starting with one finger and then resting four more, or lifting and resting fingers again, triggers the gesture for that count. Disabling the three and four finger swipes removes the conflict; **two-finger scroll is ignored while gliding.**
- Scrolling is only blocked in programs which receive it as mouse wheel input, not in those which scroll through the Windows precision touchpad path.
- **While a program running as administrator is in the foreground, Windows neither delivers the touchpad nor accepts pointer movement from an ordinary process, and the glide stops.**
- Movements at the edge of the screen don't teach the curve, since the pointer stops there while the finger goes on.
- The two-finger scroll reaches programs as mouse wheel input. Excel gets the native touchpad scroll through its own precision channel, which the mod can't use, and scrolls poorly with the wheel over its cells: keep the Windows two-finger scroll if you work a lot in Excel.

## Credits

Reading the touchpad follows the approach of the [Three Finger Drag](https://windhawk.net/mods/three-finger-drag) mod by RodrigoThomazi, under the GPL v3.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- tapToClick: normal
  $name: Tap to click
  $description: Tapping with one finger while other rest. Strict ignores more accidental taps but needs quicker, steadier taps; Lenient is the opposite.
  $options:
  - "off": "Off"
  - strict: Strict
  - normal: Normal
  - lenient: Lenient
- multiplierFive: "2.0"
  $name: 5-3 fingers speed multiplier
  $description: When more fingers are gliding. 1.0 is the system's touchpad speed and acceleration, 1.5 is 50% faster. Defaults to 2.0 since the resting hand has less room to travel.
- multiplierOne: "1.0"
  $name: 2-1 fingers speed multiplier
  $description: When fewer fingers move or are left gliding. 1.0 is native.
- armFour: false
  $name: 4 fingers detection
  $description: ! Disable the four-finger swipes in Windows touchpad settings, otherwise it also responds.
- armThree: false
  $name: 3 fingers detection
  $description: Disable the three-finger swipes in Windows touchpad settings, otherwise it also responds.
- armTwo: false
  $name: 2 fingers detection
  $description: Disable "Drag two fingers to scroll" and "Tap with two fingers to right-click" in Windows touchpad settings. TAP to RIGHT CLICK, TAP-DRAG to SCROLL. You can stay in the scroll operation after tap-dragging and keeping another finger (thumb) down while other 2 scroll.
- scrollReverse: false
  $name: Reverse scroll direction
  $description: For the two-finger scroll. Off, page follows the fingers, like Windows default.
*/
// ==/WindhawkModSettings==

// The touchpad is read as raw HID input (Raw Input with RIDEV_INPUTSINK), in a dedicated Windhawk process
// (tool mod block at the end). The pointer is placed with SetCursorPos by the learned native curve, so that
// it doesn't go through the mouse acceleration curve.

#include <hidusage.h>
#include <hidpi.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>

constexpr USAGE kPageGeneric = 0x01;
constexpr USAGE kUsageX = 0x30;
constexpr USAGE kUsageY = 0x31;
constexpr USAGE kPageDigitizer = 0x0D;
constexpr USAGE kUsageTouchPad = 0x05;
constexpr USAGE kUsageTipSwitch = 0x42;
constexpr USAGE kUsageConfidence = 0x47;
constexpr USAGE kUsageContactId = 0x51;
constexpr USAGE kUsageContactCount = 0x54;

constexpr int kFingers = 5;
// The second most moved finger under the first share of the most moved one: a single finger is moving; over
// the second: the hand is. In between, the last decision stays, so that the multiplier doesn't flap.
constexpr double kSingleFingerShare = 0.33;
constexpr double kHandShare = 0.5;
// Under this, per frame, the most moved finger is resting jitter: the average is followed, which cancels it.
constexpr double kJitterMm = 0.1;
// A single finger counts as moving alone only past this, per frame: the uneven tails of a stopping hand stay
// under it, and following the most moved of them made the pointer slide on after the hand stopped.
constexpr double kSingleStartMm = 0.3;
// After a stop the touchpad keeps reporting the resting fingers creeping at 2–4 mm/s for a few hundred ms,
// which Windows doesn't turn into pointer movement and the glide did: the pointer slid on. Under the first
// speed the pointer stops, and it only starts again over the second.
constexpr double kStopSpeed = 4.0;
constexpr double kStartSpeed = 7.0;
// More than this in one frame isn't a finger moving: the touchpad gave a contact's id to another contact,
// typically when the thumb lands late, and following it would throw the pointer to the thumb.
constexpr double kJumpMm = 10.0;

struct TapProfile {
    bool on;
    DWORD maxMs;      // longer contacts are a finger resting, not a tap
    double maxMm;     // contacts which moved more are a finger shifting, not a tap
    DWORD minMs;      // shorter contacts are a finger flickering, not a tap
    DWORD settleMs;   // fingers landing this soon after the start are the hand settling
    DWORD flickerMs;  // a contact appearing this soon after another vanished is the same finger
};

// Fingers landing this close together are the hand coming down, not a one-finger tap.
constexpr DWORD kTogetherMs = 100;
// Tap and hold: a finger landing within the double-click time and this close to where the last tap was holds
// the primary button once it can't be a tap anymore, until the whole hand leaves.
constexpr double kHoldNearMm = 12.0;
// The first tap of a hold is judged looser than a click: a finger which came down after the start and lifted
// within these, even if it didn't click, starts a hold when it comes back. A click needs the stricter profile, a
// tap-and-hold only needs its return.
constexpr DWORD kHoldTapMaxMs = 400;
// Right after a click, the hold presses just outside the double-click rectangle around it: pressed at the
// click's spot within the double-click time, Windows took it for the second click of a double click, and a file
// opened instead of moving. Waiting out the double-click time instead held the drag back noticeably.

// Tap, tap and hold: the second click of a double click waits up to this long for the finger to come back. If it
// does, the button goes down there and stays, a held double click, which selects text word by word like
// Windows' own tap-tap-drag; if not, the double click completes as usual.
constexpr DWORD kDoubleWaitMs = 200;
constexpr double kHoldTapMaxMm = 4.0;

// Two-finger tap: both fingers down together, up within kTwoTapMaxMs, moving at most kTwoTapMaxMm. A right click
// follows unless the fingers come back within kTwoTapWaitMs, which starts a scroll instead.
constexpr DWORD kTwoTapMaxMs = 250;
constexpr double kTwoTapMaxMm = 3.0;
constexpr DWORD kTwoTapWaitMs = 300;
// Wheel units per finger mm while scrolling: one notch (120) every 6 mm.
constexpr double kScrollUnitsPerMm = 20.0;
// Marks the mod's own wheel events, which the wheel block lets through.
constexpr ULONG_PTR kScrollMark = 0x52484754;
// After the fingers lift from a scroll moving faster than kInertiaMinSpeed (wheel units/s), it goes on and slows
// down with time constant kInertiaTau (s), like the Windows touchpad. Any touch stops it.
constexpr double kInertiaMinSpeed = 600.0;
constexpr double kInertiaStopSpeed = 120.0;
constexpr double kInertiaTau = 1.0;
constexpr UINT kInertiaStepMs = 16;

constexpr TapProfile kTapProfiles[] = {
    {false, 0, 0, 0, 0, 0},          // off
    {true, 180, 1.5, 50, 500, 120},  // strict
    {true, 250, 2.0, 30, 300, 80},   // normal
    {true, 350, 3.0, 20, 200, 50},   // lenient
};

constexpr WCHAR kWindowClassName[] = L"Touchpad_Relaxed_Hand";

// Native curve: pointer px per finger mm, by finger speed range (mm/s, range centers).
constexpr double kSpeedBins[] = {4, 8, 16, 32, 64, 128, 256, 512};
constexpr int kBinCount = sizeof(kSpeedBins) / sizeof(kSpeedBins[0]);
constexpr int kBinTrusted = 3;            // samples before a range is used
constexpr double kDefaultPxPerMm = 10.0;  // before any range is learned
constexpr double kChunkSec = 0.1;         // window of each sample
constexpr double kChunkMinMm = 0.5;
constexpr double kSaveIntervalSec = 30;

// No frame for this long with an armed glide: the lift of the fingers was missed (an elevated window or the
// secure desktop took the touchpad), and a timer ends the glide, so that the wheel hook comes off.
constexpr DWORD kStaleFrameMs = 500;
// The wheel is only blocked while the touchpad reports: a wheel event without a recent report comes from a
// mouse, or from a glide whose end was missed.
constexpr DWORD kWheelReportMs = 250;
// Frames processed back to back would give a near-zero interval and spike the measured speed.
constexpr double kMinFrameSec = 0.005;

std::atomic<double> g_multFive{2.0};
std::atomic<DWORD> g_lastReportTick;
std::atomic<double> g_multOne{1.0};
std::atomic<bool> g_armTwo;
std::atomic<bool> g_scrollReverse;
std::atomic<bool> g_armThree;
std::atomic<bool> g_armFour;
std::atomic<int> g_tapProfile{2};

struct Bin {
    double pxPerMm = 0;
    int samples = 0;
    int rejected = 0;  // in a row, not saved
};

// Loaded before the touchpad thread starts, saved by it every kSaveIntervalSec and by WhTool_ModUninit
// after it ends.
Bin g_curve[kBinCount];
bool g_curveDirty;
double g_lastSave;

HANDLE g_stopEvent;
HANDLE g_thread;
HWND g_hWnd;
HANDLE g_hookThread;
DWORD g_hookThreadId;
constexpr UINT kWheelBlockMessage = WM_APP + 1;
constexpr UINT_PTR kStaleTimerId = 1;
constexpr UINT_PTR kTwoTapTimerId = 2;
constexpr UINT_PTR kInertiaTimerId = 3;
constexpr UINT_PTR kDoubleTimerId = 4;

void SetWheelBlock(bool on);

// Touchpad and glide state, up to g_state: only used by the touchpad thread.

struct Device {
    bool valid = false;
    std::vector<BYTE> preparsed;
    std::vector<USHORT> slots;  // link collection of each finger
    bool hasContactCount = false;
    USHORT contactCountCollection = 0;
    double mmX = 0;  // mm per logical unit
    double mmY = 0;
};

struct Contact {
    ULONG id;
    double x, y;  // mm
    bool tip, conf;
};

struct Track {
    DWORD born;
    double x0, y0;
    double maxDist = 0;
    bool noTap = false;
    bool holdCandidate = false;
    bool afterClick = false;  // came back after a click, not a loose tap
};

std::unordered_map<HANDLE, Device> g_devices;
std::vector<Contact> g_frame;
ULONG g_frameExpected;
bool g_seenConfidence;

struct {
    bool armed;
    DWORD armTime;
    bool anchorValid;
    std::unordered_map<ULONG, std::pair<double, double>> last;  // id -> previous position, mm
    bool single;
    double accX, accY;
    std::unordered_map<ULONG, Track> tracks;
    bool recentClick;
    DWORD lastLossTime;
    DWORD lastLandTime;
    double lastTapX, lastTapY;  // mm, where the last tap's finger was
    DWORD looseTapTime;         // when a finger last lifted within the hold's looser tap, 0 if none
    double looseTapX, looseTapY;
    bool holding;
    ULONG holdId;
    bool pendingDouble;  // a double click's second click waits for a possible hold
    ULONG lastLandId;
    DWORD lastClickTime;
    POINT lastClickPos;
    double lastFrameTime;  // s
    double speed;          // mm/s, smoothed
    bool stopped;
    DWORD lastFrameTick;
} g_state;

// A touch from the first contact down to the last one up, for the two-finger gestures. The pending tap outlives
// the touch, until a right click or a scroll settles it.
struct {
    bool inTouch;
    DWORD start;
    int maxFingers;
    int maxContacts;
    DWORD twoAt;  // when two fingers were first down, 0 if never
    double maxMove;
    std::unordered_map<ULONG, std::pair<double, double>> origin;
    bool scrolling;
    bool deciding;  // started within the wait after a two-finger tap
    bool tapPending;
    DWORD tapTime;
    double scrollAccX, scrollAccY;
    double velX, velY;  // wheel units/s, smoothed
    bool inertia;
    bool lifted;  // the scrolling fingers are up, another finger holds the scroll
    double inertiaTime;
} g_two;

// Sample under way of native one-finger movement.
struct {
    bool active;
    ULONG id;
    double t0;
    double fx0, fy0;
    POINT c0;
} g_learn;

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

double ReadMultiplier(PCWSTR name, double fallback) {
    std::wstring value = WindhawkUtils::StringSetting::make(name).get();
    std::replace(value.begin(), value.end(), L',', L'.');
    double d = wcstod(value.c_str(), nullptr);
    return d > 0 ? std::clamp(d, 0.05, 10.0) : fallback;
}

void LoadSettings() {
    g_multFive = ReadMultiplier(L"multiplierFive", 2.0);
    g_multOne = ReadMultiplier(L"multiplierOne", 1.0);
    g_armTwo = Wh_GetIntSetting(L"armTwo");
    g_scrollReverse = Wh_GetIntSetting(L"scrollReverse");
    g_armThree = Wh_GetIntSetting(L"armThree");
    g_armFour = Wh_GetIntSetting(L"armFour");

    auto tap = WindhawkUtils::StringSetting::make(L"tapToClick");
    int profile = 2;
    if (wcscmp(tap, L"off") == 0) {
        profile = 0;
    } else if (wcscmp(tap, L"strict") == 0) {
        profile = 1;
    } else if (wcscmp(tap, L"lenient") == 0) {
        profile = 3;
    }
    g_tapProfile = profile;
}

// The learned curve.

void LogCurve() {
    WCHAR text[512];
    int length = 0;
    for (int i = 0; i < kBinCount && length >= 0; i++) {
        length += swprintf_s(text + length, ARRAYSIZE(text) - length, L" %.0f:%.2f(%d)", kSpeedBins[i],
                             g_curve[i].pxPerMm, g_curve[i].samples);
    }
    Wh_Log(L"Curve (mm/s:px/mm(samples)):%s", text);
}

void LoadCurve() {
    for (int i = 0; i < kBinCount; i++) {
        WCHAR name[16];
        swprintf_s(name, L"curve%d", i);
        g_curve[i].pxPerMm = Wh_GetIntValue(name, 0) / 1000.0;
        swprintf_s(name, L"curve%dn", i);
        g_curve[i].samples = Wh_GetIntValue(name, 0);
    }
    LogCurve();
}

void SaveCurve() {
    if (!g_curveDirty) {
        return;
    }
    for (int i = 0; i < kBinCount; i++) {
        WCHAR name[16];
        swprintf_s(name, L"curve%d", i);
        Wh_SetIntValue(name, (int)std::lround(g_curve[i].pxPerMm * 1000));
        swprintf_s(name, L"curve%dn", i);
        Wh_SetIntValue(name, g_curve[i].samples);
    }
    g_curveDirty = false;
    g_lastSave = Now();
    LogCurve();
}

int NearestBin(double speed) {
    double ls = std::log(std::max(speed, 1e-3));
    int best = 0;
    for (int i = 1; i < kBinCount; i++) {
        if (std::fabs(ls - std::log(kSpeedBins[i])) < std::fabs(ls - std::log(kSpeedBins[best]))) {
            best = i;
        }
    }
    return best;
}

void AddSample(double speed, double pxPerMm) {
    Bin& bin = g_curve[NearestBin(speed)];
    // Far above a trusted range: something else moved the pointer meanwhile (a mouse, a program). Several in a
    // row mean the range itself is off (the touchpad speed was raised, or it sank to 0): it's learned again.
    if (bin.samples >= kBinTrusted && pxPerMm > bin.pxPerMm * 3) {
        if (++bin.rejected < 5) {
            return;
        }
        bin.samples = 0;
    }
    bin.rejected = 0;
    // A plain average for the first samples, then a moving one, to follow setting changes.
    double alpha = std::max(0.05, 1.0 / (bin.samples + 1));
    bin.pxPerMm = bin.samples ? bin.pxPerMm + (pxPerMm - bin.pxPerMm) * alpha : pxPerMm;
    bin.samples = std::min(bin.samples + 1, 1000);
    g_curveDirty = true;
}

// Interpolates in log speed between the neighboring trusted ranges; outside them, the nearest one. A range is
// never above a faster one: the slowest, fed by few samples, came out above the next and raised the gain of a
// stopping hand.
double TrustedBin(int i) {
    double v = g_curve[i].pxPerMm;
    for (int j = i + 1; j < kBinCount; j++) {
        if (g_curve[j].samples >= kBinTrusted) {
            v = std::min(v, g_curve[j].pxPerMm);
        }
    }
    return v;
}

double CurveAt(double speed) {
    double ls = std::log(std::max(speed, 1e-3));
    int lo = -1;
    int hi = -1;
    for (int i = 0; i < kBinCount; i++) {
        if (g_curve[i].samples < kBinTrusted) {
            continue;
        }
        if (std::log(kSpeedBins[i]) <= ls) {
            lo = i;
        } else if (hi < 0) {
            hi = i;
        }
    }
    if (lo < 0 && hi < 0) {
        return kDefaultPxPerMm;
    }
    if (lo < 0) {
        return TrustedBin(hi);
    }
    if (hi < 0) {
        return TrustedBin(lo);
    }
    double a = std::log(kSpeedBins[lo]);
    double b = std::log(kSpeedBins[hi]);
    double t = (ls - a) / (b - a);
    return TrustedBin(lo) + (TrustedBin(hi) - TrustedBin(lo)) * t;
}

// The pointer stops at the edge while the finger goes on: samples there would underestimate the curve.
bool NearMonitorEdge(POINT p) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST), &mi)) {
        return true;
    }
    const RECT& r = mi.rcMonitor;
    return p.x - r.left < 3 || r.right - 1 - p.x < 3 || p.y - r.top < 3 || r.bottom - 1 - p.y < 3;
}

// A hidden or clipped pointer (games, 3D viewports, slider drags) is moved back or held, and doesn't show
// Windows' movement.
bool PointerCaptured() {
    CURSORINFO ci{.cbSize = sizeof(ci)};
    if (GetCursorInfo(&ci) && !(ci.flags & CURSOR_SHOWING)) {
        return true;
    }
    RECT clip;
    if (!GetClipCursor(&clip)) {
        return false;
    }
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    return clip.left > x || clip.top > y || clip.right < x + GetSystemMetrics(SM_CXVIRTUALSCREEN) ||
           clip.bottom < y + GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

// Outside a glide, with one finger, Windows moves the pointer: compares the finger's displacement with the
// pointer's over kChunkSec windows.
void Learn(const std::vector<const Contact*>& fingers, int touching, double t) {
    if (g_state.armed || touching != 1 || fingers.size() != 1) {
        g_learn.active = false;
        return;
    }

    const Contact* c = fingers[0];
    POINT cursor;
    if (!GetCursorPos(&cursor) || PointerCaptured()) {
        g_learn.active = false;
        return;
    }

    if (g_learn.active && g_learn.id == c->id) {
        double dt = t - g_learn.t0;
        if (dt < kChunkSec) {
            return;
        }
        double mm = std::hypot(c->x - g_learn.fx0, c->y - g_learn.fy0);
        double px = std::hypot((double)(cursor.x - g_learn.c0.x), (double)(cursor.y - g_learn.c0.y));
        // Samples where Windows didn't move the pointer (px = 0) are learned too: skipping them pushed the slow
        // end of the curve up, and the glide slid on after the hand stopped.
        if (mm >= kChunkMinMm && !NearMonitorEdge(g_learn.c0) && !NearMonitorEdge(cursor)) {
            AddSample(mm / dt, px / mm);
        }
    }

    g_learn.active = true;
    g_learn.id = c->id;
    g_learn.t0 = t;
    g_learn.fx0 = c->x;
    g_learn.fy0 = c->y;
    g_learn.c0 = cursor;
}

void SendPrimary(bool down);

void FirePendingDouble(DWORD now);

void EndGlide() {
    FirePendingDouble(GetTickCount());
    if (g_state.holding) {
        g_state.holding = false;
        SendPrimary(false);
    }
    if (g_state.armed) {
        Wh_Log(L"Glide stopped");
        SetWheelBlock(false);
        KillTimer(g_hWnd, kStaleTimerId);
    }
    g_state.armed = false;
    g_state.anchorValid = false;
    g_state.speed = 0;
    g_state.stopped = true;
    g_state.tracks.clear();
    g_learn.active = false;
    if (g_curveDirty && Now() - g_lastSave >= kSaveIntervalSec) {
        SaveCurve();
    }
}

void ResetSession() {
    EndGlide();
    g_frame.clear();
    g_frameExpected = 0;
}

// Output.

// SendInput's button flags name the physical buttons: with the buttons swapped, the primary one is the right.
void SendPrimaryClick() {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input[2]{};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
}

void SendPrimary(bool down) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? (swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN)
                            : (swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP);
    SendInput(1, &input, sizeof(input));
}

// An absolute move as input: unlike SetCursorPos, programs reading mouse input see it, and a drag with the
// button held follows it. Absolute, it skips the mouse acceleration like SetCursorPos.
void SendCursorPos(int x, int y) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) {
        SetCursorPos(x, y);
        return;
    }
    // Windows maps an absolute coordinate n to the pixel n * width / 65536, rounded down: rounding up here lands
    // on the exact pixel, so one-pixel steps aren't lost.
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)(((LONGLONG)(x - vx) * 65536 + vw - 1) / vw);
    input.mi.dy = (LONG)(((LONGLONG)(y - vy) * 65536 + vh - 1) / vh);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    SendInput(1, &input, sizeof(input));
}

void SendSecondaryClick() {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input[2]{};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = swapped ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = swapped ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
    Wh_Log(L"Right click");
}

void SendWheel(DWORD flag, LONG amount) {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = flag;
    input.mi.mouseData = (DWORD)amount;
    input.mi.dwExtraInfo = kScrollMark;
    SendInput(1, &input, sizeof(input));
}

// Sends what's accumulated, in small deltas: a smooth scroll that follows the fingers frame by frame.
void FlushScroll() {
    LONG wy = (LONG)g_two.scrollAccY;
    LONG wx = (LONG)g_two.scrollAccX;
    g_two.scrollAccY -= wy;
    g_two.scrollAccX -= wx;
    if (wy) {
        SendWheel(MOUSEEVENTF_WHEEL, wy);
    }
    if (wx) {
        SendWheel(MOUSEEVENTF_HWHEEL, wx);
    }
}

// Wheel units follow the fingers, without acceleration. Off, the page follows the fingers: down scrolls up.
void Scroll(double dxMm, double dyMm, double dt) {
    bool jitter = std::hypot(dxMm, dyMm) < kJitterMm;
    if (jitter) {
        dxMm = 0;
        dyMm = 0;
    }
    // Mostly along one axis: the other is the fingers' drift, not a scroll.
    if (std::fabs(dyMm) > 2 * std::fabs(dxMm)) {
        dxMm = 0;
    } else if (std::fabs(dxMm) > 2 * std::fabs(dyMm)) {
        dyMm = 0;
    }
    double sign = g_scrollReverse ? -1 : 1;
    double ux = -dxMm * kScrollUnitsPerMm * sign;
    double uy = dyMm * kScrollUnitsPerMm * sign;
    // The velocity is followed even while the fingers hold still, so that a stop before the lift leaves no
    // inertia.
    if (dt > 0) {
        double k = 0.5;
        g_two.velX += (ux / std::max(dt, kMinFrameSec) - g_two.velX) * k;
        g_two.velY += (uy / std::max(dt, kMinFrameSec) - g_two.velY) * k;
    }
    if (jitter) {
        return;
    }
    g_two.scrollAccX += ux;
    g_two.scrollAccY += uy;
    FlushScroll();
}

void StopInertia() {
    if (g_two.inertia) {
        g_two.inertia = false;
        KillTimer(g_hWnd, kInertiaTimerId);
    }
}

void InertiaStep() {
    double t = Now();
    double dt = t - g_two.inertiaTime;
    g_two.inertiaTime = t;
    double decay = std::exp(-dt / kInertiaTau);
    g_two.velX *= decay;
    g_two.velY *= decay;
    if (std::hypot(g_two.velX, g_two.velY) < kInertiaStopSpeed) {
        StopInertia();
        return;
    }
    g_two.scrollAccX += g_two.velX * dt;
    g_two.scrollAccY += g_two.velY * dt;
    FlushScroll();
}

void StartInertia() {
    if (!g_two.inertia && std::hypot(g_two.velX, g_two.velY) >= kInertiaMinSpeed) {
        g_two.inertia = true;
        g_two.inertiaTime = Now();
        SetTimer(g_hWnd, kInertiaTimerId, kInertiaStepMs, nullptr);
    }
}

void SettleTwoTap() {
    if (g_two.tapPending) {
        g_two.tapPending = false;
        KillTimer(g_hWnd, kTwoTapTimerId);
        SendSecondaryClick();
    }
}

// Two-finger gestures, tracked over a whole touch: a tap ends as the last finger lifts, before the glide does.
void TrackTwoFingers(const std::vector<const Contact*>& fingers, int touching, DWORD now) {
    if (!g_armTwo) {
        g_two.inTouch = false;
        g_two.scrolling = false;
        return;
    }

    if (touching == 0) {
        if (g_two.inTouch && g_two.scrolling) {
            StartInertia();
        }
        if (g_two.inTouch && !g_two.scrolling && g_two.maxFingers == 2 && g_two.maxContacts == 2 && g_two.twoAt &&
            g_two.twoAt - g_two.start <= kTogetherMs && now - g_two.start <= kTwoTapMaxMs &&
            g_two.maxMove <= kTwoTapMaxMm) {
            g_two.tapPending = true;
            g_two.tapTime = now;
            SetTimer(g_hWnd, kTwoTapTimerId, kTwoTapWaitMs, nullptr);
        }
        g_two.inTouch = false;
        g_two.scrolling = false;
        return;
    }

    if (!g_two.inTouch) {
        g_two.inTouch = true;
        g_two.start = now;
        g_two.maxFingers = 0;
        g_two.maxContacts = 0;
        g_two.twoAt = 0;
        g_two.maxMove = 0;
        g_two.origin.clear();
        StopInertia();
        g_two.scrollAccX = 0;
        g_two.scrollAccY = 0;
        g_two.velX = 0;
        g_two.velY = 0;
        g_two.lifted = false;
        g_two.deciding = g_two.tapPending && now - g_two.tapTime <= kTwoTapWaitMs;
    }

    g_two.maxFingers = std::max(g_two.maxFingers, (int)fingers.size());
    g_two.maxContacts = std::max(g_two.maxContacts, touching);
    if (fingers.size() >= 2 && !g_two.twoAt) {
        g_two.twoAt = now;
    }
    for (const Contact* c : fingers) {
        auto it = g_two.origin.find(c->id);
        if (it == g_two.origin.end()) {
            g_two.origin[c->id] = {c->x, c->y};
        } else {
            g_two.maxMove = std::max(g_two.maxMove, std::hypot(c->x - it->second.first, c->y - it->second.second));
        }
    }

    // Back within the wait after a tap: two fingers make it a scroll, anything else lets the right click go.
    if (g_two.deciding) {
        if (fingers.size() >= 2 && now - g_two.start <= kTogetherMs) {
            g_two.deciding = false;
            g_two.tapPending = false;
            KillTimer(g_hWnd, kTwoTapTimerId);
            g_two.scrolling = true;
            Wh_Log(L"Scroll started");
        } else if (now - g_two.start > kTogetherMs) {
            g_two.deciding = false;
            SettleTwoTap();
        }
    }

    // A finger left down (the thumb) holds the scroll: the scrolling fingers lifting coast like a lift of the
    // whole hand, and coming back down scroll again without a new tap.
    if (g_two.scrolling) {
        if (fingers.size() <= 1 && !g_two.lifted) {
            g_two.lifted = true;
            StartInertia();
        } else if (fingers.size() >= 2 && g_two.lifted) {
            g_two.lifted = false;
            StopInertia();
            g_two.velX = 0;
            g_two.velY = 0;
            g_two.scrollAccX = 0;
            g_two.scrollAccY = 0;
        }
    }
}

void Move(double dxMm, double dyMm, double dt, double multiplier) {
    if (dt > 0) {
        double v = std::hypot(dxMm, dyMm) / std::max(dt, kMinFrameSec);
        if (g_state.stopped ? v < kStartSpeed : v < kStopSpeed) {
            g_state.stopped = true;
            g_state.speed = v;
            g_state.accX = 0;
            g_state.accY = 0;
            return;
        }
        g_state.stopped = false;
        // Smoothed when speeding up, which absorbs spikes, but followed at once when slowing down: a slow
        // drop kept the last millimeters of a stop on the fast part of the curve, and the pointer slid on.
        g_state.speed = v < g_state.speed ? v : g_state.speed + (v - g_state.speed) * 0.3;
    }

    double pxPerMm = CurveAt(g_state.speed) * multiplier;
    g_state.accX += dxMm * pxPerMm;
    g_state.accY += dyMm * pxPerMm;
    LONG mx = (LONG)g_state.accX;
    LONG my = (LONG)g_state.accY;
    g_state.accX -= mx;
    g_state.accY -= my;
    if (!mx && !my) {
        return;
    }

    POINT pos;
    if (GetCursorPos(&pos)) {
        SendCursorPos(pos.x + mx, pos.y + my);
    }
}

bool InDoubleClickTime(DWORD now) {
    return g_state.recentClick && now - g_state.lastClickTime <= GetDoubleClickTime();
}

// The tap shakes the resting hand and the glide moves the pointer past the Windows double click
// tolerance; the second tap goes back to the first one's spot when it's near it.
void SnapToLastClick() {
    POINT pos;
    GetCursorPos(&pos);
    int tolX = GetSystemMetrics(SM_CXDOUBLECLK) * 4;
    int tolY = GetSystemMetrics(SM_CYDOUBLECLK) * 4;
    if (std::abs(pos.x - g_state.lastClickPos.x) <= tolX && std::abs(pos.y - g_state.lastClickPos.y) <= tolY) {
        SetCursorPos(g_state.lastClickPos.x, g_state.lastClickPos.y);
    }
}

void Click(DWORD now) {
    bool second = InDoubleClickTime(now);
    if (second) {
        SnapToLastClick();
        g_state.recentClick = false;
    } else {
        GetCursorPos(&g_state.lastClickPos);
        g_state.lastClickTime = now;
        g_state.recentClick = true;
    }
    SendPrimaryClick();
    Wh_Log(L"%s", second ? L"Double click" : L"Click");
}

// The glide.

bool IsTapCandidate(const Track& t, DWORD now) {
    const TapProfile& tap = kTapProfiles[g_tapProfile];
    return tap.on && g_state.armed && (LONG)(t.born - g_state.armTime) > 0 && now - t.born <= tap.maxMs;
}

void FirePendingDouble(DWORD now) {
    if (g_state.pendingDouble) {
        g_state.pendingDouble = false;
        KillTimer(g_hWnd, kDoubleTimerId);
        Click(now);
    }
}

// Windows doesn't recognize taps with other fingers resting; here, while gliding, a finger which came down
// after the start and lifts quickly without moving is a click. The fingers which started it never are.
void UpdateTracks(const std::vector<const Contact*>& fingers, DWORD now, int touching) {
    // Losses first, so that a contact which vanishes and comes back under a new id in the same frame is
    // seen as a flicker.
    for (auto it = g_state.tracks.begin(); it != g_state.tracks.end();) {
        ULONG id = it->first;
        if (std::any_of(fingers.begin(), fingers.end(), [id](const Contact* c) { return c->id == id; })) {
            ++it;
            continue;
        }
        Track t = it->second;
        it = g_state.tracks.erase(it);
        g_state.lastLossTime = now;
        // Like a hold on the touchpad alone, the button stays down until the whole hand leaves: the finger which
        // started it can lift while the hand shifts and other fingers keep dragging.
        if (g_state.holding && id == g_state.holdId) {
            g_state.holdId = (ULONG)-1;
            continue;
        }
        // touching > 0: the tap ended with the hand still resting, it's not the whole hand leaving.
        const TapProfile& tap = kTapProfiles[g_tapProfile];
        bool isTap = touching > 0 && !g_state.holding && !t.noTap && IsTapCandidate(t, now) &&
                     now - t.born >= tap.minMs && t.maxDist <= tap.maxMm;
        DWORD left = GetDoubleClickTime() - (now - g_state.lastClickTime);
        if (isTap && t.afterClick && InDoubleClickTime(now) && left > 60) {
            // The double click's second click, sent before the double-click time runs out either way.
            g_state.pendingDouble = true;
            SetTimer(g_hWnd, kDoubleTimerId, std::min(kDoubleWaitMs, left - 30), nullptr);
        } else if (isTap) {
            g_state.lastTapX = t.x0;
            g_state.lastTapY = t.y0;
            Click(now);
        } else {
            FirePendingDouble(now);
        }
        if (touching > 0 && tap.on && g_state.armed && (LONG)(t.born - g_state.armTime) >= (LONG)tap.settleMs &&
            now - t.born >= tap.minMs && now - t.born <= kHoldTapMaxMs && t.maxDist <= kHoldTapMaxMm) {
            g_state.looseTapTime = now;
            g_state.looseTapX = t.x0;
            g_state.looseTapY = t.y0;
        }
    }

    for (const Contact* c : fingers) {
        auto it = g_state.tracks.find(c->id);
        if (it == g_state.tracks.end()) {
            Track t{now, c->x, c->y};
            const TapProfile& tap = kTapProfiles[g_tapProfile];
            t.noTap = !tap.on || now - g_state.armTime < tap.settleMs || now - g_state.lastLossTime <= tap.flickerMs;
            auto other = g_state.tracks.find(g_state.lastLandId);
            if (other != g_state.tracks.end() && now - g_state.lastLandTime <= kTogetherMs) {
                t.noTap = true;
                other->second.noTap = true;
            }
            // Back during a double click's wait: the second click's press goes down here and stays.
            if (g_state.pendingDouble && !g_state.holding &&
                std::hypot(c->x - g_state.lastTapX, c->y - g_state.lastTapY) <= kHoldNearMm) {
                g_state.pendingDouble = false;
                KillTimer(g_hWnd, kDoubleTimerId);
                SnapToLastClick();
                g_state.recentClick = false;
                SendPrimary(true);
                g_state.holding = true;
                g_state.holdId = c->id;
                g_state.looseTapTime = 0;
                t.noTap = true;
            }
            // The tap finger coming back is no flicker nor hand settling: it's the second tap, or a hold.
            if (!g_state.holding && tap.on && g_state.armed && InDoubleClickTime(now)) {
                double fromTap = std::hypot(c->x - g_state.lastTapX, c->y - g_state.lastTapY);
                if (fromTap <= kHoldNearMm) {
                    t.holdCandidate = true;
                    t.afterClick = true;
                    t.noTap = false;
                }
            }
            if (!t.holdCandidate && !g_state.holding && tap.on && g_state.armed && g_state.looseTapTime &&
                now - g_state.looseTapTime <= GetDoubleClickTime()) {
                double fromTap = std::hypot(c->x - g_state.looseTapX, c->y - g_state.looseTapY);
                if (fromTap <= kHoldNearMm) {
                    t.holdCandidate = true;
                }
            }
            g_state.lastLandTime = now;
            g_state.lastLandId = c->id;
            g_state.tracks[c->id] = t;
        } else {
            Track& t = it->second;
            t.maxDist = std::max(t.maxDist, std::hypot(c->x - t.x0, c->y - t.y0));
            // No longer a tap (it stayed or moved): the button goes down, without the waiting click, and stays
            // until the finger lifts.
            const TapProfile& tap = kTapProfiles[g_tapProfile];
            if (t.holdCandidate && !g_state.holding && (now - t.born > tap.maxMs || t.maxDist > tap.maxMm)) {
                t.holdCandidate = false;
                g_state.holding = true;
                g_state.holdId = c->id;
                g_state.looseTapTime = 0;
                POINT pos;
                GetCursorPos(&pos);
                if (t.afterClick && now - g_state.lastClickTime <= GetDoubleClickTime()) {
                    // Off the click's spot by just past half the double-click rectangle, toward where the pointer
                    // went since (to the right if it didn't move), then back where the pointer is.
                    POINT click = g_state.lastClickPos;
                    int offX = GetSystemMetrics(SM_CXDOUBLECLK) / 2 + 1;
                    int offY = GetSystemMetrics(SM_CYDOUBLECLK) / 2 + 1;
                    int dx = pos.x - click.x;
                    int dy = pos.y - click.y;
                    POINT press = click;
                    if (std::abs(dy) > std::abs(dx)) {
                        press.y += dy < 0 ? -offY : offY;
                    } else {
                        press.x += dx < 0 ? -offX : offX;
                    }
                    if (std::abs(dx) <= offX && std::abs(dy) <= offY) {
                        pos = press;
                    }
                    SendCursorPos(press.x, press.y);
                    SendPrimary(true);
                    SendCursorPos(pos.x, pos.y);
                } else {
                    SendPrimary(true);
                }
                g_state.recentClick = false;
            }
        }
    }
}

double Median(std::vector<double>& v) {
    size_t mid = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + mid, v.end());
    double m = v[mid];
    if (v.size() % 2 == 0) {
        m = (m + *std::max_element(v.begin(), v.begin() + mid)) / 2;
    }
    return m;
}

void ProcessFrame() {
    DWORD now = GetTickCount();
    double t = Now();
    if (g_state.armed && now - g_state.lastFrameTick > kStaleFrameMs) {
        EndGlide();
    }
    g_state.lastFrameTick = now;
    for (const Contact& c : g_frame) {
        if (c.conf) {
            g_seenConfidence = true;
        }
    }

    int touching = 0;
    std::vector<const Contact*> fingers;
    std::vector<const Contact*> tips;
    for (const Contact& c : g_frame) {
        if (!c.tip) {
            continue;
        }
        touching++;
        tips.push_back(&c);
        // Confidence = 0 marks a resting palm or thumb: it counts to start, but stays out of the center and
        // of taps. Before Confidence is seen once, it's unknown whether the touchpad reports it.
        if (c.conf || !g_seenConfidence) {
            fingers.push_back(&c);
        }
    }

    UpdateTracks(fingers, now, touching);
    Learn(fingers, touching, t);
    TrackTwoFingers(fingers, touching, now);

    if (touching == 0) {
        ResetSession();
        return;
    }

    // A palm or thumb (Confidence = 0) only counts toward the whole hand's five: with three on, a two-finger
    // scroll with the thumb resting would start a glide. A count which is off doesn't start: its swipe is Windows'.
    int count = (int)fingers.size();
    bool canArm = touching >= kFingers || count >= kFingers || (count == 2 && g_armTwo) || (count == 3 && g_armThree) ||
                  (count == 4 && g_armFour);
    if (canArm && !g_state.armed) {
        g_state.armed = true;
        g_state.armTime = now;
        SetWheelBlock(true);
        SetTimer(g_hWnd, kStaleTimerId, kStaleFrameMs, nullptr);
        Wh_Log(L"Glide started");
    }

    if (!g_state.armed) {
        g_state.anchorValid = false;
        return;
    }

    // A finger which just came down stays out until it can't be a tap anymore, otherwise the tap itself would
    // drag the pointer and the click would miss its target.
    const std::vector<const Contact*>& movers = fingers.empty() ? tips : fingers;
    std::vector<const Contact*> used;
    for (const Contact* c : movers) {
        auto it = g_state.tracks.find(c->id);
        // A finger which already moved past a tap's reach can't be one: it joins at once.
        if (it != g_state.tracks.end() && !it->second.noTap && IsTapCandidate(it->second, now) &&
            it->second.maxDist <= kTapProfiles[g_tapProfile].maxMm) {
            continue;
        }
        used.push_back(c);
    }
    if (used.empty()) {
        g_state.anchorValid = false;
        return;
    }

    if (!g_state.anchorValid) {
        g_state.last.clear();
        for (const Contact* c : used) {
            g_state.last[c->id] = {c->x, c->y};
        }
        g_state.accX = 0;
        g_state.accY = 0;
        g_state.lastFrameTime = t;
        g_state.anchorValid = true;
        return;
    }

    // Fingers joining or leaving don't stop the others: a joining one only takes its position this frame. Taking
    // every position again on each change lost a frame of movement whenever a finger landed, lifted or became
    // part of the movement, which made five fingers with tap to click on less responsive.
    for (auto it = g_state.last.begin(); it != g_state.last.end();) {
        ULONG id = it->first;
        bool present = std::any_of(used.begin(), used.end(), [id](const Contact* c) { return c->id == id; });
        it = present ? std::next(it) : g_state.last.erase(it);
    }

    // A single finger moving alone is followed on its own, not divided by the resting ones; otherwise the
    // median of the fingers is: it cancels their jitter like an average, and a thumb which pivots or lags in a
    // fast flick doesn't pull the pointer toward it.
    double dx = 0;
    double dy = 0;
    std::vector<double> allX;
    std::vector<double> allY;
    double best = -1;
    double second = 0;
    int counted = 0;
    for (const Contact* c : used) {
        auto found = g_state.last.find(c->id);
        if (found == g_state.last.end()) {
            g_state.last[c->id] = {c->x, c->y};
            continue;
        }
        auto& prev = found->second;
        double fx = c->x - prev.first;
        double fy = c->y - prev.second;
        prev = {c->x, c->y};
        if (std::hypot(fx, fy) > kJumpMm) {
            continue;
        }
        counted++;
        allX.push_back(fx);
        allY.push_back(fy);
        double d = std::hypot(fx, fy);
        if (d > best) {
            second = std::max(second, best);
            best = d;
            dx = fx;
            dy = fy;
        } else {
            second = std::max(second, d);
        }
    }

    if (!counted) {
        g_state.lastFrameTime = t;
        return;
    }

    if (best < kJitterMm) {
        g_state.single = false;
    } else if (best >= kSingleStartMm && second < best * kSingleFingerShare) {
        g_state.single = true;
    } else if (second > best * kHandShare) {
        g_state.single = false;
    }
    if (!g_state.single) {
        dx = Median(allX);
        dy = Median(allY);
    }

    double dt = t - g_state.lastFrameTime;
    g_state.lastFrameTime = t;
    if (g_two.scrolling) {
        if (!g_two.lifted) {
            Scroll(Median(allX), Median(allY), dt);
        }
        return;
    }
    // Up to two fingers moving, or one moving alone, aim like a single finger; three to five move the hand.
    Move(dx, dy, dt, g_state.single || counted <= 2 ? g_multOne.load() : g_multFive.load());
}

// The touchpad.

double LogicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG min = caps.LogicalMin;
    LONGLONG max = caps.LogicalMax;
    // Some devices declare a maximum which only fits unsigned.
    if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
        min = 0;
        max = (1LL << caps.BitSize) - 1;
    }
    return (double)(max - min);
}

// Units: low nibble 1 = SI (cm), 3 = English (inch); UnitsExp is a signed nibble.
double MmPerUnit(const HIDP_VALUE_CAPS& caps) {
    double logical = LogicalRange(caps);
    if (logical <= 0) {
        return 0;
    }
    int exponent = caps.UnitsExp & 0xF;
    if (exponent > 7) {
        exponent -= 16;
    }
    ULONG system = caps.Units & 0xF;
    double unitMm = system == 1 ? 10.0 : system == 3 ? 25.4 : 0;
    double extentMm = ((double)caps.PhysicalMax - caps.PhysicalMin) * std::pow(10.0, exponent) * unitMm;
    if (extentMm < 20 || extentMm > 400) {
        return 0;
    }
    return extentMm / logical;
}

const Device* GetDevice(HANDLE hDevice) {
    auto it = g_devices.find(hDevice);
    if (it != g_devices.end()) {
        return it->second.valid ? &it->second : nullptr;
    }

    Device& dev = g_devices[hDevice];

    UINT size = 0;
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, nullptr, &size) != 0 || !size) {
        return nullptr;
    }
    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, dev.preparsed.data(), &size) == (UINT)-1) {
        return nullptr;
    }

    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps;
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount, preparsed) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    const HIDP_VALUE_CAPS* xCaps = nullptr;
    const HIDP_VALUE_CAPS* yCaps = nullptr;
    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        USAGE usage = vc.IsRange ? vc.Range.UsageMin : vc.NotRange.Usage;
        if (vc.UsagePage == kPageDigitizer && usage == kUsageContactCount) {
            dev.hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageX) {
            if (std::find(dev.slots.begin(), dev.slots.end(), vc.LinkCollection) == dev.slots.end()) {
                dev.slots.push_back(vc.LinkCollection);
            }
            if (!xCaps) {
                xCaps = &vc;
            }
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageY && !yCaps) {
            yCaps = &vc;
        }
    }
    if (!xCaps || !yCaps) {
        return nullptr;
    }

    dev.mmX = MmPerUnit(*xCaps);
    dev.mmY = MmPerUnit(*yCaps);
    if (dev.mmX <= 0) {
        double range = LogicalRange(*xCaps);
        dev.mmX = range > 0 ? 100.0 / range : 0;  // no physical size: assume 100 mm wide
    }
    if (dev.mmY <= 0) {
        dev.mmY = dev.mmX;
    }

    dev.valid = !dev.slots.empty() && dev.mmX > 0;
    Wh_Log(L"Touchpad %p: valid=%d, slots=%u, contact count=%d, ~%.0fx%.0f mm", hDevice, dev.valid,
           (UINT)dev.slots.size(), dev.hasContactCount, LogicalRange(*xCaps) * dev.mmX,
           LogicalRange(*yCaps) * dev.mmY);

    return dev.valid ? &dev : nullptr;
}

// In hybrid mode a frame is spread over several reports: only the first carries a contact count above zero,
// and the following ones carry zero with the remaining contacts.
void OnReport(const Device& dev, PCHAR report, ULONG length) {
    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    if (dev.hasContactCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, kPageDigitizer, dev.contactCountCollection, kUsageContactCount, &count,
                               preparsed, report, length) == HIDP_STATUS_SUCCESS &&
            count > 0) {
            g_frame.clear();
            g_frameExpected = count;
        }
        // The rest of a frame whose start was missed.
        if (!g_frameExpected) {
            return;
        }
    } else {
        g_frame.clear();
        g_frameExpected = (ULONG)dev.slots.size();
    }

    for (USHORT collection : dev.slots) {
        if (g_frame.size() >= g_frameExpected) {
            break;
        }

        ULONG x;
        ULONG y;
        if (HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageX, &x, preparsed, report, length) !=
                HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageY, &y, preparsed, report, length) !=
                HIDP_STATUS_SUCCESS) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, kPageDigitizer, collection, kUsageContactId, &id, preparsed, report, length);

        bool tip = false;
        bool conf = false;
        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, kPageDigitizer, collection, usages, &usageCount, preparsed, report,
                           length) == HIDP_STATUS_SUCCESS) {
            for (ULONG k = 0; k < usageCount; k++) {
                if (usages[k] == kUsageTipSwitch) {
                    tip = true;
                } else if (usages[k] == kUsageConfidence) {
                    conf = true;
                }
            }
        }

        g_frame.push_back({id, x * dev.mmX, y * dev.mmY, tip, conf});
    }

    if (g_frame.size() >= g_frameExpected) {
        ProcessFrame();
        g_frame.clear();
        g_frameExpected = 0;
    }
}

void OnRawInput(HRAWINPUT hRawInput) {
    UINT size = 0;
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) != 0 || !size) {
        return;
    }

    static std::vector<BYTE> buffer;
    buffer.resize(size);
    if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == (UINT)-1) {
        return;
    }

    const RAWINPUT* raw = (const RAWINPUT*)buffer.data();
    if (raw->header.dwType != RIM_TYPEHID) {
        return;
    }

    const Device* dev = GetDevice(raw->header.hDevice);
    if (!dev) {
        return;
    }
    g_lastReportTick = GetTickCount();

    const RAWHID& hid = raw->data.hid;
    for (DWORD i = 0; i < hid.dwCount; i++) {
        OnReport(*dev, (PCHAR)hid.bRawData + i * hid.dwSizeHid, hid.dwSizeHid);
    }
}

// The Windows two-finger scroll is decided when the touch starts and goes on with five fingers; while
// gliding it's dropped here. The hook only exists during a glide: while installed, every mouse message of
// the system goes through it.
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) &&
        ((MSLLHOOKSTRUCT*)lParam)->dwExtraInfo != kScrollMark && GetTickCount() - g_lastReportTick < kWheelReportMs) {
        return 1;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// The hook runs on a thread of its own, which does nothing else: the system waits for it on every mouse
// event, and on the touchpad thread, busy moving the pointer, it held all input, the touchpad's included,
// for up to a second.
DWORD WINAPI HookThread(LPVOID readyEvent) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&MouseHookProc, &hInstance);

    // Creates this thread's message queue before anything is posted to it.
    MSG msg;
    PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
    SetEvent((HANDLE)readyEvent);

    HHOOK hook = nullptr;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != kWheelBlockMessage) {
            continue;
        }
        if (msg.wParam && !hook) {
            hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, 0);
            if (!hook) {
                Wh_Log(L"SetWindowsHookEx error: %u", GetLastError());
            }
        } else if (!msg.wParam && hook) {
            UnhookWindowsHookEx(hook);
            hook = nullptr;
        }
    }

    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    return 0;
}

void SetWheelBlock(bool on) {
    if (g_hookThreadId) {
        PostThreadMessage(g_hookThreadId, kWheelBlockMessage, on, 0);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INPUT:
            OnRawInput((HRAWINPUT)lParam);
            break;

        case WM_TIMER:
            if (wParam == kDoubleTimerId) {
                FirePendingDouble(GetTickCount());
                return 0;
            }
            if (wParam == kInertiaTimerId) {
                InertiaStep();
                return 0;
            }
            if (wParam == kTwoTapTimerId) {
                if (!g_two.deciding) {
                    SettleTwoTap();
                }
                return 0;
            }
            if (wParam == kStaleTimerId) {
                if (g_state.armed && GetTickCount() - g_state.lastFrameTick > kStaleFrameMs) {
                    ResetSession();
                }
                return 0;
            }
            break;

        // A handle can be reused by another device.
        case WM_INPUT_DEVICE_CHANGE:
            g_devices.erase((HANDLE)lParam);
            if (wParam == GIDC_REMOVAL) {
                ResetSession();
            }
            return 0;

        // The lift of the fingers may never be seen from here on, and an ongoing glide would block scrolling.
        case WM_POWERBROADCAST:
            if (wParam == PBT_APMSUSPEND) {
                ResetSession();
            }
            break;

        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK || wParam == WTS_CONSOLE_DISCONNECT || wParam == WTS_REMOTE_DISCONNECT) {
                ResetSession();
            }
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RunMessageLoop() {
    while (true) {
        DWORD result = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            return;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    // A delay in scheduling this thread shows as jitter in the pointer.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&WndProc, &hInstance);

    WNDCLASS wc{
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .lpszClassName = kWindowClassName,
    };
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass error: %u", GetLastError());
        return 0;
    }

    // A hidden top level window rather than a message-only one, which raw input in the background isn't
    // reliably delivered to.
    g_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kWindowClassName, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                            hInstance, nullptr);
    if (!g_hWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        UnregisterClass(kWindowClassName, hInstance);
        return 0;
    }

    RAWINPUTDEVICE rid{
        .usUsagePage = kPageDigitizer,
        .usUsage = kUsageTouchPad,
        .dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY,
        .hwndTarget = g_hWnd,
    };
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        WTSRegisterSessionNotification(g_hWnd, NOTIFY_FOR_THIS_SESSION);

        RunMessageLoop();

        SetWheelBlock(false);
        WTSUnRegisterSessionNotification(g_hWnd);

        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = nullptr;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    } else {
        Wh_Log(L"RegisterRawInputDevices error: %u", GetLastError());
    }

    DestroyWindow(g_hWnd);
    g_hWnd = nullptr;
    UnregisterClass(kWindowClassName, hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    LoadCurve();
    g_lastSave = Now();

    HANDLE hookReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hookReady) {
        g_hookThread = CreateThread(nullptr, 0, HookThread, hookReady, 0, &g_hookThreadId);
        if (g_hookThread) {
            WaitForSingleObject(hookReady, 5000);
        } else {
            g_hookThreadId = 0;
            Wh_Log(L"Can't start the hook thread: %u", GetLastError());
        }
        CloseHandle(hookReady);
    }

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    }
    if (!g_thread) {
        Wh_Log(L"Can't start the touchpad thread: %u", GetLastError());
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;

    if (g_hookThread) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    SaveCurve();
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

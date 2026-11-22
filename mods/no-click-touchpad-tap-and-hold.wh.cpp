// ==WindhawkMod==
// @id              no-click-touchpad-tap-and-hold
// @name            No-Click Touchpad: Tap & Hold
// @description     Tap a corner to right-click and hold two fingers to drag, with a ring around the pointer while the drag is on. For touchpads that no longer click.
// @version         1.1
// @author          mario0318
// @github          https://github.com/mario0318
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -lhid -lshcore -lgdi32 -lwtsapi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/mario0318/no-click-touchpad-tap-and-hold/issues

// ==WindhawkModReadme==
/*
# No-Click Touchpad: Tap & Hold

Right-click and drag with taps and holds, for touchpads whose click no longer works.
It's also handy if you'd rather tap than press.

## Right-click
Tap the bottom right corner of the touchpad with one finger to right-click. You can
switch to the bottom left corner in the mod's settings. Without the mod, a tap in
that corner is a normal left click.

To avoid right-clicking by accident, set "Taps to right-click" to two. Then you tap
the corner twice, as fast as a double-click, and a single tap there does nothing.
How fast is set by Windows' double-click speed (Settings > Bluetooth & devices >
Mouse > Additional mouse settings).

## Drag
Rest two fingers on the touchpad and keep them still for a moment. A ring appears
around the pointer to show that a drag has started. Move your fingers to select text
or move a window, and lift them to let go.

To scroll, move two fingers straight away instead of holding them still first.
Pinching or rotating two fingers won't start a drag.

On a small touchpad, a long drag can run out of room. Set "Time to reposition fingers"
in the mod's settings to keep the drag going for a moment after you lift your fingers,
so you can put them back down and carry on. During that moment, a one-finger tap ends
the drag right away.

![Selecting text with a two-finger drag, ring around the pointer](https://raw.githubusercontent.com/mario0318/windhawk-mods/readme-images/no-click-touchpad-tap-and-hold/drag-ring-select-1.png)

![The ring stays on the pointer until the fingers lift](https://raw.githubusercontent.com/mario0318/windhawk-mods/readme-images/no-click-touchpad-tap-and-hold/drag-ring-select-2.png)

## Requirements
The touchpad has to be a Precision Touchpad, which most laptops from the last several
years have. Some older touchpads aren't supported.

In Settings > Bluetooth & devices > Touchpad, keep "Tap with a single finger to
single-click" on, since the corner right-click needs it. Turning off "Tap twice and
drag" there is recommended, because with it on Windows waits a moment after every tap
to see whether a drag follows. If letting go of a two-finger hold too early opens a
menu, turn off "Tap with two fingers to right-click" as well.

If you also use the Touchpad Relaxed Hand Tracking mod, turn off its two-finger option
so the two mods don't both react to the same gesture.

## Limitations
While you drag, some apps may still scroll a little, and a mouse wheel doesn't scroll.

On Windhawk 1.x, neither the right-click nor the drag works in apps running as
administrator. On a regular (not portable) install of Windhawk 2.0, both work there too.

## Issues
Bugs and feature requests go on the [issues
page](https://github.com/mario0318/no-click-touchpad-tap-and-hold/issues).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerTap: true
  $name: Corner tap right-click
  $description: A quick one-finger tap in the chosen corner right-clicks
- cornerTaps: single
  $name: Taps to right-click
  $description: With two, a single tap in the corner does nothing
  $options:
  - single: One tap
  - double: Two taps
- corner: bottomRight
  $name: Corner
  $options:
  - bottomRight: Bottom right
  - bottomLeft: Bottom left
- zoneWidth: 0
  $name: Corner width (% of touchpad)
  $description: 0 uses the size of Windows' own right-click zone
- zoneHeight: 0
  $name: Corner height (% of touchpad)
  $description: 0 uses the size of Windows' own right-click zone
- maxTapMs: 400
  $name: Maximum tap length (ms)
  $description: A touch longer than this is not treated as a tap
- twoFingerDrag: true
  $name: Two-finger hold-and-drag
  $description: Hold two fingers still, then move them, to drag
- holdMs: 350
  $name: Hold time before drag (ms)
  $description: 100 to 5000. Longer avoids starting a drag when you rest two fingers before scrolling
- dragSpeed: 130
  $name: Drag pointer speed (%)
  $description: At 100, moving across the whole width of the touchpad moves the pointer across the whole width of the screen
- dragLockMs: 0
  $name: Time to reposition fingers (ms)
  $description: 0 to 2000. How long a drag keeps going after you lift your fingers, so you can put them back down and carry on. 0 ends the drag as soon as they lift
- dragIndicator: ring
  $name: Drag indicator
  $description: What shows that a two-finger drag has started
  $options:
  - ring: Ring around the pointer
  - none: Nothing
- ringColor: blue
  $name: Ring color
  $options:
  - blue: Blue
  - green: Green
  - orange: Orange
  - red: Red
  - white: White
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <hidusage.h>
#include <hidpi.h>
#include <shellscalingapi.h>
#include <wtsapi32.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
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

constexpr ULONG_PTR kMarker = 0xC0FFEE42;  // tags input we inject ourselves
constexpr UINT kMsgReloadSettings = WM_APP + 1;  // thread message from the Windhawk thread

constexpr UINT_PTR kHoldTimer = 1;   // only while two fingers are held still
constexpr UINT_PTR kTapTimer = 2;    // only while a click is held back
constexpr UINT_PTR kStaleTimer = 3;  // only while something touches the pad
constexpr UINT_PTR kHookTimer = 4;   // removes the mouse hook once the pad is idle
constexpr UINT_PTR kRingTimer = 5;   // only while the ring pulses or the fingers are lifted
constexpr UINT_PTR kReleaseTimer = 6;  // only while a button-up still has to be delivered
constexpr UINT_PTR kGraceTimer = 7;  // only while a drag waits for the fingers to return

constexpr DWORD kClickAfterLiftMs = 800;  // a tap's click comes this soon after the lift
constexpr DWORD kLiftWaitMs = 60;         // how long a held-back click waits for the lift report
constexpr DWORD kPendingMaxMs = 1500;     // never hold a click back longer than this
constexpr DWORD kStaleFrameMs = 500;      // no report for this long means the touch is over
constexpr DWORD kStaleDragMs = 5000;      // the same during a drag, where fingers may rest still
constexpr DWORD kAfterDragMs = 600;       // Windows' leftover scroll events after a drag
constexpr DWORD kHookIdleMs = 1000;       // longer than kClickAfterLiftMs and kAfterDragMs
constexpr DWORD kReleaseRetryMs = 500;    // how often an undelivered button-up is retried
constexpr LONG kOtherMouseMovePx = 8;     // pointer travel after a lift that means another mouse
constexpr double kMaxTapMove = 0.04;      // fraction of the touchpad width
constexpr double kHoldMove = 0.015;

// The drag ring, in pixels at 100% scaling.
constexpr double kRingRadius = 15;
constexpr double kRingThick = 3;
constexpr double kRingGlow = 5;
constexpr double kRingPulseFrom = 1.8;  // starts this much bigger and shrinks into place
constexpr DWORD kRingPulseMs = 200;
constexpr DWORD kRingFrameMs = 15;

// ---------------------------------------------------------------------------
// Settings. Written on the Windhawk thread only before the worker starts, then
// reloaded on the worker thread itself.
// ---------------------------------------------------------------------------
struct Settings {
    bool cornerTap = true;
    bool doubleTap = false;
    bool bottomLeft = false;
    double zoneW = 0.5, zoneH = 0.33;
    DWORD maxTapMs = 400;
    bool twoFingerDrag = true;
    DWORD holdMs = 350;
    double dragSpeed = 1.3;
    DWORD dragLockMs = 0;
    bool ring = true;
    BYTE ringR = 0x3B, ringG = 0x82, ringB = 0xF6;
};
Settings g_s;

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void ReadWindowsZone(double* w, double* h) {
    const wchar_t* key = L"Software\\Microsoft\\Windows\\CurrentVersion\\PrecisionTouchPad";
    DWORD v, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"RightClickZoneWidth", RRF_RT_REG_DWORD,
                     nullptr, &v, &cb) == ERROR_SUCCESS && v > 0 && v <= 100)
        *w = v / 100.0;
    cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"RightClickZoneHeight", RRF_RT_REG_DWORD,
                     nullptr, &v, &cb) == ERROR_SUCCESS && v > 0 && v <= 100)
        *h = v / 100.0;
}

static void LoadSettings() {
    Settings s;
    s.cornerTap = Wh_GetIntSetting(L"cornerTap") != 0;
    s.doubleTap =
        wcscmp(WindhawkUtils::StringSetting::make(L"cornerTaps"), L"double") == 0;
    s.bottomLeft =
        wcscmp(WindhawkUtils::StringSetting::make(L"corner"), L"bottomLeft") == 0;

    double ww = 0.5, wh = 0.33;
    ReadWindowsZone(&ww, &wh);
    int zw = Wh_GetIntSetting(L"zoneWidth"), zh = Wh_GetIntSetting(L"zoneHeight");
    s.zoneW = zw > 0 ? Clamp(zw, 1, 100) / 100.0 : ww;
    s.zoneH = zh > 0 ? Clamp(zh, 1, 100) / 100.0 : wh;

    s.maxTapMs = Clamp(Wh_GetIntSetting(L"maxTapMs"), 100, 1500);
    s.twoFingerDrag = Wh_GetIntSetting(L"twoFingerDrag") != 0;
    s.holdMs = Clamp(Wh_GetIntSetting(L"holdMs"), 100, 5000);
    s.dragSpeed = Clamp(Wh_GetIntSetting(L"dragSpeed"), 20, 500) / 100.0;
    s.dragLockMs = Clamp(Wh_GetIntSetting(L"dragLockMs"), 0, 2000);

    s.ring = wcscmp(WindhawkUtils::StringSetting::make(L"dragIndicator"), L"none") != 0;
    struct {
        PCWSTR name;
        BYTE r, g, b;
    } const colors[] = {
        {L"blue", 0x3B, 0x82, 0xF6},   {L"green", 0x22, 0xC5, 0x5E},
        {L"orange", 0xF9, 0x73, 0x16}, {L"red", 0xEF, 0x44, 0x44},
        {L"white", 0xFF, 0xFF, 0xFF},
    };
    auto color = WindhawkUtils::StringSetting::make(L"ringColor");
    for (const auto& c : colors) {
        if (wcscmp(color, c.name) == 0) {
            s.ringR = c.r;
            s.ringG = c.g;
            s.ringB = c.b;
        }
    }
    g_s = s;
    Wh_Log(L"Settings: corner %s %.2fx%.2f, cornerTap %d, doubleTap %d, twoFingerDrag %d, "
           L"hold %u ms, lift grace %u ms, ring %d #%02X%02X%02X",
           s.bottomLeft ? L"bottomLeft" : L"bottomRight", s.zoneW, s.zoneH, s.cornerTap,
           s.doubleTap, s.twoFingerDrag, s.holdMs, s.dragLockMs, s.ring, s.ringR, s.ringG, s.ringB);
}

// ---------------------------------------------------------------------------
// Output. SendInput's button flags name the physical buttons, so with the
// buttons swapped the primary one is the right.
// ---------------------------------------------------------------------------
HWND g_hwnd;  // the worker thread's hidden window, which owns all the timers

static bool SendMouse(DWORD flags, LONG dx = 0, LONG dy = 0) {
    INPUT in = {};
    in.type = INPUT_MOUSE;
    in.mi.dx = dx;
    in.mi.dy = dy;
    in.mi.dwFlags = flags;
    in.mi.dwExtraInfo = kMarker;
    return SendInput(1, &in, sizeof(in)) == 1;
}

// Physical button flags. The up flag of a button is its down flag shifted by one.
static DWORD PrimaryDown() {
    return GetSystemMetrics(SM_SWAPBUTTON) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
}
static DWORD SecondaryDown() {
    return GetSystemMetrics(SM_SWAPBUTTON) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
}

// A button-up that may not have arrived: SendInput failed, or the input went to
// the lock screen or a UAC prompt. It's sent again once the user's desktop is back,
// so the button can't stay held down.
DWORD g_owedUp;

static bool OnInputDesktop() {
    HDESK desk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!desk)
        return false;  // the secure desktop, which this process can't open
    CloseDesktop(desk);
    return true;
}

static void OweUp(DWORD upFlags) {
    g_owedUp |= upFlags;
    SetTimer(g_hwnd, kReleaseTimer, kReleaseRetryMs, nullptr);
}

// force: the session was just unlocked, so send even if the desktop check fails.
static void RetryOwedUp(bool force) {
    if (g_owedUp && (force || OnInputDesktop())) {
        for (DWORD up : {(DWORD)MOUSEEVENTF_LEFTUP, (DWORD)MOUSEEVENTF_RIGHTUP}) {
            if ((g_owedUp & up) && SendMouse(up)) {
                g_owedUp &= ~up;
                Wh_Log(L"Delivered a button-up that was owed");
            }
        }
    }
    if (!g_owedUp)
        KillTimer(g_hwnd, kReleaseTimer);
}

static bool SendDown(DWORD downFlag) {
    if (!SendMouse(downFlag))
        return false;
    g_owedUp &= ~(downFlag << 1);  // a new press makes an older owed release moot
    return true;
}

static void SendUp(DWORD upFlag) {
    if (!SendMouse(upFlag) || !OnInputDesktop()) {
        Wh_Log(L"Button-up may not have arrived, retrying later");
        OweUp(upFlag);
    }
}

static void SendClick(DWORD downFlag) {
    if (SendDown(downFlag))
        SendUp(downFlag << 1);
}

// An absolute move skips the pointer speed and acceleration settings.
static void SendCursorPos(int x, int y) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN), vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN), vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) {
        SetCursorPos(x, y);
        return;
    }
    // Windows maps n to the pixel n * width / 65536 rounded down, so round up here.
    LONG ax = (LONG)(((LONGLONG)(x - vx) * 65536 + vw - 1) / vw);
    LONG ay = (LONG)(((LONGLONG)(y - vy) * 65536 + vh - 1) / vh);
    SendMouse(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK, ax, ay);
}

// ---------------------------------------------------------------------------
// Touchpad devices
// ---------------------------------------------------------------------------
struct Dev {
    bool valid = false;
    std::vector<BYTE> preparsed;
    std::vector<USHORT> slots;  // link collections that each hold one finger
    bool hasCount = false;
    USHORT countCollection = 0;
    double xMin = 0, xRange = 0, yMin = 0, yRange = 0;
    double aspect = 1;  // physical height / width of the pad
    bool seenConfidence = false;  // until the pad reports Confidence once, nothing counts as a palm
};

struct Contact {
    ULONG id;
    double x, y;  // 0..1 across the pad
    bool tip, conf;
};

std::unordered_map<HANDLE, Dev> g_devs;

static double LogicalRange(const HIDP_VALUE_CAPS& c) {
    LONGLONG lo = c.LogicalMin, hi = c.LogicalMax;
    if (hi <= lo && c.BitSize > 0 && c.BitSize <= 32) {  // a maximum that only fits unsigned
        lo = 0;
        hi = (1LL << c.BitSize) - 1;
    }
    return (double)(hi - lo);
}

// Physical size in millimeters, or 0 when the device doesn't say.
static double ExtentMm(const HIDP_VALUE_CAPS& c) {
    int exponent = c.UnitsExp & 0xF;
    if (exponent > 7)
        exponent -= 16;
    ULONG system = c.Units & 0xF;
    double unitMm = system == 1 ? 10.0 : system == 3 ? 25.4 : 0;
    double mm = ((double)c.PhysicalMax - c.PhysicalMin) * std::pow(10.0, exponent) * unitMm;
    return mm >= 20 && mm <= 400 ? mm : 0;
}

static Dev* GetDev(HANDLE hDevice) {
    auto it = g_devs.find(hDevice);
    if (it != g_devs.end())
        return it->second.valid ? &it->second : nullptr;

    Dev& d = g_devs[hDevice];
    UINT size = 0;
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, nullptr, &size) != 0 || !size)
        return nullptr;
    d.preparsed.resize(size);
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, d.preparsed.data(), &size) ==
        (UINT)-1)
        return nullptr;
    auto pp = (PHIDP_PREPARSED_DATA)d.preparsed.data();

    HIDP_CAPS caps;
    if (HidP_GetCaps(pp, &caps) != HIDP_STATUS_SUCCESS)
        return nullptr;
    USHORT n = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> vc(n);
    if (!n || HidP_GetValueCaps(HidP_Input, vc.data(), &n, pp) != HIDP_STATUS_SUCCESS)
        return nullptr;

    const HIDP_VALUE_CAPS* xCaps = nullptr;
    const HIDP_VALUE_CAPS* yCaps = nullptr;
    for (USHORT i = 0; i < n; i++) {
        const HIDP_VALUE_CAPS& c = vc[i];
        USAGE usage = c.IsRange ? c.Range.UsageMin : c.NotRange.Usage;
        if (c.UsagePage == kPageDigitizer && usage == kUsageContactCount) {
            d.hasCount = true;
            d.countCollection = c.LinkCollection;
        } else if (c.UsagePage == kPageGeneric && usage == kUsageX) {
            bool known = false;
            for (USHORT s : d.slots)
                known = known || s == c.LinkCollection;
            if (!known)
                d.slots.push_back(c.LinkCollection);
            if (!xCaps)
                xCaps = &c;
        } else if (c.UsagePage == kPageGeneric && usage == kUsageY && !yCaps) {
            yCaps = &c;
        }
    }
    if (!xCaps || !yCaps)
        return nullptr;

    d.xMin = xCaps->LogicalMin;
    d.xRange = LogicalRange(*xCaps);
    d.yMin = yCaps->LogicalMin;
    d.yRange = LogicalRange(*yCaps);
    double wMm = ExtentMm(*xCaps), hMm = ExtentMm(*yCaps);
    // Without a physical size, assume the X and Y units are the same size.
    d.aspect = wMm > 0 && hMm > 0 ? hMm / wMm
                                  : (d.xRange > 0 ? d.yRange / d.xRange : 1);

    d.valid = !d.slots.empty() && d.xRange > 0 && d.yRange > 0;
    Wh_Log(L"Touchpad %p: valid %d, %u contact slots, contact count %d, aspect %.2f",
           hDevice, d.valid, (UINT)d.slots.size(), d.hasCount, d.aspect);
    return d.valid ? &d : nullptr;
}

// ---------------------------------------------------------------------------
// Touch state (worker thread only). Rebuilt from every frame, so a lift report
// that never arrives can't leave a finger "down" for good.
// ---------------------------------------------------------------------------
HHOOK g_hook;
HANDLE g_thread, g_stopEvent;

// Gestures follow one device at a time: the first one touched, until its
// fingers lift. A second pad's reports are ignored meanwhile, so the two can't
// mix contacts into one frame or move each other's drag.
HANDLE g_activeDev;
DWORD g_activeTick;  // last report from g_activeDev

std::vector<Contact> g_frame;
ULONG g_frameExpected;
std::unordered_map<ULONG, std::pair<double, double>> g_fingers;  // this frame, palms left out
int g_touching;  // everything on the pad this frame, palms included
double g_aspect = 1;

// Corner tap session, from the first finger down to the last one up.
bool g_haveSession, g_sessionEnded, g_consumed;
DWORD g_startTick, g_endTick;
bool g_endPtSet;  // g_endPt holds the pointer position from the first move after the lift
POINT g_endPt;
ULONG g_primaryId;
double g_sx, g_sy, g_lx, g_ly;
int g_maxFingers;

// The session before this one, for a tap whose click comes after the next
// touch has already started.
bool g_prevTap;
DWORD g_prevStart, g_prevEnd;

// With two taps to right-click: the first corner tap was swallowed, and a second
// one that starts within the double-click time of its lift right-clicks.
bool g_armed;
DWORD g_armedEnd;

// A primary click held back until it's clear whether it was a corner tap.
bool g_pending, g_pendingUp;
bool g_pendingPrev;  // judged by the previous session, which ended as a corner tap
DWORD g_pendingTick, g_upTick;

// Secondary button presses that passed the hook or were dropped by it, so the
// matching release gets the same treatment.
enum { kRNone, kRPassed, kRDropped } g_rState;

// Two-finger drag.
// 0 idle, 1 holding, 2 dragging, 3 done until all fingers lift,
// 4 dragging with the fingers lifted, waiting out the lift grace
int g_twoMode;
DWORD g_twoStart, g_dragEndTick;
std::unordered_map<ULONG, std::pair<double, double>> g_holdStart;  // each finger at the hold's start
double g_refX, g_refY;
double g_curX, g_curY, g_scaleX, g_scaleY;  // pointer position and pixels per pad width
int g_lastCount;
bool g_buttonHeld;
DWORD g_heldUp;  // the button-up flag for the press the drag made, fixed at the press
bool g_graceTapDown;  // a tap's press swallowed during the lift grace

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);

static HMODULE ThisModule() {
    HMODULE h = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&g_s, &h);
    return h;
}

// Restarted by every frame, so it only fires once the reports stop. A pad may
// send nothing while the fingers rest still, so a hold or a drag waits longer.
static void ArmStaleTimer() {
    DWORD ms = kStaleFrameMs;
    if (g_twoMode == 1)
        ms = std::max<DWORD>(kStaleFrameMs, g_s.holdMs + 250);
    else if (g_twoMode == 2 || g_twoMode == 4)
        ms = kStaleDragMs;
    SetTimer(g_hwnd, kStaleTimer, ms, nullptr);
}

// The mouse hook is only needed during a touch and shortly after it, so it's
// installed on a touch's first frame (before Windows sends the tap's click,
// which comes on the lift) and removed once the pad has been idle a while.
static void UpdateHook() {
    if (g_touching) {
        KillTimer(g_hwnd, kHookTimer);
        if (!g_hook) {
            g_hook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, ThisModule(), 0);
            if (!g_hook)
                Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        }
    } else if (g_hook) {
        SetTimer(g_hwnd, kHookTimer, kHookIdleMs, nullptr);
    }
}

static void Centroid(double* cx, double* cy) {
    *cx = *cy = 0;
    for (auto& f : g_fingers) {
        *cx += f.second.first;
        *cy += f.second.second;
    }
    if (!g_fingers.empty()) {
        *cx /= g_fingers.size();
        *cy /= g_fingers.size();
    }
}

static double Distance(double dx, double dy) {
    dy *= g_aspect;  // in pad widths, so both axes count the same
    return std::sqrt(dx * dx + dy * dy);
}

// ---------------------------------------------------------------------------
// Drag ring. A click-through layered window centered on the pointer, shown
// while the mod holds the button down. The tool-mod process is per-monitor DPI
// aware through its manifest, so positions and sizes are physical pixels.
// ---------------------------------------------------------------------------
HWND g_ring;
bool g_ringShown, g_ringSteady;  // steady: the pulse is over and the bitmap is final
DWORD g_ringStart;
UINT g_ringDpi = 96;
int g_ringSize;
POINT g_ringPt;

static LRESULT CALLBACK RingProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST)
        return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// t runs from 0 to 1 over the pulse: the ring shrinks into place and fades in.
static void DrawRing(POINT pt, double t) {
    double k = g_ringDpi / 96.0;
    double ease = (1 - t) * (1 - t);
    double radius = kRingRadius * k * (1 + (kRingPulseFrom - 1) * ease);
    double half = kRingThick * k / 2, glow = kRingGlow * k;
    double opacity = 0.35 + 0.65 * t;

    int size = (int)std::ceil(2 * ((kRingRadius * kRingPulseFrom + kRingGlow * 2.5) * k)) + 2;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bmp && bits) {
        HGDIOBJ oldBmp = SelectObject(mem, bmp);
        DWORD* px = (DWORD*)bits;
        double c = size / 2.0;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                double d = std::hypot(x + 0.5 - c, y + 0.5 - c);
                double e = std::fabs(d - radius) - half;  // distance outside the band
                double a = std::clamp(0.5 - e, 0.0, 1.0);  // the band, with a 1 px soft edge
                if (e > 0)
                    a = std::max(a, 0.5 * std::exp(-(e * e) / (glow * glow)));
                a *= opacity;
                BYTE A = (BYTE)std::lround(a * 255);
                // UpdateLayeredWindow wants premultiplied alpha.
                px[y * size + x] = ((DWORD)A << 24) | ((DWORD)(g_s.ringR * A / 255) << 16) |
                                   ((DWORD)(g_s.ringG * A / 255) << 8) |
                                   (DWORD)(g_s.ringB * A / 255);
            }
        }
        POINT dst = {pt.x - size / 2, pt.y - size / 2}, src = {0, 0};
        SIZE sz = {size, size};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_ring, screen, &dst, &sz, mem, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mem, oldBmp);
        DeleteObject(bmp);
        g_ringSize = size;
    }
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

// The drag moves the ring together with the pointer. The timer is only needed
// for the pulse, and while the fingers are lifted mid-drag, when a single
// finger can move the pointer through Windows instead.
static bool RingNeedsTimer() {
    return g_ringShown && (!g_ringSteady || g_twoMode == 4);
}

static void RingUpdate(POINT pt) {
    if (!g_ringShown)
        return;
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                                MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        dpiX = 96;

    DWORD elapsed = GetTickCount() - g_ringStart;
    bool pulsing = elapsed < kRingPulseMs;
    if (pulsing || !g_ringSteady || dpiX != g_ringDpi) {
        g_ringDpi = dpiX;
        DrawRing(pt, pulsing ? (double)elapsed / kRingPulseMs : 1.0);
        g_ringSteady = !pulsing;
    } else if (pt.x != g_ringPt.x || pt.y != g_ringPt.y) {
        SetWindowPos(g_ring, nullptr, pt.x - g_ringSize / 2, pt.y - g_ringSize / 2, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    g_ringPt = pt;
}

static void RingFollowCursor() {
    POINT pt;
    if (GetCursorPos(&pt))
        RingUpdate(pt);
}

static void StartRingTimer() {
    if (RingNeedsTimer())
        SetTimer(g_hwnd, kRingTimer, kRingFrameMs, nullptr);
}

static void ShowRing() {
    if (!g_ring || !g_s.ring || g_ringShown)
        return;
    g_ringShown = true;
    g_ringSteady = false;
    g_ringStart = GetTickCount();
    RingFollowCursor();
    SetWindowPos(g_ring, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    StartRingTimer();
}

static void HideRing() {
    if (!g_ringShown)
        return;
    g_ringShown = false;
    KillTimer(g_hwnd, kRingTimer);
    ShowWindow(g_ring, SW_HIDE);
}

static void EndDrag() {
    HideRing();
    if (!g_buttonHeld)
        return;
    g_buttonHeld = false;
    g_dragEndTick = GetTickCount();
    SendUp(g_heldUp);  // the button that was pressed, even if the swap changed since
    Wh_Log(L"Two-finger drag end");
}

static void SetTwoMode(int mode) {
    if (g_twoMode == 1 && mode != 1)
        KillTimer(g_hwnd, kHoldTimer);
    if (g_twoMode == 4 && mode != 4)
        KillTimer(g_hwnd, kGraceTimer);
    g_twoMode = mode;
}

static void TryStartDrag() {
    if (g_twoMode != 1 || !g_s.twoFingerDrag || g_fingers.size() != 2)
        return;
    if (GetTickCount() - g_twoStart < g_s.holdMs)
        return;

    DWORD down = PrimaryDown();
    if (!SendDown(down)) {
        Wh_Log(L"Two-finger drag: the button press failed (%u)", GetLastError());
        SetTwoMode(3);
        return;
    }
    g_heldUp = down << 1;
    g_buttonHeld = true;

    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    int width = GetSystemMetrics(SM_CXSCREEN);
    if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi))
        width = mi.rcMonitor.right - mi.rcMonitor.left;
    g_curX = pt.x;
    g_curY = pt.y;
    g_scaleX = width * g_s.dragSpeed;
    g_scaleY = g_scaleX * g_aspect;

    SetTwoMode(2);
    ArmStaleTimer();
    Centroid(&g_refX, &g_refY);
    g_lastCount = 2;
    ShowRing();
    Wh_Log(L"Two-finger drag start");
}

// The lift grace ran out, or a one-finger tap asked to drop the item.
static void DropLiftedDrag() {
    if (g_twoMode != 4)
        return;
    EndDrag();
    SetTwoMode(g_fingers.empty() ? 0 : 3);
}

static void UpdateTwoFinger() {
    int count = (int)g_fingers.size();
    double cx, cy;

    if (g_twoMode == 4) {
        // One finger moves the pointer through Windows with the button still
        // held, which is fine. Two fingers back on the pad carry on the drag.
        if (count < 2)
            return;
        SetTwoMode(2);
        POINT pt;
        if (GetCursorPos(&pt)) {
            g_curX = pt.x;
            g_curY = pt.y;
        }
        Centroid(&g_refX, &g_refY);
        g_lastCount = count;
        Wh_Log(L"Two-finger drag resumed");
        return;
    }

    if (g_twoMode == 2 && count < 2) {
        if (g_s.dragLockMs) {
            SetTwoMode(4);
            SetTimer(g_hwnd, kGraceTimer, g_s.dragLockMs, nullptr);
            StartRingTimer();
            return;
        }
        EndDrag();
        SetTwoMode(count ? 3 : 0);
        return;
    }
    if (count == 0) {
        EndDrag();
        SetTwoMode(0);
        return;
    }

    if (g_twoMode == 2) {
        Centroid(&cx, &cy);
        if (count != g_lastCount) {  // a finger joined: the centroid jumps, so start over from here
            g_refX = cx;
            g_refY = cy;
            g_lastCount = count;
            return;
        }
        g_curX += (cx - g_refX) * g_scaleX;
        g_curY += (cy - g_refY) * g_scaleY;
        g_refX = cx;
        g_refY = cy;
        // Keep the target on a monitor, so moving back from an edge (or a gap
        // between monitors) moves the pointer straight away.
        POINT pt = {(LONG)std::lround(g_curX), (LONG)std::lround(g_curY)};
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
            g_curX = std::clamp(g_curX, (double)mi.rcMonitor.left, mi.rcMonitor.right - 1.0);
            g_curY = std::clamp(g_curY, (double)mi.rcMonitor.top, mi.rcMonitor.bottom - 1.0);
        }
        pt = {(LONG)std::lround(g_curX), (LONG)std::lround(g_curY)};
        SendCursorPos(pt.x, pt.y);
        RingUpdate(pt);
        return;
    }
    if (g_twoMode == 0) {
        if (count == 2 && g_s.twoFingerDrag) {
            SetTwoMode(1);
            g_twoStart = GetTickCount();
            g_holdStart = g_fingers;
            SetTimer(g_hwnd, kHoldTimer, g_s.holdMs, nullptr);
        } else if (count > 2) {
            SetTwoMode(3);
        }
        return;
    }
    if (g_twoMode == 1) {
        if (count != 2) {
            SetTwoMode(3);
            return;
        }
        // Each finger has to stay still, not just their midpoint, or a pinch or
        // a turn (fingers moving in opposite directions) would count as a hold.
        for (auto& f : g_fingers) {
            auto start = g_holdStart.find(f.first);
            if (start == g_holdStart.end() ||
                Distance(f.second.first - start->second.first,
                         f.second.second - start->second.second) > kHoldMove) {
                SetTwoMode(3);  // moved before the hold finished: a scroll or a gesture
                return;
            }
        }
        TryStartDrag();
    }
}

// Forget everything on the pad, for when its reports stop mid-touch (a secure
// desktop, or raw input paused while an elevated window has the focus).
static void ResetTouch() {
    EndDrag();
    SetTwoMode(0);
    g_fingers.clear();
    g_touching = 0;
    g_frame.clear();
    g_frameExpected = 0;
    g_activeDev = nullptr;
    g_graceTapDown = false;
    if (g_haveSession && !g_sessionEnded) {
        g_sessionEnded = true;
        g_consumed = true;  // a cut-off touch is never a tap
    }
    KillTimer(g_hwnd, kStaleTimer);
    UpdateHook();
}

static void ResolvePending();
static bool IsCornerTap();

static void ProcessFrame(Dev& d) {
    DWORD now = GetTickCount();
    g_aspect = d.aspect;

    for (const Contact& c : g_frame)
        d.seenConfidence = d.seenConfidence || c.conf;

    g_fingers.clear();
    g_touching = 0;
    for (const Contact& c : g_frame) {
        if (!c.tip)
            continue;
        g_touching++;
        // Confidence 0 marks a resting palm, which neither taps nor drags.
        if (c.conf || !d.seenConfidence)
            g_fingers[c.id] = {c.x, c.y};
    }

    if (!g_fingers.empty()) {
        if (!g_haveSession || g_sessionEnded) {
            // A click held back for the touch that just ended is judged by that
            // touch, before the new one replaces it.
            if (g_pending)
                ResolvePending();
            g_prevTap = IsCornerTap();
            g_prevStart = g_startTick;
            g_prevEnd = g_endTick;
            auto& first = *g_fingers.begin();
            g_haveSession = true;
            g_sessionEnded = false;
            g_consumed = false;
            g_startTick = now;
            g_primaryId = first.first;
            g_maxFingers = 0;
            g_sx = g_lx = first.second.first;
            g_sy = g_ly = first.second.second;
        }
        auto it = g_fingers.find(g_primaryId);
        if (it != g_fingers.end()) {
            g_lx = it->second.first;
            g_ly = it->second.second;
        }
        if ((int)g_fingers.size() > g_maxFingers)
            g_maxFingers = (int)g_fingers.size();
    } else if (g_haveSession && !g_sessionEnded) {
        g_sessionEnded = true;
        g_endTick = now;
        g_endPtSet = false;
    }

    UpdateHook();
    UpdateTwoFinger();

    if (g_touching)
        ArmStaleTimer();
    else
        KillTimer(g_hwnd, kStaleTimer);
}

// In hybrid mode a frame is spread over several reports. Only the first one
// carries a contact count above zero, the rest carry zero.
static void ProcessReport(Dev& d, PCHAR report, ULONG len) {
    auto pp = (PHIDP_PREPARSED_DATA)d.preparsed.data();

    if (d.hasCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, kPageDigitizer, d.countCollection,
                               kUsageContactCount, &count, pp, report,
                               len) == HIDP_STATUS_SUCCESS &&
            count > 0) {
            g_frame.clear();
            g_frameExpected = count;
        }
        if (!g_frameExpected)
            return;  // the rest of a frame whose start was missed
    } else {
        g_frame.clear();
        g_frameExpected = (ULONG)d.slots.size();
    }

    for (USHORT lc : d.slots) {
        if (g_frame.size() >= g_frameExpected)
            break;
        ULONG x, y;
        if (HidP_GetUsageValue(HidP_Input, kPageGeneric, lc, kUsageX, &x, pp, report, len) !=
                HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, kPageGeneric, lc, kUsageY, &y, pp, report, len) !=
                HIDP_STATUS_SUCCESS)
            continue;
        ULONG id = lc;
        HidP_GetUsageValue(HidP_Input, kPageDigitizer, lc, kUsageContactId, &id, pp, report,
                           len);

        bool tip = false, conf = false;
        USAGE usages[16];
        ULONG ulen = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, kPageDigitizer, lc, usages, &ulen, pp, report, len) ==
            HIDP_STATUS_SUCCESS) {
            for (ULONG u = 0; u < ulen; u++) {
                if (usages[u] == kUsageTipSwitch)
                    tip = true;
                else if (usages[u] == kUsageConfidence)
                    conf = true;
            }
        }
        g_frame.push_back({id, ((LONG)x - d.xMin) / d.xRange, ((LONG)y - d.yMin) / d.yRange,
                           tip, conf});
    }

    if (g_frame.size() >= g_frameExpected) {
        ProcessFrame(d);
        g_frame.clear();
        g_frameExpected = 0;
    }
}

static void OnRawInput(HRAWINPUT hRaw) {
    UINT size = 0;
    GetRawInputData(hRaw, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
    if (!size)
        return;
    std::vector<BYTE> buf(size);
    if (GetRawInputData(hRaw, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER)) != size)
        return;
    RAWINPUT* ri = (RAWINPUT*)buf.data();
    if (ri->header.dwType != RIM_TYPEHID)
        return;
    HANDLE h = ri->header.hDevice;
    Dev* d = GetDev(h);
    if (!d)
        return;

    DWORD now = GetTickCount();
    if (h != g_activeDev) {
        // Another pad is in the middle of a touch. If it went quiet, it's over.
        if (g_activeDev && now - g_activeTick < kStaleFrameMs)
            return;
        if (g_activeDev) {
            Wh_Log(L"Switching to touchpad %p", h);
            ResetTouch();
        }
        g_frame.clear();
        g_frameExpected = 0;
        g_activeDev = h;
    }
    g_activeTick = now;

    DWORD sz = ri->data.hid.dwSizeHid, cnt = ri->data.hid.dwCount;
    for (DWORD i = 0; i < cnt; i++)
        ProcessReport(*d, (PCHAR)ri->data.hid.bRawData + i * sz, sz);

    // Free once the pad is idle with no frame half read and no drag waiting for
    // the fingers to come back.
    if (!g_touching && !g_frameExpected && g_twoMode != 4)
        g_activeDev = nullptr;
}

// ---------------------------------------------------------------------------
// Corner tap. The hook and WM_INPUT run on the same thread, and the hook can
// run before the report of the finger lifting has been read. So a click that
// might be a corner tap is held back, and decided once the lift has been seen.
// ---------------------------------------------------------------------------
static bool InZone(double x, double y) {
    bool inX = g_s.bottomLeft ? x <= g_s.zoneW : x >= 1.0 - g_s.zoneW;
    return inX && y >= 1.0 - g_s.zoneH;
}

static bool TapShape(DWORD length) {
    return g_s.cornerTap && g_haveSession && !g_consumed && g_maxFingers == 1 &&
           length <= g_s.maxTapMs && InZone(g_sx, g_sy) &&
           Distance(g_lx - g_sx, g_ly - g_sy) <= kMaxTapMove;
}

// Could this click still turn out to be a corner tap?
static bool MaybeCornerTap(DWORD now) {
    if (g_sessionEnded)
        return now - g_endTick <= kClickAfterLiftMs && TapShape(g_endTick - g_startTick);
    return TapShape(now - g_startTick);
}

static bool IsCornerTap() {
    return g_sessionEnded && TapShape(g_endTick - g_startTick);
}

// A click while a new touch is under way that belongs to the tap before it.
static bool PrevTapClick(DWORD now) {
    return g_haveSession && !g_sessionEnded && g_prevTap && now - g_prevEnd <= kClickAfterLiftMs;
}

// Decide the held-back click now, from what is known so far.
static void ResolvePending() {
    g_pending = false;
    KillTimer(g_hwnd, kTapTimer);
    if (!g_pendingUp) {
        g_armed = false;
        SendDown(PrimaryDown());  // a press: the real release follows through the hook
        return;
    }
    if (g_pendingPrev || IsCornerTap()) {
        DWORD tapStart = g_pendingPrev ? g_prevStart : g_startTick;
        DWORD tapEnd = g_pendingPrev ? g_prevEnd : g_endTick;
        if (g_pendingPrev)
            g_prevTap = false;
        else
            g_consumed = true;
        if (g_s.doubleTap &&
            !(g_armed && tapStart - g_armedEnd <= GetDoubleClickTime())) {
            // The first of two taps: nothing happens yet.
            g_armed = true;
            g_armedEnd = tapEnd;
            Wh_Log(L"Corner tap, waiting for a second one");
            return;
        }
        g_armed = false;
        Wh_Log(L"Corner tap -> right click");
        SendClick(SecondaryDown());
    } else {
        g_armed = false;
        SendClick(PrimaryDown());
    }
}

static void CheckPending() {
    if (!g_pending) {
        KillTimer(g_hwnd, kTapTimer);
        return;
    }
    DWORD now = GetTickCount();
    // A click judged by the previous touch has nothing left to wait for.
    bool lifted = g_pendingPrev || (g_haveSession && g_sessionEnded);

    if (!g_pendingUp) {
        // Still pressed: a press or a drag, so let it through. A tap's click
        // brings its release right behind the press.
        if (((!lifted || g_pendingPrev) && now - g_pendingTick > kLiftWaitMs) ||
            now - g_pendingTick > kPendingMaxMs)
            ResolvePending();
        return;
    }
    if (!lifted && now - g_upTick < kLiftWaitMs)
        return;
    ResolvePending();
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
    const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;
    if (nCode >= 0 && ms->dwExtraInfo != kMarker) {
        // Low-level hooks see the buttons after the swap, so WM_LBUTTON* is always
        // the primary button here. Only the SendInput side needs to know the swap.
        DWORD now = GetTickCount();

        // A lifted finger can't move the pointer, so movement after the lift means
        // another mouse is in use, and its next click isn't this tap's. The first
        // kLiftWaitMs are skipped, since the pad's last moves can trail the lift.
        if (wParam == WM_MOUSEMOVE && g_haveSession && g_sessionEnded && !g_consumed &&
            !g_pending && now - g_endTick > kLiftWaitMs) {
            if (!g_endPtSet) {
                g_endPt = ms->pt;
                g_endPtSet = true;
            } else if (std::abs(ms->pt.x - g_endPt.x) > kOtherMouseMovePx ||
                       std::abs(ms->pt.y - g_endPt.y) > kOtherMouseMovePx) {
                g_consumed = true;
            }
        }

        // During (and just after) a two-finger drag Windows still thinks the fingers
        // are scrolling or two-finger-tapping, so drop those events. A secondary
        // press always gets the same treatment as its release, so one that was
        // already let through before the drag still has its release delivered.
        bool dragging = g_twoMode == 2 || g_twoMode == 4 ||
                        (g_dragEndTick && now - g_dragEndTick < kAfterDragMs);
        if (dragging && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL))
            return 1;
        if (wParam == WM_RBUTTONDOWN) {
            g_rState = dragging ? kRDropped : kRPassed;
            if (dragging)
                return 1;
        } else if (wParam == WM_RBUTTONUP) {
            bool drop = g_rState == kRDropped;
            g_rState = kRNone;
            if (drop)
                return 1;
        }

        // With the fingers lifted mid-drag, a one-finger tap drops the item. Its
        // release is swallowed too, even if the drag carried on in between. Every
        // left press resets the flag, so a release that never arrived (a lock
        // between press and release) can't make a later click lose its release.
        if (wParam == WM_LBUTTONDOWN) {
            g_graceTapDown = g_twoMode == 4;
            if (g_graceTapDown)
                return 1;
        }
        if (wParam == WM_LBUTTONUP && g_graceTapDown) {
            g_graceTapDown = false;
            Wh_Log(L"Tap during the lift grace, dropping");
            DropLiftedDrag();
            return 1;
        }

        if (wParam == WM_LBUTTONDOWN && !g_pending) {
            bool prev = PrevTapClick(now);
            if (prev || MaybeCornerTap(now)) {
                g_pending = true;
                g_pendingUp = false;
                g_pendingPrev = prev;
                g_pendingTick = now;
                SetTimer(g_hwnd, kTapTimer, 15, nullptr);
                return 1;
            }
        }
        if (wParam == WM_LBUTTONUP && g_pending && !g_pendingUp) {
            g_pendingUp = true;
            g_upTick = now;
            return 1;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Locking the PC or going to sleep ends the touch. The release sent now can be
// lost on the way to the lock screen, so it's sent again once the desktop is back.
static void ReleaseForAway() {
    DWORD up = g_buttonHeld ? g_heldUp : 0;
    ResetTouch();
    if (up)
        OweUp(up);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawInput((HRAWINPUT)lParam);
            break;
        case WM_TIMER:
            if (wParam == kHoldTimer) {
                TryStartDrag();
                // GetTickCount is coarse, so the hold can come up a few ms short.
                if (g_twoMode == 1)
                    SetTimer(hwnd, kHoldTimer, 20, nullptr);
                else
                    KillTimer(hwnd, kHoldTimer);
            } else if (wParam == kRingTimer) {
                RingFollowCursor();
                if (!RingNeedsTimer())
                    KillTimer(hwnd, kRingTimer);
            } else if (wParam == kTapTimer) {
                CheckPending();
            } else if (wParam == kStaleTimer) {
                Wh_Log(L"Touchpad reports stopped mid-touch, resetting");
                ResetTouch();
            } else if (wParam == kReleaseTimer) {
                RetryOwedUp(false);
            } else if (wParam == kGraceTimer) {
                KillTimer(hwnd, kGraceTimer);
                DropLiftedDrag();
            } else if (wParam == kHookTimer) {
                // Otherwise the timer fires again and checks once more.
                if (!g_hook) {
                    KillTimer(hwnd, kHookTimer);
                } else if (!g_touching && !g_pending && !g_buttonHeld) {
                    KillTimer(hwnd, kHookTimer);
                    UnhookWindowsHookEx(g_hook);
                    g_hook = nullptr;
                }
            }
            return 0;
        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK)
                ReleaseForAway();
            else if (wParam == WTS_SESSION_UNLOCK)
                RetryOwedUp(true);
            break;
        case WM_POWERBROADCAST:
            if (wParam == PBT_APMSUSPEND)
                ReleaseForAway();
            else if (wParam == PBT_APMRESUMEAUTOMATIC)
                RetryOwedUp(false);
            return TRUE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void OnSettingsReloaded() {
    LoadSettings();
    if (!g_s.ring) {
        HideRing();
    } else if (g_ringShown) {
        g_ringSteady = false;  // redraw in the new color
        StartRingTimer();
    }
    if (!g_s.twoFingerDrag && (g_twoMode == 1 || g_twoMode == 2 || g_twoMode == 4)) {
        EndDrag();
        SetTwoMode(3);
    }
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
static DWORD WINAPI WorkerThread(LPVOID) {
    HMODULE mod = ThisModule();
    const wchar_t* cls = L"WH_TouchpadCornerTap";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = mod;
    wc.lpszClassName = cls;
    RegisterClassW(&wc);
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, cls, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                             nullptr, mod, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindow failed: %u", GetLastError());
        UnregisterClassW(cls, mod);
        return 0;
    }

    const wchar_t* ringCls = L"WH_TouchpadDragRing";
    WNDCLASSW rc = {};
    rc.lpfnWndProc = RingProc;
    rc.hInstance = mod;
    rc.lpszClassName = ringCls;
    RegisterClassW(&rc);
    g_ring = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                                 WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                             ringCls, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, mod,
                             nullptr);
    if (!g_ring)
        Wh_Log(L"Ring window failed: %u", GetLastError());

    RAWINPUTDEVICE rid = {kPageDigitizer, kUsageTouchPad, RIDEV_INPUTSINK, g_hwnd};
    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());

    // Lock and unlock, so a drag ends cleanly and its release isn't lost.
    bool sessionNotify = WTSRegisterSessionNotification(g_hwnd, NOTIFY_FOR_THIS_SESSION);
    if (!sessionNotify)
        Wh_Log(L"WTSRegisterSessionNotification failed: %u", GetLastError());

    Wh_Log(L"Running");

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0)
            break;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (!msg.hwnd && msg.message == kMsgReloadSettings) {
                OnSettingsReloaded();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    EndDrag();  // never leave the button stuck down
    if (g_pending) {
        g_pending = false;
        if (g_pendingUp)
            SendClick(PrimaryDown());
        else
            SendDown(PrimaryDown());
    }
    // Last chance for a release that hasn't arrived yet.
    for (DWORD up : {(DWORD)MOUSEEVENTF_LEFTUP, (DWORD)MOUSEEVENTF_RIGHTUP}) {
        if (g_owedUp & up)
            SendMouse(up);
    }
    g_owedUp = 0;
    if (sessionNotify)
        WTSUnRegisterSessionNotification(g_hwnd);
    KillTimer(g_hwnd, kHoldTimer);
    KillTimer(g_hwnd, kTapTimer);
    KillTimer(g_hwnd, kStaleTimer);
    KillTimer(g_hwnd, kHookTimer);
    KillTimer(g_hwnd, kReleaseTimer);
    KillTimer(g_hwnd, kGraceTimer);
    if (g_hook)
        UnhookWindowsHookEx(g_hook);
    g_hook = nullptr;

    RAWINPUTDEVICE rm = {kPageDigitizer, kUsageTouchPad, RIDEV_REMOVE, nullptr};
    RegisterRawInputDevices(&rm, 1, sizeof(rm));

    KillTimer(g_hwnd, kRingTimer);
    if (g_ring)
        DestroyWindow(g_ring);
    g_ring = nullptr;
    UnregisterClassW(ringCls, mod);

    DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(cls, mod);
    g_devs.clear();
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod entry points
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent)
        return FALSE;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// Settings belong to the worker thread, so it reloads them itself.
void WhTool_ModSettingsChanged() {
    if (g_thread)
        PostThreadMessageW(GetThreadId(g_thread), kMsgReloadSettings, 0, 0);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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

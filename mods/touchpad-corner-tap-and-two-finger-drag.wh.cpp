// ==WindhawkMod==
// @id              touchpad-corner-tap-and-two-finger-drag
// @name            Touchpad Corner Tap Right-Click & Two-Finger Drag
// @description     Tap a touchpad corner to right-click, and hold two fingers to drag. Works on any Precision Touchpad, including ones whose click buttons no longer work.
// @version         1.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -lhid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Touchpad Corner Tap Right-Click & Two-Finger Drag

For laptops whose touchpad no longer clicks physically (worn hinge, broken switch,
or a battery pushing the pad up), or for anyone who prefers tapping to pressing.

A quick one-finger tap in the bottom right (or bottom left) corner of the touchpad
becomes a right click. Windows only honors its right-click zone for physical presses,
so tapping there normally just left clicks.

For dragging, put two fingers down, hold still for a moment, then move them. The left
button stays held while you move, so you can select text or move windows around.
Moving two fingers straight away still scrolls like normal.

Works with any Windows Precision Touchpad (Synaptics, ELAN, Alps, Goodix, Surface and
so on). Older non-precision touchpads on vendor-only drivers don't send the raw finger
positions this mod needs.

The corner tap builds on Windows' own tap to click, so keep "Tap with a single finger
to single-click" turned on in Settings > Bluetooth & devices > Touchpad. You'll
probably want to turn off "Tap with two fingers to right-click" and "Tap twice and
drag" there though.

If you also use Touchpad Relaxed Hand Tracking, leave its two-finger option off, since
both mods would be handling the same two fingers.

## How it works
The mod reads raw finger positions from the touchpad (HID Raw Input) and, while you're
touching the pad and for a second after, watches mouse events with a low-level hook.
When Windows turns a corner tap into a left click, the
click is swapped for a right click. A resting palm is ignored, as long as the touchpad
reports it as one.

Two fingers held still start a drag that the mod drives itself, and Windows' scroll
events are dropped for the duration of the drag. Apps that scroll with the touchpad
directly instead of with scroll events (browsers, File Explorer and most newer apps)
can still scroll a little while you drag.

It runs as a Windhawk tool mod in its own background process, and doesn't inject into
or change any other program. On Windhawk 2.0 that process runs with UIAccess, so the
mod also works over windows of apps running as administrator. On Windhawk 1.x it
doesn't, so neither the corner tap nor the drag works over those windows.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerTap: true
  $name: Corner tap right-click
  $description: A quick one-finger tap in the chosen corner right-clicks
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
  $description: Longer avoids starting a drag when you rest two fingers before scrolling
- dragSpeed: 130
  $name: Drag pointer speed (%)
  $description: At 100, moving across the whole width of the touchpad moves the pointer across the whole width of the screen
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <hidusage.h>
#include <hidpi.h>

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

constexpr DWORD kClickAfterLiftMs = 800;  // a tap's click comes this soon after the lift
constexpr DWORD kLiftWaitMs = 60;         // how long a held-back click waits for the lift report
constexpr DWORD kPendingMaxMs = 1500;     // never hold a click back longer than this
constexpr DWORD kStaleFrameMs = 500;      // no report for this long means the touch is over
constexpr DWORD kStaleDragMs = 5000;      // the same during a drag, where fingers may rest still
constexpr DWORD kAfterDragMs = 600;       // Windows' leftover scroll events after a drag
constexpr DWORD kHookIdleMs = 1000;       // longer than kClickAfterLiftMs and kAfterDragMs
constexpr double kMaxTapMove = 0.04;      // fraction of the touchpad width
constexpr double kHoldMove = 0.015;

// ---------------------------------------------------------------------------
// Settings. Written on the Windhawk thread only before the worker starts, then
// reloaded on the worker thread itself.
// ---------------------------------------------------------------------------
struct Settings {
    bool cornerTap = true;
    bool bottomLeft = false;
    double zoneW = 0.5, zoneH = 0.33;
    DWORD maxTapMs = 400;
    bool twoFingerDrag = true;
    DWORD holdMs = 350;
    double dragSpeed = 1.3;
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
    s.bottomLeft =
        wcscmp(WindhawkUtils::StringSetting::make(L"corner"), L"bottomLeft") == 0;

    double ww = 0.5, wh = 0.33;
    ReadWindowsZone(&ww, &wh);
    int zw = Wh_GetIntSetting(L"zoneWidth"), zh = Wh_GetIntSetting(L"zoneHeight");
    s.zoneW = zw > 0 ? Clamp(zw, 1, 100) / 100.0 : ww;
    s.zoneH = zh > 0 ? Clamp(zh, 1, 100) / 100.0 : wh;

    s.maxTapMs = Clamp(Wh_GetIntSetting(L"maxTapMs"), 100, 1500);
    s.twoFingerDrag = Wh_GetIntSetting(L"twoFingerDrag") != 0;
    s.holdMs = Clamp(Wh_GetIntSetting(L"holdMs"), 100, 2000);
    s.dragSpeed = Clamp(Wh_GetIntSetting(L"dragSpeed"), 20, 500) / 100.0;
    g_s = s;
    Wh_Log(L"Settings: corner %s %.2fx%.2f, cornerTap %d, twoFingerDrag %d, hold %u ms",
           s.bottomLeft ? L"bottomLeft" : L"bottomRight", s.zoneW, s.zoneH, s.cornerTap,
           s.twoFingerDrag, s.holdMs);
}

// ---------------------------------------------------------------------------
// Output. SendInput's button flags name the physical buttons, so with the
// buttons swapped the primary one is the right.
// ---------------------------------------------------------------------------
static void SendMouse(DWORD flags, LONG dx = 0, LONG dy = 0) {
    INPUT in = {};
    in.type = INPUT_MOUSE;
    in.mi.dx = dx;
    in.mi.dy = dy;
    in.mi.dwFlags = flags;
    in.mi.dwExtraInfo = kMarker;
    SendInput(1, &in, sizeof(in));
}

static void SendPrimary(bool down) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    SendMouse(down ? (swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN)
                   : (swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP));
}

static void SendSecondaryClick() {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    SendMouse(swapped ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN);
    SendMouse(swapped ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP);
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
HWND g_hwnd;
HHOOK g_hook;
HANDLE g_thread, g_stopEvent;

std::vector<Contact> g_frame;
ULONG g_frameExpected;
bool g_seenConfidence;  // until the pad reports Confidence once, nothing counts as a palm
std::unordered_map<ULONG, std::pair<double, double>> g_fingers;  // this frame, palms left out
int g_touching;  // everything on the pad this frame, palms included
double g_aspect = 1;

// Corner tap session, from the first finger down to the last one up.
bool g_haveSession, g_sessionEnded, g_consumed;
DWORD g_startTick, g_endTick;
ULONG g_primaryId;
double g_sx, g_sy, g_lx, g_ly;
int g_maxFingers;

// A primary click held back until it's clear whether it was a corner tap.
bool g_pending, g_pendingUp;
DWORD g_pendingTick, g_upTick;

// Two-finger drag.
int g_twoMode;  // 0 idle, 1 holding, 2 dragging, 3 done until all fingers lift
DWORD g_twoStart, g_dragEndTick;
double g_startCx, g_startCy, g_refX, g_refY;
double g_curX, g_curY, g_scaleX, g_scaleY;  // pointer position and pixels per pad width
int g_lastCount;
bool g_buttonHeld;

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
    else if (g_twoMode == 2)
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

static void EndDrag() {
    if (!g_buttonHeld)
        return;
    g_buttonHeld = false;
    g_dragEndTick = GetTickCount();
    SendPrimary(false);
    Wh_Log(L"Two-finger drag end");
}

static void SetTwoMode(int mode) {
    if (g_twoMode == 1 && mode != 1)
        KillTimer(g_hwnd, kHoldTimer);
    g_twoMode = mode;
}

static void TryStartDrag() {
    if (g_twoMode != 1 || !g_s.twoFingerDrag || g_fingers.size() != 2)
        return;
    if (GetTickCount() - g_twoStart < g_s.holdMs)
        return;

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
    SendPrimary(true);
    g_buttonHeld = true;
    Wh_Log(L"Two-finger drag start");
}

static void UpdateTwoFinger() {
    int count = (int)g_fingers.size();
    if (count == 0) {
        EndDrag();
        SetTwoMode(0);
        return;
    }
    double cx, cy;

    if (g_twoMode == 2) {
        if (count < 2) {
            EndDrag();
            SetTwoMode(3);
            return;
        }
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
        SendCursorPos((int)std::lround(g_curX), (int)std::lround(g_curY));
        return;
    }
    if (g_twoMode == 0) {
        if (count == 2 && g_s.twoFingerDrag) {
            SetTwoMode(1);
            g_twoStart = GetTickCount();
            Centroid(&g_startCx, &g_startCy);
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
        Centroid(&cx, &cy);
        if (Distance(cx - g_startCx, cy - g_startCy) > kHoldMove) {
            SetTwoMode(3);  // moved before the hold finished, so it's a scroll
            return;
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
    if (g_haveSession && !g_sessionEnded) {
        g_sessionEnded = true;
        g_consumed = true;  // a cut-off touch is never a tap
    }
    KillTimer(g_hwnd, kStaleTimer);
    UpdateHook();
}

static void ProcessFrame(const Dev& d) {
    DWORD now = GetTickCount();
    g_aspect = d.aspect;

    for (const Contact& c : g_frame)
        g_seenConfidence = g_seenConfidence || c.conf;

    g_fingers.clear();
    g_touching = 0;
    for (const Contact& c : g_frame) {
        if (!c.tip)
            continue;
        g_touching++;
        // Confidence 0 marks a resting palm, which neither taps nor drags.
        if (c.conf || !g_seenConfidence)
            g_fingers[c.id] = {c.x, c.y};
    }

    if (!g_fingers.empty()) {
        if (!g_haveSession || g_sessionEnded) {
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
static void ProcessReport(const Dev& d, PCHAR report, ULONG len) {
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
    Dev* d = GetDev(ri->header.hDevice);
    if (!d)
        return;
    DWORD sz = ri->data.hid.dwSizeHid, cnt = ri->data.hid.dwCount;
    for (DWORD i = 0; i < cnt; i++)
        ProcessReport(*d, (PCHAR)ri->data.hid.bRawData + i * sz, sz);
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

static void CheckPending() {
    if (!g_pending) {
        KillTimer(g_hwnd, kTapTimer);
        return;
    }
    DWORD now = GetTickCount();
    bool lifted = g_haveSession && g_sessionEnded;

    if (!g_pendingUp) {
        // Still pressed with the finger down: a press or a drag, so let it through.
        if ((!lifted && now - g_pendingTick > kLiftWaitMs) ||
            now - g_pendingTick > kPendingMaxMs) {
            g_pending = false;
            KillTimer(g_hwnd, kTapTimer);
            SendPrimary(true);
        }
        return;
    }
    if (!lifted && now - g_upTick < kLiftWaitMs)
        return;

    g_pending = false;
    KillTimer(g_hwnd, kTapTimer);
    if (IsCornerTap()) {
        g_consumed = true;
        Wh_Log(L"Corner tap -> right click");
        SendSecondaryClick();
    } else {
        SendPrimary(true);
        SendPrimary(false);
    }
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && ((MSLLHOOKSTRUCT*)lParam)->dwExtraInfo != kMarker) {
        // Low-level hooks see the buttons after the swap, so WM_LBUTTON* is always
        // the primary button here. Only the SendInput side needs to know the swap.
        DWORD now = GetTickCount();

        // During (and just after) a two-finger drag Windows still thinks the fingers
        // are scrolling or two-finger-tapping, so drop those events.
        bool dragging = g_twoMode == 2 ||
                        (g_dragEndTick && now - g_dragEndTick < kAfterDragMs);
        if (dragging && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL ||
                         wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP))
            return 1;

        if (wParam == WM_LBUTTONDOWN && !g_pending && MaybeCornerTap(now)) {
            g_pending = true;
            g_pendingUp = false;
            g_pendingTick = now;
            SetTimer(g_hwnd, kTapTimer, 15, nullptr);
            return 1;
        }
        if (wParam == WM_LBUTTONUP && g_pending && !g_pendingUp) {
            g_pendingUp = true;
            g_upTick = now;
            return 1;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
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
            } else if (wParam == kTapTimer) {
                CheckPending();
            } else if (wParam == kStaleTimer) {
                Wh_Log(L"Touchpad reports stopped mid-touch, resetting");
                ResetTouch();
                g_frame.clear();
                g_frameExpected = 0;
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
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void OnSettingsReloaded() {
    LoadSettings();
    if (!g_s.twoFingerDrag && (g_twoMode == 1 || g_twoMode == 2)) {
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

    RAWINPUTDEVICE rid = {kPageDigitizer, kUsageTouchPad, RIDEV_INPUTSINK, g_hwnd};
    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());

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
        SendPrimary(true);
        if (g_pendingUp)
            SendPrimary(false);
    }
    KillTimer(g_hwnd, kHoldTimer);
    KillTimer(g_hwnd, kTapTimer);
    KillTimer(g_hwnd, kStaleTimer);
    KillTimer(g_hwnd, kHookTimer);
    if (g_hook)
        UnhookWindowsHookEx(g_hook);
    g_hook = nullptr;

    RAWINPUTDEVICE rm = {kPageDigitizer, kUsageTouchPad, RIDEV_REMOVE, nullptr};
    RegisterRawInputDevices(&rm, 1, sizeof(rm));

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

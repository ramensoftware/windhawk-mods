// ==WindhawkMod==
// @id              edge-snap-throw
// @name            Edge Snap Throw
// @description     Slide a window into a screen edge and it snaps like native Windows snap
// @version         1.6.0
// @author          getrektbynoob20
// @github          https://github.com/getrektbynoob20
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Edge Snap Throw

![Demo](https://raw.githubusercontent.com/getrektbynoob20/Edge-snap/refs/heads/main/edge-snap.gif)

Works with the Slick Window Arrangement mod. When the sliding animation
moves a window into a screen edge fast enough, it snaps using native
Windows snap.

Snap zones:
- Left or right edge: 50 percent snap
- Top edge: maximize
- Corners: 25 percent snap

Corner detection uses both position (corner zone) and throw direction.

## All settings explained

- Velocity Threshold: how fast (px/sec) the window must be moving to trigger any snap. Lower = easier to snap. Default 500.
- Corner Zone: how many pixels from a screen corner the window center must land to trigger a corner snap. Default 120.
- Diagonal Angle: how close to a perfect 45 degree angle your throw must be to count as diagonal. Lower = stricter. Default 20.
- Diagonal Min Speed: minimum speed on each axis (px/sec) for a throw to be considered diagonal. Prevents slow drifts from counting. Default 100.
- Diagonal Axis Ratio: horizontal speed must be at least this percent of vertical (and vice versa) for diagonal detection. 40 means neither axis can be less than 40 percent of the other. Default 40.
- Direction Threshold: minimum px/sec on an axis before it counts as intentional direction. Default 30.
- Snap Settle Delay: ms to wait after sending snap keys before re-enabling Snap Assist. Increase if Snap Assist still shows. Default 300.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- VelocityThreshold: 500
  $name: Velocity Threshold px per second (default 500)
- CornerZone: 120
  $name: Corner Zone Size in pixels (default 120)
- DiagonalAngle: 20
  $name: Diagonal Angle tolerance degrees (default 20)
- DiagonalMinSpeed: 100
  $name: Diagonal Min Speed per axis px per second (default 100)
- DiagonalAxisRatio: 40
  $name: Diagonal Axis Ratio percent (default 40)
- DirectionThreshold: 30
  $name: Direction Threshold px per second (default 30)
- SnapSettleDelay: 300
  $name: Snap Settle Delay milliseconds (default 300)
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windowsx.h>
#include <winreg.h>

#include <atomic>
#include <cmath>
#include <mutex>
#include <unordered_map>

struct Settings {
    int velocityThreshold;
    int cornerZone;
    int diagonalAngle;
    int diagonalMinSpeed;
    int diagonalAxisRatio;
    int directionThreshold;
    int snapSettleDelay;
} g_settings;

enum class SnapZone {
    None, Maximize, Left, Right,
    TopLeft, TopRight, BottomLeft, BottomRight,
};

// ── Velocity tracker ──────────────────────────────────────────────────────────
struct PosSnapshot { DWORD tick; int x, y; };

struct WinTrack {
    PosSnapshot history[6]{};
    int  head   = 0;
    int  count  = 0;
    bool active = false;

    void Reset() { count = 0; active = false; }

    void Push(int x, int y) {
        DWORD now = GetTickCount();
        if (count > 0 && now - history[(head + count - 1) % 6].tick > 400)
            count = 0;
        int idx = (head + count) % 6;
        history[idx] = { now, x, y };
        if (count < 6) count++;
        else           head = (head + 1) % 6;
        active = true;
    }

    bool Velocity(double* vx, double* vy) const {
        if (count < 2) return false;
        auto& last = history[(head + count - 1) % 6];
        int prevIdx = head;
        for (int i = 0; i < count - 1; i++) {
            int idx = (head + i) % 6;
            if ((int)(last.tick - history[idx].tick) <= 80) break;
            prevIdx = idx;
        }
        auto& prev = history[prevIdx];
        int dt = (int)(last.tick - prev.tick);
        if (dt <= 0) return false;
        *vx = 1000.0 * (last.x - prev.x) / dt;
        *vy = 1000.0 * (last.y - prev.y) / dt;
        return true;
    }
};

std::mutex g_trackMutex;
std::unordered_map<HWND, WinTrack> g_tracks;

// ── Helpers ───────────────────────────────────────────────────────────────────
static RECT GetWorkArea(HWND hWnd) {
    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(mon, &mi);
    return mi.rcWork;
}

static RECT GetFrameRect(HWND hWnd) {
    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc))))
        GetWindowRect(hWnd, &rc);
    return rc;
}

static bool IsDiagonal(double vx, double vy) {
    double ax = fabs(vx), ay = fabs(vy);
    double minSpeed = (double)g_settings.diagonalMinSpeed;
    double ratio    = g_settings.diagonalAxisRatio / 100.0;

    if (ax < minSpeed || ay < minSpeed) return false;
    if (ax < ay * ratio || ay < ax * ratio) return false;

    constexpr double PI = 3.14159265358979323846;
    double angle = fabs(atan2(ay, ax) * 180.0 / PI);
    return fabs(angle - 45.0) <= (double)g_settings.diagonalAngle;
}

static SnapZone ClassifyCollision(HWND hWnd, double vx, double vy) {
    double speed = sqrt(vx * vx + vy * vy);
    if (speed < g_settings.velocityThreshold)
        return SnapZone::None;

    RECT frame = GetFrameRect(hWnd);
    RECT wa    = GetWorkArea(hWnd);
    int  cz    = g_settings.cornerZone;
    int  dt    = g_settings.directionThreshold;

    bool hitLeft   = frame.left   <= wa.left;
    bool hitRight  = frame.right  >= wa.right;
    bool hitTop    = frame.top    <= wa.top;
    bool hitBottom = frame.bottom >= wa.bottom;

    if (!hitLeft && !hitRight && !hitTop && !hitBottom)
        return SnapZone::None;

    int cx = (frame.left + frame.right)  / 2;
    int cy = (frame.top  + frame.bottom) / 2;

    bool nearLeft   = (cx - wa.left)  < cz;
    bool nearRight  = (wa.right - cx) < cz;
    bool nearTop    = (cy - wa.top)   < cz;
    bool nearBottom = (wa.bottom - cy)< cz;

    bool throwLeft  = vx < -(double)dt;
    bool throwRight = vx >  (double)dt;
    bool throwUp    = vy < -(double)dt;
    bool throwDown  = vy >  (double)dt;

    bool diagonal = IsDiagonal(vx, vy);

    // Two-edge hits
    if (hitLeft  && hitTop)    return SnapZone::TopLeft;
    if (hitRight && hitTop)    return SnapZone::TopRight;
    if (hitLeft  && hitBottom) return SnapZone::BottomLeft;
    if (hitRight && hitBottom) return SnapZone::BottomRight;

    // One-edge hit + in corner zone
    if (hitTop    && nearLeft)   return SnapZone::TopLeft;
    if (hitTop    && nearRight)  return SnapZone::TopRight;
    if (hitBottom && nearLeft)   return SnapZone::BottomLeft;
    if (hitBottom && nearRight)  return SnapZone::BottomRight;
    if (hitLeft   && nearTop)    return SnapZone::TopLeft;
    if (hitLeft   && nearBottom) return SnapZone::BottomLeft;
    if (hitRight  && nearTop)    return SnapZone::TopRight;
    if (hitRight  && nearBottom) return SnapZone::BottomRight;

    // Diagonal throw direction
    if (diagonal) {
        if (hitLeft  && throwUp)    return SnapZone::TopLeft;
        if (hitLeft  && throwDown)  return SnapZone::BottomLeft;
        if (hitRight && throwUp)    return SnapZone::TopRight;
        if (hitRight && throwDown)  return SnapZone::BottomRight;
        if (hitTop   && throwLeft)  return SnapZone::TopLeft;
        if (hitTop   && throwRight) return SnapZone::TopRight;
        if (hitBottom&& throwLeft)  return SnapZone::BottomLeft;
        if (hitBottom&& throwRight) return SnapZone::BottomRight;
    }

    // Plain edges
    if (hitTop)    return SnapZone::Maximize;
    if (hitLeft)   return SnapZone::Left;
    if (hitRight)  return SnapZone::Right;

    return SnapZone::None;
}

// ── Apply snap ────────────────────────────────────────────────────────────────
static void PressWinKey(BYTE vk) {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = vk;
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = vk;    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_LWIN; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

static void SetSnapAssist(bool enabled) {
    DWORD val = enabled ? 1 : 0;
    RegSetKeyValueW(HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"SnapAssist", REG_DWORD, &val, sizeof(val));
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
        (LPARAM)L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
}

static void SnapToQuarter(HWND hWnd, SnapZone zone) {
    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(mon, &mi);
    RECT wa = mi.rcWork;

    int halfW = (wa.right  - wa.left) / 2;
    int halfH = (wa.bottom - wa.top)  / 2;

    int x = wa.left, y = wa.top;
    switch (zone) {
    case SnapZone::TopLeft:     x = wa.left;        y = wa.top;         break;
    case SnapZone::TopRight:    x = wa.left + halfW; y = wa.top;         break;
    case SnapZone::BottomLeft:  x = wa.left;        y = wa.top + halfH; break;
    case SnapZone::BottomRight: x = wa.left + halfW; y = wa.top + halfH; break;
    default: return;
    }

    ShowWindow(hWnd, SW_RESTORE);
    Sleep(30);
    SetWindowPos(hWnd, nullptr, x, y, halfW, halfH,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void ApplySnap(HWND hWnd, SnapZone zone) {
    SetForegroundWindow(hWnd);
    Sleep(20);

    // Corners: direct SetWindowPos — no snap engine, no Snap Assist popup
    if (zone == SnapZone::TopLeft    || zone == SnapZone::TopRight ||
        zone == SnapZone::BottomLeft || zone == SnapZone::BottomRight) {
        SnapToQuarter(hWnd, zone);
        return;
    }

    // Halves + maximize: suppress Snap Assist around the key send
    SetSnapAssist(false);
    switch (zone) {
    case SnapZone::Maximize: ShowWindow(hWnd, SW_MAXIMIZE); break;
    case SnapZone::Left:     PressWinKey(VK_LEFT);          break;
    case SnapZone::Right:    PressWinKey(VK_RIGHT);         break;
    default: break;
    }
    Sleep(g_settings.snapSettleDelay);
    SetSnapAssist(true);
}

struct SnapThreadParam { HWND hWnd; SnapZone zone; };
static DWORD WINAPI SnapThreadProc(LPVOID p) {
    auto* sp = reinterpret_cast<SnapThreadParam*>(p);
    ApplySnap(sp->hWnd, sp->zone);
    { std::lock_guard<std::mutex> lock(g_trackMutex); g_tracks.erase(sp->hWnd); }
    delete sp;
    return 0;
}

// ── Hook SetWindowPos ─────────────────────────────────────────────────────────
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t pOrigSetWindowPos;

BOOL WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags)
{
    const UINT slideFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

    if ((uFlags & slideFlags) == slideFlags && !(uFlags & SWP_NOMOVE)) {
        LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_TOOLWINDOW) && IsWindowVisible(hWnd)) {
            std::lock_guard<std::mutex> lock(g_trackMutex);
            auto& track = g_tracks[hWnd];

            if (track.count > 0) {
                track.Push(X, Y);
                double vx = 0, vy = 0;
                if (track.Velocity(&vx, &vy)) {
                    SnapZone zone = ClassifyCollision(hWnd, vx, vy);
                    if (zone != SnapZone::None) {
                        track.Reset();
                        auto* param = new SnapThreadParam{ hWnd, zone };
                        CreateThread(nullptr, 0, SnapThreadProc, param, 0, nullptr);
                        return TRUE;
                    }
                }
            } else {
                track.Push(X, Y);
            }
        }
    } else {
        std::lock_guard<std::mutex> lock(g_trackMutex);
        auto it = g_tracks.find(hWnd);
        if (it != g_tracks.end()) it->second.Reset();
    }

    return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ── Windhawk lifecycle ────────────────────────────────────────────────────────
void LoadSettings() {
    g_settings.velocityThreshold  = Wh_GetIntSetting(L"VelocityThreshold");
    g_settings.cornerZone         = Wh_GetIntSetting(L"CornerZone");
    g_settings.diagonalAngle      = Wh_GetIntSetting(L"DiagonalAngle");
    g_settings.diagonalMinSpeed   = Wh_GetIntSetting(L"DiagonalMinSpeed");
    g_settings.diagonalAxisRatio  = Wh_GetIntSetting(L"DiagonalAxisRatio");
    g_settings.directionThreshold = Wh_GetIntSetting(L"DirectionThreshold");
    g_settings.snapSettleDelay    = Wh_GetIntSetting(L"SnapSettleDelay");

    if (g_settings.velocityThreshold  < 1)   g_settings.velocityThreshold  = 500;
    if (g_settings.cornerZone         < 0)   g_settings.cornerZone         = 120;
    if (g_settings.diagonalAngle      < 1)   g_settings.diagonalAngle      = 20;
    if (g_settings.diagonalAngle      > 44)  g_settings.diagonalAngle      = 44;
    if (g_settings.diagonalMinSpeed   < 0)   g_settings.diagonalMinSpeed   = 100;
    if (g_settings.diagonalAxisRatio  < 0)   g_settings.diagonalAxisRatio  = 40;
    if (g_settings.diagonalAxisRatio  > 99)  g_settings.diagonalAxisRatio  = 99;
    if (g_settings.directionThreshold < 0)   g_settings.directionThreshold = 30;
    if (g_settings.snapSettleDelay    < 0)   g_settings.snapSettleDelay    = 300;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Edge Snap Throw: Init");
    LoadSettings();
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPosHook, (void**)&pOrigSetWindowPos);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Edge Snap Throw: Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Edge Snap Throw: Settings changed");
    LoadSettings();
}

// ==WindhawkMod==
// @id              edge-snap-throw
// @name            Edge Snap Throw
// @description     Slide a window into a screen edge and it snaps like native Windows snap
// @version         2.0.1
// @author          Getrektbynoob15
// @github          https://github.com/getrektbynoob15
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Edge Snap Throw

![Demo](https://raw.githubusercontent.com/getrektbynoob20/Edge-snap/refs/heads/main/edge-snap2.0.gif)

Works best with **[Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)** by m417z.
Install that mod first. It adds sliding momentum when you release a dragged window.
This mod detects when that sliding window hits a screen edge fast enough and snaps it.

Snap zones:
- Left or right edge: 50 percent snap
- Top edge: maximize
- Corners: 25 percent snap

When you drag a snapped window back out, it restores to its original size (toggle in settings).

## Settings

- Velocity Threshold: how fast the window must be moving to trigger a snap. Default 500.
- Corner Zone: pixels from a corner that count as a corner hit. Default 120.
- Diagonal Angle: how close to 45 degrees a throw must be to count as diagonal. Default 20.
- Diagonal Min Speed: minimum speed per axis for diagonal detection. Default 100.
- Diagonal Axis Ratio: how balanced both axes must be for diagonal detection. Default 40.
- Direction Threshold: minimum speed before a direction counts as intentional. Default 30.
- Restore On Drag: when on, dragging a snapped window restores its original size. Default on.
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
- RestoreOnDrag: true
  $name: Restore original size when dragging out of snap (default on)
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windowsx.h>

#include <cmath>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

// ── Settings ──────────────────────────────────────────────────────────────────
struct Settings {
    int  velocityThreshold;
    int  cornerZone;
    int  diagonalAngle;
    int  diagonalMinSpeed;
    int  diagonalAxisRatio;
    int  directionThreshold;
    bool restoreOnDrag;
} g_settings;

// ── Snap zone ─────────────────────────────────────────────────────────────────
enum class SnapZone {
    None, Maximize, Left, Right,
    TopLeft, TopRight, BottomLeft, BottomRight,
};

// ── Velocity tracker (thread_local — no mutex needed) ─────────────────────────
struct PosSnapshot { DWORD tick; int x, y; };

struct WinTrack {
    PosSnapshot history[6]{};
    int head  = 0;
    int count = 0;

    void Reset() { count = 0; }

    void Push(int x, int y) {
        DWORD now = GetTickCount();
        if (count > 0 && now - history[(head + count - 1) % 6].tick > 400)
            count = 0;
        int idx = (head + count) % 6;
        history[idx] = { now, x, y };
        if (count < 6) count++;
        else           head = (head + 1) % 6;
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

// thread_local: each UI thread has its own map, no mutex needed
thread_local std::unordered_map<HWND, WinTrack> g_tracks;

// Timer-based snap: maps timer ID -> {hWnd, zone}, also thread_local
thread_local std::unordered_map<UINT_PTR, std::pair<HWND, SnapZone>> g_pendingSnaps;

// ── SetWindowPos hook (forward declared for RestoreSubclassProc) ──────────────
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t pOrigSetWindowPos;

// ── Restore-on-drag ───────────────────────────────────────────────────────────
struct SnapRestore {
    RECT originalRect;
    bool restored;
};

std::mutex g_restoreMutex;
std::unordered_map<HWND, SnapRestore> g_restoreMap;

// Track subclassed windows so we can clean them up on unload
std::mutex g_subclassedMutex;
std::unordered_set<HWND> g_subclassedWindows;

UINT g_unsubclassMsg = RegisterWindowMessage(L"Windhawk_Unsubclass_restore_edge-snap-throw");

LRESULT CALLBACK RestoreSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto cleanup = [&]() {
        RemoveWindowSubclass(hWnd, RestoreSubclassProc, 0);
        {
            std::lock_guard<std::mutex> lock(g_subclassedMutex);
            g_subclassedWindows.erase(hWnd);
        }
        {
            std::lock_guard<std::mutex> lock(g_restoreMutex);
            g_restoreMap.erase(hWnd);
        }
    };

    if (uMsg == WM_MOVING) {
        bool didRestore = false;
        {
            std::lock_guard<std::mutex> lock(g_restoreMutex);
            auto it = g_restoreMap.find(hWnd);
            if (it != g_restoreMap.end() && !it->second.restored) {
                it->second.restored = true;

                RECT& orig = it->second.originalRect;
                int w = orig.right  - orig.left;
                int h = orig.bottom - orig.top;

                POINT cursor;
                GetCursorPos(&cursor);

                RECT* r = (RECT*)lParam;
                r->left   = cursor.x - w / 2;
                r->top    = cursor.y - 10;
                r->right  = r->left + w;
                r->bottom = r->top  + h;

                pOrigSetWindowPos(hWnd, nullptr, r->left, r->top, w, h,
                    SWP_NOZORDER | SWP_NOACTIVATE);

                didRestore = true;
            }
        }
        if (didRestore) {
            // Remove subclass immediately — job is done
            cleanup();
            return TRUE;
        }
    }
    else if (uMsg == WM_NCDESTROY) {
        cleanup();
    }
    else if (uMsg == g_unsubclassMsg) {
        // Sent by Wh_ModUninit to cleanly remove subclass on unload
        cleanup();
        return 0;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

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
    if (speed < g_settings.velocityThreshold) return SnapZone::None;

    RECT frame = GetFrameRect(hWnd);
    RECT wa    = GetWorkArea(hWnd);
    int  cz    = g_settings.cornerZone;
    int  dt    = g_settings.directionThreshold;

    bool hitLeft   = frame.left   <= wa.left;
    bool hitRight  = frame.right  >= wa.right;
    bool hitTop    = frame.top    <= wa.top;
    bool hitBottom = frame.bottom >= wa.bottom;

    if (!hitLeft && !hitRight && !hitTop && !hitBottom) return SnapZone::None;

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
    bool diagonal   = IsDiagonal(vx, vy);

    if (hitLeft  && hitTop)    return SnapZone::TopLeft;
    if (hitRight && hitTop)    return SnapZone::TopRight;
    if (hitLeft  && hitBottom) return SnapZone::BottomLeft;
    if (hitRight && hitBottom) return SnapZone::BottomRight;

    if (hitTop    && nearLeft)   return SnapZone::TopLeft;
    if (hitTop    && nearRight)  return SnapZone::TopRight;
    if (hitBottom && nearLeft)   return SnapZone::BottomLeft;
    if (hitBottom && nearRight)  return SnapZone::BottomRight;
    if (hitLeft   && nearTop)    return SnapZone::TopLeft;
    if (hitLeft   && nearBottom) return SnapZone::BottomLeft;
    if (hitRight  && nearTop)    return SnapZone::TopRight;
    if (hitRight  && nearBottom) return SnapZone::BottomRight;

    if (diagonal) {
        if (hitLeft   && throwUp)    return SnapZone::TopLeft;
        if (hitLeft   && throwDown)  return SnapZone::BottomLeft;
        if (hitRight  && throwUp)    return SnapZone::TopRight;
        if (hitRight  && throwDown)  return SnapZone::BottomRight;
        if (hitTop    && throwLeft)  return SnapZone::TopLeft;
        if (hitTop    && throwRight) return SnapZone::TopRight;
        if (hitBottom && throwLeft)  return SnapZone::BottomLeft;
        if (hitBottom && throwRight) return SnapZone::BottomRight;
    }

    if (hitTop)   return SnapZone::Maximize;
    if (hitLeft)  return SnapZone::Left;
    if (hitRight) return SnapZone::Right;

    return SnapZone::None;
}

// ── Apply snap ────────────────────────────────────────────────────────────────
static void SnapWindow(HWND hWnd, SnapZone zone) {
    HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(mon, &mi);
    RECT wa = mi.rcWork;

    int fullW = wa.right  - wa.left;
    int fullH = wa.bottom - wa.top;
    int halfW = fullW / 2;
    int halfH = fullH / 2;

    int x = wa.left, y = wa.top, w = fullW, h = fullH;

    switch (zone) {
    case SnapZone::Maximize:
        ShowWindow(hWnd, SW_MAXIMIZE);
        return;
    case SnapZone::Left:        x = wa.left;        y = wa.top;         w = halfW; h = fullH; break;
    case SnapZone::Right:       x = wa.left + halfW; y = wa.top;         w = halfW; h = fullH; break;
    case SnapZone::TopLeft:     x = wa.left;        y = wa.top;         w = halfW; h = halfH; break;
    case SnapZone::TopRight:    x = wa.left + halfW; y = wa.top;         w = halfW; h = halfH; break;
    case SnapZone::BottomLeft:  x = wa.left;        y = wa.top + halfH; w = halfW; h = halfH; break;
    case SnapZone::BottomRight: x = wa.left + halfW; y = wa.top + halfH; w = halfW; h = halfH; break;
    default: return;
    }

    ShowWindow(hWnd, SW_RESTORE);
    pOrigSetWindowPos(hWnd, nullptr, x, y, w, h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

// ── Timer-based snap (runs on UI thread — no CreateThread needed) ─────────────
void CALLBACK SnapTimerProc(HWND, UINT, UINT_PTR idTimer, DWORD) {
    KillTimer(nullptr, idTimer);

    auto it = g_pendingSnaps.find(idTimer);
    if (it == g_pendingSnaps.end()) return;

    HWND     hWnd = it->second.first;
    SnapZone zone = it->second.second;
    g_pendingSnaps.erase(it);

    SnapWindow(hWnd, zone);
}

// ── Hook SetWindowPos ─────────────────────────────────────────────────────────
BOOL WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags)
{
    // Verify the caller is slick-window-arrangement by checking which module
    // the return address belongs to. _ReturnAddress() must be called directly
    // in the hook function — it reads the actual stack return address.
    void* callerAddr = __builtin_return_address(0);
    {
        HMODULE callerMod = nullptr;
        if (GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCWSTR)callerAddr, &callerMod) && callerMod) {
            WCHAR modPath[MAX_PATH];
            if (GetModuleFileNameW(callerMod, modPath, MAX_PATH)) {
                if (!wcsstr(modPath, L"slick-window-arrangement")) {
                    return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
                }
            }
        } else {
            return pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
        }
    }

    const UINT slideFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

    if ((uFlags & slideFlags) == slideFlags && !(uFlags & SWP_NOMOVE)) {
        LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_TOOLWINDOW) && IsWindowVisible(hWnd)) {
            auto& track = g_tracks[hWnd];

            if (track.count > 0) {
                track.Push(X, Y);
                double vx = 0, vy = 0;
                if (track.Velocity(&vx, &vy)) {
                    SnapZone zone = ClassifyCollision(hWnd, vx, vy);
                    if (zone != SnapZone::None) {
                        track.Reset();

                        // Save original size and subclass for restore-on-drag.
                        // Safe to call SetWindowSubclass here — we are on the
                        // window's own UI thread (slide timer runs there).
                        if (g_settings.restoreOnDrag && zone != SnapZone::Maximize) {
                            RECT rc{};
                            GetWindowRect(hWnd, &rc);
                            {
                                std::lock_guard<std::mutex> lock(g_restoreMutex);
                                g_restoreMap[hWnd] = { rc, false };
                            }
                            if (SetWindowSubclass(hWnd, RestoreSubclassProc, 0, 0)) {
                                std::lock_guard<std::mutex> lock(g_subclassedMutex);
                                g_subclassedWindows.insert(hWnd);
                            }
                        }

                        // Use SetTimer instead of CreateThread so snap runs on
                        // the UI thread and g_tracks stays thread_local.
                        UINT_PTR timerId = SetTimer(nullptr, 0, 1, SnapTimerProc);
                        g_pendingSnaps[timerId] = { hWnd, zone };

                        return TRUE; // stop the slide
                    }
                }
            } else {
                track.Push(X, Y);
            }
        }
    } else {
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
    g_settings.restoreOnDrag      = Wh_GetIntSetting(L"RestoreOnDrag") != 0;

    if (g_settings.velocityThreshold  < 1)  g_settings.velocityThreshold  = 500;
    if (g_settings.cornerZone         < 0)  g_settings.cornerZone         = 120;
    if (g_settings.diagonalAngle      < 1)  g_settings.diagonalAngle      = 20;
    if (g_settings.diagonalAngle      > 44) g_settings.diagonalAngle      = 44;
    if (g_settings.diagonalMinSpeed   < 0)  g_settings.diagonalMinSpeed   = 100;
    if (g_settings.diagonalAxisRatio  < 0)  g_settings.diagonalAxisRatio  = 40;
    if (g_settings.diagonalAxisRatio  > 99) g_settings.diagonalAxisRatio  = 99;
    if (g_settings.directionThreshold < 0)  g_settings.directionThreshold = 30;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Edge Snap Throw: Init");
    LoadSettings();
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPosHook, (void**)&pOrigSetWindowPos);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Edge Snap Throw: Uninit");

    // Remove all restore subclasses so the mod unloads cleanly without crashes
    std::unordered_set<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_subclassedMutex);
        windows = g_subclassedWindows;
    }
    for (HWND hWnd : windows) {
        SendMessage(hWnd, g_unsubclassMsg, 0, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Edge Snap Throw: Settings changed");
    LoadSettings();
}

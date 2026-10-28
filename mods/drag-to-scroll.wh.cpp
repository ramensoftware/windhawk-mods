// ==WindhawkMod==
// @id              drag-to-scroll
// @name            Drag to Scroll
// @description     Hold Ctrl + Right-Click and drag to scroll smoothly in desktop applications with adaptive line precision, dynamic velocity, and hands-free cruising.
// @version         1.1.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @license         MIT
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Drag to Scroll

Hold **`Ctrl` + Right-Click** and drag your mouse to scroll smoothly and naturally across desktop applications, including web browsers (Chrome, Edge, Firefox), text editors (VS Code, Notepad++), File Explorer, and legacy Win32 utilities.

![Drag to Scroll Demo 1](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/dts_preview_1.webp)

![Drag to Scroll Demo 2](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/dts_preview_2.webp)

## Key Features

* **Full 2D Canvas & Horizontal Scrolling:** Supports natural sideways panning with directional snapping (axis-lock) to prevent diagonal drift.
* **High-Resolution Micro-Scrolling:** Injects fractional wheel increments for fluid, trackpad-like scrolling in modern desktop apps and web browsers.
* **Legacy Control Detection:** Automatically detects classic Win32 controls (e.g., standard list views, tree views, edit boxes) and routes discrete `WM_VSCROLL` and `WM_HSCROLL` line steps to preserve native line-by-line movement.
* **Hands-Free Cruising:** Release the Right Mouse Button while continuing to hold `Ctrl` to let the page cruise automatically based on cursor offset along the active axis. Cruising disengages instantly on keypress, mouse click, or `Ctrl` release.
* **Zero Zoom Interference:** Dynamically masks `Ctrl` state during scroll dispatch so applications never accidentally trigger browser zoom or editor scaling.
* **Accidental Click Protection:** Tapping `Ctrl + Right-Click` in place without exceeding the deadzone transparently passes through as a regular right-click.

## Compatibility & Limitations

* **Packaged & Modern UI Surfaces:** Certain modern Windows interfaces (such as the Windows Start Menu or some packaged store apps) do not process standard Win32 mouse-drag messages through the message queue and are not supported.
* **CAD & 3D Software:** If you use software (such as Blender, AutoCAD, or video games) that natively binds actions to `Ctrl + Right-Click`, you can exclude those executables under the mod's **Advanced** tab in Windhawk.

---

### 🧩 More from this author
* **[Explorer Custom Shortcuts](https://windhawk.net/mods/explorer-custom-shortcuts)** – Customizable keyboard shortcuts for File Explorer: open terminal, copy paths, create files/folders, toggle hidden items, and launch apps with hotkeys.

### Author & Attribution

* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- touch_style: true
  $name: "Natural / Touchscreen Scrolling"
  $description: "When enabled, dragging down pulls the content down (touchscreen style). When disabled, dragging follows traditional mouse wheel direction."
- enable_horizontal: true
  $name: "Enable Horizontal Scrolling"
  $description: "Allows dragging horizontally to scroll sideways using horizontal wheel events and WM_HSCROLL."
- axis_lock: true
  $name: "Prevent Diagonal Drift (Axis Lock)"
  $description: "Locks scrolling straight vertically or horizontally to prevent accidental drifting in the other direction. Disable for free 2D canvas panning."
- enable_cruising: false
  $name: "Continuous Cruising (Hold Ctrl)"
  $description: "Releasing Right-Click while holding Ctrl keeps scrolling automatically based on displacement. Stops immediately on any click, keypress, or Ctrl release."
- pixels_per_notch: 35
  $name: "Base Drag Distance (Pixels per Notch)"
  $description: "Pixels required to drag for one full standard scroll notch (120 units). Lower values scroll faster (minimum: 10)."
- deadzone: 5
  $name: "Drag Deadzone (Pixels)"
  $description: "Movement threshold in pixels before drag-scrolling engages. Releasing within this threshold passes through as a standard right-click (minimum: 0)."
- enable_velocity: true
  $name: "Dynamic Velocity Scaling"
  $description: "Slow, small movements offer high line-by-line precision; rapid flicks accelerate scrolling smoothly."
- smooth_wheel: true
  $name: "High-Resolution Smooth Scrolling"
  $description: "Injects high-precision fractional wheel deltas for fluid movement in modern browsers. Disable this if scrolling is unresponsive in older desktop programs."
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>

constexpr ULONG_PTR MASK_EXTRA_INFO = 0x5343524C;    // Tag: 'SCRL'
constexpr ULONG_PTR REPLAY_EXTRA_INFO = 0x5245504C;  // Tag: 'REPL'
constexpr int SMOOTH_SCROLL_CHUNK = 15;

struct ModSettings {
    bool touch_style = true;
    bool enable_cruising = false;
    double pixels_per_notch = 35.0;
    int deadzone = 5;
    bool enable_velocity = true;
    bool smooth_wheel = true;
    bool enable_horizontal = true;
    bool axis_lock = true;
};

static ModSettings g_settings;

enum class ScrollState { Idle, Pending, Scrolling, Cruising };
static std::atomic<ScrollState> g_state{ScrollState::Idle};
static std::atomic<bool> g_swallowRButtonUp{false};
static std::atomic<UINT> g_swallowButtonUp{0};

enum class DragAxis { Free, VerticalOnly, HorizontalOnly };
static DragAxis g_activeAxis = DragAxis::Free;

static POINT g_anchorPt = {0, 0};
static POINT g_lastPt = {0, 0};
static double g_accumY = 0.0;
static double g_accumX = 0.0;
static RECT g_windowBounds = {0, 0, 0, 0};
static HWND g_captureHwnd = nullptr;

static HANDLE g_hCruiseThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_cruiseEvent = nullptr;
static std::atomic<bool> g_threadCreated{false};

struct CruiseContext {
    POINT anchorPt;
    RECT windowBounds;
    double pixels_per_notch;
    int deadzone;
    bool touch_style;
    bool smooth_wheel;
    bool enable_horizontal;
    DragAxis axis;
};
static CruiseContext g_cruiseCtx;
static CRITICAL_SECTION g_cruiseCs;

// Forward declare the original API pointer so the mod can check real hardware
// key states
using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
static GetAsyncKeyState_t pfnGetAsyncKeyState_Original = nullptr;

// Queries physical key state directly, bypassing our own hook that masks Ctrl
static inline bool IsCtrlPhysicallyDown() {
    return (pfnGetAsyncKeyState_Original(VK_CONTROL) & 0x8000) != 0;
}

void LoadSettings() {
    g_settings.touch_style = Wh_GetIntSetting(L"touch_style") != 0;
    g_settings.enable_cruising = Wh_GetIntSetting(L"enable_cruising") != 0;
    g_settings.pixels_per_notch = static_cast<double>(
        std::max(10, Wh_GetIntSetting(L"pixels_per_notch")));
    g_settings.deadzone = std::clamp(Wh_GetIntSetting(L"deadzone"), 0, 1000);
    g_settings.enable_velocity = Wh_GetIntSetting(L"enable_velocity") != 0;
    g_settings.smooth_wheel = Wh_GetIntSetting(L"smooth_wheel") != 0;
    g_settings.enable_horizontal = Wh_GetIntSetting(L"enable_horizontal") != 0;
    g_settings.axis_lock = Wh_GetIntSetting(L"axis_lock") != 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void SendMaskedWheelAsync(int deltaY) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = static_cast<DWORD>(deltaY);
    input.mi.dwExtraInfo = MASK_EXTRA_INFO;
    SendInput(1, &input, sizeof(INPUT));
}

void SendMaskedHWheelAsync(int deltaX) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    input.mi.mouseData = static_cast<DWORD>(deltaX);
    input.mi.dwExtraInfo = MASK_EXTRA_INFO;
    SendInput(1, &input, sizeof(INPUT));
}

bool GetScrollTargetInfo(POINT pt, HWND& outTarget) {
    outTarget = WindowFromPoint(pt);
    if (!outTarget)
        return false;

    // Do not post messages to windows belonging to another process
    DWORD targetPid = 0;
    GetWindowThreadProcessId(outTarget, &targetPid);
    if (targetPid != GetCurrentProcessId()) {
        outTarget = nullptr;
        return false;
    }

    WCHAR cls[64] = {};
    GetClassNameW(outTarget, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"SysHeader32") == 0) {
        HWND hParent = GetParent(outTarget);
        if (hParent) {
            outTarget = hParent;
            GetClassNameW(outTarget, cls, ARRAYSIZE(cls));
        }
    }

    WCHAR lowerCls[64] = {};
    for (int i = 0; cls[i] && i < 63; ++i) {
        lowerCls[i] = static_cast<WCHAR>(towlower(cls[i]));
    }

    return (_wcsicmp(cls, L"SysListView32") == 0 ||
            _wcsicmp(cls, L"SysTreeView32") == 0 ||
            _wcsicmp(cls, L"Edit") == 0 || _wcsicmp(cls, L"ListBox") == 0 ||
            wcsstr(lowerCls, L"richedit") != nullptr);
}

void DispatchScroll(int deltaY, HWND hTarget, bool isDiscrete) {
    if (isDiscrete && hTarget) {
        UINT scrollLines = 3;
        SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);

        if (scrollLines == WHEEL_PAGESCROLL) {
            PostMessageW(hTarget, WM_VSCROLL,
                         (deltaY > 0) ? SB_PAGEUP : SB_PAGEDOWN, 0);
            PostMessageW(hTarget, WM_VSCROLL, SB_ENDSCROLL, 0);
            return;
        }

        int notches = std::max(1, std::abs(deltaY) / WHEEL_DELTA);
        int totalSteps = notches * static_cast<int>(scrollLines);
        WPARAM scrollCmd = (deltaY > 0) ? SB_LINEUP : SB_LINEDOWN;
        for (int i = 0; i < totalSteps; ++i) {
            PostMessageW(hTarget, WM_VSCROLL, scrollCmd, 0);
        }
        PostMessageW(hTarget, WM_VSCROLL, SB_ENDSCROLL, 0);
        return;
    }
    SendMaskedWheelAsync(deltaY);
}

void DispatchHScroll(int deltaX, HWND hTarget, bool isDiscrete) {
    if (isDiscrete && hTarget) {
        UINT scrollChars = 3;
        SystemParametersInfoW(SPI_GETWHEELSCROLLCHARS, 0, &scrollChars, 0);

        int notches = std::max(1, std::abs(deltaX) / WHEEL_DELTA);
        int totalSteps = notches * static_cast<int>(scrollChars);
        WPARAM scrollCmd = (deltaX > 0) ? SB_LINERIGHT : SB_LINELEFT;
        for (int i = 0; i < totalSteps; ++i) {
            PostMessageW(hTarget, WM_HSCROLL, scrollCmd, 0);
        }
        PostMessageW(hTarget, WM_HSCROLL, SB_ENDSCROLL, 0);
        return;
    }
    SendMaskedHWheelAsync(deltaX);
}

DWORD WINAPI CruiseWorkerThread(LPVOID) {
    HANDLE handles[] = {g_stopEvent, g_cruiseEvent};
    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_FAILED)
            return 0;

        double cruiseAccumY = 0.0;
        double cruiseAccumX = 0.0;
        EnterCriticalSection(&g_cruiseCs);
        CruiseContext ctx = g_cruiseCtx;
        LeaveCriticalSection(&g_cruiseCs);

        while (g_state.load() == ScrollState::Cruising) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0)
                return 0;

            // Check physical hardware Ctrl state via unhooked call
            if (!IsCtrlPhysicallyDown()) {
                g_state.store(ScrollState::Idle);
                break;
            }

            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&ctx.windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                break;
            }

            int distX = curPt.x - ctx.anchorPt.x;
            int distY = curPt.y - ctx.anchorPt.y;

            // Zero out axes according to the locked axis
            if (ctx.axis == DragAxis::VerticalOnly)
                distX = 0;
            if (ctx.axis == DragAxis::HorizontalOnly)
                distY = 0;

            HWND hTarget = nullptr;
            bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);
            int chunk = ctx.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;

            // --- Vertical Cruising ---
            if (std::abs(distY) >= ctx.deadzone) {
                double directionY = ctx.touch_style ? 1.0 : -1.0;
                double notchRateY =
                    (static_cast<double>(distY) / ctx.pixels_per_notch);
                double unitsThisTickY =
                    notchRateY * directionY * WHEEL_DELTA * 0.035;

                cruiseAccumY += unitsThisTickY;

                if (isDiscrete) {
                    int notches = 0;
                    while (cruiseAccumY <= -WHEEL_DELTA) {
                        notches -= WHEEL_DELTA;
                        cruiseAccumY += WHEEL_DELTA;
                    }
                    while (cruiseAccumY >= WHEEL_DELTA) {
                        notches += WHEEL_DELTA;
                        cruiseAccumY -= WHEEL_DELTA;
                    }
                    if (notches != 0)
                        DispatchScroll(notches, hTarget, true);
                } else {
                    int emitDelta = 0;
                    while (cruiseAccumY <= -chunk) {
                        emitDelta -= chunk;
                        cruiseAccumY += chunk;
                    }
                    while (cruiseAccumY >= chunk) {
                        emitDelta += chunk;
                        cruiseAccumY -= chunk;
                    }
                    if (emitDelta != 0)
                        DispatchScroll(emitDelta, hTarget, false);
                }
            }

            // --- Horizontal Cruising ---
            if (ctx.enable_horizontal && std::abs(distX) >= ctx.deadzone) {
                // Inverted direction compared to Y to match pull-canvas logic
                double directionX = ctx.touch_style ? -1.0 : 1.0;
                double notchRateX =
                    (static_cast<double>(distX) / ctx.pixels_per_notch);
                double unitsThisTickX =
                    notchRateX * directionX * WHEEL_DELTA * 0.035;

                cruiseAccumX += unitsThisTickX;

                if (isDiscrete) {
                    int notches = 0;
                    while (cruiseAccumX <= -WHEEL_DELTA) {
                        notches -= WHEEL_DELTA;
                        cruiseAccumX += WHEEL_DELTA;
                    }
                    while (cruiseAccumX >= WHEEL_DELTA) {
                        notches += WHEEL_DELTA;
                        cruiseAccumX -= WHEEL_DELTA;
                    }
                    if (notches != 0)
                        DispatchHScroll(notches, hTarget, true);
                } else {
                    int emitDelta = 0;
                    while (cruiseAccumX <= -chunk) {
                        emitDelta -= chunk;
                        cruiseAccumX += chunk;
                    }
                    while (cruiseAccumX >= chunk) {
                        emitDelta += chunk;
                        cruiseAccumX -= chunk;
                    }
                    if (emitDelta != 0)
                        DispatchHScroll(emitDelta, hTarget, false);
                }
            }
            if (WaitForSingleObject(g_stopEvent, 20) == WAIT_OBJECT_0)
                return 0;
        }
    }
    return 0;
}

void StartCruiseThreadIfNeeded() {
    EnterCriticalSection(&g_cruiseCs);
    g_cruiseCtx.anchorPt = g_anchorPt;
    g_cruiseCtx.windowBounds = g_windowBounds;
    g_cruiseCtx.pixels_per_notch = g_settings.pixels_per_notch;
    g_cruiseCtx.deadzone = g_settings.deadzone;
    g_cruiseCtx.touch_style = g_settings.touch_style;
    g_cruiseCtx.smooth_wheel = g_settings.smooth_wheel;
    g_cruiseCtx.enable_horizontal = g_settings.enable_horizontal;
    g_cruiseCtx.axis = g_activeAxis;
    LeaveCriticalSection(&g_cruiseCs);

    bool expected = false;
    if (g_threadCreated.compare_exchange_strong(expected, true)) {
        g_hCruiseThread =
            CreateThread(nullptr, 0, CruiseWorkerThread, nullptr, 0, nullptr);
    }
    if (g_cruiseEvent)
        SetEvent(g_cruiseEvent);
}

bool ProcessScrollInput(MSG* pMsg, bool canMutate) {
    if (!pMsg || !pMsg->hwnd)
        return false;

    // Fast reject: only mouse and keyboard messages are processed
    UINT m = pMsg->message;
    if (!(m >= WM_MOUSEFIRST && m <= WM_MOUSELAST) &&
        !(m >= WM_KEYFIRST && m <= WM_KEYLAST)) {
        return false;
    }

    ULONG_PTR extra = GetMessageExtraInfo();
    if (extra == REPLAY_EXTRA_INFO)
        return false;

    // Reset swallow flags on any explicit user button-down action
    if (m == WM_LBUTTONDOWN || m == WM_LBUTTONDBLCLK || m == WM_RBUTTONDOWN ||
        m == WM_RBUTTONDBLCLK || m == WM_MBUTTONDOWN || m == WM_MBUTTONDBLCLK ||
        m == WM_XBUTTONDOWN || m == WM_XBUTTONDBLCLK) {
        g_swallowButtonUp.store(0);
    }

    // Swallow button-up message when cruising was stopped by a click
    if (g_swallowButtonUp.load() != 0 && m == g_swallowButtonUp.load()) {
        g_swallowButtonUp.store(0);
        return true;
    }

    // Clear stale swallow/capture if mouse moves without right button pressed
    if (m == WM_MOUSEMOVE && !(pMsg->wParam & MK_RBUTTON)) {
        if (g_swallowRButtonUp.load()) {
            g_swallowRButtonUp.store(false);
            if (g_captureHwnd && GetCapture() == g_captureHwnd) {
                ReleaseCapture();
            }
            g_captureHwnd = nullptr;
            g_state.store(ScrollState::Idle);
        }
    }

    // Strip MK_CONTROL from wheel messages during active drag to avoid
    // unintended zooming
    if (m == WM_MOUSEWHEEL || m == WM_MOUSEHWHEEL) {
        if (g_state.load() != ScrollState::Idle || extra == MASK_EXTRA_INFO) {
            if (canMutate)
                pMsg->wParam &= ~((WPARAM)MK_CONTROL);
            return false;
        }
    }

    // Key presses end cruising
    if (m == WM_KEYDOWN || m == WM_SYSKEYDOWN) {
        if (g_state.load() == ScrollState::Cruising) {
            if (pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_LCONTROL &&
                pMsg->wParam != VK_RCONTROL) {
                g_state.store(ScrollState::Idle);
            }
        }
    }

    if (m == WM_KEYUP || m == WM_SYSKEYUP) {
        if (pMsg->wParam == VK_CONTROL) {
            if (!IsCtrlPhysicallyDown()) {
                ScrollState cur = g_state.load();
                if (cur == ScrollState::Cruising ||
                    cur == ScrollState::Scrolling) {
                    g_state.store(ScrollState::Idle);
                }
            }
            return false;
        }
    }

    // Mouse click ends cruising
    if (g_state.load() == ScrollState::Cruising) {
        if ((m == WM_RBUTTONDOWN || m == WM_RBUTTONDBLCLK) &&
            (pMsg->wParam & MK_CONTROL)) {
            g_state.store(ScrollState::Idle);
        } else {
            UINT upToSwallow = 0;
            switch (m) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONDBLCLK:
                    upToSwallow = WM_LBUTTONUP;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONDBLCLK:
                    upToSwallow = WM_RBUTTONUP;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONDBLCLK:
                    upToSwallow = WM_MBUTTONUP;
                    break;
                case WM_XBUTTONDOWN:
                case WM_XBUTTONDBLCLK:
                    upToSwallow = WM_XBUTTONUP;
                    break;
            }
            if (upToSwallow != 0) {
                g_swallowButtonUp.store(upToSwallow);
                g_state.store(ScrollState::Idle);
                return true;
            }
        }
    }

    // Gesture initiation: Ctrl + Right Mouse Down
    if (m == WM_RBUTTONDOWN || m == WM_RBUTTONDBLCLK) {
        if (pMsg->wParam & MK_CONTROL) {
            g_state.store(ScrollState::Pending);
            g_swallowRButtonUp.store(true);
            g_captureHwnd = pMsg->hwnd;
            SetCapture(g_captureHwnd);

            GetCursorPos(&g_anchorPt);
            g_lastPt = g_anchorPt;
            g_accumY = 0.0;
            g_accumX = 0.0;
            g_activeAxis = DragAxis::Free;

            HWND hTargetWnd = GetAncestor(pMsg->hwnd, GA_ROOT);
            if (!hTargetWnd)
                hTargetWnd = pMsg->hwnd;

            GetWindowRect(hTargetWnd, &g_windowBounds);

            // Do not steal focus from deliberately non-activating windows (e.g.
            // On-Screen Keyboard)
            LONG_PTR exStyle = GetWindowLongPtrW(hTargetWnd, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_NOACTIVATE) &&
                GetForegroundWindow() != hTargetWnd) {
                SetForegroundWindow(hTargetWnd);
            }
            return true;
        }
    }

    // Gesture dragging & displacement accumulation
    if (m == WM_MOUSEMOVE) {
        ScrollState cur = g_state.load();
        if (cur == ScrollState::Pending || cur == ScrollState::Scrolling) {
            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&g_windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                return false;
            }

            int totalDx = curPt.x - g_anchorPt.x;
            int totalDy = curPt.y - g_anchorPt.y;

            bool deadzoneExceeded = false;
            if (g_settings.enable_horizontal) {
                deadzoneExceeded = (totalDx * totalDx + totalDy * totalDy) >=
                                   (g_settings.deadzone * g_settings.deadzone);
            } else {
                deadzoneExceeded = std::abs(totalDy) >= g_settings.deadzone;
            }

            if (cur == ScrollState::Pending && deadzoneExceeded) {
                g_state.store(ScrollState::Scrolling);
                g_lastPt = curPt;

                if (g_settings.axis_lock && g_settings.enable_horizontal) {
                    int absX = std::abs(totalDx);
                    int absY = std::abs(totalDy);
                    g_activeAxis = (absY >= absX) ? DragAxis::VerticalOnly
                                                  : DragAxis::HorizontalOnly;
                } else if (!g_settings.enable_horizontal) {
                    g_activeAxis = DragAxis::VerticalOnly;
                } else {
                    g_activeAxis = DragAxis::Free;
                }
            }

            if (g_state.load() == ScrollState::Scrolling) {
                int dx = curPt.x - g_lastPt.x;
                int dy = curPt.y - g_lastPt.y;
                g_lastPt = curPt;

                // When axis_lock is OFF, g_activeAxis is DragAxis::Free,
                // so neither dx nor dy will be zeroed out.
                if (g_activeAxis == DragAxis::VerticalOnly)
                    dx = 0;
                if (g_activeAxis == DragAxis::HorizontalOnly)
                    dy = 0;

                HWND hTarget = nullptr;
                bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);
                double wheelUnits = (static_cast<double>(WHEEL_DELTA) /
                                     g_settings.pixels_per_notch);
                int chunk =
                    g_settings.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;

                // Shared dynamic velocity scaling for natural diagonal speed
                double velocityFactor = 1.0;
                if (g_settings.enable_velocity) {
                    double dist = std::hypot(dx, dy);
                    if (dist <= 2.0)
                        velocityFactor = 0.40;
                    else if (dist <= 6.0)
                        velocityFactor = 1.0;
                    else
                        velocityFactor =
                            std::min(2.5, 1.0 + ((dist - 6.0) * 0.06));
                }

                // --- 1. Vertical Processing ---
                if (dy != 0) {
                    double directionY = g_settings.touch_style ? 1.0 : -1.0;
                    g_accumY += (dy * directionY * wheelUnits * velocityFactor);

                    if (isDiscrete) {
                        int notches = 0;
                        while (g_accumY <= -WHEEL_DELTA) {
                            notches -= WHEEL_DELTA;
                            g_accumY += WHEEL_DELTA;
                        }
                        while (g_accumY >= WHEEL_DELTA) {
                            notches += WHEEL_DELTA;
                            g_accumY -= WHEEL_DELTA;
                        }
                        if (notches != 0)
                            DispatchScroll(notches, hTarget, true);
                    } else {
                        int emitDelta = 0;
                        while (g_accumY <= -chunk) {
                            emitDelta -= chunk;
                            g_accumY += chunk;
                        }
                        while (g_accumY >= chunk) {
                            emitDelta += chunk;
                            g_accumY -= chunk;
                        }
                        if (emitDelta != 0)
                            DispatchScroll(emitDelta, hTarget, false);
                    }
                }

                // --- 2. Horizontal Processing ---
                if (dx != 0 && g_settings.enable_horizontal) {
                    // In Windows MOUSEEVENTF_HWHEEL:
                    // positive delta = wheel tilted right (scroll content left,
                    // viewport moves right) negative delta = wheel tilted left
                    // (scroll content right, viewport moves left) For natural
                    // touch (pulling canvas):
                    double directionX = g_settings.touch_style ? -1.0 : 1.0;
                    g_accumX += (dx * directionX * wheelUnits * velocityFactor);

                    if (isDiscrete) {
                        int notches = 0;
                        while (g_accumX <= -WHEEL_DELTA) {
                            notches -= WHEEL_DELTA;
                            g_accumX += WHEEL_DELTA;
                        }
                        while (g_accumX >= WHEEL_DELTA) {
                            notches += WHEEL_DELTA;
                            g_accumX -= WHEEL_DELTA;
                        }
                        if (notches != 0)
                            DispatchHScroll(notches, hTarget, true);
                    } else {
                        int emitDelta = 0;
                        while (g_accumX <= -chunk) {
                            emitDelta -= chunk;
                            g_accumX += chunk;
                        }
                        while (g_accumX >= chunk) {
                            emitDelta += chunk;
                            g_accumX -= chunk;
                        }
                        if (emitDelta != 0)
                            DispatchHScroll(emitDelta, hTarget, false);
                    }
                }

                return true;
            }
        }
    }

    // Always intercept and swallow WM_RBUTTONUP if we swallowed the matching
    // WM_RBUTTONDOWN
    if (m == WM_RBUTTONUP && g_swallowRButtonUp.load()) {
        g_swallowRButtonUp.store(false);
        if (g_captureHwnd && GetCapture() == g_captureHwnd) {
            ReleaseCapture();
        }
        g_captureHwnd = nullptr;

        ScrollState cur = g_state.load();
        if (cur == ScrollState::Pending) {
            g_state.store(ScrollState::Idle);
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_MOUSE;
            inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputs[0].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
            inputs[1].type = INPUT_MOUSE;
            inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            inputs[1].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
            SendInput(2, inputs, sizeof(INPUT));
        } else if (cur == ScrollState::Scrolling &&
                   g_settings.enable_cruising && IsCtrlPhysicallyDown()) {
            g_state.store(ScrollState::Cruising);
            StartCruiseThreadIfNeeded();
        } else {
            g_state.store(ScrollState::Idle);
        }
        // App never saw WM_RBUTTONDOWN, so never let WM_RBUTTONUP through
        return true;
    }

    return false;
}

// --- STANDARD MESSAGE RETRIEVAL HOOKS ---
using GetMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
GetMessageW_t pfnGetMessageW_Original = nullptr;
BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg,
                             HWND hWnd,
                             UINT wMsgFilterMin,
                             UINT wMsgFilterMax) {
    BOOL ret =
        pfnGetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (ret > 0 && ProcessScrollInput(lpMsg, true))
        lpMsg->message = WM_NULL;
    return ret;
}

using PeekMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
PeekMessageW_t pfnPeekMessageW_Original = nullptr;
BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg,
                              HWND hWnd,
                              UINT wMsgFilterMin,
                              UINT wMsgFilterMax,
                              UINT wRemoveMsg) {
    BOOL ret = pfnPeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin,
                                        wMsgFilterMax, wRemoveMsg);
    if (ret && (wRemoveMsg & PM_REMOVE) && ProcessScrollInput(lpMsg, true))
        lpMsg->message = WM_NULL;
    return ret;
}

using GetMessageA_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
GetMessageA_t pfnGetMessageA_Original = nullptr;
BOOL WINAPI GetMessageA_Hook(LPMSG lpMsg,
                             HWND hWnd,
                             UINT wMsgFilterMin,
                             UINT wMsgFilterMax) {
    BOOL ret =
        pfnGetMessageA_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (ret > 0 && ProcessScrollInput(lpMsg, true))
        lpMsg->message = WM_NULL;
    return ret;
}

using PeekMessageA_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
PeekMessageA_t pfnPeekMessageA_Original = nullptr;
BOOL WINAPI PeekMessageA_Hook(LPMSG lpMsg,
                              HWND hWnd,
                              UINT wMsgFilterMin,
                              UINT wMsgFilterMax,
                              UINT wRemoveMsg) {
    BOOL ret = pfnPeekMessageA_Original(lpMsg, hWnd, wMsgFilterMin,
                                        wMsgFilterMax, wRemoveMsg);
    if (ret && (wRemoveMsg & PM_REMOVE) && ProcessScrollInput(lpMsg, true))
        lpMsg->message = WM_NULL;
    return ret;
}

// --- SYNCHRONOUS KEY STATE MASKING ---
using GetKeyState_t = SHORT(WINAPI*)(int);
GetKeyState_t pfnGetKeyState_Original = nullptr;
SHORT WINAPI GetKeyState_Hook(int nVirtKey) {
    if (g_state.load() != ScrollState::Idle &&
        (nVirtKey == VK_CONTROL || nVirtKey == VK_LCONTROL ||
         nVirtKey == VK_RCONTROL)) {
        return 0;
    }
    return pfnGetKeyState_Original(nVirtKey);
}

SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    if (g_state.load() != ScrollState::Idle &&
        (vKey == VK_CONTROL || vKey == VK_LCONTROL || vKey == VK_RCONTROL)) {
        return 0;
    }
    return pfnGetAsyncKeyState_Original(vKey);
}

using GetKeyboardState_t = BOOL(WINAPI*)(PBYTE);
GetKeyboardState_t pfnGetKeyboardState_Original = nullptr;
BOOL WINAPI GetKeyboardState_Hook(PBYTE lpKeyState) {
    BOOL ret = pfnGetKeyboardState_Original(lpKeyState);
    if (ret && g_state.load() != ScrollState::Idle) {
        lpKeyState[VK_CONTROL] &= 0x7F;
        lpKeyState[VK_LCONTROL] &= 0x7F;
        lpKeyState[VK_RCONTROL] &= 0x7F;
    }
    return ret;
}

// --- WINDHAWK LIFECYCLE ---
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Drag to Scroll mod");
    LoadSettings();
    InitializeCriticalSection(&g_cruiseCs);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cruiseEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_threadCreated.store(false);

    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook,
                                   &pfnGetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook,
                                   &pfnPeekMessageW_Original);
    WindhawkUtils::SetFunctionHook(GetMessageA, GetMessageA_Hook,
                                   &pfnGetMessageA_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageA, PeekMessageA_Hook,
                                   &pfnPeekMessageA_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook,
                                   &pfnGetKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetAsyncKeyState, GetAsyncKeyState_Hook,
                                   &pfnGetAsyncKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetKeyboardState, GetKeyboardState_Hook,
                                   &pfnGetKeyboardState_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Drag to Scroll mod");
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_hCruiseThread) {
        WaitForSingleObject(g_hCruiseThread, INFINITE);
        CloseHandle(g_hCruiseThread);
        g_hCruiseThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_cruiseEvent) {
        CloseHandle(g_cruiseEvent);
        g_cruiseEvent = nullptr;
    }
    DeleteCriticalSection(&g_cruiseCs);
    g_threadCreated.store(false);
}

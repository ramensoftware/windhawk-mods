// ==WindhawkMod==
// @id              global-drag-to-scroll
// @name            Global Drag-to-Scroll
// @description     Hold Ctrl + Right-Click and drag to scroll smoothly in any application with adaptive line precision, dynamic velocity, and hands-free cruising.
// @version         0.3.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @license         MIT
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Global Drag-to-Scroll

Hold **`Ctrl` + Right-Click** and drag your mouse to scroll naturally in any application (Chrome, VS Code, File Explorer, Notepad, Services, Task Scheduler, and classic Win32 tools).

![Global Drag-to-Scroll Demo](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/preview-global-drag-to-scroll.webp)

## Key Features

* **High-Resolution Smooth Scrolling:** Injects fractional wheel deltas for pixel-perfect micro-scrolling in modern apps, while safely preserving classic line-by-line navigation in legacy Win32 lists.
* **Zero Interference:** Uses window capture and key masking so applications never accidentally zoom or trigger unwanted context menus.
* **Continuous Hands-Free Cruising:** Release the Right Mouse Button while keeping `Ctrl` held down to let the page scroll on its own. Cruising stops the moment you release `Ctrl`, click any mouse button, or press another key.
* **Safe Tap Pass-Through:** Tapping `Ctrl + Right-Click` in place without dragging passes through as a standard right-click.

## Compatibility Note
If you use CAD software (like AutoCAD/Blender) or games that rely heavily on `Ctrl + Right-Click` for their own features, you should exclude those specific processes in the Windhawk mod's "Advanced" tab to prevent conflicts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- touch_style: true
  $name: "Natural / Touchscreen Scrolling"
  $description: "When enabled, dragging down pulls content down (touchscreen style). When disabled, behaves like a standard mouse wheel."
- enable_cruising: true
  $name: "Continuous Cruising (Hold Ctrl)"
  $description: "Releasing Right-Click while holding Ctrl keeps scrolling automatically based on displacement. Breaks immediately on any click, keypress, or Ctrl release."
- pixels_per_notch: 35
  $name: "Base Drag Distance (Pixels per Notch)"
  $description: "Pixels required to drag for one scroll notch (120 units). Clamped to a minimum of 10. 35-45 gives steady, calm control; 20-25 is faster."
- enable_velocity: true
  $name: "Dynamic Velocity Scaling"
  $description: "Slow drags move line-by-line with high precision; rapid flicks accelerate without overshooting."
- deadzone: 5
  $name: "Drag Deadzone (pixels)"
  $description: "Movement threshold in pixels before drag-scrolling engages. Releasing within this threshold preserves normal right-clicks (minimum 0)."
- smooth_wheel: true
  $name: "High-Resolution Smooth Scrolling"
  $description: "Injects smaller scroll chunks for smoother movement in modern apps (Chrome, Edge). Disable this if scrolling feels completely broken in older desktop apps."
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

constexpr ULONG_PTR MASK_EXTRA_INFO = 0x5343524C;
constexpr ULONG_PTR REPLAY_EXTRA_INFO = 0x5245504C;
constexpr int SMOOTH_SCROLL_CHUNK = 15;

struct ModSettings {
    bool touch_style = true;
    bool enable_cruising = true;
    double pixels_per_notch = 35.0;
    bool enable_velocity = true;
    int deadzone = 5;
    bool smooth_wheel = true;
};

static ModSettings g_settings;

enum class ScrollState { Idle, Pending, Scrolling, Cruising };
static std::atomic<ScrollState> g_state{ScrollState::Idle};
static std::atomic<bool> g_swallowRButtonUp{false};
static std::atomic<UINT> g_swallowButtonUp{0};

static POINT g_anchorPt = {0, 0};
static POINT g_lastPt = {0, 0};
static double g_accumY = 0.0;
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
};
static CruiseContext g_cruiseCtx;
static CRITICAL_SECTION g_cruiseCs;

void LoadSettings() {
    g_settings.touch_style = Wh_GetIntSetting(L"touch_style") != 0;
    g_settings.enable_cruising = Wh_GetIntSetting(L"enable_cruising") != 0;
    g_settings.pixels_per_notch = static_cast<double>(
        std::max(10, Wh_GetIntSetting(L"pixels_per_notch")));
    g_settings.enable_velocity = Wh_GetIntSetting(L"enable_velocity") != 0;
    g_settings.deadzone = std::max(0, Wh_GetIntSetting(L"deadzone"));
    g_settings.smooth_wheel = Wh_GetIntSetting(L"smooth_wheel") != 0;
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

bool GetScrollTargetInfo(POINT pt, HWND& outTarget) {
    outTarget = WindowFromPoint(pt);
    if (!outTarget)
        return false;

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
    for (int i = 0; cls[i] && i < 63; ++i)
        lowerCls[i] = static_cast<WCHAR>(towlower(cls[i]));

    return (_wcsicmp(cls, L"SysListView32") == 0 ||
            _wcsicmp(cls, L"SysTreeView32") == 0 ||
            _wcsicmp(cls, L"Edit") == 0 || _wcsicmp(cls, L"ListBox") == 0 ||
            wcsstr(lowerCls, L"richedit") != nullptr);
}

void DispatchScroll(int deltaY, HWND hTarget, bool isDiscrete) {
    if (isDiscrete && hTarget) {
        int steps = std::max(1, std::abs(deltaY) / WHEEL_DELTA);
        WPARAM scrollCmd = (deltaY > 0) ? SB_LINEUP : SB_LINEDOWN;
        for (int i = 0; i < steps; ++i) {
            PostMessageW(hTarget, WM_VSCROLL, scrollCmd, 0);
            PostMessageW(hTarget, WM_VSCROLL, SB_ENDSCROLL, 0);
        }
        return;
    }
    SendMaskedWheelAsync(deltaY);
}

DWORD WINAPI CruiseWorkerThread(LPVOID) {
    HANDLE handles[] = {g_stopEvent, g_cruiseEvent};
    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_FAILED)
            return 0;

        double cruiseAccum = 0.0;
        EnterCriticalSection(&g_cruiseCs);
        CruiseContext ctx = g_cruiseCtx;
        LeaveCriticalSection(&g_cruiseCs);

        while (g_state.load() == ScrollState::Cruising) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0)
                return 0;
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                g_state.store(ScrollState::Idle);
                break;
            }

            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&ctx.windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                break;
            }

            int distY = curPt.y - ctx.anchorPt.y;
            if (std::abs(distY) >= ctx.deadzone) {
                double direction = ctx.touch_style ? 1.0 : -1.0;
                double notchRate =
                    (static_cast<double>(distY) / ctx.pixels_per_notch);
                double unitsThisTick =
                    notchRate * direction * WHEEL_DELTA * 0.035;

                cruiseAccum += unitsThisTick;
                HWND hTarget = nullptr;
                bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);

                if (isDiscrete) {
                    int notches = 0;
                    while (cruiseAccum <= -WHEEL_DELTA) {
                        notches -= WHEEL_DELTA;
                        cruiseAccum += WHEEL_DELTA;
                    }
                    while (cruiseAccum >= WHEEL_DELTA) {
                        notches += WHEEL_DELTA;
                        cruiseAccum -= WHEEL_DELTA;
                    }
                    if (notches != 0)
                        DispatchScroll(notches, hTarget, true);
                } else {
                    int chunk =
                        ctx.smooth_wheel ? SMOOTH_SCROLL_CHUNK : WHEEL_DELTA;
                    int emitDelta = 0;
                    while (cruiseAccum <= -chunk) {
                        emitDelta -= chunk;
                        cruiseAccum += chunk;
                    }
                    while (cruiseAccum >= chunk) {
                        emitDelta += chunk;
                        cruiseAccum -= chunk;
                    }
                    if (emitDelta != 0)
                        DispatchScroll(emitDelta, hTarget, false);
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
    LeaveCriticalSection(&g_cruiseCs);

    bool expected = false;
    if (g_threadCreated.compare_exchange_strong(expected, true)) {
        g_hCruiseThread =
            CreateThread(nullptr, 0, CruiseWorkerThread, nullptr, 0, nullptr);
    }
    if (g_cruiseEvent)
        SetEvent(g_cruiseEvent);
}

// Memory-safe processor block
bool ProcessScrollInput(MSG* pMsg, bool canMutate) {
    if (!pMsg || !pMsg->hwnd)
        return false;
    ULONG_PTR extra = GetMessageExtraInfo();
    if (extra == REPLAY_EXTRA_INFO)
        return false;

    if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONDBLCLK ||
        pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK ||
        pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_MBUTTONDBLCLK ||
        pMsg->message == WM_XBUTTONDOWN || pMsg->message == WM_XBUTTONDBLCLK) {
        g_swallowButtonUp.store(0);
    }

    if (g_swallowButtonUp.load() != 0 &&
        pMsg->message == g_swallowButtonUp.load()) {
        g_swallowButtonUp.store(0);
        return true;
    }

    if (pMsg->message == WM_MOUSEMOVE && !(pMsg->wParam & MK_RBUTTON)) {
        if (g_swallowRButtonUp.load()) {
            g_swallowRButtonUp.store(false);
            if (GetCapture() == g_captureHwnd && g_captureHwnd != nullptr)
                ReleaseCapture();
            g_captureHwnd = nullptr;
            g_state.store(ScrollState::Idle);
        }
    }

    if (pMsg->message == WM_MOUSEWHEEL) {
        if (g_state.load() != ScrollState::Idle || extra == MASK_EXTRA_INFO) {
            if (canMutate)
                pMsg->wParam &= ~((WPARAM)MK_CONTROL);
            return false;
        }
    }

    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) {
        if (g_state.load() == ScrollState::Cruising) {
            if (pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_LCONTROL &&
                pMsg->wParam != VK_RCONTROL) {
                g_state.store(ScrollState::Idle);
            }
        }
    }

    if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) {
        if (pMsg->wParam == VK_CONTROL) {
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                ScrollState cur = g_state.load();
                if (cur == ScrollState::Cruising ||
                    cur == ScrollState::Scrolling)
                    g_state.store(ScrollState::Idle);
            }
            return false;
        }
    }

    if (g_state.load() == ScrollState::Cruising) {
        if ((pMsg->message == WM_RBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDBLCLK) &&
            (pMsg->wParam & MK_CONTROL)) {
            g_state.store(ScrollState::Idle);
        } else {
            UINT upToSwallow = 0;
            switch (pMsg->message) {
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

    if (pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK) {
        if (pMsg->wParam & MK_CONTROL) {
            g_state = ScrollState::Pending;
            g_swallowRButtonUp.store(true);
            g_captureHwnd = pMsg->hwnd;
            SetCapture(g_captureHwnd);

            GetCursorPos(&g_anchorPt);
            g_lastPt = g_anchorPt;
            g_accumY = 0.0;

            HWND hRoot = GetAncestor(pMsg->hwnd, GA_ROOT);
            if (hRoot) {
                GetWindowRect(hRoot, &g_windowBounds);
                if (GetForegroundWindow() != hRoot)
                    SetForegroundWindow(hRoot);
            } else {
                GetWindowRect(pMsg->hwnd, &g_windowBounds);
                if (GetForegroundWindow() != pMsg->hwnd)
                    SetForegroundWindow(pMsg->hwnd);
            }
            return true;
        }
    }

    if (pMsg->message == WM_MOUSEMOVE) {
        ScrollState cur = g_state.load();
        if (cur == ScrollState::Pending || cur == ScrollState::Scrolling) {
            POINT curPt;
            GetCursorPos(&curPt);
            if (!PtInRect(&g_windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                return false;
            }

            int totalDy = curPt.y - g_anchorPt.y;
            if (cur == ScrollState::Pending &&
                std::abs(totalDy) >= g_settings.deadzone) {
                g_state = ScrollState::Scrolling;
                g_lastPt = curPt;
            }

            if (g_state.load() == ScrollState::Scrolling) {
                int dy = curPt.y - g_lastPt.y;
                g_lastPt = curPt;

                if (dy != 0) {
                    double direction = g_settings.touch_style ? 1.0 : -1.0;
                    double velocityFactor = 1.0;
                    if (g_settings.enable_velocity) {
                        int absDy = std::abs(dy);
                        if (absDy <= 2)
                            velocityFactor = 0.40;
                        else if (absDy <= 6)
                            velocityFactor = 1.0;
                        else
                            velocityFactor =
                                std::min(2.5, 1.0 + ((absDy - 6) * 0.06));
                    }

                    double wheelUnits = (static_cast<double>(WHEEL_DELTA) /
                                         g_settings.pixels_per_notch);
                    g_accumY += (dy * direction * wheelUnits * velocityFactor);

                    HWND hTarget = nullptr;
                    bool isDiscrete = GetScrollTargetInfo(curPt, hTarget);

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
                        int chunk = g_settings.smooth_wheel
                                        ? SMOOTH_SCROLL_CHUNK
                                        : WHEEL_DELTA;
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
                return true;
            }
        }
    }

    if (pMsg->message == WM_RBUTTONUP) {
        if (g_swallowRButtonUp.load()) {
            g_swallowRButtonUp.store(false);
            if (GetCapture() == g_captureHwnd && g_captureHwnd != nullptr)
                ReleaseCapture();
            g_captureHwnd = nullptr;

            ScrollState cur = g_state.load();
            if (cur == ScrollState::Scrolling) {
                g_accumY = 0.0;
                if (g_settings.enable_cruising &&
                    (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                    g_state.store(ScrollState::Cruising);
                    StartCruiseThreadIfNeeded();
                } else {
                    g_state.store(ScrollState::Idle);
                }
                return true;
            } else if (cur == ScrollState::Pending) {
                g_state.store(ScrollState::Idle);
                INPUT inputs[2] = {};
                inputs[0].type = INPUT_MOUSE;
                inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                inputs[0].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
                inputs[1].type = INPUT_MOUSE;
                inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
                inputs[1].mi.dwExtraInfo = REPLAY_EXTRA_INFO;
                SendInput(2, inputs, sizeof(INPUT));
                return true;
            }
        }
    }
    return false;
}

// --- STANDARD MESSAGE LOOPS ---
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

// --- SAFE UWP/XAML DISPATCH HOOKS ---
using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
DispatchMessageW_t pfnDispatchMessageW_Original = nullptr;
LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    MSG msgCopy = *lpMsg;
    if (ProcessScrollInput(&msgCopy, false))
        return 0;  // Return 0 without mutating read-only MSG
    return pfnDispatchMessageW_Original(lpMsg);
}

using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
DispatchMessageA_t pfnDispatchMessageA_Original = nullptr;
LRESULT WINAPI DispatchMessageA_Hook(const MSG* lpMsg) {
    MSG msgCopy = *lpMsg;
    if (ProcessScrollInput(&msgCopy, false))
        return 0;  // Return 0 without mutating read-only MSG
    return pfnDispatchMessageA_Original(lpMsg);
}

// --- KEY MASKING ---
using GetKeyState_t = SHORT(WINAPI*)(int);
GetKeyState_t pfnGetKeyState_Original = nullptr;
SHORT WINAPI GetKeyState_Hook(int nVirtKey) {
    if (g_state.load() != ScrollState::Idle &&
        (nVirtKey == VK_CONTROL || nVirtKey == VK_LCONTROL ||
         nVirtKey == VK_RCONTROL))
        return 0;
    return pfnGetKeyState_Original(nVirtKey);
}

using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
GetAsyncKeyState_t pfnGetAsyncKeyState_Original = nullptr;
SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    if (g_state.load() != ScrollState::Idle &&
        (vKey == VK_CONTROL || vKey == VK_LCONTROL || vKey == VK_RCONTROL))
        return 0;
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

BOOL Wh_ModInit() {
    Wh_Log(
        L"Initializing dynamically across standard and elevated UIPI "
        L"processes");
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
    WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook,
                                   &pfnDispatchMessageW_Original);
    WindhawkUtils::SetFunctionHook(DispatchMessageA, DispatchMessageA_Hook,
                                   &pfnDispatchMessageA_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook,
                                   &pfnGetKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetAsyncKeyState, GetAsyncKeyState_Hook,
                                   &pfnGetAsyncKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetKeyboardState, GetKeyboardState_Hook,
                                   &pfnGetKeyboardState_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading.");
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

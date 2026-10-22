// ==WindhawkMod==
// @id              global-drag-to-scroll
// @name            Global Drag-to-Scroll
// @description     Hold Ctrl + Right-Click and drag to scroll smoothly in any application with adaptive line precision, dynamic velocity, and hands-free cruising.
// @version         0.0.5
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @license         MIT
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Drag-to-Scroll

Hold **`Ctrl` + Right-Click** and drag your mouse to scroll naturally in any application (Chrome, VS Code, File Explorer, Notepad, Services, Task Scheduler, and classic Win32 tools).

## Key Features

* **Zero Interference:** Uses window capture and key masking so applications never accidentally zoom or trigger unwanted context menus.
* **Adaptive Precision:** Crawling your mouse slowly moves exactly **1 line or 1 item at a time** in list views (like Services). Rapid swipes scale up smoothly to traverse long documents.
* **Continuous Hands-Free Cruising:** Release the Right Mouse Button while keeping `Ctrl` held down to let the page scroll on its own. Cruising stops the moment you release `Ctrl`, click any mouse button, or press another key.
* **Safe Tap Pass-Through:** Tapping `Ctrl + Right-Click` in place without dragging passes through as a standard right-click.

*Note: If a specific full-screen game or CAD software conflicts with `Ctrl + Right-Click`, you can exclude that process in Windhawk's Advanced mod settings tab.*
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
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>

constexpr ULONG_PTR MASK_EXTRA_INFO = 0x5343524C;   // 'SCRL'
constexpr ULONG_PTR REPLAY_EXTRA_INFO = 0x5245504C; // 'REPL'

struct ModSettings {
    bool touch_style = true;
    bool enable_cruising = true;
    double pixels_per_notch = 35.0;
    bool enable_velocity = true;
    int deadzone = 5;
};

static ModSettings g_settings;

enum class ScrollState {
    Idle,
    Pending,
    Scrolling,
    Cruising
};

static std::atomic<ScrollState> g_state{ScrollState::Idle};
static std::atomic<bool> g_swallowRButtonUp{false};

static POINT g_anchorPt = {0, 0};
static POINT g_lastPt = {0, 0};
static double g_accumY = 0.0;
static RECT g_windowBounds = {0, 0, 0, 0};

// Event-driven thread management (Thread-safe Lazy Initialization)
static HANDLE g_hCruiseThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_cruiseEvent = nullptr;
static std::atomic<bool> g_threadCreated{false};

void LoadSettings() {
    g_settings.touch_style = Wh_GetIntSetting(L"touch_style") != 0;
    g_settings.enable_cruising = Wh_GetIntSetting(L"enable_cruising") != 0;

    int ppn = Wh_GetIntSetting(L"pixels_per_notch");
    g_settings.pixels_per_notch = static_cast<double>(std::max(10, ppn));

    g_settings.enable_velocity = Wh_GetIntSetting(L"enable_velocity") != 0;

    int deadzone = Wh_GetIntSetting(L"deadzone");
    g_settings.deadzone = std::max(0, deadzone);

    Wh_Log(L"Settings loaded: TouchStyle=%d, Cruising=%d, PPN=%.1f",
           g_settings.touch_style ? 1 : 0, g_settings.enable_cruising ? 1 : 0, g_settings.pixels_per_notch);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings reloaded.");
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

// Dispatches scroll: adaptive 1-by-1 line steps for Win32 lists; masked wheel ticks for modern apps
void DispatchScroll(int deltaY, POINT pt, bool isCruising = false) {
    HWND hTarget = WindowFromPoint(pt);
    if (hTarget) {
        WCHAR cls[64] = {};
        GetClassNameW(hTarget, cls, ARRAYSIZE(cls));

        if (_wcsicmp(cls, L"SysHeader32") == 0) {
            HWND hParent = GetParent(hTarget);
            if (hParent) {
                hTarget = hParent;
                GetClassNameW(hTarget, cls, ARRAYSIZE(cls));
            }
        }

        // Case-insensitive check covering RICHEDIT50W, richedit20w, Edit, etc.
        WCHAR lowerCls[64] = {};
        for (int i = 0; cls[i] && i < 63; ++i) {
            lowerCls[i] = static_cast<WCHAR>(towlower(cls[i]));
        }

        bool isWin32ListOrText = (_wcsicmp(cls, L"SysListView32") == 0 ||
                                  _wcsicmp(cls, L"SysTreeView32") == 0 ||
                                  _wcsicmp(cls, L"Edit") == 0 ||
                                  _wcsicmp(cls, L"ListBox") == 0 ||
                                  wcsstr(lowerCls, L"richedit") != nullptr);

        if (isWin32ListOrText) {
            int steps = std::max(1, std::abs(deltaY) / WHEEL_DELTA);

            WPARAM scrollCmd = (deltaY > 0) ? SB_LINEUP : SB_LINEDOWN;
            for (int i = 0; i < steps; ++i) {
                PostMessageW(hTarget, WM_VSCROLL, scrollCmd, 0);
                PostMessageW(hTarget, WM_VSCROLL, SB_ENDSCROLL, 0);
            }
            return;
        }
    }

    SendMaskedWheelAsync(deltaY);
}

// Background thread: blocks on events when idle (zero CPU wakeups)
DWORD WINAPI CruiseWorkerThread(LPVOID) {
    HANDLE handles[] = {g_stopEvent, g_cruiseEvent};

    while (true) {
        DWORD waitRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

        // g_stopEvent signaled or wait failed -> exit thread cleanly
        if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_FAILED) {
            return 0;
        }

        // g_cruiseEvent signaled
        double cruiseAccum = 0.0;

        while (g_state.load() == ScrollState::Cruising) {
            // Check if mod is unloading
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                return 0;
            }

            // Halt if Ctrl key is released
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                g_state.store(ScrollState::Idle);
                break;
            }

            POINT curPt;
            GetCursorPos(&curPt);

            // Halt if cursor left active window bounds
            if (!PtInRect(&g_windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                break;
            }

            int distY = curPt.y - g_anchorPt.y;

            if (std::abs(distY) >= g_settings.deadzone) {
                double direction = g_settings.touch_style ? 1.0 : -1.0;
                double notchRate = (static_cast<double>(distY) / g_settings.pixels_per_notch);
                double unitsThisTick = notchRate * direction * WHEEL_DELTA * 0.035;

                cruiseAccum += unitsThisTick;

                int notchesToEmit = 0;
                while (cruiseAccum <= -WHEEL_DELTA) {
                    notchesToEmit -= WHEEL_DELTA;
                    cruiseAccum += WHEEL_DELTA;
                }
                while (cruiseAccum >= WHEEL_DELTA) {
                    notchesToEmit += WHEEL_DELTA;
                    cruiseAccum -= WHEEL_DELTA;
                }

                if (notchesToEmit != 0) {
                    DispatchScroll(notchesToEmit, curPt, true);
                }
            }

            // Sleep 20ms or exit immediately if stopped
            if (WaitForSingleObject(g_stopEvent, 20) == WAIT_OBJECT_0) {
                return 0;
            }
        }
    }
    return 0;
}

void StartCruiseThreadIfNeeded() {
    bool expected = false;
    if (g_threadCreated.compare_exchange_strong(expected, true)) {
        g_hCruiseThread = CreateThread(nullptr, 0, CruiseWorkerThread, nullptr, 0, nullptr);
    }
    if (g_cruiseEvent) {
        SetEvent(g_cruiseEvent);
    }
}

bool ProcessScrollInput(MSG* pMsg) {
    if (!pMsg || !pMsg->hwnd) return false; // Ignore Explorer.exe thread messages that lack physical UI

    ULONG_PTR extra = GetMessageExtraInfo();
    if (extra == REPLAY_EXTRA_INFO) {
        return false;
    }

    // Zero-Zoom Protection: Strip MK_CONTROL from wheel messages during drag or from mod
    if (pMsg->message == WM_MOUSEWHEEL) {
        if (g_state.load() != ScrollState::Idle || extra == MASK_EXTRA_INFO) {
            pMsg->wParam &= ~((WPARAM)MK_CONTROL);
            return false;
        }
    }

    // Keydown break condition: Any key besides Ctrl halts cruising
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) {
        if (g_state.load() == ScrollState::Cruising) {
            if (pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_LCONTROL && pMsg->wParam != VK_RCONTROL) {
                g_state.store(ScrollState::Idle);
            }
        }
    }

    // Keyup break condition: Releasing Ctrl halts active scrolling or cruising
    if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) {
        if (pMsg->wParam == VK_CONTROL) {
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                ScrollState cur = g_state.load();
                if (cur == ScrollState::Cruising) {
                    g_state.store(ScrollState::Idle);
                } else if (cur == ScrollState::Scrolling || cur == ScrollState::Pending) {
                    // Stop scrolling, but leave capture and g_swallowRButtonUp intact
                    // so the pending button-up is still consumed cleanly.
                    g_state.store(ScrollState::Idle);
                }
            }
            return false;
        }
    }

    // Mouse click break condition: Swallow the click that terminates cruising
    if (g_state.load() == ScrollState::Cruising) {
        // If initiating a new drag while cruising, yield so the main block catches it
        if ((pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK) && (pMsg->wParam & MK_CONTROL)) {
            g_state.store(ScrollState::Idle);
        }
        else if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN ||
                 pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_XBUTTONDOWN ||
                 pMsg->message == WM_LBUTTONDBLCLK || pMsg->message == WM_RBUTTONDBLCLK ||
                 pMsg->message == WM_MBUTTONDBLCLK || pMsg->message == WM_XBUTTONDBLCLK) {
            g_state.store(ScrollState::Idle);
            return true; // Swallow to avoid unwanted action/context menu
        }
    }

    // Ctrl + Right Mouse Down OR Right Mouse Double-Click (for rapid swipes)
    if (pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK) {
        if (pMsg->wParam & MK_CONTROL) {
            g_state = ScrollState::Pending;
            g_swallowRButtonUp.store(true);
            SetCapture(pMsg->hwnd);

            GetCursorPos(&g_anchorPt);
            g_lastPt = g_anchorPt;
            g_accumY = 0.0; // Wipe accumulation instantly on fresh grab

            HWND hRoot = GetAncestor(pMsg->hwnd, GA_ROOT);
            if (hRoot) {
                GetWindowRect(hRoot, &g_windowBounds);
            } else {
                GetWindowRect(pMsg->hwnd, &g_windowBounds);
            }
            return true; // Swallow original button-down/double-click
        }
    }

    // Mouse Movement
    if (pMsg->message == WM_MOUSEMOVE) {
        ScrollState cur = g_state.load();

        if (cur == ScrollState::Pending || cur == ScrollState::Scrolling) {
            // Guard: If button was released outside our knowledge, clean up capture
            if (!(pMsg->wParam & MK_RBUTTON)) {
                ReleaseCapture();
                g_swallowRButtonUp.store(false);
                g_state.store(ScrollState::Idle);
                return false;
            }

            POINT curPt;
            GetCursorPos(&curPt);

            if (!PtInRect(&g_windowBounds, curPt)) {
                // Out of bounds: stop scrolling but keep capture until button-up is swallowed
                g_state.store(ScrollState::Idle);
                return false;
            }

            int totalDy = curPt.y - g_anchorPt.y;

            if (cur == ScrollState::Pending) {
                if (std::abs(totalDy) >= g_settings.deadzone) {
                    g_state = ScrollState::Scrolling;
                    g_lastPt = curPt;
                }
            }

            if (g_state.load() == ScrollState::Scrolling) {
                int dy = curPt.y - g_lastPt.y;
                g_lastPt = curPt;

                if (dy != 0) {
                    double direction = g_settings.touch_style ? 1.0 : -1.0;

                    // Clamped, smooth velocity curve (max 2.5x)
                    double velocityFactor = 1.0;
                    if (g_settings.enable_velocity) {
                        int absDy = std::abs(dy);
                        if (absDy <= 2) {
                            velocityFactor = 0.40; // Precision line crawling
                        } else if (absDy <= 6) {
                            velocityFactor = 1.0;  // Standard speed
                        } else {
                            velocityFactor = std::min(2.5, 1.0 + ((absDy - 6) * 0.06));
                        }
                    }

                    double wheelUnitsPerPixel = (static_cast<double>(WHEEL_DELTA) / g_settings.pixels_per_notch);
                    g_accumY += (dy * direction * wheelUnitsPerPixel * velocityFactor);

                    int notchesToEmit = 0;
                    while (g_accumY <= -WHEEL_DELTA) {
                        notchesToEmit -= WHEEL_DELTA;
                        g_accumY += WHEEL_DELTA;
                    }
                    while (g_accumY >= WHEEL_DELTA) {
                        notchesToEmit += WHEEL_DELTA;
                        g_accumY -= WHEEL_DELTA;
                    }

                    if (notchesToEmit != 0) {
                        DispatchScroll(notchesToEmit, curPt, false);
                    }
                }
                return true;
            }
        }
    }

    // Right Mouse Up
    if (pMsg->message == WM_RBUTTONUP) {
        if (g_swallowRButtonUp.load()) {
            g_swallowRButtonUp.store(false);
            ReleaseCapture();

            ScrollState cur = g_state.load();

            if (cur == ScrollState::Scrolling) {
                g_accumY = 0.0; // Wipe out residual velocity so scrolling stops instantly
                bool ctrlPhysicallyDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                
                if (g_settings.enable_cruising && ctrlPhysicallyDown) {
                    g_state.store(ScrollState::Cruising);
                    StartCruiseThreadIfNeeded();
                } else {
                    g_state.store(ScrollState::Idle);
                }
                return true;
            } 
            else if (cur == ScrollState::Pending) {
                // Tapped inside deadzone: replay clean right-click
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
            } else {
                // Drag was cancelled earlier while right button was still down
                return true;
            }
        }
    }

    return false;
}

using GetKeyState_t = SHORT(WINAPI*)(int);
GetKeyState_t pfnGetKeyState_Original = nullptr;

SHORT WINAPI GetKeyState_Hook(int nVirtKey) {
    if (g_state.load() != ScrollState::Idle) {
        if (nVirtKey == VK_CONTROL || nVirtKey == VK_LCONTROL || nVirtKey == VK_RCONTROL) {
            return 0;
        }
    }
    return pfnGetKeyState_Original(nVirtKey);
}

using GetMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
GetMessageW_t pfnGetMessageW_Original = nullptr;

BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) {
    BOOL ret = pfnGetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (ret > 0 && ProcessScrollInput(lpMsg)) {
        lpMsg->message = WM_NULL;
    }
    return ret;
}

using PeekMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
PeekMessageW_t pfnPeekMessageW_Original = nullptr;

BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    BOOL ret = pfnPeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    if (ret && (wRemoveMsg & PM_REMOVE) && ProcessScrollInput(lpMsg)) {
        lpMsg->message = WM_NULL;
    }
    return ret;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing globally in PID: %lu", GetCurrentProcessId());
    LoadSettings();

    // Create sync events globally on init, but leave the thread creation for later
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_cruiseEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_threadCreated.store(false);

    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook, &pfnGetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook, &pfnPeekMessageW_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook, &pfnGetKeyState_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading.");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_hCruiseThread) {
        // INFINITE ensures Windhawk does not prematurely unload the DLL memory 
        // while the background thread is still executing, preventing process crashes.
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
    
    g_threadCreated.store(false);
}

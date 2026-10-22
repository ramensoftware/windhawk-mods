// ==WindhawkMod==
// @id              global-drag-to-scroll
// @name            Global Drag-to-Scroll
// @description     Hold Ctrl + Right-Click and drag to scroll smoothly in any application with adaptive line precision, dynamic velocity, and hands-free cruising.
// @version         0.0.1
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @license         MIT
// @include         *
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Drag-to-Scroll

Hold **`Ctrl` + Right-Click** and drag your mouse to scroll naturally in any application (Chrome, VS Code, File Explorer, Notepad, Services, Task Scheduler, and classic Win32 tools).

## Key Features

* **Zero Interference:** Masks modifier keys and context menus during dragging so applications never accidentally zoom or trigger context menus.
* **Adaptive Precision:** Crawling your mouse slowly moves exactly **1 line or 1 item at a time** in list views (like Services). Rapid swipes scale up smoothly to traverse long documents.
* **Continuous Hands-Free Cruising:** Release the Right Mouse Button while keeping `Ctrl` held down to let the page scroll on its own. Cruising stops the moment you release `Ctrl`, click any mouse button, or press another key.
* **Safe Tap Pass-Through:** Tapping `Ctrl + Right-Click` in place without dragging passes through as a standard right-click.
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
  $description: "Pixels required to drag for one scroll notch (120 units). 35-45 gives steady, calm control; 20-25 is faster."
- enable_velocity: true
  $name: "Dynamic Velocity Scaling"
  $description: "Slow drags move line-by-line with high precision; rapid flicks accelerate without overshooting."
- deadzone: 5
  $name: "Drag Deadzone (pixels)"
  $description: "Movement threshold in pixels before drag-scrolling engages. Releasing within this threshold preserves normal right-clicks."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>

constexpr ULONG_PTR MASK_EXTRA_INFO = 0x5343524C; // 'SCRL'
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
static POINT g_anchorPt = {0, 0};
static POINT g_lastPt = {0, 0};
static double g_accumY = 0.0;
static RECT g_windowBounds = {0, 0, 0, 0};
static std::atomic<bool> g_blockContextMenu{false};

static HANDLE g_hCruiseThread = nullptr;
static std::atomic<bool> g_stopCruiseThread{false};

void LoadSettings() {
    g_settings.touch_style = Wh_GetIntSetting(L"touch_style") != 0;
    g_settings.enable_cruising = Wh_GetIntSetting(L"enable_cruising") != 0;

    int ppn = Wh_GetIntSetting(L"pixels_per_notch");
    g_settings.pixels_per_notch = (ppn >= 10) ? static_cast<double>(ppn) : 35.0;

    g_settings.enable_velocity = Wh_GetIntSetting(L"enable_velocity") != 0;

    int deadzone = Wh_GetIntSetting(L"deadzone");
    g_settings.deadzone = (deadzone > 0) ? deadzone : 5;

    Wh_Log(L"[Drag-to-Scroll] Settings loaded: TouchStyle=%d, Cruising=%d, PPN=%.1f",
           g_settings.touch_style ? 1 : 0, g_settings.enable_cruising ? 1 : 0, g_settings.pixels_per_notch);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[Drag-to-Scroll] Settings reloaded.");
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
void DispatchScroll(int deltaY, POINT pt, int currentDy = 0) {
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

        bool isWin32ListOrText = (_wcsicmp(cls, L"SysListView32") == 0 ||
                                  _wcsicmp(cls, L"SysTreeView32") == 0 ||
                                  _wcsicmp(cls, L"Edit") == 0 ||
                                  _wcsicmp(cls, L"ListBox") == 0 ||
                                  wcsstr(cls, L"RichEdit") != nullptr);

        if (isWin32ListOrText) {
            int steps = std::max(1, std::abs(deltaY) / WHEEL_DELTA);

            // Fast drag in list views scrolls multiple rows to avoid feeling sluggish
            if (g_settings.enable_velocity && std::abs(currentDy) > 8) {
                steps *= 3;
            }

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

// Background thread: drives continuous cruising when mouse is stationary
DWORD WINAPI CruiseWorkerThread(LPVOID) {
    double cruiseAccum = 0.0;

    while (!g_stopCruiseThread.load()) {
        if (g_state.load() == ScrollState::Cruising) {
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                g_state.store(ScrollState::Idle);
                g_blockContextMenu.store(true);
                cruiseAccum = 0.0;
                Sleep(20);
                continue;
            }

            POINT curPt;
            GetCursorPos(&curPt);

            if (!PtInRect(&g_windowBounds, curPt)) {
                g_state.store(ScrollState::Idle);
                g_blockContextMenu.store(true);
                cruiseAccum = 0.0;
                Sleep(20);
                continue;
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
                    DispatchScroll(notchesToEmit, curPt, distY);
                }
            }
        } else {
            cruiseAccum = 0.0;
        }

        Sleep(20);
    }
    return 0;
}

bool ProcessScrollInput(MSG* pMsg) {
    if (!pMsg) return false;

    ULONG_PTR extra = GetMessageExtraInfo();

    if (extra == REPLAY_EXTRA_INFO) {
        return false;
    }

    // Zero-Zoom Protection: Strip MK_CONTROL from wheel messages
    if (pMsg->message == WM_MOUSEWHEEL) {
        if (g_state.load() != ScrollState::Idle || extra == MASK_EXTRA_INFO) {
            pMsg->wParam &= ~((WPARAM)MK_CONTROL);
            return false;
        }
    }

    if (pMsg->message == WM_CONTEXTMENU) {
        if (g_blockContextMenu.load() || g_state.load() != ScrollState::Idle) {
            Wh_Log(L"[Drag-to-Scroll] Suppressed WM_CONTEXTMENU.");
            g_blockContextMenu.store(false);
            return true;
        }
        return false;
    }

    // Break condition: Any key other than Ctrl terminates cruising
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) {
        if (g_state.load() == ScrollState::Cruising) {
            if (pMsg->wParam != VK_CONTROL && pMsg->wParam != VK_LCONTROL && pMsg->wParam != VK_RCONTROL) {
                Wh_Log(L"[Drag-to-Scroll] Key pressed. Halting cruise.");
                g_state.store(ScrollState::Idle);
                g_blockContextMenu.store(true);
            }
        }
    }

    // Break condition: Ctrl release terminates scrolling or cruising
    if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) {
        if (pMsg->wParam == VK_CONTROL) {
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                ScrollState cur = g_state.load();
                if (cur == ScrollState::Cruising || cur == ScrollState::Scrolling) {
                    Wh_Log(L"[Drag-to-Scroll] Ctrl released. Halting.");
                    g_state.store(ScrollState::Idle);
                    g_blockContextMenu.store(true);
                }
            }
            return false;
        }
    }

    // Break condition: Any mouse button click terminates cruising
    if (g_state.load() == ScrollState::Cruising) {
        if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN ||
            pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_XBUTTONDOWN) {
            Wh_Log(L"[Drag-to-Scroll] Mouse click detected during cruise. Halting.");
            g_state.store(ScrollState::Idle);
            g_blockContextMenu.store(true);
            return false;
        }
    }

    bool ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    // Right Button Down
    if (pMsg->message == WM_RBUTTONDOWN) {
        if (ctrlHeld) {
            Wh_Log(L"[Drag-to-Scroll] Ctrl + RightDown. State -> Pending");
            g_state = ScrollState::Pending;
            g_blockContextMenu.store(false);

            GetCursorPos(&g_anchorPt);
            g_lastPt = g_anchorPt;
            g_accumY = 0.0;

            HWND hRoot = GetAncestor(pMsg->hwnd, GA_ROOT);
            if (hRoot) {
                GetWindowRect(hRoot, &g_windowBounds);
            } else {
                GetWindowRect(pMsg->hwnd, &g_windowBounds);
            }
            return true;
        }
    }

    // Mouse Movement
    if (pMsg->message == WM_MOUSEMOVE) {
        ScrollState cur = g_state.load();

        if (cur == ScrollState::Pending || cur == ScrollState::Scrolling) {
            POINT curPt;
            GetCursorPos(&curPt);

            if (!PtInRect(&g_windowBounds, curPt)) {
                Wh_Log(L"[Drag-to-Scroll] Exited window bounds. Cancelling.");
                g_state.store(ScrollState::Idle);
                g_blockContextMenu.store(true);
                return false;
            }

            int totalDy = curPt.y - g_anchorPt.y;

            if (cur == ScrollState::Pending) {
                if (std::abs(totalDy) >= g_settings.deadzone) {
                    Wh_Log(L"[Drag-to-Scroll] Deadzone passed (%d px). State -> Scrolling", totalDy);
                    g_state = ScrollState::Scrolling;
                    g_blockContextMenu.store(true);
                    g_lastPt = curPt;
                }
            }

            if (g_state.load() == ScrollState::Scrolling) {
                int dy = curPt.y - g_lastPt.y;
                g_lastPt = curPt;

                if (dy != 0) {
                    double direction = g_settings.touch_style ? 1.0 : -1.0;

                    // Clamped, smooth velocity curve (max 2.5x to prevent overshoot)
                    double velocityFactor = 1.0;
                    if (g_settings.enable_velocity) {
                        int absDy = std::abs(dy);
                        if (absDy <= 2) {
                            velocityFactor = 0.40; // Precision line crawling
                        } else if (absDy <= 6) {
                            velocityFactor = 1.0;  // Standard speed
                        } else {
                            velocityFactor = std::min(2.5, 1.0 + ((absDy - 6) * 0.06)); // Clamped fast swipe
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
                        g_blockContextMenu.store(true);
                        DispatchScroll(notchesToEmit, curPt, dy);
                    }
                }
                return true;
            }
        }
    }

    // Right Button Up
    if (pMsg->message == WM_RBUTTONUP) {
        ScrollState cur = g_state.load();

        if (cur == ScrollState::Scrolling) {
            bool ctrlPhysicallyDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            g_blockContextMenu.store(true);

            if (g_settings.enable_cruising && ctrlPhysicallyDown) {
                Wh_Log(L"[Drag-to-Scroll] Right-Click lifted. Cruising engaged.");
                g_state.store(ScrollState::Cruising);
            } else {
                Wh_Log(L"[Drag-to-Scroll] Drag completed. State -> Idle");
                g_state.store(ScrollState::Idle);
            }
            return true;
        } 
        else if (cur == ScrollState::Pending) {
            Wh_Log(L"[Drag-to-Scroll] Released in deadzone. Replaying right-click.");
            g_state.store(ScrollState::Idle);
            g_blockContextMenu.store(false);

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
    Wh_Log(L"[Drag-to-Scroll] Initializing globally in PID: %lu", GetCurrentProcessId());
    LoadSettings();

    g_stopCruiseThread.store(false);
    g_hCruiseThread = CreateThread(nullptr, 0, CruiseWorkerThread, nullptr, 0, nullptr);

    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook, &pfnGetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook, &pfnPeekMessageW_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook, &pfnGetKeyState_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[Drag-to-Scroll] Unloading.");
    g_stopCruiseThread.store(true);
    if (g_hCruiseThread) {
        WaitForSingleObject(g_hCruiseThread, 200);
        CloseHandle(g_hCruiseThread);
        g_hCruiseThread = nullptr;
    }
}

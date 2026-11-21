// ==WindhawkMod==
// @id              yandex-music-transparency
// @name            Yandex Music Transparency
// @description     Automatically applies and enforces custom transparency for Yandex Music
// @version         1.2.8
// @author          NIKTO
// @github          https://github.com/NIKTO-2
// @include         Яндекс Музыка.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Yandex Music Transparency

Adds configurable transparency for the Yandex Music desktop application.

## Features

- Custom opacity for active and inactive windows
- Optional full opacity while the mouse hovers over the window
- Smooth animated transitions between opacity levels (can be disabled)
- Automatically enforces transparency even if the app tries to remove `WS_EX_LAYERED`

## Notes

- Only the main application window (class `Chrome_WidgetWin_1`) is modified
- Auxiliary Electron processes (`--type=*`) are ignored
- Windows that already use `LWA_COLORKEY` are not modified

*/
// ==/WindhawkModReadme==

// ==WindhawkModChangelog==
/*
## 1.2.8

- Performance: the polling timer now runs only while needed. It stops when
  neither hover polling (OpaqueOnHover) nor an opacity animation is active,
  instead of ticking forever at 25 ms.
- Fixed thread safety: the timer is now created and killed on the window's
  own thread via a registered control message, never from the Windhawk
  engine thread. g_timerOwner is only set after the timer actually started.

## 1.27

- Fixed transparency being broken: the function that applies the target alpha
  now ensures `WS_EX_LAYERED` is set first (and skips `LWA_COLORKEY`
  windows). Previously the layered style was never applied, so
  `SetLayeredWindowAttributes` failed and the window stayed opaque.

## 1.26

- Removed the `PlaybackAlpha` feature (title-based playback detection) — it
  could interfere with normal transparency. Smooth transitions remain.

## 1.25

- Added smooth animated transitions between opacity levels (toggleable).
- Added `PlaybackAlpha` — when a track is playing (title-based detection), a
  separate opacity value is used. `0` disables this feature.

## 1.24

- Removed all background blur effects (Blur / Acrylic / Mica). The mod now
  only provides window transparency.
*/
// ==/WindhawkModChangelog==

// ==WindhawkModSettings==
/*

- Alpha: 180
  $name: Active window opacity
  $description: Opacity level of the active window (50-255)
- InactiveAlpha: 0
  $name: Inactive window opacity
  $description: Opacity level of the inactive window. Set to 0 to use the active alpha value (50-255)
- OpaqueOnHover: false
  $name: Fully opaque on hover
  $description: Make the window fully opaque while the mouse hovers over it
- SmoothTransitions: true
  $name: Smooth transitions
  $description: Animate opacity changes instead of applying them instantly.

*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <wchar.h>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

struct Settings {
    int alpha;
    int inactiveAlpha;
    bool opaqueOnHover;
    bool smoothTransitions;
};

struct OriginalWindowState {
    bool hadLayeredStyle;
    BYTE originalAlpha;
    bool hasOriginalAlpha;
    bool isActive;
    bool isHovered;
    BYTE currentAlpha;      // currently applied alpha (0 = not initialized)
    BYTE targetAlpha;       // desired alpha
    bool alphaInitialized;
};

Settings g_settings;
std::mutex g_windowsMutex;
std::unordered_map<HWND, OriginalWindowState> g_trackedWindows;
bool g_unloading = false;

constexpr UINT_PTR kHoverTimerId = 0x4D545248;  // 'MTRH'
constexpr UINT kTimerIntervalFastMs = 25;      // animation tick
constexpr UINT kTimerIntervalSlowMs = 100;     // hover polling
constexpr int kAnimStep = 10;                  // alpha units per fast tick

// Registered message used to start/stop the timer on the window's own thread.
UINT g_timerControlMsg = 0;
HWND g_timerOwner = nullptr;

void LoadSettings() {
    g_settings.alpha = Wh_GetIntSetting(L"Alpha");
    if (g_settings.alpha < 50) g_settings.alpha = 50;
    if (g_settings.alpha > 255) g_settings.alpha = 255;

    g_settings.inactiveAlpha = Wh_GetIntSetting(L"InactiveAlpha");
    if (g_settings.inactiveAlpha != 0) {
        if (g_settings.inactiveAlpha < 50) g_settings.inactiveAlpha = 50;
        if (g_settings.inactiveAlpha > 255) g_settings.inactiveAlpha = 255;
    }

    g_settings.opaqueOnHover = Wh_GetIntSetting(L"OpaqueOnHover") != 0;

    g_settings.smoothTransitions = Wh_GetIntSetting(L"SmoothTransitions") != 0;
}

bool IsTargetWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    if (GetParent(hwnd) != NULL) return false;

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ||
        wcscmp(className, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }

    return true;
}

bool UsesColorKey(HWND hwnd) {
    DWORD flags = 0;
    if (GetLayeredWindowAttributes(hwnd, NULL, NULL, &flags)) {
        return (flags & LWA_COLORKEY) != 0;
    }
    return false;
}

BYTE ComputeTargetAlpha(const OriginalWindowState& state) {
    if (g_settings.opaqueOnHover && state.isHovered) {
        return 255;
    }
    if (!state.isActive && g_settings.inactiveAlpha > 0) {
        return (BYTE)g_settings.inactiveAlpha;
    }
    return (BYTE)g_settings.alpha;
}

using SetLayeredWindowAttributes_t = decltype(&SetLayeredWindowAttributes);
SetLayeredWindowAttributes_t SetLayeredWindowAttributes_Original;

BOOL WINAPI SetLayeredWindowAttributes_Hook(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) {
    BYTE newAlpha = bAlpha;
    if (!g_unloading && (dwFlags & LWA_ALPHA) && !(dwFlags & LWA_COLORKEY)) {
        bool shouldReplace = false;
        int computedAlpha = g_settings.alpha;
        {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            auto it = g_trackedWindows.find(hwnd);
            if (it != g_trackedWindows.end()) {
                if (!it->second.hasOriginalAlpha) {
                    it->second.originalAlpha = bAlpha;
                    it->second.hasOriginalAlpha = true;
                }
                shouldReplace = true;
                computedAlpha = ComputeTargetAlpha(it->second);
            }
        }
        if (shouldReplace) {
            newAlpha = (BYTE)computedAlpha;
        }
    }
    return SetLayeredWindowAttributes_Original(hwnd, crKey, newAlpha, dwFlags);
}

// Applies the target alpha. When smooth transitions are enabled, the actual
// value is only stored as target and approached gradually in the timer.
void ApplyTargetAlpha(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    // Ensure the window is layered first; otherwise SetLayeredWindowAttributes
    // fails and transparency never applies.
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        if (UsesColorKey(hwnd)) return;
    } else {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    std::lock_guard<std::mutex> lock(g_windowsMutex);
    auto it = g_trackedWindows.find(hwnd);
    if (it == g_trackedWindows.end()) return;

    BYTE target = ComputeTargetAlpha(it->second);

    if (!it->second.alphaInitialized || !g_settings.smoothTransitions) {
        it->second.currentAlpha = target;
        it->second.targetAlpha = target;
        it->second.alphaInitialized = true;
        SetLayeredWindowAttributes_Original(hwnd, 0, target, LWA_ALPHA);
    } else {
        it->second.targetAlpha = target;
        // Animation just started: switch the timer to the fast interval.
        if (target != it->second.currentAlpha) {
            SendMessageTimeoutW(hwnd, g_timerControlMsg, kTimerIntervalFastMs, 0,
                                SMTO_ABORTIFHUNG, 200, nullptr);
        }
    }
}

// Advances the animated alpha toward the target for all tracked windows.
// Returns true if the animation is still running (timer should continue).
bool AnimateAllWindows() {
    std::vector<HWND> toSet;
    bool anyRunning = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        for (auto& pair : g_trackedWindows) {
            auto& st = pair.second;
            if (!st.alphaInitialized) continue;
            if (st.currentAlpha == st.targetAlpha) continue;

            int delta = (int)st.targetAlpha - (int)st.currentAlpha;
            int step = kAnimStep;
            BYTE newAlpha;
            if (delta > step) {
                newAlpha = (BYTE)(st.currentAlpha + step);
            } else if (delta < -step) {
                newAlpha = (BYTE)(st.currentAlpha - step);
            } else {
                newAlpha = st.targetAlpha;
            }
            st.currentAlpha = newAlpha;
            if (newAlpha != st.targetAlpha) {
                anyRunning = true;
            }
            toSet.push_back(pair.first);
        }
    }

    for (HWND hwnd : toSet) {
        BYTE alpha = 0;
        {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            auto it = g_trackedWindows.find(hwnd);
            if (it == g_trackedWindows.end()) continue;
            alpha = it->second.currentAlpha;
        }
        SetLayeredWindowAttributes_Original(hwnd, 0, alpha, LWA_ALPHA);
    }

    return anyRunning;
}

void UpdateHoverStateFromCursor() {
    POINT pt;
    if (!GetCursorPos(&pt)) return;

    HWND under = WindowFromPoint(pt);
    HWND topLevel = under ? GetAncestor(under, GA_ROOT) : nullptr;

    std::vector<HWND> toApply;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        for (auto& pair : g_trackedWindows) {
            bool nowHovered = (pair.first == topLevel);
            if (pair.second.isHovered != nowHovered) {
                pair.second.isHovered = nowHovered;
                toApply.push_back(pair.first);
            }
        }
    }
    // The hover state only affects the target when OpaqueOnHover is on.
    if (g_settings.opaqueOnHover) {
        for (HWND hwnd : toApply) {
            ApplyTargetAlpha(hwnd);
        }
    }
}

void RestoreWindow(HWND hwnd, const OriginalWindowState& state) {
    if (!IsWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!state.hadLayeredStyle) {
        if ((exStyle & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
            SetLayeredWindowAttributes_Original(hwnd, 0, 255, LWA_ALPHA);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    } else if (state.hasOriginalAlpha && (exStyle & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
        SetLayeredWindowAttributes_Original(hwnd, 0, state.originalAlpha, LWA_ALPHA);
    }
}

LRESULT CALLBACK WindowSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    // Timer control message: handled on the window's own thread, so SetTimer/
    // KillTimer are always called on the thread that owns the window.
    // wParam != 0 -> interval in ms (start), wParam == 0 -> stop.
    if (uMsg == g_timerControlMsg) {
        if (wParam) {
            SetTimer(hwnd, kHoverTimerId, (UINT)wParam, nullptr);
        } else {
            KillTimer(hwnd, kHoverTimerId);
        }
        // Nonzero return so SendMessageTimeout can distinguish success.
        return 1;
    }

    switch (uMsg) {
        case WM_STYLECHANGING: {
            if (!g_unloading && wParam == GWL_EXSTYLE) {
                STYLESTRUCT* ss = (STYLESTRUCT*)lParam;
                if (ss && !(ss->styleNew & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
                    ss->styleNew |= WS_EX_LAYERED;
                }
            }
            break;
        }
        case WM_STYLECHANGED: {
            if (!g_unloading && wParam == GWL_EXSTYLE) {
                ApplyTargetAlpha(hwnd);
            }
            break;
        }
        case WM_ACTIVATE: {
            if (!g_unloading) {
                bool isActive = (LOWORD(wParam) != WA_INACTIVE);
                bool changed = false;
                {
                    std::lock_guard<std::mutex> lock(g_windowsMutex);
                    auto it = g_trackedWindows.find(hwnd);
                    if (it != g_trackedWindows.end() && it->second.isActive != isActive) {
                        it->second.isActive = isActive;
                        changed = true;
                    }
                }
                if (changed) {
                    ApplyTargetAlpha(hwnd);
                }
            }
            break;
        }
        case WM_TIMER: {
            if (!g_unloading && wParam == kHoverTimerId && hwnd == g_timerOwner) {
                // Hover polling on the slow interval, animation on the fast one.
                if (g_settings.opaqueOnHover) {
                    UpdateHoverStateFromCursor();
                }
                bool stillAnimating = AnimateAllWindows();

                // Stop the timer when neither hover polling nor animation is
                // needed anymore.
                if (!g_settings.opaqueOnHover && !stillAnimating) {
                    KillTimer(hwnd, kHoverTimerId);
                } else if (g_settings.opaqueOnHover && !stillAnimating) {
                    // Hover-only: slow down to the slow interval.
                    SetTimer(hwnd, kHoverTimerId, kTimerIntervalSlowMs, nullptr);
                }
                return 0;
            }
            break;
        }
        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_windowsMutex);
                g_trackedWindows.erase(hwnd);
                if (g_timerOwner == hwnd) {
                    g_timerOwner = nullptr;
                }
            }
            break;
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachToWindow(HWND hwnd) {
    if (g_unloading || !IsTargetWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) && UsesColorKey(hwnd)) {
        return;
    }

    bool alreadyTracked = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (g_trackedWindows.count(hwnd)) {
            alreadyTracked = true;
        } else {
            OriginalWindowState state{};
            state.hadLayeredStyle = (exStyle & WS_EX_LAYERED) != 0;
            state.isActive = (GetForegroundWindow() == hwnd);
            state.isHovered = false;
            if (state.hadLayeredStyle) {
                BYTE alpha = 255;
                DWORD flags = 0;
                if (GetLayeredWindowAttributes(hwnd, NULL, &alpha, &flags) && (flags & LWA_ALPHA)) {
                    state.originalAlpha = alpha;
                    state.hasOriginalAlpha = true;
                }
            }
            g_trackedWindows[hwnd] = state;
        }
    }

    if (alreadyTracked) {
        ApplyTargetAlpha(hwnd);
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, WindowSubclassProc, 0)) {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        g_trackedWindows.erase(hwnd);
        return;
    }

    // Start the timer on the window's own thread via the registered control
    // message, but only when it is actually needed (hover polling requires the
    // slow timer; animations start the fast timer themselves). Without this,
    // no timer runs at all.
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (!g_timerOwner && g_settings.opaqueOnHover) {
            LRESULT res = SendMessageTimeoutW(hwnd, g_timerControlMsg,
                                              kTimerIntervalSlowMs, 0,
                                              SMTO_ABORTIFHUNG, 200, nullptr);
            if (res != 0) {
                // SendMessageTimeout succeeded; SetTimer ran on the window's
                // thread. Only now claim ownership.
                g_timerOwner = hwnd;
            } else {
                Wh_Log(L"Sending timer start to window failed, error %d", (int)GetLastError());
            }
        }
    }

    ApplyTargetAlpha(hwnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hwnd && !g_unloading && hWndParent == NULL) {
        AttachToWindow(hwnd);
    }

    return hwnd;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    AttachToWindow(hwnd);
    return TRUE;
}

BOOL Wh_ModInit() {
    LPCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && wcsstr(cmdLine, L"--type=") != NULL) {
        return FALSE;
    }

    g_timerControlMsg = RegisterWindowMessageW(L"Windhawk_YandexMusicTransparency_TimerControl");
    if (g_timerControlMsg == 0) {
        Wh_Log(L"RegisterWindowMessage failed, error %d", (int)GetLastError());
        return FALSE;
    }

    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original)) {
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)SetLayeredWindowAttributes, (void*)SetLayeredWindowAttributes_Hook, (void**)&SetLayeredWindowAttributes_Original)) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        windows.reserve(g_trackedWindows.size());
        for (const auto& pair : g_trackedWindows) {
            windows.push_back(pair.first);
        }
    }

    // If hover polling just became needed, make sure a slow timer is running.
    if (g_settings.opaqueOnHover) {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (!g_timerOwner) {
            for (HWND hwnd : windows) {
                LRESULT res = SendMessageTimeoutW(hwnd, g_timerControlMsg,
                                                  kTimerIntervalSlowMs, 0,
                                                  SMTO_ABORTIFHUNG, 200, nullptr);
                if (res != 0) {
                    g_timerOwner = hwnd;
                    break;
                }
            }
        }
    }

    for (HWND hwnd : windows) {
        ApplyTargetAlpha(hwnd);
    }
}

void Wh_ModUninit() {
    g_unloading = true;

    HWND timerOwner = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        timerOwner = g_timerOwner;
        g_timerOwner = nullptr;
    }
    // Stop the timer on the window's own thread before removing the subclass.
    if (timerOwner && IsWindow(timerOwner)) {
        SendMessageTimeoutW(timerOwner, g_timerControlMsg, 0, 0,
                            SMTO_ABORTIFHUNG, 200, nullptr);
    }

    std::vector<std::pair<HWND, OriginalWindowState>> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        windows.assign(g_trackedWindows.begin(), g_trackedWindows.end());
        g_trackedWindows.clear();
    }

    for (const auto& entry : windows) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(entry.first, WindowSubclassProc);
        RestoreWindow(entry.first, entry.second);
    }
}

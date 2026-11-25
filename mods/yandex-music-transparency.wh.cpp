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
- Optional 100% opacity when the window is maximized or fullscreen (alpha)
- Smooth animated transitions between opacity levels (can be disabled)
- Automatically enforces transparency even if the app tries to remove `WS_EX_LAYERED`

## Universal for Chromium apps

This mod is universal: it works with any Chromium / Electron application
(the main window class is `Chrome_WidgetWin_1`). Although it is tailored for
Yandex Music by default, you can apply the same persistent transparency to
other Chromium/Electron apps by adding their executable names in the mod's
**Advanced** tab under **Custom process inclusion list**.

## Notes

- Only the main application window (class `Chrome_WidgetWin_1`) is modified
- Auxiliary Electron processes (`--type=*`) are ignored
- Windows that already use `LWA_COLORKEY` are not modified

*/
// ==/WindhawkModReadme==

// ==WindhawkModChangelog==
/*
## 1.2.8

- Added configurable opacity for active and inactive windows
  (`Alpha` / `InactiveAlpha`).
- Added "fully opaque on hover" option (`OpaqueOnHover`), which now works
  with both SmoothTransitions settings.
- Added "fully opaque when maximized/fullscreen" option
  (`OpaqueWhenMaximized`), applied immediately to avoid clashing with the
  system resize animation.
- Added smooth animated transitions between opacity levels
  (`SmoothTransitions`).
- Defaults: active opacity 220, inactive opacity 170, smooth transitions on,
  fully opaque on hover off.
- Documented that the mod is universal and works with any Chromium / Electron
  application.
- Fixed the layered-window style (`WS_EX_LAYERED`) always being applied, so
  transparency reliably works even after the mod is loaded into a running app.
- Fixed restoring the original alpha on unload (the window's true original
  opacity is now captured once).
- Each tracked window manages its own timer, which runs only while needed
  (hover polling or an animation); timers are controlled on the window's own
  thread, and the control message is never sent while holding the mutex.
*/
// ==/WindhawkModChangelog==

// ==WindhawkModSettings==
/*
- Alpha: 220
  $name: Active window opacity
  $description: Opacity level of the active window (50-255)
- InactiveAlpha: 170
  $name: Inactive window opacity
  $description: Opacity level of the inactive window. Set to 0 to use the active alpha value (50-255)
- OpaqueOnHover: false
  $name: Fully opaque on hover
  $description: Make the window fully opaque while the mouse hovers over it
- OpaqueWhenMaximized: false
  $name: Fully opaque when maximized (alpha)
  $description: alpha v1.2.8 - Make the window 100% opaque when it is maximized or fullscreen
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
    bool opaqueWhenMaximized;
    bool smoothTransitions;
};

struct OriginalWindowState {
    bool hadLayeredStyle;
    BYTE originalAlpha;
    bool hasOriginalAlpha;
    bool isActive;
    bool isHovered;
    bool wasMaximized;
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

// Registered message used to ask the window's own thread to update its timer.
UINT g_timerControlMsg = 0;

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

    g_settings.opaqueWhenMaximized = Wh_GetIntSetting(L"OpaqueWhenMaximized") != 0;

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

// [alpha v1.2.8] Fully opaque when maximized: true when the window is
// maximized or covers the full work area (fullscreen).
bool IsMaximizedOrFullscreen(HWND hwnd) {
    if (IsZoomed(hwnd)) {
        return true;
    }

    RECT wndRect;
    if (!GetWindowRect(hwnd, &wndRect)) {
        return false;
    }
    if (wndRect.right - wndRect.left <= 0 || wndRect.bottom - wndRect.top <= 0) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    return (wndRect.left <= mi.rcMonitor.left &&
            wndRect.top <= mi.rcMonitor.top &&
            wndRect.right >= mi.rcMonitor.right &&
            wndRect.bottom >= mi.rcMonitor.bottom);
}

BYTE ComputeTargetAlpha(HWND hwnd, const OriginalWindowState& state) {
    if (g_settings.opaqueWhenMaximized && IsMaximizedOrFullscreen(hwnd)) {
        return 255;
    }
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
                computedAlpha = ComputeTargetAlpha(hwnd, it->second);
            }
        }
        if (shouldReplace) {
            newAlpha = (BYTE)computedAlpha;
        }
    }
    return SetLayeredWindowAttributes_Original(hwnd, crKey, newAlpha, dwFlags);
}

// Runs on the window's own thread: fast while animating, slow for hover
// polling, otherwise stopped.
void UpdateTimer(HWND hwnd) {
    bool animating = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_trackedWindows.find(hwnd);
        animating = it != g_trackedWindows.end() &&
                    it->second.currentAlpha != it->second.targetAlpha;
    }

    if (g_unloading) {
        KillTimer(hwnd, kHoverTimerId);
    } else if (animating) {
        SetTimer(hwnd, kHoverTimerId, kTimerIntervalFastMs, nullptr);
    } else if (g_settings.opaqueOnHover) {
        SetTimer(hwnd, kHoverTimerId, kTimerIntervalSlowMs, nullptr);
    } else {
        KillTimer(hwnd, kHoverTimerId);
    }
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

    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_trackedWindows.find(hwnd);
        if (it == g_trackedWindows.end()) return;

        BYTE target = ComputeTargetAlpha(hwnd, it->second);

        // [alpha v1.2.8] Fully opaque when maximized: apply immediately when
        // transitioning into or out of the maximized / fullscreen state with
        // OpaqueWhenMaximized on: animating simultaneously with the system
        // resize animation looks like overlapping flicker.
        bool isMaximized = (g_settings.opaqueWhenMaximized &&
                            IsMaximizedOrFullscreen(hwnd));
        bool maximizedStateChanged = (it->second.wasMaximized != isMaximized);
        it->second.wasMaximized = isMaximized;

        bool instant = maximizedStateChanged ||
                       !it->second.alphaInitialized ||
                       !g_settings.smoothTransitions;

        if (instant) {
            it->second.currentAlpha = target;
            it->second.targetAlpha = target;
            it->second.alphaInitialized = true;
            SetLayeredWindowAttributes_Original(hwnd, 0, target, LWA_ALPHA);
        } else {
            it->second.targetAlpha = target;
        }
    }

    // Always ask the window's own thread to update its timer (start/stop/interval).
    // This makes hover polling work regardless of SmoothTransitions: UpdateTimer
    // runs the slow timer while OpaqueOnHover is on, the fast one while
    // animating, and kills it otherwise. Sent outside the mutex (UpdateTimer
    // takes the mutex, so sending under the lock would deadlock).
    SendMessageTimeoutW(hwnd, g_timerControlMsg, 0, 0,
                        SMTO_ABORTIFHUNG, 200, nullptr);
}

// Advances the animated alpha one step for this window.
void AnimateWindow(HWND hwnd) {
    BYTE newAlpha = 0;
    bool needSet = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_trackedWindows.find(hwnd);
        if (it == g_trackedWindows.end()) return;
        auto& st = it->second;
        if (!st.alphaInitialized || st.currentAlpha == st.targetAlpha) return;

        int delta = (int)st.targetAlpha - (int)st.currentAlpha;
        int step = kAnimStep;
        if (delta > step) {
            newAlpha = (BYTE)(st.currentAlpha + step);
        } else if (delta < -step) {
            newAlpha = (BYTE)(st.currentAlpha - step);
        } else {
            newAlpha = st.targetAlpha;
        }
        st.currentAlpha = newAlpha;
        needSet = true;
    }

    if (needSet) {
        SetLayeredWindowAttributes_Original(hwnd, 0, newAlpha, LWA_ALPHA);
    }
}

// Updates the hover state for a single window.
void UpdateHoverState(HWND hwnd) {
    POINT pt;
    if (!GetCursorPos(&pt)) return;

    HWND under = WindowFromPoint(pt);
    HWND topLevel = under ? GetAncestor(under, GA_ROOT) : nullptr;
    bool nowHovered = (topLevel == hwnd);

    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_trackedWindows.find(hwnd);
        if (it != g_trackedWindows.end() && it->second.isHovered != nowHovered) {
            it->second.isHovered = nowHovered;
            changed = true;
        }
    }

    if (changed) {
        ApplyTargetAlpha(hwnd);
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
    // Timer control message: handled on the window's own thread. UpdateTimer
    // picks the interval from this window's state (no parameter needed).
    if (uMsg == g_timerControlMsg) {
        UpdateTimer(hwnd);
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
        case WM_WINDOWPOSCHANGED: {
            if (!g_unloading && g_settings.opaqueWhenMaximized) {
                ApplyTargetAlpha(hwnd);
            }
            break;
        }
        case WM_TIMER: {
            if (wParam == kHoverTimerId) {
                if (!g_unloading) {
                    if (g_settings.opaqueOnHover) {
                        UpdateHoverState(hwnd);
                    }
                    AnimateWindow(hwnd);
                }
                UpdateTimer(hwnd);
                return 0;
            }
            break;
        }
        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_windowsMutex);
                g_trackedWindows.erase(hwnd);
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

    // ApplyTargetAlpha schedules the timer update on the window's own thread,
    // so no explicit timer management is needed here.
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

    // ApplyTargetAlpha sends the timer-control message for each window, so
    // hover polling / animation timers are (re)started or stopped as needed.
    for (HWND hwnd : windows) {
        ApplyTargetAlpha(hwnd);
    }
}

void Wh_ModUninit() {
    g_unloading = true;

    std::vector<std::pair<HWND, OriginalWindowState>> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        windows.assign(g_trackedWindows.begin(), g_trackedWindows.end());
        g_trackedWindows.clear();
    }

    // Stop each window's timer on its own thread (g_unloading is set, so
    // UpdateTimer kills it) before removing the subclass.
    for (const auto& entry : windows) {
        SendMessageTimeoutW(entry.first, g_timerControlMsg, 0, 0,
                            SMTO_ABORTIFHUNG, 200, nullptr);
    }

    for (const auto& entry : windows) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(entry.first, WindowSubclassProc);
        RestoreWindow(entry.first, entry.second);
    }
}

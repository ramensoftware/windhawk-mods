// ==WindhawkMod==
// @id              scroll-window-transparency
// @name            Scroll Window Transparency
// @description     Hold a customizable modifier key or mouse button and scroll over any window to adjust its transparency
// @version         1.0.0
// @author          notactuallyoak
// @github          https://github.com/notactuallyoak
// @include         windhawk.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Scroll Window Transparency

Hold your chosen **modifier key** (or mouse side button) and scroll the mouse
wheel over any window to smoothly adjust its opacity.

## Relation to `scroll-window-opacity`
This mod shares the core modifier-plus-scroll concept of
[Scroll Window Opacity](https://windhawk.net/mods/scroll-window-opacity) by
Sondre Myrmel, and adds several extra workflow options:
- **Additional Triggers:** Adds support for the `Win` key and mouse side
  buttons (`Mouse Button 4` / `Mouse Button 5`) alongside `Alt`, `Ctrl`,
  `Shift`, `Ctrl + Alt`, `Ctrl + Shift`, and `Alt + Shift`.
- **Quick Reset (`Modifier + Middle-Click`):** Hold your trigger modifier and
  middle-click any window to immediately restore its original opacity.
- **Titlebar-Only Mode:** Optional setting to restrict scrolling to the top
  titlebar region so normal scrolling inside documents or browsers is unaffected.
- **Menu Bar / Start Menu Suppression:** Masks `Alt` and `Win` key releases
  after scrolling so releasing the modifier does not focus the window's menu
  bar or open the Start Menu.

## Settings Reference
| Setting | Default | Description |
| :--- | :---: | :--- |
| **Trigger Keybind (Hold + Scroll)** | `Alt` | Key combination or mouse side button required while scrolling. |
| **Opacity Step (%)** | `10` | Transparency change per full scroll wheel notch (`1`–`50`). |
| **Minimum Opacity (%)** | `25` | Lowest allowed opacity (`10`–`90`). |
| **Titlebar Only** | `false` | Restrict scrolling to the top titlebar area instead of the entire window. |
| **Modifier + Middle-Click Resets Opacity** | `true` | Instantly restore original opacity when middle-clicking with the modifier held. |

## Notes on Shortcut Conflicts
Depending on your chosen trigger, holding a modifier while scrolling overrides
that shortcut over eligible windows (for example, `Ctrl + Scroll` zooms in
browsers, `Shift + Scroll` scrolls horizontally, and `Alt + Scroll` fast-scrolls
in some editors). Enable **Titlebar Only** or choose `Ctrl + Alt` / mouse side
buttons if you want to keep in-app modifier scrolling intact.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- triggerModifier: alt
  $name: Trigger Keybind (Hold + Scroll)
  $description: Key or mouse button to hold while scrolling the wheel over a window.
  $options:
  - alt: Alt
  - ctrl: Ctrl
  - shift: Shift
  - win: Win
  - ctrl_alt: Ctrl + Alt
  - ctrl_shift: Ctrl + Shift
  - alt_shift: Alt + Shift
  - xbutton1: Mouse Button 4 (Back)
  - xbutton2: Mouse Button 5 (Forward)
- alphaStepPercent: 10
  $name: Opacity Step (%)
  $description: Opacity change per scroll notch (1 to 50).
- minAlphaPercent: 25
  $name: Minimum Opacity (%)
  $description: Minimum allowed window opacity (10 to 90).
- titlebarOnly: false
  $name: Titlebar Only
  $description: Only trigger when the cursor is over the window's titlebar.
- middleClickReset: true
  $name: Modifier + Middle-Click Resets Opacity
  $description: Hold the trigger modifier and middle-click a window to restore its original opacity.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

constexpr UINT WM_APP_ADJUST_OPACITY = WM_APP + 1;
constexpr UINT WM_APP_RESET_OPACITY  = WM_APP + 2;

// Unassigned virtual key used to mask Alt/Win release from triggering menu bar or Start menu
constexpr WORD kMenuMaskVk = 0xE8;

enum class TriggerModifier : int {
    Alt,
    Ctrl,
    Shift,
    Win,
    CtrlAlt,
    CtrlShift,
    AltShift,
    XButton1,
    XButton2
};

struct {
    std::atomic<TriggerModifier> modifier{TriggerModifier::Alt};
    std::atomic<int> alphaStep{26};
    std::atomic<int> minAlpha{64};
    std::atomic<bool> titlebarOnly{false};
    std::atomic<bool> middleClickReset{true};
} g_settings;

struct WindowOpacityState {
    bool wasInitiallyLayered = false;
    DWORD origFlags = 0;
    COLORREF origColorKey = 0;
    BYTE origAlpha = 255;
    BYTE currentAlpha = 255;
    int wheelRemainder = 0;
};

// Accessed exclusively on g_workerThread (and in RestoreAllWindows after g_workerThread joins)
std::unordered_map<HWND, WindowOpacityState> g_trackedWindows;

// Protected by g_blockedMutex; lock is only ever held for fast memory lookups/inserts
std::mutex g_blockedMutex;
std::unordered_set<HWND> g_blockedWindows;

HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HANDLE g_hookThreadReady = nullptr;
HHOOK g_mouseHook = nullptr;
HHOOK g_keyboardHook = nullptr;

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_workerThreadReady = nullptr;

std::atomic<bool> g_gestureTriggered{false};

HMODULE GetCurrentModuleHandle() {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        &hMod
    );
    return hMod;
}

UINT GetMonitorDpiForWindowSafe(HWND hWnd) {
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (hMon) {
        static HMODULE hShcore = LoadLibraryW(L"shcore.dll");
        if (hShcore) {
            using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
            static auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
                GetProcAddress(hShcore, "GetDpiForMonitor"));
            if (pGetDpiForMonitor) {
                UINT dpiX = 96, dpiY = 96;
                if (SUCCEEDED(pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY)) && dpiX > 0) {
                    return dpiX;
                }
            }
        }
    }
    return 96;
}

int GetCaptionHeightForWindow(HWND hWnd) {
    UINT dpi = GetMonitorDpiForWindowSafe(hWnd);
    static auto pGetSystemMetricsForDpi = reinterpret_cast<int(WINAPI*)(int, UINT)>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetSystemMetricsForDpi"));
    if (pGetSystemMetricsForDpi) {
        return pGetSystemMetricsForDpi(SM_CYCAPTION, dpi) +
               (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) * 2);
    }
    return MulDiv(36, dpi, 96);
}

void LoadSettings() {
    WindhawkUtils::StringSetting modStr = WindhawkUtils::StringSetting::make(L"triggerModifier");
    TriggerModifier parsedMod = TriggerModifier::Alt;
    if (wcscmp(modStr.get(), L"ctrl") == 0) {
        parsedMod = TriggerModifier::Ctrl;
    } else if (wcscmp(modStr.get(), L"shift") == 0) {
        parsedMod = TriggerModifier::Shift;
    } else if (wcscmp(modStr.get(), L"win") == 0) {
        parsedMod = TriggerModifier::Win;
    } else if (wcscmp(modStr.get(), L"ctrl_alt") == 0) {
        parsedMod = TriggerModifier::CtrlAlt;
    } else if (wcscmp(modStr.get(), L"ctrl_shift") == 0) {
        parsedMod = TriggerModifier::CtrlShift;
    } else if (wcscmp(modStr.get(), L"alt_shift") == 0) {
        parsedMod = TriggerModifier::AltShift;
    } else if (wcscmp(modStr.get(), L"xbutton1") == 0) {
        parsedMod = TriggerModifier::XButton1;
    } else if (wcscmp(modStr.get(), L"xbutton2") == 0) {
        parsedMod = TriggerModifier::XButton2;
    }

    int stepPct = std::clamp(Wh_GetIntSetting(L"alphaStepPercent"), 1, 50);
    int minPct  = std::clamp(Wh_GetIntSetting(L"minAlphaPercent"), 10, 90);

    g_settings.modifier.store(parsedMod, std::memory_order_relaxed);
    g_settings.alphaStep.store(static_cast<int>(std::round((stepPct / 100.0) * 255.0)), std::memory_order_relaxed);
    g_settings.minAlpha.store(static_cast<int>(std::round((minPct / 100.0) * 255.0)), std::memory_order_relaxed);
    g_settings.titlebarOnly.store(Wh_GetIntSetting(L"titlebarOnly") != 0, std::memory_order_relaxed);
    g_settings.middleClickReset.store(Wh_GetIntSetting(L"middleClickReset") != 0, std::memory_order_relaxed);
}

bool IsTriggerModifierPressed() {
    bool alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool win   = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
                 ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

    switch (g_settings.modifier.load(std::memory_order_relaxed)) {
        case TriggerModifier::Alt:       return alt && !ctrl && !shift && !win;
        case TriggerModifier::Ctrl:      return ctrl && !alt && !shift && !win;
        case TriggerModifier::Shift:     return shift && !alt && !ctrl && !win;
        case TriggerModifier::Win:       return win && !alt && !ctrl && !shift;
        case TriggerModifier::CtrlAlt:   return ctrl && alt && !shift && !win;
        case TriggerModifier::CtrlShift: return ctrl && shift && !alt && !win;
        case TriggerModifier::AltShift:  return alt && shift && !ctrl && !win;
        case TriggerModifier::XButton1:  return (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
        case TriggerModifier::XButton2:  return (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
    }
    return false;
}

bool IsWindowKnownBlocked(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_blockedMutex);
    return g_blockedWindows.count(hWnd) > 0;
}

void AddBlockedWindow(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_blockedMutex);
    for (auto it = g_blockedWindows.begin(); it != g_blockedWindows.end();) {
        if (!IsWindow(*it)) {
            it = g_blockedWindows.erase(it);
        } else {
            ++it;
        }
    }
    g_blockedWindows.insert(hWnd);
}

bool IsValidTargetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
        return false;
    }

    if (IsWindowKnownBlocked(hWnd)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) || (exStyle & WS_EX_NOACTIVATE)) {
        return false;
    }

    if (!(style & (WS_CAPTION | WS_THICKFRAME))) {
        return false;
    }

    wchar_t className[64] = {0};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
        if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(className, L"Progman") == 0 ||
            wcscmp(className, L"WorkerW") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
            return false;
        }
    }

    return true;
}

// Fast non-blocking titlebar bounds check safe for use inside WH_MOUSE_LL
bool IsCursorInTitlebarBounds(HWND hWnd, POINT pt) {
    RECT rc = {0};
    if (!GetWindowRect(hWnd, &rc)) {
        return false;
    }
    int captionHeight = GetCaptionHeightForWindow(hWnd);
    return (pt.x >= rc.left && pt.x <= rc.right &&
            pt.y >= rc.top && pt.y <= rc.top + captionHeight);
}

void PruneStaleTrackedHandles() {
    for (auto it = g_trackedWindows.begin(); it != g_trackedWindows.end();) {
        if (!IsWindow(it->first)) {
            it = g_trackedWindows.erase(it);
        } else {
            ++it;
        }
    }
}

void RestoreWindowState(HWND hWnd, const WindowOpacityState& state) {
    if (!IsWindow(hWnd)) {
        return;
    }

    if (state.wasInitiallyLayered) {
        SetLayeredWindowAttributes(hWnd, state.origColorKey, state.origAlpha, state.origFlags);
    } else {
        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
    }
}

void WorkerResetWindowOpacity(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    PruneStaleTrackedHandles();

    auto it = g_trackedWindows.find(hWnd);
    if (it != g_trackedWindows.end()) {
        WindowOpacityState saved = it->second;
        g_trackedWindows.erase(it);
        RestoreWindowState(hWnd, saved);
        return;
    }

    // Fallback if window was left translucent by a previous session; preserve WS_EX_LAYERED and flags
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        COLORREF key = 0;
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hWnd, &key, &alpha, &flags) && (flags & LWA_ALPHA) && alpha < 255) {
            SetLayeredWindowAttributes(hWnd, key, 255, flags);
        }
    }
}

void WorkerAdjustWindowOpacity(HWND hWnd, int wheelDelta) {
    if (!hWnd || !IsWindow(hWnd) || IsWindowKnownBlocked(hWnd)) {
        return;
    }

    PruneStaleTrackedHandles();

    auto it = g_trackedWindows.find(hWnd);
    if (it == g_trackedWindows.end()) {
        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        WindowOpacityState initialState{};

        if (exStyle & WS_EX_LAYERED) {
            COLORREF key = 0;
            BYTE alpha = 255;
            DWORD flags = 0;
            if (!GetLayeredWindowAttributes(hWnd, &key, &alpha, &flags)) {
                // Window uses UpdateLayeredWindow; do not touch it
                AddBlockedWindow(hWnd);
                return;
            }
            initialState.wasInitiallyLayered = true;
            initialState.origFlags = flags;
            initialState.origColorKey = key;
            initialState.origAlpha = alpha;
            initialState.currentAlpha = (flags & LWA_ALPHA) ? alpha : 255;
        } else {
            initialState.wasInitiallyLayered = false;
            initialState.origAlpha = 255;
            initialState.currentAlpha = 255;
        }

        it = g_trackedWindows.emplace(hWnd, initialState).first;
    }

    WindowOpacityState& state = it->second;
    state.wheelRemainder += wheelDelta;
    int steps = state.wheelRemainder / WHEEL_DELTA;
    if (steps == 0) {
        return;
    }
    state.wheelRemainder %= WHEEL_DELTA;

    int stepSize = g_settings.alphaStep.load(std::memory_order_relaxed);
    int minAlpha = g_settings.minAlpha.load(std::memory_order_relaxed);
    int targetAlpha = std::clamp(
        static_cast<int>(state.currentAlpha) + (steps * stepSize),
        minAlpha,
        255
    );

    if (targetAlpha == state.currentAlpha) {
        return;
    }

    if (targetAlpha >= 255 && !state.wasInitiallyLayered) {
        WindowOpacityState saved = state;
        g_trackedWindows.erase(it);
        RestoreWindowState(hWnd, saved);
        return;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool addedLayered = false;
    if (!(exStyle & WS_EX_LAYERED)) {
        SetLastError(0);
        if (SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED) == 0 && GetLastError() != 0) {
            g_trackedWindows.erase(it);
            AddBlockedWindow(hWnd);
            return;
        }
        addedLayered = true;
    }

    DWORD applyFlags = LWA_ALPHA;
    COLORREF applyKey = 0;
    if (state.wasInitiallyLayered && (state.origFlags & LWA_COLORKEY)) {
        applyFlags |= LWA_COLORKEY;
        applyKey = state.origColorKey;
    }

    if (!SetLayeredWindowAttributes(hWnd, applyKey, static_cast<BYTE>(targetAlpha), applyFlags)) {
        if (addedLayered) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle);
        }
        g_trackedWindows.erase(it);
        AddBlockedWindow(hWnd);
        return;
    }

    state.currentAlpha = static_cast<BYTE>(targetAlpha);
}

void InjectMenuMaskKey() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = kMenuMaskVk;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = kMenuMaskVk;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && lParam && g_gestureTriggered.load(std::memory_order_relaxed)) {
        const KBDLLHOOKSTRUCT* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            TriggerModifier mod = g_settings.modifier.load(std::memory_order_relaxed);
            bool isAlt   = (kb->vkCode == VK_LMENU || kb->vkCode == VK_RMENU || kb->vkCode == VK_MENU);
            bool isWin   = (kb->vkCode == VK_LWIN || kb->vkCode == VK_RWIN);
            bool isCtrl  = (kb->vkCode == VK_LCONTROL || kb->vkCode == VK_RCONTROL || kb->vkCode == VK_CONTROL);
            bool isShift = (kb->vkCode == VK_LSHIFT || kb->vkCode == VK_RSHIFT || kb->vkCode == VK_SHIFT);

            // For combinations containing Alt, keep g_gestureTriggered set until Alt itself is released
            switch (mod) {
                case TriggerModifier::Alt:
                case TriggerModifier::CtrlAlt:
                case TriggerModifier::AltShift:
                    if (isAlt) {
                        g_gestureTriggered.store(false, std::memory_order_relaxed);
                        InjectMenuMaskKey();
                    }
                    break;
                case TriggerModifier::Win:
                    if (isWin) {
                        g_gestureTriggered.store(false, std::memory_order_relaxed);
                        InjectMenuMaskKey();
                    }
                    break;
                case TriggerModifier::Ctrl:
                    if (isCtrl) {
                        g_gestureTriggered.store(false, std::memory_order_relaxed);
                    }
                    break;
                case TriggerModifier::Shift:
                    if (isShift) {
                        g_gestureTriggered.store(false, std::memory_order_relaxed);
                    }
                    break;
                case TriggerModifier::CtrlShift:
                    if (isCtrl || isShift) {
                        g_gestureTriggered.store(false, std::memory_order_relaxed);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && lParam) {
        const MSLLHOOKSTRUCT* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        TriggerModifier mod = g_settings.modifier.load(std::memory_order_relaxed);

        if (wParam == WM_XBUTTONUP && g_gestureTriggered.load(std::memory_order_relaxed)) {
            WORD xButton = HIWORD(ms->mouseData);
            if ((mod == TriggerModifier::XButton1 && xButton == XBUTTON1) ||
                (mod == TriggerModifier::XButton2 && xButton == XBUTTON2)) {
                g_gestureTriggered.store(false, std::memory_order_relaxed);
                return 1;
            }
        }

        if (wParam == WM_MOUSEWHEEL && IsTriggerModifierPressed()) {
            HWND hPoint = WindowFromPoint(ms->pt);
            HWND hRoot = hPoint ? GetAncestor(hPoint, GA_ROOT) : nullptr;

            if (IsValidTargetWindow(hRoot)) {
                bool titlebarOnly = g_settings.titlebarOnly.load(std::memory_order_relaxed);
                if (!titlebarOnly || IsCursorInTitlebarBounds(hRoot, ms->pt)) {
                    short wheelDelta = static_cast<short>(HIWORD(ms->mouseData));
                    if (PostThreadMessageW(g_workerThreadId, WM_APP_ADJUST_OPACITY,
                                           reinterpret_cast<WPARAM>(hRoot),
                                           static_cast<LPARAM>(wheelDelta))) {
                        g_gestureTriggered.store(true, std::memory_order_relaxed);
                        return 1;
                    }
                }
            }
        }

        if (wParam == WM_MBUTTONDOWN &&
            g_settings.middleClickReset.load(std::memory_order_relaxed) &&
            IsTriggerModifierPressed()) {
            HWND hPoint = WindowFromPoint(ms->pt);
            HWND hRoot = hPoint ? GetAncestor(hPoint, GA_ROOT) : nullptr;

            if (IsValidTargetWindow(hRoot)) {
                bool titlebarOnly = g_settings.titlebarOnly.load(std::memory_order_relaxed);
                if (!titlebarOnly || IsCursorInTitlebarBounds(hRoot, ms->pt)) {
                    if (PostThreadMessageW(g_workerThreadId, WM_APP_RESET_OPACITY,
                                           reinterpret_cast<WPARAM>(hRoot), 0)) {
                        g_gestureTriggered.store(true, std::memory_order_relaxed);
                        return 1;
                    }
                }
            }
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_workerThreadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_ADJUST_OPACITY) {
            WorkerAdjustWindowOpacity(
                reinterpret_cast<HWND>(msg.wParam),
                static_cast<int>(msg.lParam)
            );
        } else if (msg.message == WM_APP_RESET_OPACITY) {
            WorkerResetWindowOpacity(reinterpret_cast<HWND>(msg.wParam));
        }
    }

    return 0;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HMODULE hMod = GetCurrentModuleHandle();
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse hook: %lu", GetLastError());
        SetEvent(g_hookThreadReady);
        return 1;
    }

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    SetEvent(g_hookThreadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    return 0;
}

void RestoreAllWindows() {
    for (const auto& [hWnd, state] : g_trackedWindows) {
        RestoreWindowState(hWnd, state);
    }
    g_trackedWindows.clear();

    std::lock_guard<std::mutex> lock(g_blockedMutex);
    g_blockedWindows.clear();
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Scroll Window Transparency v" WH_MOD_VERSION L" initialized");
    LoadSettings();

    g_workerThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerThreadReady) {
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        CloseHandle(g_workerThreadReady);
        g_workerThreadReady = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_workerThreadReady, INFINITE);

    g_hookThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookThreadReady) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workerThreadReady);
        g_workerThreadReady = nullptr;
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        CloseHandle(g_hookThreadReady);
        g_hookThreadReady = nullptr;
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workerThreadReady);
        g_workerThreadReady = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_hookThreadReady, INFINITE);

    if (!g_mouseHook) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        CloseHandle(g_hookThreadReady);
        g_hookThreadReady = nullptr;

        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workerThreadReady);
        g_workerThreadReady = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    if (g_hookThreadReady) {
        CloseHandle(g_hookThreadReady);
        g_hookThreadReady = nullptr;
    }

    if (g_workerThread) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_workerThreadReady) {
        CloseHandle(g_workerThreadReady);
        g_workerThreadReady = nullptr;
    }

    RestoreAllWindows();
    Wh_Log(L"Scroll Window Transparency unloaded");
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

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
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

// ==WindhawkMod==
// @id              scroll-window-transparency
// @name            Scroll Window Transparency
// @description     Hold a customizable modifier key or mouse button and scroll over any window to adjust its transparency
// @version         1.0.0
// @author          notactuallyoak
// @github          https://github.com/notactuallyoak
// @include         explorer.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Scroll Window Transparency

Hold your chosen **modifier key** (or mouse side button) and scroll the mouse
wheel over any window to smoothly adjust its opacity.

## Features
- **Customizable Keybind:** Choose between `Alt`, `Ctrl`, `Shift`, `Win`,
  `Ctrl + Alt`, `Ctrl + Shift`, `Alt + Shift`, or mouse side buttons
  (`Mouse Back` / `Mouse Forward`).
- **Quick Reset (`Modifier + Middle-Click`):** Hold your trigger modifier and
  middle-click any window to immediately restore `100%` opacity.
- **No Accidental Start Menu / Menu Bar Focus:** Automatically suppresses
  Windows' `Alt` menu bar highlight and `Win` Start Menu popup after scrolling.
- **Zero Overhead at 100%:** Restoring `100%` opacity removes the
  `WS_EX_LAYERED` window style for native rendering performance.

## Settings Reference
| Setting | Default | Description |
| :--- | :---: | :--- |
| **Trigger Keybind (Hold + Scroll)** | `Alt` | Key combination or mouse side button required while scrolling. |
| **Opacity Step (%)** | `10` | Transparency change per scroll wheel notch (`1`–`50`). |
| **Minimum Opacity (%)** | `25` | Lowest allowed opacity (`10`–`90`). |
| **Titlebar Only** | `false` | Restrict scrolling to the top titlebar area instead of the entire window. |
| **Modifier + Middle-Click Resets to 100%** | `true` | Instantly restore full opacity when middle-clicking with the modifier held. |
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
  $name: Modifier + Middle-Click Resets to 100%
  $description: Hold the trigger modifier and middle-click a window to reset its opacity to 100%.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <cwchar>

enum class TriggerModifier {
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
    TriggerModifier modifier;
    int alphaStep;
    int minAlpha;
    bool titlebarOnly;
    bool middleClickReset;
} g_settings;

std::mutex g_stateMutex;
std::unordered_map<HWND, BYTE> g_windowAlphas;

HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HHOOK g_mouseHook = nullptr;
HHOOK g_keyboardHook = nullptr;
bool g_gestureTriggered = false;

HMODULE GetCurrentModuleHandle() {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        &hMod
    );
    return hMod;
}

void LoadSettings() {
    PCWSTR modStr = Wh_GetStringSetting(L"triggerModifier");
    if (wcscmp(modStr, L"ctrl") == 0) {
        g_settings.modifier = TriggerModifier::Ctrl;
    } else if (wcscmp(modStr, L"shift") == 0) {
        g_settings.modifier = TriggerModifier::Shift;
    } else if (wcscmp(modStr, L"win") == 0) {
        g_settings.modifier = TriggerModifier::Win;
    } else if (wcscmp(modStr, L"ctrl_alt") == 0) {
        g_settings.modifier = TriggerModifier::CtrlAlt;
    } else if (wcscmp(modStr, L"ctrl_shift") == 0) {
        g_settings.modifier = TriggerModifier::CtrlShift;
    } else if (wcscmp(modStr, L"alt_shift") == 0) {
        g_settings.modifier = TriggerModifier::AltShift;
    } else if (wcscmp(modStr, L"xbutton1") == 0) {
        g_settings.modifier = TriggerModifier::XButton1;
    } else if (wcscmp(modStr, L"xbutton2") == 0) {
        g_settings.modifier = TriggerModifier::XButton2;
    } else {
        g_settings.modifier = TriggerModifier::Alt;
    }
    Wh_FreeStringSetting(modStr);

    int stepPct = std::clamp(Wh_GetIntSetting(L"alphaStepPercent"), 1, 50);
    int minPct  = std::clamp(Wh_GetIntSetting(L"minAlphaPercent"), 10, 90);
    g_settings.alphaStep        = static_cast<int>(std::round((stepPct / 100.0) * 255.0));
    g_settings.minAlpha         = static_cast<int>(std::round((minPct / 100.0) * 255.0));
    g_settings.titlebarOnly     = Wh_GetIntSetting(L"titlebarOnly");
    g_settings.middleClickReset = Wh_GetIntSetting(L"middleClickReset");
}

bool IsTriggerModifierPressed() {
    bool alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool win   = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
                 ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

    switch (g_settings.modifier) {
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

bool IsValidTargetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd)) {
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

bool IsCursorOverTitlebar(HWND hWnd, POINT pt) {
    DWORD_PTR hitResult = HTNOWHERE;
    LRESULT sent = SendMessageTimeoutW(
        hWnd,
        WM_NCHITTEST,
        0,
        MAKELPARAM(pt.x, pt.y),
        SMTO_ABORTIFHUNG | SMTO_BLOCK,
        35,
        &hitResult
    );
    if (sent != 0) {
        LRESULT ht = static_cast<LRESULT>(hitResult);
        if (ht == HTCAPTION || ht == HTMINBUTTON || ht == HTMAXBUTTON || ht == HTCLOSE) {
            return true;
        }
    }

    RECT rc = {0};
    if (GetWindowRect(hWnd, &rc)) {
        return (pt.y >= rc.top && pt.y <= rc.top + 40);
    }
    return false;
}

void ResetWindowOpacity(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    if (!g_windowAlphas.count(hWnd)) {
        return;
    }

    g_windowAlphas.erase(hWnd);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

void AdjustWindowOpacity(HWND hWnd, int wheelDelta) {
    std::lock_guard<std::mutex> lock(g_stateMutex);

    BYTE currentAlpha = 255;
    auto it = g_windowAlphas.find(hWnd);
    if (it != g_windowAlphas.end()) {
        currentAlpha = it->second;
    }

    int steps = wheelDelta / WHEEL_DELTA;
    if (steps == 0) {
        steps = (wheelDelta > 0) ? 1 : -1;
    }

    int newAlpha = std::clamp(
        static_cast<int>(currentAlpha) + (steps * g_settings.alphaStep),
        g_settings.minAlpha,
        255
    );

    if (newAlpha == currentAlpha) {
        return;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (newAlpha >= 255) {
        g_windowAlphas.erase(hWnd);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
    } else {
        if (!(exStyle & WS_EX_LAYERED)) {
            SetLastError(0);
            if (SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED) == 0 && GetLastError() != 0) {
                return;
            }
        }
        SetLayeredWindowAttributes(hWnd, 0, static_cast<BYTE>(newAlpha), LWA_ALPHA);
        g_windowAlphas[hWnd] = static_cast<BYTE>(newAlpha);
    }
}

// Suppresses Alt menu-bar focus or Win Start Menu popup when releasing the modifier after scrolling
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && lParam && g_gestureTriggered) {
        const KBDLLHOOKSTRUCT* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            bool isAlt = (kb->vkCode == VK_LMENU || kb->vkCode == VK_RMENU || kb->vkCode == VK_MENU);
            bool isWin = (kb->vkCode == VK_LWIN || kb->vkCode == VK_RWIN);
            if (isAlt || isWin) {
                g_gestureTriggered = false;
                keybd_event(VK_F24, 0, 0, 0);
                keybd_event(VK_F24, 0, KEYEVENTF_KEYUP, 0);
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && lParam) {
        const MSLLHOOKSTRUCT* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

        // Suppress Back/Forward action on XButton release if it was used as a scroll modifier
        if (wParam == WM_XBUTTONUP && g_gestureTriggered &&
            (g_settings.modifier == TriggerModifier::XButton1 ||
             g_settings.modifier == TriggerModifier::XButton2)) {
            g_gestureTriggered = false;
            return 1;
        }

        if (wParam == WM_MOUSEWHEEL && IsTriggerModifierPressed()) {
            HWND hPoint = WindowFromPoint(ms->pt);
            HWND hRoot = hPoint ? GetAncestor(hPoint, GA_ROOT) : nullptr;

            if (IsValidTargetWindow(hRoot)) {
                if (!g_settings.titlebarOnly || IsCursorOverTitlebar(hRoot, ms->pt)) {
                    g_gestureTriggered = true;
                    short wheelDelta = static_cast<short>(HIWORD(ms->mouseData));
                    AdjustWindowOpacity(hRoot, wheelDelta);
                    return 1;
                }
            }
        }

        if (wParam == WM_MBUTTONDOWN && g_settings.middleClickReset && IsTriggerModifierPressed()) {
            HWND hPoint = WindowFromPoint(ms->pt);
            HWND hRoot = hPoint ? GetAncestor(hPoint, GA_ROOT) : nullptr;

            if (IsValidTargetWindow(hRoot)) {
                if (!g_settings.titlebarOnly || IsCursorOverTitlebar(hRoot, ms->pt)) {
                    g_gestureTriggered = true;
                    ResetWindowOpacity(hRoot);
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    HMODULE hMod = GetCurrentModuleHandle();
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse hook: %lu", GetLastError());
        return 1;
    }

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);

    MSG msg;
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
    std::lock_guard<std::mutex> lock(g_stateMutex);

    for (const auto& [hWnd, alpha] : g_windowAlphas) {
        if (IsWindow(hWnd)) {
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
            RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
        }
    }
    g_windowAlphas.clear();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Scroll Window Transparency v1.1.0 initialized");
    LoadSettings();

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    return g_hookThread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    RestoreAllWindows();
    Wh_Log(L"Scroll Window Transparency unloaded");
}
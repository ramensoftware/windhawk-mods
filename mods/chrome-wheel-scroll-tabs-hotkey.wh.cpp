// ==WindhawkMod==
// @id              chrome-wheel-scroll-tabs-hotkey
// @name            Chrome/Edge scroll tabs with configurable hotkey
// @description     Hold Shift, Ctrl or Alt and scroll anywhere in the active browser to switch tabs
// @version         1.0.0
// @author          AurionX
// @github          https://github.com/fagnerlemes
// @include         chrome.exe
// @include         msedge.exe
// @include         opera.exe
// @include         brave.exe
// @include         *\YandexBrowser\Application\browser.exe
// @include         thorium.exe
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Derived from chrome-wheel-scroll-tabs 1.3.1 by m417z.
// Customization credit: AurionX.
// Original: https://github.com/ramensoftware/windhawk-mods/blob/main/mods/chrome-wheel-scroll-tabs.wh.cpp
// Changes (2026-10-28): configurable modifier + wheel in page content,
// optional original tab-bar behavior, and separate wheel remainders per mode.
// This derivative is not an official release by the original author.

// ==WindhawkModReadme==
/*
# Chrome/Edge: switch tabs with a key + mouse wheel

Hold **Shift** and use the vertical mouse wheel anywhere in the active browser
window, including over the web page, to switch tabs.

Under **Settings > Activation key**, choose **Shift**, **Ctrl**, or **Alt**.
Shift is the default. Combinations with additional modifier keys are ignored.

- Scroll down for the next tab; scroll up for the previous tab.
- The reverse scrolling direction option also applies to the shortcut.
- Release the key to resume normal page scrolling.
- Tab switching over the tab bar without holding a key can be enabled or disabled.
- The shortcut only works while the browser is active and the pointer is over it.
- The shortcut uses the vertical wheel. The horizontal wheel option applies to the tab bar.

The selected gesture replaces its usual browser action, such as horizontal
scrolling with Shift + wheel or zooming with Ctrl + wheel.

Disable the original mod when using this variant. Updates to the original mod
must be incorporated into this copy manually.

Based on chrome-wheel-scroll-tabs 1.3.1 by **m417z**. License: GPL-3.0.
Customization: **AurionX**.
This independently maintained variant is not an official release by m417z.

Tested by the submitter in Google Chrome and Microsoft Edge. The executable
list inherited from the original mod also includes Opera, Brave, Yandex Browser,
and Thorium; these browsers have not been tested with this variant.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activationKey: shift
  $name: Activation key
  $description: Hold this key and use the vertical mouse wheel over the active browser window.
  $options:
  - shift: Shift (default)
  - ctrl: Ctrl
  - alt: Alt
- tabBarScrolling: true
  $name: Enable tab bar scrolling without a key
  $description: Keeps the original behavior. Disable to switch tabs only while holding the selected key.
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
- horizontalScrolling: false
  $name: Enable horizontal wheel scrolling over the tab bar
  $description: Applies to tab bar scrolling without a key. The shortcut uses the vertical wheel.
- throttleMs: 0
  $name: Minimum interval between tab switches (ms)
  $description: Set to 0 to respond to every wheel step. Try 120 if scrolling switches tabs too quickly.
- scrollAreaLimit:
  - pixelsFromTop: 0
    $name: Pixels from the top
  - pixelsFromLeft: 0
    $name: Pixels from the left
  $name: Scroll area limit without a key
  $description: Limits only the original scrolling behavior without a key. Set to 0 for no limit. Does not restrict the shortcut.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windowsx.h>
#include <atomic>

struct {
    int activationVirtualKey;
    bool tabBarScrolling;
    bool reverseScrollingDirection;
    bool horizontalScrolling;
    int throttleMs;
    int scrollAreaLimitPixelsFromTop;
    int scrollAreaLimitPixelsFromLeft;
} g_settings;

bool g_isEdge;
DWORD g_uiThreadId;
DWORD g_lastScrollTime;
HWND g_lastScrollWnd;
short g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;
bool g_lastScrollWasShortcut;
HHOOK g_mouseHook;
std::atomic<bool> g_hooksReady{false};
std::atomic<bool> g_unloading{false};
UINT g_installMouseHookMsg = RegisterWindowMessage(
    L"Windhawk_InstallMouseHook_chrome-wheel-scroll-tabs-hotkey");
void EnsureMouseHook();
thread_local bool g_simulateCtrlKeyDown;

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_chrome-wheel-scroll-tabs-hotkey");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        dpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(nullptr);
        if (hdc) {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(nullptr, hdc);
        }
    }

    return dpi;
}

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode,
                                              WPARAM wParam,
                                              LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result =
                SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                  param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass,
                                  nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

bool OnMouseWheel(HWND hWnd, WORD keys, short delta, int xPos, int yPos,
                  bool shortcut = false) {
    if (keys || g_unloading.load()) {
        return false;
    }

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    UINT dpi = GetDpiForWindowWithFallback(hWnd);

    if (!shortcut) {
        if (int scrollAreaLimitPixelsFromTop =
                g_settings.scrollAreaLimitPixelsFromTop) {
            scrollAreaLimitPixelsFromTop =
                MulDiv(scrollAreaLimitPixelsFromTop, dpi, 96);
            if (yPos >= rect.top + scrollAreaLimitPixelsFromTop) {
                Wh_Log(L"Mouse wheel event outside of the top scroll area limit");
                return false;
            }
        }

        if (int scrollAreaLimitPixelsFromLeft =
                g_settings.scrollAreaLimitPixelsFromLeft) {
            scrollAreaLimitPixelsFromLeft =
                MulDiv(scrollAreaLimitPixelsFromLeft, dpi, 96);
            if (xPos >= rect.left + scrollAreaLimitPixelsFromLeft) {
                Wh_Log(L"Mouse wheel event outside of the left scroll area limit");
                return false;
            }
        }

        switch (SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(xPos, yPos))) {
            case HTCLIENT:
            case HTCAPTION:
            case HTSYSMENU:
            case HTMINBUTTON:
            case HTMAXBUTTON:
            case HTCLOSE:
                break;

            default:
                return false;
        }

    } // The modifier shortcut intentionally bypasses tab-bar area checks.

    if ((!shortcut && GetKeyState(VK_MENU) < 0) || GetKeyState(VK_LWIN) < 0 ||
        GetKeyState(VK_RWIN) < 0 || GetKeyState(VK_PRIOR) < 0 ||
        GetKeyState(VK_NEXT) < 0) {
        return false;
    }

    // Edge has a thin border around the web content which triggers the tab
    // scrolling. Ignore events which are very close to the window borders.
    if (!shortcut && g_isEdge) {
        if (MulDiv(xPos - rect.left, 96, dpi) <= 15 ||
            MulDiv(rect.right - xPos, 96, dpi) <= 15 ||
            MulDiv(rect.bottom - yPos, 96, dpi) <= 15) {
            return false;
        }
    }

    if (hWnd == g_lastScrollWnd && shortcut == g_lastScrollWasShortcut &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks != 0 && g_settings.throttleMs > 0) {
        if (GetTickCount() - g_lastActionTime < (DWORD)g_settings.throttleMs) {
            // It's too soon, ignore this scroll event.
            clicks = 0;

            // Reset remainder too.
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            // Throttle to a single action at a time.
            clicks = clicks > 0 ? 1 : -1;

            // Reset remainder if going too fast.
            delta = 0;
        }
    }

    WORD key = VK_NEXT;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_PRIOR;
    }

    if (clicks > 0) {
        Wh_Log(L"Simulating input for window %08X", (DWORD)(ULONG_PTR)hWnd);

        // Set flag to fake Ctrl key state in GetKeyState hook.
        g_simulateCtrlKeyDown = true;

        // Check if window is already active.
        HWND hForegroundWnd = GetForegroundWindow();
        bool needsActivation = !hForegroundWnd || hForegroundWnd != hWnd;

        // Check if foreground window is from the same thread.
        HWND hSameThreadForeground = nullptr;
        if (needsActivation && hForegroundWnd) {
            DWORD dwForegroundThread =
                GetWindowThreadProcessId(hForegroundWnd, nullptr);
            DWORD dwTargetThread = GetWindowThreadProcessId(hWnd, nullptr);
            if (dwForegroundThread == dwTargetThread) {
                hSameThreadForeground = hForegroundWnd;
            }
        }

        if (needsActivation) {
            // If foreground window is from same thread, deactivate it first.
            if (hSameThreadForeground) {
                SendMessage(hSameThreadForeground, WM_ACTIVATE, WA_INACTIVE, 0);
            }

            // Fake window activation so key messages are processed even if not
            // focused.
            SendMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0);
        }

        // Send only Page Up/Down keys (Ctrl state will be faked via hooks).
        for (int i = 0; i < clicks; i++) {
            SendMessage(hWnd, WM_KEYDOWN, key, 0);
            SendMessage(hWnd, WM_KEYUP, key, 0);
        }

        if (needsActivation) {
            // Restore deactivated state.
            SendMessage(hWnd, WM_ACTIVATE, WA_INACTIVE, 0);

            // If there was a same-thread foreground window, reactivate it.
            if (hSameThreadForeground) {
                SendMessage(hSameThreadForeground, WM_ACTIVATE, WA_ACTIVE, 0);
            }
        }

        // Clear flag.
        g_simulateCtrlKeyDown = false;

        g_lastActionTime = GetTickCount();
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollWnd = hWnd;
    g_lastScrollWasShortcut = shortcut;
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return true;
}

LRESULT CALLBACK BrowserWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass,
                                           _In_ DWORD_PTR dwRefData) {
    if (uMsg == g_installMouseHookMsg) {
        EnsureMouseHook();
        return 0;
    }
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        if (g_unloading.load() && g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
        }
        RemoveWindowSubclass(hWnd, BrowserWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            if (!g_settings.tabBarScrolling || g_unloading.load()) {
                break;
            }
            WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (uMsg == WM_MOUSEHWHEEL) {
                if (!g_settings.horizontalScrolling) {
                    break;
                }

                // For horizontal scrolling, a large delta value might be posted
                // for a single click (e.g. 480). Limit the value to 120.
                if (zDelta < -120) {
                    zDelta = -120;
                } else if (zDelta > 120) {
                    zDelta = 120;
                }
            } else if (!g_settings.reverseScrollingDirection) {
                zDelta = -zDelta;
            }

            if (OnMouseWheel(hWnd, fwKeys, zDelta, xPos, yPos)) {
                return 0;
            }
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Best effort determination as of whether the window is a browser window.
bool IsBrowserWindow(HWND hWnd) {
    if (!(GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)) {
        return false;
    }

    WCHAR windowClassName[256];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    bool classNameMatch = false;
    PCWSTR classNames[] = {
        L"Chrome_WidgetWin_1",
        L"YandexBrowser_WidgetWin_1",
    };
    for (PCWSTR className : classNames) {
        if (_wcsicmp(windowClassName, className) == 0) {
            classNameMatch = true;
            break;
        }
    }

    return classNameMatch;
}

// A thread mouse hook also receives wheel events aimed at the web-content
// child window. Subclassing only the top-level window misses those events.
bool IsActivationGestureActive() {
    const int activationKey = g_settings.activationVirtualKey;
    if (GetKeyState(activationKey) >= 0) {
        return false;
    }
    const int modifiers[] = {VK_SHIFT, VK_CONTROL, VK_MENU};
    for (int key : modifiers) {
        if (key != activationKey && GetKeyState(key) < 0) {
            return false;
        }
    }
    const int blockedKeys[] = {VK_LWIN, VK_RWIN, VK_LBUTTON, VK_RBUTTON,
                               VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2,
                               VK_PRIOR, VK_NEXT};
    for (int key : blockedKeys) {
        if (GetKeyState(key) < 0) {
            return false;
        }
    }
    return true;
}

LRESULT CALLBACK BrowserMouseHook(int code, WPARAM message, LPARAM data) {
    if (code != HC_ACTION || message != WM_MOUSEWHEEL ||
        !g_hooksReady.load() || g_unloading.load() ||
        !IsActivationGestureActive()) {
        return CallNextHookEx(nullptr, code, message, data);
    }

    const auto* event = reinterpret_cast<const MOUSEHOOKSTRUCTEX*>(data);
    HWND root = GetAncestor(event->hwnd, GA_ROOT);
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(root, &processId);
    if (!root || processId != GetCurrentProcessId() ||
        threadId != GetCurrentThreadId() || !IsBrowserWindow(root) ||
        !IsWindowEnabled(root) || GetForegroundWindow() != root ||
        GetAncestor(WindowFromPoint(event->pt), GA_ROOT) != root) {
        return CallNextHookEx(nullptr, code, message, data);
    }

    short delta = static_cast<short>(HIWORD(event->mouseData));
    if (!g_settings.reverseScrollingDirection) {
        delta = -delta;
    }
    if (OnMouseWheel(root, 0, delta, event->pt.x, event->pt.y, true)) {
        // Consume the wheel so the page does not also scroll or zoom.
        return 1;
    }
    return CallNextHookEx(nullptr, code, message, data);
}

// Called in the browser UI thread, including when loading into an open browser.
void EnsureMouseHook() {
    if (!g_mouseHook && g_hooksReady.load() && !g_unloading.load()) {
        g_mouseHook = SetWindowsHookEx(WH_MOUSE, BrowserMouseHook, nullptr,
                                      GetCurrentThreadId());
        if (!g_mouseHook) {
            Wh_Log(L"Failed to install modifier wheel hook: %lu", GetLastError());
        }
    }
}

BOOL CALLBACK InitialEnumBrowserWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (g_uiThreadId && g_uiThreadId != dwThreadId) {
        return TRUE;
    }

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window found: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = dwThreadId;
        }

        if (SetWindowSubclassFromAnyThread(hWnd, BrowserWindowSubclassProc, 0, 0)) {
            SendMessage(hWnd, g_installMouseHookMsg, 0, 0);
        }
    }

    return TRUE;
}

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window to unsubclass: %08X", (DWORD)(ULONG_PTR)hWnd);

        SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
    }

    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle,
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
                                LPVOID lpParam) {
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd) {
        return hWnd;
    }

    if (g_uiThreadId && g_uiThreadId != GetCurrentThreadId()) {
        return hWnd;
    }

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = GetCurrentThreadId();
        }

        if (!g_unloading.load() &&
            SetWindowSubclass(hWnd, BrowserWindowSubclassProc, 0, 0)) {
            EnsureMouseHook();
        }
    }

    return hWnd;
}

using GetKeyState_t = decltype(&GetKeyState);
GetKeyState_t pOriginalGetKeyState;
SHORT WINAPI GetKeyStateHook(int nVirtKey) {
    if (g_simulateCtrlKeyDown) {
        Wh_Log(L"Simulating for GetKeyState(%04X)", nVirtKey);
        // High bit set = key is down.
        return (nVirtKey == VK_CONTROL) ? 0x8000 : 0;
    }

    return pOriginalGetKeyState(nVirtKey);
}

void LoadSettings() {
    PCWSTR key = Wh_GetStringSetting(L"activationKey");
    g_settings.activationVirtualKey = VK_SHIFT;
    if (key) {
        if (_wcsicmp(key, L"ctrl") == 0) {
            g_settings.activationVirtualKey = VK_CONTROL;
        } else if (_wcsicmp(key, L"alt") == 0) {
            g_settings.activationVirtualKey = VK_MENU;
        }
        Wh_FreeStringSetting(key);
    }
    g_settings.tabBarScrolling = Wh_GetIntSetting(L"tabBarScrolling") != 0;
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.horizontalScrolling = Wh_GetIntSetting(L"horizontalScrolling");
    g_settings.throttleMs = Wh_GetIntSetting(L"throttleMs");
    g_settings.scrollAreaLimitPixelsFromTop =
        Wh_GetIntSetting(L"scrollAreaLimit.pixelsFromTop");
    g_settings.scrollAreaLimitPixelsFromLeft =
        Wh_GetIntSetting(L"scrollAreaLimit.pixelsFromLeft");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_isEdge = !!GetModuleHandle(L"msedge.exe");

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook,
                           (void**)&pOriginalCreateWindowExW) ||
        !Wh_SetFunctionHook((void*)GetKeyState, (void*)GetKeyStateHook,
                           (void**)&pOriginalGetKeyState)) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    g_hooksReady.store(true);
    EnumWindows(InitialEnumBrowserWindowsFunc, 0);
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    g_hooksReady.store(false);
    if (g_uiThreadId != 0) {
        // The synchronous messages also wait for in-progress UI work.
        EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
    }
}

void Wh_ModUninit() {
    // Catch a window created concurrently with the first enumeration.
    if (g_uiThreadId != 0) {
        EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    // Reload to apply the key and other settings without cross-thread races.
    *reload = TRUE;
    return TRUE;
}

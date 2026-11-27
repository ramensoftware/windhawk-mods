// ==WindhawkMod==
// @id              middle-click-minimize
// @name            Middle Click Minimize
// @description     Minimizes (or closes, maximizes, pins on top) the window when middle-clicking on the title bar.
// @version         1.1.4
// @author          Ar4ujo_009
// @github          https://github.com/Ar4ujo009
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Middle Click Minimize

Middle-click (mouse wheel button) on a window's title bar to minimize it,
inspired by the GNOME desktop.

## Actions

Each combination can be configured in the mod settings:

- Middle click (default: minimize)
- Ctrl + middle click (default: do nothing)
- Shift + middle click (default: do nothing)

Available actions: minimize, maximize / restore, close, toggle "always on top",
or nothing (the app handles the click normally).

## Notes

- Ctrl takes priority over Shift when both are held.
- Windows that cannot perform the action (no minimize box, disabled by a modal
  dialog, close button disabled...) are left alone.
- Elevated (admin) apps only work if Windhawk has the required privileges.
- Games with anti-cheat: exclude them in the mod's Advanced tab.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- middleAction: minimize
  $name: Middle click
  $options:
  - minimize: Minimize
  - maximize: Maximize / restore
  - close: Close
  - topmost: Toggle always on top
  - none: Do nothing
- ctrlAction: none
  $name: Ctrl + middle click
  $options:
  - minimize: Minimize
  - maximize: Maximize / restore
  - close: Close
  - topmost: Toggle always on top
  - none: Do nothing
- shiftAction: none
  $name: Shift + middle click
  $options:
  - minimize: Minimize
  - maximize: Maximize / restore
  - close: Close
  - topmost: Toggle always on top
  - none: Do nothing
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <wchar.h>

//? ======================= Settings =======================
enum Action {
    ACT_NONE,
    ACT_MINIMIZE,
    ACT_MAXIMIZE,
    ACT_CLOSE,
    ACT_TOPMOST
};

Action g_middleAction = ACT_MINIMIZE;
Action g_ctrlAction = ACT_NONE;
Action g_shiftAction = ACT_NONE;

Action LoadAction(PCWSTR key, Action fallback) {
    PCWSTR value = Wh_GetStringSetting(key);
    Action result = fallback;

    if (wcscmp(value, L"minimize") == 0) result = ACT_MINIMIZE;
    else if (wcscmp(value, L"maximize") == 0) result = ACT_MAXIMIZE;
    else if (wcscmp(value, L"close") == 0) result = ACT_CLOSE;
    else if (wcscmp(value, L"topmost") == 0) result = ACT_TOPMOST;
    else if (wcscmp(value, L"none") == 0) result = ACT_NONE;

    //! Always free the string, otherwise it leaks on every settings change
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    g_middleAction = LoadAction(L"middleAction", ACT_MINIMIZE);
    g_ctrlAction = LoadAction(L"ctrlAction", ACT_NONE);
    g_shiftAction = LoadAction(L"shiftAction", ACT_NONE);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

//? ======================= Core logic =======================
Action PickAction() {
    //* GetKeyState reflects the keyboard state at the time of the click message
    if (GetKeyState(VK_CONTROL) & 0x8000) return g_ctrlAction;
    if (GetKeyState(VK_SHIFT) & 0x8000) return g_shiftAction;
    return g_middleAction;
}

// Returns true if the click was consumed, false to let the app handle it
bool RunAction(HWND target, Action action) {
    LONG_PTR style = GetWindowLongPtr(target, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtr(target, GWL_EXSTYLE);

    switch (action) {
        case ACT_MINIMIZE:
            //! Only minimize windows that natively have a minimize box
            if (!(style & WS_MINIMIZEBOX)) return false;
            PostMessage(target, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            return true;

        case ACT_MAXIMIZE:
            if (!(style & WS_MAXIMIZEBOX)) return false;
            PostMessage(target, WM_SYSCOMMAND, IsZoomed(target) ? SC_RESTORE : SC_MAXIMIZE, 0);
            return true;

        case ACT_CLOSE:
            //! Respect windows whose close button is disabled
            if (GetClassLongPtr(target, GCL_STYLE) & CS_NOCLOSE) return false;
            PostMessage(target, WM_SYSCOMMAND, SC_CLOSE, 0);
            return true;

        case ACT_TOPMOST: {
            //* "Always on top" makes no sense for MDI child windows
            if (exStyle & WS_EX_MDICHILD) return false;
            bool isTopmost = (exStyle & WS_EX_TOPMOST) != 0;
            SetWindowPos(target, isTopmost ? HWND_NOTOPMOST : HWND_TOPMOST,
                         0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            return true;
        }

        default:
            return false;
    }
}

bool HandleMiddleClickOnCaption(const MSG* msg) {
    if (msg->message != WM_NCMBUTTONDOWN || msg->wParam != HTCAPTION) {
        return false;
    }

    //* MDI children are handled on their own, everything else uses the root window
    HWND target = (GetWindowLongPtr(msg->hwnd, GWL_EXSTYLE) & WS_EX_MDICHILD)
                      ? msg->hwnd
                      : GetAncestor(msg->hwnd, GA_ROOT);

    //! A window blocked by a modal dialog must not be touched
    if (!target || !IsWindowEnabled(target)) {
        return false;
    }

    Action action = PickAction();
    bool handled = RunAction(target, action);

    if (handled) {
        Wh_Log(L"Action %d on window %p", (int)action, target);
    }
    return handled;
}

//? ======================= Hooks =======================
using DispatchMessage_t = decltype(&DispatchMessageW);
DispatchMessage_t DispatchMessageW_Original;
DispatchMessage_t DispatchMessageA_Original;

//* One template for both the W and A versions
template <DispatchMessage_t& Original>
LRESULT WINAPI DispatchMessage_Hook(const MSG* lpMsg) {
    if (lpMsg && HandleMiddleClickOnCaption(lpMsg)) {
        return 0;
    }
    return Original(lpMsg);
}

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(DispatchMessageW,
                                   DispatchMessage_Hook<DispatchMessageW_Original>,
                                   &DispatchMessageW_Original);
    WindhawkUtils::SetFunctionHook(DispatchMessageA,
                                   DispatchMessage_Hook<DispatchMessageA_Original>,
                                   &DispatchMessageA_Original);
    return TRUE;
}

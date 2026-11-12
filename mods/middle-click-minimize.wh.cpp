// ==WindhawkMod==
// @id              middle-click-minimize
// @name            Middle Click Minimize
// @description     Minimizes the window when middle-clicking (scroll wheel) on the title bar.
// @version         1.1.3
// @author          Ar4ujo_009
// @github          https://github.com/Ar4ujo009
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Middle Click Minimize
This mod allows you to minimize windows by middle-clicking (using the scroll wheel) directly on the title bar.

## Exclusions and Conflicts
* **How to exclude programs:** To disable the mod for a specific program (e.g., `steam.exe` or `vivaldi.exe`), add it to the custom process exclusion list in the mod's **Advanced** tab in Windhawk.
* **Known conflicts:** If you use the "Middle-Click New Tab Chrome/Edge/Brave" mod, middle-clicking the empty tab-strip might minimize the browser instead of opening a new tab. You can add your browser to the exclusion list in the Advanced tab to prevent this.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

bool HandleMiddleClickOnCaption(const MSG* msg) {
    if (msg->message != WM_NCMBUTTONDOWN || msg->wParam != HTCAPTION) {
        return false;
    }

    // Identifica se é uma sub-janela (MDI child) ou pega a janela principal
    HWND target = (GetWindowLongPtr(msg->hwnd, GWL_EXSTYLE) & WS_EX_MDICHILD)
                      ? msg->hwnd
                      : GetAncestor(msg->hwnd, GA_ROOT);

    // Só minimiza se a janela possuir o botão/estilo de minimizar nativo
    if (!target || !(GetWindowLongPtr(target, GWL_STYLE) & WS_MINIMIZEBOX)) {
        return false;  // Deixa o aplicativo lidar com o clique normalmente
    }

    PostMessage(target, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    return true;
}

using DispatchMessage_t = decltype(&DispatchMessageW);
DispatchMessage_t DispatchMessageW_Original;
DispatchMessage_t DispatchMessageA_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && HandleMiddleClickOnCaption(lpMsg)) {
        return 0;
    }
    return DispatchMessageW_Original(lpMsg);
}

LRESULT WINAPI DispatchMessageA_Hook(const MSG* lpMsg) {
    if (lpMsg && HandleMiddleClickOnCaption(lpMsg)) {
        return 0;
    }
    return DispatchMessageA_Original(lpMsg);
}

BOOL Wh_ModInit() {
    WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageW_Hook, &DispatchMessageW_Original);
    WindhawkUtils::SetFunctionHook(DispatchMessageA, DispatchMessageA_Hook, &DispatchMessageA_Original);
    return TRUE;
}

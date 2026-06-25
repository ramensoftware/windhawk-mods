// ==WindhawkMod==
// @id              vscode-dark-nonclient-frame
// @name            VS Code Dark Non-client Frame
// @description     Forces VS Code's non-client frame into dark mode to remove the light 2px bottom edge in Windows light mode.
// @version         1.1
// @author          v1b3s0
// @github          https://github.com/v1b3s0
// @license         MIT
// @include         Code.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VS Code Dark Non-client Frame

Forces Visual Studio Code's Windows non-client frame into immersive dark mode.

This fixes the 2px light gray bottom edge that can appear when VS Code is maximized with the custom title bar in Windows light mode, especially with a hidden or transparent taskbar.

Before:

![Before](https://i.imgur.com/ldT3xJt.png)

After:

![After](https://i.imgur.com/DVrOPag.png)

The mod targets only `Code.exe` by default. Users of VS Code Insiders, VSCodium, or renamed/portable builds can add the relevant executable name to the mod's custom include list.

When the mod is disabled, it stops overriding VS Code's frame mode. Already affected windows may keep their current frame mode until VS Code applies a new value, for example after a theme or window state change.
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_api.h>

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;

BOOL IsCurrentProcessWindow(HWND hWnd) {
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);

    return windowPid == GetCurrentProcessId();
}

BOOL IsValidWindow(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);

    return (style & WS_THICKFRAME) == WS_THICKFRAME ||
           (style & WS_CAPTION) == WS_CAPTION;
}

void ApplyDarkFrame(HWND hWnd) {
    if (!IsValidWindow(hWnd)) {
        return;
    }

    BOOL enabled = TRUE;

    DwmSetWindowAttribute_orig(
        hWnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &enabled,
        sizeof(enabled)
    );
}

HRESULT WINAPI DwmSetWindowAttribute_hook(
    HWND hWnd,
    DWORD dwAttribute,
    LPCVOID pvAttribute,
    DWORD cbAttribute
) {
    if (dwAttribute == DWMWA_USE_IMMERSIVE_DARK_MODE &&
        IsCurrentProcessWindow(hWnd) &&
        IsValidWindow(hWnd)) {
        BOOL enabled = TRUE;

        return DwmSetWindowAttribute_orig(
            hWnd,
            dwAttribute,
            &enabled,
            sizeof(enabled)
        );
    }

    return DwmSetWindowAttribute_orig(
        hWnd,
        dwAttribute,
        pvAttribute,
        cbAttribute
    );
}

BOOL CALLBACK EnableEnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    DWORD targetPid = static_cast<DWORD>(lParam);

    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);

    if (targetPid == windowPid) {
        ApplyDarkFrame(hWnd);
    }

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    Wh_SetFunctionHook(
        (void*)DwmSetWindowAttribute,
        (void*)DwmSetWindowAttribute_hook,
        (void**)&DwmSetWindowAttribute_orig
    );

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");

    // Do not force DWMWA_USE_IMMERSIVE_DARK_MODE to FALSE here.
    // Disabling the mod should stop overriding VS Code's own value instead of
    // forcing a light non-client frame.
}

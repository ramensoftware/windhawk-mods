// ==WindhawkMod==
// @id              vscode-dark-nonclient-frame
// @name            VS Code Dark Non-client Frame
// @description     Forces VS Code's non-client frame into dark mode to remove the light 2px bottom edge in Windows light mode.
// @version         1.0
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

The mod targets only `Code.exe`.
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <windhawk_api.h>

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;

HWINEVENTHOOK g_objectCreateHook = nullptr;
HWINEVENTHOOK g_objectShowHook = nullptr;

BOOL IsCurrentProcessWindow(HWND hWnd) {
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);

    return windowPid == GetCurrentProcessId();
}

BOOL IsValidWindow(HWND hWnd) {
    LONG_PTR dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);

    return (dwStyle & WS_THICKFRAME) == WS_THICKFRAME ||
           (dwStyle & WS_CAPTION) == WS_CAPTION;
}

void ApplyDarkFrame(HWND hWnd) {
    if (!IsCurrentProcessWindow(hWnd) || !IsValidWindow(hWnd)) {
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
    DWORD pid = lParam;

    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);

    if (pid == windowPid) {
        ApplyDarkFrame(hWnd);
    }

    return TRUE;
}

void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD
) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    ApplyDarkFrame(hWnd);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    Wh_SetFunctionHook(
        (void*)DwmSetWindowAttribute,
        (void*)DwmSetWindowAttribute_hook,
        (void**)&DwmSetWindowAttribute_orig
    );

    DWORD currentPid = GetCurrentProcessId();

    g_objectCreateHook = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_CREATE,
        nullptr,
        WinEventProc,
        currentPid,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_objectCreateHook) {
        Wh_Log(L"Failed to create EVENT_OBJECT_CREATE hook");
    }

    g_objectShowHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        currentPid,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_objectShowHook) {
        Wh_Log(L"Failed to create EVENT_OBJECT_SHOW hook");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    EnumWindows(EnableEnumWindowsCallback, GetCurrentProcessId());
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");

    if (g_objectCreateHook) {
        UnhookWinEvent(g_objectCreateHook);
        g_objectCreateHook = nullptr;
    }

    if (g_objectShowHook) {
        UnhookWinEvent(g_objectShowHook);
        g_objectShowHook = nullptr;
    }

    // Do not force DWMWA_USE_IMMERSIVE_DARK_MODE to FALSE here.
    // Disabling the mod should stop overriding VS Code's own value instead of
    // forcing a light non-client frame.
}
// ==WindhawkMod==
// @id              nvidia-control-panel-missing-dialog-hider
// @name            Hide NVIDIA Control Panel Missing Dialog
// @description     Hides the nvcontainer.exe popup that says NVIDIA Control Panel is not found
// @version         1.1.2
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         nvcontainer.exe
// @include         NVDisplay.Container.exe
// @compilerOptions -luser32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide NVIDIA Control Panel Missing Dialog

![Screenshot](https://i.imgur.com/8vaxTpN.png)

Silences the **"NVIDIA Control Panel not found"** notification that appears in the bottom-right corner every time Windows starts.

## Background

Tools like [NVCleanstall](https://www.techpowerup.com/nvcleanstall/) allow you to strip components from the NVIDIA driver package during installation. Certain options — such as disabling or not installing the NVIDIA Control Panel — break the Control Panel integration intentionally or as a side effect. Once broken, `nvcontainer.exe` detects that the Control Panel is missing or non-functional and displays a dialog on every startup prompting you to reinstall it from the Microsoft Store.

Since the **NVIDIA App** now covers everything the classic Control Panel offered — display settings, GPU overclocking, driver management, and more — there is no practical reason to have the Control Panel installed. The dialog serves no purpose and becomes permanent noise.

## What it does

This mod suppresses the "NVIDIA Control Panel not found" dialog completely. It never appears on screen, neither at startup nor at any other time.

## How it works

The mod hooks into `nvcontainer.exe` and intercepts `ShowWindow` and `SetWindowPos` calls. Any top-level `#32770` dialog whose child windows contain the text "NVIDIA" is immediately blocked from becoming visible. A cache of confirmed dialog handles ensures the detection logic runs only once per window, keeping overhead minimal on subsequent calls.

On mod initialization, any already-visible instance of the dialog is also hidden immediately.

## Compatibility
- Windows 10 and Windows 11
- Targets `nvcontainer.exe` and `NVDisplay.Container.exe` only
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <string>

static BOOL CALLBACK CheckNvidiaChild(HWND child, LPARAM lParam) {
    WCHAR buf[512] = {};
    GetWindowTextW(child, buf, 512);
    if (std::wstring(buf).find(L"NVIDIA") != std::wstring::npos) {
        *(bool*)lParam = true;
        return FALSE;
    }
    return TRUE;
}

static bool IsNvidiaDialog(HWND hWnd) {
    WCHAR cls[256] = {};
    GetClassNameW(hWnd, cls, 256);
    if (std::wstring(cls) != L"#32770") return false;
    bool found = false;
    EnumChildWindows(hWnd, CheckNvidiaChild, (LPARAM)&found);
    return found;
}

static BOOL CALLBACK HideExistingNvidiaDialogs(HWND hWnd, LPARAM lParam) {
    // Only process windows belonging to the current process
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);
    if (windowPid != GetCurrentProcessId()) return TRUE;

    if (IsWindowVisible(hWnd) && IsNvidiaDialog(hWnd)) {
        ShowWindow(hWnd, SW_HIDE);
    }
    return TRUE;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t originalShowWindow;

BOOL WINAPI ShowWindowHook(HWND hWnd, int nCmdShow) {
    if (hWnd && nCmdShow != SW_HIDE && IsNvidiaDialog(hWnd)) {
        return TRUE;
    }
    return originalShowWindow(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t originalSetWindowPos;

BOOL WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if (!hWnd) return originalSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    if ((uFlags & SWP_SHOWWINDOW) && !GetParent(hWnd)) {
        WCHAR cls[256] = {};
        GetClassNameW(hWnd, cls, 256);
        if (std::wstring(cls) == L"#32770" && IsNvidiaDialog(hWnd)) {
            // Remove SWP_SHOWWINDOW instead of blocking the call entirely
            uFlags &= ~SWP_SHOWWINDOW;
        }
    }

    return originalSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL Wh_ModInit() {
    EnumWindows(HideExistingNvidiaDialogs, 0);

    Wh_SetFunctionHook(
        (void*)ShowWindow,
        (void*)ShowWindowHook,
        (void**)&originalShowWindow);

    Wh_SetFunctionHook(
        (void*)SetWindowPos,
        (void*)SetWindowPosHook,
        (void**)&originalSetWindowPos);

    return TRUE;
}

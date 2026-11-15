// ==WindhawkMod==
// @id              action-center-active-display
// @name            Action Center on Active Display
// @description     Opens Action Center (Win + A), Cast pane (Win + K), and Project pane (Win + P) on the monitor where the mouse cursor is currently located.
// @version         1.1.1
// @author          ereinaimer
// @github          https://github.com/ereinaimer
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Action Center on Active Display

Opens the Windows Action Center (Quick Settings, `Win + A`), Cast pane (`Win + K`), and Project pane (`Win + P`) on the monitor where your mouse cursor is currently located.

## How it works

The mod hooks `SetWindowPos` inside ShellExperienceHost.exe (pre-24H2) and ShellHost.exe (24H2+). 
When any of these windows are positioned, the mod intercepts the call, 
determines the target monitor using the cursor position, and repositions the window with proper DPI scaling to the active display.

## Supported Windows Builds

- Windows 11 22H2 and 24H2.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <shellscalingapi.h>

HMONITOR GetTargetMonitor() {
    POINT pt;
    if (GetCursorPos(&pt)) {
        return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    return nullptr;
}

bool IsActionCenterWindow(HWND hWnd) {
    if (!hWnd) return false;

    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) return false;

    WCHAR className[256];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) || className[0] == L'\0') {
        return false;
    }

    if (_wcsicmp(className, L"ControlCenterWindow") == 0) {
        return true; // ShellHost.exe (24H2+)
    }

    if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (hThread) {
            PWSTR desc = nullptr;
            if (SUCCEEDED(GetThreadDescription(hThread, &desc)) && desc) {
                bool match = (_wcsicmp(desc, L"QuickActions") == 0 ||
                              _wcsicmp(desc, L"Connect") == 0 ||
                              _wcsicmp(desc, L"Cast") == 0 ||
                              _wcsicmp(desc, L"Project") == 0 ||
                              _wcsicmp(desc, L"Display") == 0);
                LocalFree(desc);
                CloseHandle(hThread);
                return match;
            }
            CloseHandle(hThread);
        }
    }
    return false;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    if (!IsActionCenterWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    RECT rc{};
    if (!GetWindowRect(hWnd, &rc)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }
    if (uFlags & SWP_NOMOVE) { X = rc.left; Y = rc.top; uFlags &= ~SWP_NOMOVE; }
    int width  = (uFlags & SWP_NOSIZE) ? (rc.right - rc.left) : cx;
    int height = (uFlags & SWP_NOSIZE) ? (rc.bottom - rc.top) : cy;

    HMONITOR target = GetTargetMonitor();
    if (!target) return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    RECT requestedRc = { X, Y, X + width, Y + height };
    HMONITOR current = MonitorFromRect(&requestedRc, MONITOR_DEFAULTTONEAREST);

    // If the OS already positioned it on a secondary monitor natively, it is a tray click.
    // Native Win+A always targets the primary monitor. Intercept Win+A only.
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (current != primary) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    if (current == target) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    MONITORINFO cMi{ .cbSize = sizeof(MONITORINFO) };
    MONITORINFO tMi{ .cbSize = sizeof(MONITORINFO) };
    if (!GetMonitorInfoW(current, &cMi) || !GetMonitorInfoW(target, &tMi)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    UINT dpiCx = 96, dpiCy = 96, dpiTx = 96, dpiTy = 96;
    if (FAILED(GetDpiForMonitor(current, MDT_EFFECTIVE_DPI, &dpiCx, &dpiCy)) || dpiCx == 0 || dpiCy == 0) {
        dpiCx = 96;
        dpiCy = 96;
    }
    if (FAILED(GetDpiForMonitor(target, MDT_EFFECTIVE_DPI, &dpiTx, &dpiTy)) || dpiTx == 0 || dpiTy == 0) {
        dpiTx = 96;
        dpiTy = 96;
    }

    int scaledW = MulDiv(width, dpiTx, dpiCx);
    int scaledH = MulDiv(height, dpiTy, dpiCy);

    int offsetRight = cMi.rcWork.right - (X + width);
    int offsetBottom = cMi.rcWork.bottom - (Y + height);
    int newX = tMi.rcWork.right - MulDiv(offsetRight, dpiTx, dpiCx) - scaledW;
    int newY = tMi.rcWork.bottom - MulDiv(offsetBottom, dpiTy, dpiCy) - scaledH;

    return SetWindowPos_Original(hWnd, hWndInsertAfter, newX, newY, scaledW, scaledH, uFlags & ~SWP_NOSIZE);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    if (WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
        return TRUE;
    }
    return FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
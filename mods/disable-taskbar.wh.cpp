// ==WindhawkMod==
// @id              disable-taskbar
// @name            Disable Taskbar
// @description     Completely hides the taskbar on all monitors
// @version         1.0
// @author          miraquel
// @github          https://github.com/miraquel
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Taskbar

Completely hides the Windows taskbar across all monitors. Ideal for kiosk
setups, custom shell environments, or presentations where an uncluttered
desktop is required.

## What it does

- Hides the primary taskbar and all secondary-monitor taskbars.
- Reclaims the reserved screen space so windows can use the full desktop area.
- Prevents the taskbar from reappearing (hooks `ShowWindow` and `SetWindowPos`).

## Restoring the taskbar

Disable or remove this mod in Windhawk, then restart Windows Explorer
(`explorer.exe`) via Task Manager to fully restore the taskbar and its
screen-space reservation.

## Supported Windows versions

Windows 10 and Windows 11 (64-bit).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reclaimScreenSpace: true
  $name: Reclaim screen space
  $description: >-
    Remove the taskbar's desktop space reservation so windows can use the full
    monitor area. A restart of Explorer may be required to restore it after
    disabling the mod.
*/
// ==/WindhawkModSettings==

#include <shellapi.h>

// Maximum taskbar height used as the threshold for detecting the screen-edge
// gap left behind when the taskbar is hidden.
static constexpr int kMaxTaskbarHeight = 80;

enum class Target { Explorer, StartMenuExperienceHost };
static Target g_target;

static struct {
    bool reclaimScreenSpace;
} g_settings;

static HWINEVENTHOOK g_winEventHook = nullptr;
static DWORD g_winEventThreadId = 0;
static HWINEVENTHOOK g_smehWinEventHook = nullptr;
static DWORD g_smehWinEventThreadId = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool IsTaskbarWindow(HWND hWnd) {
    WCHAR cls[64];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static bool IsTopLevelWindow(HWND hWnd) {
    return !(GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD);
}

// Returns the pixel gap between a window's bottom edge and its monitor's
// bottom edge if it falls in the 1–kMaxTaskbarHeight range (indicating a
// hidden taskbar reservation).  Returns 0 otherwise.
static int TaskbarGapBelow(int left, int top, int width, int height) {
    HMONITOR hMon = MonitorFromPoint({left + width / 2, top + height},
                                     MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);
    int gap = mi.rcMonitor.bottom - (top + height);
    return (gap > 0 && gap <= kMaxTaskbarHeight) ? gap : 0;
}

// Forward declaration needed by ShowWindow_Hook.
using SetWindowPos_t = decltype(&SetWindowPos);
extern SetWindowPos_t SetWindowPos_Original;

// ---------------------------------------------------------------------------
// Hook: ShowWindow
// ---------------------------------------------------------------------------

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_target == Target::Explorer && IsTaskbarWindow(hWnd) && nCmdShow != SW_HIDE) {
        return FALSE;
    }

    // The Start menu is positioned at process startup, then only shown/hidden
    // on Win-key press.  Correct its Y position at show-time to close the gap
    // left by the hidden taskbar.
    //
    // Note: GetParent() returns the owner HWND even for owned top-level
    // windows, so WS_CHILD is the correct check for non-child windows.
    if (g_target == Target::StartMenuExperienceHost &&
        IsTopLevelWindow(hWnd) &&
        nCmdShow != SW_HIDE) {
        RECT wr = {};
        GetWindowRect(hWnd, &wr);
        int width  = wr.right - wr.left;
        int height = wr.bottom - wr.top;
        int gap    = TaskbarGapBelow(wr.left, wr.top, width, height);
        if (gap > 0) {
            SetWindowPos_Original(hWnd, nullptr, wr.left, wr.top + gap, width, height,
                                  SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
        }
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// ---------------------------------------------------------------------------
// Hook: SetWindowPos
// ---------------------------------------------------------------------------

SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    if (g_target == Target::Explorer && IsTaskbarWindow(hWnd)) {
        if (uFlags & SWP_SHOWWINDOW) {
            uFlags = (uFlags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW | SWP_NOACTIVATE;
        } else if (IsWindowVisible(hWnd)) {
            // Kernel-level show bypassed our ShowWindow hook; re-hide immediately.
            ShowWindow_Original(hWnd, SW_HIDE);
            return TRUE;
        }
    }

    if (g_target == Target::StartMenuExperienceHost && IsTopLevelWindow(hWnd)) {
        RECT wr = {};
        GetWindowRect(hWnd, &wr);
        int actualX  = (uFlags & SWP_NOMOVE) ? wr.left              : X;
        int actualY  = (uFlags & SWP_NOMOVE) ? wr.top               : Y;
        int actualCx = (uFlags & SWP_NOSIZE) ? (wr.right - wr.left) : cx;
        int actualCy = (uFlags & SWP_NOSIZE) ? (wr.bottom - wr.top) : cy;
        int gap      = TaskbarGapBelow(actualX, actualY, actualCx, actualCy);
        if (gap > 0) {
            uFlags &= ~(SWP_NOMOVE | SWP_NOSIZE);
            X  = actualX;
            Y  = actualY + gap;
            cx = actualCx;
            cy = actualCy;
        }
    }

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ---------------------------------------------------------------------------
// Hook: MoveWindow
// ---------------------------------------------------------------------------

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;

BOOL WINAPI MoveWindow_Hook(HWND hWnd, int X, int Y, int nWidth, int nHeight,
                            BOOL bRepaint) {
    if (g_target == Target::StartMenuExperienceHost && IsTopLevelWindow(hWnd)) {
        Y += TaskbarGapBelow(X, Y, nWidth, nHeight);
    }
    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

// ---------------------------------------------------------------------------
// Hook: AnimateWindow
// ---------------------------------------------------------------------------

using AnimateWindow_t = decltype(&AnimateWindow);
AnimateWindow_t AnimateWindow_Original;

BOOL WINAPI AnimateWindow_Hook(HWND hWnd, DWORD dwTime, DWORD dwFlags) {
    if (IsTaskbarWindow(hWnd) && !(dwFlags & AW_HIDE)) {
        return FALSE;
    }
    return AnimateWindow_Original(hWnd, dwTime, dwFlags);
}

// ---------------------------------------------------------------------------
// Hook: SetWindowLongW / SetWindowLongPtrW
// Catches WS_VISIBLE being set directly, bypassing ShowWindow.
// ---------------------------------------------------------------------------

using SetWindowLongW_t = decltype(&SetWindowLongW);
SetWindowLongW_t SetWindowLongW_Original;

LONG WINAPI SetWindowLongW_Hook(HWND hWnd, int nIndex, LONG dwNewLong) {
    if (nIndex == GWL_STYLE && IsTaskbarWindow(hWnd)) {
        dwNewLong &= ~WS_VISIBLE;
    }
    return SetWindowLongW_Original(hWnd, nIndex, dwNewLong);
}

using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Original;

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (nIndex == GWL_STYLE && IsTaskbarWindow(hWnd)) {
        dwNewLong &= ~WS_VISIBLE;
    }
    return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
}

// ---------------------------------------------------------------------------
// WinEvent — Explorer: safety net for kernel-level taskbar show calls
// ---------------------------------------------------------------------------

static void CALLBACK OnWinEvent(HWINEVENTHOOK, DWORD, HWND hWnd,
                                LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (IsTaskbarWindow(hWnd) && ShowWindow_Original) {
        ShowWindow_Original(hWnd, SW_HIDE);
    }
}

static DWORD WINAPI WinEventThreadProc(LPVOID) {
    g_winEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW,
                                     nullptr, OnWinEvent, 0, 0,
                                     WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook) {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWinEvent(g_winEventHook);
    g_winEventHook = nullptr;
    return 0;
}

// ---------------------------------------------------------------------------
// WinEvent — SMEH: safety net for Start menu position changes via any path
// ---------------------------------------------------------------------------

static void CALLBACK OnWinEventSmeh(HWINEVENTHOOK, DWORD, HWND hWnd,
                                     LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hWnd) {
        return;
    }
    if (!IsTopLevelWindow(hWnd)) {
        return;
    }
    RECT wr = {};
    if (!GetWindowRect(hWnd, &wr)) {
        return;
    }
    int width  = wr.right - wr.left;
    int height = wr.bottom - wr.top;
    if (width < 100 || height < 100) {
        return;
    }
    int gap = TaskbarGapBelow(wr.left, wr.top, width, height);
    if (gap > 0) {
        SetWindowPos_Original(hWnd, nullptr, wr.left, wr.top + gap, width, height,
                              SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    }
}

static DWORD WINAPI WinEventThreadProcSmeh(LPVOID) {
    g_smehWinEventHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, OnWinEventSmeh,
        GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);
    if (!g_smehWinEventHook) {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWinEvent(g_smehWinEventHook);
    g_smehWinEventHook = nullptr;
    return 0;
}

// ---------------------------------------------------------------------------
// Taskbar management (Explorer only)
// ---------------------------------------------------------------------------

static void HideTaskbarWindow(HWND hWnd) {
    if (g_settings.reclaimScreenSpace) {
        APPBARDATA abd = {sizeof(abd)};
        abd.hWnd = hWnd;
        SHAppBarMessage(ABM_REMOVE, &abd);
    }
    ShowWindow_Original(hWnd, SW_HIDE);
}

static void HideAllTaskbars() {
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hPrimary) {
        HideTaskbarWindow(hPrimary);
    }
    HWND hSecondary = nullptr;
    while ((hSecondary = FindWindowEx(nullptr, hSecondary,
                                      L"Shell_SecondaryTrayWnd",
                                      nullptr)) != nullptr) {
        HideTaskbarWindow(hSecondary);
    }
}

static void ShowAllTaskbars() {
    HWND hPrimary = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hPrimary) {
        ShowWindow_Original(hPrimary, SW_SHOW);
    }
    HWND hSecondary = nullptr;
    while ((hSecondary = FindWindowEx(nullptr, hSecondary,
                                      L"Shell_SecondaryTrayWnd",
                                      nullptr)) != nullptr) {
        ShowWindow_Original(hSecondary, SW_SHOW);
    }
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static void LoadSettings() {
    g_settings.reclaimScreenSpace = Wh_GetIntSetting(L"reclaimScreenSpace") != 0;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool StartThread(LPTHREAD_START_ROUTINE fn, DWORD* outTid) {
    HANDLE hThread = CreateThread(nullptr, 0, fn, nullptr, 0, outTid);
    if (!hThread) {
        return false;
    }
    CloseHandle(hThread);
    return true;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    WCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, ARRAYSIZE(exePath));
    const WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    if (_wcsicmp(exeName, L"StartMenuExperienceHost.exe") == 0) {
        g_target = Target::StartMenuExperienceHost;
        Wh_SetFunctionHook((void*)ShowWindow,   (void*)ShowWindow_Hook,   (void**)&ShowWindow_Original);
        Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
        Wh_SetFunctionHook((void*)MoveWindow,   (void*)MoveWindow_Hook,   (void**)&MoveWindow_Original);
        return TRUE;
    }

    g_target = Target::Explorer;
    Wh_SetFunctionHook((void*)ShowWindow,        (void*)ShowWindow_Hook,        (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos,      (void*)SetWindowPos_Hook,      (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook((void*)AnimateWindow,     (void*)AnimateWindow_Hook,     (void**)&AnimateWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowLongW,    (void*)SetWindowLongW_Hook,    (void**)&SetWindowLongW_Original);
    Wh_SetFunctionHook((void*)SetWindowLongPtrW, (void*)SetWindowLongPtrW_Hook, (void**)&SetWindowLongPtrW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_target == Target::Explorer) {
        HideAllTaskbars();
        StartThread(WinEventThreadProc, &g_winEventThreadId);
    } else if (g_target == Target::StartMenuExperienceHost) {
        StartThread(WinEventThreadProcSmeh, &g_smehWinEventThreadId);
    }
}

void Wh_ModBeforeUninit() {
    if (g_target == Target::Explorer) {
        if (g_winEventThreadId) {
            PostThreadMessageW(g_winEventThreadId, WM_QUIT, 0, 0);
            g_winEventThreadId = 0;
        }
        ShowAllTaskbars();
    } else if (g_target == Target::StartMenuExperienceHost) {
        if (g_smehWinEventThreadId) {
            PostThreadMessageW(g_smehWinEventThreadId, WM_QUIT, 0, 0);
            g_smehWinEventThreadId = 0;
        }
    }
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

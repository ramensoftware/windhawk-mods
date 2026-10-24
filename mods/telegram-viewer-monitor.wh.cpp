// ==WindhawkMod==
// @id              telegram-viewer-monitor
// @name            Telegram: media viewer on the chat monitor
// @name:ru-RU      Telegram: просмотрщик на мониторе чата
// @description     Telegram Desktop secondary windows (photo/video viewer, etc.) open on the monitor of the main window instead of the remembered one
// @description:ru-RU Окна Telegram Desktop (просмотр фото/видео и др.) открываются на том мониторе, где находится главное окно, а не на запомненном
// @version         1.0
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/telegram-viewer-monitor
// @license         MIT
// @include         Telegram.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Telegram: media viewer on the chat monitor

Telegram Desktop remembers which monitor the media viewer was opened on and
opens it there again, even if the main window has moved to another monitor.
The mod intercepts the showing of Telegram's secondary windows and, if a window
ends up on a different monitor than the main window (which is active at the
moment it opens), moves it to the main window's monitor. A fullscreen window
stays fullscreen.

System files and Telegram data are not modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- titleFilter: ""
  $name: Window title filter
  $name:ru-RU: Фильтр по заголовку окна
  $description: Move only windows whose title contains this text. Empty - move all secondary Telegram windows.
  $description:ru-RU: Переносить только окна, в заголовке которых есть этот текст. Пусто — переносить все второстепенные окна Telegram.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wchar.h>

static WCHAR g_titleFilter[128];

static void LoadSettings() {
    g_titleFilter[0] = L'\0';
    PCWSTR s = Wh_GetStringSetting(L"titleFilter");
    if (s) {
        wcsncpy(g_titleFilter, s, ARRAYSIZE(g_titleFilter) - 1);
        g_titleFilter[ARRAYSIZE(g_titleFilter) - 1] = L'\0';
        Wh_FreeStringSetting(s);
    }
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original;

// Moves the window to the monitor of the active (main) Telegram window if needed.
static void MoveToMainMonitor(HWND hwnd) {
    if (!hwnd || (GetWindowLongW(hwnd, GWL_STYLE) & WS_CHILD)) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return;
    }

    WCHAR cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    // Regular Qt windows; menus, tooltips, etc. have other classes.
    if (!wcsstr(cls, L"QWindowIcon")) {
        return;
    }

    HWND main = GetForegroundWindow();
    if (!main || main == hwnd) {
        return;
    }
    DWORD mainPid = 0;
    GetWindowThreadProcessId(main, &mainPid);
    if (mainPid != pid) {
        return;
    }

    WCHAR title[256] = L"";
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    if (g_titleFilter[0] && !wcsstr(title, g_titleFilter)) {
        return;
    }

    HMONITOR target = MonitorFromWindow(main, MONITOR_DEFAULTTONEAREST);
    HMONITOR current = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (target == current) {
        return;
    }

    MONITORINFO tm = {sizeof(tm)};
    MONITORINFO cm = {sizeof(cm)};
    if (!GetMonitorInfoW(target, &tm) || !GetMonitorInfoW(current, &cm)) {
        return;
    }

    RECT r;
    if (!GetWindowRect(hwnd, &r)) {
        return;
    }
    int w = r.right - r.left;
    int h = r.bottom - r.top;

    int x, y;
    bool fullscreen = w >= (cm.rcMonitor.right - cm.rcMonitor.left) - 2 &&
                      h >= (cm.rcMonitor.bottom - cm.rcMonitor.top) - 2;
    if (fullscreen) {
        x = tm.rcMonitor.left;
        y = tm.rcMonitor.top;
        w = tm.rcMonitor.right - tm.rcMonitor.left;
        h = tm.rcMonitor.bottom - tm.rcMonitor.top;
    } else {
        int tw = tm.rcWork.right - tm.rcWork.left;
        int th = tm.rcWork.bottom - tm.rcWork.top;
        if (w > tw) w = tw;
        if (h > th) h = th;
        x = tm.rcWork.left + (tw - w) / 2;
        y = tm.rcWork.top + (th - h) / 2;
    }

    Wh_Log(L"Moving window %p class=%s title=%s: (%d,%d %dx%d) fullscreen=%d",
           hwnd, cls, title, x, y, w, h, (int)fullscreen);
    SetWindowPos_Original(hwnd, nullptr, x, y, w, h,
                          SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE &&
        nCmdShow != SW_SHOWMINIMIZED && nCmdShow != SW_SHOWMINNOACTIVE &&
        nCmdShow != SW_FORCEMINIMIZE && !IsWindowVisible(hWnd)) {
        MoveToMainMonitor(hWnd);
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    bool show = (uFlags & SWP_SHOWWINDOW) && !IsWindowVisible(hWnd);
    BOOL ret = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    if (show) {
        MoveToMainMonitor(hWnd);
    }
    return ret;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                       (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

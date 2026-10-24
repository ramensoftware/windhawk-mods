// ==WindhawkMod==
// @id              telegram-viewer-monitor
// @name            Telegram: media viewer on the chat monitor
// @name:ru-RU      Telegram: просмотрщик на мониторе чата
// @description     Telegram Desktop secondary windows (photo/video viewer, etc.) open on the monitor of the main window instead of the remembered one
// @description:ru-RU Окна Telegram Desktop (просмотр фото/видео и др.) открываются на том мониторе, где находится главное окно, а не на запомненном
// @version         1.2
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/telegram-viewer-monitor
// @license         MIT
// @include         Telegram.exe
// @compilerOptions -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Telegram: media viewer on the chat monitor

Telegram Desktop remembers which monitor the media viewer was opened on and
opens it there again, even if the main window has moved to another monitor.
The mod intercepts the showing of Telegram's secondary windows and, if a window
is about to appear on a different monitor than the main window (which is active
at the moment it opens), moves it to the main window's monitor before it is
shown.

Which windows are moved: by default **all** secondary Telegram windows, not
only the media viewer - also picture-in-picture video, call windows and chats
opened in a separate window. If you keep some of them on another monitor on
purpose, use the *Window title filter* setting to limit the mod to windows
with a given title.

How a window is placed:
- it keeps its relative position on the screen (a window in the top right
  corner of one monitor appears in the top right corner of the other);
- a window larger than the target monitor is shrunk to fit;
- a fullscreen window stays fullscreen;
- a maximized window stays maximized, and when restored it stays on the new
  monitor;
- monitors with different scaling are taken into account.

Keeping the position and accounting for scaling can each be turned off in the
settings.

System files and Telegram data are not modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepPosition: true
  $name: Keep window position
  $name:ru-RU: Сохранять положение окна
  $description: On - the window keeps its place relative to the screen (a window in a corner stays in the same corner of the other monitor). Off - the window is centered on the monitor of the main window.
  $description:ru-RU: Вкл — окно сохраняет своё место на экране (окно в углу остаётся в том же углу другого монитора). Выкл — окно встаёт в центр монитора главного окна.
- scaleForDpi: true
  $name: Account for monitor scaling
  $name:ru-RU: Учитывать масштаб мониторов
  $description: On - when monitors have different scaling (for example 100% and 150%), the window is placed so that it fits after Telegram resizes it for the new scale. Turn off if windows are placed wrongly on such a setup.
  $description:ru-RU: Вкл — при разном масштабе мониторов (например, 100% и 150%) окно ставится так, чтобы поместиться после того, как Telegram изменит его размер под новый масштаб. Выключите, если на такой конфигурации окна встают неправильно.
- titleFilter: ""
  $name: Window title filter
  $name:ru-RU: Фильтр по заголовку окна
  $description: Move only windows whose title contains this text. Empty - move all secondary Telegram windows. Window titles follow Telegram's interface language, so the filter works only for that language.
  $description:ru-RU: Переносить только окна, в заголовке которых есть этот текст. Пусто — переносить все второстепенные окна Telegram. Заголовки окон зависят от языка интерфейса Telegram, поэтому фильтр работает только для этого языка.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellscalingapi.h>
#include <wchar.h>

#include <mutex>
#include <string>

struct Settings {
    bool keepPosition = true;
    bool scaleForDpi = true;
    std::wstring titleFilter;
};

// Settings are written on the Windhawk engine thread and read on Telegram's
// UI thread.
static std::mutex g_settingsMutex;
static Settings g_settings;

static void LoadSettings() {
    Settings s;
    s.keepPosition = Wh_GetIntSetting(L"keepPosition") != 0;
    s.scaleForDpi = Wh_GetIntSetting(L"scaleForDpi") != 0;
    s.titleFilter = WindhawkUtils::StringSetting::make(L"titleFilter").get();
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

static bool TitleMatchesFilter(HWND hwnd, const std::wstring& filter) {
    if (filter.empty()) {
        return true;
    }
    WCHAR title[256] = L"";
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    return wcsstr(title, filter.c_str()) != nullptr;
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original;

// Where a window that is about to be shown should go.
struct MoveTarget {
    RECT rect;        // new window rect, screen coordinates
    bool zoomed;      // the window is maximized
    RECT normalRect;  // new restore rect for a maximized window, screen coordinates
};

static UINT GetMonitorDpi(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 96;
    }
    return dpiX;
}

// Maps one axis of a window from the source work area to the target one,
// keeping the window's relative position (or centering it). `expected` is the
// size the window will have on the target monitor, already clamped to the
// target.
static int MapAxis(int pos, int origSize, int fromStart, int fromLen,
                   int toStart, int toLen, int expected, bool keepPosition) {
    double frac = 0.5;
    if (keepPosition && fromLen > origSize) {
        frac = double(pos - fromStart) / double(fromLen - origSize);
        if (frac < 0) frac = 0;
        if (frac > 1) frac = 1;
    }
    int p = toStart + int(frac * (toLen - expected) + 0.5);
    if (p + expected > toStart + toLen) p = toStart + toLen - expected;
    if (p < toStart) p = toStart;
    return p;
}

// Maps a non-maximized, non-fullscreen window rect from one work area to
// another. The window gets the size it has now scaled to the target DPI and
// clamped to the target work area. If `rescaled` is true, Telegram will
// rescale the window on the DPI change, so the returned size is the one to
// set now (before that rescale); otherwise it's the final size.
static RECT MapRect(const RECT& r, const RECT& from, const RECT& to,
                    UINT fromDpi, UINT toDpi, bool keepPosition,
                    bool rescaled) {
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    int toW = to.right - to.left;
    int toH = to.bottom - to.top;

    int ew = MulDiv(w, toDpi, fromDpi);
    int eh = MulDiv(h, toDpi, fromDpi);
    if (ew > toW) ew = toW;
    if (eh > toH) eh = toH;
    int nw = rescaled ? MulDiv(ew, fromDpi, toDpi) : ew;
    int nh = rescaled ? MulDiv(eh, fromDpi, toDpi) : eh;

    int x = MapAxis(r.left, w, from.left, from.right - from.left, to.left,
                    toW, ew, keepPosition);
    int y = MapAxis(r.top, h, from.top, from.bottom - from.top, to.top, toH,
                    eh, keepPosition);
    return {x, y, x + nw, y + nh};
}

// Offset between workspace coordinates (used by WINDOWPLACEMENT) and screen
// coordinates.
static POINT GetWorkspaceOffset(HWND hwnd) {
    if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return {0, 0};
    }
    MONITORINFO pm = {sizeof(pm)};
    if (!GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
                         &pm)) {
        return {0, 0};
    }
    return {pm.rcWork.left - pm.rcMonitor.left,
            pm.rcWork.top - pm.rcMonitor.top};
}

// Decides whether a Telegram window that is about to be shown with the given
// rect must move to the monitor of the main (foreground) Telegram window, and
// where to.
static bool GetMoveTarget(HWND hwnd, const RECT& rect, MoveTarget* target) {
    if (!hwnd || (GetWindowLongW(hwnd, GWL_STYLE) & WS_CHILD)) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }

    WCHAR cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    // Regular Qt windows; menus, tooltips, etc. have other classes.
    if (!wcsstr(cls, L"QWindowIcon")) {
        return false;
    }

    HWND main = GetForegroundWindow();
    if (!main || main == hwnd) {
        return false;
    }
    DWORD mainPid = 0;
    GetWindowThreadProcessId(main, &mainPid);
    if (mainPid != pid) {
        return false;
    }

    Settings settings = GetSettings();
    if (!TitleMatchesFilter(hwnd, settings.titleFilter)) {
        return false;
    }

    HMONITOR targetMonitor = MonitorFromWindow(main, MONITOR_DEFAULTTONEAREST);
    HMONITOR currentMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    if (targetMonitor == currentMonitor) {
        return false;
    }

    MONITORINFO tm = {sizeof(tm)};
    MONITORINFO cm = {sizeof(cm)};
    if (!GetMonitorInfoW(targetMonitor, &tm) ||
        !GetMonitorInfoW(currentMonitor, &cm)) {
        return false;
    }

    // Telegram is per-monitor DPI aware and rescales its windows when they
    // move to a monitor with a different scale. Other windows aren't rescaled.
    UINT fromDpi = 96, toDpi = 96;
    if (settings.scaleForDpi &&
        GetAwarenessFromDpiAwarenessContext(GetWindowDpiAwarenessContext(
            hwnd)) == DPI_AWARENESS_PER_MONITOR_AWARE) {
        fromDpi = GetMonitorDpi(currentMonitor);
        toDpi = GetMonitorDpi(targetMonitor);
    }

    target->zoomed = IsZoomed(hwnd);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (target->zoomed) {
        // Keep the maximized frame overhang (invisible borders) of the window.
        target->rect = {
            tm.rcWork.left + (rect.left - cm.rcWork.left),
            tm.rcWork.top + (rect.top - cm.rcWork.top),
            tm.rcWork.right + (rect.right - cm.rcWork.right),
            tm.rcWork.bottom + (rect.bottom - cm.rcWork.bottom),
        };

        WINDOWPLACEMENT wp = {sizeof(wp)};
        if (!GetWindowPlacement(hwnd, &wp)) {
            return false;
        }
        POINT off = GetWorkspaceOffset(hwnd);
        RECT normal = wp.rcNormalPosition;
        OffsetRect(&normal, off.x, off.y);
        // Nothing rescales the restore rect when the window is restored on
        // the target monitor, so it gets the final size right away.
        target->normalRect = MapRect(normal, cm.rcWork, tm.rcWork, fromDpi,
                                     toDpi, settings.keepPosition, false);
    } else if (w >= (cm.rcMonitor.right - cm.rcMonitor.left) - 2 &&
               h >= (cm.rcMonitor.bottom - cm.rcMonitor.top) - 2) {
        // Fullscreen.
        target->rect = tm.rcMonitor;
    } else {
        target->rect = MapRect(rect, cm.rcWork, tm.rcWork, fromDpi, toDpi,
                               settings.keepPosition, true);
    }

    Wh_Log(L"Moving window %p class=%s: (%d,%d)-(%d,%d) -> (%d,%d)-(%d,%d)"
           L" zoomed=%d dpi %u->%u",
           hwnd, cls, rect.left, rect.top, rect.right, rect.bottom,
           target->rect.left, target->rect.top, target->rect.right,
           target->rect.bottom, (int)target->zoomed, fromDpi, toDpi);
    return true;
}

// For a maximized window, moves its restore rect to the target monitor, so
// that restoring it doesn't send it back to the old monitor.
static void ApplyNormalRect(HWND hwnd, const MoveTarget& target) {
    if (!target.zoomed) {
        return;
    }
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (!GetWindowPlacement(hwnd, &wp)) {
        return;
    }
    POINT off = GetWorkspaceOffset(hwnd);
    wp.rcNormalPosition = target.normalRect;
    OffsetRect(&wp.rcNormalPosition, -off.x, -off.y);
    wp.flags = 0;
    // The window is still hidden; keep it hidden and maximized.
    wp.showCmd = SW_HIDE;
    SetWindowPlacement(hwnd, &wp);
}

static bool IsShowCommand(int nCmdShow) {
    switch (nCmdShow) {
        case SW_HIDE:
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
        case SW_SHOWMINNOACTIVE:
        case SW_FORCEMINIMIZE:
            return false;
    }
    return true;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (IsShowCommand(nCmdShow) && !IsWindowVisible(hWnd)) {
        RECT r;
        MoveTarget target;
        if (GetWindowRect(hWnd, &r) && GetMoveTarget(hWnd, r, &target)) {
            ApplyNormalRect(hWnd, target);
            SetWindowPos_Original(hWnd, nullptr, target.rect.left,
                                  target.rect.top,
                                  target.rect.right - target.rect.left,
                                  target.rect.bottom - target.rect.top,
                                  SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    // Change the requested rect before the window is shown, so that it
    // appears on the right monitor right away.
    if ((uFlags & SWP_SHOWWINDOW) && !IsWindowVisible(hWnd)) {
        RECT r;
        if (GetWindowRect(hWnd, &r)) {
            if (!(uFlags & SWP_NOMOVE)) {
                OffsetRect(&r, X - r.left, Y - r.top);
            }
            if (!(uFlags & SWP_NOSIZE)) {
                r.right = r.left + cx;
                r.bottom = r.top + cy;
            }
            MoveTarget target;
            if (GetMoveTarget(hWnd, r, &target)) {
                ApplyNormalRect(hWnd, target);
                X = target.rect.left;
                Y = target.rect.top;
                cx = target.rect.right - target.rect.left;
                cy = target.rect.bottom - target.rect.top;
                uFlags &= ~(SWP_NOMOVE | SWP_NOSIZE);
            }
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL Wh_ModInit() {
    LoadSettings();
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

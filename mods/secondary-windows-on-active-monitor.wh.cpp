// ==WindhawkMod==
// @id              secondary-windows-on-active-monitor
// @name            Secondary windows on the active monitor
// @name:ru-RU      Второстепенные окна на активном мониторе
// @description     Multi-monitor: secondary windows of the selected apps (Telegram media viewer, etc.) open on the monitor of the app's active window instead of the remembered one
// @description:ru-RU Для нескольких мониторов: второстепенные окна выбранных приложений (просмотрщик Telegram и др.) открываются на мониторе активного окна приложения, а не на запомненном
// @version         2.1
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/secondary-windows-on-active-monitor
// @license         MIT
// @include         *
// @compilerOptions -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Secondary windows on the active monitor

Useful if you have **several monitors**. Many apps remember which monitor a
secondary window (media viewer, player, dialog) was opened on and open it there
again, even if you now work with the app on another monitor. For example,
Telegram Desktop opens photos and videos on the monitor where the viewer was
last time, not where the chat is.

![Demo](https://raw.githubusercontent.com/limpompo/windhawk-mods-limpompo/main/mods/secondary-windows-on-active-monitor/demo.gif)

The mod intercepts the showing of secondary windows of the apps from the
*Applications* list and, if a window is about to appear on a different monitor
than the app's active window, moves it to the active window's monitor before
it is shown. By default the list contains a placeholder (`test.exe`), replace it with your app, for example Telegram Desktop (`Telegram.exe`);
add any other app by its executable name in the mod settings.

For apps that are not built with Qt, the mod decides which windows are regular
ones by a heuristic (it skips menus, tooltips, tool windows and the like), so
it may move something it shouldn't, such as a drop-down list or a splash
screen. Telegram is the only app the mod was tested with; if another app
misbehaves, use the *Window title filter* of that app.

Which windows are moved: **all** secondary windows of the app that get their
own window - for Telegram also picture-in-picture video, call windows and
chats opened in a separate window. Menus, tooltips, notifications and tool
windows are not moved. If you keep some windows on another monitor on purpose,
set a *Window title filter* for the app to limit the mod to windows with a
given title.

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

The mod is loaded into every process and unloads itself at once from those that
are not in the *Applications* list. An app added to the list starts to work
after it is restarted; removing an app from the list applies immediately.

System files and app data are not modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- applications:
  - - process: test.exe
      $name: Process
      $name:ru-RU: Процесс
      $description: Executable name (with or without .exe), for example Telegram.exe, or its full path.
      $description:ru-RU: Имя исполняемого файла (с .exe или без), например Telegram.exe, или полный путь к нему.
    - titleFilter: ""
      $name: Window title filter
      $name:ru-RU: Фильтр по заголовку окна
      $description: Move only windows whose title contains this text. Empty - move all secondary windows of the app. Titles often depend on the app's interface language.
      $description:ru-RU: Переносить только окна, в заголовке которых есть этот текст. Пусто — все второстепенные окна приложения. Заголовки часто зависят от языка интерфейса приложения.
  $name: Applications
  $name:ru-RU: Приложения
  $description: Apps whose secondary windows open on the monitor of the app's active window.
  $description:ru-RU: Приложения, второстепенные окна которых открываются на мониторе активного окна приложения.
- keepPosition: true
  $name: Keep window position
  $name:ru-RU: Сохранять положение окна
  $description: On - the window keeps its place relative to the screen (a window in a corner stays in the same corner of the other monitor). Off - the window is centered on the monitor of the main window.
  $description:ru-RU: Вкл — окно сохраняет своё место на экране (окно в углу остаётся в том же углу другого монитора). Выкл — окно встаёт в центр монитора главного окна.
- scaleForDpi: true
  $name: Account for monitor scaling
  $name:ru-RU: Учитывать масштаб мониторов
  $description: On - when monitors have different scaling (for example 100% and 150%), the window is placed so that it fits after the app resizes it for the new scale. Turn off if windows are placed wrongly on such a setup.
  $description:ru-RU: Вкл — при разном масштабе мониторов (например, 100% и 150%) окно ставится так, чтобы поместиться после того, как приложение изменит его размер под новый масштаб. Выключите, если на такой конфигурации окна встают неправильно.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellscalingapi.h>
#include <wchar.h>
#include <wctype.h>

#include <mutex>
#include <string>

struct Settings {
    bool matched = false;  // the current process is in the Applications list
    bool keepPosition = true;
    bool scaleForDpi = true;
    std::wstring titleFilter;
};

// Settings are written on the Windhawk engine thread and read on the app's
// UI thread.
static std::mutex g_settingsMutex;
static Settings g_settings;

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = towlower(c);
    }
    return s;
}

static std::wstring StripExe(std::wstring s) {
    if (s.size() >= 4 && s.compare(s.size() - 4, 4, L".exe") == 0) {
        s.resize(s.size() - 4);
    }
    return s;
}

// Matches a process from the settings (a file name with or without .exe, or a
// full path) against the current process path.
static bool ProcessMatches(std::wstring entry, const std::wstring& path) {
    entry = ToLower(entry);
    while (!entry.empty() && iswspace(entry.back())) entry.pop_back();
    while (!entry.empty() && iswspace(entry.front())) entry.erase(0, 1);
    if (entry.empty()) {
        return false;
    }
    if (entry.find_first_of(L"\\/") != std::wstring::npos) {
        return StripExe(entry) == StripExe(path);
    }
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    return StripExe(entry) == StripExe(name);
}

static void LoadSettings() {
    Settings s;
    s.keepPosition = Wh_GetIntSetting(L"keepPosition") != 0;
    s.scaleForDpi = Wh_GetIntSetting(L"scaleForDpi") != 0;

    WCHAR path[MAX_PATH] = L"";
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    std::wstring processPath =
        len && len < ARRAYSIZE(path) ? ToLower(path) : std::wstring();

    for (int i = 0; !processPath.empty(); i++) {
        auto process =
            WindhawkUtils::StringSetting::make(L"applications[%d].process", i);
        if (!*process.get()) {
            break;
        }
        if (ProcessMatches(process.get(), processPath)) {
            s.matched = true;
            s.titleFilter = WindhawkUtils::StringSetting::make(
                                L"applications[%d].titleFilter", i)
                                .get();
            break;
        }
    }

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
// clamped to the target work area. If `rescaled` is true, the app will
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

// Whether a top-level window is a regular app window, not a menu, tooltip,
// notification, drop-down list or tool window.
static bool IsRegularWindow(HWND hwnd, PCWSTR cls) {
    // Qt: regular windows have the QWindowIcon class; menus, tooltips and
    // tool windows (Telegram notifications) have other QWindow* classes.
    if (wcsstr(cls, L"QWindow")) {
        return wcsstr(cls, L"QWindowIcon") != nullptr;
    }

    static const PCWSTR kSkipClasses[] = {
        L"#32768",           // menu
        L"#32769",           // desktop
        L"#32771",           // Alt+Tab
        L"tooltips_class32",
        L"ComboLBox",
        L"SysShadow",
        L"IME",
        L"MSCTFIME UI",
    };
    for (PCWSTR skip : kSkipClasses) {
        if (_wcsicmp(cls, skip) == 0) {
            return false;
        }
    }

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        return false;
    }
    return true;
}

// Decides whether a window that is about to be shown with the given rect must
// move to the monitor of the app's active (foreground) window, and where to.
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
    if (!IsRegularWindow(hwnd, cls)) {
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
    if (!settings.matched ||
        !TitleMatchesFilter(hwnd, settings.titleFilter)) {
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

    // Per-monitor DPI aware apps (such as Telegram) rescale their windows when
    // they move to a monitor with a different scale. Other windows aren't
    // rescaled by the app.
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
    // The mod is loaded into every process; unload from those that are not
    // in the Applications list.
    if (!GetSettings().matched) {
        return FALSE;
    }
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    Wh_Log(L"Hooked");
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    LoadSettings();
    // Reload (and unload) when the process was removed from the Applications
    // list.
    *bReload = !GetSettings().matched;
    return TRUE;
}

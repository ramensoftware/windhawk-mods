// ==WindhawkMod==
// @id              open-windows-on-cursor-monitor
// @name            Open windows on the cursor's monitor
// @name:zh-CN      窗口跟随鼠标所在屏幕打开
// @description    Clicking a taskbar button, a tray icon or a Start menu entry opens the window on the monitor under the mouse cursor, and newly launched programs open there too
// @description:zh-CN  点击任务栏/托盘/开始菜单里的程序时，窗口出现在鼠标所在的显示器；新启动的程序也开在鼠标所在的屏幕
// @version         1.0
// @author          qstdnx
// @github          https://github.com/qstdnx
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion -luser32 -ldwmapi -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Demo](https://i.imgur.com/zQZIHsM.gif)

Windows restores a window on the monitor it was last on, and new
programs always open on the primary monitor. This mod makes every window open
on the monitor under the mouse cursor instead. All four features have their own


# 任务栏 / 托盘点击窗口跟随鼠标所在屏幕

Windows 默认行为是：点击任务栏上的程序图标时，窗口回到它**之前所在的显示器**；
点击托盘（通知区）图标时，程序弹出的窗口也常常出现在主显示器上。

本插件让这两种情况弹出的窗口，都出现在**鼠标所在的显示器**（也就是你点击的
那个任务栏所在的屏幕）。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarClickEnabled: true
  $name: Taskbar button click
  $name:zh-CN: 任务栏图标点击
  $description: >-
    Move the window to the monitor under the mouse cursor when its taskbar
    button is clicked
  $description:zh-CN: >-
    点击任务栏上的程序图标激活窗口时，把窗口移动到鼠标所在的显示器
- trayWindowEnabled: true
  $name: Tray icon click
  $name:zh-CN: 托盘图标点击
  $description: >-
    Move the window raised by a tray (notification area) icon click to the
    monitor under the mouse cursor - both newly opened and already open windows
  $description:zh-CN: >-
    点击托盘（通知区）图标后弹出的窗口（新弹出的、或已打开被激活的），
    移动到鼠标所在的显示器
- newWindowEnabled: true
  $name: Newly launched programs
  $name:zh-CN: 新打开的软件
  $description: >-
    A newly launched program normally opens its main window on the primary
    monitor. With this on, it opens on the monitor under the mouse cursor
  $description:zh-CN: >-
    新启动的程序，它的主窗口默认会开在主屏幕上。开启后改为开在鼠标所在的屏幕
- startMenuClickEnabled: true
  $name: Start menu / search launch
  $name:zh-CN: 开始菜单 / 搜索面板启动
  $description: >-
    Launching a program from the Start menu or the search panel opens it on the
    monitor under the mouse cursor, whether it's newly launched or already open
  $description:zh-CN: >-
    点击开始菜单（或搜索面板）里的程序时，不管它是新启动、还是已经打开的
    现有窗口，都让它出现在鼠标所在的屏幕
- activeWindowClick: move
  $name: Clicking the taskbar button of the active window
  $name:zh-CN: 点击“当前活动窗口”的任务栏图标时
  $description: >-
    Windows minimizes the active window when its taskbar button is clicked.
    With "move", a single click brings it over instead when it's on another
    monitor
  $description:zh-CN: >-
    窗口未最小化且是当前活动窗口时，系统默认行为是把它最小化。选 "move" 时：
    若该窗口不在鼠标所在屏幕，则只点一下就把它移过来，而不再最小化
  $options:
  - move: Move it over (don't minimize)
  - minimize: Keep the default behavior (minimize)
  $options:zh-CN:
  - move: 移到鼠标所在屏幕（不再最小化）
  - minimize: 保持系统默认（最小化）
- windowSizeMode: auto
  $name: Window size when moving across monitors
  $name:zh-CN: 跨屏移动时的窗口大小
  $description: >-
    With "auto" (recommended) the window is only moved - the size is left to
    Windows and the app, exactly like dragging the window over by hand, so
    there's no extra zoom animation. The other modes resize it instead
  $description:zh-CN: >-
    默认 auto：只移动位置，大小交给 Windows 和程序自己按缩放比处理（和用鼠标
    拖动窗口到另一个屏幕的效果一致），不会出现额外的缩放过程；其余模式由插件
    主动调整大小
  $options:
  - auto: Move only, let the system handle the size (recommended)
  - relative: Keep the window's share of the screen
  - dpi: Keep the logical pixel size
  - none: Don't adjust the size
  $options:zh-CN:
  - auto: 只移动，大小交给系统/程序处理（推荐）
  - relative: 保持相对屏幕的比例（两块屏上占比一致）
  - dpi: 保持逻辑像素大小（仅按缩放比换算）
  - none: 完全不调整大小
- clampToWorkArea: true
  $name: Keep the window inside the target monitor
  $name:zh-CN: 确保窗口完整落在目标屏幕内
  $description: >-
    Pull the window back into the target monitor's work area when it ends up
    outside of it
  $description:zh-CN: 移动后若窗口超出目标屏幕的可用区域，自动收回到屏幕内
- placement: center
  $name: Window placement
  $name:zh-CN: 窗口放置方式
  $options:
  - center: Center it on the monitor under the cursor
  - keepOffset: Keep its relative position
  $options:zh-CN:
  - center: 在鼠标所在屏幕居中
  - keepOffset: 保持相对位置平移到鼠标所在屏幕
- trayArea: right
  $name: Tray area detection
  $name:zh-CN: 托盘区域判定
  $description: >-
    How "the cursor is on the tray area" is determined. Only used by the tray
    icon click feature
  $description:zh-CN: >-
    判定“鼠标位于托盘区域”的方式，仅在托盘图标点击功能中使用
  $options:
  - right: Right half of the taskbar (where the tray usually is)
  - full: Anywhere on the taskbar
  $options:zh-CN:
  - right: 任务栏右半部分（托盘通常在这里）
  - full: 任务栏任意位置
- taskbarClickDelayMs: 100
  $name: Taskbar click delay (ms)
  $name:zh-CN: 任务栏点击：移动窗口的延迟（毫秒）
  $description: >-
    How long to wait for the window to be activated after the click. Increase
    it if a window is occasionally not moved
  $description:zh-CN: >-
    点击后等待窗口被激活的延迟。若窗口偶尔没被移动，可尝试调大
- trayWindowDelayMs: 250
  $name: Tray window delay (ms)
  $name:zh-CN: 托盘弹出窗口：移动窗口的延迟（毫秒）
  $description: >-
    How long to wait for the window to finish laying itself out. Increase it if
    the window ends up in the wrong position
  $description:zh-CN: >-
    窗口出现后等待它完成布局的延迟。若窗口被移动后位置不对，可尝试调大
- newWindowDelayMs: 400
  $name: New program window delay (ms)
  $name:zh-CN: 新软件窗口：移动窗口的延迟（毫秒）
  $description: >-
    How long to wait for the window to finish laying itself out. Increase it if
    the window ends up in the wrong position
  $description:zh-CN: 窗口出现后等待它完成布局的时间。若窗口被移动后位置不对，可调大
- newAppMaxAgeMs: 15000
  $name: New program detection window (ms)
  $name:zh-CN: 新软件的判定时间（毫秒）
  $description: >-
    The first main window that appears within this long after the process
    started counts as "newly launched". Increase it for slow starting programs,
    decrease it to avoid false positives
  $description:zh-CN: >-
    进程启动后这么久之内出现的第一个主窗口，才算“新打开的软件”。程序启动很慢时
    可调大；调小可减少误判（例如常驻程序迟一点弹窗不会被移动）
- trayClickGraceMs: 1500
  $name: Tray / Start menu click grace period (ms)
  $name:zh-CN: 托盘 / 开始菜单点击的有效期（毫秒）
  $description: >-
    A program may raise its window some time after the click. Only windows that
    appear within this window are moved. Set to 0 for no time limit: the last
    click position then keeps qualifying, wherever the cursor has moved to
  $description:zh-CN: >-
    点击托盘图标或开始菜单项后，程序可能立刻也可能稍后才弹出/激活窗口。这个
    时间窗内出现的窗口才会被移动；设为 0 表示只要鼠标停在该区域就一直生效
- settleDelayMs: 200
  $name: Post-move correction delay (ms)
  $name:zh-CN: 移动后的二次校正延迟（毫秒）
  $description: >-
    Some apps resize themselves after crossing monitors, so the position is
    corrected once after this delay. Set to 0 to turn it off
  $description:zh-CN: >-
    有些程序在跨屏后会自行调整窗口大小，插件会在该延迟后再校正一次位置，
    使窗口正确居中且不超出屏幕。设为 0 可关闭
- minWindowSize: 120
  $name: Minimum window size (px)
  $name:zh-CN: 最小窗口尺寸（像素）
  $description: >-
    Windows narrower or shorter than this are left alone, so small tooltips and
    bubbles aren't moved by mistake
  $description:zh-CN: 宽或高小于该值的窗口不会被处理，避免误移动小提示气泡
- excludedProcesses: "SearchApp.exe,SearchHost.exe,StartMenuExperienceHost.exe,ShellExperienceHost.exe,ShellHost.exe,SystemSettings.exe,TextInputHost.exe,ctfmon.exe"
  $name: Excluded processes
  $name:zh-CN: 排除的进程
  $description: >-
    Comma separated process file names. Windows created by these processes are
    never moved
  $description:zh-CN: 逗号分隔的进程文件名，这些进程弹出的窗口不会被移动
- excludedClasses: "#32768,tooltips_class32,ToolTip,NotifyIconOverflowWindow,Windows.UI.Core.CoreWindow"
  $name: Excluded window classes
  $name:zh-CN: 排除的窗口类名
  $description: >-
    Comma separated window class names that are never moved (#32768 is the
    context menu). Matching is case insensitive
  $description:zh-CN: >-
    逗号分隔的窗口类名，这些窗口不会被移动（#32768 为右键菜单），不区分大小写
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <psapi.h>
#include <shellscalingapi.h>

#include <atomic>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// DWM is used to suppress the maximize/restore animation while a maximized
// window travels to another monitor.
void SetWindowTransitionsDisabled(HWND hWnd, bool disabled) {
    BOOL value = disabled ? TRUE : FALSE;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &value,
                          sizeof(value));
}

// Windows keeps cloaked windows "visible" for the window manager while hiding
// them from the user - the Start menu is cloaked whenever it's closed.
bool IsWindowCloaked(HWND hWnd) {
    int cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

// ============================================================================
// Settings
// ============================================================================

// How the window size is adapted when it travels to a monitor with a different
// resolution / scaling.
enum class SizeMode {
    // Don't touch the size at all - only move the window, exactly like dragging
    // it with the mouse. Windows then sends WM_DPICHANGED to the app (or bitmap
    // scales it), and the app resizes itself once. This is the only mode with no
    // visible "zoom" of our own: the whole trip is a single position change
    // plus the app's own DPI reaction.
    Auto,
    // Keep the window's share of the screen (best "looks the same" result when
    // the two monitors differ in both resolution and scaling).
    Relative,
    // Keep the same logical (DPI-independent) pixel size.
    Dpi,
    // Don't touch the size at all, and don't compensate for anything either.
    None,
};

struct Settings {
    bool taskbarClickEnabled = true;
    bool trayWindowEnabled = true;
    bool newWindowEnabled = true;
    bool startMenuEnabled = true;
    bool moveActiveInsteadOfMinimize = true;
    SizeMode sizeMode = SizeMode::Auto;
    bool clampToWorkArea = true;
    bool centerOnMonitor = true;
    bool trayAreaOnlyRight = true;
    int taskbarClickDelayMs = 100;
    int trayWindowDelayMs = 250;
    int newWindowDelayMs = 400;
    int newAppMaxAgeMs = 15000;
    int trayClickGraceMs = 1500;
    int settleDelayMs = 200;
    int minWindowSize = 120;
    std::vector<std::wstring> excludedProcesses;
    std::vector<std::wstring> excludedClasses;
};

Settings g_settings;

std::vector<std::wstring> ParseList(PCWSTR str) {
    std::vector<std::wstring> result;

    if (!str || !*str) {
        return result;
    }

    std::wstring_view view(str);
    size_t start = 0;

    for (;;) {
        size_t pos = view.find(L',', start);
        std::wstring_view token = pos == std::wstring_view::npos
                                      ? view.substr(start)
                                      : view.substr(start, pos - start);

        while (!token.empty() && (token.front() == L' ' || token.front() == L'\t')) {
            token.remove_prefix(1);
        }
        while (!token.empty() && (token.back() == L' ' || token.back() == L'\t')) {
            token.remove_suffix(1);
        }

        if (!token.empty()) {
            result.push_back(std::wstring(token));
        }

        if (pos == std::wstring_view::npos) {
            break;
        }

        start = pos + 1;
    }

    return result;
}

void LoadSettings() {
    g_settings.taskbarClickEnabled =
        Wh_GetIntSetting(L"taskbarClickEnabled") != 0;
    g_settings.trayWindowEnabled = Wh_GetIntSetting(L"trayWindowEnabled") != 0;
    g_settings.newWindowEnabled = Wh_GetIntSetting(L"newWindowEnabled") != 0;
    g_settings.startMenuEnabled =
        Wh_GetIntSetting(L"startMenuClickEnabled") != 0;

    PCWSTR activeWindowClick = Wh_GetStringSetting(L"activeWindowClick");
    g_settings.moveActiveInsteadOfMinimize =
        wcscmp(activeWindowClick, L"minimize") != 0;
    Wh_FreeStringSetting(activeWindowClick);

    PCWSTR sizeMode = Wh_GetStringSetting(L"windowSizeMode");
    if (wcscmp(sizeMode, L"relative") == 0) {
        g_settings.sizeMode = SizeMode::Relative;
    } else if (wcscmp(sizeMode, L"dpi") == 0) {
        g_settings.sizeMode = SizeMode::Dpi;
    } else if (wcscmp(sizeMode, L"none") == 0) {
        g_settings.sizeMode = SizeMode::None;
    } else {
        g_settings.sizeMode = SizeMode::Auto;
    }
    Wh_FreeStringSetting(sizeMode);

    g_settings.clampToWorkArea = Wh_GetIntSetting(L"clampToWorkArea") != 0;

    PCWSTR placement = Wh_GetStringSetting(L"placement");
    g_settings.centerOnMonitor = wcscmp(placement, L"keepOffset") != 0;
    Wh_FreeStringSetting(placement);

    PCWSTR trayArea = Wh_GetStringSetting(L"trayArea");
    g_settings.trayAreaOnlyRight = wcscmp(trayArea, L"full") != 0;
    Wh_FreeStringSetting(trayArea);

    g_settings.taskbarClickDelayMs = Wh_GetIntSetting(L"taskbarClickDelayMs");
    if (g_settings.taskbarClickDelayMs < 0 || g_settings.taskbarClickDelayMs > 5000) {
        g_settings.taskbarClickDelayMs = 100;
    }

    g_settings.trayWindowDelayMs = Wh_GetIntSetting(L"trayWindowDelayMs");
    if (g_settings.trayWindowDelayMs < 0 || g_settings.trayWindowDelayMs > 5000) {
        g_settings.trayWindowDelayMs = 250;
    }

    g_settings.newWindowDelayMs = Wh_GetIntSetting(L"newWindowDelayMs");
    if (g_settings.newWindowDelayMs < 0 || g_settings.newWindowDelayMs > 5000) {
        g_settings.newWindowDelayMs = 400;
    }

    g_settings.newAppMaxAgeMs = Wh_GetIntSetting(L"newAppMaxAgeMs");
    if (g_settings.newAppMaxAgeMs < 0 || g_settings.newAppMaxAgeMs > 120000) {
        g_settings.newAppMaxAgeMs = 15000;
    }

    g_settings.trayClickGraceMs = Wh_GetIntSetting(L"trayClickGraceMs");
    if (g_settings.trayClickGraceMs < 0 || g_settings.trayClickGraceMs > 10000) {
        g_settings.trayClickGraceMs = 1500;
    }

    g_settings.settleDelayMs = Wh_GetIntSetting(L"settleDelayMs");
    if (g_settings.settleDelayMs < 0 || g_settings.settleDelayMs > 5000) {
        g_settings.settleDelayMs = 200;
    }

    g_settings.minWindowSize = Wh_GetIntSetting(L"minWindowSize");
    if (g_settings.minWindowSize < 0) {
        g_settings.minWindowSize = 0;
    }

    PCWSTR processes = Wh_GetStringSetting(L"excludedProcesses");
    g_settings.excludedProcesses = ParseList(processes);
    Wh_FreeStringSetting(processes);

    PCWSTR classes = Wh_GetStringSetting(L"excludedClasses");
    g_settings.excludedClasses = ParseList(classes);
    Wh_FreeStringSetting(classes);
}

// ============================================================================
// Helpers
// ============================================================================

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u", major, build);

    if (major == 10) {
        return build >= 22000 ? WinVersion::Win11 : WinVersion::Win10;
    }

    return WinVersion::Unsupported;
}

std::wstring GetWindowClass(HWND hWnd) {
    wchar_t className[256] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return L"";
    }
    return std::wstring(className);
}

std::wstring GetProcessFileName(HWND hWnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId) {
        return L"";
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return L"";
    }

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    BOOL result = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);

    if (!result) {
        return L"";
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    return std::wstring(fileName ? fileName + 1 : path);
}

bool IsExcludedWindow(HWND hWnd) {
    std::wstring className = GetWindowClass(hWnd);
    for (const auto& excluded : g_settings.excludedClasses) {
        if (_wcsicmp(className.c_str(), excluded.c_str()) == 0) {
            return true;
        }
    }

    std::wstring processName = GetProcessFileName(hWnd);
    for (const auto& excluded : g_settings.excludedProcesses) {
        if (_wcsicmp(processName.c_str(), excluded.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

// ============================================================================
// Monitor / DPI helpers
// ============================================================================

UINT GetMonitorDpi(HMONITOR hMonitor) {
    UINT dpiX = 96;
    UINT dpiY = 96;

    if (SUCCEEDED(
            GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX >= 96) {
        return dpiX;
    }

    return 96;
}

// True for windows whose app handles DPI changes itself. Windows sends those
// WM_DPICHANGED when they cross a monitor boundary, and they resize themselves
// by the DPI ratio - which is the size we can aim at while the window is still
// on its way over.
bool IsPerMonitorDpiAwareWindow(HWND hWnd) {
    DPI_AWARENESS_CONTEXT context = GetWindowDpiAwarenessContext(hWnd);
    if (!context) {
        return false;
    }

    // Covers both per-monitor awareness v1 and v2.
    return GetAwarenessFromDpiAwarenessContext(context) ==
           DPI_AWARENESS_PER_MONITOR_AWARE;
}

// True when the given point lies on one of this process' taskbars, optionally
// only on the half where the tray lives (right for a horizontal taskbar,
// bottom for a vertical one).
bool IsPointOnTrayArea(POINT pt) {
    struct EnumContext {
        DWORD processId;
        POINT pt;
        bool result;
    } context{GetCurrentProcessId(), pt, false};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& ctx = *reinterpret_cast<EnumContext*>(lParam);

            DWORD processId = 0;
            if (!GetWindowThreadProcessId(hWnd, &processId) ||
                processId != ctx.processId) {
                return TRUE;
            }

            if (!IsWindowVisible(hWnd)) {
                return TRUE;
            }

            wchar_t className[64] = {};
            if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }

            RECT rect = {};
            if (!GetWindowRect(hWnd, &rect)) {
                return TRUE;
            }

            if (ctx.pt.x < rect.left || ctx.pt.x > rect.right ||
                ctx.pt.y < rect.top || ctx.pt.y > rect.bottom) {
                return TRUE;
            }

            if (!g_settings.trayAreaOnlyRight) {
                ctx.result = true;
                return FALSE;
            }

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Horizontal taskbar: the tray sits on the right half.
            // Vertical taskbar: it sits on the bottom half.
            bool onTrayHalf =
                width >= height
                    ? (width > 0 && ctx.pt.x >= rect.left + width / 2)
                    : (height > 0 && ctx.pt.y >= rect.top + height / 2);

            if (onTrayHalf) {
                ctx.result = true;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));

    return context.result;
}


// ============================================================================
// Delayed window moving (performed on the mod's worker thread)
// ============================================================================

struct PendingMove {
    HWND hWnd;
    HMONITOR hMonitor;
    bool requireForeground;
    bool adjustOnly;
    int desiredWidth;
    int desiredHeight;
    DWORD dueTime;
};

CRITICAL_SECTION g_pendingLock;
std::vector<PendingMove> g_pendingMoves;
bool g_pendingInitialized = false;

HANDLE g_stopEvent = nullptr;
// Auto-reset event used to wake the worker thread when a move is queued, so it
// can sleep until the next move is due instead of polling.
HANDLE g_wakeEvent = nullptr;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;

// Posted to the worker thread when the settings change: it reads them while
// moving windows, so reloading them there avoids replacing the setting values
// under its feet.
constexpr UINT kReloadSettingsMessage = WM_APP;

// Where and when the last mouse click happened, and what was under the cursor
// at that moment. The taskbar click hook clears the timestamp to say "this click
// was a taskbar button, not a tray icon or a Start menu entry".
std::atomic<DWORD> g_lastMouseDownTick{0};
std::atomic<LONG> g_lastMouseDownX{0};
std::atomic<LONG> g_lastMouseDownY{0};
std::atomic<HWND> g_lastMouseDownHwnd{nullptr};

void QueuePendingMove(HWND hWnd, HMONITOR hMonitor, int delayMs,
                      bool requireForeground, bool adjustOnly,
                      int desiredWidth, int desiredHeight);

int ClampInt(int value, int lo, int hi) {
    if (lo > hi) {
        return lo;
    }
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

// Moves (and, when needed, rescales) a window onto the target monitor.
//
// The tricky part is mixed-DPI setups: a window that was last sized on a 4K
// monitor at 150% is physically 1.5x bigger than the same window on a 1080p
// monitor at 100%. Naively moving it keeps the physical size, so it ends up
// oversized and, once centered, partly off screen. Scaling the size by the
// ratio of the two monitors' DPI keeps it looking the same on both.
//
// adjustOnly performs a second, lighter pass: no un-maximizing, no recomputing
// of the size - it just re-applies the already decided size and position, to
// undo whatever the app did when reacting to WM_DPICHANGED.
void PerformMove(HWND hWnd, HMONITOR hTargetMonitor, bool requireForeground,
                 bool adjustOnly, int desiredWidth = 0, int desiredHeight = 0) {
    if (!IsWindow(hWnd)) {
        return;
    }

    if (requireForeground) {
        // Only relocate windows that were actually activated by the click. This
        // filters out clicks that showed thumbnails or minimized the window.
        if (GetForegroundWindow() != hWnd || IsIconic(hWnd)) {
            return;
        }
    } else {
        if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
            return;
        }
    }

    MONITORINFO targetInfo = {sizeof(targetInfo)};
    if (!GetMonitorInfoW(hTargetMonitor, &targetInfo)) {
        return;
    }

    HMONITOR hCurrentMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // The first pass has nothing to do if the window is already there; the
    // correction pass has nothing to do if the window is somewhere else.
    if (adjustOnly ? hCurrentMonitor != hTargetMonitor
                   : hCurrentMonitor == hTargetMonitor) {
        return;
    }

    bool wasMaximized = false;
    if (adjustOnly) {
        // A maximized window already fills the target monitor - touching it
        // here would only break its maximized state.
        WINDOWPLACEMENT wp = {sizeof(wp)};
        if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
            return;
        }
    } else {
        WINDOWPLACEMENT wp = {sizeof(wp)};
        if (GetWindowPlacement(hWnd, &wp)) {
            wasMaximized = wp.showCmd == SW_SHOWMAXIMIZED;
        }
        if (wasMaximized) {
            // Keep the restore / re-maximize pair free of the zoom animation.
            SetWindowTransitionsDisabled(hWnd, true);
            ShowWindow(hWnd, SW_RESTORE);
        }
    }

    // Read the rect only after restoring - the restored size differs from the
    // maximized one.
    RECT rc = {};
    if (!GetWindowRect(hWnd, &rc)) {
        if (wasMaximized) {
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetWindowTransitionsDisabled(hWnd, false);
        }
        return;
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width < g_settings.minWindowSize || height < g_settings.minWindowSize) {
        if (wasMaximized) {
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetWindowTransitionsDisabled(hWnd, false);
        }
        return;
    }

    int workWidth = targetInfo.rcWork.right - targetInfo.rcWork.left;
    int workHeight = targetInfo.rcWork.bottom - targetInfo.rcWork.top;

    MONITORINFO sourceInfo = {sizeof(sourceInfo)};
    bool hasSourceInfo = GetMonitorInfoW(hCurrentMonitor, &sourceInfo) != FALSE;

    // In auto mode the window is only relocated, never resized. Windows then
    // sends the app WM_DPICHANGED (or bitmap-scales it) and the app adapts its
    // own size - the very same thing that happens when the window is dragged
    // over by hand. Resizing it here would fight with that and show up as a
    // visible zoom-in / zoom-out right after the jump.
    bool moveOnly = g_settings.sizeMode == SizeMode::Auto;

    int newWidth = width;
    int newHeight = height;

    if (adjustOnly) {
        if (!moveOnly && desiredWidth > 0 && desiredHeight > 0) {
            newWidth = desiredWidth;
            newHeight = desiredHeight;
        }
    } else if (!moveOnly && g_settings.sizeMode != SizeMode::None &&
               (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_THICKFRAME) != 0) {
        // Only resize windows the user can resize anyway.
        int scaledWidth = 0;
        int scaledHeight = 0;

        if (g_settings.sizeMode == SizeMode::Dpi) {
            UINT sourceDpi = GetMonitorDpi(hCurrentMonitor);
            UINT targetDpi = GetMonitorDpi(hTargetMonitor);

            if (sourceDpi > 0 && sourceDpi != targetDpi) {
                scaledWidth = MulDiv(width, (int)targetDpi, (int)sourceDpi);
                scaledHeight = MulDiv(height, (int)targetDpi, (int)sourceDpi);
            }
        } else if (hasSourceInfo) {
            // Keep the window's share of the work area, so it takes up the
            // same part of the screen on both monitors.
            int sourceWorkWidth =
                sourceInfo.rcWork.right - sourceInfo.rcWork.left;
            int sourceWorkHeight =
                sourceInfo.rcWork.bottom - sourceInfo.rcWork.top;

            if (sourceWorkWidth > 0 && sourceWorkHeight > 0) {
                scaledWidth = MulDiv(width, workWidth, sourceWorkWidth);
                scaledHeight = MulDiv(height, workHeight, sourceWorkHeight);
            }
        }

        if (scaledWidth > 0 && scaledHeight > 0) {
            newWidth = scaledWidth;
            newHeight = scaledHeight;
        }
    }

    // Only the modes that rescale on purpose give the window a new size; in the
    // move-only modes ("auto" and "none") the size is left entirely alone, down
    // to a window that is bigger than the target monitor.
    bool rescaling = g_settings.sizeMode == SizeMode::Relative ||
                     g_settings.sizeMode == SizeMode::Dpi;

    if (rescaling) {
        // Never let rescaling shrink a window into oblivion.
        if (newWidth < g_settings.minWindowSize) {
            newWidth = g_settings.minWindowSize;
        }
        if (newHeight < g_settings.minWindowSize) {
            newHeight = g_settings.minWindowSize;
        }

        if (newWidth > workWidth) {
            newWidth = workWidth;
        }
        if (newHeight > workHeight) {
            newHeight = workHeight;
        }
    }

    // The size the window will have once everything has settled - this is what
    // the position is computed from. In move-only mode that's the size the app
    // is about to give itself when it reacts to WM_DPICHANGED (it scales the
    // window by the ratio of the two monitors' DPI). Aiming at it places the
    // window where it belongs right away, so no follow-up correction is needed.
    int layoutWidth = newWidth;
    int layoutHeight = newHeight;

    if (moveOnly && !adjustOnly && IsPerMonitorDpiAwareWindow(hWnd)) {
        UINT sourceDpi = GetMonitorDpi(hCurrentMonitor);
        UINT targetDpi = GetMonitorDpi(hTargetMonitor);

        if (sourceDpi > 0 && targetDpi > 0 && sourceDpi != targetDpi) {
            int predictedWidth = MulDiv(width, (int)targetDpi, (int)sourceDpi);
            int predictedHeight = MulDiv(height, (int)targetDpi, (int)sourceDpi);

            if (predictedWidth > 0 && predictedHeight > 0) {
                layoutWidth = predictedWidth;
                layoutHeight = predictedHeight;
            }
        }
    }

    int x = 0;
    int y = 0;

    if (g_settings.centerOnMonitor) {
        x = targetInfo.rcWork.left + (workWidth - layoutWidth) / 2;
        y = targetInfo.rcWork.top + (workHeight - layoutHeight) / 2;
    } else if (adjustOnly) {
        // The window is already where we put it - just keep it in bounds.
        x = rc.left;
        y = rc.top;
    } else if (hasSourceInfo) {
        // Keep the window's centre at the same relative spot inside the work
        // area. Mapping the centre rather than the top-left corner is what makes
        // the window land where it looks like it belongs: the two monitors (and
        // therefore the window's share of them) usually differ, and the app
        // resizes the window around its centre-ish anchor, not its corner.
        int sourceWorkWidth = sourceInfo.rcWork.right - sourceInfo.rcWork.left;
        int sourceWorkHeight = sourceInfo.rcWork.bottom - sourceInfo.rcWork.top;

        if (sourceWorkWidth > 0) {
            x = targetInfo.rcWork.left +
                MulDiv(2 * (rc.left - sourceInfo.rcWork.left) + width,
                       workWidth, 2 * sourceWorkWidth) -
                layoutWidth / 2;
        } else {
            x = targetInfo.rcWork.left + (workWidth - layoutWidth) / 2;
        }

        if (sourceWorkHeight > 0) {
            y = targetInfo.rcWork.top +
                MulDiv(2 * (rc.top - sourceInfo.rcWork.top) + height,
                       workHeight, 2 * sourceWorkHeight) -
                layoutHeight / 2;
        } else {
            y = targetInfo.rcWork.top + (workHeight - layoutHeight) / 2;
        }
    } else {
        x = targetInfo.rcWork.left + (workWidth - layoutWidth) / 2;
        y = targetInfo.rcWork.top + (workHeight - layoutHeight) / 2;
    }

    if (g_settings.clampToWorkArea) {
        // Make sure the window ends up fully inside the target monitor. A window
        // that doesn't fit is left centered instead of squeezed - in move-only
        // mode shrinking it is exactly what we're trying to avoid.
        if (layoutWidth <= workWidth) {
            x = ClampInt(x, targetInfo.rcWork.left,
                         targetInfo.rcWork.right - layoutWidth);
        }
        if (layoutHeight <= workHeight) {
            y = ClampInt(y, targetInfo.rcWork.top,
                         targetInfo.rcWork.bottom - layoutHeight);
        }
    }

    bool sizeUnchanged =
        moveOnly || (newWidth == width && newHeight == height);

    if (x == rc.left && y == rc.top && sizeUnchanged) {
        // Already where it should be - skip the call so the window isn't
        // touched (and possibly repainted / animated) for nothing.
        if (wasMaximized) {
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetWindowTransitionsDisabled(hWnd, false);
        }
        return;
    }

    Wh_Log(L"Moving window %08X (%dx%d) to %d,%d on the cursor's monitor",
           (DWORD)(ULONG_PTR)hWnd, newWidth, newHeight, x, y);

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (sizeUnchanged) {
        flags |= SWP_NOSIZE;
    }

    SetWindowPos(hWnd, nullptr, x, y, newWidth, newHeight, flags);

    if (wasMaximized) {
        ShowWindow(hWnd, SW_MAXIMIZE);
        SetWindowTransitionsDisabled(hWnd, false);
    }

    // Safety net: a few apps don't end up at the size we predicted (minimum
    // size limits, custom DPI handling). The correction pass re-centers using
    // whatever size the app settled on - it never overrides the size, and it
    // does nothing at all when the window is already where it should be, which
    // is the normal case now that the position is computed from the predicted
    // size. Set "settleDelayMs" to 0 to skip it entirely.
    if (!adjustOnly && g_settings.settleDelayMs > 0) {
        QueuePendingMove(hWnd, hTargetMonitor, g_settings.settleDelayMs, false,
                         true, moveOnly ? 0 : newWidth,
                         moveOnly ? 0 : newHeight);
    }
}

void QueuePendingMove(HWND hWnd, HMONITOR hMonitor, int delayMs,
                      bool requireForeground, bool adjustOnly = false,
                      int desiredWidth = 0, int desiredHeight = 0) {
    if (!g_pendingInitialized) {
        return;
    }

    PendingMove move{hWnd,        hMonitor,      requireForeground,
                     adjustOnly,  desiredWidth,  desiredHeight,
                     GetTickCount() + (DWORD)delayMs};

    EnterCriticalSection(&g_pendingLock);
    g_pendingMoves.push_back(move);
    LeaveCriticalSection(&g_pendingLock);

    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void ProcessDueMoves() {
    for (;;) {
        PendingMove move{};
        bool found = false;
        DWORD now = GetTickCount();

        EnterCriticalSection(&g_pendingLock);
        for (size_t i = 0; i < g_pendingMoves.size(); i++) {
            if ((long)(g_pendingMoves[i].dueTime - now) <= 0) {
                move = g_pendingMoves[i];
                g_pendingMoves.erase(g_pendingMoves.begin() + i);
                found = true;
                break;
            }
        }
        LeaveCriticalSection(&g_pendingLock);

        if (!found) {
            return;
        }

        PerformMove(move.hWnd, move.hMonitor, move.requireForeground,
                    move.adjustOnly, move.desiredWidth, move.desiredHeight);
    }
}

// How long the worker thread may sleep: until the earliest queued move is due,
// or forever when nothing is queued.
DWORD GetTimeUntilNextMove() {
    DWORD now = GetTickCount();
    DWORD timeout = INFINITE;

    EnterCriticalSection(&g_pendingLock);

    for (const auto& move : g_pendingMoves) {
        long remaining = (long)(move.dueTime - now);
        if (remaining <= 0) {
            timeout = 0;
            break;
        }

        if ((DWORD)remaining < timeout) {
            timeout = (DWORD)remaining;
        }
    }

    LeaveCriticalSection(&g_pendingLock);

    return timeout;
}

void ClearPendingMoves() {
    EnterCriticalSection(&g_pendingLock);
    g_pendingMoves.clear();
    LeaveCriticalSection(&g_pendingLock);
}

// ============================================================================
// Feature 1: taskbar button click
// ============================================================================

using CTaskListWnd__HandleClick_t = void(WINAPI*)(LPVOID pThis,
                                                  LPVOID,  // ITaskBtnGroup *
                                                  int,
                                                  int,  // eCLICKACTION
                                                  int,
                                                  int);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(LPVOID pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(LPVOID pThis, int);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(LPVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

void WINAPI CTaskListWnd__HandleClick_Hook(LPVOID pThis,
                                           LPVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> taskbar click, action %d", clickAction);

    // This click was on a taskbar button. Forget the recorded click, so the
    // tray / Start menu paths don't mistake it for one of theirs. The low-level
    // mouse hook records the mouse-down before the taskbar sees the click, so
    // clearing it here is always in time.
    g_lastMouseDownTick = 0;

    HWND pendingWnd = nullptr;
    bool suppressClick = false;
    bool requireForeground = true;

    // eCLICKACTION: 0 - click, 1/2 - Win+number, 3 - new instance (middle
    // click or Shift+click), 4 - Ctrl+click. Only a plain click restores a
    // window, so leave the others to Windows.
    if (g_settings.taskbarClickEnabled && taskBtnGroup && clickAction == 0) {
        int index = taskItemIndex;
        if (index < 0) {
            // Group types: 1 - single/uncombined, 2 - pinned,
            // 3 - combined (a click only shows thumbnails, skip it).
            int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
            if (groupType == 1 || groupType == 2) {
                index = 0;
            }
        }

        if (index >= 0) {
            void* taskItem =
                CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, index);
            if (taskItem) {
                if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
                    pendingWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);
                } else {
                    pendingWnd = CWindowTaskItem_GetWindow_Original(taskItem);
                }
            }
        }

        // Clicking the button of the active window minimizes it by default.
        // When the window is on a different monitor than the cursor, move it
        // over instead, so a single click is enough to bring it here.
        if (pendingWnd && GetForegroundWindow() == pendingWnd) {
            POINT pt = {};
            if (g_settings.moveActiveInsteadOfMinimize && GetCursorPos(&pt) &&
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST) !=
                    MonitorFromWindow(pendingWnd, MONITOR_DEFAULTTONEAREST)) {
                // Swallow the click: it would only minimize the window.
                suppressClick = true;
                requireForeground = false;
            } else {
                pendingWnd = nullptr;
            }
        }
    }

    if (!suppressClick) {
        CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                           clickAction, param4, param5);
    }

    if (pendingWnd) {
        POINT pt = {};
        if (GetCursorPos(&pt)) {
            HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            QueuePendingMove(pendingWnd, hMonitor,
                             g_settings.taskbarClickDelayMs,
                             requireForeground);
        }
    }
}

bool HookTaskbarSymbols() {
    // taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
    };

    if (g_winVersion == WinVersion::Win11) {
        HMODULE taskbarModule = LoadLibraryEx(L"taskbar.dll", nullptr,
                                              LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!taskbarModule) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }

        return HookSymbols(taskbarModule, symbolHooks, ARRAYSIZE(symbolHooks));
    }

    return HookSymbols(GetModuleHandle(nullptr), symbolHooks,
                       ARRAYSIZE(symbolHooks));
}

// ============================================================================
// Shell launch surfaces (Start menu / search panel)
// ============================================================================

// True when the click landed on the Start menu or the search panel - the two
// places a program is launched from by clicking. The window is the one that was
// under the cursor at click time, so it still identifies the click after the
// Start menu has closed (it's only cloaked by then, which is why the cloaked
// state has to be checked here rather than by looking at what's on top now).
bool IsShellLaunchWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    hWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hWnd || !IsWindowVisible(hWnd) || IsWindowCloaked(hWnd)) {
        return false;
    }

    std::wstring name = GetProcessFileName(hWnd);
    return _wcsicmp(name.c_str(), L"StartMenuExperienceHost.exe") == 0 ||
           _wcsicmp(name.c_str(), L"SearchHost.exe") == 0 ||
           _wcsicmp(name.c_str(), L"SearchApp.exe") == 0;
}

// True when the click landed on a taskbar button rather than on the tray. The
// taskbar click hook already rules those out; this is the fallback for the case
// where that hook couldn't be installed.
bool IsTaskListWindow(HWND hWnd) {
    for (HWND w = hWnd; w; w = GetParent(w)) {
        wchar_t className[64] = {};
        if (!GetClassNameW(w, className, ARRAYSIZE(className))) {
            break;
        }

        if (_wcsicmp(className, L"MSTaskListWClass") == 0) {
            return true;
        }
    }

    return false;
}

// ============================================================================
// Feature 3: main windows of freshly launched programs
// ============================================================================

// A newly started program opens its main window on the primary monitor. To
// recognize "just launched" we look at the process creation time: anything
// older than newAppMaxAgeMs is a program that has been running for a while and
// merely opened another window (which the user positioned deliberately).
bool GetProcessCreationAgeMs(DWORD processId, ULONGLONG* ageMs) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    FILETIME creationTime = {};
    FILETIME exitTime = {};
    FILETIME kernelTime = {};
    FILETIME userTime = {};
    BOOL ok = GetProcessTimes(process, &creationTime, &exitTime, &kernelTime,
                              &userTime);
    CloseHandle(process);

    if (!ok) {
        return false;
    }

    FILETIME now = {};
    GetSystemTimeAsFileTime(&now);

    ULARGE_INTEGER creation;
    creation.LowPart = creationTime.dwLowDateTime;
    creation.HighPart = creationTime.dwHighDateTime;

    ULARGE_INTEGER current;
    current.LowPart = now.dwLowDateTime;
    current.HighPart = now.dwHighDateTime;

    if (current.QuadPart <= creation.QuadPart) {
        *ageMs = 0;
        return true;
    }

    // FILETIME ticks are 100 nanoseconds.
    *ageMs = (current.QuadPart - creation.QuadPart) / 10000;
    return true;
}

// Remembers which processes already got their window moved, so that only the
// first window of a program is relocated (splash screens, secondary windows and
// the like stay where they are).
struct HandledProcess {
    DWORD processId;
    DWORD tick;
    int width;
    int height;
};

CRITICAL_SECTION g_handledLock;
std::vector<HandledProcess> g_handledProcesses;
bool g_handledInitialized = false;

// Decides whether this window should be moved, and remembers it.
//
// Normally only the first window of a program is moved. The exception is a
// splash screen: those are usually not tool windows, so they can slip through,
// and moving one instead of the real main window would be pointless. Whenever a
// clearly bigger window of the same process shows up shortly after, it takes
// over.
bool ShouldHandleProcessWindow(DWORD processId, int width, int height) {
    if (!g_handledInitialized) {
        return true;
    }

    DWORD now = GetTickCount();
    bool handled = false;
    bool result = true;
    long long area = (long long)width * height;

    EnterCriticalSection(&g_handledLock);

    for (size_t i = 0; i < g_handledProcesses.size();) {
        // Forget entries after a minute.
        if (now - g_handledProcesses[i].tick > 60000) {
            g_handledProcesses.erase(g_handledProcesses.begin() + i);
            continue;
        }

        if (g_handledProcesses[i].processId == processId) {
            handled = true;

            long long knownArea =
                (long long)g_handledProcesses[i].width *
                g_handledProcesses[i].height;

            if (area > knownArea + knownArea / 3) {
                g_handledProcesses[i].tick = now;
                g_handledProcesses[i].width = width;
                g_handledProcesses[i].height = height;
            } else {
                result = false;
            }
        }

        i++;
    }

    if (!handled) {
        g_handledProcesses.push_back(
            HandledProcess{processId, now, width, height});
    }

    LeaveCriticalSection(&g_handledLock);

    return result;
}

void ClearHandledProcesses() {
    if (!g_handledInitialized) {
        return;
    }

    EnterCriticalSection(&g_handledLock);
    g_handledProcesses.clear();
    LeaveCriticalSection(&g_handledLock);
}

// ============================================================================
// Feature 2: windows raised by a tray icon click
// ============================================================================

// Clicking a tray icon has two very different outcomes depending on the
// program: it either creates/shows a brand new window (EVENT_OBJECT_SHOW), or
// it simply activates the window that is already open (EVENT_SYSTEM_FOREGROUND
// only). Watching both is what makes already-open windows follow the cursor.
//
// In addition, a low-level mouse hook remembers where and when the last click
// happened, so a window is only moved when the click really landed on the tray
// area - that keeps ordinary window switching (Alt+Tab, clicking another
// window, ...) from being hijacked.
HWINEVENTHOOK g_windowShownHook = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
HHOOK g_mouseHook = nullptr;

HWND g_lastQueuedWnd = nullptr;
DWORD g_lastQueuedTick = 0;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
         wParam == WM_RBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)) {
        auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        if (info) {
            // WindowFromPoint is cheap and must happen now: a moment later the
            // clicked surface (Start menu, tray flyout, ...) may be gone.
            g_lastMouseDownHwnd = WindowFromPoint(info->pt);
            g_lastMouseDownX = info->pt.x;
            g_lastMouseDownY = info->pt.y;
            g_lastMouseDownTick = GetTickCount();
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Where the last mouse click landed, as far as this mod cares.
enum class ClickArea {
    None,
    Tray,
    ShellLaunch,  // Start menu / search panel
};

// Returns the area of the most recent click, provided it happened recently
// enough. The remembered click position is more reliable than the current
// cursor position - the cursor may already have moved by the time the window
// appears.
ClickArea GetRecentClickArea(POINT* clickPoint) {
    DWORD tick = g_lastMouseDownTick;
    if (!tick) {
        return ClickArea::None;
    }

    if (g_settings.trayClickGraceMs > 0 &&
        GetTickCount() - tick > (DWORD)g_settings.trayClickGraceMs) {
        return ClickArea::None;
    }

    POINT pt{g_lastMouseDownX.load(), g_lastMouseDownY.load()};
    HWND clickedWnd = g_lastMouseDownHwnd;

    ClickArea area = ClickArea::None;

    if (g_settings.trayWindowEnabled && !IsTaskListWindow(clickedWnd) &&
        IsPointOnTrayArea(pt)) {
        area = ClickArea::Tray;
    } else if (g_settings.startMenuEnabled && IsShellLaunchWindow(clickedWnd)) {
        area = ClickArea::ShellLaunch;
    }

    if (area != ClickArea::None && clickPoint) {
        *clickPoint = pt;
    }

    return area;
}

// Minimum size used to throw out menus, tooltips and other noise right away.
// The user configured minimum is applied later, after the delay, once the
// window had time to lay itself out (a window often appears small first and
// only then maximizes).
constexpr int kMinSizeForDetection = 64;

void QueueWindowMove(HWND hWnd, HMONITOR hMonitor, int delayMs, PCWSTR reason);

// isNewWindow: the window just appeared (EVENT_OBJECT_SHOW). When false the
// window already existed and was merely activated (EVENT_SYSTEM_FOREGROUND), so
// it can be moved almost immediately.
void HandleCandidateWindow(HWND hWnd, bool isNewWindow) {
    if (!hWnd) {
        return;
    }

    bool trayEnabled = g_settings.trayWindowEnabled;
    bool newWindowEnabled = g_settings.newWindowEnabled;

    if (!trayEnabled && !newWindowEnabled && !g_settings.startMenuEnabled) {
        return;
    }

    // Only top-level windows.
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return;
    }

    // Never touch Explorer's own windows (taskbar, overflow panel, ...).
    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId) || !processId ||
        processId == GetCurrentProcessId()) {
        return;
    }

    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
        return;
    }

    if (IsExcludedWindow(hWnd)) {
        Wh_Log(L"Skipping excluded window %08X", (DWORD)(ULONG_PTR)hWnd);
        return;
    }

    RECT rc = {};
    if (!GetWindowRect(hWnd, &rc)) {
        return;
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width < kMinSizeForDetection || height < kMinSizeForDetection) {
        return;
    }

    // A window raised by clicking a tray icon or a Start menu / search entry:
    // the click position decides which monitor it belongs to. This covers both
    // freshly launched programs and ones that were already running.
    POINT clickPoint = {};
    ClickArea area = GetRecentClickArea(&clickPoint);
    if (area != ClickArea::None) {
        int delay = isNewWindow ? (area == ClickArea::Tray
                                       ? g_settings.trayWindowDelayMs
                                       : g_settings.newWindowDelayMs)
                                : g_settings.taskbarClickDelayMs;

        QueueWindowMove(
            hWnd, MonitorFromPoint(clickPoint, MONITOR_DEFAULTTONEAREST), delay,
            area == ClickArea::Tray ? L"raised by a tray click"
                                    : L"launched from the Start menu");
        return;
    }

    // The main window of a program that was just launched.
    if (newWindowEnabled) {
        // Dialogs and helper windows have an owner - the program's main window
        // doesn't. Tool windows (WS_EX_TOOLWINDOW) are skipped as well.
        if (GetWindow(hWnd, GW_OWNER)) {
            return;
        }

        if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
            return;
        }

        ULONGLONG ageMs = 0;
        if (!GetProcessCreationAgeMs(processId, &ageMs) ||
            ageMs > (ULONGLONG)g_settings.newAppMaxAgeMs) {
            return;
        }

        // Only the first (or a clearly bigger) window of a program gets moved.
        if (!ShouldHandleProcessWindow(processId, width, height)) {
            return;
        }

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            return;
        }

        QueueWindowMove(hWnd, MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                        g_settings.newWindowDelayMs, L"just launched");
    }
}

void QueueWindowMove(HWND hWnd,
                     HMONITOR hMonitor,
                     int delayMs,
                     PCWSTR reason) {
    if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == hMonitor) {
        return;
    }

    // A shown window also becomes foreground - don't handle the same window
    // twice.
    if (g_lastQueuedWnd == hWnd && GetTickCount() - g_lastQueuedTick < 1000) {
        return;
    }
    g_lastQueuedWnd = hWnd;
    g_lastQueuedTick = GetTickCount();

    Wh_Log(L"Window %08X %s, queuing move", (DWORD)(ULONG_PTR)hWnd, reason);

    QueuePendingMove(hWnd, hMonitor, delayMs, false);
}

void CALLBACK WindowEventProc(HWINEVENTHOOK,
                              DWORD event,
                              HWND hWnd,
                              LONG idObject,
                              LONG idChild,
                              DWORD,
                              DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event != EVENT_OBJECT_SHOW && event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }

    HandleCandidateWindow(hWnd, event == EVENT_OBJECT_SHOW);
}

// The worker thread owns the accessibility event hook (out-of-context hooks are
// delivered to the registering thread's message loop) and performs the delayed
// window moves.
void UnhookTrayHooks();

DWORD WINAPI WorkerThreadProc(LPVOID) {
    if (g_settings.trayWindowEnabled || g_settings.newWindowEnabled ||
        g_settings.startMenuEnabled) {
        // Two separate hooks: a single SetWinEventHook call covers a whole
        // range of events, and these two are far apart.
        g_windowShownHook = SetWinEventHook(
            EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WindowEventProc, 0,
            0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

        // Only needed to catch already-open windows raised from the tray or
        // launched from the Start menu.
        g_foregroundHook = (g_settings.trayWindowEnabled ||
                            g_settings.startMenuEnabled)
                               ? SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
                                                 EVENT_SYSTEM_FOREGROUND,
                                                 nullptr, WindowEventProc, 0, 0,
                                                 WINEVENT_OUTOFCONTEXT |
                                                     WINEVENT_SKIPOWNPROCESS)
                               : nullptr;

        // Only needed to tell tray / Start menu clicks apart from ordinary
        // window switching.
        g_mouseHook = (g_settings.trayWindowEnabled ||
                       g_settings.startMenuEnabled)
                          ? SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                              nullptr, 0)
                          : nullptr;

        Wh_Log(L"Window hooks: show=%s foreground=%s mouse=%s",
               g_windowShownHook ? L"ok" : L"failed",
               g_foregroundHook ? L"ok" : L"skipped",
               g_mouseHook ? L"ok" : L"skipped");
    }

    MSG msg = {};
    bool stop = false;

    while (!stop) {
        ProcessDueMoves();

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                stop = true;
                break;
            }

            if (msg.message == kReloadSettingsMessage) {
                LoadSettings();
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (stop) {
            break;
        }

        // Sleep until the next move is due - no need to wake up in between.
        HANDLE handles[] = {g_stopEvent, g_wakeEvent};
        DWORD wait = MsgWaitForMultipleObjects(
            ARRAYSIZE(handles), handles, FALSE, GetTimeUntilNextMove(),
            QS_ALLINPUT);

        if (wait == WAIT_OBJECT_0) {
            break;
        }
    }

    // WinEvent hooks must be removed by the thread that installed them.
    UnhookTrayHooks();

    return 0;
}

bool StartWorkerThread() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return false;
    }

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_wakeEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return false;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return false;
    }

    return true;
}

void StopWorkerThread() {
    if (!g_workerThread) {
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        return;
    }

    SetEvent(g_stopEvent);
    PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);

    // Wait for real: the worker calls into other processes (SetWindowPos,
    // ShowWindow), which block until those processes answer. Returning before it
    // finished would leave a thread running in an unloaded DLL.
    WaitForSingleObject(g_workerThread, INFINITE);

    CloseHandle(g_workerThread);
    g_workerThread = nullptr;

    CloseHandle(g_wakeEvent);
    g_wakeEvent = nullptr;

    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

void UnhookTrayHooks() {
    if (g_windowShownHook) {
        UnhookWinEvent(g_windowShownHook);
        g_windowShownHook = nullptr;
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

// ============================================================================
// Mod entry points
// ============================================================================

// Windhawk loads the mod into every explorer.exe process, including the ones
// that only host folder windows ("Launch folder windows in a separate process").
// Only the process that hosts the taskbar may install the system-wide hooks -
// otherwise every instance reacts to the same events and moves the same window
// several times over.
bool IsFolderWindowsProcess() {
    std::wstring commandLine = GetCommandLineW();
    for (auto& c : commandLine) {
        c = towlower(c);
    }

    return commandLine.find(L"/factory") != std::wstring::npos ||
           commandLine.find(L"-embedding") != std::wstring::npos ||
           commandLine.find(L"/separate") != std::wstring::npos;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsFolderWindowsProcess()) {
        Wh_Log(L"Not the taskbar process, skipping");
        return FALSE;
    }

    LoadSettings();

    g_winVersion = GetWindowsVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    InitializeCriticalSection(&g_pendingLock);
    g_pendingInitialized = true;

    InitializeCriticalSection(&g_handledLock);
    g_handledInitialized = true;

    if (!StartWorkerThread()) {
        Wh_Log(L"Failed to start the worker thread");
    }

    // Installed whatever the setting is: the hook body checks it, and the hook
    // is also what tells taskbar clicks apart from tray clicks.
    if (!HookTaskbarSymbols()) {
        Wh_Log(L"Taskbar click symbols not found");
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    // The worker thread unhooks the WinEvent hooks on its way out, so stopping
    // it is all that's needed here.
    StopWorkerThread();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    StopWorkerThread();

    ClearPendingMoves();
    ClearHandledProcesses();

    if (g_pendingInitialized) {
        DeleteCriticalSection(&g_pendingLock);
        g_pendingInitialized = false;
    }

    if (g_handledInitialized) {
        DeleteCriticalSection(&g_handledLock);
        g_handledInitialized = false;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool taskbarClick = Wh_GetIntSetting(L"taskbarClickEnabled") != 0;
    bool trayWindow = Wh_GetIntSetting(L"trayWindowEnabled") != 0;
    bool newWindow = Wh_GetIntSetting(L"newWindowEnabled") != 0;
    bool startMenu = Wh_GetIntSetting(L"startMenuClickEnabled") != 0;

    // The hooks are installed at init only, so changing a feature toggle needs a
    // reload - and the reload re-reads every setting anyway.
    if (taskbarClick != g_settings.taskbarClickEnabled ||
        trayWindow != g_settings.trayWindowEnabled ||
        newWindow != g_settings.newWindowEnabled ||
        startMenu != g_settings.startMenuEnabled) {
        *bReload = TRUE;
        return TRUE;
    }

    // The worker thread reads the settings while it moves windows, so let it
    // reload them instead of replacing the values here.
    if (g_workerThreadId &&
        !PostThreadMessageW(g_workerThreadId, kReloadSettingsMessage, 0, 0)) {
        LoadSettings();
    }

    return TRUE;
}

// ==WindhawkMod==
// @id              win11-26h2-taskbar-position-auto-hide
// @name            Windows 11 26H2 Taskbar Position Auto-Hide (KB5120998)
// @description     Per-monitor edge auto-hide for Shell_TrayWnd and Shell_SecondaryTrayWnd on Left, Top, Right, and Bottom positions via DWMWA_CLOAK, SetWindowRgn, and SPI_SETWORKAREA hooks.
// @version         1.0.0
// @author          tbc06w
// @github          https://github.com/TheoreticallyBryChen
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lshell32 -luser32 -lcomctl32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 26H2 Taskbar Position Auto-Hide (KB5120998)

Per-monitor auto-hide for `Shell_TrayWnd` and `Shell_SecondaryTrayWnd` on
**Left, Top, Right, and Bottom** taskbar positions on Windows 11 (26H2 / KB5120998+).

## Implementation
- Subclasses `Shell_TrayWnd` and `Shell_SecondaryTrayWnd` on the taskbar UI thread (`SetWindowSubclassFromAnyThread`).
- Hides via `DWMWA_CLOAK` + empty `SetWindowRgn(0, 0, 0, 0)` instead of `SW_HIDE`, keeping the XAML host alive while passing mouse input through.
- Slides along the monitor edge using `SetWindowPos` (`HWND_TOPMOST`, `SWP_NOSENDCHANGING`) + monitor-clipped `SetWindowRgn` while swallowing `WM_WINDOWPOSCHANGING` / `WM_WINDOWPOSCHANGED`.
- Hooks `SystemParametersInfoW` and `SystemParametersInfoForDpi` (`SPI_SETWORKAREA`) to hold `rcWork == rcMonitor` so open/maximized windows do not resize on reveal.
- Ignores edge triggers while `VK_LBUTTON` or `VK_RBUTTON` is held; remains shown while Start, Search, Quick Settings, Jump Lists, thumbnails, or `#32768` menus are active.
- Uses public Win32/DWM APIs only; no `taskbar.dll` or `Taskbar.View.dll` symbol hooks.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animationDurationMs: 160
  $name: Slide duration (ms)
  $description: Ease-out slide duration in milliseconds (0 = instant).
- hoverShowDelayMs: 40
  $name: Hover show delay (ms)
  $description: Dwell time in milliseconds inside the 4 px screen-edge strip before reveal.
- hideDelayMs: 220
  $name: Hide delay (ms)
  $description: Delay in milliseconds before hiding after pointer leaves the taskbar and active flyouts.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

namespace {

enum class TaskbarEdge {
    Unknown = -1,
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
};

struct Settings {
    int animationDurationMs = 160;
    int hoverShowDelayMs = 40;
    int hideDelayMs = 220;
};

struct TaskbarState {
    HWND hwnd = nullptr;
    HMONITOR hMonitor = nullptr;
    TaskbarEdge edge = TaskbarEdge::Bottom;
    RECT monitorRect{};
    RECT normalRect{};
    RECT reservedWorkRect{};
    bool validGeometry = false;

    bool isHoverShown = false;
    bool isCloaked = false;
    bool isApplyingAnimPos = false;

    // 0.0f = hidden, 1.0f = visible
    float animProgress = 1.0f;
    float animStartProgress = 1.0f;
    float animTargetProgress = 1.0f;
    ULONGLONG animStartTick = 0;

    ULONGLONG hoverStartTick = 0;
    ULONGLONG leaveStartTick = 0;
};

struct AdjustWorkAreaCtx {
    HMONITOR targetMon;
    RECT oldWork;
    RECT newWork;
};

constexpr UINT_PTR kTickTimerId = 0x57484148;
constexpr UINT kIdleTickMs = 25;
constexpr UINT kAnimTickMs = USER_TIMER_MINIMUM; // 10 ms
constexpr ULONGLONG kSecondaryScanIntervalMs = 2000;
constexpr int kEdgeTriggerPixels = 4;
constexpr int kTaskbarKeepShownPaddingPx = 12;

const UINT g_msgInitOrRefresh = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Refresh_" WH_MOD_ID);
const UINT g_msgSettingsChanged = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Settings_" WH_MOD_ID);
const UINT g_msgUninitCleanup = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Cleanup_" WH_MOD_ID);

std::mutex g_settingsMutex;
Settings g_settings;

std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_reclaimWorkAreaActive{true};
std::atomic<HANDLE> g_waiterThread{nullptr};

// Accessed strictly on the taskbar UI thread
std::unordered_map<HWND, TaskbarState> g_taskbars;
std::unordered_map<DWORD, std::wstring> g_pidExeCache;
bool g_savedNativeAutoHide = false;
bool g_hadNativeAutoHide = false;
bool g_needsInitialWorkAreaSync = true;
bool g_usingFastTimer = false;
ULONGLONG g_lastSecondaryScanTick = 0;
ULONGLONG g_lastWorkAreaCheckTick = 0;

HWND g_cachedFgHwnd = nullptr;
bool g_cachedFgIsFlyout = false;
HMONITOR g_cachedFgMonitor = nullptr;

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t SystemParametersInfoW_Original = nullptr;

using SystemParametersInfoForDpi_t = BOOL(WINAPI*)(UINT, UINT, PVOID, UINT, UINT);
SystemParametersInfoForDpi_t SystemParametersInfoForDpi_Original = nullptr;

std::wstring ToLower(std::wstring s) {
    for (wchar_t& ch : s) {
        ch = static_cast<wchar_t>(std::towlower(ch));
    }
    return s;
}

void LoadSettings() {
    Settings s;
    s.animationDurationMs = std::clamp(Wh_GetIntSetting(L"animationDurationMs"), 0, 1000);
    s.hoverShowDelayMs = std::clamp(Wh_GetIntSetting(L"hoverShowDelayMs"), 0, 2000);
    s.hideDelayMs = std::clamp(Wh_GetIntSetting(L"hideDelayMs"), 0, 5000);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

Settings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

BOOL WINAPI SystemParametersInfoW_Hook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    if (uiAction == SPI_SETWORKAREA && g_reclaimWorkAreaActive.load(std::memory_order_acquire)) {
        if (!pvParam) {
            return TRUE;
        }
        const RECT* req = static_cast<const RECT*>(pvParam);
        HMONITOR hMon = MonitorFromRect(req, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (hMon && GetMonitorInfoW(hMon, &mi)) {
            if (EqualRect(&mi.rcWork, &mi.rcMonitor)) {
                return TRUE;
            }
            RECT fullMon = mi.rcMonitor;
            return SystemParametersInfoW_Original(
                uiAction, uiParam, &fullMon, fWinIni & ~(SPIF_SENDCHANGE | SPIF_SENDWININICHANGE));
        }
        return TRUE;
    }
    return SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);
}

BOOL WINAPI SystemParametersInfoForDpi_Hook(UINT uiAction,
                                            UINT uiParam,
                                            PVOID pvParam,
                                            UINT fWinIni,
                                            UINT dpi) {
    if (uiAction == SPI_SETWORKAREA && g_reclaimWorkAreaActive.load(std::memory_order_acquire)) {
        if (!pvParam) {
            return TRUE;
        }
        const RECT* req = static_cast<const RECT*>(pvParam);
        HMONITOR hMon = MonitorFromRect(req, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (hMon && GetMonitorInfoW(hMon, &mi)) {
            if (EqualRect(&mi.rcWork, &mi.rcMonitor)) {
                return TRUE;
            }
            RECT fullMon = mi.rcMonitor;
            return SystemParametersInfoForDpi_Original(
                uiAction, uiParam, &fullMon,
                fWinIni & ~(SPIF_SENDCHANGE | SPIF_SENDWININICHANGE), dpi);
        }
        return TRUE;
    }
    return SystemParametersInfoForDpi_Original(uiAction, uiParam, pvParam, fWinIni, dpi);
}

bool IsTaskbarClassBuf(const wchar_t* cls) {
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsWindowCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked != FALSE;
}

const std::wstring& GetProcessExeNameLowerCached(HWND hwnd) {
    static const std::wstring kEmpty;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return kEmpty;
    }

    auto it = g_pidExeCache.find(pid);
    if (it != g_pidExeCache.end()) {
        return it->second;
    }

    if (g_pidExeCache.size() > 512) {
        g_pidExeCache.clear();
    }

    std::wstring exeName;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        wchar_t path[MAX_PATH] = {};
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(hProc, 0, path, &size) && size > 0) {
            const wchar_t* slash = wcsrchr(path, L'\\');
            exeName = ToLower(slash ? (slash + 1) : path);
        }
        CloseHandle(hProc);
    }

    return g_pidExeCache.emplace(pid, std::move(exeName)).first->second;
}

bool IsShellFlyoutProcess(const std::wstring& exeLower) {
    return exeLower == L"startmenuexperiencehost.exe" ||
           exeLower == L"searchhost.exe" ||
           exeLower == L"searchapp.exe" ||
           exeLower == L"shellexperiencehost.exe" ||
           exeLower == L"widgets.exe";
}

bool IsShellPopupClassBuf(const wchar_t* cls) {
    return wcscmp(cls, L"#32768") == 0 ||
           wcscmp(cls, L"Xaml_WindowedPopupClass") == 0 ||
           wcscmp(cls, L"NotifyIconOverflowWindow") == 0 ||
           wcscmp(cls, L"TaskListThumbnailWnd") == 0 ||
           wcscmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
           wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

bool IsFlyoutWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0 && IsShellPopupClassBuf(cls)) {
        return true;
    }
    return IsShellFlyoutProcess(GetProcessExeNameLowerCached(hwnd));
}

bool IsNativeAutoHideEnabled() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    UINT_PTR state = SHAppBarMessage(ABM_GETSTATE, &abd);
    return (state & ABS_AUTOHIDE) != 0;
}

void SetNativeAutoHide(bool enable) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    UINT_PTR state = SHAppBarMessage(ABM_GETSTATE, &abd);
    LPARAM desired = enable ? static_cast<LPARAM>(state | ABS_AUTOHIDE)
                            : static_cast<LPARAM>(state & ~ABS_AUTOHIDE);
    if (static_cast<UINT_PTR>(desired) != state) {
        abd.lParam = desired;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
}

TaskbarEdge DetectEdgeFromRect(const RECT& tb, const RECT& mon) {
    long tbW = tb.right - tb.left;
    long tbH = tb.bottom - tb.top;
    long monW = mon.right - mon.left;
    long monH = mon.bottom - mon.top;
    if (tbW <= 0 || tbH <= 0 || monW <= 0 || monH <= 0) {
        return TaskbarEdge::Unknown;
    }

    if (tbW >= tbH) {
        long distTop = std::abs(tb.top - mon.top);
        long distBottom = std::abs(mon.bottom - tb.bottom);
        return (distTop <= distBottom) ? TaskbarEdge::Top : TaskbarEdge::Bottom;
    } else {
        long distLeft = std::abs(tb.left - mon.left);
        long distRight = std::abs(mon.right - tb.right);
        return (distLeft <= distRight) ? TaskbarEdge::Left : TaskbarEdge::Right;
    }
}

RECT ComputeReservedWorkArea(const RECT& mon, const RECT& tb, TaskbarEdge edge) {
    RECT work = mon;
    switch (edge) {
        case TaskbarEdge::Left:
            work.left = std::max(work.left, tb.right);
            break;
        case TaskbarEdge::Right:
            work.right = std::min(work.right, tb.left);
            break;
        case TaskbarEdge::Top:
            work.top = std::max(work.top, tb.bottom);
            break;
        case TaskbarEdge::Bottom:
            work.bottom = std::min(work.bottom, tb.top);
            break;
        default:
            break;
    }
    return work;
}

bool IsValidDockedTaskbarRect(const RECT& tb, const RECT& mon) {
    long tbW = tb.right - tb.left;
    long tbH = tb.bottom - tb.top;
    long monW = mon.right - mon.left;
    long monH = mon.bottom - mon.top;
    if (tbW <= 0 || tbH <= 0 || monW <= 0 || monH <= 0) {
        return false;
    }

    RECT inter{};
    if (!IntersectRect(&inter, &tb, &mon)) {
        return false;
    }

    bool horizontal = (tbW > monW / 2) && (tbH < monH / 2);
    bool vertical = (tbH > monH / 2) && (tbW < monW / 2);
    return horizontal || vertical;
}

bool RectMatchesApprox(const RECT& a, const RECT& b, long tol = 4) {
    return std::abs(a.left - b.left) <= tol &&
           std::abs(a.top - b.top) <= tol &&
           std::abs(a.right - b.right) <= tol &&
           std::abs(a.bottom - b.bottom) <= tol;
}

BOOL CALLBACK EnumAdjustWorkAreaWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* c = reinterpret_cast<AdjustWorkAreaCtx*>(lParam);
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsWindowCloaked(hwnd)) {
        return TRUE;
    }

    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (IsTaskbarClassBuf(cls) ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            return TRUE;
        }
    }

    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) != c->targetMon) {
        return TRUE;
    }

    bool isZoomed = IsZoomed(hwnd) != FALSE;

    RECT winRect{};
    RECT frameRect{};
    if (!GetWindowRect(hwnd, &winRect)) {
        return TRUE;
    }
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frameRect, sizeof(frameRect)))) {
        frameRect = winRect;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    bool isBorderlessOldWork =
        ((style & WS_CAPTION) != WS_CAPTION) &&
        RectMatchesApprox(frameRect, c->oldWork, 4);

    if (!isZoomed && !isBorderlessOldWork) {
        return TRUE;
    }

    DWORD_PTR res = 0;
    SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0,
                        SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, 30, &res);

    if (isZoomed && RectMatchesApprox(frameRect, c->newWork, 2)) {
        return TRUE;
    }

    long dLeft = winRect.left - frameRect.left;
    long dTop = winRect.top - frameRect.top;
    long dRight = winRect.right - frameRect.right;
    long dBottom = winRect.bottom - frameRect.bottom;

    long targetX = c->newWork.left + dLeft;
    long targetY = c->newWork.top + dTop;
    long targetW = (c->newWork.right + dRight) - targetX;
    long targetH = (c->newWork.bottom + dBottom) - targetY;

    if (targetW > 0 && targetH > 0) {
        SetWindowPos(hwnd, nullptr, targetX, targetY, targetW, targetH,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
    }
    return TRUE;
}

void ApplyMonitorWorkArea(TaskbarState& tb, bool reclaim, bool notifyApps) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(tb.hMonitor, &mi)) {
        return;
    }

    RECT desiredWork = reclaim ? mi.rcMonitor : tb.reservedWorkRect;
    if (desiredWork.right <= desiredWork.left || desiredWork.bottom <= desiredWork.top) {
        return;
    }

    if (!EqualRect(&mi.rcWork, &desiredWork)) {
        RECT oldWork = mi.rcWork;
        if (SystemParametersInfoW_Original(SPI_SETWORKAREA, 0, &desiredWork, 0)) {
            if (notifyApps) {
                AdjustWorkAreaCtx ctx{tb.hMonitor, oldWork, desiredWork};
                EnumWindows(EnumAdjustWorkAreaWindowsProc, reinterpret_cast<LPARAM>(&ctx));
            }
        }
    }
}

void SetTaskbarSlidePose(TaskbarState& tb, float progress) {
    if (!IsWindow(tb.hwnd) || !tb.validGeometry) {
        return;
    }

    progress = std::clamp(progress, 0.0f, 1.0f);
    tb.animProgress = progress;

    long tbW = tb.normalRect.right - tb.normalRect.left;
    long tbH = tb.normalRect.bottom - tb.normalRect.top;
    long thickness = (tb.edge == TaskbarEdge::Left || tb.edge == TaskbarEdge::Right) ? tbW : tbH;

    tb.isApplyingAnimPos = true;

    if (progress <= 0.001f) {
        if (!tb.isCloaked) {
            BOOL cloak = TRUE;
            DwmSetWindowAttribute(tb.hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
            tb.isCloaked = true;
        }
        SetWindowRgn(tb.hwnd, nullptr, FALSE);
        SetWindowPos(tb.hwnd, nullptr, tb.normalRect.left, tb.normalRect.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
        HRGN emptyRgn = CreateRectRgn(0, 0, 0, 0);
        if (emptyRgn && !SetWindowRgn(tb.hwnd, emptyRgn, FALSE)) {
            DeleteObject(emptyRgn);
        }
        tb.isApplyingAnimPos = false;
        return;
    }

    long offset = std::lround((1.0f - progress) * static_cast<float>(thickness));
    if (progress >= 0.999f) {
        offset = 0;
    }

    int x = tb.normalRect.left;
    int y = tb.normalRect.top;
    HRGN clipRgn = nullptr;

    if (offset > 0) {
        switch (tb.edge) {
            case TaskbarEdge::Left:
                x -= offset;
                clipRgn = CreateRectRgn(offset, 0, tbW, tbH);
                break;
            case TaskbarEdge::Right:
                x += offset;
                clipRgn = CreateRectRgn(0, 0, tbW - offset, tbH);
                break;
            case TaskbarEdge::Top:
                y -= offset;
                clipRgn = CreateRectRgn(0, offset, tbW, tbH);
                break;
            case TaskbarEdge::Bottom:
                y += offset;
                clipRgn = CreateRectRgn(0, 0, tbW, tbH - offset);
                break;
            default:
                break;
        }
    }

    if (!IsWindowVisible(tb.hwnd)) {
        ShowWindow(tb.hwnd, SW_SHOWNOACTIVATE);
    }

    SetWindowRgn(tb.hwnd, nullptr, FALSE);
    SetWindowPos(tb.hwnd, HWND_TOPMOST, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

    if (clipRgn) {
        if (!SetWindowRgn(tb.hwnd, clipRgn, TRUE)) {
            DeleteObject(clipRgn);
        }
    }

    if (tb.isCloaked) {
        BOOL cloak = FALSE;
        DwmSetWindowAttribute(tb.hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
        tb.isCloaked = false;
    }

    tb.isApplyingAnimPos = false;
}

void StartSlideAnimation(TaskbarState& tb, float target, int durationMs) {
    target = std::clamp(target, 0.0f, 1.0f);
    if (durationMs <= 0) {
        tb.animStartProgress = target;
        tb.animTargetProgress = target;
        SetTaskbarSlidePose(tb, target);
        return;
    }

    if (std::abs(tb.animTargetProgress - target) > 0.01f) {
        tb.animStartProgress = tb.animProgress;
        tb.animTargetProgress = target;
        tb.animStartTick = GetTickCount64() - kAnimTickMs;
    }
}

bool StepTaskbarAnimation(TaskbarState& tb, int durationMs) {
    if (std::abs(tb.animProgress - tb.animTargetProgress) <= 0.001f) {
        return false;
    }

    if (durationMs <= 0) {
        SetTaskbarSlidePose(tb, tb.animTargetProgress);
        return false;
    }

    ULONGLONG elapsed = GetTickCount64() - tb.animStartTick;
    float t = std::clamp(static_cast<float>(elapsed) / static_cast<float>(durationMs), 0.0f, 1.0f);

    float eased = 1.0f - std::pow(1.0f - t, 3.0f);
    float current = tb.animStartProgress + (tb.animTargetProgress - tb.animStartProgress) * eased;

    SetTaskbarSlidePose(tb, current);
    return t < 1.0f;
}

void UpdateTaskbarGeometry(TaskbarState& tb) {
    if (!IsWindow(tb.hwnd) || tb.isApplyingAnimPos) {
        return;
    }
    HMONITOR hMon = MonitorFromWindow(tb.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        return;
    }

    tb.hMonitor = hMon;
    tb.monitorRect = mi.rcMonitor;

    RECT curRect{};
    if (GetWindowRect(tb.hwnd, &curRect) && IsValidDockedTaskbarRect(curRect, mi.rcMonitor)) {
        TaskbarEdge detectedEdge = DetectEdgeFromRect(curRect, mi.rcMonitor);
        if (detectedEdge != TaskbarEdge::Unknown) {
            tb.normalRect = curRect;
            tb.edge = detectedEdge;
            tb.reservedWorkRect = ComputeReservedWorkArea(mi.rcMonitor, curRect, detectedEdge);
            tb.validGeometry = true;
        }
    }
}

RECT ComputeTriggerRect(const TaskbarState& tb) {
    RECT r = tb.monitorRect;
    switch (tb.edge) {
        case TaskbarEdge::Left:
            r.right = std::min(r.right, r.left + kEdgeTriggerPixels);
            break;
        case TaskbarEdge::Right:
            r.left = std::max(r.left, r.right - kEdgeTriggerPixels);
            break;
        case TaskbarEdge::Top:
            r.bottom = std::min(r.bottom, r.top + kEdgeTriggerPixels);
            break;
        case TaskbarEdge::Bottom:
            r.top = std::max(r.top, r.bottom - kEdgeTriggerPixels);
            break;
        default:
            SetRectEmpty(&r);
            break;
    }
    return r;
}

bool IsCursorInRectWithPadding(POINT pt, RECT rc, int pad) {
    rc.left -= pad;
    rc.top -= pad;
    rc.right += pad;
    rc.bottom += pad;
    return PtInRect(&rc, pt) != FALSE;
}

bool IsTaskbarOrFlyoutActive(const TaskbarState& tb,
                             POINT cursorPt,
                             HWND fg,
                             bool fgIsFlyout,
                             HMONITOR fgMonitor) {
    if (fg == tb.hwnd) {
        return true;
    }

    if (fg && fgIsFlyout &&
        (fgMonitor == tb.hMonitor || tb.hwnd == FindWindowW(L"Shell_TrayWnd", nullptr))) {
        return true;
    }

    if (tb.isHoverShown || tb.animProgress > 0.05f) {
        if (IsCursorInRectWithPadding(cursorPt, tb.normalRect, kTaskbarKeepShownPaddingPx)) {
            return true;
        }
        HWND underCursor = WindowFromPoint(cursorPt);
        if (underCursor) {
            HWND root = GetAncestor(underCursor, GA_ROOT);
            if (root == tb.hwnd || IsFlyoutWindow(root)) {
                return true;
            }
        }
    }

    return false;
}

void EnsureWorkAreaAndNativeState() {
    g_lastWorkAreaCheckTick = GetTickCount64();

    if (IsNativeAutoHideEnabled()) {
        SetNativeAutoHide(false);
    }

    g_reclaimWorkAreaActive.store(true, std::memory_order_release);

    for (auto& [hwnd, tb] : g_taskbars) {
        if (tb.validGeometry && IsWindow(hwnd)) {
            ApplyMonitorWorkArea(tb, true, g_needsInitialWorkAreaSync);
        }
    }
    g_needsInitialWorkAreaSync = false;
}

void EvaluateHoverAndAnimate(HWND primaryHwnd, const Settings& settings) {
    POINT cursorPt{};
    bool hasCursor = GetCursorPos(&cursorPt) != FALSE;
    ULONGLONG now = GetTickCount64();

    bool mouseDragging =
        ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) ||
        ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);

    HWND fg = GetForegroundWindow();
    if (fg != g_cachedFgHwnd) {
        g_cachedFgHwnd = fg;
        g_cachedFgIsFlyout = IsFlyoutWindow(fg);
        g_cachedFgMonitor = fg ? MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) : nullptr;
    }

    bool anyAnimating = false;

    for (auto& [hwnd, tb] : g_taskbars) {
        if (!tb.validGeometry || !IsWindow(hwnd)) {
            continue;
        }

        bool interactionActive =
            hasCursor && IsTaskbarOrFlyoutActive(tb, cursorPt, fg, g_cachedFgIsFlyout, g_cachedFgMonitor);

        RECT triggerRect = ComputeTriggerRect(tb);
        bool inTriggerZone = hasCursor && PtInRect(&triggerRect, cursorPt) != FALSE;

        if (mouseDragging && !tb.isHoverShown) {
            inTriggerZone = false;
        }

        if (!tb.isHoverShown) {
            if (interactionActive && !mouseDragging) {
                tb.hoverStartTick = 0;
                tb.leaveStartTick = 0;
                tb.isHoverShown = true;
                StartSlideAnimation(tb, 1.0f, settings.animationDurationMs);
            } else if (inTriggerZone) {
                if (tb.hoverStartTick == 0) {
                    tb.hoverStartTick = now;
                }
                if (now - tb.hoverStartTick >= static_cast<ULONGLONG>(settings.hoverShowDelayMs)) {
                    tb.hoverStartTick = 0;
                    tb.leaveStartTick = 0;
                    tb.isHoverShown = true;
                    StartSlideAnimation(tb, 1.0f, settings.animationDurationMs);
                }
            } else {
                tb.hoverStartTick = 0;
                StartSlideAnimation(tb, 0.0f, settings.animationDurationMs);
            }
        } else {
            if (!mouseDragging) {
                bool keepShown = inTriggerZone || interactionActive;
                if (keepShown) {
                    tb.leaveStartTick = 0;
                    StartSlideAnimation(tb, 1.0f, settings.animationDurationMs);
                } else {
                    if (tb.leaveStartTick == 0) {
                        tb.leaveStartTick = now;
                    }
                    if (now - tb.leaveStartTick >= static_cast<ULONGLONG>(settings.hideDelayMs)) {
                        tb.isHoverShown = false;
                        tb.leaveStartTick = 0;
                        StartSlideAnimation(tb, 0.0f, settings.animationDurationMs);
                    }
                }
            } else {
                tb.leaveStartTick = 0;
            }
        }

        if (StepTaskbarAnimation(tb, settings.animationDurationMs)) {
            anyAnimating = true;
        }
    }

    if (anyAnimating != g_usingFastTimer) {
        g_usingFastTimer = anyAnimating;
        SetTimer(primaryHwnd, kTickTimerId, anyAnimating ? kAnimTickMs : kIdleTickMs, nullptr);
    }
}

void ScanAndRegisterTaskbars();

LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    bool isPrimary = (dwRefData == 1);

    if (uMsg == g_msgInitOrRefresh) {
        TaskbarState& tb = g_taskbars[hWnd];
        tb.hwnd = hWnd;
        UpdateTaskbarGeometry(tb);
        if (isPrimary) {
            ScanAndRegisterTaskbars();
            g_usingFastTimer = false;
            SetTimer(hWnd, kTickTimerId, kIdleTickMs, nullptr);
            Settings settings = GetSettingsCopy();
            EnsureWorkAreaAndNativeState();
            EvaluateHoverAndAnimate(hWnd, settings);
        }
        return 0;
    }

    if (uMsg == g_msgSettingsChanged) {
        if (isPrimary) {
            LoadSettings();
            Settings settings = GetSettingsCopy();
            EvaluateHoverAndAnimate(hWnd, settings);
        }
        return 0;
    }

    if (uMsg == g_msgUninitCleanup) {
        if (isPrimary) {
            KillTimer(hWnd, kTickTimerId);
            g_reclaimWorkAreaActive.store(false, std::memory_order_release);
            for (auto& [tbHwnd, tb] : g_taskbars) {
                SetTaskbarSlidePose(tb, 1.0f);
                if (tb.validGeometry) {
                    ApplyMonitorWorkArea(tb, false, true);
                }
            }
            g_taskbars.clear();
            if (g_hadNativeAutoHide && g_savedNativeAutoHide) {
                SetNativeAutoHide(true);
            }
        }
        return 0;
    }

    switch (uMsg) {
        case WM_TIMER:
            if (isPrimary && wParam == kTickTimerId && !g_shuttingDown.load(std::memory_order_relaxed)) {
                Settings settings = GetSettingsCopy();
                ULONGLONG now = GetTickCount64();

                if (now - g_lastSecondaryScanTick >= kSecondaryScanIntervalMs) {
                    g_lastSecondaryScanTick = now;
                    ScanAndRegisterTaskbars();
                }

                if (now - g_lastWorkAreaCheckTick >= 1000) {
                    EnsureWorkAreaAndNativeState();
                }

                EvaluateHoverAndAnimate(hWnd, settings);
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGING: {
            auto it = g_taskbars.find(hWnd);
            if (it != g_taskbars.end() && it->second.isApplyingAnimPos) {
                return 0;
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            auto it = g_taskbars.find(hWnd);
            if (it != g_taskbars.end() && it->second.isApplyingAnimPos) {
                return 0;
            }
            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (it != g_taskbars.end() &&
                !it->second.isCloaked &&
                it->second.animProgress >= 0.999f) {
                UpdateTaskbarGeometry(it->second);
            }
            return res;
        }

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            if (isPrimary) {
                g_pidExeCache.clear();
                g_needsInitialWorkAreaSync = true;
                for (auto& [tbHwnd, tb] : g_taskbars) {
                    tb.isHoverShown = false;
                    SetTaskbarSlidePose(tb, 1.0f);
                    UpdateTaskbarGeometry(tb);
                }
                Settings settings = GetSettingsCopy();
                EnsureWorkAreaAndNativeState();
                EvaluateHoverAndAnimate(hWnd, settings);
            }
            break;

        case WM_SETTINGCHANGE:
            if (isPrimary && lParam &&
                _wcsicmp(reinterpret_cast<LPCWSTR>(lParam), L"TraySettings") == 0) {
                g_needsInitialWorkAreaSync = true;
                for (auto& [tbHwnd, tb] : g_taskbars) {
                    SetTaskbarSlidePose(tb, 1.0f);
                    UpdateTaskbarGeometry(tb);
                }
                Settings settings = GetSettingsCopy();
                EnsureWorkAreaAndNativeState();
                EvaluateHoverAndAnimate(hWnd, settings);
            }
            break;

        case WM_NCDESTROY:
            g_taskbars.erase(hWnd);
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumThreadTaskbarsProc(HWND hWnd, LPARAM lParam) {
    WCHAR cls[64] = {};
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 && IsTaskbarClassBuf(cls)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
    }
    return TRUE;
}

HWND FindCurrentProcessPrimaryTaskbar() {
    struct Ctx {
        DWORD pid;
        HWND found;
    } ctx{GetCurrentProcessId(), nullptr};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lParam);
            DWORD pid = 0;
            WCHAR cls[64] = {};
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == c->pid &&
                GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 &&
                _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
                c->found = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    return ctx.found;
}

void ScanAndRegisterTaskbars() {
    HWND primary = FindCurrentProcessPrimaryTaskbar();
    if (!primary) {
        return;
    }

    DWORD tid = GetWindowThreadProcessId(primary, nullptr);
    std::vector<HWND> threadTaskbars;
    EnumThreadWindows(tid, EnumThreadTaskbarsProc, reinterpret_cast<LPARAM>(&threadTaskbars));

    for (auto it = g_taskbars.begin(); it != g_taskbars.end();) {
        if (!IsWindow(it->first) ||
            std::find(threadTaskbars.begin(), threadTaskbars.end(), it->first) == threadTaskbars.end()) {
            it = g_taskbars.erase(it);
        } else {
            ++it;
        }
    }

    for (HWND hwnd : threadTaskbars) {
        if (g_taskbars.find(hwnd) == g_taskbars.end()) {
            TaskbarState& tb = g_taskbars[hwnd];
            tb.hwnd = hwnd;
            UpdateTaskbarGeometry(tb);
            DWORD_PTR refData = (hwnd == primary) ? 1 : 2;
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc, refData);
        }
    }
}

bool InitializeOnExistingTaskbar() {
    HWND primary = FindCurrentProcessPrimaryTaskbar();
    if (!primary) {
        return false;
    }

    if (!g_hadNativeAutoHide) {
        g_hadNativeAutoHide = true;
        g_savedNativeAutoHide = IsNativeAutoHideEnabled();
    }

    DWORD tid = GetWindowThreadProcessId(primary, nullptr);
    std::vector<HWND> threadTaskbars;
    EnumThreadWindows(tid, EnumThreadTaskbarsProc, reinterpret_cast<LPARAM>(&threadTaskbars));

    for (HWND hwnd : threadTaskbars) {
        if (hwnd != primary) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc, 2);
            PostMessageW(hwnd, g_msgInitOrRefresh, 0, 0);
        }
    }

    WindhawkUtils::SetWindowSubclassFromAnyThread(primary, TaskbarSubclassProc, 1);
    PostMessageW(primary, g_msgInitOrRefresh, 0, 0);
    return true;
}

DWORD WINAPI TaskbarWaiterThreadProc(LPVOID) {
    for (int i = 0; i < 150 && !g_shuttingDown.load(std::memory_order_relaxed); i++) {
        if (InitializeOnExistingTaskbar()) {
            break;
        }
        Sleep(100);
    }
    return 0;
}

} // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tray, &pid);
        if (pid != 0 && pid != GetCurrentProcessId()) {
            Wh_Log(L"Not the shell explorer process, skipping");
            return FALSE;
        }
    }

    LoadSettings();
    g_shuttingDown = false;
    g_reclaimWorkAreaActive.store(true, std::memory_order_release);
    g_needsInitialWorkAreaSync = true;
    g_usingFastTimer = false;
    g_cachedFgHwnd = nullptr;
    g_cachedFgIsFlyout = false;
    g_cachedFgMonitor = nullptr;

    Wh_SetFunctionHook((void*)SystemParametersInfoW,
                       (void*)SystemParametersInfoW_Hook,
                       (void**)&SystemParametersInfoW_Original);

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        if (void* pSpiDpi = reinterpret_cast<void*>(GetProcAddress(user32, "SystemParametersInfoForDpi"))) {
            Wh_SetFunctionHook(pSpiDpi,
                               (void*)SystemParametersInfoForDpi_Hook,
                               (void**)&SystemParametersInfoForDpi_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    if (!InitializeOnExistingTaskbar()) {
        HANDLE h = CreateThread(nullptr, 0, TaskbarWaiterThreadProc, nullptr, 0, nullptr);
        g_waiterThread.store(h, std::memory_order_release);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    if (HWND primary = FindCurrentProcessPrimaryTaskbar()) {
        PostMessageW(primary, g_msgSettingsChanged, 0, 0);
    } else {
        LoadSettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_shuttingDown = true;
    g_reclaimWorkAreaActive.store(false, std::memory_order_release);

    if (HANDLE h = g_waiterThread.exchange(nullptr, std::memory_order_acq_rel)) {
        WaitForSingleObject(h, 2000);
        CloseHandle(h);
    }

    HWND primary = FindCurrentProcessPrimaryTaskbar();
    if (primary) {
        DWORD_PTR res = 0;
        SendMessageTimeoutW(primary, g_msgUninitCleanup, 0, 0, SMTO_ABORTIFHUNG, 500, &res);

        DWORD tid = GetWindowThreadProcessId(primary, nullptr);
        std::vector<HWND> threadTaskbars;
        EnumThreadWindows(tid, EnumThreadTaskbarsProc, reinterpret_cast<LPARAM>(&threadTaskbars));
        for (HWND hwnd : threadTaskbars) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc);
        }
    }
}

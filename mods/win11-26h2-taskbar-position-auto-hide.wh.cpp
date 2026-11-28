// ==WindhawkMod==
// @id              win11-26h2-taskbar-position-auto-hide
// @name            Windows 11 26H2 Taskbar Auto-Hide (All Edges)
// @description     Auto-hides the Windows 11 taskbar on Left, Top, Right, and Bottom edges with a slide animation.
// @version         1.0.1
// @author          tbc06w
// @github          https://github.com/TheoreticallyBryChen
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lshell32 -luser32 -lcomctl32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 26H2 Taskbar Auto-Hide (All Edges)

Adds taskbar auto-hide with a slide-in/slide-out animation for **Left, Top,
Right, and Bottom** taskbar positions on Windows 11 (26H2+).

![Taskbar Auto-Hide Demo](https://i.imgur.com/REPLACE_WITH_YOUR_GIF.gif)

### Why this mod?
Windows 11 26H2 reintroduced moving the taskbar to the Left, Top, and Right
screen edges, but Windows' built-in *"Automatically hide the taskbar"* setting
only works at the bottom edge (and existing auto-hide mods rely on that native
behavior). This mod provides edge-hover auto-hide for all four edges without
shrinking or rescaling your open windows when the taskbar appears.

### Setup
1. Turn **off** Windows' built-in setting: **Settings → Personalization →
   Taskbar → Taskbar behaviors → Automatically hide the taskbar**.
   *(If Windows' built-in auto-hide is left on, this mod stays inactive so it
   never overwrites your Windows settings.)*
2. Enable this mod and move your taskbar to any edge (**Left**, **Top**,
   **Right**, or **Bottom**).
3. Hover the screen edge where your taskbar is placed to reveal it.

### Limitations
- **Drag and drop:** Hovering the hidden edge while holding a mouse button does
  not reveal the taskbar, so you cannot drag files onto hidden taskbar buttons.
- **Touch & flashing apps:** Touch edge swipes and flashing taskbar buttons do
  not reveal the hidden taskbar.
- **Shared monitor edges:** On multi-monitor setups, if the taskbar sits on an
  edge shared with another monitor, sweeping quickly across into the next
  monitor may skip the edge trigger; pause briefly at the edge to reveal it.
- **Exclusive fullscreen:** Does not pop up over exclusive Direct3D fullscreen
  games.
- **Other taskbar mods:** May conflict with mods that change the taskbar's
  window shape (rounded/floating taskbar mods) or hide individual taskbars
  (such as *Disable taskbar per monitor*).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animationDurationMs: 160
  $name: Slide animation duration (ms)
  $description: Duration of the slide-in and slide-out animation (0 = instant).
- hoverShowDelayMs: 40
  $name: Hover show delay (ms)
  $description: Time the cursor must stay at the screen edge before the taskbar appears.
- hideDelayMs: 220
  $name: Hide delay (ms)
  $description: Time to wait after the cursor leaves the taskbar and any open menus before hiding it again.
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
    bool isPrimary = false;
    TaskbarEdge edge = TaskbarEdge::Bottom;
    RECT monitorRect{};
    RECT normalRect{};
    RECT explorerWorkRect{};
    bool hasExplorerWorkRect = false;
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
constexpr UINT kIdleTickMs = 40;
constexpr UINT kAnimTickMs = USER_TIMER_MINIMUM; // 10 ms
constexpr ULONGLONG kWorkAreaVerifyIntervalMs = 1000;
constexpr int kBaseEdgeTriggerPx = 4;
constexpr int kBaseKeepShownPaddingPx = 12;

const UINT g_msgInitNewTaskbar = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Init_" WH_MOD_ID);
const UINT g_msgSettingsChanged = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Settings_" WH_MOD_ID);
const UINT g_msgUninitCleanup = RegisterWindowMessageW(L"Windhawk_TaskbarAutoHide_Cleanup_" WH_MOD_ID);

std::mutex g_settingsMutex;
Settings g_settings;

std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_reclaimWorkAreaActive{false};

// Accessed strictly on the taskbar UI thread
std::unordered_map<HWND, TaskbarState> g_taskbars;
std::unordered_map<DWORD, std::wstring> g_pidExeCache;
HWND g_primaryTaskbarHwnd = nullptr;
bool g_needsInitialWorkAreaSync = true;
bool g_usingFastTimer = false;
ULONGLONG g_lastWorkAreaCheckTick = 0;

HWND g_cachedFgHwnd = nullptr;
bool g_cachedFgIsFlyout = false;
HMONITOR g_cachedFgMonitor = nullptr;

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t SystemParametersInfoW_Original = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD,
                                           LPCWSTR,
                                           LPCWSTR,
                                           DWORD,
                                           int,
                                           int,
                                           int,
                                           int,
                                           HWND,
                                           HMENU,
                                           HINSTANCE,
                                           LPVOID,
                                           DWORD);
CreateWindowInBand_t CreateWindowInBand_Original = nullptr;

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

bool IsNativeAutoHideEnabled() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    UINT_PTR state = SHAppBarMessage(ABM_GETSTATE, &abd);
    return (state & ABS_AUTOHIDE) != 0;
}

TaskbarState* FindTaskbarForMonitor(HMONITOR hMon) {
    for (auto& [hwnd, tb] : g_taskbars) {
        if (tb.validGeometry && tb.hMonitor == hMon) {
            return &tb;
        }
    }
    return nullptr;
}

// Expands only the edge occupied by this monitor's taskbar so other registered
// app bars (docked toolbars/launchers) retain their reserved space.
RECT ComputeExpandedWorkArea(const TaskbarState& tb, const RECT& baseWork) {
    RECT expanded = baseWork;
    switch (tb.edge) {
        case TaskbarEdge::Left:
            expanded.left = tb.monitorRect.left;
            break;
        case TaskbarEdge::Right:
            expanded.right = tb.monitorRect.right;
            break;
        case TaskbarEdge::Top:
            expanded.top = tb.monitorRect.top;
            break;
        case TaskbarEdge::Bottom:
            expanded.bottom = tb.monitorRect.bottom;
            break;
        default:
            break;
    }
    return expanded;
}

RECT ComputeShrunkWorkAreaFromBase(const TaskbarState& tb, const RECT& currentWork) {
    RECT work = currentWork;
    switch (tb.edge) {
        case TaskbarEdge::Left:
            work.left = std::max(work.left, tb.normalRect.right);
            break;
        case TaskbarEdge::Right:
            work.right = std::min(work.right, tb.normalRect.left);
            break;
        case TaskbarEdge::Top:
            work.top = std::max(work.top, tb.normalRect.bottom);
            break;
        case TaskbarEdge::Bottom:
            work.bottom = std::min(work.bottom, tb.normalRect.top);
            break;
        default:
            break;
    }
    return work;
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
            // Only rewrite monitors that actually host a managed taskbar
            if (TaskbarState* tb = FindTaskbarForMonitor(hMon)) {
                tb->explorerWorkRect = *req;
                tb->hasExplorerWorkRect = true;

                RECT expanded = ComputeExpandedWorkArea(*tb, *req);
                if (EqualRect(&mi.rcWork, &expanded)) {
                    return TRUE;
                }
                return SystemParametersInfoW_Original(
                    uiAction, uiParam, &expanded,
                    fWinIni & ~(SPIF_SENDCHANGE | SPIF_SENDWININICHANGE));
            }
        }
    }
    return SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);
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

    if (!IsZoomed(hwnd)) {
        return TRUE;
    }

    RECT winRect{};
    RECT frameRect{};
    if (!GetWindowRect(hwnd, &winRect)) {
        return TRUE;
    }
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frameRect, sizeof(frameRect)))) {
        frameRect = winRect;
    }

    DWORD_PTR res = 0;
    SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0,
                        SMTO_ABORTIFHUNG, 30, &res);

    if (RectMatchesApprox(frameRect, c->newWork, 2)) {
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

    RECT desiredWork;
    if (reclaim) {
        if (!tb.hasExplorerWorkRect) {
            tb.explorerWorkRect = mi.rcWork;
            tb.hasExplorerWorkRect = true;
        }
        desiredWork = ComputeExpandedWorkArea(tb, tb.explorerWorkRect);
    } else {
        desiredWork = tb.hasExplorerWorkRect
                          ? tb.explorerWorkRect
                          : ComputeShrunkWorkAreaFromBase(tb, mi.rcWork);
    }

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

void ComputeSlidPoseRect(const TaskbarState& tb, float progress, int* outX, int* outY, HRGN* outClipRgn) {
    long tbW = tb.normalRect.right - tb.normalRect.left;
    long tbH = tb.normalRect.bottom - tb.normalRect.top;
    long thickness = (tb.edge == TaskbarEdge::Left || tb.edge == TaskbarEdge::Right) ? tbW : tbH;

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

    *outX = x;
    *outY = y;
    *outClipRgn = clipRgn;
}

void SetTaskbarSlidePose(TaskbarState& tb, float progress) {
    if (!IsWindow(tb.hwnd) || !tb.validGeometry) {
        return;
    }

    progress = std::clamp(progress, 0.0f, 1.0f);
    tb.animProgress = progress;
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

    int x = tb.normalRect.left;
    int y = tb.normalRect.top;
    HRGN clipRgn = nullptr;
    ComputeSlidPoseRect(tb, progress, &x, &y, &clipRgn);

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
        ULONGLONG now = GetTickCount64();
        tb.animStartTick = (now > kAnimTickMs) ? (now - kAnimTickMs) : 0;
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

void UpdateTaskbarGeometryFromRect(TaskbarState& tb, const RECT& rect) {
    if (!IsWindow(tb.hwnd)) {
        return;
    }
    HMONITOR hMon = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        return;
    }

    tb.hMonitor = hMon;
    tb.monitorRect = mi.rcMonitor;

    if (IsValidDockedTaskbarRect(rect, mi.rcMonitor)) {
        TaskbarEdge detectedEdge = DetectEdgeFromRect(rect, mi.rcMonitor);
        if (detectedEdge != TaskbarEdge::Unknown) {
            tb.normalRect = rect;
            tb.edge = detectedEdge;
            tb.validGeometry = true;
        }
    }
}

void UpdateTaskbarGeometry(TaskbarState& tb) {
    if (!IsWindow(tb.hwnd) || tb.isApplyingAnimPos) {
        return;
    }
    RECT curRect{};
    if (GetWindowRect(tb.hwnd, &curRect)) {
        UpdateTaskbarGeometryFromRect(tb, curRect);
    }
}

int ScaleByWindowDpi(HWND hwnd, int px96) {
    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi == 0) {
        dpi = 96;
    }
    return std::max(1, MulDiv(px96, static_cast<int>(dpi), 96));
}

RECT ComputeTriggerRect(const TaskbarState& tb) {
    RECT r = tb.monitorRect;
    long trig = ScaleByWindowDpi(tb.hwnd, kBaseEdgeTriggerPx);
    switch (tb.edge) {
        case TaskbarEdge::Left:
            r.right = std::min(r.right, r.left + trig);
            break;
        case TaskbarEdge::Right:
            r.left = std::max(r.left, r.right - trig);
            break;
        case TaskbarEdge::Top:
            r.bottom = std::min(r.bottom, r.top + trig);
            break;
        case TaskbarEdge::Bottom:
            r.top = std::max(r.top, r.bottom - trig);
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

    if (fg && fgIsFlyout && (fgMonitor == tb.hMonitor || tb.isPrimary)) {
        return true;
    }

    if (tb.isHoverShown || tb.animProgress > 0.05f) {
        int pad = ScaleByWindowDpi(tb.hwnd, kBaseKeepShownPaddingPx);
        if (IsCursorInRectWithPadding(cursorPt, tb.normalRect, pad)) {
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

bool IsExclusiveFullscreenActive() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    return SUCCEEDED(SHQueryUserNotificationState(&state)) &&
           state == QUNS_RUNNING_D3D_FULL_SCREEN;
}

void EnsureWorkAreaState(bool allowReclaim) {
    g_lastWorkAreaCheckTick = GetTickCount64();
    g_reclaimWorkAreaActive.store(allowReclaim, std::memory_order_release);

    for (auto& [hwnd, tb] : g_taskbars) {
        if (tb.validGeometry && IsWindow(hwnd)) {
            ApplyMonitorWorkArea(tb, allowReclaim, g_needsInitialWorkAreaSync);
        }
    }
    g_needsInitialWorkAreaSync = false;
}

void EvaluateHoverAndAnimate(HWND primaryHwnd, const Settings& settings) {
    // Stay completely passive when Windows' built-in auto-hide is enabled
    if (IsNativeAutoHideEnabled()) {
        if (g_reclaimWorkAreaActive.load(std::memory_order_relaxed)) {
            g_reclaimWorkAreaActive.store(false, std::memory_order_release);
            for (auto& [hwnd, tb] : g_taskbars) {
                tb.isHoverShown = false;
                SetTaskbarSlidePose(tb, 1.0f);
            }
        }
        return;
    }

    if (!g_reclaimWorkAreaActive.load(std::memory_order_relaxed)) {
        g_needsInitialWorkAreaSync = true;
        EnsureWorkAreaState(true);
    }

    POINT cursorPt{};
    bool hasCursor = GetCursorPos(&cursorPt) != FALSE;
    ULONGLONG now = GetTickCount64();

    bool mouseDragging =
        ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) ||
        ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);

    bool exclusiveFullscreen = IsExclusiveFullscreenActive();

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

        if ((mouseDragging && !tb.isHoverShown) || exclusiveFullscreen) {
            inTriggerZone = false;
        }

        if (!tb.isHoverShown) {
            if (interactionActive && !mouseDragging && !exclusiveFullscreen) {
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

LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    bool isPrimary = (dwRefData == 1);

    if (uMsg == g_msgInitNewTaskbar) {
        TaskbarState& tb = g_taskbars[hWnd];
        tb.hwnd = hWnd;
        tb.isPrimary = isPrimary;
        if (isPrimary) {
            g_primaryTaskbarHwnd = hWnd;
        }
        UpdateTaskbarGeometry(tb);

        if (isPrimary) {
            g_usingFastTimer = false;
            SetTimer(hWnd, kTickTimerId, kIdleTickMs, nullptr);
        }

        Settings settings = GetSettingsCopy();
        if (!IsNativeAutoHideEnabled()) {
            EnsureWorkAreaState(true);
            if (g_primaryTaskbarHwnd) {
                EvaluateHoverAndAnimate(g_primaryTaskbarHwnd, settings);
            }
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
            g_primaryTaskbarHwnd = nullptr;
        }
        return 0;
    }

    switch (uMsg) {
        case WM_TIMER:
            if (isPrimary && wParam == kTickTimerId && !g_shuttingDown.load(std::memory_order_relaxed)) {
                Settings settings = GetSettingsCopy();
                ULONGLONG now = GetTickCount64();

                if (!IsNativeAutoHideEnabled() &&
                    now - g_lastWorkAreaCheckTick >= kWorkAreaVerifyIntervalMs) {
                    EnsureWorkAreaState(true);
                }

                EvaluateHoverAndAnimate(hWnd, settings);
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGING: {
            auto it = g_taskbars.find(hWnd);
            if (it != g_taskbars.end()) {
                TaskbarState& tb = it->second;
                if (tb.isApplyingAnimPos) {
                    return 0;
                }

                // Keep normalRect in sync if Explorer moves/resizes the taskbar while hidden/animating
                auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                if (wp && !(wp->flags & SWP_NOMOVE) && !(wp->flags & SWP_NOSIZE) &&
                    wp->cx > 0 && wp->cy > 0) {
                    RECT req{wp->x, wp->y, wp->x + wp->cx, wp->y + wp->cy};
                    UpdateTaskbarGeometryFromRect(tb, req);
                }

                if (tb.animProgress < 0.999f && !tb.isCloaked && wp) {
                    int x = tb.normalRect.left;
                    int y = tb.normalRect.top;
                    HRGN clipRgn = nullptr;
                    ComputeSlidPoseRect(tb, tb.animProgress, &x, &y, &clipRgn);
                    if (clipRgn) {
                        DeleteObject(clipRgn);
                    }
                    wp->x = x;
                    wp->y = y;
                    wp->flags &= ~SWP_NOMOVE;
                }
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
                    tb.hasExplorerWorkRect = false;
                    SetTaskbarSlidePose(tb, 1.0f);
                    UpdateTaskbarGeometry(tb);
                }
                if (!IsNativeAutoHideEnabled()) {
                    EnsureWorkAreaState(true);
                }
                Settings settings = GetSettingsCopy();
                EvaluateHoverAndAnimate(hWnd, settings);
            }
            break;

        case WM_SETTINGCHANGE:
            if (isPrimary && lParam &&
                _wcsicmp(reinterpret_cast<LPCWSTR>(lParam), L"TraySettings") == 0) {
                g_needsInitialWorkAreaSync = true;
                for (auto& [tbHwnd, tb] : g_taskbars) {
                    tb.hasExplorerWorkRect = false;
                    SetTaskbarSlidePose(tb, 1.0f);
                    UpdateTaskbarGeometry(tb);
                }
                if (!IsNativeAutoHideEnabled()) {
                    EnsureWorkAreaState(true);
                }
                Settings settings = GetSettingsCopy();
                EvaluateHoverAndAnimate(hWnd, settings);
            }
            break;

        case WM_NCDESTROY:
            g_taskbars.erase(hWnd);
            if (isPrimary) {
                g_primaryTaskbarHwnd = nullptr;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void HandleNewWindowCreated(HWND hwnd) {
    if (!hwnd || g_shuttingDown.load(std::memory_order_relaxed)) {
        return;
    }
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) <= 0) {
        return;
    }
    bool isPrimary = (_wcsicmp(cls, L"Shell_TrayWnd") == 0);
    bool isSecondary = (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0);
    if (!isPrimary && !isSecondary) {
        return;
    }

    DWORD_PTR refData = isPrimary ? 1 : 2;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc, refData)) {
        PostMessageW(hwnd, g_msgInitNewTaskbar, 0, 0);
    }
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd) {
        HandleNewWindowCreated(hwnd);
    }
    return hwnd;
}

HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hwnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                            X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hwnd) {
        HandleNewWindowCreated(hwnd);
    }
    return hwnd;
}

BOOL CALLBACK EnumFindPrimaryTaskbarProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    WCHAR cls[64] = {};
    if (GetWindowThreadProcessId(hWnd, &pid) &&
        pid == GetCurrentProcessId() &&
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 &&
        _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindCurrentProcessPrimaryTaskbar() {
    HWND found = nullptr;
    EnumWindows(EnumFindPrimaryTaskbarProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

BOOL CALLBACK EnumThreadTaskbarsProc(HWND hWnd, LPARAM lParam) {
    WCHAR cls[64] = {};
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 && IsTaskbarClassBuf(cls)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
    }
    return TRUE;
}

void SubclassExistingTaskbars() {
    HWND primary = FindCurrentProcessPrimaryTaskbar();
    if (!primary) {
        return;
    }

    DWORD tid = GetWindowThreadProcessId(primary, nullptr);
    std::vector<HWND> threadTaskbars;
    EnumThreadWindows(tid, EnumThreadTaskbarsProc, reinterpret_cast<LPARAM>(&threadTaskbars));

    for (HWND hwnd : threadTaskbars) {
        if (hwnd != primary) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc, 2)) {
                PostMessageW(hwnd, g_msgInitNewTaskbar, 0, 0);
            }
        }
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(primary, TaskbarSubclassProc, 1)) {
        PostMessageW(primary, g_msgInitNewTaskbar, 0, 0);
    }
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

    WindhawkUtils::SetFunctionHook(SystemParametersInfoW,
                                   SystemParametersInfoW_Hook,
                                   &SystemParametersInfoW_Original);

    WindhawkUtils::SetFunctionHook(CreateWindowExW,
                                   CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pCreateWindowInBand = reinterpret_cast<CreateWindowInBand_t>(
            GetProcAddress(user32, "CreateWindowInBand"));
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    SubclassExistingTaskbars();
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

    HWND primary = FindCurrentProcessPrimaryTaskbar();
    if (primary) {
        DWORD tid = GetWindowThreadProcessId(primary, nullptr);
        std::vector<HWND> threadTaskbars;
        EnumThreadWindows(tid, EnumThreadTaskbarsProc, reinterpret_cast<LPARAM>(&threadTaskbars));

        // Synchronous wait ensures cleanup completes before removing subclasses and unloading DLL
        SendMessageW(primary, g_msgUninitCleanup, 0, 0);

        for (HWND hwnd : threadTaskbars) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc);
        }
    }
}

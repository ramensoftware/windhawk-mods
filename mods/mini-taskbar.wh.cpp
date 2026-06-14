// ==WindhawkMod==
// @id              mini-taskbar
// @name            MiniTaskbar
// @description     Transparent Windows 10 taskbar, wallpaper backing, and centered app buttons.
// @version         1.0.0
// @author          Mirochill
// @github          https://github.com/Mirochill
// @homepage        https://github.com/Mirochill/MiniTaskbar
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -loleacc -loleaut32 -lole32 -lgdi32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MiniTaskbar

MiniTaskbar keeps the Windows 10 taskbar visually minimal: transparent taskbar
accent, optional live wallpaper backing through DWM thumbnails, and centered
application buttons.

Windhawk starts this mod automatically in a dedicated helper process. The main
Explorer process is not used for a resident loop. WinEvent hooks wake the helper
when taskbar button geometry, foreground state, or Explorer taskbar windows
change, and fallback polling only refreshes handles/state.

## Compatibility

- Designed for the classic Windows 10 Explorer taskbar.
- Windows 11 uses a different XAML taskbar and should use Windows 11-specific
  Windhawk taskbar mods instead.
- If Mini Wallpaper is running, MiniTaskbar uses its desktop WorkerW host as
  the live backing source. Otherwise it falls back to the standard desktop host.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- transparentTaskbar: true
  $name: Transparent taskbar
  $description: Apply a transparent-gradient accent to primary and secondary taskbars.

- centerButtons: true
  $name: Center application buttons
  $description: Center Explorer's MSTaskListWClass application-button group.

- wallpaperBackdrop: true
  $name: Wallpaper backing
  $description: Place a DWM thumbnail of the desktop host behind each taskbar.

- hideBackdropOnFullscreen: true
  $name: Hide backing in fullscreen
  $description: Hide wallpaper backing while the foreground window covers its monitor.

- animateCentering: true
  $name: Animate centering
  $description: Slide the app-button group to its new center when taskbar buttons change.

- pollMilliseconds: 750
  $name: Fallback poll interval
  $description: Inexpensive fallback scan interval for taskbar handles and geometry.

- animationFrameMilliseconds: 15
  $name: Animation frame interval
  $description: Frame interval used only while a centering animation is active.

- slideDurationMilliseconds: 180
  $name: Slide duration
  $description: Duration of the taskbar button centering animation.

- sourceRefreshMilliseconds: 5000
  $name: Wallpaper source refresh interval
  $description: How often to re-check the desktop host used for taskbar wallpaper backing.

- centerFallbackMilliseconds: 5000
  $name: Centering fallback interval
  $description: Reassert the centered position even if Explorer misses an event.

- restoreOnUnload: true
  $name: Restore on unload
  $description: Restore the default taskbar accent and task-list position when the mod unloads.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <oleacc.h>
#include <oleauto.h>
#include <shellapi.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace {

constexpr PCWSTR kMainTaskbarClass = L"Shell_TrayWnd";
constexpr PCWSTR kSecondaryTaskbarClass = L"Shell_SecondaryTrayWnd";
constexpr PCWSTR kMiniWallpaperClass = L"MiniWallpaperWindhawkWindow";
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr DWORD kWinEventOutOfContext = WINEVENT_OUTOFCONTEXT;
constexpr DWORD kWinEventSkipOwnProcess = WINEVENT_SKIPOWNPROCESS;
constexpr LONG kObjIdWindow = OBJID_WINDOW;
constexpr LONG kObjIdClient = OBJID_CLIENT;
constexpr LONG kChildIdSelf = CHILDID_SELF;
constexpr UINT kSwpCenterFlags =
    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING |
    SWP_ASYNCWINDOWPOS;

enum class AccentState {
    Disabled = 0,
    TransparentGradient = 2,
};

struct Settings {
    bool transparentTaskbar = true;
    bool centerButtons = true;
    bool wallpaperBackdrop = true;
    bool hideBackdropOnFullscreen = true;
    bool animateCentering = true;
    int pollMilliseconds = 750;
    int animationFrameMilliseconds = 15;
    int slideDurationMilliseconds = 180;
    int sourceRefreshMilliseconds = 5000;
    int centerFallbackMilliseconds = 5000;
    bool restoreOnUnload = true;
};

struct TaskbarLayout {
    HWND parent = nullptr;
    HWND taskList = nullptr;
    bool horizontal = true;
};

struct ButtonBounds {
    int start = 0;
    int end = 0;
};

struct CenterState {
    int expectedScreenStart = 0;
    int relativeOffset = 0;
    bool horizontal = true;
    bool animating = false;
    int animationStartScreenStart = 0;
    int animationTargetScreenStart = 0;
    ULONGLONG animationStartTick = 0;
};

struct DwmThumbnailPropertiesCompat {
    DWORD dwFlags;
    RECT rcDestination;
    RECT rcSource;
    BYTE opacity;
    BOOL fVisible;
    BOOL fSourceClientAreaOnly;
};

struct Backdrop {
    HWND taskbar = nullptr;
    HWND window = nullptr;
    HTHUMBNAIL thumbnail = nullptr;
    HWND source = nullptr;
    bool hasLayout = false;
    RECT lastTaskbarRect{};
    RECT lastSourceRect{};
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
std::atomic<bool> g_stopWorker = false;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

HWINEVENTHOOK g_taskbarHook = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_foregroundLocationHook = nullptr;
HWND g_foregroundWindow = nullptr;
DWORD g_taskbarProcessId = 0;
std::vector<HWND> g_eventWindows;
std::vector<HWND> g_taskListEventWindows;
std::atomic<bool> g_centeringEvent = false;
std::atomic<bool> g_foregroundEvent = false;
std::atomic<bool> g_foregroundLocationEvent = false;
std::map<HWND, CenterState> g_centerStates;
std::vector<Backdrop> g_backdrops;
HWND g_wallpaperSource = nullptr;
bool g_backdropsVisible = true;

int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(maxValue, value));
}

Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

void LoadSettings() {
    Settings next;
    next.transparentTaskbar = Wh_GetIntSetting(L"transparentTaskbar") != 0;
    next.centerButtons = Wh_GetIntSetting(L"centerButtons") != 0;
    next.wallpaperBackdrop = Wh_GetIntSetting(L"wallpaperBackdrop") != 0;
    next.hideBackdropOnFullscreen =
        Wh_GetIntSetting(L"hideBackdropOnFullscreen") != 0;
    next.animateCentering = Wh_GetIntSetting(L"animateCentering") != 0;
    next.pollMilliseconds =
        ClampInt(Wh_GetIntSetting(L"pollMilliseconds"), 100, 10000);
    next.animationFrameMilliseconds =
        ClampInt(Wh_GetIntSetting(L"animationFrameMilliseconds"), 8, 100);
    next.slideDurationMilliseconds =
        ClampInt(Wh_GetIntSetting(L"slideDurationMilliseconds"), 0, 2000);
    next.sourceRefreshMilliseconds =
        ClampInt(Wh_GetIntSetting(L"sourceRefreshMilliseconds"), 1000, 60000);
    next.centerFallbackMilliseconds =
        ClampInt(Wh_GetIntSetting(L"centerFallbackMilliseconds"), 1000, 60000);
    next.restoreOnUnload = Wh_GetIntSetting(L"restoreOnUnload") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = next;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

int RectWidth(const RECT& rect) {
    return rect.right - rect.left;
}

int RectHeight(const RECT& rect) {
    return rect.bottom - rect.top;
}

bool RectEquals(const RECT& left, const RECT& right) {
    return left.left == right.left && left.top == right.top &&
           left.right == right.right && left.bottom == right.bottom;
}

std::wstring GetClassNameString(HWND hwnd) {
    wchar_t className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return className;
}

std::vector<HWND> FindTaskbars() {
    std::vector<HWND> taskbars;
    HWND mainTaskbar = FindWindowW(kMainTaskbarClass, nullptr);
    if (mainTaskbar) {
        taskbars.push_back(mainTaskbar);
    }

    HWND previous = nullptr;
    while (true) {
        HWND secondary = FindWindowExW(nullptr, previous,
                                       kSecondaryTaskbarClass, nullptr);
        if (!secondary) {
            break;
        }

        taskbars.push_back(secondary);
        previous = secondary;
    }

    return taskbars;
}

bool HaveSameHandles(const std::vector<HWND>& left,
                     const std::vector<HWND>& right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (HWND hwnd : left) {
        if (std::find(right.begin(), right.end(), hwnd) == right.end()) {
            return false;
        }
    }

    return true;
}

struct AccentPolicy {
    AccentState accentState;
    int accentFlags;
    int gradientColor;
    int animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    void* data;
    int sizeOfData;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

bool TrySetAccent(HWND hwnd, AccentState accentState) {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return false;
    }

    auto setWindowCompositionAttribute =
        reinterpret_cast<SetWindowCompositionAttribute_t>(
            GetProcAddress(user32, "SetWindowCompositionAttribute"));
    if (!setWindowCompositionAttribute) {
        return false;
    }

    AccentPolicy policy{
        accentState,
        0,
        accentState == AccentState::TransparentGradient ? 0x00000001 : 0,
        0,
    };
    WindowCompositionAttributeData data{19, &policy, sizeof(policy)};
    return setWindowCompositionAttribute(hwnd, &data) != FALSE;
}

void ApplyAccent(const std::vector<HWND>& taskbars, AccentState accent) {
    for (HWND taskbar : taskbars) {
        TrySetAccent(taskbar, accent);
    }
}

bool TryFindLayout(HWND taskbar, TaskbarLayout* layout) {
    HWND rebar = FindWindowExW(taskbar, nullptr, L"ReBarWindow32", nullptr);
    HWND parent = FindWindowExW(rebar, nullptr, L"MSTaskSwWClass", nullptr);
    HWND taskList = FindWindowExW(parent, nullptr, L"MSTaskListWClass", nullptr);

    if (!taskList) {
        parent = FindWindowExW(taskbar, nullptr, L"WorkerW", nullptr);
        taskList = FindWindowExW(parent, nullptr, L"MSTaskListWClass", nullptr);
    }

    if (!parent || !taskList) {
        return false;
    }

    RECT taskbarRect{};
    if (!GetWindowRect(taskbar, &taskbarRect)) {
        return false;
    }

    layout->parent = parent;
    layout->taskList = taskList;
    layout->horizontal = RectWidth(taskbarRect) >= RectHeight(taskbarRect);
    return true;
}

bool SelectCentralAnchor(std::vector<ButtonBounds>* buttons,
                         int* anchorCenterTimesFour) {
    if (buttons->empty()) {
        return false;
    }

    std::sort(buttons->begin(), buttons->end(), [](const auto& left,
                                                   const auto& right) {
        if (left.start != right.start) {
            return left.start < right.start;
        }
        return left.end < right.end;
    });

    std::vector<ButtonBounds> unique;
    for (const ButtonBounds& button : *buttons) {
        if (unique.empty() || unique.back().start != button.start ||
            unique.back().end != button.end) {
            unique.push_back(button);
        }
    }

    const ButtonBounds& middle = unique[(unique.size() - 1) / 2];
    *anchorCenterTimesFour = 2 * (middle.start + middle.end);
    return true;
}

int RoundTimesFourToInt(long long value) {
    return value >= 0 ? static_cast<int>((value + 2) / 4)
                      : static_cast<int>((value - 2) / 4);
}

int CalculateCenteredOffset(int taskbarStart,
                            int taskbarSize,
                            int parentStart,
                            int taskListStart,
                            int anchorCenterTimesFour) {
    long long taskbarCenterTimesFour =
        (4LL * taskbarStart) + (2LL * taskbarSize);
    long long anchorInsetTimesFour =
        anchorCenterTimesFour - (4LL * taskListStart);
    int targetListStart =
        RoundTimesFourToInt(taskbarCenterTimesFour - anchorInsetTimesFour);
    return targetListStart - parentStart;
}

bool TryGetCentralApplicationAnchor(HWND taskList,
                                    bool horizontal,
                                    int* anchorCenterTimesFour) {
    IAccessible* accessible = nullptr;
    HRESULT hr = AccessibleObjectFromWindow(
        taskList, OBJID_CLIENT, IID_IAccessible,
        reinterpret_cast<void**>(&accessible));
    if (FAILED(hr) || !accessible) {
        return false;
    }

    long childCount = 0;
    hr = accessible->get_accChildCount(&childCount);
    if (FAILED(hr) || childCount <= 0) {
        accessible->Release();
        return false;
    }

    std::vector<ButtonBounds> buttons;
    for (long childIndex = 1; childIndex <= childCount; childIndex++) {
        VARIANT child;
        VariantInit(&child);
        child.vt = VT_I4;
        child.lVal = childIndex;

        long left = 0;
        long top = 0;
        long width = 0;
        long height = 0;
        hr = accessible->accLocation(&left, &top, &width, &height, child);
        VariantClear(&child);
        if (FAILED(hr) || width <= 0 || height <= 0) {
            continue;
        }

        int start = horizontal ? static_cast<int>(left) : static_cast<int>(top);
        int size = horizontal ? static_cast<int>(width) : static_cast<int>(height);
        buttons.push_back(ButtonBounds{start, start + size});
    }

    accessible->Release();
    return SelectCentralAnchor(&buttons, anchorCenterTimesFour);
}

int CalculateSlidePosition(int start,
                           int target,
                           int elapsedMilliseconds,
                           int durationMilliseconds) {
    if (durationMilliseconds <= 0 || elapsedMilliseconds <= 0) {
        return target;
    }

    if (elapsedMilliseconds >= durationMilliseconds) {
        return target;
    }

    double remaining =
        1.0 - (elapsedMilliseconds / static_cast<double>(durationMilliseconds));
    double progress = 1.0 - (remaining * remaining * remaining);
    return start + static_cast<int>(
                       std::round((target - start) * progress));
}

bool MoveTaskList(const TaskbarLayout& layout, int relativeOffset) {
    return SetWindowPos(layout.taskList, nullptr,
                        layout.horizontal ? relativeOffset : 0,
                        layout.horizontal ? 0 : relativeOffset, 0, 0,
                        kSwpCenterFlags) != FALSE;
}

bool CenterTaskbar(HWND taskbar, bool force, bool animate, const Settings& s) {
    TaskbarLayout layout;
    RECT taskbarRect{};
    RECT parentRect{};
    RECT taskListRect{};
    if (!TryFindLayout(taskbar, &layout) ||
        !GetWindowRect(taskbar, &taskbarRect) ||
        !GetWindowRect(layout.parent, &parentRect) ||
        !GetWindowRect(layout.taskList, &taskListRect)) {
        return false;
    }

    int anchorCenterTimesFour = 0;
    if (!TryGetCentralApplicationAnchor(layout.taskList, layout.horizontal,
                                        &anchorCenterTimesFour)) {
        return false;
    }

    int taskbarStart = layout.horizontal ? taskbarRect.left : taskbarRect.top;
    int taskbarSize = layout.horizontal ? RectWidth(taskbarRect)
                                        : RectHeight(taskbarRect);
    int parentStart = layout.horizontal ? parentRect.left : parentRect.top;
    int taskListStart = layout.horizontal ? taskListRect.left : taskListRect.top;
    int relativeOffset = CalculateCenteredOffset(
        taskbarStart, taskbarSize, parentStart, taskListStart,
        anchorCenterTimesFour);
    int expectedScreenStart = parentStart + relativeOffset;

    CenterState& state = g_centerStates[layout.taskList];
    state.expectedScreenStart = expectedScreenStart;
    state.relativeOffset = relativeOffset;
    state.horizontal = layout.horizontal;

    if (taskListStart == expectedScreenStart && !force) {
        state.animating = false;
        return true;
    }

    if (!animate || s.slideDurationMilliseconds <= 0) {
        state.animating = false;
        return MoveTaskList(layout, relativeOffset);
    }

    ULONGLONG now = GetTickCount64();
    if (!state.animating ||
        state.animationTargetScreenStart != expectedScreenStart) {
        state.animating = true;
        state.animationStartScreenStart = taskListStart;
        state.animationTargetScreenStart = expectedScreenStart;
        state.animationStartTick = now;
    }

    int elapsed = static_cast<int>(now - state.animationStartTick);
    int screenStart = CalculateSlidePosition(
        state.animationStartScreenStart, state.animationTargetScreenStart,
        elapsed, s.slideDurationMilliseconds);
    if (screenStart == state.animationTargetScreenStart) {
        state.animating = false;
    }

    return screenStart == taskListStart ||
           MoveTaskList(layout, screenStart - parentStart);
}

void CenterTaskbars(const std::vector<HWND>& taskbars,
                    bool force,
                    bool animate,
                    const Settings& s) {
    for (HWND taskbar : taskbars) {
        CenterTaskbar(taskbar, force, animate, s);
    }
}

bool HasActiveAnimation() {
    for (const auto& pair : g_centerStates) {
        if (pair.second.animating) {
            return true;
        }
    }
    return false;
}

void RestoreTaskbarPositions() {
    g_centerStates.clear();
    for (HWND taskbar : FindTaskbars()) {
        TaskbarLayout layout;
        if (TryFindLayout(taskbar, &layout)) {
            SetWindowPos(layout.taskList, nullptr, 0, 0, 0, 0, kSwpCenterFlags);
        }
    }
}

bool CoversMonitor(const RECT& windowRect, const RECT& monitorRect) {
    return windowRect.left <= monitorRect.left &&
           windowRect.top <= monitorRect.top &&
           windowRect.right >= monitorRect.right &&
           windowRect.bottom >= monitorRect.bottom;
}

bool IsForegroundFullscreen() {
    HWND foreground = GetForegroundWindow();
    if (!foreground || !IsWindowVisible(foreground)) {
        return false;
    }

    std::wstring className = GetClassNameString(foreground);
    if (className == kMainTaskbarClass || className == kSecondaryTaskbarClass ||
        className == L"Progman" || className == L"WorkerW") {
        return false;
    }

    RECT windowRect{};
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    return monitor && GetWindowRect(foreground, &windowRect) &&
           GetMonitorInfoW(monitor, &monitorInfo) &&
           CoversMonitor(windowRect, monitorInfo.rcMonitor);
}

BOOL CALLBACK FindMiniWallpaperWorkerProc(HWND hwnd, LPARAM lParam) {
    if (GetClassNameString(hwnd) != L"WorkerW") {
        return TRUE;
    }

    HWND child = FindWindowExW(hwnd, nullptr, kMiniWallpaperClass, nullptr);
    if (child) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindWallpaperSource() {
    HWND animatedWorker = nullptr;
    EnumWindows(FindMiniWallpaperWorkerProc,
                reinterpret_cast<LPARAM>(&animatedWorker));
    if (animatedWorker) {
        return animatedWorker;
    }

    HWND progman = FindWindowW(L"Progman", nullptr);
    return progman;
}

Backdrop* FindBackdrop(HWND taskbar) {
    for (Backdrop& backdrop : g_backdrops) {
        if (backdrop.taskbar == taskbar) {
            return &backdrop;
        }
    }

    g_backdrops.push_back(Backdrop{taskbar});
    return &g_backdrops.back();
}

void DestroyBackdrop(Backdrop* backdrop) {
    if (backdrop->thumbnail) {
        DwmUnregisterThumbnail(backdrop->thumbnail);
        backdrop->thumbnail = nullptr;
    }
    if (backdrop->window) {
        DestroyWindow(backdrop->window);
        backdrop->window = nullptr;
    }
    backdrop->source = nullptr;
    backdrop->hasLayout = false;
}

void CleanupRemovedBackdrops(const std::vector<HWND>& taskbars) {
    for (auto it = g_backdrops.begin(); it != g_backdrops.end();) {
        if (std::find(taskbars.begin(), taskbars.end(), it->taskbar) ==
            taskbars.end()) {
            DestroyBackdrop(&*it);
            it = g_backdrops.erase(it);
        } else {
            ++it;
        }
    }
}

bool TryGetSourceCrop(const RECT& taskbarRect,
                      const RECT& sourceRect,
                      RECT* sourceCrop) {
    if (taskbarRect.left < sourceRect.left ||
        taskbarRect.top < sourceRect.top ||
        taskbarRect.right > sourceRect.right ||
        taskbarRect.bottom > sourceRect.bottom) {
        return false;
    }

    sourceCrop->left = taskbarRect.left - sourceRect.left;
    sourceCrop->top = taskbarRect.top - sourceRect.top;
    sourceCrop->right = taskbarRect.right - sourceRect.left;
    sourceCrop->bottom = taskbarRect.bottom - sourceRect.top;
    return true;
}

bool RefreshBackdrop(Backdrop* backdrop,
                     HWND source,
                     bool reassert,
                     bool visible) {
    RECT taskbarRect{};
    RECT sourceRect{};
    if (!GetWindowRect(backdrop->taskbar, &taskbarRect) ||
        !GetWindowRect(source, &sourceRect)) {
        return false;
    }

    RECT sourceCrop{};
    if (!TryGetSourceCrop(taskbarRect, sourceRect, &sourceCrop)) {
        return false;
    }

    bool layoutChanged = !backdrop->hasLayout ||
                         !RectEquals(taskbarRect, backdrop->lastTaskbarRect) ||
                         !RectEquals(sourceRect, backdrop->lastSourceRect);

    if (!backdrop->window) {
        backdrop->window = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, L"STATIC",
            L"MiniTaskbar.WallpaperBacking", WS_POPUP, taskbarRect.left,
            taskbarRect.top, RectWidth(taskbarRect), RectHeight(taskbarRect),
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!backdrop->window) {
            return false;
        }
    }

    bool thumbnailChanged = backdrop->source != source || !backdrop->thumbnail;
    if (thumbnailChanged) {
        if (backdrop->thumbnail) {
            DwmUnregisterThumbnail(backdrop->thumbnail);
            backdrop->thumbnail = nullptr;
        }

        if (FAILED(DwmRegisterThumbnail(backdrop->window, source,
                                        &backdrop->thumbnail))) {
            backdrop->thumbnail = nullptr;
            return false;
        }

        backdrop->source = source;
    }

    if (!thumbnailChanged && !layoutChanged && !reassert) {
        return true;
    }

    SetWindowPos(backdrop->window, backdrop->taskbar, taskbarRect.left,
                 taskbarRect.top, RectWidth(taskbarRect),
                 RectHeight(taskbarRect),
                 SWP_NOACTIVATE | (visible ? SWP_SHOWWINDOW : 0));

    DwmThumbnailPropertiesCompat props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                    DWM_TNP_OPACITY | DWM_TNP_VISIBLE |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = RECT{0, 0, RectWidth(taskbarRect),
                               RectHeight(taskbarRect)};
    props.rcSource = sourceCrop;
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    HRESULT hr = DwmUpdateThumbnailProperties(
        backdrop->thumbnail,
        reinterpret_cast<DWM_THUMBNAIL_PROPERTIES*>(&props));
    if (SUCCEEDED(hr)) {
        backdrop->lastTaskbarRect = taskbarRect;
        backdrop->lastSourceRect = sourceRect;
        backdrop->hasLayout = true;
    }

    ShowWindow(backdrop->window, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
    return SUCCEEDED(hr);
}

void SetBackdropsVisible(bool visible) {
    g_backdropsVisible = visible;
    for (Backdrop& backdrop : g_backdrops) {
        if (backdrop.window) {
            ShowWindow(backdrop.window, visible ? SW_SHOWNOACTIVATE : SW_HIDE);
        }
    }
}

void RefreshBackdrops(const std::vector<HWND>& taskbars,
                      bool refreshSource,
                      bool reassert,
                      bool visible) {
    CleanupRemovedBackdrops(taskbars);
    if (refreshSource || !g_wallpaperSource || !IsWindow(g_wallpaperSource)) {
        g_wallpaperSource = FindWallpaperSource();
    }
    if (!g_wallpaperSource) {
        return;
    }

    for (HWND taskbar : taskbars) {
        Backdrop* backdrop = FindBackdrop(taskbar);
        RefreshBackdrop(backdrop, g_wallpaperSource, reassert, visible);
    }
}

void DestroyAllBackdrops() {
    for (Backdrop& backdrop : g_backdrops) {
        DestroyBackdrop(&backdrop);
    }
    g_backdrops.clear();
    g_wallpaperSource = nullptr;
}

DWORD FindTaskbarProcessId(const std::vector<HWND>& taskbars) {
    for (HWND taskbar : taskbars) {
        DWORD processId = 0;
        GetWindowThreadProcessId(taskbar, &processId);
        if (processId) {
            return processId;
        }
    }
    return 0;
}

void RefreshEventWindows(const std::vector<HWND>& taskbars) {
    g_eventWindows.clear();
    g_taskListEventWindows.clear();

    for (HWND taskbar : taskbars) {
        TaskbarLayout layout;
        if (TryFindLayout(taskbar, &layout)) {
            g_eventWindows.push_back(taskbar);
            g_eventWindows.push_back(layout.parent);
            g_eventWindows.push_back(layout.taskList);
            g_taskListEventWindows.push_back(layout.taskList);
        }
    }
}

bool ContainsWindow(const std::vector<HWND>& windows, HWND hwnd) {
    return std::find(windows.begin(), windows.end(), hwnd) != windows.end();
}

bool IsCenteringEvent(DWORD eventType,
                      LONG objectId,
                      LONG childId,
                      bool taskListWindow) {
    bool centeringEvent =
        (eventType >= EVENT_OBJECT_CREATE && eventType <= EVENT_OBJECT_REORDER) ||
        eventType == EVENT_OBJECT_LOCATIONCHANGE;
    return centeringEvent &&
           !(taskListWindow && eventType == EVENT_OBJECT_LOCATIONCHANGE &&
             objectId == kObjIdWindow && childId == kChildIdSelf);
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD eventType,
                           HWND hwnd,
                           LONG objectId,
                           LONG childId,
                           DWORD,
                           DWORD) {
    if (eventType == EVENT_SYSTEM_FOREGROUND) {
        g_foregroundWindow = hwnd;
        g_foregroundEvent = true;
        return;
    }

    if (eventType == EVENT_OBJECT_LOCATIONCHANGE &&
        hwnd == g_foregroundWindow) {
        g_foregroundLocationEvent = true;
    }

    bool taskListWindow = ContainsWindow(g_taskListEventWindows, hwnd);
    if (ContainsWindow(g_eventWindows, hwnd) &&
        IsCenteringEvent(eventType, objectId, childId, taskListWindow)) {
        g_centeringEvent = true;
    }
}

void ReconfigureTaskbarHook(const std::vector<HWND>& taskbars) {
    DWORD processId = FindTaskbarProcessId(taskbars);
    if (processId == g_taskbarProcessId && g_taskbarHook) {
        return;
    }

    if (g_taskbarHook) {
        UnhookWinEvent(g_taskbarHook);
        g_taskbarHook = nullptr;
    }

    g_taskbarProcessId = processId;
    if (processId) {
        g_taskbarHook = SetWinEventHook(
            EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            WinEventProc, processId, 0,
            kWinEventOutOfContext | kWinEventSkipOwnProcess);
    }
}

void InstallWinEventHooks(const std::vector<HWND>& taskbars) {
    g_foregroundWindow = GetForegroundWindow();
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, kWinEventOutOfContext | kWinEventSkipOwnProcess);
    g_foregroundLocationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, kWinEventOutOfContext | kWinEventSkipOwnProcess);
    RefreshEventWindows(taskbars);
    ReconfigureTaskbarHook(taskbars);
}

void UninstallWinEventHooks() {
    if (g_taskbarHook) {
        UnhookWinEvent(g_taskbarHook);
        g_taskbarHook = nullptr;
    }
    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    if (g_foregroundLocationHook) {
        UnhookWinEvent(g_foregroundLocationHook);
        g_foregroundLocationHook = nullptr;
    }
}

void ApplySettingsToCurrentState(const std::vector<HWND>& taskbars,
                                 bool fullscreen) {
    Settings s = GetSettingsSnapshot();
    if (s.transparentTaskbar) {
        ApplyAccent(taskbars, AccentState::TransparentGradient);
    } else if (s.restoreOnUnload) {
        ApplyAccent(taskbars, AccentState::Disabled);
    }
    if (s.wallpaperBackdrop) {
        RefreshBackdrops(taskbars, true, true,
                         !(s.hideBackdropOnFullscreen && fullscreen));
    } else {
        DestroyAllBackdrops();
    }
    if (s.centerButtons) {
        CenterTaskbars(taskbars, true, false, s);
    } else if (s.restoreOnUnload) {
        RestoreTaskbarPositions();
    }
}

DWORD WINAPI WorkerThreadProc(void*) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MSG queueMessage{};
    PeekMessageW(&queueMessage, nullptr, 0, 0, PM_NOREMOVE);
    g_workerThreadId = GetCurrentThreadId();
    LoadSettings();

    std::vector<HWND> taskbars = FindTaskbars();
    bool fullscreen = IsForegroundFullscreen();
    InstallWinEventHooks(taskbars);
    ApplySettingsToCurrentState(taskbars, fullscreen);

    ULONGLONG lastPoll = GetTickCount64();
    ULONGLONG lastSourceRefresh = GetTickCount64();
    ULONGLONG lastCenterFallback = GetTickCount64();

    while (!g_stopWorker.load()) {
        Settings s = GetSettingsSnapshot();
        DWORD wait = HasActiveAnimation()
                         ? static_cast<DWORD>(s.animationFrameMilliseconds)
                         : static_cast<DWORD>(s.pollMilliseconds);
        DWORD result = MsgWaitForMultipleObjectsEx(
            0, nullptr, wait, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_stopWorker = true;
                break;
            }
            if (msg.message == kMsgReloadSettings) {
                LoadSettings();
                s = GetSettingsSnapshot();
                ApplySettingsToCurrentState(taskbars, fullscreen);
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_stopWorker.load()) {
            break;
        }

        bool centeringEvent = g_centeringEvent.exchange(false);
        bool foregroundEvent = g_foregroundEvent.exchange(false);
        bool foregroundLocationEvent = g_foregroundLocationEvent.exchange(false);
        bool fullscreenChanged = false;
        if (foregroundEvent || foregroundLocationEvent) {
            bool currentFullscreen = IsForegroundFullscreen();
            fullscreenChanged = currentFullscreen != fullscreen;
            fullscreen = currentFullscreen;
            if (fullscreenChanged && s.wallpaperBackdrop) {
                SetBackdropsVisible(!(s.hideBackdropOnFullscreen && fullscreen));
            }
        }

        ULONGLONG now = GetTickCount64();
        bool pollDue =
            now - lastPoll >= static_cast<ULONGLONG>(s.pollMilliseconds);
        bool taskbarsChanged = false;
        if (pollDue) {
            std::vector<HWND> current = FindTaskbars();
            taskbarsChanged = !HaveSameHandles(taskbars, current);
            if (taskbarsChanged) {
                taskbars = current;
                RefreshEventWindows(taskbars);
                ReconfigureTaskbarHook(taskbars);
            }
            lastPoll = now;
        }

        if (s.transparentTaskbar &&
            (taskbarsChanged || centeringEvent || foregroundEvent)) {
            ApplyAccent(taskbars, AccentState::TransparentGradient);
        }

        bool refreshSource =
            now - lastSourceRefresh >=
            static_cast<ULONGLONG>(s.sourceRefreshMilliseconds);
        if (s.wallpaperBackdrop &&
            (taskbarsChanged || refreshSource || foregroundEvent ||
             fullscreenChanged)) {
            RefreshBackdrops(taskbars, refreshSource || taskbarsChanged,
                             true,
                             !(s.hideBackdropOnFullscreen && fullscreen));
            if (refreshSource) {
                lastSourceRefresh = now;
            }
        }

        bool fallbackCenter =
            now - lastCenterFallback >=
            static_cast<ULONGLONG>(s.centerFallbackMilliseconds);
        bool activeAnimation = HasActiveAnimation();
        if (s.centerButtons &&
            (centeringEvent || taskbarsChanged || fallbackCenter ||
             activeAnimation)) {
            CenterTaskbars(taskbars, centeringEvent || taskbarsChanged,
                           s.animateCentering &&
                               (centeringEvent || taskbarsChanged ||
                                activeAnimation),
                           s);
            if (centeringEvent || taskbarsChanged || fallbackCenter) {
                lastCenterFallback = now;
            }
        }

        (void)result;
    }

    UninstallWinEventHooks();
    Settings s = GetSettingsSnapshot();
    if (s.restoreOnUnload) {
        ApplyAccent(FindTaskbars(), AccentState::Disabled);
        RestoreTaskbarPositions();
    }
    DestroyAllBackdrops();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_stopWorker = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, kMsgReloadSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_stopWorker = true;
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 10000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
}

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

enum class ToolProcessKind {
    NormalExplorer,
    OtherToolMod,
    CurrentToolMod,
    Excluded,
};

ToolProcessKind GetToolProcessKind() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        return ToolProcessKind::NormalExplorer;
    }

    ToolProcessKind kind = ToolProcessKind::NormalExplorer;
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            kind = ToolProcessKind::Excluded;
            break;
        }
    }

    if (kind == ToolProcessKind::NormalExplorer) {
        for (int i = 1; i < argc - 1; i++) {
            if (wcscmp(argv[i], L"-tool-mod") == 0) {
                kind = wcscmp(argv[i + 1], WH_MOD_ID) == 0
                           ? ToolProcessKind::CurrentToolMod
                           : ToolProcessKind::OtherToolMod;
                break;
            }
        }
    }

    LocalFree(argv);
    return kind;
}

bool LaunchToolModProcess() {
    WCHAR currentProcessPath[MAX_PATH]{};
    DWORD length = GetModuleFileNameW(nullptr, currentProcessPath,
                                      ARRAYSIZE(currentProcessPath));
    if (length == 0 || length == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileNameW failed");
        return false;
    }

    WCHAR commandLine[MAX_PATH + 64]{};
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
    }
    if (!kernelModule) {
        return false;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
        LPPROCESS_INFORMATION, PHANDLE);

    auto createProcessInternal =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal) {
        return false;
    }

    STARTUPINFOW startupInfo{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr,
                               nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr,
                               nullptr, &startupInfo, &processInfo, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed: %lu", GetLastError());
        return false;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    ToolProcessKind kind = GetToolProcessKind();
    if (kind == ToolProcessKind::Excluded ||
        kind == ToolProcessKind::OtherToolMod) {
        return FALSE;
    }

    if (kind == ToolProcessKind::CurrentToolMod) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex ||
            GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        auto* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) +
            ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
        return TRUE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isToolModProcessLauncher) {
        LaunchToolModProcess();
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}

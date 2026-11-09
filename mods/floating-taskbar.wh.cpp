// ==WindhawkMod==
// @id              floating-taskbar
// @name            Floating Taskbar
// @name:zh-CN      悬浮任务栏
// @description     Turns the Windows 11 taskbar into a floating rounded bar with configurable width, height, spacing, corners, colors and gradients. Blank areas click through; tray menus and Quick Settings follow the bar.
// @description:zh-CN 将 Windows 11 任务栏变为独立的悬浮圆角条，可调整长度、区域高度、悬浮间距、圆角与背景颜色/渐变；空白区域点击穿透，托盘菜单和快速设置面板对齐悬浮条右端。
// @version         2.1.11
// @author          chaochenzhudian
// @github          https://github.com/chaochenzhudian
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -loleaut32 -lruntimeobject -luser32 -lwindowsapp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Floating Taskbar

## Features

- Configure the floating bar width, taskbar area height, bottom gap, and
  corner radii.
- Configure the corner radius of taskbar hover highlights.
- Use the system background, a transparent background, translucent color, a
  solid color, a linear gradient, or a custom XAML brush.
- Click through blank areas inside the floating taskbar.
- Keep tray menus and Quick Settings aligned with the right edge of the bar.

## Settings

Widths, area height, spacing, and radii are physical pixels. A taskbar area
height of 0 follows the system. A custom XAML background that is left empty
also follows the system background.

The same physical-pixel values appear differently at different display scales.
Set the values separately if precise consistency across mixed-DPI monitors is
required.

Multi-window taskbar highlights are currently detected from the Chinese and
English taskbar accessible names. On other Windows display languages, those
highlights may not be hidden.

## Differences from other mods

Windows 11 Taskbar Styler, Taskbar height and icon size, and MacOS Style Dock
can also create a floating or unusually sized taskbar. Floating Taskbar also
adds click-through blank space, tray and Quick Settings alignment, automatic
tray layout reassertion, and multi-window highlight cleanup. It writes
properties on the taskbar's `RootGrid` and `BackgroundFill`, so combining it
with a Taskbar Styler theme that styles the same properties is not supported.

Use Windhawk's normal mod toggle to disable the mod; there is no separate
enable setting.

## 中文说明

将 Windows 11 任务栏变为独立的悬浮圆角条，可调整长度、区域高度、悬浮间距、
任务栏与待选框圆角，以及背景颜色或渐变。悬浮条内部空白区域可以点击穿透，
托盘菜单与快速设置面板会对齐悬浮条右端。

长度、区域高度、间距和圆角使用物理像素。多窗口任务栏高亮目前依靠中文或英文
辅助功能名称识别，其他显示语言可能不隐藏该高亮。若同时使用 Taskbar Styler
并修改相同的 `RootGrid` 或 `BackgroundFill` 属性，两个 Mod 可能冲突。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- floatingBar:
  - taskbarWidth: 1250
    $name: Taskbar width
    $name:zh-CN: 任务栏长度
    $description: >-
      Total width of the floating bar, including the system tray area, in
      physical pixels. 0 uses the content width.
    $description:zh-CN: >-
      悬浮条的总长度（包含右下角系统托盘区），单位为物理像素。0 表示根据内容自动宽度。
  - taskbarAreaHeight: 60
    $name: Taskbar area height
    $name:zh-CN: 任务栏区域高度
    $description: >-
      Height from the top of the floating bar to the bottom of the screen in
      physical pixels. 0 follows the system taskbar height. The floating bar
      height is this value minus the bottom gap.
    $description:zh-CN: >-
      悬浮条顶端到屏幕底端的距离（即整个“任务栏区域”的高度），单位为物理像素。
      0 表示跟随系统任务栏高度。悬浮条高度 = 本项 − 悬浮间距。
  - floatMargin: 5
    $name: Bottom gap
    $name:zh-CN: 悬浮间距
    $description: >-
      Distance from the bottom of the floating bar to the bottom of the screen
      in physical pixels.
    $description:zh-CN: >-
      悬浮条底端到屏幕底端的距离，单位为物理像素。
  - cornerRadius: 14
    $name: Taskbar corner radius
    $name:zh-CN: 任务栏圆角半径
    $description: Corner radius of the floating bar in physical pixels.
    $description:zh-CN: 悬浮条的圆角半径，单位为物理像素。
  - hoverCornerRadius: 20
    $name: Hover highlight corner radius
    $name:zh-CN: 任务栏待选框圆角半径
    $description: >-
      Corner radius of taskbar hover highlights in physical pixels. 0 uses
      square corners.
    $description:zh-CN: >-
      任务栏图标悬停时待选框的圆角半径，单位为物理像素。0 表示直角。
  $name: Floating taskbar
  $name:zh-CN: 悬浮任务栏
- taskbarBackgroundBrush:
  - backgroundStyle: system
    $name: Background style
    $name:zh-CN: 背景画刷
    $description: >-
      Choose the floating bar background. Presets use the color, opacity, and
      gradient settings below. Custom XAML uses the XAML setting below.
    $description:zh-CN: >-
      选择悬浮条背景样式。预设会使用下方的颜色、透明度和渐变参数；
      “自定义 XAML”留空时等同跟随系统。
    $options:
    - custom: Custom XAML
    - system: Follow system
    - transparent: Transparent
    - translucent: Translucent color
    - solid: Solid color
    - gradient: Linear gradient
    $options:zh-CN:
    - custom: 自定义 XAML
    - system: 跟随系统
    - transparent: 完全透明
    - translucent: 色彩通透定制
    - solid: 纯色
    - gradient: 线性渐变
  - backgroundColor: "#202020"
    $name: Background color
    $name:zh-CN: 背景颜色
    $description: >-
      Color used by the translucent and solid styles. Use #RRGGBB or
      #AARRGGBB. The solid style preserves the color's own alpha.
    $description:zh-CN: >-
      用于色彩通透定制和纯色样式，格式为 #RRGGBB 或 #AARRGGBB。
      纯色样式会保留颜色自带的透明度。
  - backgroundOpacity: 75
    $name: Background opacity
    $name:zh-CN: 背景透明度
    $description: >-
      0 is fully transparent, 100 is fully opaque. Used by the translucent and
      gradient styles.
    $description:zh-CN: >-
      0 = 完全透明，100 = 完全不透明。用于色彩通透定制和渐变样式。
  - gradientColor1: "#1F2937"
    $name: Gradient start color
    $name:zh-CN: 渐变起始颜色
    $description: Linear gradient start color in #RRGGBB or #AARRGGBB format.
    $description:zh-CN: 线性渐变的起始颜色，格式为 #RRGGBB 或 #AARRGGBB。
  - gradientColor2: "#111827"
    $name: Gradient end color
    $name:zh-CN: 渐变结束颜色
    $description: Linear gradient end color in #RRGGBB or #AARRGGBB format.
    $description:zh-CN: 线性渐变的结束颜色，格式为 #RRGGBB 或 #AARRGGBB。
  - gradientAngle: 45
    $name: Gradient angle
    $name:zh-CN: 渐变角度
    $description: >-
      0 is left to right, 45 is top-left to bottom-right, 90 is top to bottom,
      and 180 is right to left.
    $description:zh-CN: >-
      0 = 从左到右，45 = 左上到右下，90 = 从上到下，180 = 从右到左。
  - gradientStartPosition: 0
    $name: Gradient start position (%)
    $name:zh-CN: 渐变起始位置（%）
    $description: First color position along the gradient; 0 is start, 100 is end.
    $description:zh-CN: 第一个颜色在渐变方向上的落点，0 = 起点，100 = 终点。
  - gradientEndPosition: 100
    $name: Gradient end position (%)
    $name:zh-CN: 渐变结束位置（%）
    $description: Second color position along the gradient; 0 is start, 100 is end.
    $description:zh-CN: 第二个颜色在渐变方向上的落点，0 = 起点，100 = 终点。
  - backgroundBrush: ""
    $name: Custom XAML
    $name:zh-CN: 自定义 XAML
    $description: >-
      Used when Background style is Custom XAML. Leave empty to follow the
      system background. Example:
      <SolidColorBrush Color="#CC202020" />
    $description:zh-CN: >-
      仅当背景画刷选择“自定义 XAML”时使用，留空则沿用系统背景。
      示例：<SolidColorBrush Color="#CC202020" />
  $name: Taskbar background
  $name:zh-CN: 任务栏背景
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Xaml::Markup;
using namespace winrt::Windows::UI::Xaml::Media;

enum class BackgroundStyle {
    custom,
    system,
    transparent,
    translucent,
    solid,
    gradient,
};

struct BackgroundSettings {
    BackgroundStyle style = BackgroundStyle::system;
    std::wstring color = L"#202020";
    int opacity = 75;
    std::wstring gradientColor1 = L"#1F2937";
    std::wstring gradientColor2 = L"#111827";
    int gradientAngle = 45;
    int gradientStartPosition = 0;
    int gradientEndPosition = 100;
    std::wstring customXaml;
};

// Settings are loaded by the mod thread and read by taskbar UI threads.
std::atomic<int> g_settingTaskbarWidth{0};
std::atomic<int> g_settingTaskbarAreaHeight{0};
std::atomic<int> g_settingFloatMargin{0};
std::atomic<int> g_settingCornerRadius{0};
std::atomic<int> g_settingHoverCornerRadius{20};
std::atomic<double> g_hoverRadiusDip{16.0};
std::atomic<double> g_systemTaskbarContentHeightDip{0.0};
std::mutex g_settingsMutex;
BackgroundSettings g_backgroundSettings;

std::atomic<unsigned> g_settingsVersion{1};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_systemTrayDllLoaded{false};
// Counts the hook functions that are currently running, so that the mod can
// wait for them to finish before its code gets unloaded.
std::atomic<int> g_hookCallCounter{0};
std::atomic<bool> g_loggedMissingRootGrid{false};
std::atomic<bool> g_loggedMissingRects{false};
std::atomic<bool> g_loggedMissingTray{false};
std::atomic<bool> g_loggedMissingIsland{false};
std::atomic<bool> g_loggedRegionBusy{false};
std::atomic<bool> g_loggedMissingXamlRoot{false};
std::atomic<bool> g_loggedNoDirectApply{false};
std::atomic<bool> g_loggedStripUnknown{false};

class HookScope {
   public:
    HookScope() {
        g_hookCallCounter.fetch_add(1);
    }

    ~HookScope() {
        g_hookCallCounter.fetch_sub(1);
    }
};

void* g_ctaskBandTaskListWndSiteVftable = nullptr;
void* g_csecondaryTaskBandTaskListWndSiteVftable = nullptr;
void* g_taskbarHostFrameHeight = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original =
    nullptr;
using RefCountDecref_t = void(WINAPI*)(void* pThis);
RefCountDecref_t RefCountDecref_Original = nullptr;

enum class HoverShapeKind {
    Border,
    Grid,
    ContentPresenter,
};

struct SavedHoverShape {
    winrt::weak_ref<FrameworkElement> element;
    HoverShapeKind kind = HoverShapeKind::Border;
    CornerRadius cornerRadius{};
    int64_t callbackToken = 0;
    double originalHeight = std::numeric_limits<double>::quiet_NaN();
    double originalMaxHeight = std::numeric_limits<double>::infinity();
    VerticalAlignment originalAlignment{VerticalAlignment::Stretch};
    Thickness originalMargin{};
    double targetHeight = std::numeric_limits<double>::quiet_NaN();
    bool layoutSaved = false;
    int64_t heightToken = 0;
    int64_t maxHeightToken = 0;
    int64_t alignmentToken = 0;
    int64_t marginToken = 0;
};

struct SavedVerticalLayout {
    winrt::weak_ref<FrameworkElement> element;
    double originalHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment originalAlignment{VerticalAlignment::Stretch};
    Thickness originalMargin{};
    double targetHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment targetAlignment{VerticalAlignment::Center};
    int64_t heightToken = 0;
    int64_t alignmentToken = 0;
    int64_t marginToken = 0;
};

struct SavedMultiWindowHighlight {
    winrt::weak_ref<FrameworkElement> element;
    double opacity = 1.0;
    Visibility visibility{Visibility::Visible};
    bool compositionVisible = true;
    int64_t opacityToken = 0;
    int64_t visibilityToken = 0;
};

struct SavedTaskButtonNameCallback {
    winrt::weak_ref<FrameworkElement> button;
    int64_t token = 0;
};

struct SavedTaskbarToolTip {
    winrt::weak_ref<ToolTip> toolTip;
    CornerRadius originalCornerRadius{};
    DWORD threadId = 0;
};

struct AppliedTaskbarState {
    DWORD threadId = 0;
    unsigned version = 0;
    winrt::weak_ref<FrameworkElement> frame;
    winrt::weak_ref<Grid> rootGrid;
    winrt::weak_ref<FrameworkElement> tray;
    winrt::weak_ref<Shapes::Rectangle> gradientOverlay;
    Brush gradientOverlayOriginalFill = nullptr;
    bool gradientOverlayApplied = false;
    bool trayArranged = false;
    double savedFrameMinWidth = 0;
    double savedFrameMaxWidth = std::numeric_limits<double>::infinity();
    double savedFrameMaxHeight = std::numeric_limits<double>::infinity();
    HorizontalAlignment savedFrameAlignment{HorizontalAlignment::Stretch};
    double savedFrameHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedFrameVerticalAlignment{VerticalAlignment::Stretch};
    Thickness savedFrameMargin{};
    bool frameHeightSet = false;
    bool frameMarginSet = false;
    // The parent of the taskbar frame gets an explicit height while a custom
    // taskbar area height is in use, so remember the original value for a
    // later restore.
    winrt::weak_ref<FrameworkElement> frameParent;
    double savedParentHeight = std::numeric_limits<double>::quiet_NaN();
    bool parentHeightSet = false;
    Thickness savedTrayMargin{};
    double savedTrayHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedTrayVerticalAlignment{VerticalAlignment::Stretch};
    std::vector<SavedHoverShape> savedHoverShapes;
    std::vector<SavedMultiWindowHighlight> savedMultiWindowHighlights;
    std::vector<SavedTaskButtonNameCallback> savedTaskButtonNameCallbacks;
    std::vector<SavedVerticalLayout> savedVerticalLayouts;
    DWORD lastApplyTick = 0;
    HWND taskbarWnd = nullptr;
    HWND islandHwnd = nullptr;
    HWND topLevelWnd = nullptr;
    RECT appliedRegion{};
    bool hasAppliedRegion = false;
    int regionRetries = 0;
};

std::mutex g_appliedMutex;
[[clang::no_destroy]] std::optional<std::vector<AppliedTaskbarState>>
    g_appliedTaskbarStates{std::in_place};
std::vector<SavedTaskbarToolTip> g_savedTaskbarToolTips;

std::atomic<HWND> g_controlCenterWnd{nullptr};
HWINEVENTHOOK g_controlCenterWinEventHook = nullptr;
HANDLE g_controlCenterHookThread = nullptr;
std::atomic<DWORD> g_controlCenterHookThreadId{0};
std::atomic<bool> g_controlCenterHookStop{false};
std::atomic<unsigned long> g_lastControlCenterFixTick{0};
std::atomic<double> g_lastPillWidthPx{0.0};
constexpr double kControlCenterRightInsetDip = 12.8;
std::mutex g_layoutCalibrationMutex;
HANDLE g_layoutCalibrationTimerQueue = nullptr;
bool g_layoutCalibrationScheduled = false;

thread_local HMONITOR g_cachedTaskbarMonitor = nullptr;
thread_local HWND g_cachedTaskbarMonitorWnd = nullptr;

// The mod runs both in the taskbar process (explorer.exe) and in the process
// which hosts the Quick Settings panel (ShellHost.exe on recent Windows
// versions), as the latter is the one that positions the panel.
enum class HostKind {
    Taskbar,
    ControlCenter,
};

HostKind g_hostKind = HostKind::Taskbar;

bool EqualsIgnoreCase(PCWSTR left, PCWSTR right) {
    return _wcsicmp(left, right) == 0;
}

bool IsTaskbarClassName(PCWSTR className) {
    return EqualsIgnoreCase(className, L"Shell_TrayWnd") ||
           EqualsIgnoreCase(className, L"Shell_SecondaryTrayWnd");
}

bool IsControlCenterClassName(PCWSTR className) {
    return EqualsIgnoreCase(className, L"ControlCenterWindow");
}

HostKind GetHostKind() {
    WCHAR executablePath[MAX_PATH];
    if (!GetModuleFileName(nullptr, executablePath,
                           ARRAYSIZE(executablePath))) {
        return HostKind::Taskbar;
    }

    PCWSTR executableName = wcsrchr(executablePath, L'\\');
    executableName = executableName ? executableName + 1 : executablePath;

    if (EqualsIgnoreCase(executableName, L"ShellHost.exe") ||
        EqualsIgnoreCase(executableName, L"ShellExperienceHost.exe")) {
        return HostKind::ControlCenter;
    }

    return HostKind::Taskbar;
}

// A window which is owned by another process can only be moved after it has
// already been shown, which makes it visibly jump. Instead, the taskbar side
// publishes the position that it wants as a property of the panel window, and
// the process which hosts the panel applies it while the window is being
// moved.
constexpr WCHAR kControlCenterTargetRightProp[] =
    L"FloatingTaskbarControlCenterTargetRight";
constexpr WCHAR kControlCenterPillWidthProp[] =
    L"FloatingTaskbarControlCenterPillWidth";

HWND FindTaskbarWindowOnMonitor(HMONITOR monitor);
double GetTaskbarWindowScale(HWND taskbarWnd);

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;

bool IsOwnControlCenterWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return IsControlCenterClassName(className);
}

bool IsTaskbarWindowClass(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return IsTaskbarClassName(className);
}

void EnsureTaskbarWindowHeight(HWND hWnd, int areaHeightPx) {
    if (!hWnd || areaHeightPx <= 0) {
        return;
    }

    RECT rect{};
    if (!GetWindowRect(hWnd, &rect)) {
        return;
    }

    int currentHeight = rect.bottom - rect.top;
    if (currentHeight <= 0 || currentHeight >= areaHeightPx) {
        return;
    }

    int newY = rect.top;
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        int monitorBottom = monitorInfo.rcMonitor.bottom;
        if (rect.bottom >= monitorBottom - 2 &&
            rect.bottom <= monitorBottom + 2) {
            newY = monitorBottom - areaHeightPx;
        }
    }

    SetWindowPos(hWnd, nullptr, rect.left, newY, rect.right - rect.left,
                 areaHeightPx, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    HookScope hookScope;

    if (g_hostKind == HostKind::Taskbar && !g_unloading) {
        const int areaHeightPx = g_settingTaskbarAreaHeight.load();
        if (areaHeightPx > 0 && IsTaskbarWindowClass(hWnd)) {
            RECT rect{};
            GetWindowRect(hWnd, &rect);
            int currentWidth = rect.right - rect.left;
            int currentHeight = rect.bottom - rect.top;
            int requestedWidth =
                (uFlags & SWP_NOSIZE) ? currentWidth : cx;
            int requestedHeight =
                (uFlags & SWP_NOSIZE) ? currentHeight : cy;

            if (currentWidth > 0 && currentHeight > 0 &&
                requestedWidth > 0 && requestedHeight > 0 &&
                requestedHeight < areaHeightPx) {
                int newX = X;
                int newY = Y;
                if (uFlags & SWP_NOMOVE) {
                    newX = rect.left;
                    newY = rect.top;
                } else if (Y == 0 && rect.top != 0) {
                    newY = rect.top;
                }

                MONITORINFO monitorInfo{};
                monitorInfo.cbSize = sizeof(monitorInfo);
                HMONITOR monitor =
                    MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
                    int monitorBottom = monitorInfo.rcMonitor.bottom;
                    int originalBottom = newY + requestedHeight;
                    if (originalBottom >= monitorBottom - 2 &&
                        originalBottom <= monitorBottom + 2) {
                        newY = monitorBottom - areaHeightPx;
                    }
                }

                uFlags &= ~(SWP_NOSIZE | SWP_NOMOVE);
                return SetWindowPos_Original(
                    hWnd, hWndInsertAfter, newX, newY, requestedWidth,
                    areaHeightPx, uFlags);
            }
        }

        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    if (g_hostKind != HostKind::ControlCenter || g_unloading ||
        !IsOwnControlCenterWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    ULONG_PTR targetRightPx =
        (ULONG_PTR)GetPropW(hWnd, kControlCenterTargetRightProp);
    if (!targetRightPx ||
        (uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    ULONG_PTR pillWidthPx =
        (ULONG_PTR)GetPropW(hWnd, kControlCenterPillWidthProp);
    HWND taskbarWnd = FindTaskbarWindowOnMonitor(
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
    if (taskbarWnd && pillWidthPx > 0) {
        RECT taskbarRect{};
        if (GetWindowRect(taskbarWnd, &taskbarRect)) {
            const double scale = GetTaskbarWindowScale(taskbarWnd);
            const double windowWidthPx =
                taskbarRect.right - taskbarRect.left;
            const double clampedPillWidth =
                std::min(static_cast<double>(pillWidthPx), windowWidthPx);
            const double barRightPx =
                taskbarRect.left + (windowWidthPx + clampedPillWidth) / 2.0;
            if (scale > 0) {
                targetRightPx = static_cast<ULONG_PTR>(std::lround(
                    barRightPx + std::lround(kControlCenterRightInsetDip *
                                             scale)));
            }
        }
    }

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    if (uFlags & SWP_NOMOVE) {
        // The window is being resized, adjust its position as well.
        uFlags &= ~SWP_NOMOVE;
        X = rect.left;
        Y = rect.top;
    }

    int width = (uFlags & SWP_NOSIZE) ? rect.right - rect.left : cx;
    if (width <= 0) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    X = static_cast<int>(targetRightPx) - width;

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                 uFlags | SWP_ASYNCWINDOWPOS);
}

BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    HookScope hookScope;

    if (g_hostKind == HostKind::Taskbar && !g_unloading) {
        const int areaHeightPx = g_settingTaskbarAreaHeight.load();
        if (areaHeightPx > 0 && nWidth > 0 && nHeight > 0 &&
            nHeight < areaHeightPx && IsTaskbarWindowClass(hWnd)) {
            int newY = Y;
            MONITORINFO monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            HMONITOR monitor =
                MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
                int monitorBottom = monitorInfo.rcMonitor.bottom;
                if (Y + nHeight >= monitorBottom - 2 &&
                    Y + nHeight <= monitorBottom + 2) {
                    newY = monitorBottom - areaHeightPx;
                }
            }

            return MoveWindow_Original(hWnd, X, newY, nWidth, areaHeightPx,
                                       bRepaint);
        }
    }

    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

BackgroundStyle ParseBackgroundStyle(std::wstring const& value) {
    struct StyleName {
        PCWSTR name;
        BackgroundStyle style;
    };

    constexpr StyleName styleNames[] = {
        {L"system", BackgroundStyle::system},
        {L"transparent", BackgroundStyle::transparent},
        {L"translucent", BackgroundStyle::translucent},
        {L"solid", BackgroundStyle::solid},
        {L"gradient", BackgroundStyle::gradient},
    };

    for (auto const& entry : styleNames) {
        if (EqualsIgnoreCase(value.c_str(), entry.name)) {
            return entry.style;
        }
    }

    return BackgroundStyle::custom;
}

void LoadSettings() {
    auto readString = [](PCWSTR name) {
        WindhawkUtils::StringSetting setting =
            WindhawkUtils::StringSetting::make(name);
        return std::wstring(setting.get());
    };
    auto readNonNegativeInt = [](PCWSTR name) {
        return std::max(Wh_GetIntSetting(name), 0);
    };

    BackgroundSettings background;
    background.style = ParseBackgroundStyle(
        readString(L"taskbarBackgroundBrush.backgroundStyle"));

    background.color = readString(L"taskbarBackgroundBrush.backgroundColor");
    background.opacity = std::clamp(
        Wh_GetIntSetting(L"taskbarBackgroundBrush.backgroundOpacity"), 0, 100);
    background.gradientColor1 = readString(L"taskbarBackgroundBrush.gradientColor1");
    background.gradientColor2 = readString(L"taskbarBackgroundBrush.gradientColor2");
    background.gradientAngle =
        Wh_GetIntSetting(L"taskbarBackgroundBrush.gradientAngle");
    background.gradientStartPosition = std::clamp(
        Wh_GetIntSetting(L"taskbarBackgroundBrush.gradientStartPosition"), 0, 100);
    background.gradientEndPosition = std::clamp(
        Wh_GetIntSetting(L"taskbarBackgroundBrush.gradientEndPosition"), 0, 100);
    background.customXaml = readString(L"taskbarBackgroundBrush.backgroundBrush");

    const int taskbarWidth = readNonNegativeInt(L"floatingBar.taskbarWidth");
    const int taskbarAreaHeight =
        readNonNegativeInt(L"floatingBar.taskbarAreaHeight");
    const int floatMargin = readNonNegativeInt(L"floatingBar.floatMargin");
    const int cornerRadius = readNonNegativeInt(L"floatingBar.cornerRadius");
    const int hoverCornerRadius =
        std::clamp(Wh_GetIntSetting(L"floatingBar.hoverCornerRadius"), 0, 1000);

    g_settingTaskbarWidth = taskbarWidth;
    g_settingTaskbarAreaHeight = taskbarAreaHeight;
    g_settingFloatMargin = floatMargin;
    g_settingCornerRadius = cornerRadius;
    g_settingHoverCornerRadius = hoverCornerRadius;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_backgroundSettings = std::move(background);
    }
}

void ResetLoggedFlags() {
    g_loggedMissingRootGrid = false;
    g_loggedMissingRects = false;
    g_loggedMissingTray = false;
    g_loggedMissingIsland = false;
    g_loggedRegionBusy = false;
    g_loggedMissingXamlRoot = false;
    g_loggedNoDirectApply = false;
    g_loggedStripUnknown = false;
}

std::wstring_view AsWStringView(winrt::hstring const& value) {
    return {value.c_str(), value.size()};
}

bool ClassNameEquals(winrt::hstring const& className,
                     std::wstring_view expectedClassName) {
    return AsWStringView(className) == expectedClassName;
}

bool ClassNameEndsWith(winrt::hstring const& className,
                       std::wstring_view suffix) {
    const std::wstring_view name = AsWStringView(className);
    if (!name.ends_with(suffix)) {
        return false;
    }

    const size_t suffixOffset = name.size() - suffix.size();
    return suffixOffset == 0 || name[suffixOffset - 1] == L'.';
}

bool ElementNameEquals(FrameworkElement const& element,
                       std::wstring_view expectedName) {
    return AsWStringView(element.Name()) == expectedName;
}

FrameworkElement FindChildByPredicate(
    FrameworkElement const& root,
    std::function<bool(FrameworkElement const&)> const& predicate) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (predicate(child)) {
            return child;
        }

        FrameworkElement found = FindChildByPredicate(child, predicate);
        if (found) {
            return found;
        }
    }

    return nullptr;
}

CornerRadius GetHoverShapeRadius(FrameworkElement const& element,
                                 HoverShapeKind kind) {
    try {
        switch (kind) {
        case HoverShapeKind::Border:
            if (auto border = element.try_as<Border>()) {
                return border.CornerRadius();
            }
            break;
        case HoverShapeKind::Grid:
            if (auto grid = element.try_as<Grid>()) {
                return grid.CornerRadius();
            }
            break;
        case HoverShapeKind::ContentPresenter:
            if (auto presenter = element.try_as<ContentPresenter>()) {
                return presenter.CornerRadius();
            }
            break;
        }
    } catch (...) {
    }
    return CornerRadius{};
}

void SetHoverShapeRadius(FrameworkElement const& element,
                         HoverShapeKind kind,
                         CornerRadius radius) {
    try {
        switch (kind) {
        case HoverShapeKind::Border:
            if (auto border = element.try_as<Border>()) {
                border.CornerRadius(radius);
            }
            break;
        case HoverShapeKind::Grid:
            if (auto grid = element.try_as<Grid>()) {
                grid.CornerRadius(radius);
            }
            break;
        case HoverShapeKind::ContentPresenter:
            if (auto presenter = element.try_as<ContentPresenter>()) {
                presenter.CornerRadius(radius);
            }
            break;
        }
    } catch (...) {
    }
}

// Taskbar tooltips use an 8 DIP outer corner. The standard ToolTip template
// forwards CornerRadius to ContentPresenter#LayoutRoot through TemplateBinding.
constexpr double kTaskbarToolTipCornerRadiusDip = 8.0;

void SaveAndApplyTaskbarToolTipCornerRadius(ToolTip const& toolTip) {
    if (!toolTip) {
        return;
    }

    bool alreadySaved = false;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto it = g_savedTaskbarToolTips.begin();
             it != g_savedTaskbarToolTips.end();) {
            auto savedToolTip = it->toolTip.get();
            if (!savedToolTip) {
                it = g_savedTaskbarToolTips.erase(it);
                continue;
            }
            if (savedToolTip == toolTip) {
                alreadySaved = true;
                break;
            }
            ++it;
        }

        if (!alreadySaved) {
            SavedTaskbarToolTip saved;
            saved.toolTip = winrt::make_weak(toolTip);
            saved.originalCornerRadius = toolTip.CornerRadius();
            saved.threadId = GetCurrentThreadId();
            g_savedTaskbarToolTips.push_back(std::move(saved));
        }
    }

    const double radius = kTaskbarToolTipCornerRadiusDip;
    CornerRadius current = toolTip.CornerRadius();
    if (std::abs(current.TopLeft - radius) > 0.05 ||
        std::abs(current.TopRight - radius) > 0.05 ||
        std::abs(current.BottomRight - radius) > 0.05 ||
        std::abs(current.BottomLeft - radius) > 0.05) {
        toolTip.CornerRadius(
            CornerRadius{radius, radius, radius, radius});
    }
}

void ApplyTaskbarToolTipCornerRadius(ToolTip const& toolTip) {
    if (!toolTip) {
        return;
    }

    try {
        SaveAndApplyTaskbarToolTipCornerRadius(toolTip);
    } catch (...) {
    }
}

void ApplyTaskbarToolTipCornerRadiiRecursive(
    FrameworkElement const& root,
    int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    try {
        auto attachedToolTip = ToolTipService::GetToolTip(root);
        if (auto toolTip = attachedToolTip.try_as<ToolTip>()) {
            ApplyTaskbarToolTipCornerRadius(toolTip);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }
        if (child) {
            ApplyTaskbarToolTipCornerRadiiRecursive(child, depth + 1);
        }
    }
}

void ApplyOpenTaskbarToolTipCornerRadii(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return;
    }

    try {
        for (auto const& popup :
             VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot)) {
            auto child =
                popup.Child().try_as<FrameworkElement>();
            if (!child) {
                continue;
            }

            if (auto toolTip = child.try_as<ToolTip>()) {
                ApplyTaskbarToolTipCornerRadius(toolTip);
                continue;
            }

            FrameworkElement toolTipElement = FindChildByPredicate(
                child, [](FrameworkElement const& element) {
                    return element.try_as<ToolTip>() != nullptr;
                });
            if (toolTipElement) {
                ApplyTaskbarToolTipCornerRadius(
                    toolTipElement.try_as<ToolTip>());
            }
        }
    } catch (...) {
    }
}

void RestoreTaskbarToolTipCornerRadiiOnCurrentThread() {
    const DWORD threadId = GetCurrentThreadId();
    std::vector<SavedTaskbarToolTip> pending;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto it = g_savedTaskbarToolTips.begin();
             it != g_savedTaskbarToolTips.end();) {
            if (it->threadId == threadId) {
                pending.push_back(std::move(*it));
                it = g_savedTaskbarToolTips.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto const& saved : pending) {
        if (auto toolTip = saved.toolTip.get()) {
            try {
                toolTip.CornerRadius(saved.originalCornerRadius);
            } catch (...) {
            }
        }
    }
}

DependencyProperty GetHoverShapeRadiusProperty(HoverShapeKind kind) {
    switch (kind) {
    case HoverShapeKind::Border:
        return Border::CornerRadiusProperty();
    case HoverShapeKind::Grid:
        return Grid::CornerRadiusProperty();
    case HoverShapeKind::ContentPresenter:
        return ContentPresenter::CornerRadiusProperty();
    }
    return nullptr;
}

bool NearlyEqual(double a, double b, double tolerance = 0.05);

void UnregisterTrayHoverBoxCallbacks(SavedHoverShape& saved) {
    auto element = saved.element.get();
    if (!element) {
        return;
    }

    try {
        if (saved.heightToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::HeightProperty(), saved.heightToken);
            saved.heightToken = 0;
        }
        if (saved.maxHeightToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::MaxHeightProperty(),
                saved.maxHeightToken);
            saved.maxHeightToken = 0;
        }
        if (saved.alignmentToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                saved.alignmentToken);
            saved.alignmentToken = 0;
        }
        if (saved.marginToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::MarginProperty(), saved.marginToken);
            saved.marginToken = 0;
        }
    } catch (...) {
    }
}

void ApplyTrayHoverBoxTarget(SavedHoverShape const& saved) {
    auto element = saved.element.get();
    if (!element || std::isnan(saved.targetHeight)) {
        return;
    }

    try {
        if (!NearlyEqual(element.Height(), saved.targetHeight)) {
            element.Height(saved.targetHeight);
        }
        if (!NearlyEqual(element.MaxHeight(), saved.targetHeight)) {
            element.MaxHeight(saved.targetHeight);
        }
        if (element.VerticalAlignment() != VerticalAlignment::Center) {
            element.VerticalAlignment(VerticalAlignment::Center);
        }

        Thickness margin = element.Margin();
        if (std::abs(margin.Top) > 0.05 ||
            std::abs(margin.Bottom) > 0.05) {
            margin.Top = 0;
            margin.Bottom = 0;
            element.Margin(margin);
        }
    } catch (...) {
    }
}

void RegisterTrayHoverBoxCallbacks(SavedHoverShape& saved) {
    auto element = saved.element.get();
    if (!element || std::isnan(saved.targetHeight)) {
        return;
    }

    const double targetHeight = saved.targetHeight;
    try {
        saved.heightToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::HeightProperty(),
            [targetHeight](DependencyObject const& sender,
                           DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    !NearlyEqual(element.Height(), targetHeight)) {
                    element.Height(targetHeight);
                }
            });
        saved.maxHeightToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::MaxHeightProperty(),
            [targetHeight](DependencyObject const& sender,
                           DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    !NearlyEqual(element.MaxHeight(), targetHeight)) {
                    element.MaxHeight(targetHeight);
                }
            });
        saved.alignmentToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::VerticalAlignmentProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    element.VerticalAlignment() !=
                        VerticalAlignment::Center) {
                    element.VerticalAlignment(VerticalAlignment::Center);
                }
            });
        saved.marginToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::MarginProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (!element) {
                    return;
                }
                Thickness margin = element.Margin();
                if (std::abs(margin.Top) > 0.05 ||
                    std::abs(margin.Bottom) > 0.05) {
                    margin.Top = 0;
                    margin.Bottom = 0;
                    element.Margin(margin);
                }
            });
    } catch (...) {
    }
}

void SaveAndApplyTrayHoverBoxLayout(SavedHoverShape& saved,
                                    FrameworkElement const& element,
                                    double height) {
    if (!element || !(height > 0)) {
        return;
    }

    try {
        if (!saved.layoutSaved) {
            saved.originalHeight = element.Height();
            saved.originalMaxHeight = element.MaxHeight();
            saved.originalAlignment = element.VerticalAlignment();
            saved.originalMargin = element.Margin();
            saved.layoutSaved = true;
        }
    } catch (...) {
        return;
    }

    UnregisterTrayHoverBoxCallbacks(saved);
    saved.targetHeight = height;
    RegisterTrayHoverBoxCallbacks(saved);
    ApplyTrayHoverBoxTarget(saved);
}

bool IsHoverBackgroundElement(FrameworkElement const& element) {
    return ElementNameEquals(element, L"BackgroundElement") ||
           ElementNameEquals(element, L"BackgroundBorder");
}

void HideMultiWindowHighlight(AppliedTaskbarState& record,
                              FrameworkElement const& element) {
    bool alreadySaved = false;
    for (auto const& saved : record.savedMultiWindowHighlights) {
        if (saved.element.get() == element) {
            alreadySaved = true;
            break;
        }
    }

    if (!alreadySaved) {
        SavedMultiWindowHighlight saved;
        saved.element = winrt::make_weak(element);
        try {
            saved.opacity = element.Opacity();
            saved.visibility = element.Visibility();
            saved.compositionVisible =
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible();
        } catch (...) {
            return;
        }

        saved.opacityToken = element.RegisterPropertyChangedCallback(
            UIElement::OpacityProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                if (g_unloading) {
                    return;
                }
                auto element = sender.try_as<FrameworkElement>();
                if (element && element.Opacity() != 0.0) {
                    element.Opacity(0.0);
                }
            });
        saved.visibilityToken = element.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                if (g_unloading) {
                    return;
                }
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    element.Visibility() != Visibility::Collapsed) {
                    element.Visibility(Visibility::Collapsed);
                }
            });

        record.savedMultiWindowHighlights.push_back(std::move(saved));
    }

    try {
        ElementCompositionPreview::GetElementVisual(element).IsVisible(false);
        element.Opacity(0.0);
        element.Visibility(Visibility::Collapsed);
    } catch (...) {
    }
}

int GetTaskButtonWindowCount(FrameworkElement const& button) {
    try {
        const auto automationName = AutomationProperties::GetName(button);
        const std::wstring_view text = AsWStringView(automationName);
        size_t windowCountMarker = text.find(L"个运行窗口");
        if (windowCountMarker == std::wstring_view::npos) {
            windowCountMarker = text.find(L"running window");
        }

        size_t numberEnd = windowCountMarker == std::wstring_view::npos
                               ? text.size()
                               : windowCountMarker;
        while (numberEnd > 0 &&
               (text[numberEnd - 1] == L' ' ||
                text[numberEnd - 1] == L'-' ||
                text[numberEnd - 1] == L':')) {
            --numberEnd;
        }

        const size_t numberDigitsEnd = numberEnd;
        while (numberEnd > 0 && text[numberEnd - 1] >= L'0' &&
               text[numberEnd - 1] <= L'9') {
            --numberEnd;
        }
        if (numberDigitsEnd > numberEnd) {
            unsigned count = 0;
            for (size_t i = numberEnd; i < numberDigitsEnd; ++i) {
                const unsigned digit =
                    static_cast<unsigned>(text[i] - L'0');
                if (count >
                    (static_cast<unsigned>(std::numeric_limits<int>::max()) -
                     digit) /
                        10) {
                    return 0;
                }
                count = count * 10 + digit;
            }
            return static_cast<int>(count);
        }
    } catch (...) {
    }
    return 0;
}

void RefreshTaskButtonVisuals(FrameworkElement const& button);

void EnsureTaskButtonNameCallback(AppliedTaskbarState& record,
                                  FrameworkElement const& button) {
    for (auto const& saved : record.savedTaskButtonNameCallbacks) {
        if (saved.button.get() == button) {
            return;
        }
    }

    SavedTaskButtonNameCallback saved;
    saved.button = winrt::make_weak(button);
    try {
        saved.token = button.RegisterPropertyChangedCallback(
            AutomationProperties::NameProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                if (auto changedButton =
                        sender.try_as<FrameworkElement>()) {
                    RefreshTaskButtonVisuals(changedButton);
                }
            });
    } catch (...) {
        return;
    }
    record.savedTaskButtonNameCallbacks.push_back(std::move(saved));
}

void HideMultiWindowVisualsRecursive(FrameworkElement const& root,
                                     AppliedTaskbarState& record,
                                     int depth = 0,
                                     int multiWindowDepth = -1) {
    if (!root || depth > 40) {
        return;
    }

    bool isTaskButton = false;
    try {
        isTaskButton =
            ClassNameEquals(winrt::get_class_name(root),
                            L"Taskbar.TaskListButton");
    } catch (...) {
    }

    if (isTaskButton) {
        EnsureTaskButtonNameCallback(record, root);
        if (GetTaskButtonWindowCount(root) > 1) {
            multiWindowDepth = 0;
        }
    }

    const bool hideElement =
        ElementNameEquals(root, L"MultiWindowElement") ||
        (multiWindowDepth >= 0 && multiWindowDepth <= 20 &&
         IsHoverBackgroundElement(root));
    if (hideElement) {
        HideMultiWindowHighlight(record, root);
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }
        if (child) {
            HideMultiWindowVisualsRecursive(
                child, record, depth + 1,
                multiWindowDepth >= 0 ? multiWindowDepth + 1 : -1);
        }
    }
}

void RestoreMultiWindowHighlights(
    std::vector<SavedMultiWindowHighlight> const& savedHighlights) {
    for (auto const& saved : savedHighlights) {
        if (auto element = saved.element.get()) {
            try {
                if (saved.opacityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::OpacityProperty(), saved.opacityToken);
                }
                if (saved.visibilityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::VisibilityProperty(),
                        saved.visibilityToken);
                }
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible(saved.compositionVisible);
                element.Opacity(saved.opacity);
                element.Visibility(saved.visibility);
            } catch (...) {
            }
        }
    }
}

void RestoreTaskButtonNameCallbacks(
    std::vector<SavedTaskButtonNameCallback> const& savedCallbacks) {
    for (auto const& saved : savedCallbacks) {
        if (auto button = saved.button.get()) {
            try {
                if (saved.token) {
                    button.UnregisterPropertyChangedCallback(
                        AutomationProperties::NameProperty(), saved.token);
                }
            } catch (...) {
            }
        }
    }
}

bool NearlyEqual(double a, double b, double tolerance) {
    if (std::isnan(a) || std::isnan(b)) {
        return std::isnan(a) && std::isnan(b);
    }
    return std::abs(a - b) <= tolerance;
}

void UnregisterVerticalLayoutCallbacks(SavedVerticalLayout& saved) {
    auto element = saved.element.get();
    if (!element) {
        return;
    }

    try {
        if (saved.heightToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::HeightProperty(), saved.heightToken);
            saved.heightToken = 0;
        }
        if (saved.alignmentToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                saved.alignmentToken);
            saved.alignmentToken = 0;
        }
        if (saved.marginToken) {
            element.UnregisterPropertyChangedCallback(
                FrameworkElement::MarginProperty(), saved.marginToken);
            saved.marginToken = 0;
        }
    } catch (...) {
    }
}

void RegisterVerticalLayoutCallbacks(SavedVerticalLayout& saved) {
    auto element = saved.element.get();
    if (!element) {
        return;
    }

    const double targetHeight = saved.targetHeight;
    const VerticalAlignment targetAlignment = saved.targetAlignment;
    try {
        saved.heightToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::HeightProperty(),
            [targetHeight](DependencyObject const& sender,
                           DependencyProperty const&) {
                if (std::isnan(targetHeight)) {
                    return;
                }
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    !NearlyEqual(element.Height(), targetHeight)) {
                    element.Height(targetHeight);
                }
            });

        saved.alignmentToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::VerticalAlignmentProperty(),
            [targetAlignment](DependencyObject const& sender,
                              DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (element &&
                    element.VerticalAlignment() != targetAlignment) {
                    element.VerticalAlignment(targetAlignment);
                }
            });

        saved.marginToken = element.RegisterPropertyChangedCallback(
            FrameworkElement::MarginProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (!element) {
                    return;
                }
                Thickness margin = element.Margin();
                if (std::abs(margin.Top) > 0.05 ||
                    std::abs(margin.Bottom) > 0.05) {
                    margin.Top = 0;
                    margin.Bottom = 0;
                    element.Margin(margin);
                }
            });
    } catch (...) {
    }
}

void ApplyVerticalLayoutTarget(SavedVerticalLayout const& saved) {
    auto element = saved.element.get();
    if (!element) {
        return;
    }

    try {
        if (!std::isnan(saved.targetHeight) &&
            !NearlyEqual(element.Height(), saved.targetHeight)) {
            element.Height(saved.targetHeight);
        }
        if (element.VerticalAlignment() != saved.targetAlignment) {
            element.VerticalAlignment(saved.targetAlignment);
        }

        Thickness margin = element.Margin();
        if (std::abs(margin.Top) > 0.05 ||
            std::abs(margin.Bottom) > 0.05) {
            margin.Top = 0;
            margin.Bottom = 0;
            element.Margin(margin);
        }
    } catch (...) {
    }
}

void SaveAndApplyVerticalLayout(std::vector<SavedVerticalLayout>& layouts,
                                FrameworkElement const& element,
                                double targetHeight,
                                VerticalAlignment targetAlignment) {
    if (!element) {
        return;
    }

    for (auto& saved : layouts) {
        if (saved.element.get() == element) {
            const bool bothTargetHeightsNaN =
                std::isnan(saved.targetHeight) && std::isnan(targetHeight);
            const bool heightTargetChanged =
                !bothTargetHeightsNaN &&
                saved.targetHeight != targetHeight;
            const bool alignmentTargetChanged =
                saved.targetAlignment != targetAlignment;
            if (heightTargetChanged || alignmentTargetChanged) {
                UnregisterVerticalLayoutCallbacks(saved);
                saved.targetHeight = targetHeight;
                saved.targetAlignment = targetAlignment;
                RegisterVerticalLayoutCallbacks(saved);
            }
            ApplyVerticalLayoutTarget(saved);
            return;
        }
    }

    SavedVerticalLayout saved;
    saved.element = winrt::make_weak(element);
    try {
        saved.originalHeight = element.Height();
        saved.originalAlignment = element.VerticalAlignment();
        saved.originalMargin = element.Margin();
        saved.targetHeight = targetHeight;
        saved.targetAlignment = targetAlignment;
    } catch (...) {
    }
    RegisterVerticalLayoutCallbacks(saved);

    layouts.push_back(std::move(saved));
    ApplyVerticalLayoutTarget(layouts.back());
}

void RestoreVerticalLayouts(
    std::vector<SavedVerticalLayout> const& layouts) {
    for (auto const& saved : layouts) {
        if (auto element = saved.element.get()) {
            try {
                SavedVerticalLayout layoutCopy = saved;
                UnregisterVerticalLayoutCallbacks(layoutCopy);
                element.Height(saved.originalHeight);
                element.VerticalAlignment(saved.originalAlignment);
                element.Margin(saved.originalMargin);
            } catch (...) {
            }
        }
    }
}

double GetSystemTaskbarContentHeightDip() {
    double height = g_systemTaskbarContentHeightDip.load();
    return height > 0 ? height : 48.0;
}

double GetCenteredContentHeightDip(double barHeight) {
    return std::max(
        1.0, std::min(barHeight, GetSystemTaskbarContentHeightDip()));
}

void ApplyVerticalCenteringRecursive(FrameworkElement const& root,
                                     AppliedTaskbarState& record,
                                     int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    try {
        const bool namedRow =
            ElementNameEquals(root, L"MainStack") ||
            ElementNameEquals(root, L"NonActivatableStack") ||
            ElementNameEquals(root, L"ControlCenterButton") ||
            ElementNameEquals(root, L"NotificationCenterButton") ||
            ElementNameEquals(root, L"ShowDesktopStack");
        const bool omniButton =
            ClassNameEndsWith(winrt::get_class_name(root), L"OmniButton");
        if (namedRow || omniButton) {
            SaveAndApplyVerticalLayout(
                record.savedVerticalLayouts, root,
                std::numeric_limits<double>::quiet_NaN(),
                VerticalAlignment::Center);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }
        if (child) {
            ApplyVerticalCenteringRecursive(child, record, depth + 1);
        }
    }
}

void ApplyCenteredTrayContent(AppliedTaskbarState& record,
                              FrameworkElement const& trayElement,
                              double barHeight) {
    if (!trayElement || !(barHeight > 0)) {
        return;
    }

    FrameworkElement trayFrameGrid = FindChildByPredicate(
        trayElement, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"SystemTrayFrameGrid");
        });
    if (trayFrameGrid) {
        SaveAndApplyVerticalLayout(
            record.savedVerticalLayouts, trayFrameGrid, barHeight,
            VerticalAlignment::Stretch);
    }
    ApplyVerticalCenteringRecursive(trayElement, record);
}

void ApplyCenteredTaskbarContent(AppliedTaskbarState& record,
                                 FrameworkElement const& frame,
                                 FrameworkElement const& trayElement,
                                 double barHeight) {
    if (!(barHeight > 0)) {
        return;
    }

    FrameworkElement iconRepeater = FindChildByPredicate(
        frame, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"TaskbarFrameRepeater");
        });
    if (iconRepeater) {
        SaveAndApplyVerticalLayout(
            record.savedVerticalLayouts, iconRepeater,
            GetCenteredContentHeightDip(barHeight),
            VerticalAlignment::Center);
    }

    ApplyVerticalCenteringRecursive(frame, record);

    ApplyCenteredTrayContent(record, trayElement, barHeight);
}

void RestoreHoverShapes(std::vector<SavedHoverShape> const& savedShapes) {
    for (auto const& saved : savedShapes) {
        if (auto element = saved.element.get()) {
            try {
                if (saved.layoutSaved) {
                    SavedHoverShape layoutCopy = saved;
                    UnregisterTrayHoverBoxCallbacks(layoutCopy);
                    element.Height(saved.originalHeight);
                    element.MaxHeight(saved.originalMaxHeight);
                    element.VerticalAlignment(saved.originalAlignment);
                    element.Margin(saved.originalMargin);
                }

                DependencyProperty property =
                    GetHoverShapeRadiusProperty(saved.kind);
                if (property && saved.callbackToken) {
                    element.UnregisterPropertyChangedCallback(
                        property, saved.callbackToken);
                }
                SetHoverShapeRadius(element, saved.kind,
                                    saved.cornerRadius);
            } catch (...) {
            }
        }
    }
}

void ApplyHoverShapesRecursive(FrameworkElement const& root,
                               AppliedTaskbarState& record,
                               double radius,
                               bool trayContext = false,
                               int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    if (IsHoverBackgroundElement(root)) {
        HoverShapeKind kind;
        bool supported = false;
        if (root.try_as<Border>()) {
            kind = HoverShapeKind::Border;
            supported = true;
        } else if (root.try_as<Grid>()) {
            kind = HoverShapeKind::Grid;
            supported = true;
        } else if (root.try_as<ContentPresenter>()) {
            kind = HoverShapeKind::ContentPresenter;
            supported = true;
        }

        if (supported) {
            if (!trayContext) {
                // The left taskbar row keeps its existing behavior. Its
                // auto-sized highlight must not be forced visible.
                SaveAndApplyVerticalLayout(
                    record.savedVerticalLayouts, root,
                    std::numeric_limits<double>::quiet_NaN(),
                    VerticalAlignment::Center);
            }

            SavedHoverShape* savedShape = nullptr;
            for (auto& saved : record.savedHoverShapes) {
                if (saved.element.get() == root) {
                    savedShape = &saved;
                    break;
                }
            }

            if (!savedShape) {
                SavedHoverShape saved;
                saved.element = winrt::make_weak(root);
                saved.kind = kind;
                saved.cornerRadius = GetHoverShapeRadius(root, kind);

                DependencyProperty property =
                    GetHoverShapeRadiusProperty(kind);
                if (property) {
                    saved.callbackToken =
                        root.RegisterPropertyChangedCallback(
                            property,
                            [kind](DependencyObject const& sender,
                                   DependencyProperty const&) {
                                if (g_unloading) {
                                    return;
                                }
                                auto element =
                                    sender.try_as<FrameworkElement>();
                                if (!element) {
                                    return;
                                }

                                double target = g_hoverRadiusDip.load();
                                CornerRadius current =
                                    GetHoverShapeRadius(element, kind);
                                if (std::abs(current.TopLeft - target) >
                                    0.05) {
                                    SetHoverShapeRadius(
                                        element, kind,
                                        CornerRadius{
                                            target, target, target, target});
                                }
                            });
                }

                record.savedHoverShapes.push_back(std::move(saved));
                savedShape = &record.savedHoverShapes.back();
            }

            if (trayContext) {
                double hoverHeight =
                    std::max(1.0,
                             GetSystemTaskbarContentHeightDip() - 14.0);
                SaveAndApplyTrayHoverBoxLayout(*savedShape, root,
                                               hoverHeight);
            }

            SetHoverShapeRadius(
                root, kind, CornerRadius{radius, radius, radius, radius});
        }
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }
        if (child) {
            ApplyHoverShapesRecursive(child, record, radius, trayContext,
                                      depth + 1);
        }
    }
}

void CollectBackgroundRects(FrameworkElement const& root,
                            std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        auto className = winrt::get_class_name(child);
        if (ClassNameEquals(className, L"Taskbar.TaskbarBackground") &&
            ElementNameEquals(child, L"HoverFlyoutBackgroundControl")) {
            continue;
        }

        if (ClassNameEndsWith(className, L"Rectangle") &&
            (ElementNameEquals(child, L"BackgroundFill") ||
             ElementNameEquals(child, L"BackgroundStroke"))) {
            result.push_back(child);
        }

        CollectBackgroundRects(child, result);
    }
}

Grid FindRootGrid(FrameworkElement const& frame) {
    FrameworkElement rootElement = FindChildByPredicate(
        frame, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"RootGrid");
        });
    return rootElement ? rootElement.try_as<Grid>() : nullptr;
}

Brush FindOriginalBackgroundBrush(
    std::vector<FrameworkElement> const& backgroundRects) {
    for (auto const& element : backgroundRects) {
        if (ElementNameEquals(element, L"BackgroundFill")) {
            if (auto rect = element.try_as<Shapes::Rectangle>()) {
                return rect.Fill();
            }
        }
    }

    return nullptr;
}

int HexDigitValue(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') {
        return ch - L'0';
    }
    if (ch >= L'a' && ch <= L'f') {
        return ch - L'a' + 10;
    }
    if (ch >= L'A' && ch <= L'F') {
        return ch - L'A' + 10;
    }
    return -1;
}

bool ParseHexColor(std::wstring_view value,
                   unsigned& rgb,
                   unsigned& alpha,
                   bool& hasAlpha) {
    if (!value.empty() && value.front() == L'#') {
        value.remove_prefix(1);
    }
    if (value.size() != 6 && value.size() != 8) {
        return false;
    }

    unsigned parsed = 0;
    for (wchar_t ch : value) {
        int digit = HexDigitValue(ch);
        if (digit < 0) {
            return false;
        }
        parsed = (parsed << 4) | static_cast<unsigned>(digit);
    }

    if (value.size() == 6) {
        rgb = parsed;
        alpha = 0xFF;
        hasAlpha = false;
    } else {
        alpha = (parsed >> 24) & 0xFF;
        rgb = parsed & 0xFFFFFF;
        hasAlpha = true;
    }
    return true;
}

struct ParsedBackgroundColor {
    unsigned rgb = 0x202020;
    unsigned alpha = 0xFF;
    bool hasAlpha = false;
};

ParsedBackgroundColor ParseBackgroundColor(std::wstring_view value,
                                           std::wstring_view fallback) {
    ParsedBackgroundColor result;
    unsigned alpha = 0xFF;
    bool hasAlpha = false;
    if (ParseHexColor(value, result.rgb, alpha, hasAlpha)) {
        result.alpha = alpha;
        result.hasAlpha = hasAlpha;
        return result;
    }
    if (ParseHexColor(fallback, result.rgb, alpha, hasAlpha)) {
        result.alpha = alpha;
        result.hasAlpha = hasAlpha;
        return result;
    }
    return result;
}

winrt::Windows::UI::Color MakeColor(unsigned rgb, unsigned alpha) {
    return winrt::Windows::UI::Color{
        static_cast<uint8_t>(alpha & 0xFF),
        static_cast<uint8_t>((rgb >> 16) & 0xFF),
        static_cast<uint8_t>((rgb >> 8) & 0xFF),
        static_cast<uint8_t>(rgb & 0xFF)};
}

unsigned AlphaFromOpacity(int opacityPercent) {
    return static_cast<unsigned>(
        std::lround(std::clamp(opacityPercent, 0, 100) * 255.0 / 100.0));
}

std::wstring EnsureXamlNamespace(std::wstring xaml) {
    size_t firstTag = xaml.find(L'<');
    if (firstTag == std::wstring::npos) {
        return xaml;
    }

    size_t firstTagClose = xaml.find(L'>', firstTag);
    if (firstTagClose == std::wstring::npos) {
        return xaml;
    }

    size_t xmlnsPos = xaml.find(L"xmlns", firstTag);
    if (xmlnsPos != std::wstring::npos && xmlnsPos < firstTagClose) {
        return xaml;
    }

    size_t insertPos = xaml.find_first_of(L" \t\r\n/>", firstTag + 1);
    if (insertPos == std::wstring::npos || insertPos > firstTagClose) {
        insertPos = firstTagClose;
    }
    xaml.insert(insertPos,
                L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
                L"presentation\"");
    return xaml;
}

Brush CreateBackgroundBrush(BackgroundSettings const& settings) {
    try {
        switch (settings.style) {
        case BackgroundStyle::custom: {
            if (settings.customXaml.empty()) {
                return nullptr;
            }
            try {
                return XamlReader::Load(winrt::hstring(
                                            EnsureXamlNamespace(
                                                settings.customXaml)))
                    .try_as<Brush>();
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Failed to parse custom background brush: %08X",
                       static_cast<unsigned int>(ex.code()));
                return nullptr;
            }
        }

        case BackgroundStyle::system:
            return nullptr;

        case BackgroundStyle::transparent: {
            SolidColorBrush brush;
            brush.Color(MakeColor(0, 0));
            return brush;
        }

        case BackgroundStyle::solid: {
            ParsedBackgroundColor color =
                ParseBackgroundColor(settings.color, L"#202020");
            SolidColorBrush brush;
            brush.Color(MakeColor(
                color.rgb, color.hasAlpha ? color.alpha : 0xFF));
            return brush;
        }

        case BackgroundStyle::translucent: {
            ParsedBackgroundColor color =
                ParseBackgroundColor(settings.color, L"#202020");
            SolidColorBrush brush;
            brush.Color(MakeColor(color.rgb,
                                  AlphaFromOpacity(settings.opacity)));
            return brush;
        }

        case BackgroundStyle::gradient: {
            ParsedBackgroundColor color1 =
                ParseBackgroundColor(settings.gradientColor1, L"#1F2937");
            ParsedBackgroundColor color2 =
                ParseBackgroundColor(settings.gradientColor2, L"#111827");
            unsigned alpha = AlphaFromOpacity(settings.opacity);

            LinearGradientBrush brush;
            GradientStop stop1;
            stop1.Color(MakeColor(color1.rgb, alpha));
            stop1.Offset(
                std::clamp(settings.gradientStartPosition, 0, 100) / 100.0);
            GradientStop stop2;
            stop2.Color(MakeColor(color2.rgb, alpha));
            stop2.Offset(
                std::clamp(settings.gradientEndPosition, 0, 100) / 100.0);
            brush.GradientStops().Append(stop1);
            brush.GradientStops().Append(stop2);

            double angle =
                settings.gradientAngle * 3.14159265358979323846 / 180.0;
            double dx = std::cos(angle);
            double dy = std::sin(angle);
            brush.StartPoint(winrt::Windows::Foundation::Point{
                static_cast<float>(0.5 - dx / 2),
                static_cast<float>(0.5 - dy / 2)});
            brush.EndPoint(winrt::Windows::Foundation::Point{
                static_cast<float>(0.5 + dx / 2),
                static_cast<float>(0.5 + dy / 2)});
            return brush;
        }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create background brush (style=%d): %08X",
               static_cast<int>(settings.style),
               static_cast<unsigned int>(ex.code()));
    } catch (...) {
    }

    return nullptr;
}

struct TaskbarWindowSearchContext {
    DWORD threadId;
    HWND result;
};

BOOL CALLBACK FindTaskbarWindowEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<TaskbarWindowSearchContext*>(lParam);

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (!IsTaskbarClassName(className)) {
        return TRUE;
    }

    if (GetWindowThreadProcessId(hWnd, nullptr) != context->threadId) {
        return TRUE;
    }

    context->result = hWnd;
    return FALSE;
}

HWND FindTaskbarWindowForCurrentThread() {
    TaskbarWindowSearchContext context{GetCurrentThreadId(), nullptr};
    EnumWindows(FindTaskbarWindowEnumProc, reinterpret_cast<LPARAM>(&context));
    return context.result;
}

XamlRoot GetXamlRootForTaskbarWindow(HWND taskbarWnd);
HWND FindTaskbarWindowForXamlRoot(XamlRoot const& xamlRoot);
HWND FindTaskbarWindowForFrame(FrameworkElement const& frame);
FrameworkElement FindTaskbarFrameInXamlRoot(XamlRoot const& xamlRoot);

double GetTaskbarWindowScale(HWND taskbarWnd) {
    if (taskbarWnd) {
        UINT dpi = GetDpiForWindow(taskbarWnd);
        if (dpi) {
            return static_cast<double>(dpi) / 96.0;
        }
    }

    return 0.0;
}

double GetWindowWidthDip(HWND taskbarWnd, double scale) {
    if (!taskbarWnd || scale <= 0) {
        return 0.0;
    }

    RECT rect{};
    if (!GetWindowRect(taskbarWnd, &rect)) {
        return 0.0;
    }

    double width = (rect.right - rect.left) / scale;
    return width > 0 ? width : 0.0;
}

double GetWindowHeightDip(HWND taskbarWnd, double scale) {
    if (!taskbarWnd || scale <= 0) {
        return 0.0;
    }

    RECT rect{};
    if (!GetWindowRect(taskbarWnd, &rect)) {
        return 0.0;
    }

    double height = (rect.bottom - rect.top) / scale;
    return height > 0 ? height : 0.0;
}

double GetTaskbarScale(FrameworkElement const& frame) {
    try {
        if (auto xamlRoot = frame.XamlRoot()) {
            double rasterizationScale = xamlRoot.RasterizationScale();
            if (rasterizationScale > 0) {
                return rasterizationScale;
            }
        }
    } catch (...) {
    }

    return 1.0;
}

double GetEffectiveScale(FrameworkElement const& frame, HWND taskbarWnd) {
    double scale = GetTaskbarWindowScale(taskbarWnd);
    if (scale <= 0) {
        scale = GetTaskbarScale(frame);
    }
    return scale;
}

double GetTaskbarAreaHeightDip(int areaHeightSetting,
                               FrameworkElement const& frame,
                               HWND taskbarWnd,
                               double scale) {
    if (areaHeightSetting > 0 && scale > 0) {
        return areaHeightSetting / scale;
    }

    try {
        double height = frame.ActualHeight();
        if (height > 0) {
            return height;
        }
    } catch (...) {
    }

    return GetWindowHeightDip(taskbarWnd, scale);
}

double GetTaskbarWidthDip(int taskbarWidthSetting,
                          FrameworkElement const& frame,
                          HWND taskbarWnd,
                          double scale) {
    if (taskbarWidthSetting <= 0) {
        return 0.0;
    }

    double width = taskbarWidthSetting / scale;

    double windowWidth = GetWindowWidthDip(taskbarWnd, scale);
    if (windowWidth <= 0) {
        try {
            if (auto xamlRoot = frame.XamlRoot()) {
                windowWidth = xamlRoot.Size().Width;
            }
        } catch (...) {
        }
    }

    if (windowWidth > 0 && width > windowWidth) {
        width = windowWidth;
    }

    return width;
}

double GetFloatMarginDip(int floatMarginSetting, double scale) {
    if (scale <= 0) {
        return 0.0;
    }

    return floatMarginSetting / scale;
}

double GetFrameHeightDip(int areaHeightSetting, double scale) {
    if (areaHeightSetting <= 0 || scale <= 0) {
        return 0.0;
    }

    return areaHeightSetting / scale;
}

double GetBarHeightDip(double areaHeight,
                       double windowHeight,
                       double floatMargin) {
    if (areaHeight <= 0) {
        return 0.0;
    }

    double visibleHeight = areaHeight;
    if (windowHeight > 0 && windowHeight < visibleHeight) {
        visibleHeight = windowHeight;
    }

    double barHeight = visibleHeight - floatMargin;
    if (barHeight < 1) {
        barHeight = 1;
    }

    return barHeight;
}

double GetTrayGapDip(int taskbarWidthSetting,
                     FrameworkElement const& frame,
                     FrameworkElement const& trayElement,
                     HWND taskbarWnd,
                     double scale) {
    double windowWidth = GetWindowWidthDip(taskbarWnd, scale);

    double pillWidth = 0.0;
    if (taskbarWidthSetting > 0) {
        pillWidth = GetTaskbarWidthDip(taskbarWidthSetting, frame, taskbarWnd,
                                       scale);
    } else {
        try {
            pillWidth = frame.ActualWidth();
        } catch (...) {
        }
    }

    if (windowWidth <= 0 || pillWidth <= 0 || windowWidth <= pillWidth) {
        return 0.0;
    }

    double gap = (windowWidth - pillWidth) / 2.0;

    double trayWidth = 0.0;
    if (trayElement) {
        try {
            trayWidth = trayElement.ActualWidth();
        } catch (...) {
        }
    }

    if (trayWidth > 0 && gap > pillWidth - trayWidth) {
        gap = pillWidth - trayWidth;
    }

    return gap > 0 ? gap : 0.0;
}

FrameworkElement FindSiblingTray(FrameworkElement const& frame) {
    FrameworkElement parent = nullptr;
    try {
        parent = VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }

    if (!parent) {
        return nullptr;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(parent);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            if (ClassNameEquals(winrt::get_class_name(child),
                                L"SystemTray.SystemTrayFrame")) {
                return child;
            }
        } catch (...) {
        }
    }

    return nullptr;
}

// While a custom taskbar area height is in use, the taskbar frame gets an
// explicit height and its parent has to be stretched to that height as well,
// otherwise explorer's own layout updates squeeze the frame back. The original
// parent height is remembered so that it can be restored later on.
void SetParentHeightForFrame(FrameworkElement const& frame,
                             double heightDip,
                             AppliedTaskbarState& record) {
    if (!(heightDip > 0)) {
        return;
    }

    try {
        FrameworkElement parent =
            VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
        if (!parent) {
            return;
        }

        if (!record.parentHeightSet) {
            record.savedParentHeight = parent.Height();
            record.frameParent = winrt::make_weak(parent);
            record.parentHeightSet = true;
        }

        parent.Height(heightDip);
    } catch (...) {
    }
}

void ReassertParentHeight(AppliedTaskbarState const& record,
                          double heightDip) {
    if (!record.parentHeightSet || !(heightDip > 0)) {
        return;
    }

    if (auto parent = record.frameParent.get()) {
        try {
            parent.Height(heightDip);
        } catch (...) {
        }
    }
}

HWND FindIslandWindow(HWND taskbarWnd) {
    if (!taskbarWnd) {
        return nullptr;
    }

    return FindWindowExW(
        taskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
}

struct FindTaskbarWindowContext {
    HMONITOR monitor;
    HWND result;
};

BOOL CALLBACK FindTaskbarWindowOnMonitorEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<FindTaskbarWindowContext*>(lParam);

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (!IsTaskbarClassName(className)) {
        return TRUE;
    }

    if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != context->monitor) {
        return TRUE;
    }

    context->result = hWnd;
    return FALSE;
}

HWND FindTaskbarWindowOnMonitor(HMONITOR monitor) {
    if (g_cachedTaskbarMonitorWnd &&
        g_cachedTaskbarMonitor == monitor &&
        IsWindow(g_cachedTaskbarMonitorWnd) &&
        MonitorFromWindow(g_cachedTaskbarMonitorWnd,
                          MONITOR_DEFAULTTONULL) == monitor &&
        IsTaskbarWindowClass(g_cachedTaskbarMonitorWnd)) {
        return g_cachedTaskbarMonitorWnd;
    }

    FindTaskbarWindowContext context{monitor, nullptr};
    EnumWindows(FindTaskbarWindowOnMonitorEnumProc,
                reinterpret_cast<LPARAM>(&context));

    if (!context.result) {
        context.result = FindWindowW(L"Shell_TrayWnd", nullptr);
    }

    g_cachedTaskbarMonitor = monitor;
    g_cachedTaskbarMonitorWnd = context.result;
    return context.result;
}

void RepositionControlCenterWindow(HWND controlCenterWnd) {
    if (g_unloading || !controlCenterWnd || !IsWindow(controlCenterWnd)) {
        return;
    }

    const int taskbarWidthSetting = g_settingTaskbarWidth.load();

    HWND taskbarWnd = FindTaskbarWindowOnMonitor(
        MonitorFromWindow(controlCenterWnd, MONITOR_DEFAULTTONEAREST));
    if (!taskbarWnd) {
        return;
    }

    RECT taskbarRect{};
    RECT controlCenterRect{};
    if (!GetWindowRect(taskbarWnd, &taskbarRect) ||
        !GetWindowRect(controlCenterWnd, &controlCenterRect)) {
        return;
    }

    const double scale = GetTaskbarWindowScale(taskbarWnd);
    if (scale <= 0) {
        return;
    }

    const double controlCenterWidth =
        static_cast<double>(controlCenterRect.right - controlCenterRect.left);

    const double windowWidthPx =
        static_cast<double>(taskbarRect.right - taskbarRect.left);
    double pillWidthPx =
        taskbarWidthSetting > 0
            ? static_cast<double>(taskbarWidthSetting)
            : g_lastPillWidthPx.load();
    if (pillWidthPx <= 0) {
        return;
    }
    if (pillWidthPx > windowWidthPx) {
        pillWidthPx = windowWidthPx;
    }

    const double barRightPx = static_cast<double>(taskbarRect.left) +
                              (windowWidthPx + pillWidthPx) / 2.0;
    const long targetRightPx =
        std::lround(barRightPx + std::lround(kControlCenterRightInsetDip *
                                             scale));

    // Publish the target position for the process which hosts the panel, which
    // applies it while the window is being moved. This way, the panel is in the
    // right place as soon as it appears instead of jumping there later on.
    SetPropW(controlCenterWnd, kControlCenterTargetRightProp,
             (HANDLE)(ULONG_PTR)targetRightPx);
    SetPropW(controlCenterWnd, kControlCenterPillWidthProp,
             (HANDLE)(ULONG_PTR)std::lround(pillWidthPx));

    if (controlCenterWidth <= 0) {
        return;
    }

    const long newLeft = std::lround(targetRightPx - controlCenterWidth);

    if (controlCenterRect.left == newLeft) {
        return;
    }

    SetWindowPos(controlCenterWnd, nullptr, newLeft, controlCenterRect.top, 0,
                 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                        SWP_ASYNCWINDOWPOS);
}

BOOL CALLBACK RepositionControlCenterEnumProc(HWND hWnd, LPARAM) {
    WCHAR className[64];
    if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        IsControlCenterClassName(className)) {
        RepositionControlCenterWindow(hWnd);
    }

    return TRUE;
}

bool IsControlCenterWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return IsControlCenterClassName(className);
}

void RefreshControlCenterWindow() {
    if (IsControlCenterWindow(g_controlCenterWnd.load())) {
        return;
    }

    g_controlCenterWnd.store(nullptr);

    HWND found = FindWindowW(L"ControlCenterWindow", nullptr);
    if (IsControlCenterWindow(found)) {
        g_controlCenterWnd.store(found);
    }
}

void RepositionControlCenterCached() {
    if (g_unloading) {
        return;
    }

    g_lastControlCenterFixTick = GetTickCount();
    RefreshControlCenterWindow();

    if (HWND controlCenterWnd = g_controlCenterWnd.load()) {
        RepositionControlCenterWindow(controlCenterWnd);
    }
}

void RepositionControlCenterWindows() {
    if (g_unloading) {
        return;
    }

    g_lastControlCenterFixTick = GetTickCount();
    RefreshControlCenterWindow();
    EnumWindows(RepositionControlCenterEnumProc, 0);
}

void CALLBACK ControlCenterLocationChanged(HWINEVENTHOOK hook,
                                           DWORD event,
                                           HWND hWnd,
                                           LONG idObject,
                                           LONG idChild,
                                           DWORD threadId,
                                           DWORD time) {
    HookScope hookScope;
    (void)hook;
    (void)event;
    (void)threadId;
    (void)time;

    if (g_unloading || idObject != OBJID_WINDOW || idChild != 0 || !hWnd) {
        return;
    }

    if (hWnd == g_controlCenterWnd.load()) {
        RepositionControlCenterWindow(hWnd);
        return;
    }

    if (IsControlCenterWindow(hWnd)) {
        g_controlCenterWnd.store(hWnd);
        RepositionControlCenterWindow(hWnd);
        return;
    }

    const DWORD now = GetTickCount();
    if (now - g_lastControlCenterFixTick.load() <= 600) {
        return;
    }
    g_lastControlCenterFixTick = now;

    RefreshControlCenterWindow();
    if (hWnd == g_controlCenterWnd.load()) {
        RepositionControlCenterWindow(hWnd);
    }
}

using RunFromWindowThreadProc_t = void (*)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam);

DWORD WINAPI ControlCenterHookThreadProc(LPVOID) {
    g_controlCenterHookThreadId = GetCurrentThreadId();
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_controlCenterWinEventHook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                        EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        ControlCenterLocationChanged, 0, 0,
                        WINEVENT_OUTOFCONTEXT);
    if (!g_controlCenterWinEventHook) {
        return 0;
    }

    while (!g_controlCenterHookStop.load() &&
           GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_controlCenterWinEventHook) {
        UnhookWinEvent(g_controlCenterWinEventHook);
        g_controlCenterWinEventHook = nullptr;
    }

    return 0;
}

void InstallControlCenterHook() {
    if (g_controlCenterHookThread) {
        if (WaitForSingleObject(g_controlCenterHookThread, 0) ==
            WAIT_TIMEOUT) {
            return;
        }
        CloseHandle(g_controlCenterHookThread);
        g_controlCenterHookThread = nullptr;
        g_controlCenterHookThreadId = 0;
    }

    g_controlCenterHookStop = false;
    g_controlCenterHookThread =
        CreateThread(nullptr, 0, ControlCenterHookThreadProc, nullptr, 0,
                     nullptr);
}

void RemoveControlCenterHookOnCurrentThread() {
    if (!g_controlCenterHookThread) {
        return;
    }

    g_controlCenterHookStop = true;
    for (int i = 0; i < 100 && !g_controlCenterHookThreadId.load(); ++i) {
        Sleep(1);
    }
    if (DWORD threadId = g_controlCenterHookThreadId.load()) {
        PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }
    WaitForSingleObject(g_controlCenterHookThread, 3000);
    CloseHandle(g_controlCenterHookThread);
    g_controlCenterHookThread = nullptr;
    g_controlCenterHookThreadId = 0;
}

void RemoveControlCenterHook() {
    RemoveControlCenterHookOnCurrentThread();
    g_controlCenterWnd.store(nullptr);
    g_lastPillWidthPx.store(0.0);
}

bool TryGetElementRectInContent(
    FrameworkElement const& element,
    UIElement const& contentElement,
    winrt::Windows::Foundation::Rect& resultRect) {
    if (!element || !contentElement) {
        return false;
    }

    try {
        double width = element.ActualWidth();
        double height = element.ActualHeight();
        if (width <= 0 || height <= 0) {
            return false;
        }

        resultRect =
            element.TransformToVisual(contentElement)
                .TransformBounds({0.0f, 0.0f, static_cast<float>(width),
                                  static_cast<float>(height)});
        return resultRect.Width > 0 && resultRect.Height > 0;
    } catch (...) {
    }

    return false;
}

void UpdateTaskbarClickThroughRegion(
    FrameworkElement const& rootGrid,
    FrameworkElement const& trayElement,
    AppliedTaskbarState& record,
    HWND taskbarWnd) {
    if (g_unloading || !taskbarWnd || !IsWindow(taskbarWnd)) {
        return;
    }

    if (record.taskbarWnd != taskbarWnd) {
        record.taskbarWnd = taskbarWnd;
        record.islandHwnd = nullptr;
        record.topLevelWnd = nullptr;
        record.hasAppliedRegion = false;
    }

    HWND islandHwnd = record.islandHwnd;
    if (!islandHwnd || !IsWindow(islandHwnd)) {
        islandHwnd = FindIslandWindow(taskbarWnd);
        if (!islandHwnd) {
            if (!g_loggedMissingIsland.exchange(true)) {
                Wh_Log(L"Taskbar XAML island window not found");
            }
            return;
        }
        record.islandHwnd = islandHwnd;
    }

    g_loggedMissingIsland = false;

    HWND topLevelWnd = GetAncestor(islandHwnd, GA_ROOT);
    if (!topLevelWnd) {
        return;
    }

    auto frame = record.frame.get();
    if (!frame) {
        return;
    }

    UIElement contentElement = nullptr;
    try {
        if (auto xamlRoot = frame.XamlRoot()) {
            contentElement = xamlRoot.Content().try_as<UIElement>();
        }
    } catch (...) {
    }
    if (!contentElement) {
        return;
    }

    winrt::Windows::Foundation::Rect pillDip{};
    if (!TryGetElementRectInContent(rootGrid, contentElement, pillDip)) {
        return;
    }

    winrt::Windows::Foundation::Rect trayDip{};
    const bool hasTrayRect =
        TryGetElementRectInContent(trayElement, contentElement, trayDip);

    double scale = 0.0;
    UINT dpi = GetDpiForWindow(islandHwnd);
    if (dpi) {
        scale = static_cast<double>(dpi) / 96.0;
    }
    if (scale <= 0) {
        scale = GetTaskbarScale(frame);
    }
    if (scale <= 0) {
        return;
    }

    RECT topLevelRect{};
    if (!GetWindowRect(topLevelWnd, &topLevelRect)) {
        return;
    }

    POINT islandOrigin{0, 0};
    if (!ClientToScreen(islandHwnd, &islandOrigin)) {
        return;
    }

    int topLevelWidth = topLevelRect.right - topLevelRect.left;
    int topLevelHeight = topLevelRect.bottom - topLevelRect.top;

    auto toWindowRect = [&](winrt::Windows::Foundation::Rect const& dip) {
        int left =
            (islandOrigin.x - topLevelRect.left) +
            static_cast<int>(std::lround(dip.X * scale));
        int top = (islandOrigin.y - topLevelRect.top) +
                  static_cast<int>(std::lround(dip.Y * scale));
        int right = left + static_cast<int>(std::lround(dip.Width * scale));
        int bottom = top + static_cast<int>(std::lround(dip.Height * scale));
        RECT rect{std::max(0, std::min(left, topLevelWidth)),
                  std::max(0, std::min(top, topLevelHeight)),
                  std::max(0, std::min(right, topLevelWidth)),
                  std::max(0, std::min(bottom, topLevelHeight))};
        return rect;
    };

    RECT regionRect = toWindowRect(pillDip);

    if (hasTrayRect) {
        RECT trayRect = toWindowRect(trayDip);
        UnionRect(&regionRect, &regionRect, &trayRect);
    }

    if (regionRect.right <= regionRect.left ||
        regionRect.bottom <= regionRect.top) {
        return;
    }

    RECT currentRegionBox{};
    bool hasRegion = GetWindowRgnBox(topLevelWnd, &currentRegionBox) != ERROR;
    bool regionIsOurs =
        hasRegion && record.hasAppliedRegion &&
        EqualRect(&currentRegionBox, &record.appliedRegion);

    if (hasRegion && !regionIsOurs) {
        RECT fullRegionBox{0, 0, topLevelWidth, topLevelHeight};
        if (!EqualRect(&currentRegionBox, &fullRegionBox)) {
            if (!g_loggedRegionBusy.exchange(true)) {
                Wh_Log(L"Taskbar window region is owned by Explorer");
            }
            return;
        }
    }

    if (regionIsOurs && EqualRect(&record.appliedRegion, &regionRect)) {
        return;
    }

    const int cornerDiameter = std::max(
        0, static_cast<int>(
               std::lround(2.0 * g_settingCornerRadius.load() * scale)));
    HRGN region =
        cornerDiameter > 0
            ? CreateRoundRectRgn(regionRect.left, regionRect.top,
                                 regionRect.right + 1,
                                 regionRect.bottom + 1, cornerDiameter,
                                 cornerDiameter)
            : CreateRectRgn(regionRect.left, regionRect.top,
                            regionRect.right, regionRect.bottom);
    if (!region) {
        return;
    }

    if (SetWindowRgn(topLevelWnd, region, TRUE)) {
        record.topLevelWnd = topLevelWnd;
        record.appliedRegion = regionRect;
        record.hasAppliedRegion = true;
    } else {
        DeleteObject(region);
    }
}

void RemoveClickThroughRegion(AppliedTaskbarState const& record) {
    if (!record.hasAppliedRegion || !record.topLevelWnd ||
        !IsWindow(record.topLevelWnd)) {
        return;
    }

    RECT currentRegionBox{};
    if (GetWindowRgnBox(record.topLevelWnd, &currentRegionBox) == ERROR ||
        !EqualRect(&currentRegionBox, &record.appliedRegion)) {
        return;
    }

    SetWindowRgn(record.topLevelWnd, nullptr, TRUE);
}

void RestoreGradientOverlayFill(AppliedTaskbarState const& record) {
    if (!record.gradientOverlayApplied) {
        return;
    }

    if (auto overlay = record.gradientOverlay.get()) {
        try {
            overlay.Fill(record.gradientOverlayOriginalFill);
        } catch (...) {
        }
    }
}

void DetachAppliedTaskbarCallbacks(AppliedTaskbarState& record) {
    RestoreGradientOverlayFill(record);
    record.gradientOverlayOriginalFill = nullptr;
    record.gradientOverlayApplied = false;

    RestoreTaskButtonNameCallbacks(record.savedTaskButtonNameCallbacks);
    RestoreMultiWindowHighlights(record.savedMultiWindowHighlights);
    RestoreVerticalLayouts(record.savedVerticalLayouts);
    RestoreHoverShapes(record.savedHoverShapes);
    record.savedTaskButtonNameCallbacks.clear();
    record.savedMultiWindowHighlights.clear();
    record.savedVerticalLayouts.clear();
    record.savedHoverShapes.clear();
}

bool RestoreFrame(AppliedTaskbarState& record) {
    bool restored = true;
    RemoveClickThroughRegion(record);

    auto frame = record.frame.get();
    if (frame) {
        try {
            frame.MinWidth(record.savedFrameMinWidth);
            frame.MaxWidth(record.savedFrameMaxWidth);
            frame.HorizontalAlignment(record.savedFrameAlignment);
            frame.VerticalAlignment(record.savedFrameVerticalAlignment);
            if (record.frameHeightSet) {
                frame.MaxHeight(record.savedFrameMaxHeight);
                frame.Height(record.savedFrameHeight);
            }
            if (record.frameMarginSet) {
                frame.Margin(record.savedFrameMargin);
            }
        } catch (...) {
            restored = false;
        }

        if (record.parentHeightSet) {
            if (auto parent = record.frameParent.get()) {
                try {
                    if (std::isnan(record.savedParentHeight)) {
                        parent.ClearValue(FrameworkElement::HeightProperty());
                    } else {
                        parent.Height(record.savedParentHeight);
                    }
                } catch (...) {
                }
            }
        }

        if (auto tray = record.tray.get()) {
            try {
                tray.Margin(record.savedTrayMargin);
                tray.Height(record.savedTrayHeight);
                tray.VerticalAlignment(record.savedTrayVerticalAlignment);
            } catch (...) {
            }
        }

        Grid rootGrid = record.rootGrid.get();
        if (!rootGrid) {
            rootGrid = FindRootGrid(frame);
        }
        if (rootGrid) {
            try {
                rootGrid.ClearValue(FrameworkElement::WidthProperty());
                rootGrid.ClearValue(
                    FrameworkElement::HorizontalAlignmentProperty());
                rootGrid.ClearValue(Grid::CornerRadiusProperty());
                rootGrid.ClearValue(Panel::BackgroundProperty());
                rootGrid.ClearValue(FrameworkElement::MarginProperty());
                rootGrid.ClearValue(FrameworkElement::HeightProperty());
                rootGrid.ClearValue(
                    FrameworkElement::VerticalAlignmentProperty());
            } catch (...) {
            }
        }
    }

    if (Grid rootGrid = record.rootGrid.get()) {
        std::vector<FrameworkElement> backgroundRects;
        CollectBackgroundRects(rootGrid, backgroundRects);
        for (auto const& element : backgroundRects) {
            try {
                element.ClearValue(UIElement::VisibilityProperty());
            } catch (...) {
            }
        }
    }

    DetachAppliedTaskbarCallbacks(record);
    return restored;
}

bool ApplyToFrame(FrameworkElement const& frame, HWND knownTaskbarWnd = nullptr) {
    if (g_unloading) {
        return false;
    }

    const int taskbarWidthSetting = g_settingTaskbarWidth.load();
    const int taskbarAreaHeightSetting = g_settingTaskbarAreaHeight.load();
    const int floatMarginSetting = g_settingFloatMargin.load();
    const int cornerRadiusSetting = g_settingCornerRadius.load();
    const int hoverCornerRadiusSetting =
        g_settingHoverCornerRadius.load();
    const unsigned settingsVersion = g_settingsVersion.load();

    Grid rootGrid = FindRootGrid(frame);
    if (!rootGrid) {
        if (!g_loggedMissingRootGrid.exchange(true)) {
            Wh_Log(L"RootGrid not found, will retry on the next layout");
        }
        return false;
    }

    g_loggedMissingRootGrid = false;

    HWND taskbarWnd = knownTaskbarWnd;
    if (!taskbarWnd || !IsWindow(taskbarWnd)) {
        taskbarWnd = FindTaskbarWindowForFrame(frame);
    }

    std::vector<FrameworkElement> backgroundRects;
    CollectBackgroundRects(rootGrid, backgroundRects);

    if (backgroundRects.empty()) {
        if (!g_loggedMissingRects.exchange(true)) {
            Wh_Log(L"Background rectangles not found");
        }
    } else {
        g_loggedMissingRects = false;
    }

    if (g_systemTaskbarContentHeightDip.load() <= 0) {
        double contentHeight = 0.0;
        try {
            contentHeight = frame.ActualHeight();
        } catch (...) {
        }
        if (!(contentHeight > 0) || contentHeight > 60.0) {
            contentHeight = 48.0;
        }
        g_systemTaskbarContentHeightDip = contentHeight;
    }
    if (taskbarAreaHeightSetting > 0) {
        EnsureTaskbarWindowHeight(taskbarWnd, taskbarAreaHeightSetting);
    }
    double scale = GetEffectiveScale(frame, taskbarWnd);
    auto toDip = [scale](int pixels) {
        return static_cast<double>(pixels) / scale;
    };

    FrameworkElement trayElement = FindSiblingTray(frame);
    if (!trayElement && !g_loggedMissingTray.exchange(true)) {
        Wh_Log(L"System tray frame not found next to the taskbar frame");
    }
    if (trayElement) {
        g_loggedMissingTray = false;
    }

    AppliedTaskbarState record{};
    bool hasRecord = false;
    std::vector<AppliedTaskbarState> expiredRecords;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        auto& appliedStates = *g_appliedTaskbarStates;
        for (auto it = appliedStates.begin(); it != appliedStates.end();) {
            auto appliedFrame = it->frame.get();
            if (!appliedFrame) {
                expiredRecords.push_back(std::move(*it));
                it = appliedStates.erase(it);
            } else if (appliedFrame == frame) {
                record = std::move(*it);
                hasRecord = true;
                it = appliedStates.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto& expired : expiredRecords) {
        RestoreFrame(expired);
    }

    if (hasRecord && record.taskbarWnd && record.taskbarWnd != taskbarWnd) {
        RemoveClickThroughRegion(record);
        record.islandHwnd = nullptr;
        record.topLevelWnd = nullptr;
        record.hasAppliedRegion = false;
    }

    record.threadId = GetCurrentThreadId();
    record.taskbarWnd = taskbarWnd;
    try {
        record.frame = winrt::make_weak(frame);
        record.rootGrid = winrt::make_weak(rootGrid);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create frame weak references: %08X",
               static_cast<unsigned int>(ex.code()));
        return false;
    }

    record.savedHoverShapes.erase(
        std::remove_if(record.savedHoverShapes.begin(),
                       record.savedHoverShapes.end(),
                       [](SavedHoverShape const& saved) {
                           return !saved.element.get();
                       }),
        record.savedHoverShapes.end());
    record.savedVerticalLayouts.erase(
        std::remove_if(record.savedVerticalLayouts.begin(),
                       record.savedVerticalLayouts.end(),
                       [](SavedVerticalLayout const& saved) {
                           return !saved.element.get();
                       }),
        record.savedVerticalLayouts.end());
    record.savedMultiWindowHighlights.erase(
        std::remove_if(record.savedMultiWindowHighlights.begin(),
                       record.savedMultiWindowHighlights.end(),
                       [](SavedMultiWindowHighlight const& saved) {
                           return !saved.element.get();
                       }),
        record.savedMultiWindowHighlights.end());
    record.savedTaskButtonNameCallbacks.erase(
        std::remove_if(record.savedTaskButtonNameCallbacks.begin(),
                       record.savedTaskButtonNameCallbacks.end(),
                       [](SavedTaskButtonNameCallback const& saved) {
                           return !saved.button.get();
                       }),
        record.savedTaskButtonNameCallbacks.end());

    RestoreMultiWindowHighlights(record.savedMultiWindowHighlights);
    record.savedMultiWindowHighlights.clear();
    HideMultiWindowVisualsRecursive(rootGrid, record);

    if (!hasRecord) {
        try {
            record.savedFrameMinWidth = frame.MinWidth();
            record.savedFrameMaxWidth = frame.MaxWidth();
            record.savedFrameMaxHeight = frame.MaxHeight();
            record.savedFrameAlignment = frame.HorizontalAlignment();
            record.savedFrameHeight = frame.Height();
            record.savedFrameVerticalAlignment = frame.VerticalAlignment();
            record.savedFrameMargin = frame.Margin();
        } catch (...) {
        }
    }

    double appliedFrameHeightDip = 0.0;

    try {
        if (taskbarWidthSetting > 0) {
            double pillWidth = GetTaskbarWidthDip(taskbarWidthSetting, frame,
                                                  taskbarWnd, scale);
            frame.MinWidth(pillWidth);
            frame.MaxWidth(pillWidth);
            rootGrid.Width(pillWidth);
            rootGrid.HorizontalAlignment(HorizontalAlignment::Center);
        } else {
            frame.MinWidth(record.savedFrameMinWidth);
            frame.MaxWidth(record.savedFrameMaxWidth);
            rootGrid.ClearValue(FrameworkElement::WidthProperty());
            rootGrid.ClearValue(
                FrameworkElement::HorizontalAlignmentProperty());
        }

        frame.HorizontalAlignment(HorizontalAlignment::Center);

        if (taskbarAreaHeightSetting > 0) {
            frame.VerticalAlignment(VerticalAlignment::Bottom);
            frame.MaxHeight(std::numeric_limits<double>::infinity());
            appliedFrameHeightDip =
                GetFrameHeightDip(taskbarAreaHeightSetting, scale);
            if (appliedFrameHeightDip > 0) {
                frame.Height(appliedFrameHeightDip);
                record.frameHeightSet = true;
            }
        } else {
            frame.VerticalAlignment(record.savedFrameVerticalAlignment);
            if (record.frameHeightSet) {
                frame.MaxHeight(record.savedFrameMaxHeight);
                frame.Height(record.savedFrameHeight);
                record.frameHeightSet = false;
            }
        }

        frame.Margin(Thickness{0, 0, 0, 0});
        record.frameMarginSet = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply frame properties: %08X %s",
               static_cast<unsigned int>(ex.code()), ex.message().c_str());
        RestoreFrame(record);
        return false;
    }

    if (record.frameHeightSet && appliedFrameHeightDip > 0) {
        SetParentHeightForFrame(frame, appliedFrameHeightDip, record);
    }

    double areaHeight = GetTaskbarAreaHeightDip(taskbarAreaHeightSetting, frame,
                                                taskbarWnd, scale);
    double floatMargin = GetFloatMarginDip(floatMarginSetting, scale);
    double windowHeight = GetWindowHeightDip(taskbarWnd, scale);
    double barHeight = GetBarHeightDip(areaHeight, windowHeight, floatMargin);

    if (areaHeight <= 0 && !g_loggedStripUnknown.exchange(true)) {
        Wh_Log(L"Taskbar strip height not available yet");
    }
    if (areaHeight > 0) {
        g_loggedStripUnknown = false;
    }

    try {
        double cornerRadius = toDip(cornerRadiusSetting);
        rootGrid.CornerRadius(
            CornerRadius{cornerRadius, cornerRadius, cornerRadius,
                         cornerRadius});

        if (areaHeight > 0) {
            rootGrid.Height(barHeight);
            rootGrid.VerticalAlignment(VerticalAlignment::Bottom);
            rootGrid.Margin(Thickness{0, 0, 0, floatMargin});
        } else {
            rootGrid.ClearValue(FrameworkElement::HeightProperty());
            rootGrid.ClearValue(FrameworkElement::VerticalAlignmentProperty());
            rootGrid.Margin(Thickness{0, 0, 0, 0});
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply grid properties: %08X %s",
               static_cast<unsigned int>(ex.code()), ex.message().c_str());
        RestoreFrame(record);
        return false;
    }

    if (trayElement) {
        if (!hasRecord) {
            try {
                record.savedTrayMargin = trayElement.Margin();
                record.savedTrayHeight = trayElement.Height();
                record.savedTrayVerticalAlignment =
                    trayElement.VerticalAlignment();
            } catch (...) {
            }
        }

        double gapDip =
            GetTrayGapDip(taskbarWidthSetting, frame, trayElement, taskbarWnd,
                          scale);

        try {
            if (areaHeight > 0) {
                // Lock the tray row itself, not only its inner controls.
                // Explorer can reset its height, alignment, or margin without
                // a frame event, which moves the whole panel asynchronously.
                SaveAndApplyVerticalLayout(record.savedVerticalLayouts,
                                           trayElement, barHeight,
                                           VerticalAlignment::Top);
                Thickness trayMargin = trayElement.Margin();
                if (std::abs(trayMargin.Left) > 0.05 ||
                    std::abs(trayMargin.Right - gapDip) > 0.05) {
                    trayMargin.Left = 0;
                    trayMargin.Right = gapDip;
                    trayElement.Margin(trayMargin);
                }
            } else {
                trayElement.Height(record.savedTrayHeight);
                trayElement.VerticalAlignment(
                    record.savedTrayVerticalAlignment);
                trayElement.Margin(Thickness{0, 0, gapDip, 0});
            }
            record.trayArranged = true;
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Failed to apply tray properties: %08X",
                   static_cast<unsigned int>(ex.code()));
        }
    }

    if (areaHeight > 0) {
        ApplyCenteredTaskbarContent(record, frame, trayElement, barHeight);
    }

    double hoverRadius = toDip(hoverCornerRadiusSetting);
    g_hoverRadiusDip = hoverRadius;
    ApplyHoverShapesRecursive(rootGrid, record, hoverRadius, false);
    if (trayElement) {
        ApplyHoverShapesRecursive(trayElement, record, hoverRadius, true);
    }

    // Update ToolTips already attached to taskbar controls.
    ApplyTaskbarToolTipCornerRadiiRecursive(rootGrid);
    if (trayElement) {
        ApplyTaskbarToolTipCornerRadiiRecursive(trayElement);
    }
    try {
        ApplyOpenTaskbarToolTipCornerRadii(rootGrid.XamlRoot());
    } catch (...) {
    }

    BackgroundSettings backgroundSettings;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        backgroundSettings = g_backgroundSettings;
    }

    RestoreGradientOverlayFill(record);
    record.gradientOverlay = winrt::weak_ref<Shapes::Rectangle>{};
    record.gradientOverlayOriginalFill = nullptr;
    record.gradientOverlayApplied = false;

    try {
        Brush customBrush = CreateBackgroundBrush(backgroundSettings);
        Brush backgroundBrush =
            customBrush ? customBrush
                        : FindOriginalBackgroundBrush(backgroundRects);

        Shapes::Rectangle gradientOverlay = nullptr;
        if (backgroundSettings.style == BackgroundStyle::gradient) {
            for (auto const& element : backgroundRects) {
                if (ElementNameEquals(element, L"BackgroundFill")) {
                    gradientOverlay = element.try_as<Shapes::Rectangle>();
                    if (gradientOverlay) {
                        break;
                    }
                }
            }
        }

        if (backgroundSettings.style == BackgroundStyle::gradient &&
            customBrush && gradientOverlay) {
            ParsedBackgroundColor baseColor = ParseBackgroundColor(
                backgroundSettings.gradientColor1, L"#1F2937");
            SolidColorBrush baseBrush;
            baseBrush.Color(MakeColor(baseColor.rgb, 0xFF));

            record.gradientOverlayOriginalFill = gradientOverlay.Fill();
            record.gradientOverlay = winrt::make_weak(gradientOverlay);
            record.gradientOverlayApplied = true;

            rootGrid.Background(baseBrush);
            gradientOverlay.Fill(customBrush);
            gradientOverlay.Visibility(Visibility::Visible);
        } else if (backgroundBrush) {
            rootGrid.Background(backgroundBrush);
        } else {
            rootGrid.ClearValue(Panel::BackgroundProperty());
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply background: %08X",
               static_cast<unsigned int>(ex.code()));
    }

    Shapes::Rectangle appliedGradientOverlay =
        record.gradientOverlayApplied ? record.gradientOverlay.get() : nullptr;
    for (auto const& element : backgroundRects) {
        if (appliedGradientOverlay && element == appliedGradientOverlay) {
            continue;
        }
        try {
            element.Visibility(Visibility::Collapsed);
        } catch (...) {
        }
    }

    record.version = settingsVersion;
    record.lastApplyTick = GetTickCount();
    try {
        record.tray = winrt::weak_ref<FrameworkElement>{};
        if (trayElement) {
            record.tray = winrt::make_weak(trayElement);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create weak references: %08X",
               static_cast<unsigned int>(ex.code()));
    }

    UpdateTaskbarClickThroughRegion(rootGrid, trayElement, record, taskbarWnd);
    if (!record.hasAppliedRegion) {
        record.regionRetries++;
    }
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        g_appliedTaskbarStates->push_back(std::move(record));
    }

    InstallControlCenterHook();

    try {
        double pillWidthPx = taskbarWidthSetting > 0
                                 ? static_cast<double>(taskbarWidthSetting)
                                 : rootGrid.ActualWidth() * scale;
        if (pillWidthPx > 0) {
            g_lastPillWidthPx.store(pillWidthPx);
        }
    } catch (...) {
    }

    if (GetTickCount() - g_lastControlCenterFixTick.load() > 500) {
        RepositionControlCenterWindows();
    }

    return true;
}

void RefreshTaskButtonVisuals(FrameworkElement const& button) {
    if (g_unloading || !button) {
        return;
    }

    try {
        XamlRoot xamlRoot = button.XamlRoot();
        HWND taskbarWnd = FindTaskbarWindowForXamlRoot(xamlRoot);
        FrameworkElement frame = FindTaskbarFrameInXamlRoot(xamlRoot);
        if (frame) {
            ApplyToFrame(frame, taskbarWnd);
        }
    } catch (...) {
    }
}

// Revalidates the properties of an existing state record without changing it.
// The caller applies the existing 500 ms throttle after this check.
bool NeedsTaskbarReapply(FrameworkElement const& appliedFrame,
                         AppliedTaskbarState const& applied,
                         int taskbarWidthSetting,
                         int taskbarAreaHeightSetting,
                         int floatMarginSetting,
                         int cornerRadiusSetting) {
    bool needsApply = false;

    HWND taskbarWnd = applied.taskbarWnd;
    if (!taskbarWnd || !IsWindow(taskbarWnd)) {
        taskbarWnd = FindTaskbarWindowForFrame(appliedFrame);
    }
    double scale = GetEffectiveScale(appliedFrame, taskbarWnd);
    const double expectedWidth =
        taskbarWidthSetting > 0
            ? GetTaskbarWidthDip(taskbarWidthSetting, appliedFrame,
                                 taskbarWnd, scale)
            : 0.0;

    if (taskbarWidthSetting > 0) {
        double currentWidth = appliedFrame.ActualWidth();
        if (currentWidth <= 0 ||
            std::abs(currentWidth - expectedWidth) > 2.0) {
            needsApply = true;
        }
    }

    if (appliedFrame.HorizontalAlignment() != HorizontalAlignment::Center) {
        needsApply = true;
    }

    Thickness frameMargin = appliedFrame.Margin();
    if (std::abs(frameMargin.Left) > 0.5 ||
        std::abs(frameMargin.Top) > 0.5 ||
        std::abs(frameMargin.Right) > 0.5 ||
        std::abs(frameMargin.Bottom) > 0.5) {
        needsApply = true;
    }

    if (auto appliedRootGrid = applied.rootGrid.get()) {
        double expectedRadius =
            scale > 0 ? cornerRadiusSetting / scale : 0.0;
        if (taskbarWidthSetting > 0 && scale > 0) {
            double currentGridWidth = appliedRootGrid.Width();
            if (std::isnan(currentGridWidth) ||
                std::abs(currentGridWidth - expectedWidth) > 2.0) {
                needsApply = true;
            }
        }

        CornerRadius radius = appliedRootGrid.CornerRadius();
        if (std::abs(radius.TopLeft - expectedRadius) > 0.5 ||
            std::abs(radius.TopRight - expectedRadius) > 0.5 ||
            std::abs(radius.BottomRight - expectedRadius) > 0.5 ||
            std::abs(radius.BottomLeft - expectedRadius) > 0.5) {
            needsApply = true;
        }
    }

    double areaHeight = GetTaskbarAreaHeightDip(
        taskbarAreaHeightSetting, appliedFrame, taskbarWnd, scale);
    double floatMargin = GetFloatMarginDip(floatMarginSetting, scale);
    double barHeight =
        GetBarHeightDip(areaHeight,
                        GetWindowHeightDip(taskbarWnd, scale), floatMargin);

    if (!needsApply && areaHeight > 0) {
        if (auto appliedRootGrid = applied.rootGrid.get()) {
            double currentHeight = appliedRootGrid.Height();
            if (std::isnan(currentHeight) ||
                std::abs(currentHeight - barHeight) > 0.5) {
                needsApply = true;
            } else {
                Thickness margin = appliedRootGrid.Margin();
                if (std::abs(margin.Bottom - floatMargin) > 0.5 ||
                    appliedRootGrid.VerticalAlignment() !=
                        VerticalAlignment::Bottom) {
                    needsApply = true;
                }
            }
        } else {
            needsApply = true;
        }
    }

    if (!needsApply && taskbarAreaHeightSetting > 0) {
        double expectedAreaHeight =
            GetFrameHeightDip(taskbarAreaHeightSetting, scale);
        double currentAreaHeight = appliedFrame.Height();
        if (std::isnan(currentAreaHeight) ||
            std::abs(currentAreaHeight - expectedAreaHeight) > 0.5 ||
            appliedFrame.VerticalAlignment() != VerticalAlignment::Bottom) {
            needsApply = true;
        }
    }

    if (!needsApply && applied.trayArranged) {
        FrameworkElement currentTray = FindSiblingTray(appliedFrame);
        if (currentTray) {
            double gapDip =
                GetTrayGapDip(taskbarWidthSetting, appliedFrame, currentTray,
                              taskbarWnd, scale);
            Thickness margin = currentTray.Margin();
            if (std::abs(margin.Right - gapDip) > 0.5 ||
                std::abs(margin.Left) > 0.5 ||
                std::abs(margin.Top) > 0.5 ||
                std::abs(margin.Bottom) > 0.5) {
                needsApply = true;
            }

            if (!needsApply && areaHeight > 0) {
                double currentTrayHeight = currentTray.Height();
                if (std::isnan(currentTrayHeight) ||
                    std::abs(currentTrayHeight - barHeight) > 0.5 ||
                    currentTray.VerticalAlignment() !=
                        VerticalAlignment::Top) {
                    needsApply = true;
                }
            }
        }
    }

    if (!needsApply) {
        for (auto const& saved : applied.savedVerticalLayouts) {
            auto element = saved.element.get();
            if (!element) {
                continue;
            }
            Thickness margin = element.Margin();
            if ((!std::isnan(saved.targetHeight) &&
                 !NearlyEqual(element.Height(), saved.targetHeight, 0.5)) ||
                element.VerticalAlignment() != saved.targetAlignment ||
                std::abs(margin.Top) > 0.5 ||
                std::abs(margin.Bottom) > 0.5) {
                needsApply = true;
                break;
            }
        }
    }

    if (!needsApply) {
        if (!applied.hasAppliedRegion) {
            needsApply = applied.regionRetries < 20;
        } else if (applied.topLevelWnd && IsWindow(applied.topLevelWnd)) {
            RECT currentRegionBox{};
            if (GetWindowRgnBox(applied.topLevelWnd, &currentRegionBox) ==
                ERROR) {
                needsApply = true;
            } else if (!EqualRect(&currentRegionBox,
                                  &applied.appliedRegion)) {
                RECT topLevelRect{};
                if (GetWindowRect(applied.topLevelWnd, &topLevelRect)) {
                    RECT fullRegionBox{
                        0, 0, topLevelRect.right - topLevelRect.left,
                        topLevelRect.bottom - topLevelRect.top};
                    if (EqualRect(&currentRegionBox, &fullRegionBox)) {
                        needsApply = true;
                    }
                }
            }
        }
    }

    return needsApply;
}

void ApplyToFrameWhenNeeded(void* taskbarFrameThis) {
    if (g_unloading) {
        return;
    }

    try {
        FrameworkElement frame = nullptr;
        static_cast<IUnknown*>(taskbarFrameThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(frame));
        if (!frame) {
            return;
        }

        if (!ClassNameEquals(winrt::get_class_name(frame),
                             L"Taskbar.TaskbarFrame")) {
            return;
        }

        if (GetTickCount() - g_lastControlCenterFixTick.load() > 400) {
            RepositionControlCenterCached();
        }

        const DWORD now = GetTickCount();
        bool needsApply = true;
        HWND taskbarWnd = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_appliedMutex);
            const unsigned settingsVersion = g_settingsVersion.load();
            const int taskbarWidthSetting = g_settingTaskbarWidth.load();
            const int taskbarAreaHeightSetting =
                g_settingTaskbarAreaHeight.load();
            const int floatMarginSetting = g_settingFloatMargin.load();
            const int cornerRadiusSetting = g_settingCornerRadius.load();

            for (auto const& applied : *g_appliedTaskbarStates) {
                auto appliedFrame = applied.frame.get();
                if (!appliedFrame || appliedFrame != frame) {
                    continue;
                }

                taskbarWnd = applied.taskbarWnd;

                if (applied.version != settingsVersion) {
                    break;
                }

                if ((now - applied.lastApplyTick) < 500) {
                    return;
                }

                needsApply = NeedsTaskbarReapply(
                    appliedFrame, applied, taskbarWidthSetting,
                    taskbarAreaHeightSetting, floatMarginSetting,
                    cornerRadiusSetting);

                break;
            }
        }

        if (needsApply) {
            ApplyToFrame(frame, taskbarWnd);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"ApplyToFrameWhenNeeded failed: %08X",
               static_cast<unsigned int>(ex.code()));
    } catch (...) {
    }
}

XamlRoot GetXamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    if (!taskbarHostSharedPtr[0]) {
        if (RefCountDecref_Original) {
            RefCountDecref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    const BYTE* frameHeightBytes =
        static_cast<const BYTE*>(g_taskbarHostFrameHeight);
    if (frameHeightBytes && frameHeightBytes[0] == 0x48 &&
        frameHeightBytes[1] == 0x83 && frameHeightBytes[2] == 0xEC &&
        frameHeightBytes[4] == 0x48 && frameHeightBytes[5] == 0x83 &&
        frameHeightBytes[6] == 0xC1 && frameHeightBytes[7] <= 0x7F) {
        taskbarElementIUnknownOffset = frameHeightBytes[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        if (RefCountDecref_Original) {
            RefCountDecref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* frameHeightInstructions =
        static_cast<const DWORD*>(g_taskbarHostFrameHeight);
    if (frameHeightInstructions &&
        frameHeightInstructions[0] == 0xD503237F &&
        (frameHeightInstructions[1] & 0xFFC07FFF) == 0xA9807BFD &&
        frameHeightInstructions[2] == 0x910003FD &&
        (frameHeightInstructions[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementIUnknownOffset =
            (frameHeightInstructions[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        if (RefCountDecref_Original) {
            RefCountDecref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) +
        taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (RefCountDecref_Original) {
        RefCountDecref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

XamlRoot GetXamlRootForTaskbarWindow(HWND taskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original || !g_taskbarHostFrameHeight) {
        return nullptr;
    }

    WCHAR className[64];
    if (!GetClassName(taskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }

    const bool isSecondary =
        EqualsIgnoreCase(className, L"Shell_SecondaryTrayWnd");

    HWND taskSwitchWnd = nullptr;
    void* expectedVftable = nullptr;

    if (isSecondary) {
        if (!g_csecondaryTaskBandTaskListWndSiteVftable ||
            !CSecondaryTaskBand_GetTaskbarHost_Original) {
            return nullptr;
        }
        taskSwitchWnd =
            FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
        expectedVftable = g_csecondaryTaskBandTaskListWndSiteVftable;
    } else {
        if (!g_ctaskBandTaskListWndSiteVftable) {
            return nullptr;
        }
        taskSwitchWnd =
            static_cast<HWND>(GetProp(taskbarWnd, L"TaskbandHWND"));
        expectedVftable = g_ctaskBandTaskListWndSiteVftable;
    }

    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(
        GetWindowLongPtr(taskSwitchWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* siteCandidate = taskBand;
    bool foundVftable = false;
    for (int i = 0; i < 20; i++) {
        if (*static_cast<void**>(siteCandidate) == expectedVftable) {
            foundVftable = true;
            break;
        }
        siteCandidate = static_cast<BYTE*>(siteCandidate) + sizeof(void*);
    }

    if (!foundVftable) {
        return nullptr;
    }

    void* taskbarHostSharedPtr[2]{};
    if (isSecondary) {
        CSecondaryTaskBand_GetTaskbarHost_Original(siteCandidate,
                                                   taskbarHostSharedPtr);
    } else {
        CTaskBand_GetTaskbarHost_Original(siteCandidate,
                                          taskbarHostSharedPtr);
    }

    return GetXamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

struct TaskbarWindowForXamlRootContext {
    XamlRoot xamlRoot;
    DWORD threadId = 0;
    HWND result = nullptr;
};

BOOL CALLBACK FindTaskbarWindowForXamlRootEnumProc(HWND hWnd, LPARAM lParam) {
    auto context =
        reinterpret_cast<TaskbarWindowForXamlRootContext*>(lParam);

    DWORD processId = 0;
    WCHAR className[64];
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId() ||
        GetWindowThreadProcessId(hWnd, nullptr) != context->threadId ||
        !GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        !IsTaskbarClassName(className)) {
        return TRUE;
    }

    XamlRoot windowXamlRoot = GetXamlRootForTaskbarWindow(hWnd);
    if (windowXamlRoot && windowXamlRoot == context->xamlRoot) {
        context->result = hWnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindTaskbarWindowForXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }

    TaskbarWindowForXamlRootContext context{xamlRoot, GetCurrentThreadId(),
                                            nullptr};
    EnumWindows(FindTaskbarWindowForXamlRootEnumProc,
                reinterpret_cast<LPARAM>(&context));
    return context.result;
}

HWND FindTaskbarWindowForFrame(FrameworkElement const& frame) {
    if (!frame) {
        return nullptr;
    }

    try {
        return FindTaskbarWindowForXamlRoot(frame.XamlRoot());
    } catch (...) {
    }

    return nullptr;
}

FrameworkElement FindTaskbarFrameInXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }

    FrameworkElement contentElement = nullptr;
    try {
        contentElement = xamlRoot.Content().try_as<FrameworkElement>();
    } catch (...) {
    }

    if (!contentElement) {
        return nullptr;
    }

    return FindChildByPredicate(contentElement,
                                [](FrameworkElement const& element) {
                                    return ClassNameEquals(
                                        winrt::get_class_name(element),
                                        L"Taskbar.TaskbarFrame");
                                });
}

void ApplyNowOnTaskbarThread(PVOID taskbarWindowParam) {
    HWND taskbarWnd = static_cast<HWND>(taskbarWindowParam);
    if (g_unloading) {
        return;
    }

    try {
        XamlRoot xamlRoot = GetXamlRootForTaskbarWindow(taskbarWnd);
        if (!xamlRoot) {
            if (!g_loggedMissingXamlRoot.exchange(true)) {
                Wh_Log(L"Taskbar XAML root not found");
            }
            return;
        }

        g_loggedMissingXamlRoot = false;

        FrameworkElement frame = FindTaskbarFrameInXamlRoot(xamlRoot);
        if (!frame) {
            if (!g_loggedNoDirectApply.exchange(true)) {
                Wh_Log(L"Taskbar frame not found in the XAML root");
            }
            return;
        }

        g_loggedNoDirectApply = false;

        ApplyToFrame(frame, taskbarWnd);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"ApplyNowOnTaskbarThread failed: %08X",
               static_cast<unsigned int>(ex.code()));
    } catch (...) {
    }
}

void ApplyToAllTaskbarsNow();

VOID CALLBACK LayoutCalibrationTimerProc(PVOID, BOOLEAN) {
    HookScope hookScope;

    bool shouldApply = false;
    {
        std::lock_guard<std::mutex> lock(g_layoutCalibrationMutex);
        shouldApply = g_layoutCalibrationScheduled;
        g_layoutCalibrationScheduled = false;
    }

    if (shouldApply && !g_unloading) {
        ApplyToAllTaskbarsNow();
    }
}

// A freshly loaded taskbar can finish Explorer's initial layout after
// OnLoaded has already called ApplyToFrameWhenNeeded. Coalesce those loads
// into one calibration callback instead of polling in a worker thread.
void ScheduleTaskbarLayoutCalibrationAfterCreate() {
    if (g_unloading) {
        return;
    }

    constexpr DWORD kLayoutCalibrationDelayMs = 600;
    std::lock_guard<std::mutex> lock(g_layoutCalibrationMutex);
    if (g_layoutCalibrationScheduled || g_unloading) {
        return;
    }

    if (!g_layoutCalibrationTimerQueue) {
        g_layoutCalibrationTimerQueue = CreateTimerQueue();
    }
    if (!g_layoutCalibrationTimerQueue) {
        return;
    }

    g_layoutCalibrationScheduled = true;
    if (!CreateTimerQueueTimer(nullptr, g_layoutCalibrationTimerQueue,
                               LayoutCalibrationTimerProc, nullptr,
                               kLayoutCalibrationDelayMs, 0,
                               WT_EXECUTEONLYONCE)) {
        g_layoutCalibrationScheduled = false;
    }
}

void StopLayoutCalibrationTimer() {
    HANDLE timerQueue = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_layoutCalibrationMutex);
        g_layoutCalibrationScheduled = false;
        timerQueue = g_layoutCalibrationTimerQueue;
        g_layoutCalibrationTimerQueue = nullptr;
    }

    if (timerQueue) {
        DeleteTimerQueueEx(timerQueue, INVALID_HANDLE_VALUE);
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &g_ctaskBandTaskListWndSiteVftable,
            nullptr,
            true,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &g_csecondaryTaskBandTaskListWndSiteVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &g_taskbarHostFrameHeight,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &RefCountDecref_Original,
            nullptr,
            true,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    return true;
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    HookScope hookScope;

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    ApplyToFrameWhenNeeded(pThis);
    return ret;
}

using TaskbarFrame_OnApplyTemplate_t = int(WINAPI*)(void* pThis);
TaskbarFrame_OnApplyTemplate_t TaskbarFrame_OnApplyTemplate_Original;
int WINAPI TaskbarFrame_OnApplyTemplate_Hook(void* pThis) {
    HookScope hookScope;

    int ret = TaskbarFrame_OnApplyTemplate_Original(pThis);
    ApplyToFrameWhenNeeded(pThis);
    return ret;
}

using TaskbarFrame_OnLoaded_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
TaskbarFrame_OnLoaded_t TaskbarFrame_OnLoaded_Original;
void WINAPI TaskbarFrame_OnLoaded_Hook(void* pThis, void* sender, void* args) {
    HookScope hookScope;

    TaskbarFrame_OnLoaded_Original(pThis, sender, args);
    ApplyToFrameWhenNeeded(pThis);
    ScheduleTaskbarLayoutCalibrationAfterCreate();
}

using TaskbarFrame_OnPointerMoved_t =
    int(WINAPI*)(void* pThis, void* args);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;
int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* args) {
    HookScope hookScope;

    int ret = TaskbarFrame_OnPointerMoved_Original(pThis, args);
    ApplyToFrameWhenNeeded(pThis);
    return ret;
}

using ApplyTaskbarTooltipPlacement_t = void(__cdecl*)(
    ToolTip const& toolTip,
    int taskbarLocation,
    winrt::Windows::Foundation::Size primarySize,
    winrt::Windows::Foundation::Size secondarySize,
    bool flag1,
    bool flag2);
ApplyTaskbarTooltipPlacement_t ApplyTaskbarTooltipPlacement_Original;

void __cdecl ApplyTaskbarTooltipPlacement_Hook(
    ToolTip const& toolTip,
    int taskbarLocation,
    winrt::Windows::Foundation::Size primarySize,
    winrt::Windows::Foundation::Size secondarySize,
    bool flag1,
    bool flag2) {
    HookScope hookScope;

    ApplyTaskbarTooltipPlacement_Original(
        toolTip, taskbarLocation, primarySize, secondarySize, flag1, flag2);

    // Change only the outer corner radius of taskbar ToolTips.
    if (!g_unloading) {
        ApplyTaskbarToolTipCornerRadius(toolTip);
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void ReassertFramePropertiesOnCurrentThread() {
    if (g_unloading) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();
    std::vector<AppliedTaskbarState> records;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto const& applied : *g_appliedTaskbarStates) {
            if (applied.threadId == threadId) {
                records.push_back(applied);
            }
        }
    }

    for (auto const& record : records) {
        auto frame = record.frame.get();
        if (!frame) {
            continue;
        }

        try {
            const int taskbarWidthSetting = g_settingTaskbarWidth.load();
            const int taskbarAreaHeightSetting =
                g_settingTaskbarAreaHeight.load();
            const int floatMarginSetting = g_settingFloatMargin.load();

            HWND taskbarWnd = record.taskbarWnd;
            if (!taskbarWnd || !IsWindow(taskbarWnd)) {
                taskbarWnd = FindTaskbarWindowForFrame(frame);
            }
            double scale = GetEffectiveScale(frame, taskbarWnd);
            double areaHeight = GetTaskbarAreaHeightDip(
                taskbarAreaHeightSetting, frame, taskbarWnd, scale);
            double floatMargin = GetFloatMarginDip(floatMarginSetting, scale);
            double barHeight =
                GetBarHeightDip(areaHeight,
                                GetWindowHeightDip(taskbarWnd, scale),
                                floatMargin);

            if (taskbarWidthSetting > 0) {
                double pillWidth = GetTaskbarWidthDip(
                    taskbarWidthSetting, frame, taskbarWnd, scale);
                frame.MinWidth(pillWidth);
                frame.MaxWidth(pillWidth);
                if (auto rootGrid = record.rootGrid.get()) {
                    rootGrid.Width(pillWidth);
                    rootGrid.HorizontalAlignment(HorizontalAlignment::Center);
                }
            }

            frame.HorizontalAlignment(HorizontalAlignment::Center);
            frame.Margin(Thickness{0, 0, 0, 0});

            if (taskbarAreaHeightSetting > 0 && scale > 0) {
                frame.MaxHeight(std::numeric_limits<double>::infinity());
                double frameHeightDip =
                    GetFrameHeightDip(taskbarAreaHeightSetting, scale);
                if (frameHeightDip > 0) {
                    frame.Height(frameHeightDip);
                    // Explorer's frame height update shrinks the frame's parent
                    // back, so reassert the parent height as well.
                    ReassertParentHeight(record, frameHeightDip);
                }
                frame.VerticalAlignment(VerticalAlignment::Bottom);
            }

            if (areaHeight > 0 && barHeight > 0) {
                if (auto rootGrid = record.rootGrid.get()) {
                    rootGrid.Height(barHeight);
                    rootGrid.VerticalAlignment(VerticalAlignment::Bottom);
                    rootGrid.Margin(Thickness{0, 0, 0, floatMargin});
                }

                if (auto tray = record.tray.get()) {
                    double gapDip = GetTrayGapDip(taskbarWidthSetting, frame,
                                                  tray, taskbarWnd, scale);
                    tray.Height(barHeight);
                    tray.VerticalAlignment(VerticalAlignment::Top);
                    tray.Margin(Thickness{0, 0, gapDip, 0});
                }

                for (auto const& saved : record.savedVerticalLayouts) {
                    ApplyVerticalLayoutTarget(saved);
                }
            }
        } catch (...) {
        }
    }
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    HookScope hookScope;

    TaskbarController_UpdateFrameHeight_Original(pThis);
    ReassertFramePropertiesOnCurrentThread();
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
            },
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnApplyTemplate(void))",
            },
            &TaskbarFrame_OnApplyTemplate_Original,
            TaskbarFrame_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnLoaded(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::UI::Xaml::RoutedEventArgs const &))",
            },
            &TaskbarFrame_OnLoaded_Original,
            TaskbarFrame_OnLoaded_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))",
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))",
            },
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
            },
            &ApplyTaskbarTooltipPlacement_Original,
            ApplyTaskbarTooltipPlacement_Hook,
            true,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookSystemTrayDllSymbols(HMODULE module);

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HookScope hookScope;

    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading ||
        (g_taskbarViewDllLoaded && g_systemTrayDllLoaded)) {
        return module;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            if (HookTaskbarViewDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    if (!g_systemTrayDllLoaded && GetModuleHandle(L"SystemTray.dll") == module) {
        if (!g_systemTrayDllLoaded.exchange(true)) {
            if (HookSystemTrayDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    return module;
}

struct ForEachTaskbarWindowContext {
    void (*proc)(HWND);
};

BOOL CALLBACK ForEachTaskbarWindowEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<ForEachTaskbarWindowContext*>(lParam);

    DWORD processId = 0;
    WCHAR className[64];
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId() ||
        !GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (!IsTaskbarClassName(className)) {
        return TRUE;
    }

    context->proc(hWnd);
    return TRUE;
}

void ForEachTaskbarWindow(void (*proc)(HWND)) {
    ForEachTaskbarWindowContext context{proc};
    EnumWindows(ForEachTaskbarWindowEnumProc,
                reinterpret_cast<LPARAM>(&context));
}

struct FindFirstTaskbarWindowContext {
    HWND result;
};

BOOL CALLBACK FindFirstTaskbarWindowEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<FindFirstTaskbarWindowContext*>(lParam);

    DWORD processId = 0;
    WCHAR className[64];
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId() ||
        !GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        !IsTaskbarClassName(className)) {
        return TRUE;
    }

    context->result = hWnd;
    return FALSE;
}

HWND FindFirstTaskbarWindow() {
    FindFirstTaskbarWindowContext context{nullptr};
    EnumWindows(FindFirstTaskbarWindowEnumProc,
                reinterpret_cast<LPARAM>(&context));
    return context.result;
}

void KickTaskbarRelayout() {
    ForEachTaskbarWindow([](HWND hWnd) {
        SendMessageTimeoutW(hWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                            0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, nullptr);
    });
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RunFromWindowThreadContext {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* context =
                        reinterpret_cast<RunFromWindowThreadContext*>(
                            cwp->lParam);
                    context->proc(context->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadContext context{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&context));

    UnhookWindowsHookEx(hook);
    return true;
}

using PositionContextMenuPopup_t = void(WINAPI*)(void const* popup,
                                                 Grid const* anchorGrid,
                                                 double layoutParameter1,
                                                 int taskbarLocation,
                                                 double layoutParameter2);
PositionContextMenuPopup_t PositionContextMenuPopup_Original;
void WINAPI PositionContextMenuPopup_Hook(void const* popup,
                                          Grid const* anchorGrid,
                                          double layoutParameter1,
                                          int taskbarLocation,
                                          double layoutParameter2) {
    HookScope hookScope;

    PositionContextMenuPopup_Original(popup, anchorGrid, layoutParameter1,
                                      taskbarLocation, layoutParameter2);

    if (taskbarLocation != 3 || !popup || !anchorGrid) {
        return;
    }

    try {
        auto popupObject =
            *static_cast<winrt::Windows::UI::Xaml::Controls::Primitives::
                             Popup const*>(popup);
        if (!popupObject) {
            return;
        }

        double menuWidthDip = 0.0;
        double menuHeightDip = 0.0;
        if (auto child = popupObject.Child()) {
            if (auto childElement = child.try_as<FrameworkElement>()) {
                menuWidthDip = childElement.ActualWidth();
                menuHeightDip = childElement.ActualHeight();
            }
        }

        XamlRoot xamlRoot = (*anchorGrid).XamlRoot();
        HWND taskbarWnd = FindTaskbarWindowForXamlRoot(xamlRoot);
        double scale = GetTaskbarWindowScale(taskbarWnd);
        if (scale <= 0) {
            scale = 1.0;
        }

        HWND islandHwnd = FindIslandWindow(taskbarWnd);
        if (menuWidthDip <= 1.0 || menuHeightDip <= 1.0 || !islandHwnd) {
            return;
        }

        UIElement contentElement = nullptr;
        if (auto root = xamlRoot.Content()) {
            contentElement = root.try_as<UIElement>();
        }
        winrt::Windows::Foundation::Rect anchorRect{};
        if (!contentElement ||
            !TryGetElementRectInContent(*anchorGrid, contentElement,
                                        anchorRect)) {
            return;
        }

        POINT islandOrigin{};
        ClientToScreen(islandHwnd, &islandOrigin);
        POINT cursor{};
        GetCursorPos(&cursor);

        double cursorDipX = (cursor.x - islandOrigin.x) / scale;
        double cursorDipY = (cursor.y - islandOrigin.y) / scale;
        if (cursorDipX < anchorRect.X ||
            cursorDipX > anchorRect.X + anchorRect.Width ||
            cursorDipY < anchorRect.Y ||
            cursorDipY > anchorRect.Y + anchorRect.Height) {
            return;
        }
        popupObject.HorizontalOffset(cursorDipX - menuWidthDip / 2.0);
        popupObject.VerticalOffset(-(menuHeightDip + 4.0));
    } catch (...) {
    }
}

bool HookSystemTrayDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {
            {
                LR"(void __cdecl ContextMenuPositionHelper::PositionContextMenuPopup(struct winrt::Windows::UI::Xaml::Controls::Primitives::Popup const &,struct winrt::Windows::UI::Xaml::Controls::Grid const &,double,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,double))",
                LR"(public: static void __cdecl ContextMenuPositionHelper::PositionContextMenuPopup(struct winrt::Windows::UI::Xaml::Controls::Primitives::Popup const &,struct winrt::Windows::UI::Xaml::Controls::Grid const &,double,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,double))",
            },
            &PositionContextMenuPopup_Original,
            PositionContextMenuPopup_Hook,
            true,
        },
    };

    if (!HookSymbols(module, systemTrayDllHooks,
                     ARRAYSIZE(systemTrayDllHooks))) {
        Wh_Log(L"HookSymbols failed for SystemTray.dll");
        return false;
    }

    return true;
}

void KickTaskbarHeightUpdate() {
    ForEachTaskbarWindow([](HWND hWnd) {
        HWND rebarWnd =
            FindWindowExW(hWnd, nullptr, L"ReBarWindow32", nullptr);
        if (!rebarWnd) {
            return;
        }

        HWND taskSwitchWnd =
            FindWindowExW(rebarWnd, nullptr, L"MSTaskSwWClass", nullptr);
        if (taskSwitchWnd) {
            SendMessageW(taskSwitchWnd, 0x452, 3, 0);
        }
    });
}

void ApplyToAllTaskbarsNow() {
    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, ApplyNowOnTaskbarThread, hWnd);
    });
}

void ReapplyOnCurrentThread(PVOID) {
    DWORD threadId = GetCurrentThreadId();
    struct ReapplyTarget {
        winrt::weak_ref<FrameworkElement> frame;
        HWND taskbarWnd = nullptr;
    };
    std::vector<ReapplyTarget> targets;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto const& applied : *g_appliedTaskbarStates) {
            if (applied.threadId == threadId) {
                targets.push_back({applied.frame, applied.taskbarWnd});
            }
        }
    }

    for (auto const& target : targets) {
        if (auto element = target.frame.get()) {
            try {
                ApplyToFrame(element, target.taskbarWnd);
            } catch (...) {
            }
        }
    }
}

void RestoreOnCurrentThread(PVOID) {
    DWORD threadId = GetCurrentThreadId();
    RestoreTaskbarToolTipCornerRadiiOnCurrentThread();
    std::vector<AppliedTaskbarState> pending;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        auto& appliedStates = *g_appliedTaskbarStates;
        for (auto it = appliedStates.begin(); it != appliedStates.end();) {
            if (it->threadId == threadId) {
                pending.push_back(std::move(*it));
                it = appliedStates.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<AppliedTaskbarState> failed;
    for (auto& applied : pending) {
        bool restored = false;
        try {
            restored = RestoreFrame(applied);
        } catch (...) {
        }

        if (!restored) {
            failed.push_back(std::move(applied));
        }
    }

    if (!failed.empty() && !g_unloading) {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto& applied : failed) {
            g_appliedTaskbarStates->push_back(std::move(applied));
        }
    }
}

void ReleaseAppliedTaskbarStatesOnCurrentThread(PVOID) {
    const DWORD threadId = GetCurrentThreadId();
    std::vector<AppliedTaskbarState> pending;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        auto& appliedStates = *g_appliedTaskbarStates;
        for (auto it = appliedStates.begin(); it != appliedStates.end();) {
            if (it->threadId == threadId) {
                pending.push_back(std::move(*it));
                it = appliedStates.erase(it);
            } else {
                ++it;
            }
        }
    }

    pending.clear();

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        if (g_appliedTaskbarStates->empty()) {
            g_appliedTaskbarStates.reset();
        }
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_hostKind = GetHostKind();

    // The Quick Settings panel is positioned by the process which hosts it, so
    // the mod is loaded into that process as well and adjusts the position
    // there. Only the taskbar process needs the separate MoveWindow path.
    if (g_hostKind == HostKind::ControlCenter) {
        WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                       &SetWindowPos_Original);
        return TRUE;
    }

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                   &MoveWindow_Original);

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Taskbar symbols are not available, the taskbar window won't "
               L"be found by the mod");
    }

    // The LoadLibraryExW hook is needed as long as one of the modules isn't
    // loaded yet, so that its symbols can be hooked when it is.
    bool delayLoadingNeeded = false;

    if (HMODULE systemTrayModule = GetModuleHandle(L"SystemTray.dll")) {
        g_systemTrayDllLoaded = true;
        HookSystemTrayDllSymbols(systemTrayModule);
    } else {
        delayLoadingNeeded = true;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    ApplyToAllTaskbarsNow();
    KickTaskbarRelayout();
    KickTaskbarHeightUpdate();

    InstallControlCenterHook();

    RepositionControlCenterWindows();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_settingsVersion.fetch_add(1);
    ResetLoggedFlags();

    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    if (g_unloading) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, RestoreOnCurrentThread, nullptr);
    });

    ApplyToAllTaskbarsNow();
    KickTaskbarHeightUpdate();

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, ReapplyOnCurrentThread, nullptr);
    });

    InstallControlCenterHook();
    RepositionControlCenterWindows();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopLayoutCalibrationTimer();

    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, RestoreOnCurrentThread, nullptr);
    });

    if (HWND releaseTaskbarWnd = FindFirstTaskbarWindow()) {
        RunFromWindowThread(releaseTaskbarWnd,
                            ReleaseAppliedTaskbarStatesOnCurrentThread, nullptr);
    }

    KickTaskbarRelayout();
    KickTaskbarHeightUpdate();
    RemoveControlCenterHook();
}

void Wh_ModUninit() {
    // Hooks are already removed at this point, but another thread may still be
    // running one of the hook functions, so wait for it to finish before the
    // mod's code is unloaded.
    while (g_hookCallCounter.load() > 0) {
        Sleep(100);
    }
}

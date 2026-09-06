// ==WindhawkMod==
// @id              windows-11-better-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers the taskbar and system tray as a single unit
// @version         1.9
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -ldwmapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Windows 11 Better Ultrawide Taskbar

Centers the taskbar buttons and system tray as a single unit on the Windows 11
taskbar. The mod dynamically adjusts positioning when windows are opened or
closed.

## Requirements

**Important:** This mod requires the Windows taskbar alignment to be set to
**Center** in System Settings:
- Open Settings → Personalization → Taskbar → Taskbar behaviors
- Set "Taskbar alignment" to **Center**

![Screenshot](https://i.imgur.com/j8a5deC.png)

## Settings

- **Minimum total width**: Set a minimum width for the centered group. If the
  actual width is smaller, extra spacing is added to maintain the minimum.
- **Gap**: Additional spacing (in pixels) between taskbar buttons and system
  tray. Positive values push them apart, negative values pull them closer.
- **Maximum taskbar width**: Limit the taskbar buttons area width. Set to 0 to
  disable (no maximum).
- **Apply to all monitors**: In Compatibility settings, choose whether the mod
  also affects secondary monitor taskbars.

## Unified Background (Dock Panel Effect)

The mod creates a unified background that spans both the taskbar buttons and
system tray, creating a seamless dock panel effect. This feature:

- Removes individual backgrounds from RootGrid and SystemTrayFrameGrid
- Applies a single background element with customizable styling:
  - **Background**: Semi-transparent tint with optional blur effect
  - **Corner Radius**: Rounded corners (default: 10px)
  - **Border**: Gradient border brush for a glass-like appearance
  - **Border Thickness**: Configurable border width (default: 1px)

The unified background is enabled by default and can be configured through
the settings.

## Multi-Monitor Support

The mod can apply to all monitor taskbars. Use the compatibility setting to
control whether secondary monitor taskbars are affected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Layout:
    - centerEnabled: true
      $name: Enable center positioning
      $description: >-
        Reposition taskbar buttons and tray as a centered group.
    - minTotalWidth: 0
    - gap: 0
    - maxTotalWidth: 0
    - taskbarHeight: 0
    - taskbarEdgeSpacing: 0
- Divider:
    - enabled: true
    - style: hairline
      $name: Divider style
      $options:
      - hairline: Hairline (bright)
      - soft: Soft (subtle)
    - width: 1
    - marginY: 8
    - paddingX: 4
- StartButton:
    - leftPadding: 8
- UnifiedBackground:
    - enabled: true
    - style: dockGlass
      $name: Background style
      $options:
      - dockGlass: Dock Glass (blurred)
      - flatTint: Flat Tint (solid)
    - cornerRadius: 10
    - blurAmount: 10
    - tintColor: 0x10ffffff
    - borderThickness: 1
- Animation:
    - enabled: true
      $name: Enable animations
      $description: >-
        Animate taskbar, system tray, divider, and unified background movements.
    - durationMs: 200
      $name: Animation duration (ms)
      $description: >-
        Duration of movement animations in milliseconds. Larger values feel smoother
        but slower.
    - curve: springLazy
      $name: Animation curve
      $description: >-
        Motion profile used by taskbar animations.
      $options:
      - cubic: In-Out Expo (cubic-bezier(0.9, 0, 0.1, 1))
      - springLazy: Spring Lazy
      - springRelaxed: Spring Relaxed
- Compatibility:
    - applyToAllMonitors: true
      $name: Apply to all monitors
      $description: >-
        Apply the mod to secondary monitor taskbars in addition to the primary one.
    - start11CompatibilityFixEnabled: true
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cwctype>
#include <functional>
#include <string_view>
#include <stdio.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <dwmapi.h> // For DwmGetWindowAttribute
#include <tlhelp32.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Composition;

////////////////////////////////////////////////////////////////////////////////
// Helper to create WindhawkBlur brush via XAML
// clang-format off

Media::Brush CreateWindhawkBlurBrush(int blurAmount, UINT32 tintColorARGB)
{
    try {
        // Parse ARGB color
        uint8_t a = static_cast<uint8_t>((tintColorARGB >> 24) & 0xFF);
        uint8_t r = static_cast<uint8_t>((tintColorARGB >> 16) & 0xFF);
        uint8_t g = static_cast<uint8_t>((tintColorARGB >> 8) & 0xFF);
        uint8_t b = static_cast<uint8_t>(tintColorARGB & 0xFF);

        // Build XAML string for WindhawkBlur (requires xmlns for XamlReader::Load)
        wchar_t xaml[512];
        swprintf_s(xaml,
            LR"(<WindhawkBlur xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation")"
            LR"( BlurAmount="%d" TintColor="#%02X%02X%02X%02X" />)",
            blurAmount, a, r, g, b);

        // Parse XAML to create the brush
        auto brush = Markup::XamlReader::Load(xaml).as<Media::Brush>();
        Wh_Log(L"Created WindhawkBlur: blurAmount=%d, tintColor=#%08X", blurAmount, tintColorARGB);
        return brush;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create WindhawkBlur: %08X - %s", ex.code().value, ex.message().c_str());
        // Fallback to AcrylicBrush if WindhawkBlur is not available
        try {
            uint8_t a = static_cast<uint8_t>((tintColorARGB >> 24) & 0xFF);
            uint8_t r = static_cast<uint8_t>((tintColorARGB >> 16) & 0xFF);
            uint8_t g = static_cast<uint8_t>((tintColorARGB >> 8) & 0xFF);
            uint8_t b = static_cast<uint8_t>(tintColorARGB & 0xFF);
            double tintOpacity = static_cast<double>(a) / 255.0;

            wchar_t xaml[512];
            swprintf_s(xaml,
                LR"(<AcrylicBrush xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation")"
                LR"( TintColor="#%02X%02X%02X%02X" TintOpacity="%f" FallbackColor="#%02X%02X%02X%02X" />)",
                a, r, g, b, tintOpacity, a, r, g, b);

            auto brush = Markup::XamlReader::Load(xaml).as<Media::Brush>();
            Wh_Log(L"Fallback to AcrylicBrush: tintColor=#%08X", tintColorARGB);
            return brush;
        } catch (...) {
            Wh_Log(L"AcrylicBrush fallback also failed");
            return nullptr;
        }
    }
}

// clang-format on
////////////////////////////////////////////////////////////////////////////////

// Unique names for custom visual elements
constexpr PCWSTR kUnifiedBackgroundName = L"WindhawkUnifiedTaskbarBackground";
constexpr PCWSTR kDividerName = L"WindhawkUltrawideTaskbarDivider";

enum AnimationCurveType
{
    kAnimationCurveCubic = 0,
    kAnimationCurveSpringLazy = 1,
    kAnimationCurveSpringRelaxed = 2,
};

enum DividerStyleType
{
    kDividerStyleHairline = 0,
    kDividerStyleSoft = 1,
};

enum UnifiedBackgroundStyleType
{
    kUnifiedBackgroundStyleDockGlass = 0,
    kUnifiedBackgroundStyleFlatTint = 1,
};

// Settings
struct
{
    bool centerPositionEnabled;
    int minTotalWidth;
    int gap;
    int maxTotalWidth;
    int taskbarHeight;
    int taskbarEdgeSpacing;
    // Divider settings
    bool dividerEnabled;
    int dividerStyle;
    int dividerWidth;
    int dividerMarginY;
    int dividerPaddingX;
    // Start button padding
    int startButtonLeftPadding;
    // Unified background settings
    bool unifiedBackgroundEnabled;
    int unifiedBackgroundStyle;
    int unifiedBackgroundCornerRadius;
    int unifiedBackgroundBlurAmount;
    UINT32 unifiedBackgroundTintColor;  // ARGB format
    int unifiedBackgroundBorderThickness;
    // Animation settings
    bool animationEnabled;
    int animationDurationMs;
    int animationCurve;
    bool applyToAllMonitors;
    // Start11 compatibility
    bool start11CompatibilityFixEnabled;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Re-entry guard to prevent feedback loop when ApplyStyle modifies UI
std::atomic<bool> g_isApplyingStyle{false};

// Time-based debounce to prevent rapid oscillation after ApplyStyle.
// Stored per-root in TaskbarInstanceState::lastApplyTick.
static const DWORD APPLY_STYLE_DEBOUNCE_MS = 50; // Debounce window in ms

// Value stability tracking - detect oscillation and wait for stable value
static double g_lastSystemTrayExtent = 0;
static double g_lastLastSystemTrayExtent = 0; // Previous previous value to detect oscillation
static int g_oscillationCount = 0;

// Delay values used by per-root poll scheduler.
static const DWORD DELAYED_REFRESH_MS = 200; // Delay before re-checking width
static const DWORD CLOSE_DELAY_MS = 250; // Slight delay after window close

// Cached XamlRoot from the SystemTrayExtent hook (safe path).
// Used as a stable fallback when root discovery is temporarily unavailable.
static winrt::weak_ref<XamlRoot> g_cachedPrimaryXamlRoot;
struct TrackedTaskbarRoot {
    winrt::weak_ref<XamlRoot> root;
    bool isPrimary;
};
static TrackedTaskbarRoot g_trackedTaskbarRoots[8] = {};
static int g_trackedTaskbarRootsCount = 0;
static winrt::weak_ref<XamlRoot> g_appliedXamlRoots[8] = {};
static int g_appliedXamlRootsCount = 0;

// Polling timer to detect taskbar button changes.
// LayoutUpdated and SizeChanged both fail:
//   - SizeChanged: repeater is stretched, doesn't change size when buttons change
//   - LayoutUpdated: cross-monitor mismatch causes infinite feedback loop
// A simple polling timer is the most robust approach.
static UINT_PTR g_pollTimerId = 0;
static const DWORD POLL_INTERVAL_MS = 250;

struct TaskbarInstanceState {
    winrt::weak_ref<XamlRoot> root;
    double previousTaskbarButtonsWidth = 0;
    double stableVisualCenterOffset = 0;
    bool hasStableVisualCenterOffset = false;
    bool firstLayoutComplete = false;
    ULONGLONG animationSuppressDirectUntil = 0;
    ULONGLONG lastSignificantWidthChangeTick = 0;
    double lastPolledButtonsWidth = 0;
    ULONGLONG delayedRefreshDueTick = 0;
    ULONGLONG closeRefreshDueTick = 0;
    uint64_t refreshGeneration = 0;
    uint64_t delayedRefreshGeneration = 0;
    uint64_t closeRefreshGeneration = 0;
    DWORD lastApplyTick = 0;
};
using RootLayoutState = TaskbarInstanceState;
static TaskbarInstanceState g_rootLayoutStates[8] = {};

struct TaskbarInstance {
    XamlRoot root = nullptr;
    bool isPrimary = false;
    TaskbarInstanceState* state = nullptr;
};

struct TaskbarVisualElements {
    FrameworkElement taskbarFrame = nullptr;
    FrameworkElement rootGrid = nullptr;
    FrameworkElement taskbarFrameRepeater = nullptr;
    FrameworkElement systemTrayFrame = nullptr;
    FrameworkElement systemTrayFrameGrid = nullptr;
};

struct TaskbarSnapshot {
    TaskbarInstance instance;
    FrameworkElement rootContent = nullptr;
    TaskbarVisualElements elements;
    double systemTrayWidth = 0;
    double taskbarButtonsWidth = 0;
    double taskbarButtonsDividerWidth = 0;
    double taskbarButtonsVisualCenterOffset = 0;
    bool hasTaskbarButtonsVisualCenterOffset = false;
    double screenWidth = 0;
};

struct LayoutPlan {
    double halfGap = 0;
    double effectiveTotalWidth = 0;
    double taskbarTransformX = 0;
    double systemTrayTransformX = 0;
    double taskbarOverlayShiftX = 0;
};

static double ClampVisualCenterOffsetForSnapshot(const TaskbarSnapshot& snapshot)
{
    if (!snapshot.hasTaskbarButtonsVisualCenterOffset)
    {
        return 0.0;
    }

    double visualCenterOffset = snapshot.taskbarButtonsVisualCenterOffset;
    if (!std::isfinite(visualCenterOffset))
    {
        return 0.0;
    }

    return (std::max)(-400.0, (std::min)(400.0, visualCenterOffset));
}

enum class RefreshReason
{
    SettingsApply,
    PollOpen,
    DelayedRefresh,
    CloseDelayRefresh,
    SystemTrayExtent,
};

typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    ULONG cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef struct _ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    DWORD GradientColor;
    int AnimationId;
} ACCENT_POLICY;

using SetWindowCompositionAttribute_t = BOOL (WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static SetWindowCompositionAttribute_t g_setWindowCompositionAttribute_Original = nullptr;
static bool g_setWindowCompositionHookInstalled = false;
static WCHAR g_primaryTaskbandClassName[64] = L"";

// Forward declaration
void CALLBACK ButtonWidthPollTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void ApplyTransformOffset(FrameworkElement element, double offsetX, bool animate,
                          double fromOffsetX, int delayMs);
bool ApplyStyle(XamlRoot xamlRoot, bool restoreNativeOnly = false);
bool RequestApplyForTaskbarRoot(XamlRoot xamlRoot, RefreshReason reason);
RootLayoutState* GetRootLayoutState(XamlRoot xamlRoot, bool create);

static void AppendAgentDebugLog(const char* hypothesisId,
                                const char* location,
                                const char* message,
                                const char* runId,
                                const char* dataJson)
{
    FILE* file = nullptr;
#if defined(_MSC_VER)
    fopen_s(&file, "d:\\Workspace\\windhawk-mods\\debug-6ca260.log", "a");
#else
    file = fopen("d:\\Workspace\\windhawk-mods\\debug-6ca260.log", "a");
#endif
    if (!file)
    {
        return;
    }

    long long timestampMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    static unsigned long long s_logSequence = 0;
    unsigned long long sequence = ++s_logSequence;

    char line[2048];
    int written = snprintf(
        line,
        sizeof(line),
        "{\"sessionId\":\"6ca260\",\"id\":\"log_%lld_%llu\",\"timestamp\":%lld,"
        "\"location\":\"%s\",\"message\":\"%s\",\"runId\":\"%s\","
        "\"hypothesisId\":\"%s\",\"data\":%s}",
        timestampMs,
        sequence,
        timestampMs,
        location,
        message,
        runId,
        hypothesisId,
        dataJson ? dataJson : "{}");
    if (written > 0)
    {
        fwrite(line, 1, static_cast<size_t>(written), file);
        fwrite("\n", 1, 1, file);
    }

    fclose(file);
}

HWND FindCurrentProcessTaskbarWnd()
{
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND *>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// Detect if the taskbar is positioned at the top of the screen
bool IsTaskbarAtTop()
{
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd)
    {
        return false;
    }

    RECT taskbarRect;
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect))
    {
        return false;
    }

    HMONITOR hMonitor = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMonitor, &mi))
    {
        return false;
    }

    // If taskbar center is in the upper half of the monitor, it's at top
    int taskbarCenterY = (taskbarRect.top + taskbarRect.bottom) / 2;
    int monitorCenterY = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;

    return taskbarCenterY < monitorCenterY;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback)
{
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child)
        {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child))
        {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name)
{
    return EnumChildElements(element, [name](FrameworkElement child)
                             { return child.Name() == name; });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className)
{
    return EnumChildElements(element, [className](FrameworkElement child)
                             { return winrt::get_class_name(child) == className; });
}

// Recursive search: find a descendant element by name anywhere in the visual tree
FrameworkElement FindDescendantByName(DependencyObject root, PCWSTR name)
{
    int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(root, i);
        if (!child) continue;

        auto fe = child.try_as<FrameworkElement>();
        if (fe && fe.Name() == name) return fe;

        auto result = FindDescendantByName(child, name);
        if (result) return result;
    }
    return nullptr;
}

bool IsSameXamlRoot(XamlRoot lhs, XamlRoot rhs)
{
    return lhs && rhs && winrt::get_abi(lhs) == winrt::get_abi(rhs);
}

void TrackAppliedXamlRoot(XamlRoot xamlRoot)
{
    if (!xamlRoot)
    {
        return;
    }

    void* rootPtr = winrt::get_abi(xamlRoot);
    int reusableSlot = -1;
    for (int i = 0; i < g_appliedXamlRootsCount; i++)
    {
        auto existingRoot = g_appliedXamlRoots[i].get();
        if (!existingRoot)
        {
            if (reusableSlot < 0)
            {
                reusableSlot = i;
            }
            continue;
        }

        if (winrt::get_abi(existingRoot) == rootPtr)
        {
            return;
        }
    }

    if (reusableSlot >= 0)
    {
        g_appliedXamlRoots[reusableSlot] = winrt::make_weak(xamlRoot);
        return;
    }

    if (g_appliedXamlRootsCount < static_cast<int>(ARRAYSIZE(g_appliedXamlRoots)))
    {
        g_appliedXamlRoots[g_appliedXamlRootsCount++] = winrt::make_weak(xamlRoot);
    }
}

void TrackTaskbarXamlRoot(XamlRoot xamlRoot, bool isPrimary)
{
    if (!xamlRoot)
    {
        return;
    }

    void* rootPtr = winrt::get_abi(xamlRoot);
    int reusableSlot = -1;
    for (int i = 0; i < g_trackedTaskbarRootsCount; i++)
    {
        auto existingRoot = g_trackedTaskbarRoots[i].root.get();
        if (!existingRoot)
        {
            if (reusableSlot < 0)
            {
                reusableSlot = i;
            }
            continue;
        }

        if (winrt::get_abi(existingRoot) == rootPtr)
        {
            g_trackedTaskbarRoots[i].isPrimary =
                g_trackedTaskbarRoots[i].isPrimary || isPrimary;
            return;
        }
    }

    int slot = reusableSlot;
    if (slot < 0)
    {
        if (g_trackedTaskbarRootsCount >= static_cast<int>(ARRAYSIZE(g_trackedTaskbarRoots)))
        {
            return;
        }

        slot = g_trackedTaskbarRootsCount++;
    }

    g_trackedTaskbarRoots[slot].root = winrt::make_weak(xamlRoot);
    g_trackedTaskbarRoots[slot].isPrimary = isPrimary;
}

bool IsPrimaryTaskbarXamlRoot(XamlRoot xamlRoot)
{
    if (!xamlRoot)
    {
        return false;
    }

    auto cachedPrimary = g_cachedPrimaryXamlRoot.get();
    if (IsSameXamlRoot(xamlRoot, cachedPrimary))
    {
        return true;
    }

    for (int i = 0; i < g_trackedTaskbarRootsCount; i++)
    {
        auto trackedRoot = g_trackedTaskbarRoots[i].root.get();
        if (!trackedRoot)
        {
            continue;
        }

        if (IsSameXamlRoot(xamlRoot, trackedRoot))
        {
            return g_trackedTaskbarRoots[i].isPrimary;
        }
    }

    return false;
}

bool IsTaskbarXamlRootEnabled(XamlRoot xamlRoot)
{
    if (g_settings.applyToAllMonitors)
    {
        return true;
    }

    return IsPrimaryTaskbarXamlRoot(xamlRoot);
}

void ForEachConfiguredTaskbarXamlRoot(const std::function<void(XamlRoot)>& callback)
{
    if (!callback)
    {
        return;
    }

    if (g_settings.applyToAllMonitors)
    {
        bool invoked = false;
        for (int i = 0; i < g_trackedTaskbarRootsCount; i++)
        {
            auto trackedRoot = g_trackedTaskbarRoots[i].root.get();
            if (!trackedRoot)
            {
                continue;
            }

            callback(trackedRoot);
            invoked = true;
        }

        if (!invoked)
        {
            auto cachedPrimary = g_cachedPrimaryXamlRoot.get();
            if (cachedPrimary)
            {
                callback(cachedPrimary);
            }
        }

        return;
    }

    auto cachedPrimary = g_cachedPrimaryXamlRoot.get();
    if (cachedPrimary)
    {
        callback(cachedPrimary);
        return;
    }

    for (int i = 0; i < g_trackedTaskbarRootsCount; i++)
    {
        if (!g_trackedTaskbarRoots[i].isPrimary)
        {
            continue;
        }

        auto trackedRoot = g_trackedTaskbarRoots[i].root.get();
        if (!trackedRoot)
        {
            continue;
        }

        callback(trackedRoot);
        return;
    }
}

const wchar_t* RefreshReasonName(RefreshReason reason)
{
    switch (reason)
    {
    case RefreshReason::SettingsApply:
        return L"settings-apply";
    case RefreshReason::PollOpen:
        return L"poll-open";
    case RefreshReason::DelayedRefresh:
        return L"delayed-refresh";
    case RefreshReason::CloseDelayRefresh:
        return L"close-delay-refresh";
    case RefreshReason::SystemTrayExtent:
        return L"system-tray-extent";
    default:
        return L"unknown";
    }
}

bool RequestApplyForTaskbarRoot(XamlRoot xamlRoot, RefreshReason reason)
{
    if (!xamlRoot || !IsTaskbarXamlRootEnabled(xamlRoot))
    {
        return false;
    }

    bool applied = ApplyStyle(xamlRoot);
    // #region agent log
    {
        auto* layoutState = GetRootLayoutState(xamlRoot, false);
        char data[512];
        snprintf(
            data,
            sizeof(data),
            "{\"reason\":%d,\"applied\":%d,\"isPrimary\":%d,"
            "\"delayedRefreshDueTick\":%llu,\"closeRefreshDueTick\":%llu,"
            "\"refreshGeneration\":%llu}",
            static_cast<int>(reason),
            applied ? 1 : 0,
            IsPrimaryTaskbarXamlRoot(xamlRoot) ? 1 : 0,
            layoutState ? static_cast<unsigned long long>(layoutState->delayedRefreshDueTick)
                        : 0ULL,
            layoutState ? static_cast<unsigned long long>(layoutState->closeRefreshDueTick)
                        : 0ULL,
            layoutState ? static_cast<unsigned long long>(layoutState->refreshGeneration)
                        : 0ULL);
        AppendAgentDebugLog("H16",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:RequestApplyForTaskbarRoot",
                            "Refresh apply request outcome",
                            "refresh-flow-audit",
                            data);
    }
    // #endregion
    if (!applied)
    {
        Wh_Log(L"ApplyStyle skipped for reason=%s", RefreshReasonName(reason));
        return false;
    }

    auto* layoutState = GetRootLayoutState(xamlRoot, false);
    if (layoutState)
    {
        layoutState->lastApplyTick = GetTickCount();
    }
    return true;
}

namespace RefreshScheduler
{
void ScheduleDelayedRefresh(TaskbarInstanceState* state, ULONGLONG nowTick)
{
    if (!state)
    {
        return;
    }

    state->refreshGeneration++;
    state->delayedRefreshGeneration = state->refreshGeneration;
    state->delayedRefreshDueTick =
        nowTick + static_cast<ULONGLONG>(DELAYED_REFRESH_MS);
    state->closeRefreshDueTick = 0;
    // #region agent log
    char data[384];
    snprintf(
        data,
        sizeof(data),
        "{\"reason\":%d,\"nowTick\":%llu,\"delayedRefreshDueTick\":%llu,"
        "\"closeRefreshDueTick\":%llu,\"refreshGeneration\":%llu,"
        "\"delayedRefreshGeneration\":%llu}",
        static_cast<int>(RefreshReason::DelayedRefresh),
        static_cast<unsigned long long>(nowTick),
        static_cast<unsigned long long>(state->delayedRefreshDueTick),
        static_cast<unsigned long long>(state->closeRefreshDueTick),
        static_cast<unsigned long long>(state->refreshGeneration),
        static_cast<unsigned long long>(state->delayedRefreshGeneration));
    AppendAgentDebugLog("H16",
                        "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::ScheduleDelayedRefresh",
                        "Scheduled delayed refresh",
                        "refresh-flow-audit",
                        data);
    // #endregion
}

void ScheduleCloseRefresh(TaskbarInstanceState* state, ULONGLONG nowTick)
{
    if (!state)
    {
        return;
    }

    state->refreshGeneration++;
    state->closeRefreshGeneration = state->refreshGeneration;
    state->closeRefreshDueTick =
        nowTick + static_cast<ULONGLONG>(CLOSE_DELAY_MS);
    state->delayedRefreshDueTick = 0;
    // #region agent log
    char data[384];
    snprintf(
        data,
        sizeof(data),
        "{\"reason\":%d,\"nowTick\":%llu,\"closeRefreshDueTick\":%llu,"
        "\"delayedRefreshDueTick\":%llu,\"refreshGeneration\":%llu,"
        "\"closeRefreshGeneration\":%llu}",
        static_cast<int>(RefreshReason::CloseDelayRefresh),
        static_cast<unsigned long long>(nowTick),
        static_cast<unsigned long long>(state->closeRefreshDueTick),
        static_cast<unsigned long long>(state->delayedRefreshDueTick),
        static_cast<unsigned long long>(state->refreshGeneration),
        static_cast<unsigned long long>(state->closeRefreshGeneration));
    AppendAgentDebugLog("H16",
                        "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::ScheduleCloseRefresh",
                        "Scheduled close refresh",
                        "refresh-flow-audit",
                        data);
    // #endregion
}

void ClearPending(TaskbarInstanceState* state)
{
    if (!state)
    {
        return;
    }

    state->delayedRefreshDueTick = 0;
    state->closeRefreshDueTick = 0;
}

bool TryApplyScheduledForRoot(XamlRoot root,
                              TaskbarInstanceState* state,
                              ULONGLONG nowTick)
{
    if (!root || !state)
    {
        return false;
    }

    if (state->closeRefreshDueTick != 0 && nowTick >= state->closeRefreshDueTick)
    {
        uint64_t dueGeneration = state->closeRefreshGeneration;
        if (dueGeneration != state->refreshGeneration)
        {
            state->closeRefreshDueTick = 0;
            // #region agent log
            char data[384];
            snprintf(
                data,
                sizeof(data),
                "{\"reason\":%d,\"nowTick\":%llu,\"dueGeneration\":%llu,"
                "\"refreshGeneration\":%llu}",
                static_cast<int>(RefreshReason::CloseDelayRefresh),
                static_cast<unsigned long long>(nowTick),
                static_cast<unsigned long long>(dueGeneration),
                static_cast<unsigned long long>(state->refreshGeneration));
            AppendAgentDebugLog("H16",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::TryApplyScheduledForRoot",
                                "Skipped close refresh due to stale generation",
                                "refresh-flow-audit",
                                data);
            // #endregion
        }
        else if (!g_isApplyingStyle.exchange(true))
        {
            state->closeRefreshDueTick = 0;
            bool applied =
                RequestApplyForTaskbarRoot(root, RefreshReason::CloseDelayRefresh);
            g_isApplyingStyle = false;
            // #region agent log
            char data[384];
            snprintf(
                data,
                sizeof(data),
                "{\"reason\":%d,\"nowTick\":%llu,\"applied\":%d,"
                "\"dueGeneration\":%llu,\"refreshGeneration\":%llu}",
                static_cast<int>(RefreshReason::CloseDelayRefresh),
                static_cast<unsigned long long>(nowTick),
                applied ? 1 : 0,
                static_cast<unsigned long long>(dueGeneration),
                static_cast<unsigned long long>(state->refreshGeneration));
            AppendAgentDebugLog("H16",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::TryApplyScheduledForRoot",
                                "Executed close refresh due pass",
                                "refresh-flow-audit",
                                data);
            // #endregion
            return applied;
        }
    }

    if (state->delayedRefreshDueTick != 0 &&
        nowTick >= state->delayedRefreshDueTick)
    {
        uint64_t dueGeneration = state->delayedRefreshGeneration;
        if (dueGeneration != state->refreshGeneration)
        {
            state->delayedRefreshDueTick = 0;
            // #region agent log
            char data[384];
            snprintf(
                data,
                sizeof(data),
                "{\"reason\":%d,\"nowTick\":%llu,\"dueGeneration\":%llu,"
                "\"refreshGeneration\":%llu}",
                static_cast<int>(RefreshReason::DelayedRefresh),
                static_cast<unsigned long long>(nowTick),
                static_cast<unsigned long long>(dueGeneration),
                static_cast<unsigned long long>(state->refreshGeneration));
            AppendAgentDebugLog("H16",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::TryApplyScheduledForRoot",
                                "Skipped delayed refresh due to stale generation",
                                "refresh-flow-audit",
                                data);
            // #endregion
        }
        else if (!g_isApplyingStyle.exchange(true))
        {
            state->delayedRefreshDueTick = 0;
            bool applied =
                RequestApplyForTaskbarRoot(root, RefreshReason::DelayedRefresh);
            g_isApplyingStyle = false;
            // #region agent log
            char data[384];
            snprintf(
                data,
                sizeof(data),
                "{\"reason\":%d,\"nowTick\":%llu,\"applied\":%d,"
                "\"dueGeneration\":%llu,\"refreshGeneration\":%llu}",
                static_cast<int>(RefreshReason::DelayedRefresh),
                static_cast<unsigned long long>(nowTick),
                applied ? 1 : 0,
                static_cast<unsigned long long>(dueGeneration),
                static_cast<unsigned long long>(state->refreshGeneration));
            AppendAgentDebugLog("H16",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:RefreshScheduler::TryApplyScheduledForRoot",
                                "Executed delayed refresh due pass",
                                "refresh-flow-audit",
                                data);
            // #endregion
            return applied;
        }
    }

    return false;
}
} // namespace RefreshScheduler

RootLayoutState* GetRootLayoutState(XamlRoot xamlRoot, bool create)
{
    if (!xamlRoot)
    {
        return nullptr;
    }

    void* rootPtr = winrt::get_abi(xamlRoot);
    int reusableSlot = -1;

    for (int i = 0; i < static_cast<int>(ARRAYSIZE(g_rootLayoutStates)); i++)
    {
        auto existingRoot = g_rootLayoutStates[i].root.get();
        if (!existingRoot)
        {
            if (reusableSlot < 0)
            {
                reusableSlot = i;
            }
            continue;
        }

        if (winrt::get_abi(existingRoot) == rootPtr)
        {
            return &g_rootLayoutStates[i];
        }
    }

    if (!create || reusableSlot < 0)
    {
        return nullptr;
    }

    g_rootLayoutStates[reusableSlot] = {};
    g_rootLayoutStates[reusableSlot].root = winrt::make_weak(xamlRoot);
    return &g_rootLayoutStates[reusableSlot];
}

void ResetRootLayoutStates()
{
    for (auto& state : g_rootLayoutStates)
    {
        state = {};
    }
}

void ResetTrackedTaskbarRoots()
{
    g_cachedPrimaryXamlRoot = {};
    g_trackedTaskbarRootsCount = 0;
    for (auto& tracked : g_trackedTaskbarRoots)
    {
        tracked = {};
    }

    g_appliedXamlRootsCount = 0;
    for (auto& applied : g_appliedXamlRoots)
    {
        applied = {};
    }
}

// #region agent log - debug visual tree dumper
static UINT_PTR g_debugDumpTimerId = 0;

void DebugLogLine(const wchar_t* msg)
{
    (void)msg;
}

void DebugLogLineA(const char* msg)
{
    (void)msg;
}

// Structured debug log for hypothesis-driven runtime evidence.
// dataJson must be a valid JSON object string (e.g. "{}" or "{\"k\":1}").
void DebugLogHypothesisA(const char* runId,
                         const char* hypothesisId,
                         const char* location,
                         const char* message,
                         const char* dataJson)
{
    (void)runId;
    (void)hypothesisId;
    (void)location;
    (void)message;
    (void)dataJson;
}

std::string DescribeBrush(Media::Brush brush)
{
    if (!brush) return "null";
    try {
        auto solid = brush.try_as<Media::SolidColorBrush>();
        if (solid) {
            auto c = solid.Color();
            char buf[64];
            snprintf(buf, sizeof(buf), "Solid(#%02X%02X%02X%02X,op=%.2f)",
                     c.A, c.R, c.G, c.B, brush.Opacity());
            return buf;
        }
        auto className = winrt::get_class_name(brush);
        char buf[256];
        snprintf(buf, sizeof(buf), "%ls(op=%.2f)",
                 className.c_str(), brush.Opacity());
        return buf;
    } catch (...) { return "error"; }
}

void DumpVisualTree(FrameworkElement element, int depth, int maxDepth = 5)
{
    if (!element || depth > maxDepth) return;

    try {
        auto className = winrt::get_class_name(element);
        auto name = element.Name();
        double w = element.ActualWidth();
        double h = element.ActualHeight();
        double opacity = element.Opacity();
        auto vis = element.Visibility();

        // Check for background on various types, with brush details
        std::string bgInfo = "none";
        auto control = element.try_as<Controls::Control>();
        if (control) {
            auto bg = control.Background();
            if (bg) bgInfo = "ctrl:" + DescribeBrush(bg);
        }
        auto panel = element.try_as<Controls::Panel>();
        if (panel) {
            auto bg = panel.Background();
            if (bg) bgInfo = "panel:" + DescribeBrush(bg);
        }
        auto border = element.try_as<Controls::Border>();
        if (border) {
            auto bg = border.Background();
            if (bg) bgInfo = "border:" + DescribeBrush(bg);
        }

        char buf[2048];
        std::string indent(depth * 2, ' ');
        snprintf(buf, sizeof(buf),
                 "%s[%d] class=%ls name=%ls w=%.1f h=%.1f opacity=%.2f vis=%d bg=%s",
                 indent.c_str(), depth,
                 className.c_str(), name.c_str(),
                 w, h, opacity, (int)vis, bgInfo.c_str());
        DebugLogLineA(buf);

        // Recurse into children
        int childCount = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < childCount; i++) {
            auto child = Media::VisualTreeHelper::GetChild(element, i)
                             .try_as<FrameworkElement>();
            if (child) {
                DumpVisualTree(child, depth + 1, maxDepth);
            }
        }
    } catch (...) {
        DebugLogLine(L"DumpVisualTree exception");
    }
}

struct EnumXamlWindowsData {
    DWORD processId;
};

BOOL CALLBACK EnumXamlChildWindowsProc(HWND hWnd, LPARAM lParam)
{
    WCHAR className[256] = {};
    GetClassName(hWnd, className, ARRAYSIZE(className));

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    BOOL visible = IsWindowVisible(hWnd);

    char buf[512];
    snprintf(buf, sizeof(buf), "  Child HWND=%p class=%ls visible=%d rect=(%ld,%ld,%ld,%ld) size=%dx%d",
             (void*)hWnd, className, (int)visible,
             rc.left, rc.top, rc.right, rc.bottom, w, h);
    DebugLogLineA(buf);

    return TRUE;
}

BOOL CALLBACK EnumProcessWindowsProc(HWND hWnd, LPARAM lParam)
{
    auto* data = reinterpret_cast<EnumXamlWindowsData*>(lParam);

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != data->processId) return TRUE;

    WCHAR className[256] = {};
    GetClassName(hWnd, className, ARRAYSIZE(className));

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    BOOL visible = IsWindowVisible(hWnd);

    // Only log visible windows or taskbar-related windows
    if (!visible && w == 0 && h == 0) return TRUE;

    char buf[512];
    snprintf(buf, sizeof(buf), "HWND=%p class=%ls visible=%d rect=(%ld,%ld,%ld,%ld) size=%dx%d",
             (void*)hWnd, className, (int)visible,
             rc.left, rc.top, rc.right, rc.bottom, w, h);
    DebugLogLineA(buf);

    // Enumerate child windows for taskbar and XAML-related windows
    if (wcsstr(className, L"Shell_TrayWnd") ||
        wcsstr(className, L"TopLevelWindowForOverflowXamlIsland") ||
        wcsstr(className, L"Xaml") ||
        wcsstr(className, L"TaskListThumbnailWnd") ||
        wcsstr(className, L"MSTaskSwWClass") ||
        (visible && h > 0 && h <= 200))
    {
        EnumChildWindows(hWnd, EnumXamlChildWindowsProc, 0);
    }

    return TRUE;
}

void DumpWindowDetails(HWND hWnd, const char* label)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    BYTE alpha = 255;
    DWORD flags = 0;
    GetLayeredWindowAttributes(hWnd, nullptr, &alpha, &flags);

    char buf[512];
    snprintf(buf, sizeof(buf),
             "%s HWND=%p visible=%d rect=(%ld,%ld,%ld,%ld) style=0x%08lX exStyle=0x%08lX cloaked=%d alpha=%d flags=0x%lX",
             label, (void*)hWnd, (int)IsWindowVisible(hWnd),
             rc.left, rc.top, rc.right, rc.bottom,
             style, exStyle, (int)cloaked, (int)alpha, flags);
    DebugLogLineA(buf);

    // Enumerate children
    HWND hChild = GetWindow(hWnd, GW_CHILD);
    while (hChild) {
        WCHAR className[256] = {};
        GetClassName(hChild, className, ARRAYSIZE(className));
        RECT crc;
        GetWindowRect(hChild, &crc);
        snprintf(buf, sizeof(buf), "  child class=%ls visible=%d rect=(%ld,%ld,%ld,%ld)",
                 className, (int)IsWindowVisible(hChild),
                 crc.left, crc.top, crc.right, crc.bottom);
        DebugLogLineA(buf);
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
}

void CALLBACK DebugDumpTimerProc(HWND, UINT, UINT_PTR idEvent, DWORD)
{
    KillTimer(nullptr, idEvent);
    g_debugDumpTimerId = 0;

    // Find and dump the XamlExplorerHostIslandWindow
    HWND hIsland = FindWindowW(L"XamlExplorerHostIslandWindow", nullptr);
    if (hIsland) {
        DumpWindowDetails(hIsland, "XamlExplorerHostIslandWindow");
    } else {
        DebugLogLine(L"XamlExplorerHostIslandWindow not found");
    }

    // Also check the Xaml_WindowedPopupClass
    HWND hPopup = FindWindowW(L"Xaml_WindowedPopupClass", nullptr);
    if (hPopup) {
        DumpWindowDetails(hPopup, "Xaml_WindowedPopupClass");
    }

    // Enumerate ALL windows to see full state
    DebugLogLine(L"=== PROCESS WINDOWS DUMP ===");
    EnumXamlWindowsData data;
    data.processId = GetCurrentProcessId();
    EnumWindows(EnumProcessWindowsProc, reinterpret_cast<LPARAM>(&data));
    DebugLogLine(L"=== END PROCESS WINDOWS DUMP ===");
}
// #endregion

// Create the LinearGradientBrush for the unified background border via XAML
Media::Brush CreateUnifiedBorderBrush()
{
    try {
        // Use XamlReader to create the brush with proper ThemeResource support
        const wchar_t* xaml =
            LR"(<LinearGradientBrush xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation")"
            LR"( StartPoint="0,0" EndPoint="1,1" Opacity="0.75">)"
            LR"(<GradientStop Color="#80ffffff" Offset="0.0" />)"
            LR"(<GradientStop Color="{ThemeResource SurfaceStrokeColorDefault}" Offset="0.6" />)"
            LR"(<GradientStop Color="#80ffffff" Offset="1.0" />)"
            LR"(</LinearGradientBrush>)";

        auto brush = Markup::XamlReader::Load(xaml).as<Media::Brush>();
        Wh_Log(L"Created LinearGradientBrush with ThemeResource");
        return brush;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create LinearGradientBrush: %08X - %s", ex.code().value, ex.message().c_str());
        // Fallback to programmatic brush without ThemeResource
        Media::LinearGradientBrush brush;
        brush.StartPoint({0, 0});
        brush.EndPoint({1, 1});
        brush.Opacity(0.75);

        Media::GradientStop stop1;
        winrt::Windows::UI::Color color1{128, 255, 255, 255}; // #80ffffff
        stop1.Color(color1);
        stop1.Offset(0.0);
        brush.GradientStops().Append(stop1);

        Media::GradientStop stop2;
        winrt::Windows::UI::Color color2{60, 255, 255, 255}; // Approximation
        stop2.Color(color2);
        stop2.Offset(0.55);
        brush.GradientStops().Append(stop2);

        Media::GradientStop stop3;
        winrt::Windows::UI::Color color3{128, 255, 255, 255}; // #80ffffff
        stop3.Color(color3);
        stop3.Offset(1.0);
        brush.GradientStops().Append(stop3);

        return brush;
    }
}

// Clear the background and border properties from a Grid element
void ClearGridBackground(FrameworkElement gridElement)
{
    if (!gridElement)
    {
        return;
    }

    try
    {
        auto grid = gridElement.try_as<Controls::Grid>();
        if (grid)
        {
            // Clear Background
            grid.Background(nullptr);
            Wh_Log(L"Cleared background from Grid: %s", gridElement.Name().c_str());
        }

        // For Grid, border properties are typically set via a parent Border
        // or through visual states. We need to find and clear those.
        // Check if this is a Control-derived type that has BorderBrush
        auto control = gridElement.try_as<Controls::Control>();
        if (control)
        {
            control.BorderBrush(nullptr);
            control.BorderThickness(Thickness{0, 0, 0, 0});
            Wh_Log(L"Cleared border from Control: %s", gridElement.Name().c_str());
        }
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error clearing grid background: %08X - %s",
               ex.code().value, ex.message().c_str());
    }
}

void RestoreGridBackground(FrameworkElement gridElement)
{
    if (!gridElement)
    {
        return;
    }

    try
    {
        auto grid = gridElement.try_as<Controls::Grid>();
        if (grid)
        {
            // Clear local value so the default style/theme value is restored.
            grid.ClearValue(Controls::Panel::BackgroundProperty());
            Wh_Log(L"Restored Grid background binding: %s", gridElement.Name().c_str());
        }

        auto control = gridElement.try_as<Controls::Control>();
        if (control)
        {
            control.ClearValue(Controls::Control::BorderBrushProperty());
            control.ClearValue(Controls::Control::BorderThicknessProperty());
            Wh_Log(L"Restored border bindings from Control: %s", gridElement.Name().c_str());
        }
    }
    catch (winrt::hresult_error const& ex)
    {
        Wh_Log(L"Error restoring grid background: %08X - %s",
               ex.code().value, ex.message().c_str());
    }
}

// Find the unified background element in a grid
FrameworkElement FindUnifiedBackground(Controls::Grid grid)
{
    if (!grid)
    {
        return nullptr;
    }

    auto children = grid.Children();
    for (uint32_t i = 0; i < children.Size(); i++)
    {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        if (child && child.Name() == kUnifiedBackgroundName)
        {
            return child;
        }
    }

    return nullptr;
}

// Remove the unified background element from a grid
void RemoveUnifiedBackground(Controls::Grid grid)
{
    if (!grid)
    {
        return;
    }

    auto existingBackground = FindUnifiedBackground(grid);
    if (existingBackground)
    {
        uint32_t index;
        if (grid.Children().IndexOf(existingBackground.as<UIElement>(), index))
        {
            grid.Children().RemoveAt(index);
            Wh_Log(L"Removed unified background from grid");
        }
    }
}

const wchar_t* GetAnimationCurveName(int curve)
{
    switch (curve)
    {
    case kAnimationCurveSpringLazy:
        return L"Spring Lazy";
    case kAnimationCurveSpringRelaxed:
        return L"Spring Relaxed";
    case kAnimationCurveCubic:
    default:
        return L"cubic";
    }
}

int ParseAnimationCurveSetting()
{
    int curve = kAnimationCurveSpringLazy;
    PCWSTR curveSetting = Wh_GetStringSetting(L"Animation.curve");
    if (curveSetting)
    {
        if (_wcsicmp(curveSetting, L"cubic") == 0 || wcscmp(curveSetting, L"0") == 0)
        {
            curve = kAnimationCurveCubic;
        }
        else if (_wcsicmp(curveSetting, L"springLazy") == 0 || wcscmp(curveSetting, L"1") == 0)
        {
            curve = kAnimationCurveSpringLazy;
        }
        else if (_wcsicmp(curveSetting, L"springRelaxed") == 0 ||
                 _wcsicmp(curveSetting, L"backEase") == 0 ||
                 wcscmp(curveSetting, L"2") == 0)
        {
            curve = kAnimationCurveSpringRelaxed;
        }

        Wh_FreeStringSetting(curveSetting);
    }

    return curve;
}

const wchar_t* GetDividerStyleName(int style)
{
    switch (style)
    {
    case kDividerStyleSoft:
        return L"soft";
    case kDividerStyleHairline:
    default:
        return L"hairline";
    }
}

int ParseDividerStyleSetting()
{
    int style = kDividerStyleHairline;
    PCWSTR styleSetting = Wh_GetStringSetting(L"Divider.style");
    if (styleSetting)
    {
        if (_wcsicmp(styleSetting, L"soft") == 0 || wcscmp(styleSetting, L"1") == 0)
        {
            style = kDividerStyleSoft;
        }
        else
        {
            style = kDividerStyleHairline;
        }

        Wh_FreeStringSetting(styleSetting);
    }

    return style;
}

const wchar_t* GetUnifiedBackgroundStyleName(int style)
{
    switch (style)
    {
    case kUnifiedBackgroundStyleFlatTint:
        return L"flatTint";
    case kUnifiedBackgroundStyleDockGlass:
    default:
        return L"dockGlass";
    }
}

int ParseUnifiedBackgroundStyleSetting()
{
    int style = kUnifiedBackgroundStyleDockGlass;
    PCWSTR styleSetting = Wh_GetStringSetting(L"UnifiedBackground.style");
    if (styleSetting)
    {
        if (_wcsicmp(styleSetting, L"flatTint") == 0 ||
            _wcsicmp(styleSetting, L"solid") == 0 ||
            wcscmp(styleSetting, L"1") == 0)
        {
            style = kUnifiedBackgroundStyleFlatTint;
        }
        else
        {
            style = kUnifiedBackgroundStyleDockGlass;
        }

        Wh_FreeStringSetting(styleSetting);
    }

    return style;
}

void ConfigureCubicBezierKeyframes(Media::Animation::DoubleAnimationUsingKeyFrames animation,
                                   double fromValue,
                                   double toValue,
                                   int durationMs)
{
    auto startKeyframe = Media::Animation::EasingDoubleKeyFrame();
    startKeyframe.KeyTime(Media::Animation::KeyTimeHelper::FromTimeSpan(
        winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(0))));
    startKeyframe.Value(fromValue);
    animation.KeyFrames().Append(startKeyframe);

    auto endKeyframe = Media::Animation::SplineDoubleKeyFrame();
    endKeyframe.KeyTime(Media::Animation::KeyTimeHelper::FromTimeSpan(
        winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(durationMs))));
    endKeyframe.Value(toValue);

    Media::Animation::KeySpline keySpline;
    keySpline.ControlPoint1(winrt::Windows::Foundation::Point{0.9f, 0.0f});
    keySpline.ControlPoint2(winrt::Windows::Foundation::Point{0.1f, 1.0f});
    endKeyframe.KeySpline(keySpline);

    animation.KeyFrames().Append(endKeyframe);
}

void ConfigureSpringLazyKeyframes(Media::Animation::DoubleAnimationUsingKeyFrames animation,
                                  double fromValue,
                                  double toValue,
                                  int durationMs)
{
    constexpr std::array<double, 19> kKeyframeTimes = {
        0.000, 0.011, 0.023, 0.049, 0.073, 0.143, 0.165, 0.186, 0.207, 0.228,
        0.250, 0.273, 0.306, 0.343, 0.495, 0.586, 0.652, 0.845, 1.000};
    constexpr std::array<double, 19> kKeyframeValues = {
        0.000, 0.008, 0.034, 0.134, 0.264, 0.683, 0.797, 0.890, 0.967, 1.027,
        1.073, 1.104, 1.123, 1.119, 1.018, 0.988, 0.985, 1.000, 1.000};

    double delta = toValue - fromValue;

    for (size_t i = 0; i < kKeyframeTimes.size(); i++)
    {
        auto keyframe = Media::Animation::EasingDoubleKeyFrame();
        int keyframeMs = static_cast<int>(std::lround(durationMs * kKeyframeTimes[i]));
        keyframe.KeyTime(Media::Animation::KeyTimeHelper::FromTimeSpan(
            winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(keyframeMs))));
        keyframe.Value(fromValue + delta * kKeyframeValues[i]);
        animation.KeyFrames().Append(keyframe);
    }
}

void ConfigureSpringRelaxedKeyframes(Media::Animation::DoubleAnimationUsingKeyFrames animation,
                                     double fromValue,
                                     double toValue,
                                     int durationMs)
{
    constexpr std::array<double, 101> kRawValues = {
        1.0, 1.0909451808077864, 1.2574785555149028, 1.4900327640080993,
        1.779009671137812, 2.114920131703241, 2.4885089012192787, 2.8908643252336335,
        3.3135126919147027, 3.7484973622499984, 4.188442998332861, 4.626605392167698,
        5.056907554898406, 5.473962859445483, 5.873086138648565, 6.250293726878308,
        6.602293496692375, 6.926465984675932, 7.220837723523389, 7.484047902228258,
        7.71530946460646, 7.914365730016737, 8.081443580836577, 8.217204210795433,
        8.322692368439537, 8.399284962548851, 8.44863982293418, 8.472645332322688,
        8.473371564512632, 8.453023482058448, 8.413896664735878, 8.358335959111125,
        8.288697360749016, 8.207313364862333, 8.116461949316891, 8.018339286523586,
        7.915036218401287, 7.808518471693097, 7.700610539755744, 7.592983111701194,
        7.487143890532496, 7.384431608670124, 7.286013021914904, 7.192882641267533,
        7.105864945886509, 7.0256188095238015, 6.952643866692239, 6.887288543215153,
        6.829759478283343, 6.780132071273892, 6.738361895934233, 6.704296736663815,
        6.6776890160970375, 6.658208399575652, 6.645454379981032, 6.638968665378931,
        6.638247211636447, 6.64275176225289, 6.651920777781088, 6.665179657111377,
        6.681950172286137, 6.701659057179392, 6.723745708116831, 6.747668971162012,
        6.77291300622059, 6.798992232213043, 6.825455370262641, 6.851888613091928,
        6.877917958595302, 6.903210753858108, 6.927476502745854, 6.9504669956306175,
        6.971975823912417, 6.991837344801267, 7.009925163433189, 7.026150199891714,
        7.040458408193972, 7.052828212880703, 7.063267726629146, 7.071811809394465,
        7.078519026086822, 7.083468555812721, 7.086757101353469, 7.088495842918409,
        7.088807475388573, 7.087823363344345, 7.085680843227943, 7.082520697100935,
        7.078484817682825, 7.07371407975645, 7.0683464286482005, 7.062515192377361,
        7.056347620252579, 7.049963647200738, 7.043474879963397, 7.0369837985006125,
        7.030583163507732, 7.0243556188776415, 7.018373476224518, 7.012698667215547,
        7.007382848421193};

    constexpr double kStart = kRawValues.front();
    constexpr double kEnd = kRawValues.back();
    constexpr double kRange = kEnd - kStart;
    double delta = toValue - fromValue;

    for (size_t i = 0; i < kRawValues.size(); i++)
    {
        auto keyframe = Media::Animation::EasingDoubleKeyFrame();
        double progress = static_cast<double>(i) / static_cast<double>(kRawValues.size() - 1);
        int keyframeMs = static_cast<int>(std::lround(durationMs * progress));
        keyframe.KeyTime(Media::Animation::KeyTimeHelper::FromTimeSpan(
            winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(keyframeMs))));

        double normalized = (kRawValues[i] - kStart) / kRange;
        keyframe.Value(fromValue + delta * normalized);
        animation.KeyFrames().Append(keyframe);
    }
}

// Apply animated width change to an element
void ApplyWidthAnimation(FrameworkElement element, double targetWidth,
                         bool animate = false, double fromWidth = NAN,
                         int delayMs = 0)
{
    try
    {
        double startWidth = std::isnan(fromWidth) ? element.ActualWidth() : fromWidth;

        bool useAnimation = animate && g_settings.animationEnabled &&
                           g_settings.animationDurationMs > 0 && !g_unloading;

        if (useAnimation && startWidth > 0)
        {
            // Clear any existing animated value so our animation takes effect
            element.ClearValue(FrameworkElement::WidthProperty());

            // If we have an explicit "from" value, first set the width to that
            if (!std::isnan(fromWidth))
            {
                element.Width(fromWidth);
            }

            Media::Animation::Storyboard storyboard;

            if (g_settings.animationCurve == kAnimationCurveSpringLazy ||
                g_settings.animationCurve == kAnimationCurveCubic ||
                g_settings.animationCurve == kAnimationCurveSpringRelaxed)
            {
                Media::Animation::DoubleAnimationUsingKeyFrames animation;
                animation.Duration(DurationHelper::FromTimeSpan(
                    winrt::Windows::Foundation::TimeSpan(
                        std::chrono::milliseconds(g_settings.animationDurationMs))));
                if (g_settings.animationCurve == kAnimationCurveSpringLazy)
                {
                    ConfigureSpringLazyKeyframes(animation, startWidth, targetWidth,
                                                 g_settings.animationDurationMs);
                }
                else if (g_settings.animationCurve == kAnimationCurveCubic)
                {
                    ConfigureCubicBezierKeyframes(animation, startWidth, targetWidth,
                                                  g_settings.animationDurationMs);
                }
                else
                {
                    ConfigureSpringRelaxedKeyframes(animation, startWidth, targetWidth,
                                                    g_settings.animationDurationMs);
                }
                animation.EnableDependentAnimation(true);
                if (delayMs > 0)
                {
                    animation.BeginTime(winrt::Windows::Foundation::TimeSpan(
                        std::chrono::milliseconds(delayMs)));
                }
                storyboard.Children().Append(animation);
                Media::Animation::Storyboard::SetTarget(animation, element);
                Media::Animation::Storyboard::SetTargetProperty(animation, L"Width");
            }

            // Start the animation
            storyboard.Begin();
        }
        else
        {
            // Clear any existing animated value so our new value takes effect
            element.ClearValue(FrameworkElement::WidthProperty());
            element.Width(targetWidth);
        }
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error applying width animation: %08X - %s", ex.code().value, ex.message().c_str());
        // #region agent log
        char data[320];
        snprintf(
            data,
            sizeof(data),
            "{\"targetWidth\":%.4f,\"fromWidth\":%.4f,\"animate\":%d,\"delayMs\":%d,"
            "\"exceptionCode\":%ld}",
            targetWidth,
            fromWidth,
            animate ? 1 : 0,
            delayMs,
            static_cast<long>(ex.code().value));
        AppendAgentDebugLog("H25",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyWidthAnimation",
                            "Width animation exception",
                            "crash-audit",
                            data);
        // #endregion
        element.ClearValue(FrameworkElement::WidthProperty());
        element.Width(targetWidth);
    }
}

// Create or update the unified background element
// On 24H2, we insert into the RootGrid (inside TaskbarFrame) instead of
// the root content Grid, because the root content Grid crashes when it
// has more than its expected 2 children (TaskbarFrame + SystemTrayFrame).
// The background is sized to cover both buttons and the system tray area,
// using a TranslateTransform to position it correctly.
void UpdateUnifiedBackground(FrameworkElement rootGrid,
                             double taskbarButtonsWidth,
                             double systemTrayWidth,
                             double halfGap,
                             double screenWidth,
                             double taskbarOverlayShiftX,
                             bool add,
                             bool animate = false,
                             double fromTaskbarButtonsWidth = NAN,
                             int delayMs = 0)
{
    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid)
    {
        Wh_Log(L"UpdateUnifiedBackground: rootGrid is not a Grid");
        return;
    }

    if (!add)
    {
        RemoveUnifiedBackground(grid);
        return;
    }

    if (!g_settings.unifiedBackgroundEnabled)
    {
        RemoveUnifiedBackground(grid);
        return;
    }

    // Early return if XAML tree is not fully initialized
    if (taskbarButtonsWidth == 0 && systemTrayWidth == 0)
    {
        Wh_Log(L"Skipping UpdateUnifiedBackground - XAML tree not fully initialized");
        return;
    }

    // Calculate the total width of the unified background
    double leftPadding = static_cast<double>(g_settings.startButtonLeftPadding);
    double totalWidth = leftPadding + taskbarButtonsWidth + systemTrayWidth + halfGap * 2;

    // Calculate "from" width if we have a previous taskbar width
    double fromTotalWidth = NAN;
    if (!std::isnan(fromTaskbarButtonsWidth))
    {
        fromTotalWidth = leftPadding + fromTaskbarButtonsWidth + systemTrayWidth + halfGap * 2;
    }

    // Find or create the unified background in the RootGrid
    auto existingBackground = FindUnifiedBackground(grid);
    Controls::Border background{nullptr};
    bool isNewBackground = false;

    if (existingBackground)
    {
        background = existingBackground.try_as<Controls::Border>();
    }

    if (!background)
    {
        // Create new unified background
        background = Controls::Border();
        background.Name(kUnifiedBackgroundName);
        isNewBackground = true;

        // Insert at position 0 (behind BackgroundControl and everything else)
        grid.Children().InsertAt(0, background);
        Wh_Log(L"Created unified background element in RootGrid");
    }

    try
    {
        // Apply styling only when newly created (brushes, corners, borders are expensive)
        if (isNewBackground)
        {
            // Corner radius
            CornerRadius cr;
            cr.TopLeft = cr.TopRight = cr.BottomLeft = cr.BottomRight =
                static_cast<double>(g_settings.unifiedBackgroundCornerRadius);
            background.CornerRadius(cr);

            // Border thickness
            double bt = static_cast<double>(g_settings.unifiedBackgroundBorderThickness);
            background.BorderThickness(Thickness{bt, bt, bt, bt});

            winrt::Windows::UI::Color tintColor;
            tintColor.A = static_cast<uint8_t>((g_settings.unifiedBackgroundTintColor >> 24) & 0xFF);
            tintColor.R = static_cast<uint8_t>((g_settings.unifiedBackgroundTintColor >> 16) & 0xFF);
            tintColor.G = static_cast<uint8_t>((g_settings.unifiedBackgroundTintColor >> 8) & 0xFF);
            tintColor.B = static_cast<uint8_t>(g_settings.unifiedBackgroundTintColor & 0xFF);

            if (g_settings.unifiedBackgroundStyle == kUnifiedBackgroundStyleFlatTint)
            {
                Media::SolidColorBrush bgBrush;
                bgBrush.Color(tintColor);
                background.Background(bgBrush);

                auto borderColor = tintColor;
                borderColor.A = static_cast<uint8_t>(
                    (std::max)(0x28, (std::min)(0x80, static_cast<int>(borderColor.A))));
                Media::SolidColorBrush borderBrush;
                borderBrush.Color(borderColor);
                background.BorderBrush(borderBrush);
            }
            else
            {
                // Border brush (gradient)
                background.BorderBrush(CreateUnifiedBorderBrush());

                // Background brush (blur or fallback)
                auto blurBrush = CreateWindhawkBlurBrush(
                    g_settings.unifiedBackgroundBlurAmount,
                    g_settings.unifiedBackgroundTintColor);
                if (blurBrush)
                {
                    background.Background(blurBrush);
                }
                else
                {
                    Media::SolidColorBrush bgBrush;
                    bgBrush.Color(tintColor);
                    background.Background(bgBrush);
                }
            }

            // Alignment, height, and edge spacing
            background.HorizontalAlignment(HorizontalAlignment::Center);
            background.VerticalAlignment(VerticalAlignment::Stretch);
            if (g_settings.taskbarHeight > 0 || g_settings.taskbarEdgeSpacing > 0)
            {
                bool isAtTop = IsTaskbarAtTop();
                double edgeSpacing = static_cast<double>(g_settings.taskbarEdgeSpacing);
                Thickness bgMargin = isAtTop
                    ? Thickness{0, edgeSpacing, 0, 0}
                    : Thickness{0, 0, 0, edgeSpacing};
                background.Margin(bgMargin);

                if (g_settings.taskbarHeight > 0)
                {
                    background.Height(static_cast<double>(g_settings.taskbarHeight));
                    background.VerticalAlignment(isAtTop ? VerticalAlignment::Bottom : VerticalAlignment::Top);
                }
            }

            Wh_Log(L"Created unified background: cornerRadius=%d, borderThickness=%d",
                   g_settings.unifiedBackgroundCornerRadius,
                   g_settings.unifiedBackgroundBorderThickness);
        }

        // Always update width and position (these change on every window open/close)
        ApplyWidthAnimation(background, totalWidth, animate && !isNewBackground, fromTotalWidth, delayMs);

        // Position in taskbar-frame local space. TaskbarFrame's own transform
        // already applies -(halfGap + tray/2), so add halfGap here to avoid
        // an extra left bias of halfGap on the unified background.
        // The left padding extends the background to the left of the Start button
        double buttonCenter = screenWidth / 2.0 + halfGap;
        double bgLeft = buttonCenter - taskbarButtonsWidth / 2.0 - halfGap - leftPadding;
        double bgCenter = bgLeft + totalWidth / 2.0;
        double bgTranslateX = bgCenter - screenWidth / 2.0 + taskbarOverlayShiftX;

        Media::TranslateTransform bgTransform;
        bgTransform.X(bgTranslateX);
        background.RenderTransform(bgTransform);

        // #region agent log
        char data[512];
        snprintf(
            data,
            sizeof(data),
            "{\"taskbarButtonsWidth\":%.4f,\"systemTrayWidth\":%.4f,"
            "\"halfGap\":%.4f,\"taskbarOverlayShiftX\":%.4f,"
            "\"bgTranslateX\":%.4f,\"totalWidth\":%.4f}",
            taskbarButtonsWidth,
            systemTrayWidth,
            halfGap,
            taskbarOverlayShiftX,
            bgTranslateX,
            totalWidth);
        AppendAgentDebugLog("H21",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:UpdateUnifiedBackground",
                            "Unified background transform alignment",
                            "overlay-alignment-audit",
                            data);
        // #endregion
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error updating unified background: %08X - %s",
               ex.code().value, ex.message().c_str());
        // #region agent log
        char data[320];
        snprintf(
            data,
            sizeof(data),
            "{\"taskbarButtonsWidth\":%.4f,\"systemTrayWidth\":%.4f,"
            "\"halfGap\":%.4f,\"screenWidth\":%.4f,\"taskbarOverlayShiftX\":%.4f,"
            "\"exceptionCode\":%ld}",
            taskbarButtonsWidth,
            systemTrayWidth,
            halfGap,
            screenWidth,
            taskbarOverlayShiftX,
            static_cast<long>(ex.code().value));
        AppendAgentDebugLog("H25",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:UpdateUnifiedBackground",
                            "Unified background update exception",
                            "crash-audit",
                            data);
        // #endregion
    }
}

// Find the divider element in a grid by name
FrameworkElement FindDivider(Controls::Grid grid)
{
    if (!grid)
    {
        return nullptr;
    }

    auto children = grid.Children();
    for (uint32_t i = 0; i < children.Size(); i++)
    {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        if (child && child.Name() == kDividerName)
        {
            return child;
        }
    }
    return nullptr;
}

// Remove the divider element from a grid
void RemoveDivider(Controls::Grid grid)
{
    if (!grid)
    {
        return;
    }

    auto existingDivider = FindDivider(grid);
    if (existingDivider)
    {
        uint32_t index;
        if (grid.Children().IndexOf(existingDivider.as<UIElement>(), index))
        {
            grid.Children().RemoveAt(index);
            Wh_Log(L"Removed divider from grid");
        }
    }
}

// Update or create the divider element between taskbar buttons and system tray.
// Uses the same TranslateTransform overlay pattern as the unified background.
void UpdateDivider(FrameworkElement rootGrid, bool add,
                   double taskbarButtonsWidth, double halfGap,
                   double screenWidth,
                   double taskbarOverlayShiftX,
                   bool animate = false,
                   double fromTaskbarButtonsWidth = NAN,
                   int delayMs = 0)
{
    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid)
    {
        return;
    }

    if (!add || !g_settings.dividerEnabled)
    {
        RemoveDivider(grid);
        return;
    }

    // Early return if XAML tree is not fully initialized
    if (taskbarButtonsWidth == 0 && screenWidth == 0)
    {
        return;
    }

    double dividerExtra =
        static_cast<double>(g_settings.dividerWidth) / 2.0 +
        static_cast<double>(g_settings.dividerPaddingX);

    double fromDividerX = NAN;
    if (!std::isnan(fromTaskbarButtonsWidth))
    {
        fromDividerX = fromTaskbarButtonsWidth / 2.0 + halfGap + dividerExtra;
    }

    // Find or create the divider
    auto existingDivider = FindDivider(grid);
    Controls::Border divider{nullptr};
    bool isNewDivider = false;

    if (existingDivider)
    {
        divider = existingDivider.try_as<Controls::Border>();
    }

    if (!divider)
    {
        divider = Controls::Border();
        divider.Name(kDividerName);
        isNewDivider = true;

        // Insert at index 1 (above background at 0, but behind actual content)
        uint32_t insertIndex = (std::min)(static_cast<uint32_t>(1), grid.Children().Size());
        grid.Children().InsertAt(insertIndex, divider);
        Wh_Log(L"Created divider element in RootGrid");
    }

    try
    {
        // Apply styling only when newly created
        if (isNewDivider)
        {
            divider.Width(static_cast<double>(g_settings.dividerWidth));
            divider.VerticalAlignment(VerticalAlignment::Stretch);
            divider.HorizontalAlignment(HorizontalAlignment::Center);

            // Vertical margin + edge spacing
            double marginY = static_cast<double>(g_settings.dividerMarginY);
            double edgeSpacing = static_cast<double>(g_settings.taskbarEdgeSpacing);
            bool isAtTop = IsTaskbarAtTop();
            Thickness divMargin = isAtTop
                ? Thickness{0, marginY + edgeSpacing, 0, marginY}
                : Thickness{0, marginY, 0, marginY + edgeSpacing};
            divider.Margin(divMargin);

            // Divider brush style
            winrt::Windows::UI::Color divColor;
            divColor.A = (g_settings.dividerStyle == kDividerStyleSoft) ? 0x35 : 0x60;
            divColor.R = 0xFF;
            divColor.G = 0xFF;
            divColor.B = 0xFF;
            Media::SolidColorBrush divBrush;
            divBrush.Color(divColor);
            divider.Background(divBrush);

            if (g_settings.dividerStyle == kDividerStyleSoft)
            {
                CornerRadius divCorner;
                divCorner.TopLeft = divCorner.TopRight = divCorner.BottomLeft =
                    divCorner.BottomRight = 1.0;
                divider.CornerRadius(divCorner);
            }

            // Custom height matching taskbar height if set
            if (g_settings.taskbarHeight > 0)
            {
                divider.Height(static_cast<double>(g_settings.taskbarHeight) - 2.0 * marginY);
                divider.VerticalAlignment(isAtTop ? VerticalAlignment::Bottom : VerticalAlignment::Top);
            }

            Wh_Log(L"Styled divider: width=%d, marginY=%d",
                   g_settings.dividerWidth, g_settings.dividerMarginY);
        }

        // Position: divider sits at the right edge of the taskbar buttons + halfGap
        // In taskbar-frame local space (parent frame transform handles global shift).
        double dividerX =
            taskbarButtonsWidth / 2.0 + halfGap + dividerExtra + taskbarOverlayShiftX;
        // TranslateTransform is relative to the element's center (HorizontalAlignment::Center)
        // so dividerX is already the offset from center

        ApplyTransformOffset(divider, dividerX, animate && !isNewDivider, fromDividerX, delayMs);

        // #region agent log
        char data[512];
        snprintf(
            data,
            sizeof(data),
            "{\"taskbarButtonsWidth\":%.4f,\"halfGap\":%.4f,"
            "\"dividerExtra\":%.4f,\"taskbarOverlayShiftX\":%.4f,"
            "\"dividerX\":%.4f}",
            taskbarButtonsWidth,
            halfGap,
            dividerExtra,
            taskbarOverlayShiftX,
            dividerX);
        AppendAgentDebugLog("H21",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:UpdateDivider",
                            "Divider transform alignment",
                            "overlay-alignment-audit",
                            data);
        // #endregion
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error updating divider: %08X - %s",
               ex.code().value, ex.message().c_str());
        // #region agent log
        char data[320];
        snprintf(
            data,
            sizeof(data),
            "{\"taskbarButtonsWidth\":%.4f,\"halfGap\":%.4f,\"screenWidth\":%.4f,"
            "\"taskbarOverlayShiftX\":%.4f,\"exceptionCode\":%ld}",
            taskbarButtonsWidth,
            halfGap,
            screenWidth,
            taskbarOverlayShiftX,
            static_cast<long>(ex.code().value));
        AppendAgentDebugLog("H25",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:UpdateDivider",
                            "Divider update exception",
                            "crash-audit",
                            data);
        // #endregion
    }
}

struct TaskbarButtonsVisualBounds
{
    double taskListButtonMinLeft = 0.0;
    double taskListButtonMaxRight = 0.0;
    bool foundTaskListButton = false;
    double taskListButtonMinLeftInBounds = 0.0;
    double taskListButtonMaxRightInBounds = 0.0;
    bool foundTaskListButtonInBounds = false;
    double leafMinLeft = 0.0;
    double leafMaxRight = 0.0;
    bool foundLeaf = false;
    int taskListButtonCandidateCount = 0;
    int taskListButtonInBoundsCount = 0;
};

// Estimate rendered taskbar button visual bounds inside the repeater.
TaskbarButtonsVisualBounds CalculateTaskbarButtonsVisualBounds(
    FrameworkElement taskbarFrameRepeater)
{
    TaskbarButtonsVisualBounds result{};
    if (!taskbarFrameRepeater)
    {
        return result;
    }

    double repeaterWidth = taskbarFrameRepeater.ActualWidth();
    result.taskListButtonMinLeft = repeaterWidth > 1.0 ? repeaterWidth : 0.0;
    result.taskListButtonMinLeftInBounds =
        repeaterWidth > 1.0 ? repeaterWidth : 0.0;
    result.leafMinLeft = repeaterWidth > 1.0 ? repeaterWidth : 0.0;

    std::function<void(DependencyObject)> walk;
    walk = [&](DependencyObject node)
    {
        int childCount = Media::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < childCount; i++)
        {
            auto child = Media::VisualTreeHelper::GetChild(node, i);
            if (!child)
            {
                continue;
            }

            auto fe = child.try_as<FrameworkElement>();
            if (fe &&
                fe.Visibility() == Visibility::Visible &&
                fe.Opacity() > 0.01 &&
                fe.IsHitTestVisible())
            {
                double width = fe.ActualWidth();
                double height = fe.ActualHeight();
                bool isRenderable = width > 1.0 && height > 1.0;
                bool isFullWidthContainer =
                    repeaterWidth > 2.0 && width >= repeaterWidth - 1.0;
                auto className = winrt::get_class_name(fe);

                if (isRenderable && !isFullWidthContainer)
                {
                    try
                    {
                        auto transform = fe.TransformToVisual(taskbarFrameRepeater);
                        auto bounds = transform.TransformBounds(
                            winrt::Windows::Foundation::Rect{
                                0.0f,
                                0.0f,
                                static_cast<float>(width),
                                static_cast<float>(height),
                            });

                        double right =
                            static_cast<double>(bounds.X) +
                            static_cast<double>(bounds.Width);
                        double left = static_cast<double>(bounds.X);
                        std::wstring_view classNameView{className.c_str()};
                        bool isTaskListButtonClass =
                            classNameView.find(L"TaskListButton") !=
                            std::wstring_view::npos;
                        bool isLeaf =
                            Media::VisualTreeHelper::GetChildrenCount(child) == 0;

                        if (std::isfinite(right) && std::isfinite(left) &&
                            isTaskListButtonClass)
                        {
                            if (!result.foundTaskListButton ||
                                right > result.taskListButtonMaxRight)
                            {
                                result.taskListButtonMaxRight = right;
                            }
                            if (!result.foundTaskListButton ||
                                left < result.taskListButtonMinLeft)
                            {
                                result.taskListButtonMinLeft = left;
                            }
                            result.foundTaskListButton = true;
                            result.taskListButtonCandidateCount++;

                            bool inRepeaterVisualRange =
                                left >= -1.0 &&
                                right <= repeaterWidth + 1.0;
                            if (inRepeaterVisualRange)
                            {
                                if (!result.foundTaskListButtonInBounds ||
                                    right > result.taskListButtonMaxRightInBounds)
                                {
                                    result.taskListButtonMaxRightInBounds = right;
                                }
                                if (!result.foundTaskListButtonInBounds ||
                                    left < result.taskListButtonMinLeftInBounds)
                                {
                                    result.taskListButtonMinLeftInBounds = left;
                                }
                                result.foundTaskListButtonInBounds = true;
                                result.taskListButtonInBoundsCount++;
                            }
                        }

                        if (std::isfinite(right) && std::isfinite(left) && isLeaf)
                        {
                            if (!result.foundLeaf || right > result.leafMaxRight)
                            {
                                result.leafMaxRight = right;
                            }
                            if (!result.foundLeaf || left < result.leafMinLeft)
                            {
                                result.leafMinLeft = left;
                            }
                            result.foundLeaf = true;
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }

            walk(child);
        }
    };

    walk(taskbarFrameRepeater);
    return result;
}

// Estimate the rendered right-most extent of button visuals inside the repeater.
// This trims tiny trailing slack that can appear with labeled taskbar buttons.
double CalculateTaskbarButtonsVisibleRightExtent(FrameworkElement taskbarFrameRepeater)
{
    TaskbarButtonsVisualBounds bounds =
        CalculateTaskbarButtonsVisualBounds(taskbarFrameRepeater);
    if (bounds.foundTaskListButton && bounds.taskListButtonMaxRight > 1.0)
    {
        return bounds.taskListButtonMaxRight;
    }

    return bounds.foundLeaf && bounds.leafMaxRight > 1.0 ? bounds.leafMaxRight
                                                          : 0.0;
}

// Calculate the effective visual width of taskbar buttons.
// DesiredSize tracks open/close transitions well on 24H2, but can include a
// small trailing slack when labels are shown. Prefer rendered extent when
// DesiredSize overshoots by a noticeable amount.
double CalculateTaskbarButtonsWidth(FrameworkElement taskbarFrameRepeater)
{
    double desiredWidth = taskbarFrameRepeater.DesiredSize().Width;
    if (desiredWidth <= 1.0)
    {
        return desiredWidth;
    }

    double visibleRightExtent =
        CalculateTaskbarButtonsVisibleRightExtent(taskbarFrameRepeater);
    if (visibleRightExtent > 1.0)
    {
        // DesiredSize can include trailing slack placeholders. Keep whichever
        // is tighter so divider/background align to visible button content.
        return (std::min)(desiredWidth, visibleRightExtent);
    }

    return desiredWidth;
}

// Layout width used for centering/background sizing.
// Keep this on DesiredSize to preserve stable total-group centering.
double CalculateTaskbarButtonsLayoutWidth(FrameworkElement taskbarFrameRepeater)
{
    return taskbarFrameRepeater.DesiredSize().Width;
}

bool TryBuildTaskbarSnapshot(const TaskbarInstance& instance,
                             TaskbarSnapshot* snapshot)
{
    if (!snapshot || !instance.root)
    {
        return false;
    }

    auto xamlRootContent = instance.root.Content().try_as<FrameworkElement>();
    if (!xamlRootContent)
    {
        return false;
    }

    FrameworkElement taskbarFrame = nullptr;
    FrameworkElement rootGrid = nullptr;
    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")))
    {
        taskbarFrame = child;
        if ((child = FindChildByName(child, L"RootGrid")))
        {
            rootGrid = child;
            if ((child = FindChildByName(child, L"TaskbarFrameRepeater")))
            {
                taskbarFrameRepeater = child;
            }
        }
    }

    if (!taskbarFrame || !rootGrid || !taskbarFrameRepeater)
    {
        Wh_Log(L"Failed to find TaskbarFrame, RootGrid, or TaskbarFrameRepeater");
        return false;
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame)
    {
        Wh_Log(L"Failed to find SystemTrayFrame");
        return false;
    }

    TaskbarSnapshot resolvedSnapshot{};
    resolvedSnapshot.instance = instance;
    resolvedSnapshot.rootContent = xamlRootContent;
    resolvedSnapshot.elements.taskbarFrame = taskbarFrame;
    resolvedSnapshot.elements.rootGrid = rootGrid;
    resolvedSnapshot.elements.taskbarFrameRepeater = taskbarFrameRepeater;
    resolvedSnapshot.elements.systemTrayFrame = systemTrayFrame;
    resolvedSnapshot.elements.systemTrayFrameGrid =
        FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    // Use the visible tray grid width when available. SystemTrayFrame can include
    // extra non-visual frame space, which skews centering to the left.
    double trayFrameWidth = systemTrayFrame.ActualWidth();
    double trayVisualWidth = 0.0;
    if (resolvedSnapshot.elements.systemTrayFrameGrid)
    {
        trayVisualWidth = resolvedSnapshot.elements.systemTrayFrameGrid.ActualWidth();
    }

    resolvedSnapshot.systemTrayWidth =
        trayVisualWidth > 1.0 ? trayVisualWidth : trayFrameWidth;
    resolvedSnapshot.taskbarButtonsWidth =
        CalculateTaskbarButtonsLayoutWidth(taskbarFrameRepeater);
    resolvedSnapshot.taskbarButtonsDividerWidth =
        CalculateTaskbarButtonsWidth(taskbarFrameRepeater);
    {
        TaskbarButtonsVisualBounds buttonBounds =
            CalculateTaskbarButtonsVisualBounds(taskbarFrameRepeater);
        double repeaterWidth = taskbarFrameRepeater.ActualWidth();
        if (buttonBounds.foundTaskListButtonInBounds && repeaterWidth > 1.0)
        {
            double visualCenter =
                (buttonBounds.taskListButtonMinLeftInBounds +
                 buttonBounds.taskListButtonMaxRightInBounds) /
                2.0;
            double containerCenter = repeaterWidth / 2.0;
            double visualCenterOffset = visualCenter - containerCenter;
            if (std::isfinite(visualCenterOffset))
            {
                resolvedSnapshot.taskbarButtonsVisualCenterOffset =
                    visualCenterOffset;
                resolvedSnapshot.hasTaskbarButtonsVisualCenterOffset = true;
            }
        }
    }
    if (resolvedSnapshot.taskbarButtonsDividerWidth < 1.0)
    {
        resolvedSnapshot.taskbarButtonsDividerWidth =
            resolvedSnapshot.taskbarButtonsWidth;
    }
    resolvedSnapshot.screenWidth = xamlRootContent.ActualWidth();

    // #region agent log
    {
        char data[768];
        snprintf(
            data,
            sizeof(data),
            "{\"isPrimary\":%d,\"screenWidth\":%.4f,\"trayFrameWidth\":%.4f,"
            "\"trayVisualWidth\":%.4f,\"resolvedTrayWidth\":%.4f,"
            "\"taskbarLayoutWidth\":%.4f,\"taskbarVisibleWidth\":%.4f}",
            resolvedSnapshot.instance.isPrimary ? 1 : 0,
            resolvedSnapshot.screenWidth,
            trayFrameWidth,
            trayVisualWidth,
            resolvedSnapshot.systemTrayWidth,
            resolvedSnapshot.taskbarButtonsWidth,
            resolvedSnapshot.taskbarButtonsDividerWidth);
        AppendAgentDebugLog("H1",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:TryBuildTaskbarSnapshot",
                            "Tray width source selection",
                            "initial",
                            data);
    }
    // #endregion

    // #region agent log
    {
        char data[512];
        double layoutMinusVisible =
            resolvedSnapshot.taskbarButtonsWidth -
            resolvedSnapshot.taskbarButtonsDividerWidth;
        snprintf(
            data,
            sizeof(data),
            "{\"taskbarLayoutWidth\":%.4f,\"taskbarVisibleWidth\":%.4f,"
            "\"layoutMinusVisible\":%.4f}",
            resolvedSnapshot.taskbarButtonsWidth,
            resolvedSnapshot.taskbarButtonsDividerWidth,
            layoutMinusVisible);
        AppendAgentDebugLog("H4",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:TryBuildTaskbarSnapshot",
                            "Taskbar trailing slack",
                            "initial",
                            data);
    }
    // #endregion

    *snapshot = resolvedSnapshot;
    return true;
}

LayoutPlan BuildLayoutPlan(const TaskbarSnapshot& snapshot)
{
    LayoutPlan plan{};

    double dividerExtra = 0.0;
    if (g_settings.dividerEnabled)
    {
        dividerExtra = static_cast<double>(g_settings.dividerWidth) / 2.0
                     + static_cast<double>(g_settings.dividerPaddingX);
    }
    // Keep layout gap driven only by Layout.gap.
    // Divider spacing is applied by the divider overlay placement itself.
    plan.halfGap = g_settings.gap / 2.0;

    plan.effectiveTotalWidth =
        snapshot.taskbarButtonsWidth + snapshot.systemTrayWidth;
    double minWidthExtraGap = 0.0;
    bool minWidthApplied = false;
    if (g_settings.minTotalWidth > 0 &&
        plan.effectiveTotalWidth < g_settings.minTotalWidth)
    {
        minWidthExtraGap =
            (g_settings.minTotalWidth - plan.effectiveTotalWidth) / 2.0;
        plan.halfGap += minWidthExtraGap;
        minWidthApplied = true;
    }

    // Positive => visuals are shifted right within repeater, so move taskbar left.
    // Negative => visuals are shifted left within repeater, so move taskbar right.
    double visualCenterOffset = ClampVisualCenterOffsetForSnapshot(snapshot);
    plan.taskbarOverlayShiftX = visualCenterOffset;

    // Center the combined (buttons + tray) group, then compensate using
    // measured visual-center offset of button content inside the repeater.
    plan.taskbarTransformX =
        -(plan.halfGap + snapshot.systemTrayWidth / 2.0) - visualCenterOffset;
    plan.systemTrayTransformX =
        (snapshot.taskbarButtonsWidth + snapshot.systemTrayWidth +
         plan.halfGap * 2 - snapshot.screenWidth) / 2.0;

    // #region agent log
    {
        char data[768];
        snprintf(
            data,
            sizeof(data),
            "{\"gapSetting\":%d,\"dividerEnabled\":%d,\"dividerWidth\":%d,"
            "\"dividerPaddingX\":%d,\"dividerExtra\":%.4f,\"halfGap\":%.4f,"
            "\"taskbarWidth\":%.4f,\"trayWidth\":%.4f,\"screenWidth\":%.4f,"
            "\"taskbarTransformX\":%.4f,\"systemTrayTransformX\":%.4f,"
            "\"visualCenterOffset\":%.4f,"
            "\"hasVisualCenterOffset\":%d}",
            g_settings.gap,
            g_settings.dividerEnabled ? 1 : 0,
            g_settings.dividerWidth,
            g_settings.dividerPaddingX,
            dividerExtra,
            plan.halfGap,
            snapshot.taskbarButtonsWidth,
            snapshot.systemTrayWidth,
            snapshot.screenWidth,
            plan.taskbarTransformX,
            plan.systemTrayTransformX,
            visualCenterOffset,
            snapshot.hasTaskbarButtonsVisualCenterOffset ? 1 : 0);
        AppendAgentDebugLog("H2",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:BuildLayoutPlan",
                            "Layout plan core spacing",
                            "initial",
                            data);
    }
    // #endregion

    // #region agent log
    {
        char targetData[384];
        snprintf(
            targetData,
            sizeof(targetData),
            "{\"isPrimary\":%d,\"hasVisualCenterOffset\":%d,"
            "\"visualCenterOffset\":%.4f,\"taskbarTransformX\":%.4f,"
            "\"systemTrayTransformX\":%.4f,\"taskbarOverlayShiftX\":%.4f}",
            snapshot.instance.isPrimary ? 1 : 0,
            snapshot.hasTaskbarButtonsVisualCenterOffset ? 1 : 0,
            visualCenterOffset,
            plan.taskbarTransformX,
            plan.systemTrayTransformX,
            plan.taskbarOverlayShiftX);
        AppendAgentDebugLog("H20",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:BuildLayoutPlan",
                            "Taskbar visual-center compensation",
                            "visual-center-audit",
                            targetData);
    }
    // #endregion

    // #region agent log
    {
        char data[512];
        snprintf(
            data,
            sizeof(data),
            "{\"minTotalWidth\":%d,\"effectiveTotalWidth\":%.4f,"
            "\"minWidthApplied\":%d,\"minWidthExtraGap\":%.4f,\"halfGap\":%.4f}",
            g_settings.minTotalWidth,
            plan.effectiveTotalWidth,
            minWidthApplied ? 1 : 0,
            minWidthExtraGap,
            plan.halfGap);
        AppendAgentDebugLog("H3",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:BuildLayoutPlan",
                            "Min-width gap injection",
                            "initial",
                            data);
    }
    // #endregion

    return plan;
}


// Apply a TranslateTransform to visually offset an element (with optional animation)
// The fromOffsetX parameter allows specifying the starting position for the animation
void ApplyTransformOffset(FrameworkElement element, double offsetX, bool animate = false,
                          double fromOffsetX = NAN, int delayMs = 0)
{
    try
    {
        // #region agent log
        if (!std::isfinite(offsetX) ||
            (!std::isnan(fromOffsetX) && !std::isfinite(fromOffsetX)))
        {
            char data[320];
            snprintf(
                data,
                sizeof(data),
                "{\"offsetX\":%.4f,\"fromOffsetX\":%.4f,\"animate\":%d,\"delayMs\":%d}",
                offsetX,
                fromOffsetX,
                animate ? 1 : 0,
                delayMs);
            AppendAgentDebugLog("H24",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyTransformOffset",
                                "Non-finite transform input",
                                "crash-audit",
                                data);
        }
        // #endregion

        // Get or create the TranslateTransform
        Media::TranslateTransform transform{nullptr};
        auto existingTransform = element.RenderTransform();
        if (auto tt = existingTransform.try_as<Media::TranslateTransform>())
        {
            transform = tt;
        }
        else
        {
            transform = Media::TranslateTransform();
            transform.Y(0);
            transform.X(0);
            element.RenderTransform(transform);
        }

        // Determine start position
        double startX = std::isnan(fromOffsetX) ? transform.X() : fromOffsetX;

        // Check if animation should be used
        bool useAnimation = animate && g_settings.animationEnabled &&
                           g_settings.animationDurationMs > 0 && !g_unloading;

        if (useAnimation)
        {
            // Clear any existing animated value so our animation takes effect
            transform.ClearValue(Media::TranslateTransform::XProperty());

            // If we have an explicit "from" value, first set the transform to that position
            if (!std::isnan(fromOffsetX))
            {
                transform.X(fromOffsetX);
            }

            Media::Animation::Storyboard storyboard;

            if (g_settings.animationCurve == kAnimationCurveSpringLazy ||
                g_settings.animationCurve == kAnimationCurveCubic ||
                g_settings.animationCurve == kAnimationCurveSpringRelaxed)
            {
                Media::Animation::DoubleAnimationUsingKeyFrames animation;
                animation.Duration(DurationHelper::FromTimeSpan(
                    winrt::Windows::Foundation::TimeSpan(
                        std::chrono::milliseconds(g_settings.animationDurationMs))));
                if (g_settings.animationCurve == kAnimationCurveSpringLazy)
                {
                    ConfigureSpringLazyKeyframes(animation, startX, offsetX,
                                                 g_settings.animationDurationMs);
                }
                else if (g_settings.animationCurve == kAnimationCurveCubic)
                {
                    ConfigureCubicBezierKeyframes(animation, startX, offsetX,
                                                  g_settings.animationDurationMs);
                }
                else
                {
                    ConfigureSpringRelaxedKeyframes(animation, startX, offsetX,
                                                    g_settings.animationDurationMs);
                }
                animation.EnableDependentAnimation(true);
                if (delayMs > 0)
                {
                    animation.BeginTime(winrt::Windows::Foundation::TimeSpan(
                        std::chrono::milliseconds(delayMs)));
                }
                storyboard.Children().Append(animation);
                Media::Animation::Storyboard::SetTarget(animation, transform);
                Media::Animation::Storyboard::SetTargetProperty(animation, L"X");
            }

            // Start the animation
            storyboard.Begin();
        }
        else
        {
            // Clear any existing animated value and apply directly
            transform.ClearValue(Media::TranslateTransform::XProperty());
            transform.X(offsetX);
        }
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error applying transform %08X: %s", ex.code().value, ex.message().c_str());
        // #region agent log
        char data[256];
        snprintf(
            data,
            sizeof(data),
            "{\"exceptionCode\":%ld}",
            static_cast<long>(ex.code().value));
        AppendAgentDebugLog("H25",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyTransformOffset",
                            "Transform apply exception",
                            "crash-audit",
                            data);
        // #endregion
    }
}

namespace CenterPositionFeature
{
void Apply(const TaskbarSnapshot& snapshot,
           const LayoutPlan& layoutPlan,
           bool animate,
           double fromTaskbarTransformX,
           double fromSystemTrayTransformX,
           int delayMs = 0)
{
    // #region agent log
    {
        bool hasTaskbarTransform = false;
        bool hasTrayTransform = false;
        double currentTaskbarTransformX = 0.0;
        double currentTrayTransformX = 0.0;
        if (snapshot.elements.taskbarFrame)
        {
            if (auto taskbarTransform =
                    snapshot.elements.taskbarFrame.RenderTransform()
                        .try_as<Media::TranslateTransform>())
            {
                hasTaskbarTransform = true;
                currentTaskbarTransformX = taskbarTransform.X();
            }
        }
        if (snapshot.elements.systemTrayFrame)
        {
            if (auto trayTransform =
                    snapshot.elements.systemTrayFrame.RenderTransform()
                        .try_as<Media::TranslateTransform>())
            {
                hasTrayTransform = true;
                currentTrayTransformX = trayTransform.X();
            }
        }

        char data[768];
        snprintf(
            data,
            sizeof(data),
            "{\"animate\":%d,\"fromTaskbarTransformX\":%.4f,"
            "\"fromSystemTrayTransformX\":%.4f,\"targetTaskbarTransformX\":%.4f,"
            "\"targetSystemTrayTransformX\":%.4f,\"hasTaskbarTransform\":%d,"
            "\"hasTrayTransform\":%d,\"currentTaskbarTransformX\":%.4f,"
            "\"currentTrayTransformX\":%.4f}",
            animate ? 1 : 0,
            fromTaskbarTransformX,
            fromSystemTrayTransformX,
            layoutPlan.taskbarTransformX,
            layoutPlan.systemTrayTransformX,
            hasTaskbarTransform ? 1 : 0,
            hasTrayTransform ? 1 : 0,
            currentTaskbarTransformX,
            currentTrayTransformX);
        AppendAgentDebugLog("H5",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:CenterPositionFeature::Apply",
                            "Transform handoff",
                            "initial",
                            data);
    }
    // #endregion

    if (snapshot.elements.taskbarFrame)
    {
        ApplyTransformOffset(snapshot.elements.taskbarFrame,
                             layoutPlan.taskbarTransformX,
                             animate,
                             fromTaskbarTransformX,
                             delayMs);
    }

    if (snapshot.elements.systemTrayFrame)
    {
        ApplyTransformOffset(snapshot.elements.systemTrayFrame,
                             layoutPlan.systemTrayTransformX,
                             animate,
                             fromSystemTrayTransformX,
                             delayMs);
    }
}

void Unload(const TaskbarSnapshot& snapshot)
{
    try
    {
        if (snapshot.elements.taskbarFrame)
        {
            snapshot.elements.taskbarFrame.RenderTransform(nullptr);
        }
    }
    catch (...)
    {
    }

    try
    {
        if (snapshot.elements.systemTrayFrame)
        {
            snapshot.elements.systemTrayFrame.RenderTransform(nullptr);
        }
    }
    catch (...)
    {
    }
}
} // namespace CenterPositionFeature

namespace UnifiedBackgroundFeature
{
static void SetNativeBackgroundVisibility(FrameworkElement rootGrid, bool visible)
{
    if (!rootGrid)
    {
        return;
    }

    try
    {
        FrameworkElement bgControl = FindChildByName(rootGrid, L"BackgroundControl");
        if (!bgControl)
        {
            return;
        }

        bgControl.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
        bgControl.Opacity(visible ? 1.0 : 0.0);

        using namespace winrt::Windows::UI::Xaml::Hosting;
        auto compositionVisual = ElementCompositionPreview::GetElementChildVisual(bgControl);
        if (compositionVisual)
        {
            compositionVisual.IsVisible(visible);
        }

        if (!visible)
        {
            Wh_Log(L"Set BackgroundControl to Collapsed");
        }
    }
    catch (...)
    {
    }
}

void PrepareForFirstLayout(const TaskbarSnapshot& snapshot)
{
    auto rootGrid = snapshot.elements.rootGrid;
    auto systemTrayFrameGrid = snapshot.elements.systemTrayFrameGrid;
    if (!rootGrid)
    {
        return;
    }

    try
    {
        auto grid = rootGrid.try_as<Controls::Grid>();
        if (grid)
        {
            RemoveUnifiedBackground(grid);
        }
    }
    catch (...)
    {
    }

    if (g_settings.unifiedBackgroundEnabled)
    {
        try
        {
            ClearGridBackground(rootGrid);
        }
        catch (...)
        {
        }

        if (systemTrayFrameGrid)
        {
            try
            {
                ClearGridBackground(systemTrayFrameGrid);
            }
            catch (...)
            {
            }
        }

        SetNativeBackgroundVisibility(rootGrid, false);
    }
    else
    {
        try
        {
            RestoreGridBackground(rootGrid);
        }
        catch (...)
        {
        }

        if (systemTrayFrameGrid)
        {
            try
            {
                RestoreGridBackground(systemTrayFrameGrid);
            }
            catch (...)
            {
            }
        }

        SetNativeBackgroundVisibility(rootGrid, true);
    }
}

void Apply(const TaskbarSnapshot& snapshot,
           const LayoutPlan& layoutPlan,
           bool animate,
           double fromTaskbarButtonsWidth,
           int delayMs = 0)
{
    if (!snapshot.elements.rootGrid)
    {
        return;
    }

    UpdateUnifiedBackground(snapshot.elements.rootGrid,
                            snapshot.taskbarButtonsWidth,
                            snapshot.systemTrayWidth,
                            layoutPlan.halfGap,
                            snapshot.screenWidth,
                            layoutPlan.taskbarOverlayShiftX,
                            true,
                            animate,
                            fromTaskbarButtonsWidth,
                            delayMs);
}

void Unload(const TaskbarSnapshot& snapshot, bool restoreNativeOnly)
{
    if (!snapshot.elements.rootGrid)
    {
        return;
    }

    UpdateUnifiedBackground(snapshot.elements.rootGrid,
                            snapshot.taskbarButtonsWidth,
                            snapshot.systemTrayWidth,
                            0,
                            0,
                            0,
                            false);

    if (restoreNativeOnly)
    {
        try
        {
            RestoreGridBackground(snapshot.elements.rootGrid);
        }
        catch (...)
        {
        }

        if (snapshot.elements.systemTrayFrameGrid)
        {
            try
            {
                RestoreGridBackground(snapshot.elements.systemTrayFrameGrid);
            }
            catch (...)
            {
            }
        }
    }

    SetNativeBackgroundVisibility(snapshot.elements.rootGrid, true);
}
} // namespace UnifiedBackgroundFeature

namespace DividerFeature
{
void PrepareForFirstLayout(const TaskbarSnapshot& snapshot)
{
    auto rootGrid = snapshot.elements.rootGrid;
    if (!rootGrid)
    {
        return;
    }

    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid)
    {
        return;
    }

    RemoveDivider(grid);
}

void Apply(const TaskbarSnapshot& snapshot,
           const LayoutPlan& layoutPlan,
           bool animate,
           double fromTaskbarButtonsWidth,
           int delayMs = 0)
{
    if (!snapshot.elements.rootGrid)
    {
        return;
    }

    UpdateDivider(snapshot.elements.rootGrid,
                  true,
                  snapshot.taskbarButtonsDividerWidth,
                  layoutPlan.halfGap,
                  snapshot.screenWidth,
                  layoutPlan.taskbarOverlayShiftX,
                  animate,
                  fromTaskbarButtonsWidth,
                  delayMs);
}

void Unload(const TaskbarSnapshot& snapshot)
{
    if (!snapshot.elements.rootGrid)
    {
        return;
    }

    UpdateDivider(snapshot.elements.rootGrid, false, 0, 0, 0, 0);
}
} // namespace DividerFeature

namespace LayoutTuningFeature
{
void ApplyFirstLayout(const TaskbarSnapshot& snapshot)
{
    auto taskbarFrame = snapshot.elements.taskbarFrame;
    auto taskbarFrameRepeater = snapshot.elements.taskbarFrameRepeater;
    auto systemTrayFrameGrid = snapshot.elements.systemTrayFrameGrid;
    if (!taskbarFrame || !taskbarFrameRepeater)
    {
        return;
    }

    bool isAtTop = IsTaskbarAtTop();
    VerticalAlignment vertAlign =
        isAtTop ? VerticalAlignment::Bottom : VerticalAlignment::Top;

    // Apply or clear custom height.
    if (g_settings.taskbarHeight > 0)
    {
        taskbarFrame.Height(static_cast<double>(g_settings.taskbarHeight));
        taskbarFrame.VerticalAlignment(vertAlign);

        if (systemTrayFrameGrid)
        {
            systemTrayFrameGrid.Height(
                static_cast<double>(g_settings.taskbarHeight));
            systemTrayFrameGrid.VerticalAlignment(vertAlign);
        }

        Wh_Log(L"Applied taskbar height=%d, alignment=%s",
               g_settings.taskbarHeight, isAtTop ? L"Top" : L"Bottom");
    }
    else
    {
        taskbarFrame.ClearValue(FrameworkElement::HeightProperty());
        taskbarFrame.ClearValue(FrameworkElement::VerticalAlignmentProperty());
        if (systemTrayFrameGrid)
        {
            systemTrayFrameGrid.ClearValue(FrameworkElement::HeightProperty());
            systemTrayFrameGrid.ClearValue(
                FrameworkElement::VerticalAlignmentProperty());
        }
    }

    // Apply or clear edge spacing margins.
    if (g_settings.taskbarEdgeSpacing > 0)
    {
        double edgeSpacing = static_cast<double>(g_settings.taskbarEdgeSpacing);
        Thickness edgeMargin = isAtTop ? Thickness{0, edgeSpacing, 0, 0}
                                       : Thickness{0, 0, 0, edgeSpacing};

        taskbarFrameRepeater.Margin(edgeMargin);
        if (systemTrayFrameGrid)
        {
            systemTrayFrameGrid.Margin(edgeMargin);
        }

        Wh_Log(L"Applied edgeSpacing=%d, alignment=%s",
               g_settings.taskbarEdgeSpacing, isAtTop ? L"Top" : L"Bottom");
    }
    else
    {
        taskbarFrameRepeater.Margin(Thickness{0, 0, 0, 0});
        if (systemTrayFrameGrid)
        {
            systemTrayFrameGrid.Margin(Thickness{0, 0, 0, 0});
        }
    }

    // Apply or clear max width.
    if (g_settings.maxTotalWidth > 0)
    {
        taskbarFrameRepeater.MaxWidth(static_cast<double>(g_settings.maxTotalWidth));
    }
    else
    {
        taskbarFrameRepeater.ClearValue(FrameworkElement::MaxWidthProperty());
    }
}

void Unload(const TaskbarSnapshot& snapshot, bool restoreNativeOnly)
{
    auto taskbarFrame = snapshot.elements.taskbarFrame;
    auto taskbarFrameRepeater = snapshot.elements.taskbarFrameRepeater;
    auto systemTrayFrameGrid = snapshot.elements.systemTrayFrameGrid;
    if (!taskbarFrameRepeater)
    {
        return;
    }

    try
    {
        taskbarFrameRepeater.Margin(Thickness{0, 0, 0, 0});
    }
    catch (...)
    {
    }

    if (systemTrayFrameGrid)
    {
        try
        {
            systemTrayFrameGrid.Margin(Thickness{0, 0, 0, 0});
        }
        catch (...)
        {
        }
    }

    if (!restoreNativeOnly)
    {
        return;
    }

    try
    {
        if (taskbarFrame)
        {
            taskbarFrame.ClearValue(FrameworkElement::HeightProperty());
            taskbarFrame.ClearValue(FrameworkElement::VerticalAlignmentProperty());
        }
    }
    catch (...)
    {
    }

    if (systemTrayFrameGrid)
    {
        try
        {
            systemTrayFrameGrid.ClearValue(FrameworkElement::HeightProperty());
            systemTrayFrameGrid.ClearValue(
                FrameworkElement::VerticalAlignmentProperty());
        }
        catch (...)
        {
        }
    }

    try
    {
        taskbarFrameRepeater.ClearValue(FrameworkElement::MaxWidthProperty());
    }
    catch (...)
    {
    }
}
} // namespace LayoutTuningFeature

namespace AnimationDriver
{
struct TransitionDecision
{
    bool shouldAnimate = false;
    bool suppressFrame = false;
    bool isWindowOpening = false;
    double widthDelta = 0;
    double fromTaskbarTransformX = NAN;
    double fromSystemTrayTransformX = NAN;
};

TransitionDecision Evaluate(TaskbarInstanceState* state,
                            const TaskbarSnapshot& snapshot,
                            const LayoutPlan& layoutPlan)
{
    TransitionDecision decision{};
    if (!state)
    {
        return decision;
    }

    if (state->firstLayoutComplete)
    {
        decision.widthDelta =
            snapshot.taskbarButtonsWidth - state->previousTaskbarButtonsWidth;
        if (std::abs(decision.widthDelta) > 5.0)
        {
            state->lastSignificantWidthChangeTick = GetTickCount64();
            decision.isWindowOpening = decision.widthDelta > 0;
            decision.shouldAnimate = g_settings.animationEnabled;
        }
    }

    ULONGLONG nowTick = GetTickCount64();
    if (decision.shouldAnimate && state->firstLayoutComplete)
    {
        state->animationSuppressDirectUntil =
            nowTick + static_cast<ULONGLONG>(g_settings.animationDurationMs + 40);
    }
    else if (g_settings.animationEnabled && state->firstLayoutComplete &&
             nowTick < state->animationSuppressDirectUntil)
    {
        decision.suppressFrame = true;
        return decision;
    }

    if (decision.shouldAnimate && state->firstLayoutComplete)
    {
        double oldTaskbarButtonsWidth = state->previousTaskbarButtonsWidth;
        decision.fromTaskbarTransformX =
            -(layoutPlan.halfGap + snapshot.systemTrayWidth / 2.0);
        decision.fromSystemTrayTransformX =
            (oldTaskbarButtonsWidth + snapshot.systemTrayWidth +
             layoutPlan.halfGap * 2 - snapshot.screenWidth) /
            2.0;
    }

    // #region agent log
    {
        char data[768];
        ULONGLONG sinceLastWidthChangeMs = 0;
        if (state->lastSignificantWidthChangeTick > 0 &&
            nowTick >= state->lastSignificantWidthChangeTick)
        {
            sinceLastWidthChangeMs = nowTick - state->lastSignificantWidthChangeTick;
        }
        snprintf(
            data,
            sizeof(data),
            "{\"isPrimary\":%d,\"screenWidth\":%.4f,\"taskbarWidth\":%.4f,"
            "\"previousTaskbarWidth\":%.4f,\"trayWidth\":%.4f,"
            "\"firstLayoutComplete\":%d,\"widthDelta\":%.4f,"
            "\"shouldAnimate\":%d,\"suppressFrame\":%d,\"isWindowOpening\":%d,"
            "\"fromTaskbarTransformX\":%.4f,\"fromSystemTrayTransformX\":%.4f,"
            "\"lastSignificantWidthChangeTick\":%llu,"
            "\"sinceLastSignificantWidthChangeMs\":%llu}",
            snapshot.instance.isPrimary ? 1 : 0,
            snapshot.screenWidth,
            snapshot.taskbarButtonsWidth,
            state->previousTaskbarButtonsWidth,
            snapshot.systemTrayWidth,
            state->firstLayoutComplete ? 1 : 0,
            decision.widthDelta,
            decision.shouldAnimate ? 1 : 0,
            decision.suppressFrame ? 1 : 0,
            decision.isWindowOpening ? 1 : 0,
            decision.fromTaskbarTransformX,
            decision.fromSystemTrayTransformX,
            static_cast<unsigned long long>(state->lastSignificantWidthChangeTick),
            static_cast<unsigned long long>(sinceLastWidthChangeMs));
        AppendAgentDebugLog("H14",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:AnimationDriver::Evaluate",
                            "Animation decision by root",
                            "layout-audit",
                            data);
    }
    // #endregion

    return decision;
}

void CommitLayoutWidth(TaskbarInstanceState* state,
                       const TaskbarSnapshot& snapshot)
{
    if (!state)
    {
        return;
    }

    state->previousTaskbarButtonsWidth = snapshot.taskbarButtonsWidth;
    state->firstLayoutComplete = true;
}

void Unload(TaskbarInstanceState* state)
{
    if (!state)
    {
        return;
    }

    state->animationSuppressDirectUntil = 0;
    state->stableVisualCenterOffset = 0;
    state->hasStableVisualCenterOffset = false;
}
} // namespace AnimationDriver

bool ApplyStyle(XamlRoot xamlRoot, bool restoreNativeOnly)
{
    if (!xamlRoot)
    {
        return false;
    }

    TrackAppliedXamlRoot(xamlRoot);

    auto* layoutState = GetRootLayoutState(xamlRoot, true);
    if (!layoutState)
    {
        return false;
    }
    bool& firstLayoutComplete = layoutState->firstLayoutComplete;

    TaskbarInstance taskbarInstance{};
    taskbarInstance.root = xamlRoot;
    taskbarInstance.isPrimary = IsPrimaryTaskbarXamlRoot(xamlRoot);
    taskbarInstance.state = layoutState;

    TaskbarSnapshot snapshot{};
    if (!TryBuildTaskbarSnapshot(taskbarInstance, &snapshot))
    {
        return false;
    }

    double taskbarButtonsWidth = snapshot.taskbarButtonsWidth;

    // Start polling timer if not already running (skip during unload).
    // This detects taskbar button width changes (windows opening/closing).
    // We use polling instead of events because:
    //   - SizeChanged: repeater is stretch-aligned, doesn't fire when buttons change
    //   - LayoutUpdated: cross-monitor XamlRoot mismatch causes infinite feedback loop
    if (g_pollTimerId == 0 && !g_unloading && !restoreNativeOnly)
    {
        g_pollTimerId = SetTimer(nullptr, 0, POLL_INTERVAL_MS, ButtonWidthPollTimerProc);
        Wh_Log(L"Started button width poll timer (id=%llu)", (unsigned long long)g_pollTimerId);
    }

    // Guard: skip if elements aren't laid out yet (e.g. during explorer restart)
    // Applying transforms with zero widths can break the layout
    if (!(g_unloading || restoreNativeOnly) &&
        (snapshot.screenWidth < 1.0 || taskbarButtonsWidth < 1.0))
    {
        return false;
    }

    if (g_unloading || restoreNativeOnly)
    {
        // On 24H2, modifying many XAML properties during unload triggers a deferred layout
        // pass that crashes explorer. Only remove the visual transform offsets, the
        // unified background, divider, and edge spacing margins.
        // Height/MaxWidth/VerticalAlignment are NOT reset - the taskbar naturally
        // recovers its native layout once our hooks are removed.
        try { UnifiedBackgroundFeature::Unload(snapshot, restoreNativeOnly); } catch (...) {}
        try { DividerFeature::Unload(snapshot); } catch (...) {}
        try { CenterPositionFeature::Unload(snapshot); } catch (...) {}
        try { LayoutTuningFeature::Unload(snapshot, restoreNativeOnly); } catch (...) {}
        AnimationDriver::Unload(layoutState);
        RefreshScheduler::ClearPending(layoutState);

        Wh_Log(L"%s",
               restoreNativeOnly
                   ? L"Restored taskbar native style"
                   : L"Restored taskbar transforms (safe unload for 24H2)");
        return true;
    }

    // Apply one-time settings on first layout (or after settings change).
    // These are idempotent and don't need to run on every poll tick.
    if (!firstLayoutComplete)
    {
        if (g_settings.centerPositionEnabled)
        {
            // Feature-specific one-time setup and recreation.
            UnifiedBackgroundFeature::PrepareForFirstLayout(snapshot);
            DividerFeature::PrepareForFirstLayout(snapshot);
        }
        else
        {
            // Overlay features depend on centered group positioning.
            UnifiedBackgroundFeature::Unload(snapshot, true);
            DividerFeature::Unload(snapshot);
        }

        LayoutTuningFeature::ApplyFirstLayout(snapshot);
    }

    // DO NOT change HorizontalAlignment - keep native Stretch/Right
    // Changing to Center breaks TaskbarFrame's internal layout on 24H2
    // Instead, use TranslateTransform only for visual positioning

    // Build the transform-only centering layout plan.
    // This keeps layout math pure and reusable across taskbar instances.
    LayoutPlan layoutPlan = BuildLayoutPlan(snapshot);

    // Evaluate animation behavior independently from feature modules.
    auto animationDecision =
        AnimationDriver::Evaluate(layoutState, snapshot, layoutPlan);
    if (animationDecision.suppressFrame)
    {
        return true;
    }

    bool shouldAnimate = animationDecision.shouldAnimate;
    bool isWindowOpening = animationDecision.isWindowOpening;
    double widthDelta = animationDecision.widthDelta;
    bool isCloseTransition = animationDecision.shouldAnimate && !isWindowOpening;

    double measuredVisualCenterOffset =
        ClampVisualCenterOffsetForSnapshot(snapshot);
    double effectiveVisualCenterOffset = measuredVisualCenterOffset;

    // During close-transition frames only, prefer a stable previously-settled
    // offset. Open-transition frames must track live measurements.
    if (isCloseTransition && layoutState->hasStableVisualCenterOffset)
    {
        effectiveVisualCenterOffset = layoutState->stableVisualCenterOffset;
    }

    // Disable close animation to avoid sampling volatile close-frame geometry.
    if (isCloseTransition)
    {
        shouldAnimate = false;
    }

    // Apply any effective offset override to the layout plan that was built from
    // the current snapshot value.
    double visualCenterAdjustDelta =
        measuredVisualCenterOffset - effectiveVisualCenterOffset;
    if (std::abs(visualCenterAdjustDelta) > 0.01)
    {
        layoutPlan.taskbarTransformX += visualCenterAdjustDelta;
        layoutPlan.taskbarOverlayShiftX = effectiveVisualCenterOffset;
    }

    if (!animationDecision.shouldAnimate &&
        snapshot.hasTaskbarButtonsVisualCenterOffset &&
        std::isfinite(measuredVisualCenterOffset))
    {
        layoutState->stableVisualCenterOffset = measuredVisualCenterOffset;
        layoutState->hasStableVisualCenterOffset = true;
    }

    // #region agent log
    {
        char data[640];
        snprintf(
            data,
            sizeof(data),
            "{\"isPrimary\":%d,\"shouldAnimateBefore\":%d,\"shouldAnimateAfter\":%d,"
            "\"isWindowOpening\":%d,\"isCloseTransition\":%d,"
            "\"hasStableVisualCenterOffset\":%d,\"stableVisualCenterOffset\":%.4f,"
            "\"measuredVisualCenterOffset\":%.4f,\"effectiveVisualCenterOffset\":%.4f,"
            "\"visualCenterAdjustDelta\":%.4f}",
            snapshot.instance.isPrimary ? 1 : 0,
            animationDecision.shouldAnimate ? 1 : 0,
            shouldAnimate ? 1 : 0,
            isWindowOpening ? 1 : 0,
            isCloseTransition ? 1 : 0,
            layoutState->hasStableVisualCenterOffset ? 1 : 0,
            layoutState->stableVisualCenterOffset,
            measuredVisualCenterOffset,
            effectiveVisualCenterOffset,
            visualCenterAdjustDelta);
        AppendAgentDebugLog("H23",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Close-transition stabilization decision",
                            "close-path-fix",
                            data);
    }
    // #endregion

    // #region agent log
    {
        char data[640];
        snprintf(
            data,
            sizeof(data),
            "{\"isPrimary\":%d,\"firstLayoutComplete\":%d,\"shouldAnimate\":%d,"
            "\"isWindowOpening\":%d,\"widthDelta\":%.4f,\"taskbarButtonsWidth\":%.4f,"
            "\"previousTaskbarButtonsWidth\":%.4f,\"hasVisualCenterOffset\":%d,"
            "\"visualCenterOffset\":%.4f,\"taskbarTransformX\":%.4f,"
            "\"systemTrayTransformX\":%.4f,\"taskbarOverlayShiftX\":%.4f}",
            snapshot.instance.isPrimary ? 1 : 0,
            layoutState->firstLayoutComplete ? 1 : 0,
            shouldAnimate ? 1 : 0,
            isWindowOpening ? 1 : 0,
            widthDelta,
            snapshot.taskbarButtonsWidth,
            layoutState->previousTaskbarButtonsWidth,
            snapshot.hasTaskbarButtonsVisualCenterOffset ? 1 : 0,
            effectiveVisualCenterOffset,
            layoutPlan.taskbarTransformX,
            layoutPlan.systemTrayTransformX,
            layoutPlan.taskbarOverlayShiftX);
        AppendAgentDebugLog("H22",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Animation-frame offset source",
                            "close-path-audit",
                            data);
    }
    // #endregion

    // Update previous width tracking
    AnimationDriver::CommitLayoutWidth(layoutState, snapshot);

    // Apply center-position feature (transform-only, no layout changes).
    if (g_settings.centerPositionEnabled)
    {
        // #region agent log
        AppendAgentDebugLog("H26",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Apply stage: center-position begin",
                            "crash-stage-audit",
                            "{\"stage\":\"center-begin\"}");
        // #endregion
        CenterPositionFeature::Apply(snapshot,
                                     layoutPlan,
                                     shouldAnimate,
                                     animationDecision.fromTaskbarTransformX,
                                     animationDecision.fromSystemTrayTransformX,
                                     0);
        // #region agent log
        AppendAgentDebugLog("H26",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Apply stage: center-position end",
                            "crash-stage-audit",
                            "{\"stage\":\"center-end\"}");
        // #endregion
    }
    else
    {
        CenterPositionFeature::Unload(snapshot);
    }

    if (g_settings.centerPositionEnabled)
    {
        // #region agent log
        AppendAgentDebugLog("H26",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Apply stage: overlay begin",
                            "crash-stage-audit",
                            "{\"stage\":\"overlay-begin\"}");
        // #endregion
        // Apply unified background and divider feature overlays.
        double fromTaskbarWidth = shouldAnimate ? (taskbarButtonsWidth - widthDelta) : NAN;
        double fromDividerWidth = NAN;
        if (shouldAnimate)
        {
            double dividerCompensation =
                snapshot.taskbarButtonsWidth - snapshot.taskbarButtonsDividerWidth;
            fromDividerWidth =
                (std::max)(0.0, fromTaskbarWidth - dividerCompensation);
        }
        UnifiedBackgroundFeature::Apply(snapshot,
                                        layoutPlan,
                                        shouldAnimate,
                                        fromTaskbarWidth,
                                        0);
        DividerFeature::Apply(snapshot,
                              layoutPlan,
                              shouldAnimate,
                              fromDividerWidth,
                              0);
        // #region agent log
        AppendAgentDebugLog("H26",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Apply stage: overlay end",
                            "crash-stage-audit",
                            "{\"stage\":\"overlay-end\"}");
        // #endregion
    }
    else
    {
        UnifiedBackgroundFeature::Unload(snapshot, true);
        DividerFeature::Unload(snapshot);
    }

    // #region agent log
    {
        FrameworkElement trayMeasureElement = snapshot.elements.systemTrayFrameGrid
            ? snapshot.elements.systemTrayFrameGrid
            : snapshot.elements.systemTrayFrame;

        bool hasRepeaterBounds = false;
        bool hasTrayBounds = false;
        bool hasTaskbarFrameBounds = false;
        bool hasSystemTrayFrameBounds = false;
        winrt::Windows::Foundation::Rect repeaterBounds{};
        winrt::Windows::Foundation::Rect trayBounds{};
        winrt::Windows::Foundation::Rect taskbarFrameBounds{};
        winrt::Windows::Foundation::Rect systemTrayFrameBounds{};

        auto measureInRoot = [&](FrameworkElement element,
                                 bool* hasBounds,
                                 winrt::Windows::Foundation::Rect* bounds)
        {
            if (!element || !snapshot.rootContent || !hasBounds || !bounds)
            {
                return;
            }

            try
            {
                auto transform = element.TransformToVisual(snapshot.rootContent);
                *bounds = transform.TransformBounds(
                    winrt::Windows::Foundation::Rect{
                        0.0f,
                        0.0f,
                        static_cast<float>(element.ActualWidth()),
                        static_cast<float>(element.ActualHeight()),
                    });
                *hasBounds = true;
            }
            catch (...)
            {
            }
        };

        measureInRoot(snapshot.elements.taskbarFrameRepeater,
                      &hasRepeaterBounds,
                      &repeaterBounds);
        measureInRoot(trayMeasureElement,
                      &hasTrayBounds,
                      &trayBounds);
        measureInRoot(snapshot.elements.taskbarFrame,
                      &hasTaskbarFrameBounds,
                      &taskbarFrameBounds);
        measureInRoot(snapshot.elements.systemTrayFrame,
                      &hasSystemTrayFrameBounds,
                      &systemTrayFrameBounds);

        double visualGapPx = NAN;
        if (hasRepeaterBounds && hasTrayBounds)
        {
            visualGapPx = static_cast<double>(trayBounds.X) -
                          (static_cast<double>(repeaterBounds.X) +
                           static_cast<double>(repeaterBounds.Width));
        }

        double frameGapPx = NAN;
        if (hasTaskbarFrameBounds && hasSystemTrayFrameBounds)
        {
            frameGapPx = static_cast<double>(systemTrayFrameBounds.X) -
                         (static_cast<double>(taskbarFrameBounds.X) +
                          static_cast<double>(taskbarFrameBounds.Width));
        }

        char data[1024];
        snprintf(
            data,
            sizeof(data),
            "{\"hasRepeaterBounds\":%d,\"hasTrayBounds\":%d,\"visualGapPx\":%.4f,"
            "\"hasTaskbarFrameBounds\":%d,\"hasSystemTrayFrameBounds\":%d,"
            "\"frameGapPx\":%.4f,\"repeaterLeft\":%.4f,\"repeaterRight\":%.4f,"
            "\"trayLeft\":%.4f,\"trayRight\":%.4f}",
            hasRepeaterBounds ? 1 : 0,
            hasTrayBounds ? 1 : 0,
            visualGapPx,
            hasTaskbarFrameBounds ? 1 : 0,
            hasSystemTrayFrameBounds ? 1 : 0,
            frameGapPx,
            static_cast<double>(repeaterBounds.X),
            static_cast<double>(repeaterBounds.X) +
                static_cast<double>(repeaterBounds.Width),
            static_cast<double>(trayBounds.X),
            static_cast<double>(trayBounds.X) +
                static_cast<double>(trayBounds.Width));
        AppendAgentDebugLog("H6",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Post-transform visual bounds gap",
                            "post-fix-geometry",
                            data);
    }
    // #endregion

    // #region agent log
    {
        bool hasDividerBounds = false;
        bool hasRepeaterBounds = false;
        bool hasTrayBounds = false;
        winrt::Windows::Foundation::Rect dividerBounds{};
        winrt::Windows::Foundation::Rect repeaterBounds{};
        winrt::Windows::Foundation::Rect trayBounds{};
        FrameworkElement trayMeasureElement = snapshot.elements.systemTrayFrameGrid
            ? snapshot.elements.systemTrayFrameGrid
            : snapshot.elements.systemTrayFrame;

        auto measureInRoot = [&](FrameworkElement element,
                                 bool* hasBounds,
                                 winrt::Windows::Foundation::Rect* bounds)
        {
            if (!element || !snapshot.rootContent || !hasBounds || !bounds)
            {
                return;
            }

            try
            {
                auto transform = element.TransformToVisual(snapshot.rootContent);
                *bounds = transform.TransformBounds(
                    winrt::Windows::Foundation::Rect{
                        0.0f,
                        0.0f,
                        static_cast<float>(element.ActualWidth()),
                        static_cast<float>(element.ActualHeight()),
                    });
                *hasBounds = true;
            }
            catch (...)
            {
            }
        };

        auto rootGrid = snapshot.elements.rootGrid.try_as<Controls::Grid>();
        if (rootGrid)
        {
            auto divider = FindDivider(rootGrid);
            measureInRoot(divider, &hasDividerBounds, &dividerBounds);
        }

        measureInRoot(snapshot.elements.taskbarFrameRepeater,
                      &hasRepeaterBounds,
                      &repeaterBounds);
        measureInRoot(trayMeasureElement, &hasTrayBounds, &trayBounds);

        double buttonToDividerGapPx = NAN;
        double dividerToTrayGapPx = NAN;
        if (hasDividerBounds && hasRepeaterBounds)
        {
            buttonToDividerGapPx =
                static_cast<double>(dividerBounds.X) -
                (static_cast<double>(repeaterBounds.X) +
                 static_cast<double>(repeaterBounds.Width));
        }
        if (hasDividerBounds && hasTrayBounds)
        {
            dividerToTrayGapPx =
                static_cast<double>(trayBounds.X) -
                (static_cast<double>(dividerBounds.X) +
                 static_cast<double>(dividerBounds.Width));
        }

        char data[1024];
        snprintf(
            data,
            sizeof(data),
            "{\"hasDividerBounds\":%d,\"hasRepeaterBounds\":%d,\"hasTrayBounds\":%d,"
            "\"buttonToDividerGapPx\":%.4f,\"dividerToTrayGapPx\":%.4f,"
            "\"dividerLeft\":%.4f,\"dividerRight\":%.4f}",
            hasDividerBounds ? 1 : 0,
            hasRepeaterBounds ? 1 : 0,
            hasTrayBounds ? 1 : 0,
            buttonToDividerGapPx,
            dividerToTrayGapPx,
            static_cast<double>(dividerBounds.X),
            static_cast<double>(dividerBounds.X) +
                static_cast<double>(dividerBounds.Width));
        AppendAgentDebugLog("H7",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Divider occupancy within visual gap",
                            "post-fix-geometry",
                            data);
    }
    // #endregion

    // #region agent log
    {
        bool hasRepeaterBounds = false;
        bool hasTrayBounds = false;
        winrt::Windows::Foundation::Rect repeaterBounds{};
        winrt::Windows::Foundation::Rect trayBounds{};
        FrameworkElement trayMeasureElement = snapshot.elements.systemTrayFrameGrid
            ? snapshot.elements.systemTrayFrameGrid
            : snapshot.elements.systemTrayFrame;

        auto measureInRoot = [&](FrameworkElement element,
                                 bool* hasBounds,
                                 winrt::Windows::Foundation::Rect* bounds)
        {
            if (!element || !snapshot.rootContent || !hasBounds || !bounds)
            {
                return;
            }

            try
            {
                auto transform = element.TransformToVisual(snapshot.rootContent);
                *bounds = transform.TransformBounds(
                    winrt::Windows::Foundation::Rect{
                        0.0f,
                        0.0f,
                        static_cast<float>(element.ActualWidth()),
                        static_cast<float>(element.ActualHeight()),
                    });
                *hasBounds = true;
            }
            catch (...)
            {
            }
        };

        measureInRoot(snapshot.elements.taskbarFrameRepeater,
                      &hasRepeaterBounds,
                      &repeaterBounds);
        measureInRoot(trayMeasureElement, &hasTrayBounds, &trayBounds);

        double repeaterDesiredWidth =
            snapshot.elements.taskbarFrameRepeater
                ? snapshot.elements.taskbarFrameRepeater.DesiredSize().Width
                : 0.0;
        double repeaterActualWidth =
            snapshot.elements.taskbarFrameRepeater
                ? snapshot.elements.taskbarFrameRepeater.ActualWidth()
                : 0.0;
        TaskbarButtonsVisualBounds buttonBounds =
            CalculateTaskbarButtonsVisualBounds(
                snapshot.elements.taskbarFrameRepeater);
        double buttonVisibleLeftLocal = buttonBounds.foundTaskListButton
                                            ? buttonBounds.taskListButtonMinLeft
                                            : (buttonBounds.foundLeaf
                                                   ? buttonBounds.leafMinLeft
                                                   : NAN);
        double buttonVisibleRightLocal = buttonBounds.foundTaskListButton
                                             ? buttonBounds.taskListButtonMaxRight
                                             : (buttonBounds.foundLeaf
                                                    ? buttonBounds.leafMaxRight
                                                    : NAN);
        double buttonVisibleWidthLocal =
            (std::isfinite(buttonVisibleLeftLocal) &&
             std::isfinite(buttonVisibleRightLocal))
                ? (buttonVisibleRightLocal - buttonVisibleLeftLocal)
                : NAN;
        double buttonVisibleLeftLocalInBounds =
            buttonBounds.foundTaskListButtonInBounds
                ? buttonBounds.taskListButtonMinLeftInBounds
                : NAN;
        double buttonVisibleRightLocalInBounds =
            buttonBounds.foundTaskListButtonInBounds
                ? buttonBounds.taskListButtonMaxRightInBounds
                : NAN;
        double buttonVisibleWidthLocalInBounds =
            (std::isfinite(buttonVisibleLeftLocalInBounds) &&
             std::isfinite(buttonVisibleRightLocalInBounds))
                ? (buttonVisibleRightLocalInBounds -
                   buttonVisibleLeftLocalInBounds)
                : NAN;
        double buttonVisibleLeftRoot = NAN;
        double buttonVisibleRightRoot = NAN;
        double buttonVisibleRightRootInBounds = NAN;
        double buttonToTrayVisualGapPx = NAN;
        double buttonToTrayVisualGapInBoundsPx = NAN;
        bool hasTrayFrameBounds = false;
        winrt::Windows::Foundation::Rect trayFrameBounds{};
        measureInRoot(snapshot.elements.systemTrayFrame,
                      &hasTrayFrameBounds,
                      &trayFrameBounds);
        double trayMeasureInsetFromFrameLeft = NAN;
        if (hasRepeaterBounds && buttonVisibleRightLocal > 1.0)
        {
            buttonVisibleLeftRoot =
                static_cast<double>(repeaterBounds.X) + buttonVisibleLeftLocal;
            buttonVisibleRightRoot =
                static_cast<double>(repeaterBounds.X) + buttonVisibleRightLocal;
            if (hasTrayBounds)
            {
                buttonToTrayVisualGapPx =
                    static_cast<double>(trayBounds.X) - buttonVisibleRightRoot;
                if (std::isfinite(buttonVisibleRightLocalInBounds))
                {
                    buttonVisibleRightRootInBounds =
                        static_cast<double>(repeaterBounds.X) +
                        buttonVisibleRightLocalInBounds;
                    buttonToTrayVisualGapInBoundsPx =
                        static_cast<double>(trayBounds.X) -
                        buttonVisibleRightRootInBounds;
                }
            }
        }
        if (hasTrayBounds && hasTrayFrameBounds)
        {
            trayMeasureInsetFromFrameLeft =
                static_cast<double>(trayBounds.X) -
                static_cast<double>(trayFrameBounds.X);
        }

        double trayVisibleMinLeftLocal = NAN;
        double trayVisibleMaxRightLocal = NAN;
        double trayVisibleWidthLocal = NAN;
        int trayLeafCandidateCount = 0;
        int trayLeafInBoundsCount = 0;
        if (trayMeasureElement)
        {
            double trayContainerWidth = trayMeasureElement.ActualWidth();
            std::function<void(DependencyObject)> walkTray;
            walkTray = [&](DependencyObject node)
            {
                int childCount = Media::VisualTreeHelper::GetChildrenCount(node);
                for (int i = 0; i < childCount; i++)
                {
                    auto child = Media::VisualTreeHelper::GetChild(node, i);
                    if (!child)
                    {
                        continue;
                    }

                    auto fe = child.try_as<FrameworkElement>();
                    if (fe &&
                        fe.Visibility() == Visibility::Visible &&
                        fe.Opacity() > 0.01 &&
                        fe.IsHitTestVisible())
                    {
                        double width = fe.ActualWidth();
                        double height = fe.ActualHeight();
                        bool isRenderable = width > 1.0 && height > 1.0;
                        bool isLeaf = Media::VisualTreeHelper::GetChildrenCount(child) == 0;
                        if (isRenderable && isLeaf)
                        {
                            trayLeafCandidateCount++;
                            try
                            {
                                auto transform = fe.TransformToVisual(trayMeasureElement);
                                auto bounds = transform.TransformBounds(
                                    winrt::Windows::Foundation::Rect{
                                        0.0f,
                                        0.0f,
                                        static_cast<float>(width),
                                        static_cast<float>(height),
                                    });
                                double left = static_cast<double>(bounds.X);
                                double right = left + static_cast<double>(bounds.Width);
                                bool inBounds =
                                    left >= -1.0 &&
                                    right <= trayContainerWidth + 1.0;
                                if (std::isfinite(left) && std::isfinite(right) && inBounds)
                                {
                                    if (!std::isfinite(trayVisibleMinLeftLocal) ||
                                        left < trayVisibleMinLeftLocal)
                                    {
                                        trayVisibleMinLeftLocal = left;
                                    }
                                    if (!std::isfinite(trayVisibleMaxRightLocal) ||
                                        right > trayVisibleMaxRightLocal)
                                    {
                                        trayVisibleMaxRightLocal = right;
                                    }
                                    trayLeafInBoundsCount++;
                                }
                            }
                            catch (...)
                            {
                            }
                        }
                    }

                    walkTray(child);
                }
            };
            walkTray(trayMeasureElement);
            if (std::isfinite(trayVisibleMinLeftLocal) &&
                std::isfinite(trayVisibleMaxRightLocal))
            {
                trayVisibleWidthLocal =
                    trayVisibleMaxRightLocal - trayVisibleMinLeftLocal;
            }
        }

        char data[1024];
        snprintf(
            data,
            sizeof(data),
            "{\"hasRepeaterBounds\":%d,\"hasTrayBounds\":%d,"
            "\"repeaterDesiredWidth\":%.4f,\"repeaterActualWidth\":%.4f,"
            "\"buttonVisibleLeftLocal\":%.4f,\"buttonVisibleRightLocal\":%.4f,"
            "\"buttonVisibleWidthLocal\":%.4f,"
            "\"buttonVisibleLeftLocalInBounds\":%.4f,"
            "\"buttonVisibleRightLocalInBounds\":%.4f,"
            "\"buttonVisibleWidthLocalInBounds\":%.4f,"
            "\"taskListButtonCandidates\":%d,\"taskListButtonInBounds\":%d,"
            "\"buttonVisibleLeftRoot\":%.4f,\"buttonVisibleRightRoot\":%.4f,"
            "\"buttonVisibleRightRootInBounds\":%.4f,"
            "\"trayLeft\":%.4f,"
            "\"hasTrayFrameBounds\":%d,\"trayFrameLeft\":%.4f,"
            "\"trayVisibleMinLeftLocal\":%.4f,\"trayVisibleMaxRightLocal\":%.4f,"
            "\"trayVisibleWidthLocal\":%.4f,"
            "\"trayLeafCandidates\":%d,\"trayLeafInBounds\":%d,"
            "\"trayMeasureInsetFromFrameLeft\":%.4f,"
            "\"buttonToTrayVisualGapPx\":%.4f,"
            "\"buttonToTrayVisualGapInBoundsPx\":%.4f}",
            hasRepeaterBounds ? 1 : 0,
            hasTrayBounds ? 1 : 0,
            repeaterDesiredWidth,
            repeaterActualWidth,
            buttonVisibleLeftLocal,
            buttonVisibleRightLocal,
            buttonVisibleWidthLocal,
            buttonVisibleLeftLocalInBounds,
            buttonVisibleRightLocalInBounds,
            buttonVisibleWidthLocalInBounds,
            buttonBounds.taskListButtonCandidateCount,
            buttonBounds.taskListButtonInBoundsCount,
            buttonVisibleLeftRoot,
            buttonVisibleRightRoot,
            buttonVisibleRightRootInBounds,
            static_cast<double>(trayBounds.X),
            hasTrayFrameBounds ? 1 : 0,
            static_cast<double>(trayFrameBounds.X),
            trayVisibleMinLeftLocal,
            trayVisibleMaxRightLocal,
            trayVisibleWidthLocal,
            trayLeafCandidateCount,
            trayLeafInBoundsCount,
            trayMeasureInsetFromFrameLeft,
            buttonToTrayVisualGapPx,
            buttonToTrayVisualGapInBoundsPx);
        AppendAgentDebugLog("H8",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Visible button extent vs tray left",
                            "post-fix-geometry",
                            data);

        // #region agent log
        char trayWidthData[768];
        snprintf(
            trayWidthData,
            sizeof(trayWidthData),
            "{\"isPrimary\":%d,\"resolvedTrayWidth\":%.4f,"
            "\"trayVisibleWidthLocal\":%.4f,\"trayVisibleMinLeftLocal\":%.4f,"
            "\"trayVisibleMaxRightLocal\":%.4f,\"trayLeafCandidates\":%d,"
            "\"trayLeafInBounds\":%d,\"trayMeasureInsetFromFrameLeft\":%.4f}",
            snapshot.instance.isPrimary ? 1 : 0,
            snapshot.systemTrayWidth,
            trayVisibleWidthLocal,
            trayVisibleMinLeftLocal,
            trayVisibleMaxRightLocal,
            trayLeafCandidateCount,
            trayLeafInBoundsCount,
            trayMeasureInsetFromFrameLeft);
        AppendAgentDebugLog("H13",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Tray visible leaf width vs resolved tray width",
                            "layout-audit",
                            trayWidthData);
        // #endregion

        // #region agent log
        {
            double measuredGap = std::isfinite(buttonToTrayVisualGapInBoundsPx)
                                     ? buttonToTrayVisualGapInBoundsPx
                                     : buttonToTrayVisualGapPx;
            char centerData[512];
            snprintf(
                centerData,
                sizeof(centerData),
                "{\"isPrimary\":%d,\"measuredGap\":%.4f,"
                "\"hasVisualCenterOffset\":%d,\"visualCenterOffset\":%.4f,"
                "\"shouldAnimate\":%d,\"isWindowOpening\":%d}",
                snapshot.instance.isPrimary ? 1 : 0,
                measuredGap,
                snapshot.hasTaskbarButtonsVisualCenterOffset ? 1 : 0,
                snapshot.taskbarButtonsVisualCenterOffset,
                shouldAnimate ? 1 : 0,
                isWindowOpening ? 1 : 0);
            AppendAgentDebugLog("H20",
                                "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                                "Measured gap vs visual-center offset",
                                "visual-center-audit",
                                centerData);
        }
        // #endregion

        // #region agent log
        char trayInsetData[512];
        snprintf(
            trayInsetData,
            sizeof(trayInsetData),
            "{\"hasTrayBounds\":%d,\"hasTrayFrameBounds\":%d,"
            "\"trayMeasureLeft\":%.4f,\"trayFrameLeft\":%.4f,"
            "\"trayMeasureInsetFromFrameLeft\":%.4f}",
            hasTrayBounds ? 1 : 0,
            hasTrayFrameBounds ? 1 : 0,
            static_cast<double>(trayBounds.X),
            static_cast<double>(trayFrameBounds.X),
            trayMeasureInsetFromFrameLeft);
        AppendAgentDebugLog("H11",
                            "windows-11-better-ultrawide-taskbar.wh.cpp:ApplyStyle",
                            "Tray measure element inset inside frame",
                            "layout-audit",
                            trayInsetData);
        // #endregion
    }
    // #endregion

    // Schedule a per-root delayed refresh to catch post-open label stabilization.
    if (isWindowOpening)
    {
        RefreshScheduler::ScheduleDelayedRefresh(layoutState, GetTickCount64());
    }

    return true;
}

// Symbol hook targets
void *CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void *TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI *)(void *pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void *taskbarHostSharedPtr[2])
{
    auto releaseSharedPtr = [&]()
    {
        if (taskbarHostSharedPtr[1] && std__Ref_count_base__Decref_Original)
        {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
            taskbarHostSharedPtr[1] = nullptr;
        }
    };

    if (!taskbarHostSharedPtr[0])
    {
        releaseSharedPtr();
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        bool offsetResolved = false;

        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE *b = (const BYTE *)TaskbarHost_FrameHeight_Original;
        if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
        {
            taskbarElementIUnknownOffset = b[7];
            offsetResolved = true;
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }

        if (!offsetResolved)
        {
            releaseSharedPtr();
            return nullptr;
        }
    }
#elif defined(_M_ARM64)
    // Just use the default offset which will hopefully work in most cases.
#else
#error "Unsupported architecture"
#endif

    auto *taskbarElementIUnknown =
        *(IUnknown **)((BYTE *)taskbarHostSharedPtr[0] +
                       taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown)
    {
        releaseSharedPtr();
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarElement));
    if (FAILED(hr) || !taskbarElement)
    {
        releaseSharedPtr();
        return nullptr;
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    releaseSharedPtr();

    return result;
}

XamlRoot XamlRootFromTaskbandObject(void* taskBand)
{
    if (!taskBand)
    {
        return nullptr;
    }

    void *taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void **)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20)
        {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void **)taskBandForTaskListWndSite + 1;
    }

    void *taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd)
{
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd)
    {
        return nullptr;
    }

    WCHAR trayClass[64] = L"";
    GetClassName(hTaskbarWnd, trayClass, ARRAYSIZE(trayClass));
    WCHAR taskbandClass[64] = L"";
    GetClassName(hTaskSwWnd, taskbandClass, ARRAYSIZE(taskbandClass));
    if (_wcsicmp(trayClass, L"Shell_TrayWnd") == 0)
    {
        wcsncpy_s(g_primaryTaskbandClassName, taskbandClass, _TRUNCATE);
    }

    void *taskBand = (void *)GetWindowLongPtr(hTaskSwWnd, 0);
    return XamlRootFromTaskbandObject(taskBand);
}

using RunFromWindowThreadProc_t = void(WINAPI *)(void *parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void *procParam)
{
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM
    {
        RunFromWindowThreadProc_t proc;
        void *procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION)
            {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg)
                {
                    RUN_FROM_WINDOW_THREAD_PARAM *param =
                        (RUN_FROM_WINDOW_THREAD_PARAM *)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook)
    {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void ApplySettingsFromTaskbarThread()
{
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            // Shell_TrayWnd is primary. Shell_SecondaryTrayWnd is controlled by settings.
            bool isPrimaryTaskbar = _wcsicmp(szClassName, L"Shell_TrayWnd") == 0;
            bool isSecondaryTaskbar = _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
            if (!isPrimaryTaskbar && !isSecondaryTaskbar)
            {
                return TRUE;
            }

            XamlRoot xamlRoot = GetTaskbarXamlRoot(hWnd);
            if (!xamlRoot)
            {
                Wh_Log(L"Getting XamlRoot failed for %s", szClassName);
                return TRUE;
            }

            TrackTaskbarXamlRoot(xamlRoot, isPrimaryTaskbar);
            if (isPrimaryTaskbar)
            {
                g_cachedPrimaryXamlRoot = winrt::make_weak(xamlRoot);
            }

            if (isSecondaryTaskbar && !g_settings.applyToAllMonitors)
            {
                // If secondary styling was previously active, restore native visuals.
                ApplyStyle(xamlRoot, true);
                return TRUE;
            }

            if (!RequestApplyForTaskbarRoot(xamlRoot, RefreshReason::SettingsApply))
            {
                Wh_Log(L"ApplyStyle failed for %s", szClassName);
            }

            return TRUE;
        },
        0);

    if (g_unloading)
    {
        for (int i = 0; i < g_appliedXamlRootsCount; i++)
        {
            auto root = g_appliedXamlRoots[i].get();
            if (!root)
            {
                continue;
            }

            bool ok = false;
            try
            {
                ok = ApplyStyle(root);
            }
            catch (...)
            {
                ok = false;
            }

            (void)ok;
        }
    }

    // Touch a registry value to trigger the taskbar's internal settings watcher
    // This forces the taskbar to refresh and apply our changes
    // SKIP during unload - forcing a refresh mid-teardown triggers a deferred crash on 24H2
    if (!g_unloading)
    {
        constexpr WCHAR kTempValueName[] = L"_temp_windhawk_" WH_MOD_ID;
        HKEY hSubKey;
        LONG result = RegOpenKeyEx(
            HKEY_CURRENT_USER,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)", 0,
            KEY_WRITE, &hSubKey);
        if (result == ERROR_SUCCESS)
        {
            if (RegSetValueEx(hSubKey, kTempValueName, 0, REG_SZ, (const BYTE *)L"",
                              sizeof(WCHAR)) != ERROR_SUCCESS)
            {
                Wh_Log(L"Failed to create temp registry value");
            }
            else if (RegDeleteValue(hSubKey, kTempValueName) != ERROR_SUCCESS)
            {
                Wh_Log(L"Failed to remove temp registry value");
            }

            RegCloseKey(hSubKey);
        }
        else
        {
            Wh_Log(L"Failed to open Explorer\\Advanced registry key: %d", result);
        }
    }
}

void ApplySettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *pParam)
        { ApplySettingsFromTaskbarThread(); }, 0);
}

// Polling timer callback - checks if taskbar button width changed and re-applies styles.
// This is the primary mechanism for detecting window open/close events.
// Fires every POLL_INTERVAL_MS (250ms). All roots are processed, and delayed/close
// refreshes are scheduled per-root using TaskbarInstanceState.
void CALLBACK ButtonWidthPollTimerProc(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR idEvent, DWORD /*dwTime*/)
{
    if (g_unloading)
    {
        KillTimer(nullptr, idEvent);
        if (g_pollTimerId == idEvent)
        {
            g_pollTimerId = 0;
        }
        return;
    }

    ULONGLONG nowTick = GetTickCount64();
    ForEachConfiguredTaskbarXamlRoot(
        [&](XamlRoot xamlRoot)
        {
            if (!xamlRoot || !IsTaskbarXamlRootEnabled(xamlRoot))
            {
                return;
            }

            auto* layoutState = GetRootLayoutState(xamlRoot, true);
            if (!layoutState)
            {
                return;
            }

            // Execute due scheduled refreshes first.
            RefreshScheduler::TryApplyScheduledForRoot(xamlRoot, layoutState, nowTick);

            // Clear tooltip backgrounds on every poll tick.
            try
            {
                auto popups = Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot);
                for (uint32_t i = 0; i < popups.Size(); i++)
                {
                    auto popup = popups.GetAt(i);
                    auto popupChild = popup.Child();
                    if (!popupChild)
                    {
                        continue;
                    }

                    auto tooltip = popupChild.try_as<Controls::ToolTip>();
                    if (tooltip && tooltip.Background())
                    {
                        tooltip.Background(nullptr);
                    }
                }
            }
            catch (...)
            {
            }

            // Build per-root snapshot for width change detection.
            TaskbarInstance taskbarInstance{};
            taskbarInstance.root = xamlRoot;
            taskbarInstance.isPrimary = IsPrimaryTaskbarXamlRoot(xamlRoot);
            taskbarInstance.state = layoutState;

            TaskbarSnapshot snapshot{};
            if (!TryBuildTaskbarSnapshot(taskbarInstance, &snapshot))
            {
                return;
            }

            double currentWidth = snapshot.taskbarButtonsWidth;
            bool widthChanged =
                std::abs(currentWidth - layoutState->lastPolledButtonsWidth) >= 5.0;
            if (!widthChanged)
            {
                return;
            }

            bool isClosing = currentWidth < layoutState->lastPolledButtonsWidth;
            layoutState->lastPolledButtonsWidth = currentWidth;

            if (isClosing)
            {
                // Defer close refresh until close animation settles.
                RefreshScheduler::ScheduleCloseRefresh(layoutState, nowTick);
                return;
            }

            // Window opening: apply immediately for this root, then schedule
            // delayed stabilization refresh to capture late label width changes.
            bool appliedImmediately = false;
            if (!g_isApplyingStyle.exchange(true))
            {
                appliedImmediately =
                    RequestApplyForTaskbarRoot(xamlRoot, RefreshReason::PollOpen);
                g_isApplyingStyle = false;
            }

            // If immediate apply was skipped due re-entry, keep a deferred pass.
            if (!appliedImmediately)
            {
                RefreshScheduler::ScheduleDelayedRefresh(layoutState, nowTick);
            }
        });
}

// Hook for FlyoutFrame::OnApplyTemplate - hide HoverFlyoutBackgroundControl
// The flyout lives in a separate XAML island, so we must access it from the hook.
// We try both offset 0 (pThis directly) and offset 1 (pThisArray[1]) for safety.
// If the tree isn't fully ready, a deferred timer retries.
using FlyoutFrame_OnApplyTemplate_t = void(WINAPI *)(void *pThis);
FlyoutFrame_OnApplyTemplate_t FlyoutFrame_OnApplyTemplate_Original;
static winrt::weak_ref<FrameworkElement> g_pendingFlyoutElement;
static UINT_PTR g_flyoutBgTimerId = 0;

// (Loaded event tokens removed - the background comes from parent containers, not just the child)

// Helper: aggressively hide an element and clear its background if it's a Border/Panel
static void HideElement(FrameworkElement fe, const char* name)
{
    if (!fe) return;
    fe.Visibility(Visibility::Collapsed);
    fe.Opacity(0.0);
    fe.MaxHeight(0);
    fe.MaxWidth(0);

    // Clear background brush if the element is a Border or Panel
    if (auto border = fe.try_as<Controls::Border>()) {
        border.Background(nullptr);
    }
    if (auto panel = fe.try_as<Controls::Panel>()) {
        panel.Background(nullptr);
    }

    // #region agent log
    char buf[128];
    snprintf(buf, sizeof(buf), "FlyoutBg: HIDDEN %s", name);
    DebugLogLineA(buf);
    // #endregion
}

static bool TryHideFlyoutBackground(FrameworkElement element)
{
    bool hidden = false;
    bool foundBgCtrl = false;
    bool foundBgBorder = false;
    bool foundShadow = false;
    bool foundGrid = false;
    bool foundCanvas = false;

    // Hide the HoverFlyoutBackgroundControl (Taskbar.TaskbarBackground custom control)
    auto bgCtrl = FindDescendantByName(element, L"HoverFlyoutBackgroundControl");
    if (bgCtrl) {
        foundBgCtrl = true;
        HideElement(bgCtrl, "HoverFlyoutBackgroundControl");
        hidden = true;
    }

    // Hide HoverFlyoutBackground (Border) - the PARENT container that has its own Background brush
    auto bgBorder = FindDescendantByName(element, L"HoverFlyoutBackground");
    if (bgBorder) {
        foundBgBorder = true;
        HideElement(bgBorder, "HoverFlyoutBackground");
        hidden = true;
    }

    // Hide HoverFlyoutBackgroundShadowReceiver (Border with shadow)
    auto shadow = FindDescendantByName(element, L"HoverFlyoutBackgroundShadowReceiver");
    if (shadow) {
        foundShadow = true;
        HideElement(shadow, "HoverFlyoutBackgroundShadowReceiver");
        hidden = true;
    }

    // Also clear background on HoverFlyoutGrid and HoverFlyoutCanvas in case they have backgrounds
    auto grid = FindDescendantByName(element, L"HoverFlyoutGrid");
    if (grid) {
        foundGrid = true;
        if (auto panel = grid.try_as<Controls::Panel>()) {
            panel.Background(nullptr);
        }
        // #region agent log
        DebugLogLineA("FlyoutBg: Cleared HoverFlyoutGrid background");
        // #endregion
    }
    auto canvas = FindDescendantByName(element, L"HoverFlyoutCanvas");
    if (canvas) {
        foundCanvas = true;
        if (auto panel = canvas.try_as<Controls::Panel>()) {
            panel.Background(nullptr);
        }
        // #region agent log
        DebugLogLineA("FlyoutBg: Cleared HoverFlyoutCanvas background");
        // #endregion
    }

    // #region agent log
    {
        char data[256];
        snprintf(data, sizeof(data),
                 "{\"foundBgCtrl\":%d,\"foundBgBorder\":%d,\"foundShadow\":%d,"
                 "\"foundGrid\":%d,\"foundCanvas\":%d,\"hidden\":%d}",
                 (int)foundBgCtrl, (int)foundBgBorder, (int)foundShadow,
                 (int)foundGrid, (int)foundCanvas, (int)hidden);
        DebugLogHypothesisA("run-initial", "H3", "TryHideFlyoutBackground",
                            "search_and_hide_result", data);
    }
    // #endregion

    return hidden;
}

static void ProbeHostWindowBackdropSuppression(int retry)
{
    if (retry != 15) {
        return;
    }

    auto LogBackdropAttributes = [](HWND hWnd, const char* tag, const char* hypothesisId) {
        if (!hWnd) return;
        int backdropType = -1;
        BOOL useHostBackdrop = FALSE;
        HRESULT hrBackdrop = DwmGetWindowAttribute(hWnd, 38, &backdropType, sizeof(backdropType));
        HRESULT hrHostBackdrop = DwmGetWindowAttribute(hWnd, 17, &useHostBackdrop, sizeof(useHostBackdrop));

        // #region agent log
        char data[320];
        snprintf(data, sizeof(data),
                 "{\"tag\":\"%s\",\"hwnd\":\"%p\",\"hrBackdrop\":%ld,\"backdropType\":%d,"
                 "\"hrHostBackdrop\":%ld,\"useHostBackdrop\":%d}",
                 tag, (void*)hWnd, (long)hrBackdrop, backdropType,
                 (long)hrHostBackdrop, (int)useHostBackdrop);
        DebugLogHypothesisA("run-initial", hypothesisId, "ProbeHostWindowBackdropSuppression",
                            "backdrop_attributes_snapshot", data);
        // #endregion
    };

    auto LogHostWindowState = [](HWND hWnd, const char* tag) {
        if (!hWnd) return;

        RECT rc = {};
        GetWindowRect(hWnd, &rc);
        LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        BOOL cloaked = FALSE;
        DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        BYTE alpha = 255;
        DWORD layeredFlags = 0;
        GetLayeredWindowAttributes(hWnd, nullptr, &alpha, &layeredFlags);

        int visibleChildCount = 0;
        int bridgeChildCount = 0;
        RECT firstBridgeRect = {};
        bool haveBridgeRect = false;
        for (HWND c = GetWindow(hWnd, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
            if (IsWindowVisible(c)) {
                visibleChildCount++;
            }
            WCHAR cls[128] = {};
            GetClassName(c, cls, ARRAYSIZE(cls));
            if (wcsstr(cls, L"DesktopWindowContentBridge")) {
                bridgeChildCount++;
                if (!haveBridgeRect) {
                    GetWindowRect(c, &firstBridgeRect);
                    haveBridgeRect = true;
                }
            }
        }

        char data[640];
        snprintf(data, sizeof(data),
                 "{\"tag\":\"%s\",\"hwnd\":\"%p\",\"visible\":%d,"
                 "\"left\":%ld,\"top\":%ld,\"right\":%ld,\"bottom\":%ld,"
                 "\"style\":\"0x%llX\",\"exStyle\":\"0x%llX\","
                 "\"cloaked\":%d,\"alpha\":%u,\"layeredFlags\":\"0x%lX\","
                 "\"visibleChildCount\":%d,\"bridgeChildCount\":%d,"
                 "\"bridgeLeft\":%ld,\"bridgeTop\":%ld,\"bridgeRight\":%ld,\"bridgeBottom\":%ld,"
                 "\"bridgeRectKnown\":%d}",
                 tag, (void*)hWnd, (int)IsWindowVisible(hWnd),
                 rc.left, rc.top, rc.right, rc.bottom,
                 (unsigned long long)style, (unsigned long long)exStyle,
                 (int)cloaked, (unsigned int)alpha, layeredFlags,
                 visibleChildCount, bridgeChildCount,
                 firstBridgeRect.left, firstBridgeRect.top, firstBridgeRect.right, firstBridgeRect.bottom,
                 haveBridgeRect ? 1 : 0);
        DebugLogHypothesisA("run-initial", "H23", "ProbeHostWindowBackdropSuppression",
                            "host_window_backdrop_snapshot", data);
    };

    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    LogHostWindowState(hTray, "Shell_TrayWnd");

    HWND hSecondaryTray = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
    LogHostWindowState(hSecondaryTray, "Shell_SecondaryTrayWnd");

    int islandCount = 0;
    for (HWND h = FindWindowExW(nullptr, nullptr, L"XamlExplorerHostIslandWindow", nullptr);
         h;
         h = FindWindowExW(nullptr, h, L"XamlExplorerHostIslandWindow", nullptr))
    {
        if (!IsWindowVisible(h)) continue;
        islandCount++;
        LogHostWindowState(h, "XamlExplorerHostIslandWindow");
        LogBackdropAttributes(h, "XamlExplorerHostIslandWindow", "H27");
    }

    int visiblePopupCount = 0;
    for (HWND h = FindWindowExW(nullptr, nullptr, L"Xaml_WindowedPopupClass", nullptr);
         h;
         h = FindWindowExW(nullptr, h, L"Xaml_WindowedPopupClass", nullptr))
    {
        if (!IsWindowVisible(h)) continue;
        visiblePopupCount++;

        RECT rc = {};
        GetWindowRect(h, &rc);
        LONG_PTR style = GetWindowLongPtrW(h, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(h, GWL_EXSTYLE);
        HWND owner = GetWindow(h, GW_OWNER);
        HWND parent = GetParent(h);

        RECT ownerRc = {};
        if (owner) {
            GetWindowRect(owner, &ownerRc);
        }

        // #region agent log
        char data[640];
        snprintf(data, sizeof(data),
                 "{\"hwnd\":\"%p\",\"left\":%ld,\"top\":%ld,\"right\":%ld,\"bottom\":%ld,"
                 "\"style\":\"0x%llX\",\"exStyle\":\"0x%llX\",\"owner\":\"%p\",\"parent\":\"%p\","
                 "\"ownerLeft\":%ld,\"ownerTop\":%ld,\"ownerRight\":%ld,\"ownerBottom\":%ld}",
                 (void*)h, rc.left, rc.top, rc.right, rc.bottom,
                 (unsigned long long)style, (unsigned long long)exStyle,
                 (void*)owner, (void*)parent,
                 ownerRc.left, ownerRc.top, ownerRc.right, ownerRc.bottom);
        DebugLogHypothesisA("run-initial", "H28", "ProbeHostWindowBackdropSuppression",
                            "windowed_popup_snapshot", data);
        // #endregion
    }

    char summary[160];
    snprintf(summary, sizeof(summary),
             "{\"retry\":%d,\"visibleIslandCount\":%d,\"visiblePopupCount\":%d}",
             retry, islandCount, visiblePopupCount);
    DebugLogHypothesisA("run-initial", "H23", "ProbeHostWindowBackdropSuppression",
                        "host_window_backdrop_probe_summary", summary);
}

static void LogFlyoutAncestorAndRootEvidence(FrameworkElement element)
{
    auto HasBackground = [](DependencyObject node) -> int {
        bool hasBg = false;
        if (auto border = node.try_as<Controls::Border>()) {
            hasBg = hasBg || (border.Background() != nullptr);
        }
        if (auto panel = node.try_as<Controls::Panel>()) {
            hasBg = hasBg || (panel.Background() != nullptr);
        }
        if (auto control = node.try_as<Controls::Control>()) {
            hasBg = hasBg || (control.Background() != nullptr);
        }
        return hasBg ? 1 : 0;
    };

    auto xamlRoot = element.XamlRoot();
    auto rootContent = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
    if (rootContent) {
        // #region agent log
        char data[384];
        std::wstring rootClass{winrt::get_class_name(rootContent)};
        std::wstring rootName{rootContent.Name()};
        snprintf(data, sizeof(data),
                 "{\"class\":\"%ls\",\"name\":\"%ls\",\"w\":%.0f,\"h\":%.0f,\"vis\":%d,\"op\":%.2f,\"hasBg\":%d}",
                 rootClass.c_str(), rootName.c_str(),
                 rootContent.ActualWidth(), rootContent.ActualHeight(),
                 (int)rootContent.Visibility(), rootContent.Opacity(),
                 HasBackground(rootContent));
        DebugLogHypothesisA("run-initial", "H29", "LogFlyoutAncestorAndRootEvidence",
                            "flyout_xamlroot_content_state", data);
        // #endregion
    }

    auto current = element.try_as<DependencyObject>();
    int depth = 0;
    while (current && depth < 8) {
        auto fe = current.try_as<FrameworkElement>();
        if (fe) {
            // #region agent log
            char data[384];
            std::wstring cls{winrt::get_class_name(fe)};
            std::wstring name{fe.Name()};
            snprintf(data, sizeof(data),
                     "{\"depth\":%d,\"class\":\"%ls\",\"name\":\"%ls\",\"w\":%.0f,\"h\":%.0f,\"vis\":%d,\"op\":%.2f,\"hasBg\":%d}",
                     depth, cls.c_str(), name.c_str(),
                     fe.ActualWidth(), fe.ActualHeight(),
                     (int)fe.Visibility(), fe.Opacity(),
                     HasBackground(fe));
            DebugLogHypothesisA("run-initial", "H30", "LogFlyoutAncestorAndRootEvidence",
                                "flyout_ancestor_state", data);
            // #endregion
        }
        current = Media::VisualTreeHelper::GetParent(current);
        depth++;
    }
}

static void ProbeFlyoutAncestorBorderState(FrameworkElement element, int retry)
{
    auto current = Media::VisualTreeHelper::GetParent(element);
    int depth = 1;
    while (current && depth <= 4) {
        auto fe = current.try_as<FrameworkElement>();
        if (fe) {
            if (auto border = fe.try_as<Controls::Border>()) {
                bool hasBackground = border.Background() != nullptr;
                bool hasBorderBrush = border.BorderBrush() != nullptr;

                // #region agent log
                char data[448];
                std::wstring cls{winrt::get_class_name(fe)};
                std::wstring name{fe.Name()};
                snprintf(data, sizeof(data),
                         "{\"retry\":%d,\"depth\":%d,\"class\":\"%ls\",\"name\":\"%ls\","
                         "\"w\":%.0f,\"h\":%.0f,\"vis\":%d,\"op\":%.2f,"
                         "\"hasBackground\":%d,\"hasBorderBrush\":%d}",
                         retry, depth, cls.c_str(), name.c_str(),
                         fe.ActualWidth(), fe.ActualHeight(),
                         (int)fe.Visibility(), fe.Opacity(),
                         hasBackground ? 1 : 0, hasBorderBrush ? 1 : 0);
                DebugLogHypothesisA("run-initial", "H33", "ProbeFlyoutAncestorBorderState",
                                    "ancestor_border_state", data);
                // #endregion
                return;
            }
        }
        current = Media::VisualTreeHelper::GetParent(current);
        depth++;
    }

    // #region agent log
    char data[96];
    snprintf(data, sizeof(data), "{\"retry\":%d}", retry);
    DebugLogHypothesisA("run-initial", "H33", "ProbeFlyoutAncestorBorderState",
                        "no_ancestor_border_found", data);
    // #endregion
}

static void ProbeBandHitTestOwners(int retry)
{
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) {
        return;
    }

    RECT trayRc = {};
    GetWindowRect(hTray, &trayRc);
    POINT points[3] = {
        { (trayRc.left + trayRc.right) / 2, trayRc.top + 2 },    // near top edge of tray
        { (trayRc.left + trayRc.right) / 2, (trayRc.top + trayRc.bottom) / 2 }, // tray center
        { (trayRc.left + trayRc.right) / 2, trayRc.top - 2 }     // just above tray
    };

    for (int i = 0; i < 3; i++) {
        POINT p = points[i];
        HWND hAtPoint = WindowFromPoint(p);
        WCHAR cls[128] = {};
        if (hAtPoint) {
            GetClassNameW(hAtPoint, cls, ARRAYSIZE(cls));
        }
        HWND hRoot = hAtPoint ? GetAncestor(hAtPoint, GA_ROOT) : nullptr;
        WCHAR rootCls[128] = {};
        if (hRoot) {
            GetClassNameW(hRoot, rootCls, ARRAYSIZE(rootCls));
        }

        HWND trayChildReal = nullptr;
        HWND trayChildFast = nullptr;
        WCHAR trayChildRealCls[128] = {};
        WCHAR trayChildFastCls[128] = {};
        POINT trayClientPt = p;
        if (ScreenToClient(hTray, &trayClientPt)) {
            trayChildReal = RealChildWindowFromPoint(hTray, trayClientPt);
            trayChildFast = ChildWindowFromPointEx(hTray, trayClientPt, CWP_SKIPDISABLED);
            if (trayChildReal) {
                GetClassNameW(trayChildReal, trayChildRealCls, ARRAYSIZE(trayChildRealCls));
            }
            if (trayChildFast) {
                GetClassNameW(trayChildFast, trayChildFastCls, ARRAYSIZE(trayChildFastCls));
            }
        }

        // #region agent log
        char data[768];
        snprintf(data, sizeof(data),
                 "{\"retry\":%d,\"sample\":%d,\"x\":%ld,\"y\":%ld,"
                 "\"hwnd\":\"%p\",\"class\":\"%ls\",\"root\":\"%p\",\"rootClass\":\"%ls\","
                 "\"trayClientX\":%ld,\"trayClientY\":%ld,"
                 "\"trayChildReal\":\"%p\",\"trayChildRealClass\":\"%ls\","
                 "\"trayChildFast\":\"%p\",\"trayChildFastClass\":\"%ls\"}",
                 retry, i, p.x, p.y,
                 (void*)hAtPoint, cls, (void*)hRoot, rootCls,
                 trayClientPt.x, trayClientPt.y,
                 (void*)trayChildReal, trayChildRealCls,
                 (void*)trayChildFast, trayChildFastCls);
        DebugLogHypothesisA("run-initial", "H34", "ProbeBandHitTestOwners",
                            "band_hittest_window_owner", data);
        // #endregion
    }
}

static HWND FindPrimaryTrayBridgeWindow()
{
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray) return nullptr;
    for (HWND c = GetWindow(hTray, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
        WCHAR cls[128] = {};
        GetClassNameW(c, cls, ARRAYSIZE(cls));
        if (wcsstr(cls, L"DesktopWindowContentBridge")) {
            return c;
        }
    }
    return nullptr;
}

static bool IsTrayOrBridgeWindow(HWND hWnd, const char** tagOut)
{
    if (!hWnd) return false;
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray && hWnd == hTray) {
        if (tagOut) *tagOut = "Shell_TrayWnd";
        return true;
    }
    HWND hBridge = FindPrimaryTrayBridgeWindow();
    if (hBridge && hWnd == hBridge) {
        if (tagOut) *tagOut = "DesktopWindowContentBridge";
        return true;
    }
    return false;
}

static BOOL WINAPI SetWindowCompositionAttribute_Hook(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA* data)
{
    const char* targetTag = nullptr;
    WINDOWCOMPOSITIONATTRIBDATA patchedData{};
    ACCENT_POLICY patchedAccent{};
    bool applyPatch = false;

    if (data) {
        patchedData = *data;
    }

    // H69: patch every tray accent write with state=2.
    // If Start11 sets ACCENT_ENABLE_TRANSPARENTGRADIENT (state 2) on Shell_TrayWnd,
    // keep state=2 but force transparent gradient color on every write.
    // This removes timing races between first/second hover and late re-writes.
    if (data && !g_unloading && targetTag == nullptr && IsTrayOrBridgeWindow(hWnd, &targetTag) &&
        targetTag && lstrcmpA(targetTag, "Shell_TrayWnd") == 0 &&
        g_settings.start11CompatibilityFixEnabled &&
        data->Attrib == 19 &&
        data->pvData && data->cbData >= sizeof(ACCENT_POLICY)) {
        auto incomingAccent = reinterpret_cast<const ACCENT_POLICY*>(data->pvData);
        if (incomingAccent->AccentState == 2) {
            patchedAccent = *incomingAccent;
            patchedAccent.GradientColor = 0x00000000;
            patchedData.pvData = &patchedAccent;
            applyPatch = true;
        }
    }

    return g_setWindowCompositionAttribute_Original
               ? g_setWindowCompositionAttribute_Original(hWnd, applyPatch ? &patchedData : data)
               : FALSE;
}

static void LogPrimaryRootEvidenceSnapshot(int retry)
{
    auto xamlRoot = g_cachedPrimaryXamlRoot.get();
    if (!xamlRoot) {
        DebugLogHypothesisA("run-initial", "H23", "LogPrimaryRootEvidenceSnapshot",
                            "primary_root_missing", "{}");
        return;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        DebugLogHypothesisA("run-initial", "H23", "LogPrimaryRootEvidenceSnapshot",
                            "primary_root_content_missing", "{}");
        return;
    }

    // Safety recovery: previous probe iterations might have left these at opacity 0.
    // Restore immediately so taskbar items remain visible while debugging continues.
    try {
        auto taskbarFrame = FindDescendantByName(content, L"TaskbarFrame");
        if (taskbarFrame && taskbarFrame.Opacity() < 1.0) {
            taskbarFrame.Opacity(1.0);
        }
        auto rootGrid = FindDescendantByName(content, L"RootGrid");
        if (rootGrid && rootGrid.Opacity() < 1.0) {
            rootGrid.Opacity(1.0);
        }
        auto systemTrayFrame = FindDescendantByName(content, L"SystemTrayFrame");
        if (systemTrayFrame && systemTrayFrame.Opacity() < 1.0) {
            systemTrayFrame.Opacity(1.0);
        }
    } catch (...) {}

    if (retry == 15) {
        auto LogNamedElementState = [&](PCWSTR name, const char* hypothesisId) {
            auto target = FindDescendantByName(content, name);
            if (!target) {
                // #region agent log
                char missData[160];
                snprintf(missData, sizeof(missData),
                         "{\"name\":\"%ls\"}", name);
                DebugLogHypothesisA("run-initial", hypothesisId, "LogPrimaryRootEvidenceSnapshot",
                                    "primary_named_element_missing", missData);
                // #endregion
                return;
            }

            bool hasBg = false;
            if (auto border = target.try_as<Controls::Border>()) {
                hasBg = hasBg || (border.Background() != nullptr);
            }
            if (auto panel = target.try_as<Controls::Panel>()) {
                hasBg = hasBg || (panel.Background() != nullptr);
            }
            if (auto control = target.try_as<Controls::Control>()) {
                hasBg = hasBg || (control.Background() != nullptr);
            }

            int childVisualExists = 0;
            int childVisualVisible = 0;
            double childVisualOpacity = -1.0;
            {
                using namespace winrt::Windows::UI::Xaml::Hosting;
                auto cv = ElementCompositionPreview::GetElementChildVisual(target);
                if (cv) {
                    childVisualExists = 1;
                    childVisualVisible = cv.IsVisible() ? 1 : 0;
                    childVisualOpacity = cv.Opacity();
                }
            }

            std::wstring cls{winrt::get_class_name(target)};
            // #region agent log
            char stateData[640];
            snprintf(stateData, sizeof(stateData),
                     "{\"name\":\"%ls\",\"class\":\"%ls\",\"w\":%.0f,\"h\":%.0f,"
                     "\"vis\":%d,\"op\":%.2f,\"hasBg\":%d,"
                     "\"childVisualExists\":%d,\"childVisualVisible\":%d,\"childVisualOpacity\":%.3f}",
                     name, cls.c_str(),
                     target.ActualWidth(), target.ActualHeight(),
                     (int)target.Visibility(), target.Opacity(), hasBg ? 1 : 0,
                     childVisualExists, childVisualVisible, childVisualOpacity);
            DebugLogHypothesisA("run-initial", hypothesisId, "LogPrimaryRootEvidenceSnapshot",
                                "primary_named_element_state", stateData);
            // #endregion
        };

        LogNamedElementState(L"RootGrid", "H37");
        LogNamedElementState(L"TaskbarFrame", "H37");
        LogNamedElementState(L"SystemTrayFrame", "H37");
        LogNamedElementState(L"BackgroundControl", "H38");
        LogNamedElementState(L"BackgroundElement", "H38");
        LogNamedElementState(L"WindhawkUnifiedTaskbarBackground", "H38");
    }

    ProbeHostWindowBackdropSuppression(retry);
}

// #region agent log - recursive tree dump for flyout debugging
static void DumpFlyoutTree(DependencyObject node, int depth)
{
    if (depth > 8) return; // limit depth
    auto fe = node.try_as<FrameworkElement>();
    std::wstring className{winrt::get_class_name(node)};
    std::wstring name = fe ? std::wstring{fe.Name()} : L"";
    double w = fe ? fe.ActualWidth() : 0;
    double h = fe ? fe.ActualHeight() : 0;
    int vis = fe ? (int)fe.Visibility() : -1;
    double op = fe ? fe.Opacity() : -1;

    // Check for background brushes
    bool hasBg = false;
    if (auto border = node.try_as<Controls::Border>()) {
        hasBg = (border.Background() != nullptr);
    }
    if (auto panel = node.try_as<Controls::Panel>()) {
        hasBg = (panel.Background() != nullptr);
    }
    if (auto control = node.try_as<Controls::Control>()) {
        hasBg = hasBg || (control.Background() != nullptr);
    }

    char indent[32] = {};
    for (int i = 0; i < depth && i < 15; i++) indent[i] = ' ';

    char buf[512];
    snprintf(buf, sizeof(buf), "%s[%ls] name='%ls' %.0fx%.0f vis=%d op=%.2f bg=%d",
             indent, className.c_str(), name.c_str(), w, h, vis, op, (int)hasBg);
    DebugLogLineA(buf);

    int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(node, i);
        if (child) DumpFlyoutTree(child, depth + 1);
    }
}
// #endregion

static int g_flyoutBgRetry = 0;
void CALLBACK FlyoutBgHideTimerProc(HWND, UINT, UINT_PTR, DWORD)
{
    g_flyoutBgRetry++;
    if (g_flyoutBgRetry > 20 || g_unloading) {
        KillTimer(nullptr, g_flyoutBgTimerId);
        g_flyoutBgTimerId = 0;
        return;
    }

    try {
        auto element = g_pendingFlyoutElement.get();
        if (!element) {
            KillTimer(nullptr, g_flyoutBgTimerId);
            g_flyoutBgTimerId = 0;
            return;
        }

        TryHideFlyoutBackground(element);
    } catch (...) {
        KillTimer(nullptr, g_flyoutBgTimerId);
        g_flyoutBgTimerId = 0;
    }
}

void WINAPI FlyoutFrame_OnApplyTemplate_Hook(void *pThis)
{
    FlyoutFrame_OnApplyTemplate_Original(pThis);

    if (g_unloading || !pThis) return;

    // #region agent log
    {
        char data[128];
        snprintf(data, sizeof(data), "{\"pThis\":\"%p\"}", pThis);
        DebugLogHypothesisA("run-initial", "H2", "FlyoutFrame_OnApplyTemplate_Hook",
                            "hook_invoked", data);
    }
    // #endregion

    try {
        FrameworkElement flyoutElement = nullptr;

        // Try pThis directly as IUnknown (offset 0)
        {
            IUnknown *pUnk = reinterpret_cast<IUnknown*>(pThis);
            HRESULT hr = pUnk->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                              winrt::put_abi(flyoutElement));
            if (SUCCEEDED(hr) && flyoutElement) {
                // #region agent log
                DebugLogLineA("FlyoutFrame hook: QI succeeded with offset 0");
                // #endregion
            }
        }

        // If offset 0 failed, try offset 1 (same pattern as TaskbarFrame)
        if (!flyoutElement) {
            IUnknown **pArr = reinterpret_cast<IUnknown**>(pThis);
            IUnknown *pUnk = pArr[1];
            if (pUnk) {
                HRESULT hr = pUnk->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                                  winrt::put_abi(flyoutElement));
                if (SUCCEEDED(hr) && flyoutElement) {
                    // #region agent log
                    DebugLogLineA("FlyoutFrame hook: QI succeeded with offset 1");
                    // #endregion
                }
            }
        }

        if (!flyoutElement) {
            // #region agent log
            DebugLogLineA("FlyoutFrame hook: could not get FrameworkElement from pThis");
            // #endregion
            // #region agent log
            DebugLogHypothesisA("run-initial", "H2", "FlyoutFrame_OnApplyTemplate_Hook",
                                "framework_element_qi_failed", "{}");
            // #endregion
            return;
        }

        // Try immediate search (tree might be ready)
        TryHideFlyoutBackground(flyoutElement);

        // ALWAYS start deferred timer - the XAML framework applies visual states AFTER
        // OnApplyTemplate returns, which can reset our Visibility/Opacity changes.
        // The timer re-hides the background after the framework finishes.
        g_pendingFlyoutElement = winrt::make_weak(flyoutElement);
        g_flyoutBgRetry = 0;
        if (g_flyoutBgTimerId) KillTimer(nullptr, g_flyoutBgTimerId);
        g_flyoutBgTimerId = SetTimer(nullptr, 0, 16, FlyoutBgHideTimerProc);
        // #region agent log
        DebugLogLineA("FlyoutFrame hook: started deferred timer (16ms intervals)");
        // #endregion
        // #region agent log
        {
            char data[128];
            snprintf(data, sizeof(data), "{\"timerId\":%llu}",
                     (unsigned long long)g_flyoutBgTimerId);
            DebugLogHypothesisA("run-initial", "H4", "FlyoutFrame_OnApplyTemplate_Hook",
                                "deferred_hide_timer_started", data);
        }
        // #endregion
    } catch (...) {
        // #region agent log
        DebugLogLineA("FlyoutFrame hook: C++ exception caught");
        // #endregion
    }
}

// Hook for TaskbarFrame::SystemTrayExtent - called when system tray size changes
using TaskbarFrame_SystemTrayExtent_t = void(WINAPI *)(void *pThis, double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;

void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void *pThis, double value)
{
    TaskbarFrame_SystemTrayExtent_Original(pThis, value);

    if (g_unloading)
    {
        return;
    }

    // Validate pThis pointer before accessing
    if (!pThis)
    {
        Wh_Log(L"pThis is null, skipping");
        return;
    }

    // Get the TaskbarFrame element
    // The IUnknown pointer is at offset 1 in the object's vtable array
    IUnknown **pThisArray = (IUnknown **)pThis;
    IUnknown *pUnknown = pThisArray[1];
    if (!pUnknown)
    {
        Wh_Log(L"IUnknown pointer at offset 1 is null, skipping");
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    HRESULT hr = pUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                          winrt::put_abi(taskbarFrameElement));
    if (FAILED(hr) || !taskbarFrameElement)
    {
        Wh_Log(L"QueryInterface failed or returned null element, hr=%08X", hr);
        return;
    }

    auto xamlRoot = taskbarFrameElement.XamlRoot();
    if (xamlRoot)
    {
        bool isPrimaryRoot = IsPrimaryTaskbarXamlRoot(xamlRoot);
        if (isPrimaryRoot)
        {
            TrackTaskbarXamlRoot(xamlRoot, true);
            // Cache the primary XamlRoot for use by timers and fallback paths.
            g_cachedPrimaryXamlRoot = winrt::make_weak(xamlRoot);
        }
        else if (g_settings.applyToAllMonitors)
        {
            TrackTaskbarXamlRoot(xamlRoot, false);
        }

        if (!g_settings.applyToAllMonitors && !isPrimaryRoot)
        {
            return;
        }

        auto* layoutState = GetRootLayoutState(xamlRoot, true);
        if (!layoutState)
        {
            return;
        }

        // Re-entry guard: skip if we're already applying styles to prevent feedback loop
        if (g_isApplyingStyle.exchange(true))
        {
            return;
        }

        // Time-based debounce: skip if we applied styles very recently
        DWORD now = GetTickCount();
        if (now - layoutState->lastApplyTick < APPLY_STYLE_DEBOUNCE_MS)
        {
            g_isApplyingStyle = false;
            return;
        }

        // Oscillation detection: if value alternates between two states, skip to stabilize
        if (std::abs(value - g_lastLastSystemTrayExtent) < 1.0 &&
            std::abs(value - g_lastSystemTrayExtent) > 10.0)
        {
            g_oscillationCount++;
            if (g_oscillationCount > 2)
            {
                g_isApplyingStyle = false;
                g_lastLastSystemTrayExtent = g_lastSystemTrayExtent;
                g_lastSystemTrayExtent = value;
                return;
            }
        }
        else
        {
            g_oscillationCount = 0;
        }

        // Update value tracking
        g_lastLastSystemTrayExtent = g_lastSystemTrayExtent;
        g_lastSystemTrayExtent = value;

        RequestApplyForTaskbarRoot(xamlRoot, RefreshReason::SystemTrayExtent);
        g_isApplyingStyle = false;
    }
}

bool HookTaskbarDllSymbols()
{
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module)
{
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::OnApplyTemplate(void))"},
            &FlyoutFrame_OnApplyTemplate_Original,
            FlyoutFrame_OnApplyTemplate_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks)))
    {
        Wh_Log(L"HookSymbols failed for Taskbar.View.dll");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle()
{
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module)
    {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName)
{
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true))
    {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module))
        {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags)
{
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module)
    {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

static void InstallStart11ApiTraceHooks()
{
    if (!g_setWindowCompositionHookInstalled) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            auto pSetWindowCompositionAttribute =
                reinterpret_cast<SetWindowCompositionAttribute_t>(
                    GetProcAddress(user32, "SetWindowCompositionAttribute"));
            if (pSetWindowCompositionAttribute) {
                WindhawkUtils::SetFunctionHook(pSetWindowCompositionAttribute,
                                               SetWindowCompositionAttribute_Hook,
                                               &g_setWindowCompositionAttribute_Original);
                g_setWindowCompositionHookInstalled =
                    (g_setWindowCompositionAttribute_Original != nullptr);
            }
        }
    }
}

void LoadSettings()
{
    g_settings.centerPositionEnabled =
        Wh_GetIntSetting(L"Layout.centerEnabled");

    // Load and validate minTotalWidth (minimum 0)
    // 0 means disabled, negative values make no sense
    int minTotalWidth = Wh_GetIntSetting(L"Layout.minTotalWidth");
    g_settings.minTotalWidth = (std::max)(0, minTotalWidth);

    // Load gap without clamping
    // Negative values pull elements closer, positive push apart
    g_settings.gap = Wh_GetIntSetting(L"Layout.gap");

    // Load and validate maxTotalWidth (minimum 0)
    // 0 means disabled (no maximum), negative values make no sense
    int maxTotalWidth = Wh_GetIntSetting(L"Layout.maxTotalWidth");
    g_settings.maxTotalWidth = (std::max)(0, maxTotalWidth);

    // Load taskbar height setting
    // 0 means disabled (use default height), positive values set custom height
    int taskbarHeight = Wh_GetIntSetting(L"Layout.taskbarHeight");
    g_settings.taskbarHeight = (std::max)(0, taskbarHeight);

    // Load taskbar edge spacing
    int edgeSpacing = Wh_GetIntSetting(L"Layout.taskbarEdgeSpacing");
    g_settings.taskbarEdgeSpacing = (std::max)(0, edgeSpacing);

    // Load divider settings
    g_settings.dividerEnabled = Wh_GetIntSetting(L"Divider.enabled");
    g_settings.dividerStyle = ParseDividerStyleSetting();
    int dividerWidth = Wh_GetIntSetting(L"Divider.width");
    g_settings.dividerWidth = (std::max)(0, dividerWidth);
    int dividerMarginY = Wh_GetIntSetting(L"Divider.marginY");
    g_settings.dividerMarginY = (std::max)(0, dividerMarginY);
    int dividerPaddingX = Wh_GetIntSetting(L"Divider.paddingX");
    g_settings.dividerPaddingX = (std::max)(0, dividerPaddingX);

    // Load start button padding
    int startPadding = Wh_GetIntSetting(L"StartButton.leftPadding");
    g_settings.startButtonLeftPadding = (std::max)(0, startPadding);

    // Load unified background settings
    g_settings.unifiedBackgroundEnabled = Wh_GetIntSetting(L"UnifiedBackground.enabled");
    g_settings.unifiedBackgroundStyle = ParseUnifiedBackgroundStyleSetting();

    int cornerRadius = Wh_GetIntSetting(L"UnifiedBackground.cornerRadius");
    g_settings.unifiedBackgroundCornerRadius = (std::max)(0, cornerRadius);

    int blurAmount = Wh_GetIntSetting(L"UnifiedBackground.blurAmount");
    g_settings.unifiedBackgroundBlurAmount = (std::max)(0, blurAmount);

    g_settings.unifiedBackgroundTintColor =
        static_cast<UINT32>(Wh_GetIntSetting(L"UnifiedBackground.tintColor"));

    int borderThickness = Wh_GetIntSetting(L"UnifiedBackground.borderThickness");
    g_settings.unifiedBackgroundBorderThickness = (std::max)(0, borderThickness);

    // Load animation settings
    g_settings.animationEnabled = Wh_GetIntSetting(L"Animation.enabled");
    int animDuration = Wh_GetIntSetting(L"Animation.durationMs");
    g_settings.animationDurationMs = (std::max)(0, animDuration);
    g_settings.animationCurve = ParseAnimationCurveSetting();
    g_settings.applyToAllMonitors =
        Wh_GetIntSetting(L"Compatibility.applyToAllMonitors");
    g_settings.start11CompatibilityFixEnabled =
        Wh_GetIntSetting(L"Compatibility.start11CompatibilityFixEnabled");

    Wh_Log(L"Settings loaded: center=%d, minTotalWidth=%d, gap=%d, maxTotalWidth=%d, "
           L"taskbarHeight=%d, edgeSpacing=%d, divider=%d/%d/%d/%d style=%s, startPadding=%d",
           g_settings.centerPositionEnabled,
           g_settings.minTotalWidth, g_settings.gap, g_settings.maxTotalWidth,
           g_settings.taskbarHeight, g_settings.taskbarEdgeSpacing,
           g_settings.dividerEnabled, g_settings.dividerWidth,
           g_settings.dividerMarginY, g_settings.dividerPaddingX,
           GetDividerStyleName(g_settings.dividerStyle),
           g_settings.startButtonLeftPadding);

    Wh_Log(L"Unified background: enabled=%d style=%s, cornerRadius=%d, blurAmount=%d, "
           L"tintColor=0x%08X, borderThickness=%d",
           g_settings.unifiedBackgroundEnabled,
           GetUnifiedBackgroundStyleName(g_settings.unifiedBackgroundStyle),
           g_settings.unifiedBackgroundCornerRadius,
           g_settings.unifiedBackgroundBlurAmount, g_settings.unifiedBackgroundTintColor,
           g_settings.unifiedBackgroundBorderThickness);

    Wh_Log(L"Animation: enabled=%d, durationMs=%d, curve=%d (%s)",
           g_settings.animationEnabled, g_settings.animationDurationMs,
           g_settings.animationCurve, GetAnimationCurveName(g_settings.animationCurve));
    Wh_Log(L"Compatibility: applyToAllMonitors=%d, start11Fix=%d",
           g_settings.applyToAllMonitors,
           g_settings.start11CompatibilityFixEnabled);
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();
    InstallStart11ApiTraceHooks();

    if (!HookTaskbarDllSymbols())
    {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
    {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule))
        {
            return FALSE;
        }
    }
    else
    {
        Wh_Log(L"Taskbar view module not loaded yet");

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

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded)
    {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
        {
            if (!g_taskbarViewDllLoaded.exchange(true))
            {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule))
                {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L">");

    g_unloading = true;

    // Kill timers
    if (g_pollTimerId != 0)
    {
        KillTimer(nullptr, g_pollTimerId);
        g_pollTimerId = 0;
    }

    // Restore taskbar windows via ApplySettings.
    // With g_unloading=true, ApplyStyle safely removes transforms/backgrounds.
    // ApplySettingsFromTaskbarThread also replays any previously-applied roots.
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit()
{
    Wh_Log(L">");

    if (g_flyoutBgTimerId) {
        KillTimer(nullptr, g_flyoutBgTimerId);
        g_flyoutBgTimerId = 0;
    }

    ResetRootLayoutStates();
    ResetTrackedTaskbarRoots();
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    LoadSettings();

    // Reset per-root layout state so one-time settings (height, maxWidth,
    // backgrounds) get re-applied on the next ApplyStyle call.
    ResetRootLayoutStates();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

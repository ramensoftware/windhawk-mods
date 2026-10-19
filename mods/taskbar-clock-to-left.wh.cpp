// ==WindhawkMod==
// @id              taskbar-clock-to-left
// @name            Taskbar Clock to Left
// @description     Move the native Windows 11 clock/notification button to the left without shifting centered taskbar apps
// @version         2.3
// @author          pleromyst
// @github          https://github.com/pleromyst
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Clock to Left

Moves the native Windows 11 clock/notification button to the left side of the
taskbar while keeping centered app buttons centered.

The clock is hosted in its own left-side layer. Instead of adding columns or
invisible spacers, the mod reserves width only where needed and keeps the
native system-tray extent in sync, so the remaining tray stays flush with the
right edge.

## What it does

- Places the clock after the native Start, search and Task View buttons, and
  after the Widgets/weather button when it is visible on the left.
- Also handles a Start button pinned to the physical left edge by another mod.
- On a left-aligned taskbar, the clock stays in its native position by default;
  the left-align setting keeps it after the system-button prefix when the
  layout has room.
- Optionally moves these native tray groups before the clock: the control
  center, the non-activatable system-icon group, the hidden-icons chevron, and
  the notification-area application tray. Native data sources, panels, tooltips
  and clicks are preserved, and each group restores independently.
- In a crowded centered layout without fixed system buttons or Widgets, the mod
  can reserve the native app viewport after a real collision, so Windows can
  shrink buttons or overflow them while the clock stays on the left.

## Requirements and limits

- Windows 11's modern taskbar is required.
- Only one clock-relocation mod should be enabled at a time.
- The automatic centered reservation requires at least three visible apps, a
  complete native item source, no fixed left system buttons and no Widgets. It
  is not released automatically after apps decrease; turn the setting off to
  restore the reservation.
- An external left margin added to the taskbar repeater (for example by
  Taskbar Styler) is not reserved automatically in this version; the clock
  falls back to its native position when the apps collide.
- Unsupported native layouts, external property changes and unsafe geometry
  restore the clock instead of fighting the layout.

## Disable

Turn the mod off, or disable the relevant settings. Width, margin and sibling
order changes are restored. If another program changed the same property after
the mod did, the newer value is kept.

![Taskbar Clock to Left](https://i.imgur.com/Wew0LZh.png)

## Acknowledgements

This mod was inspired by [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) by [m417z](https://github.com/m417z).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keepOnLeftWhenLeftAligned: false
  $name: Keep the clock on the left when the taskbar is left-aligned
  $description: Keep the clock right after the Start, search and Task View buttons when Windows is set to left-align the taskbar.
- moveControlCenter: false
  $name: Move the control center before the clock
  $description: Move the network, sound and battery buttons to the left of the clock.
- moveNonActivatableStack: false
  $name: Move the non-activatable system-icon group
  $description: Move the small group of Windows system icons to the left of the clock.
- moveNotifyIconStack: false
  $name: Move the hidden-icons chevron
  $description: Move the arrow that shows hidden icons to the left of the clock.
- moveNotificationAreaIcons: false
  $name: Move the notification-area application tray
  $description: Move the small app icons next to the clock to its left.
- reserveCenteredViewport: false
  $name: Reserve the app viewport in a crowded centered layout
  $description: When a centered taskbar gets crowded and apps would overlap the clock, make room for the clock so Windows can shrink or hide the app buttons.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <optional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
// The taskbar uses WinUI 2 controls on top of system XAML.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

constexpr double kLayoutTolerance = 0.5;

// XAML layout distances are DIP. Preserve the verified values and keep time
// limits with different purposes separate even when their numbers match.
constexpr double kTaskbarLayoutGap = 4.0;
constexpr uint64_t kLayoutReadyTimeoutMs = 10000;
constexpr uint32_t kLayoutReadyMaxAttempts = 600;
constexpr uint64_t kCenteredProbeTimeoutMs = 10000;
constexpr uint32_t kAutomaticArrangeMaxCallbacks = 16;
constexpr uint64_t kAutomaticArrangeTimeoutMs = 1000;
constexpr double kNativeClockFontSize = 12.0;
constexpr float kNativeClockMeasureLimit = 10000.0f;
constexpr double kNativeClockDefaultChromeWidth = 24.0;
constexpr double kNativeClockMinChromeWidth = 8.0;
constexpr double kNativeClockMaxChromeWidth = 64.0;
constexpr double kNativeClockMinLayoutWidth = 64.0;
constexpr double kNativeClockMaxLayoutWidth = 180.0;

enum class SystemButtonKind { None, Start, Search, TaskView, Widgets };

struct HorizontalBounds {
    double left;
    double right;
};

struct SystemButtonLayout {
    uintptr_t identity;
    SystemButtonKind kind;
    bool visible;
    bool fixed;
    HorizontalBounds bounds;
    double marginLeft;
    double outerWidth{};
    double contentWidth{};
    double marginRight{};
};

struct FlowElementLayout {
    uintptr_t identity;
    HorizontalBounds bounds;
    bool overflow{};
};

struct TaskbarItemIdentity {
    int index;
    uintptr_t elementIdentity;
    uintptr_t itemIdentity;
    winrt::Windows::Foundation::IInspectable item{nullptr};
};

struct LeftHostLayoutSnapshot {
    uintptr_t repeaterIdentity;
    double contentLeft;
    double contentWidth;
    double rootWidth;
    double hostLeft;
    double hostWidth;
    double offset;
    bool centered{};
    bool unknownVisible{};
    double rightBoundary{};
    double repeaterDesiredWidth{};
    std::vector<uintptr_t> childIdentities;
    std::vector<SystemButtonLayout> buttons;
    std::vector<FlowElementLayout> flowElements;
    std::vector<TaskbarItemIdentity> itemIdentities;
};

struct WidgetsSpacingData {
    winrt::weak_ref<FrameworkElement> target;
    Thickness originalMargin;
    bool localUnset;
    double lastRight;
};

struct CenteredWidthConstraintData {
    winrt::weak_ref<Controls::Grid> parent;
    double originalWidth;
    HorizontalAlignment originalAlignment;
    bool widthLocalUnset;
    bool alignmentLocalUnset;
    double targetWidth{};
    uint64_t operation{};
    bool widthWritten{};
    bool alignmentWritten{};
};

struct FlowSpacingData {
    winrt::weak_ref<FrameworkElement> target;
    Thickness originalMargin;
    bool localUnset;
    double lastValue;
    double increment{};
    bool rightSide{};
    std::optional<CenteredWidthConstraintData> widthConstraint;
};

struct CenteredViewportProbeData {
    LeftHostLayoutSnapshot baseline;
    ULONGLONG startTick;
    double increment;
    bool restored{};
    bool failed{};
    bool appliedLogged{};
    bool automatic{};
    uint32_t initialLayoutWaits{};
    bool initialArrangeAccepted{};
};

struct MovedTrayElementData {
    winrt::weak_ref<FrameworkElement> element;
    winrt::weak_ref<Controls::Panel> originalParent;
    winrt::weak_ref<UIElement> previousSibling;
    winrt::weak_ref<UIElement> nextSibling;
    // Retain this measurement object while moved; still verify its original collection.
    Controls::ColumnDefinition nativeColumn{nullptr};
    uint32_t originalIndex;
    int originalColumn;
    bool columnLocalUnset;
    double nativeColumnWidth;
    int hostColumn{};
    winrt::Windows::Foundation::IInspectable itemsSource{nullptr};
    winrt::weak_ref<FrameworkElement> nativeChevron;
    bool columnChanged{};
    double lastContribution{-1};
};

struct MovedClockData {
    winrt::weak_ref<FrameworkElement> clock;
    winrt::weak_ref<Controls::Panel> originalParent;
    winrt::weak_ref<Controls::Grid> taskbarRoot;
    winrt::weak_ref<FrameworkElement> widgets;
    winrt::weak_ref<FrameworkElement> repeater;
    Controls::Grid leftHost{nullptr};
    std::optional<MovedTrayElementData> controlCenter;
    std::optional<MovedTrayElementData> trayGroup;
    std::optional<MovedTrayElementData> notifyIcons;
    std::optional<MovedTrayElementData> applicationTray;
    winrt::weak_ref<UIElement> originalPreviousSibling;
    winrt::weak_ref<UIElement> originalNextSibling;
    std::optional<winrt::event_token> widgetsSizeChangedToken;
    int64_t widgetsVisibilityChangedToken{};
    std::optional<winrt::event_token> rootLayoutUpdatedToken;
    std::optional<LeftHostLayoutSnapshot> layoutSnapshot;
    std::optional<WidgetsSpacingData> widgetsSpacing;
    std::optional<FlowSpacingData> flowSpacing;
    std::optional<CenteredViewportProbeData> centeredProbe;
    bool centeredProbeAttempted{};
    bool widgetsSpacingBlocked{};
    bool updatingLayout{};
    bool restoring{};
    bool layoutSampleUnavailableLogged{};
    double layoutReservedWidth;
    uint32_t originalIndex;
    int originalColumn;
    int originalColumnSpan;
    int originalRow;
    int originalRowSpan;
    HorizontalAlignment originalHorizontalAlignment;
    VerticalAlignment originalVerticalAlignment;
    Thickness originalMargin;
};

using TrayElementRecord = std::optional<MovedTrayElementData> MovedClockData::*;

struct DeferredClockData {
    winrt::weak_ref<FrameworkElement> content;
    std::optional<winrt::event_token> loadedToken;
    std::optional<winrt::event_token> layoutUpdatedToken;
    uint64_t loadedRegistration{};
    uint64_t layoutUpdatedRegistration{};
    ULONGLONG layoutUpdatedStartTick;
    uint32_t layoutUpdatedAttempts;
    int diagnosticAlignment{-1};
    winrt::weak_ref<FrameworkElement> centeredFallbackClock;
};

// Deferred event registration and cleanup run on the captured taskbar UI thread.
// A serial distinguishes an in-flight registration from a cancelled replacement.
uint64_t g_nextDeferredRegistration{};
// Paired property writes also run on this UI thread; reject replaced plans.
uint64_t g_nextCenteredWidthOperation{};

struct {
    bool keepOnLeftWhenLeftAligned;
    bool moveControlCenter;
    bool moveNonActivatableStack;
    bool moveNotifyIconStack;
    bool moveNotificationAreaIcons;
    bool reserveCenteredViewport;
    bool measureCenteredViewport;
    bool holdCenteredViewportForInteraction;
} g_settings;

void LoadSettings() {
    g_settings.keepOnLeftWhenLeftAligned =
        Wh_GetIntSetting(L"keepOnLeftWhenLeftAligned") != 0;
    g_settings.moveControlCenter = Wh_GetIntSetting(L"moveControlCenter") != 0;
    g_settings.moveNonActivatableStack = Wh_GetIntSetting(L"moveNonActivatableStack") != 0;
    g_settings.moveNotifyIconStack = Wh_GetIntSetting(L"moveNotifyIconStack") != 0;
    g_settings.moveNotificationAreaIcons = Wh_GetIntSetting(L"moveNotificationAreaIcons") != 0;
    g_settings.reserveCenteredViewport =
        Wh_GetIntSetting(L"reserveCenteredViewport") != 0;
    // Development-only finite diagnostics are not part of this release.
    // Do not read stale settings left by an older build.
    g_settings.measureCenteredViewport = false;
    g_settings.holdCenteredViewportForInteraction = false;
}

decltype(&SetWindowPos) SetWindowPos_Original;
bool g_notifyPopupHooked;

std::atomic<bool> g_callbacksEnabled;
std::atomic<bool> g_systemTrayHooked;
std::atomic<bool> g_taskbarViewHooked;
std::atomic<bool> g_systemTrayHookAttempted;
std::atomic<bool> g_taskbarViewHookAttempted;
std::atomic<DWORD> g_clockThreadId;
std::atomic<bool> g_extentThreadMismatchLogged;
std::atomic<int> g_lastTaskbarCenteredAlignment{-1};
std::atomic_flag g_hookSetupInProgress = ATOMIC_FLAG_INIT;

// BEGIN ALIGNMENT REGISTRY WATCH
// Observe setting changes without polling. Only the taskbar-thread callback
// below can update the alignment cache and invoke the existing placement code.
struct AlignmentRegistryWatch {
    HKEY key{};
    HANDLE event{};
    PTP_WAIT wait{};
    SRWLOCK lock = SRWLOCK_INIT;
    bool stopping = true;
};
AlignmentRegistryWatch g_alignmentRegistryWatch;
std::atomic<bool> g_alignmentRegistryWatchActive;

constexpr wchar_t kAlignmentWatchKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

void ApplyRegistryAlignmentSynchronously();

bool RegistryAlignmentNeedsUpdate() {
    DWORD value = MAXDWORD;
    DWORD size = sizeof(value);
    LSTATUS status = RegGetValueW(
        g_alignmentRegistryWatch.key, nullptr, L"TaskbarAl",
        RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"[alignment-watch] setting read failed: %ld", status);
        return false;
    }
    // Other values under Explorer\Advanced can change when Settings opens.
    // Ignore them: neither placement work nor normal logging is needed.
    int cached = g_lastTaskbarCenteredAlignment;
    return cached != (value != 0 ? 1 : 0);
}

void CALLBACK AlignmentRegistryWatchCallback(PTP_CALLBACK_INSTANCE,
                                             void*,
                                             PTP_WAIT wait,
                                             TP_WAIT_RESULT) {
    auto& watch = g_alignmentRegistryWatch;
    AcquireSRWLockExclusive(&watch.lock);
    if (watch.stopping) {
        ReleaseSRWLockExclusive(&watch.lock);
        return;
    }

    // Keep collecting registry changes while the taskbar handles this one.
    LSTATUS status = RegNotifyChangeKeyValue(
        watch.key, FALSE,
        REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_THREAD_AGNOSTIC,
        watch.event, TRUE);
    bool needsUpdate = RegistryAlignmentNeedsUpdate();
    ReleaseSRWLockExclusive(&watch.lock);

    // Never hold the watch lock across a synchronous call into the UI thread.
    if (needsUpdate && g_callbacksEnabled) {
        ApplyRegistryAlignmentSynchronously();
    }

    AcquireSRWLockExclusive(&watch.lock);
    if (status != ERROR_SUCCESS) {
        g_alignmentRegistryWatchActive = false;
        Wh_Log(L"[alignment-watch] re-arm failed: %ld; using old trigger",
               status);
    } else if (!watch.stopping) {
        // Re-arm the threadpool wait only after the UI call finishes. Rapid
        // changes coalesce instead of running concurrent placement callbacks.
        SetThreadpoolWait(wait, watch.event, nullptr);
    }
    ReleaseSRWLockExclusive(&watch.lock);
}

void StopAlignmentRegistryWatch() {
    auto& watch = g_alignmentRegistryWatch;
    AcquireSRWLockExclusive(&watch.lock);
    watch.stopping = true;
    if (watch.wait) {
        SetThreadpoolWait(watch.wait, nullptr, nullptr);
    }
    ReleaseSRWLockExclusive(&watch.lock);

    if (watch.wait) {
        // Drain before XAML cleanup or DLL unload, with no lock held.
        WaitForThreadpoolWaitCallbacks(watch.wait, TRUE);
        CloseThreadpoolWait(watch.wait);
        watch.wait = nullptr;
    }
    g_alignmentRegistryWatchActive = false;
    if (watch.key) {
        RegCloseKey(watch.key);
        watch.key = nullptr;
    }
    if (watch.event) {
        CloseHandle(watch.event);
        watch.event = nullptr;
    }
}

void StartAlignmentRegistryWatch() {
    auto& watch = g_alignmentRegistryWatch;
    if (watch.wait) {
        return;
    }

    LSTATUS status = RegOpenKeyExW(HKEY_CURRENT_USER, kAlignmentWatchKey, 0,
                                  KEY_QUERY_VALUE | KEY_NOTIFY, &watch.key);
    if (status == ERROR_SUCCESS) {
        watch.event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!watch.event) {
            status = GetLastError();
        }
    }
    if (status == ERROR_SUCCESS) {
        watch.wait = CreateThreadpoolWait(AlignmentRegistryWatchCallback,
                                          nullptr, nullptr);
        if (!watch.wait) {
            status = GetLastError();
        }
    }
    if (status == ERROR_SUCCESS) {
        status = RegNotifyChangeKeyValue(
            watch.key, FALSE,
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_THREAD_AGNOSTIC,
            watch.event, TRUE);
    }
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"[alignment-watch] start failed: %ld; using old trigger",
               status);
        StopAlignmentRegistryWatch();
        return;
    }

    g_alignmentRegistryWatchActive = true;
    // The existing initialization has finished before this watch starts.
    // Catch an alignment change that may have happened during initialization.
    if (RegistryAlignmentNeedsUpdate() && g_callbacksEnabled) {
        ApplyRegistryAlignmentSynchronously();
    }
    AcquireSRWLockExclusive(&watch.lock);
    watch.stopping = false;
    SetThreadpoolWait(watch.wait, watch.event, nullptr);
    ReleaseSRWLockExclusive(&watch.lock);
}
// END ALIGNMENT REGISTRY WATCH

[[clang::no_destroy]] std::optional<std::vector<DeferredClockData>>
    g_deferredClocks{std::in_place};
[[clang::no_destroy]] std::optional<std::vector<MovedClockData>>
    g_movedClocks{std::in_place};

std::vector<DeferredClockData>& DeferredClocks() {
    return *g_deferredClocks;
}

std::vector<MovedClockData>& MovedClocks() {
    return *g_movedClocks;
}

using DateTimeIconContent_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t DateTimeIconContent_OnApplyTemplate_Original;

using BadgeIconContent_get_ViewModel_t =
    HRESULT(WINAPI*)(LPVOID pThis, LPVOID pArgs);
BadgeIconContent_get_ViewModel_t BadgeIconContent_get_ViewModel_Original;

using TaskbarFrame_SystemTrayExtent_t =
    void(WINAPI*)(void* pThis, double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;

using TaskbarFrame_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                      int* alignment);
TaskbarFrame_get_Alignment_t TaskbarFrame_get_Alignment_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

std::vector<HWND> FindExplorerTaskbarWindows();
HWND FindExplorerTaskbarWindow();
void QueueKnownClockPlacementUpdate();

using RunFromWindowThreadProc = void (*)(void* parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc procedure,
                         void* parameter,
                         DWORD expectedThreadId = 0) {
    static const UINT message = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParameters {
        RunFromWindowThreadProc procedure;
        void* parameter;
        bool executed;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId || (expectedThreadId && threadId != expectedThreadId)) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* messageData =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (messageData->message == message) {
                    auto* runParameters =
                        reinterpret_cast<RunParameters*>(messageData->lParam);
                    runParameters->procedure(runParameters->parameter);
                    runParameters->executed = true;
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunParameters runParameters{procedure, parameter, false};
    // The parameters and callback both live in this mod. A timeout could let
    // this function return while the target thread is still entering the hook,
    // leaving it with dangling stack and code pointers during mod unloading.
    // Wait synchronously until the taskbar thread has finished the callback.
    SendMessage(window, message, 0,
                reinterpret_cast<LPARAM>(&runParameters));
    UnhookWindowsHookEx(hook);

    return runParameters.executed;
}

FrameworkElement FindAncestor(FrameworkElement element,
                              PCWSTR className,
                              PCWSTR name) {
    for (auto current = element; current;
         current = Media::VisualTreeHelper::GetParent(current)
                       .try_as<FrameworkElement>()) {
        if ((!*name || current.Name() == name) &&
            winrt::get_class_name(current) == className) {
            return current;
        }
    }

    return nullptr;
}

FrameworkElement FindDescendantByName(FrameworkElement element,
                                      PCWSTR name) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (child.Name() == name) {
            return child;
        }

        if (auto result = FindDescendantByName(child, name)) {
            return result;
        }
    }

    return nullptr;
}

bool ReadTaskbarUsesCenteredAlignment() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValue(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
            L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return true;
    }

    return value != 0;
}

bool TaskbarUsesCenteredAlignment() {
    int alignment = g_lastTaskbarCenteredAlignment;
    return alignment >= 0 ? alignment != 0
                          : ReadTaskbarUsesCenteredAlignment();
}

bool ShouldRelocateClock() {
    return TaskbarUsesCenteredAlignment() ||
           g_settings.keepOnLeftWhenLeftAligned;
}

void LogLayoutElement(FrameworkElement element, Controls::Grid root,
                      int siblingIndex, uint32_t depth) {
    try {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        auto margin = element.Margin();
        auto desired = element.DesiredSize();
        auto slot = Controls::Primitives::LayoutInformation::GetLayoutSlot(
            element);
        auto localMargin = element.ReadLocalValue(
            FrameworkElement::MarginProperty());
        bool bound = static_cast<bool>(element.GetBindingExpression(
            FrameworkElement::MarginProperty()));
        auto xamlRoot = element.XamlRoot();
        Wh_Log(L"[layout] node=%p parent=%p sibling=%d depth=%u "
               L"class=%s name=%s automationId=%s xamlRoot=%p "
               L"loaded=%d visibility=%d opacity=%.3f",
               winrt::get_abi(element), winrt::get_abi(parent), siblingIndex,
               depth, winrt::get_class_name(element).c_str(),
               element.Name().c_str(),
               Automation::AutomationProperties::GetAutomationId(element)
                   .c_str(),
               winrt::get_abi(xamlRoot), element.IsLoaded(),
               static_cast<int>(element.Visibility()), element.Opacity());
        Wh_Log(L"[layout] node=%p actual=(%.3f,%.3f) "
               L"desired=(%.3f,%.3f) slotParent=(%.3f,%.3f,%.3f,%.3f) "
               L"margin=(%.3f,%.3f,%.3f,%.3f) bound=%d "
               L"localUnset=%d localClass=%s",
               winrt::get_abi(element), element.ActualWidth(),
               element.ActualHeight(), desired.Width, desired.Height,
               slot.X, slot.Y, slot.Width, slot.Height,
               margin.Left, margin.Top, margin.Right, margin.Bottom, bound,
               localMargin == DependencyProperty::UnsetValue(),
               localMargin ? winrt::get_class_name(localMargin).c_str()
                           : L"<null>");
        if (xamlRoot && xamlRoot == root.XamlRoot()) {
            auto visual = element.TransformToVisual(root).TransformBounds(
                {0, 0, static_cast<float>(element.ActualWidth()),
                 static_cast<float>(element.ActualHeight())});
            Wh_Log(L"[layout] node=%p visualRoot=(%.3f,%.3f,%.3f,%.3f)",
                   winrt::get_abi(element), visual.X, visual.Y,
                   visual.Width, visual.Height);
            if (parent) {
                auto origin = parent.TransformToVisual(root)
                                  .TransformPoint({0, 0});
                auto offset = element.ActualOffset();
                double left = origin.X + offset.x;
                double right = left + element.ActualWidth();
                // This excludes only the element's own render transform.
                // Parent transforms can still affect this diagnostic candidate.
                Wh_Log(L"[layout] node=%p parentVisualOrigin=(%.3f,%.3f) "
                       L"actualOffset=(%.3f,%.3f) "
                       L"candidateRootX=(%.3f,%.3f) finite=%d",
                       winrt::get_abi(element), origin.X, origin.Y,
                       offset.x, offset.y, left, right,
                       std::isfinite(left) && std::isfinite(right));
            }
        }
    } catch (...) {
        Wh_Log(L"[layout] node=%p inspection failed: %08X",
               winrt::get_abi(element), winrt::to_hresult());
    }
}

void LogControlCenterCandidate(FrameworkElement clock, Controls::Grid root) {
    try {
        auto grid = Media::VisualTreeHelper::GetParent(clock)
                        .try_as<Controls::Grid>();
        if (!grid || grid.Name() != L"SystemTrayFrameGrid") {
            Wh_Log(L"[control-center] native parent Grid unavailable");
            return;
        }
        auto frame = Media::VisualTreeHelper::GetParent(grid)
                         .try_as<FrameworkElement>();
        if (!frame || winrt::get_class_name(frame) !=
                          L"SystemTray.SystemTrayFrame" ||
            Media::VisualTreeHelper::GetParent(frame) != root ||
            grid.XamlRoot() != root.XamlRoot()) {
            Wh_Log(L"[control-center] native parent chain mismatch");
            return;
        }

        auto children = grid.Children();
        if (children.Size() > 32 || grid.ColumnDefinitions().Size() > 32) {
            Wh_Log(L"[control-center] native Grid exceeds inspection limit");
            return;
        }
        FrameworkElement controlCenter{nullptr};
        int controlIndex = -1;
        int clockIndex = -1;
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (child == clock) {
                clockIndex = static_cast<int>(i);
            }
            if (!child || child.Name() != L"ControlCenterButton" ||
                winrt::get_class_name(child) != L"SystemTray.OmniButton") {
                continue;
            }
            if (controlCenter) {
                Wh_Log(L"[control-center] duplicate native candidates");
                return;
            }
            controlCenter = child;
            controlIndex = static_cast<int>(i);
        }
        if (!controlCenter || clockIndex < 0) {
            Wh_Log(L"[control-center] native sibling candidate unavailable");
            return;
        }

        Wh_Log(L"[control-center] BEGIN build=" WH_MOD_VERSION L" candidate=%p clock=%p "
               L"parent=%p frame=%p indices=(%d,%d) sameXamlRoot=%d "
               L"gridWidth=%.3f frameWidth=%.3f context=(%p,%p,%p)",
               winrt::get_abi(controlCenter), winrt::get_abi(clock),
               winrt::get_abi(grid), winrt::get_abi(frame), controlIndex,
               clockIndex, controlCenter.XamlRoot() == root.XamlRoot(),
               grid.ActualWidth(), frame.ActualWidth(),
               winrt::get_abi(grid.DataContext()),
               winrt::get_abi(frame.DataContext()),
               winrt::get_abi(root.DataContext()));
        LogLayoutElement(controlCenter, root, controlIndex, 0);

        // Read only at the existing explicit snapshot, before reparenting.
        // Local value classes expose expressions we must not overwrite.
        struct Property {
            PCWSTR name;
            DependencyProperty property;
        };
        const Property properties[] = {
            {L"Grid.Column", Controls::Grid::ColumnProperty()},
            {L"Grid.ColumnSpan", Controls::Grid::ColumnSpanProperty()},
            {L"Grid.Row", Controls::Grid::RowProperty()},
            {L"Grid.RowSpan", Controls::Grid::RowSpanProperty()},
            {L"HorizontalAlignment", FrameworkElement::HorizontalAlignmentProperty()},
            {L"VerticalAlignment", FrameworkElement::VerticalAlignmentProperty()},
            {L"Margin", FrameworkElement::MarginProperty()},
            {L"DataContext", FrameworkElement::DataContextProperty()},
            {L"Width", FrameworkElement::WidthProperty()},
            {L"MinWidth", FrameworkElement::MinWidthProperty()},
            {L"MaxWidth", FrameworkElement::MaxWidthProperty()},
        };
        for (auto element : {controlCenter, clock}) {
            Wh_Log(L"[control-center] node=%p grid=(%d,%d,%d,%d) "
                   L"alignment=(%d,%d) width=%.3f minWidth=%.3f "
                   L"maxWidth=%.3f context=%p",
                   winrt::get_abi(element), Controls::Grid::GetColumn(element),
                   Controls::Grid::GetColumnSpan(element),
                   Controls::Grid::GetRow(element),
                   Controls::Grid::GetRowSpan(element),
                   static_cast<int>(element.HorizontalAlignment()),
                   static_cast<int>(element.VerticalAlignment()), element.Width(),
                   element.MinWidth(), element.MaxWidth(),
                   winrt::get_abi(element.DataContext()));
            for (const auto& entry : properties) {
                auto local = element.ReadLocalValue(entry.property);
                Wh_Log(L"[control-center] node=%p property=%s bound=%d "
                       L"localUnset=%d localClass=%s",
                       winrt::get_abi(element), entry.name,
                       static_cast<bool>(element.GetBindingExpression(entry.property)),
                       local == DependencyProperty::UnsetValue(),
                       local ? winrt::get_class_name(local).c_str() : L"<null>");
            }
        }
        auto columns = grid.ColumnDefinitions();
        for (uint32_t i = 0; i < columns.Size(); i++) {
            auto column = columns.GetAt(i);
            auto width = column.Width();
            auto local = column.ReadLocalValue(Controls::ColumnDefinition::WidthProperty());
            Wh_Log(L"[control-center] column=%u width=(%.3f,%d) "
                   L"actualWidth=%.3f minWidth=%.3f maxWidth=%.3f "
                   L"localUnset=%d localClass=%s",
                   i, width.Value, static_cast<int>(width.GridUnitType),
                   column.ActualWidth(), column.MinWidth(), column.MaxWidth(),
                   local == DependencyProperty::UnsetValue(),
                   local ? winrt::get_class_name(local).c_str() : L"<null>");
        }
        Wh_Log(L"[control-center] END children=%u columns=%u; "
               L"inspection only, migration not enabled", children.Size(),
               columns.Size());
    } catch (...) {
        // Discovery must not affect clock placement or restoration.
        Wh_Log(L"[control-center] inspection failed: %08X", winrt::to_hresult());
    }
}

void LogTrayGroupCandidate(FrameworkElement clock, Controls::Grid root,
                           PCWSTR groupName = L"NonActivatableStack") {
    try {
        auto grid = Media::VisualTreeHelper::GetParent(clock).try_as<Controls::Grid>();
        auto frame = grid ? Media::VisualTreeHelper::GetParent(grid)
                                .try_as<FrameworkElement>() : nullptr;
        if (!grid || grid.Name() != L"SystemTrayFrameGrid" || !frame ||
            winrt::get_class_name(frame) != L"SystemTray.SystemTrayFrame" ||
            Media::VisualTreeHelper::GetParent(frame) != root ||
            grid.XamlRoot() != root.XamlRoot() || grid.Children().Size() > 32 ||
            grid.ColumnDefinitions().Size() > 32) {
            Wh_Log(L"[tray-group] native parent chain unavailable or exceeds inspection limit");
            return;
        }
        FrameworkElement group{nullptr};
        uint32_t groupIndex = 0;
        auto children = grid.Children();
        PCWSTR expectedClass = wcscmp(groupName, L"NotificationAreaIcons") == 0
            ? L"SystemTray.NotificationAreaIcons" : L"SystemTray.Stack";
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (!child || child.Name() != groupName ||
                winrt::get_class_name(child) != expectedClass) continue;
            if (group) {
                Wh_Log(L"[tray-group] duplicate %s candidates", groupName);
                return;
            }
            group = child;
            groupIndex = i;
        }
        if (!group) {
            Wh_Log(L"[tray-group] %s candidate unavailable", groupName);
            return;
        }
        Wh_Log(L"[tray-group] BEGIN build=" WH_MOD_VERSION L" name=%s candidate=%p "
               L"parent=%p frame=%p sibling=%u sameXamlRoot=%d "
               L"gridWidth=%.3f context=(%p,%p,%p,%p)",
               groupName, winrt::get_abi(group), winrt::get_abi(grid), winrt::get_abi(frame),
               groupIndex, group.XamlRoot() == root.XamlRoot(), grid.ActualWidth(),
               winrt::get_abi(group.DataContext()), winrt::get_abi(grid.DataContext()),
               winrt::get_abi(frame.DataContext()), winrt::get_abi(root.DataContext()));
        Wh_Log(L"[tray-group] grid=(%d,%d,%d,%d) alignment=(%d,%d) "
               L"width=%.3f minWidth=%.3f maxWidth=%.3f",
               Controls::Grid::GetColumn(group), Controls::Grid::GetColumnSpan(group),
               Controls::Grid::GetRow(group), Controls::Grid::GetRowSpan(group),
               static_cast<int>(group.HorizontalAlignment()),
               static_cast<int>(group.VerticalAlignment()), group.Width(),
               group.MinWidth(), group.MaxWidth());
        struct Property { PCWSTR name; DependencyProperty property; };
        const Property properties[] = {
            {L"Grid.Column", Controls::Grid::ColumnProperty()},
            {L"Grid.ColumnSpan", Controls::Grid::ColumnSpanProperty()},
            {L"Grid.Row", Controls::Grid::RowProperty()},
            {L"Grid.RowSpan", Controls::Grid::RowSpanProperty()},
            {L"HorizontalAlignment", FrameworkElement::HorizontalAlignmentProperty()},
            {L"VerticalAlignment", FrameworkElement::VerticalAlignmentProperty()},
            {L"Margin", FrameworkElement::MarginProperty()},
            {L"DataContext", FrameworkElement::DataContextProperty()},
            {L"Width", FrameworkElement::WidthProperty()},
            {L"MinWidth", FrameworkElement::MinWidthProperty()},
            {L"MaxWidth", FrameworkElement::MaxWidthProperty()},
        };
        for (auto element : {group, grid.as<FrameworkElement>(), frame, root.as<FrameworkElement>()}) {
            for (const auto& entry : properties) {
                if (element != group && entry.property != FrameworkElement::DataContextProperty()) continue;
                auto local = element.ReadLocalValue(entry.property);
                Wh_Log(L"[tray-group] node=%p property=%s bound=%d localUnset=%d localClass=%s",
                       winrt::get_abi(element), entry.name,
                       static_cast<bool>(element.GetBindingExpression(entry.property)),
                       local == DependencyProperty::UnsetValue(),
                       local ? winrt::get_class_name(local).c_str() : L"<null>");
            }
        }
        int columnIndex = Controls::Grid::GetColumn(group);
        if (columnIndex >= 0 && static_cast<uint32_t>(columnIndex) < grid.ColumnDefinitions().Size()) {
            auto column = grid.ColumnDefinitions().GetAt(columnIndex);
            auto width = column.Width();
            auto local = column.ReadLocalValue(Controls::ColumnDefinition::WidthProperty());
            uint32_t shared = 0, unknown = 0;
            for (auto child : children) {
                auto element = child.try_as<FrameworkElement>();
                if (!element) { unknown++; continue; }
                int first = Controls::Grid::GetColumn(element);
                int span = Controls::Grid::GetColumnSpan(element);
                shared += element != group && first <= columnIndex && columnIndex - first < span;
            }
            Wh_Log(L"[tray-group] column=%d object=%p width=(%.3f,%d) actualWidth=%.3f "
                   L"minWidth=%.3f maxWidth=%.3f sharedSiblings=%u untypedSiblings=%u localUnset=%d localClass=%s",
                   columnIndex, winrt::get_abi(column), width.Value, static_cast<int>(width.GridUnitType),
                   column.ActualWidth(), column.MinWidth(), column.MaxWidth(), shared, unknown,
                   local == DependencyProperty::UnsetValue(),
                   local ? winrt::get_class_name(local).c_str() : L"<null>");
        } else {
            Wh_Log(L"[tray-group] original column unavailable: %d", columnIndex);
        }
        // Inspect this group's subtree because the whole-root snapshot can truncate it.
        struct Node { FrameworkElement element; int sibling; uint32_t depth; };
        std::vector<Node> nodes{{group, static_cast<int>(groupIndex), 0}};
        bool truncated = false;
        for (size_t i = 0; i < nodes.size(); i++) {
            auto node = nodes[i];
            LogLayoutElement(node.element, root, node.sibling, node.depth);
            if (group.Name() == L"NotifyIconStack") {
                auto name = Automation::AutomationProperties::GetName(node.element);
                if (!name.empty()) {
                    Wh_Log(L"[tray-group] accessible node=%p name=%.160s",
                           winrt::get_abi(node.element), name.c_str());
                }
            }
            if (auto items = node.element.try_as<Controls::ItemsControl>()) {
                auto source = items.ItemsSource();
                auto local = items.ReadLocalValue(Controls::ItemsControl::ItemsSourceProperty());
                Wh_Log(L"[tray-group] items node=%p count=%u source=%p sourceClass=%s "
                       L"bound=%d localUnset=%d localClass=%s context=%p",
                       winrt::get_abi(items), items.Items().Size(), winrt::get_abi(source),
                       source ? winrt::get_class_name(source).c_str() : L"<null>",
                       static_cast<bool>(items.GetBindingExpression(Controls::ItemsControl::ItemsSourceProperty())),
                       local == DependencyProperty::UnsetValue(),
                       local ? winrt::get_class_name(local).c_str() : L"<null>",
                       winrt::get_abi(items.DataContext()));
                if (group.Name() == L"NotifyIconStack" || group.Name() == L"MainStack" ||
                    group.Name() == L"NotificationAreaIcons") {
                    auto values = items.Items();
                    uint32_t count = values.Size();
                    for (uint32_t j = 0; j < std::min(count, 16u); j++) {
                        auto value = values.GetAt(j);
                        Wh_Log(L"[tray-group] item node=%p index=%u object=%p class=%s",
                               winrt::get_abi(items), j, winrt::get_abi(value),
                               value ? winrt::get_class_name(value).c_str() : L"<null>");
                    }
                    if (count > 16) Wh_Log(L"[tray-group] item metadata truncated count=%u", count);
                }
            }
            int count = Media::VisualTreeHelper::GetChildrenCount(node.element);
            if (node.depth >= 8) { truncated |= count > 0; continue; }
            truncated |= count > 32;
            for (int j = 0; j < std::min(count, 32); j++) {
                auto child = Media::VisualTreeHelper::GetChild(node.element, j).try_as<FrameworkElement>();
                if (!child) continue;
                if (nodes.size() == 64) { truncated = true; break; }
                nodes.push_back({child, j, node.depth + 1});
            }
        }
        Wh_Log(L"[tray-group] END name=%s nodes=%u truncated=%d; read-only inspection, group not moved",
               groupName, static_cast<uint32_t>(nodes.size()), truncated);
    } catch (...) {
        Wh_Log(L"[tray-group] inspection failed: %08X", winrt::to_hresult());
    }
}

void LogLayoutSnapshot(FrameworkElement clock, Controls::Grid root,
                       PCWSTR reason) {
    if (!g_callbacksEnabled || g_clockThreadId != GetCurrentThreadId()) {
        return;
    }

    try {
        if (!clock || !root || !root.IsLoaded() || !root.XamlRoot() ||
            root.ActualWidth() <= 0 || root.ActualHeight() <= 0) {
            return;
        }

        Wh_Log(L"[layout] BEGIN build=" WH_MOD_VERSION L" reason=%s thread=%u "
               L"centered=%d root=%p xamlRoot=%p viewport=(0,%.3f) "
               L"height=%.3f scale=%.3f",
               reason, GetCurrentThreadId(), TaskbarUsesCenteredAlignment(),
               winrt::get_abi(root), winrt::get_abi(root.XamlRoot()),
               root.ActualWidth(), root.ActualHeight(),
               root.XamlRoot().RasterizationScale());
        LogLayoutElement(clock, root, -1, 0);
        LogControlCenterCandidate(clock, root);
        LogTrayGroupCandidate(clock, root);
        LogTrayGroupCandidate(clock, root, L"NotifyIconStack");
        LogTrayGroupCandidate(clock, root, L"MainStack");
        LogTrayGroupCandidate(clock, root, L"NotificationAreaIcons");

        // Discovery is bounded and runs only at explicit diagnostic points,
        // never on every LayoutUpdated callback. Indices refer to the live tree.
        struct Node {
            FrameworkElement element;
            int siblingIndex;
            uint32_t depth;
        };
        constexpr size_t kMaxNodes = 128;
        constexpr int kMaxChildren = 32;
        constexpr uint32_t kMaxDepth = 10;
        std::vector<Node> nodes{{root, -1, 0}};
        bool truncated = false;
        for (size_t i = 0; i < nodes.size(); i++) {
            // Copy before appending: push_back may invalidate vector entries.
            auto node = nodes[i];
            LogLayoutElement(node.element, root, node.siblingIndex, node.depth);
            int count = Media::VisualTreeHelper::GetChildrenCount(node.element);
            if (node.depth >= kMaxDepth) {
                truncated |= count > 0;
                continue;
            }
            truncated |= count > kMaxChildren;
            for (int j = 0; j < std::min(count, kMaxChildren); j++) {
                auto child = Media::VisualTreeHelper::GetChild(node.element, j)
                                 .try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }
                if (nodes.size() == kMaxNodes) {
                    truncated = true;
                    break;
                }
                nodes.push_back({child, j, node.depth + 1});
            }
        }
        Wh_Log(L"[layout] END nodes=%u truncated=%d; "
               L"candidate coordinates are diagnostic, not layout policy",
               static_cast<uint32_t>(nodes.size()), truncated);
    } catch (...) {
        // A diagnostic failure must not prevent the existing relocation path.
        Wh_Log(L"[layout] snapshot failed: %08X", winrt::to_hresult());
    }
}

bool IsValidHorizontalBounds(HorizontalBounds bounds) {
    return std::isfinite(bounds.left) && std::isfinite(bounds.right) &&
           bounds.left < bounds.right;
}

bool IntersectsViewport(HorizontalBounds bounds, HorizontalBounds viewport) {
    return IsValidHorizontalBounds(bounds) &&
           IsValidHorizontalBounds(viewport) &&
           bounds.left < viewport.right && viewport.left < bounds.right;
}

SystemButtonKind IdentifySystemButton(FrameworkElement element) {
    if (!element) {
        return SystemButtonKind::None;
    }
    auto className = winrt::get_class_name(element);
    if (className == L"Taskbar.ExperienceToggleButton") {
        auto id = Automation::AutomationProperties::GetAutomationId(element);
        if (id == L"StartButton") {
            return SystemButtonKind::Start;
        }
        if (id == L"TaskViewButton") {
            return SystemButtonKind::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton" &&
               element.Name() == L"AugmentedEntryPointButton") {
        return SystemButtonKind::Widgets;
    } else if (className == L"Taskbar.TaskbarExtensionElement" &&
               Media::VisualTreeHelper::GetChildrenCount(element) == 1) {
        // Only the search-icon template confirmed by the supplied logs.
        auto presenter = Media::VisualTreeHelper::GetChild(element, 0)
                             .try_as<FrameworkElement>();
        if (presenter && winrt::get_class_name(presenter) ==
                             L"Windows.UI.Xaml.Controls.ContentPresenter" &&
            Media::VisualTreeHelper::GetChildrenCount(presenter) == 1) {
            auto content = Media::VisualTreeHelper::GetChild(presenter, 0);
            if (winrt::get_class_name(content) ==
                L"SearchUx.SearchUI.SearchButtonControl") {
                return SystemButtonKind::Search;
            }
        }
    }
    return SystemButtonKind::None;
}

bool IsTaskbarRepeater(FrameworkElement repeater, Controls::Grid root) {
    if (!repeater || repeater.Name() != L"TaskbarFrameRepeater" ||
        winrt::get_class_name(repeater) !=
            L"Microsoft.UI.Xaml.Controls.ItemsRepeater") {
        return false;
    }
    auto grid = Media::VisualTreeHelper::GetParent(repeater)
                    .try_as<Controls::Grid>();
    auto frame = grid ? Media::VisualTreeHelper::GetParent(grid)
                           .try_as<FrameworkElement>()
                      : nullptr;
    return grid && grid.Name() == L"RootGrid" && frame &&
           frame.Name() == L"TaskbarFrame" &&
           winrt::get_class_name(frame) == L"Taskbar.TaskbarFrame" &&
           Media::VisualTreeHelper::GetParent(frame) == root;
}

FrameworkElement FindDirectChildByName(FrameworkElement parent, PCWSTR name) {
    if (parent) {
        int count = Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto child = Media::VisualTreeHelper::GetChild(parent, i)
                             .try_as<FrameworkElement>();
            if (child && child.Name() == name) {
                return child;
            }
        }
    }
    return nullptr;
}

FrameworkElement FindTaskbarRepeater(Controls::Grid root) {
    auto frame = FindDirectChildByName(root, L"TaskbarFrame");
    auto grid = FindDirectChildByName(frame, L"RootGrid");
    auto repeater = FindDirectChildByName(grid, L"TaskbarFrameRepeater");
    return IsTaskbarRepeater(repeater, root) ? repeater : nullptr;
}

std::optional<std::vector<FrameworkElement>> GetRealizedTaskbarElements(
    FrameworkElement element,
    std::vector<TaskbarItemIdentity>* itemIdentities = nullptr) {
    std::vector<FrameworkElement> elements;
    auto repeater =
        element.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return std::nullopt;
    }
    auto source = repeater.ItemsSourceView();
    if (!source) {
        return std::nullopt;
    }
    int count = source.Count();
    for (int i = 0; i < count; i++) {
        auto child = repeater.TryGetElement(i).try_as<FrameworkElement>();
        if (child) {
            elements.push_back(child);
        }
        if (itemIdentities) {
            uintptr_t itemIdentity = 0;
            winrt::Windows::Foundation::IInspectable item{nullptr};
            try {
                item = source.GetAt(i);
                itemIdentity = reinterpret_cast<uintptr_t>(winrt::get_abi(item));
            } catch (...) {
                // A diagnostic read must not change the realization contract.
                Wh_Log(L"[layout] item identity unavailable index=%d error=%08X",
                       i, winrt::to_hresult());
            }
            itemIdentities->push_back({i,
                reinterpret_cast<uintptr_t>(winrt::get_abi(child)), itemIdentity, item});
        }
    }
    if (elements.empty()) {
        return std::nullopt;
    }
    return elements;
}

std::optional<HorizontalBounds> TryGetLayoutBounds(
    FrameworkElement element, Controls::Grid root, double width) {
    if (!element || !root || !element.IsLoaded() || !root.XamlRoot() ||
        element.XamlRoot() != root.XamlRoot() ||
        element.Visibility() != Visibility::Visible ||
        !std::isfinite(width) || width <= 0 ||
        !std::isfinite(element.ActualHeight()) || element.ActualHeight() <= 0) {
        return std::nullopt;
    }
    auto parent = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    // Reject ancestor render/composition transforms, including translations.
    // The element's own Dock transform is deliberately excluded below.
    auto ancestor = parent;
    while (ancestor && ancestor != root) {
        if (!ancestor.IsLoaded() ||
            ancestor.Visibility() != Visibility::Visible) {
            return std::nullopt;
        }
        auto next = Media::VisualTreeHelper::GetParent(ancestor)
                        .try_as<FrameworkElement>();
        if (!next) {
            return std::nullopt;
        }
        auto transform = ancestor.TransformToVisual(next);
        auto origin = transform.TransformPoint({0, 0});
        auto xAxis = transform.TransformPoint({1, 0});
        auto yAxis = transform.TransformPoint({0, 1});
        auto offset = ancestor.ActualOffset();
        if (!std::isfinite(origin.X) || !std::isfinite(origin.Y) ||
            !std::isfinite(offset.x) || !std::isfinite(offset.y) ||
            !std::isfinite(xAxis.X) || !std::isfinite(xAxis.Y) ||
            !std::isfinite(yAxis.X) || !std::isfinite(yAxis.Y) ||
            std::abs(origin.X - offset.x) > kLayoutTolerance ||
            std::abs(origin.Y - offset.y) > kLayoutTolerance ||
            std::abs(xAxis.X - origin.X - 1) > 0.0001 ||
            std::abs(xAxis.Y - origin.Y) > 0.0001 ||
            std::abs(yAxis.X - origin.X) > 0.0001 ||
            std::abs(yAxis.Y - origin.Y - 1) > 0.0001) {
            return std::nullopt;
        }
        ancestor = next;
    }
    if (ancestor != root || root.Visibility() != Visibility::Visible) {
        return std::nullopt;
    }
    auto origin = parent.TransformToVisual(root).TransformPoint({0, 0});
    auto offset = element.ActualOffset();
    if (!std::isfinite(offset.x) || !std::isfinite(offset.y)) {
        return std::nullopt;
    }
    HorizontalBounds bounds{origin.X + offset.x, origin.X + offset.x + width};
    return IsValidHorizontalBounds(bounds) ? std::optional{bounds}
                                           : std::nullopt;
}

std::optional<double> IdentifyFixedSystemPrefix(
    std::vector<SystemButtonLayout>& buttons, bool centered, double origin,
    uintptr_t spacingTarget, double spacingIncrement) {
    // The known Start-left implementation arranges a cluster in this order.
    // Match that arrangement, not a screen-half heuristic or negative Margin
    // alone. An unpinned preceding button stops the cluster.
    double right = origin;
    bool hasPrefix = false;
    bool pinned = true;
    bool followsSpacing = false;
    double appliedShift = 0;
    for (auto& button : buttons) {
        button.fixed = false;
    }
    for (auto kind : {SystemButtonKind::Start, SystemButtonKind::Search,
                      SystemButtonKind::TaskView}) {
        for (auto& button : buttons) {
            if (button.kind != kind || !button.visible) {
                continue;
            }
            double left = button.bounds.left - appliedShift;
            // A newly shown system button can still follow our previous anchor.
            // Normalize only that verified reservation, keeping real bounds intact.
            if (pinned && followsSpacing && appliedShift == 0 &&
                std::abs(left - right) > kLayoutTolerance &&
                std::abs(left - right - spacingIncrement) <= kLayoutTolerance) {
                appliedShift = spacingIncrement;
                left -= appliedShift;
            }
            button.fixed = pinned &&
                std::abs(left - right) <= kLayoutTolerance;
            pinned = button.fixed;
            if (button.fixed) {
                right = button.bounds.right - appliedShift;
                hasPrefix = true;
                followsSpacing |= !centered &&
                    button.identity == spacingTarget &&
                    std::isfinite(spacingIncrement) && spacingIncrement > 0;
            }
        }
        if (kind == SystemButtonKind::Start && !hasPrefix) {
            pinned = false;
        }
    }
    for (auto& button : buttons) {
        if (button.kind == SystemButtonKind::Widgets && button.visible) {
            // Native Widgets is left-arranged at its local left Margin. The
            // Start-left mod changes that Margin to place it after its cluster.
            button.fixed = std::abs(button.bounds.left -
                origin - button.marginLeft) <= kLayoutTolerance;
            if (button.fixed) {
                right = std::max(right, button.bounds.right);
                hasPrefix = true;
            }
        }
    }
    return hasPrefix ? std::optional{right} : std::nullopt;
}

double ComputeLeftHostOffset(double origin, std::optional<double> prefixRight) {
    return prefixRight ? *prefixRight + kTaskbarLayoutGap : origin;
}

double GetLeftHostOffset(LeftHostLayoutSnapshot& snapshot,
                         std::optional<double> prefixOrigin = std::nullopt,
                         uintptr_t spacingTarget = 0,
                         double spacingIncrement = 0) {
    double origin = prefixOrigin.value_or(snapshot.contentLeft);
    auto prefixRight = IdentifyFixedSystemPrefix(snapshot.buttons, snapshot.centered,
                                                 origin, spacingTarget, spacingIncrement);
    return ComputeLeftHostOffset(origin, prefixRight);
}

double GetLeftAlignedRightBoundary(const LeftHostLayoutSnapshot& layout,
                                   double reservedRightWidth) {
    double boundary = layout.rightBoundary - reservedRightWidth;
    for (const auto& button : layout.buttons) {
        if (button.visible && !button.fixed &&
            button.kind == SystemButtonKind::Widgets) {
            // Native trailing Widgets is a boundary, not part of the app flow.
            // The native clock compensation belongs to the tray boundary only.
            boundary = std::min(boundary, button.bounds.left);
        }
    }
    return boundary;
}

std::optional<LeftHostLayoutSnapshot> ReadLeftHostLayout(
    Controls::Grid root, FrameworkElement repeater, Controls::Grid host,
    std::optional<double> prefixOrigin = std::nullopt,
    uintptr_t spacingTarget = 0, double spacingIncrement = 0) {
    if (!IsTaskbarRepeater(repeater, root) || !root.IsLoaded() ||
        !std::isfinite(root.ActualWidth()) || root.ActualWidth() <= 0) {
        return std::nullopt;
    }
    auto repeaterBounds = TryGetLayoutBounds(repeater, root,
                                             repeater.ActualWidth());
    if (!repeaterBounds) {
        return std::nullopt;
    }
    LeftHostLayoutSnapshot snapshot{
        .repeaterIdentity = reinterpret_cast<uintptr_t>(winrt::get_abi(repeater)),
        .contentLeft = repeaterBounds->left,
        .contentWidth = repeaterBounds->right - repeaterBounds->left,
        .rootWidth = root.ActualWidth(),
        .hostLeft = host ? host.ActualOffset().x : 0,
        .hostWidth = host ? host.ActualWidth() : 0,
        .centered = TaskbarUsesCenteredAlignment(),
        .rightBoundary = root.ActualWidth(),
        .repeaterDesiredWidth = repeater.DesiredSize().Width,
    };
    if (!snapshot.centered || g_settings.measureCenteredViewport ||
        g_settings.reserveCenteredViewport || g_settings.moveControlCenter ||
        g_settings.moveNonActivatableStack || g_settings.moveNotifyIconStack ||
        g_settings.moveNotificationAreaIcons) {
        FrameworkElement tray{nullptr};
        for (auto child : root.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (element && winrt::get_class_name(element) ==
                               L"SystemTray.SystemTrayFrame") {
                tray = element;
                break;
            }
        }
        auto bounds = tray ? TryGetLayoutBounds(tray, root, tray.ActualWidth())
                           : std::nullopt;
        if (!bounds) {
            return std::nullopt;
        }
        snapshot.rightBoundary = bounds->left;
    }
    // An unavailable projection or unfinished realization is not an empty prefix.
    auto children = GetRealizedTaskbarElements(repeater,
        (g_settings.measureCenteredViewport || g_settings.reserveCenteredViewport)
            ? &snapshot.itemIdentities : nullptr);
    if (!children) {
        return std::nullopt;
    }
    // Use realized ItemsRepeater entries, excluding its visual-tree cache.
    for (auto child : *children) {
        auto identity = reinterpret_cast<uintptr_t>(winrt::get_abi(child));
        snapshot.childIdentities.push_back(identity);
        auto kind = IdentifySystemButton(child);
        if (kind == SystemButtonKind::None) {
            auto className = winrt::get_class_name(child);
            bool overflow = className == L"Taskbar.OverflowToggleButton" &&
                child.Name() == L"OverflowButton" &&
                Automation::AutomationProperties::GetAutomationId(child) == L"OverflowButton";
            if (className == L"Taskbar.TaskListButton" || overflow) {
                auto bounds = TryGetLayoutBounds(child, root, child.ActualWidth());
                if (bounds && IntersectsViewport(*bounds, {0, snapshot.rootWidth})) {
                    snapshot.flowElements.push_back({identity, *bounds, overflow});
                }
            } else {
                auto bounds = TryGetLayoutBounds(child, root, child.ActualWidth());
                snapshot.unknownVisible |= bounds &&
                    IntersectsViewport(*bounds, {0, snapshot.rootWidth});
            }
            continue;
        }
        SystemButtonLayout button{identity, kind,
                                 child.Visibility() == Visibility::Visible,
                                 false, {}, child.Margin().Left};
        button.marginRight = child.Margin().Right;
        if (button.visible) {
            double width = child.ActualWidth();
            button.outerWidth = width;
            if (kind == SystemButtonKind::Widgets) {
                auto panel = FindDirectChildByName(
                    child, L"ExperienceToggleButtonRootPanel");
                if (panel && winrt::get_class_name(panel) ==
                                 L"Taskbar.TaskListButtonPanel" &&
                    panel.IsLoaded() && panel.Visibility() == Visibility::Visible) {
                    auto x = panel.ActualOffset().x;
                    auto panelWidth = panel.ActualWidth();
                    if (std::isfinite(x) && x >= 0 &&
                        std::isfinite(panelWidth) && panelWidth > 0) {
                        // ActualWidth excludes the panel's trailing Margin.
                        // Keep its local left offset, without its Dock transform.
                        width = x + panelWidth;
                    }
                }
            } else if (
                Media::VisualTreeHelper::GetChildrenCount(child) > 0) {
                auto content = Media::VisualTreeHelper::GetChild(child, 0)
                                   .try_as<FrameworkElement>();
                if (content) {
                    width = content.DesiredSize().Width;
                }
            }
            button.contentWidth = width;
            auto bounds = TryGetLayoutBounds(child, root, width);
            if (!bounds || !std::isfinite(button.marginLeft)) {
                return std::nullopt;
            }
            button.bounds = *bounds;
            button.visible = IntersectsViewport(*bounds, {0, snapshot.rootWidth});
        }
        snapshot.buttons.push_back(button);
    }
    snapshot.offset = GetLeftHostOffset(snapshot, prefixOrigin,
                                        spacingTarget, spacingIncrement);
    for (const auto& button : snapshot.buttons) {
        if (button.visible && !button.fixed &&
            (snapshot.centered || button.kind != SystemButtonKind::Widgets)) {
            snapshot.flowElements.push_back({button.identity, button.bounds});
        }
    }
    if (!std::isfinite(snapshot.offset) ||
        !std::isfinite(snapshot.hostLeft) ||
        !std::isfinite(snapshot.hostWidth)) {
        return std::nullopt;
    }
    return snapshot;
}

bool LeftHostFlowCollision(const LeftHostLayoutSnapshot& layout) {
    if (layout.hostWidth <= 0) {
        return false;
    }
    double requiredEnd = layout.offset + layout.hostWidth + kTaskbarLayoutGap;
    return std::any_of(layout.flowElements.begin(), layout.flowElements.end(),
        [&](const auto& flow) {
            return flow.bounds.left + kLayoutTolerance < requiredEnd;
        });
}

void LogLeftHostLayout(const LeftHostLayoutSnapshot& snapshot, PCWSTR reason,
                       double reservedRightWidth = 0) {
    double flowLeft = snapshot.rootWidth;
    for (const auto& element : snapshot.flowElements) {
        flowLeft = std::min(flowLeft, element.bounds.left);
    }
    double hostRight = snapshot.offset + snapshot.hostWidth;
    bool overlap = LeftHostFlowCollision(snapshot);
    Wh_Log(L"[layout] left-host-sample build=" WH_MOD_VERSION L" reason=%s repeater=%p "
           L"realized=%zu systemButtons=%zu targetOffset=%.3f "
           L"actualLeft=%.3f width=%.3f targetRight=%.3f "
           L"flowCount=%zu flowLeft=%.3f overlap=%d centered=%d rightBoundary=%.3f nativeRightLimit=%.3f unknownVisible=%d viewportLeft=%.3f viewportWidth=%.3f repeaterDesiredWidth=%.3f",
           reason, reinterpret_cast<void*>(snapshot.repeaterIdentity),
           snapshot.childIdentities.size(), snapshot.buttons.size(),
           snapshot.offset, snapshot.hostLeft, snapshot.hostWidth, hostRight,
           snapshot.flowElements.size(),
           snapshot.flowElements.empty() ? -1 : flowLeft, overlap,
           snapshot.centered, snapshot.rightBoundary,
           snapshot.centered ? -1 :
               GetLeftAlignedRightBoundary(snapshot, reservedRightWidth),
           snapshot.unknownVisible, snapshot.contentLeft, snapshot.contentWidth,
           snapshot.repeaterDesiredWidth);
    for (const auto& button : snapshot.buttons) {
        Wh_Log(L"[layout] button=%p kind=%d visible=%d fixed=%d "
               L"layoutX=(%.3f,%.3f) outerWidth=%.3f contentWidth=%.3f "
               L"marginLeft=%.3f marginRight=%.3f",
               reinterpret_cast<void*>(button.identity),
               static_cast<int>(button.kind), button.visible,
               button.fixed, button.bounds.left, button.bounds.right,
               button.outerWidth, button.contentWidth, button.marginLeft,
               button.marginRight);
    }
    for (const auto& element : snapshot.flowElements) {
        Wh_Log(L"[layout] flow=%p layoutX=(%.3f,%.3f) overflow=%d",
               reinterpret_cast<void*>(element.identity),
               element.bounds.left, element.bounds.right, element.overflow);
    }
    for (const auto& item : snapshot.itemIdentities) {
        Wh_Log(L"[layout] item index=%d element=%p sourceItem=%p",
               item.index, reinterpret_cast<void*>(item.elementIdentity),
               reinterpret_cast<void*>(item.itemIdentity));
    }
}

bool SameLeftHostLayout(const LeftHostLayoutSnapshot& a,
                        const LeftHostLayoutSnapshot& b) {
    if (a.repeaterIdentity != b.repeaterIdentity ||
        a.centered != b.centered || a.unknownVisible != b.unknownVisible ||
        std::abs(a.rightBoundary - b.rightBoundary) > kLayoutTolerance ||
        a.childIdentities != b.childIdentities ||
        a.buttons.size() != b.buttons.size() ||
        a.flowElements.size() != b.flowElements.size() ||
        std::abs(a.contentLeft - b.contentLeft) > kLayoutTolerance ||
        std::abs(a.contentWidth - b.contentWidth) > kLayoutTolerance ||
        std::abs(a.rootWidth - b.rootWidth) > kLayoutTolerance ||
        std::abs(a.hostLeft - b.hostLeft) > kLayoutTolerance ||
        std::abs(a.hostWidth - b.hostWidth) > kLayoutTolerance ||
        std::abs(a.offset - b.offset) > kLayoutTolerance) {
        return false;
    }
    for (size_t i = 0; i < a.buttons.size(); i++) {
        const auto& x = a.buttons[i];
        const auto& y = b.buttons[i];
        if (x.identity != y.identity || x.kind != y.kind ||
            x.visible != y.visible || x.fixed != y.fixed ||
            std::abs(x.bounds.left - y.bounds.left) > kLayoutTolerance ||
            std::abs(x.bounds.right - y.bounds.right) > kLayoutTolerance ||
            std::abs(x.marginLeft - y.marginLeft) > kLayoutTolerance ||
            std::abs(x.outerWidth - y.outerWidth) > kLayoutTolerance ||
            std::abs(x.contentWidth - y.contentWidth) > kLayoutTolerance ||
            std::abs(x.marginRight - y.marginRight) > kLayoutTolerance) {
            return false;
        }
    }
    for (size_t i = 0; i < a.flowElements.size(); i++) {
        const auto& x = a.flowElements[i];
        const auto& y = b.flowElements[i];
        if (x.identity != y.identity || x.overflow != y.overflow ||
            std::abs(x.bounds.left - y.bounds.left) > kLayoutTolerance ||
            std::abs(x.bounds.right - y.bounds.right) > kLayoutTolerance) {
            return false;
        }
    }
    return true;
}

bool TaskbarClockShowsSeconds() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    return RegGetValue(
               HKEY_CURRENT_USER,
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
               L"ShowSecondsInSystemClock", RRF_RT_REG_DWORD, nullptr,
               &value, &size) == ERROR_SUCCESS &&
           value != 0;
}

winrt::hstring GetNativeClockText(bool timeText) {
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);

    WCHAR buffer[128];
    int length = 0;
    if (timeText) {
        DWORD flags = TaskbarClockShowsSeconds() ? 0 : TIME_NOSECONDS;
        length = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, flags, &localTime,
                                 nullptr, buffer, ARRAYSIZE(buffer));
    } else {
        length = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE,
                                 &localTime, nullptr, buffer,
                                 ARRAYSIZE(buffer), nullptr);
    }

    if (length > 1) {
        return winrt::hstring{buffer, static_cast<uint32_t>(length - 1)};
    }

    return timeText ? winrt::hstring{L"00:00"}
                    : winrt::hstring{L"2000/1/1"};
}

double MeasureNativeClockLine(const winrt::hstring& text) {
    Controls::TextBlock probe;
    probe.Text(text);
    probe.FontFamily(Media::FontFamily{L"Segoe UI Variable Text"});
    probe.FontSize(kNativeClockFontSize);
    probe.TextWrapping(TextWrapping::NoWrap);

    WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        probe.Language(localeName);
    }

    probe.Measure(winrt::Windows::Foundation::Size{kNativeClockMeasureLimit, kNativeClockMeasureLimit});
    return probe.DesiredSize().Width;
}

double CalculateNativeClockLayoutWidth(FrameworkElement content,
                                       FrameworkElement clock) {
    // Clock customization mods replace these strings and can make the button
    // hundreds of pixels wide. Measure fresh native strings instead of using
    // the customized element's DesiredSize/ActualWidth.
    double nativeTextWidth = std::max(
        MeasureNativeClockLine(GetNativeClockText(true)),
        MeasureNativeClockLine(GetNativeClockText(false)));

    // Everything outside the text StackPanel is the native button chrome
    // (padding and surrounding presenters). Its size is unaffected by text
    // formatting mods, so it can be recovered from the live visual tree.
    double chromeWidth = kNativeClockDefaultChromeWidth;
    auto dateText = FindDescendantByName(content, L"DateInnerTextBlock");
    auto timeText = FindDescendantByName(content, L"TimeInnerTextBlock");
    auto textElement = dateText ? dateText : timeText;
    if (textElement) {
        auto stackPanel = Media::VisualTreeHelper::GetParent(textElement)
                              .try_as<Controls::StackPanel>();
        if (stackPanel && clock.ActualWidth() > 0 &&
            stackPanel.ActualWidth() > 0) {
            double measuredChrome =
                clock.ActualWidth() - stackPanel.ActualWidth();
            if (measuredChrome >= kNativeClockMinChromeWidth && measuredChrome <= kNativeClockMaxChromeWidth) {
                chromeWidth = measuredChrome;
            }
        }
    }

    // Native taskbar clock widths are DPI-independent XAML units. Bounds keep
    // a malformed third-party style from poisoning the layout baseline.
    return std::clamp(nativeTextWidth + chromeWidth, kNativeClockMinLayoutWidth, kNativeClockMaxLayoutWidth);
}

MovedClockData* FindMovedClockData(FrameworkElement clock) {
    if (g_movedClocks) {
        for (auto& data : MovedClocks()) {
            if (data.clock.get() == clock) {
                return &data;
            }
        }
    }
    return nullptr;
}

FrameworkElement FindNotifyChevron(FrameworkElement group) {
    struct Node { FrameworkElement element; uint32_t depth; };
    std::vector<Node> nodes{{group, 0}};
    FrameworkElement chevron{nullptr};
    for (size_t i = 0; i < nodes.size(); i++) {
        auto node = nodes[i];
        if (winrt::get_class_name(node.element) == L"SystemTray.ChevronIconView") {
            if (chevron || Automation::AutomationProperties::GetAutomationId(node.element) != L"SystemTrayIcon") return nullptr;
            chevron = node.element;
            continue; // Its template cannot contain another source item.
        }
        int count = Media::VisualTreeHelper::GetChildrenCount(node.element);
        if (count > 32 || (node.depth >= 8 && count > 0)) return nullptr;
        for (int j = 0; j < count; j++) {
            auto child = Media::VisualTreeHelper::GetChild(node.element, j).try_as<FrameworkElement>();
            if (!child || nodes.size() == 64) return nullptr;
            nodes.push_back({child, node.depth + 1});
        }
    }
    return chevron;
}

winrt::Windows::Foundation::IInspectable ReadTrayGroupItemsSource(FrameworkElement group) {
    try {
        if (group && group.Name() == L"NotificationAreaIcons") {
            if (winrt::get_class_name(group) != L"SystemTray.NotificationAreaIcons" ||
                Automation::AutomationProperties::GetAutomationId(group) != L"NotificationAreaIcons") return nullptr;
            auto view = group.try_as<Controls::ItemsControl>();
            if (!view) return nullptr;
            auto context = group.ReadLocalValue(FrameworkElement::DataContextProperty());
            if (group.DataContext() || group.GetBindingExpression(FrameworkElement::DataContextProperty()) ||
                (context && context != DependencyProperty::UnsetValue())) return nullptr;
            auto source = view.ItemsSource();
            if (!source || view.GetBindingExpression(Controls::ItemsControl::ItemsSourceProperty()) ||
                view.ReadLocalValue(Controls::ItemsControl::ItemsSourceProperty()) != source ||
                winrt::get_class_name(source) !=
                    L"Windows.Foundation.Collections.IObservableVector`1<SystemTray.IconViewModel>" ||
                (view.Items().Size() && winrt::get_class_name(view.Items().GetAt(0)) != L"SystemTray.IconViewModel")) return nullptr;
            return source;
        }
        if (!group || (group.Name() != L"NonActivatableStack" && group.Name() != L"NotifyIconStack") ||
            winrt::get_class_name(group) != L"SystemTray.Stack" ||
            Media::VisualTreeHelper::GetChildrenCount(group) != 1) return nullptr;
        auto content = Media::VisualTreeHelper::GetChild(group, 0).try_as<Controls::Grid>();
        if (!content || content.Name() != L"Content" ||
            Media::VisualTreeHelper::GetChildrenCount(content) != 1) return nullptr;
        auto view = Media::VisualTreeHelper::GetChild(content, 0).try_as<Controls::ItemsControl>();
        if (!view || view.Name() != L"IconStack" ||
            winrt::get_class_name(view) != L"SystemTray.StackListView" ||
            Automation::AutomationProperties::GetAutomationId(view) !=
                (group.Name() == L"NotifyIconStack" ? L"NotifyIcon" : L"NonActivatable")) return nullptr;
        for (auto element : {group, content.as<FrameworkElement>(), view.as<FrameworkElement>()}) {
            auto context = element.ReadLocalValue(FrameworkElement::DataContextProperty());
            if (element.DataContext() || element.GetBindingExpression(FrameworkElement::DataContextProperty()) ||
                (context && context != DependencyProperty::UnsetValue())) return nullptr;
        }
        auto source = view.ItemsSource();
        if (!source || view.GetBindingExpression(Controls::ItemsControl::ItemsSourceProperty()) ||
            view.ReadLocalValue(Controls::ItemsControl::ItemsSourceProperty()) != source ||
            winrt::get_class_name(source) !=
                L"Windows.Foundation.Collections.IObservableVector`1<SystemTray.IconViewModel>") return nullptr;
        if (group.Name() == L"NotifyIconStack" &&
            (view.Items().Size() != 1 ||
             winrt::get_class_name(view.Items().GetAt(0)) != L"SystemTray.IconViewModel")) return nullptr;
        return source;
    } catch (...) {
        return nullptr; // Treat unavailable native source APIs as an unsupported group.
    }
}

std::optional<MovedTrayElementData> ReadTrayElementToMove(
    FrameworkElement clock, Controls::Grid root, bool trayGroup,
    PCWSTR groupName = L"NonActivatableStack") {
    auto parent = Media::VisualTreeHelper::GetParent(clock).try_as<Controls::Grid>();
    auto frame = parent ? Media::VisualTreeHelper::GetParent(parent)
                              .try_as<FrameworkElement>() : nullptr;
    if (!parent || parent.Name() != L"SystemTrayFrameGrid" || !frame ||
        winrt::get_class_name(frame) != L"SystemTray.SystemTrayFrame" ||
        Media::VisualTreeHelper::GetParent(frame) != root ||
        parent.XamlRoot() != root.XamlRoot() || parent.Children().Size() > 32) {
        return std::nullopt;
    }
    bool applicationTray = trayGroup && std::wstring_view(groupName) == L"NotificationAreaIcons";
    FrameworkElement target{nullptr};
    auto children = parent.Children();
    uint32_t index = 0, clockIndex = 0;
    for (auto child : children) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == (trayGroup ? groupName : L"ControlCenterButton") &&
            winrt::get_class_name(element) == (applicationTray ? L"SystemTray.NotificationAreaIcons" :
                trayGroup ? L"SystemTray.Stack" : L"SystemTray.OmniButton") &&
            Automation::AutomationProperties::GetAutomationId(element) ==
                (applicationTray ? L"NotificationAreaIcons" : trayGroup ? L"" : L"ControlCenterButton")) {
            if (target) return std::nullopt;
            target = element;
        }
    }
    if (!target || !target.IsLoaded() || target.XamlRoot() != root.XamlRoot() ||
        target.Visibility() != Visibility::Visible ||
        !children.IndexOf(target.as<UIElement>(), index) ||
        !children.IndexOf(clock.as<UIElement>(), clockIndex) || index >= clockIndex ||
        Controls::Grid::GetRow(target) != 0 || Controls::Grid::GetRowSpan(target) != 1 ||
        Controls::Grid::GetColumnSpan(target) != 1 ||
        Controls::Grid::GetRow(clock) != 0 || Controls::Grid::GetRowSpan(clock) != 1 ||
        Controls::Grid::GetColumnSpan(clock) != 1) {
        return std::nullopt;
    }
    auto property = Controls::Grid::ColumnProperty();
    auto local = target.ReadLocalValue(property);
    bool unset = local == DependencyProperty::UnsetValue();
    int columnIndex = Controls::Grid::GetColumn(target);
    auto columns = parent.ColumnDefinitions();
    if (target.GetBindingExpression(property) ||
        (!unset && !local.try_as<winrt::Windows::Foundation::IReference<int32_t>>()) ||
        columnIndex < 0 || static_cast<uint32_t>(columnIndex) >= columns.Size() ||
        columnIndex >= Controls::Grid::GetColumn(clock)) {
        return std::nullopt;
    }
    for (auto element : {target, parent.as<FrameworkElement>(), frame, root.as<FrameworkElement>()}) {
        auto context = element.ReadLocalValue(FrameworkElement::DataContextProperty());
        if (element.DataContext() || element.GetBindingExpression(FrameworkElement::DataContextProperty()) ||
            (context && context != DependencyProperty::UnsetValue())) {
            return std::nullopt;
        }
    }
    for (auto gridProperty : {Controls::Grid::RowProperty(), Controls::Grid::RowSpanProperty(),
                             Controls::Grid::ColumnSpanProperty()}) {
        auto value = target.ReadLocalValue(gridProperty);
        if (target.GetBindingExpression(gridProperty) ||
            (value != DependencyProperty::UnsetValue() &&
             !value.try_as<winrt::Windows::Foundation::IReference<int32_t>>())) return std::nullopt;
    }
    auto column = columns.GetAt(columnIndex);
    auto localWidth = column.ReadLocalValue(Controls::ColumnDefinition::WidthProperty());
    if (column.Width().GridUnitType != GridUnitType::Auto || column.MinWidth() != 0 ||
        !std::isfinite(column.ActualWidth()) || column.ActualWidth() <= 0 ||
        (localWidth != DependencyProperty::UnsetValue() &&
         !localWidth.try_as<winrt::Windows::Foundation::IReference<GridLength>>())) {
        return std::nullopt;
    }
    for (auto child : children) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) return std::nullopt;
        int first = Controls::Grid::GetColumn(element);
        int span = Controls::Grid::GetColumnSpan(element);
        if (child != target && first <= columnIndex && columnIndex - first < span) {
            return std::nullopt; // A shared or spanning column is not this control's contribution.
        }
    }
    auto bounds = TryGetLayoutBounds(target, root, target.ActualWidth());
    auto clockBounds = TryGetLayoutBounds(clock, root, clock.ActualWidth());
    if (!bounds || !clockBounds ||
        (trayGroup ? bounds->right > clockBounds->left + kLayoutTolerance :
                    std::abs(bounds->right - clockBounds->left) > kLayoutTolerance)) {
        return std::nullopt;
    }
    auto source = trayGroup ? ReadTrayGroupItemsSource(target) : nullptr;
    if (trayGroup && !source) return std::nullopt;
    if (applicationTray && !target.as<Controls::ItemsControl>().Items().Size()) return std::nullopt;
    auto chevron = trayGroup && target.Name() == L"NotifyIconStack" ? FindNotifyChevron(target) : nullptr;
    if (target.Name() == L"NotifyIconStack" &&
        (!chevron || !chevron.IsLoaded() || chevron.Visibility() != Visibility::Visible ||
         chevron.XamlRoot() != root.XamlRoot())) return std::nullopt;
    return MovedTrayElementData{
        .element = target, .originalParent = parent.as<Controls::Panel>(),
        .previousSibling = index ? winrt::make_weak(children.GetAt(index - 1)) : winrt::weak_ref<UIElement>{},
        .nextSibling = index + 1 < children.Size() ? winrt::make_weak(children.GetAt(index + 1)) : winrt::weak_ref<UIElement>{},
        .nativeColumn = column, .originalIndex = index, .originalColumn = columnIndex,
        .columnLocalUnset = unset, .nativeColumnWidth = column.ActualWidth(),
        .itemsSource = source, .nativeChevron = chevron,
    };
}

std::optional<MovedTrayElementData> ReadControlCenterToMove(
    FrameworkElement clock, Controls::Grid root) {
    return g_settings.moveControlCenter ? ReadTrayElementToMove(clock, root, false) : std::nullopt;
}

std::optional<MovedTrayElementData> ReadTrayGroupToMove(
    FrameworkElement clock, Controls::Grid root) {
    return g_settings.moveNonActivatableStack ? ReadTrayElementToMove(clock, root, true) : std::nullopt;
}

std::optional<MovedTrayElementData> ReadNotifyIconGroupToMove(
    FrameworkElement clock, Controls::Grid root) {
    if (!g_settings.moveNotifyIconStack || !g_notifyPopupHooked ||
        GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext()) != DPI_AWARENESS_PER_MONITOR_AWARE) return std::nullopt;
    return ReadTrayElementToMove(clock, root, true, L"NotifyIconStack");
}

std::optional<MovedTrayElementData> ReadApplicationTrayToMove(
    FrameworkElement clock, Controls::Grid root) {
    return g_settings.moveNotificationAreaIcons ?
        ReadTrayElementToMove(clock, root, true, L"NotificationAreaIcons") : std::nullopt;
}

std::optional<double> ReadMovedTrayContribution(
    const MovedClockData& data, const std::optional<MovedTrayElementData>& record, PCWSTR* reason) {
    if (reason) *reason = nullptr;
    if (!record || data.restoring) return 0;
    auto reject = [reason](PCWSTR value) -> std::optional<double> {
        if (reason) *reason = value;
        return std::nullopt;
    };
    const auto& saved = *record;
    auto target = saved.element.get();
    auto parent = saved.originalParent.get().try_as<Controls::Grid>();
    auto column = saved.nativeColumn;
    uint32_t index = 0;
    if (!target) return reject(saved.itemsSource ? L"tray-group-expired" : L"control-center-expired");
    if (saved.itemsSource && ReadTrayGroupItemsSource(target) != saved.itemsSource) {
        return reject(L"tray-group-source");
    }
    if (target.Name() == L"NotifyIconStack" && !saved.nativeChevron.get()) return reject(L"notify-chevron-expired");
    if (!parent) return reject(L"native-parent-unavailable");
    if (!column) return reject(L"native-column-unavailable");
    if (!data.leftHost) return reject(L"left-host-unavailable");
    if (!saved.columnChanged) return reject(L"column-write-record-missing");
    if (!data.leftHost.Children().IndexOf(target.as<UIElement>(), index)) {
        return reject(L"host-child-ownership");
    }
    if (target.GetBindingExpression(Controls::Grid::ColumnProperty()) ||
        Controls::Grid::GetColumn(target) != saved.hostColumn ||
        !target.ReadLocalValue(Controls::Grid::ColumnProperty())
             .try_as<winrt::Windows::Foundation::IReference<int32_t>>() ||
        Controls::Grid::GetRow(target) != 0 || Controls::Grid::GetRowSpan(target) != 1 ||
        Controls::Grid::GetColumnSpan(target) != 1) {
        return reject(L"grid-properties");
    }
    if (static_cast<uint32_t>(saved.originalColumn) >= parent.ColumnDefinitions().Size() ||
        parent.ColumnDefinitions().GetAt(saved.originalColumn) != column ||
        column.Width().GridUnitType != GridUnitType::Auto || column.MinWidth() != 0 ||
        !std::isfinite(column.ActualWidth()) || column.ActualWidth() < 0 ||
        column.ActualWidth() > saved.nativeColumnWidth + kLayoutTolerance) {
        return reject(L"native-column");
    }
    auto visualParent = Media::VisualTreeHelper::GetParent(target);
    auto targetRoot = target.XamlRoot();
    auto hostRoot = data.leftHost.XamlRoot();
    if (visualParent && visualParent != data.leftHost) {
        return reject(L"visual-parent");
    }
    if (targetRoot && hostRoot && targetRoot != hostRoot) {
        return reject(L"xaml-root");
    }
    // Children.Append can precede Loaded, visual parent/root and first measure.
    // Only initial attachment may wait; known ownership conflicts above never do.
    bool layoutPending = !targetRoot || !hostRoot || !target.IsLoaded() ||
                         !data.leftHost.IsLoaded() ||
                         data.leftHost.ActualWidth() <= kLayoutTolerance;
    if (saved.lastContribution < 0 && layoutPending) {
        if (reason) *reason = L"layout-pending";
        return 0;
    }
    if (!visualParent || !targetRoot || !hostRoot ||
        !target.IsLoaded() || !data.leftHost.IsLoaded()) {
        return reject(L"visual-attachment-lost");
    }
    return std::max(0.0, saved.nativeColumnWidth - column.ActualWidth());
}

std::optional<double> ReadControlCenterContribution(
    const MovedClockData& data, PCWSTR* reason = nullptr) {
    return ReadMovedTrayContribution(data, data.controlCenter, reason);
}

std::optional<double> ReadTrayGroupContribution(
    const MovedClockData& data, PCWSTR* reason = nullptr) {
    return ReadMovedTrayContribution(data, data.trayGroup, reason);
}

std::optional<double> ReadNotifyIconGroupContribution(
    const MovedClockData& data, PCWSTR* reason = nullptr) {
    return ReadMovedTrayContribution(data, data.notifyIcons, reason);
}

double GetReservedTrayLayoutWidth(const MovedClockData& data) {
    // Native-left layout reserves the host after the fixed prefix and uses
    // the actual remaining tray edge. Only centered layout needs compensation.
    if (!TaskbarUsesCenteredAlignment()) {
        return 0;
    }
    // Keep the original clock baseline separate from the measured extra shrink.
    return data.layoutReservedWidth + ReadControlCenterContribution(data).value_or(0) +
           ReadTrayGroupContribution(data).value_or(0) + ReadNotifyIconGroupContribution(data).value_or(0) +
           ReadMovedTrayContribution(data, data.applicationTray, nullptr).value_or(0);
}

PCWSTR TrayElementLogTag(TrayElementRecord record) {
    return record == &MovedClockData::applicationTray ? L"application-tray" :
           record == &MovedClockData::notifyIcons ? L"notify-icons" :
           record == &MovedClockData::trayGroup ? L"tray-group" : L"control-center";
}

bool TryMoveTrayElement(FrameworkElement clock, TrayElementRecord record) {
    bool trayGroup = record != &MovedClockData::controlCenter;
    auto data = FindMovedClockData(clock);
    if (!data || data->restoring) return false;
    if (!(data->*record)) return true;
    auto saved = *(data->*record);
    auto host = data->leftHost;
    auto target = saved.element.get();
    auto parent = saved.originalParent.get();
    uint32_t index = 0;
    if (!target || !parent || Media::VisualTreeHelper::GetParent(target) != parent ||
        !parent.Children().IndexOf(target.as<UIElement>(), index)) return false;
    data = nullptr;
    parent.Children().RemoveAt(index);
    data = FindMovedClockData(clock);
    if (!data || data->restoring || data->leftHost != host || !(data->*record) ||
        (data->*record)->element.get() != target) return false;
    (data->*record)->columnChanged = true;
    data = nullptr;
    Controls::Grid::SetColumn(target, saved.hostColumn);
    data = FindMovedClockData(clock);
    if (!data || data->restoring || data->leftHost != host || !(data->*record) ||
        (data->*record)->element.get() != target) return false;
    data = nullptr;
    // Remove clock, control center, system group, application tray, arrow in descending native order.
    // Each earlier group precedes the controls already moved into the host.
    if (trayGroup) host.Children().InsertAt(0, target.as<UIElement>());
    else host.Children().Append(target.as<UIElement>());
    data = FindMovedClockData(clock);
    if (!data || data->restoring || data->leftHost != host ||
        !(data->*record) ||
        (data->*record)->element.get() != target) return false;
    Wh_Log(L"[%s] native element moved before clock; originalColumnWidth=%.3f hostColumn=%d",
           TrayElementLogTag(record), saved.nativeColumnWidth, saved.hostColumn);
    return true;
}

bool TryMoveControlCenter(FrameworkElement clock) {
    return TryMoveTrayElement(clock, &MovedClockData::controlCenter);
}

uint32_t GetTrayRestoreIndex(Controls::Panel parent,
                             winrt::weak_ref<UIElement> previous,
                             winrt::weak_ref<UIElement> next, uint32_t original) {
    uint32_t index = 0;
    auto children = parent.Children();
    if (auto anchor = next.get(); anchor && children.IndexOf(anchor, index)) return index;
    if (auto anchor = previous.get(); anchor && children.IndexOf(anchor, index)) return index + 1;
    return std::min<uint32_t>(original, children.Size());
}

bool RestoreExtraTrayElements(MovedClockData& data, TrayElementRecord record = &MovedClockData::controlCenter) {
    if (!(data.*record)) return true;
    auto saved = *(data.*record);
    auto target = saved.element.get();
    if (!target) { (data.*record).reset(); return true; }
    auto parent = saved.originalParent.get();
    auto current = Media::VisualTreeHelper::GetParent(target).try_as<Controls::Panel>();
    uint32_t index = 0;
    bool owned = data.leftHost && data.leftHost.Children().IndexOf(target.as<UIElement>(), index);
    if (current && current != parent && !owned) {
        Wh_Log(L"[%s] restore conflict: third-party parent; leaving native object and properties",
               TrayElementLogTag(record));
        (data.*record).reset();
        return false;
    }
    if (!parent) {
        Wh_Log(L"[%s] restore conflict: original parent expired; retaining nonempty host",
               TrayElementLogTag(record));
        throw winrt::hresult_error(E_FAIL);
    }
    if (owned) data.leftHost.Children().RemoveAt(index);
    if (Media::VisualTreeHelper::GetParent(target) != parent) {
        index = GetTrayRestoreIndex(parent, saved.previousSibling, saved.nextSibling, saved.originalIndex);
        parent.Children().InsertAt(index, target.as<UIElement>());
    }
    bool columnOwned = !target.GetBindingExpression(Controls::Grid::ColumnProperty()) &&
        Controls::Grid::GetColumn(target) == saved.hostColumn &&
        static_cast<bool>(target.ReadLocalValue(Controls::Grid::ColumnProperty())
                              .try_as<winrt::Windows::Foundation::IReference<int32_t>>());
    if (saved.columnChanged && columnOwned) {
        if (saved.columnLocalUnset) target.ClearValue(Controls::Grid::ColumnProperty());
        else Controls::Grid::SetColumn(target, saved.originalColumn);
    } else if (saved.columnChanged) {
        Wh_Log(L"[%s] restore conflict: external Column; leaving property",
               TrayElementLogTag(record));
    }
    (data.*record).reset();
    Wh_Log(L"[%s] restored native parent/order columnOwned=%d localUnset=%d",
           TrayElementLogTag(record), columnOwned, saved.columnLocalUnset);
    return !saved.columnChanged || columnOwned;
}

void RegisterWidgetsTracking(MovedClockData& data, FrameworkElement clock);
void RevokeWidgetsTracking(MovedClockData& data);
bool RestoreClock(FrameworkElement clock);
DeferredClockData* FindDeferredClockData(FrameworkElement content);
bool RestoreFlowSpacing(const FlowSpacingData& spacing);
void RestoreWidgetsSpacing(const WidgetsSpacingData& spacing);
FrameworkElement FindFlowTarget(FrameworkElement repeater, uintptr_t identity);
std::optional<FlowSpacingData> ReadFlowSpacing(
    FrameworkElement target, bool rightSide = false);

bool HasCenteredOnlyFlow(const LeftHostLayoutSnapshot& layout) {
    return layout.centered && !layout.unknownVisible &&
        std::none_of(layout.buttons.begin(), layout.buttons.end(), [](const auto& button) {
            return button.kind == SystemButtonKind::Widgets ||
                   (button.visible && button.fixed);
        });
}

bool IsSupportedCenteredViewportBaseline(
    const LeftHostLayoutSnapshot& layout, const FlowSpacingData& spacing,
    PCWSTR* reason = nullptr) {
    auto reject = [&](PCWSTR value) {
        if (reason) *reason = value;
        return false;
    };
    if (reason) *reason = nullptr;
    const auto& margin = spacing.originalMargin;
    for (double value : {layout.rootWidth, layout.contentLeft, layout.contentWidth,
                         layout.hostLeft, layout.hostWidth, layout.offset,
                         layout.rightBoundary, margin.Left, margin.Top,
                         margin.Right, margin.Bottom}) {
        if (!std::isfinite(value)) return reject(L"nonfinite-baseline");
    }
    if (layout.rootWidth <= 0 || layout.contentWidth <= 0 || layout.hostWidth <= 0 ||
        layout.hostLeft < 0 || layout.offset < 0 || layout.rightBoundary <= 0 ||
        margin.Left < 0 || margin.Right < 0 || margin.Top != 0 || margin.Bottom != 0 ||
        spacing.localUnset || spacing.rightSide) {
        return reject(L"unsupported-baseline-values-or-source");
    }
    if (std::abs(layout.contentLeft - margin.Left) > kLayoutTolerance ||
        std::abs(layout.contentWidth - (layout.rootWidth - margin.Left - margin.Right)) > kLayoutTolerance ||
        std::abs(layout.hostLeft - layout.offset) > kLayoutTolerance) {
        return reject(L"baseline-geometry-mismatch");
    }
    return true;
}

std::optional<FlowSpacingData> ReadCenteredWidthSpacing(
    FrameworkElement repeater, Controls::Grid root, double rootWidth) {
    if (!IsTaskbarRepeater(repeater, root)) {
        Wh_Log(L"[centered-width] rejected: not the native taskbar repeater");
        return std::nullopt;
    }
    auto parent = Media::VisualTreeHelper::GetParent(repeater).try_as<Controls::Grid>();
    if (!parent) {
        Wh_Log(L"[centered-width] rejected: repeater parent is not a Grid");
        return std::nullopt;
    }
    auto padding = parent.Padding();
    auto border = parent.BorderThickness();
    LogLayoutElement(parent, root, 0, 0);
    Wh_Log(L"[centered-width] parent=%p repeater=%p children=%u rows=%u columns=%u "
           L"width=%.3f min=%.3f max=%.3f padding=(%.3f,%.3f,%.3f,%.3f) border=(%.3f,%.3f,%.3f,%.3f) "
           L"alignment=%d flow=%d sameXamlRoot=%d",
           winrt::get_abi(parent), winrt::get_abi(repeater), parent.Children().Size(),
           parent.RowDefinitions().Size(), parent.ColumnDefinitions().Size(),
           parent.Width(), parent.MinWidth(), parent.MaxWidth(),
           padding.Left, padding.Top, padding.Right, padding.Bottom,
           border.Left, border.Top, border.Right, border.Bottom,
           static_cast<int>(parent.HorizontalAlignment()),
           static_cast<int>(parent.FlowDirection()),
           parent.XamlRoot() == repeater.XamlRoot());
    if (winrt::get_class_name(parent) != L"Windows.UI.Xaml.Controls.Grid" ||
        parent.Children().Size() != 4 || parent.Children().GetAt(1) != repeater ||
        parent.XamlRoot() != repeater.XamlRoot() ||
        parent.HorizontalAlignment() != HorizontalAlignment::Stretch ||
        parent.FlowDirection() != FlowDirection::LeftToRight ||
        !std::isnan(parent.Width()) || parent.MinWidth() != 0 ||
        parent.MaxWidth() != INFINITY || padding != Thickness{} || border != Thickness{} ||
        parent.RowDefinitions().Size() || parent.ColumnDefinitions().Size()) {
        Wh_Log(L"[centered-width] rejected: native parent contract");
        return std::nullopt;
    }
    constexpr PCWSTR classes[] = {L"Taskbar.TaskbarBackground", L"Microsoft.UI.Xaml.Controls.ItemsRepeater",
                                 L"Taskbar.Gripper", L"Windows.UI.Xaml.Shapes.Rectangle"};
    constexpr PCWSTR names[] = {L"BackgroundControl", L"TaskbarFrameRepeater", L"GripperControl", L"ScreenEdgeStroke"};
    for (uint32_t i = 0; i < ARRAYSIZE(classes); i++) {
        auto child = parent.Children().GetAt(i).try_as<FrameworkElement>();
        if (!child || winrt::get_class_name(child) != classes[i] || child.Name() != names[i]) {
            Wh_Log(L"[centered-width] rejected: native sibling %u is not the recognized object", i);
            return std::nullopt;
        }
    }
    auto parentSpacing = ReadFlowSpacing(parent);
    auto bounds = TryGetLayoutBounds(parent, root, parent.ActualWidth());
    auto base = parent.GetAnimationBaseValue(FrameworkElement::MarginProperty())
                    .try_as<winrt::Windows::Foundation::IReference<Thickness>>();
    if (!parentSpacing || parentSpacing->originalMargin != Thickness{} || !base ||
        base.Value() != parentSpacing->originalMargin || !bounds ||
        !std::isfinite(rootWidth) || rootWidth <= 0 ||
        std::abs(bounds->left) > kLayoutTolerance ||
        std::abs(bounds->right - rootWidth) > kLayoutTolerance) {
        Wh_Log(L"[centered-width] rejected: parent geometry or Margin source "
               L"(spacing=%d margin=(%.3f,%.3f,%.3f,%.3f) base=%d bounds=(%.3f,%.3f) root=%.3f)",
               static_cast<bool>(parentSpacing),
               parentSpacing ? parentSpacing->originalMargin.Left : NAN,
               parentSpacing ? parentSpacing->originalMargin.Top : NAN,
               parentSpacing ? parentSpacing->originalMargin.Right : NAN,
               parentSpacing ? parentSpacing->originalMargin.Bottom : NAN,
               static_cast<bool>(base), bounds ? bounds->left : NAN,
               bounds ? bounds->right : NAN, rootWidth);
        return std::nullopt;
    }
    auto widthProperty = FrameworkElement::WidthProperty();
    auto alignmentProperty = FrameworkElement::HorizontalAlignmentProperty();
    auto widthLocal = repeater.ReadLocalValue(widthProperty);
    auto alignmentLocal = repeater.ReadLocalValue(alignmentProperty);
    bool widthUnset = widthLocal == DependencyProperty::UnsetValue();
    bool alignmentUnset = alignmentLocal == DependencyProperty::UnsetValue();
    double width = repeater.Width();
    auto alignment = repeater.HorizontalAlignment();
    auto widthBase = repeater.GetAnimationBaseValue(widthProperty)
                         .try_as<winrt::Windows::Foundation::IReference<double>>();
    auto alignmentBase = repeater.GetAnimationBaseValue(alignmentProperty)
                             .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
    Wh_Log(L"[centered-width] baseline target=%p width=%.3f min=%.3f max=%.3f alignment=%d unset=(%d,%d) bound=(%d,%d) "
           L"widthBase=%.3f alignmentBase=%d",
           winrt::get_abi(repeater), width, repeater.MinWidth(), repeater.MaxWidth(), static_cast<int>(alignment),
           widthUnset, alignmentUnset, static_cast<bool>(repeater.GetBindingExpression(widthProperty)),
           static_cast<bool>(repeater.GetBindingExpression(alignmentProperty)),
           widthBase ? widthBase.Value() : NAN,
           alignmentBase ? static_cast<int>(alignmentBase.Value()) : -1);
    if (!std::isnan(width) || repeater.MinWidth() != 0 || repeater.MaxWidth() != INFINITY ||
        alignment != HorizontalAlignment::Stretch ||
        repeater.GetBindingExpression(widthProperty) || repeater.GetBindingExpression(alignmentProperty) ||
        (!widthUnset && !widthLocal.try_as<winrt::Windows::Foundation::IReference<double>>()) ||
        (!alignmentUnset && !alignmentLocal.try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>()) ||
        !widthBase || !std::isnan(widthBase.Value()) || !alignmentBase || alignmentBase.Value() != alignment) {
        Wh_Log(L"[centered-width] rejected: repeater Width/Alignment source");
        return std::nullopt;
    }
    auto spacing = ReadFlowSpacing(repeater);
    if (!spacing) {
        Wh_Log(L"[centered-width] rejected: Repeater.Margin is bound, animated or unreadable");
        return std::nullopt;
    }
    spacing->widthConstraint = CenteredWidthConstraintData{parent, width, alignment, widthUnset, alignmentUnset};
    return spacing;
}

bool OwnsCenteredWidthConstraint(FrameworkElement target, const CenteredWidthConstraintData& constraint,
                                 bool beforeAlignment = false) {
    auto widthProperty = FrameworkElement::WidthProperty();
    auto alignmentProperty = FrameworkElement::HorizontalAlignmentProperty();
    auto widthBase = target.GetAnimationBaseValue(widthProperty)
                         .try_as<winrt::Windows::Foundation::IReference<double>>();
    auto alignmentBase = target.GetAnimationBaseValue(alignmentProperty)
                             .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
    auto alignmentLocal = target.ReadLocalValue(alignmentProperty);
    bool alignmentUnset = alignmentLocal == DependencyProperty::UnsetValue();
    auto expectedAlignment = beforeAlignment ? constraint.originalAlignment : HorizontalAlignment::Right;
    return constraint.widthWritten && (beforeAlignment ? !constraint.alignmentWritten : constraint.alignmentWritten) &&
        target.Width() == constraint.targetWidth && target.HorizontalAlignment() == expectedAlignment &&
        !target.GetBindingExpression(widthProperty) && !target.GetBindingExpression(alignmentProperty) &&
        target.ReadLocalValue(widthProperty).try_as<winrt::Windows::Foundation::IReference<double>>() &&
        alignmentUnset == (beforeAlignment && constraint.alignmentLocalUnset) &&
        (alignmentUnset || alignmentLocal.try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>()) &&
        widthBase && widthBase.Value() == constraint.targetWidth && alignmentBase &&
        alignmentBase.Value() == expectedAlignment;
}

bool RestoreCenteredWidthConstraint(FrameworkElement target, const CenteredWidthConstraintData& constraint) {
    auto widthProperty = FrameworkElement::WidthProperty();
    bool widthRestored = !constraint.widthWritten;
    if (constraint.widthWritten) {
        auto local = target.ReadLocalValue(widthProperty);
        bool unset = local == DependencyProperty::UnsetValue();
        auto base = target.GetAnimationBaseValue(widthProperty)
                        .try_as<winrt::Windows::Foundation::IReference<double>>();
        if (!target.GetBindingExpression(widthProperty) && base &&
            (unset || local.try_as<winrt::Windows::Foundation::IReference<double>>())) {
            if (!unset && target.Width() == constraint.targetWidth && base.Value() == constraint.targetWidth) {
                if (constraint.widthLocalUnset) target.ClearValue(widthProperty);
                else target.Width(constraint.originalWidth);
                widthRestored = true;
            } else if (unset == constraint.widthLocalUnset && std::isnan(target.Width()) && std::isnan(base.Value())) {
                widthRestored = true; // A nested cleanup already restored the original source.
            }
        }
    }
    auto alignmentProperty = FrameworkElement::HorizontalAlignmentProperty();
    bool alignmentRestored = !constraint.alignmentWritten;
    if (constraint.alignmentWritten) {
        auto local = target.ReadLocalValue(alignmentProperty);
        bool unset = local == DependencyProperty::UnsetValue();
        auto base = target.GetAnimationBaseValue(alignmentProperty)
                        .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
        if (!target.GetBindingExpression(alignmentProperty) && base &&
            (unset || local.try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>())) {
            if (!unset && target.HorizontalAlignment() == HorizontalAlignment::Right && base.Value() == HorizontalAlignment::Right) {
                if (constraint.alignmentLocalUnset) target.ClearValue(alignmentProperty);
                else target.HorizontalAlignment(constraint.originalAlignment);
                alignmentRestored = true;
            } else if (unset == constraint.alignmentLocalUnset && target.HorizontalAlignment() == constraint.originalAlignment &&
                       base.Value() == constraint.originalAlignment) {
                alignmentRestored = true;
            }
        }
    }
    // Sample the live source after restoration. The saved flags in the log
    // below describe the original source, not what the property reports now;
    // this line is the evidence that Width/Alignment actually returned.
    auto widthLocalFinal = target.ReadLocalValue(widthProperty);
    auto alignmentLocalFinal = target.ReadLocalValue(alignmentProperty);
    auto widthBaseFinal = target.GetAnimationBaseValue(widthProperty)
                              .try_as<winrt::Windows::Foundation::IReference<double>>();
    auto alignmentBaseFinal = target.GetAnimationBaseValue(alignmentProperty)
                                  .try_as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>();
    Wh_Log(L"[centered-width] restored-source target=%p width=%.3f widthUnset=%d widthLocal=%d widthBound=%d "
           L"widthBase=%.3f alignment=%d alignmentUnset=%d alignmentLocal=%d alignmentBound=%d alignmentBase=%d "
           L"marginLeft=%.3f",
           winrt::get_abi(target), target.Width(),
           widthLocalFinal == DependencyProperty::UnsetValue(),
           static_cast<bool>(widthLocalFinal.try_as<winrt::Windows::Foundation::IReference<double>>()),
           static_cast<bool>(target.GetBindingExpression(widthProperty)),
           widthBaseFinal ? widthBaseFinal.Value() : NAN,
           static_cast<int>(target.HorizontalAlignment()),
           alignmentLocalFinal == DependencyProperty::UnsetValue(),
           static_cast<bool>(alignmentLocalFinal.try_as<
               winrt::Windows::Foundation::IReference<HorizontalAlignment>>()),
           static_cast<bool>(target.GetBindingExpression(alignmentProperty)),
           alignmentBaseFinal ? static_cast<int>(alignmentBaseFinal.Value()) : -1,
           target.Margin().Left);
    Wh_Log(L"[centered-width] restoration target=%p widthAccepted=%d alignmentAccepted=%d unset=(%d,%d)",
           winrt::get_abi(target), widthRestored, alignmentRestored,
           constraint.widthLocalUnset, constraint.alignmentLocalUnset);
    return widthRestored && alignmentRestored;
}

bool SameTaskbarItemSource(const LeftHostLayoutSnapshot& a,
                           const LeftHostLayoutSnapshot& b) {
    if (a.itemIdentities.empty() || a.itemIdentities.size() != b.itemIdentities.size()) {
        return false;
    }
    for (size_t i = 0; i < a.itemIdentities.size(); i++) {
        const auto& x = a.itemIdentities[i];
        const auto& y = b.itemIdentities[i];
        if (x.index != static_cast<int>(i) || y.index != x.index ||
            !x.item || !y.item || !x.itemIdentity || x.itemIdentity != y.itemIdentity ||
            std::count_if(a.itemIdentities.begin(), a.itemIdentities.end(),
                [&](const auto& item) { return item.itemIdentity == x.itemIdentity; }) != 1) {
            return false;
        }
    }
    return true;
}

bool SameCenteredLogicalLayout(const LeftHostLayoutSnapshot& a,
                               const LeftHostLayoutSnapshot& b) {
    if (!HasCenteredOnlyFlow(a) || !HasCenteredOnlyFlow(b) || !SameTaskbarItemSource(a, b)) {
        return false;
    }
    // Pin the source objects while comparing: recycled UI elements are not app IDs.
    auto baseline = a;
    auto restored = b;
    auto normalize = [](LeftHostLayoutSnapshot& layout) {
        auto children = layout.childIdentities;
        for (auto& flow : layout.flowElements) {
            if (std::any_of(layout.buttons.begin(), layout.buttons.end(),
                [&](const auto& button) { return button.identity == flow.identity; })) {
                continue; // System-button identity and geometry remain strict.
            }
            auto item = std::find_if(layout.itemIdentities.begin(), layout.itemIdentities.end(),
                [&](const auto& entry) { return entry.elementIdentity == flow.identity; });
            if (item == layout.itemIdentities.end() ||
                std::count_if(layout.itemIdentities.begin(), layout.itemIdentities.end(),
                    [&](const auto& entry) {
                        return entry.elementIdentity == flow.identity ||
                               entry.itemIdentity == item->itemIdentity;
                    }) != 1) {
                return false;
            }
            for (size_t i = 0; i < children.size(); i++) {
                if (children[i] == flow.identity) {
                    layout.childIdentities[i] = item->itemIdentity;
                }
            }
            flow.identity = item->itemIdentity;
        }
        return true;
    };
    return normalize(baseline) && normalize(restored) &&
           SameLeftHostLayout(baseline, restored);
}

bool CenteredViewportApplied(const CenteredViewportProbeData& probe,
                             const LeftHostLayoutSnapshot& current) {
    if (std::abs(current.contentLeft - probe.baseline.contentLeft - probe.increment) > kLayoutTolerance ||
        std::abs(current.contentWidth - probe.baseline.contentWidth + probe.increment) > kLayoutTolerance ||
        std::abs(current.hostLeft - probe.baseline.hostLeft) > kLayoutTolerance ||
        current.flowElements.empty()) {
        return false;
    }
    double requiredEnd = current.hostLeft + current.hostWidth + kTaskbarLayoutGap;
    return std::all_of(current.flowElements.begin(), current.flowElements.end(),
        [&](const auto& flow) {
            return flow.bounds.left + kLayoutTolerance >= requiredEnd &&
                   flow.bounds.right + kTaskbarLayoutGap <= current.rightBoundary + kLayoutTolerance;
        });
}

bool LogCenteredViewportComparison(const CenteredViewportProbeData& probe,
                                   const LeftHostLayoutSnapshot& current) {
    const auto& baseline = probe.baseline;
    bool comparable = HasCenteredOnlyFlow(current) &&
        baseline.repeaterIdentity == current.repeaterIdentity &&
        std::abs(baseline.rootWidth - current.rootWidth) <= kLayoutTolerance &&
        std::abs(baseline.rightBoundary - current.rightBoundary) <= kLayoutTolerance &&
        std::abs(baseline.offset - current.offset) <= kLayoutTolerance &&
        std::abs(baseline.hostLeft - current.hostLeft) <= kLayoutTolerance &&
        std::abs(baseline.hostWidth - current.hostWidth) <= kLayoutTolerance;
    double leftDelta = current.contentLeft - baseline.contentLeft;
    double widthDelta = current.contentWidth - baseline.contentWidth;
    double requiredEnd = baseline.offset + baseline.hostWidth + kTaskbarLayoutGap;
    bool flowInside = !current.flowElements.empty();
    for (const auto& item : current.flowElements) {
        flowInside = flowInside && item.bounds.left + kLayoutTolerance >= requiredEnd &&
            item.bounds.right + kTaskbarLayoutGap <= current.rightBoundary + kLayoutTolerance;
    }
    bool viewportAccepted = comparable &&
        std::abs(leftDelta - probe.increment) <= kLayoutTolerance &&
        std::abs(widthDelta + probe.increment) <= kLayoutTolerance;
    bool returned = comparable && SameLeftHostLayout(baseline, current);
    bool logicalReturned = comparable && SameCenteredLogicalLayout(baseline, current);
    Wh_Log(L"[centered-viewport] phase=%s comparable=%d leftDelta=%.3f "
           L"widthDelta=%.3f increment=%.3f viewportAccepted=%d "
           L"visibleFlowInside=%d flowCount=%zu baselineCount=%zu baselineReturned=%d logicalBaselineReturned=%d; "
           L"overflow, hit testing and focus require manual verification",
           probe.restored ? L"restored" : L"applied", comparable, leftDelta,
           widthDelta, probe.increment, !probe.restored && viewportAccepted,
           flowInside, current.flowElements.size(), baseline.flowElements.size(),
           probe.restored && returned, probe.restored && logicalReturned);
    return returned;
}

bool StopCenteredViewportProbe(FrameworkElement clock) {
    auto data = FindMovedClockData(clock);
    if (!data || !data->centeredProbe) {
        return false;
    }
    auto root = data->taskbarRoot.get();
    auto spacing = data->flowSpacing;
    data = nullptr;
    // Keep the owning record reachable if a XAML read/write throws.
    bool restored = !spacing || RestoreFlowSpacing(*spacing);
    if (auto current = FindMovedClockData(clock); current && current->centeredProbe) {
        current->flowSpacing.reset();
        current->centeredProbe.reset();
    }
    if (root) {
        root.InvalidateMeasure();
    }
    Wh_Log(L"[centered-viewport] reservation restoration accepted=%d", restored);
    return restored;
}

bool CenteredViewportFallbackActive(FrameworkElement clock) {
    bool centered = TaskbarUsesCenteredAlignment();
    for (auto& data : DeferredClocks()) {
        if (data.centeredFallbackClock.get() != clock) {
            continue;
        }
        if (centered) {
            return true;
        }
        data.centeredFallbackClock = nullptr;
    }
    return false;
}

bool RestoreCenteredClockFallback(FrameworkElement clock) {
    if (auto data = FindMovedClockData(clock); !data || data->restoring) {
        return false;
    }
    // Block rediscovery before restoring, including when no viewport was owned.
    std::vector<FrameworkElement> contents;
    for (const auto& deferred : DeferredClocks()) {
        if (auto content = deferred.content.get()) {
            contents.push_back(content);
        }
    }
    for (const auto& content : contents) {
        if (FindAncestor(content, L"SystemTray.OmniButton",
                         L"NotificationCenterButton") == clock) {
            if (auto deferred = FindDeferredClockData(content)) {
                deferred->centeredFallbackClock = clock;
            }
        }
    }
    return RestoreClock(clock);
}

void CancelCenteredViewportProbe(FrameworkElement clock, const wchar_t* reason,
                                 const LeftHostLayoutSnapshot* layout) {
    auto data = FindMovedClockData(clock);
    if (!data || !data->centeredProbe) {
        return;
    }
    auto probe = *data->centeredProbe;
    double reserved = GetReservedTrayLayoutWidth(*data);
    bool holding = (probe.automatic || g_settings.holdCenteredViewportForInteraction) && !probe.restored;
    data = nullptr;
    if (layout) {
        LogLeftHostLayout(*layout, L"centered-viewport-cancelled", reserved);
    }
    Wh_Log(L"[centered-viewport] cancelled: reason=%s held=%d elapsedMs=%llu "
           L"clockWidth=(%.3f,%.3f) viewport=(%.3f,%.3f) expected=(%.3f,%.3f)",
           reason, holding,
           static_cast<unsigned long long>(GetTickCount64() - probe.startTick),
           layout ? layout->hostWidth : -1.0, probe.baseline.hostWidth,
           layout ? layout->contentLeft : -1.0, layout ? layout->contentWidth : -1.0,
           probe.baseline.contentLeft + probe.increment,
           probe.baseline.contentWidth - probe.increment);
    if (holding) {
        bool restored = RestoreCenteredClockFallback(clock);
        Wh_Log(L"[centered-viewport] native clock fallback accepted=%d", restored);
    } else {
        StopCenteredViewportProbe(clock);
    }
}

bool UpdateCenteredViewportProbe(FrameworkElement clock,
                                const LeftHostLayoutSnapshot& layout) {
    auto data = FindMovedClockData(clock);
    if (!data || data->restoring) {
        return false;
    }
    if (data->centeredProbe) {
        if (data->centeredProbe->failed) {
            return true; // Retain ownership for cleanup; do not retry writes.
        }
        auto probe = *data->centeredProbe;
        bool holding = (probe.automatic || g_settings.holdCenteredViewportForInteraction) && !probe.restored;
        // Held widths are validated against actual native item bounds below.
        bool widthFits = holding ? layout.hostWidth > 0 :
            std::abs(layout.hostWidth - probe.baseline.hostWidth) <= kLayoutTolerance;
        const wchar_t* reason = nullptr;
        if (!layout.centered) {
            reason = L"alignment-changed";
        } else if (!HasCenteredOnlyFlow(layout)) {
            reason = L"unsupported-flow";
        } else if (layout.repeaterIdentity != probe.baseline.repeaterIdentity) {
            reason = L"repeater-replaced";
        } else if (std::abs(layout.rootWidth - probe.baseline.rootWidth) > kLayoutTolerance) {
            reason = L"root-width-changed";
        } else if (!holding &&
                   std::abs(layout.rightBoundary - probe.baseline.rightBoundary) > kLayoutTolerance) {
            reason = L"finite-tray-boundary-changed";
        } else if (!widthFits) {
            reason = layout.hostWidth <= 0 ? L"clock-width-invalid" :
                L"finite-clock-width-changed";
        } else if (std::abs(layout.offset - probe.baseline.offset) > kLayoutTolerance) {
            reason = L"fixed-prefix-offset-changed";
        } else if (!holding && GetTickCount64() - probe.startTick >= kCenteredProbeTimeoutMs) {
            reason = L"finite-probe-timeout";
        }
        if (reason) {
            data = nullptr;
            CancelCenteredViewportProbe(clock, reason, &layout);
            return true;
        }
        double reserved = GetReservedTrayLayoutWidth(*data);
        auto target = data->repeater.get();
        auto root = data->taskbarRoot.get();
        auto spacing = data->flowSpacing;
        if (!probe.restored && spacing && spacing->widthConstraint) {
            data = nullptr;
            auto parent = target ? Media::VisualTreeHelper::GetParent(target).try_as<Controls::Grid>() : nullptr;
            if (holding || !IsTaskbarRepeater(target, root) || !parent ||
                parent != spacing->widthConstraint->parent.get() ||
                !OwnsCenteredWidthConstraint(target, *spacing->widthConstraint)) {
                CancelCenteredViewportProbe(clock, L"width-constraint-contract-changed", &layout);
                return true;
            }
        }
        if (holding) {
            data = nullptr;
            bool flowOutside = false;
            // Native app changes can keep the same safe, owned viewport.
            if (!SameTaskbarItemSource(layout, layout)) {
                reason = L"item-source-invalid";
            } else if (!target) {
                reason = L"repeater-expired";
            } else if (!spacing) {
                reason = L"reservation-missing";
            } else if (!ReadFlowSpacing(target)) {
                reason = L"margin-bound-or-expression";
            } else if (target.Margin().Left != spacing->lastValue) {
                reason = L"margin-external-change";
            } else if (!CenteredViewportApplied(probe, layout)) {
                // Margin ownership can be current while the first arranged
                // viewport is still the baseline. Never wait on other changes,
                // diagnostics, an accepted viewport, or a stale growth result.
                if (probe.automatic && !probe.appliedLogged && !probe.initialArrangeAccepted &&
                    probe.initialLayoutWaits < kAutomaticArrangeMaxCallbacks &&
                    GetTickCount64() - probe.startTick < kAutomaticArrangeTimeoutMs &&
                    std::abs(layout.contentLeft - probe.baseline.contentLeft) <= kLayoutTolerance &&
                    std::abs(layout.contentWidth - probe.baseline.contentWidth) <= kLayoutTolerance &&
                    std::abs(layout.hostLeft - probe.baseline.hostLeft) <= kLayoutTolerance &&
                    std::abs(layout.hostWidth - probe.baseline.hostWidth) <= kLayoutTolerance &&
                    SameTaskbarItemSource(probe.baseline, layout) && !layout.flowElements.empty() &&
                    std::all_of(layout.flowElements.begin(), layout.flowElements.end(),
                        [&](const auto& flow) {
                            return flow.bounds.right + kTaskbarLayoutGap <= layout.rightBoundary + kLayoutTolerance;
                        })) {
                    if (auto current = FindMovedClockData(clock);
                        current && !current->restoring && current->centeredProbe && current->flowSpacing) {
                        current->centeredProbe->initialLayoutWaits++;
                        if (!probe.initialLayoutWaits) {
                            Wh_Log(L"[centered-viewport] waiting for first automatic arrange: increment=%.3f; owned Margin, baseline viewport; limits=%u callbacks/%llums",
                                   probe.increment, kAutomaticArrangeMaxCallbacks,
                                   static_cast<unsigned long long>(kAutomaticArrangeTimeoutMs));
                        }
                    }
                    return true; // Existing root LayoutUpdated validates the next native result.
                }
                if (std::abs(layout.contentLeft - probe.baseline.contentLeft - probe.increment) > kLayoutTolerance) {
                    reason = L"viewport-origin-changed";
                } else if (std::abs(layout.contentWidth - probe.baseline.contentWidth + probe.increment) > kLayoutTolerance) {
                    reason = L"viewport-width-changed";
                } else if (std::abs(layout.hostLeft - probe.baseline.hostLeft) > kLayoutTolerance) {
                    reason = L"clock-position-changed";
                } else {
                    reason = L"flow-outside-boundaries";
                    flowOutside = !layout.flowElements.empty();
                }
            }
            if (reason) {
                double increment = layout.hostLeft + layout.hostWidth + kTaskbarLayoutGap -
                                   probe.baseline.contentLeft;
                if (probe.automatic && probe.appliedLogged && flowOutside && root &&
                    std::isfinite(increment) && increment > probe.increment + kLayoutTolerance &&
                    increment < probe.baseline.contentWidth &&
                    probe.baseline.contentLeft + increment < layout.rightBoundary &&
                    std::isfinite(spacing->originalMargin.Left + increment) &&
                    std::all_of(layout.flowElements.begin(), layout.flowElements.end(),
                        [&](const auto& flow) {
                            return flow.bounds.right + kTaskbarLayoutGap <= layout.rightBoundary + kLayoutTolerance;
                        })) {
                    auto margin = target.Margin();
                    margin.Left = spacing->originalMargin.Left + increment;
                    if (auto current = FindMovedClockData(clock);
                        current && !current->restoring && current->centeredProbe && current->flowSpacing) {
                        current->centeredProbe->increment = increment;
                        current->centeredProbe->appliedLogged = false;
                        current->flowSpacing->increment = increment;
                        current->flowSpacing->lastValue = margin.Left;
                    } else {
                        return true;
                    }
                    Wh_Log(L"[centered-viewport] automatic growth requested: increment=(%.3f,%.3f) clockWidth=%.3f; awaiting native layout",
                           probe.increment, increment, layout.hostWidth);
                    target.Margin(margin);
                    root.InvalidateMeasure();
                    return true;
                }
                CancelCenteredViewportProbe(clock, reason, &layout);
                return true;
            }
            if (probe.appliedLogged) {
                return true;
            }
        }
        data = nullptr;
        LogLeftHostLayout(layout, probe.restored ? L"centered-viewport-restored"
                                                : L"centered-viewport-applied", reserved);
        if (probe.automatic) {
            Wh_Log(L"[centered-viewport] automatic viewport accepted: origin=%.3f width=%.3f increment=%.3f clockWidth=%.3f visibleFlowInside=1",
                   layout.contentLeft, layout.contentWidth, probe.increment, layout.hostWidth);
        } else {
            LogCenteredViewportComparison(probe, layout);
        }
        LogLayoutElement(target, root, 0, 0);
        if (holding) {
            if (auto current = FindMovedClockData(clock); current && current->centeredProbe) {
                current->centeredProbe->appliedLogged = true;
                current->centeredProbe->initialArrangeAccepted = true;
            }
            if (!probe.automatic) {
                Wh_Log(L"[centered-viewport] interaction held; manually disable diagnostic after testing");
            }
            return true;
        }
        if (probe.restored) {
            if (auto current = FindMovedClockData(clock)) {
                current->centeredProbe.reset();
            }
        } else if (StopCenteredViewportProbe(clock)) {
            if (auto current = FindMovedClockData(clock);
                current && current->centeredProbeAttempted &&
                !current->centeredProbe && !current->flowSpacing) {
                probe.restored = true;
                current->centeredProbe = std::move(probe);
            }
        }
        return true;
    }
    bool automatic = g_settings.reserveCenteredViewport && !g_settings.measureCenteredViewport;
    if ((!automatic && !g_settings.measureCenteredViewport) || !layout.centered ||
        data->centeredProbeAttempted || layout.hostWidth <= 0 ||
        std::abs(layout.hostLeft - layout.offset) > kLayoutTolerance ||
        data->flowSpacing || data->widgetsSpacing) {
        return false;
    }
    bool fullRootViewport = std::abs(layout.contentLeft) <= kLayoutTolerance &&
        std::abs(layout.contentWidth - layout.rootWidth) <= kLayoutTolerance;
    if (!automatic && !fullRootViewport && !LeftHostFlowCollision(layout)) {
        return false; // Do not consume the finite attempt before a real collision.
    }
    auto appCount = std::count_if(layout.flowElements.begin(), layout.flowElements.end(),
        [&](const auto& item) {
            return !item.overflow && std::none_of(layout.buttons.begin(), layout.buttons.end(),
                [&](const auto& button) { return button.identity == item.identity; });
        });
    if (!HasCenteredOnlyFlow(layout) || appCount < 3 ||
        (automatic && !fullRootViewport)) {
        if (!automatic) {
            Wh_Log(L"[centered-viewport] skipped: fixed prefix, Widgets, unknown item or fewer than three apps");
        }
        return false;
    }
    // Nonzero baselines remain a finite diagnostic until live geometry and
    // restoration have been verified. The original full-root path is unchanged.
    bool externalBaseline = !fullRootViewport;
    if ((automatic || g_settings.holdCenteredViewportForInteraction || externalBaseline) &&
        !SameTaskbarItemSource(layout, layout)) {
        if (!automatic) {
            Wh_Log(L"[centered-viewport] skipped: complete source identities required");
        }
        return false;
    }
    // Consume the finite attempt only after the layout can actually qualify.
    // A collision with fewer than three apps, or a transient incomplete item
    // source while apps are still being realized, must not burn the one shot.
    if (!automatic) {
        data->centeredProbeAttempted = true;
    }
    auto target = data->repeater;
    auto root = data->taskbarRoot;
    double reserved = GetReservedTrayLayoutWidth(*data);
    data = nullptr;
    auto element = target.get();
    auto taskbarRoot = root.get();
    auto spacing = element ? ReadFlowSpacing(element) : std::nullopt;
    if (!spacing || !taskbarRoot) {
        if (!automatic) Wh_Log(L"[centered-viewport] skipped: unavailable, bound or unknown Margin");
        return false;
    }
    if (externalBaseline) {
        PCWSTR reason = nullptr;
        if (g_settings.holdCenteredViewportForInteraction ||
            reinterpret_cast<uintptr_t>(winrt::get_abi(element)) != layout.repeaterIdentity ||
            !IsTaskbarRepeater(element, taskbarRoot) ||
            element.HorizontalAlignment() != HorizontalAlignment::Stretch ||
            element.FlowDirection() != FlowDirection::LeftToRight ||
            !IsSupportedCenteredViewportBaseline(layout, *spacing, &reason)) {
            Wh_Log(L"[centered-viewport] nonzero baseline skipped: %s",
                   reason ? reason : L"finite-mode-or-parent-layout-contract");
            return false;
        }
        auto base = element.GetAnimationBaseValue(FrameworkElement::MarginProperty())
                        .try_as<winrt::Windows::Foundation::IReference<Thickness>>();
        if (!base || base.Value() != spacing->originalMargin) {
            Wh_Log(L"[centered-viewport] nonzero baseline skipped: animated or unknown base Margin");
            return false;
        }
        spacing = ReadCenteredWidthSpacing(element, taskbarRoot, layout.rootWidth);
        if (!spacing) {
            Wh_Log(L"[centered-viewport] nonzero baseline skipped: width/alignment source or native parent contract");
            return false;
        }
    }
    auto leading = std::min_element(layout.flowElements.begin(), layout.flowElements.end(),
        [](const auto& a, const auto& b) { return a.bounds.left < b.bounds.left; });
    double increment = layout.offset + layout.hostWidth + kTaskbarLayoutGap - layout.contentLeft;
    if (leading->bounds.left + kLayoutTolerance >= layout.offset + layout.hostWidth + kTaskbarLayoutGap ||
        !std::isfinite(increment) || increment <= 0 || increment >= layout.contentWidth ||
        !std::isfinite(layout.contentLeft + increment) ||
        layout.contentLeft + increment >= layout.rightBoundary ||
        !std::isfinite(spacing->originalMargin.Left + increment)) {
        if (!automatic) Wh_Log(L"[centered-viewport] skipped: no collision or no available width");
        return false;
    }
    LogLeftHostLayout(layout, L"centered-viewport-baseline", reserved);
    LogLayoutElement(element, taskbarRoot, 0, 0);
    // Property reads and diagnostics can reenter and replace the moved record.
    // Recheck the source/value, then reacquire by clock identity before saving.
    auto spacingElement = spacing->target.get();
    auto currentSpacing = externalBaseline ?
        ReadCenteredWidthSpacing(element, taskbarRoot, layout.rootWidth) :
        ReadFlowSpacing(element);
    if (!currentSpacing || currentSpacing->localUnset != spacing->localUnset ||
        currentSpacing->originalMargin != spacing->originalMargin ||
        currentSpacing->target.get() != spacingElement || !spacingElement) {
        return false;
    }
    if (spacing->widthConstraint &&
        (!currentSpacing->widthConstraint ||
         currentSpacing->widthConstraint->parent.get() != spacing->widthConstraint->parent.get() ||
         currentSpacing->widthConstraint->widthLocalUnset != spacing->widthConstraint->widthLocalUnset ||
         currentSpacing->widthConstraint->alignmentLocalUnset != spacing->widthConstraint->alignmentLocalUnset)) {
        return false;
    }
    // The Repeater remains Styler-owned even when its parent is writable.
    if (externalBaseline) {
        auto repeaterSpacing = ReadFlowSpacing(element);
        PCWSTR reason = nullptr;
        if (!repeaterSpacing ||
            !IsSupportedCenteredViewportBaseline(layout, *repeaterSpacing, &reason)) {
            return false;
        }
    }
    data = FindMovedClockData(clock);
    if (!data || data->restoring || data->centeredProbe || data->flowSpacing || data->widgetsSpacing ||
        data->repeater.get() != element || data->taskbarRoot.get() != taskbarRoot ||
        automatic != (g_settings.reserveCenteredViewport && !g_settings.measureCenteredViewport) ||
        (!automatic && !g_settings.measureCenteredViewport) ||
        (externalBaseline && g_settings.holdCenteredViewportForInteraction)) {
        return false;
    }
    spacing->increment = increment;
    spacing->lastValue = spacing->originalMargin.Left + (spacing->widthConstraint ? 0 : increment);
    if (spacing->widthConstraint) {
        spacing->widthConstraint->targetWidth = layout.contentWidth - increment;
        spacing->widthConstraint->operation = ++g_nextCenteredWidthOperation;
    }
    data->flowSpacing = *spacing;
    data->centeredProbe = CenteredViewportProbeData{layout, GetTickCount64(), increment};
    data->centeredProbe->automatic = automatic;
    data->centeredProbeAttempted = true;
    data = nullptr;
    if (spacing->widthConstraint) {
        Wh_Log(L"[centered-width] request target=%p targetWidth=%.3f alignment=Right increment=%.3f "
               L"expectedViewport=(%.3f,%.3f) unchangedMarginLeft=%.3f finite=1",
               winrt::get_abi(spacingElement), spacing->widthConstraint->targetWidth, increment,
               layout.contentLeft + increment, layout.contentWidth - increment, spacing->originalMargin.Left);
        data = FindMovedClockData(clock);
        if (!g_callbacksEnabled || !data || data->restoring || !data->flowSpacing || !data->flowSpacing->widthConstraint || !data->centeredProbe ||
            data->flowSpacing->widthConstraint->operation != spacing->widthConstraint->operation) return true;
        data->flowSpacing->widthConstraint->widthWritten = true;
        data = nullptr;
        spacingElement.Width(spacing->widthConstraint->targetWidth);
        data = FindMovedClockData(clock);
        if (!g_callbacksEnabled || !data || data->restoring || !data->flowSpacing || !data->flowSpacing->widthConstraint || !data->centeredProbe ||
            data->flowSpacing->widthConstraint->operation != spacing->widthConstraint->operation) return true;
        auto currentRepeater = data->repeater;
        auto checkpoint = *data->flowSpacing->widthConstraint;
        data = nullptr;
        if (currentRepeater.get() != spacingElement) return true;
        if (!OwnsCenteredWidthConstraint(spacingElement, checkpoint, true)) {
            CancelCenteredViewportProbe(clock, L"width-constraint-write-interrupted", nullptr);
            return true;
        }
        data = FindMovedClockData(clock);
        if (!g_callbacksEnabled || !data || data->restoring || !data->flowSpacing || !data->flowSpacing->widthConstraint || !data->centeredProbe ||
            data->flowSpacing->widthConstraint->operation != spacing->widthConstraint->operation) return true;
        data->flowSpacing->widthConstraint->alignmentWritten = true;
        data = nullptr;
        spacingElement.HorizontalAlignment(HorizontalAlignment::Right);
    } else {
        Wh_Log(L"[centered-viewport] target=%p originalLeft=%.3f localUnset=%d increment=%.3f "
           L"targetLeft=%.3f expectedViewport=(%.3f,%.3f) externalBaseline=%d finite=%d repeater=%p",
           winrt::get_abi(spacingElement), spacing->originalMargin.Left,
           spacing->localUnset, increment, spacing->lastValue,
           layout.contentLeft + increment, layout.contentWidth - increment,
           externalBaseline, !automatic,
           reinterpret_cast<void*>(layout.repeaterIdentity));
        auto margin = currentSpacing->originalMargin;
        margin.Left = spacing->lastValue;
        spacingElement.Margin(margin);
    }
    taskbarRoot.InvalidateMeasure();
    return true;
}

std::optional<FlowElementLayout> GetLeadingLeftAlignedFlow(
    const LeftHostLayoutSnapshot& layout) {
    if (layout.centered || layout.unknownVisible || layout.flowElements.empty() ||
        std::any_of(layout.flowElements.begin(), layout.flowElements.end(),
                    [](const auto& item) { return item.overflow; })) {
        return std::nullopt;
    }
    for (const auto& button : layout.buttons) {
        if (button.visible &&
            (button.kind == SystemButtonKind::Widgets ? button.fixed
                                                       : !button.fixed)) {
            return std::nullopt;
        }
    }
    return *std::min_element(layout.flowElements.begin(), layout.flowElements.end(),
        [](const auto& a, const auto& b) { return a.bounds.left < b.bounds.left; });
}

std::optional<SystemButtonLayout> GetLeftAlignedSpacingAnchor(
    const LeftHostLayoutSnapshot& layout) {
    if (!GetLeadingLeftAlignedFlow(layout)) {
        return std::nullopt;
    }
    std::optional<SystemButtonLayout> anchor;
    for (const auto& button : layout.buttons) {
        if (button.visible && button.fixed &&
            button.kind != SystemButtonKind::Widgets &&
            (!anchor || button.bounds.right > anchor->bounds.right)) {
            anchor = button;
        }
    }
    return anchor && std::isfinite(anchor->marginRight) && anchor->marginRight >= 0
        ? anchor : std::nullopt;
}

FrameworkElement FindFlowTarget(FrameworkElement repeater, uintptr_t identity) {
    auto children = GetRealizedTaskbarElements(repeater);
    if (children) {
        for (auto child : *children) {
            if (reinterpret_cast<uintptr_t>(winrt::get_abi(child)) == identity) {
                return child;
            }
        }
    }
    return nullptr;
}

std::optional<FlowSpacingData> ReadFlowSpacing(
    FrameworkElement target, bool rightSide) {
    auto property = FrameworkElement::MarginProperty();
    auto margin = target.Margin();
    auto local = target.ReadLocalValue(property);
    bool unset = local == DependencyProperty::UnsetValue();
    double value = rightSide ? margin.Right : margin.Left;
    if (target.GetBindingExpression(property) ||
        (!unset && !local.try_as<winrt::Windows::Foundation::IReference<Thickness>>()) ||
        !std::isfinite(value)) {
        return std::nullopt;
    }
    return FlowSpacingData{target, margin, unset, value, 0, rightSide};
}

std::optional<double> ComputeRequiredSpacing(double hostRight, double flowLeft,
                                            double currentIncrement) {
    if (!std::isfinite(hostRight) || !std::isfinite(flowLeft) ||
        !std::isfinite(currentIncrement) || currentIncrement < 0) {
        return std::nullopt;
    }
    double needed = hostRight + kTaskbarLayoutGap - (flowLeft - currentIncrement);
    return std::isfinite(needed) ? std::optional{std::max(0.0, needed)}
                                 : std::nullopt;
}

bool FlowSpacingFits(const LeftHostLayoutSnapshot& layout, double oldIncrement,
                      double newIncrement, double reservedRightWidth) {
    double right = layout.offset + layout.hostWidth;
    for (const auto& element : layout.flowElements) {
        right = std::max(right, element.bounds.right - oldIncrement + newIncrement);
    }
    return std::isfinite(right) &&
           right + kTaskbarLayoutGap <= GetLeftAlignedRightBoundary(layout, reservedRightWidth);
}

bool UpdateFlowSpacing(FrameworkElement clock,
                        const LeftHostLayoutSnapshot& layout) {
    auto data = FindMovedClockData(clock);
    auto anchor = GetLeftAlignedSpacingAnchor(layout);
    if (!data || data->restoring || !anchor) {
        Wh_Log(L"[layout] native-left unsupported prefix, left-pinned Widgets or flow");
        return false;
    }
    auto root = data->taskbarRoot.get();
    // Keep the reservation off reorderable application containers. Its owner
    // must remain the fixed prefix even when the first application changes.
    auto target = FindFlowTarget(data->repeater.get(), anchor->identity);
    if (data->flowSpacing && data->flowSpacing->target.get() != target) {
        auto spacing = *data->flowSpacing;
        data->flowSpacing.reset();
        data = nullptr;
        RestoreFlowSpacing(spacing);
        root.InvalidateMeasure();
        return true; // Establish the replacement baseline on the next layout.
    }
    if (!target) {
        return false;
    }
    auto initial = ReadFlowSpacing(target, true);
    auto margin = target.Margin();
    if (!initial || (data->flowSpacing &&
                    (!data->flowSpacing->rightSide ||
                     margin.Right != data->flowSpacing->lastValue))) {
        Wh_Log(L"[layout] native-left Margin bound, unknown or externally changed");
        return false;
    }
    if (layout.hostWidth <= 0) {
        return true;
    }
    if (!data->flowSpacing) {
        data->flowSpacing = *initial;
    }
    double oldIncrement = data->flowSpacing->increment;
    // Derive the baseline from the fixed prefix, not a moving drag rectangle.
    double baselineLeft = anchor->bounds.right + data->flowSpacing->originalMargin.Right;
    auto increment = ComputeRequiredSpacing(layout.offset + layout.hostWidth,
                                             baselineLeft, 0);
    if (!increment || !FlowSpacingFits(layout, oldIncrement, *increment,
                                       GetReservedTrayLayoutWidth(*data))) {
        Wh_Log(L"[layout] native-left insufficient room; overflow path unverified");
        return false;
    }
    double right = data->flowSpacing->originalMargin.Right + *increment;
    if (!std::isfinite(right)) {
        return false;
    }
    if (std::abs(right - margin.Right) <= kLayoutTolerance) {
        return true;
    }
    data->flowSpacing->lastValue = right;
    data->flowSpacing->increment = *increment;
    double layoutRightBoundary =
        GetLeftAlignedRightBoundary(layout, GetReservedTrayLayoutWidth(*data));
    data = nullptr;
    Wh_Log(L"[layout] native-left prefix reservation target=%p increment=%.3f rightMargin=%.3f "
           L"baselineLeft=%.3f requiredEnd=%.3f layoutRightBoundary=%.3f",
           reinterpret_cast<void*>(anchor->identity), *increment, right, baselineLeft,
           layout.offset + layout.hostWidth + kTaskbarLayoutGap,
           layoutRightBoundary);
    margin.Right = right;
    target.Margin(margin);
    return true;
}

bool RestoreSpacingForAlignment(FrameworkElement clock,
                                const LeftHostLayoutSnapshot& layout) {
    auto data = FindMovedClockData(clock);
    if (!data) {
        return false;
    }
    auto root = data->taskbarRoot.get();
    if (layout.centered && data->flowSpacing) {
        auto spacing = *data->flowSpacing;
        data->flowSpacing.reset();
        data = nullptr;
        RestoreFlowSpacing(spacing);
    } else if (!layout.centered && data->widgetsSpacing) {
        auto spacing = *data->widgetsSpacing;
        data->widgetsSpacing.reset();
        data = nullptr;
        RestoreWidgetsSpacing(spacing);
    } else {
        return false;
    }
    root.InvalidateMeasure();
    return true;
}

double CalculateWidgetsRightMargin(const SystemButtonLayout& widgets,
                                   const LeftHostLayoutSnapshot& layout,
                                   double originalRight) {
    return std::max(originalRight, layout.offset + layout.hostWidth + kTaskbarLayoutGap -
                                      widgets.bounds.left - widgets.outerWidth);
}

void RestoreWidgetsSpacing(const WidgetsSpacingData& spacing) {
    auto target = spacing.target.get();
    if (!target) {
        return;
    }
    auto property = FrameworkElement::MarginProperty();
    auto margin = target.Margin();
    if (target.GetBindingExpression(property) ||
        margin.Right != spacing.lastRight) {
        Wh_Log(L"Widgets spacing ownership changed; leaving external Margin");
        return;
    }
    margin.Right = spacing.originalMargin.Right;
    // Start-left can change Left while we own Right; preserve its new value.
    if (spacing.localUnset && margin.Left == spacing.originalMargin.Left &&
        margin.Top == spacing.originalMargin.Top &&
        margin.Bottom == spacing.originalMargin.Bottom) {
        target.ClearValue(property);
    } else {
        target.Margin(margin);
    }
    Wh_Log(L"[layout] Widgets spacing restored right=%.3f", margin.Right);
}

void UpdateWidgetsSpacing(FrameworkElement clock,
                          const LeftHostLayoutSnapshot& layout) {
    auto data = FindMovedClockData(clock);
    if (!data || data->restoring || data->widgetsSpacingBlocked) {
        return;
    }
    auto widgets = data->widgets.get();
    const SystemButtonLayout* button = nullptr;
    for (const auto& candidate : layout.buttons) {
        if (candidate.kind == SystemButtonKind::Widgets && candidate.visible &&
            candidate.fixed && widgets && candidate.identity ==
                reinterpret_cast<uintptr_t>(winrt::get_abi(widgets))) {
            button = &candidate;
            break;
        }
    }
    if (data->widgetsSpacing &&
        (!button || data->widgetsSpacing->target.get() != widgets)) {
        auto spacing = *data->widgetsSpacing;
        data->widgetsSpacing.reset();
        data = nullptr;
        RestoreWidgetsSpacing(spacing);
        return;  // Sample the replacement after the old reservation is removed.
    }
    if (!button || layout.hostWidth <= 0 ||
        !std::isfinite(button->outerWidth) || button->outerWidth <= 0) {
        return;
    }
    auto property = FrameworkElement::MarginProperty();
    auto margin = widgets.Margin();
    auto local = widgets.ReadLocalValue(property);
    bool localUnset = local == DependencyProperty::UnsetValue();
    if (widgets.GetBindingExpression(property) ||
        (!localUnset && !local.try_as<winrt::Windows::Foundation::IReference<Thickness>>()) ||
        !std::isfinite(margin.Right) ||
        (data->widgetsSpacing &&
         margin.Right != data->widgetsSpacing->lastRight)) {
        data->widgetsSpacingBlocked = true;
        data->widgetsSpacing.reset();
        Wh_Log(L"Widgets spacing unsupported or externally changed; no Margin takeover");
        return;
    }
    if (!data->widgetsSpacing) {
        data->widgetsSpacing = WidgetsSpacingData{
            widgets, margin, localUnset, margin.Right};
    }
    double right = CalculateWidgetsRightMargin(
        *button, layout, data->widgetsSpacing->originalMargin.Right);
    if (!std::isfinite(right) ||
        std::abs(right - margin.Right) <= kLayoutTolerance) {
        return;
    }
    data->widgetsSpacing->lastRight = right;
    data = nullptr;
    // Absolute reservation from the original value, never cumulative padding.
    Wh_Log(L"[layout] Widgets native reservation rightMargin=%.3f "
           L"clockWidth=%.3f requiredEnd=%.3f",
           right, layout.hostWidth, layout.offset + layout.hostWidth + kTaskbarLayoutGap);
    margin.Right = right;
    widgets.Margin(margin);
}

void RefreshTrackedElements(MovedClockData& data) {
    auto root = data.taskbarRoot.get();
    if (root && !IsTaskbarRepeater(data.repeater.get(), root)) {
        data.repeater = FindTaskbarRepeater(root);
        data.layoutSnapshot.reset();
    }
    auto repeater = data.repeater.get();
    if (root && IsTaskbarRepeater(repeater, root)) {
        auto children = GetRealizedTaskbarElements(repeater);
        if (!children) {
            return;
        }
        FrameworkElement widgets = nullptr;
        for (auto child : *children) {
            if (IdentifySystemButton(child) == SystemButtonKind::Widgets) {
                widgets = child;
                break;
            }
        }
        if (widgets != data.widgets.get()) {
            RevokeWidgetsTracking(data);
            data.widgets = widgets;
            if (auto clock = data.clock.get()) {
                RegisterWidgetsTracking(data, clock);
            }
        }
    }
}

void UpdateLeftHostOffsetForClock(
    winrt::weak_ref<FrameworkElement> weakClock) {
    if (!g_callbacksEnabled || !g_movedClocks ||
        g_clockThreadId != GetCurrentThreadId()) {
        return;
    }

    auto clock = weakClock.get();
    if (!clock) {
        return;
    }

    auto data = FindMovedClockData(clock);
    if (!data || data->updatingLayout || data->restoring ||
        !ShouldRelocateClock()) {
        return;
    }
    data->updatingLayout = true;
    try {
        // Keep the original processing order and the control-center fallback.
        for (auto record : {&MovedClockData::notifyIcons,
                            &MovedClockData::applicationTray,
                            &MovedClockData::trayGroup,
                            &MovedClockData::controlCenter}) {
            if (!(data->*record)) {
                continue;
            }
            PCWSTR tag = TrayElementLogTag(record);
            PCWSTR reason = nullptr;
            auto contribution = ReadMovedTrayContribution(*data, data->*record, &reason);
            if (!contribution) {
                if (record == &MovedClockData::controlCenter) {
                    data = nullptr;
                    Wh_Log(L"[%s] layout ownership changed reason=%s; restoring moved modules",
                           tag, reason);
                    RestoreClock(clock);
                    return;
                }
                Wh_Log(L"[%s] layout ownership changed reason=%s; restoring only this group",
                       tag, reason);
                RestoreExtraTrayElements(*data, record);
                data = FindMovedClockData(clock);
                if (!data || data->restoring) return;
            } else if (reason) {
                if ((data->*record)->lastContribution != -2) {
                    (data->*record)->lastContribution = -2;
                    Wh_Log(L"[%s] waiting for first host layout; extraReserved=0", tag);
                }
                if (TaskbarUsesCenteredAlignment()) {
                    data->updatingLayout = false;
                    return;
                }
            } else if (std::abs((data->*record)->lastContribution - *contribution) > kLayoutTolerance) {
                (data->*record)->lastContribution = *contribution;
                Wh_Log(L"[%s] measured native column shrink=%.3f clockBaseline=%.3f totalReserved=%.3f",
                       tag, *contribution, data->layoutReservedWidth, GetReservedTrayLayoutWidth(*data));
            }
        }
        RefreshTrackedElements(*data);
        auto root = data->taskbarRoot.get();
        auto repeater = data->repeater.get();
        auto host = data->leftHost;
        // An owned viewport is an app constraint, not a new fixed prefix.
        // Keep the clock at its baseline while managing that constraint.
        auto prefixOrigin = data->centeredProbe ?
            std::optional{data->centeredProbe->baseline.contentLeft} : std::nullopt;
        uintptr_t spacingTarget = 0;
        double spacingIncrement = 0;
        auto flowSpacing = data->flowSpacing;
        if (flowSpacing && flowSpacing->rightSide) {
            auto target = flowSpacing->target.get();
            auto current = target ? ReadFlowSpacing(target, true) : std::nullopt;
            if (current && current->lastValue == flowSpacing->lastValue) {
                spacingTarget = reinterpret_cast<uintptr_t>(winrt::get_abi(target));
                spacingIncrement = flowSpacing->increment;
            }
        }
        auto snapshot = ReadLeftHostLayout(root, repeater, host, prefixOrigin,
                                           spacingTarget, spacingIncrement);
        data = FindMovedClockData(clock);
        if (data && !data->restoring && data->leftHost == host && snapshot) {
            data->layoutSampleUnavailableLogged = false;
            bool changed = !data->layoutSnapshot ||
                !SameLeftHostLayout(*data->layoutSnapshot, *snapshot) ||
                std::abs(data->layoutSnapshot->repeaterDesiredWidth -
                         snapshot->repeaterDesiredWidth) > kLayoutTolerance ||
                (data->centeredProbe &&
                 !SameTaskbarItemSource(*data->layoutSnapshot, *snapshot));
            auto margin = host.Margin();
            bool needsMove =
                std::abs(margin.Left - snapshot->offset) > kLayoutTolerance;
            double reservedRightWidth = GetReservedTrayLayoutWidth(*data);
            data->layoutSnapshot = *snapshot;
            // Save all state before a write; XAML can reenter and erase records.
            data = nullptr;
            if (changed) {
                LogLeftHostLayout(*snapshot, L"host-update", reservedRightWidth);
            }
            if (UpdateCenteredViewportProbe(clock, *snapshot)) {
                // The owned viewport handles this layout until restoration.
            } else if (snapshot->centered && LeftHostFlowCollision(*snapshot)) {
                Wh_Log(L"[centered-viewport] collision without a usable reservation; "
                       L"viewport=(%.3f,%.3f) clockRight=%.3f; restoring native clock",
                       snapshot->contentLeft, snapshot->contentWidth,
                       snapshot->offset + snapshot->hostWidth);
                bool restored = RestoreCenteredClockFallback(clock);
                Wh_Log(L"[centered-viewport] unreserved collision fallback accepted=%d "
                       L"viewportLeftMargin=%.3f", restored,
                       repeater ? repeater.Margin().Left : -1.0);
            } else if (!snapshot->centered && !GetLeadingLeftAlignedFlow(*snapshot)) {
                Wh_Log(L"[layout] native-left layout no longer supported; restoring clock");
                RestoreClock(clock);
            } else if (RestoreSpacingForAlignment(clock, *snapshot)) {
                // Wait for the old reservation to leave the native layout.
            } else if (needsMove) {
                margin.Left = snapshot->offset;
                host.Margin(margin);
            } else {
                if (snapshot->centered) {
                    UpdateWidgetsSpacing(clock, *snapshot);
                } else if (!UpdateFlowSpacing(clock, *snapshot)) {
                    RestoreClock(clock);
                }
            }
        } else if (data && !data->restoring && data->centeredProbe) {
            data = nullptr;
            CancelCenteredViewportProbe(clock, L"layout-sample-unavailable", nullptr);
        } else if (data && !data->restoring && !TaskbarUsesCenteredAlignment()) {
            data = nullptr;
            Wh_Log(L"[layout] native-left sample unavailable; restoring clock");
            RestoreClock(clock);
        } else if (data && !data->restoring &&
                   !data->layoutSampleUnavailableLogged) {
            data->layoutSampleUnavailableLogged = true;
            Wh_Log(L"Left host sample unavailable; preserving its last offset");
        }
    } catch (...) {
        data = FindMovedClockData(clock);
        if (data && data->centeredProbe) {
            data->centeredProbe->failed = true;
            Wh_Log(L"[centered-viewport] failed: reason=layout-exception hr=%08X; retaining Margin record for cleanup",
                   winrt::to_hresult());
        }
        if (data && !data->layoutSampleUnavailableLogged) {
            data->layoutSampleUnavailableLogged = true;
            Wh_Log(L"Left host layout handling failed: %08X", winrt::to_hresult());
        }
        data = nullptr;
        if (!TaskbarUsesCenteredAlignment()) {
            try {
                RestoreClock(clock);
            } catch (...) {
                Wh_Log(L"[layout] native-left fallback restoration failed: %08X",
                       winrt::to_hresult());
            }
        }
    }
    if (auto current = FindMovedClockData(clock)) {
        current->updatingLayout = false;
    }
}

void RegisterWidgetsTracking(MovedClockData& data, FrameworkElement clock) {
    auto widgets = data.widgets.get();
    if (!widgets) {
        return;
    }

    auto weakClock = winrt::make_weak(clock);
    data.widgetsSizeChangedToken = widgets.SizeChanged(
        [weakClock](winrt::Windows::Foundation::IInspectable const&,
                    SizeChangedEventArgs const&) {
            try {
                UpdateLeftHostOffsetForClock(weakClock);
            } catch (...) {
                Wh_Log(L"Widgets size handling failed: %08X",
                       winrt::to_hresult());
            }
        });
    data.widgetsVisibilityChangedToken =
        widgets.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [weakClock](DependencyObject const&, DependencyProperty const&) {
                try {
                    UpdateLeftHostOffsetForClock(weakClock);
                } catch (...) {
                    Wh_Log(L"Widgets visibility handling failed: %08X",
                           winrt::to_hresult());
                }
            });
}

void RevokeWidgetsTracking(MovedClockData& data) {
    auto widgets = data.widgets.get();
    if (widgets) {
        if (data.widgetsSizeChangedToken) {
            widgets.SizeChanged(*data.widgetsSizeChangedToken);
        }
        if (data.widgetsVisibilityChangedToken) {
            widgets.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(),
                data.widgetsVisibilityChangedToken);
        }
    }

    data.widgetsSizeChangedToken.reset();
    data.widgetsVisibilityChangedToken = 0;
}

void RegisterLeftHostTracking(MovedClockData& data, FrameworkElement clock) {
    if (auto root = data.taskbarRoot.get()) {
        auto weakClock = winrt::make_weak(clock);
        data.rootLayoutUpdatedToken = root.LayoutUpdated(
            [weakClock](winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) {
                try {
                    UpdateLeftHostOffsetForClock(weakClock);
                } catch (...) {
                    Wh_Log(L"Root layout callback failed: %08X",
                           winrt::to_hresult());
                }
            });
    }
    RegisterWidgetsTracking(data, clock);
}

void RevokeLeftHostTracking(MovedClockData& data) {
    auto token = data.rootLayoutUpdatedToken;
    data.rootLayoutUpdatedToken.reset();
    if (token) {
        if (auto root = data.taskbarRoot.get()) {
            root.LayoutUpdated(*token);
        }
    }
    RevokeWidgetsTracking(data);
    data.layoutSnapshot.reset();
}

void RemoveLeftHost(MovedClockData& data) {
    RevokeLeftHostTracking(data);

    if (!data.leftHost) {
        return;
    }

    if (data.leftHost.Children().Size()) {
        Wh_Log(L"Left host cleanup conflict: host still owns native elements");
        return;
    }
    if (auto root = data.taskbarRoot.get()) {
        auto hostElement = data.leftHost.as<UIElement>();
        uint32_t index = 0;
        if (root.Children().IndexOf(hostElement, index)) {
            root.Children().RemoveAt(index);
        }
    }

    data.leftHost = nullptr;
}

void RestoreMovedClockData(MovedClockData& data) {
    data.restoring = true;
    RevokeLeftHostTracking(data);
    RestoreExtraTrayElements(data, &MovedClockData::notifyIcons);
    RestoreExtraTrayElements(data, &MovedClockData::applicationTray);
    RestoreExtraTrayElements(data, &MovedClockData::trayGroup);
    RestoreExtraTrayElements(data);  // Original native order: arrow, application tray, system group, control center, clock.
    auto movedClock = data.clock.get();
    if (movedClock) {
        auto originalParent = data.originalParent.get();
        auto element = movedClock.as<UIElement>();
        // A not-yet-attached host can own the clock even when GetParent is null.
        if (data.leftHost) {
            uint32_t index = 0;
            if (data.leftHost.Children().IndexOf(element, index)) {
                data.leftHost.Children().RemoveAt(index);
            }
        }
        auto currentParent = Media::VisualTreeHelper::GetParent(movedClock)
                                 .try_as<Controls::Panel>();

        if (currentParent &&
            (!originalParent ||
             currentParent != originalParent)) {
            uint32_t index = 0;
            if (currentParent.Children().IndexOf(element, index)) {
                currentParent.Children().RemoveAt(index);
            }
        }

        // Reattach before restoring properties or removing the host, so a
        // property failure can't strand the clock inside a discarded host.
        currentParent = Media::VisualTreeHelper::GetParent(movedClock)
                            .try_as<Controls::Panel>();
        if (originalParent && !currentParent) {
            auto children = originalParent.Children();
            uint32_t index =
                GetTrayRestoreIndex(originalParent, data.originalPreviousSibling,
                                    data.originalNextSibling, data.originalIndex);
            if (index < children.Size()) {
                children.InsertAt(index, element);
            } else {
                children.Append(element);
            }
        }

        Controls::Grid::SetColumn(movedClock, data.originalColumn);
        Controls::Grid::SetColumnSpan(movedClock, data.originalColumnSpan);
        Controls::Grid::SetRow(movedClock, data.originalRow);
        Controls::Grid::SetRowSpan(movedClock, data.originalRowSpan);
        movedClock.HorizontalAlignment(data.originalHorizontalAlignment);
        movedClock.VerticalAlignment(data.originalVerticalAlignment);
        movedClock.Margin(data.originalMargin);
    }

    RemoveLeftHost(data);
    auto spacing = data.widgetsSpacing;
    auto flowSpacing = data.flowSpacing;
    data.widgetsSpacing.reset();
    data.flowSpacing.reset();
    data.centeredProbe.reset();
    if (flowSpacing) {
        RestoreFlowSpacing(*flowSpacing);
    }
    if (spacing) {
        RestoreWidgetsSpacing(*spacing);
    }
    if (movedClock) {
        Wh_Log(L"Clock restored from the left host");
    }
}

void CleanupMovedClockData() {
    auto& movedClocks = MovedClocks();
    for (auto it = movedClocks.begin(); it != movedClocks.end();) {
        if (!it->clock.get() || !it->taskbarRoot.get()) {
            RestoreMovedClockData(*it);
            it = movedClocks.erase(it);
        } else {
            ++it;
        }
    }
}

bool RestoreClock(FrameworkElement clock) {
    auto& movedClocks = MovedClocks();
    for (auto it = movedClocks.begin(); it != movedClocks.end(); ++it) {
        auto movedClock = it->clock.get();
        if (!movedClock || movedClock != clock) {
            continue;
        }

        RestoreMovedClockData(*it);
        movedClocks.erase(it);
        return true;
    }

    return false;
}

bool MoveClock(FrameworkElement content) {
    CleanupMovedClockData();

    if (!g_taskbarViewHooked) {
        Wh_Log(L"Taskbar layout hook isn't ready; relocation skipped");
        return false;
    }

    auto clock = FindAncestor(content, L"SystemTray.OmniButton",
                              L"NotificationCenterButton");
    if (!clock) {
        Wh_Log(L"NotificationCenterButton not found");
        return false;
    }

    if (!ShouldRelocateClock()) {
        RestoreClock(clock);
        return false;
    }

    if (CenteredViewportFallbackActive(clock)) {
        return false;
    }

    for (const auto& data : MovedClocks()) {
        if (auto movedClock = data.clock.get(); movedClock == clock) {
            auto host = data.leftHost;
            UpdateLeftHostOffsetForClock(winrt::make_weak(clock));
            auto current = FindMovedClockData(clock);
            return current && !current->restoring && current->leftHost == host;
        }
    }

    auto originalParent = Media::VisualTreeHelper::GetParent(clock)
                              .try_as<Controls::Panel>();
    if (!originalParent || originalParent.Name() != L"SystemTrayFrameGrid") {
        Wh_Log(L"Unexpected clock parent");
        return false;
    }

    auto systemTrayFrame = Media::VisualTreeHelper::GetParent(originalParent)
                               .try_as<FrameworkElement>();
    if (!systemTrayFrame ||
        winrt::get_class_name(systemTrayFrame) !=
            L"SystemTray.SystemTrayFrame") {
        Wh_Log(L"SystemTrayFrame not found");
        return false;
    }

    auto root = Media::VisualTreeHelper::GetParent(systemTrayFrame)
                    .try_as<Controls::Grid>();
    if (!root ||
        root.as<winrt::Windows::Foundation::IInspectable>() ==
            originalParent.as<winrt::Windows::Foundation::IInspectable>()) {
        Wh_Log(L"Taskbar root Grid not found");
        return false;
    }

    LogLayoutSnapshot(clock, root, L"before-relocation");

    auto repeater = FindTaskbarRepeater(root);
    auto layout = ReadLeftHostLayout(root, repeater, nullptr);
    if (!layout) {
        Wh_Log(L"Unsupported or unfinished taskbar layout; relocation skipped");
        return false;
    }
    LogLeftHostLayout(*layout, L"relocation-plan");
    auto controlCenter = ReadControlCenterToMove(clock, root);
    auto trayGroup = ReadTrayGroupToMove(clock, root);
    auto notifyIcons = ReadNotifyIconGroupToMove(clock, root);
    auto applicationTray = ReadApplicationTrayToMove(clock, root);
    if (applicationTray && ((notifyIcons && notifyIcons->originalIndex >= applicationTray->originalIndex) ||
                            (trayGroup && applicationTray->originalIndex >= trayGroup->originalIndex) ||
                            (controlCenter && applicationTray->originalIndex >= controlCenter->originalIndex))) applicationTray.reset();
    if (g_settings.moveNotificationAreaIcons && !applicationTray) {
        Wh_Log(L"[application-tray] empty or unsupported native contract; keeping application tray in its current parent");
    }
    if (notifyIcons && ((trayGroup && notifyIcons->originalIndex >= trayGroup->originalIndex) ||
                        (controlCenter && notifyIcons->originalIndex >= controlCenter->originalIndex))) notifyIcons.reset();
    if (g_settings.moveNotifyIconStack && !notifyIcons) {
        Wh_Log(L"[notify-icons] unsupported native contract or popup hook/DPI context; keeping arrow in its current parent");
    }
    if (trayGroup && controlCenter && trayGroup->originalIndex >= controlCenter->originalIndex) {
        trayGroup.reset(); // Only the sampled native ordering is supported.
    }
    if (g_settings.moveNonActivatableStack && !trayGroup) {
        Wh_Log(L"[tray-group] unsupported native contract; keeping group in its current parent");
    }
    if (g_settings.moveControlCenter && !controlCenter) {
        Wh_Log(L"[control-center] unsupported native contract; keeping control in its current parent");
    }
    if (!layout->centered) {
        auto leading = GetLeadingLeftAlignedFlow(*layout);
        auto anchor = GetLeftAlignedSpacingAnchor(*layout);
        auto target = anchor ? FindFlowTarget(repeater, anchor->identity) : nullptr;
        auto spacing = target ? ReadFlowSpacing(target, true) : std::nullopt;
        double baselineLeft = anchor && spacing ?
            anchor->bounds.right + spacing->originalMargin.Right : 0;
        layout->hostWidth = clock.ActualWidth() +
            (controlCenter ? controlCenter->element.get().ActualWidth() : 0) +
            (trayGroup ? trayGroup->element.get().ActualWidth() : 0) +
            (notifyIcons ? notifyIcons->element.get().ActualWidth() : 0) +
            (applicationTray ? applicationTray->element.get().ActualWidth() : 0);
        auto increment = leading ? ComputeRequiredSpacing(
            layout->offset + layout->hostWidth, baselineLeft, 0)
                                 : std::nullopt;
        if (applicationTray && (!increment || !FlowSpacingFits(*layout, 0, *increment, 0))) {
            layout->hostWidth -= applicationTray->element.get().ActualWidth();
            applicationTray.reset();
            increment = leading ? ComputeRequiredSpacing(
                layout->offset + layout->hostWidth, baselineLeft, 0) : std::nullopt;
            Wh_Log(L"[application-tray] native-left space insufficient; keeping application tray in its current parent");
        }
        if (notifyIcons && (!increment || !FlowSpacingFits(*layout, 0, *increment, 0))) {
            layout->hostWidth -= notifyIcons->element.get().ActualWidth();
            notifyIcons.reset();
            increment = leading ? ComputeRequiredSpacing(
                layout->offset + layout->hostWidth, baselineLeft, 0) : std::nullopt;
            Wh_Log(L"[notify-icons] native-left space insufficient; keeping arrow in its current parent");
        }
        if (trayGroup && (!increment || !FlowSpacingFits(*layout, 0, *increment, 0))) {
            layout->hostWidth -= trayGroup->element.get().ActualWidth();
            trayGroup.reset();
            increment = leading ? ComputeRequiredSpacing(
                layout->offset + layout->hostWidth, baselineLeft, 0) : std::nullopt;
            Wh_Log(L"[tray-group] native-left space insufficient; keeping group in its current parent");
        }
        if (!spacing || layout->hostWidth <= 0 || !increment ||
            !FlowSpacingFits(*layout, 0, *increment, 0)) {
            double requiredRight = layout->offset + layout->hostWidth;
            for (const auto& flow : layout->flowElements) {
                requiredRight = std::max(requiredRight, flow.bounds.right + increment.value_or(0));
            }
            Wh_Log(L"[layout] native-left relocation rejected: anchor=%d margin=%d "
                   L"hostWidth=%.3f increment=%.3f requiredRight=%.3f availableRight=%.3f; keeping native clock",
                   anchor.has_value(), spacing.has_value(), layout->hostWidth,
                   increment.value_or(-1), requiredRight + kTaskbarLayoutGap,
                   GetLeftAlignedRightBoundary(*layout, 0));
            return false;
        }
    }

    auto element = clock.as<UIElement>();
    uint32_t originalIndex = 0;
    if (!originalParent.Children().IndexOf(element, originalIndex)) {
        return false;
    }

    double layoutReservedWidth =
        CalculateNativeClockLayoutWidth(content, clock);
    auto widgets = FindDirectChildByName(repeater, L"AugmentedEntryPointButton");

    if (applicationTray) applicationTray->hostColumn = notifyIcons ? 1 : 0;
    if (trayGroup) trayGroup->hostColumn = static_cast<int>(notifyIcons.has_value()) + static_cast<int>(applicationTray.has_value());
    if (controlCenter) controlCenter->hostColumn = static_cast<int>(trayGroup.has_value()) +
        static_cast<int>(notifyIcons.has_value()) + static_cast<int>(applicationTray.has_value());
    int clockHostColumn = static_cast<int>(controlCenter.has_value()) + static_cast<int>(trayGroup.has_value()) +
                          static_cast<int>(notifyIcons.has_value()) + static_cast<int>(applicationTray.has_value());
    MovedClockData data{
        .clock = clock,
        .originalParent = originalParent,
        .taskbarRoot = root,
        .widgets = widgets,
        .repeater = repeater,
        .controlCenter = controlCenter,
        .trayGroup = trayGroup,
        .notifyIcons = notifyIcons,
        .applicationTray = applicationTray,
        .originalPreviousSibling = (controlCenter || trayGroup || notifyIcons || applicationTray) && originalIndex ?
            winrt::make_weak(originalParent.Children().GetAt(originalIndex - 1)) : winrt::weak_ref<UIElement>{},
        .originalNextSibling = (controlCenter || trayGroup || notifyIcons || applicationTray) && originalIndex + 1 < originalParent.Children().Size() ?
            winrt::make_weak(originalParent.Children().GetAt(originalIndex + 1)) : winrt::weak_ref<UIElement>{},
        .layoutReservedWidth = layoutReservedWidth,
        .originalIndex = originalIndex,
        .originalColumn = Controls::Grid::GetColumn(clock),
        .originalColumnSpan = Controls::Grid::GetColumnSpan(clock),
        .originalRow = Controls::Grid::GetRow(clock),
        .originalRowSpan = Controls::Grid::GetRowSpan(clock),
        .originalHorizontalAlignment = clock.HorizontalAlignment(),
        .originalVerticalAlignment = clock.VerticalAlignment(),
        .originalMargin = clock.Margin(),
    };

    // The host is a sibling layer in the existing root cell. It doesn't add a
    // Grid column and therefore doesn't change any TaskbarFrame coordinates.
    data.leftHost = Controls::Grid();
    data.leftHost.Name(L"TaskbarClockToLeftHost");
    data.leftHost.HorizontalAlignment(HorizontalAlignment::Left);
    data.leftHost.VerticalAlignment(VerticalAlignment::Stretch);
    data.leftHost.Margin(Thickness{layout->offset, 0, 0, 0});
    Controls::Grid::SetColumn(data.leftHost, 0);
    Controls::Grid::SetColumnSpan(
        data.leftHost,
        std::max<int>(1, root.ColumnDefinitions().Size()));
    Controls::Grid::SetRow(data.leftHost, 0);
    Controls::Grid::SetRowSpan(
        data.leftHost, std::max<int>(1, root.RowDefinitions().Size()));

    if (clockHostColumn) {
        for (int i = 0; i <= clockHostColumn; i++) {
            Controls::ColumnDefinition column;
            column.Width(GridLength{1, GridUnitType::Auto});
            data.leftHost.ColumnDefinitions().Append(column);
        }
    }
    auto host = data.leftHost;
    MovedClocks().push_back(std::move(data));

    try {
        originalParent.Children().RemoveAt(originalIndex);
        Controls::Grid::SetColumn(clock, clockHostColumn);
        Controls::Grid::SetColumnSpan(clock, 1);
        Controls::Grid::SetRow(clock, 0);
        Controls::Grid::SetRowSpan(clock, 1);
        clock.HorizontalAlignment(HorizontalAlignment::Stretch);
        clock.VerticalAlignment(VerticalAlignment::Stretch);
        clock.Margin(Thickness{0, 0, 0, 0});

        if (auto current = FindMovedClockData(clock);
            !current || current->restoring || current->leftHost != host) {
            return false;
        }
        if (!TryMoveControlCenter(clock)) {
            throw winrt::hresult_error(E_FAIL);
        }
        bool groupMoved = false;
        try {
            groupMoved = TryMoveTrayElement(clock, &MovedClockData::trayGroup);
        } catch (...) {
            Wh_Log(L"[tray-group] migration failed: %08X", winrt::to_hresult());
        }
        if (!groupMoved) {
            auto current = FindMovedClockData(clock);
            if (!current || current->restoring || current->leftHost != host) return false;
            RestoreExtraTrayElements(*current, &MovedClockData::trayGroup);
            Wh_Log(L"[tray-group] migration failed; restored group independently");
        }
        if (auto current = FindMovedClockData(clock);
            !current || current->restoring || current->leftHost != host) return false;
        bool applicationMoved = false;
        try {
            applicationMoved = TryMoveTrayElement(clock, &MovedClockData::applicationTray);
        } catch (...) {
            Wh_Log(L"[application-tray] migration failed: %08X", winrt::to_hresult());
        }
        if (!applicationMoved) {
            auto current = FindMovedClockData(clock);
            if (!current || current->restoring || current->leftHost != host) return false;
            RestoreExtraTrayElements(*current, &MovedClockData::applicationTray);
            Wh_Log(L"[application-tray] migration failed; restored application tray independently");
        }
        if (auto current = FindMovedClockData(clock);
            !current || current->restoring || current->leftHost != host) return false;
        bool notifyMoved = false;
        try {
            notifyMoved = TryMoveTrayElement(clock, &MovedClockData::notifyIcons);
        } catch (...) {
            Wh_Log(L"[notify-icons] migration failed: %08X", winrt::to_hresult());
        }
        if (!notifyMoved) {
            auto current = FindMovedClockData(clock);
            if (!current || current->restoring || current->leftHost != host) return false;
            RestoreExtraTrayElements(*current, &MovedClockData::notifyIcons);
            Wh_Log(L"[notify-icons] migration failed; restored arrow independently");
        }
        if (auto current = FindMovedClockData(clock);
            !current || current->restoring || current->leftHost != host) return false;
        host.Children().Append(element);
        root.Children().Append(host.as<UIElement>());
        if (auto current = FindMovedClockData(clock);
            current && !current->restoring && current->leftHost == host) {
            RegisterLeftHostTracking(*current, clock);
        } else {
            return false;
        }
        UpdateLeftHostOffsetForClock(winrt::make_weak(clock));
        if (auto current = FindMovedClockData(clock);
            !current || current->restoring || current->leftHost != host) {
            return false;
        }

        // If a clock customization mod loaded first, TaskbarFrame has already
        // arranged app buttons using the expanded clock. Queue a fresh measure
        // with our native-width baseline. Don't call UpdateLayout here: this
        // path can run from LayoutUpdated, where synchronous layout re-entry is
        // unsafe.
        originalParent.InvalidateMeasure();
        systemTrayFrame.InvalidateMeasure();
        root.InvalidateMeasure();
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        RestoreClock(clock);
        Wh_Log(L"Clock relocation failed: %08X", hr);
        return false;
    }

    if (auto current = FindMovedClockData(clock);
        !current || current->restoring || current->leftHost != host) {
        return false;
    }
    Wh_Log(L"Clock moved to the left host; visual=%.1f, layout=%.1f",
           clock.ActualWidth(), layoutReservedWidth);
    return true;
}

double GetLayoutReservedClockWidthForTaskbarFrame(void* pThis) {
    if (!g_callbacksEnabled || !pThis) {
        return 0;
    }

    DWORD clockThreadId = g_clockThreadId;
    if (!clockThreadId || clockThreadId != GetCurrentThreadId()) {
        if (clockThreadId && !g_extentThreadMismatchLogged.exchange(true)) {
            Wh_Log(L"Taskbar extent skipped on unexpected thread: %u != %u",
                   GetCurrentThreadId(), clockThreadId);
        }
        return 0;
    }
    g_extentThreadMismatchLogged = false;

    try {
        FrameworkElement taskbarFrame = nullptr;
        IUnknown* taskbarFrameUnknown = ((IUnknown**)pThis)[1];
        if (!taskbarFrameUnknown ||
            FAILED(taskbarFrameUnknown->QueryInterface(
                winrt::guid_of<FrameworkElement>(),
                winrt::put_abi(taskbarFrame)))) {
            return 0;
        }

        auto taskbarXamlRoot = taskbarFrame.XamlRoot();
        if (!taskbarXamlRoot) {
            return 0;
        }

        if (!g_movedClocks) {
            return 0;
        }

        for (const auto& data : MovedClocks()) {
            auto clock = data.clock.get();
            auto host = data.leftHost;
            if (!clock || !host || host.XamlRoot() != taskbarXamlRoot) {
                continue;
            }

            return GetReservedTrayLayoutWidth(data);
        }
    } catch (...) {
        Wh_Log(L"Taskbar extent lookup failed: %08X", winrt::to_hresult());
    }

    return 0;
}

void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void* pThis, double value) {
    double layoutReservedWidth =
        GetLayoutReservedClockWidthForTaskbarFrame(pThis);
    TaskbarFrame_SystemTrayExtent_Original(pThis,
                                           value + layoutReservedWidth);
}

HRESULT WINAPI TaskbarFrame_get_Alignment_Hook(void* pThis, int* alignment) {
    HRESULT result = TaskbarFrame_get_Alignment_Original(pThis, alignment);

    try {
        // The registry watch is authoritative while active. Keep this hook
        // as a fallback without letting a stale getter overwrite its cache.
        if (!g_alignmentRegistryWatchActive && g_callbacksEnabled &&
            SUCCEEDED(result) && alignment) {
            int centeredAlignment = *alignment != 0 ? 1 : 0;
            int previousAlignment =
                g_lastTaskbarCenteredAlignment.exchange(centeredAlignment);
            if (g_callbacksEnabled && previousAlignment >= 0 &&
                previousAlignment != centeredAlignment) {
                QueueKnownClockPlacementUpdate();
            }
        }
    } catch (...) {
        Wh_Log(L"Taskbar alignment handling failed: %08X",
               winrt::to_hresult());
    }

    return result;
}

bool RestoreFlowSpacing(const FlowSpacingData& spacing) {
    auto target = spacing.target.get();
    if (!target) {
        return false;
    }
    if (spacing.widthConstraint) {
        return RestoreCenteredWidthConstraint(target, *spacing.widthConstraint);
    }
    auto margin = target.Margin();
    double& value = spacing.rightSide ? margin.Right : margin.Left;
    if (!ReadFlowSpacing(target, spacing.rightSide) || value != spacing.lastValue) {
        Wh_Log(L"[layout] flow spacing restore relinquished: binding, expression or external %s",
               spacing.rightSide ? L"Right" : L"Left");
        return false;
    }
    value = spacing.rightSide ? spacing.originalMargin.Right : spacing.originalMargin.Left;
    if (spacing.localUnset && margin.Left == spacing.originalMargin.Left &&
        margin.Top == spacing.originalMargin.Top &&
        margin.Right == spacing.originalMargin.Right &&
        margin.Bottom == spacing.originalMargin.Bottom) {
        target.ClearValue(FrameworkElement::MarginProperty());
    } else {
        target.Margin(margin);
    }
    Wh_Log(L"[layout] flow spacing %s restored to %.3f",
           spacing.rightSide ? L"Right" : L"Left", value);
    return true;
}

DeferredClockData& GetDeferredData(FrameworkElement content) {
    auto& deferredClocks = DeferredClocks();
    for (auto it = deferredClocks.begin(); it != deferredClocks.end();) {
        auto element = it->content.get();
        if (!element && !it->centeredFallbackClock.get()) {
            it = deferredClocks.erase(it);
            continue;
        }
        if (element == content) {
            return *it;
        }
        ++it;
    }

    deferredClocks.push_back({.content = content});
    return deferredClocks.back();
}

DeferredClockData* FindDeferredClockData(FrameworkElement content) {
    if (g_deferredClocks) {
        for (auto& data : DeferredClocks()) {
            if (data.content.get() == content) {
                return &data;
            }
        }
    }
    return nullptr;
}

void RememberClockContent(FrameworkElement content) {
    GetDeferredData(content);
}

void RemoveLoadedHandler(FrameworkElement content) {
    std::optional<winrt::event_token> token;
    for (auto& data : DeferredClocks()) {
        if (data.content.get() == content) {
            data.loadedRegistration = 0;
            token = data.loadedToken;
            data.loadedToken.reset();
            break;
        }
    }

    if (token) {
        content.Loaded(*token);
    }
}

void RemoveLayoutUpdatedHandler(FrameworkElement content) {
    std::optional<winrt::event_token> token;
    for (auto& data : DeferredClocks()) {
        if (data.content.get() == content) {
            data.layoutUpdatedRegistration = 0;
            token = data.layoutUpdatedToken;
            data.layoutUpdatedToken.reset();
            data.layoutUpdatedStartTick = 0;
            data.layoutUpdatedAttempts = 0;
            break;
        }
    }

    if (token) {
        content.LayoutUpdated(*token);
    }
}

void RemoveDeferredHandlers(FrameworkElement content) {
    RemoveLoadedHandler(content);
    RemoveLayoutUpdatedHandler(content);
}

bool ClockIsAlreadyMoved(FrameworkElement clock) {
    for (const auto& data : MovedClocks()) {
        if (data.clock.get() == clock) {
            return true;
        }
    }

    return false;
}

bool ClockLayoutIsReady(FrameworkElement content) {
    if (!content.IsLoaded()) {
        return false;
    }

    auto clock = FindAncestor(content, L"SystemTray.OmniButton",
                              L"NotificationCenterButton");
    if (!clock || ClockIsAlreadyMoved(clock)) {
        return true;
    }
    if (!TaskbarUsesCenteredAlignment() &&
        g_settings.keepOnLeftWhenLeftAligned &&
        (!clock.IsLoaded() || clock.ActualWidth() <= 0 ||
         !std::isfinite(clock.ActualWidth()))) {
        return false;
    }

    auto originalParent = Media::VisualTreeHelper::GetParent(clock)
                              .try_as<Controls::Panel>();
    if (!originalParent || originalParent.Name() != L"SystemTrayFrameGrid") {
        return false;
    }

    auto systemTrayFrame = Media::VisualTreeHelper::GetParent(originalParent)
                               .try_as<FrameworkElement>();
    auto root = systemTrayFrame
                    ? Media::VisualTreeHelper::GetParent(systemTrayFrame)
                          .try_as<Controls::Grid>()
                    : nullptr;
    if (!root || !root.XamlRoot() || root.ActualWidth() <= 0 ||
        root.ActualHeight() <= 0) {
        return false;
    }

    auto widgets = FindDescendantByName(root, L"AugmentedEntryPointButton");
    return !widgets || widgets.Visibility() != Visibility::Visible ||
           widgets.ActualWidth() > 0;
}

bool LayoutUpdatedWaitExpired(FrameworkElement content) {
    for (auto& data : DeferredClocks()) {
        if (data.content.get() != content || !data.layoutUpdatedToken) {
            continue;
        }

        data.layoutUpdatedAttempts++;
        ULONGLONG elapsed =
            data.layoutUpdatedStartTick
                ? GetTickCount64() - data.layoutUpdatedStartTick
                : kLayoutReadyTimeoutMs;
        return data.layoutUpdatedAttempts >= kLayoutReadyMaxAttempts ||
               elapsed >= kLayoutReadyTimeoutMs;
    }

    return true;
}

void MoveClockSafely(FrameworkElement content) {
    if (!g_callbacksEnabled || !g_movedClocks) {
        return;
    }

    try {
        MoveClock(content);
    } catch (...) {
        Wh_Log(L"Clock relocation failed: %08X", winrt::to_hresult());
    }
}

void RegisterLayoutUpdatedHandler(FrameworkElement content) {
    auto weakContent = winrt::make_weak(content);

    winrt::Windows::Foundation::EventHandler<
        winrt::Windows::Foundation::IInspectable>
        handler =
            [weakContent](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) {
                if (!g_callbacksEnabled) {
                    return;
                }

                if (auto content = weakContent.get()) {
                    try {
                        if (!ClockLayoutIsReady(content)) {
                            if (LayoutUpdatedWaitExpired(content)) {
                                RemoveLayoutUpdatedHandler(content);
                                Wh_Log(L"Clock layout wait expired; relocation "
                                       L"will retry after the next template "
                                       L"callback");
                            }
                            return;
                        }

                        RemoveLayoutUpdatedHandler(content);
                        MoveClockSafely(content);
                    } catch (...) {
                        Wh_Log(L"Layout-ready clock handling failed: %08X",
                               winrt::to_hresult());
                    }
                }
            };

    if (!g_callbacksEnabled) {
        return;
    }

    uint64_t registration;
    {
        auto& data = GetDeferredData(content);
        if (data.layoutUpdatedToken || data.layoutUpdatedRegistration) {
            return;
        }
        registration = ++g_nextDeferredRegistration;
        data.layoutUpdatedRegistration = registration;
        data.layoutUpdatedStartTick = GetTickCount64();
        data.layoutUpdatedAttempts = 0;
    }
    winrt::event_token token;
    try {
        token = content.LayoutUpdated(handler);
    } catch (...) {
        if (auto current = FindDeferredClockData(content);
            current && current->layoutUpdatedRegistration == registration) {
            current->layoutUpdatedRegistration = 0;
            current->layoutUpdatedStartTick = 0;
            current->layoutUpdatedAttempts = 0;
        }
        throw;
    }
    auto current = FindDeferredClockData(content);
    if (!g_callbacksEnabled || !current ||
        current->layoutUpdatedRegistration != registration) {
        content.LayoutUpdated(token);
        return;
    }
    current->layoutUpdatedToken = token;
    current->layoutUpdatedRegistration = 0;
    current = nullptr;
    content.InvalidateMeasure();
}

void RegisterLoadedHandler(FrameworkElement content) {
    auto weakContent = winrt::make_weak(content);

    RoutedEventHandler handler =
        [weakContent](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            if (!g_callbacksEnabled) {
                return;
            }

            if (auto content = weakContent.get()) {
                try {
                    RemoveLoadedHandler(content);
                    RegisterLayoutUpdatedHandler(content);
                } catch (...) {
                    Wh_Log(L"Loaded clock handling failed: %08X",
                           winrt::to_hresult());
                }
            }
        };

    if (!g_callbacksEnabled) {
        return;
    }

    uint64_t registration;
    {
        auto& data = GetDeferredData(content);
        if (data.loadedToken || data.loadedRegistration) {
            return;
        }
        registration = ++g_nextDeferredRegistration;
        data.loadedRegistration = registration;
    }
    winrt::event_token token;
    try {
        token = content.Loaded(handler);
    } catch (...) {
        if (auto current = FindDeferredClockData(content);
            current && current->loadedRegistration == registration) {
            current->loadedRegistration = 0;
        }
        throw;
    }
    auto current = FindDeferredClockData(content);
    if (!g_callbacksEnabled || !current ||
        current->loadedRegistration != registration) {
        content.Loaded(token);
        return;
    }
    current->loadedToken = token;
    current->loadedRegistration = 0;
}

void QueueKnownClockPlacementUpdate() {
    if (!g_callbacksEnabled || !g_deferredClocks ||
        g_clockThreadId != GetCurrentThreadId()) {
        return;
    }

    std::vector<FrameworkElement> contents;
    for (auto& data : DeferredClocks()) {
        if (auto content = data.content.get()) {
            contents.push_back(content);
        }
    }

    for (const auto& content : contents) {
        RegisterLayoutUpdatedHandler(content);
    }
}

void UpdateClockPlacement(FrameworkElement content) {
    if (!g_callbacksEnabled || !g_deferredClocks || !g_movedClocks) {
        return;
    }

    // The clock template and TaskbarFrame layout callbacks are expected to run
    // on the same XAML UI thread. Capture the first clock callback's thread and
    // reject later callbacks from any other thread before touching XAML state.
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD clockThreadId = g_clockThreadId;
    if (clockThreadId && clockThreadId != currentThreadId) {
        Wh_Log(L"Clock callback skipped on a non-taskbar thread: %u != %u",
               currentThreadId, clockThreadId);
        return;
    }
    g_clockThreadId = currentThreadId;

    RememberClockContent(content);

    auto clock = FindAncestor(content, L"SystemTray.OmniButton",
                              L"NotificationCenterButton");
    if (!clock) {
        return;
    }

    if (CenteredViewportFallbackActive(clock)) {
        RemoveDeferredHandlers(content);
        return;
    }

    if (!ShouldRelocateClock()) {
        RestoreClock(clock);
        RemoveDeferredHandlers(content);
        bool logAlignment = GetDeferredData(content).diagnosticAlignment != 0;
        if (logAlignment &&
            ClockLayoutIsReady(content)) {
            auto tray = FindAncestor(clock, L"SystemTray.SystemTrayFrame", L"");
            auto root = tray ? Media::VisualTreeHelper::GetParent(tray)
                                  .try_as<Controls::Grid>()
                             : nullptr;
            if (root) {
                if (auto current = FindDeferredClockData(content)) {
                    current->diagnosticAlignment = 0;
                }
                LogLayoutSnapshot(clock, root, L"native-left-aligned");
            }
        }
    } else if (content.IsLoaded()) {
        GetDeferredData(content).diagnosticAlignment = 1;
        RemoveLoadedHandler(content);
        if (ClockIsAlreadyMoved(clock)) {
            RemoveLayoutUpdatedHandler(content);
            UpdateLeftHostOffsetForClock(winrt::make_weak(clock));
        } else {
            RegisterLayoutUpdatedHandler(content);
        }
    } else {
        RegisterLoadedHandler(content);
    }
}

void StopDeferredCallbacksOnTaskbarThread() {
    if (!g_deferredClocks) {
        return;
    }

    std::vector<FrameworkElement> contents;
    for (const auto& data : DeferredClocks()) {
        if (auto content = data.content.get()) {
            contents.push_back(content);
        }
    }
    for (const auto& content : contents) {
        try {
            RemoveDeferredHandlers(content);
        } catch (...) {
            Wh_Log(L"Deferred handler cleanup failed: %08X",
                   winrt::to_hresult());
        }
    }
}

void RestoreAllMovedClocksOnTaskbarThread() {
    if (!g_movedClocks) {
        return;
    }

    auto& movedClocks = MovedClocks();
    while (!movedClocks.empty()) {
        // Own this record before calling XAML; cleanup must never act on a
        // different entry that happens to become the vector's front later.
        auto data = std::move(movedClocks.front());
        movedClocks.erase(movedClocks.begin());

        try {
            RestoreMovedClockData(data);
        } catch (...) {
            Wh_Log(L"Clock restore failed: %08X", winrt::to_hresult());
            try {
                // Never strand native elements by clearing a failed restore.
                RemoveLeftHost(data);
            } catch (...) {
                Wh_Log(L"Clock host cleanup failed: %08X", winrt::to_hresult());
            }
        }
    }
}

void UpdateKnownClocksOnTaskbarThread() {
    if (!g_deferredClocks) {
        return;
    }

    std::vector<FrameworkElement> contents;
    for (auto& data : DeferredClocks()) {
        if (auto content = data.content.get()) {
            contents.push_back(content);
        }
    }

    for (const auto& content : contents) {
        UpdateClockPlacement(content);
    }
}

void UpdateKnownClocksCallback(void*) {
    try {
        UpdateKnownClocksOnTaskbarThread();
    } catch (...) {
        Wh_Log(L"Known clock update failed: %08X", winrt::to_hresult());
    }
}

struct CleanupTaskbarStateParameters {
    bool releaseContainers;
};

void CleanupTaskbarStateCallback(void* parameter) {
    auto* cleanup = static_cast<CleanupTaskbarStateParameters*>(parameter);

    StopDeferredCallbacksOnTaskbarThread();
    RestoreAllMovedClocksOnTaskbarThread();

    if (cleanup->releaseContainers) {
        // These containers intentionally have no process-shutdown destructor.
        // Release their XAML/WinRT members here, on the owning UI thread.
        g_deferredClocks.reset();
        g_movedClocks.reset();
    }
}

bool RunOnTaskbarThread(RunFromWindowThreadProc procedure, void* parameter,
                        PCWSTR operation) {
    auto taskbarWindows = FindExplorerTaskbarWindows();
    if (taskbarWindows.empty()) {
        Wh_Log(L"%s skipped: taskbar window not found", operation);
        return false;
    }

    DWORD expectedThreadId = g_clockThreadId;
    for (HWND taskbarWindow : taskbarWindows) {
        if (RunFromWindowThread(taskbarWindow, procedure, parameter,
                                expectedThreadId)) {
            return true;
        }
    }

    Wh_Log(L"%s failed on all %u taskbar-thread windows", operation,
           static_cast<unsigned>(taskbarWindows.size()));
    return false;
}

// BEGIN REGISTRY ALIGNMENT DISPATCH
void ApplyRegistryAlignmentCallback(void*) {
    if (!g_callbacksEnabled) {
        return;
    }

    // Read the latest value on the UI thread, not the value from a possibly
    // older notification. Quick back-and-forth switches cannot apply stale data.
    DWORD value = MAXDWORD;
    DWORD size = sizeof(value);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER, kAlignmentWatchKey, L"TaskbarAl",
        RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"[alignment-watch] UI alignment read failed: %ld", status);
        return;
    }

    int alignment = value != 0 ? 1 : 0;
    int previous = g_lastTaskbarCenteredAlignment.exchange(alignment);
    if (previous == alignment) {
        return;
    }
    UpdateKnownClocksCallback(nullptr);
}

void ApplyRegistryAlignmentSynchronously() {
    try {
        if (g_callbacksEnabled) {
            RunOnTaskbarThread(ApplyRegistryAlignmentCallback, nullptr,
                              L"Alignment notification");
        }
    } catch (...) {
        Wh_Log(L"[alignment-watch] UI dispatch failed: %08X",
               winrt::to_hresult());
    }
}
// END REGISTRY ALIGNMENT DISPATCH

void UpdateKnownClocksSynchronously() {
    RunOnTaskbarThread(UpdateKnownClocksCallback, nullptr,
                       L"Clock placement update");
}

bool CleanupTaskbarStateSynchronously(bool releaseContainers) {
    CleanupTaskbarStateParameters parameters{releaseContainers};
    if (RunOnTaskbarThread(CleanupTaskbarStateCallback, &parameters,
                           L"Clock layout cleanup")) {
        return true;
    }

    // A taskbar window can disappear briefly while Explorer rebuilds it. The
    // stored clock content still exposes its owning dispatcher, so use that as
    // a window-independent cleanup path before allowing this DLL to unload.
    if (!g_deferredClocks) {
        return false;
    }

    for (auto& data : DeferredClocks()) {
        auto content = data.content.get();
        if (!content) {
            continue;
        }

        try {
            auto dispatcher = content.Dispatcher();
            if (!dispatcher) {
                continue;
            }

            if (dispatcher.HasThreadAccess()) {
                CleanupTaskbarStateParameters fallbackParameters{
                    releaseContainers};
                CleanupTaskbarStateCallback(&fallbackParameters);
            } else {
                dispatcher
                    .RunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                        [releaseContainers] {
                            CleanupTaskbarStateParameters fallbackParameters{
                                releaseContainers};
                            CleanupTaskbarStateCallback(&fallbackParameters);
                        })
                    .get();
            }

            return true;
        } catch (...) {
            Wh_Log(L"Clock dispatcher cleanup failed: %08X",
                   winrt::to_hresult());
        }
    }

    Wh_Log(L"Clock layout cleanup failed: no live taskbar dispatcher");
    return false;
}

void WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    DateTimeIconContent_OnApplyTemplate_Original(pThis);

    try {
        FrameworkElement content = nullptr;
        IUnknown* contentUnknown = pThis ? ((IUnknown**)pThis)[1] : nullptr;
        if (contentUnknown &&
            SUCCEEDED(contentUnknown->QueryInterface(
                winrt::guid_of<FrameworkElement>(),
                winrt::put_abi(content)))) {
            UpdateClockPlacement(content);
        }
    } catch (...) {
        Wh_Log(L"Clock template handling failed: %08X",
               winrt::to_hresult());
    }
}

HRESULT WINAPI BadgeIconContent_get_ViewModel_Hook(LPVOID pThis,
                                                    LPVOID pArgs) {
    HRESULT result = BadgeIconContent_get_ViewModel_Original(pThis, pArgs);

    try {
        winrt::Windows::Foundation::IInspectable object = nullptr;
        winrt::check_hresult(
            ((IUnknown*)pThis)
                ->QueryInterface(
                    winrt::guid_of<
                        winrt::Windows::Foundation::IInspectable>(),
                    winrt::put_abi(object)));

        if (winrt::get_class_name(object) ==
            L"SystemTray.DateTimeIconContent") {
            auto content = object.as<FrameworkElement>();
            if (content.IsLoaded()) {
                UpdateClockPlacement(content);
            }
        }
    } catch (...) {
        Wh_Log(L"Clock ViewModel handling failed: %08X",
               winrt::to_hresult());
    }

    return result;
}

std::vector<HWND> FindExplorerTaskbarWindows() {
    struct TaskbarWindows {
        DWORD preferredThreadId;
        HWND preferredSystemTrayWindow;
        HWND preferredShellTrayWindow;
        std::vector<HWND> preferredOtherWindows;
        HWND systemTrayWindow;
        HWND shellTrayWindow;
    } windows{};
    windows.preferredThreadId = g_clockThreadId;

    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* windows = reinterpret_cast<TaskbarWindows*>(parameter);
            DWORD processId = 0;
            DWORD threadId = GetWindowThreadProcessId(window, &processId);
            WCHAR className[64];
            if (!threadId || processId != GetCurrentProcessId() ||
                !GetClassName(window, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"SystemTray_Main") == 0) {
                if (!windows->systemTrayWindow) {
                    windows->systemTrayWindow = window;
                }
                if (windows->preferredThreadId == threadId &&
                    !windows->preferredSystemTrayWindow) {
                    windows->preferredSystemTrayWindow = window;
                }
            } else if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                if (!windows->shellTrayWindow) {
                    windows->shellTrayWindow = window;
                }
                if (windows->preferredThreadId == threadId &&
                    !windows->preferredShellTrayWindow) {
                    windows->preferredShellTrayWindow = window;
                }
            } else if (windows->preferredThreadId == threadId) {
                windows->preferredOtherWindows.push_back(window);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));

    std::vector<HWND> result;
    auto appendUnique = [&result](HWND window) {
        if (window && std::find(result.begin(), result.end(), window) ==
                          result.end()) {
            result.push_back(window);
        }
    };

    if (windows.preferredThreadId) {
        appendUnique(windows.preferredSystemTrayWindow);
        appendUnique(windows.preferredShellTrayWindow);
        for (HWND window : windows.preferredOtherWindows) {
            appendUnique(window);
        }
    } else {
        appendUnique(windows.systemTrayWindow);
        appendUnique(windows.shellTrayWindow);
    }

    return result;
}

HWND FindExplorerTaskbarWindow() {
    auto windows = FindExplorerTaskbarWindows();
    return windows.empty() ? nullptr : windows.front();
}

void RefreshTaskbarClock() {
    if (!FindExplorerTaskbarWindow()) {
        return;
    }

    // Windows watches this key for clock changes. Creating and immediately
    // deleting a private value refreshes the already-created clock without
    // changing any user setting.
    constexpr WCHAR valueName[] =
        L"_temp_windhawk_taskbar-clock-to-left";
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                     KEY_WRITE, &key) == ERROR_SUCCESS) {
        if (RegSetValueEx(key, valueName, 0, REG_SZ,
                          reinterpret_cast<const BYTE*>(L""),
                          sizeof(WCHAR)) == ERROR_SUCCESS) {
            RegDeleteValue(key, valueName);
        }
        RegCloseKey(key);
    }
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    HRSRC resource =
        FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (!resource) {
        return nullptr;
    }

    HGLOBAL loadedResource = LoadResource(module, resource);
    void* data = loadedResource ? LockResource(loadedResource) : nullptr;
    void* fixedInfo = nullptr;
    UINT size = 0;
    if (!data || !VerQueryValue(data, L"\\", &fixedInfo, &size) || !size) {
        return nullptr;
    }

    return static_cast<VS_FIXEDFILEINFO*>(fixedInfo);
}

HMODULE GetSystemTrayModule() {
    if (HMODULE module = GetModuleHandle(L"SystemTray.dll")) {
        return module;
    }

    if (HMODULE module = GetModuleHandle(L"Taskbar.View.dll")) {
        auto version = GetModuleVersionInfo(module);
        WORD major = version ? HIWORD(version->dwFileVersionMS) : 0;
        if (major && major < 2604) {
            return module;
        }

        // On newer builds the clock implementation is loaded later from
        // SystemTray.dll. Don't latch an ExplorerExtensions fallback before
        // that real module arrives.
        return nullptr;
    }

    return GetModuleHandle(L"ExplorerExtensions.dll");
}

HMODULE GetTaskbarViewModule() {
    if (HMODULE module = GetModuleHandle(L"Taskbar.View.dll")) {
        return module;
    }

    return GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarView(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Taskbar::ITaskbarFrame>::get_Alignment(int *))"},
            &TaskbarFrame_get_Alignment_Original,
            TaskbarFrame_get_Alignment_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

bool HookSystemTray(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &BadgeIconContent_get_ViewModel_Original,
            BadgeIconContent_get_ViewModel_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

bool HookTaskbarViewAndSystemTray(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Taskbar::ITaskbarFrame>::get_Alignment(int *))"},
            &TaskbarFrame_get_Alignment_Original,
            TaskbarFrame_get_Alignment_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &BadgeIconContent_get_ViewModel_Original,
            BadgeIconContent_get_ViewModel_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

struct HookSetupGuard {
    ~HookSetupGuard() {
        g_hookSetupInProgress.clear(std::memory_order_release);
    }
};

bool TryHookAvailableModules(bool applyImmediately) {
    // Once both module roles have been attempted, no later DLL load can add
    // useful work. In particular, don't repeatedly invalidate and resolve the
    // same symbol cache if a symbol wasn't available on this Windows build.
    if (g_taskbarViewHookAttempted && g_systemTrayHookAttempted) {
        return g_taskbarViewHooked && g_systemTrayHooked;
    }

    if (g_hookSetupInProgress.test_and_set(std::memory_order_acquire)) {
        return true;
    }
    HookSetupGuard guard;

    if (g_taskbarViewHookAttempted && g_systemTrayHookAttempted) {
        return g_taskbarViewHooked && g_systemTrayHooked;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModule();
    HMODULE systemTrayModule = GetSystemTrayModule();
    bool taskbarViewHookedNow = false;
    bool systemTrayHookedNow = false;
    bool success = true;

    if (taskbarViewModule && taskbarViewModule == systemTrayModule) {
        if (!g_taskbarViewHookAttempted &&
            !g_systemTrayHookAttempted) {
            // Mark the shared module before resolving symbols so a failed
            // attempt isn't repeated from every later LoadLibraryExW call.
            g_taskbarViewHookAttempted = true;
            g_systemTrayHookAttempted = true;
            if (HookTaskbarViewAndSystemTray(taskbarViewModule)) {
                g_taskbarViewHooked = true;
                g_systemTrayHooked = true;
                taskbarViewHookedNow = true;
                systemTrayHookedNow = true;
            } else {
                Wh_Log(L"Failed to hook the shared taskbar module");
                success = false;
            }
        } else {
            // A mixed attempted state is unusual, but can occur if the module
            // candidates change during Explorer startup. Attempt only the role
            // that hasn't been handled yet.
            if (!g_taskbarViewHookAttempted) {
                g_taskbarViewHookAttempted = true;
                if (HookTaskbarView(taskbarViewModule)) {
                    g_taskbarViewHooked = true;
                    taskbarViewHookedNow = true;
                } else {
                    Wh_Log(L"Failed to hook the Windows 11 taskbar layout");
                    success = false;
                }
            }
            if (!g_systemTrayHookAttempted) {
                g_systemTrayHookAttempted = true;
                if (HookSystemTray(systemTrayModule)) {
                    g_systemTrayHooked = true;
                    systemTrayHookedNow = true;
                } else {
                    Wh_Log(L"Failed to hook the Windows 11 system tray");
                    success = false;
                }
            }
        }
    } else {
        if (taskbarViewModule && !g_taskbarViewHookAttempted) {
            g_taskbarViewHookAttempted = true;
            if (HookTaskbarView(taskbarViewModule)) {
                g_taskbarViewHooked = true;
                taskbarViewHookedNow = true;
            } else {
                Wh_Log(L"Failed to hook the Windows 11 taskbar layout");
                success = false;
            }
        }

        if (systemTrayModule && !g_systemTrayHookAttempted) {
            g_systemTrayHookAttempted = true;
            if (HookSystemTray(systemTrayModule)) {
                g_systemTrayHooked = true;
                systemTrayHookedNow = true;
            } else {
                Wh_Log(L"Failed to hook the Windows 11 system tray");
                success = false;
            }
        }
    }

    if (applyImmediately && (taskbarViewHookedNow || systemTrayHookedNow)) {
        Wh_ApplyHookOperations();
    }

    return success;
}

FrameworkElement ReadNativeNotifyChevron(const MovedClockData& data, Controls::Grid root) {
    auto clock = data.clock.get();
    auto parent = data.originalParent.get().try_as<Controls::Grid>();
    auto frame = parent ? Media::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>() : nullptr;
    if (!clock || !data.leftHost || Media::VisualTreeHelper::GetParent(clock) != data.leftHost ||
        !root || !root.IsLoaded() || !root.XamlRoot() || !parent || !parent.IsLoaded() ||
        parent.Name() != L"SystemTrayFrameGrid" || !frame ||
        winrt::get_class_name(frame) != L"SystemTray.SystemTrayFrame" ||
        Media::VisualTreeHelper::GetParent(frame) != root || parent.XamlRoot() != root.XamlRoot() ||
        parent.Children().Size() > 32) return nullptr;
    FrameworkElement group{nullptr};
    for (auto child : parent.Children()) {
        auto candidate = child.try_as<FrameworkElement>();
        if (!candidate || candidate.Name() != L"NotifyIconStack" ||
            winrt::get_class_name(candidate) != L"SystemTray.Stack") continue;
        if (group) return nullptr;
        group = candidate;
    }
    if (!group || !group.IsLoaded() || group.Visibility() != Visibility::Visible ||
        group.XamlRoot() != root.XamlRoot() || !ReadTrayGroupItemsSource(group)) return nullptr;
    auto arrow = FindNotifyChevron(group);
    if (!arrow || !arrow.IsLoaded() || arrow.Visibility() != Visibility::Visible ||
        arrow.XamlRoot() != root.XamlRoot()) return nullptr;
    return arrow;
}

std::optional<int> ClampNotifyPopupX(int x, int width, const RECT& work) {
    int64_t span = static_cast<int64_t>(work.right) - work.left;
    if (width <= 0 || span <= 0 || width > span) return std::nullopt;
    return static_cast<int>(std::clamp<int64_t>(x, work.left, static_cast<int64_t>(work.right) - width));
}

BOOL WINAPI SetWindowPos_Hook(HWND window, HWND insertAfter, int x, int y,
                             int cx, int cy, UINT flags) {
    bool inspectPlacement = false;
    try {
        DWORD process = 0;
        wchar_t name[64];
        if (g_callbacksEnabled && g_movedClocks &&
            GetCurrentThreadId() == g_clockThreadId && !(flags & SWP_HIDEWINDOW) &&
            GetWindowThreadProcessId(window, &process) == g_clockThreadId &&
            process == GetCurrentProcessId() && GetAncestor(window, GA_ROOT) == window &&
            !GetWindow(window, GW_OWNER) && GetClassNameW(window, name, ARRAYSIZE(name)) &&
            wcscmp(name, L"TopLevelWindowForOverflowXamlIsland") == 0 &&
            GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext()) == DPI_AWARENESS_PER_MONITOR_AWARE) {
            uint32_t candidates = 0;
            PCWSTR arrowSide = L"unknown";
            for (auto& data : MovedClocks()) {
                if (data.restoring || data.updatingLayout) continue;
                auto root = data.taskbarRoot.get();
                if (!root || !root.IsLoaded() || !root.XamlRoot()) continue;
                FrameworkElement arrow{nullptr};
                bool movedArrow = false;
                if (data.notifyIcons && data.notifyIcons->lastContribution >= 0) {
                    PCWSTR reason = nullptr;
                    auto contribution = ReadNotifyIconGroupContribution(data, &reason);
                    if (contribution && !reason) {
                        arrow = data.notifyIcons->nativeChevron.get();
                        movedArrow = true;
                    }
                } else if (!data.notifyIcons) {
                    bool movedGroup = false;
                    for (const auto* saved : {&data.controlCenter, &data.trayGroup, &data.applicationTray}) {
                        if (!*saved || (*saved)->lastContribution < 0) continue;
                        PCWSTR reason = nullptr;
                        auto contribution = ReadMovedTrayContribution(data, *saved, &reason);
                        movedGroup |= contribution.has_value() && !reason;
                    }
                    if (movedGroup) arrow = ReadNativeNotifyChevron(data, root);
                }
                if (arrow && arrow.IsLoaded() && arrow.XamlRoot() == root.XamlRoot()) {
                    candidates++;
                    arrowSide = movedArrow ? L"left" : L"right";
                }
            }
            RECT current{};
            MONITORINFO monitor{sizeof(monitor)};
            if (candidates == 1 && GetWindowRect(window, &current) &&
                GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) {
                int nativeX = flags & SWP_NOMOVE ? current.left : x;
                int width = flags & SWP_NOSIZE ? current.right - current.left : cx;
                auto corrected = ClampNotifyPopupX(nativeX, width, monitor.rcWork);
                if (corrected) {
                    inspectPlacement = true;
                    Wh_Log(L"[notify-popup] placement window=%p nativeX=%d correctedX=%d width=%d work=(%ld,%ld) flags=%08X arrowSide=%s",
                           window, nativeX, *corrected, width, monitor.rcWork.left, monitor.rcWork.right, flags, arrowSide);
                    if (*corrected != nativeX) {
                        x = *corrected;
                        if (flags & SWP_NOMOVE) y = current.top;
                        flags &= ~SWP_NOMOVE;
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[notify-popup] placement inspection failed: %08X", winrt::to_hresult());
    }
    BOOL result = SetWindowPos_Original(window, insertAfter, x, y, cx, cy, flags);
    if (inspectPlacement) {
        DWORD error = GetLastError();
        RECT actual{};
        BOOL rectValid = GetWindowRect(window, &actual);
        Wh_Log(L"[notify-popup] result window=%p success=%d rectValid=%d actual=(%ld,%ld,%ld,%ld)",
               window, result, rectValid, actual.left, actual.top, actual.right, actual.bottom);
        SetLastError(error);
    }
    return result;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module) {
        TryHookAvailableModules(true);
    }
    return module;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Taskbar Clock to Left 2.3: keepOnLeftWhenLeftAligned=%d moveControlCenter=%d moveNonActivatableStack=%d moveNotifyIconStack=%d moveNotificationAreaIcons=%d reserveCenteredViewport=%d",
           g_settings.keepOnLeftWhenLeftAligned, g_settings.moveControlCenter,
           g_settings.moveNonActivatableStack, g_settings.moveNotifyIconStack,
           g_settings.moveNotificationAreaIcons, g_settings.reserveCenteredViewport);
    g_clockThreadId = 0;
    g_lastTaskbarCenteredAlignment =
        ReadTaskbarUsesCenteredAlignment() ? 1 : 0;
    g_callbacksEnabled = true;

    if (!TryHookAvailableModules(false)) {
        return FALSE;
    }

    HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
    auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
        GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                        &LoadLibraryExW_Original)) {
        return FALSE;
    }

    if (g_settings.moveNotifyIconStack || g_settings.moveControlCenter ||
        g_settings.moveNonActivatableStack || g_settings.moveNotificationAreaIcons) {
        HMODULE user32 = GetModuleHandle(L"user32.dll");
        auto setWindowPos = user32 ? reinterpret_cast<decltype(&SetWindowPos)>(GetProcAddress(user32, "SetWindowPos")) : nullptr;
        g_notifyPopupHooked = setWindowPos && WindhawkUtils::SetFunctionHook(
            setWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
        if (!g_notifyPopupHooked) Wh_Log(L"[notify-icons] popup hook unavailable; keeping native arrow");
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookAvailableModules(true);

    if (g_taskbarViewHooked) {
        UpdateKnownClocksSynchronously();
    }
    if (g_systemTrayHooked) {
        RefreshTaskbarClock();
    }
    StartAlignmentRegistryWatch();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_callbacksEnabled = false;
    StopAlignmentRegistryWatch();
    // Revoke all mod-owned XAML delegates and release their containers while
    // the mod code and hooks are still present. The synchronous taskbar-thread
    // call must finish before Windhawk can continue unloading this DLL.
    CleanupTaskbarStateSynchronously(true);
}

void Wh_ModUninit() {
    g_callbacksEnabled = false;
    StopAlignmentRegistryWatch();
    // Normally Wh_ModBeforeUninit already released both containers. Retry only
    // if taskbar discovery failed during that earlier cleanup.
    if (g_deferredClocks || g_movedClocks) {
        CleanupTaskbarStateSynchronously(true);
    }
}

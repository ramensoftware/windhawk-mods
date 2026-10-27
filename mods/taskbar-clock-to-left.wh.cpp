// ==WindhawkMod==
// @id              taskbar-clock-to-left
// @name            Taskbar Clock to Left
// @description     Move the native Windows 11 clock/notification button to the left without shifting centered taskbar apps
// @version         2.3.13
// @author          pleromyst
// @github          https://github.com/pleromyst
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Clock to Left - Tray Test

Places the single native Windows 11 clock button in a dedicated left container.
The original button, text, calendar and notification interactions are preserved.

- Follows the native system tray background, border and spacing. Asymmetric
  decoration faces the left screen edge; the clock text is never mirrored.
- Keeps clear of Start, search, Task View, Widgets and application buttons,
  including a Start button fixed to the left by another mod.
- Uses existing empty space. If placement is unsafe or cannot be verified,
  restores the clock to its native position without pushing other buttons.
- Supports one/two text lines. Three or more lines and content exceeding the
  taskbar window fall back; clipping caused only by a short theme box is allowed.
- Enabling the mod starts placement; disabling it restores the native clock.
- After a safety fallback, retries when effective space, text size or layout
  changes. Unchanged time ticks do not rescan taskbar items or move the clock.

![Taskbar Clock to Left](https://i.imgur.com/Wew0LZh.png)

Shown with **Windows 11 Taskbar Styler** using the **Surface** theme and
**Taskbar Clock Customization**. This mod places the clock in a matching
container on the left for a more balanced taskbar layout.

![Taskbar Clock to Left with the Surface theme and Taskbar Clock Customization](https://i.imgur.com/ms9rpmz.png)
Inspired by [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization)
by [m417z](https://github.com/m417z).

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <optional>
#include <string_view>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>
// The taskbar uses WinUI 2 controls on top of system XAML.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

constexpr double kLayoutTolerance = 0.5;
constexpr double kTaskbarLayoutGap = 4.0;
constexpr float kNativeClockMeasureLimit = 10000.0f;

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

struct TaskbarElementSample {
    uintptr_t repeaterIdentity{};
    Controls::Grid root{nullptr};
    FrameworkElement repeater{nullptr};
    std::vector<FrameworkElement> elements;
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
    std::vector<SystemButtonLayout> buttons;
    std::vector<FlowElementLayout> flowElements;
};

// BEGIN CLOCK TRAY
// Cleanup retains unfinished restoration and rejects callbacks from old work.
enum class ClockTrayState {
    Disabled,
    AwaitingLayout,
    NativeWaiting,
    Placing,
    Stable,
    Restoring,
};

enum class ClockTrayReason {
    None,
    NoSpace,
    TooTall,
    TooManyLines,
    UnknownMetrics,
    TargetOwned,
    UnsupportedLayout,
    UnsupportedAppearance,
    ElementReplaced,
    UserDisabled,
};

PCWSTR ClockTrayStateName(ClockTrayState state) {
    switch (state) {
        case ClockTrayState::Disabled:
            return L"disabled";
        case ClockTrayState::AwaitingLayout:
            return L"awaiting-layout";
        case ClockTrayState::NativeWaiting:
            return L"native-waiting";
        case ClockTrayState::Placing:
            return L"placing";
        case ClockTrayState::Stable:
            return L"stable";
        case ClockTrayState::Restoring:
            return L"restoring";
    }
    return L"unknown";
}

PCWSTR ClockTrayReasonName(ClockTrayReason reason) {
    switch (reason) {
        case ClockTrayReason::None:
            return L"none";
        case ClockTrayReason::NoSpace:
            return L"no-space";
        case ClockTrayReason::TooTall:
            return L"too-tall";
        case ClockTrayReason::TooManyLines:
            return L"too-many-lines";
        case ClockTrayReason::UnknownMetrics:
            return L"unknown-metrics";
        case ClockTrayReason::TargetOwned:
            return L"target-owned";
        case ClockTrayReason::UnsupportedLayout:
            return L"unsupported-layout";
        case ClockTrayReason::UnsupportedAppearance:
            return L"unsupported-appearance";
        case ClockTrayReason::ElementReplaced:
            return L"element-replaced";
        case ClockTrayReason::UserDisabled:
            return L"user-disabled";
    }
    return L"unknown";
}

struct ClockTrayRetryKey {
    std::vector<double> values;
    std::vector<uintptr_t> identities;
    bool operator==(const ClockTrayRetryKey&) const = default;
};

struct ClockTrayCounters {
    uint64_t signals{}, metricReads{}, layoutReads{}, marginWrites{};
    uint64_t attachments{}, restorations{}, nativeAttempts{}, unchangedMetrics{};
};
ClockTrayCounters g_clockTrayCounters;

void LogClockTrayCounters() {
    Wh_Log(L"[clock-tray] counters signals=%llu metrics=%llu layouts=%llu margins=%llu attaches=%llu restores=%llu attempts=%llu unchanged=%llu",
        g_clockTrayCounters.signals, g_clockTrayCounters.metricReads, g_clockTrayCounters.layoutReads,
        g_clockTrayCounters.marginWrites, g_clockTrayCounters.attachments, g_clockTrayCounters.restorations,
        g_clockTrayCounters.nativeAttempts, g_clockTrayCounters.unchangedMetrics);
}

struct ClockTrayRuntime {
    ClockTrayState state{ClockTrayState::Disabled};
    ClockTrayReason reason{ClockTrayReason::None};
    // Bumped whenever queued work must be discarded: reload, unload, or an
    // element replacement seen by the dispatcher.
    uint64_t generation{1};
    bool pending{};
    bool reportedInGeneration{};
    // Compare only inputs sampled in the native position. Our own migration
    // changes the tray extent and must not count as newly available space.
    std::optional<ClockTrayRetryKey> lastNativeAttempt;
    winrt::weak_ref<FrameworkElement> clock;
};

ClockTrayRuntime g_clockTray;

void SetClockTrayResult(ClockTrayState state, ClockTrayReason reason) {
    if (g_clockTray.state == state && g_clockTray.reason == reason) {
        return;
    }
    g_clockTray.state = state;
    g_clockTray.reason = reason;
    Wh_Log(L"[clock-tray] state=%s reason=%s generation=%llu",
           ClockTrayStateName(state), ClockTrayReasonName(reason),
           static_cast<unsigned long long>(g_clockTray.generation));
}

void InvalidateClockTrayWork(ClockTrayReason reason, ClockTrayState state) {
    g_clockTray.generation++;
    g_clockTray.pending = false;
    g_clockTray.reportedInGeneration = false;
    g_clockTray.clock = {};
    g_clockTray.reason = reason;
    g_clockTray.state = state;
}

// Everything needed to put the native clock back. There is exactly one native
// clock, so one record is enough; a second entry would mean two clocks.
struct ClockTrayAppearanceHook {
    winrt::weak_ref<DependencyObject> owner;
    DependencyProperty property{nullptr};
    int64_t token{};
};

struct ClockTrayMetrics {
    double requiredWidth{};
    double requiredHeight{};
    unsigned lines{}; // Three means three or more, including clipped lines.
    double nativeChromeWidth{};
};

struct ClockTrayPlan {
    ClockTrayReason reason{ClockTrayReason::None};
    double layoutLeft{};
    double visualShift{};
    double hostLeft{};
    double hostWidth{};
    HorizontalBounds visible{};
};

struct ClockTrayLayoutStamp {
    uintptr_t repeater{};
    uintptr_t source{};
    int count{};
    bool centered{};
    double nativeColumnWidth{};
    std::array<double, 9> geometry{};
    std::array<double, 12> systemGeometry{};
    bool operator==(const ClockTrayLayoutStamp&) const = default;
};

struct ClockTrayPlacement {
    bool placed{};
    bool restoring{};
    winrt::weak_ref<FrameworkElement> clock;
    // Hold the native element only while it is between parents.
    FrameworkElement detachedClock{nullptr};
    winrt::weak_ref<Controls::Grid> root;
    winrt::weak_ref<Controls::Panel> originalParent;
    uint32_t originalIndex{};
    int originalColumn{};
    int originalColumnSpan{1};
    int originalRow{};
    int originalRowSpan{1};
    HorizontalAlignment originalHorizontalAlignment{};
    VerticalAlignment originalVerticalAlignment{};
    VerticalAlignment appliedVerticalAlignment{VerticalAlignment::Stretch};
    Thickness originalMargin{};
    uint32_t changedProperties{};
    uint32_t unsetProperties{};
    Controls::Grid host{nullptr};
    Controls::Border decoration{nullptr};
    std::optional<winrt::event_token> clockSizeToken;
    winrt::weak_ref<FrameworkElement> clockSizeOwner;
    winrt::weak_ref<Controls::Panel> appearanceSource;
    std::vector<ClockTrayAppearanceHook> appearanceHooks;
    std::vector<ClockTrayAppearanceHook> metricHooks;
    std::optional<winrt::event_token> metricLayoutToken;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater> layoutRepeater;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::ItemsSourceView> layoutItems;
    std::optional<winrt::event_token> itemsChangedToken;
    std::optional<winrt::event_token> elementPreparedToken;
    std::optional<winrt::event_token> elementClearingToken;
    std::optional<ClockTrayLayoutStamp> layoutStamp;
    bool layoutTrackingDirty{};
    bool geometryDirty{};
    bool metricsDirty{};
    winrt::weak_ref<Controls::StackPanel> textStack;
    std::array<winrt::weak_ref<FrameworkElement>, 4> systemButtons;
    std::optional<ClockTrayMetrics> observedMetrics;
    ClockTrayMetrics metrics;
    ClockTrayPlan plan;
    Controls::ColumnDefinition nativeColumn{nullptr};
    double nativeColumnWidth{};
    double lastReservedWidth{-1};
    uint64_t arrangeWaitStart{};
    DispatcherTimer arrangeDeadline{nullptr};
    std::optional<winrt::event_token> arrangeDeadlineToken;
    std::optional<winrt::event_token> clockLoadedToken;
};

[[clang::no_destroy]] ClockTrayPlacement g_clockTrayPlacement;
bool g_clockTrayWorkActive{};
std::atomic<bool> g_clockTrayCleanupPending{};

struct ClockTrayWorkScope {
    bool previous{g_clockTrayWorkActive};
    ClockTrayWorkScope() { g_clockTrayWorkActive = true; }
    ~ClockTrayWorkScope() { g_clockTrayWorkActive = previous; }
};

// Defined with the layout code below; declared here for the dispatcher.
bool TryAttachClockTray(FrameworkElement clock);
void UpdateClockTrayPlacement();
void RestoreClockTray(ClockTrayReason reason);
void EnsureClockTrayRetry(FrameworkElement clock);
void ProcessClockTrayChanges(uint64_t generation);
bool ClearClockTrayMetricHooks();
bool ClockTrayWorkIsCurrent(uint64_t generation);

void ResetClockTrayForSession() {
    g_clockTray.clock = {};
    g_clockTray.pending = false;
    g_clockTray.reportedInGeneration = false;
    g_clockTray.lastNativeAttempt.reset();
    g_clockTrayCounters = {};
    g_clockTray.generation++;
    if (g_clockTrayCleanupPending) {
        SetClockTrayResult(ClockTrayState::Restoring,
                           ClockTrayReason::ElementReplaced);
        Wh_Log(L"[clock-tray] pending cleanup retained at session start");
        return;
    }
    g_clockTrayPlacement = ClockTrayPlacement{};
    g_clockTray.reason = ClockTrayReason::None;
    g_clockTray.state = ClockTrayState::AwaitingLayout;
    Wh_Log(L"[clock-tray] session state=%s reason=%s generation=%llu",
           ClockTrayStateName(g_clockTray.state),
           ClockTrayReasonName(g_clockTray.reason),
           static_cast<unsigned long long>(g_clockTray.generation));
}

// The enabled mod owns the clock. This path attaches one
// container and mirrors the native tray background; it never pushes the app
// row. A placement that does not fit is dropped, not forced.
void QueueClockTrayUpdate(FrameworkElement clock) {
    if (!clock || g_clockTrayWorkActive ||
        !ClockTrayWorkIsCurrent(g_clockTray.generation)) {
        return;
    }
    if (auto previous = g_clockTrayPlacement.clock.get(); previous && previous != clock) {
        RestoreClockTray(ClockTrayReason::ElementReplaced);
        if (!g_clockTrayPlacement.placed && !g_clockTrayPlacement.detachedClock &&
            !g_clockTrayPlacement.changedProperties && ClearClockTrayMetricHooks()) {
            g_clockTrayPlacement = {};
            g_clockTrayCleanupPending = false;
        }
        if (g_clockTrayCleanupPending) return;
        g_clockTray.lastNativeAttempt.reset();
    }
    if (g_clockTrayPlacement.placed) {
        g_clockTrayPlacement.metricsDirty = true;
        g_clockTrayPlacement.layoutTrackingDirty = true;
        return;
    }
    if (g_clockTrayPlacement.clock.get() == clock &&
        g_clockTrayPlacement.metricLayoutToken) {
        g_clockTrayPlacement.geometryDirty = true;
        g_clockTrayPlacement.metricsDirty = true;
        g_clockTrayPlacement.layoutTrackingDirty = true;
        return;
    }
    if (g_clockTrayCleanupPending) return;
    g_clockTray.clock = clock;
    g_clockTray.pending = true;
    if (g_clockTray.state == ClockTrayState::Disabled) {
        SetClockTrayResult(ClockTrayState::AwaitingLayout, ClockTrayReason::None);
    }
    if (!g_clockTray.reportedInGeneration) {
        g_clockTray.reportedInGeneration = true;
        Wh_Log(L"[clock-tray] dispatch generation=%llu",
               static_cast<unsigned long long>(g_clockTray.generation));
    }
    if (!TryAttachClockTray(clock)) EnsureClockTrayRetry(clock);
}
// END CLOCK TRAY (P2 dispatch)

std::atomic<bool> g_callbacksEnabled;
std::atomic<bool> g_systemTrayHooked;
std::atomic<bool> g_taskbarViewHooked;
std::atomic<bool> g_systemTrayHookAttempted;
std::atomic<bool> g_taskbarViewHookAttempted;
std::atomic<DWORD> g_clockThreadId;
std::atomic<bool> g_extentThreadMismatchLogged;

bool ClockTrayWorkIsCurrent(uint64_t generation) {
    return g_callbacksEnabled &&
           g_clockThreadId == GetCurrentThreadId() &&
           generation == g_clockTray.generation &&
           g_clockTray.state != ClockTrayState::Restoring;
}

template <typename Callback>
void ApplyClockTrayStep(uint64_t generation, Callback&& callback) {
    if (!ClockTrayWorkIsCurrent(generation)) {
        throw winrt::hresult_error(E_ABORT);
    }
    callback();
    if (!ClockTrayWorkIsCurrent(generation)) {
        throw winrt::hresult_error(E_ABORT);
    }
}

template <typename T>
uint32_t ClockTrayUnsetProperty(FrameworkElement clock,
                               DependencyProperty property, uint32_t bit) {
    auto local = clock.ReadLocalValue(property);
    bool unset = local == DependencyProperty::UnsetValue();
    if (clock.GetBindingExpression(property) ||
        (!unset && !local.try_as<winrt::Windows::Foundation::IReference<T>>())) {
        throw winrt::hresult_error(E_NOTIMPL);
    }
    return unset ? bit : 0;
}

template <typename T>
void SetClockTrayProperty(FrameworkElement clock, DependencyProperty property,
                         T value, uint32_t bit, uint64_t generation) {
    auto parent = Media::VisualTreeHelper::GetParent(clock);
    if ((parent && parent != g_clockTrayPlacement.decoration) ||
        (!parent && g_clockTrayPlacement.detachedClock != clock)) {
        throw winrt::hresult_error(E_ABORT);
    }
    // Recheck binding ownership, but retain the pre-detach local-value snapshot.
    ClockTrayUnsetProperty<T>(clock, property, bit);
    g_clockTrayPlacement.changedProperties |= bit;
    ApplyClockTrayStep(generation, [&] {
        if (bit == 64) g_clockTrayCounters.marginWrites++;
        clock.SetValue(property, winrt::box_value(value));
    });
}

template <typename T>
void RestoreClockTrayProperty(FrameworkElement clock, Controls::Panel parent,
                             DependencyProperty property, T original,
                             T applied, uint32_t bit) {
    if (!(g_clockTrayPlacement.changedProperties & bit)) return;
    if (Media::VisualTreeHelper::GetParent(clock) != parent) {
        throw winrt::hresult_error(E_ABORT);
    }
    auto local = clock.ReadLocalValue(property);
    auto value = local.try_as<winrt::Windows::Foundation::IReference<T>>();
    // A binding, cleared local value or a different value belongs to its writer.
    if (!clock.GetBindingExpression(property) && value &&
        value.Value() == applied) {
        if (bit == 64) g_clockTrayCounters.marginWrites++;
        if (g_clockTrayPlacement.unsetProperties & bit) {
            clock.ClearValue(property);
        } else {
            clock.SetValue(property, winrt::box_value(original));
        }
    }
    g_clockTrayPlacement.changedProperties &= ~bit;
}
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
    FrameworkElement element) {
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
    }
    if (elements.empty()) {
        return std::nullopt;
    }
    return elements;
}

std::optional<TaskbarElementSample> CollectTaskbarElements(
    Controls::Grid root, FrameworkElement repeater) {
    if (!root || !repeater || !IsTaskbarRepeater(repeater, root)) {
        return std::nullopt;
    }
    TaskbarElementSample sample;
    sample.repeaterIdentity =
        reinterpret_cast<uintptr_t>(winrt::get_abi(repeater));
    sample.root = root;
    sample.repeater = repeater;
    auto elements = GetRealizedTaskbarElements(repeater);
    if (!elements) {
        return std::nullopt;
    }
    sample.elements = std::move(*elements);
    return sample;
}

bool HasLayoutOnlyTransform(FrameworkElement element, FrameworkElement parent) {
    auto transform = element.TransformToVisual(parent);
    auto origin = transform.TransformPoint({0, 0});
    auto xAxis = transform.TransformPoint({1, 0});
    auto yAxis = transform.TransformPoint({0, 1});
    auto offset = element.ActualOffset();
    return std::isfinite(origin.X) && std::isfinite(origin.Y) &&
           std::isfinite(offset.x) && std::isfinite(offset.y) &&
           std::isfinite(xAxis.X) && std::isfinite(xAxis.Y) &&
           std::isfinite(yAxis.X) && std::isfinite(yAxis.Y) &&
           std::abs(origin.X - offset.x) <= kLayoutTolerance &&
           std::abs(origin.Y - offset.y) <= kLayoutTolerance &&
           std::abs(xAxis.X - origin.X - 1) <= 0.0001 &&
           std::abs(xAxis.Y - origin.Y) <= 0.0001 &&
           std::abs(yAxis.X - origin.X) <= 0.0001 &&
           std::abs(yAxis.Y - origin.Y - 1) <= 0.0001;
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
        if (!HasLayoutOnlyTransform(ancestor, next)) {
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

std::optional<HorizontalBounds> TryGetRepeaterChildBounds(
    FrameworkElement child, FrameworkElement repeater, Controls::Grid root,
    std::optional<double> repeaterOriginX, double width) {
    if (!child || !root || !child.IsLoaded() || !root.XamlRoot() ||
        child.XamlRoot() != root.XamlRoot() ||
        child.Visibility() != Visibility::Visible ||
        !std::isfinite(width) || width <= 0 ||
        !std::isfinite(child.ActualHeight()) || child.ActualHeight() <= 0) {
        return std::nullopt;
    }
    if (Media::VisualTreeHelper::GetParent(child) != repeater) {
        return TryGetLayoutBounds(child, root, width);
    }
    if (!repeaterOriginX) return std::nullopt;
    auto offset = child.ActualOffset();
    if (!std::isfinite(offset.x) || !std::isfinite(offset.y)) {
        return std::nullopt;
    }
    // As in the full path, ignore the child's own Dock transform.
    HorizontalBounds bounds{*repeaterOriginX + offset.x,
                            *repeaterOriginX + offset.x + width};
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


std::optional<LeftHostLayoutSnapshot> ReadLeftHostLayout(
    Controls::Grid root, FrameworkElement repeater, Controls::Grid host,
    std::optional<double> prefixOrigin = std::nullopt,
    uintptr_t spacingTarget = 0, double spacingIncrement = 0,
    const TaskbarElementSample* sample = nullptr) {
    if (!IsTaskbarRepeater(repeater, root) || !root.IsLoaded() ||
        !std::isfinite(root.ActualWidth()) || root.ActualWidth() <= 0) {
        return std::nullopt;
    }
    auto repeaterBounds = TryGetLayoutBounds(repeater, root,
                                             repeater.ActualWidth());
    if (!repeaterBounds) {
        return std::nullopt;
    }
    std::optional<TaskbarElementSample> fallbackSample;
    if (!sample) {
        fallbackSample = CollectTaskbarElements(
            root, repeater);
        if (!fallbackSample) {
            return std::nullopt;
        }
        sample = &*fallbackSample;
    }
    if (sample->repeaterIdentity !=
            reinterpret_cast<uintptr_t>(winrt::get_abi(repeater)) ||
        winrt::get_abi(sample->root) != winrt::get_abi(root)) {
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
    {
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
    // The repeater bounds already validated its parent-to-root chain. Validate
    // the repeater's own hop once; reuse this origin only in this read-only pass.
    std::optional<double> repeaterOriginX;
    auto repeaterParent = Media::VisualTreeHelper::GetParent(repeater)
                              .try_as<FrameworkElement>();
    if (repeaterParent && HasLayoutOnlyTransform(repeater, repeaterParent)) {
        repeaterOriginX = repeater.TransformToVisual(root).TransformPoint({0, 0}).X;
    }
    for (auto child : sample->elements) {
        auto identity = reinterpret_cast<uintptr_t>(winrt::get_abi(child));
        auto kind = IdentifySystemButton(child);
        if (kind == SystemButtonKind::None) {
            auto className = winrt::get_class_name(child);
            bool overflow = className == L"Taskbar.OverflowToggleButton" &&
                child.Name() == L"OverflowButton" &&
                Automation::AutomationProperties::GetAutomationId(child) == L"OverflowButton";
            if (className == L"Taskbar.TaskListButton" || overflow) {
                auto bounds = TryGetRepeaterChildBounds(
                    child, repeater, root, repeaterOriginX, child.ActualWidth());
                if (bounds && IntersectsViewport(*bounds, {0, snapshot.rootWidth})) {
                    snapshot.flowElements.push_back({identity, *bounds, overflow});
                }
            } else {
                auto bounds = TryGetRepeaterChildBounds(
                    child, repeater, root, repeaterOriginX, child.ActualWidth());
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
            auto bounds = TryGetRepeaterChildBounds(
                child, repeater, root, repeaterOriginX, width);
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

// BEGIN CLOCK TRAY LAYOUT (P2)
//
// P2 scope: one container with a stable name, the untouched native clock
// inside it, and a read-only mirror of the native tray background. No app-row
// pushing, no viewport reservation, no theme table.
// R02: the clock belongs at the screen's left edge unless a real left-pinned
// system-button cluster sits there. A centered dock/pill carries the same
// buttons too, but its left edge is not the screen edge, so anchoring inside
// the pill would land on top of the app row for no reason.
constexpr double kTrayScreenEdgeInset = 12.0;

double ResolveClockTrayOffset(const LeftHostLayoutSnapshot& layout,
                              double screenLeft) {
    auto buttons = layout.buttons;
    double origin = screenLeft;
    for (const auto& button : buttons) {
        if (button.visible && button.kind == SystemButtonKind::Start &&
            button.bounds.left >= screenLeft - kLayoutTolerance &&
            button.bounds.left <= screenLeft + kTrayScreenEdgeInset) {
            origin = button.bounds.left;
        }
    }
    IdentifyFixedSystemPrefix(buttons, layout.centered, origin, 0, 0);
    std::optional<double> right;
    for (auto& button : buttons) {
        if (button.kind == SystemButtonKind::Widgets) {
            button.fixed = button.visible && std::abs(button.bounds.left -
                screenLeft - button.marginLeft) <= kLayoutTolerance;
        }
        if (!button.visible || !button.fixed) continue;
        double edge = button.bounds.right;
        // Include the current visible extent of the fixed button, not just
        // its layout cell (themes may expand the button outside that cell).
        for (const auto& item : layout.flowElements) {
            if (item.identity == button.identity) edge = std::max(edge, item.bounds.right);
        }
        right = right ? std::max(*right, edge) : edge;
    }
    return right ? *right + kTaskbarLayoutGap : screenLeft;
}

ClockTrayReason CheckClockTrayHorizontalBounds(
    const LeftHostLayoutSnapshot& layout, double screenLeft,
    const HorizontalBounds& bounds) {
    if (layout.unknownVisible || !IsValidHorizontalBounds(bounds) ||
        !std::isfinite(screenLeft) || !std::isfinite(layout.rightBoundary)) {
        return ClockTrayReason::UnsupportedLayout;
    }
    if (bounds.left < screenLeft - kLayoutTolerance ||
        bounds.right + kTaskbarLayoutGap > layout.rightBoundary + kLayoutTolerance) {
        return ClockTrayReason::NoSpace;
    }
    for (const auto& item : layout.flowElements) {
        if (!IsValidHorizontalBounds(item.bounds)) return ClockTrayReason::UnsupportedLayout;
        if (bounds.left < item.bounds.right + kTaskbarLayoutGap - kLayoutTolerance &&
            item.bounds.left < bounds.right + kTaskbarLayoutGap - kLayoutTolerance) {
            return ClockTrayReason::NoSpace;
        }
    }
    return ClockTrayReason::None;
}

ClockTrayPlan ComputeClockTrayPlan(const LeftHostLayoutSnapshot& layout,
                                  double screenLeft, double width,
                                  Thickness margin, Thickness border,
                                  Thickness padding) {
    ClockTrayPlan plan;
    auto valid = [](Thickness value, bool nonnegative) {
        for (double part : {value.Left, value.Top, value.Right, value.Bottom}) {
            if (!std::isfinite(part) || (nonnegative && part < 0)) return false;
        }
        return true;
    };
    if (!std::isfinite(width) || width <= 0 || !valid(margin, false) ||
        !valid(border, true) || !valid(padding, true)) {
        plan.reason = ClockTrayReason::UnsupportedAppearance;
        return plan;
    }
    double visibleWidth = width + border.Left + border.Right + padding.Left + padding.Right;
    plan.layoutLeft = ResolveClockTrayOffset(layout, screenLeft);
    plan.visualShift = std::max(0.0, -margin.Left);
    plan.hostLeft = plan.layoutLeft + plan.visualShift;
    plan.hostWidth = visibleWidth + margin.Left + margin.Right;
    plan.visible = {plan.hostLeft + margin.Left, plan.hostLeft + margin.Left + visibleWidth};
    if (!std::isfinite(plan.hostWidth) || plan.hostWidth <= 0 ||
        !std::isfinite(plan.hostLeft)) {
        plan.reason = ClockTrayReason::UnsupportedAppearance;
    } else {
        plan.reason = CheckClockTrayHorizontalBounds(layout, screenLeft, plan.visible);
    }
    return plan;
}

ClockTrayReason CheckClockTrayActualBounds(const LeftHostLayoutSnapshot& layout,
                                         double screenLeft, const ClockTrayPlan& plan,
                                         HorizontalBounds actual) {
    auto reason = CheckClockTrayHorizontalBounds(layout, screenLeft, actual);
    if (reason != ClockTrayReason::None) return reason;
    return std::abs(actual.left - plan.visible.left) > kLayoutTolerance ||
           std::abs(actual.right - plan.visible.right) > kLayoutTolerance
               ? ClockTrayReason::UnsupportedLayout : ClockTrayReason::None;
}

double ComputeClockTrayReservedWidth(bool centered, bool owned,
                                    double before, double current) {
    if (!centered || !owned || !std::isfinite(before) || !std::isfinite(current) ||
        before <= 0 || current < 0) return 0;
    return std::max(0.0, before - current);
}

std::optional<HorizontalBounds> ReadClockTrayVisualBounds(
    FrameworkElement element, Controls::Grid root) {
    if (!element || !element.IsLoaded() || element.Visibility() != Visibility::Visible ||
        !root.XamlRoot() || element.XamlRoot() != root.XamlRoot() ||
        element.ActualWidth() <= 0 || element.ActualHeight() <= 0) return std::nullopt;
    auto bounds = element.TransformToVisual(root).TransformBounds(
        {0, 0, static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight())});
    HorizontalBounds horizontal{bounds.X, bounds.X + bounds.Width};
    return IsValidHorizontalBounds(horizontal) ? std::optional{horizontal} : std::nullopt;
}

struct ClockTrayLayout {
    LeftHostLayoutSnapshot snapshot;
    double screenLeft{};
};

Controls::Panel FindNativeTrayPanel(Controls::Grid root);

std::optional<ClockTrayLayout> ReadClockTrayLayout(Controls::Grid root,
                                                FrameworkElement repeater) {
    auto reject = [](PCWSTR stage) -> std::optional<ClockTrayLayout> {
        Wh_Log(L"[clock-tray] layout unavailable stage=%s", stage);
        return std::nullopt;
    };
    g_clockTrayCounters.layoutReads++;
    auto sample = CollectTaskbarElements(root, repeater);
    if (!sample) return reject(L"repeater-sample");
    auto layout = ReadLeftHostLayout(root, repeater, nullptr, {}, 0, 0, &*sample);
    if (!layout || layout->unknownVisible) return reject(L"layout-or-unknown-item");
    auto content = root.XamlRoot().Content();
    auto transform = root.TransformToVisual(content);
    auto origin = transform.TransformPoint({0, 0});
    auto xAxis = transform.TransformPoint({1, 0});
    auto yAxis = transform.TransformPoint({0, 1});
    RECT rect{};
    if (!std::isfinite(origin.X) || !std::isfinite(origin.Y) ||
        !std::isfinite(xAxis.X) || !std::isfinite(xAxis.Y) ||
        !std::isfinite(yAxis.X) || !std::isfinite(yAxis.Y) ||
        std::abs(xAxis.X - origin.X - 1) > 0.0001 || std::abs(xAxis.Y - origin.Y) > 0.0001 ||
        std::abs(yAxis.X - origin.X) > 0.0001 || std::abs(yAxis.Y - origin.Y - 1) > 0.0001 ||
        !GetClientRect(FindWindowW(L"Shell_TrayWnd", nullptr), &rect)) return reject(L"root-transform-or-window");
    double scale = root.XamlRoot().RasterizationScale();
    if (!std::isfinite(scale) || scale <= 0) return reject(L"scale");
    double screenLeft = -origin.X;
    double screenRight = (rect.right - rect.left) / scale - origin.X;
    // The native root must cover the taskbar window. Do not pretend an
    // off-root child is visible through a theme's narrowed/clipped root.
    if (!std::isfinite(screenRight) || screenLeft < -kLayoutTolerance ||
        screenRight > root.ActualWidth() + kLayoutTolerance || root.Clip()) {
        Wh_Log(L"[clock-tray] root bounds origin=%.3f width=%.3f screen=%.3f..%.3f clip=%d",
               origin.X, root.ActualWidth(), screenLeft, screenRight, root.Clip() ? 1 : 0);
        return reject(L"root-coverage");
    }
    layout->rightBoundary = screenRight;
    FrameworkElement tray{nullptr};
    for (auto child : root.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && winrt::get_class_name(element) == L"SystemTray.SystemTrayFrame") {
            tray = element;
            break;
        }
    }
    auto trayBounds = ReadClockTrayVisualBounds(tray, root);
    if (!trayBounds) return reject(L"native-tray-bounds");
    layout->rightBoundary = std::min(layout->rightBoundary, trayBounds->left);
    if (auto panel = FindNativeTrayPanel(root)) {
        auto panelBounds = ReadClockTrayVisualBounds(panel, root);
        if (!panelBounds) return reject(L"native-panel-bounds");
        layout->rightBoundary = std::min(layout->rightBoundary, panelBounds->left);
    }
    // Check every realized visible button, including fixed system buttons and
    // trailing Widgets. A failed measurement is not an empty slot.
    layout->flowElements.clear();
    for (const auto& child : sample->elements) {
        if (child.Visibility() != Visibility::Visible) continue;
        auto bounds = ReadClockTrayVisualBounds(child, root);
        if (!bounds) {
            Wh_Log(L"[clock-tray] visible item unavailable class=%s name=%s loaded=%d size=%.3fx%.3f",
                   winrt::get_class_name(child).c_str(), child.Name().c_str(), child.IsLoaded() ? 1 : 0,
                   child.ActualWidth(), child.ActualHeight());
            return reject(L"visible-item-bounds");
        }
        if (bounds->right > screenLeft && bounds->left < screenRight) {
            layout->flowElements.push_back({reinterpret_cast<uintptr_t>(winrt::get_abi(child)), *bounds});
        }
    }
    return ClockTrayLayout{*layout, screenLeft};
}

Controls::Panel FindNativeTrayPanel(Controls::Grid root) {
    if (!root) {
        return nullptr;
    }
    for (auto child : root.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element ||
            winrt::get_class_name(element) != L"SystemTray.SystemTrayFrame") {
            continue;
        }
        if (auto grid = FindDirectChildByName(element, L"SystemTrayFrameGrid")) {
            if (auto panel = grid.try_as<Controls::Panel>()) {
                return panel;
            }
        }
        return element.try_as<Controls::Panel>();
    }
    return nullptr;
}

// Snapshot of the appearance values the native tray actually exposes. Themes
// differ, so nothing is assumed: whatever cannot be read stays at its default.
struct TrayAppearance {
    Media::Brush background{nullptr};
    Media::Brush borderBrush{nullptr};
    CornerRadius cornerRadius{};
    Thickness borderThickness{};
    Thickness margin{};
    Thickness padding{};
    bool hasCornerRadius{};
    bool hasBorder{};
    bool hasMargin{};
    bool hasPadding{};
};

TrayAppearance ReadNativeTrayAppearance(Controls::Panel panel) {
    TrayAppearance appearance{};
    if (!panel) {
        return appearance;
    }
    appearance.background = panel.Background();
    // The native visual box is (cell + Margin - BorderThickness). Themes use
    // both signs of Margin, so mirroring only the border is not enough.
    appearance.margin = panel.Margin();
    appearance.hasMargin = true;
    if (auto grid = panel.try_as<Controls::Grid>()) {
        appearance.cornerRadius = grid.CornerRadius();
        appearance.borderThickness = grid.BorderThickness();
        appearance.borderBrush = grid.BorderBrush();
        appearance.padding = grid.Padding();
        appearance.hasCornerRadius = true;
        appearance.hasBorder = true;
        appearance.hasPadding = true;
    } else if (auto stack = panel.try_as<Controls::StackPanel>()) {
        appearance.cornerRadius = stack.CornerRadius();
        appearance.borderThickness = stack.BorderThickness();
        appearance.borderBrush = stack.BorderBrush();
        appearance.padding = stack.Padding();
        appearance.hasCornerRadius = true;
        appearance.hasBorder = true;
        appearance.hasPadding = true;
    } else if (auto border = panel.try_as<Controls::Border>()) {
        appearance.cornerRadius = border.CornerRadius();
        appearance.borderThickness = border.BorderThickness();
        appearance.borderBrush = border.BorderBrush();
        appearance.padding = border.Padding();
        appearance.hasCornerRadius = true;
        appearance.hasBorder = true;
        appearance.hasPadding = true;
    }
    return appearance;
}

bool TrayThicknessIsFinite(const Thickness& value) {
    return std::isfinite(value.Left) && std::isfinite(value.Top) &&
           std::isfinite(value.Right) && std::isfinite(value.Bottom);
}

TrayAppearance LeftClockTrayAppearance(TrayAppearance appearance) {
    // The source is at the right screen edge. Face its outer edge left while
    // leaving the native button, text and shared brush in their original direction.
    std::swap(appearance.cornerRadius.TopLeft, appearance.cornerRadius.TopRight);
    std::swap(appearance.cornerRadius.BottomLeft, appearance.cornerRadius.BottomRight);
    for (auto value : {&appearance.margin, &appearance.borderThickness, &appearance.padding}) {
        std::swap(value->Left, value->Right);
    }
    return appearance;
}

// Padding moves content, not the box, so a negative component cannot be
// reproduced by shifting. Clamp it and keep the usable parts.
Thickness ClampTrayPadding(const Thickness& value) {
    return Thickness{std::max(0.0, value.Left), std::max(0.0, value.Top),
                     std::max(0.0, value.Right), std::max(0.0, value.Bottom)};
}

// A negative left Margin is how themes expand the pill past its own cell. The
// native tray never sits at the screen edge, so it is never clipped there; our
// clock does. Instead of clipping, move the whole host right by the overflow,
// so the pill keeps its full size and simply starts at the screen edge.
void ApplyClockTrayHostOffset() {
    auto generation = g_clockTray.generation;
    auto host = g_clockTrayPlacement.host;
    if (!host || !g_clockTrayPlacement.placed) {
        return;
    }
    double left = g_clockTrayPlacement.plan.hostLeft;
    auto margin = host.Margin();
    if (std::abs(margin.Left - left) > kLayoutTolerance) {
        margin.Left = left;
        g_clockTrayCounters.marginWrites++;
        ApplyClockTrayStep(generation, [&] { host.Margin(margin); });
    }
}

void ApplyClockTrayAppearance() {
    auto generation = g_clockTray.generation;
    if (!ClockTrayWorkIsCurrent(generation)) return;
    auto decoration = g_clockTrayPlacement.decoration;
    auto source = g_clockTrayPlacement.appearanceSource.get();
    if (!decoration || !source) {
        return;
    }
    auto appearance = LeftClockTrayAppearance(ReadNativeTrayAppearance(source));
    // One bad property must not abort the whole placement: each step is
    // independent and reports its own failure once.
    auto tryStep = [generation](PCWSTR name, auto&& step) {
        try {
            ApplyClockTrayStep(generation, step);
        } catch (...) {
            Wh_Log(L"[clock-tray] appearance step=%s failed hr=%08X", name,
                   winrt::to_hresult());
        }
    };
    tryStep(L"background",
            [&] { decoration.Background(appearance.background); });
    if (appearance.hasCornerRadius) {
        tryStep(L"radius", [&] {
            decoration.CornerRadius(appearance.cornerRadius);
        });
    }
    if (appearance.hasBorder) {
        tryStep(L"border", [&] {
            ApplyClockTrayStep(generation, [&] {
                decoration.BorderThickness(appearance.borderThickness);
            });
            ApplyClockTrayStep(generation, [&] {
                decoration.BorderBrush(appearance.borderBrush);
            });
        });
    }
    if (appearance.hasPadding) {
        auto padding = ClampTrayPadding(appearance.padding);
        if (!TrayThicknessIsFinite(appearance.padding) ||
            padding.Left != appearance.padding.Left ||
            padding.Top != appearance.padding.Top ||
            padding.Right != appearance.padding.Right ||
            padding.Bottom != appearance.padding.Bottom) {
            // XAML rejects negative Padding outright; keep the positive parts.
            Wh_Log(L"[clock-tray] appearance padding clamped: from "
                   L"%.1f,%.1f,%.1f,%.1f to %.1f,%.1f,%.1f,%.1f",
                   appearance.padding.Left, appearance.padding.Top,
                   appearance.padding.Right, appearance.padding.Bottom,
                   padding.Left, padding.Top, padding.Right, padding.Bottom);
        }
        tryStep(L"padding", [&] { decoration.Padding(padding); });
        appearance.padding = padding;
    }
    if (appearance.hasMargin) {
        tryStep(L"margin", [&] {
            auto host = g_clockTrayPlacement.host;
            double cellHeight = host ? host.ActualHeight() : 0;
            double cellWidth = host ? host.ActualWidth() : 0;
            double fillHeight = cellHeight - appearance.margin.Top -
                                appearance.margin.Bottom -
                                appearance.borderThickness.Top -
                                appearance.borderThickness.Bottom;
            double fillWidth = cellWidth - appearance.margin.Left -
                               appearance.margin.Right -
                               appearance.borderThickness.Left -
                               appearance.borderThickness.Right;
            if (cellHeight > 0 && (fillHeight < 1 || fillWidth < 1)) {
                // A theme with extreme insets must not collapse the layer.
                ApplyClockTrayStep(generation, [&] {
                    decoration.Margin(Thickness{0, 0, 0, 0});
                });
                Wh_Log(L"[clock-tray] appearance margin skipped: cell=%.1fx%.1f "
                       L"margin=%.1f,%.1f,%.1f,%.1f",
                       cellWidth, cellHeight, appearance.margin.Left,
                       appearance.margin.Top, appearance.margin.Right,
                       appearance.margin.Bottom);
            } else {
                ApplyClockTrayStep(generation, [&] {
                    decoration.Margin(appearance.margin);
                });
            }
            // Geometry uses the values that were actually applied, after this
            // pass, rather than mixing the requested appearance with old sizes.
            g_clockTrayPlacement.metricsDirty = true;
        });
    }
}

bool ClearClockTrayAppearanceHooks() {
    // Take the list out before calling XAML; an unregister may reenter us.
    auto hooks = std::move(g_clockTrayPlacement.appearanceHooks);
    g_clockTrayPlacement.appearanceHooks.clear();
    for (const auto& hook : hooks) {
        try {
            if (auto owner = hook.owner.get()) {
                if (hook.property) {
                    owner.UnregisterPropertyChangedCallback(hook.property,
                                                            hook.token);
                }
            }
        } catch (...) {
            g_clockTrayPlacement.appearanceHooks.push_back(hook);
            g_clockTrayCleanupPending = true;
            Wh_Log(L"[clock-tray] appearance removal failed: %08X",
                   winrt::to_hresult());
        }
    }
    return g_clockTrayPlacement.appearanceHooks.empty();
}

void AddClockTrayAppearanceHook(DependencyObject owner,
                                DependencyProperty property) {
    if (!owner || !property) {
        return;
    }
    auto weakDecoration = winrt::make_weak(g_clockTrayPlacement.decoration);
    auto generation = g_clockTray.generation;
    if (!ClockTrayWorkIsCurrent(generation)) {
        throw winrt::hresult_error(E_ABORT);
    }
    int64_t token = owner.RegisterPropertyChangedCallback(
        property,
        [weakDecoration, generation](DependencyObject const&,
                                     DependencyProperty const&) {
            try {
                if (!g_clockTrayWorkActive && ClockTrayWorkIsCurrent(generation) &&
                    weakDecoration.get() == g_clockTrayPlacement.decoration) {
                    ClockTrayWorkScope work;
                    ApplyClockTrayAppearance();
                }
            } catch (...) {
                Wh_Log(L"[clock-tray] appearance update failed: %08X",
                       winrt::to_hresult());
            }
        });
    g_clockTrayPlacement.appearanceHooks.push_back({owner, property, token});
    g_clockTrayCleanupPending = true;
    if (!ClockTrayWorkIsCurrent(generation)) {
        ClearClockTrayAppearanceHooks();
        throw winrt::hresult_error(E_ABORT);
    }
}

// One-way mirror: read the source values, then re-read them when the source
// properties change. Brushes are shareable, so no copy is invented. The source
// is read-only: this never writes to the native tray.
void MirrorNativeTrayAppearance(Controls::Grid root) {
    auto& placement = g_clockTrayPlacement;
    auto panel = FindNativeTrayPanel(root);
    if (!panel) {
        Wh_Log(L"[clock-tray] appearance source not found");
        return;
    }
    if (placement.appearanceSource.get() != panel) {
        if (!ClearClockTrayAppearanceHooks()) {
            throw winrt::hresult_error(E_FAIL);
        }
        placement.appearanceSource = panel;
        AddClockTrayAppearanceHook(panel, Controls::Panel::BackgroundProperty());
        AddClockTrayAppearanceHook(panel, FrameworkElement::MarginProperty());
        if (auto grid = panel.try_as<Controls::Grid>()) {
            AddClockTrayAppearanceHook(grid, Controls::Grid::CornerRadiusProperty());
            AddClockTrayAppearanceHook(grid, Controls::Grid::BorderThicknessProperty());
            AddClockTrayAppearanceHook(grid, Controls::Grid::PaddingProperty());
        } else if (auto stack = panel.try_as<Controls::StackPanel>()) {
            AddClockTrayAppearanceHook(stack,
                                       Controls::StackPanel::CornerRadiusProperty());
            AddClockTrayAppearanceHook(
                stack, Controls::StackPanel::BorderThicknessProperty());
            AddClockTrayAppearanceHook(stack,
                                       Controls::StackPanel::PaddingProperty());
        } else if (auto border = panel.try_as<Controls::Border>()) {
            AddClockTrayAppearanceHook(border,
                                       Controls::Border::CornerRadiusProperty());
            AddClockTrayAppearanceHook(border,
                                       Controls::Border::BorderThicknessProperty());
            AddClockTrayAppearanceHook(border,
                                       Controls::Border::PaddingProperty());
        }
    }
    ApplyClockTrayAppearance();
    auto appearance = ReadNativeTrayAppearance(panel);
    Wh_Log(L"[clock-tray] appearance source=%s hasBackground=%d "
           L"hasRadius=%d radius=%.1f,%.1f,%.1f,%.1f border=%.1f,%.1f,%.1f,%.1f "
           L"margin=%.1f,%.1f,%.1f,%.1f padding=%.1f,%.1f,%.1f,%.1f",
           winrt::get_class_name(panel).c_str(),
           appearance.background ? 1 : 0, appearance.hasCornerRadius ? 1 : 0,
           appearance.cornerRadius.TopLeft, appearance.cornerRadius.TopRight,
           appearance.cornerRadius.BottomRight,
           appearance.cornerRadius.BottomLeft,
           appearance.borderThickness.Left, appearance.borderThickness.Top,
           appearance.borderThickness.Right,
           appearance.borderThickness.Bottom,
           appearance.margin.Left, appearance.margin.Top,
           appearance.margin.Right, appearance.margin.Bottom,
           appearance.padding.Left, appearance.padding.Top,
           appearance.padding.Right, appearance.padding.Bottom);
}

// BEGIN CLOCK TRAY METRICS (P3)
unsigned ClockTrayExplicitLines(std::wstring_view text) {
    unsigned lines = 1;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == L'\r' || text[i] == L'\n' ||
            text[i] == L'\u2028' || text[i] == L'\u2029') {
            if (++lines == 3) return lines;
            if (text[i] == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') i++;
        }
    }
    return lines;
}

unsigned ClockTrayMeasuredLines(double full, double one, double two) {
    // Compare layouts of the SAME text, not height / FontSize. Font fallback,
    // explicit LineHeight and wrapped lines need not have the nominal height.
    constexpr double epsilon = 0.01;
    if (!std::isfinite(full) || !std::isfinite(one) || !std::isfinite(two) ||
        one < 1 || two < one - epsilon || full < two - epsilon) return 0;
    if (std::abs(full - one) <= epsilon) return 1;
    if (std::abs(full - two) <= epsilon) return 2;
    return 3;
}

ClockTrayReason CheckClockTrayMetrics(const ClockTrayMetrics& metrics,
                                     double windowHeight) {
    if (metrics.lines >= 3) return ClockTrayReason::TooManyLines;
    if (!metrics.lines || !std::isfinite(metrics.requiredWidth) ||
        !std::isfinite(metrics.requiredHeight) ||
        !std::isfinite(windowHeight) || metrics.requiredWidth <= 0 ||
        metrics.requiredHeight <= 0 || windowHeight <= 0) {
        return ClockTrayReason::UnknownMetrics;
    }
    // Theme insets are deliberately NOT subtracted from windowHeight.
    // One/two lines clipped only by a short themed content box stay on the left.
    return metrics.requiredHeight > windowHeight + kLayoutTolerance
               ? ClockTrayReason::TooTall : ClockTrayReason::None;
}

ClockTrayReason CheckClockTrayVerticalBounds(double top, double height,
                                            double windowHeight) {
    if (!std::isfinite(top) || !std::isfinite(height) ||
        !std::isfinite(windowHeight) || height <= 0 || windowHeight <= 0) {
        return ClockTrayReason::UnknownMetrics;
    }
    return top < -kLayoutTolerance || top + height > windowHeight + kLayoutTolerance
               ? ClockTrayReason::TooTall : ClockTrayReason::None;
}

double ClockTrayWindowHeight(Controls::Grid root) {
    RECT rect{};
    auto xamlRoot = root.XamlRoot();
    HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!xamlRoot || !window || !GetClientRect(window, &rect)) return 0;
    double scale = xamlRoot.RasterizationScale();
    return scale > 0 ? (rect.bottom - rect.top) / scale : 0;
}

Controls::StackPanel FindClockTrayTextStack(FrameworkElement clock) {
    auto time = FindDescendantByName(clock, L"TimeInnerTextBlock");
    auto date = FindDescendantByName(clock, L"DateInnerTextBlock");
    if (!time || !date) return nullptr;
    auto stack = Media::VisualTreeHelper::GetParent(time).try_as<Controls::StackPanel>();
    if (!stack || Media::VisualTreeHelper::GetParent(date) != stack ||
        stack.Orientation() != Controls::Orientation::Vertical ||
        stack.Children().Size() != 2) return nullptr;
    return stack;
}

std::optional<double> ResolveClockTrayChrome(bool placed, double saved,
                                            double clockWidth, double stackWidth) {
    double value = placed ? saved : clockWidth - stackWidth;
    if (!std::isfinite(value) || value < -kLayoutTolerance) return std::nullopt;
    return std::max(0.0, value);
}

bool ClockTrayCanWaitForArrange(bool placing, uint64_t elapsed) {
    return placing && elapsed < 500;
}

bool WaitForClockTrayArrange() {
    auto& placement = g_clockTrayPlacement;
    if (!ClockTrayCanWaitForArrange(g_clockTray.state == ClockTrayState::Placing,
            GetTickCount64() - placement.arrangeWaitStart)) return false;
    placement.metricsDirty = true;
    // Loaded and the one-shot deadline drive progress. Invalidating arrange
    // here spins through LayoutUpdated before XAML can deliver Loaded.
    return true;
}

bool StopClockTrayArrangeDeadline() {
    auto& placement = g_clockTrayPlacement;
    try {
        if (placement.arrangeDeadline) {
            placement.arrangeDeadline.Stop();
            if (placement.arrangeDeadlineToken) {
                auto events = placement.arrangeDeadline.as<IDispatcherTimer>();
                auto abi = reinterpret_cast<winrt::impl::abi_t<IDispatcherTimer>*>(winrt::get_abi(events));
                winrt::check_hresult(abi->remove_Tick(*placement.arrangeDeadlineToken));
            }
        }
        placement.arrangeDeadlineToken.reset();
        placement.arrangeDeadline = nullptr;
        return true;
    } catch (...) {
        g_clockTrayCleanupPending = true;
        Wh_Log(L"[clock-tray] arrange deadline removal failed: %08X", winrt::to_hresult());
        return false;
    }
}

void StartClockTrayArrangeDeadline(bool nativeRetry = false) {
    if (!StopClockTrayArrangeDeadline()) throw winrt::hresult_error(E_FAIL);
    auto generation = g_clockTray.generation;
    auto& placement = g_clockTrayPlacement;
    placement.arrangeWaitStart = GetTickCount64();
    placement.arrangeDeadline = DispatcherTimer{};
    placement.arrangeDeadline.Interval(winrt::Windows::Foundation::TimeSpan{nativeRetry ? 1000000 : 5000000});
    placement.arrangeDeadlineToken = placement.arrangeDeadline.Tick(
        [generation](winrt::Windows::Foundation::IInspectable const& sender,
                     winrt::Windows::Foundation::IInspectable const&) {
            try {
                sender.as<DispatcherTimer>().Stop();
                if (!ClockTrayWorkIsCurrent(generation)) return;
                g_clockTrayPlacement.metricsDirty = true;
                if (g_clockTrayWorkActive) return;
                if (g_clockTrayPlacement.placed) {
                    UpdateClockTrayPlacement();
                } else {
                    auto clock = g_clockTrayPlacement.clock.get();
                    if (clock && !TryAttachClockTray(clock)) EnsureClockTrayRetry(clock);
                }
            } catch (...) {
                if (ClockTrayWorkIsCurrent(generation)) RestoreClockTray(ClockTrayReason::UnknownMetrics);
            }
        });
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [&] { placement.arrangeDeadline.Start(); });
}

ClockTrayMetrics ReadClockTrayMetrics(FrameworkElement clock) {
    g_clockTrayCounters.metricReads++;
    ClockTrayMetrics result;
    auto unknown = [](PCWSTR stage) -> ClockTrayMetrics {
        if (g_clockTray.reason != ClockTrayReason::UnknownMetrics) {
            Wh_Log(L"[clock-tray] metrics unavailable stage=%s", stage);
        }
        return {};
    };
    auto stack = g_clockTrayPlacement.clock.get() == clock
        ? g_clockTrayPlacement.textStack.get() : nullptr;
    if (!stack) stack = FindClockTrayTextStack(clock);
    if (!stack || !stack.IsLoaded() || stack.Visibility() != Visibility::Visible ||
        clock.ActualWidth() <= 0 || stack.ActualWidth() <= 0) {
        if (g_clockTray.reason != ClockTrayReason::UnknownMetrics) {
            auto time = FindDescendantByName(clock, L"TimeInnerTextBlock");
            auto parent = time ? Media::VisualTreeHelper::GetParent(time) : nullptr;
            Wh_Log(L"[clock-tray] text stack parent=%s children=%d clockWidth=%.3f stackWidth=%.3f",
                   parent ? winrt::get_class_name(parent).c_str() : L"none",
                   parent ? Media::VisualTreeHelper::GetChildrenCount(parent) : -1,
                   clock.ActualWidth(), stack ? stack.ActualWidth() : -1.0);
        }
        return unknown(L"text-stack");
    }
    // Once hosted, ActualWidth is constrained by our own border. Reusing it
    // to derive chrome feeds rounding/clipping back into the next width.
    auto chrome = ResolveClockTrayChrome(g_clockTrayPlacement.placed,
        g_clockTrayPlacement.metrics.nativeChromeWidth, clock.ActualWidth(), stack.ActualWidth());
    if (!chrome) return unknown(L"button-chrome");
    auto padding = stack.Padding();
    auto border = stack.BorderThickness();
    double stackWidth = 0;
    double stackHeight = padding.Top + padding.Bottom + border.Top + border.Bottom;
    unsigned blocks = 0;
    for (const auto& child : stack.Children()) {
        auto text = child.try_as<Controls::TextBlock>();
        if (!text) return unknown(L"text-child-type");
        if (text.Visibility() == Visibility::Collapsed) continue;
        // Plain Text is the native/TCC contract. Rich inline formatting needs
        // a different measurer; never silently flatten it and claim accuracy.
        auto inlines = text.Inlines();
        if (inlines.Size() > 1) return unknown(L"multiple-inlines");
        if (inlines.Size() == 1) {
            auto run = inlines.GetAt(0).try_as<Documents::Run>();
            if (!run || run.Text() != text.Text() ||
                run.FontFamily().Source() != text.FontFamily().Source() ||
                run.FontSize() != text.FontSize() || run.FontWeight() != text.FontWeight() ||
                run.FontStyle() != text.FontStyle() || run.FontStretch() != text.FontStretch() ||
                run.CharacterSpacing() != text.CharacterSpacing()) return unknown(L"inline-style");
        }
        unsigned explicitLines = ClockTrayExplicitLines(text.Text());
        if (result.lines + explicitLines >= 3) {
            result.lines = 3;
            return result;
        }
        Controls::TextBlock probe;
        probe.Text(text.Text());
        probe.FontFamily(text.FontFamily());
        probe.FontSize(text.FontSize());
        probe.FontWeight(text.FontWeight());
        probe.FontStyle(text.FontStyle());
        probe.FontStretch(text.FontStretch());
        probe.CharacterSpacing(text.CharacterSpacing());
        probe.LineHeight(text.LineHeight());
        probe.LineStackingStrategy(text.LineStackingStrategy());
        probe.TextLineBounds(text.TextLineBounds());
        probe.TextReadingOrder(text.TextReadingOrder());
        probe.TextAlignment(text.TextAlignment());
        probe.FlowDirection(text.FlowDirection());
        probe.Language(text.Language());
        probe.IsTextScaleFactorEnabled(text.IsTextScaleFactorEnabled());
        probe.Padding(text.Padding());
        probe.TextTrimming(TextTrimming::None);
        probe.TextWrapping(TextWrapping::NoWrap);
        // Only the detached probe is measured. Measuring the live element
        // would invalidate its arrange and overwrite its constrained result.
        probe.Measure({kNativeClockMeasureLimit, kNativeClockMeasureLimit});
        auto natural = probe.DesiredSize();
        if (natural.Width >= kNativeClockMeasureLimit ||
            natural.Height >= kNativeClockMeasureLimit) return unknown(L"measure-limit");
        float available = kNativeClockMeasureLimit;
        if (text.TextWrapping() != TextWrapping::NoWrap) {
            available = static_cast<float>(text.ActualWidth());
            if (!std::isfinite(available) || available <= 0) return unknown(L"wrap-width");
            probe.TextWrapping(text.TextWrapping());
        }
        probe.Measure({available, kNativeClockMeasureLimit});
        auto full = probe.DesiredSize();
        if (full.Height >= kNativeClockMeasureLimit) return unknown(L"wrapped-measure-limit");
        probe.MaxLines(1);
        probe.Measure({available, kNativeClockMeasureLimit});
        double one = probe.DesiredSize().Height;
        probe.MaxLines(2);
        probe.Measure({available, kNativeClockMeasureLimit});
        unsigned lines = ClockTrayMeasuredLines(full.Height, one, probe.DesiredSize().Height);
        if (!lines) {
            if (g_clockTray.reason != ClockTrayReason::UnknownMetrics) {
                Wh_Log(L"[clock-tray] text heights full=%.3f one=%.3f two=%.3f name=%s",
                       static_cast<double>(full.Height), one, static_cast<double>(probe.DesiredSize().Height), text.Name().c_str());
            }
            return unknown(L"measured-lines");
        }
        result.lines += std::max(lines, explicitLines);
        if (result.lines >= 3) return result;
        auto margin = text.Margin();
        double width = std::max<double>(full.Width, text.MinWidth());
        if (std::isfinite(text.Width())) width = std::max(width, text.Width());
        // A caller-imposed horizontal cap can still hide complete content.
        // Keep the caller's property intact and report an unsupported input.
        if (width > text.MaxWidth() + kLayoutTolerance ||
            (std::isfinite(text.Width()) && full.Width > text.Width() + kLayoutTolerance)) return unknown(L"text-width-cap");
        stackWidth = std::max(stackWidth, width + margin.Left + margin.Right);
        double height = std::max<double>(full.Height, text.MinHeight());
        if (std::isfinite(text.Height())) height = std::max(height, text.Height());
        stackHeight += height + margin.Top + margin.Bottom;
        blocks++;
    }
    if (!blocks || !std::isfinite(stack.Spacing()) || stack.Spacing() < 0) return unknown(L"empty-or-negative-spacing");
    stackHeight += (blocks - 1) * stack.Spacing();
    stackWidth += padding.Left + padding.Right + border.Left + border.Right;
    stackWidth = std::max(stackWidth, stack.MinWidth());
    if (std::isfinite(stack.Width())) stackWidth = std::max(stackWidth, stack.Width());
    if (stackWidth > stack.MaxWidth() + kLayoutTolerance ||
        (std::isfinite(stack.Width()) && stackWidth > stack.Width() + kLayoutTolerance)) return unknown(L"stack-width-cap");
    result.nativeChromeWidth = *chrome;
    result.requiredWidth = std::max(stackWidth + *chrome, clock.MinWidth());
    if (std::isfinite(clock.Width())) result.requiredWidth = std::max(result.requiredWidth, clock.Width());
    if (result.requiredWidth > clock.MaxWidth() + kLayoutTolerance ||
        (std::isfinite(clock.Width()) && result.requiredWidth > clock.Width() + kLayoutTolerance)) return unknown(L"clock-width-cap");
    result.requiredHeight = std::max({stackHeight, stack.MinHeight(), clock.MinHeight()});
    if (std::isfinite(stack.Height())) result.requiredHeight = std::max(result.requiredHeight, stack.Height());
    if (std::isfinite(clock.Height())) result.requiredHeight = std::max(result.requiredHeight, clock.Height());
    return result;
}

bool ClearClockTrayMetricHooks() {
    auto& placement = g_clockTrayPlacement;
    bool deadlineRemoved = StopClockTrayArrangeDeadline();
    auto hooks = std::move(placement.metricHooks);
    placement.metricHooks.clear();
    for (const auto& hook : hooks) {
        try {
            if (auto owner = hook.owner.get()) {
                owner.UnregisterPropertyChangedCallback(hook.property, hook.token);
            }
        } catch (...) {
            placement.metricHooks.push_back(hook);
        }
    }
    try {
        if (auto clock = placement.clock.get(); clock && placement.metricLayoutToken) {
            auto events = clock.as<IFrameworkElement>();
            auto abi = reinterpret_cast<winrt::impl::abi_t<IFrameworkElement>*>(winrt::get_abi(events));
            winrt::check_hresult(abi->remove_LayoutUpdated(*placement.metricLayoutToken));
        }
        placement.metricLayoutToken.reset();
    } catch (...) {}
    auto revoke = [](auto owner, auto& token, auto remove) {
        try {
            if (owner && token) remove(owner, *token);
            token.reset();
        } catch (...) {}
    };
    revoke(placement.clock.get(), placement.clockLoadedToken,
           [](auto owner, auto token) {
               auto events = owner.template as<IFrameworkElement>();
               auto abi = reinterpret_cast<winrt::impl::abi_t<IFrameworkElement>*>(winrt::get_abi(events));
               winrt::check_hresult(abi->remove_Loaded(token));
           });
    // The projected event removers are noexcept and discard HRESULT. Check the
    // ABI result for these new subscriptions so a failure really retains them.
    revoke(placement.layoutItems.get(), placement.itemsChangedToken,
           [](auto owner, auto token) {
               auto events = owner.template as<Interop::INotifyCollectionChanged>();
               auto abi = reinterpret_cast<winrt::impl::abi_t<Interop::INotifyCollectionChanged>*>(winrt::get_abi(events));
               winrt::check_hresult(abi->remove_CollectionChanged(token));
           });
    revoke(placement.layoutRepeater.get(), placement.elementPreparedToken,
           [](auto owner, auto token) {
               auto events = owner.template as<winrt::Microsoft::UI::Xaml::Controls::IItemsRepeater>();
               auto abi = reinterpret_cast<winrt::impl::abi_t<winrt::Microsoft::UI::Xaml::Controls::IItemsRepeater>*>(winrt::get_abi(events));
               winrt::check_hresult(abi->remove_ElementPrepared(token));
           });
    revoke(placement.layoutRepeater.get(), placement.elementClearingToken,
           [](auto owner, auto token) {
               auto events = owner.template as<winrt::Microsoft::UI::Xaml::Controls::IItemsRepeater>();
               auto abi = reinterpret_cast<winrt::impl::abi_t<winrt::Microsoft::UI::Xaml::Controls::IItemsRepeater>*>(winrt::get_abi(events));
               winrt::check_hresult(abi->remove_ElementClearing(token));
           });
    bool complete = deadlineRemoved && !placement.clockLoadedToken &&
                    placement.metricHooks.empty() && !placement.metricLayoutToken &&
                    !placement.itemsChangedToken && !placement.elementPreparedToken && !placement.elementClearingToken;
    if (complete) {
        placement.layoutItems = {};
        placement.layoutRepeater = {};
        placement.layoutStamp.reset();
    }
    if (!complete) {
        g_clockTrayCleanupPending = true;
        Wh_Log(L"[clock-tray] metrics removal incomplete");
    }
    return complete;
}

ClockTrayLayoutStamp ReadClockTrayLayoutStamp() {
    auto& placement = g_clockTrayPlacement;
    auto root = placement.root.get();
    // Three known direct-child levels; never enumerate taskbar items here.
    auto repeater = FindTaskbarRepeater(root).as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    auto source = repeater.ItemsSourceView();
    auto parent = placement.originalParent.get();
    if (!source || !parent || !parent.IsLoaded() || !root.IsLoaded() ||
        !root.XamlRoot() || parent.XamlRoot() != root.XamlRoot() || !placement.nativeColumn) {
        throw winrt::hresult_error(E_FAIL);
    }
    auto grid = parent.as<Controls::Grid>();
    uint32_t columnIndex = 0;
    if (!grid.ColumnDefinitions().IndexOf(placement.nativeColumn, columnIndex) ||
        static_cast<int>(columnIndex) != placement.originalColumn) throw winrt::hresult_error(E_FAIL);
    auto rootOrigin = root.TransformToVisual(root.XamlRoot().Content()).TransformPoint({0, 0});
    auto repeaterOrigin = repeater.TransformToVisual(root).TransformPoint({0, 0});
    ClockTrayLayoutStamp stamp{
        reinterpret_cast<uintptr_t>(winrt::get_abi(repeater)),
        reinterpret_cast<uintptr_t>(winrt::get_abi(source)), source.Count(), TaskbarUsesCenteredAlignment(),
        placement.nativeColumn.ActualWidth(),
        {rootOrigin.X, root.ActualWidth(), root.ActualHeight(), root.XamlRoot().RasterizationScale(),
         repeaterOrigin.X, repeater.ActualWidth(), repeater.DesiredSize().Width,
         parent.ActualWidth(), parent.ActualOffset().x}};
    if (stamp.count < 0 || !std::isfinite(stamp.nativeColumnWidth)) throw winrt::hresult_error(E_FAIL);
    for (double value : stamp.geometry) {
        if (!std::isfinite(value)) throw winrt::hresult_error(E_FAIL);
    }
    // A Start-left mod can mutate an existing Transform instead of replacing
    // the RenderTransform property. Check only the four cached system buttons.
    for (size_t i = 0; i < placement.systemButtons.size(); i++) {
        if (auto button = placement.systemButtons[i].get();
            button && button.Visibility() == Visibility::Visible) {
            auto bounds = ReadClockTrayVisualBounds(button, root);
            if (!bounds) throw winrt::hresult_error(E_PENDING);
            stamp.systemGeometry[i * 3] = bounds->left;
            stamp.systemGeometry[i * 3 + 1] = bounds->right;
            stamp.systemGeometry[i * 3 + 2] = 1;
        }
    }
    return stamp;
}

void WatchClockTrayMetrics(FrameworkElement clock, uint64_t generation) {
    auto watchGeometry = [generation](DependencyObject owner, DependencyProperty property) {
        auto token = owner.RegisterPropertyChangedCallback(property,
            [generation](auto const&, auto const&) {
                if (ClockTrayWorkIsCurrent(generation)) g_clockTrayPlacement.geometryDirty = true;
            });
        g_clockTrayPlacement.metricHooks.push_back({owner, property, token});
        g_clockTrayCleanupPending = true;
        ApplyClockTrayStep(generation, [] {});
    };
    auto watch = [generation](DependencyObject owner, DependencyProperty property) {
        int64_t token = owner.RegisterPropertyChangedCallback(property,
            [generation](DependencyObject const&, DependencyProperty const&) {
                if (ClockTrayWorkIsCurrent(generation)) g_clockTrayPlacement.metricsDirty = true;
            });
        g_clockTrayPlacement.metricHooks.push_back({owner, property, token});
        g_clockTrayCleanupPending = true;
        ApplyClockTrayStep(generation, [] {});
    };
    auto stack = FindClockTrayTextStack(clock);
    if (!stack) throw winrt::hresult_error(E_FAIL);
    g_clockTrayPlacement.textStack = stack;
    for (FrameworkElement element : {clock, stack.as<FrameworkElement>(),
             stack.Children().GetAt(0).as<FrameworkElement>(),
             stack.Children().GetAt(1).as<FrameworkElement>()}) {
        for (auto property : {UIElement::VisibilityProperty(), FrameworkElement::WidthProperty(),
                 FrameworkElement::HeightProperty(), FrameworkElement::MinWidthProperty(),
                 FrameworkElement::MaxWidthProperty(), FrameworkElement::MinHeightProperty(),
                 FrameworkElement::MaxHeightProperty(), FrameworkElement::MarginProperty()}) watch(element, property);
        if (auto text = element.try_as<Controls::TextBlock>()) {
            for (auto property : {Controls::TextBlock::TextProperty(), Controls::TextBlock::FontFamilyProperty(),
                     Controls::TextBlock::FontSizeProperty(), Controls::TextBlock::FontWeightProperty(),
                     Controls::TextBlock::FontStyleProperty(), Controls::TextBlock::FontStretchProperty(),
                     Controls::TextBlock::CharacterSpacingProperty(), Controls::TextBlock::LineHeightProperty(),
                     Controls::TextBlock::LineStackingStrategyProperty(), Controls::TextBlock::TextWrappingProperty(),
                     Controls::TextBlock::TextLineBoundsProperty(), Controls::TextBlock::PaddingProperty(),
                     Controls::TextBlock::IsTextScaleFactorEnabledProperty(), FrameworkElement::LanguageProperty(),
                     FrameworkElement::FlowDirectionProperty(), Controls::TextBlock::TextReadingOrderProperty()}) watch(text, property);
        }
    }
    watch(stack, Controls::StackPanel::SpacingProperty());
    watch(stack, Controls::StackPanel::PaddingProperty());
    watch(stack, Controls::StackPanel::BorderThicknessProperty());
    watch(stack, Controls::StackPanel::OrientationProperty());
    for (auto property : {Controls::Grid::ColumnProperty(), Controls::Grid::ColumnSpanProperty(),
             Controls::Grid::RowProperty(), Controls::Grid::RowSpanProperty(),
             FrameworkElement::HorizontalAlignmentProperty(), FrameworkElement::VerticalAlignmentProperty(),
             FrameworkElement::MarginProperty()}) watchGeometry(clock, property);
    // The same observers remain useful in the native waiting position.
    auto& placement = g_clockTrayPlacement;
    placement.layoutTrackingDirty = false;
    auto root = placement.root.get();
    auto repeater = FindTaskbarRepeater(root).as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    auto source = repeater.ItemsSourceView();
    if (!source) throw winrt::hresult_error(E_FAIL);
    placement.layoutRepeater = repeater;
    placement.layoutItems = source;
    auto changed = [generation](auto const&, auto const&) {
        if (ClockTrayWorkIsCurrent(generation)) {
            g_clockTrayPlacement.layoutTrackingDirty = true;
            g_clockTrayPlacement.geometryDirty = true;
        }
    };
    placement.itemsChangedToken = source.CollectionChanged(changed);
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [] {});
    placement.elementPreparedToken = repeater.ElementPrepared(changed);
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [] {});
    placement.elementClearingToken = repeater.ElementClearing(changed);
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [] {});
    auto elements = GetRealizedTaskbarElements(repeater);
    if (!elements) throw winrt::hresult_error(E_FAIL);
    placement.systemButtons = {};
    for (auto element : *elements) {
        auto kind = IdentifySystemButton(element);
        if (kind != SystemButtonKind::None) {
            placement.systemButtons[static_cast<size_t>(kind) - 1] = element;
        }
    }
    elements->push_back(root);
    elements->push_back(placement.originalParent.get());
    auto ancestor = repeater.as<FrameworkElement>();
    while (ancestor && ancestor != root) {
        elements->push_back(ancestor);
        ancestor = Media::VisualTreeHelper::GetParent(ancestor).try_as<FrameworkElement>();
    }
    if (auto sourcePanel = FindNativeTrayPanel(root)) elements->push_back(sourcePanel);
    for (auto element : *elements) {
        for (auto property : {FrameworkElement::ActualWidthProperty(), FrameworkElement::ActualHeightProperty(),
                 FrameworkElement::MarginProperty(), UIElement::VisibilityProperty(),
                 FrameworkElement::HorizontalAlignmentProperty(), UIElement::RenderTransformProperty()}) {
            watchGeometry(element, property);
        }
    }
    // Native tray insets may change without changing the source panel's size.
    if (auto panel = FindNativeTrayPanel(root)) {
        if (auto grid = panel.try_as<Controls::Grid>()) {
            watchGeometry(grid, Controls::Grid::PaddingProperty());
            watchGeometry(grid, Controls::Grid::BorderThicknessProperty());
        } else if (auto stackPanel = panel.try_as<Controls::StackPanel>()) {
            watchGeometry(stackPanel, Controls::StackPanel::PaddingProperty());
            watchGeometry(stackPanel, Controls::StackPanel::BorderThicknessProperty());
        } else if (auto border = panel.try_as<Controls::Border>()) {
            watchGeometry(border, Controls::Border::PaddingProperty());
            watchGeometry(border, Controls::Border::BorderThicknessProperty());
        }
    }
    placement.layoutStamp = ReadClockTrayLayoutStamp();
    placement.clockLoadedToken = clock.Loaded(
        [generation](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            if (!ClockTrayWorkIsCurrent(generation)) return;
            g_clockTrayPlacement.metricsDirty = true;
            // Let the native Loaded wave finish before reading descendants.
            // The next layout or the one-shot deadline performs the check.
        });
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [] {});
    g_clockTrayPlacement.metricLayoutToken = clock.LayoutUpdated(
        [generation](winrt::Windows::Foundation::IInspectable const&,
                     winrt::Windows::Foundation::IInspectable const&) {
            ProcessClockTrayChanges(generation);
        });
    g_clockTrayCleanupPending = true;
    ApplyClockTrayStep(generation, [] {});
    g_clockTrayPlacement.metricsDirty = true;
}
// END CLOCK TRAY METRICS (P3)

bool ClockTraySameMetrics(const ClockTrayMetrics& a, const ClockTrayMetrics& b) {
    // Do not round away small growth: it may consume the last safe gap.
    return a.requiredWidth == b.requiredWidth && a.requiredHeight == b.requiredHeight &&
           a.lines == b.lines && a.nativeChromeWidth == b.nativeChromeWidth;
}

ClockTrayRetryKey MakeClockTrayRetryKey(const ClockTrayLayout& layout,
                                       const ClockTrayMetrics& metrics,
                                       const TrayAppearance& appearance,
                                       double windowHeight) {
    auto& sample = layout.snapshot;
    ClockTrayRetryKey key{{layout.screenLeft, windowHeight, metrics.requiredWidth,
        metrics.requiredHeight, double(metrics.lines), metrics.nativeChromeWidth,
        sample.contentLeft, sample.contentWidth, sample.rootWidth, sample.offset,
        double(sample.centered), sample.rightBoundary, sample.repeaterDesiredWidth},
        {sample.repeaterIdentity}};
    for (auto value : {appearance.margin, appearance.borderThickness, appearance.padding}) {
        key.values.insert(key.values.end(), {value.Left, value.Top, value.Right, value.Bottom});
    }
    for (auto& button : sample.buttons) {
        key.identities.push_back(button.identity);
        key.values.insert(key.values.end(), {double(button.kind), double(button.visible),
            double(button.fixed), button.bounds.left, button.bounds.right, button.marginLeft,
            button.outerWidth, button.contentWidth, button.marginRight});
    }
    for (auto& item : sample.flowElements) {
        key.identities.push_back(item.identity);
        key.values.insert(key.values.end(), {item.bounds.left, item.bounds.right, double(item.overflow)});
    }
    // Unknown measurements also need a stable key; NaN never compares equal.
    for (auto& value : key.values) if (std::isnan(value)) value = 0;
    return key;
}

bool ClockTrayBeginNativeAttempt(ClockTrayRetryKey key) {
    if (g_clockTray.lastNativeAttempt && *g_clockTray.lastNativeAttempt == key) return false;
    g_clockTray.lastNativeAttempt = std::move(key);
    g_clockTrayCounters.nativeAttempts++;
    return true;
}

void ProcessClockTrayChanges(uint64_t generation) {
    if (g_clockTrayWorkActive || !ClockTrayWorkIsCurrent(generation)) return;
    g_clockTrayCounters.signals++;
    try {
        auto& current = g_clockTrayPlacement;
        auto clock = current.clock.get();
        if (!clock) return;
        if (current.placed && !clock.IsLoaded() && WaitForClockTrayArrange()) return;
        auto stamp = ReadClockTrayLayoutStamp();
        if (!current.layoutStamp || *current.layoutStamp != stamp) {
            if (!current.layoutStamp || current.layoutStamp->source != stamp.source ||
                current.layoutStamp->repeater != stamp.repeater) current.layoutTrackingDirty = true;
            bool extentChanged = current.placed && current.layoutStamp &&
                (current.layoutStamp->nativeColumnWidth != stamp.nativeColumnWidth ||
                 current.layoutStamp->centered != stamp.centered);
            current.layoutStamp = stamp;
            current.geometryDirty = true;
            if (extentChanged) {
                ClockTrayWorkScope work;
                ApplyClockTrayStep(generation, [&] { current.root.get().InvalidateMeasure(); });
            }
        }
        if (current.layoutTrackingDirty && !current.placed) {
            ClockTrayWorkScope work;
            if (!ClearClockTrayMetricHooks()) return;
            WatchClockTrayMetrics(clock, generation);
            current.geometryDirty = true;
        }
        bool metricsChanged = false;
        if (current.metricsDirty) {
            ClockTrayWorkScope work;
            current.metricsDirty = false;
            auto metrics = ReadClockTrayMetrics(clock);
            metricsChanged = !current.observedMetrics || !ClockTraySameMetrics(*current.observedMetrics, metrics);
            if (!metricsChanged) g_clockTrayCounters.unchangedMetrics++;
            current.observedMetrics = metrics;
        }
        if (current.geometryDirty || current.layoutTrackingDirty || metricsChanged) {
            current.geometryDirty = false;
            if (current.placed) {
                UpdateClockTrayPlacement();
            } else if (!current.arrangeDeadline || !current.arrangeDeadline.IsEnabled()) {
                StartClockTrayArrangeDeadline(true);
            }
        }
    } catch (...) {
        if (ClockTrayWorkIsCurrent(generation)) {
            if (g_clockTrayPlacement.placed) RestoreClockTray(ClockTrayReason::ElementReplaced);
            else {
                ClockTrayWorkScope work;
                if (ClearClockTrayMetricHooks()) {
                    g_clockTrayPlacement = {};
                    g_clockTrayCleanupPending = false;
                    EnsureClockTrayRetry(g_clockTray.clock.get());
                }
            }
        }
    }
}

void EnsureClockTrayRetry(FrameworkElement clock) {
    if (!clock || g_clockTrayPlacement.placed || !ClockTrayWorkIsCurrent(g_clockTray.generation)) return;
    ClockTrayWorkScope work;
    if (g_clockTrayPlacement.metricLayoutToken) return;
    if (g_clockTrayPlacement.clockLoadedToken) {
        if (!clock.IsLoaded() || !ClearClockTrayMetricHooks()) return;
        g_clockTrayPlacement = {};
        g_clockTrayCleanupPending = false;
    }
    if (g_clockTrayCleanupPending) return;
    auto generation = g_clockTray.generation;
    try {
        if (!clock.IsLoaded()) {
            // Startup/template creation needs only Loaded, not a frame budget.
            g_clockTrayPlacement.clock = clock;
            g_clockTray.clock = clock;
            g_clockTrayPlacement.clockLoadedToken = clock.Loaded(
                [generation](auto const&, auto const&) {
                    if (!g_clockTrayWorkActive && ClockTrayWorkIsCurrent(generation)) {
                        try { StartClockTrayArrangeDeadline(true); }
                        catch (...) { RestoreClockTray(ClockTrayReason::UnknownMetrics); }
                    }
                });
            g_clockTrayCleanupPending = true;
            ApplyClockTrayStep(generation, [] {});
            StartClockTrayArrangeDeadline(true);
            return;
        }
        auto parent = Media::VisualTreeHelper::GetParent(clock).try_as<Controls::Grid>();
        auto frame = parent ? Media::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>() : nullptr;
        auto root = frame ? Media::VisualTreeHelper::GetParent(frame).try_as<Controls::Grid>() : nullptr;
        int column = Controls::Grid::GetColumn(clock);
        if (!parent || parent.Name() != L"SystemTrayFrameGrid" || !root ||
            !root.IsLoaded() || column < 0 || uint32_t(column) >= parent.ColumnDefinitions().Size()) return;
        auto& current = g_clockTrayPlacement;
        current.clock = clock;
        current.root = root;
        current.originalParent = parent;
        current.originalColumn = column;
        current.nativeColumn = parent.ColumnDefinitions().GetAt(column);
        g_clockTray.clock = clock;
        WatchClockTrayMetrics(clock, generation);
        StartClockTrayArrangeDeadline(true);
    } catch (...) {
        // Keep failed removals recorded; a later unload can retry safely.
        bool complete = ClearClockTrayMetricHooks();
        if (complete) g_clockTrayPlacement = {};
        g_clockTrayCleanupPending = !complete;
        SetClockTrayResult(ClockTrayState::NativeWaiting, ClockTrayReason::UnsupportedLayout);
    }
}

bool TryAttachClockTray(FrameworkElement clock) {
    auto& placement = g_clockTrayPlacement;
    auto generation = g_clockTray.generation;
    if (!clock || placement.placed || g_clockTrayWorkActive ||
        !ClockTrayWorkIsCurrent(generation)) {
        return false;
    }
    ClockTrayWorkScope work;
    PCWSTR stage = L"validate";
    try {
        auto originalParent =
            Media::VisualTreeHelper::GetParent(clock).try_as<Controls::Panel>();
        if (!originalParent || originalParent.Name() != L"SystemTrayFrameGrid") {
            Wh_Log(L"[clock-tray] unexpected clock parent");
            SetClockTrayResult(ClockTrayState::NativeWaiting,
                               ClockTrayReason::UnsupportedLayout);
            return false;
        }
        auto frame =
            Media::VisualTreeHelper::GetParent(originalParent)
                .try_as<FrameworkElement>();
        if (!frame ||
            winrt::get_class_name(frame) != L"SystemTray.SystemTrayFrame") {
            SetClockTrayResult(ClockTrayState::NativeWaiting,
                               ClockTrayReason::UnsupportedLayout);
            return false;
        }
        auto root = Media::VisualTreeHelper::GetParent(frame)
                        .try_as<Controls::Grid>();
        if (!root || !root.IsLoaded()) {
            SetClockTrayResult(ClockTrayState::AwaitingLayout,
                               ClockTrayReason::None);
            return false;
        }
        auto nativeGrid = originalParent.try_as<Controls::Grid>();
        int columnIndex = Controls::Grid::GetColumn(clock);
        if (!nativeGrid || columnIndex < 0 || Controls::Grid::GetColumnSpan(clock) != 1 ||
            static_cast<uint32_t>(columnIndex) >= nativeGrid.ColumnDefinitions().Size()) {
            Wh_Log(L"[clock-tray] native column unavailable parent=%s column=%d span=%d count=%u",
                   winrt::get_class_name(originalParent).c_str(), columnIndex,
                   Controls::Grid::GetColumnSpan(clock), nativeGrid ? nativeGrid.ColumnDefinitions().Size() : 0);
            SetClockTrayResult(ClockTrayState::NativeWaiting, ClockTrayReason::UnsupportedLayout);
            return false;
        }
        auto nativeColumn = nativeGrid.ColumnDefinitions().GetAt(columnIndex);
        double nativeColumnWidth = nativeColumn.ActualWidth();
        if (!std::isfinite(nativeColumnWidth) || nativeColumnWidth <= 0) {
            SetClockTrayResult(ClockTrayState::AwaitingLayout, ClockTrayReason::UnknownMetrics);
            return false;
        }
        auto repeater = FindTaskbarRepeater(root);
        stage = L"layout";
        auto layout = ReadClockTrayLayout(root, repeater);
        if (!layout) {
            SetClockTrayResult(ClockTrayState::NativeWaiting,
                               ClockTrayReason::UnsupportedLayout);
            return false;
        }
        stage = L"metrics";
        auto metrics = ReadClockTrayMetrics(clock);
        auto appearance = LeftClockTrayAppearance(ReadNativeTrayAppearance(FindNativeTrayPanel(root)));
        auto key = MakeClockTrayRetryKey(*layout, metrics, appearance, ClockTrayWindowHeight(root));
        key.identities.push_back(reinterpret_cast<uintptr_t>(winrt::get_abi(clock)));
        key.identities.push_back(reinterpret_cast<uintptr_t>(winrt::get_abi(FindClockTrayTextStack(clock))));
        key.identities.push_back(reinterpret_cast<uintptr_t>(winrt::get_abi(
            repeater.as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>().ItemsSourceView())));
        // Releasing a binding or replacing a local style is an effective
        // ownership change even when the resulting text width stays equal.
        for (auto property : {Controls::Grid::ColumnProperty(), Controls::Grid::ColumnSpanProperty(),
                 Controls::Grid::RowProperty(), Controls::Grid::RowSpanProperty(),
                 FrameworkElement::HorizontalAlignmentProperty(), FrameworkElement::VerticalAlignmentProperty(),
                 FrameworkElement::MarginProperty()}) {
            key.values.push_back(clock.GetBindingExpression(property) ? 1 : 0);
            key.values.push_back(clock.ReadLocalValue(property) == DependencyProperty::UnsetValue() ? 1 : 0);
        }
        if (!ClockTrayBeginNativeAttempt(std::move(key))) return false;
        auto metricsReason = CheckClockTrayMetrics(metrics, ClockTrayWindowHeight(root));
        if (metricsReason != ClockTrayReason::None) {
            SetClockTrayResult(metricsReason == ClockTrayReason::UnknownMetrics
                                   ? ClockTrayState::AwaitingLayout : ClockTrayState::NativeWaiting,
                               metricsReason);
            return false;
        }
        double width = metrics.requiredWidth;
        auto plan = ComputeClockTrayPlan(layout->snapshot, layout->screenLeft, width,
            appearance.margin, appearance.borderThickness, ClampTrayPadding(appearance.padding));
        if (plan.reason != ClockTrayReason::None) {
            Wh_Log(L"[clock-tray] plan rejected reason=%s visible=%.3f..%.3f boundary=%.3f",
                   ClockTrayReasonName(plan.reason), plan.visible.left, plan.visible.right,
                   layout->snapshot.rightBoundary);
            SetClockTrayResult(ClockTrayState::NativeWaiting,
                               plan.reason);
                return false;
        }
        uint32_t originalIndex = 0;
        if (!originalParent.Children().IndexOf(clock.as<UIElement>(),
                                               originalIndex)) {
            SetClockTrayResult(ClockTrayState::NativeWaiting,
                               ClockTrayReason::ElementReplaced);
            return false;
        }

        if (!ClearClockTrayMetricHooks()) return false;
        Controls::Grid host;
        host.Name(L"TaskbarClockToLeftHost");
        host.HorizontalAlignment(HorizontalAlignment::Left);
        host.VerticalAlignment(VerticalAlignment::Stretch);
        g_clockTrayCounters.marginWrites++;
        host.Margin(Thickness{plan.hostLeft, 0, 0, 0});
        host.Width(plan.hostWidth);
        Controls::Grid::SetColumn(host, 0);
        Controls::Grid::SetColumnSpan(
            host, std::max<int>(1, root.ColumnDefinitions().Size()));
        Controls::Grid::SetRow(host, 0);
        Controls::Grid::SetRowSpan(
            host, std::max<int>(1, root.RowDefinitions().Size()));

        Controls::Border decoration;
        decoration.Name(L"TaskbarClockToLeftBackground");
        // The border is the clock's parent now, so it must stay hit-testable;
        // its insets are what the theme applies around the native tray content.
        decoration.HorizontalAlignment(HorizontalAlignment::Stretch);
        decoration.VerticalAlignment(VerticalAlignment::Stretch);

        placement = ClockTrayPlacement{};
        placement.clock = clock;
        placement.root = root;
        placement.originalParent = originalParent;
        placement.originalIndex = originalIndex;
        placement.originalColumn = Controls::Grid::GetColumn(clock);
        placement.originalColumnSpan = Controls::Grid::GetColumnSpan(clock);
        placement.originalRow = Controls::Grid::GetRow(clock);
        placement.originalRowSpan = Controls::Grid::GetRowSpan(clock);
        placement.originalHorizontalAlignment = clock.HorizontalAlignment();
        placement.originalVerticalAlignment = clock.VerticalAlignment();
        placement.appliedVerticalAlignment = metrics.lines == 1
            ? VerticalAlignment::Center : VerticalAlignment::Stretch;
        placement.originalMargin = clock.Margin();
        placement.unsetProperties =
            ClockTrayUnsetProperty<int>(clock, Controls::Grid::ColumnProperty(), 1) |
            ClockTrayUnsetProperty<int>(clock, Controls::Grid::ColumnSpanProperty(), 2) |
            ClockTrayUnsetProperty<int>(clock, Controls::Grid::RowProperty(), 4) |
            ClockTrayUnsetProperty<int>(clock, Controls::Grid::RowSpanProperty(), 8) |
            ClockTrayUnsetProperty<HorizontalAlignment>(clock,
                FrameworkElement::HorizontalAlignmentProperty(), 16) |
            ClockTrayUnsetProperty<VerticalAlignment>(clock,
                FrameworkElement::VerticalAlignmentProperty(), 32) |
            ClockTrayUnsetProperty<Thickness>(clock,
                FrameworkElement::MarginProperty(), 64);
        placement.host = host;
        placement.decoration = decoration;
        placement.plan = plan;
        placement.nativeColumn = nativeColumn;
        placement.nativeColumnWidth = nativeColumnWidth;
        placement.metrics = metrics;
        placement.observedMetrics = metrics;
        placement.arrangeWaitStart = GetTickCount64();
        placement.placed = true;
        g_clockTrayCleanupPending = true;

        stage = L"remove-clock";
        placement.detachedClock = clock;
        ApplyClockTrayStep(generation, [&] {
            originalParent.Children().RemoveAt(originalIndex);
        });
        SetClockTrayProperty(clock, Controls::Grid::ColumnProperty(),
                             0, 1, generation);
        SetClockTrayProperty(clock, Controls::Grid::ColumnSpanProperty(),
                             1, 2, generation);
        SetClockTrayProperty(clock, Controls::Grid::RowProperty(),
                             0, 4, generation);
        SetClockTrayProperty(clock, Controls::Grid::RowSpanProperty(),
                             1, 8, generation);
        SetClockTrayProperty(clock, FrameworkElement::HorizontalAlignmentProperty(),
                             HorizontalAlignment::Stretch, 16, generation);
        SetClockTrayProperty(clock, FrameworkElement::VerticalAlignmentProperty(),
                             placement.appliedVerticalAlignment, 32, generation);
        SetClockTrayProperty(clock, FrameworkElement::MarginProperty(),
                             Thickness{0, 0, 0, 0}, 64, generation);
        // Faithful structure: the decoration is the clock's parent, so the
        // theme's Margin and BorderThickness inset the content exactly as they
        // inset the native tray's icons.
        stage = L"attach-child";
        ApplyClockTrayStep(generation, [&] { decoration.Child(clock.as<UIElement>()); });
        placement.detachedClock = nullptr;
        stage = L"append-host";
        ApplyClockTrayStep(generation, [&] { host.Children().Append(decoration); });
        ApplyClockTrayStep(generation, [&] { root.Children().Append(host.as<UIElement>()); });
        stage = L"appearance";
        ApplyClockTrayStep(generation, [&] { MirrorNativeTrayAppearance(root); });
        stage = L"size-hook";
        auto weakClock = winrt::make_weak(clock);
        placement.clockSizeToken = clock.SizeChanged(
            [weakClock, generation](winrt::Windows::Foundation::IInspectable const&,
                        SizeChangedEventArgs const&) {
                try {
                    if (!g_clockTrayWorkActive && ClockTrayWorkIsCurrent(generation) &&
                        weakClock.get() == g_clockTrayPlacement.clock.get()) {
                        g_clockTrayPlacement.metricsDirty = true;
                    }
                } catch (...) {
                    Wh_Log(L"[clock-tray] size handling failed: %08X",
                           winrt::to_hresult());
                }
            });
        placement.clockSizeOwner = clock;
        g_clockTrayCleanupPending = true;
        ApplyClockTrayStep(generation, [] {});
        stage = L"metrics-hooks";
        WatchClockTrayMetrics(clock, generation);
        StartClockTrayArrangeDeadline();
        g_clockTray.pending = false;
        g_clockTrayCounters.attachments++;
        SetClockTrayResult(ClockTrayState::Placing, ClockTrayReason::None);
        Wh_Log(L"[clock-tray] attached anchor=%.3f shift=%.3f host=%.3f+%.3f "
               L"visible=%.3f..%.3f nativeColumn=%.3f centered=%d",
               plan.layoutLeft, plan.visualShift, plan.hostLeft, plan.hostWidth,
               plan.visible.left, plan.visible.right, nativeColumnWidth, layout->snapshot.centered);
        Wh_Log(L"[clock-tray] metrics lines=%u required=%.3fx%.3f actual=%.3fx%.3f",
               metrics.lines, metrics.requiredWidth, metrics.requiredHeight,
               clock.ActualWidth(), clock.ActualHeight());
        return true;
    } catch (...) {
        Wh_Log(L"[clock-tray] attach failed stage=%s hr=%08X",
               stage, winrt::to_hresult());
        if (generation == g_clockTray.generation) {
            RestoreClockTray(ClockTrayReason::UnsupportedLayout);
        }
        return false;
    }
}

void UpdateClockTrayPlacement() {
    auto& placement = g_clockTrayPlacement;
    auto generation = g_clockTray.generation;
    if (!placement.placed || g_clockTrayWorkActive ||
        !ClockTrayWorkIsCurrent(generation)) {
        return;
    }
    ClockTrayWorkScope work;
    auto clock = placement.clock.get();
    auto host = placement.host;
    auto root = placement.root.get();
    if (!clock || !host || !root) {
        RestoreClockTray(ClockTrayReason::ElementReplaced);
        return;
    }
    try {
        if (Media::VisualTreeHelper::GetParent(clock) != placement.decoration) {
            RestoreClockTray(ClockTrayReason::TargetOwned);
            return;
        }
        auto nativeGrid = placement.originalParent.get().try_as<Controls::Grid>();
        uint32_t columnIndex = 0;
        if (!nativeGrid || !placement.nativeColumn ||
            !nativeGrid.ColumnDefinitions().IndexOf(placement.nativeColumn, columnIndex) ||
            static_cast<int>(columnIndex) != placement.originalColumn) {
            RestoreClockTray(ClockTrayReason::UnsupportedLayout);
            return;
        }
        if (placement.layoutTrackingDirty) {
            if (!ClearClockTrayMetricHooks()) throw winrt::hresult_error(E_FAIL);
            WatchClockTrayMetrics(clock, generation);
            if (g_clockTray.state == ClockTrayState::Placing) StartClockTrayArrangeDeadline();
        }
        if (!clock.IsLoaded() || !host.IsLoaded() || !placement.decoration.IsLoaded() ||
            clock.ActualWidth() <= 0 || clock.ActualHeight() <= 0) {
            if (WaitForClockTrayArrange()) return;
            Wh_Log(L"[clock-tray] arrange deadline loaded=%d,%d,%d clock=%.3fx%.3f",
                   clock.IsLoaded() ? 1 : 0, host.IsLoaded() ? 1 : 0,
                   placement.decoration.IsLoaded() ? 1 : 0, clock.ActualWidth(), clock.ActualHeight());
            RestoreClockTray(ClockTrayReason::UnknownMetrics);
            return;
        }
        auto repeater = FindTaskbarRepeater(root);
        auto layout = ReadClockTrayLayout(root, repeater);
        if (!layout) {
            RestoreClockTray(ClockTrayReason::UnsupportedLayout);
            return;
        }
        auto metrics = placement.observedMetrics.value_or(ClockTrayMetrics{});
        if (placement.metricsDirty || !placement.observedMetrics) metrics = ReadClockTrayMetrics(clock);
        placement.observedMetrics = metrics;
        placement.metricsDirty = false;
        double windowHeight = ClockTrayWindowHeight(root);
        auto metricsReason = CheckClockTrayMetrics(metrics, windowHeight);
        if (metricsReason != ClockTrayReason::None) {
            Wh_Log(L"[clock-tray] metrics rejected lines=%u required=%.3fx%.3f window=%.3f",
                   metrics.lines, metrics.requiredWidth, metrics.requiredHeight, windowHeight);
            RestoreClockTray(metricsReason);
            return;
        }
        auto bounds = clock.TransformToVisual(root.XamlRoot().Content()).TransformBounds(
            {0, 0, static_cast<float>(clock.ActualWidth()), static_cast<float>(clock.ActualHeight())});
        auto boundsReason = CheckClockTrayVerticalBounds(bounds.Y, bounds.Height, windowHeight);
        if (boundsReason != ClockTrayReason::None) {
            RestoreClockTray(boundsReason);
            return;
        }
        auto decoration = placement.decoration;
        auto plan = ComputeClockTrayPlan(layout->snapshot, layout->screenLeft,
            metrics.requiredWidth, decoration.Margin(), decoration.BorderThickness(), decoration.Padding());
        if (plan.reason != ClockTrayReason::None) {
            Wh_Log(L"[clock-tray] plan rejected reason=%s visible=%.3f..%.3f boundary=%.3f",
                   ClockTrayReasonName(plan.reason), plan.visible.left, plan.visible.right,
                   layout->snapshot.rightBoundary);
            RestoreClockTray(plan.reason);
            return;
        }
        bool changed = !std::isfinite(host.Width()) || std::abs(host.Width() - plan.hostWidth) > 0.01 ||
                       std::abs(host.Margin().Left - plan.hostLeft) > kLayoutTolerance;
        placement.plan = plan;
        if (!std::isfinite(host.Width()) || std::abs(host.Width() - plan.hostWidth) > 0.01) {
            ApplyClockTrayStep(generation, [&] { host.Width(plan.hostWidth); });
        }
        auto alignment = metrics.lines == 1 ? VerticalAlignment::Center : VerticalAlignment::Stretch;
        if (clock.VerticalAlignment() != placement.appliedVerticalAlignment) {
            RestoreClockTray(ClockTrayReason::TargetOwned);
            return;
        }
        if (alignment != placement.appliedVerticalAlignment) {
            changed = true;
            // Record the intended value before the setter can synchronously reenter.
            placement.appliedVerticalAlignment = alignment;
            SetClockTrayProperty(clock, FrameworkElement::VerticalAlignmentProperty(),
                                 alignment, 32, generation);
        }
        ApplyClockTrayHostOffset();
        if (changed) {
            // Validate the resulting rectangle after XAML has arranged our
            // writes. Old ActualWidth is not evidence that a new width fits.
            placement.metricsDirty = true;
            if (g_clockTray.state == ClockTrayState::Stable) StartClockTrayArrangeDeadline();
            SetClockTrayResult(ClockTrayState::Placing, ClockTrayReason::None);
            return;
        }
        auto boxBounds = ReadClockTrayVisualBounds(decoration, root);
        auto clockBounds = ReadClockTrayVisualBounds(clock, root);
        if (!boxBounds || !clockBounds || clock.ActualWidth() + kLayoutTolerance < metrics.requiredWidth) {
            Wh_Log(L"[clock-tray] arranged metrics boxValid=%d clockValid=%d actualWidth=%.3f requiredWidth=%.3f nativeChrome=%.3f",
                   boxBounds ? 1 : 0, clockBounds ? 1 : 0, clock.ActualWidth(), metrics.requiredWidth,
                   metrics.nativeChromeWidth);
            if (WaitForClockTrayArrange()) return;
            RestoreClockTray(ClockTrayReason::UnknownMetrics);
            return;
        }
        HorizontalBounds actual{std::min(boxBounds->left, clockBounds->left),
                                std::max(boxBounds->right, clockBounds->right)};
        auto actualReason = CheckClockTrayActualBounds(layout->snapshot, layout->screenLeft, plan, actual);
        if (actualReason != ClockTrayReason::None) {
            Wh_Log(L"[clock-tray] arranged bounds rejected reason=%s planned=%.3f..%.3f actual=%.3f..%.3f",
                   ClockTrayReasonName(actualReason), plan.visible.left, plan.visible.right,
                   actual.left, actual.right);
            RestoreClockTray(actualReason);
            return;
        }
        if (g_clockTray.state != ClockTrayState::Stable) {
            Wh_Log(L"[clock-tray] arranged anchor=%.3f shift=%.3f host=%.3f+%.3f actual=%.3f..%.3f boundary=%.3f",
                   plan.layoutLeft, plan.visualShift, plan.hostLeft, plan.hostWidth,
                   actual.left, actual.right, layout->snapshot.rightBoundary);
            LogClockTrayCounters();
        }
        if (placement.metrics.lines != metrics.lines ||
            std::abs(placement.metrics.requiredWidth - metrics.requiredWidth) > 0.01 ||
            std::abs(placement.metrics.requiredHeight - metrics.requiredHeight) > 0.01) {
            Wh_Log(L"[clock-tray] metrics lines=%u required=%.3fx%.3f actual=%.3fx%.3f",
                   metrics.lines, metrics.requiredWidth, metrics.requiredHeight,
                   clock.ActualWidth(), clock.ActualHeight());
        }
        placement.metrics = metrics;
        if (!StopClockTrayArrangeDeadline()) throw winrt::hresult_error(E_FAIL);
        placement.arrangeWaitStart = 0;
        // A confirmed success is not a failed input. The next safety fallback
        // must allow recovery even if space returns before its deferred check.
        g_clockTray.lastNativeAttempt.reset();
        SetClockTrayResult(ClockTrayState::Stable, ClockTrayReason::None);
    } catch (...) {
        Wh_Log(L"[clock-tray] placement update failed: %08X",
               winrt::to_hresult());
        if (generation == g_clockTray.generation) {
            RestoreClockTray(ClockTrayReason::UnsupportedLayout);
        }
    }
}

void RestoreClockTray(ClockTrayReason reason) {
    auto& placement = g_clockTrayPlacement;
    if (placement.restoring) return;
    ClockTrayWorkScope work;
    placement.restoring = true;
    InvalidateClockTrayWork(reason, ClockTrayState::Restoring);
    bool subscriptionsRemoved = true;
    try {
        if (auto owner = placement.clockSizeOwner.get()) {
            if (placement.clockSizeToken) {
                owner.SizeChanged(*placement.clockSizeToken);
            }
        }
        placement.clockSizeToken.reset();
        placement.clockSizeOwner = {};
    } catch (...) {
        subscriptionsRemoved = false;
        Wh_Log(L"[clock-tray] size removal failed: %08X", winrt::to_hresult());
    }
    if (!ClearClockTrayAppearanceHooks()) subscriptionsRemoved = false;
    if (!ClearClockTrayMetricHooks()) subscriptionsRemoved = false;

    auto clock = placement.clock.get();
    if (!clock) clock = placement.detachedClock;
    auto host = placement.host;
    auto root = placement.root.get();
    auto decoration = placement.decoration;
    auto originalParent = placement.originalParent.get();
    bool nativeWaiting = clock && !placement.placed && !placement.detachedClock && !placement.changedProperties;
    bool restored = false;
    bool released = !clock;
    try {
        if (nativeWaiting) {
            // A native waiting record owns observers, never the native parent.
            released = true;
        } else if (clock) {
            auto parent = Media::VisualTreeHelper::GetParent(clock);
            bool external = parent && parent != decoration && parent != host &&
                            parent != originalParent;
            if (external) {
                // Never remove the clock from a different mod's parent.
                reason = ClockTrayReason::TargetOwned;
                released = true;
            } else {
                auto nativeParentIsValid = [&] {
                    return originalParent && originalParent.IsLoaded() &&
                           root && root.IsLoaded() && originalParent.XamlRoot() &&
                           originalParent.XamlRoot() == root.XamlRoot();
                };
                if (!nativeParentIsValid()) {
                    throw winrt::hresult_error(E_PENDING);
                }
                if (!parent && placement.detachedClock != clock) {
                    // An unparented element without our handoff record is not ours.
                    throw winrt::hresult_error(E_ABORT);
                }
                if (parent == decoration || parent == host) {
                    placement.detachedClock = clock;
                    if (parent == decoration && decoration.Child() == clock) {
                        decoration.Child(nullptr);
                    } else if (parent == host) {
                        uint32_t index = 0;
                        if (host.Children().IndexOf(clock.as<UIElement>(), index)) {
                            host.Children().RemoveAt(index);
                        }
                    }
                }
                parent = Media::VisualTreeHelper::GetParent(clock);
                if (!parent && nativeParentIsValid()) {
                    auto children = originalParent.Children();
                    children.InsertAt(std::min<uint32_t>(placement.originalIndex,
                                                        children.Size()),
                                      clock.as<UIElement>());
                }
                if (Media::VisualTreeHelper::GetParent(clock) != originalParent ||
                    !nativeParentIsValid()) {
                    throw winrt::hresult_error(E_ABORT);
                }
                placement.detachedClock = nullptr;
                RestoreClockTrayProperty(clock, originalParent,
                    Controls::Grid::ColumnProperty(), placement.originalColumn, 0, 1);
                RestoreClockTrayProperty(clock, originalParent,
                    Controls::Grid::ColumnSpanProperty(), placement.originalColumnSpan, 1, 2);
                RestoreClockTrayProperty(clock, originalParent,
                    Controls::Grid::RowProperty(), placement.originalRow, 0, 4);
                RestoreClockTrayProperty(clock, originalParent,
                    Controls::Grid::RowSpanProperty(), placement.originalRowSpan, 1, 8);
                RestoreClockTrayProperty(clock, originalParent,
                    FrameworkElement::HorizontalAlignmentProperty(),
                    placement.originalHorizontalAlignment, HorizontalAlignment::Stretch, 16);
                RestoreClockTrayProperty(clock, originalParent,
                    FrameworkElement::VerticalAlignmentProperty(),
                    placement.originalVerticalAlignment, placement.appliedVerticalAlignment, 32);
                RestoreClockTrayProperty(clock, originalParent,
                    FrameworkElement::MarginProperty(), placement.originalMargin,
                    Thickness{0, 0, 0, 0}, 64);
                restored = Media::VisualTreeHelper::GetParent(clock) == originalParent &&
                           nativeParentIsValid();
                released = restored;
            }
        }
    } catch (...) {
        Wh_Log(L"[clock-tray] restore failed: %08X", winrt::to_hresult());
    }
    bool containersRemoved = false;
    if (released) try {
        if (decoration && decoration.Child()) {
            throw winrt::hresult_error(E_PENDING);
        }
        if (host) {
            // Only the empty decoration is ours to remove. Keep unexpected children.
            if (host.Children().Size() == 1 && host.Children().GetAt(0) == decoration) {
                host.Children().RemoveAt(0);
            }
            if (host.Children().Size() != 0 || (decoration && decoration.Child())) {
                throw winrt::hresult_error(E_PENDING);
            }
            auto parent = Media::VisualTreeHelper::GetParent(host);
            if (parent && parent == root) {
                uint32_t index = 0;
                if (root.Children().IndexOf(host.as<UIElement>(), index)) {
                    root.Children().RemoveAt(index);
                }
            }
            if (Media::VisualTreeHelper::GetParent(host) || host.Children().Size() ||
                (decoration && decoration.Child())) {
                throw winrt::hresult_error(E_PENDING);
            }
        }
        containersRemoved = true;
    } catch (...) {
        Wh_Log(L"[clock-tray] container cleanup failed: %08X",
               winrt::to_hresult());
    }
    bool complete = released && containersRemoved && subscriptionsRemoved;
    placement.restoring = false;
    if (complete) placement = ClockTrayPlacement{};
    g_clockTrayCleanupPending = !complete;
    SetClockTrayResult(!complete ? ClockTrayState::Restoring :
                      reason == ClockTrayReason::UserDisabled ? ClockTrayState::Disabled :
                      ClockTrayState::NativeWaiting, reason);
    if (restored) g_clockTrayCounters.restorations++;
    Wh_Log(L"[clock-tray] restored reason=%s accepted=%d cleanup=%d",
           ClockTrayReasonName(reason), restored ? 1 : 0, complete ? 1 : 0);
    if (complete && (restored || nativeWaiting) && reason != ClockTrayReason::UserDisabled && g_callbacksEnabled) {
        EnsureClockTrayRetry(clock);
    }
}
// END CLOCK TRAY LAYOUT (P2)

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

        if (g_clockTrayPlacement.placed) {
            auto trayClock = g_clockTrayPlacement.clock.get();
            // Compensate only the width that the original native column lost.
            // A fixed/minimum column can retain space after the clock leaves;
            // counting the whole old clock width would reserve it twice.
            if (trayClock && trayClock.XamlRoot() == taskbarXamlRoot &&
                Media::VisualTreeHelper::GetParent(trayClock) ==
                    g_clockTrayPlacement.decoration) {
                auto parent = g_clockTrayPlacement.originalParent.get().try_as<Controls::Grid>();
                auto column = g_clockTrayPlacement.nativeColumn;
                uint32_t index = 0;
                if (parent && parent.IsLoaded() && parent.XamlRoot() == taskbarXamlRoot &&
                    column && parent.ColumnDefinitions().IndexOf(column, index) &&
                    static_cast<int>(index) == g_clockTrayPlacement.originalColumn) {
                    double current = column.ActualWidth();
                    bool centered = TaskbarUsesCenteredAlignment();
                    double reserved = ComputeClockTrayReservedWidth(centered, true,
                        g_clockTrayPlacement.nativeColumnWidth, current);
                    if (std::abs(g_clockTrayPlacement.lastReservedWidth - reserved) > kLayoutTolerance) {
                        g_clockTrayPlacement.lastReservedWidth = reserved;
                        Wh_Log(L"[clock-tray] native extent before=%.3f remaining=%.3f reserved=%.3f centered=%d",
                               g_clockTrayPlacement.nativeColumnWidth, current, reserved, centered);
                    }
                    return reserved;
                }
            }
            return 0;
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
winrt::weak_ref<FrameworkElement> g_clockContent;

void QueueKnownClockPlacementUpdate() {
    if (g_callbacksEnabled && g_clockThreadId == GetCurrentThreadId()) {
        if (auto clock = g_clockTray.clock.get()) QueueClockTrayUpdate(clock);
    }
}

void UpdateClockPlacement(FrameworkElement content) {
    if (!g_callbacksEnabled) return;
    DWORD thread = GetCurrentThreadId();
    if (g_clockThreadId && g_clockThreadId != thread) return;
    g_clockThreadId = thread;
    g_clockContent = content;
    if (auto clock = FindAncestor(content, L"SystemTray.OmniButton", L"NotificationCenterButton")) {
        QueueClockTrayUpdate(clock);
    }
}

void UpdateKnownClocksCallback(void*) {
    try {
        if (auto content = g_clockContent.get()) UpdateClockPlacement(content);
    } catch (...) {
        Wh_Log(L"Known clock update failed: %08X", winrt::to_hresult());
    }
}

void CleanupTaskbarStateCallback(void*) {
    LogClockTrayCounters();
    RestoreClockTray(ClockTrayReason::UserDisabled);
    if (!g_clockTrayCleanupPending) g_clockContent = {};
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
bool CleanupTaskbarStateSynchronously() {
    if (RunOnTaskbarThread(CleanupTaskbarStateCallback, nullptr, L"Clock layout cleanup")) return true;
    auto content = g_clockContent.get();
    if (!content) content = g_clockTrayPlacement.clock.get();
    if (!content) return !g_clockTrayCleanupPending;
    try {
        auto dispatcher = content.Dispatcher();
        if (!dispatcher) return false;
        if (dispatcher.HasThreadAccess()) {
            CleanupTaskbarStateCallback(nullptr);
        } else {
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                [] { CleanupTaskbarStateCallback(nullptr); }).get();
        }
        return true;
    } catch (...) {
        Wh_Log(L"Clock dispatcher cleanup failed: %08X", winrt::to_hresult());
        return false;
    }
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
    ResetClockTrayForSession();
    Wh_Log(L"Taskbar Clock to Left " WH_MOD_VERSION L": enabled");
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

void Wh_ModBeforeUninit() {
    g_callbacksEnabled = false;
    // Invalidate tray work inside RestoreClockTray on the owning UI thread.
    Wh_Log(L"[clock-tray] unloading; callbacks disabled");
    StopAlignmentRegistryWatch();
    // Revoke all mod-owned XAML delegates and release their containers while
    // the mod code and hooks are still present. The synchronous taskbar-thread
    // call must finish before Windhawk can continue unloading this DLL.
    CleanupTaskbarStateSynchronously();
}

void Wh_ModUninit() {
    g_callbacksEnabled = false;
    StopAlignmentRegistryWatch();
    // Retry incomplete cleanup before the DLL can unload.
    if (g_clockTrayCleanupPending) {
        CleanupTaskbarStateSynchronously();
    }
    if (g_clockTrayCleanupPending) {
        // Windhawk cannot cancel unload here. A surviving XAML delegate must
        // not point into an unloaded DLL; retain its disabled code and records
        // until Explorer exits if both synchronous cleanup attempts failed.
        HMODULE retainedModule = nullptr;
        BOOL pinned = GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&Wh_ModUninit), &retainedModule);
        Wh_Log(L"[clock-tray] cleanup incomplete; module retained=%d error=%u",
               pinned, pinned ? 0 : GetLastError());
    }
}

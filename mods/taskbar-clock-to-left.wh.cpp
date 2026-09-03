// ==WindhawkMod==
// @id              taskbar-clock-to-left
// @name            Taskbar Clock to Left
// @description     Move the native Windows 11 clock/notification button to the left without shifting centered taskbar apps
// @version         2.1
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

Moves the native Windows 11 notification-center clock button to the left side
of the taskbar. The clock is hosted in its own left-side layer. Instead of
adding columns or invisible spacers, the mod preserves the native system-tray
extent supplied to the taskbar layout engine. Centered Start/app buttons remain
at their original position and the remaining tray stays flush with the right
edge. Clock customization mods can load before or after this mod: customized
text width only changes the left host and never becomes a taskbar-layout input.
On a centered taskbar, the left host is placed after the Widgets/weather button
when it is visible. If Windows is configured to left-align Start and app icons,
the clock stays in its native position to avoid covering those buttons.

![Taskbar Clock to Left](https://i.imgur.com/Wew0LZh.png)

Windows 11's modern taskbar is required.

This mod was inspired by [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) by [m417z](https://github.com/m417z).
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct MovedClockData {
    winrt::weak_ref<FrameworkElement> clock;
    winrt::weak_ref<Controls::Grid> originalParent;
    winrt::weak_ref<Controls::Grid> taskbarRoot;
    winrt::weak_ref<FrameworkElement> widgets;
    Controls::Grid leftHost{nullptr};
    std::optional<winrt::event_token> widgetsSizeChangedToken;
    int64_t widgetsVisibilityChangedToken{};
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

struct DeferredClockData {
    winrt::weak_ref<FrameworkElement> content;
    std::optional<winrt::event_token> loadedToken;
    std::optional<winrt::event_token> layoutUpdatedToken;
    ULONGLONG layoutUpdatedStartTick;
    uint32_t layoutUpdatedAttempts;
};

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

double GetLeftHostOffset(Controls::Grid root) {
    // The Widgets/weather entry point occupies the physical left edge on the
    // centered taskbar. Keep the relocated clock beside it instead of placing
    // an input-capable overlay on top of it.
    auto widgets = FindDescendantByName(root, L"AugmentedEntryPointButton");
    if (!widgets || widgets.Visibility() != Visibility::Visible ||
        widgets.ActualWidth() <= 0) {
        return 0;
    }

    try {
        auto origin = widgets.TransformToVisual(root).TransformPoint({0, 0});
        double rightEdge = origin.X + widgets.ActualWidth();
        if (origin.X >= 0 && rightEdge > 0 &&
            rightEdge <= root.ActualWidth() / 2) {
            return rightEdge + 4;
        }
    } catch (...) {
        Wh_Log(L"Widgets position lookup failed: %08X", winrt::to_hresult());
    }

    return 0;
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
    probe.FontSize(12);
    probe.TextWrapping(TextWrapping::NoWrap);

    WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName))) {
        probe.Language(localeName);
    }

    probe.Measure(winrt::Windows::Foundation::Size{10000, 10000});
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
    double chromeWidth = 24;
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
            if (measuredChrome >= 8 && measuredChrome <= 64) {
                chromeWidth = measuredChrome;
            }
        }
    }

    // Native taskbar clock widths are DPI-independent XAML units. Bounds keep
    // a malformed third-party style from poisoning the layout baseline.
    return std::clamp(nativeTextWidth + chromeWidth, 64.0, 180.0);
}

void UpdateLeftHostOffset(MovedClockData& data) {
    auto root = data.taskbarRoot.get();
    if (!root || !data.leftHost) {
        return;
    }

    double offset = GetLeftHostOffset(root);
    Thickness margin = data.leftHost.Margin();
    if (margin.Left != offset) {
        margin.Left = offset;
        data.leftHost.Margin(margin);
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

    for (auto& data : MovedClocks()) {
        if (data.clock.get() == clock) {
            UpdateLeftHostOffset(data);
            return;
        }
    }
}

void RegisterLeftHostTracking(MovedClockData& data,
                              FrameworkElement clock) {
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

void RevokeLeftHostTracking(MovedClockData& data) {
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

void RemoveLeftHost(MovedClockData& data) {
    RevokeLeftHostTracking(data);

    if (!data.leftHost) {
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

void CleanupMovedClockData() {
    auto& movedClocks = MovedClocks();
    for (auto it = movedClocks.begin(); it != movedClocks.end();) {
        if (!it->clock.get() || !it->taskbarRoot.get()) {
            RemoveLeftHost(*it);
            it = movedClocks.erase(it);
        } else {
            ++it;
        }
    }
}

bool RestoreClock(FrameworkElement clock) {
    CleanupMovedClockData();

    auto& movedClocks = MovedClocks();
    for (auto it = movedClocks.begin(); it != movedClocks.end(); ++it) {
        auto movedClock = it->clock.get();
        if (!movedClock || movedClock != clock) {
            continue;
        }

        auto originalParent = it->originalParent.get();
        auto element = movedClock.as<UIElement>();
        auto currentParent = Media::VisualTreeHelper::GetParent(movedClock)
                                 .try_as<Controls::Panel>();

        if (currentParent &&
            (!originalParent ||
             currentParent != originalParent.as<Controls::Panel>())) {
            uint32_t index = 0;
            if (currentParent.Children().IndexOf(element, index)) {
                currentParent.Children().RemoveAt(index);
            }
        }

        RemoveLeftHost(*it);

        Controls::Grid::SetColumn(movedClock, it->originalColumn);
        Controls::Grid::SetColumnSpan(movedClock, it->originalColumnSpan);
        Controls::Grid::SetRow(movedClock, it->originalRow);
        Controls::Grid::SetRowSpan(movedClock, it->originalRowSpan);
        movedClock.HorizontalAlignment(it->originalHorizontalAlignment);
        movedClock.VerticalAlignment(it->originalVerticalAlignment);
        movedClock.Margin(it->originalMargin);

        currentParent = Media::VisualTreeHelper::GetParent(movedClock)
                            .try_as<Controls::Panel>();
        if (originalParent && !currentParent) {
            auto children = originalParent.Children();
            uint32_t index =
                std::min<uint32_t>(it->originalIndex, children.Size());
            if (index < children.Size()) {
                children.InsertAt(index, element);
            } else {
                children.Append(element);
            }
        }

        movedClocks.erase(it);
        Wh_Log(L"Clock restored from the left host");
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

    if (!TaskbarUsesCenteredAlignment()) {
        RestoreClock(clock);
        return false;
    }

    for (const auto& data : MovedClocks()) {
        if (auto movedClock = data.clock.get(); movedClock == clock) {
            return true;
        }
    }

    auto originalParent = Media::VisualTreeHelper::GetParent(clock)
                              .try_as<Controls::Grid>();
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
    if (!root || root == originalParent) {
        Wh_Log(L"Taskbar root Grid not found");
        return false;
    }

    auto element = clock.as<UIElement>();
    uint32_t originalIndex = 0;
    if (!originalParent.Children().IndexOf(element, originalIndex)) {
        return false;
    }

    double layoutReservedWidth =
        CalculateNativeClockLayoutWidth(content, clock);
    auto widgets = FindDescendantByName(root, L"AugmentedEntryPointButton");

    MovedClockData data{
        .clock = clock,
        .originalParent = originalParent,
        .taskbarRoot = root,
        .widgets = widgets,
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
    data.leftHost.HorizontalAlignment(HorizontalAlignment::Left);
    data.leftHost.VerticalAlignment(VerticalAlignment::Stretch);
    data.leftHost.Margin(Thickness{GetLeftHostOffset(root), 0, 0, 0});
    Controls::Grid::SetColumn(data.leftHost, 0);
    Controls::Grid::SetColumnSpan(
        data.leftHost,
        std::max<int>(1, root.ColumnDefinitions().Size()));
    Controls::Grid::SetRow(data.leftHost, 0);
    Controls::Grid::SetRowSpan(
        data.leftHost, std::max<int>(1, root.RowDefinitions().Size()));

    MovedClocks().push_back(std::move(data));

    try {
        originalParent.Children().RemoveAt(originalIndex);
        Controls::Grid::SetColumn(clock, 0);
        Controls::Grid::SetColumnSpan(clock, 1);
        Controls::Grid::SetRow(clock, 0);
        Controls::Grid::SetRowSpan(clock, 1);
        clock.HorizontalAlignment(HorizontalAlignment::Stretch);
        clock.VerticalAlignment(VerticalAlignment::Stretch);
        clock.Margin(Thickness{0, 0, 0, 0});

        auto& moved = MovedClocks().back();
        moved.leftHost.Children().Append(element);
        root.Children().Append(moved.leftHost.as<UIElement>());
        RegisterLeftHostTracking(moved, clock);
        UpdateLeftHostOffset(moved);

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

            return data.layoutReservedWidth;
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

DeferredClockData& GetDeferredData(FrameworkElement content) {
    auto& deferredClocks = DeferredClocks();
    for (auto it = deferredClocks.begin(); it != deferredClocks.end();) {
        auto element = it->content.get();
        if (!element) {
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

void RememberClockContent(FrameworkElement content) {
    GetDeferredData(content);
}

void RemoveLoadedHandler(FrameworkElement content) {
    std::optional<winrt::event_token> token;
    for (auto& data : DeferredClocks()) {
        if (data.content.get() == content && data.loadedToken) {
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
        if (data.content.get() == content && data.layoutUpdatedToken) {
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

    auto originalParent = Media::VisualTreeHelper::GetParent(clock)
                              .try_as<Controls::Grid>();
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
    constexpr ULONGLONG kLayoutReadyTimeoutMs = 10000;
    constexpr uint32_t kLayoutReadyMaxAttempts = 600;

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

    auto& data = GetDeferredData(content);
    if (!data.layoutUpdatedToken) {
        data.layoutUpdatedStartTick = GetTickCount64();
        data.layoutUpdatedAttempts = 0;
        data.layoutUpdatedToken = content.LayoutUpdated(handler);
        content.InvalidateMeasure();
    }
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

    auto& data = GetDeferredData(content);
    if (!data.loadedToken) {
        data.loadedToken = content.Loaded(handler);
    }
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

    if (!TaskbarUsesCenteredAlignment()) {
        RemoveDeferredHandlers(content);
        RestoreClock(clock);
    } else if (content.IsLoaded()) {
        RemoveLoadedHandler(content);
        if (ClockIsAlreadyMoved(clock)) {
            RemoveLayoutUpdatedHandler(content);
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

    for (auto& data : DeferredClocks()) {
        try {
            if (auto content = data.content.get()) {
                if (data.loadedToken) {
                    content.Loaded(*data.loadedToken);
                }
                if (data.layoutUpdatedToken) {
                    content.LayoutUpdated(*data.layoutUpdatedToken);
                }
            }
        } catch (...) {
            Wh_Log(L"Deferred handler cleanup failed: %08X",
                   winrt::to_hresult());
        }
        data.loadedToken.reset();
        data.layoutUpdatedToken.reset();
    }
}

void RestoreAllMovedClocksOnTaskbarThread() {
    if (!g_movedClocks) {
        return;
    }

    auto& movedClocks = MovedClocks();
    while (!movedClocks.empty()) {
        auto clock = movedClocks.front().clock.get();
        if (!clock) {
            CleanupMovedClockData();
            continue;
        }

        try {
            if (!RestoreClock(clock)) {
                RemoveLeftHost(movedClocks.front());
                movedClocks.erase(movedClocks.begin());
            }
        } catch (...) {
            Wh_Log(L"Clock restore failed: %08X", winrt::to_hresult());
            try {
                RemoveLeftHost(movedClocks.front());
            } catch (...) {
            }
            movedClocks.erase(movedClocks.begin());
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

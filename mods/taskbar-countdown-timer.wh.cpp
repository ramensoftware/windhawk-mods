// ==WindhawkMod==
// @id              taskbar-countdown-timer
// @name            Taskbar Countdown Timer
// @description     A simple countdown timer integrated into the Windows 11 taskbar (Windows 11 only)
// @version         1.9.1
// @author          Richi
// @github          https://github.com/richilp
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ldwmapi -lgdi32 -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Countdown Timer

Adds a compact countdown timer directly to the Windows 11 taskbar.

## Features

- Create a countdown in minutes from the taskbar
- Add a custom reminder message
- Live countdown display in the taskbar
- Cancel an active timer
- Snooze a completed timer
- Completion sound
- Reliable completion alert that doesn't depend on the taskbar XAML tree
- Timer state survives Explorer restarts and mod reloads
- Native XAML flyout for timer configuration
- Completion alert follows the Windows light/dark app theme
- No external application required

## How to use

1. Click **⏱ Timer** on the taskbar.
2. Enter a reminder and duration.
3. Click **Start**.
4. The taskbar button shows the remaining time.
5. When the countdown finishes, a completion alert appears with **Snooze** and **Dismiss**.

The mod supports one active timer at a time and currently places its button on the primary taskbar.

![Taskbar Countdown Timer](https://raw.githubusercontent.com/richilp/taskbar-countdown-timer/main/16c5364b-0834-4a83-8796-1f6b1df4f702.png)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- defaultMinutes: 20
  $name: Default timer duration (minutes)
  $description: Default duration shown when creating a new timer.

- defaultSnoozeMinutes: 5
  $name: Default snooze duration (minutes)
  $description: Default duration shown in the snooze field.

- maximumMinutes: 1440
  $name: Maximum duration (minutes)
  $description: Maximum value accepted for timers and snoozes.

- buttonLabel: "⏱ Timer"
  $name: Idle taskbar button label
  $description: Text shown on the taskbar when no timer is running.

- completionSound: true
  $name: Play completion sound
  $description: Play a Windows notification sound when the timer finishes.
*/
// ==/WindhawkModSettings==


#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <dwmapi.h>
#include <uxtheme.h>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <list>
#include <optional>
#include <cwchar>
#include <string>
#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
namespace Controls = winrt::Windows::UI::Xaml::Controls;


// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

[[clang::no_destroy]] static Button g_timerButton{nullptr};
[[clang::no_destroy]] static TextBlock g_timerText{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_countdownTimer{nullptr};
[[clang::no_destroy]] static ColumnDefinition g_timerColumn{nullptr};

[[clang::no_destroy]] static Flyout g_timerFlyout{nullptr};
[[clang::no_destroy]] static TextBox g_timerReminderBox{nullptr};
[[clang::no_destroy]] static TextBox g_timerMinutesBox{nullptr};
[[clang::no_destroy]] static Button g_timerStartButton{nullptr};
[[clang::no_destroy]] static Button g_timerCancelButton{nullptr};

[[clang::no_destroy]] static std::optional<
    std::list<FrameworkElement::Loaded_revoker>>
    g_loadedRevokers{std::in_place};

static winrt::event_token g_timerButtonClickToken{};
static winrt::event_token g_timerTickToken{};

static std::atomic<int> g_secondsRemaining{0};
static std::atomic<ULONGLONG> g_deadlineUtc100ns{0};
static std::atomic_bool g_pendingFinished{false};

static SRWLOCK g_reminderLock = SRWLOCK_INIT;
static wchar_t g_reminder[256]{};

struct ModSettings {
    int defaultMinutes = 20;
    int defaultSnoozeMinutes = 5;
    int maximumMinutes = 1440;
    bool completionSound = true;
    std::wstring buttonLabel = L"⏱ Timer";
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;

static HANDLE g_timerStopEvent = nullptr;
static HANDLE g_timerRearmEvent = nullptr;
static HANDLE g_timerWaitable = nullptr;
static HANDLE g_timerWorkerThread = nullptr;
static std::atomic_bool g_timerWorkerStarted{false};

// Completion alert is independent from the taskbar XAML tree.
static std::atomic<HWND> g_finishedAlertWnd{nullptr};
static std::atomic<HANDLE> g_finishedAlertThread{nullptr};
static HANDLE g_finishedAlertStopEvent = nullptr;

static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic<DWORD> g_taskbarThreadId{0};
static std::atomic_bool g_buttonInjected{false};
static std::atomic_bool g_systemTrayModuleHooked{false};
static std::atomic<HMODULE> g_systemTrayModuleAttempted{nullptr};

static std::atomic_bool g_unloading{false};
static HANDLE g_retryStopEvent = nullptr;
static HANDLE g_retryThread = nullptr;

static void ApplyTimerButtonIfAvailable();
static bool EnsureTimerWorkerStarted();

// -----------------------------------------------------------------------------
// XAML helpers
// -----------------------------------------------------------------------------

static FrameworkElement FindChildRecursive(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& callback,
    int maxDepth = 20)
{
    if (!element || maxDepth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(element, i)
                .try_as<FrameworkElement>();

        if (!child) {
            continue;
        }

        if (callback(child)) {
            return child;
        }

        auto found =
            FindChildRecursive(
                child,
                callback,
                maxDepth - 1
            );

        if (found) {
            return found;
        }
    }

    return nullptr;
}


// -----------------------------------------------------------------------------
// Taskbar window
// -----------------------------------------------------------------------------

static HWND FindCurrentProcessTaskbarWnd()
{
    HWND result = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD pid = 0;
            WCHAR className[32];

            if (GetWindowThreadProcessId(
                    hWnd,
                    &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(
                    hWnd,
                    className,
                    ARRAYSIZE(className)) &&
                _wcsicmp(
                    className,
                    L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND*>(lParam) =
                    hWnd;

                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result)
    );

    if (result) {
        DWORD threadId = GetWindowThreadProcessId(
            result,
            nullptr
        );

        if (threadId) {
            g_taskbarThreadId.store(threadId);
        }
    }

    return result;
}


static HWND FindAnyWindowOnTaskbarThread()
{
    DWORD threadId =
        g_taskbarThreadId.load();

    if (!threadId) {
        return nullptr;
    }

    HWND result = nullptr;

    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            *reinterpret_cast<HWND*>(lParam) =
                hWnd;

            return FALSE;
        },
        reinterpret_cast<LPARAM>(&result)
    );

    return result;
}


// -----------------------------------------------------------------------------
// taskbar.dll symbols
// -----------------------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t =
    void* (WINAPI*)(void* pThis, void* result);

static CTaskBand_GetTaskbarHost_t
    CTaskBand_GetTaskbarHost_Original = nullptr;


using TaskbarHost_FrameHeight_t =
    int (WINAPI*)(void* pThis);

static TaskbarHost_FrameHeight_t
    TaskbarHost_FrameHeight_Original = nullptr;


using std__Ref_count_base__Decref_t =
    void (WINAPI*)(void* pThis);

static std__Ref_count_base__Decref_t
    std__Ref_count_base__Decref_Original = nullptr;


static void* CTaskBand_ITaskListWndSite_vftable =
    nullptr;


static bool HookTaskbarDllSymbols()
{
    HMODULE module = LoadLibraryExW(
        L"taskbar.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32
    );

    if (!module) {
        Wh_Log(
            L"ERROR: Could not load taskbar.dll"
        );

        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"
            },
            &CTaskBand_ITaskListWndSite_vftable
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"
            },
            &CTaskBand_GetTaskbarHost_Original
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"
            },
            &TaskbarHost_FrameHeight_Original
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"
            },
            &std__Ref_count_base__Decref_Original
        },
    };

    return WindhawkUtils::HookSymbols(
        module,
        taskbarDllHooks,
        ARRAYSIZE(taskbarDllHooks)
    );
}


// -----------------------------------------------------------------------------
// Get taskbar XAML root
// -----------------------------------------------------------------------------

static XamlRoot GetTaskbarXamlRoot(
    HWND hTaskbarWnd)
{
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
    {
        Wh_Log(
            L"ERROR: Required symbols are missing"
        );

        return nullptr;
    }

    HWND hTaskSwWnd =
        reinterpret_cast<HWND>(
            GetPropW(
                hTaskbarWnd,
                L"TaskbandHWND"
            )
        );

    if (!hTaskSwWnd) {
        Wh_Log(
            L"ERROR: TaskbandHWND not found"
        );

        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(
            GetWindowLongPtrW(
                hTaskSwWnd,
                0
            )
        );

    if (!taskBand) {
        Wh_Log(
            L"ERROR: CTaskBand object not found"
        );

        return nullptr;
    }

    void* taskBandForSite = taskBand;

    for (int i = 0;
         *reinterpret_cast<void**>(
             taskBandForSite) !=
             CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20) {
            Wh_Log(
                L"ERROR: ITaskListWndSite vftable not found"
            );

            return nullptr;
        }

        taskBandForSite =
            reinterpret_cast<void**>(
                taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};

    CTaskBand_GetTaskbarHost_Original(
        taskBandForSite,
        taskbarHostSharedPtr
    );

    if (!taskbarHostSharedPtr[0] ||
        !taskbarHostSharedPtr[1])
    {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(
                taskbarHostSharedPtr[1]
            );
        }

        Wh_Log(
            L"ERROR: TaskbarHost not obtained"
        );

        return nullptr;
    }

    size_t offset = 0x48;

#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(
            TaskbarHost_FrameHeight_Original
        );

    if (code[0] == 0x48 &&
        code[1] == 0x83 &&
        code[2] == 0xEC &&
        code[4] == 0x48 &&
        code[5] == 0x83 &&
        code[6] == 0xC1 &&
        code[7] <= 0x7F)
    {
        offset = code[7];
    }
    else {
        Wh_Log(
            L"Unsupported TaskbarHost::FrameHeight"
        );
        std__Ref_count_base__Decref_Original(
            taskbarHostSharedPtr[1]
        );
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* p =
        reinterpret_cast<const DWORD*>(
            TaskbarHost_FrameHeight_Original
        );

    if (p[0] == 0xD503237F &&
        (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD &&
        (p[3] & 0xFFF00FE0) == 0xF8400C00)
    {
        offset = (p[3] >> 12) & 0xFF;
    }
    else {
        Wh_Log(
            L"Unsupported TaskbarHost::FrameHeight"
        );
        std__Ref_count_base__Decref_Original(
            taskbarHostSharedPtr[1]
        );
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* unknown =
        *reinterpret_cast<IUnknown**>(
            reinterpret_cast<BYTE*>(
                taskbarHostSharedPtr[0]
            ) + offset
        );

    if (!unknown) {
        Wh_Log(
            L"ERROR: Taskbar XAML object not found"
        );

        std__Ref_count_base__Decref_Original(
            taskbarHostSharedPtr[1]
        );

        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;

    unknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarElement)
    );

    auto result =
        taskbarElement
            ? taskbarElement.XamlRoot()
            : nullptr;

    std__Ref_count_base__Decref_Original(
        taskbarHostSharedPtr[1]
    );

    return result;
}


// -----------------------------------------------------------------------------
// Execute code on taskbar XAML thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc_t =
    void (*)(void*);

static bool RunFromWindowThread(
    HWND hWnd,
    RunFromWindowThreadProc_t proc,
    void* procParam)
{
    static const UINT message =
        RegisterWindowMessageW(
            L"Windhawk_RunFromWindowThread_" WH_MOD_ID
        );

    struct Param
    {
        RunFromWindowThreadProc_t proc;
        void* procParam;
        bool invoked;
    };

    DWORD threadId =
        GetWindowThreadProcessId(
            hWnd,
            nullptr
        );

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode,
           WPARAM wParam,
           LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<
                        const CWPSTRUCT*>(
                            lParam
                        );

                if (cwp->message == message) {
                    auto* param =
                        reinterpret_cast<Param*>(
                            cwp->lParam
                        );

                    if (!param->invoked) {
                        param->proc(
                            param->procParam
                        );

                        param->invoked = true;
                    }
                }
            }

            return CallNextHookEx(
                nullptr,
                nCode,
                wParam,
                lParam
            );
        },
        nullptr,
        threadId
    );

    if (!hook) {
        return false;
    }

    Param param{
        proc,
        procParam,
        false
    };

    SendMessageW(
        hWnd,
        message,
        0,
        reinterpret_cast<LPARAM>(&param)
    );

    UnhookWindowsHookEx(hook);

    return param.invoked;
}


// -----------------------------------------------------------------------------
// Settings and persistent timer state
// -----------------------------------------------------------------------------

static ModSettings GetSettingsSnapshot()
{
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings result = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return result;
}


static void LoadSettings()
{
    ModSettings updated;

    updated.defaultMinutes =
        std::clamp(
            Wh_GetIntSetting(L"defaultMinutes"),
            1,
            1440
        );

    updated.defaultSnoozeMinutes =
        std::clamp(
            Wh_GetIntSetting(L"defaultSnoozeMinutes"),
            1,
            1440
        );

    updated.maximumMinutes =
        std::clamp(
            Wh_GetIntSetting(L"maximumMinutes"),
            1,
            10080
        );

    if (updated.defaultMinutes > updated.maximumMinutes) {
        updated.defaultMinutes = updated.maximumMinutes;
    }

    if (updated.defaultSnoozeMinutes > updated.maximumMinutes) {
        updated.defaultSnoozeMinutes = updated.maximumMinutes;
    }

    updated.completionSound =
        Wh_GetIntSetting(L"completionSound") != 0;

    PCWSTR label =
        Wh_GetStringSetting(L"buttonLabel");

    if (label && label[0]) {
        updated.buttonLabel = label;
    }

    Wh_FreeStringSetting(label);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = updated;
    ReleaseSRWLockExclusive(&g_settingsLock);
}


static ULONGLONG GetUtcFileTimeNow()
{
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER value{};
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;

    return value.QuadPart;
}


static void SetSharedReminder(
    const wchar_t* reminder)
{
    AcquireSRWLockExclusive(&g_reminderLock);

    wcsncpy_s(
        g_reminder,
        reminder ? reminder : L"",
        _TRUNCATE
    );

    ReleaseSRWLockExclusive(&g_reminderLock);
}


static void GetSharedReminder(
    wchar_t (&buffer)[256])
{
    AcquireSRWLockShared(&g_reminderLock);

    wcsncpy_s(
        buffer,
        g_reminder,
        _TRUNCATE
    );

    ReleaseSRWLockShared(&g_reminderLock);
}


static void ClearPersistedTimer()
{
    Wh_DeleteValue(L"deadlineUtc100ns");
    Wh_DeleteValue(L"reminder");
}


static void PersistTimer(
    ULONGLONG deadline,
    const wchar_t* reminder)
{
    wchar_t deadlineText[32]{};

    swprintf_s(
        deadlineText,
        L"%llu",
        static_cast<unsigned long long>(deadline)
    );

    Wh_SetStringValue(
        L"deadlineUtc100ns",
        deadlineText
    );

    Wh_SetStringValue(
        L"reminder",
        reminder ? reminder : L""
    );
}


static int RemainingSeconds(
    ULONGLONG deadline)
{
    if (!deadline) {
        return 0;
    }

    ULONGLONG now =
        GetUtcFileTimeNow();

    if (now >= deadline) {
        return 0;
    }

    ULONGLONG delta =
        deadline - now;

    ULONGLONG seconds =
        (delta + 9999999ULL) /
        10000000ULL;

    return static_cast<int>(
        std::min<ULONGLONG>(
            seconds,
            static_cast<ULONGLONG>(INT_MAX)
        )
    );
}


static std::wstring FormatCountdown(
    int totalSeconds)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    wchar_t text[64]{};

    if (totalSeconds >= 3600) {
        int hours = totalSeconds / 3600;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        swprintf_s(
            text,
            L"⏱ %d:%02d:%02d",
            hours,
            minutes,
            seconds
        );
    }
    else {
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        swprintf_s(
            text,
            L"⏱ %02d:%02d",
            minutes,
            seconds
        );
    }

    return text;
}


// -----------------------------------------------------------------------------
// XAML timer display and flyouts
// -----------------------------------------------------------------------------

static void SetIdleTaskbarText()
{
    if (!g_timerText) {
        return;
    }

    ModSettings settings =
        GetSettingsSnapshot();

    g_timerText.Text(
        settings.buttonLabel
    );
}


static void RefreshTaskbarCountdown()
{
    if (!g_timerText) {
        return;
    }

    ULONGLONG deadline =
        g_deadlineUtc100ns.load();

    int remaining =
        RemainingSeconds(deadline);

    g_secondsRemaining.store(remaining);

    if (remaining > 0) {
        g_timerText.Text(
            FormatCountdown(remaining)
        );
    }
    else {
        SetIdleTaskbarText();
    }
}


static void HideAndReleaseFlyouts()
{
    if (g_timerFlyout) {
        g_timerFlyout.Hide();
    }

    g_timerReminderBox = nullptr;
    g_timerMinutesBox = nullptr;
    g_timerStartButton = nullptr;
    g_timerCancelButton = nullptr;
    g_timerFlyout = nullptr;
}


static bool ParseMinutes(
    TextBox const& box,
    int maximum,
    int& minutes)
{
    if (!box) {
        return false;
    }

    std::wstring value =
        box.Text().c_str();

    wchar_t* end = nullptr;
    long parsed =
        wcstol(
            value.c_str(),
            &end,
            10
        );

    if (!end ||
        end == value.c_str() ||
        *end != L'\0' ||
        parsed <= 0 ||
        parsed > maximum)
    {
        return false;
    }

    minutes =
        static_cast<int>(parsed);

    return true;
}


static void SignalTimerWorker()
{
    if (g_timerRearmEvent) {
        SetEvent(g_timerRearmEvent);
    }
}


static bool ArmTimer(
    int minutes,
    const wchar_t* reminder)
{
    ModSettings settings =
        GetSettingsSnapshot();

    if (minutes <= 0 ||
        minutes > settings.maximumMinutes ||
        g_unloading.load())
    {
        return false;
    }

    if (!EnsureTimerWorkerStarted()) {
        Wh_Log(
            L"ERROR: Timer worker isn't available"
        );

        return false;
    }

    ULONGLONG seconds =
        static_cast<ULONGLONG>(minutes) *
        60ULL;

    ULONGLONG deadline =
        GetUtcFileTimeNow() +
        seconds * 10000000ULL;

    const wchar_t* reminderToUse =
        reminder && reminder[0]
            ? reminder
            : L"Timer finished";

    SetSharedReminder(reminderToUse);

    g_deadlineUtc100ns.store(deadline);
    g_secondsRemaining.store(
        static_cast<int>(
            std::min<ULONGLONG>(
                seconds,
                static_cast<ULONGLONG>(INT_MAX)
            )
        )
    );

    g_pendingFinished.store(false);

    PersistTimer(
        deadline,
        reminderToUse
    );

    SignalTimerWorker();

    HWND taskbar =
        g_taskbarWnd.load();

    if (!taskbar ||
        !IsWindow(taskbar))
    {
        taskbar =
            FindCurrentProcessTaskbarWnd();

        if (taskbar) {
            g_taskbarWnd.store(taskbar);
        }
    }

    if (taskbar) {
        RunFromWindowThread(
            taskbar,
            [](
                void*)
            {
                if (g_countdownTimer) {
                    g_countdownTimer.Start();
                }

                RefreshTaskbarCountdown();
            },
            nullptr
        );
    }

    Wh_Log(
        L"Timer armed: '%s' - %d minute(s)",
        reminderToUse,
        minutes
    );

    return true;
}


static void CancelTimer()
{
    g_deadlineUtc100ns.store(0);
    g_secondsRemaining.store(0);
    g_pendingFinished.store(false);

    ClearPersistedTimer();
    SignalTimerWorker();

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
    }

    SetIdleTaskbarText();

    Wh_Log(L"Timer cancelled");
}


static StackPanel MakeFlyoutPanel()
{
    StackPanel panel;
    panel.Width(340);
    panel.Margin(Thickness{16, 14, 16, 14});
    return panel;
}


static TextBlock MakeLabel(
    const wchar_t* text)
{
    TextBlock label;
    label.Text(text);
    label.Margin(Thickness{0, 0, 0, 4});
    return label;
}


static Button MakeActionButton(
    const wchar_t* text)
{
    Button button;
    button.Content(
        winrt::box_value(text)
    );
    button.MinWidth(120);
    button.Margin(
        Thickness{0, 8, 8, 0}
    );
    return button;
}


static void BuildTimerFlyout()
{
    if (g_timerFlyout) {
        return;
    }

    Flyout flyout;
    flyout.Placement(
        Controls::Primitives::
            FlyoutPlacementMode::Top
    );
    flyout.ShouldConstrainToRootBounds(false);

    StackPanel panel =
        MakeFlyoutPanel();

    auto reminderLabel =
        MakeLabel(L"Reminder");

    panel.Children().Append(
        reminderLabel
    );

    TextBox reminderBox;
    reminderBox.PlaceholderText(
        L"What should I remind you about?"
    );
    reminderBox.Margin(
        Thickness{0, 0, 0, 10}
    );

    panel.Children().Append(
        reminderBox
    );

    auto minutesLabel =
        MakeLabel(L"Minutes");

    panel.Children().Append(
        minutesLabel
    );

    TextBox minutesBox;
    minutesBox.Margin(
        Thickness{0, 0, 0, 4}
    );

    panel.Children().Append(
        minutesBox
    );

    StackPanel actions;
    actions.Orientation(
        Orientation::Horizontal
    );

    Button startButton =
        MakeActionButton(L"Start");

    Button cancelButton =
        MakeActionButton(L"Cancel timer");

    actions.Children().Append(
        startButton
    );

    actions.Children().Append(
        cancelButton
    );

    panel.Children().Append(
        actions
    );

    startButton.Click(
        [](
            auto const&,
            RoutedEventArgs const&)
        {
            if (g_unloading.load() ||
                !g_timerReminderBox ||
                !g_timerMinutesBox)
            {
                return;
            }

            ModSettings settings =
                GetSettingsSnapshot();

            int minutes = 0;

            if (!ParseMinutes(
                    g_timerMinutesBox,
                    settings.maximumMinutes,
                    minutes))
            {
                g_timerMinutesBox.SelectAll();
                g_timerMinutesBox.Focus(
                    FocusState::Programmatic
                );
                return;
            }

            std::wstring reminder =
                g_timerReminderBox.Text().c_str();

            if (!ArmTimer(
                    minutes,
                    reminder.c_str()))
            {
                return;
            }

            if (g_timerFlyout) {
                g_timerFlyout.Hide();
            }
        }
    );

    cancelButton.Click(
        [](
            auto const&,
            RoutedEventArgs const&)
        {
            if (g_unloading.load()) {
                return;
            }

            CancelTimer();

            if (g_timerFlyout) {
                g_timerFlyout.Hide();
            }
        }
    );

    flyout.Content(panel);

    g_timerReminderBox =
        reminderBox;
    g_timerMinutesBox =
        minutesBox;
    g_timerStartButton =
        startButton;
    g_timerCancelButton =
        cancelButton;
    g_timerFlyout =
        flyout;
}


static void ShowTimerFlyout()
{
    if (!g_timerButton ||
        g_unloading.load())
    {
        return;
    }

    BuildTimerFlyout();

    if (!g_timerFlyout ||
        !g_timerReminderBox ||
        !g_timerMinutesBox ||
        !g_timerStartButton ||
        !g_timerCancelButton)
    {
        return;
    }

    ModSettings settings =
        GetSettingsSnapshot();

    int remaining =
        RemainingSeconds(
            g_deadlineUtc100ns.load()
        );

    if (remaining > 0) {
        wchar_t reminder[256]{};
        GetSharedReminder(reminder);

        g_timerReminderBox.Text(
            reminder
        );

        wchar_t minutesText[32]{};
        swprintf_s(
            minutesText,
            L"%d",
            (remaining + 59) / 60
        );

        g_timerMinutesBox.Text(
            minutesText
        );

        g_timerReminderBox.IsEnabled(false);
        g_timerMinutesBox.IsEnabled(false);
        g_timerStartButton.IsEnabled(false);
        g_timerCancelButton.IsEnabled(true);
    }
    else {
        g_timerReminderBox.Text(L"");

        wchar_t minutesText[32]{};
        swprintf_s(
            minutesText,
            L"%d",
            settings.defaultMinutes
        );

        g_timerMinutesBox.Text(
            minutesText
        );

        g_timerReminderBox.IsEnabled(true);
        g_timerMinutesBox.IsEnabled(true);
        g_timerStartButton.IsEnabled(true);
        g_timerCancelButton.IsEnabled(false);
    }

    g_timerFlyout.ShowAt(
        g_timerButton
    );
}


// -----------------------------------------------------------------------------
// Reliable completion alert
// -----------------------------------------------------------------------------

constexpr int IDC_ALERT_REMINDER = 2100;
constexpr int IDC_ALERT_SNOOZE_LABEL = 2101;
constexpr int IDC_ALERT_SNOOZE_MINUTES = 2102;
constexpr int IDC_ALERT_SNOOZE = 2103;
constexpr int IDC_ALERT_DISMISS = 2104;

static constexpr wchar_t kFinishedAlertClass[] =
    L"TaskbarCountdownTimerFinishedAlert";

static HFONT g_finishedAlertFont = nullptr;
static HBRUSH g_finishedAlertBackgroundBrush = nullptr;
static HBRUSH g_finishedAlertEditBrush = nullptr;
static bool g_finishedAlertDark = true;

static bool AppsUseLightTheme()
{
    DWORD value = 0;
    DWORD size = sizeof(value);

    LONG result =
        RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            nullptr,
            &value,
            &size
        );

    if (result != ERROR_SUCCESS) {
        return false;
    }

    return value != 0;
}

static void EnsureFinishedAlertResources(
    UINT dpi)
{
    g_finishedAlertDark =
        !AppsUseLightTheme();

    if (g_finishedAlertBackgroundBrush) {
        DeleteObject(
            g_finishedAlertBackgroundBrush
        );
        g_finishedAlertBackgroundBrush = nullptr;
    }

    if (g_finishedAlertEditBrush) {
        DeleteObject(
            g_finishedAlertEditBrush
        );
        g_finishedAlertEditBrush = nullptr;
    }

    COLORREF background =
        g_finishedAlertDark
            ? RGB(32, 32, 32)
            : RGB(249, 249, 249);

    COLORREF editBackground =
        g_finishedAlertDark
            ? RGB(45, 45, 45)
            : RGB(255, 255, 255);

    g_finishedAlertBackgroundBrush =
        CreateSolidBrush(background);

    g_finishedAlertEditBrush =
        CreateSolidBrush(editBackground);

    if (g_finishedAlertFont) {
        DeleteObject(
            g_finishedAlertFont
        );
        g_finishedAlertFont = nullptr;
    }

    g_finishedAlertFont =
        CreateFontW(
            -MulDiv(
                11,
                dpi ? dpi : 96,
                72
            ),
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );
}

static void ApplyFinishedAlertTheme(
    HWND hWnd,
    UINT dpi)
{
    EnsureFinishedAlertResources(dpi);

    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL = 20;
    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL = 33;
    constexpr int DWMWCP_ROUND_LOCAL = 2;

    BOOL dark =
        g_finishedAlertDark
            ? TRUE
            : FALSE;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL,
        &dark,
        sizeof(dark)
    );

    int corner =
        DWMWCP_ROUND_LOCAL;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL,
        &corner,
        sizeof(corner)
    );
}

static void ThemeFinishedAlertChild(
    HWND child)
{
    if (!child) {
        return;
    }

    if (g_finishedAlertFont) {
        SendMessageW(
            child,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(
                g_finishedAlertFont
            ),
            TRUE
        );
    }

    wchar_t className[32]{};

    if (!GetClassNameW(
            child,
            className,
            ARRAYSIZE(className)))
    {
        return;
    }

    if (_wcsicmp(
            className,
            L"Button") == 0)
    {
        SetWindowTheme(
            child,
            g_finishedAlertDark
                ? L"DarkMode_Explorer"
                : L"Explorer",
            nullptr
        );
    }
    else if (_wcsicmp(
                 className,
                 L"Edit") == 0)
    {
        SetWindowTheme(
            child,
            g_finishedAlertDark
                ? L"DarkMode_CFD"
                : L"Explorer",
            nullptr
        );
    }
}

struct FinishedAlertData
{
    wchar_t reminder[256];
    int defaultSnoozeMinutes;
    int maximumMinutes;
};

static int ScaleAlertValue(int value, UINT dpi)
{
    return MulDiv(value, dpi ? dpi : 96, 96);
}

static void PositionFinishedAlert(HWND hWnd, UINT dpi)
{
    HWND taskbar = g_taskbarWnd.load();

    HMONITOR monitor =
        MonitorFromWindow(
            taskbar ? taskbar : hWnd,
            MONITOR_DEFAULTTONEAREST
        );

    MONITORINFO info{sizeof(info)};

    if (!GetMonitorInfoW(monitor, &info)) {
        return;
    }

    int width = ScaleAlertValue(380, dpi);
    int height = ScaleAlertValue(220, dpi);
    int margin = ScaleAlertValue(10, dpi);

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        info.rcWork.right - width - margin,
        info.rcWork.bottom - height - margin,
        width,
        height,
        SWP_NOACTIVATE
    );
}

static void LayoutFinishedAlert(HWND hWnd, UINT dpi)
{
    struct Item {
        int id, x, y, w, h;
    };

    constexpr Item items[] = {
        {IDC_ALERT_REMINDER,        20,  22, 340, 46},
        {IDC_ALERT_SNOOZE_LABEL,    20,  88, 155, 22},
        {IDC_ALERT_SNOOZE_MINUTES, 184,  84,  78, 30},
        {IDC_ALERT_SNOOZE,          20, 145, 160, 36},
        {IDC_ALERT_DISMISS,        200, 145, 160, 36},
    };

    for (const auto& item : items) {
        HWND child = GetDlgItem(hWnd, item.id);

        if (!child) {
            continue;
        }

        SetWindowPos(
            child,
            nullptr,
            ScaleAlertValue(item.x, dpi),
            ScaleAlertValue(item.y, dpi),
            ScaleAlertValue(item.w, dpi),
            ScaleAlertValue(item.h, dpi),
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }
}

static LRESULT CALLBACK FinishedAlertWndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
    {
        auto* create =
            reinterpret_cast<CREATESTRUCTW*>(lParam);

        auto* data =
            reinterpret_cast<FinishedAlertData*>(
                create->lpCreateParams
            );

        SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(data)
        );

        UINT dpi =
            GetDpiForWindow(hWnd);

        if (!dpi) {
            dpi = 96;
        }

        ApplyFinishedAlertTheme(
            hWnd,
            dpi
        );

        CreateWindowExW(
            0,
            L"STATIC",
            data && data->reminder[0]
                ? data->reminder
                : L"Timer finished",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(IDC_ALERT_REMINDER),
            nullptr,
            nullptr
        );

        CreateWindowExW(
            0,
            L"STATIC",
            L"Snooze minutes:",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(IDC_ALERT_SNOOZE_LABEL),
            nullptr,
            nullptr
        );

        wchar_t snoozeText[32]{};
        swprintf_s(
            snoozeText,
            L"%d",
            data ? data->defaultSnoozeMinutes : 5
        );

        CreateWindowExW(
            0,
            L"EDIT",
            snoozeText,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(IDC_ALERT_SNOOZE_MINUTES),
            nullptr,
            nullptr
        );

        CreateWindowExW(
            0,
            L"BUTTON",
            L"Snooze",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(IDC_ALERT_SNOOZE),
            nullptr,
            nullptr
        );

        CreateWindowExW(
            0,
            L"BUTTON",
            L"Dismiss",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(IDC_ALERT_DISMISS),
            nullptr,
            nullptr
        );

        EnumChildWindows(
            hWnd,
            [](
                HWND child,
                LPARAM) -> BOOL
            {
                ThemeFinishedAlertChild(
                    child
                );

                return TRUE;
            },
            0
        );

        LayoutFinishedAlert(
            hWnd,
            dpi
        );

        return 0;
    }

    case WM_DPICHANGED:
    {
        UINT dpi = HIWORD(wParam);
        auto* suggested = reinterpret_cast<RECT*>(lParam);

        SetWindowPos(
            hWnd,
            nullptr,
            suggested->left,
            suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE
        );

        ApplyFinishedAlertTheme(
            hWnd,
            dpi
        );

        EnumChildWindows(
            hWnd,
            [](
                HWND child,
                LPARAM) -> BOOL
            {
                ThemeFinishedAlertChild(
                    child
                );

                return TRUE;
            },
            0
        );

        LayoutFinishedAlert(
            hWnd,
            dpi
        );

        return 0;
    }

    case WM_ERASEBKGND:
    {
        RECT rect{};
        GetClientRect(hWnd, &rect);

        FillRect(
            reinterpret_cast<HDC>(wParam),
            &rect,
            g_finishedAlertBackgroundBrush
        );

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        COLORREF textColor =
            g_finishedAlertDark
                ? RGB(245, 245, 245)
                : RGB(25, 25, 25);

        COLORREF background =
            g_finishedAlertDark
                ? RGB(32, 32, 32)
                : RGB(249, 249, 249);

        SetTextColor(
            hdc,
            textColor
        );

        SetBkColor(
            hdc,
            background
        );

        return reinterpret_cast<LRESULT>(
            g_finishedAlertBackgroundBrush
        );
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        COLORREF textColor =
            g_finishedAlertDark
                ? RGB(245, 245, 245)
                : RGB(25, 25, 25);

        COLORREF background =
            g_finishedAlertDark
                ? RGB(45, 45, 45)
                : RGB(255, 255, 255);

        SetTextColor(
            hdc,
            textColor
        );

        SetBkColor(
            hdc,
            background
        );

        return reinterpret_cast<LRESULT>(
            g_finishedAlertEditBrush
        );
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);

        if (id == IDOK) {
            id = IDC_ALERT_SNOOZE;
        } else if (id == IDCANCEL) {
            id = IDC_ALERT_DISMISS;
        }

        if (id == IDC_ALERT_SNOOZE) {
            auto* data =
                reinterpret_cast<FinishedAlertData*>(
                    GetWindowLongPtrW(hWnd, GWLP_USERDATA)
                );

            wchar_t minutesText[32]{};

            GetWindowTextW(
                GetDlgItem(hWnd, IDC_ALERT_SNOOZE_MINUTES),
                minutesText,
                ARRAYSIZE(minutesText)
            );

            int minutes = _wtoi(minutesText);
            int maximum = data ? data->maximumMinutes : 1440;

            if (minutes <= 0 || minutes > maximum) {
                wchar_t label[96]{};

                swprintf_s(
                    label,
                    L"Snooze minutes (1-%d):",
                    maximum
                );

                SetWindowTextW(
                    GetDlgItem(hWnd, IDC_ALERT_SNOOZE_LABEL),
                    label
                );

                HWND edit =
                    GetDlgItem(hWnd, IDC_ALERT_SNOOZE_MINUTES);

                SetFocus(edit);
                SendMessageW(edit, EM_SETSEL, 0, -1);
                MessageBeep(MB_ICONWARNING);
                return 0;
            }

            const wchar_t* reminder =
                data && data->reminder[0]
                    ? data->reminder
                    : L"Timer finished";

            if (!ArmTimer(minutes, reminder)) {
                MessageBeep(MB_ICONERROR);
                return 0;
            }

            g_pendingFinished.store(false);
            DestroyWindow(hWnd);
            return 0;
        }

        if (id == IDC_ALERT_DISMISS) {
            g_pendingFinished.store(false);
            DestroyWindow(hWnd);
            return 0;
        }

        break;
    }

    case WM_CLOSE:
        g_pendingFinished.store(false);
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        g_finishedAlertWnd.store(nullptr);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static DWORD WINAPI FinishedAlertThreadProc(LPVOID param)
{
    std::unique_ptr<FinishedAlertData> data(
        reinterpret_cast<FinishedAlertData*>(param)
    );

    HMODULE modInstance = nullptr;

    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(FinishedAlertWndProc),
        &modInstance
    );

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = FinishedAlertWndProc;
    wc.hInstance = modInstance;
    wc.lpszClassName = kFinishedAlertClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground =
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();

        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(
                L"ERROR: Could not register completion alert class: %u",
                error
            );
            return 0;
        }
    }

    HWND taskbar = g_taskbarWnd.load();
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 96;

    if (!dpi) {
        dpi = 96;
    }

    HWND hWnd =
        CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            kFinishedAlertClass,
            L"Timer finished",
            WS_POPUP | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            ScaleAlertValue(380, dpi),
            ScaleAlertValue(220, dpi),
            nullptr,
            nullptr,
            modInstance,
            data.get()
        );

    if (!hWnd) {
        Wh_Log(
            L"ERROR: Could not create completion alert: %u",
            GetLastError()
        );

        UnregisterClassW(kFinishedAlertClass, modInstance);
        return 0;
    }

    g_finishedAlertWnd.store(hWnd);

    PositionFinishedAlert(hWnd, dpi);
    ShowWindow(hWnd, SW_SHOWNORMAL);

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW
    );

    FlashWindow(hWnd, TRUE);

    MSG msg{};

    while (!g_unloading.load()) {
        DWORD wait =
            MsgWaitForMultipleObjects(
                1,
                &g_finishedAlertStopEvent,
                FALSE,
                INFINITE,
                QS_ALLINPUT
            );

        if (wait == WAIT_OBJECT_0) {
            break;
        }

        if (wait != WAIT_OBJECT_0 + 1) {
            break;
        }

        while (PeekMessageW(
                   &msg,
                   nullptr,
                   0,
                   0,
                   PM_REMOVE))
        {
            if (msg.message == WM_QUIT) {
                goto ExitLoop;
            }

            if (!IsDialogMessageW(hWnd, &msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

ExitLoop:
    if (IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }

    g_finishedAlertWnd.store(nullptr);
    UnregisterClassW(kFinishedAlertClass, modInstance);
    return 0;
}

static void CloseFinishedAlertThreadIfExited()
{
    HANDLE thread = g_finishedAlertThread.load();

    if (!thread) {
        return;
    }

    if (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0) {
        return;
    }

    if (g_finishedAlertThread.compare_exchange_strong(
            thread,
            nullptr))
    {
        CloseHandle(thread);
    }
}

static bool ShowFinishedAlert(const wchar_t* reminder)
{
    CloseFinishedAlertThreadIfExited();

    if (HWND existing = g_finishedAlertWnd.load()) {
        if (IsWindow(existing)) {
            SetForegroundWindow(existing);
            FlashWindow(existing, TRUE);
            return true;
        }
    }

    HANDLE existingThread = g_finishedAlertThread.load();

    if (existingThread &&
        WaitForSingleObject(existingThread, 0) != WAIT_OBJECT_0)
    {
        return true;
    }

    if (!g_finishedAlertStopEvent) {
        g_finishedAlertStopEvent =
            CreateEventW(nullptr, TRUE, FALSE, nullptr);

        if (!g_finishedAlertStopEvent) {
            Wh_Log(
                L"ERROR: Could not create completion alert stop event"
            );
            return false;
        }
    }

    ResetEvent(g_finishedAlertStopEvent);

    auto data = std::make_unique<FinishedAlertData>();

    wcsncpy_s(
        data->reminder,
        reminder && reminder[0]
            ? reminder
            : L"Timer finished",
        _TRUNCATE
    );

    ModSettings settings = GetSettingsSnapshot();
    data->defaultSnoozeMinutes = settings.defaultSnoozeMinutes;
    data->maximumMinutes = settings.maximumMinutes;

    HANDLE thread =
        CreateThread(
            nullptr,
            0,
            FinishedAlertThreadProc,
            data.get(),
            0,
            nullptr
        );

    if (!thread) {
        Wh_Log(
            L"ERROR: Could not create completion alert thread"
        );
        return false;
    }

    data.release();
    g_finishedAlertThread.store(thread);
    return true;
}

static void TimerExpiredOnTaskbarThread(void*)
{
    if (g_unloading.load()) {
        return;
    }

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
    }

    SetIdleTaskbarText();
}

static void DispatchTimerExpiredToTaskbar()
{
    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar ||
        !IsWindow(taskbar))
    {
        taskbar = FindCurrentProcessTaskbarWnd();

        if (taskbar) {
            g_taskbarWnd.store(taskbar);
        }
    }

    if (!taskbar) {
        return;
    }

    RunFromWindowThread(
        taskbar,
        TimerExpiredOnTaskbarThread,
        nullptr
    );
}


// -----------------------------------------------------------------------------
// Authoritative worker timer
// -----------------------------------------------------------------------------

static void HandleTimerExpiredFromWorker()
{
    if (g_unloading.load()) {
        return;
    }

    ULONGLONG expected =
        g_deadlineUtc100ns.load();

    if (!expected ||
        GetUtcFileTimeNow() < expected)
    {
        return;
    }

    g_deadlineUtc100ns.store(0);
    g_secondsRemaining.store(0);
    g_pendingFinished.store(true);

    ClearPersistedTimer();

    ModSettings settings =
        GetSettingsSnapshot();

    if (settings.completionSound) {
        MessageBeep(
            MB_ICONEXCLAMATION
        );
    }

    DispatchTimerExpiredToTaskbar();

    wchar_t reminder[256]{};
    GetSharedReminder(reminder);

    if (!ShowFinishedAlert(
            reminder[0]
                ? reminder
                : L"Timer finished"))
    {
        Wh_Log(
            L"ERROR: Completion alert couldn't be shown"
        );
    }
}


static DWORD WINAPI TimerWorkerThreadProc(
    LPVOID)
{
    HANDLE handles[] = {
        g_timerStopEvent,
        g_timerRearmEvent,
        g_timerWaitable,
    };

    while (!g_unloading.load()) {
        DWORD wait =
            WaitForMultipleObjects(
                ARRAYSIZE(handles),
                handles,
                FALSE,
                INFINITE
            );

        if (wait == WAIT_OBJECT_0) {
            break;
        }

        if (wait == WAIT_OBJECT_0 + 1) {
            ULONGLONG deadline =
                g_deadlineUtc100ns.load();

            if (!deadline) {
                CancelWaitableTimer(
                    g_timerWaitable
                );

                continue;
            }

            LARGE_INTEGER due{};
            due.QuadPart =
                static_cast<LONGLONG>(
                    deadline
                );

            if (!SetWaitableTimer(
                    g_timerWaitable,
                    &due,
                    0,
                    nullptr,
                    nullptr,
                    FALSE))
            {
                Wh_Log(
                    L"ERROR: SetWaitableTimer failed: %u",
                    GetLastError()
                );
            }

            continue;
        }

        if (wait == WAIT_OBJECT_0 + 2) {
            HandleTimerExpiredFromWorker();
            continue;
        }

        break;
    }

    CancelWaitableTimer(
        g_timerWaitable
    );

    return 0;
}


static void RestorePersistedTimer()
{
    wchar_t deadlineText[32]{};

    if (!Wh_GetStringValue(
            L"deadlineUtc100ns",
            deadlineText,
            ARRAYSIZE(deadlineText)))
    {
        return;
    }

    wchar_t* end = nullptr;

    unsigned long long parsed =
        wcstoull(
            deadlineText,
            &end,
            10
        );

    if (!end ||
        end == deadlineText ||
        *end != L'\0' ||
        !parsed)
    {
        ClearPersistedTimer();
        return;
    }

    wchar_t reminder[256]{};

    Wh_GetStringValue(
        L"reminder",
        reminder,
        ARRAYSIZE(reminder)
    );

    SetSharedReminder(
        reminder[0]
            ? reminder
            : L"Timer finished"
    );

    ULONGLONG deadline =
        static_cast<ULONGLONG>(
            parsed
        );

    int remaining =
        RemainingSeconds(deadline);

    if (remaining > 0) {
        g_deadlineUtc100ns.store(
            deadline
        );

        g_secondsRemaining.store(
            remaining
        );

        g_pendingFinished.store(false);

        SignalTimerWorker();

        Wh_Log(
            L"Restored running timer with %d second(s) remaining",
            remaining
        );
    }
    else {
        g_deadlineUtc100ns.store(0);
        g_secondsRemaining.store(0);
        g_pendingFinished.store(true);

        ClearPersistedTimer();

        ModSettings settings =
            GetSettingsSnapshot();

        if (settings.completionSound) {
            MessageBeep(
                MB_ICONEXCLAMATION
            );
        }

        Wh_Log(
            L"Restored expired timer"
        );

        ShowFinishedAlert(
            reminder[0]
                ? reminder
                : L"Timer finished"
        );
    }
}


static bool EnsureTimerWorkerStarted()
{
    if (g_timerWorkerStarted.load()) {
        return true;
    }

    if (g_unloading.load()) {
        return false;
    }

    g_timerStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    g_timerRearmEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr
        );

    g_timerWaitable =
        CreateWaitableTimerW(
            nullptr,
            FALSE,
            nullptr
        );

    if (!g_timerStopEvent ||
        !g_timerRearmEvent ||
        !g_timerWaitable)
    {
        Wh_Log(
            L"ERROR: Could not create timer worker objects"
        );

        if (g_timerWaitable) {
            CloseHandle(g_timerWaitable);
            g_timerWaitable = nullptr;
        }

        if (g_timerRearmEvent) {
            CloseHandle(g_timerRearmEvent);
            g_timerRearmEvent = nullptr;
        }

        if (g_timerStopEvent) {
            CloseHandle(g_timerStopEvent);
            g_timerStopEvent = nullptr;
        }

        return false;
    }

    HANDLE thread =
        CreateThread(
            nullptr,
            0,
            TimerWorkerThreadProc,
            nullptr,
            0,
            nullptr
        );

    if (!thread) {
        Wh_Log(
            L"ERROR: Could not create timer worker thread"
        );

        CloseHandle(g_timerWaitable);
        g_timerWaitable = nullptr;

        CloseHandle(g_timerRearmEvent);
        g_timerRearmEvent = nullptr;

        CloseHandle(g_timerStopEvent);
        g_timerStopEvent = nullptr;

        return false;
    }

    g_timerWorkerThread =
        thread;

    g_timerWorkerStarted.store(true);

    RestorePersistedTimer();

    return true;
}

// -----------------------------------------------------------------------------
// Add/remove taskbar button
// -----------------------------------------------------------------------------

static void ReleaseOwnedXamlForRebuild()
{
    HideAndReleaseFlyouts();

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
        g_countdownTimer.Tick(
            g_timerTickToken
        );
        g_countdownTimer = nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(
            g_timerButtonClickToken
        );
        g_timerButton = nullptr;
    }

    g_timerText = nullptr;
    g_timerColumn = nullptr;
}


static void RemoveStaleNamedButton(
    Panel const& panel,
    FrameworkElement const& stale)
{
    if (!panel || !stale) {
        return;
    }

    auto children =
        panel.Children();

    uint32_t staleIndex = 0;

    if (!children.IndexOf(
            stale,
            staleIndex))
    {
        return;
    }

    auto grid =
        panel.try_as<Grid>();

    int staleColumn =
        grid
            ? Grid::GetColumn(stale)
            : -1;

    children.RemoveAt(
        staleIndex
    );

    if (!grid ||
        staleColumn < 0)
    {
        return;
    }

    auto columns =
        grid.ColumnDefinitions();

    if (static_cast<uint32_t>(staleColumn) >=
        columns.Size())
    {
        return;
    }

    for (uint32_t i = 0;
         i < children.Size();
         i++)
    {
        auto child =
            children.GetAt(i)
                .try_as<FrameworkElement>();

        if (!child) {
            continue;
        }

        int column =
            Grid::GetColumn(child);

        if (column > staleColumn) {
            Grid::SetColumn(
                child,
                column - 1
            );
        }
    }

    columns.RemoveAt(
        static_cast<uint32_t>(
            staleColumn
        )
    );
}


static void AddTimerButtonImpl(
    void* param)
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar =
        reinterpret_cast<HWND>(param);

    auto xamlRoot =
        GetTaskbarXamlRoot(taskbar);

    if (!xamlRoot) {
        return;
    }

    auto content =
        xamlRoot.Content()
            .try_as<FrameworkElement>();

    if (!content) {
        return;
    }

    auto tray =
        FindChildRecursive(
            content,
            [](FrameworkElement element)
            {
                return
                    element.Name() ==
                    L"SystemTrayFrameGrid";
            }
        );

    if (!tray) {
        return;
    }

    auto panel =
        tray.try_as<Panel>();

    if (!panel) {
        return;
    }

    auto children =
        panel.Children();

    FrameworkElement existing{nullptr};

    for (uint32_t i = 0;
         i < children.Size();
         i++)
    {
        auto child =
            children.GetAt(i)
                .try_as<FrameworkElement>();

        if (child &&
            child.Name() ==
                L"TaskbarCountdownTimerButton")
        {
            existing = child;
            break;
        }
    }

    if (existing &&
        g_timerButton &&
        existing == g_timerButton)
    {
        g_taskbarWnd.store(taskbar);
        g_buttonInjected.store(true);
        return;
    }

    if (existing) {
        Wh_Log(
            L"Removing stale timer button from a previous mod instance"
        );

        RemoveStaleNamedButton(
            panel,
            existing
        );
    }

    ReleaseOwnedXamlForRebuild();

    g_timerButton = Button();
    g_timerText = TextBlock();

    g_timerButton.Name(
        L"TaskbarCountdownTimerButton"
    );

    g_timerButton.MinWidth(0);
    g_timerButton.MinHeight(0);
    g_timerButton.BorderThickness(
        Thickness{0, 0, 0, 0}
    );
    g_timerButton.Padding(
        Thickness{8, 0, 8, 0}
    );
    g_timerButton.VerticalAlignment(
        VerticalAlignment::Stretch
    );

    g_timerText.VerticalAlignment(
        VerticalAlignment::Center
    );

    g_timerButton.Content(
        g_timerText
    );

    RefreshTaskbarCountdown();

    g_timerButtonClickToken =
        g_timerButton.Click(
            [](
                auto const&,
                RoutedEventArgs const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                ShowTimerFlyout();
            }
        );

    g_countdownTimer =
        DispatcherTimer();

    g_countdownTimer.Interval(
        std::chrono::seconds(1)
    );

    g_timerTickToken =
        g_countdownTimer.Tick(
            [](
                auto const&,
                auto const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                int remaining =
                    RemainingSeconds(
                        g_deadlineUtc100ns.load()
                    );

                g_secondsRemaining.store(
                    remaining
                );

                if (remaining > 0) {
                    if (g_timerText) {
                        g_timerText.Text(
                            FormatCountdown(
                                remaining
                            )
                        );
                    }
                }
                else {
                    if (g_countdownTimer) {
                        g_countdownTimer.Stop();
                    }

                    SetIdleTaskbarText();
                }
            }
        );

    auto trayClass =
        winrt::get_class_name(tray);

    if (trayClass ==
        L"Windows.UI.Xaml.Controls.StackPanel")
    {
        children.InsertAt(
            0,
            g_timerButton
        );
    }
    else {
        auto trayGrid =
            tray.try_as<Grid>();

        if (!trayGrid) {
            Wh_Log(
                L"ERROR: Unsupported SystemTrayFrameGrid layout class: %s",
                trayClass.c_str()
            );

            ReleaseOwnedXamlForRebuild();
            return;
        }

        auto columns =
            trayGrid.ColumnDefinitions();

        g_timerColumn =
            ColumnDefinition();

        g_timerColumn.Width(
            GridLengthHelper::Auto()
        );

        columns.InsertAt(
            0,
            g_timerColumn
        );

        for (uint32_t i = 0;
             i < children.Size();
             i++)
        {
            auto child =
                children.GetAt(i)
                    .try_as<FrameworkElement>();

            if (!child) {
                continue;
            }

            Grid::SetColumn(
                child,
                Grid::GetColumn(child) + 1
            );
        }

        Grid::SetColumn(
            g_timerButton,
            0
        );

        children.InsertAt(
            0,
            g_timerButton
        );
    }

    g_taskbarWnd.store(taskbar);
    g_buttonInjected.store(true);

    if (!EnsureTimerWorkerStarted()) {
        Wh_Log(
            L"ERROR: Timer worker could not be started"
        );
    }

    int remaining =
        RemainingSeconds(
            g_deadlineUtc100ns.load()
        );

    g_secondsRemaining.store(
        remaining
    );

    if (remaining > 0 &&
        g_countdownTimer)
    {
        RefreshTaskbarCountdown();
        g_countdownTimer.Start();
    }
    else {
        SetIdleTaskbarText();
    }

    Wh_Log(
        L"Timer button added to taskbar"
    );
}


static void AddTimerButton(
    void* param)
{
    try {
        AddTimerButtonImpl(param);
    }
    catch (...) {
        HRESULT error =
            winrt::to_hresult();

        Wh_Log(
            L"Applying timer button failed: %08X",
            static_cast<unsigned>(error)
        );
    }
}


static void RemoveTimerButtonImpl(
    void*)
{
    // Revoke every callback into this mod first. Nothing below this point is
    // allowed to throw while leaving a live delegate behind.
    if (g_loadedRevokers) {
        g_loadedRevokers.reset();
    }

    HideAndReleaseFlyouts();

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
        g_countdownTimer.Tick(
            g_timerTickToken
        );
        g_countdownTimer = nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(
            g_timerButtonClickToken
        );
    }

    // Best-effort visual-tree detachment follows after all callbacks are gone.
    if (g_timerButton) {
        auto parentElement =
            VisualTreeHelper::GetParent(
                g_timerButton
            ).try_as<FrameworkElement>();

        auto parent =
            parentElement.try_as<Panel>();

        if (parent) {
            auto children =
                parent.Children();

            uint32_t index = 0;

            if (children.IndexOf(
                    g_timerButton,
                    index))
            {
                children.RemoveAt(index);
            }

            auto parentGrid =
                parentElement.try_as<Grid>();

            if (parentGrid &&
                g_timerColumn)
            {
                auto columns =
                    parentGrid.ColumnDefinitions();

                uint32_t columnIndex = 0;

                if (columns.IndexOf(
                        g_timerColumn,
                        columnIndex))
                {
                    for (uint32_t i = 0;
                         i < children.Size();
                         i++)
                    {
                        auto child =
                            children.GetAt(i)
                                .try_as<FrameworkElement>();

                        if (!child) {
                            continue;
                        }

                        int currentColumn =
                            Grid::GetColumn(child);

                        if (currentColumn >
                            static_cast<int>(
                                columnIndex
                            ))
                        {
                            Grid::SetColumn(
                                child,
                                currentColumn - 1
                            );
                        }
                    }

                    columns.RemoveAt(
                        columnIndex
                    );
                }
            }
        }
    }

    g_timerColumn = nullptr;
    g_timerText = nullptr;
    g_timerButton = nullptr;
    g_buttonInjected.store(false);
}


static void RemoveTimerButton(
    void* param)
{
    try {
        RemoveTimerButtonImpl(param);
    }
    catch (...) {
        HRESULT error =
            winrt::to_hresult();

        Wh_Log(
            L"Removing timer button failed: %08X",
            static_cast<unsigned>(error)
        );
    }
}


static bool TryRemoveTimerXaml()
{
    HWND target =
        g_taskbarWnd.load();

    if (!target ||
        !IsWindow(target))
    {
        target =
            FindCurrentProcessTaskbarWnd();
    }

    if (!target) {
        target =
            FindAnyWindowOnTaskbarThread();
    }

    if (!target) {
        return false;
    }

    return RunFromWindowThread(
        target,
        RemoveTimerButton,
        nullptr
    );
}


static void ApplyTimerButtonIfAvailable()
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar =
        g_taskbarWnd.load();

    if (!taskbar ||
        !IsWindow(taskbar))
    {
        taskbar =
            FindCurrentProcessTaskbarWnd();
    }

    if (!taskbar) {
        return;
    }

    g_taskbarWnd.store(taskbar);

    if (!RunFromWindowThread(
            taskbar,
            AddTimerButton,
            taskbar))
    {
        Wh_Log(
            L"Could not access taskbar UI thread"
        );
    }
}


static DWORD WINAPI RetryThreadProc(
    LPVOID)
{
    for (int i = 0;
         i < 5 &&
         !g_unloading.load();
         i++)
    {
        if (WaitForSingleObject(
                g_retryStopEvent,
                2000) != WAIT_TIMEOUT)
        {
            break;
        }

        if (g_buttonInjected.load() ||
            g_unloading.load())
        {
            break;
        }

        Wh_Log(
            L"Taskbar injection retry %d",
            i + 1
        );

        ApplyTimerButtonIfAvailable();
    }

    return 0;
}

// -----------------------------------------------------------------------------
// System tray rebuild hook
// -----------------------------------------------------------------------------

using IconView_IconView_t =
    void* (WINAPI*)(void* pThis);

static IconView_IconView_t
    IconView_IconView_Original = nullptr;

using LoadLibraryExW_t =
    HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);

static LoadLibraryExW_t
    LoadLibraryExW_Original = nullptr;


static VS_FIXEDFILEINFO* GetModuleVersionInfo(
    HMODULE module)
{
    HRSRC resource =
        FindResourceW(
            module,
            MAKEINTRESOURCEW(VS_VERSION_INFO),
            RT_VERSION
        );

    if (!resource) {
        return nullptr;
    }

    HGLOBAL loaded =
        LoadResource(
            module,
            resource
        );

    if (!loaded) {
        return nullptr;
    }

    void* data =
        LockResource(loaded);

    if (!data) {
        return nullptr;
    }

    void* fixedInfo = nullptr;
    UINT fixedInfoSize = 0;

    if (!VerQueryValueW(
            data,
            L"\\",
            &fixedInfo,
            &fixedInfoSize) ||
        !fixedInfoSize)
    {
        return nullptr;
    }

    return reinterpret_cast<VS_FIXEDFILEINFO*>(
        fixedInfo
    );
}


static HMODULE GetSystemTrayModuleHandle()
{
    if (HMODULE module =
            GetModuleHandleW(L"SystemTray.dll"))
    {
        return module;
    }

    if (HMODULE module =
            GetModuleHandleW(L"Taskbar.View.dll"))
    {
        // First known Taskbar.View.dll version without SystemTray symbols:
        // 2604.8002.200.6000. Newer builds use SystemTray.dll instead.
        VS_FIXEDFILEINFO* fixedInfo =
            GetModuleVersionInfo(module);

        WORD moduleMajor =
            fixedInfo
                ? HIWORD(fixedInfo->dwFileVersionMS)
                : 0;

        if (moduleMajor &&
            moduleMajor < 2604)
        {
            return module;
        }

        Wh_Log(
            L"Skipping Taskbar.View.dll version %u",
            moduleMajor
        );
    }

    return GetModuleHandleW(
        L"ExplorerExtensions.dll"
    );
}


static void* WINAPI IconView_IconView_Hook(
    void* pThis)
{
    auto result =
        IconView_IconView_Original(pThis);

    if (g_unloading.load() ||
        !g_loadedRevokers)
    {
        return result;
    }

    try {
        FrameworkElement iconView = nullptr;

        reinterpret_cast<IUnknown**>(pThis)[1]
            ->QueryInterface(
                winrt::guid_of<FrameworkElement>(),
                winrt::put_abi(iconView)
            );

        if (!iconView ||
            !g_loadedRevokers)
        {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto it =
            std::prev(
                g_loadedRevokers->end()
            );

        *it = iconView.Loaded(
            winrt::auto_revoke_t{},
            [it](
                winrt::Windows::Foundation::IInspectable const&,
                RoutedEventArgs const&)
            {
                if (!g_loadedRevokers) {
                    return;
                }

                g_loadedRevokers->erase(it);

                if (g_unloading.load() ||
                    !g_loadedRevokers)
                {
                    return;
                }

                // A new IconView means the tray may have rebuilt its XAML tree.
                g_buttonInjected.store(false);
                ApplyTimerButtonIfAvailable();
            }
        );
    }
    catch (...) {
        HRESULT error = winrt::to_hresult();

        Wh_Log(
            L"IconView hook XAML work failed: %08X",
            static_cast<unsigned>(error)
        );
    }

    return result;
}


static bool HookSystemTraySymbols(
    HMODULE module)
{
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {{
        {
            LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"
        },
        &IconView_IconView_Original,
        IconView_IconView_Hook,
    }};

    return WindhawkUtils::HookSymbols(
        module,
        systemTrayHooks,
        ARRAYSIZE(systemTrayHooks)
    );
}


static void HandleLoadedModuleIfSystemTray(
    HMODULE module)
{
    if (g_systemTrayModuleHooked.load()) {
        return;
    }

    if (GetSystemTrayModuleHandle() != module) {
        return;
    }

    HMODULE previous =
        g_systemTrayModuleAttempted.exchange(
            module
        );

    if (previous == module) {
        return;
    }

    if (HookSystemTraySymbols(module)) {
        g_systemTrayModuleHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(
            L"ERROR: Failed to hook system tray symbols"
        );
    }
}


static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR fileName,
    HANDLE file,
    DWORD flags)
{
    HMODULE module =
        LoadLibraryExW_Original(
            fileName,
            file,
            flags
        );

    if (module) {
        HandleLoadedModuleIfSystemTray(module);
    }

    return module;
}


static DWORD PumpWaitForThread(
    HANDLE thread)
{
    if (!thread) {
        return WAIT_OBJECT_0;
    }

    DWORD result;
    do {
        result = MsgWaitForMultipleObjects(
            1,
            &thread,
            FALSE,
            INFINITE,
            QS_SENDMESSAGE
        );

        if (result == WAIT_OBJECT_0 + 1) {
            MSG message;
            PeekMessageW(
                &message,
                nullptr,
                0,
                0,
                PM_NOREMOVE
            );
        }
    } while (result == WAIT_OBJECT_0 + 1);

    return result;
}


// -----------------------------------------------------------------------------
// Windhawk
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(
        L"Taskbar Countdown Timer loading"
    );

    g_unloading.store(false);

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(
            L"ERROR: Failed to resolve taskbar.dll symbols"
        );

        return FALSE;
    }

    bool systemTrayHookedAtInit = false;

    if (HMODULE systemTray =
            GetSystemTrayModuleHandle())
    {
        g_systemTrayModuleAttempted.store(
            systemTray
        );

        systemTrayHookedAtInit =
            HookSystemTraySymbols(
                systemTray
            );

        if (systemTrayHookedAtInit) {
            g_systemTrayModuleHooked.store(
                true
            );
        }
        else {
            Wh_Log(
                L"ERROR: Failed to hook system tray symbols"
            );
        }
    }

    if (!systemTrayHookedAtInit) {
        HMODULE kernelbase =
            GetModuleHandleW(
                L"kernelbase.dll"
            );

        auto loadLibraryExW =
            kernelbase
                ? reinterpret_cast<
                      LoadLibraryExW_t>(
                      GetProcAddress(
                          kernelbase,
                          "LoadLibraryExW"
                      )
                  )
                : nullptr;

        if (loadLibraryExW) {
            WindhawkUtils::SetFunctionHook(
                loadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original
            );
        }
    }

    return TRUE;
}


void Wh_ModAfterInit()
{
    if (HMODULE systemTray =
            GetSystemTrayModuleHandle())
    {
        HandleLoadedModuleIfSystemTray(
            systemTray
        );
    }

    ApplyTimerButtonIfAvailable();

    g_retryStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (g_retryStopEvent &&
        !g_buttonInjected.load())
    {
        g_retryThread =
            CreateThread(
                nullptr,
                0,
                RetryThreadProc,
                nullptr,
                0,
                nullptr
            );
    }
}


static void StopTimerWorker()
{
    if (g_timerStopEvent) {
        SetEvent(
            g_timerStopEvent
        );
    }
}


void Wh_ModBeforeUninit()
{
    g_unloading.store(true);

    if (g_retryStopEvent) {
        SetEvent(
            g_retryStopEvent
        );
    }

    StopTimerWorker();

    // First teardown attempt while the taskbar thread is still available.
    if (!TryRemoveTimerXaml()) {
        Wh_Log(
            L"Timer XAML teardown will be retried in Wh_ModUninit"
        );
    }
}


void Wh_ModUninit()
{
    g_unloading.store(true);

    if (g_retryStopEvent) {
        SetEvent(
            g_retryStopEvent
        );
    }

    StopTimerWorker();

    if (g_finishedAlertStopEvent) {
        SetEvent(g_finishedAlertStopEvent);
    }

    if (HWND alert = g_finishedAlertWnd.load()) {
        PostMessageW(alert, WM_CLOSE, 0, 0);
    }

    if (HANDLE alertThread =
            g_finishedAlertThread.exchange(nullptr))
    {
        PumpWaitForThread(alertThread);
        CloseHandle(alertThread);
    }

    if (g_retryThread) {
        PumpWaitForThread(
            g_retryThread
        );

        CloseHandle(
            g_retryThread
        );

        g_retryThread = nullptr;
    }

    if (g_timerWorkerThread) {
        PumpWaitForThread(
            g_timerWorkerThread
        );

        CloseHandle(
            g_timerWorkerThread
        );

        g_timerWorkerThread = nullptr;
    }

    g_timerWorkerStarted.store(false);

    // Retry after all worker activity has stopped. We don't use
    // g_buttonInjected as a shortcut because Loaded revokers can exist even
    // when button injection never completed.
    bool removed = false;

    for (int attempt = 0;
         attempt < 10 &&
         !removed;
         attempt++)
    {
        removed =
            TryRemoveTimerXaml();

        if (!removed) {
            Sleep(50);
        }
    }

    if (!removed) {
        Wh_Log(
            L"ERROR: Could not verify timer XAML teardown before unload"
        );
    }

    if (g_timerWaitable) {
        CloseHandle(
            g_timerWaitable
        );
        g_timerWaitable = nullptr;
    }

    if (g_timerRearmEvent) {
        CloseHandle(
            g_timerRearmEvent
        );
        g_timerRearmEvent = nullptr;
    }

    if (g_timerStopEvent) {
        CloseHandle(
            g_timerStopEvent
        );
        g_timerStopEvent = nullptr;
    }

    if (g_finishedAlertStopEvent) {
        CloseHandle(g_finishedAlertStopEvent);
        g_finishedAlertStopEvent = nullptr;
    }

    if (g_finishedAlertFont) {
        DeleteObject(g_finishedAlertFont);
        g_finishedAlertFont = nullptr;
    }

    if (g_finishedAlertEditBrush) {
        DeleteObject(g_finishedAlertEditBrush);
        g_finishedAlertEditBrush = nullptr;
    }

    if (g_finishedAlertBackgroundBrush) {
        DeleteObject(g_finishedAlertBackgroundBrush);
        g_finishedAlertBackgroundBrush = nullptr;
    }

    if (g_retryStopEvent) {
        CloseHandle(
            g_retryStopEvent
        );
        g_retryStopEvent = nullptr;
    }

    Wh_Log(
        L"Taskbar Countdown Timer unloaded"
    );
}


static void UpdateIdleLabelOnTaskbarThread(
    void*)
{
    if (!g_deadlineUtc100ns.load()) {
        SetIdleTaskbarText();
    }
}


void Wh_ModSettingsChanged()
{
    LoadSettings();

    HWND taskbar =
        g_taskbarWnd.load();

    if (!taskbar ||
        !IsWindow(taskbar))
    {
        taskbar =
            FindCurrentProcessTaskbarWnd();
    }

    if (taskbar) {
        RunFromWindowThread(
            taskbar,
            UpdateIdleLabelOnTaskbarThread,
            nullptr
        );
    }
}
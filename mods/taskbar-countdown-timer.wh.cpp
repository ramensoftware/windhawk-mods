// ==WindhawkMod==
// @id              taskbar-countdown-timer
// @name            Taskbar Countdown Timer
// @description     A simple countdown timer integrated into the Windows 11 taskbar (Windows 11 only)
// @version         1.16
// @author          Richi
// @github          https://github.com/richilp
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ldwmapi -lgdi32 -luxtheme
// @license         GPL-3.0
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
- Persisted timers use the Windows wall clock; changing the system time can shift an active timer
- Missed reminders older than 6 hours are discarded on restore
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
  $description: Maximum value accepted for timers and snoozes, up to 10080 minutes (7 days).

- buttonLabel: "⏱ Timer"
  $name: Idle taskbar button label
  $description: Text shown on the taskbar when no timer is running.

- completionSound: true
  $name: Play completion sound
  $description: Play a Windows notification sound when the timer finishes.
*/
// ==/WindhawkModSettings==



// Portions of the taskbar/XAML integration are adapted from Windhawk's
// taskbar-multirow and taskbar-notification-icon-spacing mods by m417z,
// both licensed under GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-notification-icon-spacing.wh.cpp

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
#include <memory>
#include <new>
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
static winrt::event_token g_timerStartButtonClickToken{};
static winrt::event_token g_timerCancelButtonClickToken{};

static std::atomic<ULONGLONG> g_deadlineUtc100ns{0};

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

// Completion alert is independent from the taskbar XAML tree.
static std::atomic<HWND> g_finishedAlertWnd{nullptr};
static std::atomic<HANDLE> g_finishedAlertThread{nullptr};
static HANDLE g_finishedAlertStopEvent = nullptr;
static SRWLOCK g_finishedAlertLock = SRWLOCK_INIT;
static HINSTANCE g_modInstance = nullptr;
static bool g_finishedAlertClassRegistered = false;

static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic<DWORD> g_taskbarThreadId{0};
static std::atomic_bool g_systemTrayModuleHooked{false};
static std::atomic<HMODULE> g_systemTrayModuleAttempted{nullptr};
static std::atomic_bool g_taskbarDllHooked{false};
static std::atomic<HMODULE> g_taskbarDllAttempted{nullptr};

static SRWLOCK g_shellRuntimeLock = SRWLOCK_INIT;
static bool g_shellRuntimeStarted = false;

static std::atomic_bool g_unloading{false};
static std::atomic_bool g_xamlTornDown{false};

static void ApplyTimerButtonIfAvailable();
static bool EnsureShellRuntimeStarted();

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



using TrayUI_StartTaskbar_t =
    void(WINAPI*)(void* pThis);

static TrayUI_StartTaskbar_t
    TrayUI_StartTaskbar_Original = nullptr;

static void WINAPI TrayUI_StartTaskbar_Hook(
    void* pThis)
{
    if (TrayUI_StartTaskbar_Original) {
        TrayUI_StartTaskbar_Original(
            pThis
        );
    }

    if (!g_unloading.load()) {
        EnsureShellRuntimeStarted();
        ApplyTimerButtonIfAvailable();
    }
}


static bool HookTaskbarDllSymbols(
    HMODULE module)
{
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))",
            },
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
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

    size_t offset;

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

    updated.maximumMinutes =
        std::clamp(
            Wh_GetIntSetting(L"maximumMinutes"),
            1,
            10080
        );

    updated.defaultMinutes =
        std::clamp(
            Wh_GetIntSetting(L"defaultMinutes"),
            1,
            updated.maximumMinutes
        );

    updated.defaultSnoozeMinutes =
        std::clamp(
            Wh_GetIntSetting(L"defaultSnoozeMinutes"),
            1,
            updated.maximumMinutes
        );

    updated.completionSound =
        Wh_GetIntSetting(L"completionSound") != 0;

    auto label =
        WindhawkUtils::StringSetting::make(
            L"buttonLabel"
        );

    if (*label) {
        updated.buttonLabel =
            label.get();
    }

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
    // Revoke delegates before hiding/releasing the flyout tree.
    if (g_timerStartButton) {
        g_timerStartButton.Click(
            g_timerStartButtonClickToken
        );
    }

    if (g_timerCancelButton) {
        g_timerCancelButton.Click(
            g_timerCancelButtonClickToken
        );
    }

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
    reminderBox.MaxLength(255);
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

    g_timerStartButtonClickToken =
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

    g_timerCancelButtonClickToken =
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
constexpr UINT WM_APP_ALERT_REFRESH = WM_APP + 20;

static constexpr wchar_t kFinishedAlertClass[] =
    L"TaskbarCountdownTimerFinishedAlert";

static bool RegisterFinishedAlertClass();
static void UnregisterFinishedAlertClass();

static HFONT g_finishedAlertFont = nullptr;
static HBRUSH g_finishedAlertBackgroundBrush = nullptr;
static HBRUSH g_finishedAlertEditBrush = nullptr;
static bool g_finishedAlertDark = true;
static COLORREF g_finishedAlertBackgroundColor = RGB(32, 32, 32);
static COLORREF g_finishedAlertEditBackgroundColor = RGB(45, 45, 45);
static COLORREF g_finishedAlertTextColor = RGB(245, 245, 245);

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

struct FinishedAlertOldResources
{
    HFONT font = nullptr;
    HBRUSH backgroundBrush = nullptr;
    HBRUSH editBrush = nullptr;
};


static FinishedAlertOldResources CreateFinishedAlertResources(
    UINT dpi)
{
    FinishedAlertOldResources old{
        g_finishedAlertFont,
        g_finishedAlertBackgroundBrush,
        g_finishedAlertEditBrush,
    };

    g_finishedAlertDark =
        !AppsUseLightTheme();

    g_finishedAlertBackgroundColor =
        g_finishedAlertDark
            ? RGB(32, 32, 32)
            : RGB(249, 249, 249);

    g_finishedAlertEditBackgroundColor =
        g_finishedAlertDark
            ? RGB(45, 45, 45)
            : RGB(255, 255, 255);

    g_finishedAlertTextColor =
        g_finishedAlertDark
            ? RGB(245, 245, 245)
            : RGB(25, 25, 25);

    g_finishedAlertBackgroundBrush =
        CreateSolidBrush(
            g_finishedAlertBackgroundColor
        );

    g_finishedAlertEditBrush =
        CreateSolidBrush(
            g_finishedAlertEditBackgroundColor
        );

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

    return old;
}


static void DeleteFinishedAlertResources(
    FinishedAlertOldResources const& resources)
{
    if (resources.font) {
        DeleteObject(resources.font);
    }

    if (resources.backgroundBrush) {
        DeleteObject(resources.backgroundBrush);
    }

    if (resources.editBrush) {
        DeleteObject(resources.editBrush);
    }
}


static void ThemeFinishedAlertChild(
    HWND child);


static void ApplyFinishedAlertTheme(
    HWND hWnd,
    UINT dpi)
{
    FinishedAlertOldResources old =
        CreateFinishedAlertResources(
            dpi
        );

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

    // Point every child at the new font/theme before deleting the old GDI
    // resources they may still reference.
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

    InvalidateRect(
        hWnd,
        nullptr,
        TRUE
    );

    DeleteFinishedAlertResources(
        old
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

static SIZE GetFinishedAlertWindowSize(
    UINT dpi)
{
    RECT rect{
        0,
        0,
        ScaleAlertValue(380, dpi),
        ScaleAlertValue(220, dpi)
    };

    AdjustWindowRectExForDpi(
        &rect,
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        FALSE,
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        dpi ? dpi : 96
    );

    return SIZE{
        rect.right - rect.left,
        rect.bottom - rect.top
    };
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

    SIZE windowSize =
        GetFinishedAlertWindowSize(dpi);

    int width = windowSize.cx;
    int height = windowSize.cy;
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

        ApplyFinishedAlertTheme(
            hWnd,
            dpi
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

        if (g_finishedAlertBackgroundBrush) {
            FillRect(
                reinterpret_cast<HDC>(wParam),
                &rect,
                g_finishedAlertBackgroundBrush
            );
        }

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            g_finishedAlertTextColor
        );

        SetBkColor(
            hdc,
            g_finishedAlertBackgroundColor
        );

        HBRUSH brush =
            g_finishedAlertBackgroundBrush
                ? g_finishedAlertBackgroundBrush
                : GetSysColorBrush(
                      COLOR_WINDOW
                  );

        return reinterpret_cast<LRESULT>(
            brush
        );
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            g_finishedAlertTextColor
        );

        SetBkColor(
            hdc,
            g_finishedAlertEditBackgroundColor
        );

        HBRUSH brush =
            g_finishedAlertEditBrush
                ? g_finishedAlertEditBrush
                : GetSysColorBrush(
                      COLOR_WINDOW
                  );

        return reinterpret_cast<LRESULT>(
            brush
        );
    }

    case WM_THEMECHANGED:
        ApplyFinishedAlertTheme(
            hWnd,
            GetDpiForWindow(hWnd)
        );
        return 0;

    case WM_SETTINGCHANGE:
        if (lParam &&
            _wcsicmp(
                reinterpret_cast<LPCWSTR>(
                    lParam
                ),
                L"ImmersiveColorSet") == 0)
        {
            ApplyFinishedAlertTheme(
                hWnd,
                GetDpiForWindow(hWnd)
            );
        }
        return 0;

    case WM_APP_ALERT_REFRESH:
    {
        auto* incoming =
            reinterpret_cast<
                FinishedAlertData*>(
                lParam
            );

        if (incoming) {
            auto* current =
                reinterpret_cast<
                    FinishedAlertData*>(
                    GetWindowLongPtrW(
                        hWnd,
                        GWLP_USERDATA
                    )
                );

            if (current) {
                *current =
                    *incoming;

                SetWindowTextW(
                    GetDlgItem(
                        hWnd,
                        IDC_ALERT_REMINDER
                    ),
                    current->reminder[0]
                        ? current->reminder
                        : L"Timer finished"
                );

                wchar_t snoozeText[32]{};
                swprintf_s(
                    snoozeText,
                    L"%d",
                    current->defaultSnoozeMinutes
                );

                SetWindowTextW(
                    GetDlgItem(
                        hWnd,
                        IDC_ALERT_SNOOZE_MINUTES
                    ),
                    snoozeText
                );
            }

            delete incoming;
        }

        FlashWindow(
            hWnd,
            TRUE
        );

        return 0;
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

            SetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_ALERT_SNOOZE_LABEL
                ),
                L"Snooze minutes:"
            );

            const wchar_t* reminder =
                data && data->reminder[0]
                    ? data->reminder
                    : L"Timer finished";

            if (!ArmTimer(minutes, reminder)) {
                MessageBeep(MB_ICONERROR);
                return 0;
            }
            DestroyWindow(hWnd);
            return 0;
        }

        if (id == IDC_ALERT_DISMISS) {
            DestroyWindow(hWnd);
            return 0;
        }

        break;
    }

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        g_finishedAlertWnd.store(nullptr);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static bool RegisterFinishedAlertClass()
{
    if (g_finishedAlertClassRegistered) {
        return true;
    }

    HMODULE modInstance = nullptr;

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(
                FinishedAlertWndProc
            ),
            &modInstance))
    {
        Wh_Log(
            L"ERROR: Could not get mod module handle for alert class"
        );

        return false;
    }

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = FinishedAlertWndProc;
    wc.hInstance = modInstance;
    wc.lpszClassName = kFinishedAlertClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1
        );

    if (!RegisterClassExW(&wc)) {
        Wh_Log(
            L"ERROR: Could not register completion alert class: %u",
            GetLastError()
        );

        return false;
    }

    g_modInstance = modInstance;
    g_finishedAlertClassRegistered = true;
    return true;
}


static void UnregisterFinishedAlertClass()
{
    if (!g_finishedAlertClassRegistered ||
        !g_modInstance)
    {
        return;
    }

    if (!UnregisterClassW(
            kFinishedAlertClass,
            g_modInstance))
    {
        Wh_Log(
            L"ERROR: Could not unregister completion alert class: %u",
            GetLastError()
        );
    }

    g_finishedAlertClassRegistered = false;
    g_modInstance = nullptr;
}


static DWORD WINAPI FinishedAlertThreadProc(LPVOID param)
{
    std::unique_ptr<FinishedAlertData> data(
        reinterpret_cast<FinishedAlertData*>(param)
    );
    if (!g_finishedAlertClassRegistered ||
        !g_modInstance)
    {
        Wh_Log(
            L"ERROR: Completion alert class isn't registered"
        );

        return 0;
    }

    HWND taskbar = g_taskbarWnd.load();
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 96;

    if (!dpi) {
        dpi = 96;
    }

    SIZE windowSize =
        GetFinishedAlertWindowSize(
            dpi
        );

    HWND hWnd =
        CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            kFinishedAlertClass,
            L"Timer finished",
            WS_POPUP | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowSize.cx,
            windowSize.cy,
            nullptr,
            nullptr,
            g_modInstance,
            data.get()
        );

    if (!hWnd) {
        Wh_Log(
            L"ERROR: Could not create completion alert: %u",
            GetLastError()
        );

        return 0;
    }

    g_finishedAlertWnd.store(hWnd);

    PositionFinishedAlert(hWnd, dpi);
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE
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
    MSG pending{};

    while (PeekMessageW(
               &pending,
               hWnd,
               WM_APP_ALERT_REFRESH,
               WM_APP_ALERT_REFRESH,
               PM_REMOVE))
    {
        delete reinterpret_cast<
            FinishedAlertData*>(
                pending.lParam
            );
    }

    if (IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }

    g_finishedAlertWnd.store(nullptr);
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

static bool ShowFinishedAlertLocked(const wchar_t* reminder)
{
    if (g_unloading.load()) {
        return false;
    }

    if (!g_finishedAlertClassRegistered &&
        !RegisterFinishedAlertClass())
    {
        Wh_Log(
            L"ERROR: Completion alert class isn't available"
        );
        return false;
    }

    CloseFinishedAlertThreadIfExited();

    if (HWND existing = g_finishedAlertWnd.load()) {
        if (IsWindow(existing)) {
            auto* refresh =
                new (std::nothrow)
                    FinishedAlertData{};

            if (refresh) {
                wcsncpy_s(
                    refresh->reminder,
                    reminder && reminder[0]
                        ? reminder
                        : L"Timer finished",
                    _TRUNCATE
                );

                ModSettings settings =
                    GetSettingsSnapshot();

                refresh->defaultSnoozeMinutes =
                    settings.defaultSnoozeMinutes;

                refresh->maximumMinutes =
                    settings.maximumMinutes;

                if (!PostMessageW(
                        existing,
                        WM_APP_ALERT_REFRESH,
                        0,
                        reinterpret_cast<LPARAM>(
                            refresh
                        )))
                {
                    delete refresh;
                }
            }

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
        Wh_Log(
            L"ERROR: Completion alert stop event isn't available"
        );
        return false;
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

static bool ShowFinishedAlert(
    const wchar_t* reminder)
{
    AcquireSRWLockExclusive(
        &g_finishedAlertLock
    );

    bool result = false;

    if (!g_unloading.load()) {
        result =
            ShowFinishedAlertLocked(
                reminder
            );
    }

    ReleaseSRWLockExclusive(
        &g_finishedAlertLock
    );

    return result;
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

    if (!expected) {
        return;
    }

    if (GetUtcFileTimeNow() < expected) {
        if (WaitForSingleObject(
                g_timerStopEvent,
                250) == WAIT_TIMEOUT)
        {
            SignalTimerWorker();
        }

        return;
    }

    g_deadlineUtc100ns.store(0);

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

        SignalTimerWorker();

        Wh_Log(
            L"Restored running timer with %d second(s) remaining",
            remaining
        );
    }
    else {
        g_deadlineUtc100ns.store(0);

        ULONGLONG now = GetUtcFileTimeNow();
        constexpr ULONGLONG kRestoreStaleCutoff =
            6ULL * 60ULL * 60ULL * 10000000ULL;

        if (now > deadline &&
            now - deadline > kRestoreStaleCutoff)
        {
            ClearPersistedTimer();

            Wh_Log(
                L"Ignoring stale expired timer from more than 6 hours ago"
            );

            return;
        }

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


static void CleanupTimerObjects()
{
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
        CloseHandle(
            g_finishedAlertStopEvent
        );
        g_finishedAlertStopEvent = nullptr;
    }
}


static bool StartTimerWorker()
{
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

    g_finishedAlertStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_timerStopEvent ||
        !g_timerRearmEvent ||
        !g_timerWaitable ||
        !g_finishedAlertStopEvent)
    {
        Wh_Log(
            L"ERROR: Could not create timer synchronization objects"
        );

        CleanupTimerObjects();
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

        CleanupTimerObjects();
        return false;
    }

    g_timerWorkerThread =
        thread;

    return true;
}


static bool EnsureShellRuntimeStarted()
{
    AcquireSRWLockExclusive(
        &g_shellRuntimeLock
    );

    if (g_unloading.load()) {
        ReleaseSRWLockExclusive(
            &g_shellRuntimeLock
        );
        return false;
    }

    if (g_shellRuntimeStarted) {
        ReleaseSRWLockExclusive(
            &g_shellRuntimeLock
        );
        return true;
    }

    if (!g_finishedAlertClassRegistered &&
        !RegisterFinishedAlertClass())
    {
        Wh_Log(
            L"WARNING: Completion alert class could not be registered"
        );
    }

    if (!StartTimerWorker()) {
        ReleaseSRWLockExclusive(
            &g_shellRuntimeLock
        );
        return false;
    }

    g_shellRuntimeStarted = true;

    RestorePersistedTimer();

    ReleaseSRWLockExclusive(
        &g_shellRuntimeLock
    );

    return true;
}

// -----------------------------------------------------------------------------
// Add/remove taskbar button
// -----------------------------------------------------------------------------


template <typename T1, typename T2>
static bool SameWinrtObject(
    T1 const& a,
    T2 const& b)
{
    if (!a || !b) {
        return false;
    }

    auto aUnknown =
        a.template as<
            winrt::Windows::Foundation::IUnknown>();

    auto bUnknown =
        b.template as<
            winrt::Windows::Foundation::IUnknown>();

    return winrt::get_abi(aUnknown) ==
           winrt::get_abi(bUnknown);
}


static void DetachOwnedTimerVisuals()
{
    if (!g_timerButton) {
        return;
    }

    auto parentElement =
        VisualTreeHelper::GetParent(
            g_timerButton
        ).try_as<FrameworkElement>();

    auto parent =
        parentElement.try_as<Panel>();

    if (!parent) {
        return;
    }

    auto children =
        parent.Children();

    uint32_t buttonIndex = 0;

    if (children.IndexOf(
            g_timerButton,
            buttonIndex))
    {
        children.RemoveAt(
            buttonIndex
        );
    }

    auto parentGrid =
        parentElement.try_as<Grid>();

    if (!parentGrid ||
        !g_timerColumn)
    {
        return;
    }

    auto columns =
        parentGrid.ColumnDefinitions();

    uint32_t columnIndex = 0;

    if (!columns.IndexOf(
            g_timerColumn,
            columnIndex))
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
    }

    DetachOwnedTimerVisuals();

    g_timerButton = nullptr;
    g_timerText = nullptr;
    g_timerColumn = nullptr;
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

    if (g_timerButton) {
        auto currentParent =
            VisualTreeHelper::GetParent(
                g_timerButton
            ).try_as<FrameworkElement>();

        if (currentParent &&
            SameWinrtObject(
                currentParent,
                tray))
        {
            g_taskbarWnd.store(taskbar);
            return;
        }
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
        SameWinrtObject(
            existing,
            g_timerButton))
    {
        g_taskbarWnd.store(taskbar);
        return;
    }

    if (existing) {
        Wh_Log(
            L"WARNING: A foreign TaskbarCountdownTimerButton already exists; "
            L"leaving it untouched for safety"
        );
        return;
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

    int remaining =
        RemainingSeconds(
            g_deadlineUtc100ns.load()
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


struct RemoveTimerButtonRequest
{
    bool callbacksRevoked = false;
};


static void RemoveTimerButtonImpl(
    void* param)
{
    auto* request =
        reinterpret_cast<
            RemoveTimerButtonRequest*>(
            param
        );

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

    if (request) {
        request->callbacksRevoked = true;
    }

    // Best-effort visual-tree detachment follows after all callbacks are gone.
    DetachOwnedTimerVisuals();

    g_timerColumn = nullptr;
    g_timerText = nullptr;
    g_timerButton = nullptr;
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

    RemoveTimerButtonRequest request{};

    if (!RunFromWindowThread(
            target,
            RemoveTimerButton,
            &request))
    {
        return false;
    }

    return request.callbacksRevoked;
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


static void HandleLoadedModuleIfTaskbar(
    HMODULE module)
{
    if (g_unloading.load() ||
        g_taskbarDllHooked.load())
    {
        return;
    }

    HMODULE current =
        GetModuleHandleW(
            L"taskbar.dll"
        );

    if (!current ||
        current != module)
    {
        return;
    }

    HMODULE previous =
        g_taskbarDllAttempted.exchange(
            module
        );

    if (previous == module) {
        return;
    }

    if (HookTaskbarDllSymbols(module)) {
        g_taskbarDllHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(
            L"ERROR: Failed to hook taskbar.dll symbols"
        );
    }
}


static void HandleLoadedModuleIfSystemTray(
    HMODULE module)
{
    if (g_unloading.load() ||
        g_systemTrayModuleHooked.load())
    {
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
        HandleLoadedModuleIfTaskbar(module);
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

static void StopTimerWorker();


BOOL Wh_ModInit()
{
    Wh_Log(
        L"Taskbar Countdown Timer loading"
    );

    g_unloading.store(false);
    g_xamlTornDown.store(false);

    LoadSettings();

    bool taskbarHookedAtInit = false;

    if (HMODULE taskbar =
            GetModuleHandleW(
                L"taskbar.dll"))
    {
        g_taskbarDllAttempted.store(
            taskbar
        );

        taskbarHookedAtInit =
            HookTaskbarDllSymbols(
                taskbar
            );

        if (taskbarHookedAtInit) {
            g_taskbarDllHooked.store(
                true
            );
        }
        else {
            Wh_Log(
                L"ERROR: Failed to resolve taskbar.dll symbols"
            );
            return FALSE;
        }
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

    if (!taskbarHookedAtInit ||
        !systemTrayHookedAtInit)
    {
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
        else {
            Wh_Log(
                L"ERROR: Could not resolve LoadLibraryExW"
            );
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (HMODULE taskbar =
            GetModuleHandleW(
                L"taskbar.dll"))
    {
        HandleLoadedModuleIfTaskbar(
            taskbar
        );
    }

    if (HMODULE systemTray =
            GetSystemTrayModuleHandle())
    {
        HandleLoadedModuleIfSystemTray(
            systemTray
        );
    }

    if (FindCurrentProcessTaskbarWnd()) {
        EnsureShellRuntimeStarted();
        ApplyTimerButtonIfAvailable();
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
    AcquireSRWLockExclusive(
        &g_shellRuntimeLock
    );

    g_unloading.store(true);

    ReleaseSRWLockExclusive(
        &g_shellRuntimeLock
    );

    StopTimerWorker();

    // Primary teardown attempt while the taskbar UI thread is still available.
    bool tornDown =
        TryRemoveTimerXaml();

    g_xamlTornDown.store(
        tornDown
    );

    if (!tornDown) {
        Wh_Log(
            L"Initial timer XAML teardown failed; will retry in Wh_ModUninit"
        );
    }
}


void Wh_ModUninit()
{
    g_unloading.store(true);

    StopTimerWorker();

    // First stop and join every thread that can still create/use alert UI.

    if (g_timerWorkerThread) {
        PumpWaitForThread(
            g_timerWorkerThread
        );

        CloseHandle(
            g_timerWorkerThread
        );

        g_timerWorkerThread = nullptr;
    }


    // Only after the timer worker is gone can no new alert be requested by it.
    // Serialize ownership with ShowFinishedAlert in case another caller is
    // concurrently leaving the initialization path.
    AcquireSRWLockExclusive(
        &g_finishedAlertLock
    );

    if (g_finishedAlertStopEvent) {
        SetEvent(
            g_finishedAlertStopEvent
        );
    }

    if (HWND alert =
            g_finishedAlertWnd.load())
    {
        PostMessageW(
            alert,
            WM_CLOSE,
            0,
            0
        );
    }

    HANDLE alertThread =
        g_finishedAlertThread.exchange(
            nullptr
        );

    ReleaseSRWLockExclusive(
        &g_finishedAlertLock
    );

    if (alertThread) {
        PumpWaitForThread(
            alertThread
        );

        CloseHandle(
            alertThread
        );
    }

    // Retry whenever the first teardown didn't complete. Loaded revokers can
    // exist even when no timer button was ever created.
    if (!g_xamlTornDown.load()) {
        bool tornDown =
            TryRemoveTimerXaml();

        g_xamlTornDown.store(
            tornDown
        );

        if (!tornDown) {
            Wh_Log(
                L"ERROR: Could not complete final timer XAML teardown"
            );
        }
    }

    CleanupTimerObjects();

    if (g_finishedAlertFont) {
        DeleteObject(
            g_finishedAlertFont
        );
        g_finishedAlertFont = nullptr;
    }

    if (g_finishedAlertEditBrush) {
        DeleteObject(
            g_finishedAlertEditBrush
        );
        g_finishedAlertEditBrush = nullptr;
    }

    if (g_finishedAlertBackgroundBrush) {
        DeleteObject(
            g_finishedAlertBackgroundBrush
        );
        g_finishedAlertBackgroundBrush = nullptr;
    }

    UnregisterFinishedAlertClass();

    AcquireSRWLockExclusive(
        &g_shellRuntimeLock
    );
    g_shellRuntimeStarted = false;
    ReleaseSRWLockExclusive(
        &g_shellRuntimeLock
    );

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


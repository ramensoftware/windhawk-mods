// ==WindhawkMod==
// @id              taskbar-countdown-timer
// @name            Taskbar Countdown Timer
// @description     A simple countdown timer integrated into the Windows 11 taskbar (Windows 11 only)
// @version         1
// @author          K3M
// @github          https://github.com/yazanhani900-blip
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ldwmapi -lgdi32 -luxtheme -lwinmm -lcomdlg32 -lshlwapi -lwindowscodecs -luuid -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Countdown Timer

Adds a compact countdown timer directly to the Windows 11 taskbar.

## Features

- Set a countdown in **hours, minutes and seconds**
- Add a custom reminder message
- Live countdown display in the taskbar (compact button, adjustable font size)
- Cancel an active timer
- **"Time's up" alert in two sizes**: a large full-screen screen, or a small
  notification in the bottom-right corner
- **Settings page** (the gear button in the timer flyout): sound, background,
  Dark / Light / Auto look and alert size
- **Frosted-glass card**: the card is translucent and blurs the background behind it
- **Glass style**: the timer flyout follows Dark / Light / Auto, shows your background image and can be solid or frosted glass; with a video background the alert card is blurred too
- **Custom background for the alert: an image or a looping video**
  (chosen from the timer flyout, with a Preview button)
- Choose any sound file for the completion alert
- Timer state survives Explorer restarts and mod reloads
- Missed reminders older than 6 hours are discarded on restore
- Completion card is frosted glass with a **Dark / Light / Auto** look (set it on the settings page; Auto follows the Windows app theme)

## How to use

1. Click the timer button on the taskbar.
2. Enter a reminder and a duration.
3. Optionally open the gear (settings) page and click **Choose sound…** and/or **Choose background…**
   (images: jpg, png, bmp, gif, tif - videos: mp4, mov, wmv, avi, mkv, webm).
4. Open the gear (settings) page and use **Preview alert** to see it right away.
5. Click **Start**. When the timer ends, the alert appears.
   Press **Esc** or click **Dismiss** to close it.

Click **Default background** to go back to the built-in gradient.
Videos are muted by default (see settings) because the completion sound plays too.

## Notes

- Alert size (large / small) and look are set on the settings page of the timer flyout; the glass tint is in the mod settings.
- With a video background in the large alert the card stays solid (a live video
  can't be blurred). The small alert always uses an image or the default gradient.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- defaultMinutes: 20
  $name: Default timer duration (minutes)
  $description: Default duration shown when creating a new timer. It is shown in the hours / minutes fields.

- defaultSnoozeMinutes: 5
  $name: Default snooze duration (minutes)
  $description: Default duration shown in the snooze fields.

- maximumMinutes: 1440
  $name: Maximum duration (minutes)
  $description: Maximum total length accepted for timers and snoozes, up to 10080 minutes (7 days).

- buttonLabel: "⏱"
  $name: Idle taskbar button label
  $description: Text shown on the taskbar when no timer is running.

- buttonFontSize: 11
  $name: Taskbar button font size
  $description: Smaller number = smaller button on the taskbar (8-20).

- glassTint: 35
  $name: Frosted glass tint (%)
  $description: How strong the dark tint over the blurred card is (0 = clear glass, 90 = almost solid).

- completionSound: true
  $name: Play completion sound
  $description: Play a sound when the timer finishes. Pick the sound itself from the timer flyout (Choose sound…).

- backgroundDim: 40
  $name: Image background dimming (%)
  $description: How much to darken an image background so the text stays readable (0-80).

- videoSound: false
  $name: Play video sound
  $description: Play the audio track of a video background. Off by default.
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
#include <cmath>
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
#include <vector>
#include <windhawk_utils.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <propidl.h>
#include <wincodec.h>
#include <mfplay.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>

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
[[clang::no_destroy]] static TextBox g_timerHoursBox{nullptr};
[[clang::no_destroy]] static TextBox g_timerMinutesBox{nullptr};
[[clang::no_destroy]] static TextBox g_timerSecondsBox{nullptr};
[[clang::no_destroy]] static TextBlock g_soundNameText{nullptr};
[[clang::no_destroy]] static TextBlock g_bgNameText{nullptr};
[[clang::no_destroy]] static Button g_timerStartButton{nullptr};
[[clang::no_destroy]] static Button g_timerCancelButton{nullptr};
[[clang::no_destroy]] static Button g_soundChooseButton{nullptr};
[[clang::no_destroy]] static Button g_soundTestButton{nullptr};
[[clang::no_destroy]] static Button g_soundDefaultButton{nullptr};
[[clang::no_destroy]] static Button g_bgChooseButton{nullptr};
[[clang::no_destroy]] static Button g_bgPreviewButton{nullptr};
[[clang::no_destroy]] static Button g_bgDefaultButton{nullptr};
[[clang::no_destroy]] static Button g_alertSizeButton{nullptr};
[[clang::no_destroy]] static Button g_alertThemeButton{nullptr};

[[clang::no_destroy]] static std::optional<
    std::list<FrameworkElement::Loaded_revoker>>
    g_loadedRevokers{std::in_place};

static winrt::event_token g_timerButtonClickToken{};
static winrt::event_token g_timerTickToken{};
static winrt::event_token g_timerStartButtonClickToken{};
static winrt::event_token g_timerCancelButtonClickToken{};
static winrt::event_token g_soundChooseClickToken{};
static winrt::event_token g_soundTestClickToken{};
static winrt::event_token g_soundDefaultClickToken{};
static winrt::event_token g_bgChooseClickToken{};
static winrt::event_token g_bgPreviewClickToken{};
static winrt::event_token g_bgDefaultClickToken{};
static winrt::event_token g_alertSizeClickToken{};
static winrt::event_token g_alertThemeClickToken{};

// Alert size chosen from the flyout (false = large full screen, true = small).
static std::atomic_bool g_alertSmallPref{false};

// Alert look chosen from the flyout: 0 = dark, 1 = light, 2 = follow Windows.
static std::atomic<int> g_alertThemePref{0};

// Size / look of the alert window that is currently open.
static std::atomic_bool g_openAlertSmall{false};
static std::atomic<int> g_openAlertTheme{0};

// Flyout pages and handlers.
[[clang::no_destroy]] static StackPanel g_mainPage{nullptr};
[[clang::no_destroy]] static StackPanel g_settingsPage{nullptr};
[[clang::no_destroy]] static std::vector<Button> g_themeChoices;
[[clang::no_destroy]] static std::vector<Button> g_sizeChoices;
[[clang::no_destroy]] static std::vector<std::pair<Button, winrt::event_token>>
    g_flyoutHandlers;
static std::atomic_bool g_flyoutShowSettings{false};

static std::atomic<ULONGLONG> g_deadlineUtc100ns{0};

static SRWLOCK g_reminderLock = SRWLOCK_INIT;
static wchar_t g_reminder[256]{};

struct ModSettings {
    int defaultMinutes = 20;
    int defaultSnoozeMinutes = 5;
    int maximumMinutes = 1440;
    int backgroundDim = 40;
    int glassTint = 35;
    int buttonFontSize = 11;
    bool completionSound = true;
    bool videoSound = false;
    std::wstring buttonLabel = L"⏱";
    std::wstring customSoundPath;        // chosen in the flyout
    std::wstring customBackgroundPath;   // chosen in the flyout (image/video)
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;
static std::wstring g_soundOverride;        // guarded by g_settingsLock
static std::wstring g_backgroundOverride;   // guarded by g_settingsLock

static HANDLE g_timerStopEvent = nullptr;
static HANDLE g_timerRearmEvent = nullptr;
static HANDLE g_timerWaitable = nullptr;
static HANDLE g_timerWorkerThread = nullptr;

// File picker (sound or background) runs on its own thread.
static std::atomic<HANDLE> g_soundPickerThread{nullptr};
static std::atomic<int> g_pickerMode{0};   // 0 = sound, 1 = background
static std::atomic_bool g_flyoutKeepFields{false};

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
static void ShowTimerFlyout();
static bool ShowFinishedAlert(const wchar_t* reminder, bool preview = false);
static bool AppsUseLightTheme();
static bool IsImagePath(const std::wstring& path);

// Flyout frame (background / glass) and tint overlay, plus the glass choice.
[[clang::no_destroy]] static Grid g_flyoutFrame{nullptr};
[[clang::no_destroy]] static Border g_flyoutOverlay{nullptr};
[[clang::no_destroy]] static std::vector<Button> g_glassChoices;
static std::atomic_bool g_flyoutGlassPref{false};

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

        auto found = FindChildRecursive(child, callback, maxDepth - 1);

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

            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result)
    );

    if (result) {
        DWORD threadId = GetWindowThreadProcessId(result, nullptr);

        if (threadId) {
            g_taskbarThreadId.store(threadId);
        }
    }

    return result;
}


static HWND FindAnyWindowOnTaskbarThread()
{
    DWORD threadId = g_taskbarThreadId.load();

    if (!threadId) {
        return nullptr;
    }

    HWND result = nullptr;

    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
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


static void* CTaskBand_ITaskListWndSite_vftable = nullptr;


using TrayUI_StartTaskbar_t =
    void(WINAPI*)(void* pThis);

static TrayUI_StartTaskbar_t
    TrayUI_StartTaskbar_Original = nullptr;

static void WINAPI TrayUI_StartTaskbar_Hook(
    void* pThis)
{
    if (TrayUI_StartTaskbar_Original) {
        TrayUI_StartTaskbar_Original(pThis);
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
        Wh_Log(L"ERROR: Required symbols are missing");
        return nullptr;
    }

    HWND hTaskSwWnd =
        reinterpret_cast<HWND>(
            GetPropW(hTaskbarWnd, L"TaskbandHWND")
        );

    if (!hTaskSwWnd) {
        Wh_Log(L"ERROR: TaskbandHWND not found");
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(
            GetWindowLongPtrW(hTaskSwWnd, 0)
        );

    if (!taskBand) {
        Wh_Log(L"ERROR: CTaskBand object not found");
        return nullptr;
    }

    void* taskBandForSite = taskBand;

    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForSite) !=
             CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20) {
            Wh_Log(L"ERROR: ITaskListWndSite vftable not found");
            return nullptr;
        }

        taskBandForSite =
            reinterpret_cast<void**>(taskBandForSite) + 1;
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

        Wh_Log(L"ERROR: TaskbarHost not obtained");
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
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
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
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* unknown =
        *reinterpret_cast<IUnknown**>(
            reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset
        );

    if (!unknown) {
        Wh_Log(L"ERROR: Taskbar XAML object not found");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;

    unknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarElement)
    );

    auto result =
        taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}


// -----------------------------------------------------------------------------
// Execute code on taskbar XAML thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void (*)(void*);

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

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == message) {
                    auto* param =
                        reinterpret_cast<Param*>(cwp->lParam);

                    if (!param->invoked) {
                        param->proc(param->procParam);
                        param->invoked = true;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        threadId
    );

    if (!hook) {
        return false;
    }

    Param param{proc, procParam, false};

    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&param));

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
    result.customSoundPath = g_soundOverride;
    result.customBackgroundPath = g_backgroundOverride;
    ReleaseSRWLockShared(&g_settingsLock);
    return result;
}


static void LoadSettings()
{
    ModSettings updated;

    updated.maximumMinutes =
        std::clamp(Wh_GetIntSetting(L"maximumMinutes"), 1, 10080);

    updated.defaultMinutes =
        std::clamp(Wh_GetIntSetting(L"defaultMinutes"), 1, updated.maximumMinutes);

    updated.defaultSnoozeMinutes =
        std::clamp(Wh_GetIntSetting(L"defaultSnoozeMinutes"), 1, updated.maximumMinutes);

    updated.backgroundDim =
        std::clamp(Wh_GetIntSetting(L"backgroundDim"), 0, 80);

    updated.glassTint =
        std::clamp(Wh_GetIntSetting(L"glassTint"), 0, 90);

    updated.buttonFontSize =
        std::clamp(Wh_GetIntSetting(L"buttonFontSize"), 8, 20);

    updated.completionSound =
        Wh_GetIntSetting(L"completionSound") != 0;

    updated.videoSound =
        Wh_GetIntSetting(L"videoSound") != 0;

    auto label = WindhawkUtils::StringSetting::make(L"buttonLabel");

    if (*label) {
        updated.buttonLabel = label.get();
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = updated;
    ReleaseSRWLockExclusive(&g_settingsLock);
}


// Sound / background chosen from the flyout are stored as mod state and
// survive Explorer restarts. Empty means "default".
static void SetSoundOverride(const wchar_t* path)
{
    AcquireSRWLockExclusive(&g_settingsLock);
    g_soundOverride = path ? path : L"";
    ReleaseSRWLockExclusive(&g_settingsLock);

    if (path && path[0]) {
        Wh_SetStringValue(L"soundPath", path);
    }
    else {
        Wh_DeleteValue(L"soundPath");
    }
}


static void SetBackgroundOverride(const wchar_t* path)
{
    AcquireSRWLockExclusive(&g_settingsLock);
    g_backgroundOverride = path ? path : L"";
    ReleaseSRWLockExclusive(&g_settingsLock);

    if (path && path[0]) {
        Wh_SetStringValue(L"backgroundPath", path);
    }
    else {
        Wh_DeleteValue(L"backgroundPath");
    }
}


static void LoadSoundOverride()
{
    wchar_t buffer[520]{};

    if (Wh_GetStringValue(L"soundPath", buffer, ARRAYSIZE(buffer)) &&
        buffer[0])
    {
        AcquireSRWLockExclusive(&g_settingsLock);
        g_soundOverride = buffer;
        ReleaseSRWLockExclusive(&g_settingsLock);
    }
}


static void LoadBackgroundOverride()
{
    wchar_t buffer[520]{};

    if (Wh_GetStringValue(L"backgroundPath", buffer, ARRAYSIZE(buffer)) &&
        buffer[0])
    {
        AcquireSRWLockExclusive(&g_settingsLock);
        g_backgroundOverride = buffer;
        ReleaseSRWLockExclusive(&g_settingsLock);
    }
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


static void SetSharedReminder(const wchar_t* reminder)
{
    AcquireSRWLockExclusive(&g_reminderLock);
    wcsncpy_s(g_reminder, reminder ? reminder : L"", _TRUNCATE);
    ReleaseSRWLockExclusive(&g_reminderLock);
}


static void GetSharedReminder(wchar_t (&buffer)[256])
{
    AcquireSRWLockShared(&g_reminderLock);
    wcsncpy_s(buffer, g_reminder, _TRUNCATE);
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

    Wh_SetStringValue(L"deadlineUtc100ns", deadlineText);
    Wh_SetStringValue(L"reminder", reminder ? reminder : L"");
}


static int RemainingSeconds(ULONGLONG deadline)
{
    if (!deadline) {
        return 0;
    }

    ULONGLONG now = GetUtcFileTimeNow();

    if (now >= deadline) {
        return 0;
    }

    ULONGLONG delta = deadline - now;
    ULONGLONG seconds = (delta + 9999999ULL) / 10000000ULL;

    return static_cast<int>(
        std::min<ULONGLONG>(seconds, static_cast<ULONGLONG>(INT_MAX))
    );
}


static std::wstring FormatCountdown(int totalSeconds)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    wchar_t text[64]{};

    if (totalSeconds >= 3600) {
        swprintf_s(
            text,
            L"⏱ %d:%02d:%02d",
            totalSeconds / 3600,
            (totalSeconds % 3600) / 60,
            totalSeconds % 60
        );
    }
    else {
        swprintf_s(
            text,
            L"⏱ %02d:%02d",
            totalSeconds / 60,
            totalSeconds % 60
        );
    }

    return text;
}


// -----------------------------------------------------------------------------
// Completion sound
// -----------------------------------------------------------------------------
//
// .wav files are played with PlaySoundW. Every other format goes through
// Media Foundation (MediaPlayer) with an MCI fallback.

static SRWLOCK g_mciLock = SRWLOCK_INIT;
static std::shared_ptr<std::atomic_bool> g_mciStopFlag;   // guarded by g_mciLock
static std::atomic<int> g_mciActiveThreads{0};
static std::atomic<unsigned> g_mciCounter{0};

struct MciJob
{
    std::wstring path;
    std::shared_ptr<std::atomic_bool> stopFlag;
    unsigned id = 0;
};


static bool MciSend(const std::wstring& command)
{
    MCIERROR error =
        mciSendStringW(command.c_str(), nullptr, 0, nullptr);

    if (error) {
        wchar_t text[256]{};
        mciGetErrorStringW(error, text, ARRAYSIZE(text));
        Wh_Log(L"MCI '%s' failed: %s", command.c_str(), text);
    }

    return error == 0;
}


static DWORD WINAPI FallbackBeepThreadProc(LPVOID)
{
    if (!g_unloading.load()) {
        Beep(880, 180);
    }
    if (!g_unloading.load()) {
        Beep(1175, 260);
    }

    g_mciActiveThreads.fetch_sub(1);
    return 0;
}


static void PlayFallbackBeep()
{
    if (PlaySoundW(
            L"SystemExclamation",
            nullptr,
            SND_ALIAS | SND_ASYNC | SND_NODEFAULT))
    {
        return;
    }

    g_mciActiveThreads.fetch_add(1);

    HANDLE thread =
        CreateThread(nullptr, 0, FallbackBeepThreadProc, nullptr, 0, nullptr);

    if (thread) {
        CloseHandle(thread);
    }
    else {
        g_mciActiveThreads.fetch_sub(1);
        MessageBeep(MB_ICONEXCLAMATION);
    }
}


static bool PlayWithMediaPlayer(const MciJob& job)
{
    using namespace winrt::Windows::Media::Playback;
    using namespace winrt::Windows::Media::Core;

    wchar_t url[2084]{};
    DWORD urlLength = ARRAYSIZE(url);

    if (FAILED(UrlCreateFromPathW(job.path.c_str(), url, &urlLength, 0))) {
        Wh_Log(L"UrlCreateFromPath failed for: %s", job.path.c_str());
        return false;
    }

    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!done) {
        return false;
    }

    std::atomic_bool failed{false};
    std::atomic_bool started{false};
    bool ok = false;

    try {
        MediaPlayer player;
        player.AutoPlay(false);
        player.Volume(1.0);

        auto endedToken =
            player.MediaEnded(
                [&](auto const&, auto const&)
                {
                    SetEvent(done);
                }
            );

        auto failedToken =
            player.MediaFailed(
                [&](auto const&, MediaPlayerFailedEventArgs const& args)
                {
                    failed.store(true);

                    Wh_Log(
                        L"MediaPlayer failed: 0x%08X %s",
                        static_cast<unsigned>(
                            static_cast<int32_t>(args.ExtendedErrorCode())),
                        args.ErrorMessage().c_str()
                    );

                    SetEvent(done);
                }
            );

        player.Source(
            MediaSource::CreateFromUri(
                winrt::Windows::Foundation::Uri(url)
            )
        );

        player.Play();
        started.store(true);

        const ULONGLONG start = GetTickCount64();

        while (!job.stopFlag->load() &&
               !g_unloading.load() &&
               !failed.load())
        {
            if (WaitForSingleObject(done, 100) == WAIT_OBJECT_0) {
                break;
            }

            if (GetTickCount64() - start > 5ULL * 60ULL * 1000ULL) {
                break;
            }
        }

        player.MediaEnded(endedToken);
        player.MediaFailed(failedToken);

        try { player.Pause(); } catch (...) {}
        try { player.Close(); } catch (...) {}

        ok = started.load() && !failed.load();
    }
    catch (...) {
        Wh_Log(
            L"MediaPlayer exception: 0x%08X",
            static_cast<unsigned>(winrt::to_hresult().value)
        );
        ok = false;
    }

    CloseHandle(done);
    return ok;
}


static DWORD WINAPI MciPlayThreadProc(LPVOID param)
{
    std::unique_ptr<MciJob> job(reinterpret_cast<MciJob*>(param));

    bool apartmentInitialized = false;

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    }
    catch (...) {
        Wh_Log(L"init_apartment failed on sound thread");
    }

    bool played = false;

    // 1) Media Foundation based player.
    if (apartmentInitialized &&
        !job->stopFlag->load() &&
        !g_unloading.load())
    {
        played = PlayWithMediaPlayer(*job);
    }

    // 2) MCI fallback.
    if (!played &&
        !job->stopFlag->load() &&
        !g_unloading.load())
    {
        wchar_t aliasBuffer[32]{};
        swprintf_s(aliasBuffer, L"tctsound%u", job->id);

        const std::wstring alias = aliasBuffer;
        const std::wstring quoted = L"\"" + job->path + L"\"";

        bool opened =
            MciSend(L"open " + quoted + L" alias " + alias) ||
            MciSend(L"open " + quoted + L" type mpegvideo alias " + alias);

        if (opened) {
            if (MciSend(L"play " + alias)) {
                played = true;

                const ULONGLONG start = GetTickCount64();
                const std::wstring statusCommand = L"status " + alias + L" mode";

                while (!job->stopFlag->load() &&
                       !g_unloading.load())
                {
                    wchar_t mode[64]{};

                    if (mciSendStringW(
                            statusCommand.c_str(),
                            mode,
                            ARRAYSIZE(mode),
                            nullptr) != 0)
                    {
                        break;
                    }

                    ULONGLONG elapsed = GetTickCount64() - start;

                    if (_wcsicmp(mode, L"playing") != 0 && elapsed > 500) {
                        break;
                    }

                    if (elapsed > 5ULL * 60ULL * 1000ULL) {
                        break;
                    }

                    Sleep(100);
                }
            }

            mciSendStringW((L"stop " + alias).c_str(), nullptr, 0, nullptr);
            mciSendStringW((L"close " + alias).c_str(), nullptr, 0, nullptr);
        }
    }

    // 3) Nothing worked: make sure the user still hears something.
    if (!played &&
        !job->stopFlag->load() &&
        !g_unloading.load())
    {
        Wh_Log(L"Custom sound failed, falling back to default beep");
        PlayFallbackBeep();
    }

    if (apartmentInitialized) {
        winrt::uninit_apartment();
    }

    g_mciActiveThreads.fetch_sub(1);
    return 0;
}


static bool StartMciSound(const std::wstring& path)
{
    auto job = std::make_unique<MciJob>();
    job->path = path;
    job->stopFlag = std::make_shared<std::atomic_bool>(false);
    job->id = g_mciCounter.fetch_add(1) + 1;

    auto stopFlag = job->stopFlag;

    g_mciActiveThreads.fetch_add(1);

    HANDLE thread =
        CreateThread(nullptr, 0, MciPlayThreadProc, job.get(), 0, nullptr);

    if (!thread) {
        g_mciActiveThreads.fetch_sub(1);
        Wh_Log(L"ERROR: Could not create MCI sound thread");
        return false;
    }

    job.release();
    CloseHandle(thread);

    AcquireSRWLockExclusive(&g_mciLock);
    g_mciStopFlag = stopFlag;
    ReleaseSRWLockExclusive(&g_mciLock);

    return true;
}


static void StopCompletionSound()
{
    PlaySoundW(nullptr, nullptr, 0);

    std::shared_ptr<std::atomic_bool> flag;

    AcquireSRWLockExclusive(&g_mciLock);
    flag = std::move(g_mciStopFlag);
    g_mciStopFlag.reset();
    ReleaseSRWLockExclusive(&g_mciLock);

    if (flag) {
        flag->store(true);
    }
}

// force = true is used by the "Test" button and ignores the on/off setting.
static void PlayCompletionSound(bool force = false)
{
    ModSettings settings = GetSettingsSnapshot();

    if (!force && !settings.completionSound) {
        return;
    }

    StopCompletionSound();

    const std::wstring& path = settings.customSoundPath;

    if (!path.empty()) {
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Custom sound file not found: %s", path.c_str());
        }
        else {
            size_t dot = path.find_last_of(L'.');
            std::wstring ext =
                dot == std::wstring::npos ? L"" : path.substr(dot);

            if (_wcsicmp(ext.c_str(), L".wav") == 0) {
                if (PlaySoundW(
                        path.c_str(),
                        nullptr,
                        SND_FILENAME | SND_ASYNC | SND_NODEFAULT))
                {
                    return;
                }

                Wh_Log(L"PlaySound failed for: %s", path.c_str());
            }

            if (StartMciSound(path)) {
                return;
            }

            Wh_Log(L"Custom sound failed, falling back to default beep");
        }
    }

    PlayFallbackBeep();
}


static std::wstring FileNameOnly(const std::wstring& path)
{
    size_t slash = path.find_last_of(L"\\/");

    return slash == std::wstring::npos
        ? path
        : path.substr(slash + 1);
}


static std::wstring GetSoundDisplayName()
{
    ModSettings settings = GetSettingsSnapshot();

    if (settings.customSoundPath.empty()) {
        return L"Default Windows sound";
    }

    return FileNameOnly(settings.customSoundPath);
}


static std::wstring GetBackgroundDisplayName()
{
    ModSettings settings = GetSettingsSnapshot();

    if (settings.customBackgroundPath.empty()) {
        return L"Default (dark gradient)";
    }

    return FileNameOnly(settings.customBackgroundPath);
}


// -----------------------------------------------------------------------------
// File picker (own thread, never blocks the taskbar)
// -----------------------------------------------------------------------------

static void ReopenFlyoutProc(void*)
{
    if (g_unloading.load()) {
        return;
    }

    try {
        ShowTimerFlyout();
    }
    catch (...) {
        Wh_Log(L"Reopening timer flyout failed");
    }
}


static DWORD WINAPI SoundPickerThreadProc(LPVOID)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    const int mode = g_pickerMode.load();

    wchar_t file[520]{};

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);

    if (mode == 1) {
        ofn.lpstrFilter =
            L"Images and videos\0"
            L"*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;"
            L"*.mp4;*.m4v;*.mov;*.wmv;*.avi;*.mkv;*.webm\0"
            L"All files (*.*)\0*.*\0";
        ofn.lpstrTitle = L"Choose full-screen background (image or video)";
    }
    else {
        ofn.lpstrFilter =
            L"Audio files (*.wav;*.mp3;*.wma;*.m4a;*.aac)\0"
            L"*.wav;*.mp3;*.wma;*.m4a;*.aac\0"
            L"All files (*.*)\0*.*\0";
        ofn.lpstrTitle = L"Choose timer sound";
    }

    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.Flags =
        OFN_EXPLORER |
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST |
        OFN_HIDEREADONLY |
        OFN_NOCHANGEDIR;

    BOOL ok = GetOpenFileNameW(&ofn);

    CoUninitialize();

    if (g_unloading.load()) {
        return 0;
    }

    if (ok && file[0]) {
        if (mode == 1) {
            SetBackgroundOverride(file);
        }
        else {
            SetSoundOverride(file);
        }
    }

    HWND taskbar = g_taskbarWnd.load();

    if (taskbar && IsWindow(taskbar)) {
        RunFromWindowThread(taskbar, ReopenFlyoutProc, nullptr);
    }

    return 0;
}


static void StartFilePicker(int mode)
{
    if (g_unloading.load()) {
        return;
    }

    HANDLE existing = g_soundPickerThread.load();

    if (existing) {
        if (WaitForSingleObject(existing, 0) != WAIT_OBJECT_0) {
            return; // dialog is still open
        }

        CloseHandle(existing);
        g_soundPickerThread.store(nullptr);
    }

    g_pickerMode.store(mode);

    HANDLE thread =
        CreateThread(nullptr, 0, SoundPickerThreadProc, nullptr, 0, nullptr);

    if (thread) {
        g_soundPickerThread.store(thread);
    }
    else {
        Wh_Log(L"ERROR: Could not create file picker thread");
    }
}


static void CloseSoundPickerDialog(HANDLE thread)
{
    if (!thread) {
        return;
    }

    DWORD threadId = GetThreadId(thread);

    if (!threadId) {
        return;
    }

    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM) -> BOOL
        {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
            return TRUE;
        },
        0
    );
}


// -----------------------------------------------------------------------------
// XAML timer display and flyouts
// -----------------------------------------------------------------------------

static void SetIdleTaskbarText()
{
    if (!g_timerText) {
        return;
    }

    ModSettings settings = GetSettingsSnapshot();
    g_timerText.Text(settings.buttonLabel);
}


// Keeps the taskbar button small: smaller font and tight padding.
static void ApplyButtonMetrics()
{
    if (!g_timerButton || !g_timerText) {
        return;
    }

    ModSettings settings = GetSettingsSnapshot();

    g_timerText.FontSize(settings.buttonFontSize);
    g_timerButton.Padding(Thickness{4, 0, 4, 0});
}


static void RefreshTaskbarCountdown()
{
    if (!g_timerText) {
        return;
    }

    int remaining = RemainingSeconds(g_deadlineUtc100ns.load());

    if (remaining > 0) {
        g_timerText.Text(FormatCountdown(remaining));
    }
    else {
        SetIdleTaskbarText();
    }
}


static void HideAndReleaseFlyouts()
{
    // Revoke delegates before hiding/releasing the flyout tree.
    for (auto& entry : g_flyoutHandlers) {
        if (entry.first) {
            entry.first.Click(entry.second);
        }
    }

    g_flyoutHandlers.clear();

    if (g_timerFlyout) {
        g_timerFlyout.Hide();
    }

    g_themeChoices.clear();
    g_sizeChoices.clear();
    g_glassChoices.clear();

    g_flyoutFrame = nullptr;
    g_flyoutOverlay = nullptr;

    g_mainPage = nullptr;
    g_settingsPage = nullptr;
    g_timerReminderBox = nullptr;
    g_timerHoursBox = nullptr;
    g_timerMinutesBox = nullptr;
    g_timerSecondsBox = nullptr;
    g_soundNameText = nullptr;
    g_bgNameText = nullptr;
    g_timerStartButton = nullptr;
    g_timerCancelButton = nullptr;
    g_timerFlyout = nullptr;
}


// Empty field counts as 0. Returns false for anything that isn't a number.
static bool ParseDurationField(
    TextBox const& box,
    long& value)
{
    value = 0;

    if (!box) {
        return false;
    }

    std::wstring text = box.Text().c_str();

    if (text.empty()) {
        return true;
    }

    wchar_t* end = nullptr;
    long parsed = wcstol(text.c_str(), &end, 10);

    if (!end ||
        end == text.c_str() ||
        *end != L'\0' ||
        parsed < 0 ||
        parsed > 99999)
    {
        return false;
    }

    value = parsed;
    return true;
}


static void SignalTimerWorker()
{
    if (g_timerRearmEvent) {
        SetEvent(g_timerRearmEvent);
    }
}


static bool ArmTimer(
    int totalSeconds,
    const wchar_t* reminder)
{
    ModSettings settings = GetSettingsSnapshot();

    if (totalSeconds <= 0 ||
        static_cast<long long>(totalSeconds) >
            static_cast<long long>(settings.maximumMinutes) * 60LL ||
        g_unloading.load())
    {
        return false;
    }

    ULONGLONG seconds = static_cast<ULONGLONG>(totalSeconds);

    ULONGLONG deadline =
        GetUtcFileTimeNow() + seconds * 10000000ULL;

    const wchar_t* reminderToUse =
        reminder && reminder[0] ? reminder : L"Timer finished";

    SetSharedReminder(reminderToUse);

    g_deadlineUtc100ns.store(deadline);

    PersistTimer(deadline, reminderToUse);

    SignalTimerWorker();

    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();

        if (taskbar) {
            g_taskbarWnd.store(taskbar);
        }
    }

    if (taskbar) {
        RunFromWindowThread(
            taskbar,
            [](void*)
            {
                if (g_countdownTimer) {
                    g_countdownTimer.Start();
                }

                RefreshTaskbarCountdown();
            },
            nullptr
        );
    }

    Wh_Log(L"Timer armed: '%s' - %d second(s)", reminderToUse, totalSeconds);

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
    panel.Width(380);
    panel.Margin(Thickness{16, 14, 16, 14});
    return panel;
}


static TextBlock MakeLabel(const wchar_t* text)
{
    TextBlock label;
    label.Text(text);
    label.Margin(Thickness{0, 0, 0, 4});
    return label;
}


static Button MakeActionButton(
    const wchar_t* text,
    double minWidth = 120)
{
    Button button;
    button.Content(winrt::box_value(text));
    button.MinWidth(minWidth);
    button.Margin(Thickness{0, 8, 8, 0});
    return button;
}


static TextBox MakeDurationBox(
    const wchar_t* header,
    bool last)
{
    TextBox box;
    box.Header(winrt::box_value(header));
    box.Width(112);
    box.MaxLength(5);
    box.Margin(Thickness{0, 0, last ? 0.0 : 8.0, 0});
    return box;
}


// ---- Flyout look ------------------------------------------------------------

static Style GetAccentButtonStyle()
{
    try {
        if (auto app = Application::Current()) {
            auto resources = app.Resources();
            auto key = winrt::box_value(L"AccentButtonStyle");

            if (resources.HasKey(key)) {
                return resources.Lookup(key).as<Style>();
            }
        }
    }
    catch (...) {
    }

    return Style{nullptr};
}


static void AddFlyoutClick(Button const& button, std::function<void()> action)
{
    auto token =
        button.Click(
            [action](auto const&, RoutedEventArgs const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                action();
            }
        );

    g_flyoutHandlers.emplace_back(button, token);
}


static StackPanel MakeButtonRow()
{
    StackPanel row;
    row.Orientation(Orientation::Horizontal);
    return row;
}


static Button MakeRowButton(const wchar_t* text, double minWidth)
{
    Button button;
    button.Content(winrt::box_value(text));
    button.MinWidth(minWidth);
    button.Margin(Thickness{0, 0, 8, 0});
    return button;
}


static TextBlock MakeSectionTitle(const wchar_t* text)
{
    TextBlock title;
    title.Text(text);
    title.FontSize(12);
    title.Opacity(0.65);
    title.Margin(Thickness{0, 16, 0, 6});
    return title;
}


static TextBlock MakeValueText()
{
    TextBlock text;
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.Opacity(0.9);
    text.Margin(Thickness{0, 0, 0, 8});
    return text;
}


static Button MakeIconButton(const wchar_t* glyph)
{
    Button button;
    button.Content(winrt::box_value(glyph));
    button.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    button.Width(36);
    button.Height(32);
    button.Padding(Thickness{0, 0, 0, 0});
    return button;
}


static Grid MakeHeader(
    const wchar_t* title,
    Button const& button,
    bool buttonOnLeft)
{
    Grid grid;

    ColumnDefinition titleColumn;
    titleColumn.Width(
        GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));

    ColumnDefinition buttonColumn;
    buttonColumn.Width(GridLengthHelper::Auto());

    TextBlock titleText;
    titleText.Text(title);
    titleText.FontSize(20);
    titleText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    titleText.VerticalAlignment(VerticalAlignment::Center);

    if (buttonOnLeft) {
        titleText.Margin(Thickness{10, 0, 0, 0});

        grid.ColumnDefinitions().Append(buttonColumn);
        grid.ColumnDefinitions().Append(titleColumn);

        Grid::SetColumn(button, 0);
        Grid::SetColumn(titleText, 1);
    }
    else {
        grid.ColumnDefinitions().Append(titleColumn);
        grid.ColumnDefinitions().Append(buttonColumn);

        Grid::SetColumn(titleText, 0);
        Grid::SetColumn(button, 1);
    }

    grid.Children().Append(titleText);
    grid.Children().Append(button);

    return grid;
}


static void MarkChoice(Button const& button, const wchar_t* name, bool selected)
{
    std::wstring text = selected ? std::wstring(L"\u2713  ") + name : name;

    button.Content(winrt::box_value(winrt::hstring{text}));
    button.Style(selected ? GetAccentButtonStyle() : Style{nullptr});
}


static void RefreshChoiceButtons()
{
    static const wchar_t* const kThemeNames[] = {L"Dark", L"Light", L"Auto"};
    static const wchar_t* const kSizeNames[] = {L"Large", L"Small"};

    const int theme = g_alertThemePref.load();

    for (size_t i = 0; i < g_themeChoices.size() && i < 3; i++) {
        MarkChoice(
            g_themeChoices[i],
            kThemeNames[i],
            static_cast<int>(i) == theme
        );
    }

    const int size = g_alertSmallPref.load() ? 1 : 0;

    for (size_t i = 0; i < g_sizeChoices.size() && i < 2; i++) {
        MarkChoice(
            g_sizeChoices[i],
            kSizeNames[i],
            static_cast<int>(i) == size
        );
    }

    static const wchar_t* const kGlassNames[] = {L"Solid", L"Frosted glass"};

    const int glass = g_flyoutGlassPref.load() ? 1 : 0;

    for (size_t i = 0; i < g_glassChoices.size() && i < 2; i++) {
        MarkChoice(
            g_glassChoices[i],
            kGlassNames[i],
            static_cast<int>(i) == glass
        );
    }
}


static void ShowSettingsPage(bool show)
{
    if (!g_mainPage || !g_settingsPage) {
        return;
    }

    g_mainPage.Visibility(show ? Visibility::Collapsed : Visibility::Visible);
    g_settingsPage.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
}


// Applies Dark / Light / Auto, the chosen background image and the optional
// frosted-glass (acrylic) look to the flyout.
static void ApplyFlyoutLook()
{
    if (!g_timerFlyout || !g_flyoutFrame || !g_flyoutOverlay) {
        return;
    }

    using namespace winrt::Windows::UI::Xaml::Media::Imaging;
    using winrt::Windows::UI::ColorHelper;

    const int pref = g_alertThemePref.load();
    const bool light = pref == 1 || (pref == 2 && AppsUseLightTheme());
    const bool glass = g_flyoutGlassPref.load();

    if (auto root =
            g_flyoutFrame.Children().GetAt(1).try_as<FrameworkElement>())
    {
        root.RequestedTheme(light ? ElementTheme::Light : ElementTheme::Dark);
    }

    ModSettings settings = GetSettingsSnapshot();
    const std::wstring& path = settings.customBackgroundPath;

    bool hasImage = false;

    if (!path.empty() &&
        IsImagePath(path) &&
        GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        try {
            wchar_t url[2084]{};
            DWORD urlLength = ARRAYSIZE(url);

            if (SUCCEEDED(UrlCreateFromPathW(path.c_str(), url, &urlLength, 0))) {
                BitmapImage bitmap;
                bitmap.UriSource(winrt::Windows::Foundation::Uri(url));

                ImageBrush brush;
                brush.ImageSource(bitmap);
                brush.Stretch(Stretch::UniformToFill);

                // In glass mode the picture itself becomes translucent.
                brush.Opacity(glass ? 0.45 : 1.0);

                g_flyoutFrame.Background(brush);
                hasImage = true;
            }
        }
        catch (...) {
            Wh_Log(L"Flyout background image failed");
        }
    }

    if (!hasImage) {
        const auto solid = light
            ? ColorHelper::FromArgb(255, 249, 249, 249)
            : ColorHelper::FromArgb(255, 32, 32, 32);

        if (glass) {
            // Blurs the desktop behind the flyout. glassTint controls how
            // strong the tint is (same setting as the alert card).
            AcrylicBrush acrylic;
            acrylic.BackgroundSource(AcrylicBackgroundSource::HostBackdrop);
            acrylic.TintColor(
                light
                    ? ColorHelper::FromArgb(255, 252, 252, 255)
                    : ColorHelper::FromArgb(255, 24, 26, 34)
            );
            acrylic.TintOpacity(
                light
                    ? std::clamp(settings.glassTint / 100.0 + 0.30, 0.45, 0.85)
                    : std::clamp(settings.glassTint / 100.0 + 0.15, 0.0, 1.0)
            );

            try {
                acrylic.TintLuminosityOpacity(
                    winrt::box_value(light ? 0.9 : 0.85)
                        .as<winrt::Windows::Foundation::IReference<double>>()
                );
            }
            catch (...) {
            }

            acrylic.FallbackColor(solid);

            g_flyoutFrame.Background(acrylic);
        }
        else {
            g_flyoutFrame.Background(SolidColorBrush(solid));
        }
    }

    // Tint over the image so the text stays readable.
    uint8_t alpha = 0;

    if (hasImage) {
        alpha = glass ? (light ? 120 : 110) : (light ? 165 : 150);
    }

    g_flyoutOverlay.Background(
        SolidColorBrush(
            light
                ? ColorHelper::FromArgb(alpha, 255, 255, 255)
                : ColorHelper::FromArgb(alpha, 0, 0, 0)
        )
    );
}


static void BuildTimerFlyout()
{
    if (g_timerFlyout) {
        return;
    }

    Flyout flyout;
    flyout.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
    flyout.ShouldConstrainToRootBounds(false);

    StackPanel root;
    root.Width(360);
    root.Margin(Thickness{16, 14, 16, 14});

    // ---- Main page: reminder, duration, start / cancel --------------------

    StackPanel mainPage;

    Button gearButton = MakeIconButton(L"\uE713");
    mainPage.Children().Append(MakeHeader(L"Timer", gearButton, false));

    TextBox reminderBox;
    reminderBox.Header(winrt::box_value(L"Reminder"));
    reminderBox.PlaceholderText(L"What should I remind you about?");
    reminderBox.MaxLength(255);
    reminderBox.Margin(Thickness{0, 16, 0, 12});
    mainPage.Children().Append(reminderBox);

    StackPanel durationRow = MakeButtonRow();

    TextBox hoursBox = MakeDurationBox(L"Hours", false);
    TextBox minutesBox = MakeDurationBox(L"Minutes", false);
    TextBox secondsBox = MakeDurationBox(L"Seconds", true);

    durationRow.Children().Append(hoursBox);
    durationRow.Children().Append(minutesBox);
    durationRow.Children().Append(secondsBox);

    mainPage.Children().Append(durationRow);

    StackPanel actions = MakeButtonRow();
    actions.Margin(Thickness{0, 18, 0, 0});

    Button startButton = MakeRowButton(L"Start", 130);
    Button cancelButton = MakeRowButton(L"Cancel timer", 130);

    if (auto accent = GetAccentButtonStyle()) {
        startButton.Style(accent);
    }

    actions.Children().Append(startButton);
    actions.Children().Append(cancelButton);

    mainPage.Children().Append(actions);

    // ---- Settings page ------------------------------------------------------

    StackPanel settingsPage;
    settingsPage.Visibility(Visibility::Collapsed);

    Button backButton = MakeIconButton(L"\uE72B");
    settingsPage.Children().Append(MakeHeader(L"Settings", backButton, true));

    // Sound
    settingsPage.Children().Append(MakeSectionTitle(L"COMPLETION SOUND"));

    TextBlock soundName = MakeValueText();
    settingsPage.Children().Append(soundName);

    StackPanel soundRow = MakeButtonRow();
    Button soundChooseButton = MakeRowButton(L"Choose sound…", 120);
    Button soundTestButton = MakeRowButton(L"Test", 70);
    Button soundDefaultButton = MakeRowButton(L"Default", 80);

    soundRow.Children().Append(soundChooseButton);
    soundRow.Children().Append(soundTestButton);
    soundRow.Children().Append(soundDefaultButton);
    settingsPage.Children().Append(soundRow);

    // Background
    settingsPage.Children().Append(MakeSectionTitle(L"ALERT BACKGROUND (IMAGE OR VIDEO)"));

    TextBlock bgName = MakeValueText();
    settingsPage.Children().Append(bgName);

    StackPanel bgRow = MakeButtonRow();
    Button bgChooseButton = MakeRowButton(L"Choose background…", 170);
    Button bgDefaultButton = MakeRowButton(L"Default", 80);

    bgRow.Children().Append(bgChooseButton);
    bgRow.Children().Append(bgDefaultButton);
    settingsPage.Children().Append(bgRow);

    // Look: dark / light / auto
    settingsPage.Children().Append(MakeSectionTitle(L"ALERT LOOK"));

    StackPanel themeRow = MakeButtonRow();

    for (int i = 0; i < 3; i++) {
        Button choice = MakeRowButton(L"", 100);

        themeRow.Children().Append(choice);
        g_themeChoices.push_back(choice);

        AddFlyoutClick(choice, [i]()
        {
            g_alertThemePref.store(i);
            Wh_SetIntValue(L"alertTheme", i);
            RefreshChoiceButtons();
            ApplyFlyoutLook();
        });
    }

    settingsPage.Children().Append(themeRow);

    // Flyout style: solid / frosted glass
    settingsPage.Children().Append(MakeSectionTitle(L"GLASS STYLE (FLYOUT + ALERT)"));

    StackPanel glassRow = MakeButtonRow();

    for (int i = 0; i < 2; i++) {
        Button choice = MakeRowButton(L"", 150);

        glassRow.Children().Append(choice);
        g_glassChoices.push_back(choice);

        AddFlyoutClick(choice, [i]()
        {
            g_flyoutGlassPref.store(i == 1);
            Wh_SetIntValue(L"flyoutGlass", i);
            RefreshChoiceButtons();
            ApplyFlyoutLook();
        });
    }

    settingsPage.Children().Append(glassRow);

    TextBlock glassHint;
    glassHint.Text(L"Frosted glass also blurs the alert card over a video background.");
    glassHint.FontSize(12);
    glassHint.Opacity(0.6);
    glassHint.TextWrapping(TextWrapping::Wrap);
    glassHint.Margin(Thickness{0, 2, 0, 0});
    settingsPage.Children().Append(glassHint);

    // Size: large / small
    settingsPage.Children().Append(MakeSectionTitle(L"ALERT SIZE"));

    StackPanel sizeRow = MakeButtonRow();

    for (int i = 0; i < 2; i++) {
        Button choice = MakeRowButton(L"", 150);

        sizeRow.Children().Append(choice);
        g_sizeChoices.push_back(choice);

        AddFlyoutClick(choice, [i]()
        {
            g_alertSmallPref.store(i == 1);
            Wh_SetIntValue(L"alertSmall", i);
            RefreshChoiceButtons();
        });
    }

    settingsPage.Children().Append(sizeRow);

    TextBlock sizeHint;
    sizeHint.Text(L"Large: full screen. Small: notification in the bottom-right corner.");
    sizeHint.FontSize(12);
    sizeHint.Opacity(0.6);
    sizeHint.TextWrapping(TextWrapping::Wrap);
    sizeHint.Margin(Thickness{0, 2, 0, 0});
    settingsPage.Children().Append(sizeHint);

    // Preview
    Button previewButton = MakeRowButton(L"Preview alert", 160);
    previewButton.Margin(Thickness{0, 18, 0, 0});

    if (auto accent = GetAccentButtonStyle()) {
        previewButton.Style(accent);
    }

    settingsPage.Children().Append(previewButton);

    // ---- Handlers -------------------------------------------------------------

    AddFlyoutClick(gearButton, []()
    {
        if (g_soundNameText) {
            g_soundNameText.Text(L"Sound: " + GetSoundDisplayName());
        }

        if (g_bgNameText) {
            g_bgNameText.Text(L"Background: " + GetBackgroundDisplayName());
        }

        RefreshChoiceButtons();
        ShowSettingsPage(true);
    });

    AddFlyoutClick(backButton, []()
    {
        ShowSettingsPage(false);
    });

    AddFlyoutClick(startButton, []()
    {
        if (!g_timerReminderBox ||
            !g_timerHoursBox ||
            !g_timerMinutesBox ||
            !g_timerSecondsBox)
        {
            return;
        }

        ModSettings settings = GetSettingsSnapshot();

        long hours = 0;
        long minutes = 0;
        long seconds = 0;

        if (!ParseDurationField(g_timerHoursBox, hours)) {
            g_timerHoursBox.SelectAll();
            g_timerHoursBox.Focus(FocusState::Programmatic);
            return;
        }

        if (!ParseDurationField(g_timerMinutesBox, minutes)) {
            g_timerMinutesBox.SelectAll();
            g_timerMinutesBox.Focus(FocusState::Programmatic);
            return;
        }

        if (!ParseDurationField(g_timerSecondsBox, seconds)) {
            g_timerSecondsBox.SelectAll();
            g_timerSecondsBox.Focus(FocusState::Programmatic);
            return;
        }

        long long total = hours * 3600LL + minutes * 60LL + seconds;

        long long maximum =
            static_cast<long long>(settings.maximumMinutes) * 60LL;

        if (total <= 0 || total > maximum) {
            g_timerMinutesBox.SelectAll();
            g_timerMinutesBox.Focus(FocusState::Programmatic);
            return;
        }

        std::wstring reminder = g_timerReminderBox.Text().c_str();

        if (!ArmTimer(static_cast<int>(total), reminder.c_str())) {
            return;
        }

        StopCompletionSound();

        if (g_timerFlyout) {
            g_timerFlyout.Hide();
        }
    });

    AddFlyoutClick(cancelButton, []()
    {
        CancelTimer();
        StopCompletionSound();

        if (g_timerFlyout) {
            g_timerFlyout.Hide();
        }
    });

    AddFlyoutClick(soundChooseButton, []()
    {
        // The flyout closes when the file dialog takes focus; keep what the
        // user already typed and reopen it on the settings page afterwards.
        g_flyoutKeepFields.store(true);
        g_flyoutShowSettings.store(true);
        StartFilePicker(0);
    });

    AddFlyoutClick(soundTestButton, []()
    {
        PlayCompletionSound(true);
    });

    AddFlyoutClick(soundDefaultButton, []()
    {
        StopCompletionSound();
        SetSoundOverride(nullptr);

        if (g_soundNameText) {
            g_soundNameText.Text(L"Sound: " + GetSoundDisplayName());
        }
    });

    AddFlyoutClick(bgChooseButton, []()
    {
        g_flyoutKeepFields.store(true);
        g_flyoutShowSettings.store(true);
        StartFilePicker(1);
    });

    AddFlyoutClick(bgDefaultButton, []()
    {
        SetBackgroundOverride(nullptr);

        if (g_bgNameText) {
            g_bgNameText.Text(L"Background: " + GetBackgroundDisplayName());
        }

        ApplyFlyoutLook();
    });

    AddFlyoutClick(previewButton, []()
    {
        if (g_timerFlyout) {
            g_timerFlyout.Hide();
        }

        ShowFinishedAlert(L"This is how your timer alert will look.", true);
    });

    root.Children().Append(mainPage);
    root.Children().Append(settingsPage);

    Grid frame;
    frame.CornerRadius(CornerRadius{8, 8, 8, 8});

    Border overlay;
    overlay.IsHitTestVisible(false);

    frame.Children().Append(overlay);   // index 0: tint
    frame.Children().Append(root);      // index 1: content

    // Remove the default gray background / padding of the flyout presenter.
    Style presenterStyle{
        winrt::Windows::UI::Xaml::Interop::TypeName{
            L"Windows.UI.Xaml.Controls.FlyoutPresenter",
            winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
        }
    };

    presenterStyle.Setters().Append(
        Setter(
            Control::PaddingProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})
        )
    );

    presenterStyle.Setters().Append(
        Setter(
            Control::BackgroundProperty(),
            SolidColorBrush(winrt::Windows::UI::Colors::Transparent())
        )
    );

    flyout.FlyoutPresenterStyle(presenterStyle);
    flyout.Content(frame);

    g_flyoutFrame = frame;
    g_flyoutOverlay = overlay;

    g_mainPage = mainPage;
    g_settingsPage = settingsPage;
    g_timerReminderBox = reminderBox;
    g_timerHoursBox = hoursBox;
    g_timerMinutesBox = minutesBox;
    g_timerSecondsBox = secondsBox;
    g_soundNameText = soundName;
    g_bgNameText = bgName;
    g_timerStartButton = startButton;
    g_timerCancelButton = cancelButton;
    g_timerFlyout = flyout;

    RefreshChoiceButtons();
    ApplyFlyoutLook();
}


static void SetDurationBoxes(int totalSeconds)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    wchar_t text[16]{};

    swprintf_s(text, L"%d", totalSeconds / 3600);
    g_timerHoursBox.Text(text);

    swprintf_s(text, L"%d", (totalSeconds % 3600) / 60);
    g_timerMinutesBox.Text(text);

    swprintf_s(text, L"%d", totalSeconds % 60);
    g_timerSecondsBox.Text(text);
}


static void ShowTimerFlyout()
{
    if (!g_timerButton || g_unloading.load()) {
        return;
    }

    BuildTimerFlyout();

    if (!g_timerFlyout ||
        !g_mainPage ||
        !g_settingsPage ||
        !g_timerReminderBox ||
        !g_timerHoursBox ||
        !g_timerMinutesBox ||
        !g_timerSecondsBox ||
        !g_soundNameText ||
        !g_bgNameText ||
        !g_timerStartButton ||
        !g_timerCancelButton)
    {
        return;
    }

    ModSettings settings = GetSettingsSnapshot();

    g_soundNameText.Text(L"Sound: " + GetSoundDisplayName());
    g_bgNameText.Text(L"Background: " + GetBackgroundDisplayName());

    RefreshChoiceButtons();

    bool keepFields = g_flyoutKeepFields.exchange(false);
    bool openSettings = g_flyoutShowSettings.exchange(false);

    ShowSettingsPage(openSettings);

    int remaining = RemainingSeconds(g_deadlineUtc100ns.load());

    if (remaining > 0) {
        wchar_t reminder[256]{};
        GetSharedReminder(reminder);

        g_timerReminderBox.Text(reminder);

        SetDurationBoxes(remaining);

        g_timerReminderBox.IsEnabled(false);
        g_timerHoursBox.IsEnabled(false);
        g_timerMinutesBox.IsEnabled(false);
        g_timerSecondsBox.IsEnabled(false);
        g_timerStartButton.IsEnabled(false);
        g_timerCancelButton.IsEnabled(true);
    }
    else {
        if (!keepFields) {
            g_timerReminderBox.Text(L"");
            SetDurationBoxes(settings.defaultMinutes * 60);
        }

        g_timerReminderBox.IsEnabled(true);
        g_timerHoursBox.IsEnabled(true);
        g_timerMinutesBox.IsEnabled(true);
        g_timerSecondsBox.IsEnabled(true);
        g_timerStartButton.IsEnabled(true);
        g_timerCancelButton.IsEnabled(false);
    }

    ApplyFlyoutLook();

    g_timerFlyout.ShowAt(g_timerButton);
}


// -----------------------------------------------------------------------------
// Completion alert (large full-screen or small notification)
// -----------------------------------------------------------------------------
//
// Large: a borderless topmost window covers the monitor that hosts the taskbar.
// Small: a compact topmost window in the bottom-right corner of the work area.
// The background is either a generated gradient, an image (decoded with WIC and
// cropped to "cover") or a looping video (Media Foundation MFPlay, large mode
// only). The centered card is frosted glass: the background under it is blurred
// and tinted. It holds the title, the reminder, snooze fields and the
// Snooze / Dismiss buttons.

constexpr int IDC_ALERT_REMINDER = 2100;
constexpr int IDC_ALERT_SNOOZE_LABEL = 2101;
constexpr int IDC_ALERT_SNOOZE_HOURS = 2102;
constexpr int IDC_ALERT_SNOOZE = 2103;
constexpr int IDC_ALERT_DISMISS = 2104;
constexpr int IDC_ALERT_SNOOZE_MINUTES = 2105;
constexpr int IDC_ALERT_SNOOZE_SECONDS = 2106;
constexpr int IDC_ALERT_UNIT_H = 2107;
constexpr int IDC_ALERT_UNIT_M = 2108;
constexpr int IDC_ALERT_UNIT_S = 2109;
constexpr int IDC_ALERT_TITLE = 2110;
constexpr int IDC_ALERT_SUBTITLE = 2111;
constexpr UINT WM_APP_ALERT_REFRESH = WM_APP + 20;

static constexpr wchar_t kFinishedAlertClass[] =
    L"TaskbarCountdownTimerFinishedAlert";
static constexpr wchar_t kFinishedAlertCardClass[] =
    L"TaskbarCountdownTimerAlertCard";

static bool RegisterFinishedAlertClass();
static void UnregisterFinishedAlertClass();

static HFONT g_finishedAlertFont = nullptr;
static HFONT g_finishedAlertTitleFont = nullptr;
static HFONT g_finishedAlertReminderFont = nullptr;
static HFONT g_finishedAlertSubFont = nullptr;
static HBRUSH g_finishedAlertBackgroundBrush = nullptr;
static HBRUSH g_finishedAlertEditBrush = nullptr;
static bool g_finishedAlertDark = true;
static COLORREF g_finishedAlertBackgroundColor = RGB(32, 32, 32);
static COLORREF g_finishedAlertEditBackgroundColor = RGB(45, 45, 45);
static COLORREF g_finishedAlertTextColor = RGB(245, 245, 245);

// State owned by the alert thread (only one alert window exists at a time).
static HWND g_alertCard = nullptr;
static HWND g_alertVideoWnd = nullptr;
static HBITMAP g_alertBgBitmap = nullptr;
static IMFPMediaPlayer* g_alertPlayer = nullptr;
static bool g_alertSmall = false;            // small notification mode
static HBITMAP g_alertGlassBmp = nullptr;    // blurred + tinted card background
static HBITMAP g_alertPosterBmp = nullptr;   // video frame used as blur source

// Animated particles drawn over gradient / image backgrounds.
struct AlertParticle
{
    float x, y, vx, vy, radius, phase;
    int r, g, b;
};

static AlertParticle g_particles[40];
static int g_particleCount = 0;
static HBITMAP g_alertFrameBmp = nullptr;
static uint32_t* g_alertFramePx = nullptr;
static const uint32_t* g_alertBgPx = nullptr;
static int g_alertW = 0;
static int g_alertH = 0;
static int g_alertFade = 0;
static unsigned g_alertTick = 0;

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
    HFONT titleFont = nullptr;
    HFONT reminderFont = nullptr;
    HFONT subFont = nullptr;
    HBRUSH backgroundBrush = nullptr;
    HBRUSH editBrush = nullptr;
};


static HFONT MakeAlertFont(int points, UINT dpi, int weight)
{
    return CreateFontW(
        -MulDiv(points, dpi ? dpi : 96, 72),
        0, 0, 0,
        weight,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
}


static FinishedAlertOldResources CreateFinishedAlertResources(UINT dpi)
{
    FinishedAlertOldResources old{
        g_finishedAlertFont,
        g_finishedAlertTitleFont,
        g_finishedAlertReminderFont,
        g_finishedAlertSubFont,
        g_finishedAlertBackgroundBrush,
        g_finishedAlertEditBrush,
    };

    {
        const int themePref = g_alertThemePref.load();

        g_finishedAlertDark =
            themePref == 1 ? false :
            themePref == 2 ? !AppsUseLightTheme() :
            true;
    }

    g_finishedAlertBackgroundColor =
        g_finishedAlertDark ? RGB(32, 32, 32) : RGB(249, 249, 249);

    g_finishedAlertEditBackgroundColor =
        g_finishedAlertDark ? RGB(45, 45, 45) : RGB(255, 255, 255);

    g_finishedAlertTextColor =
        g_finishedAlertDark ? RGB(245, 245, 245) : RGB(25, 25, 25);

    g_finishedAlertBackgroundBrush =
        CreateSolidBrush(g_finishedAlertBackgroundColor);

    g_finishedAlertEditBrush =
        CreateSolidBrush(g_finishedAlertEditBackgroundColor);

    g_finishedAlertFont = MakeAlertFont(11, dpi, FW_NORMAL);
    g_finishedAlertTitleFont =
        MakeAlertFont(g_alertSmall ? 18 : 36, dpi, FW_SEMIBOLD);
    g_finishedAlertReminderFont =
        MakeAlertFont(g_alertSmall ? 12 : 18, dpi, FW_NORMAL);
    g_finishedAlertSubFont =
        MakeAlertFont(g_alertSmall ? 10 : 11, dpi, FW_NORMAL);

    return old;
}


static void DeleteFinishedAlertResources(
    FinishedAlertOldResources const& resources)
{
    if (resources.font) DeleteObject(resources.font);
    if (resources.titleFont) DeleteObject(resources.titleFont);
    if (resources.reminderFont) DeleteObject(resources.reminderFont);
    if (resources.subFont) DeleteObject(resources.subFont);
    if (resources.backgroundBrush) DeleteObject(resources.backgroundBrush);
    if (resources.editBrush) DeleteObject(resources.editBrush);
}


static HWND AlertItem(int id)
{
    return g_alertCard ? GetDlgItem(g_alertCard, id) : nullptr;
}


static void ThemeFinishedAlertChild(HWND child)
{
    if (!child) {
        return;
    }

    if (g_finishedAlertFont) {
        SendMessageW(
            child,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(g_finishedAlertFont),
            TRUE
        );
    }

    wchar_t className[32]{};

    if (!GetClassNameW(child, className, ARRAYSIZE(className))) {
        return;
    }

    if (_wcsicmp(className, L"Button") == 0) {
        SetWindowTheme(
            child,
            g_finishedAlertDark ? L"DarkMode_Explorer" : L"Explorer",
            nullptr
        );
    }
    else if (_wcsicmp(className, L"Edit") == 0) {
        SetWindowTheme(
            child,
            g_finishedAlertDark ? L"DarkMode_CFD" : L"Explorer",
            nullptr
        );
    }
}


static void ApplyFinishedAlertTheme(
    HWND hWnd,
    UINT dpi)
{
    FinishedAlertOldResources old = CreateFinishedAlertResources(dpi);

    // Point every child at the new font/theme before deleting the old GDI
    // resources they may still reference.
    EnumChildWindows(
        hWnd,
        [](HWND child, LPARAM) -> BOOL
        {
            ThemeFinishedAlertChild(child);
            return TRUE;
        },
        0
    );

    auto setFont = [](int id, HFONT font)
    {
        if (HWND item = AlertItem(id)) {
            SendMessageW(item, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
        }
    };

    setFont(IDC_ALERT_TITLE, g_finishedAlertTitleFont);
    setFont(IDC_ALERT_REMINDER, g_finishedAlertReminderFont);
    setFont(IDC_ALERT_SUBTITLE, g_finishedAlertSubFont);

    InvalidateRect(hWnd, nullptr, TRUE);

    if (g_alertCard) {
        InvalidateRect(g_alertCard, nullptr, TRUE);
    }

    DeleteFinishedAlertResources(old);
}

struct FinishedAlertData
{
    wchar_t reminder[256];
    int defaultSnoozeSeconds;
    int maximumSeconds;
    bool preview;
    bool compactAlert;
};

static int ScaleAlertValue(int value, UINT dpi)
{
    return MulDiv(value, dpi ? dpi : 96, 96);
}


// ---- Background media ------------------------------------------------------

static bool PathHasExtension(
    const std::wstring& path,
    const wchar_t* const* list,
    size_t count)
{
    size_t dot = path.find_last_of(L'.');

    if (dot == std::wstring::npos) {
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        if (_wcsicmp(path.c_str() + dot, list[i]) == 0) {
            return true;
        }
    }

    return false;
}

static bool IsVideoPath(const std::wstring& path)
{
    static const wchar_t* const kExt[] = {
        L".mp4", L".m4v", L".mov", L".wmv", L".avi",
        L".mkv", L".webm", L".mpg", L".mpeg"
    };

    return PathHasExtension(path, kExt, ARRAYSIZE(kExt));
}

static bool IsImagePath(const std::wstring& path)
{
    static const wchar_t* const kExt[] = {
        L".jpg", L".jpeg", L".png", L".bmp", L".gif",
        L".tif", L".tiff", L".webp"
    };

    return PathHasExtension(path, kExt, ARRAYSIZE(kExt));
}


static HBITMAP CreateDibBitmap(int w, int h, uint32_t** bits)
{
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;   // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* data = nullptr;

    HBITMAP bitmap =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &data, nullptr, 0);

    *bits = static_cast<uint32_t*>(data);
    return bitmap;
}


static void DimPixels(uint32_t* px, size_t count, int dimPercent)
{
    if (dimPercent <= 0) {
        return;
    }

    const uint32_t scale =
        static_cast<uint32_t>((100 - std::min(dimPercent, 95)) * 256 / 100);

    for (size_t i = 0; i < count; i++) {
        uint32_t p = px[i];
        uint32_t r = (((p >> 16) & 0xFF) * scale) >> 8;
        uint32_t g = (((p >> 8) & 0xFF) * scale) >> 8;
        uint32_t b = ((p & 0xFF) * scale) >> 8;
        px[i] = (r << 16) | (g << 8) | b;
    }
}


// Dark blue/purple gradient with a soft glow behind the card.
static HBITMAP CreateDefaultBackground(int w, int h)
{
    uint32_t* px = nullptr;
    HBITMAP bitmap = CreateDibBitmap(w, h, &px);

    if (!bitmap || !px) {
        return bitmap;
    }

    const double cx = w * 0.5;
    const double cy = h * 0.42;
    const double radius = std::max(w, h) * 0.75;

    for (int y = 0; y < h; y++) {
        const double t = static_cast<double>(y) / h;
        const double baseR = 10 + 22 * t;
        const double baseG = 14 + 8 * t;
        const double baseB = 30 + 40 * t;

        for (int x = 0; x < w; x++) {
            const double dx = x - cx;
            const double dy = y - cy;
            const double d = std::sqrt(dx * dx + dy * dy) / radius;
            const double glow = d < 1.0 ? (1.0 - d) * (1.0 - d) : 0.0;

            int r = std::min(255, static_cast<int>(baseR + glow * 40));
            int g = std::min(255, static_cast<int>(baseG + glow * 70));
            int b = std::min(255, static_cast<int>(baseB + glow * 150));

            px[static_cast<size_t>(y) * w + x] =
                (static_cast<uint32_t>(r) << 16) |
                (static_cast<uint32_t>(g) << 8) |
                static_cast<uint32_t>(b);
        }
    }

    return bitmap;
}


// Decodes an image with WIC, scales it to cover w x h (centered crop).
static HBITMAP LoadCoverBitmap(
    const std::wstring& path,
    int w,
    int h,
    int dimPercent)
{
    HBITMAP bitmap = nullptr;

    try {
        auto factory =
            winrt::create_instance<IWICImagingFactory>(
                CLSID_WICImagingFactory,
                CLSCTX_INPROC_SERVER
            );

        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(
            factory->CreateDecoderFromFilename(
                path.c_str(),
                nullptr,
                GENERIC_READ,
                WICDecodeMetadataCacheOnDemand,
                decoder.put()
            )
        );

        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::check_hresult(decoder->GetFrame(0, frame.put()));

        UINT sw = 0;
        UINT sh = 0;
        winrt::check_hresult(frame->GetSize(&sw, &sh));

        if (!sw || !sh) {
            return nullptr;
        }

        const double scale =
            std::max(
                static_cast<double>(w) / sw,
                static_cast<double>(h) / sh
            );

        const UINT nw =
            std::max<UINT>(w, static_cast<UINT>(std::ceil(sw * scale)));
        const UINT nh =
            std::max<UINT>(h, static_cast<UINT>(std::ceil(sh * scale)));

        winrt::com_ptr<IWICBitmapScaler> scaler;
        winrt::check_hresult(factory->CreateBitmapScaler(scaler.put()));
        winrt::check_hresult(
            scaler->Initialize(
                frame.get(),
                nw,
                nh,
                WICBitmapInterpolationModeFant
            )
        );

        winrt::com_ptr<IWICFormatConverter> converter;
        winrt::check_hresult(factory->CreateFormatConverter(converter.put()));
        winrt::check_hresult(
            converter->Initialize(
                scaler.get(),
                GUID_WICPixelFormat32bppBGR,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0,
                WICBitmapPaletteTypeCustom
            )
        );

        uint32_t* px = nullptr;
        bitmap = CreateDibBitmap(w, h, &px);

        if (!bitmap || !px) {
            return nullptr;
        }

        WICRect rect{
            static_cast<INT>((nw - w) / 2),
            static_cast<INT>((nh - h) / 2),
            w,
            h
        };

        winrt::check_hresult(
            converter->CopyPixels(
                &rect,
                static_cast<UINT>(w) * 4,
                static_cast<UINT>(w) * h * 4,
                reinterpret_cast<BYTE*>(px)
            )
        );

        DimPixels(px, static_cast<size_t>(w) * h, dimPercent);

        return bitmap;
    }
    catch (...) {
        Wh_Log(
            L"Loading background image failed: 0x%08X",
            static_cast<unsigned>(winrt::to_hresult().value)
        );

        if (bitmap) {
            DeleteObject(bitmap);
        }

        return nullptr;
    }
}


// Loops the video and keeps it muted unless the setting says otherwise.
class AlertVideoCallback final : public IMFPMediaPlayerCallback
{
public:
    explicit AlertVideoCallback(bool mute) : m_mute(mute) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (!ppv) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, __uuidof(IMFPMediaPlayerCallback)))
        {
            *ppv = static_cast<IMFPMediaPlayerCallback*>(this);
            AddRef();
            return S_OK;
        }

        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return static_cast<ULONG>(++m_ref);
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        ULONG r = static_cast<ULONG>(--m_ref);

        if (!r) {
            delete this;
        }

        return r;
    }

    void STDMETHODCALLTYPE OnMediaPlayerEvent(
        MFP_EVENT_HEADER* header) override
    {
        if (!header || !header->pMediaPlayer || FAILED(header->hrEvent)) {
            return;
        }

        switch (header->eEventType) {
        case MFP_EVENT_TYPE_MEDIAITEM_SET:
            header->pMediaPlayer->SetMute(m_mute ? TRUE : FALSE);
            break;

        case MFP_EVENT_TYPE_PLAYBACK_ENDED:
        {
            PROPVARIANT position;
            PropVariantInit(&position);
            position.vt = VT_I8;
            position.hVal.QuadPart = 0;

            header->pMediaPlayer->SetPosition(GUID_NULL, &position);
            header->pMediaPlayer->Play();
            break;
        }

        default:
            break;
        }
    }

private:
    std::atomic<LONG> m_ref{1};
    bool m_mute;
};

using MFPCreateMediaPlayer_t =
    HRESULT (WINAPI*)(
        LPCWSTR,
        BOOL,
        MFP_CREATION_OPTIONS,
        IMFPMediaPlayerCallback*,
        HWND,
        IMFPMediaPlayer**);


static bool StartAlertVideo(
    HWND hWnd,
    const ModSettings& settings,
    int w,
    int h)
{
    HMODULE mfplay = LoadLibraryW(L"mfplay.dll");

    auto create =
        mfplay
            ? reinterpret_cast<MFPCreateMediaPlayer_t>(
                  GetProcAddress(mfplay, "MFPCreateMediaPlayer"))
            : nullptr;

    if (!create) {
        Wh_Log(L"mfplay.dll / MFPCreateMediaPlayer not available");
        return false;
    }

    HWND videoWnd =
        CreateWindowExW(
            0,
            L"STATIC",
            nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_BLACKRECT,
            0, 0, w, h,
            hWnd,
            nullptr,
            nullptr,
            nullptr
        );

    if (!videoWnd) {
        return false;
    }

    auto* callback =
        new (std::nothrow) AlertVideoCallback(!settings.videoSound);

    if (!callback) {
        DestroyWindow(videoWnd);
        return false;
    }

    IMFPMediaPlayer* player = nullptr;

    HRESULT hr =
        create(
            settings.customBackgroundPath.c_str(),
            TRUE,
            MFP_OPTION_NONE,
            callback,
            videoWnd,
            &player
        );

    callback->Release();   // the player holds its own reference

    if (FAILED(hr) || !player) {
        Wh_Log(L"MFPCreateMediaPlayer failed: 0x%08X", static_cast<unsigned>(hr));
        DestroyWindow(videoWnd);
        return false;
    }

    g_alertVideoWnd = videoWnd;
    g_alertPlayer = player;
    return true;
}


// Takes a frame of the video (shell thumbnail) and crops it to cover w x h.
// It is only the source of the blurred frosted card; the live video itself
// stays unchanged around the card.
static HBITMAP GetVideoPosterBitmap(
    const std::wstring& path,
    int w,
    int h)
{
    IShellItemImageFactory* factory = nullptr;

    if (FAILED(
            SHCreateItemFromParsingName(
                path.c_str(),
                nullptr,
                IID_PPV_ARGS(&factory))) ||
        !factory)
    {
        return nullptr;
    }

    HBITMAP thumb = nullptr;
    SIZE size{std::max(w / 2, 256), std::max(h / 2, 144)};

    HRESULT hr =
        factory->GetImage(
            size,
            SIIGBF_BIGGERSIZEOK | SIIGBF_THUMBNAILONLY,
            &thumb
        );

    factory->Release();

    if (FAILED(hr) || !thumb) {
        Wh_Log(L"Video thumbnail not available: 0x%08X", static_cast<unsigned>(hr));
        return nullptr;
    }

    BITMAP bm{};
    GetObjectW(thumb, sizeof(bm), &bm);

    const int sw = bm.bmWidth;
    const int sh = bm.bmHeight;

    if (sw <= 0 || sh <= 0) {
        DeleteObject(thumb);
        return nullptr;
    }

    std::vector<uint32_t> src(static_cast<size_t>(sw) * sh);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, thumb, 0, sh, src.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(thumb);

    if (!lines) {
        return nullptr;
    }

    uint32_t* px = nullptr;
    HBITMAP out = CreateDibBitmap(w, h, &px);

    if (!out || !px) {
        if (out) {
            DeleteObject(out);
        }

        return nullptr;
    }

    const double scale =
        std::max(static_cast<double>(w) / sw, static_cast<double>(h) / sh);
    const double offX = (sw * scale - w) / 2.0;
    const double offY = (sh * scale - h) / 2.0;

    for (int y = 0; y < h; y++) {
        const int sy =
            std::clamp(static_cast<int>((y + offY) / scale), 0, sh - 1);

        for (int x = 0; x < w; x++) {
            const int sx =
                std::clamp(static_cast<int>((x + offX) / scale), 0, sw - 1);

            px[static_cast<size_t>(y) * w + x] =
                src[static_cast<size_t>(sy) * sw + sx] & 0x00FFFFFF;
        }
    }

    return out;
}


static void CreateAlertBackground(
    HWND hWnd,
    int w,
    int h)
{
    ModSettings settings = GetSettingsSnapshot();
    const std::wstring& path = settings.customBackgroundPath;

    bool videoStarted = false;

    if (!path.empty()) {
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Background file not found: %s", path.c_str());
        }
        else if (IsVideoPath(path)) {
            // The small notification is covered by its card, so a video
            // would never be visible there: use the gradient instead.
            if (!g_alertSmall) {
                videoStarted = StartAlertVideo(hWnd, settings, w, h);

                // Frosted glass option: blur a frame of the video behind the card.
                if (videoStarted && g_flyoutGlassPref.load()) {
                    g_alertPosterBmp = GetVideoPosterBitmap(path, w, h);
                }
            }
        }
        else if (IsImagePath(path)) {
            g_alertBgBitmap =
                LoadCoverBitmap(path, w, h, settings.backgroundDim);
        }
    }

    if (!videoStarted && !g_alertBgBitmap) {
        g_alertBgBitmap = CreateDefaultBackground(w, h);
    }
}


static void ReleaseAlertBackground()
{
    if (g_alertPlayer) {
        g_alertPlayer->Shutdown();
        g_alertPlayer->Release();
        g_alertPlayer = nullptr;
    }

    g_alertVideoWnd = nullptr;   // destroyed together with the parent

    if (g_alertFrameBmp) {
        DeleteObject(g_alertFrameBmp);
        g_alertFrameBmp = nullptr;
    }

    g_alertFramePx = nullptr;
    g_alertBgPx = nullptr;
    g_particleCount = 0;

    if (g_alertGlassBmp) {
        DeleteObject(g_alertGlassBmp);
        g_alertGlassBmp = nullptr;
    }

    if (g_alertBgBitmap) {
        DeleteObject(g_alertBgBitmap);
        g_alertBgBitmap = nullptr;
    }

    if (g_alertPosterBmp) {
        DeleteObject(g_alertPosterBmp);
        g_alertPosterBmp = nullptr;
    }
}


// Soft glowing orbs that drift upward and pulse, composited over the
// background into a frame buffer that WM_ERASEBKGND blits.
static void InitAlertAnimation(int w, int h)
{
    if (!g_alertBgBitmap) {
        return;
    }

    DIBSECTION section{};

    if (!GetObjectW(g_alertBgBitmap, sizeof(section), &section) ||
        !section.dsBm.bmBits)
    {
        return;
    }

    g_alertBgPx = static_cast<const uint32_t*>(section.dsBm.bmBits);

    g_alertFrameBmp = CreateDibBitmap(w, h, &g_alertFramePx);

    if (!g_alertFrameBmp || !g_alertFramePx) {
        g_alertFramePx = nullptr;
        g_alertBgPx = nullptr;
        return;
    }

    g_alertW = w;
    g_alertH = h;

    static const int palette[3][3] = {
        {70, 140, 255}, {160, 90, 255}, {50, 210, 230}
    };

    const float unit = h / 1080.0f;
    g_particleCount = ARRAYSIZE(g_particles);

    for (int i = 0; i < g_particleCount; i++) {
        auto& p = g_particles[i];
        const auto* c = palette[i % 3];

        p.x = static_cast<float>(rand() % std::max(1, w));
        p.y = static_cast<float>(rand() % std::max(1, h));
        p.vx = ((rand() % 100) / 100.0f - 0.5f) * 0.6f * unit;
        p.vy = -(0.25f + (rand() % 100) / 100.0f * 0.9f) * unit;
        p.radius = (28 + rand() % 70) * unit;
        p.phase = (rand() % 628) / 100.0f;
        p.r = c[0];
        p.g = c[1];
        p.b = c[2];
    }
}


static void RenderAlertFrame()
{
    if (!g_alertFramePx || !g_alertBgPx) {
        return;
    }

    const int w = g_alertW;
    const int h = g_alertH;

    memcpy(g_alertFramePx, g_alertBgPx, static_cast<size_t>(w) * h * 4);

    g_alertTick++;
    const float t = g_alertTick * 0.05f;

    for (int i = 0; i < g_particleCount; i++) {
        auto& p = g_particles[i];

        p.x += p.vx + std::sin(t + p.phase) * 0.35f;
        p.y += p.vy;

        if (p.y < -p.radius) {
            p.y = h + p.radius;
            p.x = static_cast<float>(rand() % std::max(1, w));
        }

        if (p.x < -p.radius) p.x = w + p.radius;
        if (p.x > w + p.radius) p.x = -p.radius;

        const float pulse = 0.55f + 0.45f * std::sin(t * 1.3f + p.phase);
        const float intensity = 0.42f * pulse;
        const int rad = static_cast<int>(p.radius);
        const float r2 = p.radius * p.radius;

        const int x0 = std::max(0, static_cast<int>(p.x) - rad);
        const int x1 = std::min(w - 1, static_cast<int>(p.x) + rad);
        const int y0 = std::max(0, static_cast<int>(p.y) - rad);
        const int y1 = std::min(h - 1, static_cast<int>(p.y) + rad);

        for (int y = y0; y <= y1; y++) {
            const float dy = y - p.y;
            uint32_t* row = g_alertFramePx + static_cast<size_t>(y) * w;

            for (int x = x0; x <= x1; x++) {
                const float dx = x - p.x;
                const float d2 = dx * dx + dy * dy;

                if (d2 >= r2) {
                    continue;
                }

                float f = 1.0f - d2 / r2;
                f = f * f * intensity;

                uint32_t px = row[x];
                int r = static_cast<int>((px >> 16) & 0xFF) + static_cast<int>(p.r * f);
                int g = static_cast<int>((px >> 8) & 0xFF) + static_cast<int>(p.g * f);
                int b = static_cast<int>(px & 0xFF) + static_cast<int>(p.b * f);

                row[x] =
                    (static_cast<uint32_t>(std::min(r, 255)) << 16) |
                    (static_cast<uint32_t>(std::min(g, 255)) << 8) |
                    static_cast<uint32_t>(std::min(b, 255));
            }
        }
    }
}


// ---- Frosted glass ---------------------------------------------------------

static void BoxBlurPass(
    const uint32_t* src,
    uint32_t* dst,
    int w,
    int h,
    int r,
    bool horizontal)
{
    const int len = horizontal ? w : h;
    const int lines = horizontal ? h : w;
    const size_t stride = horizontal ? 1 : static_cast<size_t>(w);
    const size_t lineStep = horizontal ? static_cast<size_t>(w) : 1;
    const int win = 2 * r + 1;

    for (int l = 0; l < lines; l++) {
        const uint32_t* s = src + static_cast<size_t>(l) * lineStep;
        uint32_t* d = dst + static_cast<size_t>(l) * lineStep;

        auto at = [&](int i) -> uint32_t {
            i = std::clamp(i, 0, len - 1);
            return s[static_cast<size_t>(i) * stride];
        };

        int sr = 0, sg = 0, sb = 0;

        for (int i = -r; i <= r; i++) {
            uint32_t p = at(i);
            sr += (p >> 16) & 0xFF;
            sg += (p >> 8) & 0xFF;
            sb += p & 0xFF;
        }

        for (int i = 0; i < len; i++) {
            d[static_cast<size_t>(i) * stride] =
                (static_cast<uint32_t>(sr / win) << 16) |
                (static_cast<uint32_t>(sg / win) << 8) |
                static_cast<uint32_t>(sb / win);

            uint32_t out = at(i - r);
            uint32_t in = at(i + r + 1);

            sr += static_cast<int>((in >> 16) & 0xFF) -
                  static_cast<int>((out >> 16) & 0xFF);
            sg += static_cast<int>((in >> 8) & 0xFF) -
                  static_cast<int>((out >> 8) & 0xFF);
            sb += static_cast<int>(in & 0xFF) -
                  static_cast<int>(out & 0xFF);
        }
    }
}


// Blurs the part of the background that sits under the card and tints it,
// so the card looks like frosted glass.
static void BuildCardGlass(int cx, int cy, int cw, int ch, UINT dpi)
{
    if (g_alertGlassBmp) {
        DeleteObject(g_alertGlassBmp);
        g_alertGlassBmp = nullptr;
    }

    // Image / gradient background, or a frame of the video when the frosted
    // glass option is on. Without either the card stays solid.
    HBITMAP sourceBmp = g_alertBgBitmap ? g_alertBgBitmap : g_alertPosterBmp;

    if (!sourceBmp || cw <= 0 || ch <= 0) {
        return;
    }

    DIBSECTION section{};

    if (!GetObjectW(sourceBmp, sizeof(section), &section) ||
        !section.dsBm.bmBits)
    {
        return;
    }

    const uint32_t* bg = static_cast<const uint32_t*>(section.dsBm.bmBits);
    const int bw = section.dsBm.bmWidth;
    const int bh = section.dsBm.bmHeight;

    std::vector<uint32_t> a(static_cast<size_t>(cw) * ch);
    std::vector<uint32_t> b(a.size());

    for (int y = 0; y < ch; y++) {
        const int sy = std::clamp(cy + y, 0, bh - 1);

        for (int x = 0; x < cw; x++) {
            const int sx = std::clamp(cx + x, 0, bw - 1);

            a[static_cast<size_t>(y) * cw + x] =
                bg[static_cast<size_t>(sy) * bw + sx];
        }
    }

    const int r = std::max(2, ScaleAlertValue(14, dpi));

    BoxBlurPass(a.data(), b.data(), cw, ch, r, true);
    BoxBlurPass(b.data(), a.data(), cw, ch, r, false);
    BoxBlurPass(a.data(), b.data(), cw, ch, r, true);
    BoxBlurPass(b.data(), a.data(), cw, ch, r, false);

    const ModSettings settings = GetSettingsSnapshot();

    // The light look lifts the blurred pixels toward white first and then
    // uses a moderate tint, so the card stays bright but the blur remains
    // visible (a heavy white tint would hide it completely).
    const int alpha =
        g_finishedAlertDark
            ? std::clamp(settings.glassTint, 0, 90)
            : std::clamp(settings.glassTint + 10, 30, 70);

    const int tr = g_finishedAlertDark ? 22 : 250;
    const int tg = g_finishedAlertDark ? 24 : 250;
    const int tb = g_finishedAlertDark ? 32 : 252;

    uint32_t* out = nullptr;
    HBITMAP glass = CreateDibBitmap(cw, ch, &out);

    if (!glass || !out) {
        if (glass) {
            DeleteObject(glass);
        }

        return;
    }

    for (size_t i = 0; i < a.size(); i++) {
        const uint32_t p = a[i];
        int r0 = (p >> 16) & 0xFF;
        int g0 = (p >> 8) & 0xFF;
        int b0 = p & 0xFF;

        if (!g_finishedAlertDark) {
            // Lift toward white so the frosted look stays bright.
            r0 += (255 - r0) * 35 / 100;
            g0 += (255 - g0) * 35 / 100;
            b0 += (255 - b0) * 35 / 100;
        }

        out[i] =
            (static_cast<uint32_t>((r0 * (100 - alpha) + tr * alpha) / 100) << 16) |
            (static_cast<uint32_t>((g0 * (100 - alpha) + tg * alpha) / 100) << 8) |
            static_cast<uint32_t>((b0 * (100 - alpha) + tb * alpha) / 100);
    }

    g_alertGlassBmp = glass;
}


// Static text is transparent over the glass, so after changing a label the
// area behind it has to be repainted or the old text stays visible.
static void SetAlertText(int id, const wchar_t* text)
{
    HWND item = AlertItem(id);

    if (!item) {
        return;
    }

    SetWindowTextW(item, text);

    if (g_alertGlassBmp && g_alertCard) {
        RECT rc{};
        GetWindowRect(item, &rc);

        MapWindowPoints(
            nullptr,
            g_alertCard,
            reinterpret_cast<LPPOINT>(&rc),
            2
        );

        RedrawWindow(
            g_alertCard,
            &rc,
            nullptr,
            RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW
        );
    }
}


// ---- Layout and controls ---------------------------------------------------

static void SetAlertSnoozeFields(int totalSeconds)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    wchar_t text[16]{};

    swprintf_s(text, L"%d", totalSeconds / 3600);
    SetWindowTextW(AlertItem(IDC_ALERT_SNOOZE_HOURS), text);

    swprintf_s(text, L"%d", (totalSeconds % 3600) / 60);
    SetWindowTextW(AlertItem(IDC_ALERT_SNOOZE_MINUTES), text);

    swprintf_s(text, L"%d", totalSeconds % 60);
    SetWindowTextW(AlertItem(IDC_ALERT_SNOOZE_SECONDS), text);
}

static int ReadAlertField(int id)
{
    wchar_t text[16]{};

    GetWindowTextW(AlertItem(id), text, ARRAYSIZE(text));

    return _wtoi(text);
}

static void SetAlertTexts(const FinishedAlertData* data)
{
    const bool preview = data && data->preview;

    SetAlertText(
        IDC_ALERT_TITLE,
        preview ? L"Preview" : L"Time's up"
    );

    if (preview) {
        SetAlertText(
            IDC_ALERT_SUBTITLE,
            L"Your timer alert"
        );
    }
    else {
        SYSTEMTIME now{};
        GetLocalTime(&now);

        wchar_t text[64]{};
        swprintf_s(text, L"Finished at %02d:%02d", now.wHour, now.wMinute);
        SetAlertText(IDC_ALERT_SUBTITLE, text);
    }

    SetAlertText(
        IDC_ALERT_REMINDER,
        data && data->reminder[0] ? data->reminder : L"Timer finished"
    );

    SetWindowTextW(
        AlertItem(IDC_ALERT_DISMISS),
        preview ? L"Close preview" : L"Dismiss"
    );
}

static void LayoutFinishedAlert(HWND hWnd, UINT dpi)
{
    if (!g_alertCard) {
        return;
    }

    RECT client{};
    GetClientRect(hWnd, &client);

    const int W = client.right;
    const int H = client.bottom;

    if (g_alertVideoWnd) {
        SetWindowPos(
            g_alertVideoWnd,
            HWND_BOTTOM,
            0, 0, W, H,
            SWP_NOACTIVATE
        );

        if (g_alertPlayer) {
            g_alertPlayer->UpdateVideo();
        }
    }

    auto* data =
        reinterpret_cast<FinishedAlertData*>(
            GetWindowLongPtrW(hWnd, GWLP_USERDATA)
        );

    const bool preview = data && data->preview;

    const int cardW =
        g_alertSmall ? W : std::min(ScaleAlertValue(580, dpi), W);
    const int cardH =
        g_alertSmall ? H : std::min(ScaleAlertValue(400, dpi), H);
    const int cardX = (W - cardW) / 2;
    const int cardY = (H - cardH) / 2;
    const int radius = ScaleAlertValue(g_alertSmall ? 20 : 28, dpi);

    SetWindowPos(
        g_alertCard,
        nullptr,
        cardX,
        cardY,
        cardW,
        cardH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowRgn(
        g_alertCard,
        CreateRoundRectRgn(0, 0, cardW + 1, cardH + 1, radius, radius),
        TRUE
    );

    if (g_alertSmall) {
        // The small alert is the card itself, so round the window too.
        SetWindowRgn(
            hWnd,
            CreateRoundRectRgn(0, 0, W + 1, H + 1, radius, radius),
            TRUE
        );
    }

    struct Item {
        int id, x, y, w, h;
    };

    static const Item kLarge[] = {
        {IDC_ALERT_TITLE,          30,  38, 520, 64},
        {IDC_ALERT_SUBTITLE,       30, 104, 520, 24},
        {IDC_ALERT_REMINDER,       40, 148, 500, 72},
        {IDC_ALERT_SNOOZE_LABEL,  123, 252, 100, 22},
        {IDC_ALERT_SNOOZE_HOURS,  229, 246,  52, 32},
        {IDC_ALERT_UNIT_H,        285, 252,  16, 22},
        {IDC_ALERT_SNOOZE_MINUTES,307, 246,  52, 32},
        {IDC_ALERT_UNIT_M,        363, 252,  18, 22},
        {IDC_ALERT_SNOOZE_SECONDS,385, 246,  52, 32},
        {IDC_ALERT_UNIT_S,        441, 252,  16, 22},
        {IDC_ALERT_SNOOZE,         40, 322, 240, 48},
        {IDC_ALERT_DISMISS,       300, 322, 240, 48},
    };

    // Small notification: card is 400 x 216 (at 96 dpi).
    static const Item kSmall[] = {
        {IDC_ALERT_TITLE,          20,  14, 360, 36},
        {IDC_ALERT_SUBTITLE,       20,  50, 360, 20},
        {IDC_ALERT_REMINDER,       24,  76, 352, 44},
        {IDC_ALERT_SNOOZE_LABEL,   59, 130,  90, 22},
        {IDC_ALERT_SNOOZE_HOURS,  153, 125,  44, 30},
        {IDC_ALERT_UNIT_H,        199, 130,  14, 22},
        {IDC_ALERT_SNOOZE_MINUTES,217, 125,  44, 30},
        {IDC_ALERT_UNIT_M,        263, 130,  16, 22},
        {IDC_ALERT_SNOOZE_SECONDS,281, 125,  44, 30},
        {IDC_ALERT_UNIT_S,        327, 130,  14, 22},
        {IDC_ALERT_SNOOZE,         20, 168, 176, 36},
        {IDC_ALERT_DISMISS,       204, 168, 176, 36},
    };

    const Item* items = g_alertSmall ? kSmall : kLarge;
    const size_t itemCount =
        g_alertSmall ? ARRAYSIZE(kSmall) : ARRAYSIZE(kLarge);

    for (size_t i = 0; i < itemCount; i++) {
        const Item& item = items[i];
        HWND child = AlertItem(item.id);

        if (!child) {
            continue;
        }

        int x = item.x;
        int w = item.w;

        const bool snoozeRelated =
            item.id == IDC_ALERT_SNOOZE_LABEL ||
            item.id == IDC_ALERT_SNOOZE_HOURS ||
            item.id == IDC_ALERT_SNOOZE_MINUTES ||
            item.id == IDC_ALERT_SNOOZE_SECONDS ||
            item.id == IDC_ALERT_UNIT_H ||
            item.id == IDC_ALERT_UNIT_M ||
            item.id == IDC_ALERT_UNIT_S ||
            item.id == IDC_ALERT_SNOOZE;

        if (preview && item.id == IDC_ALERT_DISMISS) {
            x = g_alertSmall ? 20 : 150;
            w = g_alertSmall ? 360 : 280;
        }

        ShowWindow(
            child,
            preview && snoozeRelated ? SW_HIDE : SW_SHOWNA
        );

        SetWindowPos(
            child,
            nullptr,
            ScaleAlertValue(x, dpi),
            ScaleAlertValue(item.y, dpi),
            ScaleAlertValue(w, dpi),
            ScaleAlertValue(item.h, dpi),
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    BuildCardGlass(cardX, cardY, cardW, cardH, dpi);

    RedrawWindow(
        g_alertCard,
        nullptr,
        nullptr,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN
    );
}

static HWND CreateAlertEdit(HWND parent, int id)
{
    HWND edit =
        CreateWindowExW(
            0,
            L"EDIT",
            L"0",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_CENTER,
            0, 0, 0, 0,
            parent,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            nullptr,
            nullptr
        );

    if (edit) {
        SendMessageW(edit, EM_LIMITTEXT, 5, 0);
    }

    return edit;
}

static HWND CreateAlertStatic(
    HWND parent,
    int id,
    const wchar_t* text,
    DWORD extraStyle = 0)
{
    return CreateWindowExW(
        0,
        L"STATIC",
        text,
        WS_CHILD | WS_VISIBLE | SS_NOPREFIX | extraStyle,
        0, 0, 0, 0,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        nullptr,
        nullptr
    );
}

static LRESULT HandleAlertCtlColor(UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc = reinterpret_cast<HDC>(wParam);

    const bool edit = msg == WM_CTLCOLOREDIT;

    const bool isTitle =
        !edit &&
        GetDlgCtrlID(reinterpret_cast<HWND>(lParam)) == IDC_ALERT_TITLE;

    SetTextColor(
        hdc,
        isTitle
            ? (g_finishedAlertDark ? RGB(96, 180, 255) : RGB(0, 95, 184))
            : g_finishedAlertTextColor
    );

    // Over the frosted glass the labels are transparent so the blur shows.
    if (!edit && g_alertGlassBmp) {
        SetBkMode(hdc, TRANSPARENT);
        return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH));
    }

    SetBkColor(
        hdc,
        edit
            ? g_finishedAlertEditBackgroundColor
            : g_finishedAlertBackgroundColor
    );

    HBRUSH brush =
        edit ? g_finishedAlertEditBrush : g_finishedAlertBackgroundBrush;

    if (!brush) {
        brush = GetSysColorBrush(COLOR_WINDOW);
    }

    return reinterpret_cast<LRESULT>(brush);
}


// The card hosts the controls; it paints its own rounded frosted background
// and an accent bar, and forwards control notifications to the alert window.
static LRESULT CALLBACK FinishedAlertCardProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND:
    {
        RECT rect{};
        GetClientRect(hWnd, &rect);

        HDC hdc = reinterpret_cast<HDC>(wParam);

        HBRUSH background =
            g_finishedAlertBackgroundBrush
                ? g_finishedAlertBackgroundBrush
                : GetSysColorBrush(COLOR_WINDOW);

        if (g_alertGlassBmp) {
            HDC memory = CreateCompatibleDC(hdc);
            HGDIOBJ oldBmp = SelectObject(memory, g_alertGlassBmp);

            BitBlt(
                hdc,
                0, 0,
                rect.right, rect.bottom,
                memory,
                0, 0,
                SRCCOPY
            );

            SelectObject(memory, oldBmp);
            DeleteDC(memory);
        }
        else {
            FillRect(hdc, &rect, background);
        }

        UINT dpi = GetDpiForWindow(hWnd);

        // Blue -> violet gradient accent bar.
        const int barHeight = ScaleAlertValue(6, dpi);
        const int width = std::max<int>(1, rect.right);

        for (int x = 0; x < rect.right; x++) {
            const float t = static_cast<float>(x) / width;

            HBRUSH accent =
                CreateSolidBrush(
                    RGB(
                        static_cast<int>(0 + 150 * t),
                        static_cast<int>(120 - 30 * t),
                        static_cast<int>(215 + 40 * t)
                    )
                );

            RECT column{x, 0, x + 1, barHeight};
            FillRect(hdc, &column, accent);
            DeleteObject(accent);
        }

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
        return HandleAlertCtlColor(msg, wParam, lParam);

    case WM_COMMAND:
        return SendMessageW(GetParent(hWnd), msg, wParam, lParam);
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
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
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);

        auto* data =
            reinterpret_cast<FinishedAlertData*>(create->lpCreateParams);

        SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(data)
        );

        UINT dpi = GetDpiForWindow(hWnd);

        if (!dpi) {
            dpi = 96;
        }

        g_alertSmall = data && data->compactAlert;

        // Background first so the card is created above the video window.
        CreateAlertBackground(hWnd, create->cx, create->cy);

        g_alertCard =
            CreateWindowExW(
                WS_EX_CONTROLPARENT,
                kFinishedAlertCardClass,
                nullptr,
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                0, 0, 10, 10,
                hWnd,
                nullptr,
                g_modInstance,
                nullptr
            );

        if (!g_alertCard) {
            return -1;
        }

        HWND card = g_alertCard;

        CreateAlertStatic(card, IDC_ALERT_TITLE, L"", SS_CENTER);
        CreateAlertStatic(card, IDC_ALERT_SUBTITLE, L"", SS_CENTER);
        CreateAlertStatic(card, IDC_ALERT_REMINDER, L"", SS_CENTER);

        CreateAlertStatic(card, IDC_ALERT_SNOOZE_LABEL, L"Snooze for:");
        CreateAlertEdit(card, IDC_ALERT_SNOOZE_HOURS);
        CreateAlertStatic(card, IDC_ALERT_UNIT_H, L"h");
        CreateAlertEdit(card, IDC_ALERT_SNOOZE_MINUTES);
        CreateAlertStatic(card, IDC_ALERT_UNIT_M, L"m");
        CreateAlertEdit(card, IDC_ALERT_SNOOZE_SECONDS);
        CreateAlertStatic(card, IDC_ALERT_UNIT_S, L"s");

        CreateWindowExW(
            0,
            L"BUTTON",
            L"Snooze",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            0, 0, 0, 0,
            card,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_ALERT_SNOOZE)),
            nullptr,
            nullptr
        );

        CreateWindowExW(
            0,
            L"BUTTON",
            L"Dismiss",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            0, 0, 0, 0,
            card,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_ALERT_DISMISS)),
            nullptr,
            nullptr
        );

        SetAlertTexts(data);

        SetAlertSnoozeFields(
            data ? data->defaultSnoozeSeconds : 300
        );

        ApplyFinishedAlertTheme(hWnd, dpi);
        LayoutFinishedAlert(hWnd, dpi);

        // Fade-in (and animated particles in large mode). Not used with
        // video, where a layered window could break the video renderer.
        if (!g_alertVideoWnd) {
            if (!g_alertSmall) {
                InitAlertAnimation(create->cx, create->cy);
            }

            SetWindowLongPtrW(
                hWnd,
                GWL_EXSTYLE,
                GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED
            );

            g_alertFade = 0;
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            SetTimer(hWnd, 1, 33, nullptr);
        }

        return 0;
    }

    case WM_TIMER:
        if (wParam == 1) {
            if (g_alertFade < 255) {
                g_alertFade = std::min(255, g_alertFade + 28);
                SetLayeredWindowAttributes(
                    hWnd,
                    0,
                    static_cast<BYTE>(g_alertFade),
                    LWA_ALPHA
                );
            }

            if (g_alertFramePx) {
                RenderAlertFrame();
                InvalidateRect(hWnd, nullptr, TRUE);
            }

            return 0;
        }
        break;

    case WM_DPICHANGED:
    {
        UINT dpi = HIWORD(wParam);

        ApplyFinishedAlertTheme(hWnd, dpi);
        LayoutFinishedAlert(hWnd, dpi);

        return 0;
    }

    case WM_ERASEBKGND:
    {
        RECT rect{};
        GetClientRect(hWnd, &rect);

        HDC hdc = reinterpret_cast<HDC>(wParam);

        HBITMAP source =
            g_alertFrameBmp ? g_alertFrameBmp : g_alertBgBitmap;

        if (source) {
            HDC memory = CreateCompatibleDC(hdc);
            HGDIOBJ old = SelectObject(memory, source);

            BITMAP info{};
            GetObjectW(source, sizeof(info), &info);

            BitBlt(
                hdc,
                0, 0,
                std::min<int>(rect.right, info.bmWidth),
                std::min<int>(rect.bottom, info.bmHeight),
                memory,
                0, 0,
                SRCCOPY
            );

            SelectObject(memory, old);
            DeleteDC(memory);
        }
        else {
            FillRect(
                hdc,
                &rect,
                static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))
            );
        }

        return 1;
    }

    case WM_THEMECHANGED:
        ApplyFinishedAlertTheme(hWnd, GetDpiForWindow(hWnd));
        LayoutFinishedAlert(hWnd, GetDpiForWindow(hWnd));
        return 0;

    case WM_SETTINGCHANGE:
        if (lParam &&
            _wcsicmp(
                reinterpret_cast<LPCWSTR>(lParam),
                L"ImmersiveColorSet") == 0)
        {
            ApplyFinishedAlertTheme(hWnd, GetDpiForWindow(hWnd));
            LayoutFinishedAlert(hWnd, GetDpiForWindow(hWnd));
        }
        return 0;

    case WM_APP_ALERT_REFRESH:
    {
        auto* incoming = reinterpret_cast<FinishedAlertData*>(lParam);

        if (incoming) {
            auto* current =
                reinterpret_cast<FinishedAlertData*>(
                    GetWindowLongPtrW(hWnd, GWLP_USERDATA)
                );

            if (current) {
                // The size of an open alert doesn't change mid-way.
                const bool keepSmall = current->compactAlert;

                *current = *incoming;
                current->compactAlert = keepSmall;

                SetAlertTexts(current);
                SetAlertSnoozeFields(current->defaultSnoozeSeconds);
                LayoutFinishedAlert(hWnd, GetDpiForWindow(hWnd));
            }

            delete incoming;
        }

        if (!g_alertSmall) {
            SetForegroundWindow(hWnd);
        }

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

            if (data && data->preview) {
                DestroyWindow(hWnd);
                return 0;
            }

            long long total =
                ReadAlertField(IDC_ALERT_SNOOZE_HOURS) * 3600LL +
                ReadAlertField(IDC_ALERT_SNOOZE_MINUTES) * 60LL +
                ReadAlertField(IDC_ALERT_SNOOZE_SECONDS);

            long long maximum =
                data ? data->maximumSeconds : 86400;

            if (total <= 0 || total > maximum) {
                SetAlertText(IDC_ALERT_SNOOZE_LABEL, L"Invalid time:");

                HWND edit = AlertItem(IDC_ALERT_SNOOZE_MINUTES);

                SetFocus(edit);
                SendMessageW(edit, EM_SETSEL, 0, -1);
                MessageBeep(MB_ICONWARNING);
                return 0;
            }

            SetAlertText(IDC_ALERT_SNOOZE_LABEL, L"Snooze for:");

            const wchar_t* reminder =
                data && data->reminder[0]
                    ? data->reminder
                    : L"Timer finished";

            if (!ArmTimer(static_cast<int>(total), reminder)) {
                MessageBeep(MB_ICONERROR);
                return 0;
            }

            StopCompletionSound();
            DestroyWindow(hWnd);
            return 0;
        }

        if (id == IDC_ALERT_DISMISS) {
            StopCompletionSound();
            DestroyWindow(hWnd);
            return 0;
        }

        break;
    }

    case WM_CLOSE:
        StopCompletionSound();
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        ReleaseAlertBackground();
        g_alertCard = nullptr;
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
            reinterpret_cast<LPCWSTR>(FinishedAlertWndProc),
            &modInstance))
    {
        Wh_Log(L"ERROR: Could not get mod module handle for alert class");
        return false;
    }

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = FinishedAlertWndProc;
    wc.hInstance = modInstance;
    wc.lpszClassName = kFinishedAlertClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;   // painted in WM_ERASEBKGND

    if (!RegisterClassExW(&wc)) {
        Wh_Log(
            L"ERROR: Could not register completion alert class: %u",
            GetLastError()
        );

        return false;
    }

    WNDCLASSEXW card{sizeof(card)};
    card.lpfnWndProc = FinishedAlertCardProc;
    card.hInstance = modInstance;
    card.lpszClassName = kFinishedAlertCardClass;
    card.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    card.hbrBackground = nullptr;

    if (!RegisterClassExW(&card)) {
        Wh_Log(
            L"ERROR: Could not register alert card class: %u",
            GetLastError()
        );

        UnregisterClassW(kFinishedAlertClass, modInstance);
        return false;
    }

    g_modInstance = modInstance;
    g_finishedAlertClassRegistered = true;
    return true;
}


static void UnregisterFinishedAlertClass()
{
    if (!g_finishedAlertClassRegistered || !g_modInstance) {
        return;
    }

    if (!UnregisterClassW(kFinishedAlertCardClass, g_modInstance)) {
        Wh_Log(
            L"ERROR: Could not unregister alert card class: %u",
            GetLastError()
        );
    }

    if (!UnregisterClassW(kFinishedAlertClass, g_modInstance)) {
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

    if (!g_finishedAlertClassRegistered || !g_modInstance) {
        Wh_Log(L"ERROR: Completion alert class isn't registered");
        return 0;
    }

    // WIC and Media Foundation need COM on this thread.
    const bool comInitialized =
        SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    HWND taskbar = g_taskbarWnd.load();

    HMONITOR monitor =
        MonitorFromWindow(
            taskbar ? taskbar : GetDesktopWindow(),
            MONITOR_DEFAULTTOPRIMARY
        );

    MONITORINFO info{sizeof(info)};

    RECT area{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    RECT work = area;

    if (GetMonitorInfoW(monitor, &info)) {
        area = info.rcMonitor;
        work = info.rcWork;
    }

    const bool compactAlert = data->compactAlert;

    int winX = area.left;
    int winY = area.top;
    int winW = area.right - area.left;
    int winH = area.bottom - area.top;

    if (compactAlert) {
        // Compact notification in the bottom-right corner of the work area.
        const UINT sysDpi = GetDpiForSystem();

        winW = ScaleAlertValue(400, sysDpi);
        winH = ScaleAlertValue(216, sysDpi);

        const int margin = ScaleAlertValue(16, sysDpi);

        winX = work.right - winW - margin;
        winY = work.bottom - winH - margin;
    }

    HWND hWnd =
        CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            kFinishedAlertClass,
            L"Timer finished",
            WS_POPUP | WS_CLIPCHILDREN,
            winX,
            winY,
            winW,
            winH,
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

        if (comInitialized) {
            CoUninitialize();
        }

        return 0;
    }

    g_finishedAlertWnd.store(hWnd);
    g_openAlertSmall.store(data->compactAlert);
    g_openAlertTheme.store(g_alertThemePref.load());

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        winX,
        winY,
        winW,
        winH,
        SWP_SHOWWINDOW | (compactAlert ? SWP_NOACTIVATE : 0)
    );

    // The small notification must not steal focus from what you're typing.
    if (!compactAlert) {
        SetForegroundWindow(hWnd);
    }

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

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
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
        delete reinterpret_cast<FinishedAlertData*>(pending.lParam);
    }

    if (IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }

    g_finishedAlertWnd.store(nullptr);

    if (comInitialized) {
        CoUninitialize();
    }

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

    if (g_finishedAlertThread.compare_exchange_strong(thread, nullptr)) {
        CloseHandle(thread);
    }
}

static void FillAlertData(
    FinishedAlertData& data,
    const wchar_t* reminder,
    bool preview)
{
    wcsncpy_s(
        data.reminder,
        reminder && reminder[0] ? reminder : L"Timer finished",
        _TRUNCATE
    );

    ModSettings settings = GetSettingsSnapshot();
    data.defaultSnoozeSeconds = settings.defaultSnoozeMinutes * 60;
    data.maximumSeconds = settings.maximumMinutes * 60;
    data.preview = preview;
    data.compactAlert = g_alertSmallPref.load();
}

static bool ShowFinishedAlertLocked(
    const wchar_t* reminder,
    bool preview)
{
    if (g_unloading.load()) {
        return false;
    }

    if (!g_finishedAlertClassRegistered && !RegisterFinishedAlertClass()) {
        Wh_Log(L"ERROR: Completion alert class isn't available");
        return false;
    }

    CloseFinishedAlertThreadIfExited();

    if (HWND existing = g_finishedAlertWnd.load()) {
        if (IsWindow(existing) &&
            (preview ||
             g_openAlertSmall.load() != g_alertSmallPref.load() ||
             g_openAlertTheme.load() != g_alertThemePref.load()))
        {
            // Look or size changed (or a preview was requested): close the open
            // alert and build a new one so the change is really applied.
            PostMessageW(existing, WM_CLOSE, 0, 0);

            HANDLE oldThread = g_finishedAlertThread.exchange(nullptr);

            if (oldThread) {
                WaitForSingleObject(oldThread, 3000);
                CloseHandle(oldThread);
            }
        }
        else if (IsWindow(existing)) {
            auto* refresh = new (std::nothrow) FinishedAlertData{};

            if (refresh) {
                FillAlertData(*refresh, reminder, preview);

                if (!PostMessageW(
                        existing,
                        WM_APP_ALERT_REFRESH,
                        0,
                        reinterpret_cast<LPARAM>(refresh)))
                {
                    delete refresh;
                }
            }

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
        Wh_Log(L"ERROR: Completion alert stop event isn't available");
        return false;
    }

    ResetEvent(g_finishedAlertStopEvent);

    auto data = std::make_unique<FinishedAlertData>();
    FillAlertData(*data, reminder, preview);

    HANDLE thread =
        CreateThread(nullptr, 0, FinishedAlertThreadProc, data.get(), 0, nullptr);

    if (!thread) {
        Wh_Log(L"ERROR: Could not create completion alert thread");
        return false;
    }

    data.release();
    g_finishedAlertThread.store(thread);
    return true;
}

static bool ShowFinishedAlert(
    const wchar_t* reminder,
    bool preview)
{
    AcquireSRWLockExclusive(&g_finishedAlertLock);

    bool result = false;

    if (!g_unloading.load()) {
        result = ShowFinishedAlertLocked(reminder, preview);
    }

    ReleaseSRWLockExclusive(&g_finishedAlertLock);

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

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();

        if (taskbar) {
            g_taskbarWnd.store(taskbar);
        }
    }

    if (!taskbar) {
        return;
    }

    RunFromWindowThread(taskbar, TimerExpiredOnTaskbarThread, nullptr);
}


// -----------------------------------------------------------------------------
// Authoritative worker timer
// -----------------------------------------------------------------------------

static void HandleTimerExpiredFromWorker()
{
    if (g_unloading.load()) {
        return;
    }

    ULONGLONG expected = g_deadlineUtc100ns.load();

    if (!expected) {
        return;
    }

    if (GetUtcFileTimeNow() < expected) {
        if (WaitForSingleObject(g_timerStopEvent, 250) == WAIT_TIMEOUT) {
            SignalTimerWorker();
        }

        return;
    }

    g_deadlineUtc100ns.store(0);

    ClearPersistedTimer();

    PlayCompletionSound();

    DispatchTimerExpiredToTaskbar();

    wchar_t reminder[256]{};
    GetSharedReminder(reminder);

    if (!ShowFinishedAlert(reminder[0] ? reminder : L"Timer finished")) {
        Wh_Log(L"ERROR: Completion alert couldn't be shown");
    }
}


static DWORD WINAPI TimerWorkerThreadProc(LPVOID)
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
            ULONGLONG deadline = g_deadlineUtc100ns.load();

            if (!deadline) {
                CancelWaitableTimer(g_timerWaitable);
                continue;
            }

            LARGE_INTEGER due{};
            due.QuadPart = static_cast<LONGLONG>(deadline);

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

    CancelWaitableTimer(g_timerWaitable);

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

    unsigned long long parsed = wcstoull(deadlineText, &end, 10);

    if (!end ||
        end == deadlineText ||
        *end != L'\0' ||
        !parsed)
    {
        ClearPersistedTimer();
        return;
    }

    wchar_t reminder[256]{};

    Wh_GetStringValue(L"reminder", reminder, ARRAYSIZE(reminder));

    SetSharedReminder(reminder[0] ? reminder : L"Timer finished");

    ULONGLONG deadline = static_cast<ULONGLONG>(parsed);

    int remaining = RemainingSeconds(deadline);

    if (remaining > 0) {
        g_deadlineUtc100ns.store(deadline);

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

            Wh_Log(L"Ignoring stale expired timer from more than 6 hours ago");

            return;
        }

        ClearPersistedTimer();

        PlayCompletionSound();

        Wh_Log(L"Restored expired timer");

        ShowFinishedAlert(reminder[0] ? reminder : L"Timer finished");
    }
}


static void CleanupTimerObjects()
{
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

    if (g_finishedAlertStopEvent) {
        CloseHandle(g_finishedAlertStopEvent);
        g_finishedAlertStopEvent = nullptr;
    }
}


static bool StartTimerWorker()
{
    g_timerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_timerRearmEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timerWaitable = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    g_finishedAlertStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_timerStopEvent ||
        !g_timerRearmEvent ||
        !g_timerWaitable ||
        !g_finishedAlertStopEvent)
    {
        Wh_Log(L"ERROR: Could not create timer synchronization objects");

        CleanupTimerObjects();
        return false;
    }

    HANDLE thread =
        CreateThread(nullptr, 0, TimerWorkerThreadProc, nullptr, 0, nullptr);

    if (!thread) {
        Wh_Log(L"ERROR: Could not create timer worker thread");

        CleanupTimerObjects();
        return false;
    }

    g_timerWorkerThread = thread;

    return true;
}


static bool EnsureShellRuntimeStarted()
{
    AcquireSRWLockExclusive(&g_shellRuntimeLock);

    if (g_unloading.load()) {
        ReleaseSRWLockExclusive(&g_shellRuntimeLock);
        return false;
    }

    if (g_shellRuntimeStarted) {
        ReleaseSRWLockExclusive(&g_shellRuntimeLock);
        return true;
    }

    if (!g_finishedAlertClassRegistered && !RegisterFinishedAlertClass()) {
        Wh_Log(L"WARNING: Completion alert class could not be registered");
    }

    if (!StartTimerWorker()) {
        ReleaseSRWLockExclusive(&g_shellRuntimeLock);
        return false;
    }

    g_shellRuntimeStarted = true;

    RestorePersistedTimer();

    ReleaseSRWLockExclusive(&g_shellRuntimeLock);

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
        a.template as<winrt::Windows::Foundation::IUnknown>();

    auto bUnknown =
        b.template as<winrt::Windows::Foundation::IUnknown>();

    return winrt::get_abi(aUnknown) == winrt::get_abi(bUnknown);
}


static void DetachOwnedTimerVisuals()
{
    if (!g_timerButton) {
        return;
    }

    auto parentElement =
        VisualTreeHelper::GetParent(g_timerButton).try_as<FrameworkElement>();

    auto parent = parentElement.try_as<Panel>();

    if (!parent) {
        return;
    }

    auto children = parent.Children();

    uint32_t buttonIndex = 0;

    if (children.IndexOf(g_timerButton, buttonIndex)) {
        children.RemoveAt(buttonIndex);
    }

    auto parentGrid = parentElement.try_as<Grid>();

    if (!parentGrid || !g_timerColumn) {
        return;
    }

    auto columns = parentGrid.ColumnDefinitions();

    uint32_t columnIndex = 0;

    if (!columns.IndexOf(g_timerColumn, columnIndex)) {
        return;
    }

    for (uint32_t i = 0; i < children.Size(); i++) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();

        if (!child) {
            continue;
        }

        int currentColumn = Grid::GetColumn(child);

        if (currentColumn > static_cast<int>(columnIndex)) {
            Grid::SetColumn(child, currentColumn - 1);
        }
    }

    columns.RemoveAt(columnIndex);
}


static void ReleaseOwnedXamlForRebuild()
{
    HideAndReleaseFlyouts();

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
        g_countdownTimer.Tick(g_timerTickToken);
        g_countdownTimer = nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(g_timerButtonClickToken);
    }

    DetachOwnedTimerVisuals();

    g_timerButton = nullptr;
    g_timerText = nullptr;
    g_timerColumn = nullptr;
}


static void AddTimerButtonImpl(void* param)
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar = reinterpret_cast<HWND>(param);

    auto xamlRoot = GetTaskbarXamlRoot(taskbar);

    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();

    if (!content) {
        return;
    }

    auto tray =
        FindChildRecursive(
            content,
            [](FrameworkElement element)
            {
                return element.Name() == L"SystemTrayFrameGrid";
            }
        );

    if (!tray) {
        return;
    }

    if (g_timerButton) {
        auto currentParent =
            VisualTreeHelper::GetParent(g_timerButton).try_as<FrameworkElement>();

        if (currentParent && SameWinrtObject(currentParent, tray)) {
            g_taskbarWnd.store(taskbar);
            return;
        }
    }

    auto panel = tray.try_as<Panel>();

    if (!panel) {
        return;
    }

    auto children = panel.Children();

    FrameworkElement existing{nullptr};

    for (uint32_t i = 0; i < children.Size(); i++) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();

        if (child &&
            child.Name() == L"TaskbarCountdownTimerButton")
        {
            existing = child;
            break;
        }
    }

    if (existing &&
        g_timerButton &&
        SameWinrtObject(existing, g_timerButton))
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

    g_timerButton.Name(L"TaskbarCountdownTimerButton");

    g_timerButton.MinWidth(0);
    g_timerButton.MinHeight(0);
    g_timerButton.BorderThickness(Thickness{0, 0, 0, 0});
    g_timerButton.Padding(Thickness{4, 0, 4, 0});
    g_timerButton.VerticalAlignment(VerticalAlignment::Stretch);

    g_timerText.VerticalAlignment(VerticalAlignment::Center);

    g_timerButton.Content(g_timerText);

    ApplyButtonMetrics();

    RefreshTaskbarCountdown();

    g_timerButtonClickToken =
        g_timerButton.Click(
            [](auto const&, RoutedEventArgs const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                ShowTimerFlyout();
            }
        );

    g_countdownTimer = DispatcherTimer();

    g_countdownTimer.Interval(std::chrono::seconds(1));

    g_timerTickToken =
        g_countdownTimer.Tick(
            [](auto const&, auto const&)
            {
                if (g_unloading.load()) {
                    return;
                }

                int remaining = RemainingSeconds(g_deadlineUtc100ns.load());

                if (remaining > 0) {
                    if (g_timerText) {
                        g_timerText.Text(FormatCountdown(remaining));
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

    auto trayClass = winrt::get_class_name(tray);

    if (trayClass == L"Windows.UI.Xaml.Controls.StackPanel") {
        children.InsertAt(0, g_timerButton);
    }
    else {
        auto trayGrid = tray.try_as<Grid>();

        if (!trayGrid) {
            Wh_Log(
                L"ERROR: Unsupported SystemTrayFrameGrid layout class: %s",
                trayClass.c_str()
            );

            ReleaseOwnedXamlForRebuild();
            return;
        }

        auto columns = trayGrid.ColumnDefinitions();

        g_timerColumn = ColumnDefinition();

        g_timerColumn.Width(GridLengthHelper::Auto());

        columns.InsertAt(0, g_timerColumn);

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();

            if (!child) {
                continue;
            }

            Grid::SetColumn(child, Grid::GetColumn(child) + 1);
        }

        Grid::SetColumn(g_timerButton, 0);

        children.InsertAt(0, g_timerButton);
    }

    g_taskbarWnd.store(taskbar);

    int remaining = RemainingSeconds(g_deadlineUtc100ns.load());

    if (remaining > 0 && g_countdownTimer) {
        RefreshTaskbarCountdown();
        g_countdownTimer.Start();
    }
    else {
        SetIdleTaskbarText();
    }

    Wh_Log(L"Timer button added to taskbar");
}


static void AddTimerButton(void* param)
{
    try {
        AddTimerButtonImpl(param);
    }
    catch (...) {
        HRESULT error = winrt::to_hresult();

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


static void RemoveTimerButtonImpl(void* param)
{
    auto* request = reinterpret_cast<RemoveTimerButtonRequest*>(param);

    // Revoke every callback into this mod first. Nothing below this point is
    // allowed to throw while leaving a live delegate behind.
    if (g_loadedRevokers) {
        g_loadedRevokers.reset();
    }

    HideAndReleaseFlyouts();

    if (g_countdownTimer) {
        g_countdownTimer.Stop();
        g_countdownTimer.Tick(g_timerTickToken);
        g_countdownTimer = nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(g_timerButtonClickToken);
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


static void RemoveTimerButton(void* param)
{
    try {
        RemoveTimerButtonImpl(param);
    }
    catch (...) {
        HRESULT error = winrt::to_hresult();

        Wh_Log(
            L"Removing timer button failed: %08X",
            static_cast<unsigned>(error)
        );
    }
}


static bool TryRemoveTimerXaml()
{
    HWND target = g_taskbarWnd.load();

    if (!target || !IsWindow(target)) {
        target = FindCurrentProcessTaskbarWnd();
    }

    if (!target) {
        target = FindAnyWindowOnTaskbarThread();
    }

    if (!target) {
        return false;
    }

    RemoveTimerButtonRequest request{};

    if (!RunFromWindowThread(target, RemoveTimerButton, &request)) {
        return false;
    }

    return request.callbacksRevoked;
}


static void ApplyTimerButtonIfAvailable()
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();
    }

    if (!taskbar) {
        return;
    }

    g_taskbarWnd.store(taskbar);

    if (!RunFromWindowThread(taskbar, AddTimerButton, taskbar)) {
        Wh_Log(L"Could not access taskbar UI thread");
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


static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module)
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

    HGLOBAL loaded = LoadResource(module, resource);

    if (!loaded) {
        return nullptr;
    }

    void* data = LockResource(loaded);

    if (!data) {
        return nullptr;
    }

    void* fixedInfo = nullptr;
    UINT fixedInfoSize = 0;

    if (!VerQueryValueW(data, L"\\", &fixedInfo, &fixedInfoSize) ||
        !fixedInfoSize)
    {
        return nullptr;
    }

    return reinterpret_cast<VS_FIXEDFILEINFO*>(fixedInfo);
}


static HMODULE GetSystemTrayModuleHandle()
{
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }

    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        // First known Taskbar.View.dll version without SystemTray symbols:
        // 2604.8002.200.6000. Newer builds use SystemTray.dll instead.
        VS_FIXEDFILEINFO* fixedInfo = GetModuleVersionInfo(module);

        WORD moduleMajor =
            fixedInfo ? HIWORD(fixedInfo->dwFileVersionMS) : 0;

        if (moduleMajor && moduleMajor < 2604) {
            return module;
        }
    }

    return GetModuleHandleW(L"ExplorerExtensions.dll");
}


static void* WINAPI IconView_IconView_Hook(void* pThis)
{
    auto result = IconView_IconView_Original(pThis);

    if (g_unloading.load() || !g_loadedRevokers) {
        return result;
    }

    try {
        FrameworkElement iconView = nullptr;

        reinterpret_cast<IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(),
            winrt::put_abi(iconView)
        );

        if (!iconView || !g_loadedRevokers) {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto it = std::prev(g_loadedRevokers->end());

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

                if (g_unloading.load() || !g_loadedRevokers) {
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


static bool HookSystemTraySymbols(HMODULE module)
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


static void HandleLoadedModuleIfTaskbar(HMODULE module)
{
    if (g_unloading.load() || g_taskbarDllHooked.load()) {
        return;
    }

    HMODULE current = GetModuleHandleW(L"taskbar.dll");

    if (!current || current != module) {
        return;
    }

    HMODULE previous = g_taskbarDllAttempted.exchange(module);

    if (previous == module) {
        return;
    }

    if (HookTaskbarDllSymbols(module)) {
        g_taskbarDllHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(L"ERROR: Failed to hook taskbar.dll symbols");
    }
}


static void HandleLoadedModuleIfSystemTray(HMODULE module)
{
    if (g_unloading.load() || g_systemTrayModuleHooked.load()) {
        return;
    }

    if (GetSystemTrayModuleHandle() != module) {
        return;
    }

    HMODULE previous = g_systemTrayModuleAttempted.exchange(module);

    if (previous == module) {
        return;
    }

    if (HookSystemTraySymbols(module)) {
        g_systemTrayModuleHooked.store(true);
        Wh_ApplyHookOperations();
    }
    else {
        Wh_Log(L"ERROR: Failed to hook system tray symbols");
    }
}


static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR fileName,
    HANDLE file,
    DWORD flags)
{
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    if (module) {
        HandleLoadedModuleIfTaskbar(module);
        HandleLoadedModuleIfSystemTray(module);
    }

    return module;
}


// Returns WAIT_OBJECT_0 when the thread ended, WAIT_TIMEOUT when the timeout
// expired first. Keeps pumping sent messages so cross-thread calls can't deadlock.
static DWORD PumpWaitForThread(
    HANDLE thread,
    DWORD timeoutMs = INFINITE)
{
    if (!thread) {
        return WAIT_OBJECT_0;
    }

    DWORD result;
    do {
        result =
            MsgWaitForMultipleObjects(
                1,
                &thread,
                FALSE,
                timeoutMs,
                QS_SENDMESSAGE
            );

        if (result == WAIT_OBJECT_0 + 1) {
            MSG message;
            PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
        }
    } while (result == WAIT_OBJECT_0 + 1);

    return result;
}


// Waits (up to timeoutMs) for every MCI sound thread to finish. Callers must
// have raised g_unloading / stopped the sound first so the threads exit fast.
static void WaitForMciThreads(DWORD timeoutMs)
{
    ULONGLONG start = GetTickCount64();

    while (g_mciActiveThreads.load() > 0 &&
           GetTickCount64() - start < timeoutMs)
    {
        DWORD result =
            MsgWaitForMultipleObjects(
                0,
                nullptr,
                FALSE,
                20,
                QS_SENDMESSAGE
            );

        if (result == WAIT_OBJECT_0) {
            MSG message;
            PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
        }
    }

    if (g_mciActiveThreads.load() > 0) {
        Wh_Log(L"WARNING: MCI sound thread did not exit in time");
    }
}


// -----------------------------------------------------------------------------
// Windhawk
// -----------------------------------------------------------------------------

static void StopTimerWorker();


BOOL Wh_ModInit()
{
    Wh_Log(L"Taskbar Countdown Timer loading");

    g_unloading.store(false);
    g_xamlTornDown.store(false);

    LoadSettings();
    LoadSoundOverride();
    LoadBackgroundOverride();
    g_alertSmallPref.store(Wh_GetIntValue(L"alertSmall", 0) != 0);
    g_alertThemePref.store(std::clamp(Wh_GetIntValue(L"alertTheme", 0), 0, 2));
    g_flyoutGlassPref.store(Wh_GetIntValue(L"flyoutGlass", 0) != 0);

    bool taskbarHookedAtInit = false;

    if (HMODULE taskbar = GetModuleHandleW(L"taskbar.dll")) {
        g_taskbarDllAttempted.store(taskbar);

        taskbarHookedAtInit = HookTaskbarDllSymbols(taskbar);

        if (taskbarHookedAtInit) {
            g_taskbarDllHooked.store(true);
        }
        else {
            Wh_Log(L"ERROR: Failed to resolve taskbar.dll symbols");
            return FALSE;
        }
    }

    bool systemTrayHookedAtInit = false;

    if (HMODULE systemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleAttempted.store(systemTray);

        systemTrayHookedAtInit = HookSystemTraySymbols(systemTray);

        if (systemTrayHookedAtInit) {
            g_systemTrayModuleHooked.store(true);
        }
        else {
            Wh_Log(L"ERROR: Failed to hook system tray symbols");
        }
    }

    if (!taskbarHookedAtInit || !systemTrayHookedAtInit) {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");

        auto loadLibraryExW =
            kernelbase
                ? reinterpret_cast<LoadLibraryExW_t>(
                      GetProcAddress(kernelbase, "LoadLibraryExW")
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
            Wh_Log(L"ERROR: Could not resolve LoadLibraryExW");
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (HMODULE taskbar = GetModuleHandleW(L"taskbar.dll")) {
        HandleLoadedModuleIfTaskbar(taskbar);
    }

    if (HMODULE systemTray = GetSystemTrayModuleHandle()) {
        HandleLoadedModuleIfSystemTray(systemTray);
    }

    if (FindCurrentProcessTaskbarWnd()) {
        EnsureShellRuntimeStarted();
        ApplyTimerButtonIfAvailable();
    }
}

static void StopTimerWorker()
{
    if (g_timerStopEvent) {
        SetEvent(g_timerStopEvent);
    }
}


void Wh_ModBeforeUninit()
{
    AcquireSRWLockExclusive(&g_shellRuntimeLock);

    g_unloading.store(true);

    ReleaseSRWLockExclusive(&g_shellRuntimeLock);

    StopTimerWorker();

    // Primary teardown attempt while the taskbar UI thread is still available.
    bool tornDown = TryRemoveTimerXaml();

    g_xamlTornDown.store(tornDown);

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

    StopCompletionSound();

    // MCI sound threads poll g_unloading and the stop flag, so they exit
    // within ~100 ms. They must be gone before this DLL can be unloaded.
    WaitForMciThreads(3000);

    // Close the file dialog (if it is open) and wait for its thread.
    if (HANDLE picker = g_soundPickerThread.exchange(nullptr)) {
        for (int i = 0; i < 50; i++) {
            CloseSoundPickerDialog(picker);

            if (PumpWaitForThread(picker, 100) != WAIT_TIMEOUT) {
                break;
            }
        }

        CloseHandle(picker);
    }

    // First stop and join every thread that can still create/use alert UI.

    if (g_timerWorkerThread) {
        PumpWaitForThread(g_timerWorkerThread);

        CloseHandle(g_timerWorkerThread);

        g_timerWorkerThread = nullptr;
    }


    // Only after the timer worker is gone can no new alert be requested by it.
    AcquireSRWLockExclusive(&g_finishedAlertLock);

    if (g_finishedAlertStopEvent) {
        SetEvent(g_finishedAlertStopEvent);
    }

    if (HWND alert = g_finishedAlertWnd.load()) {
        PostMessageW(alert, WM_CLOSE, 0, 0);
    }

    HANDLE alertThread = g_finishedAlertThread.exchange(nullptr);

    ReleaseSRWLockExclusive(&g_finishedAlertLock);

    if (alertThread) {
        PumpWaitForThread(alertThread);

        CloseHandle(alertThread);
    }

    // The alert window's WM_CLOSE handler calls StopCompletionSound(), and a
    // late sound could in theory have been started in the meantime.
    StopCompletionSound();
    WaitForMciThreads(1000);

    // Retry whenever the first teardown didn't complete. Loaded revokers can
    // exist even when no timer button was ever created.
    if (!g_xamlTornDown.load()) {
        bool tornDown = TryRemoveTimerXaml();

        g_xamlTornDown.store(tornDown);

        if (!tornDown) {
            Wh_Log(L"ERROR: Could not complete final timer XAML teardown");
        }
    }

    CleanupTimerObjects();

    if (g_finishedAlertFont) {
        DeleteObject(g_finishedAlertFont);
        g_finishedAlertFont = nullptr;
    }

    if (g_finishedAlertTitleFont) {
        DeleteObject(g_finishedAlertTitleFont);
        g_finishedAlertTitleFont = nullptr;
    }

    if (g_finishedAlertReminderFont) {
        DeleteObject(g_finishedAlertReminderFont);
        g_finishedAlertReminderFont = nullptr;
    }

    if (g_finishedAlertSubFont) {
        DeleteObject(g_finishedAlertSubFont);
        g_finishedAlertSubFont = nullptr;
    }

    if (g_finishedAlertEditBrush) {
        DeleteObject(g_finishedAlertEditBrush);
        g_finishedAlertEditBrush = nullptr;
    }

    if (g_finishedAlertBackgroundBrush) {
        DeleteObject(g_finishedAlertBackgroundBrush);
        g_finishedAlertBackgroundBrush = nullptr;
    }

    UnregisterFinishedAlertClass();

    AcquireSRWLockExclusive(&g_shellRuntimeLock);
    g_shellRuntimeStarted = false;
    ReleaseSRWLockExclusive(&g_shellRuntimeLock);

    Wh_Log(L"Taskbar Countdown Timer unloaded");
}


static void UpdateIdleLabelOnTaskbarThread(void*)
{
    ApplyButtonMetrics();

    if (!g_deadlineUtc100ns.load()) {
        SetIdleTaskbarText();
    }
}


void Wh_ModSettingsChanged()
{
    LoadSettings();

    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();
    }

    if (taskbar) {
        RunFromWindowThread(taskbar, UpdateIdleLabelOnTaskbarThread, nullptr);
    }
}
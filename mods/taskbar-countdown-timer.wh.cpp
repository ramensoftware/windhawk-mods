// ==WindhawkMod==
// @id              taskbar-countdown-timer
// @name            Taskbar Countdown Timer
// @description     A simple countdown timer integrated into the Windows taskbar
// @version         1.3
// @author          Richi
// @github          https://github.com/richilp
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Countdown Timer

A lightweight countdown timer integrated directly into the Windows 11 taskbar.

Click **⏱ Timer** to create a countdown with a custom reminder and duration.
While the timer is running, the remaining time is shown directly on the taskbar.

## Features

- Countdown timer directly in the Windows 11 taskbar
- Custom reminder text
- Live remaining time in the taskbar
- Cancel an active timer
- Snooze a finished timer for a custom number of minutes
- Separate completion popup with sound
- Modern Windows 11-style popup interface
- Close button on timer and completion popups
- No external application required

## How to use

1. Click **⏱ Timer** on the taskbar.
2. Enter a reminder, for example `Take pizza out of the oven`.
3. Enter the duration in minutes.
4. Click **Start**.
5. When the countdown finishes, choose **Snooze** or **Dismiss**.

## Screenshot

![Taskbar Countdown Timer](https://raw.githubusercontent.com/richilp/taskbar-countdown-timer/main/16c5364b-0834-4a83-8796-1f6b1df4f702.png)

*/
// ==/WindhawkModReadme==


#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
#include <cwchar>
#include <string>
#include <dwmapi.h>
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


// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

[[clang::no_destroy]] static Button g_timerButton{nullptr};
[[clang::no_destroy]] static TextBlock g_timerText{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_countdownTimer{nullptr};

static winrt::event_token g_timerButtonClickToken{};
static winrt::event_token g_timerTickToken{};

static std::atomic<int> g_secondsRemaining{0};
static std::atomic<ULONGLONG> g_deadlineTick{0};
static wchar_t g_reminder[256]{};  // taskbar UI thread only

static std::atomic<HWND> g_timerPopup{nullptr};
static std::atomic<HWND> g_finishedPopup{nullptr};
static std::atomic<DWORD> g_timerPopupThreadId{0};
static std::atomic<HANDLE> g_timerPopupThread{nullptr};
static HANDLE g_popupThreadReadyEvent = nullptr;
static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic_bool g_buttonInjected{false};
static std::atomic_bool g_systemTrayModuleHooked{false};

[[clang::no_destroy]] static ColumnDefinition g_timerColumn{nullptr};
[[clang::no_destroy]] static std::list<FrameworkElement::Loaded_revoker>
    g_loadedRevokers;

static HINSTANCE g_modInstance = nullptr;

static std::atomic_bool g_unloading{false};
static HANDLE g_retryStopEvent = nullptr;
static HANDLE g_retryThread = nullptr;


static void ApplyTimerButtonIfAvailable();

// -----------------------------------------------------------------------------
// Modern popup styling
// -----------------------------------------------------------------------------

static HBRUSH g_popupBackgroundBrush = nullptr;
static HBRUSH g_popupEditBrush = nullptr;
static HFONT g_popupFont = nullptr;
static UINT g_popupFontDpi = 0;

static constexpr COLORREF kPopupBackground =
    RGB(32, 32, 32);

static constexpr COLORREF kPopupEditBackground =
    RGB(45, 45, 45);

static constexpr COLORREF kPopupText =
    RGB(245, 245, 245);

static constexpr COLORREF kPopupMutedText =
    RGB(190, 190, 190);


static void EnsurePopupResources(
    UINT dpi = 96)
{
    if (!g_popupBackgroundBrush) {
        g_popupBackgroundBrush =
            CreateSolidBrush(kPopupBackground);
    }

    if (!g_popupEditBrush) {
        g_popupEditBrush =
            CreateSolidBrush(kPopupEditBackground);
    }

    if (!dpi) {
        dpi = 96;
    }

    if (!g_popupFont ||
        g_popupFontDpi != dpi)
    {
        HFONT newFont =
            CreateFontW(
                -MulDiv(12, dpi, 72),
                0, 0, 0,
                FW_NORMAL,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI"
            );

        if (newFont) {
            HFONT oldFont = g_popupFont;
            g_popupFont = newFont;
            g_popupFontDpi = dpi;

            auto reapply = [](HWND wnd) {
                if (!wnd || !g_popupFont) {
                    return;
                }

                EnumChildWindows(
                    wnd,
                    [](HWND child, LPARAM) -> BOOL {
                        SendMessageW(
                            child,
                            WM_SETFONT,
                            reinterpret_cast<WPARAM>(g_popupFont),
                            TRUE
                        );
                        return TRUE;
                    },
                    0
                );
            };

            reapply(g_timerPopup.load());
            reapply(g_finishedPopup.load());

            if (oldFont) {
                DeleteObject(oldFont);
            }
        }
    }
}


static void ApplyModernWindowStyle(
    HWND hWnd)
{
    UINT dpi =
        GetDpiForWindow(hWnd);

    EnsurePopupResources(
        dpi ? dpi : 96
    );

    constexpr DWORD
        DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL = 33;

    constexpr int
        DWMWCP_ROUND_LOCAL = 2;

    int cornerPreference =
        DWMWCP_ROUND_LOCAL;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL,
        &cornerPreference,
        sizeof(cornerPreference)
    );

    constexpr DWORD
        DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL = 20;

    BOOL darkMode = TRUE;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL,
        &darkMode,
        sizeof(darkMode)
    );

    MARGINS margins{
        1,
        1,
        1,
        1
    };

    DwmExtendFrameIntoClientArea(
        hWnd,
        &margins
    );
}


static void ApplyPopupFont(
    HWND hWnd,
    UINT dpi = 0)
{
    if (!hWnd) {
        return;
    }

    if (!dpi) {
        dpi =
            GetDpiForWindow(hWnd);
    }

    EnsurePopupResources(
        dpi ? dpi : 96
    );

    SendMessageW(
        hWnd,
        WM_SETFONT,
        reinterpret_cast<WPARAM>(
            g_popupFont
        ),
        TRUE
    );
}


// -----------------------------------------------------------------------------
// Popup constants
// -----------------------------------------------------------------------------

constexpr int IDC_REMINDER_LABEL = 1000;
constexpr int IDC_REMINDER       = 1001;
constexpr int IDC_MINUTES_LABEL  = 1002;
constexpr int IDC_MINUTES        = 1003;
constexpr int IDC_START          = 1004;
constexpr int IDC_CANCEL_TIMER   = 1005;
constexpr int IDC_CLOSE_TIMER    = 1006;

constexpr int IDC_FINISHED_REMINDER = 1099;
constexpr int IDC_SNOOZE_LABEL   = 1100;
constexpr int IDC_SNOOZE_MINUTES = 1101;
constexpr int IDC_SNOOZE         = 1102;
constexpr int IDC_DISMISS        = 1103;
constexpr int IDC_CLOSE_FINISHED = 1104;
constexpr int IDC_FINISHED_TEXT   = 1105;

constexpr UINT WM_APP_TIMER_SHOW =
    WM_APP + 1;

constexpr UINT WM_APP_TIMER_FINISHED =
    WM_APP + 2;

constexpr UINT WM_APP_TIMER_SHUTDOWN =
    WM_APP + 3;


// -----------------------------------------------------------------------------
// Popup DPI/layout helpers
// -----------------------------------------------------------------------------

static int ScaleByDpi(
    int value,
    UINT dpi)
{
    return MulDiv(
        value,
        dpi ? dpi : 96,
        96
    );
}


static UINT GetPopupDpi(
    HWND owner)
{
    UINT dpi =
        owner
            ? GetDpiForWindow(owner)
            : 0;

    return dpi ? dpi : 96;
}


static void PositionPopupNearTaskbar(
    HWND hWnd,
    HWND taskbar,
    int baseWidth,
    int baseHeight,
    UINT dpi)
{
    int width =
        ScaleByDpi(
            baseWidth,
            dpi
        );

    int height =
        ScaleByDpi(
            baseHeight,
            dpi
        );

    int margin =
        ScaleByDpi(
            10,
            dpi
        );

    HMONITOR monitor =
        MonitorFromWindow(
            taskbar ? taskbar : hWnd,
            MONITOR_DEFAULTTONEAREST
        );

    MONITORINFO monitorInfo{
        sizeof(monitorInfo)
    };

    if (!GetMonitorInfoW(
            monitor,
            &monitorInfo))
    {
        SetWindowPos(
            hWnd,
            HWND_TOPMOST,
            0,
            0,
            width,
            height,
            SWP_SHOWWINDOW
        );

        return;
    }

    RECT work =
        monitorInfo.rcWork;

    RECT monitorRect =
        monitorInfo.rcMonitor;

    RECT taskbarRect{};

    bool haveTaskbar =
        taskbar &&
        GetWindowRect(
            taskbar,
            &taskbarRect
        );

    int x =
        work.right -
        width -
        margin;

    int y =
        work.bottom -
        height -
        margin;

    if (haveTaskbar) {
        const int taskbarWidth =
            taskbarRect.right -
            taskbarRect.left;

        const int taskbarHeight =
            taskbarRect.bottom -
            taskbarRect.top;

        const bool horizontal =
            taskbarWidth >= taskbarHeight;

        if (horizontal) {
            // A horizontal taskbar spans the monitor width, so its left/right
            // edges also touch the monitor. Determine top vs bottom only.
            const int topDistance =
                std::abs(
                    taskbarRect.top -
                    monitorRect.top
                );

            const int bottomDistance =
                std::abs(
                    monitorRect.bottom -
                    taskbarRect.bottom
                );

            if (topDistance < bottomDistance) {
                y =
                    work.top +
                    margin;
            }
            else {
                y =
                    work.bottom -
                    height -
                    margin;
            }

            // Keep the popup aligned to the right side, matching the timer
            // button/system tray area.
            x =
                work.right -
                width -
                margin;
        }
        else {
            // Vertical taskbar: determine left vs right only.
            const int leftDistance =
                std::abs(
                    taskbarRect.left -
                    monitorRect.left
                );

            const int rightDistance =
                std::abs(
                    monitorRect.right -
                    taskbarRect.right
                );

            if (leftDistance < rightDistance) {
                x =
                    work.left +
                    margin;
            }
            else {
                x =
                    work.right -
                    width -
                    margin;
            }

            y =
                work.bottom -
                height -
                margin;
        }
    }

    x =
        std::max<int>(
            static_cast<int>(work.left),
            std::min<int>(
                x,
                static_cast<int>(
                    work.right
                ) - width
            )
        );

    y =
        std::max<int>(
            static_cast<int>(work.top),
            std::min<int>(
                y,
                static_cast<int>(
                    work.bottom
                ) - height
            )
        );

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        x,
        y,
        width,
        height,
        SWP_SHOWWINDOW
    );
}


static void LayoutTimerPopup(
    HWND hWnd,
    UINT dpi)
{
    struct Item {
        int id;
        int x;
        int y;
        int w;
        int h;
    };

    constexpr Item items[] = {
        {IDC_CLOSE_TIMER,      338,   8,  24, 24},
        {IDC_REMINDER_LABEL,    20,  20, 300, 20},
        {IDC_REMINDER,          20,  45, 340, 30},
        {IDC_MINUTES_LABEL,     20,  90, 150, 20},
        {IDC_MINUTES,           20, 115, 110, 30},
        {IDC_START,             20, 165, 165, 34},
        {IDC_CANCEL_TIMER,     195, 165, 165, 34},
    };

    EnsurePopupResources(dpi);

    for (const auto& item : items) {
        HWND child =
            GetDlgItem(
                hWnd,
                item.id
            );

        if (!child) {
            continue;
        }

        SetWindowPos(
            child,
            nullptr,
            ScaleByDpi(item.x, dpi),
            ScaleByDpi(item.y, dpi),
            ScaleByDpi(item.w, dpi),
            ScaleByDpi(item.h, dpi),
            SWP_NOZORDER |
                SWP_NOACTIVATE
        );

        ApplyPopupFont(
            child,
            dpi
        );
    }
}


static void LayoutFinishedPopup(
    HWND hWnd,
    UINT dpi)
{
    struct Item {
        int id;
        int x;
        int y;
        int w;
        int h;
    };

    constexpr Item items[] = {
        {IDC_CLOSE_FINISHED,   338,   8,  24, 24},
        {IDC_FINISHED_TEXT,     20,  20, 340, 45},
        {IDC_SNOOZE_LABEL,      20,  82, 160, 20},
        {IDC_SNOOZE_MINUTES,   190,  78,  70, 30},
        {IDC_SNOOZE,            20, 135, 165, 34},
        {IDC_DISMISS,          195, 135, 165, 34},
    };

    EnsurePopupResources(dpi);

    for (const auto& item : items) {
        HWND child =
            GetDlgItem(
                hWnd,
                item.id
            );

        if (!child) {
            continue;
        }

        SetWindowPos(
            child,
            nullptr,
            ScaleByDpi(item.x, dpi),
            ScaleByDpi(item.y, dpi),
            ScaleByDpi(item.w, dpi),
            ScaleByDpi(item.h, dpi),
            SWP_NOZORDER |
                SWP_NOACTIVATE
        );

        ApplyPopupFont(
            child,
            dpi
        );
    }
}


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

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
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
        hooks,
        ARRAYSIZE(hooks)
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

                    param->proc(
                        param->procParam
                    );
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
        procParam
    };

    SendMessageW(
        hWnd,
        message,
        0,
        reinterpret_cast<LPARAM>(&param)
    );

    UnhookWindowsHookEx(hook);

    return true;
}


// -----------------------------------------------------------------------------
// Countdown
// -----------------------------------------------------------------------------

static std::wstring FormatCountdown(
    int totalSeconds)
{
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    wchar_t text[64]{};

    swprintf_s(
        text,
        L"⏱ %02d:%02d",
        minutes,
        seconds
    );

    return text;
}


struct StartTimerRequest
{
    int seconds;
    wchar_t reminder[256];
};


static void StartCountdownOnTaskbarThread(
    void* param)
{
    auto* request =
        reinterpret_cast<StartTimerRequest*>(
            param
        );

    if (!request ||
        !g_timerText ||
        !g_countdownTimer)
    {
        return;
    }

    g_countdownTimer.Stop();

    g_secondsRemaining.store(
        request->seconds
    );

    g_deadlineTick.store(
        GetTickCount64() +
        static_cast<ULONGLONG>(
            request->seconds
        ) * 1000ULL
    );

    wcsncpy_s(
        g_reminder,
        request->reminder,
        _TRUNCATE
    );

    g_timerText.Text(
        FormatCountdown(
            request->seconds
        )
    );

    g_countdownTimer.Start();

    Wh_Log(
        L"Timer started: '%s' - %d seconds",
        g_reminder,
        request->seconds
    );
}


static void CancelCountdownOnTaskbarThread(
    void*)
{
    if (g_countdownTimer) {
        g_countdownTimer.Stop();
    }

    g_secondsRemaining.store(0);
    g_deadlineTick.store(0);

    if (g_timerText) {
        g_timerText.Text(
            L"⏱ Timer"
        );
    }

    Wh_Log(
        L"Timer cancelled"
    );
}


// -----------------------------------------------------------------------------
// Timer popup
// -----------------------------------------------------------------------------

static void ResetPopupForNewTimer(
    HWND hWnd,
    const wchar_t* activeReminder = nullptr)
{
    SetWindowTextW(
        hWnd,
        L"Timer"
    );

    HWND reminder =
        GetDlgItem(
            hWnd,
            IDC_REMINDER
        );

    HWND minutes =
        GetDlgItem(
            hWnd,
            IDC_MINUTES
        );

    HWND start =
        GetDlgItem(
            hWnd,
            IDC_START
        );

    HWND cancel =
        GetDlgItem(
            hWnd,
            IDC_CANCEL_TIMER
        );

    const int secondsRemaining =
        g_secondsRemaining.load();

    const bool timerRunning =
        secondsRemaining > 0;

    if (timerRunning) {
        SetWindowTextW(
            reminder,
            activeReminder && activeReminder[0]
                ? activeReminder
                : L"Timer"
        );

        // The input accepts whole minutes, so show the remaining time
        // rounded up to the next minute.
        int remainingMinutes =
            (secondsRemaining + 59) / 60;

        wchar_t minutesText[32]{};

        swprintf_s(
            minutesText,
            L"%d",
            remainingMinutes
        );

        SetWindowTextW(
            minutes,
            minutesText
        );

        // While a timer is active, this popup is for viewing/cancelling it.
        EnableWindow(reminder, FALSE);
        EnableWindow(minutes, FALSE);
        EnableWindow(start, FALSE);
        EnableWindow(cancel, TRUE);
    }
    else {
        SetWindowTextW(
            reminder,
            L""
        );

        SetWindowTextW(
            minutes,
            L"20"
        );

        EnableWindow(reminder, TRUE);
        EnableWindow(minutes, TRUE);
        EnableWindow(start, TRUE);
        EnableWindow(cancel, FALSE);
    }

    SetWindowTextW(
        start,
        L"Start"
    );
}


// -----------------------------------------------------------------------------
// Finished / snooze popup
// -----------------------------------------------------------------------------

static LRESULT CALLBACK FinishedPopupWndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        RECT rect{};

        GetClientRect(
            hWnd,
            &rect
        );

        FillRect(
            reinterpret_cast<HDC>(wParam),
            &rect,
            g_popupBackgroundBrush
        );

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            kPopupText
        );

        SetBkColor(
            hdc,
            kPopupBackground
        );

        return reinterpret_cast<LRESULT>(
            g_popupBackgroundBrush
        );
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            kPopupText
        );

        SetBkColor(
            hdc,
            kPopupEditBackground
        );

        return reinterpret_cast<LRESULT>(
            g_popupEditBrush
        );
    }

    case WM_DPICHANGED:
    {
        UINT dpi =
            HIWORD(wParam);

        RECT* suggested =
            reinterpret_cast<RECT*>(
                lParam
            );

        SetWindowPos(
            hWnd,
            nullptr,
            suggested->left,
            suggested->top,
            suggested->right -
                suggested->left,
            suggested->bottom -
                suggested->top,
            SWP_NOZORDER |
                SWP_NOACTIVATE
        );

        LayoutFinishedPopup(
            hWnd,
            dpi
        );

        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SNOOZE &&
            HIWORD(wParam) == BN_CLICKED)
        {
            wchar_t minutesText[32]{};

            GetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_SNOOZE_MINUTES
                ),
                minutesText,
                ARRAYSIZE(minutesText)
            );

            int minutes =
                _wtoi(minutesText);

            if (minutes <= 0 ||
                minutes > 1440)
            {
                SetWindowTextW(
                    GetDlgItem(
                        hWnd,
                        IDC_SNOOZE_LABEL
                    ),
                    L"Snooze minutes (1-1440):"
                );

                HWND edit =
                    GetDlgItem(
                        hWnd,
                        IDC_SNOOZE_MINUTES
                    );

                SetFocus(edit);

                SendMessageW(
                    edit,
                    EM_SETSEL,
                    0,
                    -1
                );

                return 0;
            }

            SetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_SNOOZE_LABEL
                ),
                L"Snooze minutes:"
            );

            HWND taskbar =
                FindCurrentProcessTaskbarWnd();

            if (!taskbar) {
                Wh_Log(
                    L"ERROR: Taskbar not found while snoozing timer"
                );

                return 0;
            }

            StartTimerRequest request{};
            request.seconds =
                minutes * 60;

            wchar_t finishedReminder[256]{};
            GetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_FINISHED_REMINDER
                ),
                finishedReminder,
                ARRAYSIZE(finishedReminder)
            );

            wcsncpy_s(
                request.reminder,
                finishedReminder[0]
                    ? finishedReminder
                    : L"Timer finished",
                _TRUNCATE
            );

            if (!RunFromWindowThread(
                    taskbar,
                    StartCountdownOnTaskbarThread,
                    &request))
            {
                Wh_Log(
                    L"ERROR: Could not snooze timer"
                );

                return 0;
            }

            Wh_Log(
                L"Timer snoozed for %d minutes",
                minutes
            );

            DestroyWindow(hWnd);
            return 0;
        }

        if ((LOWORD(wParam) == IDC_DISMISS ||
             LOWORD(wParam) == IDC_CLOSE_FINISHED) &&
            HIWORD(wParam) == BN_CLICKED)
        {
            DestroyWindow(hWnd);
            return 0;
        }

        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        g_finishedPopup.store(nullptr);
        return 0;
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam
    );
}


static LRESULT CALLBACK TimerPopupWndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);


static constexpr wchar_t kTimerPopupClass[] =
    L"TaskbarCountdownTimerPopup";

static constexpr wchar_t kFinishedPopupClass[] =
    L"TaskbarCountdownTimerFinishedPopup";


static bool RegisterOnePopupClass(
    const wchar_t* className,
    WNDPROC wndProc)
{
    WNDCLASSEXW wc{
        sizeof(wc)
    };

    wc.lpfnWndProc =
        wndProc;

    wc.hInstance =
        g_modInstance;

    wc.lpszClassName =
        className;

    wc.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW
        );

    wc.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1
        );

    if (RegisterClassExW(&wc)) {
        return true;
    }

    if (GetLastError() !=
        ERROR_CLASS_ALREADY_EXISTS)
    {
        return false;
    }

    if (!UnregisterClassW(
            className,
            g_modInstance))
    {
        return false;
    }

    return RegisterClassExW(&wc) != 0;
}


static bool RegisterPopupClasses()
{
    if (!RegisterOnePopupClass(
            kTimerPopupClass,
            TimerPopupWndProc))
    {
        Wh_Log(
            L"ERROR: Could not register timer popup class"
        );

        return false;
    }

    if (!RegisterOnePopupClass(
            kFinishedPopupClass,
            FinishedPopupWndProc))
    {
        UnregisterClassW(
            kTimerPopupClass,
            g_modInstance
        );

        Wh_Log(
            L"ERROR: Could not register finished popup class"
        );

        return false;
    }

    return true;
}


static void UnregisterPopupClasses()
{
    UnregisterClassW(
        kFinishedPopupClass,
        g_modInstance
    );

    UnregisterClassW(
        kTimerPopupClass,
        g_modInstance
    );
}


static void ShowFinishedPopup(
    HWND owner,
    const wchar_t* reminderText)
{
    if (g_finishedPopup.load()) {
        SetForegroundWindow(
            g_finishedPopup
        );

        return;
    }

    UINT dpi =
        GetPopupDpi(owner);

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
            WS_EX_TOPMOST,
        kFinishedPopupClass,
        L"Timer finished",
        WS_POPUP |
            WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ScaleByDpi(380, dpi),
        ScaleByDpi(220, dpi),
        nullptr,
        nullptr,
        g_modInstance,
        nullptr
    );

    if (!hWnd) {
        Wh_Log(
            L"ERROR: Could not create finished popup"
        );

        return;
    }

    ApplyModernWindowStyle(
        hWnd
    );

    g_finishedPopup.store(hWnd);

    CreateWindowExW(
        0,
        L"BUTTON",
        L"×",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_FLAT,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_CLOSE_FINISHED
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        reminderText && reminderText[0]
            ? reminderText
            : L"Timer finished",
        WS_CHILD |
            WS_VISIBLE |
            SS_CENTER,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_FINISHED_TEXT
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Snooze minutes:",
        WS_CHILD |
            WS_VISIBLE,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE_LABEL
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"5",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            ES_NUMBER,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE_MINUTES
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"BUTTON",
        L"Snooze",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_PUSHBUTTON,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"BUTTON",
        L"Dismiss",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_PUSHBUTTON,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_DISMISS
        ),
        g_modInstance,
        nullptr
    );

    LayoutFinishedPopup(
        hWnd,
        dpi
    );

    PositionPopupNearTaskbar(
        hWnd,
        owner,
        380,
        220,
        dpi
    );

    MessageBeep(
        MB_ICONEXCLAMATION
    );

    ShowWindow(
        hWnd,
        SW_SHOW
    );

    UpdateWindow(hWnd);

    SetForegroundWindow(hWnd);

    SetFocus(
        GetDlgItem(
            hWnd,
            IDC_SNOOZE_MINUTES
        )
    );
}


static LRESULT CALLBACK TimerPopupWndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        RECT rect{};

        GetClientRect(
            hWnd,
            &rect
        );

        FillRect(
            reinterpret_cast<HDC>(wParam),
            &rect,
            g_popupBackgroundBrush
        );

        return 1;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            kPopupText
        );

        SetBkColor(
            hdc,
            kPopupBackground
        );

        return reinterpret_cast<LRESULT>(
            g_popupBackgroundBrush
        );
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc =
            reinterpret_cast<HDC>(wParam);

        SetTextColor(
            hdc,
            kPopupText
        );

        SetBkColor(
            hdc,
            kPopupEditBackground
        );

        return reinterpret_cast<LRESULT>(
            g_popupEditBrush
        );
    }

    case WM_DPICHANGED:
    {
        UINT dpi =
            HIWORD(wParam);

        RECT* suggested =
            reinterpret_cast<RECT*>(
                lParam
            );

        SetWindowPos(
            hWnd,
            nullptr,
            suggested->left,
            suggested->top,
            suggested->right -
                suggested->left,
            suggested->bottom -
                suggested->top,
            SWP_NOZORDER |
                SWP_NOACTIVATE
        );

        LayoutTimerPopup(
            hWnd,
            dpi
        );

        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_START &&
            HIWORD(wParam) == BN_CLICKED)
        {
            wchar_t reminder[256]{};
            wchar_t minutesText[32]{};

            GetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_REMINDER
                ),
                reminder,
                ARRAYSIZE(reminder)
            );

            GetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_MINUTES
                ),
                minutesText,
                ARRAYSIZE(minutesText)
            );

            int minutes =
                _wtoi(minutesText);

            if (minutes <= 0 ||
                minutes > 1440)
            {
                SetWindowTextW(
                    GetDlgItem(
                        hWnd,
                        IDC_MINUTES_LABEL
                    ),
                    L"Minutes (1-1440):"
                );

                HWND edit =
                    GetDlgItem(
                        hWnd,
                        IDC_MINUTES
                    );

                SetFocus(edit);

                SendMessageW(
                    edit,
                    EM_SETSEL,
                    0,
                    -1
                );

                return 0;
            }

            SetWindowTextW(
                GetDlgItem(
                    hWnd,
                    IDC_MINUTES_LABEL
                ),
                L"Minutes:"
            );

            if (!reminder[0]) {
                wcsncpy_s(
                    reminder,
                    L"Timer finished",
                    _TRUNCATE
                );
            }

            HWND taskbar =
                FindCurrentProcessTaskbarWnd();

            if (!taskbar) {
                Wh_Log(
                    L"ERROR: Taskbar not found while starting timer"
                );

                return 0;
            }

            StartTimerRequest request{};
            request.seconds =
                minutes * 60;

            wcsncpy_s(
                request.reminder,
                reminder,
                _TRUNCATE
            );

            if (!RunFromWindowThread(
                    taskbar,
                    StartCountdownOnTaskbarThread,
                    &request))
            {
                Wh_Log(
                    L"ERROR: Could not start timer on taskbar thread"
                );

                return 0;
            }

            ShowWindow(
                hWnd,
                SW_HIDE
            );

            return 0;
        }

        if (LOWORD(wParam) == IDC_CLOSE_TIMER &&
            HIWORD(wParam) == BN_CLICKED)
        {
            ShowWindow(
                hWnd,
                SW_HIDE
            );

            return 0;
        }

        if (LOWORD(wParam) == IDC_CANCEL_TIMER &&
            HIWORD(wParam) == BN_CLICKED)
        {
            HWND taskbar =
                FindCurrentProcessTaskbarWnd();

            if (!taskbar) {
                Wh_Log(
                    L"ERROR: Taskbar not found while cancelling timer"
                );

                return 0;
            }

            if (!RunFromWindowThread(
                    taskbar,
                    CancelCountdownOnTaskbarThread,
                    nullptr))
            {
                Wh_Log(
                    L"ERROR: Could not cancel timer"
                );

                return 0;
            }

            ShowWindow(
                hWnd,
                SW_HIDE
            );

            return 0;
        }

        break;

    case WM_APP_TIMER_SHOW:
    {
        std::unique_ptr<std::wstring> activeReminder(
            reinterpret_cast<std::wstring*>(lParam)
        );

        ResetPopupForNewTimer(
            hWnd,
            activeReminder ? activeReminder->c_str() : nullptr
        );

        LayoutTimerPopup(
            hWnd,
            GetDpiForWindow(hWnd)
        );

        ShowWindow(
            hWnd,
            SW_SHOWNORMAL
        );

        SetWindowPos(
            hWnd,
            HWND_TOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE |
                SWP_NOSIZE |
                SWP_SHOWWINDOW
        );

        SetForegroundWindow(hWnd);

        SetFocus(
            GetDlgItem(
                hWnd,
                IDC_REMINDER
            )
        );

        return 0;
    }

    case WM_APP_TIMER_FINISHED:
    {
        std::unique_ptr<std::wstring> finishedReminder(
            reinterpret_cast<std::wstring*>(lParam)
        );
        ShowWindow(
            hWnd,
            SW_HIDE
        );

        ShowFinishedPopup(
            g_taskbarWnd.load(),
            finishedReminder ? finishedReminder->c_str() : L"Timer finished"
        );

        return 0;
    }

    case WM_APP_TIMER_SHUTDOWN:
        DestroyWindow(hWnd);
        return 0;

    case WM_CLOSE:
        ShowWindow(
            hWnd,
            SW_HIDE
        );

        return 0;

    case WM_DESTROY:
        g_timerPopup.store(nullptr);

        PostQuitMessage(0);

        return 0;
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam
    );
}


static bool ShowTimerPopup(
    HWND owner)
{
    UINT dpi =
        GetPopupDpi(owner);

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
            WS_EX_TOPMOST,
        kTimerPopupClass,
        L"Timer",
        WS_POPUP |
            WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ScaleByDpi(380, dpi),
        ScaleByDpi(235, dpi),
        nullptr,
        nullptr,
        g_modInstance,
        nullptr
    );

    if (!hWnd) {
        Wh_Log(
            L"ERROR: Could not create timer popup"
        );

        return false;
    }

    ApplyModernWindowStyle(
        hWnd
    );

    g_timerPopup.store(hWnd);

    CreateWindowExW(
        0,
        L"BUTTON",
        L"×",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_FLAT,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_CLOSE_TIMER
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Reminder:",
        WS_CHILD |
            WS_VISIBLE,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_REMINDER_LABEL
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            ES_AUTOHSCROLL,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_REMINDER
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Minutes:",
        WS_CHILD |
            WS_VISIBLE,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_MINUTES_LABEL
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"20",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            ES_NUMBER,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_MINUTES
        ),
        g_modInstance,
        nullptr
    );

    CreateWindowExW(
        0,
        L"BUTTON",
        L"Start",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_PUSHBUTTON,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_START
        ),
        g_modInstance,
        nullptr
    );

    HWND cancelButton =
        CreateWindowExW(
            0,
            L"BUTTON",
            L"Cancel timer",
            WS_CHILD |
                WS_VISIBLE |
                WS_TABSTOP |
                BS_PUSHBUTTON,
            0, 0, 0, 0,
            hWnd,
            reinterpret_cast<HMENU>(
                IDC_CANCEL_TIMER
            ),
            g_modInstance,
            nullptr
        );

    EnableWindow(
        cancelButton,
        g_secondsRemaining.load() > 0
    );

    LayoutTimerPopup(
        hWnd,
        dpi
    );

    PositionPopupNearTaskbar(
        hWnd,
        owner,
        380,
        235,
        dpi
    );

    ShowWindow(
        hWnd,
        SW_HIDE
    );

    UpdateWindow(hWnd);

    return true;
}


static bool PostStringMessage(
    HWND hWnd,
    UINT message,
    const wchar_t* text)
{
    if (!hWnd) {
        return false;
    }

    auto payload =
        new (std::nothrow) std::wstring(
            text ? text : L""
        );

    if (!payload) {
        return false;
    }

    if (!PostMessageW(
            hWnd,
            message,
            0,
            reinterpret_cast<LPARAM>(payload)))
    {
        delete payload;
        return false;
    }

    return true;
}


static DWORD WINAPI TimerPopupThreadProc(
    LPVOID)
{
    MSG msg{};

    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE
    );

    if (g_popupThreadReadyEvent) {
        SetEvent(g_popupThreadReadyEvent);
    }

    if (g_unloading.load()) {
        return 0;
    }

    if (!RegisterPopupClasses()) {
        return 0;
    }

    if (!ShowTimerPopup(nullptr)) {
        UnregisterPopupClasses();
        return 0;
    }

    // The configuration window is created up front but remains hidden until
    // the taskbar button is clicked.
    if (HWND popup = g_timerPopup.load()) {
        ShowWindow(popup, SW_HIDE);
    }

    while (!g_unloading.load() &&
           GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        bool handled = false;

        if (HWND finished = g_finishedPopup.load()) {
            handled = IsDialogMessageW(finished, &msg) != FALSE;
        }

        if (!handled) {
            if (HWND timer = g_timerPopup.load()) {
                handled = IsDialogMessageW(timer, &msg) != FALSE;
            }
        }

        if (!handled) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (HWND finished = g_finishedPopup.load()) {
        DestroyWindow(finished);
    }

    if (HWND timer = g_timerPopup.load()) {
        DestroyWindow(timer);
    }

    g_finishedPopup.store(nullptr);
    g_timerPopup.store(nullptr);

    UnregisterPopupClasses();
    return 0;
}


static void OpenTimerPopup(
    HWND taskbar)
{
    if (g_unloading.load()) {
        return;
    }

    HWND popup = g_timerPopup.load();

    if (popup) {
        PostStringMessage(
            popup,
            WM_APP_TIMER_SHOW,
            g_secondsRemaining.load() > 0
                ? g_reminder
                : L""
        );
        return;
    }

    HANDLE thread = g_timerPopupThread.load();
    if (!thread) {
        Wh_Log(L"Popup thread isn't available yet");
        return;
    }

    // Don't block the taskbar UI thread waiting for the popup thread.
    if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"Popup thread exited unexpectedly");
        return;
    }

    Wh_Log(L"Popup thread is still starting; try again");
}


// -----------------------------------------------------------------------------
// Add/remove taskbar button
// -----------------------------------------------------------------------------

static void AddTimerButton(
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

    auto existing =
        FindChildRecursive(
            tray,
            [](FrameworkElement element)
            {
                return
                    element.Name() ==
                    L"TaskbarCountdownTimerButton";
            }
        );

    // The current XAML tree already has our button.
    if (existing) {
        g_taskbarWnd.store(taskbar);
        g_buttonInjected.store(true);
        return;
    }

    // If the taskbar XAML tree was rebuilt, release the old XAML objects on
    // the taskbar UI thread before creating replacements.
    if (g_countdownTimer) {
        g_countdownTimer.Stop();

        g_countdownTimer.Tick(
            g_timerTickToken
        );

        g_countdownTimer =
            nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(
            g_timerButtonClickToken
        );

        g_timerButton =
            nullptr;
    }

    g_timerText =
        nullptr;
    g_timerColumn = nullptr;

    g_timerButton = Button();
    g_timerText = TextBlock();

    g_timerButton.Name(
        L"TaskbarCountdownTimerButton"
    );

    int secondsRemaining =
        g_secondsRemaining.load();

    g_timerText.Text(
        secondsRemaining > 0
            ? FormatCountdown(
                  secondsRemaining
              )
            : L"⏱ Timer"
    );

    g_timerText.VerticalAlignment(
        VerticalAlignment::Center
    );

    g_timerButton.Content(
        g_timerText
    );

    g_timerButton.VerticalAlignment(
        VerticalAlignment::Stretch
    );

    g_timerButton.Padding(
        Thickness{8, 0, 8, 0}
    );

    g_timerButtonClickToken =
        g_timerButton.Click(
            [](
                winrt::Windows::Foundation::
                    IInspectable const&,
                RoutedEventArgs const&)
            {
                HWND taskbar =
                    FindCurrentProcessTaskbarWnd();

                if (!taskbar ||
                    g_unloading.load())
                {
                    return;
                }

                OpenTimerPopup(taskbar);
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
                winrt::Windows::Foundation::
                    IInspectable const&,
                winrt::Windows::Foundation::
                    IInspectable const&)
            {
                ULONGLONG deadline =
                    g_deadlineTick.load();

                if (!deadline) {
                    g_secondsRemaining.store(0);

                    if (g_countdownTimer) {
                        g_countdownTimer.Stop();
                    }

                    return;
                }

                ULONGLONG now =
                    GetTickCount64();

                if (now >= deadline) {
                    g_deadlineTick.store(0);
                    g_secondsRemaining.store(0);

                    if (g_countdownTimer) {
                        g_countdownTimer.Stop();
                    }

                    if (g_timerText) {
                        g_timerText.Text(
                            L"⏱ Timer"
                        );
                    }

                    Wh_Log(
                        L"Timer finished: '%s'",
                        g_reminder
                    );

                    if (g_timerPopup.load() &&
                        IsWindow(g_timerPopup.load()))
                    {
                        PostMessageW(
            g_timerPopup.load(),
                            WM_APP_TIMER_FINISHED,
                            0,
                            0
                        );
                    }

                    return;
                }

                int remaining =
                    static_cast<int>(
                        (deadline -
                         now +
                         999ULL) /
                        1000ULL
                    );

                g_secondsRemaining.store(
                    remaining
                );

                if (g_timerText) {
                    g_timerText.Text(
                        FormatCountdown(
                            remaining
                        )
                    );
                }
            }
        );

    auto children =
        panel.Children();

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

            g_timerButton.Click(
                g_timerButtonClickToken
            );

            g_timerButton = nullptr;
            g_timerText = nullptr;
            g_countdownTimer.Tick(
                g_timerTickToken
            );
            g_countdownTimer = nullptr;

            return;
        }

        auto columns =
            trayGrid.ColumnDefinitions();

        g_timerColumn = ColumnDefinition();

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

    // Re-arm display refresh after a taskbar XAML rebuild.
    ULONGLONG deadline =
        g_deadlineTick.load();

    if (deadline) {
        ULONGLONG now =
            GetTickCount64();

        if (now < deadline) {
            int remaining =
                static_cast<int>(
                    (deadline -
                     now +
                     999ULL) /
                    1000ULL
                );

            g_secondsRemaining.store(
                remaining
            );

            g_timerText.Text(
                FormatCountdown(
                    remaining
                )
            );

            g_countdownTimer.Start();
        }
        else {
            g_deadlineTick.store(0);
            g_secondsRemaining.store(0);

            if (g_timerPopup.load() &&
                IsWindow(g_timerPopup.load()))
            {
                PostStringMessage(
                    g_timerPopup.load(),
                    WM_APP_TIMER_FINISHED,
                    g_reminder
                );
            }
        }
    }

    g_taskbarWnd.store(taskbar);
    g_buttonInjected.store(true);

    Wh_Log(
        L"Timer button added to taskbar"
    );
}

static void RemoveTimerButton(
    void*)
{
    if (g_countdownTimer) {
        g_countdownTimer.Stop();

        g_countdownTimer.Tick(
            g_timerTickToken
        );

        g_countdownTimer =
            nullptr;
    }

    if (g_timerButton) {
        g_timerButton.Click(
            g_timerButtonClickToken
        );

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

            if (parentGrid && g_timerColumn) {
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
                            static_cast<int>(columnIndex))
                        {
                            Grid::SetColumn(
                                child,
                                currentColumn - 1
                            );
                        }
                    }

                    columns.RemoveAt(columnIndex);
                }
            }
        }
    }

    g_loadedRevokers.clear();
    g_timerColumn = nullptr;
    g_timerText = nullptr;
    g_timerButton = nullptr;
    g_buttonInjected.store(false);
}


static void ApplyTimerButtonIfAvailable()
{
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar =
        FindCurrentProcessTaskbarWnd();

    if (!taskbar) {
        return;
    }

    g_taskbarWnd.store(taskbar);

    if (!RunFromWindowThread(
            taskbar,
            AddTimerButton,
            taskbar))
    {
        Wh_Log(L"Could not access taskbar UI thread");
    }
}


static DWORD WINAPI RetryThreadProc(
    LPVOID)
{
    for (int i = 0;
         i < 5 && !g_unloading.load();
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

        Wh_Log(L"Taskbar injection retry %d", i + 1);
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
        return module;
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

    if (g_unloading.load()) {
        return result;
    }

    FrameworkElement iconView = nullptr;

    reinterpret_cast<IUnknown**>(pThis)[1]
        ->QueryInterface(
            winrt::guid_of<FrameworkElement>(),
            winrt::put_abi(iconView)
        );

    if (!iconView) {
        return result;
    }

    g_loadedRevokers.emplace_back();
    auto it = std::prev(g_loadedRevokers.end());

    *it = iconView.Loaded(
        winrt::auto_revoke_t{},
        [it](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&)
        {
            g_loadedRevokers.erase(it);

            if (g_unloading.load()) {
                return;
            }

            // A new IconView means the tray may have rebuilt its XAML tree.
            g_buttonInjected.store(false);
            ApplyTimerButtonIfAvailable();
        }
    );

    return result;
}


static bool HookSystemTraySymbols(
    HMODULE module)
{
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {
            LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"
        },
        &IconView_IconView_Original,
        IconView_IconView_Hook,
    }};

    return WindhawkUtils::HookSymbols(
        module,
        hooks,
        ARRAYSIZE(hooks)
    );
}


static void HandleLoadedModuleIfSystemTray(
    HMODULE module)
{
    if (g_systemTrayModuleHooked.load() ||
        GetSystemTrayModuleHandle() != module)
    {
        return;
    }

    if (HookSystemTraySymbols(module)) {
        g_systemTrayModuleHooked.store(true);
        Wh_ApplyHookOperations();
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

    HMODULE module = nullptr;

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(
                &g_modInstance
            ),
            &module))
    {
        Wh_Log(
            L"ERROR: Could not resolve mod module handle"
        );

        return FALSE;
    }

    g_modInstance =
        reinterpret_cast<HINSTANCE>(
            module
        );

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(
            L"ERROR: Failed to resolve taskbar.dll symbols"
        );

        return FALSE;
    }

    if (HMODULE systemTray = GetSystemTrayModuleHandle()) {
        if (HookSystemTraySymbols(systemTray)) {
            g_systemTrayModuleHooked.store(true);
        }
    }
    else {
        HMODULE kernelbase =
            GetModuleHandleW(L"kernelbase.dll");

        auto loadLibraryExW = kernelbase
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
    }

    return TRUE;
}


void Wh_ModAfterInit()
{
    ApplyTimerButtonIfAvailable();

    g_retryStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (g_retryStopEvent) {
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

    g_popupThreadReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (g_popupThreadReadyEvent) {
        DWORD threadId = 0;
        HANDLE thread =
            CreateThread(
                nullptr,
                0,
                TimerPopupThreadProc,
                nullptr,
                0,
                &threadId
            );

        if (thread) {
            g_timerPopupThread.store(thread);
            g_timerPopupThreadId.store(threadId);

            // Bounded wait: initialization must never block indefinitely.
            WaitForSingleObject(
                g_popupThreadReadyEvent,
                2000
            );
        }
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
}


void Wh_ModUninit()
{
    g_unloading.store(true);

    if (g_retryStopEvent) {
        SetEvent(g_retryStopEvent);
    }

    if (g_retryThread) {
        PumpWaitForThread(g_retryThread);
        CloseHandle(g_retryThread);
        g_retryThread = nullptr;
    }

    if (g_retryStopEvent) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }

    bool removed = false;

    for (int attempt = 0;
         attempt < 5 && !removed;
         attempt++)
    {
        HWND taskbar = g_taskbarWnd.load();
        if (!taskbar || !IsWindow(taskbar)) {
            taskbar = FindCurrentProcessTaskbarWnd();
        }

        if (taskbar) {
            removed = RunFromWindowThread(
                taskbar,
                RemoveTimerButton,
                nullptr
            );
        }

        if (!removed) {
            Sleep(50);
        }
    }

    if (!removed && g_buttonInjected.load()) {
        Wh_Log(
            L"ERROR: Could not remove timer XAML objects before unload"
        );
    }

    DWORD popupThreadId =
        g_timerPopupThreadId.load();

    if (popupThreadId) {
        // The queue-ready event is created once and never recycled.
        if (g_popupThreadReadyEvent) {
            WaitForSingleObject(
                g_popupThreadReadyEvent,
                2000
            );
        }

        PostThreadMessageW(
            popupThreadId,
            WM_QUIT,
            0,
            0
        );
    }

    HANDLE popupThread =
        g_timerPopupThread.load();

    if (popupThread) {
        PumpWaitForThread(popupThread);
        CloseHandle(popupThread);
        g_timerPopupThread.store(nullptr);
    }

    if (g_popupThreadReadyEvent) {
        CloseHandle(g_popupThreadReadyEvent);
        g_popupThreadReadyEvent = nullptr;
    }

    g_timerPopupThreadId.store(0);
    g_finishedPopup.store(nullptr);
    g_timerPopup.store(nullptr);

    // Popup thread is gone, so no controls can still reference these objects.
    if (g_popupFont) {
        DeleteObject(g_popupFont);
        g_popupFont = nullptr;
    }

    if (g_popupEditBrush) {
        DeleteObject(g_popupEditBrush);
        g_popupEditBrush = nullptr;
    }

    if (g_popupBackgroundBrush) {
        DeleteObject(g_popupBackgroundBrush);
        g_popupBackgroundBrush = nullptr;
    }

    g_popupFontDpi = 0;

    Wh_Log(
        L"Taskbar Countdown Timer unloaded"
    );
}

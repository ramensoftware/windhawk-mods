// ==WindhawkMod==
// @id              taskbar-countdown-timer
// @name            Taskbar Countdown Timer
// @description     A simple countdown timer integrated into the Windows taskbar
// @version         1.1
// @author          Richi
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32
// @license         MIT
// @github          richilp
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

#include <chrono>
#include <functional>
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

static Button g_timerButton{nullptr};
static TextBlock g_timerText{nullptr};
static DispatcherTimer g_countdownTimer{nullptr};

static winrt::event_token g_timerButtonClickToken{};
static winrt::event_token g_timerTickToken{};

static int g_secondsRemaining = 0;
static wchar_t g_reminder[256]{};

static HWND g_timerPopup = nullptr;
static HWND g_finishedPopup = nullptr;
static DWORD g_timerPopupThreadId = 0;
static HANDLE g_timerPopupThread = nullptr;


// -----------------------------------------------------------------------------
// Modern popup styling
// -----------------------------------------------------------------------------

static HBRUSH g_popupBackgroundBrush = nullptr;
static HBRUSH g_popupEditBrush = nullptr;
static HFONT g_popupFont = nullptr;

static constexpr COLORREF kPopupBackground =
    RGB(32, 32, 32);

static constexpr COLORREF kPopupEditBackground =
    RGB(45, 45, 45);

static constexpr COLORREF kPopupText =
    RGB(245, 245, 245);

static constexpr COLORREF kPopupMutedText =
    RGB(190, 190, 190);


static void EnsurePopupResources()
{
    if (!g_popupBackgroundBrush) {
        g_popupBackgroundBrush =
            CreateSolidBrush(
                kPopupBackground
            );
    }

    if (!g_popupEditBrush) {
        g_popupEditBrush =
            CreateSolidBrush(
                kPopupEditBackground
            );
    }

    if (!g_popupFont) {
        g_popupFont =
            CreateFontW(
                -16,
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
                DEFAULT_PITCH |
                    FF_DONTCARE,
                L"Segoe UI"
            );
    }
}


static void ApplyModernWindowStyle(
    HWND hWnd)
{
    EnsurePopupResources();

    // Rounded Windows 11 corners.
    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL = 33;
    constexpr int DWMWCP_ROUND_LOCAL = 2;

    int cornerPreference =
        DWMWCP_ROUND_LOCAL;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_WINDOW_CORNER_PREFERENCE_LOCAL,
        &cornerPreference,
        sizeof(cornerPreference)
    );

    // Dark title / non-client rendering where supported.
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL = 20;

    BOOL darkMode = TRUE;

    DwmSetWindowAttribute(
        hWnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE_LOCAL,
        &darkMode,
        sizeof(darkMode)
    );

    // Add a subtle system shadow without a classic caption.
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
    HWND hWnd)
{
    if (!hWnd) {
        return;
    }

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

constexpr int IDC_SNOOZE_LABEL   = 1100;
constexpr int IDC_SNOOZE_MINUTES = 1101;
constexpr int IDC_SNOOZE         = 1102;
constexpr int IDC_DISMISS        = 1103;
constexpr int IDC_CLOSE_FINISHED = 1104;

constexpr UINT WM_APP_TIMER_SHOW =
    WM_APP + 1;

constexpr UINT WM_APP_TIMER_FINISHED =
    WM_APP + 2;

constexpr UINT WM_APP_TIMER_SHUTDOWN =
    WM_APP + 3;


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

    size_t offset = 0x10;

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
            L"ERROR: Unsupported TaskbarHost::FrameHeight"
        );

        std__Ref_count_base__Decref_Original(
            taskbarHostSharedPtr[1]
        );

        return nullptr;
    }

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

                const UINT message =
                    RegisterWindowMessageW(
                        L"Windhawk_RunFromWindowThread_"
                        WH_MOD_ID
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

    g_secondsRemaining =
        request->seconds;

    wcsncpy_s(
        g_reminder,
        request->reminder,
        _TRUNCATE
    );

    g_timerText.Text(
        FormatCountdown(
            g_secondsRemaining
        )
    );

    g_countdownTimer.Start();

    Wh_Log(
        L"Timer started: '%s' - %d seconds",
        g_reminder,
        g_secondsRemaining
    );
}


static void CancelCountdownOnTaskbarThread(
    void*)
{
    if (g_countdownTimer) {
        g_countdownTimer.Stop();
    }

    g_secondsRemaining = 0;

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
    HWND hWnd)
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

    const bool timerRunning =
        g_secondsRemaining > 0;

    if (timerRunning) {
        SetWindowTextW(
            reminder,
            g_reminder[0]
                ? g_reminder
                : L"Timer"
        );

        // The input accepts whole minutes, so show the remaining time
        // rounded up to the next minute.
        int remainingMinutes =
            (g_secondsRemaining + 59) / 60;

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
                MessageBoxW(
                    hWnd,
                    L"Enter a number of minutes between 1 and 1440.",
                    L"Snooze timer",
                    MB_OK |
                        MB_ICONWARNING
                );

                return 0;
            }

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

            wcsncpy_s(
                request.reminder,
                g_reminder[0]
                    ? g_reminder
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

        if (LOWORD(wParam) == IDC_DISMISS &&
            HIWORD(wParam) == BN_CLICKED)
        {
            DestroyWindow(hWnd);
            return 0;
        }

        if (LOWORD(wParam) == IDC_CLOSE_FINISHED &&
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
        g_finishedPopup = nullptr;
        return 0;
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam
    );
}


static void ShowFinishedPopup(
    HWND owner)
{
    static bool classRegistered = false;

    if (g_finishedPopup) {
        SetForegroundWindow(
            g_finishedPopup
        );

        return;
    }

    if (!classRegistered) {
        WNDCLASSW wc{};

        wc.lpfnWndProc =
            FinishedPopupWndProc;

        wc.hInstance =
            GetModuleHandleW(nullptr);

        wc.lpszClassName =
            L"TaskbarCountdownTimerFinishedPopup";

        wc.hCursor =
            LoadCursorW(
                nullptr,
                IDC_ARROW
            );

        wc.hbrBackground =
            reinterpret_cast<HBRUSH>(
                COLOR_WINDOW + 1
            );

        if (!RegisterClassW(&wc) &&
            GetLastError() !=
                ERROR_CLASS_ALREADY_EXISTS)
        {
            Wh_Log(
                L"ERROR: Could not register finished popup class"
            );

            return;
        }

        classRegistered = true;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
            WS_EX_TOPMOST,
        L"TaskbarCountdownTimerFinishedPopup",
        L"Timer finished",
        WS_POPUP |
            WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        380,
        220,
        owner,
        nullptr,
        GetModuleHandleW(nullptr),
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

    g_finishedPopup = hWnd;

    CreateWindowExW(
        0,
        L"BUTTON",
        L"×",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_FLAT,
        338,
        8,
        24,
        24,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_CLOSE_FINISHED
        ),
        GetModuleHandleW(nullptr),
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        g_reminder[0]
            ? g_reminder
            : L"Timer finished",
        WS_CHILD |
            WS_VISIBLE |
            SS_CENTER,
        20,
        20,
        340,
        45,
        hWnd,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Snooze minutes:",
        WS_CHILD |
            WS_VISIBLE,
        20,
        82,
        160,
        20,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE_LABEL
        ),
        GetModuleHandleW(nullptr),
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
        190,
        78,
        70,
        30,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE_MINUTES
        ),
        GetModuleHandleW(nullptr),
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
        20,
        135,
        165,
        34,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_SNOOZE
        ),
        GetModuleHandleW(nullptr),
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
        195,
        135,
        165,
        34,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_DISMISS
        ),
        GetModuleHandleW(nullptr),
        nullptr
    );

    for (int id : {
             IDC_SNOOZE_LABEL,
             IDC_SNOOZE_MINUTES,
             IDC_SNOOZE,
             IDC_DISMISS,
             IDC_CLOSE_FINISHED})
    {
        ApplyPopupFont(
            GetDlgItem(
                hWnd,
                id
            )
        );
    }

    RECT taskbarRect{};
    GetWindowRect(
        owner,
        &taskbarRect
    );

    constexpr int width = 380;
    constexpr int height = 220;

    int x =
        taskbarRect.right -
        width -
        20;

    int y =
        taskbarRect.top -
        height -
        10;

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        x,
        y,
        width,
        height,
        SWP_SHOWWINDOW
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
                MessageBoxW(
                    hWnd,
                    L"Enter a number of minutes between 1 and 1440.",
                    L"Timer",
                    MB_OK |
                        MB_ICONWARNING
                );

                return 0;
            }

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
        ResetPopupForNewTimer(
            hWnd
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


    case WM_APP_TIMER_FINISHED:
        ShowWindow(
            hWnd,
            SW_HIDE
        );

        ShowFinishedPopup(
            FindCurrentProcessTaskbarWnd()
        );

        return 0;


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
        g_timerPopup = nullptr;
        g_timerPopupThreadId = 0;

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
    static bool classRegistered = false;

    if (!classRegistered) {
        WNDCLASSW wc{};

        wc.lpfnWndProc =
            TimerPopupWndProc;

        wc.hInstance =
            GetModuleHandleW(nullptr);

        wc.lpszClassName =
            L"TaskbarCountdownTimerPopup";

        wc.hCursor =
            LoadCursorW(
                nullptr,
                IDC_ARROW
            );

        wc.hbrBackground =
            reinterpret_cast<HBRUSH>(
                COLOR_WINDOW + 1
            );

        if (!RegisterClassW(&wc) &&
            GetLastError() !=
                ERROR_CLASS_ALREADY_EXISTS)
        {
            Wh_Log(
                L"ERROR: Could not register popup class"
            );

            return false;
        }

        classRegistered = true;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
            WS_EX_TOPMOST,
        L"TaskbarCountdownTimerPopup",
        L"Timer",
        WS_POPUP |
            WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        380,
        235,
        owner,
        nullptr,
        GetModuleHandleW(nullptr),
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

    g_timerPopup = hWnd;
    g_timerPopupThreadId =
        GetCurrentThreadId();

    CreateWindowExW(
        0,
        L"BUTTON",
        L"×",
        WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP |
            BS_FLAT,
        338,
        8,
        24,
        24,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_CLOSE_TIMER
        ),
        GetModuleHandleW(nullptr),
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Reminder:",
        WS_CHILD |
            WS_VISIBLE,
        20,
        20,
        300,
        20,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_REMINDER_LABEL
        ),
        GetModuleHandleW(nullptr),
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
        20,
        45,
        340,
        30,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_REMINDER
        ),
        GetModuleHandleW(nullptr),
        nullptr
    );

    CreateWindowExW(
        0,
        L"STATIC",
        L"Minutes:",
        WS_CHILD |
            WS_VISIBLE,
        20,
        90,
        150,
        20,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_MINUTES_LABEL
        ),
        GetModuleHandleW(nullptr),
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
        20,
        115,
        110,
        30,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_MINUTES
        ),
        GetModuleHandleW(nullptr),
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
        20,
        165,
        150,
        35,
        hWnd,
        reinterpret_cast<HMENU>(
            IDC_START
        ),
        GetModuleHandleW(nullptr),
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
            195,
            165,
            165,
            34,
            hWnd,
            reinterpret_cast<HMENU>(
                IDC_CANCEL_TIMER
            ),
            GetModuleHandleW(nullptr),
            nullptr
        );

    EnableWindow(
        cancelButton,
        g_secondsRemaining > 0
    );

    for (int id : {
             IDC_REMINDER_LABEL,
             IDC_REMINDER,
             IDC_MINUTES_LABEL,
             IDC_MINUTES,
             IDC_START,
             IDC_CANCEL_TIMER,
             IDC_CLOSE_TIMER})
    {
        ApplyPopupFont(
            GetDlgItem(
                hWnd,
                id
            )
        );
    }

    RECT taskbarRect{};

    GetWindowRect(
        owner,
        &taskbarRect
    );

    constexpr int width = 380;
    constexpr int height = 235;

    int x =
        taskbarRect.right -
        width -
        20;

    int y =
        taskbarRect.top -
        height -
        10;

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        x,
        y,
        width,
        height,
        SWP_SHOWWINDOW
    );

    ShowWindow(
        hWnd,
        SW_SHOW
    );

    UpdateWindow(hWnd);

    return true;
}


static DWORD WINAPI TimerPopupThreadProc(
    LPVOID param)
{
    HWND taskbar =
        reinterpret_cast<HWND>(param);

    if (!ShowTimerPopup(taskbar)) {
        return 0;
    }

    MSG msg{};

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}


static void OpenTimerPopup(
    HWND taskbar)
{
    if (g_timerPopup) {
        PostMessageW(
            g_timerPopup,
            WM_APP_TIMER_SHOW,
            0,
            0
        );

        return;
    }

    if (g_timerPopupThread) {
        if (WaitForSingleObject(
                g_timerPopupThread,
                0) ==
            WAIT_OBJECT_0)
        {
            CloseHandle(
                g_timerPopupThread
            );

            g_timerPopupThread =
                nullptr;
        }
        else {
            // The thread is still starting.
            return;
        }
    }

    g_timerPopupThread =
        CreateThread(
            nullptr,
            0,
            TimerPopupThreadProc,
            taskbar,
            0,
            nullptr
        );

    if (!g_timerPopupThread) {
        Wh_Log(
            L"ERROR: Could not create popup thread"
        );
    }
}


// -----------------------------------------------------------------------------
// Add/remove taskbar button
// -----------------------------------------------------------------------------

static void AddTimerButton(
    void* param)
{
    HWND taskbar =
        reinterpret_cast<HWND>(param);

    auto xamlRoot =
        GetTaskbarXamlRoot(taskbar);

    if (!xamlRoot) {
        Wh_Log(
            L"ERROR: Taskbar XamlRoot not obtained"
        );

        return;
    }

    auto content =
        xamlRoot.Content()
            .try_as<FrameworkElement>();

    if (!content) {
        Wh_Log(
            L"ERROR: XamlRoot content unavailable"
        );

        return;
    }

    // SystemTrayFrameGrid is the live layout container for the notification
    // area. On recent Windows 11 builds it can be a StackPanel instead of a
    // Grid, while keeping the same element name.
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
        Wh_Log(
            L"ERROR: SystemTrayFrameGrid not found"
        );

        return;
    }

    auto panel =
        tray.try_as<Panel>();

    if (!panel) {
        Wh_Log(
            L"ERROR: SystemTrayFrameGrid is not a Panel"
        );

        return;
    }

    // Remove a leftover instance if this mod was reloaded.
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

    if (existing) {
        auto parent =
            VisualTreeHelper::GetParent(
                existing
            ).try_as<Panel>();

        if (parent) {
            auto children =
                parent.Children();

            uint32_t index = 0;

            if (children.IndexOf(
                    existing,
                    index))
            {
                children.RemoveAt(index);
            }
        }
    }

    g_timerButton = Button();
    g_timerText = TextBlock();

    g_timerButton.Name(
        L"TaskbarCountdownTimerButton"
    );

    g_timerText.Text(
        L"⏱ Timer"
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

                if (!taskbar) {
                    Wh_Log(
                        L"ERROR: Taskbar not found for popup"
                    );

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
                if (g_secondsRemaining <= 0) {
                    g_countdownTimer.Stop();
                    return;
                }

                g_secondsRemaining--;

                if (g_secondsRemaining > 0) {
                    if (g_timerText) {
                        g_timerText.Text(
                            FormatCountdown(
                                g_secondsRemaining
                            )
                        );
                    }

                    return;
                }

                g_countdownTimer.Stop();

                if (g_timerText) {
                    g_timerText.Text(
                        L"⏱ Timer"
                    );
                }

                Wh_Log(
                    L"Timer finished: '%s'",
                    g_reminder
                );

                if (g_timerPopup) {
                    PostMessageW(
                        g_timerPopup,
                        WM_APP_TIMER_FINISHED,
                        0,
                        0
                    );
                }
            }
        );

    auto children =
        panel.Children();

    auto trayClass =
        winrt::get_class_name(tray);

    Wh_Log(
        L"SystemTrayFrameGrid class: %s",
        trayClass.c_str()
    );

    // "Before notification icons": on current Windows 11 builds the tray
    // container is a StackPanel, so inserting at index 0 gives the timer its
    // own layout slot instead of overlaying the hidden-icons button.
    if (trayClass ==
        L"Windows.UI.Xaml.Controls.StackPanel")
    {
        children.InsertAt(
            0,
            g_timerButton
        );

        Wh_Log(
            L"Timer button inserted at start of tray StackPanel"
        );

        return;
    }

    // Fallback for builds where SystemTrayFrameGrid is still a Grid.
    // Put the button in a new Auto column at the start and shift the existing
    // children one column to the right.
    auto trayGrid =
        tray.try_as<Grid>();

    if (trayGrid) {
        auto columns =
            trayGrid.ColumnDefinitions();

        ColumnDefinition timerColumn;
        timerColumn.Width(
            GridLengthHelper::Auto()
        );

        columns.InsertAt(
            0,
            timerColumn
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

            int currentColumn =
                Grid::GetColumn(child);

            Grid::SetColumn(
                child,
                currentColumn + 1
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

        Wh_Log(
            L"Timer button inserted in new leading tray Grid column"
        );

        return;
    }

    Wh_Log(
        L"ERROR: Unsupported SystemTrayFrameGrid layout class: %s",
        trayClass.c_str()
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

    g_secondsRemaining = 0;

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

            // If the tray container is a Grid, v0.7 added one leading Auto
            // column. Restore the original column indexes and remove it.
            auto parentGrid =
                parentElement.try_as<Grid>();

            if (parentGrid) {
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

                    if (currentColumn > 0) {
                        Grid::SetColumn(
                            child,
                            currentColumn - 1
                        );
                    }
                }

                auto columns =
                    parentGrid.ColumnDefinitions();

                if (columns.Size() > 0) {
                    columns.RemoveAt(0);
                }
            }
        }
    }

    g_timerText = nullptr;
    g_timerButton = nullptr;
}


// -----------------------------------------------------------------------------
// Windhawk
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(
        L"Taskbar Countdown Timer loading"
    );

    HWND taskbar =
        FindCurrentProcessTaskbarWnd();

    if (!taskbar) {
        Wh_Log(
            L"ERROR: Taskbar not found"
        );

        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(
            L"ERROR: Failed to resolve taskbar.dll symbols"
        );

        return FALSE;
    }

    if (!RunFromWindowThread(
            taskbar,
            AddTimerButton,
            taskbar))
    {
        Wh_Log(
            L"ERROR: Could not access taskbar XAML thread"
        );

        return FALSE;
    }

    Wh_Log(
        L"Taskbar Countdown Timer loaded"
    );

    return TRUE;
}


void Wh_ModUninit()
{
    HWND taskbar =
        FindCurrentProcessTaskbarWnd();

    if (taskbar) {
        RunFromWindowThread(
            taskbar,
            RemoveTimerButton,
            nullptr
        );
    }

    if (g_finishedPopup) {
        PostMessageW(
            g_finishedPopup,
            WM_CLOSE,
            0,
            0
        );
    }

    if (g_timerPopup) {
        PostMessageW(
            g_timerPopup,
            WM_APP_TIMER_SHUTDOWN,
            0,
            0
        );
    }

    if (g_timerPopupThread) {
        WaitForSingleObject(
            g_timerPopupThread,
            2000
        );

        CloseHandle(
            g_timerPopupThread
        );

        g_timerPopupThread =
            nullptr;
    }

    g_finishedPopup = nullptr;
    g_timerPopup = nullptr;
    g_timerPopupThreadId = 0;

    if (g_popupFont) {
        DeleteObject(
            g_popupFont
        );

        g_popupFont = nullptr;
    }

    if (g_popupEditBrush) {
        DeleteObject(
            g_popupEditBrush
        );

        g_popupEditBrush = nullptr;
    }

    if (g_popupBackgroundBrush) {
        DeleteObject(
            g_popupBackgroundBrush
        );

        g_popupBackgroundBrush = nullptr;
    }

    Wh_Log(
        L"Taskbar Countdown Timer unloaded"
    );
}

// ==WindhawkMod==
// @id              systemtray-only-show
// @name            System Tray Only
// @name:ja-JP      システムトレイのみ表示
// @description     Hides everything except the system tray, turning the taskbar into a small floating tray in the bottom-right corner (Windows 11 25H2)
// @description:ja-JP システムトレイ以外を非表示にし、タスクバーを右下角の小さなフローティングトレイに変換します（Windows 11 25H2）
// @version         0.1
// @author          User
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Tray Only

Hides all taskbar elements except the system tray (notification area) and
displays it as a small floating window in the bottom-right corner of the screen.

Designed to work alongside ObjectDock 3 with its "Hide Taskbar" feature enabled.
The mod intercepts the taskbar hide request, removes the AppBar space
reservation, and keeps only the system tray visible.

Only Windows 11 version 25H2 (build 26200) is supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoHide: true
  $name: Auto-hide
  $name:ja-JP: オートハイド
  $description: Hide the tray when the mouse leaves, show on hover at the bottom-right corner
  $description:ja-JP: マウスが離れるとトレイを隠し、右下角でホバーすると表示します
- autoHideDelayMs: 1000
  $name: Auto-hide delay (ms)
  $name:ja-JP: オートハイド遅延（ミリ秒）
  $description: Milliseconds to wait before hiding the tray after the mouse leaves
  $description:ja-JP: マウスが離れてからトレイを隠すまでの待機時間
- showClock: true
  $name: Show clock
  $name:ja-JP: 時計を表示
- showShowDesktop: false
  $name: Show "Show Desktop" button
  $name:ja-JP: 「デスクトップの表示」ボタンを表示
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <windowsx.h>
#include <winrt/base.h>

#include <atomic>
#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    bool autoHide;
    int autoHideDelayMs;
    bool showClock;
    bool showShowDesktop;
} g_settings;

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;
std::atomic<bool> g_trayOnlyMode;

HWND g_hTaskbarWnd;
RECT g_originalTaskbarRect;
LONG g_originalExStyle;

bool g_trayVisible = true;
DWORD g_lastMouseInTrayTick;

constexpr UINT_PTR kAutoHideTimerId = 0xF001;
constexpr UINT kAutoHidePollMs = 50;
constexpr int kHotZoneHeight = 2;
constexpr int kTrayDefaultHeight = 48;
constexpr int kTrayMinWidth = 200;

// ---------------------------------------------------------------------------
// XAML helpers (same patterns as existing Windhawk taskbar mods)
// ---------------------------------------------------------------------------

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                     PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        auto result = FindDescendantByName(child, name);
        if (result) {
            return result;
        }
    }

    return nullptr;
}

// ---------------------------------------------------------------------------
// XamlRoot acquisition (CTaskBand -> TaskbarHost -> XamlRoot)
// ---------------------------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                     taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// ---------------------------------------------------------------------------
// Find taskbar window
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// ---------------------------------------------------------------------------
// RunFromWindowThread (post work to the taskbar UI thread)
// ---------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message ==
                    RegisterWindowMessage(
                        L"Windhawk_RunFromWindowThread_" WH_MOD_ID)) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// ---------------------------------------------------------------------------
// XAML: Apply / restore tray-only style
// ---------------------------------------------------------------------------

int GetTrayWidthFromXaml(XamlRoot xamlRoot) {
    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return kTrayMinWidth;
    }

    FrameworkElement child = xamlRootContent;
    FrameworkElement systemTrayGrid = nullptr;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (systemTrayGrid = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        double w = systemTrayGrid.ActualWidth();
        if (w > 0) {
            return static_cast<int>(w + 0.5);
        }
    }

    return kTrayMinWidth;
}

bool ApplyTrayOnlyStyle(XamlRoot xamlRoot) {
    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return false;
    }

    FrameworkElement rootGrid = nullptr;
    FrameworkElement taskbarFrameRepeater = nullptr;
    FrameworkElement taskbarBackground = nullptr;
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRootContent;
    FrameworkElement taskbarFrame = nullptr;
    if (child &&
        (taskbarFrame =
             FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (rootGrid = FindChildByName(taskbarFrame, L"RootGrid"))) {
        taskbarFrameRepeater =
            FindChildByName(rootGrid, L"TaskbarFrameRepeater");
        taskbarBackground = FindChildByClassName(
            rootGrid, L"Taskbar.TaskbarBackground");
        systemTrayFrameGrid =
            FindChildByName(rootGrid, L"SystemTrayFrameGrid");
    }

    // Fallback: recursive search if direct children lookup failed
    if (!taskbarFrameRepeater) {
        taskbarFrameRepeater =
            FindDescendantByName(xamlRootContent, L"TaskbarFrameRepeater");
    }
    if (!systemTrayFrameGrid) {
        systemTrayFrameGrid =
            FindDescendantByName(xamlRootContent, L"SystemTrayFrameGrid");
    }

    if (!taskbarFrameRepeater || !systemTrayFrameGrid) {
        Wh_Log(L"Required XAML elements not found");
        return false;
    }

    bool unloading = g_unloading.load();

    // Hide/show task list area
    taskbarFrameRepeater.Visibility(unloading ? Visibility::Visible
                                              : Visibility::Collapsed);

    // Hide/show taskbar background
    if (taskbarBackground) {
        FrameworkElement bgChild = taskbarBackground;
        FrameworkElement bgGrid =
            FindChildByClassName(bgChild, L"Windows.UI.Xaml.Controls.Grid");
        if (!bgGrid) {
            bgGrid = EnumChildElements(
                bgChild, [](FrameworkElement c) { return true; });
        }
        if (bgGrid) {
            FrameworkElement backgroundFill =
                FindChildByName(bgGrid, L"BackgroundFill");
            FrameworkElement backgroundStroke =
                FindChildByName(bgGrid, L"BackgroundStroke");

            if (backgroundFill) {
                backgroundFill.Opacity(unloading ? 1.0 : 0.0);
            }
            if (backgroundStroke) {
                backgroundStroke.Opacity(unloading ? 1.0 : 0.0);
            }
        }
    }

    // System tray: always visible
    systemTrayFrameGrid.Visibility(Visibility::Visible);

    // Optionally hide clock
    if (!unloading && !g_settings.showClock) {
        FrameworkElement clockElement = nullptr;
        EnumChildElements(systemTrayFrameGrid, [&](FrameworkElement c) {
            auto cn = winrt::get_class_name(c);
            if (cn == L"SystemTray.OmniButton") {
                auto automationId =
                    Automation::AutomationProperties::GetAutomationId(c);
                if (automationId == L"SystemTrayClockButton" ||
                    c.Name() == L"ClockButton") {
                    clockElement = c;
                    return true;
                }
            }
            return false;
        });
        if (clockElement) {
            clockElement.Visibility(Visibility::Collapsed);
        }
    }

    // Optionally hide "Show Desktop"
    if (!unloading) {
        FrameworkElement showDesktopStack =
            FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
        if (showDesktopStack) {
            showDesktopStack.Visibility(g_settings.showShowDesktop
                                            ? Visibility::Visible
                                            : Visibility::Collapsed);
        }
    }

    Wh_Log(L"ApplyTrayOnlyStyle: unloading=%d", unloading);
    return true;
}

// ---------------------------------------------------------------------------
// Auto-hide implementation
// ---------------------------------------------------------------------------

void GetTrayRect(HWND hTaskbarWnd, RECT* rc) {
    GetWindowRect(hTaskbarWnd, rc);
}

void GetHotZoneRect(RECT* rc) {
    HMONITOR hMon =
        MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);

    RECT trayRect;
    GetTrayRect(g_hTaskbarWnd, &trayRect);
    int trayWidth = trayRect.right - trayRect.left;
    if (trayWidth < kTrayMinWidth) {
        trayWidth = kTrayMinWidth;
    }

    rc->left = mi.rcMonitor.right - trayWidth;
    rc->top = mi.rcMonitor.bottom - kHotZoneHeight;
    rc->right = mi.rcMonitor.right;
    rc->bottom = mi.rcMonitor.bottom;
}

void ShowTrayWindow() {
    if (g_trayVisible || !g_trayOnlyMode) {
        return;
    }

    RECT trayRect;
    GetTrayRect(g_hTaskbarWnd, &trayRect);
    int h = trayRect.bottom - trayRect.top;
    if (h <= 0) {
        h = kTrayDefaultHeight;
    }

    HMONITOR hMon =
        MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);

    int w = trayRect.right - trayRect.left;
    int x = mi.rcMonitor.right - w;
    int y = mi.rcMonitor.bottom - h;

    SetWindowPos(g_hTaskbarWnd, HWND_TOPMOST, x, y, w, h,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);

    g_trayVisible = true;
    Wh_Log(L"Tray shown");
}

void HideTrayWindow() {
    if (!g_trayVisible || !g_trayOnlyMode) {
        return;
    }

    RECT trayRect;
    GetTrayRect(g_hTaskbarWnd, &trayRect);
    int h = trayRect.bottom - trayRect.top;
    if (h <= 0) {
        h = kTrayDefaultHeight;
    }

    HMONITOR hMon =
        MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);

    int w = trayRect.right - trayRect.left;
    int x = mi.rcMonitor.right - w;
    int y = mi.rcMonitor.bottom;  // Just below the screen edge

    SetWindowPos(g_hTaskbarWnd, HWND_TOPMOST, x, y, w, h,
                 SWP_NOACTIVATE);

    g_trayVisible = false;
    Wh_Log(L"Tray hidden");
}

void CALLBACK AutoHideTimerProc(HWND hWnd, UINT, UINT_PTR, DWORD) {
    if (!g_trayOnlyMode || !g_settings.autoHide) {
        return;
    }

    POINT pt;
    GetCursorPos(&pt);

    RECT trayRect;
    GetTrayRect(g_hTaskbarWnd, &trayRect);

    RECT hotZone;
    GetHotZoneRect(&hotZone);

    bool inTray = PtInRect(&trayRect, pt);
    bool inHotZone = PtInRect(&hotZone, pt);

    if (inTray || inHotZone) {
        g_lastMouseInTrayTick = GetTickCount();
        if (!g_trayVisible) {
            ShowTrayWindow();
        }
    } else {
        if (g_trayVisible) {
            DWORD elapsed = GetTickCount() - g_lastMouseInTrayTick;
            if (elapsed >= static_cast<DWORD>(g_settings.autoHideDelayMs)) {
                HideTrayWindow();
            }
        }
    }
}

void StartAutoHideTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }
    g_lastMouseInTrayTick = GetTickCount();
    SetTimer(g_hTaskbarWnd, kAutoHideTimerId, kAutoHidePollMs,
             AutoHideTimerProc);
}

void StopAutoHideTimer() {
    if (g_hTaskbarWnd) {
        KillTimer(g_hTaskbarWnd, kAutoHideTimerId);
    }
}

// ---------------------------------------------------------------------------
// Transform taskbar into tray-only floating window
// ---------------------------------------------------------------------------

void TransformToTrayOnly(HWND hTaskbarWnd) {
    Wh_Log(L"TransformToTrayOnly");

    g_hTaskbarWnd = hTaskbarWnd;

    // Save original rect/style for restoration (only on first transform)
    if (!g_trayOnlyMode) {
        GetWindowRect(hTaskbarWnd, &g_originalTaskbarRect);
        g_originalExStyle = GetWindowLong(hTaskbarWnd, GWL_EXSTYLE);
    }

    // Ensure the window is visible before operating on XAML
    if (!IsWindowVisible(hTaskbarWnd)) {
        ShowWindow(hTaskbarWnd, SW_SHOWNOACTIVATE);
    }

    // 1. Remove AppBar reservation
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = hTaskbarWnd;
    SHAppBarMessage(ABM_REMOVE, &abd);
    Wh_Log(L"AppBar removed");

    // 2. Apply XAML style (collapse task list, transparent background)
    XamlRoot xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
    int trayWidth = kTrayMinWidth;
    if (xamlRoot) {
        ApplyTrayOnlyStyle(xamlRoot);
        trayWidth = GetTrayWidthFromXaml(xamlRoot);
    } else {
        Wh_Log(L"XamlRoot not available yet, will retry on next opportunity");
    }

    // Add some padding to the tray width
    if (trayWidth < kTrayMinWidth) {
        trayWidth = kTrayMinWidth;
    }
    trayWidth += 16;

    // 3. Change window style to floating toolwindow
    LONG exStyle = GetWindowLong(hTaskbarWnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    exStyle &= ~WS_EX_APPWINDOW;
    SetWindowLong(hTaskbarWnd, GWL_EXSTYLE, exStyle);

    // 4. Resize and position at bottom-right
    HMONITOR hMon =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);

    int trayHeight = kTrayDefaultHeight;
    int x = mi.rcMonitor.right - trayWidth;
    int y = mi.rcMonitor.bottom - trayHeight;

    SetWindowPos(hTaskbarWnd, HWND_TOPMOST, x, y, trayWidth, trayHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);

    g_trayOnlyMode = true;
    g_trayVisible = true;

    // 5. Start auto-hide timer
    if (g_settings.autoHide) {
        g_lastMouseInTrayTick = GetTickCount();
        StartAutoHideTimer();
    }

    Wh_Log(L"Transform complete: %dx%d at (%d,%d)", trayWidth, trayHeight, x,
            y);
}

void RestoreTaskbar(HWND hTaskbarWnd) {
    Wh_Log(L"RestoreTaskbar");

    StopAutoHideTimer();

    // Restore XAML visibility
    XamlRoot xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
    if (xamlRoot) {
        ApplyTrayOnlyStyle(xamlRoot);  // g_unloading is true — restores all
    }

    // Restore window style
    SetWindowLong(hTaskbarWnd, GWL_EXSTYLE, g_originalExStyle);

    // Restore position and size
    int w = g_originalTaskbarRect.right - g_originalTaskbarRect.left;
    int h = g_originalTaskbarRect.bottom - g_originalTaskbarRect.top;
    if (w > 0 && h > 0) {
        SetWindowPos(hTaskbarWnd, HWND_BOTTOM, g_originalTaskbarRect.left,
                     g_originalTaskbarRect.top, w, h,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    // Re-register AppBar so Windows reserves the taskbar space again.
    // If this doesn't fully restore, the user can restart explorer.exe.
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = hTaskbarWnd;
    abd.uEdge = ABE_BOTTOM;
    abd.rc = g_originalTaskbarRect;
    SHAppBarMessage(ABM_NEW, &abd);
    SHAppBarMessage(ABM_SETPOS, &abd);

    g_trayOnlyMode = false;
    g_trayVisible = true;
}

// ---------------------------------------------------------------------------
// TrayUI::WndProc hook — intercept hide requests from ObjectDock etc.
// ---------------------------------------------------------------------------

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           UINT Msg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                    HWND hWnd,
                                    UINT Msg,
                                    WPARAM wParam,
                                    LPARAM lParam,
                                    bool* flag) {
    if (g_unloading) {
        return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam,
                                       flag);
    }

    switch (Msg) {
        case WM_NCCREATE:
            Wh_Log(L"WM_NCCREATE: %08X", (DWORD)(ULONG_PTR)hWnd);
            g_hTaskbarWnd = hWnd;
            break;

        case WM_SHOWWINDOW:
            if (wParam == FALSE) {
                if (g_trayOnlyMode) {
                    Wh_Log(L"Blocked repeated hide request");
                    return 0;
                }
                // First hide request (from ObjectDock etc.): intercept and
                // transform instead of hiding.
                Wh_Log(L"Hide request intercepted, transforming to tray-only");
                TransformToTrayOnly(hWnd);
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGING:
            if (g_trayOnlyMode) {
                WINDOWPOS* wp = (WINDOWPOS*)lParam;
                // Prevent hiding via SetWindowPos
                wp->flags &= ~SWP_HIDEWINDOW;
            }
            break;

        case WM_DISPLAYCHANGE:
            if (g_trayOnlyMode) {
                // Monitor layout changed — reposition
                HMONITOR hMon =
                    MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
                MONITORINFO mi = {sizeof(mi)};
                GetMonitorInfo(hMon, &mi);

                RECT rc;
                GetWindowRect(hWnd, &rc);
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;
                int x = mi.rcMonitor.right - w;
                int y = mi.rcMonitor.bottom - h;
                SetWindowPos(hWnd, HWND_TOPMOST, x, y, w, h,
                             SWP_NOACTIVATE);
            }
            break;
    }

    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

// ---------------------------------------------------------------------------
// Symbol hooks
// ---------------------------------------------------------------------------

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
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
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ---------------------------------------------------------------------------
// Taskbar.View.dll load detection
// ---------------------------------------------------------------------------

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                    HANDLE hFile,
                                    DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}

// ---------------------------------------------------------------------------
// Apply settings from taskbar thread
// ---------------------------------------------------------------------------

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings from taskbar thread");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"Taskbar window not found");
        return;
    }

    g_hTaskbarWnd = hTaskbarWnd;

    if (g_unloading) {
        if (g_trayOnlyMode) {
            RestoreTaskbar(hTaskbarWnd);
        }
        return;
    }

    // If not yet in tray-only mode, transform now (for initial load or
    // settings change).
    if (!g_trayOnlyMode) {
        TransformToTrayOnly(hTaskbarWnd);
    } else {
        // Re-apply XAML style (settings may have changed)
        XamlRoot xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
        if (xamlRoot) {
            ApplyTrayOnlyStyle(xamlRoot);
        }

        // Update auto-hide timer
        StopAutoHideTimer();
        if (g_settings.autoHide) {
            StartAutoHideTimer();
        } else {
            ShowTrayWindow();
        }
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void*) { ApplySettingsFromTaskbarThread(); },
        nullptr);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.autoHide = Wh_GetIntSetting(L"autoHide");
    g_settings.autoHideDelayMs = Wh_GetIntSetting(L"autoHideDelayMs");
    if (g_settings.autoHideDelayMs <= 0) {
        g_settings.autoHideDelayMs = 1000;
    }
    g_settings.showClock = Wh_GetIntSetting(L"showClock");
    g_settings.showShowDesktop = Wh_GetIntSetting(L"showShowDesktop");
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"HookTaskbarDllSymbols failed");
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
    } else {
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

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded.exchange(true);
        }
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }

    return TRUE;
}

// ==WindhawkMod==
// @id              systemtray-only-show
// @name            System Tray Only
// @name:ja-JP      システムトレイのみ表示
// @description     Hides everything except the system tray, turning the taskbar into a small floating tray in the bottom-right corner. Works standalone without any third-party dock. (Windows 11 25H2)
// @description:ja-JP システムトレイ以外を非表示にし、タスクバーを右下角の小さなフローティングトレイに変換します。サードパーティのドックなしで単独動作します。（Windows 11 25H2）
// @version         0.14
// @author          roflsunriz
// @github          roflsunriz
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Tray Only

Hides all taskbar elements except the system tray (notification area) and
displays it as a small floating window in the bottom-right corner of the screen.

The mod activates automatically on load — no third-party dock or external
trigger is required. It removes the AppBar space reservation so that maximized
windows use the full screen, and keeps only the system tray visible as a
small floating overlay.

## Features

- **Auto-hide**: The tray slides out of sight when the mouse leaves and
  reappears when you hover near the bottom-right corner.
- **Configurable delay**: Set how long to wait before hiding (milliseconds).
- **Optional clock**: Show or hide the date/time in the tray.
- **Optional "Show Desktop" button**: Show or hide the narrow strip at the
  far right of the tray.
- **Background opacity**: Adjust from fully transparent (0) to fully opaque
  (100). Default is 80 for a subtle glass look.

## Requirements

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
- backgroundOpacity: 80
  $name: Background opacity (0-100)
  $name:ja-JP: 背景の不透明度（0-100）
  $description: >-
    0 = fully transparent, 100 = fully opaque (default 80 for a subtle glass effect)
  $description:ja-JP: >-
    0 = 完全透明、100 = 完全不透明（デフォルト80で控えめなガラス効果）
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <windowsx.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

using namespace winrt::Windows::UI::Xaml;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct {
    bool autoHide;
    int autoHideDelayMs;
    bool showClock;
    bool showShowDesktop;
    int backgroundOpacity;
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
bool g_inOwnSetWindowPos = false;
bool g_taskbarTouchRegistered = false;
bool g_suppressAppBarReservation = false;
bool g_originalWorkAreaValid = false;
int g_desiredTrayX = 0;
int g_desiredTrayY = 0;
int g_desiredTrayWidth = 0;
int g_desiredTrayHeight = 0;
int g_screenBottom = 0;
int g_screenLeft = 0;
int g_screenRight = 0;
RECT g_originalWorkArea{};
DWORD g_lastMouseInTrayTick;
UINT_PTR g_autoHideTimerHandle = 0;
DWORD g_lastDiagLogTick = 0;
DWORD g_lastAppBarSuppressLogTick = 0;
DWORD g_lastAppBarRemoveTick = 0;
DWORD g_lastWorkAreaApplyTick = 0;
HHOOK g_mouseHook = nullptr;
std::atomic<bool> g_showRequested{false};
DWORD g_lastXamlReapplyTick = 0;

constexpr UINT kAutoHidePollMs = 100;
constexpr int kHotZoneHeight = 48;
constexpr int kTrayDefaultHeight = 48;
constexpr int kTrayMinWidth = 200;
constexpr int kTrayMeasurementPadding = 8;
constexpr DWORD kAppBarSuppressLogThrottleMs = 1000;
constexpr DWORD kAppBarRemoveEnforceIntervalMs = 1000;
constexpr DWORD kWorkAreaEnforceIntervalMs = 1000;

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

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;
DWORD g_lastFrameHeightSuppressLogTick = 0;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

int WINAPI TaskbarHost_FrameHeight_Hook(void* pThis) {
    int originalHeight =
        TaskbarHost_FrameHeight_Original
            ? TaskbarHost_FrameHeight_Original(pThis)
            : 0;

    if (g_suppressAppBarReservation && !g_unloading.load()) {
        DWORD now = GetTickCount();
        if (now - g_lastFrameHeightSuppressLogTick >=
            kAppBarSuppressLogThrottleMs) {
            g_lastFrameHeightSuppressLogTick = now;
            Wh_Log(L"Suppressed TaskbarHost::FrameHeight original=%d",
                   originalHeight);
        }
        return 0;
    }

    return originalHeight;
}

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

HWND GetTaskbandWindow(HWND hTaskbarWnd) {
    return hTaskbarWnd ? (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND")
                       : nullptr;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = GetTaskbandWindow(hTaskbarWnd);
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

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;

const wchar_t* GetAppBarMessageName(DWORD dwMessage) {
    switch (dwMessage) {
        case ABM_NEW:
            return L"ABM_NEW";
        case ABM_REMOVE:
            return L"ABM_REMOVE";
        case ABM_QUERYPOS:
            return L"ABM_QUERYPOS";
        case ABM_SETPOS:
            return L"ABM_SETPOS";
        case ABM_WINDOWPOSCHANGED:
            return L"ABM_WINDOWPOSCHANGED";
    }

    return L"ABM_OTHER";
}

bool ShouldSuppressTaskbarAppBarMessage(DWORD dwMessage, PAPPBARDATA pData) {
    if (!g_suppressAppBarReservation || g_unloading.load() || !pData) {
        return false;
    }

    HWND taskbarWnd = g_hTaskbarWnd ? g_hTaskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd || pData->hWnd != taskbarWnd) {
        return false;
    }

    switch (dwMessage) {
        case ABM_NEW:
        case ABM_QUERYPOS:
        case ABM_SETPOS:
        case ABM_WINDOWPOSCHANGED:
            return true;
    }

    return false;
}

UINT_PTR WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    if (ShouldSuppressTaskbarAppBarMessage(dwMessage, pData)) {
        DWORD now = GetTickCount();
        if (now - g_lastAppBarSuppressLogTick >= kAppBarSuppressLogThrottleMs) {
            g_lastAppBarSuppressLogTick = now;
            Wh_Log(L"Suppressed %s for taskbar AppBar reservation",
                   GetAppBarMessageName(dwMessage));
        }
        return TRUE;
    }

    return SHAppBarMessage_Original(dwMessage, pData);
}

void ForceRemoveTaskbarAppBarReservation(HWND hTaskbarWnd, bool logResult) {
    if (!hTaskbarWnd) {
        return;
    }

    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = hTaskbarWnd;

    UINT_PTR result = SHAppBarMessage_Original
                          ? SHAppBarMessage_Original(ABM_REMOVE, &abd)
                          : SHAppBarMessage(ABM_REMOVE, &abd);
    if (logResult) {
        Wh_Log(L"Force ABM_REMOVE result=%llu",
               static_cast<unsigned long long>(result));
    }
}

bool GetTaskbarMonitorInfo(HWND hTaskbarWnd, MONITORINFO* monitorInfo) {
    if (!hTaskbarWnd || !monitorInfo) {
        return false;
    }

    HMONITOR hMon =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    if (!hMon) {
        return false;
    }

    monitorInfo->cbSize = sizeof(*monitorInfo);
    return !!GetMonitorInfo(hMon, monitorInfo);
}

void CaptureOriginalWorkArea(HWND hTaskbarWnd) {
    if (g_originalWorkAreaValid) {
        return;
    }

    MONITORINFO monitorInfo{};
    if (!GetTaskbarMonitorInfo(hTaskbarWnd, &monitorInfo)) {
        Wh_Log(L"Failed to capture original work area");
        return;
    }

    g_originalWorkArea = monitorInfo.rcWork;
    g_originalWorkAreaValid = true;
    Wh_Log(L"Captured original work area=(%ld,%ld,%ld,%ld)",
           g_originalWorkArea.left, g_originalWorkArea.top,
           g_originalWorkArea.right, g_originalWorkArea.bottom);
}

void ApplyFullMonitorWorkArea(HWND hTaskbarWnd, bool logResult) {
    MONITORINFO monitorInfo{};
    if (!GetTaskbarMonitorInfo(hTaskbarWnd, &monitorInfo)) {
        if (logResult) {
            Wh_Log(L"Failed to query monitor info for work area override");
        }
        return;
    }

    RECT workArea = monitorInfo.rcMonitor;
    BOOL result = SystemParametersInfo(SPI_SETWORKAREA, 0, &workArea, 0);
    if (logResult || !result) {
        Wh_Log(L"Apply full work area result=%d rect=(%ld,%ld,%ld,%ld)",
               result,
               workArea.left, workArea.top, workArea.right, workArea.bottom);
    }
}

void RestoreOriginalWorkArea(bool logResult) {
    if (!g_originalWorkAreaValid) {
        return;
    }

    BOOL result = SystemParametersInfo(SPI_SETWORKAREA, 0,
                                       &g_originalWorkArea, 0);
    if (logResult || !result) {
        Wh_Log(L"Restore work area result=%d rect=(%ld,%ld,%ld,%ld)",
               result,
               g_originalWorkArea.left, g_originalWorkArea.top,
               g_originalWorkArea.right, g_originalWorkArea.bottom);
    }
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

bool ApplyTrayOnlyStyle(XamlRoot xamlRoot) {
    static bool s_loggedTree = false;

    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        return false;
    }

    FrameworkElement systemTrayFrameGrid =
        FindDescendantByName(xamlRootContent, L"SystemTrayFrameGrid");
    if (!systemTrayFrameGrid) {
        Wh_Log(L"SystemTrayFrameGrid not found");
        return false;
    }

    bool unloading = g_unloading.load();

    // Walk up from SystemTrayFrameGrid toward xamlRootContent.
    // At each level collapse ALL siblings (including TaskbarBackground).
    FrameworkElement current = systemTrayFrameGrid;
    int depth = 0;
    while (current) {
        auto parentDO = Media::VisualTreeHelper::GetParent(current);
        FrameworkElement parent = parentDO.try_as<FrameworkElement>();
        if (!parent) {
            break;
        }

        int count = Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto sibling =
                Media::VisualTreeHelper::GetChild(parent, i)
                    .try_as<FrameworkElement>();
            if (!sibling || sibling == current) {
                continue;
            }

            if (!s_loggedTree) {
                Wh_Log(L"  depth=%d sibling[%d]: name='%s' class='%s'",
                        depth, i, sibling.Name().c_str(),
                        winrt::get_class_name(sibling).c_str());
            }

            sibling.Visibility(unloading ? Visibility::Visible
                                         : Visibility::Collapsed);
        }

        current.Visibility(Visibility::Visible);

        if (!s_loggedTree) {
            Wh_Log(L"  depth=%d ancestor: name='%s' class='%s' "
                    L"(parent: name='%s' class='%s', %d children)",
                    depth, current.Name().c_str(),
                    winrt::get_class_name(current).c_str(),
                    parent.Name().c_str(),
                    winrt::get_class_name(parent).c_str(), count);
        }

        if (parent == xamlRootContent) {
            depth++;
            break;
        }
        current = parent;
        depth++;
    }

    if (!s_loggedTree) {
        Wh_Log(L"XAML walk-up complete: %d levels", depth);
        s_loggedTree = true;
    }

    // Set semi-transparent background directly on SystemTrayFrameGrid
    if (auto panel = systemTrayFrameGrid.try_as<Controls::Panel>()) {
        if (unloading) {
            panel.Background(nullptr);
        } else {
            double bgOpacity =
                static_cast<double>(g_settings.backgroundOpacity) / 100.0;
            winrt::Windows::UI::Color bgColor{
                static_cast<uint8_t>(bgOpacity * 255.0), 32, 32, 32};
            panel.Background(Media::SolidColorBrush(bgColor));
        }
    }

    systemTrayFrameGrid.Visibility(Visibility::Visible);
    systemTrayFrameGrid.Opacity(1.0);
    systemTrayFrameGrid.IsHitTestVisible(true);

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

    if (!unloading) {
        FrameworkElement showDesktopStack =
            FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
        if (showDesktopStack) {
            showDesktopStack.Visibility(g_settings.showShowDesktop
                                            ? Visibility::Visible
                                            : Visibility::Collapsed);
        }
    }

    return true;
}

void UnionBounds(winrt::Windows::Foundation::Rect* bounds,
                 const winrt::Windows::Foundation::Rect& other) {
    float left = std::min(bounds->X, other.X);
    float top = std::min(bounds->Y, other.Y);
    float right = std::max(bounds->X + bounds->Width, other.X + other.Width);
    float bottom =
        std::max(bounds->Y + bounds->Height, other.Y + other.Height);

    bounds->X = left;
    bounds->Y = top;
    bounds->Width = right - left;
    bounds->Height = bottom - top;
}

bool TryGetElementBounds(
    FrameworkElement element,
    FrameworkElement relativeTo,
    winrt::Windows::Foundation::Rect* bounds) {
    if (!element || !relativeTo || element.Visibility() != Visibility::Visible) {
        return false;
    }

    double actualWidth = element.ActualWidth();
    double actualHeight = element.ActualHeight();
    if (actualWidth <= 0.0 || actualHeight <= 0.0) {
        return false;
    }

    auto transform = element.TransformToVisual(relativeTo);
    winrt::Windows::Foundation::Rect elementRect{
        0.0f, 0.0f, static_cast<float>(actualWidth),
        static_cast<float>(actualHeight)};
    *bounds = transform.TransformBounds(elementRect);

    return bounds->Width > 0.0f && bounds->Height > 0.0f;
}

bool TryGetContentBounds(
    FrameworkElement element,
    FrameworkElement relativeTo,
    winrt::Windows::Foundation::Rect* bounds) {
    if (!element || element.Visibility() != Visibility::Visible) {
        return false;
    }

    bool hasChildBounds = false;
    winrt::Windows::Foundation::Rect childBounds{};
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(element, i)
                .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        winrt::Windows::Foundation::Rect currentChildBounds{};
        if (!TryGetContentBounds(child, relativeTo, &currentChildBounds)) {
            continue;
        }

        if (!hasChildBounds) {
            childBounds = currentChildBounds;
            hasChildBounds = true;
        } else {
            UnionBounds(&childBounds, currentChildBounds);
        }
    }

    if (hasChildBounds) {
        *bounds = childBounds;
        return true;
    }

    winrt::Windows::Foundation::Rect elementBounds{};
    if (!TryGetElementBounds(element, relativeTo, &elementBounds)) {
        return false;
    }

    double relativeWidth = relativeTo.ActualWidth();
    bool likelyStretchBackground =
        childrenCount == 0 && element.Name().empty() && relativeWidth > 0.0 &&
        elementBounds.Width >= static_cast<float>(relativeWidth * 0.95);
    if (likelyStretchBackground) {
        return false;
    }

    *bounds = elementBounds;
    return true;
}

void UpdateMonitorBounds(HWND hTaskbarWnd) {
    HMONITOR hMon =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMon, &mi);

    g_screenBottom = mi.rcMonitor.bottom;
    g_screenLeft = mi.rcMonitor.left;
    g_screenRight = mi.rcMonitor.right;
}

void NormalizeTrayMetrics() {
    int monitorWidth = std::max(0, g_screenRight - g_screenLeft);
    if (g_desiredTrayHeight <= 0) {
        g_desiredTrayHeight = kTrayDefaultHeight;
    }

    if (g_desiredTrayWidth <= 0) {
        g_desiredTrayWidth = monitorWidth;
    }

    if (monitorWidth > 0) {
        g_desiredTrayWidth = std::min(g_desiredTrayWidth, monitorWidth);
        int minimumWidth = std::min(kTrayMinWidth, monitorWidth);
        if (g_desiredTrayWidth < minimumWidth) {
            g_desiredTrayWidth = minimumWidth;
        }
    }

    g_desiredTrayX = g_screenRight - g_desiredTrayWidth;
    g_desiredTrayY = g_screenBottom - g_desiredTrayHeight;
}

bool RefreshTrayMetricsFromXaml(XamlRoot xamlRoot) {
    int oldX = g_desiredTrayX;
    int oldY = g_desiredTrayY;
    int oldWidth = g_desiredTrayWidth;
    int oldHeight = g_desiredTrayHeight;

    FrameworkElement xamlRootContent =
        xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
    if (xamlRootContent) {
        FrameworkElement systemTrayFrameGrid =
            FindDescendantByName(xamlRootContent, L"SystemTrayFrameGrid");
        if (systemTrayFrameGrid) {
            xamlRootContent.UpdateLayout();
            systemTrayFrameGrid.UpdateLayout();

            winrt::Windows::Foundation::Rect trayBounds{};
            if (TryGetContentBounds(systemTrayFrameGrid, systemTrayFrameGrid,
                                    &trayBounds)) {
                float scale = xamlRoot.RasterizationScale();
                if (scale <= 0.0f) {
                    scale = 1.0f;
                }

                g_desiredTrayWidth = static_cast<int>(std::ceil(
                    static_cast<double>(trayBounds.Width +
                                        kTrayMeasurementPadding) *
                    scale));
            }
        }
    }

    NormalizeTrayMetrics();

    bool changed = oldX != g_desiredTrayX || oldY != g_desiredTrayY ||
                   oldWidth != g_desiredTrayWidth ||
                   oldHeight != g_desiredTrayHeight;
    if (changed) {
        Wh_Log(L"Tray metrics updated: pos=(%d,%d) size=%dx%d",
               g_desiredTrayX, g_desiredTrayY,
               g_desiredTrayWidth, g_desiredTrayHeight);
    }

    return changed;
}

bool IsAutoHideTriggerMouseMessage(WPARAM message) {
    switch (message) {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return true;
    }

    return false;
}

void RegisterTaskbarTouchWindow(HWND hWnd) {
    if (!hWnd || g_taskbarTouchRegistered) {
        return;
    }

    if (RegisterTouchWindow(hWnd, TWF_WANTPALM)) {
        g_taskbarTouchRegistered = true;
        Wh_Log(L"RegisterTouchWindow succeeded for taskbar window");
    } else {
        Wh_Log(L"RegisterTouchWindow failed for taskbar window: %lu",
               GetLastError());
    }
}

void UnregisterTaskbarTouchWindow(HWND hWnd) {
    if (!hWnd || !g_taskbarTouchRegistered) {
        return;
    }

    if (UnregisterTouchWindow(hWnd)) {
        Wh_Log(L"UnregisterTouchWindow succeeded for taskbar window");
    } else {
        Wh_Log(L"UnregisterTouchWindow failed for taskbar window: %lu",
               GetLastError());
    }

    g_taskbarTouchRegistered = false;
}

void ShowTrayWindow();
void TransformToTrayOnly(HWND hTaskbarWnd);

// ---------------------------------------------------------------------------
// Auto-hide implementation
// ---------------------------------------------------------------------------

void GetTrayRect(RECT* rc) {
    if (g_trayVisible) {
        rc->left = g_desiredTrayX;
        rc->top = g_desiredTrayY;
        rc->right = g_desiredTrayX + g_desiredTrayWidth;
        rc->bottom = g_desiredTrayY + g_desiredTrayHeight;
    } else {
        rc->left = g_desiredTrayX;
        rc->top = g_screenBottom;
        rc->right = g_desiredTrayX + g_desiredTrayWidth;
        rc->bottom = g_screenBottom + g_desiredTrayHeight;
    }
}

void GetHotZoneRect(RECT* rc) {
    rc->left = g_desiredTrayX;
    rc->top = g_screenBottom - kHotZoneHeight;
    rc->right = g_desiredTrayX + g_desiredTrayWidth;
    rc->bottom = g_screenBottom;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    if (nCode == HC_ACTION && IsAutoHideTriggerMouseMessage(wParam) &&
        g_trayOnlyMode.load() && g_settings.autoHide) {
        auto* mhs = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        RECT hotZone;
        GetHotZoneRect(&hotZone);

        if (PtInRect(&hotZone, mhs->pt)) {
            g_lastMouseInTrayTick = GetTickCount();
            if (!g_trayVisible) {
                g_showRequested.store(true);
            }
        } else if (g_trayVisible) {
            RECT trayRect;
            GetTrayRect(&trayRect);
            if (PtInRect(&trayRect, mhs->pt)) {
                g_lastMouseInTrayTick = GetTickCount();
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

void EnsureWindowVisible(HWND hWnd, PCWSTR windowName) {
    if (!hWnd || IsWindowVisible(hWnd)) {
        return;
    }
    g_inOwnSetWindowPos = true;
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
    g_inOwnSetWindowPos = false;
    Wh_Log(L"Re-shown hidden %s", windowName);
}

void ShowTrayWindow() {
    if (g_trayVisible || !g_trayOnlyMode || !g_hTaskbarWnd) {
        return;
    }

    EnsureWindowVisible(g_hTaskbarWnd, L"taskbar window");
    XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
    if (xamlRoot) {
        ApplyTrayOnlyStyle(xamlRoot);
        RefreshTrayMetricsFromXaml(xamlRoot);
    } else {
        NormalizeTrayMetrics();
    }

    g_inOwnSetWindowPos = true;
    SetWindowPos(g_hTaskbarWnd, HWND_TOPMOST,
                 g_desiredTrayX, g_desiredTrayY,
                 g_desiredTrayWidth, g_desiredTrayHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    g_inOwnSetWindowPos = false;

    g_trayVisible = true;
    Wh_Log(L"Tray shown");
}

void HideTrayWindow() {
    if (!g_trayVisible || !g_trayOnlyMode || !g_hTaskbarWnd) {
        return;
    }

    g_trayVisible = false;

    g_inOwnSetWindowPos = true;
    SetWindowPos(g_hTaskbarWnd, HWND_TOPMOST,
                 g_desiredTrayX, g_screenBottom,
                 g_desiredTrayWidth, g_desiredTrayHeight,
                 SWP_NOACTIVATE);
    g_inOwnSetWindowPos = false;

    Wh_Log(L"Tray hidden");
}

void CALLBACK AutoHideTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    if (!g_trayOnlyMode || !g_hTaskbarWnd) {
        return;
    }

    EnsureWindowVisible(g_hTaskbarWnd, L"taskbar window");

    DWORD now = GetTickCount();
    if (now - g_lastAppBarRemoveTick >= kAppBarRemoveEnforceIntervalMs) {
        g_lastAppBarRemoveTick = now;
        ForceRemoveTaskbarAppBarReservation(g_hTaskbarWnd, false);
    }
    if (now - g_lastWorkAreaApplyTick >= kWorkAreaEnforceIntervalMs) {
        g_lastWorkAreaApplyTick = now;
        ApplyFullMonitorWorkArea(g_hTaskbarWnd, false);
    }

    // LL mouse hook may have set this flag on any mouse move
    bool hookTriggered = g_showRequested.exchange(false);

    POINT pt;
    GetCursorPos(&pt);

    RECT trayRect;
    GetTrayRect(&trayRect);

    RECT hotZone;
    GetHotZoneRect(&hotZone);

    bool inTray = g_trayVisible && PtInRect(&trayRect, pt);
    bool inHotZone = PtInRect(&hotZone, pt);

    if (now - g_lastDiagLogTick >= 10000) {
        g_lastDiagLogTick = now;
        Wh_Log(L"[Diag] cursor=(%ld,%ld) vis=%d inTray=%d inHZ=%d "
                L"tray=(%ld,%ld,%ld,%ld) hz=(%ld,%ld,%ld,%ld) wsVis=%d "
                L"hook=%d",
                pt.x, pt.y, g_trayVisible, inTray, inHotZone,
                trayRect.left, trayRect.top, trayRect.right, trayRect.bottom,
                hotZone.left, hotZone.top, hotZone.right, hotZone.bottom,
                !!IsWindowVisible(g_hTaskbarWnd),
                !!g_mouseHook);
    }

    if (g_settings.autoHide) {
        if (hookTriggered || inTray || inHotZone) {
            g_lastMouseInTrayTick = now;
            if (!g_trayVisible) {
                Wh_Log(L"Tray show reason: hook=%d inTray=%d inHZ=%d "
                       L"cursor=(%ld,%ld)",
                       hookTriggered, inTray, inHotZone, pt.x, pt.y);
                ShowTrayWindow();
            }
        } else if (g_trayVisible) {
            DWORD elapsed = now - g_lastMouseInTrayTick;
            if (elapsed >= static_cast<DWORD>(g_settings.autoHideDelayMs)) {
                HideTrayWindow();
            }
        }
    }

    // Re-apply XAML state every second while visible (shell may restore elements)
    if (g_trayVisible && (now - g_lastXamlReapplyTick >= 1000)) {
        g_lastXamlReapplyTick = now;
        XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
        if (xamlRoot) {
            ApplyTrayOnlyStyle(xamlRoot);
            if (RefreshTrayMetricsFromXaml(xamlRoot)) {
                g_inOwnSetWindowPos = true;
                SetWindowPos(g_hTaskbarWnd, HWND_TOPMOST,
                             g_desiredTrayX, g_desiredTrayY,
                             g_desiredTrayWidth, g_desiredTrayHeight,
                             SWP_NOACTIVATE);
                g_inOwnSetWindowPos = false;
            }
        }
    }
}

void StartAutoHideTimer() {
    if (g_autoHideTimerHandle) {
        return;
    }
    g_lastMouseInTrayTick = GetTickCount();
    g_autoHideTimerHandle =
        SetTimer(nullptr, 0, kAutoHidePollMs, AutoHideTimerProc);

    if (!g_mouseHook) {
        g_mouseHook =
            SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
        if (g_mouseHook) {
            Wh_Log(L"Low-level mouse hook installed");
        } else {
            Wh_Log(L"Failed to install LL mouse hook: err=%lu",
                    GetLastError());
        }
    }

    Wh_Log(L"Auto-hide timer started");
}

void StopAutoHideTimer() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    g_showRequested = false;

    if (g_autoHideTimerHandle) {
        KillTimer(nullptr, g_autoHideTimerHandle);
        g_autoHideTimerHandle = 0;
        Wh_Log(L"Auto-hide timer stopped");
    }
}

// ---------------------------------------------------------------------------
// Transform taskbar into tray-only floating window
// ---------------------------------------------------------------------------

void TransformToTrayOnly(HWND hTaskbarWnd) {
    g_hTaskbarWnd = hTaskbarWnd;

    if (!g_trayOnlyMode) {
        GetWindowRect(hTaskbarWnd, &g_originalTaskbarRect);
        g_originalExStyle = GetWindowLong(hTaskbarWnd, GWL_EXSTYLE);
    }

    g_inOwnSetWindowPos = true;
    if (!IsWindowVisible(hTaskbarWnd)) {
        ShowWindow(hTaskbarWnd, SW_SHOWNOACTIVATE);
    }

    g_suppressAppBarReservation = true;
    CaptureOriginalWorkArea(hTaskbarWnd);
    ForceRemoveTaskbarAppBarReservation(hTaskbarWnd, true);
    ApplyFullMonitorWorkArea(hTaskbarWnd, true);

    LONG exStyle = GetWindowLong(hTaskbarWnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    exStyle &= ~WS_EX_APPWINDOW;
    SetWindowLong(hTaskbarWnd, GWL_EXSTYLE, exStyle);

    UpdateMonitorBounds(hTaskbarWnd);

    XamlRoot xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
    g_desiredTrayHeight = kTrayDefaultHeight;
    g_desiredTrayWidth = g_screenRight - g_screenLeft;
    if (xamlRoot) {
        ApplyTrayOnlyStyle(xamlRoot);
        RefreshTrayMetricsFromXaml(xamlRoot);
    } else {
        NormalizeTrayMetrics();
    }

    g_trayOnlyMode = true;
    g_trayVisible = true;

    SetWindowPos(hTaskbarWnd, HWND_TOPMOST,
                 g_desiredTrayX, g_desiredTrayY,
                 g_desiredTrayWidth, g_desiredTrayHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    g_inOwnSetWindowPos = false;

    RegisterTaskbarTouchWindow(hTaskbarWnd);

    g_lastMouseInTrayTick = GetTickCount();
    StartAutoHideTimer();

    Wh_Log(L"Transform complete: pos=(%d,%d) size=%dx%d",
            g_desiredTrayX, g_desiredTrayY,
            g_desiredTrayWidth, g_desiredTrayHeight);
}

void RestoreTaskbar(HWND hTaskbarWnd) {
    StopAutoHideTimer();
    UnregisterTaskbarTouchWindow(hTaskbarWnd);
    g_suppressAppBarReservation = false;

    g_inOwnSetWindowPos = true;

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
    APPBARDATA abd = {sizeof(abd)};
    abd.hWnd = hTaskbarWnd;
    abd.uEdge = ABE_BOTTOM;
    abd.rc = g_originalTaskbarRect;
    SHAppBarMessage(ABM_NEW, &abd);
    SHAppBarMessage(ABM_SETPOS, &abd);
    RestoreOriginalWorkArea(true);

    g_inOwnSetWindowPos = false;
    g_trayOnlyMode = false;
    g_trayVisible = true;
}

// ---------------------------------------------------------------------------
// TrayUI::WndProc hook — intercept external hide requests while in tray-only mode.
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
            g_hTaskbarWnd = hWnd;
            break;

        case WM_SHOWWINDOW:
            if (wParam == FALSE) {
                if (g_trayOnlyMode) {
                    Wh_Log(L"Blocked external hide request");
                    g_inOwnSetWindowPos = true;
                    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                    g_inOwnSetWindowPos = false;
                    return 0;
                }
                Wh_Log(L"Hide request intercepted, transforming to tray-only");
                TransformToTrayOnly(hWnd);
                return 0;
            }
            break;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_POINTERDOWN:
        case WM_TOUCH:
            if (g_trayOnlyMode && g_settings.autoHide && !g_trayVisible) {
                g_lastMouseInTrayTick = GetTickCount();
                Wh_Log(L"Tray input activated: msg=%u", Msg);
                ShowTrayWindow();
                if (Msg == WM_TOUCH) {
                    CloseTouchInputHandle((HTOUCHINPUT)lParam);
                }
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* wp = (WINDOWPOS*)lParam;
            if (g_trayOnlyMode || g_inOwnSetWindowPos) {
                int desiredX, desiredY;
                if (g_inOwnSetWindowPos) {
                    desiredX = wp->x;
                    desiredY = wp->y;
                } else {
                    desiredX = g_desiredTrayX;
                    desiredY = g_desiredTrayY;
                }
                int desiredCX = g_inOwnSetWindowPos ? wp->cx
                                                    : g_desiredTrayWidth;
                int desiredCY = g_inOwnSetWindowPos ? wp->cy
                                                    : g_desiredTrayHeight;
                UINT desiredFlags = wp->flags;
                desiredFlags &= ~(SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE);

                wp->x = desiredX;
                wp->y = desiredY;
                wp->cx = desiredCX;
                wp->cy = desiredCY;
                wp->flags = desiredFlags;

                LRESULT ret = TrayUI_WndProc_Original(
                    pThis, hWnd, Msg, wParam, lParam, flag);

                wp->x = desiredX;
                wp->y = desiredY;
                wp->cx = desiredCX;
                wp->cy = desiredCY;
                wp->flags = desiredFlags;
                return ret;
            }
            break;
        }

        case WM_DISPLAYCHANGE:
            if (g_trayOnlyMode) {
                UpdateMonitorBounds(hWnd);

                XamlRoot xamlRoot = GetTaskbarXamlRoot(hWnd);
                if (xamlRoot) {
                    ApplyTrayOnlyStyle(xamlRoot);
                    RefreshTrayMetricsFromXaml(xamlRoot);
                } else {
                    NormalizeTrayMetrics();
                }

                int x = g_desiredTrayX;
                g_inOwnSetWindowPos = true;
                SetWindowPos(hWnd, HWND_TOPMOST, x, g_desiredTrayY,
                             g_desiredTrayWidth, g_desiredTrayHeight,
                             SWP_NOACTIVATE);
                g_inOwnSetWindowPos = false;
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
            TaskbarHost_FrameHeight_Hook,
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

bool HookShell32Functions() {
    HMODULE shell32Module = GetModuleHandle(L"shell32.dll");
    if (!shell32Module) {
        shell32Module =
            LoadLibraryEx(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!shell32Module) {
        Wh_Log(L"Failed to load shell32.dll");
        return false;
    }

    auto pSHAppBarMessage =
        reinterpret_cast<decltype(&SHAppBarMessage)>(
            GetProcAddress(shell32Module, "SHAppBarMessage"));
    if (!pSHAppBarMessage) {
        Wh_Log(L"SHAppBarMessage not found");
        return false;
    }

    WindhawkUtils::SetFunctionHook(pSHAppBarMessage,
                                   SHAppBarMessage_Hook,
                                   &SHAppBarMessage_Original);
    return true;
}

// ---------------------------------------------------------------------------
// Apply settings from taskbar thread
// ---------------------------------------------------------------------------

void ApplySettingsFromTaskbarThread() {
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
            if (RefreshTrayMetricsFromXaml(xamlRoot)) {
                g_inOwnSetWindowPos = true;
                SetWindowPos(hTaskbarWnd, HWND_TOPMOST,
                             g_desiredTrayX, g_desiredTrayY,
                             g_desiredTrayWidth, g_desiredTrayHeight,
                             SWP_NOACTIVATE);
                g_inOwnSetWindowPos = false;
            }
        }

        // Update auto-hide timer
        StopAutoHideTimer();
        StartAutoHideTimer();
        if (!g_settings.autoHide) {
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
    g_settings.backgroundOpacity = Wh_GetIntSetting(L"backgroundOpacity");
    if (g_settings.backgroundOpacity < 0) {
        g_settings.backgroundOpacity = 0;
    }
    if (g_settings.backgroundOpacity > 100) {
        g_settings.backgroundOpacity = 100;
    }
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    g_suppressAppBarReservation = false;

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"HookTaskbarDllSymbols failed");
        return FALSE;
    }

    if (!HookShell32Functions()) {
        Wh_Log(L"HookShell32Functions failed");
        return FALSE;
    }

    if (GetTaskbarViewModuleHandle()) {
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
        if (GetTaskbarViewModuleHandle()) {
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
    g_suppressAppBarReservation = false;

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

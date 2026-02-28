// ==WindhawkMod==
// @id              shell-flyout-positions
// @name            Shell Flyout Positions
// @description     Customize the position of the Notification Center, Action Center, and Start menu on Windows 11
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Shell Flyout Positions

Customize the position of the Notification Center (Win+N), Action Center
(Win+A), and Start menu on Windows 11.

## Notification Center and Action Center

By default, Windows 11 displays the Notification Center and Action Center (Quick
Settings) anchored to the right side of the screen. This mod allows you to
change their horizontal alignment to the left or center, with an optional pixel
shift for fine-tuning.

This is especially useful when using custom taskbar themes that relocate the
system tray area, as you can reposition these flyouts to match your custom
layout.

Note: The Notification Center and Action Center can be moved to the top of the
screen with the help of the [Windows 11 Notification Center
Styler](https://windhawk.net/mods/windows-11-notification-center-styler) mod.
For the necessary configuration, see [this comment on
GitHub](https://github.com/ramensoftware/windhawk-mods/issues/1053#issuecomment-2405461863).

![Screenshot](https://i.imgur.com/ezxArY1.png)

## Start menu

The Start menu can be repositioned both horizontally and vertically. You can
align it to the left, center, or right of the screen, and to the top, center, or
bottom. Pixel shift options allow fine-tuning in both directions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- notificationCenter:
  - horizontalAlignment: right
    $name: Horizontal alignment
    $options:
    - right: Right
    - center: Center
    - left: Left
    - tray: Aligned to tray area
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  $name: Notification Center
- actionCenter:
  - horizontalAlignment: same
    $name: Horizontal alignment
    $description: >-
      If "Same as Notification Center" is selected, all settings below are
      ignored and Notification Center settings are used
    $options:
    - same: Same as Notification Center
    - right: Right
    - center: Center
    - left: Left
    - tray: Aligned to tray area
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  $name: Action Center
- startMenu:
  - horizontalAlignment: windowsDefault
    $name: Horizontal alignment
    $options:
    - windowsDefault: Windows default
    - left: Left
    - center: Center
    - right: Right
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  - verticalAlignment: bottom
    $name: Vertical alignment
    $options:
    - bottom: Bottom
    - center: Center
    - top: Top
  - verticalShift: 0
    $name: Vertical shift
    $description: >-
      A positive value moves the element down, a negative value moves it up
  $name: Start menu
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // must come before uiautomation.h

#include <comdef.h>
#include <dwmapi.h>
#include <roapi.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <winrt/base.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <functional>
#include <future>
#include <optional>
#include <string>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

enum class TrayHorizontalAlignment {
    right,
    center,
    left,
    tray,
};

struct TrayElementSettings {
    TrayHorizontalAlignment horizontalAlignment;
    int horizontalShift;
};

enum class StartMenuHorizontalAlignment {
    windowsDefault,
    left,
    center,
    right,
};

enum class StartMenuVerticalAlignment {
    bottom,
    center,
    top,
};

struct StartMenuSettings {
    StartMenuHorizontalAlignment horizontalAlignment;
    int horizontalShift;
    StartMenuVerticalAlignment verticalAlignment;
    int verticalShift;
};

struct {
    TrayElementSettings notificationCenter;
    TrayElementSettings actionCenter;
    StartMenuSettings startMenu;
} g_settings;

enum class Target {
    Explorer,
    StartMenuExperienceHost,
    ShellExperienceHost,
    ShellHost,  // Win11 24H2.
};

Target g_target;

std::atomic<bool> g_unloading;
HWND g_notificationCenterWnd;
HWND g_searchMenuWnd;
POINT g_searchMenuOriginalPos;
POINT g_searchMenuCustomPos = {LONG_MAX, LONG_MAX};

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

std::wstring GetThreadDescriptionAsString(HANDLE thread) {
    std::wstring result;

    PWSTR threadDescription;
    HRESULT hr = GetThreadDescription(thread, &threadDescription);
    if (SUCCEEDED(hr)) {
        result = threadDescription;
        LocalFree(threadDescription);
    }

    return result;
}

std::wstring GetThreadIdDescriptionAsString(DWORD threadId) {
    std::wstring result;

    HANDLE thread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
    if (thread) {
        result = GetThreadDescriptionAsString(thread);
        CloseHandle(thread);
    }

    return result;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

HWND GetTaskbarForMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        return nullptr;
    }

    HMONITOR taskbarMonitor = (HMONITOR)GetProp(hTaskbarWnd, L"TaskbarMonitor");
    if (taskbarMonitor == monitor) {
        return hTaskbarWnd;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    struct EnumData {
        HMONITOR monitor;
        HWND result;
    } enumData = {monitor, nullptr};

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& data = *reinterpret_cast<EnumData*>(lParam);

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }

            HMONITOR taskbarMonitor =
                (HMONITOR)GetProp(hWnd, L"TaskbarMonitor");
            if (taskbarMonitor != data.monitor) {
                return TRUE;
            }

            data.result = hWnd;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&enumData));

    return enumData.result;
}

void RestoreWindowToDefault(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return;
    }

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return;
    }

    int x = monitorInfo.rcWork.right - (rc.right - rc.left);

    SetWindowPos(hWnd, nullptr, x, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
}

void RestoreSearchMenuToDefault() {
    if (!g_searchMenuWnd || !IsWindow(g_searchMenuWnd)) {
        return;
    }

    SetWindowPos(g_searchMenuWnd, nullptr, g_searchMenuOriginalPos.x,
                 g_searchMenuOriginalPos.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// Must run on a worker thread to avoid COM deadlock when called from the
// taskbar thread.
std::optional<RECT> GetShowDesktopButtonBoundsWorker(HWND hTaskbarWnd) {
    // Initialize COM for this thread.
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Failed to initialize COM: 0x%08X", hrInit);
        return std::nullopt;
    }

    // Ensure COM is uninitialized when we exit.
    struct ComUninit {
        HRESULT hr;
        ~ComUninit() {
            if (SUCCEEDED(hr)) {
                CoUninitialize();
            }
        }
    } comUninit{hrInit};

    winrt::com_ptr<IUIAutomation> automation =
        winrt::try_create_instance<IUIAutomation>(CLSID_CUIAutomation);
    if (!automation) {
        Wh_Log(L"Failed to create IUIAutomation instance");
        return std::nullopt;
    }

    auto logChildrenClassNames =
        [&automation](winrt::com_ptr<IUIAutomationElement>& parent) {
            winrt::com_ptr<IUIAutomationCondition> trueCondition;
            HRESULT hr = automation->CreateTrueCondition(trueCondition.put());
            if (FAILED(hr) || !trueCondition) {
                Wh_Log(L"Failed to create true condition");
                return;
            }

            winrt::com_ptr<IUIAutomationElementArray> children;
            hr = parent->FindAll(TreeScope_Children, trueCondition.get(),
                                 children.put());
            if (FAILED(hr) || !children) {
                Wh_Log(L"Failed to find children");
                return;
            }

            int count = 0;
            hr = children->get_Length(&count);
            if (FAILED(hr)) {
                Wh_Log(L"Failed to get children count");
                return;
            }

            Wh_Log(L"Found %d children:", count);
            for (int i = 0; i < count; i++) {
                winrt::com_ptr<IUIAutomationElement> child;
                hr = children->GetElement(i, child.put());
                if (FAILED(hr) || !child) {
                    continue;
                }

                BSTR className = nullptr;
                hr = child->get_CurrentClassName(&className);
                if (SUCCEEDED(hr) && className) {
                    Wh_Log(L"  [%d] %s", i, className);
                    SysFreeString(className);
                }
            }
        };

    auto findChildByClassName =
        [&automation](
            winrt::com_ptr<IUIAutomationElement>& parent,
            PCWSTR className) -> winrt::com_ptr<IUIAutomationElement> {
        _bstr_t classNameBstr(className);
        VARIANT classNameVariant{};
        classNameVariant.vt = VT_BSTR;
        classNameVariant.bstrVal = classNameBstr.GetBSTR();

        winrt::com_ptr<IUIAutomationCondition> condition;
        HRESULT hr = automation->CreatePropertyCondition(
            UIA_ClassNamePropertyId, classNameVariant, condition.put());
        if (FAILED(hr) || !condition) {
            return nullptr;
        }

        winrt::com_ptr<IUIAutomationElement> child;
        hr =
            parent->FindFirst(TreeScope_Children, condition.get(), child.put());
        if (FAILED(hr)) {
            return nullptr;
        }

        return child;
    };

    // The DesktopWindowContentBridge is a child HWND, not a UI Automation
    // child.
    HWND hBridgeWnd = FindWindowEx(
        hTaskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    if (!hBridgeWnd) {
        Wh_Log(L"Failed to find DesktopWindowContentBridge child window");
        return std::nullopt;
    }

    winrt::com_ptr<IUIAutomationElement> element;
    HRESULT hr = automation->ElementFromHandle(hBridgeWnd, element.put());
    if (FAILED(hr) || !element) {
        Wh_Log(L"Failed to get element from DesktopWindowContentBridge handle");
        return std::nullopt;
    }

    // ShowDesktopButton is a direct child of DesktopWindowContentBridge on the
    // primary taskbar. On secondary taskbars, use OmniButtonRight instead.
    winrt::com_ptr<IUIAutomationElement> targetElement =
        findChildByClassName(element, L"SystemTray.ShowDesktopButton");
    if (!targetElement) {
        targetElement =
            findChildByClassName(element, L"SystemTray.OmniButtonRight");
    }
    if (!targetElement) {
        Wh_Log(
            L"Failed to find ShowDesktopButton or OmniButtonRight, children:");
        logChildrenClassNames(element);
        return std::nullopt;
    }

    // Get bounds from the element.
    RECT boundingRect;
    hr = targetElement->get_CurrentBoundingRectangle(&boundingRect);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to get bounding rectangle");
        return std::nullopt;
    }

    Wh_Log(L"ShowDesktopButton bounds: %d,%d,%d,%d", boundingRect.left,
           boundingRect.top, boundingRect.right, boundingRect.bottom);

    return boundingRect;
}

std::optional<RECT> GetShowDesktopButtonBounds(HWND hTaskbarWnd) {
    // Run on a separate thread to avoid COM deadlock when called from the
    // taskbar thread.
    auto future = std::async(std::launch::async,
                             GetShowDesktopButtonBoundsWorker, hTaskbarWnd);

    // Wait with timeout.
    if (future.wait_for(std::chrono::milliseconds(1000)) ==
        std::future_status::timeout) {
        Wh_Log(L"GetShowDesktopButtonBounds timed out");
        return std::nullopt;
    }

    return future.get();
}

int CalculateAlignedX(
    const RECT& rcWork,
    int width,
    const TrayElementSettings& settings,
    UINT monitorDpi,
    std::optional<RECT> showDesktopButtonBounds = std::nullopt) {
    int x;
    switch (settings.horizontalAlignment) {
        case TrayHorizontalAlignment::right:
            x = rcWork.right - width;
            break;

        case TrayHorizontalAlignment::center:
            x = rcWork.left + (rcWork.right - rcWork.left - width) / 2;
            break;

        case TrayHorizontalAlignment::left:
            x = rcWork.left;
            break;

        case TrayHorizontalAlignment::tray:
            if (showDesktopButtonBounds) {
                // Align flyout's right edge with show desktop button's right
                // edge.
                x = showDesktopButtonBounds->right - width;
            } else {
                // Fallback to right alignment if bounds not available.
                x = rcWork.right - width;
            }
            break;
    }

    return x + MulDiv(settings.horizontalShift, monitorDpi, 96);
}

int CalculateAlignedXForMonitor(HMONITOR monitor,
                                const MONITORINFO& monitorInfo,
                                int width,
                                const TrayElementSettings& settings) {
    std::optional<RECT> showDesktopButtonBounds;
    if (settings.horizontalAlignment == TrayHorizontalAlignment::tray) {
        HWND hTaskbarWnd = GetTaskbarForMonitor(monitor);
        if (hTaskbarWnd) {
            showDesktopButtonBounds = GetShowDesktopButtonBounds(hTaskbarWnd);
        }
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    return CalculateAlignedX(monitorInfo.rcWork, width, settings, monitorDpiX,
                             showDesktopButtonBounds);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }

    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }

    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);

    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if (!processId || !threadId) {
        return original();
    }

    std::wstring processFileName = GetProcessFileName(processId);

    enum class DwmTarget {
        SearchHost,
        ShellExperienceHost,
    };
    DwmTarget target;

    if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    } else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") ==
               0) {
        std::wstring threadDescription =
            GetThreadIdDescriptionAsString(threadId);
        if (threadDescription != L"ActionCenter") {
            return original();
        }

        target = DwmTarget::ShellExperienceHost;
    } else {
        return original();
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }

    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;

    if (target == DwmTarget::SearchHost) {
        int xNew;
        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::windowsDefault:
                xNew = g_searchMenuWnd ? g_searchMenuOriginalPos.x : x;
                break;

            case StartMenuHorizontalAlignment::left:
                xNew = monitorInfo.rcWork.left;
                break;

            case StartMenuHorizontalAlignment::center:
                xNew =
                    monitorInfo.rcWork.left +
                    (monitorInfo.rcWork.right - monitorInfo.rcWork.left - cx) /
                        2;
                break;

            case StartMenuHorizontalAlignment::right:
                xNew = monitorInfo.rcWork.right - cx;
                break;
        }

        xNew += MulDiv(g_settings.startMenu.horizontalShift, monitorDpiX, 96);

        int yNew;
        switch (g_settings.startMenu.verticalAlignment) {
            case StartMenuVerticalAlignment::bottom:
                yNew = g_searchMenuWnd ? monitorInfo.rcWork.bottom - cy : y;
                break;

            case StartMenuVerticalAlignment::center:
                yNew = monitorInfo.rcWork.top + (monitorInfo.rcWork.bottom -
                                                 monitorInfo.rcWork.top - cy) /
                                                    2;
                break;

            case StartMenuVerticalAlignment::top:
                yNew = monitorInfo.rcWork.top;
                break;
        }

        yNew += MulDiv(g_settings.startMenu.verticalShift, monitorDpiY, 96);

        if (xNew == x && yNew == y) {
            return original();
        }

        Wh_Log(L"Adjusting search menu: (%d, %d) -> (%d, %d)", x, y, xNew,
               yNew);

        g_searchMenuWnd = hwnd;
        if (x != g_searchMenuCustomPos.x || y != g_searchMenuCustomPos.y) {
            g_searchMenuOriginalPos = {x, y};
        }
        x = xNew;
        y = yNew;
        g_searchMenuCustomPos = {x, y};
    } else if (target == DwmTarget::ShellExperienceHost) {
        int xNew = CalculateAlignedXForMonitor(monitor, monitorInfo, cx,
                                               g_settings.notificationCenter);

        if (xNew == x) {
            return original();
        }

        Wh_Log(L"Adjusting notification center: %d -> %d", x, xNew);

        x = xNew;
        g_notificationCenterWnd = hwnd;
    }

    SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    return original();
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
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

namespace StartMenuUI {

bool g_applyStylePending;
bool g_inApplyStyle;
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
std::optional<HorizontalAlignment> g_previousHorizontalAlignment;
winrt::event_token g_layoutUpdatedToken;
winrt::event_token g_visibilityChangedToken;

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

void ApplyStyle();

void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor) {
    FrameworkElement startSizingFrame =
        FindChildByClassName(content, L"StartDocked.StartSizingFrame");
    if (!startSizingFrame) {
        Wh_Log(L"Failed to find StartDocked.StartSizingFrame");
        return;
    }

    Wh_Log(L"Invalidating measure");
    startSizingFrame.InvalidateMeasure();

    if (!g_unloading) {
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        double canvasWidth = content.ActualWidth();
        double canvasHeight = content.ActualHeight();

        constexpr int kStartMenuMargin = 12;

        double newTop;
        switch (g_settings.startMenu.verticalAlignment) {
            case StartMenuVerticalAlignment::top:
                newTop = kStartMenuMargin;
                break;

            case StartMenuVerticalAlignment::center:
                newTop = (canvasHeight - startSizingFrame.ActualHeight()) / 2;
                break;

            case StartMenuVerticalAlignment::bottom:
                newTop = canvasHeight - startSizingFrame.ActualHeight() -
                         kStartMenuMargin;
                break;
        }

        double newLeft;
        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::windowsDefault:
                newLeft = std::numeric_limits<double>::quiet_NaN();
                break;

            case StartMenuHorizontalAlignment::left:
                newLeft = kStartMenuMargin;
                break;

            case StartMenuHorizontalAlignment::center:
                newLeft = (canvasWidth - startSizingFrame.ActualWidth()) / 2;
                break;

            case StartMenuHorizontalAlignment::right:
                newLeft = canvasWidth - startSizingFrame.ActualWidth() -
                          kStartMenuMargin;
                break;
        }

        newTop += g_settings.startMenu.verticalShift;
        if (g_settings.startMenu.horizontalShift) {
            if (std::isnan(newLeft)) {
                newLeft = Controls::Canvas::GetLeft(startSizingFrame);
            }

            newLeft += g_settings.startMenu.horizontalShift;
        }

        Wh_Log(L"Setting Canvas.Top to %f, Canvas.Left to %f", newTop, newLeft);
        Controls::Canvas::SetTop(startSizingFrame, newTop);
        if (!std::isnan(newLeft)) {
            Controls::Canvas::SetLeft(startSizingFrame, newLeft);
        }

        // Subscribe to Canvas.Top and Canvas.Left property changes to apply
        // custom styles right when that happens. Without it, the start menu may
        // end up truncated. A simple reproduction is to open the start menu on
        // different monitors, each with a different resolution/DPI/taskbar
        // side.
        if (!g_startSizingFrameWeakRef.get()) {
            auto startSizingFrameDo = startSizingFrame.as<DependencyObject>();

            g_startSizingFrameWeakRef = startSizingFrameDo;

            g_canvasTopPropertyChangedToken =
                startSizingFrameDo.RegisterPropertyChangedCallback(
                    Controls::Canvas::TopProperty(),
                    [](DependencyObject sender, DependencyProperty property) {
                        double top = Controls::Canvas::GetTop(
                            sender.as<FrameworkElement>());
                        Wh_Log(L"Canvas.Top changed to %f", top);
                        if (!g_inApplyStyle) {
                            ApplyStyle();
                        }
                    });

            g_canvasLeftPropertyChangedToken =
                startSizingFrameDo.RegisterPropertyChangedCallback(
                    Controls::Canvas::LeftProperty(),
                    [](DependencyObject sender, DependencyProperty property) {
                        double left = Controls::Canvas::GetLeft(
                            sender.as<FrameworkElement>());
                        Wh_Log(L"Canvas.Left changed to %f", left);
                        if (!g_inApplyStyle) {
                            ApplyStyle();
                        }
                    });
        }
    }
}

void ApplyStyleRedesignedStartMenu(FrameworkElement content) {
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }

    auto margin = frameRoot.Margin();
    auto marginVertical = margin.Top + margin.Bottom;

    auto startMenuAlignment = g_unloading
                                  ? StartMenuVerticalAlignment::bottom
                                  : g_settings.startMenu.verticalAlignment;
    switch (startMenuAlignment) {
        case StartMenuVerticalAlignment::top:
            frameRoot.VerticalAlignment(VerticalAlignment::Top);
            margin.Top = 0;
            margin.Bottom = marginVertical;
            break;

        case StartMenuVerticalAlignment::center:
            frameRoot.VerticalAlignment(VerticalAlignment::Center);
            margin.Top = marginVertical / 2;
            margin.Bottom = marginVertical / 2;
            break;

        case StartMenuVerticalAlignment::bottom:
            frameRoot.VerticalAlignment(VerticalAlignment::Bottom);
            margin.Top = marginVertical;
            margin.Bottom = 0;
            break;
    }

    if (g_unloading) {
        frameRoot.HorizontalAlignment(g_previousHorizontalAlignment.value_or(
            HorizontalAlignment::Center));
    } else {
        if (g_settings.startMenu.horizontalAlignment ==
                StartMenuHorizontalAlignment::windowsDefault &&
            g_previousHorizontalAlignment) {
            frameRoot.HorizontalAlignment(
                g_previousHorizontalAlignment.value());
        }

        if (g_settings.startMenu.horizontalAlignment !=
                StartMenuHorizontalAlignment::windowsDefault &&
            !g_previousHorizontalAlignment) {
            g_previousHorizontalAlignment = frameRoot.HorizontalAlignment();
        }

        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::windowsDefault:
                break;

            case StartMenuHorizontalAlignment::left:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Left);
                break;

            case StartMenuHorizontalAlignment::center:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Center);
                break;

            case StartMenuHorizontalAlignment::right:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Right);
                break;
        }
    }

    frameRoot.Margin(margin);

    Media::TranslateTransform offsetTransform;
    if (!g_unloading) {
        offsetTransform.X(g_settings.startMenu.horizontalShift);
        offsetTransform.Y(g_settings.startMenu.verticalShift);
    }

    frameRoot.RenderTransform(offsetTransform);
}

void ApplyStyle() {
    g_inApplyStyle = true;

    HWND coreWnd = GetCoreWnd();
    HMONITOR monitor = MonitorFromWindow(coreWnd, MONITOR_DEFAULTTONEAREST);

    Wh_Log(L"Applying Start menu style for monitor %p", monitor);

    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();

    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());

    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content, monitor);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }

    g_inApplyStyle = false;
}

void Init() {
    if (g_layoutUpdatedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (!g_visibilityChangedToken) {
        g_visibilityChangedToken = window.VisibilityChanged(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Windows::UI::Core::VisibilityChangedEventArgs const&
                   args) {
                Wh_Log(L"Window visibility changed: %d", args.Visible());
                if (args.Visible()) {
                    g_applyStylePending = true;
                }
            });
    }

    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }

    auto content = contentUI.as<FrameworkElement>();
    g_layoutUpdatedToken = content.LayoutUpdated(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) {
            if (g_applyStylePending) {
                g_applyStylePending = false;
                ApplyStyle();
            }
        });

    ApplyStyle();
}

void Uninit() {
    if (!g_layoutUpdatedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (g_visibilityChangedToken) {
        window.VisibilityChanged(g_visibilityChangedToken);
        g_visibilityChangedToken = {};
    }

    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }

    auto content = contentUI.as<FrameworkElement>();
    content.LayoutUpdated(g_layoutUpdatedToken);
    g_layoutUpdatedToken = {};

    auto startSizingFrameDo = g_startSizingFrameWeakRef.get();
    if (startSizingFrameDo) {
        if (g_canvasTopPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::TopProperty(),
                g_canvasTopPropertyChangedToken);
            g_canvasTopPropertyChangedToken = 0;
        }

        if (g_canvasLeftPropertyChangedToken) {
            startSizingFrameDo.UnregisterPropertyChangedCallback(
                Controls::Canvas::LeftProperty(),
                g_canvasLeftPropertyChangedToken);
            g_canvasLeftPropertyChangedToken = 0;
        }
    }

    g_startSizingFrameWeakRef = nullptr;

    ApplyStyle();
}

void SettingsChanged() {
    ApplyStyle();
}

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t RoGetActivationFactory_Original;
HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;

    if (isInHook) {
        return RoGetActivationFactory_Original(activatableClassId, iid,
                                               factory);
    }

    isInHook = true;

    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    HRESULT ret =
        RoGetActivationFactory_Original(activatableClassId, iid, factory);

    isInHook = false;

    return ret;
}

}  // namespace StartMenuUI

namespace CoreWindowUI {

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

bool IsTargetCoreWindow(HWND hWnd) {
    DWORD threadId = 0;
    DWORD processId = 0;
    if (!hWnd || !(threadId = GetWindowThreadProcessId(hWnd, &processId)) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return false;
    }

    if (g_target == Target::ShellHost) {
        if (_wcsicmp(szClassName, L"ControlCenterWindow") != 0) {
            return false;
        }
    } else {
        if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
            return false;
        }
    }

    if (g_target == Target::ShellExperienceHost) {
        std::wstring threadDescription =
            GetThreadIdDescriptionAsString(threadId);

        Wh_Log(L"%s", threadDescription.c_str());

        if (threadDescription != L"QuickActions") {
            return false;
        }
    }

    return true;
}

std::vector<HWND> GetCoreWindows() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            if (IsTargetCoreWindow(hWnd)) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void AdjustCoreWindowPos(int* x, int* y, int width, int height) {
    RECT rc{
        .left = *x,
        .top = *y,
        .right = *x + width,
        .bottom = *y + height,
    };
    HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    if (g_unloading) {
        *x = monitorInfo.rcWork.right - width;
        return;
    }

    *x = CalculateAlignedXForMonitor(monitor, monitorInfo, width,
                                     g_settings.actionCenter);
}

void ApplySettings() {
    for (HWND hCoreWnd : GetCoreWindows()) {
        Wh_Log(L"Adjusting core window %08X", (DWORD)(ULONG_PTR)hCoreWnd);

        RECT rc;
        if (!GetWindowRect(hCoreWnd, &rc)) {
            continue;
        }

        int x = rc.left;
        int y = rc.top;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        AdjustCoreWindowPos(&x, &y, cx, cy);

        SetWindowPos_Original(hCoreWnd, nullptr, x, y, cx, cy,
                              SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [=]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (!IsTargetCoreWindow(hWnd)) {
        return original();
    }

    Wh_Log(L"%08X %08X", (DWORD)(ULONG_PTR)hWnd, uFlags);

    if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return original();
    }

    RECT rc{};
    GetWindowRect(hWnd, &rc);

    // Window is being moved, then the size is adjusted. Make the position
    // adjusted too.
    if (uFlags & SWP_NOMOVE) {
        uFlags &= ~SWP_NOMOVE;
        X = rc.left;
        Y = rc.top;
    }

    int width;
    int height;
    if (uFlags & SWP_NOSIZE) {
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;
    } else {
        width = cx;
        height = cy;
    }

    if (!(uFlags & SWP_NOMOVE)) {
        AdjustCoreWindowPos(&X, &Y, width, height);
    }

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

}  // namespace CoreWindowUI

void LoadSettings() {
    // Notification center settings.
    PCWSTR notificationCenterHorizontalAlignment =
        Wh_GetStringSetting(L"notificationCenter.horizontalAlignment");
    g_settings.notificationCenter.horizontalAlignment =
        TrayHorizontalAlignment::right;
    if (wcscmp(notificationCenterHorizontalAlignment, L"center") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            TrayHorizontalAlignment::center;
    } else if (wcscmp(notificationCenterHorizontalAlignment, L"left") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            TrayHorizontalAlignment::left;
    } else if (wcscmp(notificationCenterHorizontalAlignment, L"tray") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            TrayHorizontalAlignment::tray;
    }
    Wh_FreeStringSetting(notificationCenterHorizontalAlignment);

    g_settings.notificationCenter.horizontalShift =
        Wh_GetIntSetting(L"notificationCenter.horizontalShift");

    // Action center settings.
    PCWSTR actionCenterHorizontalAlignment =
        Wh_GetStringSetting(L"actionCenter.horizontalAlignment");
    if (wcscmp(actionCenterHorizontalAlignment, L"same") == 0) {
        // Use Notification Center settings.
        g_settings.actionCenter = g_settings.notificationCenter;
    } else {
        g_settings.actionCenter.horizontalAlignment =
            TrayHorizontalAlignment::right;
        if (wcscmp(actionCenterHorizontalAlignment, L"center") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                TrayHorizontalAlignment::center;
        } else if (wcscmp(actionCenterHorizontalAlignment, L"left") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                TrayHorizontalAlignment::left;
        } else if (wcscmp(actionCenterHorizontalAlignment, L"tray") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                TrayHorizontalAlignment::tray;
        }

        g_settings.actionCenter.horizontalShift =
            Wh_GetIntSetting(L"actionCenter.horizontalShift");
    }
    Wh_FreeStringSetting(actionCenterHorizontalAlignment);

    // Start menu settings.
    PCWSTR startMenuHorizontalAlignment =
        Wh_GetStringSetting(L"startMenu.horizontalAlignment");
    g_settings.startMenu.horizontalAlignment =
        StartMenuHorizontalAlignment::windowsDefault;
    if (wcscmp(startMenuHorizontalAlignment, L"left") == 0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::left;
    } else if (wcscmp(startMenuHorizontalAlignment, L"center") == 0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::center;
    } else if (wcscmp(startMenuHorizontalAlignment, L"right") == 0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::right;
    }
    Wh_FreeStringSetting(startMenuHorizontalAlignment);

    g_settings.startMenu.horizontalShift =
        Wh_GetIntSetting(L"startMenu.horizontalShift");

    PCWSTR startMenuVerticalAlignment =
        Wh_GetStringSetting(L"startMenu.verticalAlignment");
    g_settings.startMenu.verticalAlignment = StartMenuVerticalAlignment::bottom;
    if (wcscmp(startMenuVerticalAlignment, L"center") == 0) {
        g_settings.startMenu.verticalAlignment =
            StartMenuVerticalAlignment::center;
    } else if (wcscmp(startMenuVerticalAlignment, L"top") == 0) {
        g_settings.startMenu.verticalAlignment =
            StartMenuVerticalAlignment::top;
    }
    Wh_FreeStringSetting(startMenuVerticalAlignment);

    g_settings.startMenu.verticalShift =
        Wh_GetIntSetting(L"startMenu.verticalShift");
}

bool NeedsToBeLoaded() {
    switch (g_target) {
        case Target::Explorer:
            return true;

        case Target::StartMenuExperienceHost:
            return g_settings.startMenu.horizontalAlignment !=
                       StartMenuHorizontalAlignment::windowsDefault ||
                   g_settings.startMenu.horizontalShift != 0 ||
                   g_settings.startMenu.verticalAlignment !=
                       StartMenuVerticalAlignment::bottom ||
                   g_settings.startMenu.verticalShift != 0;

        case Target::ShellExperienceHost:
        case Target::ShellHost:
            return g_settings.notificationCenter.horizontalAlignment !=
                       TrayHorizontalAlignment::right ||
                   g_settings.notificationCenter.horizontalShift != 0 ||
                   g_settings.actionCenter.horizontalAlignment !=
                       TrayHorizontalAlignment::right ||
                   g_settings.actionCenter.horizontalShift != 0;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_target = Target::Explorer;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            break;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"StartMenuExperienceHost.exe") ==
                    0) {
                    g_target = Target::StartMenuExperienceHost;
                } else if (_wcsicmp(moduleFileName,
                                    L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
    }

    if (!NeedsToBeLoaded()) {
        Wh_Log(L"No need to be loaded due to settings");
        return FALSE;
    }

    if (g_target == Target::StartMenuExperienceHost) {
        HMODULE winrtModule =
            GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
        auto pRoGetActivationFactory =
            (decltype(&RoGetActivationFactory))GetProcAddress(
                winrtModule, "RoGetActivationFactory");
        WindhawkUtils::SetFunctionHook(
            pRoGetActivationFactory, StartMenuUI::RoGetActivationFactory_Hook,
            &StartMenuUI::RoGetActivationFactory_Original);

        return TRUE;
    }

    if (g_target == Target::ShellExperienceHost ||
        g_target == Target::ShellHost) {
        WindhawkUtils::SetFunctionHook(SetWindowPos,
                                       CoreWindowUI::SetWindowPos_Hook,
                                       &CoreWindowUI::SetWindowPos_Original);
        return TRUE;
    }

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::SetFunctionHook(pDwmSetWindowAttribute,
                                           DwmSetWindowAttribute_Hook,
                                           &DwmSetWindowAttribute_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Initializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Init(); }, nullptr);
        }
    } else if (g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        RestoreWindowToDefault(g_notificationCenterWnd);
        RestoreSearchMenuToDefault();
    } else if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Uninitializing - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::Uninit(); }, nullptr);
        }
    } else if (g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    if (!NeedsToBeLoaded()) {
        Wh_Log(L"No need to be loaded due to settings");
        return FALSE;
    }

    if (g_target == Target::StartMenuExperienceHost) {
        HWND hCoreWnd = StartMenuUI::GetCoreWnd();
        if (hCoreWnd) {
            Wh_Log(L"Applying settings - Found core window");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) { StartMenuUI::SettingsChanged(); },
                nullptr);
        }
    } else if (g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }

    return TRUE;
}

// ==WindhawkMod==
// @id              shell-flyout-positions
// @name            Shell Flyout Positions
// @description     Customize the position of the Notification Center, Action Center, and Start menu on Windows 11
// @version         1.3
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
align it to the left, center, or right of the screen, or to the left, center, or
right of the Start button, and vertically to the top, center, or bottom. Pixel
shift options allow fine-tuning in both directions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- notificationCenter:
  - horizontalAlignment: windowsDefault
    $name: Horizontal alignment
    $options:
    - windowsDefault: Windows default
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
    - windowsDefault: Windows default
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
    - startButtonLeft: Left-aligned to the Start button
    - startButtonCenter: Center-aligned to the Start button
    - startButtonRight: Right-aligned to the Start button
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  - verticalAlignment: windowsDefault
    $name: Vertical alignment
    $options:
    - windowsDefault: Windows default
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
#include <winrt/Windows.UI.Xaml.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <future>
#include <limits>
#include <optional>
#include <string>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

enum class TrayHorizontalAlignment {
    windowsDefault,
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
    startButtonLeft,
    startButtonCenter,
    startButtonRight,
};

enum class StartMenuVerticalAlignment {
    windowsDefault,
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
LONG g_notificationCenterOriginalX;
LONG g_notificationCenterCustomX = LONG_MAX;
HMONITOR g_notificationCenterMonitor;
HWND g_searchMenuWnd;
LONG g_searchMenuOriginalX;
LONG g_searchMenuOriginalY;
LONG g_searchMenuCustomX = LONG_MAX;
LONG g_searchMenuCustomY = LONG_MAX;

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

bool IsStartMenuOpen() {
    bool open = false;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
                _wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
                return TRUE;
            }

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId)) {
                return TRUE;
            }

            std::wstring processFileName = GetProcessFileName(dwProcessId);
            if (_wcsicmp(processFileName.c_str(),
                         L"StartMenuExperienceHost.exe") != 0) {
                return TRUE;
            }

            // The start menu window stays cloaked while hidden and is uncloaked
            // while shown.
            BOOL cloaked = FALSE;
            if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                                sizeof(cloaked))) &&
                !cloaked) {
                *(bool*)lParam = true;
            }

            return FALSE;
        },
        (LPARAM)&open);

    return open;
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

void RestoreNotificationCenterToDefault() {
    if (!g_notificationCenterWnd || !IsWindow(g_notificationCenterWnd)) {
        return;
    }

    // On a settings change only restore when switched back to the Windows
    // default; on unload always restore.
    bool xIsDefault = g_settings.notificationCenter.horizontalAlignment ==
                          TrayHorizontalAlignment::windowsDefault &&
                      g_settings.notificationCenter.horizontalShift == 0;

    if (!g_unloading && !xIsDefault) {
        return;
    }

    HMONITOR currentMonitor =
        MonitorFromWindow(g_notificationCenterWnd, MONITOR_DEFAULTTONEAREST);
    if (!currentMonitor || currentMonitor == g_notificationCenterMonitor) {
        RECT rc;
        if (!GetWindowRect(g_notificationCenterWnd, &rc)) {
            return;
        }

        SetWindowPos(g_notificationCenterWnd, nullptr,
                     g_notificationCenterOriginalX, rc.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    g_notificationCenterWnd = nullptr;
    g_notificationCenterCustomX = LONG_MAX;
    g_notificationCenterMonitor = nullptr;
}

void RestoreSearchMenuToDefault() {
    if (!g_searchMenuWnd || !IsWindow(g_searchMenuWnd)) {
        return;
    }

    // On a settings change only restore the axes switched back to the Windows
    // default; on unload restore every customized axis.
    bool restoreX =
        g_searchMenuCustomX != LONG_MAX &&
        (g_unloading || (g_settings.startMenu.horizontalAlignment ==
                             StartMenuHorizontalAlignment::windowsDefault &&
                         g_settings.startMenu.horizontalShift == 0));
    bool restoreY =
        g_searchMenuCustomY != LONG_MAX &&
        (g_unloading || (g_settings.startMenu.verticalAlignment ==
                             StartMenuVerticalAlignment::windowsDefault &&
                         g_settings.startMenu.verticalShift == 0));

    if (!restoreX && !restoreY) {
        return;
    }

    RECT rc;
    if (!GetWindowRect(g_searchMenuWnd, &rc)) {
        return;
    }

    int x = restoreX ? g_searchMenuOriginalX : rc.left;
    int y = restoreY ? g_searchMenuOriginalY : rc.top;

    SetWindowPos(g_searchMenuWnd, nullptr, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    if (restoreX) {
        g_searchMenuCustomX = LONG_MAX;
    }
    if (restoreY) {
        g_searchMenuCustomY = LONG_MAX;
    }
    if (g_searchMenuCustomX == LONG_MAX && g_searchMenuCustomY == LONG_MAX) {
        g_searchMenuWnd = nullptr;
    }
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

// Must run on a worker thread to avoid COM deadlock when called from the
// taskbar thread.
std::optional<RECT> GetStartButtonBoundsWorker(HWND hTaskbarWnd) {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Failed to initialize COM: 0x%08X", hrInit);
        return std::nullopt;
    }

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

    _bstr_t automationIdBstr(L"StartButton");
    VARIANT automationIdVariant{};
    automationIdVariant.vt = VT_BSTR;
    automationIdVariant.bstrVal = automationIdBstr.GetBSTR();

    winrt::com_ptr<IUIAutomationCondition> condition;
    HRESULT hr = automation->CreatePropertyCondition(
        UIA_AutomationIdPropertyId, automationIdVariant, condition.put());
    if (FAILED(hr) || !condition) {
        Wh_Log(L"Failed to create condition");
        return std::nullopt;
    }

    winrt::com_ptr<IUIAutomationElement> taskbarElement;
    hr = automation->ElementFromHandle(hTaskbarWnd, taskbarElement.put());
    if (FAILED(hr) || !taskbarElement) {
        Wh_Log(L"Failed to get element from taskbar handle");
        return std::nullopt;
    }

    winrt::com_ptr<IUIAutomationElement> startButton;
    hr = taskbarElement->FindFirst(TreeScope_Descendants, condition.get(),
                                   startButton.put());
    if (FAILED(hr) || !startButton) {
        Wh_Log(L"Failed to find StartButton");
        return std::nullopt;
    }

    RECT boundingRect;
    hr = startButton->get_CurrentBoundingRectangle(&boundingRect);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to get bounding rectangle");
        return std::nullopt;
    }

    Wh_Log(L"StartButton bounds: %d,%d,%d,%d", boundingRect.left,
           boundingRect.top, boundingRect.right, boundingRect.bottom);

    return boundingRect;
}

std::optional<RECT> GetStartButtonBounds(HWND hTaskbarWnd) {
    // Run on a separate thread to avoid COM deadlock when called from the
    // taskbar thread.
    auto future =
        std::async(std::launch::async, GetStartButtonBoundsWorker, hTaskbarWnd);

    // Wait with timeout.
    if (future.wait_for(std::chrono::milliseconds(1000)) ==
        std::future_status::timeout) {
        Wh_Log(L"GetStartButtonBounds timed out");
        return std::nullopt;
    }

    return future.get();
}

std::optional<RECT> GetStartButtonBoundsForMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = GetTaskbarForMonitor(monitor);
    if (!hTaskbarWnd) {
        return std::nullopt;
    }

    auto boundingRect = GetStartButtonBounds(hTaskbarWnd);
    if (!boundingRect) {
        return std::nullopt;
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    // Extend the bounding rectangle to account for the Start menu shadows.
    boundingRect->left -= MulDiv(10, monitorDpiX, 96);
    boundingRect->right += MulDiv(10, monitorDpiX, 96);
    return boundingRect;
}

int CalculateAlignedX(
    const RECT& rcWork,
    int width,
    const TrayElementSettings& settings,
    UINT monitorDpi,
    std::optional<RECT> showDesktopButtonBounds = std::nullopt) {
    int x;
    switch (settings.horizontalAlignment) {
        case TrayHorizontalAlignment::windowsDefault:
            Wh_Log(L"Error: Called with windowsDefault alignment");
            return 0;

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
        if (!IsStartMenuOpen()) {
            // Win+S or the search bar on the taskbar cause the search menu to
            // be repositioned. Don't customize it and restore the original
            // position.
            g_searchMenuWnd = nullptr;
            g_searchMenuCustomX = LONG_MAX;
            g_searchMenuCustomY = LONG_MAX;
            return original();
        }

        int xNew;
        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::windowsDefault:
                xNew =
                    g_searchMenuCustomX != LONG_MAX ? g_searchMenuOriginalX : x;
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

            case StartMenuHorizontalAlignment::startButtonLeft: {
                std::optional<RECT> startButtonBounds =
                    GetStartButtonBoundsForMonitor(monitor);
                xNew = startButtonBounds ? startButtonBounds->left : x;
                break;
            }

            case StartMenuHorizontalAlignment::startButtonCenter: {
                std::optional<RECT> startButtonBounds =
                    GetStartButtonBoundsForMonitor(monitor);
                xNew =
                    startButtonBounds
                        ? (startButtonBounds->left + startButtonBounds->right) /
                                  2 -
                              cx / 2
                        : x;
                break;
            }

            case StartMenuHorizontalAlignment::startButtonRight: {
                std::optional<RECT> startButtonBounds =
                    GetStartButtonBoundsForMonitor(monitor);
                xNew = startButtonBounds ? startButtonBounds->right - cx : x;
                break;
            }
        }

        xNew += MulDiv(g_settings.startMenu.horizontalShift, monitorDpiX, 96);

        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::startButtonLeft:
            case StartMenuHorizontalAlignment::startButtonCenter:
            case StartMenuHorizontalAlignment::startButtonRight:
                // Keep the search menu within the monitor work area.
                xNew = std::min(
                    xNew, static_cast<int>(monitorInfo.rcWork.right - cx));
                xNew =
                    std::max(xNew, static_cast<int>(monitorInfo.rcWork.left));
                break;

            default:
                break;
        }

        int yNew;
        switch (g_settings.startMenu.verticalAlignment) {
            case StartMenuVerticalAlignment::windowsDefault:
                yNew =
                    g_searchMenuCustomY != LONG_MAX ? g_searchMenuOriginalY : y;
                break;

            case StartMenuVerticalAlignment::bottom:
                yNew = monitorInfo.rcWork.bottom - cy;
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

        // Track each axis independently. An axis is at its default only when
        // its alignment is windowsDefault and its shift is zero; otherwise
        // remember the original position so it can be restored later, and once
        // back to default, forget it.
        bool xIsDefault = g_settings.startMenu.horizontalAlignment ==
                              StartMenuHorizontalAlignment::windowsDefault &&
                          g_settings.startMenu.horizontalShift == 0;
        bool yIsDefault = g_settings.startMenu.verticalAlignment ==
                              StartMenuVerticalAlignment::windowsDefault &&
                          g_settings.startMenu.verticalShift == 0;

        if (xIsDefault) {
            g_searchMenuCustomX = LONG_MAX;
        } else {
            if (x != g_searchMenuCustomX) {
                g_searchMenuOriginalX = x;
            }
            g_searchMenuCustomX = xNew;
        }

        if (yIsDefault) {
            g_searchMenuCustomY = LONG_MAX;
        } else {
            if (y != g_searchMenuCustomY) {
                g_searchMenuOriginalY = y;
            }
            g_searchMenuCustomY = yNew;
        }

        g_searchMenuWnd =
            (g_searchMenuCustomX != LONG_MAX || g_searchMenuCustomY != LONG_MAX)
                ? hwnd
                : nullptr;

        x = xNew;
        y = yNew;
    } else if (target == DwmTarget::ShellExperienceHost) {
        bool xIsDefault = g_settings.notificationCenter.horizontalAlignment ==
                              TrayHorizontalAlignment::windowsDefault &&
                          g_settings.notificationCenter.horizontalShift == 0;

        if (xIsDefault) {
            // Restoring to the default is handled in Wh_ModSettingsChanged.
            g_notificationCenterCustomX = LONG_MAX;
            return original();
        }

        int xNew;
        if (g_settings.notificationCenter.horizontalAlignment ==
            TrayHorizontalAlignment::windowsDefault) {
            xNew = (g_notificationCenterCustomX != LONG_MAX &&
                    monitor == g_notificationCenterMonitor)
                       ? g_notificationCenterOriginalX
                       : x;
            xNew += MulDiv(g_settings.notificationCenter.horizontalShift,
                           monitorDpiX, 96);
        } else {
            xNew = CalculateAlignedXForMonitor(monitor, monitorInfo, cx,
                                               g_settings.notificationCenter);
        }

        if (xNew == x) {
            return original();
        }

        Wh_Log(L"Adjusting notification center: %d -> %d", x, xNew);

        g_notificationCenterWnd = hwnd;
        if (x != g_notificationCenterCustomX) {
            g_notificationCenterOriginalX = x;
            g_notificationCenterMonitor = monitor;
        }
        x = xNew;
        g_notificationCenterCustomX = x;
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

bool g_inApplyStyle;
winrt::weak_ref<DependencyObject> g_startSizingFrameWeakRef;
int64_t g_canvasTopPropertyChangedToken;
int64_t g_canvasLeftPropertyChangedToken;
std::optional<HorizontalAlignment> g_previousHorizontalAlignment;
std::optional<VerticalAlignment> g_previousVerticalAlignment;
std::optional<Thickness> g_previousMargin;
winrt::weak_ref<DependencyObject> g_frameRootWeakRef;
int64_t g_verticalAlignmentPropertyChangedToken;
int64_t g_horizontalAlignmentPropertyChangedToken;
winrt::event_token g_frameRootSizeChangedToken;
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

        double canvasWidth = content.ActualWidth();
        double canvasHeight = content.ActualHeight();

        constexpr int kStartMenuMargin = 12;

        double newTop;
        switch (g_settings.startMenu.verticalAlignment) {
            case StartMenuVerticalAlignment::windowsDefault:
                newTop = std::numeric_limits<double>::quiet_NaN();
                break;

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

            case StartMenuHorizontalAlignment::startButtonLeft:
            case StartMenuHorizontalAlignment::startButtonCenter:
            case StartMenuHorizontalAlignment::startButtonRight: {
                newLeft = std::numeric_limits<double>::quiet_NaN();

                std::optional<RECT> startButtonBounds =
                    GetStartButtonBoundsForMonitor(monitor);
                if (startButtonBounds) {
                    UINT monitorDpiX = 96;
                    UINT monitorDpiY = 96;
                    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                     &monitorDpiY);

                    double scale = monitorDpiX / 96.0;
                    double startButtonLeft =
                        (startButtonBounds->left - monitorInfo.rcWork.left) /
                        scale;
                    double startButtonRight =
                        (startButtonBounds->right - monitorInfo.rcWork.left) /
                        scale;

                    switch (g_settings.startMenu.horizontalAlignment) {
                        case StartMenuHorizontalAlignment::startButtonLeft:
                            newLeft = startButtonLeft;
                            break;

                        case StartMenuHorizontalAlignment::startButtonCenter:
                            newLeft = (startButtonLeft + startButtonRight) / 2 -
                                      startSizingFrame.ActualWidth() / 2;
                            break;

                        case StartMenuHorizontalAlignment::startButtonRight:
                            newLeft = startButtonRight -
                                      startSizingFrame.ActualWidth();
                            break;

                        default:
                            break;
                    }
                }
                break;
            }
        }

        if (g_settings.startMenu.verticalShift) {
            if (std::isnan(newTop)) {
                newTop = Controls::Canvas::GetTop(startSizingFrame);
            }

            newTop += g_settings.startMenu.verticalShift;
        }
        if (g_settings.startMenu.horizontalShift) {
            if (std::isnan(newLeft)) {
                newLeft = Controls::Canvas::GetLeft(startSizingFrame);
            }

            newLeft += g_settings.startMenu.horizontalShift;
        }

        // Keep the start menu within the monitor work area, which the canvas
        // spans. Use the monitor geometry rather than the canvas size, which
        // may be stale right after the menu moves to a monitor with a different
        // resolution.
        UINT clampDpiX = 96;
        UINT clampDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &clampDpiX, &clampDpiY);
        double clampScale = clampDpiX / 96.0;
        double workWidth =
            (monitorInfo.rcWork.right - monitorInfo.rcWork.left) / clampScale;
        double workHeight =
            (monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / clampScale;

        if (!std::isnan(newTop)) {
            double maxTop = workHeight - startSizingFrame.ActualHeight();
            newTop = std::clamp(newTop, 0.0, maxTop < 0 ? 0.0 : maxTop);
        }
        if (!std::isnan(newLeft)) {
            double maxLeft = workWidth - startSizingFrame.ActualWidth();
            newLeft = std::clamp(newLeft, 0.0, maxLeft < 0 ? 0.0 : maxLeft);
        }

        Wh_Log(L"Setting Canvas.Top to %f, Canvas.Left to %f", newTop, newLeft);
        if (!std::isnan(newTop)) {
            Controls::Canvas::SetTop(startSizingFrame, newTop);
        }
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

void ApplyStyleRedesignedStartMenu(FrameworkElement content, HMONITOR monitor) {
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find Start menu frame root");
        return;
    }

    auto margin = frameRoot.Margin();
    auto marginVertical = margin.Top + margin.Bottom;

    if (g_unloading || g_settings.startMenu.verticalAlignment ==
                           StartMenuVerticalAlignment::windowsDefault) {
        // Restore the alignment and margin saved before the first change, then
        // forget them. If they were never changed, leave them untouched.
        if (g_previousVerticalAlignment) {
            frameRoot.VerticalAlignment(g_previousVerticalAlignment.value());
            g_previousVerticalAlignment = std::nullopt;
        }
        if (g_previousMargin) {
            margin = g_previousMargin.value();
            g_previousMargin = std::nullopt;
        }
    } else {
        if (!g_previousVerticalAlignment) {
            g_previousVerticalAlignment = frameRoot.VerticalAlignment();
        }
        if (!g_previousMargin) {
            g_previousMargin = margin;
        }

        switch (g_settings.startMenu.verticalAlignment) {
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

            case StartMenuVerticalAlignment::windowsDefault:
                break;
        }
    }

    if (g_unloading || g_settings.startMenu.horizontalAlignment ==
                           StartMenuHorizontalAlignment::windowsDefault) {
        // Restore the alignment saved before the first change, then forget it.
        // If it was never changed, leave it untouched.
        if (g_previousHorizontalAlignment) {
            frameRoot.HorizontalAlignment(
                g_previousHorizontalAlignment.value());
            g_previousHorizontalAlignment = std::nullopt;
        }
    } else {
        if (!g_previousHorizontalAlignment) {
            g_previousHorizontalAlignment = frameRoot.HorizontalAlignment();
        }

        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::left:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Left);
                break;

            case StartMenuHorizontalAlignment::center:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Center);
                break;

            case StartMenuHorizontalAlignment::right:
                frameRoot.HorizontalAlignment(HorizontalAlignment::Right);
                break;

            case StartMenuHorizontalAlignment::startButtonLeft:
            case StartMenuHorizontalAlignment::startButtonCenter:
            case StartMenuHorizontalAlignment::startButtonRight:
                // Left alignment anchors the frame's base position to the
                // monitor's left edge, which doesn't depend on the container
                // width. The exact placement is done below via an absolute
                // offset, so it stays correct even when the menu opens on a
                // monitor whose width the container hasn't picked up yet.
                frameRoot.HorizontalAlignment(HorizontalAlignment::Left);
                break;

            case StartMenuHorizontalAlignment::windowsDefault:
                break;
        }
    }

    frameRoot.Margin(margin);

    double horizontalOffset = g_settings.startMenu.horizontalShift;
    if (!g_unloading) {
        switch (g_settings.startMenu.horizontalAlignment) {
            case StartMenuHorizontalAlignment::startButtonLeft:
            case StartMenuHorizontalAlignment::startButtonCenter:
            case StartMenuHorizontalAlignment::startButtonRight: {
                // The frame uses Left alignment, so its base position is the
                // monitor's left edge. Place the frame's matching edge/center
                // at the start button's, using an absolute offset derived from
                // the monitor geometry and the frame's own width. Nothing here
                // depends on the container width, which may be stale right
                // after the menu moves to a monitor with a different
                // resolution.
                std::optional<RECT> startButtonBounds =
                    GetStartButtonBoundsForMonitor(monitor);
                if (!startButtonBounds) {
                    break;
                }

                MONITORINFO monitorInfo{
                    .cbSize = sizeof(MONITORINFO),
                };
                GetMonitorInfo(monitor, &monitorInfo);

                UINT monitorDpiX = 96;
                UINT monitorDpiY = 96;
                GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                 &monitorDpiY);

                double scale = monitorDpiX / 96.0;
                double frameWidth = frameRoot.ActualWidth();
                double startButtonLeft =
                    (startButtonBounds->left - monitorInfo.rcMonitor.left) /
                    scale;
                double startButtonRight =
                    (startButtonBounds->right - monitorInfo.rcMonitor.left) /
                    scale;

                double newLeft;
                switch (g_settings.startMenu.horizontalAlignment) {
                    case StartMenuHorizontalAlignment::startButtonCenter:
                        newLeft = (startButtonLeft + startButtonRight) / 2 -
                                  frameWidth / 2;
                        break;

                    case StartMenuHorizontalAlignment::startButtonRight:
                        newLeft = startButtonRight - frameWidth;
                        break;

                    default:  // startButtonLeft
                        newLeft = startButtonLeft;
                        break;
                }

                newLeft += g_settings.startMenu.horizontalShift;

                // Keep the start menu within the monitor work area, which spans
                // the monitor horizontally.
                double workLeft =
                    (monitorInfo.rcWork.left - monitorInfo.rcMonitor.left) /
                    scale;
                double workRight =
                    (monitorInfo.rcWork.right - monitorInfo.rcMonitor.left) /
                    scale;
                double maxLeft = workRight - frameWidth;
                if (maxLeft < workLeft) {
                    maxLeft = workLeft;
                }
                if (frameWidth > 0) {
                    newLeft = std::clamp(newLeft, workLeft, maxLeft);
                }

                horizontalOffset = newLeft;
                break;
            }

            default:
                // Screen alignment keeps the frame within the container; only a
                // shift can move it out, so clamp the shifted position to the
                // work area, which the container spans.
                if (g_settings.startMenu.horizontalAlignment !=
                    StartMenuHorizontalAlignment::windowsDefault) {
                    double containerWidth = content.ActualWidth();
                    double frameWidth = frameRoot.ActualWidth();
                    if (frameWidth > 0 && frameWidth <= containerWidth) {
                        double alignmentLeft = 0;
                        switch (g_settings.startMenu.horizontalAlignment) {
                            case StartMenuHorizontalAlignment::center:
                                alignmentLeft =
                                    (containerWidth - frameWidth) / 2;
                                break;

                            case StartMenuHorizontalAlignment::right:
                                alignmentLeft = containerWidth - frameWidth;
                                break;

                            default:  // left
                                alignmentLeft = 0;
                                break;
                        }

                        double clampedLeft =
                            std::clamp(alignmentLeft + horizontalOffset, 0.0,
                                       containerWidth - frameWidth);
                        horizontalOffset = clampedLeft - alignmentLeft;
                    }
                }
                break;
        }
    }

    Media::TranslateTransform offsetTransform;
    if (!g_unloading) {
        offsetTransform.X(horizontalOffset);
        offsetTransform.Y(g_settings.startMenu.verticalShift);
    }

    frameRoot.RenderTransform(offsetTransform);

    if (!g_unloading && !g_frameRootWeakRef.get()) {
        auto frameRootDo = frameRoot.as<DependencyObject>();

        g_frameRootWeakRef = frameRootDo;

        g_verticalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().VerticalAlignment();
                    Wh_Log(L"FrameRoot VerticalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });

        g_horizontalAlignmentPropertyChangedToken =
            frameRootDo.RegisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                [](DependencyObject sender, DependencyProperty property) {
                    auto alignment =
                        sender.as<FrameworkElement>().HorizontalAlignment();
                    Wh_Log(L"FrameRoot HorizontalAlignment changed to %d",
                           static_cast<int>(alignment));
                    if (!g_inApplyStyle) {
                        ApplyStyle();
                    }
                });

        // Re-apply when the frame is resized. When the menu opens on a monitor
        // with a different DPI, its rasterization scale (and therefore the
        // frame and container sizes) updates after the first apply, which would
        // otherwise leave the start button alignment offset by a stale amount.
        g_frameRootSizeChangedToken = frameRoot.SizeChanged(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               SizeChangedEventArgs const& args) {
                Wh_Log(L"FrameRoot size changed to %fx%f", args.NewSize().Width,
                       args.NewSize().Height);
                if (!g_inApplyStyle) {
                    ApplyStyle();
                }
            });
    }
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
        ApplyStyleRedesignedStartMenu(content, monitor);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }

    g_inApplyStyle = false;
}

void Init() {
    if (g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    g_visibilityChangedToken = window.VisibilityChanged(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args) {
            Wh_Log(L"Window visibility changed: %d", args.Visible());
            if (args.Visible()) {
                ApplyStyle();
            }
        });

    ApplyStyle();
}

void Uninit() {
    if (!g_visibilityChangedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    window.VisibilityChanged(g_visibilityChangedToken);
    g_visibilityChangedToken = {};

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

    auto frameRootDo = g_frameRootWeakRef.get();
    if (frameRootDo) {
        if (g_verticalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::VerticalAlignmentProperty(),
                g_verticalAlignmentPropertyChangedToken);
            g_verticalAlignmentPropertyChangedToken = 0;
        }

        if (g_horizontalAlignmentPropertyChangedToken) {
            frameRootDo.UnregisterPropertyChangedCallback(
                FrameworkElement::HorizontalAlignmentProperty(),
                g_horizontalAlignmentPropertyChangedToken);
            g_horizontalAlignmentPropertyChangedToken = 0;
        }

        if (g_frameRootSizeChangedToken) {
            if (auto frameRoot = frameRootDo.try_as<FrameworkElement>()) {
                frameRoot.SizeChanged(g_frameRootSizeChangedToken);
            }
            g_frameRootSizeChangedToken = {};
        }
    }

    g_frameRootWeakRef = nullptr;

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
    bool xIsDefault = g_settings.actionCenter.horizontalAlignment ==
                          TrayHorizontalAlignment::windowsDefault &&
                      g_settings.actionCenter.horizontalShift == 0;

    // The system will reposition it on its own.
    if (g_unloading || xIsDefault) {
        return;
    }

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

    if (g_settings.actionCenter.horizontalAlignment ==
        TrayHorizontalAlignment::windowsDefault) {
        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
        *x += MulDiv(g_settings.actionCenter.horizontalShift, monitorDpiX, 96);
    } else {
        *x = CalculateAlignedXForMonitor(monitor, monitorInfo, width,
                                         g_settings.actionCenter);
    }
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
        TrayHorizontalAlignment::windowsDefault;
    if (wcscmp(notificationCenterHorizontalAlignment, L"right") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            TrayHorizontalAlignment::right;
    } else if (wcscmp(notificationCenterHorizontalAlignment, L"center") == 0) {
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
            TrayHorizontalAlignment::windowsDefault;
        if (wcscmp(actionCenterHorizontalAlignment, L"right") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                TrayHorizontalAlignment::right;
        } else if (wcscmp(actionCenterHorizontalAlignment, L"center") == 0) {
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
    } else if (wcscmp(startMenuHorizontalAlignment, L"startButtonLeft") == 0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::startButtonLeft;
    } else if (wcscmp(startMenuHorizontalAlignment, L"startButtonCenter") ==
               0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::startButtonCenter;
    } else if (wcscmp(startMenuHorizontalAlignment, L"startButtonRight") == 0) {
        g_settings.startMenu.horizontalAlignment =
            StartMenuHorizontalAlignment::startButtonRight;
    }
    Wh_FreeStringSetting(startMenuHorizontalAlignment);

    g_settings.startMenu.horizontalShift =
        Wh_GetIntSetting(L"startMenu.horizontalShift");

    PCWSTR startMenuVerticalAlignment =
        Wh_GetStringSetting(L"startMenu.verticalAlignment");
    g_settings.startMenu.verticalAlignment =
        StartMenuVerticalAlignment::windowsDefault;
    if (wcscmp(startMenuVerticalAlignment, L"bottom") == 0) {
        g_settings.startMenu.verticalAlignment =
            StartMenuVerticalAlignment::bottom;
    } else if (wcscmp(startMenuVerticalAlignment, L"center") == 0) {
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
                       StartMenuVerticalAlignment::windowsDefault ||
                   g_settings.startMenu.verticalShift != 0;

        case Target::ShellExperienceHost:
        case Target::ShellHost:
            return g_settings.notificationCenter.horizontalAlignment !=
                       TrayHorizontalAlignment::windowsDefault ||
                   g_settings.notificationCenter.horizontalShift != 0 ||
                   g_settings.actionCenter.horizontalAlignment !=
                       TrayHorizontalAlignment::windowsDefault ||
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
            return FALSE;

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
                return FALSE;
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
        RestoreNotificationCenterToDefault();
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

    if (g_target == Target::Explorer) {
        // Restore right away instead of waiting for the next time the flyouts
        // are shown.
        RestoreNotificationCenterToDefault();
        RestoreSearchMenuToDefault();
    } else if (g_target == Target::StartMenuExperienceHost) {
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

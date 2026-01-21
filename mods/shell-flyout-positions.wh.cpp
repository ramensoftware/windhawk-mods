// ==WindhawkMod==
// @id              shell-flyout-positions
// @name            Shell Flyout Positions
// @description     Customize the horizontal position of the Notification Center and Action Center on Windows 11
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lshcore
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

Customize the horizontal position of the Notification Center (Win+N) and Action
Center (Win+A) on Windows 11.

By default, Windows 11 displays the Notification Center and Action Center (Quick
Settings) anchored to the right side of the screen. This mod allows you to
change their horizontal alignment to the left or center, with an optional pixel
shift for fine-tuning.

This is especially useful when using custom taskbar themes that relocate the
system tray area, as you can reposition these flyouts to match your custom
layout.

![Screenshot](https://i.imgur.com/ezxArY1.png)
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
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // must come before uiautomation.h

#include <comdef.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <winrt/base.h>

#include <atomic>
#include <future>
#include <optional>
#include <string>
#include <vector>

enum class HorizontalAlignmentSetting {
    right,
    center,
    left,
    tray,
};

struct ElementSettings {
    HorizontalAlignmentSetting horizontalAlignment;
    int horizontalShift;
};

struct {
    ElementSettings notificationCenter;
    ElementSettings actionCenter;
} g_settings;

enum class Target {
    Explorer,
    ShellExperienceHost,
    ShellHost,  // Win11 24H2.
};

Target g_target;

std::atomic<bool> g_unloading;
HWND g_lastAdjustedNotificationCenterWindow;

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
    const ElementSettings& settings,
    UINT monitorDpi,
    std::optional<RECT> showDesktopButtonBounds = std::nullopt) {
    int x;
    switch (settings.horizontalAlignment) {
        case HorizontalAlignmentSetting::right:
            x = rcWork.right - width;
            break;

        case HorizontalAlignmentSetting::center:
            x = rcWork.left + (rcWork.right - rcWork.left - width) / 2;
            break;

        case HorizontalAlignmentSetting::left:
            x = rcWork.left;
            break;

        case HorizontalAlignmentSetting::tray:
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
                                int width,
                                const ElementSettings& settings) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    std::optional<RECT> showDesktopButtonBounds;
    if (settings.horizontalAlignment == HorizontalAlignmentSetting::tray) {
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
    if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") != 0) {
        return original();
    }

    std::wstring threadDescription = GetThreadIdDescriptionAsString(threadId);
    if (threadDescription != L"ActionCenter") {
        return original();
    }

    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }

    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    int xNew =
        CalculateAlignedXForMonitor(monitor, cx, g_settings.notificationCenter);

    if (xNew == x) {
        return original();
    }

    Wh_Log(L"Adjusting notification center: %d -> %d", x, xNew);

    SetWindowPos(hwnd, nullptr, xNew, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    g_lastAdjustedNotificationCenterWindow = hwnd;

    return original();
}

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

    if (g_unloading) {
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);
        *x = monitorInfo.rcWork.right - width;
        return;
    }

    *x = CalculateAlignedXForMonitor(monitor, width, g_settings.actionCenter);
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
        HorizontalAlignmentSetting::right;
    if (wcscmp(notificationCenterHorizontalAlignment, L"center") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            HorizontalAlignmentSetting::center;
    } else if (wcscmp(notificationCenterHorizontalAlignment, L"left") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            HorizontalAlignmentSetting::left;
    } else if (wcscmp(notificationCenterHorizontalAlignment, L"tray") == 0) {
        g_settings.notificationCenter.horizontalAlignment =
            HorizontalAlignmentSetting::tray;
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
            HorizontalAlignmentSetting::right;
        if (wcscmp(actionCenterHorizontalAlignment, L"center") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                HorizontalAlignmentSetting::center;
        } else if (wcscmp(actionCenterHorizontalAlignment, L"left") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                HorizontalAlignmentSetting::left;
        } else if (wcscmp(actionCenterHorizontalAlignment, L"tray") == 0) {
            g_settings.actionCenter.horizontalAlignment =
                HorizontalAlignmentSetting::tray;
        }

        g_settings.actionCenter.horizontalShift =
            Wh_GetIntSetting(L"actionCenter.horizontalShift");
    }
    Wh_FreeStringSetting(actionCenterHorizontalAlignment);
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
                if (_wcsicmp(moduleFileName, L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
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

    if (g_target == Target::ShellExperienceHost ||
        g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        RestoreWindowToDefault(g_lastAdjustedNotificationCenterWindow);
    } else if (g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_target == Target::ShellExperienceHost ||
        g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

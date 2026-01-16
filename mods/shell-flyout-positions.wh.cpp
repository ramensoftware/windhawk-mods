// ==WindhawkMod==
// @id              shell-flyout-positions
// @name            Shell Flyout Positions
// @description     Customize the horizontal position of the Notification Center and Action Center on Windows 11
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi
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
    - right: Right (default)
    - center: Center
    - left: Left
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  $name: Notification Center
- actionCenter:
  - horizontalAlignment: right
    $name: Horizontal alignment
    $options:
    - right: Right (default)
    - center: Center
    - left: Left
  - horizontalShift: 0
    $name: Horizontal shift
    $description: >-
      A positive value moves the element to the right, a negative value moves it
      to the left
  $name: Action Center
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <string>
#include <vector>

#include <dwmapi.h>

enum class HorizontalAlignmentSetting {
    right,
    center,
    left,
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

int CalculateAlignedX(const RECT& rcWork,
                      int width,
                      const ElementSettings& settings) {
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
    }

    return x + settings.horizontalShift;
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

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

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

    int xNew = CalculateAlignedX(monitorInfo.rcWork, cx,
                                 g_settings.notificationCenter);

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

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    if (g_unloading) {
        *x = monitorInfo.rcWork.right - width;
        return;
    }

    *x = CalculateAlignedX(monitorInfo.rcWork, width, g_settings.actionCenter);
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
    }
    Wh_FreeStringSetting(notificationCenterHorizontalAlignment);

    g_settings.notificationCenter.horizontalShift =
        Wh_GetIntSetting(L"notificationCenter.horizontalShift");

    // Action center settings.
    PCWSTR actionCenterHorizontalAlignment =
        Wh_GetStringSetting(L"actionCenter.horizontalAlignment");
    g_settings.actionCenter.horizontalAlignment =
        HorizontalAlignmentSetting::right;
    if (wcscmp(actionCenterHorizontalAlignment, L"center") == 0) {
        g_settings.actionCenter.horizontalAlignment =
            HorizontalAlignmentSetting::center;
    } else if (wcscmp(actionCenterHorizontalAlignment, L"left") == 0) {
        g_settings.actionCenter.horizontalAlignment =
            HorizontalAlignmentSetting::left;
    }
    Wh_FreeStringSetting(actionCenterHorizontalAlignment);

    g_settings.actionCenter.horizontalShift =
        Wh_GetIntSetting(L"actionCenter.horizontalShift");
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

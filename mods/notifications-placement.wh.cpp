// ==WindhawkMod==
// @id              notifications-placement
// @name            Customize Windows notifications placement
// @description     Move notifications to another monitor or another corner of the screen
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -lole32 -loleaut32 -lruntimeobject -lshcore
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
# Customize Windows notifications placement

Move notifications to another monitor or another corner of the screen.

Only Windows 10 64-bit and Windows 11 are supported.

![Screenshot](https://i.imgur.com/4PxMvLg.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitor: 1
  $name: Monitor
  $description: >-
    The monitor number that notifications will appear on, set to zero to use the
    monitor where the mouse cursor is located
- monitorInterfaceName: ""
  $name: Monitor interface name
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs, trigger a notification and look for
    "Found display device" messages.
- horizontalPlacement: right
  $name: Horizontal placement on the screen
  $options:
  - right: Right
  - left: Left
  - center: Center
- horizontalDistanceFromScreenEdge: 0
  $name: Distance from the right/left side of the screen
- verticalPlacement: bottom
  $name: Vertical placement on the screen
  $options:
  - bottom: Bottom
  - top: Top
  - center: Center
- verticalDistanceFromScreenEdge: 0
  $name: Distance from the bottom/top side of the screen
- animationDirection: automatic
  $name: Notification appearance animation direction
  $options:
  - automatic: Automatic
  - fromLeft: From left
  - fromRight: From right
  - fromTop: From top
  - fromBottom: From bottom
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#include <atomic>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_unloading;

enum class HorizontalPlacement {
    right,
    left,
    center,
};

enum class VerticalPlacement {
    bottom,
    top,
    center,
};

enum class AnimationDirection {
    automatic,
    fromLeft,
    fromRight,
    fromTop,
    fromBottom,
};

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
    HorizontalPlacement horizontalPlacement;
    int horizontalDistanceFromScreenEdge;
    VerticalPlacement verticalPlacement;
    int verticalDistanceFromScreenEdge;
    AnimationDirection animationDirection;
} g_settings;

enum class Target {
    Explorer,
    ShellExperienceHost,
};

Target g_target;

bool g_inCToastCenterExperienceManager_PositionView;

bool g_customAnimationDirectionApplied;

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

HMONITOR GetMonitorByInterfaceNameSubstr(PCWSTR interfaceNameSubstr) {
    HMONITOR monitorResult = nullptr;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (GetMonitorInfo(hMonitor, &monitorInfo)) {
            DISPLAY_DEVICE displayDevice = {
                .cb = sizeof(displayDevice),
            };

            if (EnumDisplayDevices(monitorInfo.szDevice, 0, &displayDevice,
                                   EDD_GET_DEVICE_INTERFACE_NAME)) {
                Wh_Log(L"Found display device %s, interface name: %s",
                       monitorInfo.szDevice, displayDevice.DeviceID);

                if (wcsstr(displayDevice.DeviceID, interfaceNameSubstr)) {
                    Wh_Log(L"Matched display device");
                    monitorResult = hMonitor;
                    return FALSE;
                }
            }
        }
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

bool GetMonitorWorkArea(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    return GetMonitorInfo(monitor, &monitorInfo) &&
           CopyRect(rc, &monitorInfo.rcWork);
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

    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
        return std::wstring{};
    }

    processFileNameUpper++;
    return processFileNameUpper;
}

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

bool IsTargetCoreWindow(HWND hWnd) {
    DWORD processId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
        return false;
    }

    if (_wcsicmp(GetProcessFileName(processId).c_str(),
                 L"ShellExperienceHost.exe") != 0) {
        return false;
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        _wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }

    // The window title is locale-dependent, and unfortunately I didn't find a
    // simpler way to identify the target window.
    // String source: Windows.UI.ShellCommon.<locale>.pri
    // String resource: \ActionCenter\AC_ToastCenter_Title
    // The strings were collected from here:
    // https://github.com/m417z/windows-language-files
    static const std::unordered_set<std::wstring> newNotificationStrings = {
        L"Jakinarazpen berria",
        L"Jauns paziņojums",
        L"Naujas pranešimas",
        L"Neue Benachrichtigung",
        L"New notification",
        L"Nieuwe melding",
        L"Notificació nova",
        L"Notificación nueva",
        L"Notificare nouă",
        L"Nouvelle notification",
        L"Nova notificação",
        L"Nova notificación",
        L"Nova obavijesti",
        L"Nové oznámení",
        L"Nové oznámenie",
        L"Novo obaveštenje",
        L"Novo obvestilo",
        L"Nowe powiadomienie",
        L"Nueva notificación",
        L"Nuova notifica",
        L"Ny meddelelse",
        L"Ny varsling",
        L"Nytt meddelande",
        L"Pemberitahuan baru",
        L"Thông báo mới",
        L"Új értesítés",
        L"Uus teatis",
        L"Uusi ilmoitus",
        L"Yeni bildirim",
        L"Νέα ειδοποίηση",
        L"Нове сповіщення",
        L"Ново известие",
        L"Новое уведомление",
        L"הודעה חדשה",
        L"\u200f\u200fإعلام جديد",
        L"การแจ้งให้ทราบใหม่",
        L"새 알림",
        L"新しい通知",
        L"新通知",
    };

    WCHAR szWindowText[256];
    if (GetWindowText(hWnd, szWindowText, ARRAYSIZE(szWindowText)) == 0 ||
        !newNotificationStrings.contains(szWindowText)) {
        return false;
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

void AdjustCoreWindowPos(int* x, int* y, int* cx, int* cy) {
    Wh_Log(L"Before: %dx%d %dx%d", *x, *y, *cx, *cy);

    HMONITOR srcMonitor = MonitorFromPoint({*x + *cx / 2, *y + *cy * 2},
                                           MONITOR_DEFAULTTONEAREST);

    UINT srcMonitorDpiX = 96;
    UINT srcMonitorDpiY = 96;
    GetDpiForMonitor(srcMonitor, MDT_DEFAULT, &srcMonitorDpiX, &srcMonitorDpiY);

    RECT srcMonitorWorkArea;
    if (!GetMonitorWorkArea(srcMonitor, &srcMonitorWorkArea)) {
        return;
    }

    HMONITOR destMonitor = nullptr;

    if (!g_unloading) {
        if (*g_settings.monitorInterfaceName.get()) {
            destMonitor = GetMonitorByInterfaceNameSubstr(
                g_settings.monitorInterfaceName.get());
        } else if (g_settings.monitor == 0) {
            POINT pt;
            GetCursorPos(&pt);
            destMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        } else if (g_settings.monitor >= 1) {
            destMonitor = GetMonitorById(g_settings.monitor - 1);
        }
    }

    if (!destMonitor) {
        HMONITOR primaryMonitor =
            MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);

        destMonitor = primaryMonitor;
    }

    RECT destMonitorWorkArea;
    int horizontalDistanceFromScreenEdge = 0;
    int verticalDistanceFromScreenEdge = 0;

    Wh_Log(L"Monitor %p->%p", srcMonitor, destMonitor);

    if (destMonitor != srcMonitor) {
        UINT destMonitorDpiX = 96;
        UINT destMonitorDpiY = 96;
        GetDpiForMonitor(destMonitor, MDT_DEFAULT, &destMonitorDpiX,
                         &destMonitorDpiY);

        if (!GetMonitorWorkArea(destMonitor, &destMonitorWorkArea)) {
            return;
        }

        *cx = MulDiv(*cx, destMonitorDpiX, srcMonitorDpiX);
        if (*y + *cy == srcMonitorWorkArea.bottom) {
            *y = destMonitorWorkArea.bottom -
                 MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
            *cy = MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
        } else {
            *cy = MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
        }

        if (*y == destMonitorWorkArea.top &&
            *y + *cy > destMonitorWorkArea.bottom) {
            *cy = destMonitorWorkArea.bottom - destMonitorWorkArea.top;
        }

        if (!g_unloading) {
            horizontalDistanceFromScreenEdge =
                MulDiv(g_settings.horizontalDistanceFromScreenEdge,
                       destMonitorDpiX, 96);
            verticalDistanceFromScreenEdge = MulDiv(
                g_settings.verticalDistanceFromScreenEdge, destMonitorDpiY, 96);
        }
    } else {
        CopyRect(&destMonitorWorkArea, &srcMonitorWorkArea);

        if (!g_unloading) {
            horizontalDistanceFromScreenEdge =
                MulDiv(g_settings.horizontalDistanceFromScreenEdge,
                       srcMonitorDpiX, 96);
            verticalDistanceFromScreenEdge = MulDiv(
                g_settings.verticalDistanceFromScreenEdge, srcMonitorDpiY, 96);
        }
    }

    switch (g_unloading ? HorizontalPlacement::right
                        : g_settings.horizontalPlacement) {
        case HorizontalPlacement::right:
            *x = destMonitorWorkArea.right - *cx -
                 horizontalDistanceFromScreenEdge;
            break;

        case HorizontalPlacement::left:
            *x = destMonitorWorkArea.left + horizontalDistanceFromScreenEdge;
            break;

        case HorizontalPlacement::center:
            *x = destMonitorWorkArea.left +
                 (destMonitorWorkArea.right - destMonitorWorkArea.left - *cx) /
                     2 +
                 horizontalDistanceFromScreenEdge;
            break;
    }

    switch (g_unloading ? VerticalPlacement::bottom
                        : g_settings.verticalPlacement) {
        case VerticalPlacement::bottom:
            *y = destMonitorWorkArea.bottom - *cy -
                 verticalDistanceFromScreenEdge;
            break;

        case VerticalPlacement::top:
            *y = destMonitorWorkArea.top + verticalDistanceFromScreenEdge;
            break;

        case VerticalPlacement::center:
            *y = destMonitorWorkArea.top +
                 (destMonitorWorkArea.bottom - destMonitorWorkArea.top - *cy) /
                     2 +
                 verticalDistanceFromScreenEdge;
            break;
    }

    Wh_Log(L"After: %dx%d %dx%d", *x, *y, *cx, *cy);
}

using CToastCenterExperienceManager_PositionView_t =
    HRESULT(WINAPI*)(void* pThis);
CToastCenterExperienceManager_PositionView_t
    CToastCenterExperienceManager_PositionView_Original;
HRESULT WINAPI CToastCenterExperienceManager_PositionView_Hook(void* pThis) {
    Wh_Log(L">");

    g_inCToastCenterExperienceManager_PositionView = true;
    HRESULT ret = CToastCenterExperienceManager_PositionView_Original(pThis);
    g_inCToastCenterExperienceManager_PositionView = false;

    return ret;
}

using MonitorFromPoint_t = decltype(&MonitorFromPoint);
MonitorFromPoint_t MonitorFromPoint_Original;
HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD dwFlags) {
    Wh_Log(L">");

    if (g_inCToastCenterExperienceManager_PositionView && !g_unloading &&
        pt.x == 0 && pt.y == 0) {
        HMONITOR monitor = nullptr;

        if (*g_settings.monitorInterfaceName.get()) {
            monitor = GetMonitorByInterfaceNameSubstr(
                g_settings.monitorInterfaceName.get());
        } else if (g_settings.monitor == 0) {
            POINT cursorPt;
            GetCursorPos(&cursorPt);
            monitor =
                MonitorFromPoint_Original(cursorPt, MONITOR_DEFAULTTONEAREST);
        } else if (g_settings.monitor >= 1) {
            monitor = GetMonitorById(g_settings.monitor - 1);
        }

        if (monitor) {
            return monitor;
        }
    }

    return MonitorFromPoint_Original(pt, dwFlags);
}

void UpdateAnimationDirectionStyle() {
    int angle = 0;

    switch (g_unloading ? AnimationDirection::fromRight
                        : g_settings.animationDirection) {
        case AnimationDirection::automatic:
            if (g_settings.horizontalPlacement == HorizontalPlacement::center) {
                if (g_settings.verticalPlacement == VerticalPlacement::bottom) {
                    angle = 90;
                } else {
                    angle = -90;
                }
            } else if (g_settings.horizontalPlacement ==
                       HorizontalPlacement::left) {
                angle = 180;
            }
            break;

        case AnimationDirection::fromLeft:
            angle = 180;
            break;

        case AnimationDirection::fromRight:
            break;

        case AnimationDirection::fromTop:
            angle = 90;
            break;

        case AnimationDirection::fromBottom:
            angle = -90;
            break;
    }

    if (!g_customAnimationDirectionApplied && !angle) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        Wh_Log(L"Failed to get current window");
        return;
    }

    FrameworkElement windowContent = window.Content().as<FrameworkElement>();
    if (!windowContent) {
        Wh_Log(L"Failed to get window content");
        return;
    }

    FrameworkElement launcherFrame = nullptr;

    FrameworkElement child = windowContent;
    if ((child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
        (child =
             FindChildByClassName(child, L"ActionCenter.ToastCenterPage")) &&
        (child = FindChildByName(child, L"ToastCenterMainGrid")) &&
        (child = FindChildByName(child, L"ToastCenterView")) &&
        (child = FindChildByName(child, L"ToastCenterScrollViewer")) &&
        (child = FindChildByName(child, L"Root")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ScrollContentPresenter")) &&
        (child = FindChildByName(child, L"ToastCenterGrid"))) {
        launcherFrame = child;
    }

    if (!launcherFrame) {
        Wh_Log(L"Failed to find launcher frame");
        return;
    }

    FrameworkElement rootGridContent = nullptr;
    child = launcherFrame;
    if ((child = FindChildByName(child, L"FlexibleNormalToastView")) &&
        (child = FindChildByName(child, L"MainGrid")) &&
        (child = FindChildByName(child, L"RevealGrid2"))) {
        rootGridContent = child;
    }

    if (!rootGridContent) {
        Wh_Log(L"Failed to find root grid content");
        return;
    }

    Media::RotateTransform transform;
    transform.Angle(angle);
    launcherFrame.RenderTransform(transform);
    Media::RotateTransform transform2;
    transform2.Angle(-angle);
    rootGridContent.RenderTransform(transform2);

    auto origin = winrt::Windows::Foundation::Point{0.5, 0.5};
    launcherFrame.RenderTransformOrigin(origin);
    rootGridContent.RenderTransformOrigin(origin);

    g_customAnimationDirectionApplied = (angle != 0);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [&]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (!IsTargetCoreWindow(hWnd)) {
        return original();
    }

    Wh_Log(L"%08X %08X", (DWORD)(ULONG_PTR)hWnd, uFlags);

    RECT rc{};
    GetWindowRect(hWnd, &rc);

    // Skip if no size or empty size.
    if ((uFlags & SWP_NOSIZE) || cx == 0 || cy == 0) {
        Wh_Log(L"Skipping");
        uFlags |= SWP_NOMOVE | SWP_NOSIZE;
        return original();
    }

    if (uFlags & SWP_NOMOVE) {
        uFlags &= ~SWP_NOMOVE;
        X = rc.left;
        Y = rc.top;
    }

    if (uFlags & SWP_NOSIZE) {
        uFlags &= ~SWP_NOSIZE;
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;
    }

    AdjustCoreWindowPos(&X, &Y, &cx, &cy);

    BOOL ret =
        SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    if (g_target == Target::ShellExperienceHost &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
        UpdateAnimationDirectionStyle();
    }

    return ret;
}

namespace ShellExperienceHost {

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

        AdjustCoreWindowPos(&x, &y, &cx, &cy);

        SetWindowPos_Original(hCoreWnd, nullptr, x, y, cx, cy,
                              SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

}  // namespace ShellExperienceHost

bool HookTwinuiPcshellSymbols() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Loading twinui.pcshell.dll failed");
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: long __cdecl CToastCenterExperienceManager::PositionView(void))"},
            &CToastCenterExperienceManager_PositionView_Original,
            CToastCenterExperienceManager_PositionView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");

    PCWSTR horizontalPlacement = Wh_GetStringSetting(L"horizontalPlacement");
    g_settings.horizontalPlacement = HorizontalPlacement::right;
    if (wcscmp(horizontalPlacement, L"left") == 0) {
        g_settings.horizontalPlacement = HorizontalPlacement::left;
    } else if (wcscmp(horizontalPlacement, L"center") == 0) {
        g_settings.horizontalPlacement = HorizontalPlacement::center;
    }
    Wh_FreeStringSetting(horizontalPlacement);

    g_settings.horizontalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"horizontalDistanceFromScreenEdge");

    PCWSTR verticalPlacement = Wh_GetStringSetting(L"verticalPlacement");
    g_settings.verticalPlacement = VerticalPlacement::bottom;
    if (wcscmp(verticalPlacement, L"top") == 0) {
        g_settings.verticalPlacement = VerticalPlacement::top;
    } else if (wcscmp(verticalPlacement, L"center") == 0) {
        g_settings.verticalPlacement = VerticalPlacement::center;
    }
    Wh_FreeStringSetting(verticalPlacement);

    g_settings.verticalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"verticalDistanceFromScreenEdge");

    PCWSTR animationDirection = Wh_GetStringSetting(L"animationDirection");
    g_settings.animationDirection = AnimationDirection::automatic;
    if (wcscmp(animationDirection, L"fromLeft") == 0) {
        g_settings.animationDirection = AnimationDirection::fromLeft;
    } else if (wcscmp(animationDirection, L"fromRight") == 0) {
        g_settings.animationDirection = AnimationDirection::fromRight;
    } else if (wcscmp(animationDirection, L"fromTop") == 0) {
        g_settings.animationDirection = AnimationDirection::fromTop;
    } else if (wcscmp(animationDirection, L"fromBottom") == 0) {
        g_settings.animationDirection = AnimationDirection::fromBottom;
    }
    Wh_FreeStringSetting(animationDirection);
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
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
    }

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);

    if (g_target == Target::Explorer) {
        if (!HookTwinuiPcshellSymbols()) {
            return FALSE;
        }

        WindhawkUtils::Wh_SetFunctionHookT(MonitorFromPoint,
                                           MonitorFromPoint_Hook,
                                           &MonitorFromPoint_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}

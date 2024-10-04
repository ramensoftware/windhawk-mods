// ==WindhawkMod==
// @id              taskbar-on-top
// @name            Taskbar on top for Windows 11
// @description     Moves the Windows 11 taskbar to the top of the screen
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
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
# Taskbar on top for Windows 11

Moves the Windows 11 taskbar to the top of the screen.

## Compatibility

The mod was designed for up-to-date Windows 11 versions 22H2 to 24H2. Other
versions weren't tested and are probably not compatible.

![Screenshot](https://i.imgur.com/dCABsum.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarLocation: top
  $name: Taskbar location
  $options:
  - top: Top
  - bottom: Bottom
- taskbarLocationSecondary: sameAsPrimary
  $name: Taskbar location on secondary monitors
  $options:
  - sameAsPrimary: Same as on primary monitor
  - top: Top
  - bottom: Bottom
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // must come before knownfolders.h

#include <knownfolders.h>
#include <shlobj.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

enum class TaskbarLocation {
    top,
    bottom,
};

struct {
    TaskbarLocation taskbarLocation;
    TaskbarLocation taskbarLocationSecondary;
} g_settings;

enum class Target {
    Explorer,
    StartMenu,
    SearchHost,
};

Target g_target;

HANDLE g_initializedEvent;
WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

int g_originalTaskbarHeight;
bool g_inSystemTrayController_UpdateFrameSize;
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;
bool g_inCTaskListThumbnailWnd_DisplayUI;
bool g_inCTaskListThumbnailWnd_LayoutThumbnails;
bool g_inOverflowFlyoutModel_Show;

std::vector<winrt::weak_ref<XamlRoot>> g_notifyIconsUpdated;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_notifyIconAutoRevokerList;

int g_copilotPosTimerCounter;
UINT_PTR g_copilotPosTimer;

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

bool GetMonitorRect(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    return GetMonitorInfo(monitor, &monitorInfo) &&
           CopyRect(rc, &monitorInfo.rcMonitor);
}

bool GetMonitorRectDpiUnscaled(HMONITOR monitor, RECT* rc) {
    if (!GetMonitorRect(monitor, rc)) {
        return false;
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    rc->left = MulDiv(rc->left, 96, monitorDpiX);
    rc->top = MulDiv(rc->top, 96, monitorDpiY);
    rc->right = MulDiv(rc->right, 96, monitorDpiX);
    rc->bottom = MulDiv(rc->bottom, 96, monitorDpiY);
    return true;
}

int GetPrimaryMonitorHeightDpiUnscaled() {
    const POINT ptZero = {0, 0};
    HMONITOR primaryMonitor =
        MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    RECT monitorRect;
    if (!GetMonitorRectDpiUnscaled(primaryMonitor, &monitorRect)) {
        return 0;
    }

    return monitorRect.bottom - monitorRect.top;
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (parent.Name() == name) {
            return true;
        }
    }
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (winrt::get_class_name(parent) == className) {
            return true;
        }
    }
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

TaskbarLocation GetTaskbarLocationForMonitor(HMONITOR monitor) {
    if (g_unloading) {
        return TaskbarLocation::bottom;
    }

    if (g_settings.taskbarLocation == g_settings.taskbarLocationSecondary) {
        return g_settings.taskbarLocation;
    }

    const POINT ptZero = {0, 0};
    HMONITOR primaryMonitor =
        MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

    return monitor == primaryMonitor ? g_settings.taskbarLocation
                                     : g_settings.taskbarLocationSecondary;
}

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    Wh_Log(L">");

    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);

    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

using TrayUI_GetDockedRect_t = DWORD(WINAPI*)(void* pThis,
                                              RECT* rect,
                                              BOOL param2);
TrayUI_GetDockedRect_t TrayUI_GetDockedRect_Original;
DWORD WINAPI TrayUI_GetDockedRect_Hook(void* pThis, RECT* rect, BOOL param2) {
    Wh_Log(L">");

    DWORD ret = TrayUI_GetDockedRect_Original(pThis, rect, param2);

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

    RECT monitorRect;
    GetMonitorRect(monitor, &monitorRect);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int height = rect->bottom - rect->top;

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::top:
            rect->top = monitorRect.top;
            rect->bottom = monitorRect.top + height;
            break;

        case TaskbarLocation::bottom:
            rect->top = monitorRect.bottom - height;
            rect->bottom = monitorRect.bottom;
            break;
    }

    return ret;
}

using TrayUI_MakeStuckRect_t = void(WINAPI*)(void* pThis,
                                             RECT* rect,
                                             RECT* param2,
                                             SIZE param3,
                                             DWORD taskbarPos);
TrayUI_MakeStuckRect_t TrayUI_MakeStuckRect_Original;
void WINAPI TrayUI_MakeStuckRect_Hook(void* pThis,
                                      RECT* rect,
                                      RECT* param2,
                                      SIZE param3,
                                      DWORD taskbarPos) {
    Wh_Log(L">");

    TrayUI_MakeStuckRect_Original(pThis, rect, param2, param3, taskbarPos);

    // taskbarPos:
    // 0: left
    // 1: top
    // 2: right
    // 3: bottom
    if (taskbarPos != 3) {
        return;
    }

    HMONITOR monitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

    RECT monitorRect;
    GetMonitorRect(monitor, &monitorRect);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int height = rect->bottom - rect->top;

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::top:
            rect->top = monitorRect.top;
            rect->bottom = monitorRect.top + height;
            break;

        case TaskbarLocation::bottom:
            rect->top = monitorRect.bottom - height;
            rect->bottom = monitorRect.bottom;
            break;
    }
}

LRESULT TaskbarWndProcPostProcess(HWND hWnd,
                                  UINT Msg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  LRESULT result) {
    switch (Msg) {
        case WM_SIZING: {
            Wh_Log(L"WM_SIZING: %08X", (DWORD)(ULONG_PTR)hWnd);

            if (!g_unloading) {
                RECT* rect = (RECT*)lParam;

                HMONITOR monitor =
                    MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

                RECT monitorRect;
                GetMonitorRect(monitor, &monitorRect);

                int height = rect->bottom - rect->top;

                switch (GetTaskbarLocationForMonitor(monitor)) {
                    case TaskbarLocation::top:
                        rect->top = monitorRect.top;
                        rect->bottom = monitorRect.top + height;
                        break;

                    case TaskbarLocation::bottom:
                        rect->top = monitorRect.bottom - height;
                        rect->bottom = monitorRect.bottom;
                        break;
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* windowpos = (WINDOWPOS*)lParam;
            if ((windowpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) !=
                (SWP_NOSIZE | SWP_NOMOVE)) {
                Wh_Log(L"WM_WINDOWPOSCHANGING (size or move): %08X",
                       (DWORD)(ULONG_PTR)hWnd);

                if (!g_unloading) {
                    RECT rect{
                        .left = windowpos->x,
                        .top = windowpos->y,
                        .right = windowpos->x + windowpos->cx,
                        .bottom = windowpos->y + windowpos->cy,
                    };
                    HMONITOR monitor =
                        MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

                    if (!(windowpos->flags & SWP_NOMOVE) &&
                        GetTaskbarLocationForMonitor(monitor) ==
                            TaskbarLocation::top) {
                        RECT monitorRect;
                        GetMonitorRect(monitor, &monitorRect);

                        windowpos->y = monitorRect.top;
                    }
                }
            }
            break;
        }
    }

    return result;
}

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
    g_hookCallCounter++;

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    ret = TaskbarWndProcPostProcess(hWnd, Msg, wParam, lParam, ret);

    g_hookCallCounter--;

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    g_hookCallCounter++;

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    ret = TaskbarWndProcPostProcess(hWnd, Msg, wParam, lParam, ret);

    g_hookCallCounter--;

    return ret;
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskBtnGroup,
                     int param2,
                     winrt::Windows::Foundation::Point* point,
                     HorizontalAlignment* horizontalAlignment,
                     VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t
    CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(
    void* pThis,
    void* taskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontalAlignment,
    VerticalAlignment* verticalAlignment) {
    Wh_Log(L">");

    HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original(
        pThis, taskBtnGroup, param2, point, horizontalAlignment,
        verticalAlignment);

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        return ret;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    point->Y = monitorInfo.rcWork.top + MulDiv(125, monitorDpiY, 96);

    return ret;
}

using CTaskListThumbnailWnd_DisplayUI_t = void*(WINAPI*)(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         void* param4);
CTaskListThumbnailWnd_DisplayUI_t CTaskListThumbnailWnd_DisplayUI_Original;
void* WINAPI CTaskListThumbnailWnd_DisplayUI_Hook(void* pThis,
                                                  void* param1,
                                                  void* param2,
                                                  void* param3,
                                                  void* param4) {
    Wh_Log(L">");

    g_inCTaskListThumbnailWnd_DisplayUI = true;

    void* ret = CTaskListThumbnailWnd_DisplayUI_Original(pThis, param1, param2,
                                                         param3, param4);

    g_inCTaskListThumbnailWnd_DisplayUI = false;

    return ret;
}

using CTaskListThumbnailWnd_LayoutThumbnails_t = void(WINAPI*)(void* pThis);
CTaskListThumbnailWnd_LayoutThumbnails_t
    CTaskListThumbnailWnd_LayoutThumbnails_Original;
void WINAPI CTaskListThumbnailWnd_LayoutThumbnails_Hook(void* pThis) {
    Wh_Log(L">");

    g_inCTaskListThumbnailWnd_LayoutThumbnails = true;

    CTaskListThumbnailWnd_LayoutThumbnails_Original(pThis);

    g_inCTaskListThumbnailWnd_LayoutThumbnails = false;
}

using XamlExplorerHostWindow_XamlExplorerHostWindow_t =
    void*(WINAPI*)(void* pThis,
                   unsigned int param1,
                   winrt::Windows::Foundation::Rect* rect,
                   unsigned int param3);
XamlExplorerHostWindow_XamlExplorerHostWindow_t
    XamlExplorerHostWindow_XamlExplorerHostWindow_Original;
void* WINAPI XamlExplorerHostWindow_XamlExplorerHostWindow_Hook(
    void* pThis,
    unsigned int param1,
    winrt::Windows::Foundation::Rect* rect,
    unsigned int param3) {
    Wh_Log(L">");

    if (g_inOverflowFlyoutModel_Show) {
        RECT rc{
            .left = static_cast<LONG>(rect->X),
            .top = static_cast<LONG>(rect->Y),
            .right = static_cast<LONG>(rect->X + rect->Width),
            .bottom = static_cast<LONG>(rect->Y + rect->Height),
        };

        HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::top) {
            MONITORINFO monitorInfo{
                .cbSize = sizeof(MONITORINFO),
            };
            GetMonitorInfo(monitor, &monitorInfo);
            UINT monitorDpiX = 96;
            UINT monitorDpiY = 96;
            GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

            winrt::Windows::Foundation::Rect rectNew = *rect;
            rectNew.Y = monitorInfo.rcWork.top + MulDiv(12, monitorDpiY, 96);

            return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(
                pThis, param1, &rectNew, param3);
        }
    }

    return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(pThis, param1,
                                                                  rect, param3);
}

void ApplySystemTrayChevronIconViewStyle(
    FrameworkElement systemTrayChevronIconViewElement) {
    if (g_settings.taskbarLocation != TaskbarLocation::top) {
        return;
    }

    FrameworkElement baseTextBlock = nullptr;

    FrameworkElement child = systemTrayChevronIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent")) &&
        (child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base"))) {
        baseTextBlock = child;
    }

    if (!baseTextBlock) {
        return;
    }

    double angle = g_unloading ? 0 : 180;
    Media::RotateTransform transform;
    transform.Angle(angle);
    baseTextBlock.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    baseTextBlock.RenderTransformOrigin({origin, origin});
}

using IconView_IconView_t = void(WINAPI*)(PVOID pThis);
IconView_IconView_t IconView_IconView_Original;
void WINAPI IconView_IconView_Hook(PVOID pThis) {
    Wh_Log(L">");

    IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return;
    }

    g_notifyIconAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_notifyIconAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        winrt::Windows::UI::Xaml::RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_notifyIconAutoRevokerList.erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.ChevronIconView") {
                if (IsChildOfElementByName(iconView, L"NotifyIconStack")) {
                    ApplySystemTrayChevronIconViewStyle(iconView);
                }
            }
        });
}

using OverflowFlyoutModel_Show_t = void(WINAPI*)(void* pThis);
OverflowFlyoutModel_Show_t OverflowFlyoutModel_Show_Original;
void WINAPI OverflowFlyoutModel_Show_Hook(void* pThis) {
    Wh_Log(L">");

    g_inOverflowFlyoutModel_Show = true;

    OverflowFlyoutModel_Show_Original(pThis);

    g_inOverflowFlyoutModel_Show = false;
}

using MenuFlyout_ShowAt_t = void*(WINAPI*)(void* pThis,
                                           void* placementTarget,
                                           void* showOptions);
MenuFlyout_ShowAt_t MenuFlyout_ShowAt_Original;
void* WINAPI
MenuFlyout_ShowAt_Hook(void* pThis,
                       DependencyObject* placementTarget,
                       Controls::Primitives::FlyoutShowOptions* showOptions) {
    Wh_Log(L">");

    auto original = [=]() {
        return MenuFlyout_ShowAt_Original(pThis, placementTarget, showOptions);
    };

    if (!showOptions) {
        return original();
    }

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        return original();
    }

    auto placement = showOptions->Placement();
    Wh_Log(L"Placement=%d", (int)placement);
    if (placement == Controls::Primitives::FlyoutPlacementMode::Top) {
        showOptions->Placement(
            Controls::Primitives::FlyoutPlacementMode::Bottom);
    }

    auto point =
        showOptions->Position().try_as<winrt::Windows::Foundation::Point>();
    if (point) {
        Wh_Log(L"Point=%fx%f", point->X, point->Y);
        if (point->Y < 0) {
            point->Y = -point->Y;

            FrameworkElement targetElement =
                placementTarget->try_as<FrameworkElement>();
            if (targetElement) {
                point->Y += targetElement.ActualHeight();
            }

            showOptions->Position(point);
        }
    }

    return MenuFlyout_ShowAt_Original(pThis, placementTarget, showOptions);
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
    auto original = [=]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (uFlags & (SWP_NOSIZE | SWP_NOMOVE)) {
        return original();
    }

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0) {
        if (!g_inCTaskListThumbnailWnd_DisplayUI &&
            !g_inCTaskListThumbnailWnd_LayoutThumbnails) {
            return original();
        }

        DWORD messagePos = GetMessagePos();
        POINT pt{
            GET_X_LPARAM(messagePos),
            GET_Y_LPARAM(messagePos),
        };

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        if (g_inCTaskListThumbnailWnd_DisplayUI) {
            Y = std::max(monitorInfo.rcWork.top, pt.y);
            Y += MulDiv(12, GetDpiForWindow(hWnd), 96);
        } else {
            // Keep current position.
            RECT rc;
            GetWindowRect(hWnd, &rc);
            Y = rc.top;
        }
    } else if (_wcsicmp(szClassName, L"TopLevelWindowForOverflowXamlIsland") ==
                   0 ||
               _wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0) {
        DWORD messagePos = GetMessagePos();
        POINT pt{
            GET_X_LPARAM(messagePos),
            GET_Y_LPARAM(messagePos),
        };

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        if (Y < monitorInfo.rcWork.top) {
            Y = monitorInfo.rcWork.top;
        } else if (Y > monitorInfo.rcWork.bottom - cy) {
            Y = monitorInfo.rcWork.bottom - cy;
        }
    } else {
        return original();
    }

    Wh_Log(L"Adjusting pos for %s: %dx%d, %dx%d", szClassName, X, Y, X + cx,
           Y + cy);

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

namespace CoreWindowUI {

bool IsTargetCoreWindow(HWND hWnd, int* extraXAdjustment) {
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

    if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
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
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            if (IsTargetCoreWindow(hWnd, nullptr)) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void AdjustCoreWindowSize(int x, int y, int* width, int* height) {
    if (g_target != Target::StartMenu) {
        return;
    }

    const POINT pt = {x, y};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        *height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
        return;
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    const int h1 = MulDiv(750, monitorDpiY, 96);
    const int h2 = MulDiv(694, monitorDpiY, 96);
    if (*height >= h1) {
        *height = h1;
    } else if (*height >= h2) {
        *height = h2;
    }
}

void AdjustCoreWindowPos(int* x, int* y, int width, int height) {
    const POINT pt = {*x, *y};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        if (g_target == Target::StartMenu) {
            *x = 0;
            *y = 0;
        }

        return;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    if (g_target == Target::StartMenu || g_target == Target::SearchHost) {
        *y = monitorInfo.rcWork.top;
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

        AdjustCoreWindowSize(x, y, &cx, &cy);
        AdjustCoreWindowPos(&x, &y, cx, cy);

        SetWindowPos_Original(hCoreWnd, nullptr, x, y, cx, cy,
                              SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Creating core window");
        AdjustCoreWindowSize(X, Y, &nWidth, &nHeight);
        AdjustCoreWindowPos(&X, &Y, nWidth, nHeight);
    }

    return CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Creating core window");
        AdjustCoreWindowSize(X, Y, &nWidth, &nHeight);
        AdjustCoreWindowPos(&X, &Y, nWidth, nHeight);
    }

    return CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
}

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

    int extraXAdjustment = 0;
    if (!IsTargetCoreWindow(hWnd, &extraXAdjustment)) {
        return original();
    }

    Wh_Log(L"%08X %08X", (DWORD)(ULONG_PTR)hWnd, uFlags);

    if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return original();
    }

    RECT rc{};
    GetWindowRect(hWnd, &rc);

    // SearchHost is being moved by explorer.exe, then the size is adjusted
    // by SearchHost itself. Make SearchHost adjust the position too. A
    // similar workaround is needed for other windows.
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
        AdjustCoreWindowSize(X, Y, &width, &height);
    } else {
        AdjustCoreWindowSize(X, Y, &cx, &cy);
        width = cx;
        height = cy;
    }

    if (!(uFlags & SWP_NOMOVE)) {
        AdjustCoreWindowPos(&X, &Y, width, height);
    }

    X += extraXAdjustment;

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

}  // namespace CoreWindowUI

void LoadSettings() {
    PCWSTR taskbarLocation = Wh_GetStringSetting(L"taskbarLocation");
    g_settings.taskbarLocation = TaskbarLocation::top;
    if (wcscmp(taskbarLocation, L"bottom") == 0) {
        g_settings.taskbarLocation = TaskbarLocation::bottom;
    }
    Wh_FreeStringSetting(taskbarLocation);

    PCWSTR taskbarLocationSecondary =
        Wh_GetStringSetting(L"taskbarLocationSecondary");
    g_settings.taskbarLocationSecondary = g_settings.taskbarLocation;
    if (wcscmp(taskbarLocationSecondary, L"top") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::top;
    } else if (wcscmp(taskbarLocationSecondary, L"bottom") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::bottom;
    }
    Wh_FreeStringSetting(taskbarLocationSecondary);
}

HWND GetTaskbarWnd() {
    static HWND hTaskbarWnd;

    if (!hTaskbarWnd) {
        HWND hWnd = FindWindow(L"Shell_TrayWnd", nullptr);

        DWORD processId = 0;
        if (hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hTaskbarWnd = hWnd;
        }
    }

    return hTaskbarWnd;
}

void ApplySettings() {
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    g_applyingSettings = true;

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    g_applyingSettings = false;
}

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    {
        // Taskbar.View.dll, ExplorerExtensions.dll
        WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
            {
                {
                    LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))",
                },
                (void**)&IconView_IconView_Original,
                (void*)IconView_IconView_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowFlyoutModel::Show(void))",
                },
                (void**)&OverflowFlyoutModel_Show_Original,
                (void*)OverflowFlyoutModel_Show_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_Controls_Primitives_IFlyoutBase5<struct winrt::Windows::UI::Xaml::Controls::MenuFlyout>::ShowAt(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions const &)const )",
                },
                (void**)&MenuFlyout_ShowAt_Original,
                (void*)MenuFlyout_ShowAt_Hook,
            },
        };

        if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
            return false;
        }
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))",
            },
            (void**)&TrayUI__StuckTrayChange_Original,
        },
        {
            {
                LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
            },
            (void**)&TrayUI__HandleSettingChange_Original,
            (void*)TrayUI__HandleSettingChange_Hook,
        },
        {
            {
                LR"(public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int))",
            },
            (void**)&TrayUI_GetDockedRect_Original,
            (void*)TrayUI_GetDockedRect_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int))",
            },
            (void**)&TrayUI_MakeStuckRect_Original,
            (void*)TrayUI_MakeStuckRect_Hook,
        },
        {
            {
                LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))",
            },
            (void**)&TrayUI_WndProc_Original,
            (void*)TrayUI_WndProc_Hook,
        },
        {
            {
                LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
            },
            (void**)&CSecondaryTray_v_WndProc_Original,
            (void*)CSecondaryTray_v_WndProc_Hook,
        },
        {
            {
                LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )",
            },
            (void**)&CTaskListWnd_ComputeJumpViewPosition_Original,
            (void*)CTaskListWnd_ComputeJumpViewPosition_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))",
            },
            (void**)&CTaskListThumbnailWnd_DisplayUI_Original,
            (void*)CTaskListThumbnailWnd_DisplayUI_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl CTaskListThumbnailWnd::LayoutThumbnails(void))",
            },
            (void**)&CTaskListThumbnailWnd_LayoutThumbnails_Original,
            (void*)CTaskListThumbnailWnd_LayoutThumbnails_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::Windows::Internal::Shell::XamlExplorerHost::XamlExplorerHostWindow::XamlExplorerHostWindow(unsigned int,struct winrt::Windows::Foundation::Rect const &,unsigned int))",
            },
            (void**)&XamlExplorerHostWindow_XamlExplorerHostWindow_Original,
            (void*)XamlExplorerHostWindow_XamlExplorerHostWindow_Hook,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);

    return TRUE;
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
                    g_target = Target::StartMenu;
                } else if (_wcsicmp(moduleFileName, L"SearchHost.exe") == 0) {
                    g_target = Target::SearchHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
    }

    if (g_target == Target::StartMenu || g_target == Target::SearchHost) {
        HMODULE user32Module = LoadLibrary(L"user32.dll");
        if (user32Module) {
            void* pCreateWindowInBand =
                (void*)GetProcAddress(user32Module, "CreateWindowInBand");
            if (pCreateWindowInBand) {
                Wh_SetFunctionHook(
                    pCreateWindowInBand,
                    (void*)CoreWindowUI::CreateWindowInBand_Hook,
                    (void**)&CoreWindowUI::CreateWindowInBand_Original);
            }

            void* pCreateWindowInBandEx =
                (void*)GetProcAddress(user32Module, "CreateWindowInBandEx");
            if (pCreateWindowInBandEx) {
                Wh_SetFunctionHook(
                    pCreateWindowInBandEx,
                    (void*)CoreWindowUI::CreateWindowInBandEx_Hook,
                    (void**)&CoreWindowUI::CreateWindowInBandEx_Original);
            }
        }

        Wh_SetFunctionHook((void*)SetWindowPos,
                           (void*)CoreWindowUI::SetWindowPos_Hook,
                           (void**)&SetWindowPos_Original);
        return TRUE;
    }

    if (!GetTaskbarViewDllPath(g_taskbarViewDllPath)) {
        Wh_Log(L"Taskbar view module not found");
        return FALSE;
    }

    HMODULE taskbarViewModule = LoadLibraryEx(g_taskbarViewDllPath, nullptr,
                                              LOAD_WITH_ALTERED_SEARCH_PATH);
    if (taskbarViewModule) {
        return ModInitWithTaskbarView(taskbarViewModule);
    }

    Wh_Log(L"Taskbar view module not loaded yet");
    return FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    g_initializedEvent =
        CreateEvent(nullptr, true, false, "InitializedEvent_" WH_MOD_ID);

    if (g_target == Target::Explorer) {
        ApplySettings();
        SetEvent(g_initializedEvent);
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost) {
        // Wait for the taskbar reposition to complete.
        WaitForSingleObject(g_initializedEvent, 1000);
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        ApplySettings();
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }

    if (g_initializedEvent) {
        CloseHandle(g_initializedEvent);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_target == Target::Explorer) {
        ApplySettings();
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost) {
        CoreWindowUI::ApplySettings();
    }
}

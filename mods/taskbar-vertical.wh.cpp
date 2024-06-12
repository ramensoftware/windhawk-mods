// ==WindhawkMod==
// @id              taskbar-vertical
// @name            Vertical Taskbar for Windows 11
// @description     Finally, the missing vertical taskbar option for Windows 11!
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0605 -lgdi32 -lole32 -loleaut32 -lshcore -lwininet
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
# Vertical Taskbar for Windows 11

Finally, the missing vertical taskbar option for Windows 11!

## Compatibility

The mod was designed for up-to-date Windows 11 versions 22H2 and 23H2. Other
versions weren't tested and are probably not compatible.

Some of the other taskbar mods, such as [Taskbar height and icon
size](https://windhawk.net/mods/taskbar-icon-size), aren't compatible with this
mod.

## Funding

The development of this mod was funded by [AuthLite LLC](https://authlite.com/).
Thank you for contributing and allowing all Windhawk users to enjoy it!

![Screenshot](https://i.imgur.com/BxQMy5K.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TaskbarWidth: 48
  $name: Taskbar width
  $description: >-
    The width, in pixels, of the taskbar
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // must come before knownfolders.h

#include <knownfolders.h>
#include <shlobj.h>
#include <windowsx.h>
#include <wininet.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <limits>
#include <list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

struct {
    int taskbarWidth;
} g_settings;

WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

int g_originalTaskbarHeight;
bool g_windowRgnChanging;
bool g_inSystemTraySecondaryController_UpdateFrameSize;
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;
bool g_inCTaskListThumbnailWnd_DisplayUI;
bool g_inChevronSystemTrayIconDataModel2_OnIconClicked;

int g_notifyIconsUpdateCount;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_notifyIconAutoRevokerList;

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

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    Wh_Log(L">");

    TrayUI_GetMinSize_Original(pThis, monitor, size);

    if (!g_unloading) {
        // Reassign min height to fix displaced secondary taskbar when auto-hide
        // is enabled.
        RECT monitorRect;
        GetMonitorRect(monitor, &monitorRect);
        size->cy = monitorRect.bottom - monitorRect.top;
    }
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
    int taskbarOriginalHeightScaled =
        MulDiv(g_originalTaskbarHeight, monitorDpiY, 96);

    if (!g_unloading) {
        rect->top = monitorRect.top;
        rect->right = rect->left + (rect->bottom - rect->top);
    } else {
        rect->top = rect->bottom - taskbarOriginalHeightScaled;
        rect->right = monitorRect.right;
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
    int taskbarWidthScaled = MulDiv(g_settings.taskbarWidth, monitorDpiX, 96);
    int taskbarOriginalHeightScaled =
        MulDiv(g_originalTaskbarHeight, monitorDpiY, 96);

    if (!g_unloading) {
        rect->top = monitorRect.top;
        rect->right = rect->left + taskbarWidthScaled;
    } else {
        rect->top = rect->bottom - taskbarOriginalHeightScaled;
        rect->right = monitorRect.right;
    }
}

HWND GetTaskbarWnd();

LRESULT TaskbarWndProcPostProcess(HWND hWnd,
                                  UINT Msg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  LRESULT result) {
    // Calling CreateRectRgn has two reasons: To actually set the region, and to
    // post window size change events which cause element sizes and positions to
    // be recalculated.
    auto updateWindowRgn = [](HWND hWnd, int width, int height) {
        // Avoid handling this recursively as SetWindowRgn triggers
        // WM_WINDOWPOSCHANGED again.
        if (g_windowRgnChanging) {
            return;
        }

        g_windowRgnChanging = true;

        if (!g_unloading) {
            HRGN windowRegion = CreateRectRgn(0, 0, width, height);
            if (windowRegion) {
                SetWindowRgn(hWnd, windowRegion, TRUE);
            }
        } else {
            SetWindowRgn(hWnd, nullptr, TRUE);
        }

        g_windowRgnChanging = false;
    };

    switch (Msg) {
        case WM_SIZING: {
            Wh_Log(L"WM_SIZING: %08X", (DWORD)(ULONG_PTR)hWnd);

            RECT* rect = (RECT*)lParam;

            if (!g_unloading) {
                HMONITOR monitor =
                    MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);

                RECT monitorRect;
                GetMonitorRect(monitor, &monitorRect);

                rect->top = monitorRect.top;
                rect->right = rect->left + (rect->bottom - rect->top);
            }

            updateWindowRgn(
                hWnd,
                MulDiv(g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96),
                rect->bottom - rect->top);
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* windowpos = (WINDOWPOS*)lParam;
            if (!(windowpos->flags & SWP_NOSIZE)) {
                Wh_Log(L"WM_WINDOWPOSCHANGING (no SWP_NOSIZE): %08X",
                       (DWORD)(ULONG_PTR)hWnd);

                if (!g_unloading) {
                    windowpos->cx = windowpos->cy;
                }

                updateWindowRgn(
                    hWnd,
                    MulDiv(g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96),
                    windowpos->cy);
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            auto* windowpos = (WINDOWPOS*)lParam;
            if (!(windowpos->flags & SWP_NOSIZE)) {
                Wh_Log(L"WM_WINDOWPOSCHANGED (no SWP_NOSIZE): %08X",
                       (DWORD)(ULONG_PTR)hWnd);

                updateWindowRgn(
                    hWnd,
                    MulDiv(g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96),
                    windowpos->cy);
            }
            break;
        }

        case WM_PAINT: {
            RECT rc;
            GetWindowRect(hWnd, &rc);

            updateWindowRgn(
                hWnd,
                MulDiv(g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96),
                rc.bottom - rc.top);
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
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
    pt.x = MulDiv(pt.x, 96, monitorDpiX);
    pt.y = MulDiv(pt.y, 96, monitorDpiY);

    point->X = pt.x;
    point->Y = pt.y;

    *verticalAlignment = VerticalAlignment::Center;

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

using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && !g_unloading) {
        return GetPrimaryMonitorHeightDpiUnscaled();
    }

    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && !g_unloading) {
        return GetPrimaryMonitorHeightDpiUnscaled();
    }

    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}

using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && !g_unloading) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
        }

        return GetPrimaryMonitorHeightDpiUnscaled();
    }

    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;

using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    Wh_Log(L">");

    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }

    // Set the height to NaN (Auto) to always match the parent height.
    value = std::numeric_limits<double>::quiet_NaN();

    return TaskbarFrame_Height_double_Original(pThis, value);
}

using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    g_inSystemTraySecondaryController_UpdateFrameSize = true;

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    g_inSystemTraySecondaryController_UpdateFrameSize = false;
}

using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    // Wh_Log(L">");

    if (g_inSystemTraySecondaryController_UpdateFrameSize) {
        // Set the secondary taskbar clock height to NaN, otherwise it may not
        // match the custom taskbar height.
        value = std::numeric_limits<double>::quiet_NaN();
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

bool ApplyStyle(FrameworkElement taskbarFrame,
                const winrt::Windows::Foundation::Size& size) {
    auto rootGrid =
        Media::VisualTreeHelper::GetParent(taskbarFrame).as<Controls::Grid>();

    double angle = g_unloading ? 0 : 90;
    Media::RotateTransform transform;
    transform.Angle(angle);
    rootGrid.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    rootGrid.RenderTransformOrigin({origin, origin});

    double marginTop = g_unloading ? 0 : size.Height - g_settings.taskbarWidth;
    if (marginTop < 0) {
        marginTop = 0;
    }

    FrameworkElement child = rootGrid;
    if (child && (child = FindChildByName(child, L"TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid"))) {
        child.Margin(Thickness{0, marginTop, 0, 0});
    }

    child = rootGrid;
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        child.Margin(Thickness{0, marginTop, 0, 0});
    }

    return true;
}

using TaskbarFrame_MeasureOverride_t = winrt::Windows::Foundation::Size*(
    WINAPI*)(void* pThis, void* param1, void* param2);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
winrt::Windows::Foundation::Size* WINAPI
TaskbarFrame_MeasureOverride_Hook(void* pThis, void* param1, void* param2) {
    g_hookCallCounter++;

    Wh_Log(L">");

    winrt::Windows::Foundation::Size* ret =
        TaskbarFrame_MeasureOverride_Original(pThis, param1, param2);

    IUnknown* taskbarFrameElementIUnknownPtr = *((IUnknown**)pThis + 1);
    if (taskbarFrameElementIUnknownPtr) {
        FrameworkElement taskbarFrameElement = nullptr;
        taskbarFrameElementIUnknownPtr->QueryInterface(
            winrt::guid_of<FrameworkElement>(),
            winrt::put_abi(taskbarFrameElement));
        if (taskbarFrameElement) {
            try {
                ApplyStyle(taskbarFrameElement, *ret);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        }
    }

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
}

void ApplyNotifyIconViewStyle(FrameworkElement notifyIconViewElement) {
    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.ImageIconContent")) &&
        (child = FindChildByName(child, L"ContainerGrid")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Image"))) {
        double angle = g_unloading ? 0 : -90;
        Media::RotateTransform transform;
        transform.Angle(angle);
        child.RenderTransform(transform);

        float origin = g_unloading ? 0 : 0.5;
        child.RenderTransformOrigin({origin, origin});
    }
}

void ApplySystemTrayIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        double angle = g_unloading ? 0 : -90;
        Media::RotateTransform transform;
        transform.Angle(angle);
        child.RenderTransform(transform);

        float origin = g_unloading ? 0 : 0.5;
        child.RenderTransformOrigin({origin, origin});
    }
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

            if (className == L"SystemTray.NotifyIconView") {
                if (!IsChildOfElementByClassName(
                        iconView, L"SystemTray.NotificationAreaOverflow")) {
                    ApplyNotifyIconViewStyle(iconView);
                }
            } else if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon" &&
                    IsChildOfElementByName(iconView, L"ControlCenterButton")) {
                    ApplySystemTrayIconStyle(iconView);
                }
            }
        });
}

bool ApplyStyleIfNeeded(XamlRoot xamlRoot) {
    FrameworkElement rootGrid = xamlRoot.Content().try_as<FrameworkElement>();

    FrameworkElement taskbarFrame = nullptr;

    FrameworkElement child = rootGrid;
    if (child && (child = FindChildByName(child, L"TaskbarFrame"))) {
        taskbarFrame = child;
    }

    if (!taskbarFrame ||
        taskbarFrame.ActualHeight() == g_settings.taskbarWidth) {
        return true;
    }

    return ApplyStyle(taskbarFrame, taskbarFrame.ActualSize());
}

bool UpdateNotifyIconsIfNeeded(XamlRoot xamlRoot) {
    if (!g_unloading) {
        if (g_notifyIconsUpdateCount > 0) {
            return true;
        }
    } else if (g_notifyIconsUpdateCount > 1) {
        return true;
    }

    FrameworkElement rootGrid = xamlRoot.Content().try_as<FrameworkElement>();

    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = rootGrid;
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    // Copy margin from TaskbarFrame root. It's done in
    // TaskbarFrame_MeasureOverride_Hook as well, but SystemTrayFrameGrid might
    // not exist yet at this point.
    child = rootGrid;
    if (child && (child = FindChildByName(child, L"TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid"))) {
        systemTrayFrameGrid.Margin(child.Margin());
    }

    FrameworkElement notificationAreaIcons =
        FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
    if (notificationAreaIcons) {
        FrameworkElement stackPanel = nullptr;

        FrameworkElement child = notificationAreaIcons;
        if ((child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
            stackPanel = child;
        }

        if (stackPanel) {
            EnumChildElements(stackPanel, [](FrameworkElement child) {
                auto childClassName = winrt::get_class_name(child);
                if (childClassName !=
                    L"Windows.UI.Xaml.Controls.ContentPresenter") {
                    Wh_Log(L"Unsupported class name %s of child",
                           childClassName.c_str());
                    return false;
                }

                FrameworkElement notifyIconViewElement =
                    FindChildByName(child, L"NotifyItemIcon");
                if (!notifyIconViewElement) {
                    Wh_Log(L"Failed to get notifyIconViewElement of child");
                    return false;
                }

                ApplyNotifyIconViewStyle(notifyIconViewElement);
                return false;
            });
        }
    }

    FrameworkElement controlCenterButton =
        FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        FrameworkElement stackPanel = nullptr;

        FrameworkElement child = controlCenterButton;
        if ((child = FindChildByClassName(child,
                                          L"Windows.UI.Xaml.Controls.Grid")) &&
            (child = FindChildByName(child, L"ContentPresenter")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
            stackPanel = child;
        }

        if (stackPanel) {
            EnumChildElements(stackPanel, [](FrameworkElement child) {
                auto childClassName = winrt::get_class_name(child);
                if (childClassName !=
                    L"Windows.UI.Xaml.Controls.ContentPresenter") {
                    Wh_Log(L"Unsupported class name %s of child",
                           childClassName.c_str());
                    return false;
                }

                FrameworkElement systemTrayIconElement =
                    FindChildByName(child, L"SystemTrayIcon");
                if (!systemTrayIconElement) {
                    Wh_Log(L"Failed to get SystemTrayIcon of child");
                    return false;
                }

                ApplySystemTrayIconStyle(systemTrayIconElement);
                return false;
            });
        }
    }

    g_notifyIconsUpdateCount++;

    return true;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateVisualStates_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    auto iconElement = FindChildByName(iconPanelElement, L"Icon");
    if (!iconElement) {
        return;
    }

    double angle = g_unloading ? 0 : -90;
    Media::RotateTransform transform;
    transform.Angle(angle);
    iconElement.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    iconElement.RenderTransformOrigin({origin, origin});

    auto xamlRoot = taskListButtonElement.XamlRoot();
    if (xamlRoot) {
        try {
            ApplyStyleIfNeeded(xamlRoot);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }

        try {
            UpdateNotifyIconsIfNeeded(xamlRoot);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
}

using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateVisualStates_t
    ExperienceToggleButton_UpdateVisualStates_Original;
void WINAPI ExperienceToggleButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    ExperienceToggleButton_UpdateVisualStates_Original(pThis);

    void* toggleButtonIUnknownPtr = (void**)pThis + 2;
    winrt::Windows::Foundation::IUnknown toggleButtonIUnknown;
    winrt::copy_from_abi(toggleButtonIUnknown, toggleButtonIUnknownPtr);

    auto toggleButtonElement = toggleButtonIUnknown.as<FrameworkElement>();

    auto panelElement = FindChildByName(toggleButtonElement,
                                        L"ExperienceToggleButtonRootPanel");
    if (!panelElement) {
        return;
    }

    auto iconElement = FindChildByName(panelElement, L"Icon");
    if (!iconElement) {
        return;
    }

    double angle = g_unloading ? 0 : -90;
    Media::RotateTransform transform;
    transform.Angle(angle);
    iconElement.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    iconElement.RenderTransformOrigin({origin, origin});

    auto xamlRoot = toggleButtonElement.XamlRoot();
    if (xamlRoot) {
        try {
            ApplyStyleIfNeeded(xamlRoot);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }

        try {
            UpdateNotifyIconsIfNeeded(xamlRoot);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
}

using OverflowFlyoutList_OnApplyTemplate_t = void(WINAPI*)(LPVOID pThis);
OverflowFlyoutList_OnApplyTemplate_t
    OverflowFlyoutList_OnApplyTemplate_Original;
void WINAPI OverflowFlyoutList_OnApplyTemplate_Hook(LPVOID pThis) {
    Wh_Log(L">");

    OverflowFlyoutList_OnApplyTemplate_Original(pThis);

    IUnknown* elementIUnknownPtr = *((IUnknown**)pThis + 1);
    if (!elementIUnknownPtr) {
        return;
    }

    FrameworkElement element = nullptr;
    elementIUnknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                       winrt::put_abi(element));
    if (!element) {
        return;
    }

    try {
        element.MaxHeight(48);
        element.MaxWidth(310);

        auto parentElement =
            Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();

        if (parentElement) {
            parentElement = Media::VisualTreeHelper::GetParent(parentElement)
                                .as<FrameworkElement>();
        }

        if (parentElement) {
            {
                Media::RotateTransform transform;
                transform.Angle(180);
                element.RenderTransform(transform);

                float origin = 0.5;
                element.RenderTransformOrigin({origin, origin});
            }

            {
                Media::RotateTransform transform;
                transform.Angle(-90);
                parentElement.RenderTransform(transform);

                float origin = 0;
                parentElement.RenderTransformOrigin({origin, origin});
            }

            auto translation = parentElement.Translation();
            if (!translation.x) {
                DWORD messagePos = GetMessagePos();
                POINT pt{
                    GET_X_LPARAM(messagePos),
                    GET_Y_LPARAM(messagePos),
                };

                HMONITOR monitor =
                    MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO monitorInfo{
                    .cbSize = sizeof(MONITORINFO),
                };
                GetMonitorInfo(monitor, &monitorInfo);
                UINT monitorDpiX = 96;
                UINT monitorDpiY = 96;
                GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                 &monitorDpiY);
                pt.x = MulDiv(pt.x, 96, monitorDpiX);
                pt.y = MulDiv(pt.y, 96, monitorDpiY);

                translation.x = 48 - 61 + 12;
                translation.y = pt.y + 40;

                parentElement.Translation(translation);
            }
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
}

using ChevronSystemTrayIconDataModel2_OnIconClicked_t =
    void(WINAPI*)(void* pThis, void* param1);
ChevronSystemTrayIconDataModel2_OnIconClicked_t
    ChevronSystemTrayIconDataModel2_OnIconClicked_Original;
void WINAPI ChevronSystemTrayIconDataModel2_OnIconClicked_Hook(void* pThis,
                                                               void* param1) {
    Wh_Log(L">");

    g_inChevronSystemTrayIconDataModel2_OnIconClicked = true;

    ChevronSystemTrayIconDataModel2_OnIconClicked_Original(pThis, param1);

    g_inChevronSystemTrayIconDataModel2_OnIconClicked = false;
}

using OverflowXamlIslandManager_Show_t = void(WINAPI*)(void* pThis,
                                                       POINT point,
                                                       void* param2);
OverflowXamlIslandManager_Show_t OverflowXamlIslandManager_Show_Original;
void WINAPI OverflowXamlIslandManager_Show_Hook(void* pThis,
                                                POINT point,
                                                void* param2) {
    Wh_Log(L">");

    if (g_inChevronSystemTrayIconDataModel2_OnIconClicked) {
        DWORD messagePos = GetMessagePos();
        point = POINT{
            GET_X_LPARAM(messagePos),
            GET_Y_LPARAM(messagePos),
        };

        HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);
        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        if (point.x < monitorInfo.rcWork.left) {
            point.x = monitorInfo.rcWork.left;
        } else if (point.x > monitorInfo.rcWork.right) {
            point.x = monitorInfo.rcWork.right;
        }

        point.x += MulDiv(104 + 12, monitorDpiX, 96);
        point.y += MulDiv(28, monitorDpiY, 96);
    }

    OverflowXamlIslandManager_Show_Original(pThis, point, param2);
}

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);

    // This is used to position secondary taskbars.
    RECT rc;
    if (dwMessage == ABM_QUERYPOS && ret && !g_unloading && pData->hWnd &&
        GetWindowRect(pData->hWnd, &rc)) {
        HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

        RECT monitorRect;
        GetMonitorRect(monitor, &monitorRect);

        pData->rc.top = monitorRect.top;
    }

    return ret;
}

using GetWindowRect_t = decltype(&GetWindowRect);
GetWindowRect_t GetWindowRect_Original;
BOOL WINAPI GetWindowRect_Hook(HWND hWnd, LPRECT lpRect) {
    BOOL ret = GetWindowRect_Original(hWnd, lpRect);
    if (ret && !g_unloading && hWnd == GetTaskbarWnd()) {
        if (GetCurrentThreadId() == GetWindowThreadProcessId(hWnd, nullptr) &&
            g_inCTaskListThumbnailWnd_DisplayUI) {
            // Fix thumbnails always displaying as list.
            HMONITOR monitor =
                MonitorFromRect(lpRect, MONITOR_DEFAULTTONEAREST);

            MONITORINFO monitorInfo{
                .cbSize = sizeof(MONITORINFO),
            };
            GetMonitorInfo(monitor, &monitorInfo);

            CopyRect(lpRect, &monitorInfo.rcWork);
        } else {
            // Adjust rect so that desktop icons will be positioned correctly.
            lpRect->right = lpRect->left + MulDiv(g_settings.taskbarWidth,
                                                  GetDpiForWindow(hWnd), 96);
        }
    }

    return ret;
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

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") != 0) {
        return original();
    }

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    const int distance = MulDiv(12, GetDpiForWindow(hWnd), 96);

    SIZE sz{
        .cx = cx + distance * 2,
        .cy = cy + distance * 2,
    };

    RECT rc;
    CalculatePopupWindowPosition(
        &pt, &sz, TPM_LEFTALIGN | TPM_VCENTERALIGN | TPM_WORKAREA, nullptr,
        &rc);

    rc.left += distance;
    rc.right -= distance;
    rc.top += distance;
    rc.bottom -= distance;

    return SetWindowPos_Original(hWnd, hWndInsertAfter, rc.left, rc.top,
                                 rc.right - rc.left, rc.bottom - rc.top,
                                 uFlags);
}

void LoadSettings() {
    g_settings.taskbarWidth = Wh_GetIntSetting(L"TaskbarWidth");
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

void ApplySettings(bool waitForApply = true) {
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    g_applyingSettings = true;

    if (waitForApply) {
        g_pendingMeasureOverride = true;
    }

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    if (waitForApply) {
        // Wait for the change to apply.
        for (int i = 0; i < 100; i++) {
            if (!g_pendingMeasureOverride) {
                break;
            }

            Sleep(100);
        }
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            // Trigger CTaskBand::_HandleSyncDisplayChange.
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }

    g_applyingSettings = false;
}

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            InternetCloseHandle(hUrlHandle);
            InternetCloseHandle(hOpenHandle);
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                         &dwNumberOfBytesRead);
        dwLength += dwNumberOfBytesRead;
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

bool HookSymbolsWithOnlineCacheFallback(
    HMODULE module,
    const WindhawkUtils::SYMBOL_HOOK* symbolHooks,
    size_t symbolHooksCount) {
    constexpr WCHAR kModIdForCache[] = L"taskbar-vertical";

    WH_FIND_SYMBOL_OPTIONS options{
        .symbolServer = L"",
    };
    if (HookSymbols(module, symbolHooks, symbolHooksCount, &options)) {
        return true;
    }

    Wh_Log(L"Offline HookSymbols() failed, trying to get an online cache");

    WCHAR moduleFilePath[MAX_PATH];
    DWORD moduleFilePathLen =
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath));
    if (!moduleFilePathLen || moduleFilePathLen == ARRAYSIZE(moduleFilePath)) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    DWORD moduleFileNameLen =
        moduleFilePathLen - (moduleFileName - moduleFilePath);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, moduleFileName,
                  moduleFileNameLen, moduleFileName, moduleFileNameLen, nullptr,
                  nullptr, 0);

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    std::wstring cacheStrKey =
#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#else
#error "Unsupported architecture"
#endif
    cacheStrKey += moduleFileName;

    std::wstring onlineCacheUrl =
        L"https://ramensoftware.github.io/windhawk-mod-symbol-cache/";
    onlineCacheUrl += kModIdForCache;
    onlineCacheUrl += L'/';
    onlineCacheUrl += cacheStrKey;
    onlineCacheUrl += L'/';
    onlineCacheUrl += timeStamp;
    onlineCacheUrl += L'-';
    onlineCacheUrl += imageSize;
    onlineCacheUrl += L".txt";

    Wh_Log(L"Looking for an online cache at %s", onlineCacheUrl.c_str());

    auto onlineCache = GetUrlContent(onlineCacheUrl.c_str());
    if (onlineCache) {
        Wh_SetStringValue(cacheStrKey.c_str(), onlineCache->c_str());
    } else {
        Wh_Log(L"Failed to get online cache");
    }

    return HookSymbols(module, symbolHooks, symbolHooksCount);
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
        WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
            {
                {
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                },
                (void**)&SystemTrayController_GetFrameSize_Original,
                (void*)SystemTrayController_GetFrameSize_Hook,
            },
            {
                {
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                },
                (void**)&SystemTraySecondaryController_GetFrameSize_Original,
                (void*)SystemTraySecondaryController_GetFrameSize_Hook,
            },
            {
                {
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                },
                (void**)&TaskbarConfiguration_GetFrameSize_Original,
                (void*)TaskbarConfiguration_GetFrameSize_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )",
                },
                (void**)&TaskbarFrame_MaxHeight_double_Original,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                },
                (void**)&TaskbarFrame_Height_double_Original,
                (void*)TaskbarFrame_Height_double_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))",
                },
                (void**)&SystemTraySecondaryController_UpdateFrameSize_Original,
                (void*)SystemTraySecondaryController_UpdateFrameSize_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )",
                },
                (void**)&SystemTrayFrame_Height_Original,
                (void*)SystemTrayFrame_Height_Hook,
            },
            {
                {
                    LR"(public: struct winrt::Windows::Foundation::Size __cdecl winrt::Taskbar::implementation::TaskbarFrame::MeasureOverride(struct winrt::Windows::Foundation::Size))",
                },
                (void**)&TaskbarFrame_MeasureOverride_Original,
                (void*)TaskbarFrame_MeasureOverride_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))",
                },
                (void**)&IconView_IconView_Original,
                (void*)IconView_IconView_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
                },
                (void**)&TaskListButton_UpdateVisualStates_Original,
                (void*)TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {
                    LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateVisualStates(void))",
                },
                (void**)&ExperienceToggleButton_UpdateVisualStates_Original,
                (void*)ExperienceToggleButton_UpdateVisualStates_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowFlyoutList::OnApplyTemplate(void))",
                },
                (void**)&OverflowFlyoutList_OnApplyTemplate_Original,
                (void*)OverflowFlyoutList_OnApplyTemplate_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::SystemTray::implementation::ChevronSystemTrayIconDataModel2::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))",
                },
                (void**)&ChevronSystemTrayIconDataModel2_OnIconClicked_Original,
                (void*)ChevronSystemTrayIconDataModel2_OnIconClicked_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::Show(struct tagPOINT,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))",
                },
                (void**)&OverflowXamlIslandManager_Show_Original,
                (void*)OverflowXamlIslandManager_Show_Hook,
            },
        };

        return HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                                  ARRAYSIZE(symbolHooks));
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))",
            },
            (void**)&IconContainer_IsStorageRecreationRequired_Original,
            (void*)IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))",
            },
            (void**)&TrayUI_GetMinSize_Original,
            (void*)TrayUI_GetMinSize_Hook,
        },
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
    };

    return HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                              ARRAYSIZE(symbolHooks));
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)SHAppBarMessage, (void*)SHAppBarMessage_Hook,
                       (void**)&SHAppBarMessage_Original);

    Wh_SetFunctionHook((void*)GetWindowRect, (void*)GetWindowRect_Hook,
                       (void**)&GetWindowRect_Original);

    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

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

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();

    // This is required to give time for taskbar buttons of UWP apps to update
    // the layout.
    Sleep(400);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings(/*waitForApply=*/false);
}

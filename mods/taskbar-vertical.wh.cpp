// ==WindhawkMod==
// @id              taskbar-vertical
// @name            Vertical Taskbar for Windows 11
// @description     Finally, the missing vertical taskbar option for Windows 11!
// @version         1.2.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
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
# Vertical Taskbar for Windows 11

Finally, the missing vertical taskbar option for Windows 11!

The taskbar can be placed on the left or on the right, and a custom width can be
set in the mod settings.

## Compatibility

The mod was designed for up-to-date Windows 11 versions 22H2 to 24H2. Other
versions weren't tested and are probably not compatible.

Some of the other taskbar mods, such as [Taskbar height and icon
size](https://windhawk.net/mods/taskbar-icon-size), aren't compatible with this
mod.

## Funding

The development of this mod was funded by [AuthLite LLC](https://authlite.com/).
Thank you for contributing and allowing all Windhawk users to enjoy it!

![Screenshot](https://i.imgur.com/RBK5Mv9.png)

With labels:

![Screenshot with labels](https://i.imgur.com/JloORIB.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarLocation: left
  $name: Taskbar location
  $options:
  - left: Left
  - right: Right
- TaskbarWidth: 80
  $name: Taskbar width
  $description: >-
    The width, in pixels, of the taskbar

    Note: If the clock is too wide for the taskbar width you prefer, you can use
    the "Taskbar Clock Customization" mod to customize the taskbar clock format
- taskbarLocationSecondary: sameAsPrimary
  $name: Taskbar location on secondary monitors
  $options:
  - sameAsPrimary: Same as on primary monitor
  - left: Left
  - right: Right
- startMenuWidth: 0
  $name: Start menu width
  $description: >-
    Set to zero to use the system default width, set to a custom value if using
    a customized start menu, e.g. with the Windows 11 Start Menu Styler mod
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // must come before knownfolders.h

#include <knownfolders.h>
#include <shlobj.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <limits>
#include <list>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

enum class TaskbarLocation {
    left,
    right,
};

struct {
    TaskbarLocation taskbarLocation;
    TaskbarLocation taskbarLocationSecondary;
    int taskbarWidth;
    int startMenuWidth;
} g_settings;

enum class Target {
    Explorer,
    StartMenu,
    SearchHost,
    ShellExperienceHost,
    ShellHost,  // Win11 24H2.
};

Target g_target;

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
bool g_inChevronSystemTrayIconDataModel2_OnIconClicked;
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

// Available since Windows 10 version 1607, missing in older MinGW headers.
using GetThreadDescription_t =
    WINBASEAPI HRESULT(WINAPI*)(HANDLE hThread, PWSTR* ppszThreadDescription);
GetThreadDescription_t pGetThreadDescription;

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
    if (g_settings.taskbarLocation == g_settings.taskbarLocationSecondary) {
        return g_settings.taskbarLocation;
    }

    const POINT ptZero = {0, 0};
    HMONITOR primaryMonitor =
        MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

    return monitor == primaryMonitor ? g_settings.taskbarLocation
                                     : g_settings.taskbarLocationSecondary;
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

    if (!g_unloading) {
        int taskbarWidthScaled =
            MulDiv(g_settings.taskbarWidth, monitorDpiX, 96);

        rect->top = monitorRect.top;

        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                rect->left = monitorRect.left;
                rect->right = rect->left + taskbarWidthScaled;
                break;

            case TaskbarLocation::right:
                rect->right = monitorRect.right;
                rect->left = rect->right - taskbarWidthScaled;
                break;
        }
    } else {
        int taskbarOriginalHeightScaled =
            MulDiv(g_originalTaskbarHeight, monitorDpiY, 96);

        rect->top = rect->bottom - taskbarOriginalHeightScaled;
        rect->left = monitorRect.left;
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

    if (!g_unloading) {
        int taskbarWidthScaled =
            MulDiv(g_settings.taskbarWidth, monitorDpiX, 96);

        rect->top = monitorRect.top;

        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                rect->left = monitorRect.left;
                rect->right = rect->left + taskbarWidthScaled;
                break;

            case TaskbarLocation::right:
                rect->right = monitorRect.right;
                rect->left = rect->right - taskbarWidthScaled;
                break;
        }
    } else {
        int taskbarOriginalHeightScaled =
            MulDiv(g_originalTaskbarHeight, monitorDpiY, 96);

        rect->top = rect->bottom - taskbarOriginalHeightScaled;
        rect->left = monitorRect.left;
        rect->right = monitorRect.right;
    }
}

using GetWindowRect_t = decltype(&GetWindowRect);
GetWindowRect_t GetWindowRect_Original;

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND GetTaskbarWnd();

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

                int taskbarWidthScaled =
                    MulDiv(g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96);

                rect->top = monitorRect.top;

                switch (GetTaskbarLocationForMonitor(monitor)) {
                    case TaskbarLocation::left:
                        rect->left = monitorRect.left;
                        rect->right = rect->left + taskbarWidthScaled;
                        break;

                    case TaskbarLocation::right:
                        rect->right = monitorRect.right;
                        rect->left = rect->right - taskbarWidthScaled;
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

                    if (!(windowpos->flags & SWP_NOSIZE)) {
                        int taskbarWidthScaled = MulDiv(
                            g_settings.taskbarWidth, GetDpiForWindow(hWnd), 96);

                        windowpos->cx = taskbarWidthScaled;
                    }

                    if (!(windowpos->flags & SWP_NOMOVE) &&
                        GetTaskbarLocationForMonitor(monitor) ==
                            TaskbarLocation::right) {
                        RECT monitorRect;
                        GetMonitorRect(monitor, &monitorRect);

                        windowpos->x = monitorRect.right - windowpos->cx;
                    }
                }
            }
            break;
        }

        case WM_PAINT:
        case WM_ERASEBKGND: {
            Wh_Log(L"%04X: %08X", Msg, (DWORD)(ULONG_PTR)hWnd);

            // Calling CreateRectRgn posts window size change events which cause
            // element sizes and positions to be recalculated.
            SetWindowRgn(hWnd, nullptr, TRUE);
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

    RECT rc;
    GetMonitorRect(monitor, &rc);

    int taskbarWidthScaled = MulDiv(g_settings.taskbarWidth, monitorDpiX, 96);

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::left:
            point->X = rc.left + taskbarWidthScaled;
            break;

        case TaskbarLocation::right:
            point->X = rc.right - taskbarWidthScaled;
            point->X -= 159;
            break;
    }

    point->Y = pt.y;

    // Move a bit lower to vertically center the cursor on the close item.
    point->Y += MulDiv(30, monitorDpiY, 96);

    // Avoid returning a point too close to the top of the monitor which causes
    // the menu to be cut off.
    int minY = rc.top + MulDiv(130, monitorDpiY, 96);
    if (point->Y < minY) {
        point->Y = minY;
    }

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

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        winrt::Windows::Foundation::Rect rectNew = *rect;
        rectNew.Width = 72;

        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                rectNew.X = monitorInfo.rcWork.left;
                break;

            case TaskbarLocation::right:
                rectNew.X = monitorInfo.rcWork.right - rectNew.Width;
                break;
        }

        return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(
            pThis, param1, &rectNew, param3);
    }

    return XamlExplorerHostWindow_XamlExplorerHostWindow_Original(pThis, param1,
                                                                  rect, param3);
}

using ResourceDictionary_Lookup_t = winrt::Windows::Foundation::IInspectable*(
    WINAPI*)(void* pThis,
             void** result,
             winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_t ResourceDictionary_Lookup_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_Hook(void* pThis,
                               void** result,
                               winrt::Windows::Foundation::IInspectable* key) {
    // Wh_Log(L">");

    auto ret = ResourceDictionary_Lookup_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }

    auto keyString = key->try_as<winrt::hstring>();
    if (!keyString || keyString != L"TaskbarContextMenuMargin") {
        return ret;
    }

    auto valueThickness = ret->try_as<Thickness>();
    if (!valueThickness) {
        return ret;
    }

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        return ret;
    }

    RECT rc;
    if (!GetWindowRect_Original(hTaskbarWnd, &rc)) {
        return ret;
    }

    // TODO: Handle secondary taskbars.
    switch (g_settings.taskbarLocation) {
        case TaskbarLocation::left:
            valueThickness->Bottom -=
                MulDiv(rc.bottom - rc.top, 96, GetDpiForWindow(hTaskbarWnd));
            valueThickness->Bottom += g_settings.taskbarWidth;
            break;

        case TaskbarLocation::right:
            valueThickness->Bottom -=
                MulDiv(rc.bottom - rc.top, 96, GetDpiForWindow(hTaskbarWnd));
            valueThickness->Bottom -= 230;
            break;
    }

    Wh_Log(L"Overriding value %s", keyString->c_str());
    *ret = winrt::box_value(*valueThickness);

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

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    static LONG lastHeightOffset = []() -> LONG {
        // Find the last height offset to reset the height value.
        //
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB_180075641
        // 75 4a                   JNZ        LAB_180075641
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x200;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E && p[3] == 0xB3 &&
                p[8] == 0x7A && p[10] == 0x75) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return offset;
            }
        }

        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();

    if (lastHeightOffset > 0) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    }

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTrayController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

void* TaskbarController_OnGroupingModeChanged;

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    static LONG taskbarFrameOffset = []() -> LONG {
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged;
        if (p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }

        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();

    if (taskbarFrameOffset <= 0) {
        Wh_Log(L"taskbarFrameOffset <= 0");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"!taskbarFrame");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"!taskbarFrameElement");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // Set the width/height to NaN (Auto) to always match the parent
    // width/height.
    taskbarFrameElement.Width(std::numeric_limits<double>::quiet_NaN());

    // Setting the height right away can result in ellipsis.
    // https://github.com/ramensoftware/windhawk-mods/issues/981
    taskbarFrameElement.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
        [taskbarFrameElement]() {
            taskbarFrameElement.Height(
                std::numeric_limits<double>::quiet_NaN());
        });
}

using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    // Wh_Log(L">");

    if (g_inSystemTrayController_UpdateFrameSize) {
        Wh_Log(L">");
        // Set the system tray height to NaN, otherwise it may not match the
        // custom taskbar height.
        value = std::numeric_limits<double>::quiet_NaN();
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

bool UpdateNotifyIconsIfNeeded(XamlRoot xamlRoot);

bool ApplyStyle(FrameworkElement taskbarFrame,
                const winrt::Windows::Foundation::Size& size) {
    auto contentGrid =
        Media::VisualTreeHelper::GetParent(taskbarFrame).as<Controls::Grid>();

    double angle = g_unloading ? 0 : 90;
    Media::RotateTransform transform;
    transform.Angle(angle);
    contentGrid.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    contentGrid.RenderTransformOrigin({origin, origin});

    Thickness margin{};
    if (!g_unloading) {
        double marginValue = size.Height - g_settings.taskbarWidth;
        if (marginValue > 0) {
            margin.Top = marginValue;
        }

        // Fix the edge of the taskbar being non-clickable by moving the edge
        // pixel out of the screen.
        margin.Top += 1;
        margin.Bottom -= 1;
    }

    FrameworkElement child = contentGrid;
    if (child && (child = FindChildByName(child, L"TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid"))) {
        child.Margin(margin);
    }

    child = contentGrid;
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        child.Margin(margin);
    }

    auto xamlRoot = taskbarFrame.XamlRoot();
    if (xamlRoot) {
        try {
            UpdateNotifyIconsIfNeeded(xamlRoot);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    return true;
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 void* param1,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    void* param1,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;

    Wh_Log(L">");

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, param1, resultSize);

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(taskbarFrameElement));
    if (taskbarFrameElement) {
        try {
            ApplyStyle(taskbarFrameElement, *resultSize);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
}

using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    AugmentedEntryPointButton_UpdateButtonPadding_Original;
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;

    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);

    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}

using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
    Wh_Log(L">");

    RepeatButton_Width_Original(pThis, width);

    if (!g_inAugmentedEntryPointButton_UpdateButtonPadding) {
        return;
    }

    FrameworkElement button = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(button));
    if (!button) {
        return;
    }

    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(button, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
        return;
    }

    EnumChildElements(augmentedEntryPointContentGrid, [](FrameworkElement
                                                             child) {
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
            return false;
        }

        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
            return false;
        }

        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
            return false;
        }

        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());

        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (!widePanel) {
            double angle = g_unloading ? 0 : -90;

            Wh_Log(L"Setting angle=%f for child", angle);

            Media::RotateTransform transform;
            transform.Angle(angle);
            child.RenderTransform(transform);

            float origin = g_unloading ? 0 : 0.5;
            child.RenderTransformOrigin({origin, origin});

            return false;
        }

        FrameworkElement tickerGrid = panel;
        if ((tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Border")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Grid"))) {
            // OK.
        } else {
            return false;
        }

        FrameworkElement badgeSmall = tickerGrid;
        if ((badgeSmall = FindChildByName(badgeSmall, L"SmallTicker1")) &&
            (badgeSmall = FindChildByClassName(
                 badgeSmall, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeSmall =
                 FindChildByName(badgeSmall, L"BadgeAnchorSmallTicker"))) {
            double angle = g_unloading ? 0 : -90;

            Wh_Log(L"Setting angle=%f for small badge", angle);

            Media::RotateTransform transform;
            transform.Angle(angle);
            badgeSmall.RenderTransform(transform);

            float origin = g_unloading ? 0 : 0.5;
            badgeSmall.RenderTransformOrigin({origin, origin});
        }

        FrameworkElement badgeLarge = tickerGrid;
        if ((badgeLarge = FindChildByName(badgeLarge, L"LargeTicker1")) &&
            (badgeLarge = FindChildByClassName(
                 badgeLarge, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeLarge =
                 FindChildByName(badgeLarge, L"BadgeAnchorLargeTicker"))) {
            double angle = g_unloading ? 0 : -90;

            Wh_Log(L"Setting angle=%f for small badge", angle);

            Media::RotateTransform transform;
            transform.Angle(angle);
            badgeLarge.RenderTransform(transform);

            float origin = g_unloading ? 0 : 0.5;
            badgeLarge.RenderTransformOrigin({origin, origin});
        }

        return false;
    });
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
    auto containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        return;
    }

    auto contentGrid = FindChildByName(containerGrid, L"ContentGrid");
    if (!contentGrid) {
        auto contentPresenter =
            FindChildByName(containerGrid, L"ContentPresenter");
        if (!contentPresenter) {
            return;
        }

        contentGrid = FindChildByName(contentPresenter, L"ContentGrid");
        if (!contentGrid) {
            return;
        }
    }

    bool isDateTimeIcon = false;

    auto iconContent =
        FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
    if (!iconContent) {
        iconContent = FindChildByClassName(
            contentGrid, L"SystemTray.LanguageTextIconContent");

        if (!iconContent) {
            iconContent = FindChildByClassName(
                contentGrid, L"SystemTray.DateTimeIconContent");
            if (iconContent) {
                isDateTimeIcon = true;
            }
        }

        if (!iconContent) {
            return;
        }
    }

    double angle = g_unloading ? 0 : -90;
    Media::RotateTransform transform;
    transform.Angle(angle);
    iconContent.RenderTransform(transform);

    float origin = g_unloading ? 0 : 0.5;
    iconContent.RenderTransformOrigin({origin, origin});

    if (g_unloading) {
        iconContent.as<DependencyObject>().ClearValue(
            FrameworkElement::MaxHeightProperty());
    } else {
        iconContent.MaxHeight(isDateTimeIcon ? 40 : iconContent.ActualWidth());
    }

    if (isDateTimeIcon) {
        if (g_unloading) {
            iconContent.as<DependencyObject>().ClearValue(
                FrameworkElement::WidthProperty());
            iconContent.as<DependencyObject>().ClearValue(
                FrameworkElement::MarginProperty());
        } else {
            iconContent.Width(1000);

            // If the margin is set right away, it results in an invisible clock
            // on secondary taskbars on load.
            iconContent.Dispatcher().TryRunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                [iconContent]() {
                    double marginValue = -(1000.0 - 40) / 2;
                    iconContent.Margin(
                        Thickness{marginValue, 0, marginValue, 0});
                });
        }

        FrameworkElement stackPanel = nullptr;

        FrameworkElement child = iconContent;
        if ((child = FindChildByName(child, L"ContainerGrid")) &&
            (child = FindChildByClassName(
                 child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
            stackPanel = child;
        }

        Controls::TextBlock dateInnerTextBlock =
            stackPanel ? FindChildByName(stackPanel, L"DateInnerTextBlock")
                             .try_as<Controls::TextBlock>()
                       : nullptr;
        Controls::TextBlock timeInnerTextBlock =
            stackPanel ? FindChildByName(stackPanel, L"TimeInnerTextBlock")
                             .try_as<Controls::TextBlock>()
                       : nullptr;

        if (dateInnerTextBlock) {
            dateInnerTextBlock.TextAlignment(
                g_unloading ? TextAlignment::End : TextAlignment::Center);
        }

        if (timeInnerTextBlock) {
            timeInnerTextBlock.TextAlignment(
                g_unloading ? TextAlignment::End : TextAlignment::Center);
        }
    }
}

void ApplySystemTrayChevronIconViewStyle(
    FrameworkElement systemTrayChevronIconViewElement) {
    if (g_settings.taskbarLocation != TaskbarLocation::right) {
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

            if (className == L"SystemTray.NotifyIconView") {
                if (!IsChildOfElementByClassName(
                        iconView, L"SystemTray.NotificationAreaOverflow")) {
                    ApplyNotifyIconViewStyle(iconView);
                }
            } else if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon" &&
                    (IsChildOfElementByName(iconView, L"MainStack") ||
                     IsChildOfElementByName(iconView, L"NonActivatableStack") ||
                     IsChildOfElementByName(iconView, L"SecondaryClockStack") ||
                     IsChildOfElementByName(iconView, L"ControlCenterButton") ||
                     IsChildOfElementByName(iconView,
                                            L"NotificationCenterButton"))) {
                    ApplySystemTrayIconStyle(iconView);
                }
            } else if (className == L"SystemTray.ChevronIconView") {
                if (IsChildOfElementByName(iconView, L"NotifyIconStack")) {
                    ApplySystemTrayChevronIconViewStyle(iconView);
                }
            }
        });
}

bool ApplyStyleIfNeeded(XamlRoot xamlRoot) {
    // Calling this when unloading causes a crash with a secondary taskbar.
    if (g_unloading) {
        return true;
    }

    FrameworkElement contentGrid =
        xamlRoot.Content().try_as<FrameworkElement>();

    auto taskbarFrame = FindChildByName(contentGrid, L"TaskbarFrame");
    if (!taskbarFrame) {
        return true;
    }

    FrameworkElement rootGrid = FindChildByName(taskbarFrame, L"RootGrid");
    if (!rootGrid) {
        return true;
    }

    if (rootGrid.ActualHeight() == g_settings.taskbarWidth) {
        return true;
    }

    return ApplyStyle(taskbarFrame, taskbarFrame.ActualSize());
}

bool UpdateNotifyIconsIfNeeded(XamlRoot xamlRoot) {
    bool notifyIconsUpdated =
        std::find_if(g_notifyIconsUpdated.begin(), g_notifyIconsUpdated.end(),
                     [&xamlRoot](auto x) {
                         auto element = x.get();
                         return element && element == xamlRoot;
                     }) != g_notifyIconsUpdated.end();

    if (!g_unloading) {
        if (notifyIconsUpdated) {
            return true;
        }

        g_notifyIconsUpdated.push_back(winrt::make_weak(xamlRoot));
    } else {
        if (!notifyIconsUpdated) {
            return true;
        }

        g_notifyIconsUpdated.erase(
            std::remove_if(g_notifyIconsUpdated.begin(),
                           g_notifyIconsUpdated.end(),
                           [&xamlRoot](auto x) {
                               auto element = x.get();
                               return element && element == xamlRoot;
                           }),
            g_notifyIconsUpdated.end());
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

    for (PCWSTR containerName :
         {L"NotifyIconStack", L"MainStack", L"NonActivatableStack",
          L"SecondaryClockStack", L"ControlCenterButton",
          L"NotificationCenterButton"}) {
        FrameworkElement container =
            FindChildByName(systemTrayFrameGrid, containerName);
        if (!container) {
            continue;
        }

        FrameworkElement stackPanel = nullptr;

        if (wcscmp(containerName, L"ControlCenterButton") == 0 ||
            wcscmp(containerName, L"NotificationCenterButton") == 0) {
            FrameworkElement child = container;
            if ((child = FindChildByClassName(
                     child, L"Windows.UI.Xaml.Controls.Grid")) &&
                (child = FindChildByName(child, L"ContentPresenter")) &&
                (child = FindChildByClassName(
                     child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
                (child = FindChildByClassName(
                     child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
                stackPanel = child;
            }
        } else {
            FrameworkElement child = container;
            if ((child = FindChildByName(child, L"Content")) &&
                (child = FindChildByName(child, L"IconStack")) &&
                (child = FindChildByClassName(
                     child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
                (child = FindChildByClassName(
                     child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
                stackPanel = child;
            }
        }

        if (!stackPanel) {
            continue;
        }

        EnumChildElements(stackPanel, [containerName](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName !=
                L"Windows.UI.Xaml.Controls.ContentPresenter") {
                Wh_Log(L"Unsupported class name %s of child",
                       childClassName.c_str());
                return false;
            }

            if (wcscmp(containerName, L"NotifyIconStack") == 0) {
                FrameworkElement systemTrayChevronIconViewElement =
                    FindChildByClassName(child, L"SystemTray.ChevronIconView");
                if (!systemTrayChevronIconViewElement) {
                    Wh_Log(
                        L"Failed to get SystemTray.ChevronIconView of child");
                    return false;
                }

                ApplySystemTrayChevronIconViewStyle(
                    systemTrayChevronIconViewElement);
            } else {
                FrameworkElement systemTrayIconElement =
                    FindChildByName(child, L"SystemTrayIcon");
                if (!systemTrayIconElement) {
                    Wh_Log(L"Failed to get SystemTrayIcon of child");
                    return false;
                }

                ApplySystemTrayIconStyle(systemTrayIconElement);
            }

            return false;
        });
    }

    return true;
}

void UpdateTaskListButton(FrameworkElement taskListButtonElement) {
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

    // For some reason, translation is being set to a NaN.
    iconElement.Translation(
        winrt::Windows::Foundation::Numerics::float3::zero());

    auto labelControlElement =
        FindChildByName(iconPanelElement, L"LabelControl");
    if (labelControlElement) {
        double angle = g_unloading ? 0 : -90;
        Media::RotateTransform transform;
        transform.Angle(angle);
        labelControlElement.RenderTransform(transform);

        float origin = g_unloading ? 0 : 0.5;
        labelControlElement.RenderTransformOrigin({origin, origin});

        Controls::Grid::SetColumn(labelControlElement, g_unloading ? 1 : 0);

        Thickness margin{};
        if (!g_unloading) {
            margin.Left = -40 - g_settings.taskbarWidth / 2.0;
            margin.Top = 0;
            margin.Right = -g_settings.taskbarWidth / 2.0;
            margin.Bottom = iconElement.ActualWidth() + 20;
        }
        labelControlElement.Margin(margin);

        auto width = g_settings.taskbarWidth - iconElement.ActualWidth() - 20;

        labelControlElement.MinWidth(g_unloading ? 0 : width);
        labelControlElement.MaxWidth(g_unloading ? 136 : width);
    }

    Thickness margin{};
    if (!g_unloading && labelControlElement) {
        margin.Top = g_settings.taskbarWidth - iconElement.ActualWidth() - 24;
    }
    iconElement.Margin(margin);

    for (PCWSTR badgeElementName : {
             // Badge for non-UWP apps.
             L"OverlayIcon",
             // Badge for UWP apps.
             L"BadgeControl",
         }) {
        auto badgeElement = FindChildByName(iconPanelElement, badgeElementName);
        if (badgeElement) {
            double angle = g_unloading ? 0 : -90;
            Media::RotateTransform transform;
            transform.Angle(angle);
            badgeElement.RenderTransform(transform);

            winrt::Windows::Foundation::Point origin{};
            if (!g_unloading) {
                origin.Y = labelControlElement ? 1.25 : 0.75;
            }

            badgeElement.RenderTransformOrigin(origin);

            badgeElement.Margin(margin);
        }
    }
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

    try {
        UpdateTaskListButton(taskListButtonElement);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }

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

void ApplyExperienceButtonStyle(FrameworkElement toggleButtonElement) {
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

    iconElement.MaxHeight(g_unloading ? std::numeric_limits<double>::infinity()
                                      : 24);

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
    ApplyExperienceButtonStyle(toggleButtonElement);
}

using SearchBoxButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
SearchBoxButton_UpdateVisualStates_t
    SearchBoxButton_UpdateVisualStates_Original;
void WINAPI SearchBoxButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    SearchBoxButton_UpdateVisualStates_Original(pThis);

    void* toggleButtonIUnknownPtr = (void**)pThis + 2;
    winrt::Windows::Foundation::IUnknown toggleButtonIUnknown;
    winrt::copy_from_abi(toggleButtonIUnknown, toggleButtonIUnknownPtr);

    auto toggleButtonElement = toggleButtonIUnknown.as<FrameworkElement>();
    ApplyExperienceButtonStyle(toggleButtonElement);
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

        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                point.x += MulDiv(104 + 12, monitorDpiX, 96);
                break;

            case TaskbarLocation::right:
                point.x -= MulDiv(104 + 12, monitorDpiX, 96);
                break;
        }

        point.y += MulDiv(28, monitorDpiY, 96);
    }

    OverflowXamlIslandManager_Show_Original(pThis, point, param2);
}

using CopilotIcon_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
CopilotIcon_UpdateVisualStates_t CopilotIcon_UpdateVisualStates_Original;
void WINAPI CopilotIcon_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    CopilotIcon_UpdateVisualStates_Original(pThis);

    FrameworkElement copilotIcon = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(copilotIcon));
    if (!copilotIcon) {
        return;
    }

    FrameworkElement child = copilotIcon;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByName(child, L"LottieIcon"))) {
        double angle = g_unloading ? 0 : -90;
        Media::RotateTransform transform;
        transform.Angle(angle);
        child.RenderTransform(transform);

        float origin = g_unloading ? 0 : 0.5;
        child.RenderTransformOrigin({origin, origin});
    }
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

bool UpdateCopilotPosition() {
    bool updatedPosition = false;

    auto enumWindowProc = [&](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD style = GetWindowStyle(hWnd);
        if (style & WS_CAPTION) {
            return TRUE;
        }

        DWORD exStyle = GetWindowExStyle(hWnd);
        if (!(exStyle & WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        DWORD dwProcessId = 0;
        if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
            _wcsicmp(GetProcessFileName(dwProcessId).c_str(), L"msedge.exe") !=
                0) {
            return TRUE;
        }

        RECT rc{};
        GetWindowRect_Original(hWnd, &rc);

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        if (rc.top != monitorInfo.rcWork.top ||
            rc.bottom != monitorInfo.rcWork.bottom ||
            rc.right != monitorInfo.rcMonitor.right) {
            return TRUE;
        }

        if (monitorInfo.rcWork.right != monitorInfo.rcMonitor.right) {
            int x = rc.left - monitorInfo.rcMonitor.right +
                    monitorInfo.rcWork.right;
            int y = rc.top;
            SetWindowPos(hWnd, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        updatedPosition = true;
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowProc)*>(lParam);
            return proc(hWnd, lParam);
        },
        reinterpret_cast<LPARAM>(&enumWindowProc));

    return updatedPosition;
}

using CopilotIcon_ToggleEdgeCopilot_t = void(WINAPI*)(void* pThis);
CopilotIcon_ToggleEdgeCopilot_t CopilotIcon_ToggleEdgeCopilot_Original;
void WINAPI CopilotIcon_ToggleEdgeCopilot_Hook(void* pThis) {
    Wh_Log(L">");

    CopilotIcon_ToggleEdgeCopilot_Original(pThis);

    if (g_settings.taskbarLocation != TaskbarLocation::right) {
        return;
    }

    g_copilotPosTimerCounter = 0;
    g_copilotPosTimer = SetTimer(
        nullptr, g_copilotPosTimer, 100,
        [](HWND hwnd,         // handle of window for timer messages
           UINT uMsg,         // WM_TIMER message
           UINT_PTR idEvent,  // timer identifier
           DWORD dwTime       // current system time
           ) WINAPI {
            g_copilotPosTimerCounter++;
            if (UpdateCopilotPosition() || g_copilotPosTimerCounter >= 10) {
                KillTimer(nullptr, g_copilotPosTimer);
                g_copilotPosTimer = 0;
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

BOOL WINAPI GetWindowRect_Hook(HWND hWnd, LPRECT lpRect) {
    BOOL ret = GetWindowRect_Original(hWnd, lpRect);
    if (ret && !g_unloading &&
        (g_inCTaskListThumbnailWnd_DisplayUI ||
         g_inCTaskListThumbnailWnd_LayoutThumbnails) &&
        IsTaskbarWindow(hWnd) &&
        GetCurrentThreadId() == GetWindowThreadProcessId(hWnd, nullptr)) {
        Wh_Log(L"Adjusting taskbar rect for TaskListThumbnailWnd");

        // Fix thumbnails always displaying as list.
        HMONITOR monitor = MonitorFromRect(lpRect, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        CopyRect(lpRect, &monitorInfo.rcWork);
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

    if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) ||
        (!g_inCTaskListThumbnailWnd_DisplayUI &&
         !g_inCTaskListThumbnailWnd_LayoutThumbnails)) {
        return original();
    }

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

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    RECT rc{};
    if (g_inCTaskListThumbnailWnd_DisplayUI) {
        const int distance = MulDiv(12, GetDpiForWindow(hWnd), 96);

        SIZE sz{
            .cx = cx + distance * 2,
            .cy = cy + distance * 2,
        };

        UINT alignment;
        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                alignment = TPM_LEFTALIGN;
                break;

            case TaskbarLocation::right:
                alignment = TPM_RIGHTALIGN;
                break;
        }

        CalculatePopupWindowPosition(
            &pt, &sz, alignment | TPM_VCENTERALIGN | TPM_WORKAREA, nullptr,
            &rc);

        rc.left += distance;
        rc.right -= distance;
        rc.top += distance;
        rc.bottom -= distance;
    } else {
        // Keep current position.
        GetWindowRect_Original(hWnd, &rc);
        rc.bottom = rc.top + cy;

        switch (GetTaskbarLocationForMonitor(monitor)) {
            case TaskbarLocation::left:
                rc.right = rc.left + cx;
                break;

            case TaskbarLocation::right:
                rc.left = rc.right - cx;
                break;
        }
    }

    Wh_Log(L"Adjusting pos for TaskListThumbnailWnd: %dx%d, %dx%d", rc.left,
           rc.right, rc.top, rc.bottom);

    return SetWindowPos_Original(hWnd, hWndInsertAfter, rc.left, rc.top,
                                 rc.right - rc.left, rc.bottom - rc.top,
                                 uFlags);
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;
BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            WINBOOL bRepaint) {
    auto original = [&]() {
        return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
    };

    if (g_unloading) {
        return original();
    }

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return original();
    }

    if (!IsTaskbarWindow(GetAncestor(hWnd, GA_ROOT))) {
        return original();
    }

    Wh_Log(L">");

    return MoveWindow_Original(hWnd, X, Y, nHeight, nHeight, bRepaint);
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
        HANDLE thread =
            OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (!thread) {
            return false;
        }

        PWSTR threadDescription;
        HRESULT hr = pGetThreadDescription
                         ? pGetThreadDescription(thread, &threadDescription)
                         : E_FAIL;
        CloseHandle(thread);
        if (FAILED(hr)) {
            return false;
        }

        bool isActionCenter = wcscmp(threadDescription, L"ActionCenter") == 0;
        bool isQuickActions = wcscmp(threadDescription, L"QuickActions") == 0;

        Wh_Log(L"%s", threadDescription);
        LocalFree(threadDescription);

        if (!isActionCenter && !isQuickActions) {
            return false;
        }

        if (isQuickActions && extraXAdjustment &&
            g_settings.taskbarLocation == TaskbarLocation::left) {
            *extraXAdjustment = MulDiv(-29, GetDpiForWindow(hWnd), 96);
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

    if (g_unloading) {
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        *width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        *height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
        return;
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    const int w1 =
        MulDiv(g_settings.startMenuWidth ? g_settings.startMenuWidth : 660,
               monitorDpiX, 96);
    if (*width > w1) {
        *width = w1;
    }

    const int h1 = MulDiv(750, monitorDpiY, 96);
    const int h2 = MulDiv(694, monitorDpiY, 96);
    if (*height >= h1) {
        *height = h1;
    } else if (*height >= h2) {
        *height = h2;
    }
}

void AdjustCoreWindowPos(int* x, int* y, int width, int height) {
    if (g_unloading) {
        if (g_target == Target::StartMenu) {
            *x = 0;
            *y = 0;
        }

        return;
    }

    const POINT pt = {*x, *y};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    RECT rc;
    if (!GetMonitorRect(monitor, &rc)) {
        return;
    }

    int taskbarWidthScaled = MulDiv(g_settings.taskbarWidth, monitorDpiX, 96);

    switch (GetTaskbarLocationForMonitor(monitor)) {
        case TaskbarLocation::left:
            *x = rc.left + taskbarWidthScaled;
            break;

        case TaskbarLocation::right:
            *x = rc.right - width - taskbarWidthScaled;
            break;
    }

    *y = rc.top;
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
    g_settings.taskbarLocation = TaskbarLocation::left;
    if (wcscmp(taskbarLocation, L"right") == 0) {
        g_settings.taskbarLocation = TaskbarLocation::right;
    }
    Wh_FreeStringSetting(taskbarLocation);

    PCWSTR taskbarLocationSecondary =
        Wh_GetStringSetting(L"taskbarLocationSecondary");
    g_settings.taskbarLocationSecondary = g_settings.taskbarLocation;
    if (wcscmp(taskbarLocationSecondary, L"left") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::left;
    } else if (wcscmp(taskbarLocationSecondary, L"right") == 0) {
        g_settings.taskbarLocationSecondary = TaskbarLocation::right;
    }
    Wh_FreeStringSetting(taskbarLocationSecondary);

    g_settings.taskbarWidth = Wh_GetIntSetting(L"TaskbarWidth");
    g_settings.startMenuWidth = Wh_GetIntSetting(L"startMenuWidth");
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

    // Calling CreateRectRgn posts window size change events which cause element
    // sizes and positions to be recalculated.
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            if (IsTaskbarWindow(hWnd)) {
                SetWindowRgn(hWnd, nullptr, TRUE);
            }

            return TRUE;
        },
        0);

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
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                },
                (void**)&ResourceDictionary_Lookup_Original,
                (void*)ResourceDictionary_Lookup_Hook,
            },
            {
                {
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                },
                (void**)&SystemTrayController_GetFrameSize_Original,
                (void*)SystemTrayController_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2, inlined sometimes.
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
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))",
                },
                (void**)&SystemTrayController_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))",
                },
                (void**)&TaskbarController_OnGroupingModeChanged,
            },
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))",
                },
                (void**)&TaskbarController_UpdateFrameHeight_Original,
                (void*)TaskbarController_UpdateFrameHeight_Hook,
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
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
                },
                (void**)&TaskbarFrame_MeasureOverride_Original,
                (void*)TaskbarFrame_MeasureOverride_Hook,
            },
            {
                {
                    LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))",
                },
                (void**)&AugmentedEntryPointButton_UpdateButtonPadding_Original,
                (void*)AugmentedEntryPointButton_UpdateButtonPadding_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const )",
                },
                (void**)&RepeatButton_Width_Original,
                (void*)RepeatButton_Width_Hook,
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
                    LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::SearchBoxButton::UpdateVisualStates(void))",
                },
                (void**)&SearchBoxButton_UpdateVisualStates_Original,
                (void*)SearchBoxButton_UpdateVisualStates_Hook,
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
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::CopilotIcon::UpdateVisualStates(void))",
                },
                (void**)&CopilotIcon_UpdateVisualStates_Original,
                (void*)CopilotIcon_UpdateVisualStates_Hook,
            },
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::CopilotIcon::ToggleEdgeCopilot(void))",
                },
                (void**)&CopilotIcon_ToggleEdgeCopilot_Original,
                (void*)CopilotIcon_ToggleEdgeCopilot_Hook,
            },
            {
                {
                    LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowFlyoutModel::Show(void))",
                },
                (void**)&OverflowFlyoutModel_Show_Original,
                (void*)OverflowFlyoutModel_Show_Hook,
            },
        };

        if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
            return false;
        }
    }

    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        Wh_SetFunctionHook(
            (void*)SystemTrayController_UpdateFrameSize_SymbolAddress,
            (void*)SystemTrayController_UpdateFrameSize_Hook,
            (void**)&SystemTrayController_UpdateFrameSize_Original);
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

    Wh_SetFunctionHook((void*)GetWindowRect, (void*)GetWindowRect_Hook,
                       (void**)&GetWindowRect_Original);

    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);

    Wh_SetFunctionHook((void*)MoveWindow, (void*)MoveWindow_Hook,
                       (void**)&MoveWindow_Original);

    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE kernel32Module = LoadLibrary(L"kernel32.dll")) {
        pGetThreadDescription = (GetThreadDescription_t)GetProcAddress(
            kernel32Module, "GetThreadDescription");
    }

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

    if (g_target == Target::StartMenu || g_target == Target::SearchHost ||
        g_target == Target::ShellExperienceHost ||
        g_target == Target::ShellHost) {
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

    if (g_target == Target::Explorer) {
        ApplySettings();
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost ||
               g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        ApplySettings();

        // This is required to give time for taskbar buttons of UWP apps to
        // update the layout.
        Sleep(400);
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost ||
               g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
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

    if (g_target == Target::Explorer) {
        ApplySettings(/*waitForApply=*/false);
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost ||
               g_target == Target::ShellExperienceHost ||
               g_target == Target::ShellHost) {
        CoreWindowUI::ApplySettings();
    }
}

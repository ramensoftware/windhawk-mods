// ==WindhawkMod==
// @id              taskbar-on-top
// @name            Taskbar on top for Windows 11
// @description     Moves the Windows 11 taskbar to the top of the screen
// @version         1.1.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lversion
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

## Known limitations

* The Action Center (Win+A) stays on the bottom. For now, you can use [this
  alternative
  solution](https://github.com/ramensoftware/windhawk-mods/issues/1053#issuecomment-2405461863).
* For some devices, mostly tablets and touchscreen devices, the taskbar may
  appear in the wrong location after enabling the mod. An explorer restart
  usually fixes it.
* The option to automatically hide the taskbar isn't supported.

![Screenshot](https://i.imgur.com/LqBwGVn.png)
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
- runningIndicatorsOnTop: false
  $name: Running indicators on top
  $description: Show running indicators above the taskbar icons
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <functional>
#include <list>

#ifdef _M_ARM64
#include <regex>
#endif

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
    bool runningIndicatorsOnTop;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

bool g_inCTaskListThumbnailWnd_DisplayUI;
bool g_inCTaskListThumbnailWnd_LayoutThumbnails;
bool g_inOverflowFlyoutModel_Show;
int g_lastTaskbarAlignment;

std::atomic<DWORD> g_UpdateFlyoutPosition_threadId;
void* g_UpdateFlyoutPosition_pThis;

winrt::Windows::Foundation::Size g_lastFlyoutPositionSize;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_elementLoadedAutoRevokerList;

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

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

bool IsVersionAtLeast(WORD major, WORD minor, WORD build, WORD qfe) {
    static VS_FIXEDFILEINFO* fixedFileInfo =
        GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return false;
    }

    WORD moduleMajor = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleMinor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleBuild = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD moduleQfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    if (moduleMajor != major) {
        return moduleMajor > major;
    }

    if (moduleMinor != minor) {
        return moduleMinor > minor;
    }

    if (moduleBuild != build) {
        return moduleBuild > build;
    }

    return moduleQfe >= qfe;
}

bool GetMonitorRect(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    return GetMonitorInfo(monitor, &monitorInfo) &&
           CopyRect(rc, &monitorInfo.rcMonitor);
}

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

    if (taskbarPos != ABE_BOTTOM) {
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

using TrayUI_GetStuckInfo_t = void(WINAPI*)(void* pThis,
                                            RECT* rect,
                                            DWORD* taskbarPos);
TrayUI_GetStuckInfo_t TrayUI_GetStuckInfo_Original;
void WINAPI TrayUI_GetStuckInfo_Hook(void* pThis,
                                     RECT* rect,
                                     DWORD* taskbarPos) {
    Wh_Log(L">");

    TrayUI_GetStuckInfo_Original(pThis, rect, taskbarPos);

    switch (g_settings.taskbarLocation) {
        case TaskbarLocation::top:
            *taskbarPos = ABE_TOP;
            break;

        case TaskbarLocation::bottom:
            *taskbarPos = ABE_BOTTOM;
            break;
    }
}

void TaskbarWndProcPreProcess(HWND hWnd,
                              UINT Msg,
                              WPARAM* wParam,
                              LPARAM* lParam) {
    switch (Msg) {
        case 0x5C3: {
            // On Windows 11 23H2, setting the taskbar location here also causes
            // the start menu to be opened on the left of the screen, even if
            // the icons on the taskbar are centered. Therefore, only set it if
            // the icons are aligned to left, not centered. The drawback is that
            // the jump list animations won't be correct in this case.
            if (g_lastTaskbarAlignment == 1 &&
                !IsVersionAtLeast(10, 0, 26100, 0)) {
                break;
            }

            // The taskbar location that affects the jump list animations.
            if (*wParam == ABE_BOTTOM) {
                HMONITOR monitor = (HMONITOR)lParam;
                if (GetTaskbarLocationForMonitor(monitor) ==
                    TaskbarLocation::top) {
                    *wParam = ABE_TOP;
                }
            }
            break;
        }
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

    TaskbarWndProcPreProcess(hWnd, Msg, &wParam, &lParam);

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

    TaskbarWndProcPreProcess(hWnd, Msg, &wParam, &lParam);

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

    // Place at the bottom of the monitor, will reposition later in
    // SetWindowPos.
    point->Y = monitorInfo.rcWork.bottom - 1;

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

using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                          int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;
HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis,
                                                   int* alignment) {
    Wh_Log(L">");

    HRESULT ret = ITaskbarSettings_get_Alignment_Original(pThis, alignment);
    if (SUCCEEDED(ret)) {
        Wh_Log(L"alignment=%d", *alignment);
        g_lastTaskbarAlignment = *alignment;
    }

    return ret;
}

bool IsSecondaryTaskbar(XamlRoot xamlRoot) {
    FrameworkElement controlCenterButton = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid")) &&
        (child = FindChildByName(child, L"ControlCenterButton"))) {
        controlCenterButton = child;
    }

    if (!controlCenterButton) {
        return false;
    }

    // On secondary taskbars, the element that holds the system icons is empty
    // and has the width of 2.
    return controlCenterButton.ActualWidth() < 5;
}

void ApplyTaskbarFrameStyle(FrameworkElement taskbarFrame) {
    if (g_settings.taskbarLocation != TaskbarLocation::top) {
        return;
    }

    FrameworkElement backgroundStroke = nullptr;

    FrameworkElement child = taskbarFrame;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"BackgroundControl")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"BackgroundStroke"))) {
        backgroundStroke = child;
    }

    if (!backgroundStroke) {
        return;
    }

    backgroundStroke.VerticalAlignment(VerticalAlignment::Bottom);
}

void* TaskbarController_OnGroupingModeChanged;

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    static LONG taskbarFrameOffset = []() -> LONG {
#if defined(_M_X64)
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#elif defined(_M_ARM64)
        // 00000001`806b1810 a9bf7bfd stp fp,lr,[sp,#-0x10]!
        // 00000001`806b1814 910003fd mov fp,sp
        // 00000001`806b1818 aa0003e8 mov x8,x0
        // 00000001`806b181c f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#else
#error "Unsupported architecture"
#endif

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

    // A workaround to issues related to tablet mode.
    // https://github.com/ramensoftware/windhawk-mods/issues/529#issuecomment-2419371239
    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // Adjust parent grid height if needed.
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }
}

using TaskbarFrame_TaskbarFrame_t = void*(WINAPI*)(void* pThis);
TaskbarFrame_TaskbarFrame_t TaskbarFrame_TaskbarFrame_Original;
void* WINAPI TaskbarFrame_TaskbarFrame_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = TaskbarFrame_TaskbarFrame_Original(pThis);

    FrameworkElement taskbarFrame = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrame));
    if (!taskbarFrame) {
        return ret;
    }

    g_elementLoadedAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_elementLoadedAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = taskbarFrame.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_elementLoadedAutoRevokerList.erase(autoRevokerIt);

            auto taskbarFrame = sender.try_as<FrameworkElement>();
            if (!taskbarFrame) {
                return;
            }

            auto className = winrt::get_class_name(taskbarFrame);
            Wh_Log(L"className: %s", className.c_str());

            try {
                ApplyTaskbarFrameStyle(taskbarFrame);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        });

    return ret;
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

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_elementLoadedAutoRevokerList.emplace_back();
    auto autoRevokerIt = g_elementLoadedAutoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_elementLoadedAutoRevokerList.erase(autoRevokerIt);

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

    return ret;
}

void UpdateTaskListButton(FrameworkElement taskListButtonElement) {
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    bool indicatorsOnTop = false;
    if (!g_unloading && g_settings.runningIndicatorsOnTop) {
        auto taskbarFrameRepeaterElement =
            Media::VisualTreeHelper::GetParent(taskListButtonElement)
                .as<FrameworkElement>();

        bool isSecondaryTaskbar = false;
        if (!taskbarFrameRepeaterElement ||
            taskbarFrameRepeaterElement.Name() != L"TaskbarFrameRepeater") {
            // TODO: Can also be "OverflowFlyoutListRepeater".
        } else {
            isSecondaryTaskbar =
                IsSecondaryTaskbar(taskListButtonElement.XamlRoot());
        }

        TaskbarLocation taskbarLocation =
            isSecondaryTaskbar ? g_settings.taskbarLocationSecondary
                               : g_settings.taskbarLocation;
        if (taskbarLocation == TaskbarLocation::top) {
            indicatorsOnTop = true;
        }
    }

    PCWSTR indicatorClassNames[] = {
        L"RunningIndicator",
        L"ProgressIndicator",
    };
    for (auto indicatorClassName : indicatorClassNames) {
        auto indicatorElement =
            FindChildByName(iconPanelElement, indicatorClassName);
        if (!indicatorElement) {
            continue;
        }

        indicatorElement.VerticalAlignment(indicatorsOnTop
                                               ? VerticalAlignment::Top
                                               : VerticalAlignment::Bottom);
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
}

using OverflowFlyoutModel_Show_t = void(WINAPI*)(void* pThis);
OverflowFlyoutModel_Show_t OverflowFlyoutModel_Show_Original;
void WINAPI OverflowFlyoutModel_Show_Hook(void* pThis) {
    Wh_Log(L">");

    g_inOverflowFlyoutModel_Show = true;

    OverflowFlyoutModel_Show_Original(pThis);

    g_inOverflowFlyoutModel_Show = false;
}

using FlyoutFrame_UpdateFlyoutPosition_t = void(WINAPI*)(void* pThis);
FlyoutFrame_UpdateFlyoutPosition_t FlyoutFrame_UpdateFlyoutPosition_Original;
void WINAPI FlyoutFrame_UpdateFlyoutPosition_Hook(void* pThis) {
    Wh_Log(L">");

    g_UpdateFlyoutPosition_threadId = GetCurrentThreadId();
    g_UpdateFlyoutPosition_pThis = pThis;

    FlyoutFrame_UpdateFlyoutPosition_Original(pThis);

    g_UpdateFlyoutPosition_threadId = 0;
    g_UpdateFlyoutPosition_pThis = nullptr;
}

using MenuFlyout_ShowAt_t =
    void*(WINAPI*)(void* pThis,
                   DependencyObject* placementTarget,
                   Controls::Primitives::FlyoutShowOptions* showOptions);
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

bool HandleSystemTrayContextMenu(FrameworkElement element) {
    Wh_Log(L">");

    FrameworkElement childElement = FindChildByName(element, L"ContainerGrid");
    if (!childElement) {
        Wh_Log(L"No child element");
        return false;
    }

    auto flyout =
        Controls::Primitives::FlyoutBase::GetAttachedFlyout(childElement);
    if (!flyout) {
        Wh_Log(L"No flyout");
        return false;
    }

    Controls::Primitives::FlyoutShowOptions options;
    options.Position(winrt::Windows::Foundation::Point{
        static_cast<float>(childElement.ActualWidth()),
        static_cast<float>(childElement.ActualHeight())});
    flyout.ShowAt(childElement, options);
    return true;
}

using TextIconContent_ShowContextMenu_t = void(WINAPI*)(void* pThis);
TextIconContent_ShowContextMenu_t TextIconContent_ShowContextMenu_Original;
void WINAPI TextIconContent_ShowContextMenu_Hook(void* pThis) {
    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
    if (!element || !HandleSystemTrayContextMenu(element)) {
        TextIconContent_ShowContextMenu_Original(pThis);
    }
}

using DateTimeIconContent_ShowContextMenu_t = void(WINAPI*)(void* pThis);
DateTimeIconContent_ShowContextMenu_t
    DateTimeIconContent_ShowContextMenu_Original;
void WINAPI DateTimeIconContent_ShowContextMenu_Hook(void* pThis) {
    Wh_Log(L">");

    FrameworkElement element = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
    if (!element || !HandleSystemTrayContextMenu(element)) {
        DateTimeIconContent_ShowContextMenu_Original(pThis);
    }
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

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0) {
        if (uFlags & SWP_NOMOVE) {
            return original();
        }

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

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        if (g_inCTaskListThumbnailWnd_DisplayUI) {
            Y = monitorInfo.rcWork.top + MulDiv(12, monitorDpiY, 96);
        } else {
            // Keep current position.
            RECT rc;
            GetWindowRect(hWnd, &rc);
            Y = rc.top;
        }
    } else if (_wcsicmp(szClassName, L"TopLevelWindowForOverflowXamlIsland") ==
                   0 ||
               _wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0) {
        if (uFlags & (SWP_NOMOVE | SWP_NOSIZE)) {
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

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        bool adjusted = false;

        WCHAR rootOwnerClassName[64];
        if (_wcsicmp(szClassName, L"Xaml_WindowedPopupClass") == 0 &&
            GetWindowThreadProcessId(hWnd, nullptr) ==
                GetWindowThreadProcessId(FindCurrentProcessTaskbarWnd(),
                                         nullptr) &&
            GetClassName(GetAncestor(hWnd, GA_ROOTOWNER), rootOwnerClassName,
                         ARRAYSIZE(rootOwnerClassName))) {
            if (_wcsicmp(rootOwnerClassName, L"XamlExplorerHostIslandWindow") ==
                0) {
                // Probably hovering a XAML thumbnail preview, make it so that
                // the tooltip doesn't cover the thumbnail preview.
                Y = monitorInfo.rcWork.top +
                    MulDiv(10 + g_lastFlyoutPositionSize.Height, monitorDpiY,
                           96);
                adjusted = true;
            } else if (_wcsicmp(rootOwnerClassName,
                                L"TopLevelWindowForOverflowXamlIsland") == 0) {
                // Don't adjust to prevent tooltips from covering the overflow
                // flyout.
                adjusted = true;
            }
        }

        if (!adjusted) {
            if (Y < monitorInfo.rcWork.top) {
                Y = monitorInfo.rcWork.top;
            } else if (Y > monitorInfo.rcWork.bottom - cy) {
                Y = monitorInfo.rcWork.bottom - cy;
            }
        }
    } else if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        if (uFlags & SWP_NOMOVE) {
            return original();
        }

        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (!threadId) {
            return original();
        }

        HANDLE thread =
            OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (!thread) {
            return original();
        }

        PWSTR threadDescription;
        HRESULT hr = pGetThreadDescription
                         ? pGetThreadDescription(thread, &threadDescription)
                         : E_FAIL;
        CloseHandle(thread);
        if (FAILED(hr)) {
            return original();
        }

        bool isJumpViewUI = wcscmp(threadDescription, L"JumpViewUI") == 0;

        LocalFree(threadDescription);

        if (!isJumpViewUI) {
            return original();
        }

        POINT pt;
        GetCursorPos(&pt);

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        Y = monitorInfo.rcWork.top;

        // If hovering over the overflow window, exclude it.
        HWND windowFromPoint = WindowFromPoint(pt);
        if (windowFromPoint &&
            GetWindowThreadProcessId(windowFromPoint, nullptr) ==
                GetWindowThreadProcessId(FindCurrentProcessTaskbarWnd(),
                                         nullptr)) {
            WCHAR szClassNameFromPoint[64];
            if (GetClassName(windowFromPoint, szClassNameFromPoint,
                             ARRAYSIZE(szClassNameFromPoint)) &&
                _wcsicmp(szClassNameFromPoint,
                         L"XamlExplorerHostIslandWindow") == 0) {
                UINT monitorDpiX = 96;
                UINT monitorDpiY = 96;
                GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                 &monitorDpiY);

                int overflowHeight = MulDiv(54 + 12, monitorDpiY, 96);

                Y += overflowHeight;
            }
        }
    } else {
        return original();
    }

    Wh_Log(L"Adjusting pos for %s: %dx%d, %dx%d", szClassName, X, Y, X + cx,
           Y + cy);

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;
BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    auto original = [=]() {
        return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
    };

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"XamlExplorerHostIslandWindow") == 0) {
        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (!threadId) {
            return original();
        }

        HANDLE thread =
            OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (!thread) {
            return original();
        }

        PWSTR threadDescription;
        HRESULT hr = pGetThreadDescription
                         ? pGetThreadDescription(thread, &threadDescription)
                         : E_FAIL;
        CloseHandle(thread);
        if (FAILED(hr)) {
            return original();
        }

        bool isMultitaskingView =
            wcscmp(threadDescription, L"MultitaskingView") == 0;

        LocalFree(threadDescription);

        if (!isMultitaskingView) {
            return original();
        }

        POINT pt;
        GetCursorPos(&pt);

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
            return original();
        }

        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        UINT monitorDpiX = 96;
        UINT monitorDpiY = 96;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

        Y = monitorInfo.rcWork.top + MulDiv(12, monitorDpiY, 96);
    } else {
        return original();
    }

    Wh_Log(L"Adjusting pos for %s: %dx%d, %dx%d", szClassName, X, Y, X + nWidth,
           Y + nHeight);

    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

using MapWindowPoints_t = decltype(&MapWindowPoints);
MapWindowPoints_t MapWindowPoints_Original;
int WINAPI MapWindowPoints_Hook(HWND hWndFrom,
                                HWND hWndTo,
                                LPPOINT lpPoints,
                                UINT cPoints) {
    int ret = MapWindowPoints_Original(hWndFrom, hWndTo, lpPoints, cPoints);

    if (GetCurrentThreadId() != g_UpdateFlyoutPosition_threadId ||
        !g_UpdateFlyoutPosition_pThis || cPoints != 1) {
        return ret;
    }

    Wh_Log(L">");

    FrameworkElement flyoutFrame = nullptr;
    ((IUnknown**)g_UpdateFlyoutPosition_pThis)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(flyoutFrame));
    if (!flyoutFrame) {
        Wh_Log(L"Error getting flyoutFrame");
        return ret;
    }

    FrameworkElement hoverFlyoutCanvas =
        FindChildByName(flyoutFrame, L"HoverFlyoutCanvas");
    if (!hoverFlyoutCanvas) {
        Wh_Log(L"No HoverFlyoutCanvas");
        return ret;
    }

    Controls::Grid hoverFlyoutGrid =
        FindChildByName(hoverFlyoutCanvas, L"HoverFlyoutGrid")
            .try_as<Controls::Grid>();
    if (!hoverFlyoutGrid) {
        Wh_Log(L"No HoverFlyoutGrid");
        return ret;
    }

    auto flyoutPositionSize = hoverFlyoutGrid.DesiredSize();
    g_lastFlyoutPositionSize = flyoutPositionSize;

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) != TaskbarLocation::top) {
        return ret;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    int flyoutHeight = MulDiv(flyoutPositionSize.Height, monitorDpiY, 96);

    // Align to bottom instead of top.
    lpPoints->y += flyoutHeight;

    // Add work area space.
    lpPoints->y += monitorInfo.rcWork.top - monitorInfo.rcMonitor.top;

    // Add margin.
    lpPoints->y += MulDiv(12, monitorDpiY, 96);

    return ret;
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

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (GetTaskbarLocationForMonitor(monitor) == TaskbarLocation::bottom) {
        return original();
    }

    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);

    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }

    std::wstring processFileName = GetProcessFileName(processId);

    enum class Target {
        StartMenu,
        SearchHost,
    };
    Target target;

    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
        target = Target::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = Target::SearchHost;
    } else {
        return original();
    }

    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);

    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }

    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;

    if (target == Target::StartMenu) {
        // Only change height.
        const int h1 = MulDiv(750, monitorDpiY, 96);
        const int h2 = MulDiv(694, monitorDpiY, 96);
        int cyNew = cy;
        if (cyNew >= h1) {
            cyNew = h1;
        } else if (cyNew >= h2) {
            cyNew = h2;
        }

        if (cyNew == cy) {
            return original();
        }

        cy = cyNew;
    } else if (target == Target::SearchHost) {
        // Only change y.
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        GetMonitorInfo(monitor, &monitorInfo);

        int yNew = monitorInfo.rcWork.top;

        if (yNew == y) {
            return original();
        }

        y = yNew;
    }

    SetWindowPos_Original(hwnd, nullptr, x, y, cx, cy,
                          SWP_NOZORDER | SWP_NOACTIVATE);

    return original();
}

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

    g_settings.runningIndicatorsOnTop =
        Wh_GetIntSetting(L"runningIndicatorsOnTop");
}

void ApplySettings() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    g_applyingSettings = true;

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    g_applyingSettings = false;

    // Update the taskbar location that affects the jump list animations.
    auto monitorEnumProc = [hTaskbarWnd](HMONITOR hMonitor) -> BOOL {
        PostMessage(hTaskbarWnd, 0x5C3, ABE_BOTTOM, (WPARAM)hMonitor);
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
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged,
            nullptr,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
            &TaskbarFrame_TaskbarFrame_Original,
            TaskbarFrame_TaskbarFrame_Hook,
        },
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowFlyoutModel::Show(void))"},
            &OverflowFlyoutModel_Show_Original,
            OverflowFlyoutModel_Show_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::UpdateFlyoutPosition(void))",
            },
            &FlyoutFrame_UpdateFlyoutPosition_Original,
            FlyoutFrame_UpdateFlyoutPosition_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_Controls_Primitives_IFlyoutBase5<struct winrt::Windows::UI::Xaml::Controls::MenuFlyout>::ShowAt(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions const &)const )"},
            &MenuFlyout_ShowAt_Original,
            MenuFlyout_ShowAt_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::TextIconContent::ShowContextMenu(void))"},
            &TextIconContent_ShowContextMenu_Original,
            TextIconContent_ShowContextMenu_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::ShowContextMenu(void))"},
            &DateTimeIconContent_ShowContextMenu_Original,
            DateTimeIconContent_ShowContextMenu_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return false;
    }

    return true;
}

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

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
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

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetDockedRect(struct tagRECT *,int))"},
            &TrayUI_GetDockedRect_Original,
            TrayUI_GetDockedRect_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::MakeStuckRect(struct tagRECT *,struct tagRECT const *,struct tagSIZE,unsigned int))"},
            &TrayUI_MakeStuckRect_Original,
            TrayUI_MakeStuckRect_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetStuckInfo(struct tagRECT *,unsigned int *))"},
            &TrayUI_GetStuckInfo_Original,
            TrayUI_GetStuckInfo_Hook,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
            &CTaskListThumbnailWnd_DisplayUI_Original,
            CTaskListThumbnailWnd_DisplayUI_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListThumbnailWnd::LayoutThumbnails(void))"},
            &CTaskListThumbnailWnd_LayoutThumbnails_Original,
            CTaskListThumbnailWnd_LayoutThumbnails_Hook,
        },
        {
            {LR"(public: __cdecl winrt::Windows::Internal::Shell::XamlExplorerHost::XamlExplorerHostWindow::XamlExplorerHostWindow(unsigned int,struct winrt::Windows::Foundation::Rect const &,unsigned int))"},
            &XamlExplorerHostWindow_XamlExplorerHostWindow_Original,
            XamlExplorerHostWindow_XamlExplorerHostWindow_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
            &ITaskbarSettings_get_Alignment_Original,
            ITaskbarSettings_get_Alignment_Hook,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE kernel32Module = LoadLibrary(L"kernel32.dll")) {
        pGetThreadDescription = (GetThreadDescription_t)GetProcAddress(
            kernel32Module, "GetThreadDescription");
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(SetWindowPos, SetWindowPos_Hook,
                                       &SetWindowPos_Original);

    WindhawkUtils::Wh_SetFunctionHookT(MoveWindow, MoveWindow_Hook,
                                       &MoveWindow_Original);

    WindhawkUtils::Wh_SetFunctionHookT(MapWindowPoints, MapWindowPoints_Hook,
                                       &MapWindowPoints_Original);

    HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::Wh_SetFunctionHookT(pDwmSetWindowAttribute,
                                               DwmSetWindowAttribute_Hook,
                                               &DwmSetWindowAttribute_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
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

    ApplySettings();
}

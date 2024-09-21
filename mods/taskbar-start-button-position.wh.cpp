// ==WindhawkMod==
// @id              taskbar-start-button-position
// @name            Start button always on the left
// @description     Forces the start button to be on the left of the taskbar, even when taskbar icons are centered (Windows 11 only)
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
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
# Start button always on the left

Forces the start button to be on the left of the taskbar, even when taskbar
icons are centered.

Only Windows 11 is supported.

Known limitations:
* There's a jumpy animation when a centered taskbar is animated.

![Screenshot](https://i.imgur.com/bEqvfOE.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startMenuOnTheLeft: true
  $name: Start menu on the left
  $description: >-
    Make the start menu open on the left even if taskbar icons are centered
- startMenuWidth: 0
  $name: Start menu width
  $description: >-
    Set to zero to use the system default width, set to a custom value if using
    a customized start menu, e.g. with the Windows 11 Start Menu Styler mod
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    bool startMenuOnTheLeft;
    int startMenuWidth;
} g_settings;

enum class Target {
    Explorer,
    StartMenu,
    SearchHost,
};

Target g_target;

std::atomic<bool> g_unloading;

UINT_PTR g_updateStartButtonPositionTimer;
int g_updateStartButtonPositionTimerCounter;

struct TaskbarData {
    winrt::weak_ref<XamlRoot> xamlRoot;
    winrt::weak_ref<FrameworkElement> startButtonElement;
};

std::vector<TaskbarData> g_taskbarData;

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

void UpdateStartButtonPosition(XamlRoot xamlRoot,
                               FrameworkElement startButton) {
    Wh_Log(L">");

    if (startButton.XamlRoot() != xamlRoot) {
        Wh_Log(L"XamlRoot mismatch");
        return;
    }

    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();

    auto pt = startButton.TransformToVisual(xamlRootContent)
                  .TransformPoint(winrt::Windows::Foundation::Point{0, 0});
    Wh_Log(L"%f, %f", pt.X, pt.Y);

    if (pt.X == 0) {
        return;
    }

    Media::TranslateTransform transform;
    if (auto prevTransform =
            startButton.RenderTransform().try_as<Media::TranslateTransform>()) {
        transform = prevTransform;
    }

    transform.X(transform.X() - pt.X);
    startButton.RenderTransform(transform);
}

void ResetStartButtonPosition(FrameworkElement startButton) {
    Wh_Log(L">");

    Media::TranslateTransform transform;
    if (auto prevTransform =
            startButton.RenderTransform().try_as<Media::TranslateTransform>()) {
        transform = prevTransform;
    }

    transform.X(0);
    startButton.RenderTransform(transform);
}

void ScheduleUpdateStartButtonPosition() {
    g_updateStartButtonPositionTimerCounter = 0;
    g_updateStartButtonPositionTimer = SetTimer(
        nullptr, g_updateStartButtonPositionTimer, 100,
        [](HWND hwnd,         // handle of window for timer messages
           UINT uMsg,         // WM_TIMER message
           UINT_PTR idEvent,  // timer identifier
           DWORD dwTime       // current system time
           ) WINAPI {
            g_updateStartButtonPositionTimerCounter++;
            if (g_updateStartButtonPositionTimerCounter >= 30) {
                KillTimer(nullptr, g_updateStartButtonPositionTimer);
                g_updateStartButtonPositionTimer = 0;
            }

            for (const auto& item : g_taskbarData) {
                if (auto xamlRoot = item.xamlRoot.get()) {
                    if (auto startButtonElement =
                            item.startButtonElement.get()) {
                        UpdateStartButtonPosition(xamlRoot, startButtonElement);
                    }
                }
            }
        });
}

bool ApplyStyle(XamlRoot xamlRoot) {
    auto dataItem = std::find_if(
        g_taskbarData.begin(), g_taskbarData.end(), [&xamlRoot](auto x) {
            auto xamlRootIter = x.xamlRoot.get();
            return xamlRootIter && xamlRootIter == xamlRoot;
        });
    if (dataItem != g_taskbarData.end()) {
        return true;
    }

    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return false;
    }

    auto startButton =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.ExperienceToggleButton") {
                return false;
            }

            auto automationId =
                Automation::AutomationProperties::GetAutomationId(child);
            return automationId == L"StartButton";
        });
    if (!startButton) {
        return false;
    }

    double startButtonWidth = startButton.ActualWidth();

    Thickness taskbarFrameRepeaterMargin = taskbarFrameRepeater.Margin();
    taskbarFrameRepeaterMargin.Left = g_unloading ? 0 : startButtonWidth;
    taskbarFrameRepeater.Margin(taskbarFrameRepeaterMargin);

    Thickness startButtonMargin = startButton.Margin();
    startButtonMargin.Left = g_unloading ? 0 : -startButtonWidth;
    startButton.Margin(startButtonMargin);

    g_taskbarData.push_back({
        .xamlRoot = xamlRoot,
        .startButtonElement = startButton,
    });

    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = PVOID(WINAPI*)(PVOID pThis, PVOID* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = PVOID(WINAPI*)(PVOID pThis,
                                                           PVOID* result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(PVOID pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(PVOID taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // Reference: TaskbarHost::FrameHeight
    constexpr size_t kTaskbarElementIUnknownOffset = 0x40;

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      kTaskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    PVOID taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
    PVOID taskBandForTaskListWndSite = taskBand;
    while (*(PVOID*)taskBandForTaskListWndSite !=
           CTaskBand_ITaskListWndSite_vftable) {
        taskBandForTaskListWndSite = (PVOID*)taskBandForTaskListWndSite + 1;
    }

    PVOID taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    PVOID taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
    PVOID taskBandForTaskListWndSite = taskBand;
    while (*(PVOID*)taskBandForTaskListWndSite !=
           CSecondaryTaskBand_ITaskListWndSite_vftable) {
        taskBandForTaskListWndSite = (PVOID*)taskBandForTaskListWndSite + 1;
    }

    PVOID taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyles failed");
                return TRUE;
            }

            return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](PVOID pParam) WINAPI {
            if (!g_unloading) {
                ApplySettingsFromTaskbarThread();
                ScheduleUpdateStartButtonPosition();
            } else {
                for (const auto& item : g_taskbarData) {
                    if (auto startButtonElement =
                            item.startButtonElement.get()) {
                        ResetStartButtonPosition(startButtonElement);
                    }
                }

                if (g_updateStartButtonPositionTimer) {
                    KillTimer(nullptr, g_updateStartButtonPositionTimer);
                    g_updateStartButtonPositionTimer = 0;
                }
            }
        },
        0);
}

using CPearl_SetBounds_t = HRESULT(WINAPI*)(void* pThis, void* param1);
CPearl_SetBounds_t CPearl_SetBounds_Original;
HRESULT WINAPI CPearl_SetBounds_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    if (!g_unloading) {
        ApplySettingsFromTaskbarThread();
        ScheduleUpdateStartButtonPosition();
    }

    return CPearl_SetBounds_Original(pThis, param1);
}

BOOL HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: long __cdecl CPearl::SetBounds(struct tagRECT const &))"},
            (void**)&CPearl_SetBounds_Original,
            (void*)CPearl_SetBounds_Hook,
        },
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            (void**)&CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            (void**)&CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            (void**)&CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            (void**)&CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            (void**)&std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
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

            if (IsTargetCoreWindow(hWnd)) {
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
        // *height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
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

    // const int h1 = MulDiv(750, monitorDpiY, 96);
    // const int h2 = MulDiv(694, monitorDpiY, 96);
    // if (*height >= h1) {
    //     *height = h1;
    // } else if (*height >= h2) {
    //     *height = h2;
    // }
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

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    *x = monitorInfo.rcWork.left;
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

    if (!IsTargetCoreWindow(hWnd)) {
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

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

}  // namespace CoreWindowUI

void LoadSettings() {
    g_settings.startMenuOnTheLeft = Wh_GetIntSetting(L"startMenuOnTheLeft");
    g_settings.startMenuWidth = Wh_GetIntSetting(L"startMenuWidth");
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
        if (!g_settings.startMenuOnTheLeft) {
            return FALSE;
        }

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
                           (void**)&CoreWindowUI::SetWindowPos_Original);
        return TRUE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        HWND hTaskbarWnd = GetTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettings(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        HWND hTaskbarWnd = GetTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettings(hTaskbarWnd);
        }
    } else if (g_target == Target::StartMenu ||
               g_target == Target::SearchHost) {
        CoreWindowUI::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    if (!g_settings.startMenuOnTheLeft) {
        return FALSE;
    }

    *bReload = FALSE;

    if (g_target == Target::StartMenu || g_target == Target::SearchHost) {
        CoreWindowUI::ApplySettings();
    }

    return TRUE;
}

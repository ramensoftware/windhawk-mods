// ==WindhawkMod==
// @id              taskbar-left-margin
// @name            Taskbar Left Margin
// @name:zh-CN      任务栏左边距
// @description     Adds a configurable left margin to the taskbar content
// @description:zh-CN 为任务栏内容添加可调节的左边距
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar XAML access is based on the Start button always on the left mod
// by m417z, which is also licensed under the GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/loliri/windhawk-taskbar-left-margin/issues

// ==WindhawkModReadme==
/*
# Taskbar Left Margin

Shifts the taskbar **content** to the right by a configurable number of pixels,
leaving an empty margin on the left. The taskbar background stays full width,
and the taskbar context menu follows the content.

Only the taskbar itself is affected.

![Taskbar without the margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-left-margin/main/images/before.png) \
_Before_

![Taskbar with a left margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-left-margin/main/images/after.png) \
_After_

![Taskbar context menu](https://raw.githubusercontent.com/loliri/windhawk-taskbar-left-margin/main/images/jumplist.png) \
_The context menu follows the content_

## Notes

Requires Windows 11.

## Compatibility

- **Windows 11 Taskbar Styler** can be used alongside this mod. This mod reads
  the taskbar's XAML tree directly instead of going through XAML diagnostics, so
  it does not compete with the Styler for the single XAML diagnostics consumer
  slot that Explorer allows.
- **TranslucentTB** is confirmed compatible and can be used alongside this mod.
- Tested on Windows 11 26H2.

## Suggested use

Together with [FluentFlyout](https://github.com/unchihugo/FluentFlyout): enable
the taskbar widget there, set its position to the bottom left corner, and turn
on the fixed widget width. The taskbar elements then tile linearly instead of
overlapping each other.

## Implementation notes

The taskbar context menu (the jump list) is not laid out by XAML. Its anchor
point is computed in `explorer.exe` by `CTaskListWnd::_ComputeJumpViewPosition`
in `taskbar.dll`, and handed to the process that draws the menu. The point is in
physical screen pixels.

Shifting the taskbar's XAML content therefore does not move the menu on its own,
because the menu is not placed relative to it. The mod adjusts the anchor point
instead, which is why it hooks that function.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftMargin: 220
  $name: Left margin (pixels)
  $name:zh-CN: 左边距（像素）
  $description: >-
    How many pixels of empty space to leave on the left of the taskbar content.
    A larger value shifts the content further right.
  $description:zh-CN: >-
    任务栏内容左侧留空的像素数，数值越大整体越靠右。
- followDpi: true
  $name: Follow display DPI
  $name:zh-CN: 跟随显示器 DPI
  $description: >-
    Scale the margin together with the display DPI, so it keeps the same visual
    size on high-DPI displays. Turn off to keep the margin at a constant
    physical pixel size.
  $description:zh-CN: >-
    让边距随显示器 DPI 一同缩放，在高 DPI 显示器上保持相同的视觉大小。关闭后边距将固定为恒定的物理像素数。
- displays: all
  $name: Displays
  $name:zh-CN: 生效显示器
  $description: >-
    Which displays the margin is applied to. Each display keeps its own taskbar
    and its own DPI, so the margin is calculated per display.
  $description:zh-CN: >-
    边距应用在哪些显示器上。每个显示器有自己的任务栏和 DPI，边距按显示器分别计算。
  $options:
    - all: All displays
    - primary: Primary display only
    - secondary: Secondary displays only
  $options:zh-CN:
    - all: 所有显示器
    - primary: 仅主显示器
    - secondary: 仅副显示器
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <functional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <shellscalingapi.h>

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

enum class Displays
{
    All,
    Primary,
    Secondary,
};

struct
{
    int leftMargin;
    bool followDpi;
    Displays displays;
} g_settings;

std::atomic<bool> g_unloading;

// Set once the margin has been applied to a taskbar, and cleared when it is
// removed, so that the anchor is only shifted while the padding is in place.
std::atomic<bool> g_marginApplied;

// Elements whose value the mod set, so it can be cleared again.
struct AppliedElement
{
    winrt::weak_ref<FrameworkElement> element;
    DependencyProperty property;
};

// The taskbars live on different threads, so the applied elements are tracked
// per thread.
thread_local std::vector<AppliedElement> g_appliedElements;

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// The taskbar's XAML tree is reached through its host object rather than
// through XAML diagnostics, so that the mod can run alongside other mods which
// need to be Explorer's XAML diagnostics consumer.
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

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

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
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

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
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

// Whether the margin should be applied to the display the taskbar is on. The
// primary display is the one whose top-left corner is the origin of the
// virtual screen.
bool ShouldApplyToMonitor(HMONITOR monitor) {
    switch (g_settings.displays) {
        case Displays::All:
            return true;

        case Displays::Primary: {
            MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
            if (!GetMonitorInfo(monitor, &monitorInfo)) {
                return false;
            }

            return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
        }

        case Displays::Secondary: {
            MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
            if (!GetMonitorInfo(monitor, &monitorInfo)) {
                return false;
            }

            return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0;
        }
    }

    return true;
}

// The margin in DIPs, which is what XAML expects. With DPI following on the
// setting is taken as DIPs; with it off the setting is taken as physical pixels
// and converted, so the margin keeps a constant physical size.
double GetMarginInDips(FrameworkElement const& element) {
    if (g_settings.followDpi) {
        return static_cast<double>(g_settings.leftMargin);
    }

    try {
        auto xamlRoot = element.XamlRoot();
        if (xamlRoot) {
            double scale = xamlRoot.RasterizationScale();
            if (scale > 0) {
                return static_cast<double>(g_settings.leftMargin) / scale;
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    return static_cast<double>(g_settings.leftMargin);
}

void ApplyMarginToTaskbar(HWND hTaskbarWnd, XamlRoot xamlRoot) {
    HMONITOR monitor =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    if (!ShouldApplyToMonitor(monitor)) {
        return;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        Wh_Log(L"Failed to get the taskbar content element");
        return;
    }

    auto taskbarFrame = FindChildByClassName(content, L"Taskbar.TaskbarFrame");
    auto rootGrid = taskbarFrame ? FindChildByName(taskbarFrame, L"RootGrid")
                                 : nullptr;
    if (!rootGrid) {
        Wh_Log(L"Failed to find the taskbar RootGrid");
        return;
    }

    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid) {
        Wh_Log(L"RootGrid is not a Grid, skipping");
        return;
    }

    double margin = GetMarginInDips(taskbarFrame);

    // The padding shifts the taskbar content, and the background is pulled back
    // by the same amount so that it keeps spanning the full width.
    grid.Padding(Thickness{margin, 0, 0, 0});
    g_appliedElements.push_back(
        {winrt::make_weak(rootGrid), Controls::Grid::PaddingProperty()});

    auto taskbarBackground =
        FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground");
    if (taskbarBackground) {
        taskbarBackground.Margin(Thickness{-margin, 0, 0, 0});
        g_appliedElements.push_back({winrt::make_weak(taskbarBackground),
                                     FrameworkElement::MarginProperty()});
    } else {
        Wh_Log(L"Failed to find TaskbarBackground");
    }

    g_marginApplied.store(true);
}

void RemoveAppliedMargins() {
    for (const auto& applied : g_appliedElements) {
        if (auto element = applied.element.get()) {
            try {
                element.ClearValue(applied.property);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        }
    }

    g_appliedElements.clear();
    g_marginApplied.store(false);
}

using ComputeJumpViewPosition_t = HRESULT(WINAPI*)(
    void* pThis,
    void* pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    winrt::Windows::UI::Xaml::HorizontalAlignment* hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment* vAlign);

ComputeJumpViewPosition_t ComputeJumpViewPosition_Original;

HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void* pThis,
    void* pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    winrt::Windows::UI::Xaml::HorizontalAlignment* hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment* vAlign) {
    HRESULT hr = ComputeJumpViewPosition_Original(pThis, pTaskBtnGroup, param2,
                                                  point, hAlign, vAlign);

    if (FAILED(hr) || !point || g_unloading || !g_marginApplied.load() ||
        !g_settings.leftMargin) {
        return hr;
    }

    // The anchor is in physical screen pixels. The DPI is taken from the
    // display the anchor is on, so that displays at different scales each get
    // the right offset.
    HMONITOR monitor =
        MonitorFromPoint(POINT{(LONG)point->X, (LONG)point->Y},
                         MONITOR_DEFAULTTONEAREST);
    if (!ShouldApplyToMonitor(monitor)) {
        return hr;
    }

    int offset = g_settings.leftMargin;
    if (g_settings.followDpi) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        if (SUCCEEDED(
                GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
            dpiX > 0) {
            offset = MulDiv(g_settings.leftMargin, dpiX,
                            USER_DEFAULT_SCREEN_DPI);
        }
    }

    Wh_Log(L"Jump list anchor x: %d -> %d", (int)point->X,
           (int)(point->X + offset));
    point->X += static_cast<float>(offset);

    return hr;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L">");

    RemoveAppliedMargins();

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
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

            ApplyMarginToTaskbar(hWnd, xamlRoot);

            return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd, [](void*) { ApplySettingsFromTaskbarThread(); }, nullptr);
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    if (!lpClassName) {
        return;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName) {
        return;
    }

    if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        ApplySettings(hWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
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
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
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
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
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

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &ComputeJumpViewPosition_Original,
            ComputeJumpViewPosition_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.leftMargin = Wh_GetIntSetting(L"leftMargin");
    if (g_settings.leftMargin < 0) {
        g_settings.leftMargin = 0;
    }

    g_settings.followDpi = Wh_GetIntSetting(L"followDpi") != 0;

    g_settings.displays = Displays::All;
    PCWSTR displays = Wh_GetStringSetting(L"displays");
    if (wcscmp(displays, L"primary") == 0) {
        g_settings.displays = Displays::Primary;
    } else if (wcscmp(displays, L"secondary") == 0) {
        g_settings.displays = Displays::Secondary;
    }
    Wh_FreeStringSetting(displays);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }

    return TRUE;
}

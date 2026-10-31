// ==WindhawkMod==
// @id              taskbar-margin
// @name            Taskbar Margin
// @name:zh-CN      任务栏边距
// @description     Adds a configurable margin to the taskbar content
// @description:zh-CN 为任务栏内容添加可调节的边距
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar XAML access is based on the Start button always on the left mod
// by m417z, which is also licensed under the GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/loliri/windhawk-taskbar-margin/issues

// ==WindhawkModReadme==
/*
# Taskbar Margin

Shifts the taskbar **content** to the right by a configurable number of pixels, leaving an empty margin on the left. The taskbar background stays full width, and the taskbar context menu follows the content.

Only the taskbar itself is affected.

![Taskbar without a margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/before.png) \
_Before_

![Taskbar with a left margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/after.png) \
_After_

![Taskbar context menu](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/jumplist.png) \
_The context menu follows the margin_

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| Left margin (pixels) | 220 | How much empty space to leave on the left of the taskbar content. |
| Follow display DPI | on | Scale the margin with the display DPI, so it keeps the same visual size on high-DPI displays. Turn off to keep it at a constant physical pixel size. |
| Displays | All | Which displays the margin applies to: all, only the primary one, or only the secondary ones. Each display has its own taskbar and its own DPI, so the margin is calculated per display. |

## Notes

Requires Windows 11.

The taskbar must be:

- **left-aligned** (Settings → Personalization → Taskbar → Taskbar icon alignment)
- **Bottom** or **Top** (New in Windows 11 26H2, Settings → Personalization → Taskbar → Taskbar position)

The taskbar is not mirrored correctly on right-to-left display languages: the margin is applied to the physical left regardless of the taskbar's flow direction, while the context menu is moved to the right.

## Compatibility

- **TranslucentTB** is confirmed compatible and can be used alongside this mod.
- **Windows 11 Taskbar Styler** is not an alternative to this mod. It changes the taskbar itself, while this mod changes the position of the elements inside it. A taskbar restyled that way loses the taskbar's effects on the margin area, while this mod keeps them across the whole taskbar, margin included. It also cannot move the context menu (the jump list), which this mod does.
- **Taskbar jump list on cursor pos** changes the same anchor point. Both mods offset it, so depending on the hook order the menu can open offset from the cursor. Use one or the other.
- Tested on Windows 11 26H2.

## Suggested use

For example, together with [FluentFlyout](https://github.com/unchihugo/FluentFlyout): enable the taskbar widget there, set its position to the bottom left corner, and turn on the fixed widget width. The taskbar elements then tile linearly instead of overlapping each other.

## Implementation notes

The taskbar context menu (the jump list) is not laid out by XAML. Its anchor point is computed in `explorer.exe` by `CTaskListWnd::_ComputeJumpViewPosition` in `taskbar.dll`, and handed to the process that draws the menu. The point is in physical screen pixels.

Shifting the taskbar's XAML content therefore does not move the menu on its own, because the menu is not placed relative to it. The mod adjusts the anchor point instead, which is why it hooks that function.

## License

GPL-3.0. The taskbar XAML access is based on the [Start button always on the left](https://github.com/m417z/my-windhawk-mods) mod by m417z, which is also licensed under GPL-3.0.

## Feedback

Bug reports and feature requests are welcome in [Issues](https://github.com/loliri/windhawk-taskbar-margin/issues).
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
    size on high-DPI displays. Turn off to keep it at a constant physical pixel
    size.
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

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <vector>

#undef GetCurrentTime

#include <shellscalingapi.h>

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
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

// Which property the mod set on an element. The property itself is a WinRT
// object, and keeping it in a global container would leave a strong reference
// to release at process shutdown, where the XAML objects may already be gone.
// The tag is resolved back to the static property when it is cleared.
enum class AppliedProperty
{
    GridPadding,
    FrameworkElementMargin,
};

// Elements whose value the mod set, so it can be cleared again.
struct AppliedElement
{
    winrt::weak_ref<FrameworkElement> element;
    AppliedProperty property;
};

// The elements the mod set on one taskbar, together with the thread that
// applied them and the display that taskbar is on. The thread id keeps the
// apply/remove pass scoped to its own taskbar, and the display lets the jump
// list hook look up whether this taskbar actually has the margin.
struct AppliedTaskbar
{
    DWORD threadId;
    HMONITOR monitor;
    std::vector<AppliedElement> elements;
};

// The taskbars the mod has changed. Guarded in case the jump list hook runs on
// a different thread than the apply pass.
std::mutex g_appliedTaskbarsMutex;
std::vector<AppliedTaskbar> g_appliedTaskbars;

void *CTaskBand_ITaskListWndSite_vftable;

void *CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void *TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis,
                                                             void **result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI *)(void *pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// The taskbar's XAML tree is reached through its host object rather than
// through XAML diagnostics, so that the mod can run alongside other mods which
// need to be Explorer's XAML diagnostics consumer.
XamlRoot XamlRootFromTaskbarHostSharedPtr(void *taskbarHostSharedPtr[2])
{
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1])
    {
        return nullptr;
    }

    // The element is only reachable through the host object, so a host that
    // exists without one yet has to be treated as not ready.
    if (!taskbarHostSharedPtr[0])
    {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE *b = (const BYTE *)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
        {
            taskbarElementIUnknownOffset = b[7];
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD *p = (const DWORD *)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
        {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto *taskbarElementIUnknown =
        *(IUnknown **)((BYTE *)taskbarHostSharedPtr[0] +
                       taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd)
{
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd)
    {
        return nullptr;
    }

    void *taskBand = (void *)GetWindowLongPtr(hTaskSwWnd, 0);
    void *taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void **)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20)
        {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void **)taskBandForTaskListWndSite + 1;
    }

    void *taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd)
{
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd)
    {
        return nullptr;
    }

    void *taskBand = (void *)GetWindowLongPtr(hTaskSwWnd, 0);
    void *taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void **)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20)
        {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void **)taskBandForTaskListWndSite + 1;
    }

    void *taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback)
{
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child)
        {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child))
        {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name)
{
    return EnumChildElements(element, [name](FrameworkElement child)
                             { return child.Name() == name; });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className)
{
    return EnumChildElements(element, [className](FrameworkElement child)
                             { return winrt::get_class_name(child) == className; });
}

// Whether the margin should be applied to the display the taskbar is on. The
// primary display is the one whose top-left corner is the origin of the
// virtual screen.
bool ShouldApplyToMonitor(HMONITOR monitor)
{
    switch (g_settings.displays)
    {
    case Displays::All:
        return true;

    case Displays::Primary:
    {
        MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
        if (!GetMonitorInfo(monitor, &monitorInfo))
        {
            return false;
        }

        return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
    }

    case Displays::Secondary:
    {
        MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
        if (!GetMonitorInfo(monitor, &monitorInfo))
        {
            return false;
        }

        return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0;
    }
    }

    return true;
}

// A margin in DIPs, which is what XAML expects. With DPI following on the
// setting is taken as DIPs; with it off the setting is taken as physical pixels
// and converted, so the margin keeps a constant physical size.
double GetMarginInDips(FrameworkElement const &element, int marginPixels)
{
    if (g_settings.followDpi)
    {
        return static_cast<double>(marginPixels);
    }

    try
    {
        auto xamlRoot = element.XamlRoot();
        if (xamlRoot)
        {
            double scale = xamlRoot.RasterizationScale();
            if (scale > 0)
            {
                return static_cast<double>(marginPixels) / scale;
            }
        }
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    return static_cast<double>(marginPixels);
}

// Returns whether the margin was applied to this taskbar.
bool ApplyMarginToTaskbar(HWND hTaskbarWnd, XamlRoot xamlRoot)
{
    HMONITOR monitor =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    if (!ShouldApplyToMonitor(monitor))
    {
        return true;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content)
    {
        Wh_Log(L"Failed to get the taskbar content element");
        return false;
    }

    auto taskbarFrame = FindChildByClassName(content, L"Taskbar.TaskbarFrame");
    auto rootGrid = taskbarFrame ? FindChildByName(taskbarFrame, L"RootGrid")
                                 : nullptr;
    if (!rootGrid)
    {
        Wh_Log(L"Failed to find the taskbar RootGrid");
        return false;
    }

    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid)
    {
        Wh_Log(L"RootGrid is not a Grid, skipping");
        return false;
    }

    double leftMargin =
        GetMarginInDips(taskbarFrame, g_settings.leftMargin);

    AppliedTaskbar appliedTaskbar;
    appliedTaskbar.threadId = GetCurrentThreadId();
    appliedTaskbar.monitor = monitor;

    // The padding shifts the taskbar content, and the background is pulled back
    // by the same amount so that it keeps spanning the full width.
    grid.Padding(Thickness{leftMargin, 0, 0, 0});
    appliedTaskbar.elements.push_back(
        {winrt::make_weak(rootGrid), AppliedProperty::GridPadding});

    auto taskbarBackground =
        FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground");
    if (taskbarBackground)
    {
        taskbarBackground.Margin(Thickness{-leftMargin, 0, 0, 0});
        appliedTaskbar.elements.push_back({winrt::make_weak(taskbarBackground),
                                           AppliedProperty::FrameworkElementMargin});
    }
    else
    {
        Wh_Log(L"Failed to find TaskbarBackground");
    }

    std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);
    g_appliedTaskbars.push_back(std::move(appliedTaskbar));

    return true;
}

// Clears the values the mod set on the taskbars of the calling thread.
void RemoveAppliedMargins()
{
    DWORD dwThreadId = GetCurrentThreadId();

    std::vector<AppliedTaskbar> appliedTaskbars;

    {
        std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);

        for (auto it = g_appliedTaskbars.begin();
             it != g_appliedTaskbars.end();)
        {
            if (it->threadId == dwThreadId)
            {
                appliedTaskbars.push_back(std::move(*it));
                it = g_appliedTaskbars.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    for (const auto &appliedTaskbar : appliedTaskbars)
    {
        for (const auto &applied : appliedTaskbar.elements)
        {
            if (auto element = applied.element.get())
            {
                try
                {
                    element.ClearValue(
                        applied.property == AppliedProperty::GridPadding
                            ? Controls::Grid::PaddingProperty()
                            : FrameworkElement::MarginProperty());
                }
                catch (winrt::hresult_error const &ex)
                {
                    Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
                }
            }
        }
    }
}

// Whether the margin is currently applied to the taskbar on this display. The
// jump list hook uses this instead of a single global flag, so that a taskbar
// which does not have the margin never gets its anchor shifted.
bool IsMarginAppliedToMonitor(HMONITOR monitor)
{
    std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);
    for (const auto &appliedTaskbar : g_appliedTaskbars)
    {
        if (appliedTaskbar.monitor == monitor)
        {
            return true;
        }
    }

    return false;
}

using ComputeJumpViewPosition_t = HRESULT(WINAPI *)(
    void *pThis,
    void *pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point *point,
    winrt::Windows::UI::Xaml::HorizontalAlignment *hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment *vAlign);

ComputeJumpViewPosition_t ComputeJumpViewPosition_Original;

HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void *pThis,
    void *pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point *point,
    winrt::Windows::UI::Xaml::HorizontalAlignment *hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment *vAlign)
{
    HRESULT hr = ComputeJumpViewPosition_Original(pThis, pTaskBtnGroup, param2,
                                                  point, hAlign, vAlign);

    if (FAILED(hr) || !point || g_unloading || !g_settings.leftMargin)
    {
        return hr;
    }

    // The anchor is in physical screen pixels. The DPI is taken from the
    // display the anchor is on, so that displays at different scales each get
    // the right offset.
    HMONITOR monitor =
        MonitorFromPoint(POINT{(LONG)point->X, (LONG)point->Y},
                         MONITOR_DEFAULTTONEAREST);
    if (!IsMarginAppliedToMonitor(monitor))
    {
        return hr;
    }

    int offset = g_settings.leftMargin;
    if (g_settings.followDpi)
    {
        UINT dpiX = 0;
        UINT dpiY = 0;
        if (SUCCEEDED(
                GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
            dpiX > 0)
        {
            offset = MulDiv(g_settings.leftMargin, dpiX,
                            USER_DEFAULT_SCREEN_DPI);
        }
    }

    Wh_Log(L"Jump list anchor x: %d -> %d", (int)point->X,
           (int)(point->X + offset));
    point->X += static_cast<float>(offset);

    return hr;
}

using RunFromWindowThreadProc_t = void(WINAPI *)(void *parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void *procParam)
{
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM
    {
        RunFromWindowThreadProc_t proc;
        void *procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION)
            {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg)
                {
                    RUN_FROM_WINDOW_THREAD_PARAM *param =
                        (RUN_FROM_WINDOW_THREAD_PARAM *)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook)
    {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

bool ApplySettingsFromTaskbarThread();

// The taskbar's XAML tree is not ready when the taskbar window is created, so
// the XamlRoot lookup fails at that point. Applying once and giving up is what
// breaks a late start, where the taskbar is built after the mod loads, so the
// apply is retried on a timer until every taskbar on the thread has it. The
// timer is per thread, so that each taskbar thread keeps its own retry state.
thread_local winrt::Windows::System::DispatcherQueueTimer g_retryTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_retryTimerRevoker;
thread_local int g_retryAttempts;

constexpr int kMaxApplyAttempts = 20;
constexpr int kApplyRetryIntervalMs = 500;

void StopRetryTimer()
{
    if (g_retryTimer)
    {
        try
        {
            g_retryTimer.Stop();
        }
        catch (winrt::hresult_error const &ex)
        {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_retryTimerRevoker.revoke();
    g_retryTimer = nullptr;
    g_retryAttempts = 0;
}

void RetryTimerTick(winrt::Windows::System::DispatcherQueueTimer const &,
                    winrt::Windows::Foundation::IInspectable const &)
{
    if (g_unloading)
    {
        StopRetryTimer();
        return;
    }

    if (ApplySettingsFromTaskbarThread())
    {
        Wh_Log(L"Applied on attempt %d", g_retryAttempts + 1);
        StopRetryTimer();
        return;
    }

    if (++g_retryAttempts >= kMaxApplyAttempts)
    {
        Wh_Log(L"Gave up applying the margin");
        StopRetryTimer();
    }
}

void StartRetryTimer()
{
    if (g_retryTimer)
    {
        return;
    }

    try
    {
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!dispatcherQueue)
        {
            Wh_Log(L"No dispatcher queue, cannot retry");
            return;
        }

        g_retryTimer = dispatcherQueue.CreateTimer();
        g_retryTimer.IsRepeating(true);
        g_retryTimer.Interval(
            std::chrono::milliseconds{kApplyRetryIntervalMs});
        g_retryTimerRevoker = g_retryTimer.Tick(winrt::auto_revoke,
                                                RetryTimerTick);
        g_retryTimer.Start();
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// Applies the margin to every taskbar on the calling thread. Returns whether
// all of them got it, so that a taskbar whose XAML is not built yet keeps the
// retry going instead of being skipped because another one succeeded.
bool ApplySettingsFromTaskbarThread()
{
    Wh_Log(L">");

    RemoveAppliedMargins();

    if (g_unloading)
    {
        return true;
    }

    bool allApplied = true;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0)
            {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            }
            else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
            {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            }
            else
            {
                return TRUE;
            }

            if (!xamlRoot)
            {
                Wh_Log(L"Getting XamlRoot failed");
                *reinterpret_cast<bool *>(lParam) = false;
                return TRUE;
            }

            if (!ApplyMarginToTaskbar(hWnd, xamlRoot))
            {
                *reinterpret_cast<bool *>(lParam) = false;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&allApplied));

    return allApplied;
}

void ApplySettingsOnTaskbarThread()
{
    if (!ApplySettingsFromTaskbarThread() && !g_unloading)
    {
        Wh_Log(L"Taskbar XAML not ready, will retry");
        StartRetryTimer();
    }
}

void ApplySettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *)
        { ApplySettingsOnTaskbarThread(); }, nullptr);
}

void RemoveSettingsFromTaskbarThread()
{
    StopRetryTimer();
    RemoveAppliedMargins();
}

void RemoveSettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *)
        { RemoveSettingsFromTaskbarThread(); },
        nullptr);
}

// Every taskbar window lives on the taskbar thread, and the apply pass already
// covers all the taskbars on the thread it runs on, so each thread is visited
// only once even when it owns several taskbar windows.
void ForEachTaskbarWindow(void (*proc)(HWND))
{
    struct ENUM_PARAM
    {
        void (*proc)(HWND);
        std::vector<DWORD> visitedThreadIds;
    };

    ENUM_PARAM param{proc};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            auto *param = reinterpret_cast<ENUM_PARAM *>(lParam);

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
            {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") != 0 &&
                _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0)
            {
                return TRUE;
            }

            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
            if (std::find(param->visitedThreadIds.begin(),
                          param->visitedThreadIds.end(),
                          dwThreadId) != param->visitedThreadIds.end())
            {
                return TRUE;
            }

            param->visitedThreadIds.push_back(dwThreadId);
            param->proc(hWnd);

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName)
{
    if (!lpClassName)
    {
        return;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName)
    {
        return;
    }

    if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0)
    {
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
                                 PVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI *)(DWORD dwExStyle,
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
                                    DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI *)(DWORD dwExStyle,
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
                                      DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

bool HookTaskbarDllSymbols()
{
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
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

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings()
{
    g_settings.leftMargin = Wh_GetIntSetting(L"leftMargin");
    if (g_settings.leftMargin < 0)
    {
        g_settings.leftMargin = 0;
    }

    g_settings.followDpi = Wh_GetIntSetting(L"followDpi") != 0;

    g_settings.displays = Displays::All;
    WindhawkUtils::StringSetting displays =
        WindhawkUtils::StringSetting::make(L"displays");
    if (wcscmp(displays, L"primary") == 0)
    {
        g_settings.displays = Displays::Primary;
    }
    else if (wcscmp(displays, L"secondary") == 0)
    {
        g_settings.displays = Displays::Secondary;
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols())
    {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module)
    {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    ForEachTaskbarWindow(ApplySettings);
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L">");

    g_unloading = true;

    ForEachTaskbarWindow(RemoveSettings);
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    LoadSettings();

    ForEachTaskbarWindow(ApplySettings);
}

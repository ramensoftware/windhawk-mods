// ==WindhawkMod==
// @id              windows-11-beter-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers the taskbar and system tray as a single unit
// @version         1.1
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Windows 11 Better Ultrawide Taskbar

Centers the taskbar buttons and system tray as a single unit on the Windows 11
taskbar. The mod dynamically adjusts positioning when windows are opened or
closed.

## Modes

### Center Mode (default)
Centers both the taskbar buttons and system tray together as a unified group.
The centering is calculated based on the combined width of both elements.

**Settings:**
- **Minimum total width**: Set a minimum width for the centered group
- **Gap**: Add spacing between taskbar buttons and system tray
- **Maximum taskbar width**: Limit the taskbar buttons area width

Center Mode (System Taskbar Alignment set to Center):
![Center Mode with System Taskbar Alignment set to Center](https://i.imgur.com/j8a5deC.png)

### Offset Only Mode
Only shifts the system tray by a fixed offset, reducing the taskbar width to
allow the system tray to overlap into the taskbar area. The taskbar buttons
remain in their original position.

Offset Mode (System Taskbar Alignment set to Center):
![Offset Mode with System Taskbar Alignment set to Center](https://i.imgur.com/DfhNDc4.png)

Offset Mode (System Taskbar Alignment set to Left):
![Offset Mode with System Taskbar Alignment set to Left](https://i.imgur.com/LZdzlka.png)

**Settings:**
- **Offset value**: How many pixels to shift the system tray left

## Multi-Monitor Support

The mod applies centering/offset to the primary monitor's taskbar only.
Secondary monitor taskbars are not affected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- offsetOnlyMode: false
  $name: Offset only mode
  $description: >-
    OFF = Center mode: Centers both taskbar and system tray as a single unit.
    ON = Offset only mode: Only applies offset to system tray, reduces taskbar
    width to create overlap.
- OffsetOnlySettings:
    - offsetValue: 200
      $name: Offset value
      $description: >-
        The offset (in pixels) to shift the system tray left. The taskbar width
        will be reduced by 2x this value to allow overlap.
- CenterSettings:
    - minTotalWidth: 0
      $name: Minimum total width
      $description: >-
        The minimum combined width (in pixels) for the taskbar buttons and system
        tray. If the actual width is less than this value, the centering offset
        will be calculated as if the width were this minimum value. Set to 0 to
        disable.
    - gap: 0
      $name: Gap
      $description: >-
        Additional spacing (in pixels) between the taskbar buttons and system tray.
        Positive values push them apart, negative values pull them closer together.
    - maxTotalWidth: 0
      $name: Maximum taskbar width
      $description: >-
        The maximum width (in pixels) for the taskbar buttons area. This sets the
        Width property on TaskbarFrame directly, leveraging native XAML layout.
        Set to 0 to disable (no maximum).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <limits>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

// Settings
struct
{
    bool offsetOnlyMode;
    int offsetValue;
    int minTotalWidth;
    int gap;
    int maxTotalWidth;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Store original taskbar frame width for offset mode
double g_originalTaskbarFrameWidth = 0;

// Event token for LayoutUpdated handler (used for deferred width capture)
winrt::event_token g_layoutUpdatedToken{};

// Weak reference to taskbar frame for deferred width capture
winrt::weak_ref<FrameworkElement> g_taskbarFrameWeakRef;

// XamlRoot weak reference for applying style after width capture
winrt::weak_ref<XamlRoot> g_pendingXamlRootWeakRef;

HWND FindCurrentProcessTaskbarWnd()
{
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND *>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
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

// Calculate total width of visible taskbar buttons
double CalculateTaskbarButtonsWidth(FrameworkElement taskbarFrameRepeater)
{
    double totalWidth = 0;

    EnumChildElements(taskbarFrameRepeater, [&totalWidth](FrameworkElement child)
                      {
                          try
                          {
                              // Only count elements that can be scroll anchors (actual visible buttons)
                              if (child.CanBeScrollAnchor())
                              {
                                  totalWidth += child.ActualWidth();
                              }
}
catch (...)
{
                              // Fallback: count all elements if property access fails
                              totalWidth += child.ActualWidth();
                          }
                          return false; // Continue enumeration
                      });

    return totalWidth;
}

// Apply a TranslateTransform to visually offset an element
void ApplyTransformOffset(FrameworkElement element, double offsetX)
{
    try
    {
        // Get or create the TranslateTransform
        Media::TranslateTransform transform{nullptr};
        auto existingTransform = element.RenderTransform();
        if (auto tt = existingTransform.try_as<Media::TranslateTransform>())
        {
            transform = tt;
        }
        else
        {
            transform = Media::TranslateTransform();
            transform.Y(0);
            element.RenderTransform(transform);
        }

        transform.X(offsetX);
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error applying transform %08X: %s", ex.code().value, ex.message().c_str());
    }
}

// Helper function to reset taskbar state to defaults
// Used both for unloading and for cleaning up previous mode state
void ResetTaskbarState(FrameworkElement taskbarFrame, FrameworkElement systemTrayFrame)
{
    // Clear transforms
    taskbarFrame.RenderTransform(nullptr);
    systemTrayFrame.RenderTransform(nullptr);

    // Reset margins
    auto taskbarMargin = taskbarFrame.Margin();
    taskbarMargin.Left = 0;
    taskbarMargin.Right = 0;
    taskbarFrame.Margin(taskbarMargin);

    auto systemTrayMargin = systemTrayFrame.Margin();
    systemTrayMargin.Left = 0;
    systemTrayMargin.Right = 0;
    systemTrayFrame.Margin(systemTrayMargin);

    // Reset width to auto
    taskbarFrame.Width(std::numeric_limits<double>::quiet_NaN());

    // Reset SystemTrayFrame alignment to default (Right)
    systemTrayFrame.HorizontalAlignment(HorizontalAlignment::Right);
}

// Forward declaration for deferred style application
bool ApplyStyleInternal(XamlRoot xamlRoot, bool skipWidthCapture);

// Handler for LayoutUpdated event to capture width after layout completes
void OnLayoutUpdatedForWidthCapture(winrt::Windows::Foundation::IInspectable const& sender,
                                     winrt::Windows::Foundation::IInspectable const& args)
{
    // Get the taskbar frame from weak reference
    auto taskbarFrame = g_taskbarFrameWeakRef.get();
    if (!taskbarFrame)
    {
        Wh_Log(L"LayoutUpdated: TaskbarFrame weak ref expired");
        return;
    }

    // Unsubscribe from the event immediately to prevent multiple calls
    if (g_layoutUpdatedToken)
    {
        try
        {
            taskbarFrame.LayoutUpdated(g_layoutUpdatedToken);
            g_layoutUpdatedToken = {};
        }
        catch (...)
        {
            // Ignore errors during unsubscription
        }
    }

    // Capture the width now that layout is complete
    double actualWidth = taskbarFrame.ActualWidth();
    if (actualWidth > 0)
    {
        g_originalTaskbarFrameWidth = actualWidth;
        Wh_Log(L"LayoutUpdated: Captured original TaskbarFrame ActualWidth: %f", g_originalTaskbarFrameWidth);

        // Now apply the style with the captured width
        auto xamlRoot = g_pendingXamlRootWeakRef.get();
        if (xamlRoot && !g_unloading)
        {
            ApplyStyleInternal(xamlRoot, true); // Skip width capture since we just did it
        }
    }
    else
    {
        Wh_Log(L"LayoutUpdated: ActualWidth still 0, width capture failed");
    }

    // Clear weak references
    g_taskbarFrameWeakRef = nullptr;
    g_pendingXamlRootWeakRef = nullptr;
}

// Internal implementation that can skip width capture
bool ApplyStyleInternal(XamlRoot xamlRoot, bool skipWidthCapture)
{
    if (!xamlRoot)
    {
        return false;
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent)
    {
        return false;
    }

    // Find TaskbarFrame and TaskbarFrameRepeater
    FrameworkElement taskbarFrame = nullptr;
    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")))
    {
        taskbarFrame = child;
        if ((child = FindChildByName(child, L"RootGrid")) &&
            (child = FindChildByName(child, L"TaskbarFrameRepeater")))
        {
            taskbarFrameRepeater = child;
        }
    }

    if (!taskbarFrame || !taskbarFrameRepeater)
    {
        Wh_Log(L"Failed to find TaskbarFrame or TaskbarFrameRepeater");
        return false;
    }

    // Find SystemTrayFrame
    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame)
    {
        Wh_Log(L"Failed to find SystemTrayFrame");
        return false;
    }

    double systemTrayWidth = systemTrayFrame.ActualWidth();
    double taskbarButtonsWidth = CalculateTaskbarButtonsWidth(taskbarFrameRepeater);

    Wh_Log(L"TaskbarButtons width: %f, SystemTray width: %f",
           taskbarButtonsWidth, systemTrayWidth);

    if (g_unloading)
    {
        // Restore original state
        ResetTaskbarState(taskbarFrame, systemTrayFrame);
        Wh_Log(L"Restored original taskbar state");
        return true;
    }

    // Clean up any previous mode's state before applying new mode
    // This ensures switching between modes works correctly
    ResetTaskbarState(taskbarFrame, systemTrayFrame);

    // Capture original width ONCE (after reset, so we get the true original)
    // This is used by offset mode to calculate the reduced width
    if (g_originalTaskbarFrameWidth <= 0 && !skipWidthCapture)
    {
        // XAML layout is asynchronous - UpdateLayout() doesn't guarantee immediate completion.
        // We need to use the LayoutUpdated event to capture the width after layout completes.
        // First, try to get the current ActualWidth - it might already be valid.
        double currentWidth = taskbarFrame.ActualWidth();

        if (currentWidth > 0)
        {
            // Width is already valid, use it directly
            g_originalTaskbarFrameWidth = currentWidth;
            Wh_Log(L"Captured original TaskbarFrame ActualWidth immediately: %f", g_originalTaskbarFrameWidth);
        }
        else
        {
            // Width is not yet available, defer capture using LayoutUpdated event
            Wh_Log(L"ActualWidth is 0, deferring width capture via LayoutUpdated event");

            // Store weak references for the callback
            g_taskbarFrameWeakRef = taskbarFrame;
            g_pendingXamlRootWeakRef = xamlRoot;

            // Subscribe to LayoutUpdated event
            try
            {
                g_layoutUpdatedToken = taskbarFrame.LayoutUpdated(OnLayoutUpdatedForWidthCapture);

                // Trigger a layout pass
                taskbarFrame.UpdateLayout();
            }
            catch (winrt::hresult_error const& ex)
            {
                Wh_Log(L"Failed to subscribe to LayoutUpdated: %08X: %s", ex.code().value, ex.message().c_str());
                // Fall back to using whatever width we have
                g_originalTaskbarFrameWidth = currentWidth > 0 ? currentWidth : 1000.0; // Default fallback
            }

            // Return true - we'll apply the style when LayoutUpdated fires
            return true;
        }
    }

    if (g_settings.offsetOnlyMode)
    {
        // Offset only mode: Only apply offset to system tray, reduce taskbar width
        double offsetValue = static_cast<double>(g_settings.offsetValue);

        // Reduce taskbar width by 2*offset to allow system tray to overlap
        // Clamp to a minimum of 100 pixels to prevent visual corruption from negative/tiny widths
        constexpr double minTaskbarWidth = 100.0;
        double newTaskbarWidth = g_originalTaskbarFrameWidth - (2.0 * offsetValue);
        newTaskbarWidth = (std::max)(newTaskbarWidth, minTaskbarWidth);

        taskbarFrame.Width(newTaskbarWidth);

        // Apply offset only to system tray using margin (shift left into taskbar area)
        auto systemTrayMargin = systemTrayFrame.Margin();
        systemTrayMargin.Right = offsetValue; // Positive right margin shifts left
        systemTrayFrame.Margin(systemTrayMargin);

        Wh_Log(L"Offset only mode: SystemTray margin.Right=%f, TaskbarFrame width=%f (clamped from %f)",
               offsetValue, newTaskbarWidth, g_originalTaskbarFrameWidth - (2.0 * offsetValue));
    }
    else
    {
        // Center mode: Center both as a single unit using transforms

        // Apply max width if configured
        if (g_settings.maxTotalWidth > 0)
        {
            taskbarFrame.Width(static_cast<double>(g_settings.maxTotalWidth));
        }

        // Set SystemTrayFrame to center alignment for proper centering
        systemTrayFrame.HorizontalAlignment(HorizontalAlignment::Center);

        // Calculate the gap between taskbar buttons and system tray
        // The gap setting adds extra spacing (positive = apart, negative = closer)
        double halfGap = g_settings.gap / 2.0;

        // Apply minimum total width setting
        // When minTotalWidth is set and the actual width is smaller, we increase the gap
        // to make the centered group appear wider
        double effectiveTotalWidth = taskbarButtonsWidth + systemTrayWidth;
        if (g_settings.minTotalWidth > 0 && effectiveTotalWidth < g_settings.minTotalWidth)
        {
            // Calculate extra gap needed to reach minimum width
            double extraGap = (g_settings.minTotalWidth - effectiveTotalWidth) / 2.0;
            halfGap += extraGap;
        }

        // Calculate offsets for centering both as a single unit
        // The goal is to position the combined taskbar+systray group in the center
        // TaskbarFrame shifts left (negative X offset) by half the system tray width
        // SystemTrayFrame shifts right (positive X offset) by half the taskbar buttons width
        // The halfGap adds spacing between them
        double taskbarOffset = -(systemTrayWidth / 2.0 + halfGap);
        double systemTrayOffset = taskbarButtonsWidth / 2.0 + halfGap;

        // Apply transforms for visual positioning
        ApplyTransformOffset(taskbarFrame, taskbarOffset);
        ApplyTransformOffset(systemTrayFrame, systemTrayOffset);

        Wh_Log(L"Center mode: TaskbarFrame offset=%f, SystemTray offset=%f (gap=%d)",
               taskbarOffset, systemTrayOffset, g_settings.gap);
    }

    return true;
}

// Public entry point that always attempts width capture if needed
bool ApplyStyle(XamlRoot xamlRoot)
{
    return ApplyStyleInternal(xamlRoot, false);
}

// Symbol hook targets
void *CTaskBand_ITaskListWndSite_vftable;
void *CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void *TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI *)(void *pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void *taskbarHostSharedPtr[2])
{
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1])
    {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

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
    // Just use the default offset which will hopefully work in most cases.
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

void ApplySettingsFromTaskbarThread()
{
    Wh_Log(L"Applying settings");

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
                // Skip secondary taskbars for now - only apply to primary
                return TRUE;
            }
            else
            {
                return TRUE;
            }

            if (!xamlRoot)
            {
                Wh_Log(L"Getting XamlRoot failed");
            return TRUE;
            }

            if (!ApplyStyle(xamlRoot))
            {
                Wh_Log(L"ApplyStyle failed");
                return TRUE;
            }

                return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *pParam)
        { ApplySettingsFromTaskbarThread(); }, 0);
}

// Hook for TaskbarFrame::SystemTrayExtent - called when system tray size changes
using TaskbarFrame_SystemTrayExtent_t = void(WINAPI *)(void *pThis, double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;
void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void *pThis, double value)
{
    Wh_Log(L"> SystemTrayExtent: %f", value);

    TaskbarFrame_SystemTrayExtent_Original(pThis, value);

    if (g_unloading)
    {
        return;
    }

    // Validate pThis pointer before accessing
    if (!pThis)
    {
        Wh_Log(L"pThis is null, skipping");
        return;
    }

    // Get the TaskbarFrame element
    // The IUnknown pointer is at offset 1 in the object's vtable array
    IUnknown **pThisArray = (IUnknown **)pThis;
    IUnknown *pUnknown = pThisArray[1];
    if (!pUnknown)
    {
        Wh_Log(L"IUnknown pointer at offset 1 is null, skipping");
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    HRESULT hr = pUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                          winrt::put_abi(taskbarFrameElement));
    if (FAILED(hr) || !taskbarFrameElement)
    {
        Wh_Log(L"QueryInterface failed or returned null element, hr=%08X", hr);
        return;
    }

    auto xamlRoot = taskbarFrameElement.XamlRoot();
    if (xamlRoot)
    {
        ApplyStyle(xamlRoot);
    }
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
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module)
{
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks)))
    {
        Wh_Log(L"HookSymbols failed for Taskbar.View.dll");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle()
{
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module)
    {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName)
{
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true))
    {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module))
        {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags)
{
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module)
    {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings()
{
    g_settings.offsetOnlyMode = Wh_GetIntSetting(L"offsetOnlyMode") != 0;

    // Load and validate offsetValue (minimum 0)
    // Negative values would shift system tray right (wrong direction)
    // No upper limit - users may want large offsets for ultrawide monitors
    int offsetValue = Wh_GetIntSetting(L"OffsetOnlySettings.offsetValue");
    g_settings.offsetValue = (std::max)(0, offsetValue);

    // Load and validate minTotalWidth (minimum 0)
    // 0 means disabled, negative values make no sense
    // No upper limit - users may want very large minimum widths
    int minTotalWidth = Wh_GetIntSetting(L"CenterSettings.minTotalWidth");
    g_settings.minTotalWidth = (std::max)(0, minTotalWidth);

    // Load gap without clamping
    // Negative values pull elements closer, positive push apart
    // No limits - users have full control over spacing
    g_settings.gap = Wh_GetIntSetting(L"CenterSettings.gap");

    // Load and validate maxTotalWidth (minimum 0)
    // 0 means disabled (no maximum), negative values make no sense
    // No upper limit - users may want very large maximum widths
    int maxTotalWidth = Wh_GetIntSetting(L"CenterSettings.maxTotalWidth");
    g_settings.maxTotalWidth = (std::max)(0, maxTotalWidth);

    Wh_Log(L"Settings loaded: offsetOnlyMode=%d, offsetValue=%d, minTotalWidth=%d, gap=%d, maxTotalWidth=%d",
           g_settings.offsetOnlyMode, g_settings.offsetValue, g_settings.minTotalWidth, g_settings.gap,
           g_settings.maxTotalWidth);
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols())
    {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
    {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule))
        {
            return FALSE;
        }
    }
    else
    {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded)
    {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
        {
            if (!g_taskbarViewDllLoaded.exchange(true))
            {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule))
                {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L">");

    g_unloading = true;

    // Clean up any pending LayoutUpdated event subscription
    if (g_layoutUpdatedToken)
    {
        auto taskbarFrame = g_taskbarFrameWeakRef.get();
        if (taskbarFrame)
        {
            try
            {
                taskbarFrame.LayoutUpdated(g_layoutUpdatedToken);
            }
            catch (...)
            {
                // Ignore errors during cleanup
            }
        }
        g_layoutUpdatedToken = {};
    }
    g_taskbarFrameWeakRef = nullptr;
    g_pendingXamlRootWeakRef = nullptr;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit()
{
    Wh_Log(L">");
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    // Note: We do NOT reset g_originalTaskbarFrameWidth here
    // because we want to preserve the original width captured at mod init
    // The original width should only be captured once and reused

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
    {
        ApplySettings(hTaskbarWnd);
    }
}

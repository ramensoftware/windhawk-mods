// ==WindhawkMod==
// @id              taskbar-numberer
// @name            Taskbar Numberer for Windows 11
// @description     Displays keyboard shortcut numbers (1-9, 0) on taskbar items like 7+ Taskbar Numberer
// @version         1.0.0
// @author          Rik Smeets & jsfdez
// @github          https://github.com/rik-smeets
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Numberer for Windows 11
Displays keyboard shortcut numbers (1-9, 0) on Windows 11 taskbar items, replicating the functionality of the classic 7+ Taskbar Numberer tool.
![](https://raw.githubusercontent.com/jsfdez/images/main/taskbar-icon-numberer.wh.cpp.png)

## Features
- Numbers 1-9 for first nine apps, 0 for tenth app
- Fully customizable appearance (position, size, colors)
- Stroke outline for better visibility on any background
- Supports light and dark themes
- Hidden overlays for apps beyond position 10
- Efficient overlay management with minimal performance impact

## Usage
Numbers correspond to Windows + [number] keyboard shortcuts for quick app launching. Simply press Win+1 to open the first app, Win+2 for second, etc.

## Known issues
- After reordering task bar items, numbering might be incorrect. This self-corrects in the next render.
- After initially enabling mod: numbers don't appear until first taskbar interaction (like hovering). If mod is enabled, numbers will be there on Windows startup.

## Customization
The following settings can be configured through mod settings:
- Number position (corner)
- Font size
- Text color
- Stroke color
- Enable/disable showing numbers on secondary taskbars

## Special Thanks
- [jsfdez](https://github.com/jsfdez): Created the base for this mod in [this pull request](https://github.com/ramensoftware/windhawk-mods/pull/2260).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- numberPosition: topLeft
  $name: Number position
  $options:
  - topLeft: Top left
  - topRight: Top right
  - bottomLeft: Bottom left
  - bottomRight: Bottom right
- numberSize: 10
  $name: Number font size
  $description: Size of the overlay numbers (8-16)
- numberColor: "#FFFFFF"
  $name: Number color
  $description: Text color for the numbers (hex format "#RRGGBB" or "#AARRGBB")
- backgroundColor: "#80000000"
  $name: Stroke/outline color
  $description: Outline color around the numbers for better visibility (hex format "#RRGGBB" or "#AARRGBB")
- showOnAllTaskbars: false
  $name: Show numbers on all taskbars
  $description: 'If enabled and Windows is set to show taskbar apps on all taskbars, numbers appear on secondary taskbar(s) as well.'
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>
#include <winrt/Windows.UI.ViewManagement.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <functional>
#include <commctrl.h>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Core;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Hook function pointers
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

// For resolving a button's app group from its TaskItem.
// Button → TaskListWindowViewModel → WindowsUdk TaskItem → ReportClicked
// sentinel → CTaskListWnd::HandleClick → native ITaskGroup*.
using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

// Click sentinel — triggers CTaskListWnd::HandleClick to capture the native
// ITaskGroup pointer.
WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (*launcherOptions == &g_clickSentinel) {
        g_clickSentinel_TaskGroup = taskGroup;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

enum class NumberPosition {
    topLeft,
    topRight,
    bottomLeft,
    bottomRight
};

struct {
    NumberPosition numberPosition;
    int numberSize;
    std::wstring numberColor;
    std::wstring backgroundColor;
    bool showOnAllTaskbars;
} g_settings;

// Track only primary taskbar buttons with their overlay state
struct ButtonInformation {
    winrt::weak_ref<FrameworkElement> button;
    FrameworkElement overlay = nullptr;
    int currentNumber = -1;
    bool isVisible = false;

    // Constructor for easier creation
    explicit ButtonInformation(const FrameworkElement& button = nullptr)
        : button(button) {}

    bool isSameButton(const FrameworkElement& otherButton) const {
        auto thisButton = button.get();
        if (!thisButton) return false;
        return winrt::get_abi(thisButton.as<winrt::Windows::Foundation::IUnknown>()) 
            == winrt::get_abi(otherButton.as<winrt::Windows::Foundation::IUnknown>());
    }
};

using XamlRootKey = void*;

// Simplified globals
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};
std::mutex g_overlayMutex;
std::unordered_map<XamlRootKey, std::vector<ButtonInformation>> g_trackedButtonsByRoot;

// Helper function to find button in vector
auto FindButtonInVector = [](const std::vector<ButtonInformation>& buttons, const FrameworkElement& targetButton) {
    return std::find_if(buttons.begin(), buttons.end(),
        [&targetButton](const ButtonInformation& info) {
            return info.isSameButton(targetButton);
        });
};

// Utility functions
void LogException(const char* functionName) {
    try {
        std::rethrow_exception(std::current_exception());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[%S] WinRT exception: 0x%08X - %s", functionName, ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"[%S] Standard exception: %S", functionName, ex.what());
    } catch (...) {
        Wh_Log(L"[%S] Unknown exception", functionName);
    }
}

Color ParseHexColor(const std::wstring& hex) {
    Color color{};
    if (hex.length() >= 7 && hex[0] == L'#') {
        try {
            if (hex.length() == 9) { // #AARRGGBB
                color.A = static_cast<uint8_t>(std::wcstoul(hex.substr(1, 2).c_str(), nullptr, 16));
                color.R = static_cast<uint8_t>(std::wcstoul(hex.substr(3, 2).c_str(), nullptr, 16));
                color.G = static_cast<uint8_t>(std::wcstoul(hex.substr(5, 2).c_str(), nullptr, 16));
                color.B = static_cast<uint8_t>(std::wcstoul(hex.substr(7, 2).c_str(), nullptr, 16));
            } else { // #RRGGBB
                color.A = 255;
                color.R = static_cast<uint8_t>(std::wcstoul(hex.substr(1, 2).c_str(), nullptr, 16));
                color.G = static_cast<uint8_t>(std::wcstoul(hex.substr(3, 2).c_str(), nullptr, 16));
                color.B = static_cast<uint8_t>(std::wcstoul(hex.substr(5, 2).c_str(), nullptr, 16));
            }
        } catch (...) {
            color.A = 255; color.R = 255; color.G = 255; color.B = 255;
            LogException(__FUNCTION__);
        }
    } else {
        color.A = 255; color.R = 255; color.G = 255; color.B = 255;
    }
    return color;
}

FrameworkElement EnumerateChildren(FrameworkElement element, std::function<bool(FrameworkElement)> callback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (callback(child)) return child;
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    if (!element) return nullptr;

    try {
        int childrenCount = VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < childrenCount; i++) {
            auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
            if (child && child.Name() == name) {
                return child;
            }
        }
    } catch (...) {
        LogException(__FUNCTION__);
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element, PCWSTR className) {
    return EnumerateChildren(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
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
    if (!controlCenterButton) return false;
    return controlCenterButton.ActualWidth() < 5;
}

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
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
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

// Simplified text creation helper
TextBlock CreateTextBlock(const std::wstring& text, double fontSize, const Color& color, const Thickness& margin = {}) {
    TextBlock textBlock;
    textBlock.Text(text);
    textBlock.FontSize(fontSize);
    textBlock.Margin(margin);

    auto brush = SolidColorBrush();
    brush.Color(color);
    textBlock.Foreground(brush);

    return textBlock;
}

void SetNumberPosition(Grid& textContainer) {
    switch (g_settings.numberPosition) {
        case NumberPosition::bottomRight:
            textContainer.HorizontalAlignment(HorizontalAlignment::Right);
            textContainer.VerticalAlignment(VerticalAlignment::Bottom);
            break;
        case NumberPosition::bottomLeft:
            textContainer.HorizontalAlignment(HorizontalAlignment::Left);
            textContainer.VerticalAlignment(VerticalAlignment::Bottom);
            break;
        case NumberPosition::topRight:
            textContainer.HorizontalAlignment(HorizontalAlignment::Right);
            textContainer.VerticalAlignment(VerticalAlignment::Top);
            break;
        default:  // topLeft
            textContainer.HorizontalAlignment(HorizontalAlignment::Left);
            textContainer.VerticalAlignment(VerticalAlignment::Top);
            break;
    }
}

FrameworkElement CreateNumberOverlay(int number) {
    if (number < 1) {
        Wh_Log(L"CreateNumberOverlay: Invalid number %d, skipping creation", number);
        return nullptr;
    }

    try {
        std::wstring text = (number == 10) ? L"0" : std::to_wstring(number);
        auto textColor = ParseHexColor(g_settings.numberColor);
        auto strokeColor = ParseHexColor(g_settings.backgroundColor);

        Grid textContainer;
        textContainer.Name(L"WindhawkNumberOverlay");

        // Create stroke effect
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;

                auto strokeText = CreateTextBlock(text, g_settings.numberSize, strokeColor,
                    Thickness{static_cast<double>(dx), static_cast<double>(dy), 0, 0});
                textContainer.Children().Append(strokeText);
            }
        }

        // Main text
        auto numberText = CreateTextBlock(text, g_settings.numberSize, textColor);
        textContainer.Children().Append(numberText);

        // Set position and properties
        SetNumberPosition(textContainer);

        textContainer.Margin(Thickness{4, 2, 4, 2});
        Canvas::SetZIndex(textContainer, 1000);

        if (number > 10) {
            textContainer.Visibility(Visibility::Collapsed);
        }

        Wh_Log(L"CreateNumberOverlay: Created overlay with number %d", number);
        return textContainer;

    } catch (...) {
        LogException(__FUNCTION__);
        return nullptr;
    }
}

// Simplified button cleanup
void CleanupButtonOverlays(const ButtonInformation& buttonInfo) {
    try {
        auto button = buttonInfo.button.get();
        if (!button) return;

        auto iconPanel = FindChildByName(button, L"IconPanel");
        if (!iconPanel) return;

        auto panelChildren = iconPanel.as<Panel>().Children();
        for (uint32_t j = 0; j < panelChildren.Size();) {
            auto child = panelChildren.GetAt(j).try_as<FrameworkElement>();
            if (child && child.Name() == L"WindhawkNumberOverlay") {
                panelChildren.RemoveAt(j);
            } else {
                j++;
            }
        }
    } catch (...) {
        LogException(__FUNCTION__);
    }
}

XamlRootKey GetXamlRootKeyFromElement(const FrameworkElement& element) {
    if (!element) return nullptr;
    auto root = element.XamlRoot();
    if (!root) return nullptr;
    return winrt::get_abi(root);
}

void RemoveAllNumberOverlays() {
    try {
        std::unordered_map<XamlRootKey, std::vector<ButtonInformation>> copy;
        {
            std::lock_guard<std::mutex> lock(g_overlayMutex);
            copy = g_trackedButtonsByRoot;
            g_trackedButtonsByRoot.clear();
        }

        Wh_Log(L"RemoveAllNumberOverlays: Cleaning %zu XamlRoots", copy.size());

        for (auto& [key, buttons] : copy) {
            FrameworkElement anyButton = nullptr;
            for (auto& bi : buttons) {
                anyButton = bi.button.get();
                if (anyButton) break;
            }
            if (!anyButton) continue;

            auto dispatcher = anyButton.Dispatcher();
            if (!dispatcher) continue;

            auto buttonsCopy = buttons;
            dispatcher.RunAsync(
                CoreDispatcherPriority::Normal,
                DispatchedHandler([buttonsCopy]() {
                    for (const auto& bi : buttonsCopy) {
                        CleanupButtonOverlays(bi);
                    }
                })
            );
        }
    } catch (...) {
        LogException(__FUNCTION__);
    }
}

void RemoveExistingOverlays(FrameworkElement iconPanel) {
    auto panelChildren = iconPanel.as<Panel>().Children();
    uint32_t removedCount = 0;
    for (uint32_t j = 0; j < panelChildren.Size();) {
        auto child = panelChildren.GetAt(j).try_as<FrameworkElement>();
        if (child && child.Name() == L"WindhawkNumberOverlay") {
            panelChildren.RemoveAt(j);
            removedCount++;
        } else {
            j++;
        }
    }
    if (removedCount > 0) {
        Wh_Log(L"RemoveExistingOverlays: Removed %d old overlays", removedCount);
    }
}

void UpdateButtonOverlay(FrameworkElement taskListButtonElement, int number, void* buttonPointer) {
    if (!taskListButtonElement || g_unloading || number < 1) {
        if (number < 1) Wh_Log(L"UpdateButtonOverlay: Invalid number %d, skipping", number);
        return;
    }

    try {
        auto iconPanelElement = FindChildByName(taskListButtonElement, L"IconPanel");
        if (!iconPanelElement) {
            Wh_Log(L"UpdateButtonOverlay: No IconPanel found");
            return;
        }

        winrt::Windows::Foundation::IUnknown buttonIUnknown;
        winrt::copy_from_abi(buttonIUnknown, buttonPointer);
        auto button = buttonIUnknown.as<FrameworkElement>();

        auto key = GetXamlRootKeyFromElement(button);
        if (!key) {
            Wh_Log(L"UpdateButtonOverlay: No XamlRoot for button");
            return;
        }

        std::lock_guard<std::mutex> lock(g_overlayMutex);
        auto& vector = g_trackedButtonsByRoot[key];

        auto it = FindButtonInVector(vector, button);
        ButtonInformation buttonInfo;

        if (it != vector.end()) {
            buttonInfo = *it;
            vector.erase(it);
            Wh_Log(L"UpdateButtonOverlay: Found existing button, current number %d, new number %d",
                   buttonInfo.currentNumber, number);
        } else {
            buttonInfo = ButtonInformation(button);
            Wh_Log(L"UpdateButtonOverlay: New button, number %d", number);
        }

        const bool shouldShow = number <= 10;
        const bool visibilityChanged = buttonInfo.isVisible != shouldShow;
        const bool numberChanged = buttonInfo.currentNumber != number;
        const bool needsNewOverlay = !buttonInfo.overlay || (numberChanged && buttonInfo.overlay);

        if (!visibilityChanged && !numberChanged && !needsNewOverlay) {
            vector.push_back(buttonInfo);
            return;
        }

        // Create new overlay if needed
        if (needsNewOverlay) {
            RemoveExistingOverlays(iconPanelElement);
            buttonInfo.overlay = nullptr;

            auto newOverlay = CreateNumberOverlay(number);
            if (newOverlay) {
                iconPanelElement.as<Panel>().Children().Append(newOverlay);
                buttonInfo.overlay = newOverlay;
            }
        }

        // Update visibility
        if (buttonInfo.overlay && visibilityChanged) {
            buttonInfo.overlay.Visibility(shouldShow ? Visibility::Visible : Visibility::Collapsed);
        }

        buttonInfo.currentNumber = number;
        buttonInfo.isVisible = shouldShow;
        vector.push_back(buttonInfo);

    } catch (...) {
        LogException(__FUNCTION__);
    }
}

// Get the native ITaskGroup pointer for a taskbar button.
// For grouped items each group is already a single button, so they naturally
// get distinct numbers. This function is only needed for ungrouped items where
// multiple windows of the same app each have their own button.
void* GetTaskGroupFromTaskListButton(UIElement element) {
    if (!TryGetItemFromContainer_TaskListWindowViewModel_Original ||
        !TaskListWindowViewModel_get_TaskItem_Original ||
        !TaskItem_ReportClicked_Original) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    if (!windowsUdkTaskItem) {
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                    &g_clickSentinel);
    return g_clickSentinel_TaskGroup;
}

// Only when user setting is enabled and Windows taskbar mode is set to "All taskbars"
bool ShouldShowOnAllTaskbars() {
    if (!g_settings.showOnAllTaskbars) return false;

    try {
        DWORD value = 0;
        DWORD size = sizeof(value);
        if (RegGetValueW(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                L"MMTaskbarMode",
                RRF_RT_REG_DWORD,
                nullptr,
                &value,
                &size) == ERROR_SUCCESS) {
            return value == 0; // 0 = All taskbars
        }
        return false; // Default to safe behavior
    } catch (...) {
        LogException(__FUNCTION__);
        return false; // Default to safe behavior on error
    }
}


void UpdateAllTaskbarNumbers(FrameworkElement taskbarRepeater) {
    if (g_unloading) return;

    try {
        const auto xamlRoot = taskbarRepeater.XamlRoot();
        if (!xamlRoot) return;
        if (!ShouldShowOnAllTaskbars() && IsSecondaryTaskbar(xamlRoot)) return;

        auto panel = taskbarRepeater.as<Panel>();
        if (!panel) return;

        auto children = panel.Children();
        std::vector<std::pair<double, FrameworkElement>> buttons;

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (!child) continue;

            if (child.Name() != L"TaskListButton") continue;
            if (child.Visibility() != Visibility::Visible) continue;
            if (child.ActualWidth() <= 0 || child.ActualHeight() <= 0) continue;
            if (child.Opacity() <= 0.1) continue;

            double x = child.ActualOffset().x;
            if (x < 0) continue;

            buttons.push_back({x, child});
        }

        if (buttons.empty()) {
            Wh_Log(L"UpdateAllTaskbarNumbers: No valid buttons found, skipping");
            return;
        }

        std::sort(buttons.begin(), buttons.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        Wh_Log(L"UpdateAllTaskbarNumbers: Found %zu taskbar buttons", buttons.size());

        // Clean up buttons no longer present
        std::unordered_set<FrameworkElement> currentButtons;
        for (const auto& [pos, button] : buttons) {
            currentButtons.insert(button);
        }

        auto key = GetXamlRootKeyFromElement(taskbarRepeater);
        if (!key) return;

        {
            std::lock_guard<std::mutex> lock(g_overlayMutex);
            auto itMap = g_trackedButtonsByRoot.find(key);
            if (itMap != g_trackedButtonsByRoot.end()) {
                auto& vector = itMap->second;
                auto it = vector.begin();
                size_t removedCount = 0;
                while (it != vector.end()) {
                    auto button = it->button.get();
                    if (!button || currentButtons.find(button) == currentButtons.end()) {
                        it = vector.erase(it);
                        removedCount++;
                    } else {
                        ++it;
                    }
                }
                if (removedCount > 0) {
                    Wh_Log(L"UpdateAllTaskbarNumbers: Removed %zu buttons no longer present", removedCount);
                }
            }
        }

        int currentNumber = 0;
        void* lastGroupId = nullptr;

        for (size_t i = 0; i < buttons.size(); i++) {
            auto button = buttons[i].second;
            void* buttonPointer = winrt::get_abi(button.as<winrt::Windows::Foundation::IUnknown>());

            // Use the native ITaskGroup pointer as group identity.
            // Buttons in the same app group share the same ITaskGroup.
            UIElement buttonElement = button.as<UIElement>();
            void* groupId = GetTaskGroupFromTaskListButton(buttonElement);

            // Increment when group changes, or when group is unknown.
            if (!groupId || groupId != lastGroupId) {
                currentNumber++;
            }

            lastGroupId = groupId;

            int buttonNumber = currentNumber;

            Wh_Log(L"Button %zu groupId=%p assigned number %d", i, groupId, buttonNumber);

            UpdateButtonOverlay(button, buttonNumber, buttonPointer);
        }

        Wh_Log(L"UpdateAllTaskbarNumbers: Processing complete");
    } catch (...) {
        Wh_Log(L"UpdateAllTaskbarNumbers: Exception occurred");
        LogException(__FUNCTION__);
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    // Call the original function first
    if (TaskListButton_UpdateVisualStates_Original) {
        TaskListButton_UpdateVisualStates_Original(pThis);
    }

    try {
        void* taskListButtonIUnknownPointer = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPointer);

        auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();
        if (!taskListButtonElement) return;

        if (auto xamlRoot = taskListButtonElement.XamlRoot()) {
            if (!ShouldShowOnAllTaskbars() && IsSecondaryTaskbar(xamlRoot)) return;
        }

        auto parent = VisualTreeHelper::GetParent(taskListButtonElement).as<FrameworkElement>();
        if (!parent || parent.Name() != L"TaskbarFrameRepeater") return;

        if (!g_unloading) UpdateAllTaskbarNumbers(parent);
    } catch (...) {
        LogException(__FUNCTION__);
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

void LoadSettings() {
    PCWSTR position = Wh_GetStringSetting(L"numberPosition");
    g_settings.numberPosition = NumberPosition::bottomRight;
    if (wcscmp(position, L"topLeft") == 0) g_settings.numberPosition = NumberPosition::topLeft;
    else if (wcscmp(position, L"topRight") == 0) g_settings.numberPosition = NumberPosition::topRight;
    else if (wcscmp(position, L"bottomLeft") == 0) g_settings.numberPosition = NumberPosition::bottomLeft;
    else if (wcscmp(position, L"bottomRight") == 0) g_settings.numberPosition = NumberPosition::bottomRight;
    Wh_FreeStringSetting(position);

    g_settings.numberSize = Wh_GetIntSetting(L"numberSize");
    if (g_settings.numberSize < 8) g_settings.numberSize = 8;
    if (g_settings.numberSize > 16) g_settings.numberSize = 16;

    PCWSTR numberColor = Wh_GetStringSetting(L"numberColor");
    g_settings.numberColor = numberColor;
    Wh_FreeStringSetting(numberColor);

    PCWSTR backgroundColor = Wh_GetStringSetting(L"backgroundColor");
    g_settings.backgroundColor = backgroundColor;
    Wh_FreeStringSetting(backgroundColor);

    g_settings.showOnAllTaskbars = Wh_GetIntSetting(L"showOnAllTaskbars");
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarDllSymbols()) return FALSE;

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded)
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
            if (!g_taskbarViewDllLoaded.exchange(true))
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) Wh_ApplyHookOperations();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    RemoveAllNumberOverlays();
    Sleep(100);
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, clearing existing overlays");
    LoadSettings();
    RemoveAllNumberOverlays();
}

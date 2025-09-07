// ==WindhawkMod==
// @id              taskbar-icon-numberer
// @name            Taskbar Icon Numberer for Windows 11
// @description     Add keyboard shortcut numbers (1-9, 0) to taskbar icons like 7+ Taskbar Numberer
// @version         0.0.6
// @author          js
// @github          https://github.com/jsfdez
// @homepage        https://tightcorner.substack.com/p/reverse-engineering-windows-11s-taskbar
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Icon Numberer for Windows 11
Displays keyboard shortcut numbers (1-9, 0) as overlays on Windows 11 taskbar icons, replicating the functionality of the classic 7+ Taskbar Numberer tool.
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
## Known Issues
- Numbers don't appear until first taskbar interaction (hover over any button)
- Occasional wrong numbers during initial display (self-corrects after reordering)
## Customization
Configure number position (corners), font size (8-16px), text color, and stroke color through mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- numberPosition: bottomRight
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
#include <mutex>
#include <vector>
#include <functional>
#include <commctrl.h>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Hook function pointers
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

enum class NumberPosition {
    topLeft,
    topRight,
    bottomLeft,
    bottomRight,
};

struct {
    NumberPosition numberPosition;
    int numberSize;
    std::wstring numberColor;
    std::wstring backgroundColor;
} g_settings;

// Track only primary taskbar buttons with their overlay state
struct ButtonInformation {
    winrt::weak_ref<FrameworkElement> button;
    FrameworkElement overlay = nullptr;
    int currentNumber = -1;
    bool isVisible = false;
    
    // Constructor for easier creation
    explicit ButtonInformation(const FrameworkElement& button = nullptr) : button(button) {}
    
    bool isSameButton(const FrameworkElement& otherButton) const {
        auto thisButton = button.get();
        if (!thisButton) return false;
        return winrt::get_abi(thisButton.as<winrt::Windows::Foundation::IUnknown>()) == winrt::get_abi(otherButton.as<winrt::Windows::Foundation::IUnknown>());
    }
};

// Simplified globals
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};
std::mutex g_overlayMutex;
std::vector<ButtonInformation> g_trackedButtons;

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

winrt::Windows::UI::Color ParseHexColor(const std::wstring& hex) {
    winrt::Windows::UI::Color color{};
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
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (callback(child)) return child;
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    if (!element) return nullptr;
    
    try {
        int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < childrenCount; i++) {
            auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
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

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            DWORD dwCurrentProcessId = GetCurrentProcessId();
            WCHAR className[32];

            if (GetWindowThreadProcessId(hWnd, &dwProcessId) == FALSE)
            {
                Wh_Log(L"GetWindowThreadProcessId failed");
                return TRUE;
            }

            if (dwProcessId != dwCurrentProcessId) return TRUE;
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) == FALSE) return TRUE;
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                Wh_Log(L"Found %s in %u", className, dwProcessId);
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
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
TextBlock CreateTextBlock(const std::wstring& text, double fontSize, const winrt::Windows::UI::Color& color, const Thickness& margin = {}) {
    TextBlock textBlock;
    textBlock.Text(text);
    textBlock.FontSize(fontSize);
    textBlock.Margin(margin);
    
    auto brush = SolidColorBrush();
    brush.Color(color);
    textBlock.Foreground(brush);
    
    return textBlock;
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
        switch (g_settings.numberPosition) {
            case NumberPosition::topLeft:
                textContainer.HorizontalAlignment(HorizontalAlignment::Left);
                textContainer.VerticalAlignment(VerticalAlignment::Top);
                break;
            case NumberPosition::topRight:
                textContainer.HorizontalAlignment(HorizontalAlignment::Right);
                textContainer.VerticalAlignment(VerticalAlignment::Top);
                break;
            case NumberPosition::bottomLeft:
                textContainer.HorizontalAlignment(HorizontalAlignment::Left);
                textContainer.VerticalAlignment(VerticalAlignment::Bottom);
                break;
            default: // bottomRight
                textContainer.HorizontalAlignment(HorizontalAlignment::Right);
                textContainer.VerticalAlignment(VerticalAlignment::Bottom);
                break;
        }
        
        textContainer.Margin(Thickness{0, 0, 2, 2});
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

void RemoveAllNumberOverlays() {
    try {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!taskbarWnd) return;
        
        RunFromWindowThread(taskbarWnd, [](void*) {
            try {
                std::lock_guard<std::mutex> lock(g_overlayMutex);
                for (const auto& buttonInfo : g_trackedButtons) {
                    CleanupButtonOverlays(buttonInfo);
                }
                g_trackedButtons.clear();
            } catch (...) {
                LogException(__FUNCTION__);
            }
        }, nullptr);
    } catch (...) {
        LogException(__FUNCTION__);
    }
}

// Simplified overlay update for settings changes
void UpdateOverlayAppearance(Grid& grid, const std::wstring& text) {
    auto children = grid.Children();
    children.Clear();
    
    auto textColor = ParseHexColor(g_settings.numberColor);
    auto strokeColor = ParseHexColor(g_settings.backgroundColor);
    
    // Create stroke effect
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            
            auto strokeText = CreateTextBlock(text, g_settings.numberSize, strokeColor,
                Thickness{static_cast<double>(dx), static_cast<double>(dy), 0, 0});
            children.Append(strokeText);
        }
    }
    
    // Main text
    auto numberText = CreateTextBlock(text, g_settings.numberSize, textColor);
    children.Append(numberText);
}

void UpdateOverlayWithNewSettings(const ButtonInformation& buttonInformation) {
    if (!buttonInformation.overlay) {
        Wh_Log(L"UpdateOverlayWithNewSettings: No overlay to update");
        return;
    }
    
    try {
        auto grid = buttonInformation.overlay.as<Grid>();
        std::wstring text = (buttonInformation.currentNumber == 10) ? L"0" : std::to_wstring(buttonInformation.currentNumber);
        
        UpdateOverlayAppearance(grid, text);
        
        // Update position
        switch (g_settings.numberPosition) {
            case NumberPosition::topLeft:
                grid.HorizontalAlignment(HorizontalAlignment::Left);
                grid.VerticalAlignment(VerticalAlignment::Top);
                break;
            case NumberPosition::topRight:
                grid.HorizontalAlignment(HorizontalAlignment::Right);
                grid.VerticalAlignment(VerticalAlignment::Top);
                break;
            case NumberPosition::bottomLeft:
                grid.HorizontalAlignment(HorizontalAlignment::Left);
                grid.VerticalAlignment(VerticalAlignment::Bottom);
                break;
            default: // bottomRight
                grid.HorizontalAlignment(HorizontalAlignment::Right);
                grid.VerticalAlignment(VerticalAlignment::Bottom);
                break;
        }
        
        Wh_Log(L"UpdateOverlayWithNewSettings: Successfully updated overlay");
        
    } catch (...) {
        Wh_Log(L"UpdateOverlayWithNewSettings: Failed to update overlay");
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

        std::lock_guard<std::mutex> lock(g_overlayMutex);
        
        winrt::Windows::Foundation::IUnknown buttonIUnknown;
        winrt::copy_from_abi(buttonIUnknown, buttonPointer);
        auto button = buttonIUnknown.as<FrameworkElement>();

        auto it = FindButtonInVector(g_trackedButtons, button);
        ButtonInformation buttonInfo;
        
        if (it != g_trackedButtons.end()) {
            buttonInfo = *it;
            g_trackedButtons.erase(it);
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
            g_trackedButtons.push_back(buttonInfo);
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
        g_trackedButtons.push_back(buttonInfo);
        
    } catch (...) {
        LogException(__FUNCTION__);
    }
}

void UpdateAllTaskbarNumbers(FrameworkElement taskbarRepeater) {
    if (g_unloading) return;
    
    try {
        const auto xamlRoot = taskbarRepeater.XamlRoot();
        if (!xamlRoot || IsSecondaryTaskbar(xamlRoot)) return;
        
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
        
        {
            std::lock_guard<std::mutex> lock(g_overlayMutex);
            auto it = g_trackedButtons.begin();
            size_t removedCount = 0;
            while (it != g_trackedButtons.end()) {
                auto button = it->button.get();
                if (!button || currentButtons.find(button) == currentButtons.end()) {
                    it = g_trackedButtons.erase(it);
                    removedCount++;
                } else {
                    ++it;
                }
            }
            if (removedCount > 0) {
                Wh_Log(L"UpdateAllTaskbarNumbers: Removed %zu buttons no longer present", removedCount);
            }
        }

        for (size_t i = 0; i < buttons.size(); i++) {
            auto button = buttons[i].second;
            void* buttonPointer = winrt::get_abi(button.as<winrt::Windows::Foundation::IUnknown>());
            int buttonNumber = static_cast<int>(i + 1);
            
            if (buttonNumber < 1 || buttonNumber > static_cast<int>(buttons.size())) {
                Wh_Log(L"UpdateAllTaskbarNumbers: Invalid button number %d for button %zu", buttonNumber, i);
                continue;
            }
            
            Wh_Log(L"UpdateAllTaskbarNumbers: Button %zu at position %.1f gets number %d", 
                   i, buttons[i].first, buttonNumber);
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
            if (IsSecondaryTaskbar(xamlRoot)) return;
        }

        auto parent = Media::VisualTreeHelper::GetParent(taskListButtonElement).as<FrameworkElement>();
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
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
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
}

BOOL Wh_ModInit() {
    LoadSettings();

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
    Wh_Log(L"Settings changed, updating existing overlays");
    LoadSettings();
    
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (taskbarWnd) {
        RunFromWindowThread(taskbarWnd, [](void*) {
            std::lock_guard<std::mutex> lock(g_overlayMutex);
            
            Wh_Log(L"Updating %zu tracked buttons with new settings", g_trackedButtons.size());
            
            for (const auto& buttonInfo : g_trackedButtons) {
                UpdateOverlayWithNewSettings(buttonInfo);
            }
        }, nullptr);
    }
}

// ==WindhawkMod==
// @id              taskbar-icon-numberer
// @name            Taskbar Icon Numberer for Windows 11
// @description     Add keyboard shortcut numbers (1-9, 0) to taskbar icons like 7+ Taskbar Numberer
// @version         0.0.1
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

Adds keyboard shortcut numbers (1-9, 0) to taskbar icons, similar to the classic 
7+ Taskbar Numberer tool. Numbers appear as small overlays on the taskbar button icons.

Features:
- Shows numbers 1-9 for the first 9 taskbar items, then 0 for the 10th
- Customizable number position, size, and colors
- Works with Windows 11 taskbar
- Supports both light and dark themes

The numbers correspond to the Windows + [number] keyboard shortcuts for quick app launching.
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
  $description: Text color for the numbers (hex format #RRGGBB or #AARRGGBB)
- backgroundColor: "#80000000"
  $name: Stroke/outline color
  $description: Outline color around the numbers for better visibility (hex format #RRGGBB or #AARRGGBB)
- showOnlyRunning: true
  $name: Show only on running apps
  $description: Only show numbers on apps that are currently running
- maxNumbers: 10
  $name: Maximum numbers to show
  $description: Maximum number of taskbar items to number (1-10)
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

#include <algorithm>
#include <atomic>
#include <string>
#include <unordered_set>
#include <mutex>
#include <vector>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

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
        }
    } else {
        color.A = 255; color.R = 255; color.G = 255; color.B = 255;
    }
    return color;
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
    bool showOnlyRunning;
    int maxNumbers;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;
std::mutex g_overlayMutex;
std::unordered_set<void*> g_numberedButtons;

// Hook function pointers
using TaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis, bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

// Utility functions
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
        // Ignore errors
    }
    return nullptr;
}

bool TaskListButton_IsRunning(FrameworkElement taskListButtonElement) {
    if (!TaskListButton_get_IsRunning_Original || !taskListButtonElement) {
        return false;
    }
    
    try {
        bool isRunning = false;
        TaskListButton_get_IsRunning_Original(
            winrt::get_abi(taskListButtonElement.as<winrt::Windows::Foundation::IUnknown>()),
            &isRunning);
        return isRunning;
    } catch (...) {
        return false;
    }
}

void CreateNumberOverlay(FrameworkElement taskListButtonElement, int number) {
    if (!taskListButtonElement || g_unloading) return;
    
    try {
        auto iconPanelElement = FindChildByName(taskListButtonElement, L"IconPanel");
        if (!iconPanelElement) return;

        void* buttonPtr = winrt::get_abi(taskListButtonElement.as<winrt::Windows::Foundation::IUnknown>());
        
        // Remove existing overlay
        auto existingOverlay = FindChildByName(iconPanelElement, L"WindhawkNumberOverlay");
        if (existingOverlay) {
            auto panel = iconPanelElement.as<Panel>();
            auto children = panel.Children();
            uint32_t index;
            if (children.IndexOf(existingOverlay, index)) {
                children.RemoveAt(index);
            }
        }
        
        if (g_unloading) return;

        // Create text with stroke effect
        TextBlock numberText;
        numberText.Text(number == 10 ? L"0" : std::to_wstring(number));
        numberText.FontSize(g_settings.numberSize);
        
        // White text with parsed color
        auto textColor = ParseHexColor(g_settings.numberColor);
        auto brush = SolidColorBrush();
        brush.Color(textColor);
        numberText.Foreground(brush);
        
        // Add stroke effect using multiple text elements
        Grid textContainer;
        textContainer.Name(L"WindhawkNumberOverlay");
        
        // Create stroke (background color texts at small offsets)
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                
                TextBlock strokeText;
                strokeText.Text(number == 10 ? L"0" : std::to_wstring(number));
                strokeText.FontSize(g_settings.numberSize);
                strokeText.Margin(Thickness{static_cast<double>(dx), static_cast<double>(dy), 0, 0});
                
                auto strokeColor = ParseHexColor(g_settings.backgroundColor);
                auto strokeBrush = SolidColorBrush();
                strokeBrush.Color(strokeColor);
                strokeText.Foreground(strokeBrush);
                
                textContainer.Children().Append(strokeText);
            }
        }
        
        // Add main text on top
        textContainer.Children().Append(numberText);
        
        // Position based on settings
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
        
        std::lock_guard<std::mutex> lock(g_overlayMutex);
        iconPanelElement.as<Panel>().Children().Append(textContainer);
        g_numberedButtons.insert(buttonPtr);
        
    } catch (...) {
        // Ignore errors
    }
}

void ClearAllOverlays() {
    std::lock_guard<std::mutex> lock(g_overlayMutex);
    g_numberedButtons.clear();
}

void UpdateAllTaskbarNumbers(FrameworkElement taskbarRepeater) {
    if (g_unloading) return;
    
    try {
        auto panel = taskbarRepeater.as<Panel>();
        if (!panel) return;
        
        auto children = panel.Children();
        
        // Collect all TaskListButtons with their X positions
        std::vector<std::pair<double, FrameworkElement>> buttons;
        
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (!child || child.Name() != L"TaskListButton") continue;
            
            double x = child.ActualOffset().x;
            buttons.push_back({x, child});
        }
        
        // Sort by X position (left to right)
        std::sort(buttons.begin(), buttons.end(), 
                  [](const auto& a, const auto& b) { return a.first < b.first; });
        
        // Number buttons in visual order
        int buttonIndex = 0;
        for (const auto& [x, button] : buttons) {
            if (buttonIndex >= g_settings.maxNumbers) break;
            
            bool shouldShow = true;
            if (g_settings.showOnlyRunning) {
                shouldShow = TaskListButton_IsRunning(button);
            }
            
            if (shouldShow) {
                buttonIndex++;
                CreateNumberOverlay(button, buttonIndex);
            } else {
                // Remove overlay
                auto iconPanel = FindChildByName(button, L"IconPanel");
                if (iconPanel) {
                    auto existingOverlay = FindChildByName(iconPanel, L"WindhawkNumberOverlay");
                    if (existingOverlay) {
                        auto panelChildren = iconPanel.as<Panel>().Children();
                        uint32_t index;
                        if (panelChildren.IndexOf(existingOverlay, index)) {
                            panelChildren.RemoveAt(index);
                        }
                    }
                }
            }
        }
        
    } catch (...) {
        // Ignore errors
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    if (TaskListButton_UpdateVisualStates_Original) {
        TaskListButton_UpdateVisualStates_Original(pThis);
    }
    
    if (g_unloading) return;
    
    try {
        void* taskListButtonIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

        auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();
        if (!taskListButtonElement) return;

        // Find parent repeater
        auto parent = Media::VisualTreeHelper::GetParent(taskListButtonElement).as<FrameworkElement>();
        if (!parent || parent.Name() != L"TaskbarFrameRepeater") return;

        // Update all buttons at once to ensure consistent numbering
        UpdateAllTaskbarNumbers(parent);
        
    } catch (...) {
        // Ignore errors
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
            &TaskListButton_get_IsRunning_Original,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
    };

    return HookSymbols(module, taskbarViewDllHooks, ARRAYSIZE(taskbarViewDllHooks));
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
        
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}

void LoadSettings() {
    PCWSTR position = Wh_GetStringSetting(L"numberPosition");
    g_settings.numberPosition = NumberPosition::bottomRight;
    if (wcscmp(position, L"topLeft") == 0) {
        g_settings.numberPosition = NumberPosition::topLeft;
    } else if (wcscmp(position, L"topRight") == 0) {
        g_settings.numberPosition = NumberPosition::topRight;
    } else if (wcscmp(position, L"bottomLeft") == 0) {
        g_settings.numberPosition = NumberPosition::bottomLeft;
    }
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

    g_settings.showOnlyRunning = Wh_GetIntSetting(L"showOnlyRunning");
    
    g_settings.maxNumbers = Wh_GetIntSetting(L"maxNumbers");
    if (g_settings.maxNumbers < 1) g_settings.maxNumbers = 1;
    if (g_settings.maxNumbers > 10) g_settings.maxNumbers = 10;
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    
    std::lock_guard<std::mutex> lock(g_overlayMutex);
    g_numberedButtons.clear();
}

void Wh_ModUninit() {
    // Cleanup handled in ModBeforeUninit
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ClearAllOverlays();
}

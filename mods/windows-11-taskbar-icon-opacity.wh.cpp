// ==WindhawkMod==
// @id              windows-11-taskbar-icon-opacity
// @name            Windows 11 Taskbar Application Icon Opacity
// @description     Adjust the opacity of taskbar application icons (Windows 11).
// @version         1.0.0
// @author          prpercival
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Application Icon Opacity

This mod allows you to adjust the opacity of application icons on the Windows 11 taskbar. 

### Features
*   **Customizable Opacity:** Set your preferred opacity level for taskbar icons (0% to 100%).
*   **Hover Effect:** Icons automatically restore to full opacity when you hover over them.
*   **Search Box Control:** Choose whether to include or exclude the search box/button from dimming.
*   **Instant Updates:** Changes to settings are applied immediately without needing to restart Explorer (though a restart is recommended after initial install).

### Settings
*   **Icon opacity**: The target transparency level for icons when they are not being hovered over.
*   **Dim search box**: Enable this to apply the opacity effect to the search box as well.

### Note
This mod is designed for Windows 11.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconOpacity: 100
  $name: Icon opacity
  $description: >-
    The opacity of the taskbar icons, in percent (0-100).
- DimSearchBox: false
  $name: Dim search box
  $description: >-
    Whether to apply the opacity setting to the search box/button as well.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Core.h>
#include <vector>
#include <set>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cwctype>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Automation;

struct {
    int iconOpacity;
    bool dimSearchBox;
} g_settings;

std::atomic<bool> g_hooksInstalled;

// Track weak references to buttons to update them when settings change
std::mutex g_buttonsMutex;
std::vector<winrt::weak_ref<FrameworkElement>> g_buttonRefs;

// Track hovered elements manually since IsPointerOver is not available
std::mutex g_hoverMutex;
std::set<void*> g_hoveredElements;

void LoadSettings() {
    g_settings.iconOpacity = Wh_GetIntSetting(L"IconOpacity");
    if (g_settings.iconOpacity < 0) g_settings.iconOpacity = 0;
    if (g_settings.iconOpacity > 100) g_settings.iconOpacity = 100;

    g_settings.dimSearchBox = Wh_GetIntSetting(L"DimSearchBox");
}

// Helper to check for search-related identifiers in an element
bool IsSearchElement(const FrameworkElement& element) {
    if (!element) return false;
    try {
        // Check Class Name
        auto className = winrt::get_class_name(element);
        std::wstring cname(className.c_str());
        std::transform(cname.begin(), cname.end(), cname.begin(), ::towlower);
        if (cname.find(L"search") != std::wstring::npos) return true;

        // Check Automation ID
        auto automationId = AutomationProperties::GetAutomationId(element);
        if (!automationId.empty()) {
            std::wstring id(automationId.c_str());
            std::transform(id.begin(), id.end(), id.begin(), ::towlower);
            if (id.find(L"search") != std::wstring::npos) return true;
        }
        
        // Check Element Name
        auto elemName = element.Name();
        if (!elemName.empty()) {
            std::wstring ename(elemName.c_str());
            std::transform(ename.begin(), ename.end(), ename.begin(), ::towlower);
            if (ename.find(L"search") != std::wstring::npos) return true;
        }
    } catch (...) {}
    return false;
}

// Recursive check for search elements
bool HasSearchChild(const FrameworkElement& parent, int depth = 0) {
    if (depth > 2) return false; // Don't go too deep
    try {
        int count = VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
            if (child) {
                if (IsSearchElement(child)) return true;
                if (HasSearchChild(child, depth + 1)) return true;
            }
        }
    } catch (...) {}
    return false;
}

bool IsSearchBox(const FrameworkElement& element) {
    if (IsSearchElement(element)) return true;
    if (HasSearchChild(element)) return true;
    return false;
}

void ApplyOpacityToElement(const FrameworkElement& element) {
    if (!element) return;

    if (!g_settings.dimSearchBox && IsSearchBox(element)) return;

    double targetOpacity = (double)g_settings.iconOpacity / 100.0;
    
    // Check manual hover state
    bool isHovered = false;
    {
        std::lock_guard<std::mutex> lock(g_hoverMutex);
        // Use the ABI pointer as the unique key
        isHovered = g_hoveredElements.count(winrt::get_abi(element)) > 0;
    }

    if (isHovered) {
        targetOpacity = 1.0;
    }

    double currentOpacity = element.Opacity();
    if (abs(currentOpacity - targetOpacity) > 0.001) {
        element.Opacity(targetOpacity);
    }
}

// Forward declaration
void ScanSiblings(const FrameworkElement& element);



bool RegisterButton(const FrameworkElement& element) {
    if (!element) return false;
    
    // Debug logging for identification
    try {
        auto className = winrt::get_class_name(element);
        // Wh_Log(L"Checking element: %s", className.c_str());
    } catch(...) {}

    if (!g_settings.dimSearchBox && IsSearchBox(element)) {
        // Wh_Log(L"Excluding Search Box");
        return false;
    }

    std::lock_guard<std::mutex> lock(g_buttonsMutex);
    
    bool exists = false;
    auto it = g_buttonRefs.begin();
    while (it != g_buttonRefs.end()) {
        auto strong = it->get();
        if (!strong) {
            it = g_buttonRefs.erase(it);
            continue;
        }
        
        // Element identity check
        if (strong == element) {
            exists = true;
        }
        ++it;
    }

    if (!exists) {
        g_buttonRefs.push_back(winrt::make_weak(element));

        // Attach hover handlers
        // Note: We use weak ref in lambda to avoid circular dependency
        auto weak = winrt::make_weak(element);

        auto onEnter = [weak](winrt::Windows::Foundation::IInspectable const& sender, PointerRoutedEventArgs const&) {
            if (auto strong = weak.get()) {
                {
                    std::lock_guard<std::mutex> lock(g_hoverMutex);
                    g_hoveredElements.insert(winrt::get_abi(strong));
                }
                ApplyOpacityToElement(strong);
            }
        };

        auto onExit = [weak](winrt::Windows::Foundation::IInspectable const& sender, PointerRoutedEventArgs const&) {
            if (auto strong = weak.get()) {
                {
                    std::lock_guard<std::mutex> lock(g_hoverMutex);
                    g_hoveredElements.erase(winrt::get_abi(strong));
                }
                ApplyOpacityToElement(strong);
            }
        };

        element.PointerEntered(onEnter);
        element.PointerExited(onExit);
        
        return true; // New button registered
    }
    return false;
}

void ScanSiblings(const FrameworkElement& element) {
    try {
        // Find the parent (usually TaskbarFrameRepeater or TaskListButtonPanel)
        auto parent = VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        if (!parent) return;

        int count = VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
            if (child) {
                // If it looks like a button (same type as the one that triggered this), register it
                // Simple heuristic: if it's a FrameworkElement in the same container, treat it as a taskbar item
                if (RegisterButton(child)) {
                    ApplyOpacityToElement(child);
                }
            }
        }
    } catch (...) {
        // Safe catch for visual tree traversal errors
    }
}

void ApplyOpacity(void* pThis) {
    if (!pThis) return;
    
    auto trySetOpacity = [&](void* ptr) -> bool {
        if (!ptr) return false;
        winrt::Windows::UI::Xaml::FrameworkElement element{nullptr};
        
        ::IUnknown* unknown = (::IUnknown*)ptr;
        
        HRESULT hr = unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
        
        if (SUCCEEDED(hr) && element) {
            ApplyOpacityToElement(element);
            
            // If this is a new button, scan its details and siblings
            if (RegisterButton(element)) {
                ScanSiblings(element);
            }
            return true;
        }
        return false;
    };

    try {
        bool found = false;
        if (trySetOpacity((void*)((uintptr_t)pThis + 3 * sizeof(void*)))) found = true;
        if (!found && trySetOpacity((void*)((uintptr_t)pThis + sizeof(void*)))) found = true;
        if (!found && trySetOpacity(pThis)) found = true;
    } catch (...) {
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    ApplyOpacity(pThis);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;

void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    ApplyOpacity(pThis);
}

bool InstallHooks(HMODULE module) {
    if (g_hooksInstalled) return true;
    if (!module) return false;

    Wh_Log(L"Installing hooks for module: %p", module);

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            false
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook symbols");
        return false;
    }

    g_hooksInstalled = true;
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    
    if (hModule && !g_hooksInstalled) {
        if (wcsstr(lpLibFileName, L"Taskbar.View.dll") || 
            wcsstr(lpLibFileName, L"ExplorerExtensions.dll")) {
            InstallHooks(hModule);
        }
    }

    return hModule;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    if (module) {
        InstallHooks(module);
    } 
    
    Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    
    std::vector<FrameworkElement> elements;
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        for (const auto& weakRef : g_buttonRefs) {
            if (auto element = weakRef.get()) {
                elements.push_back(element);
            }
        }
    }

    for (const auto& element : elements) {
        try {
            auto dispatcher = element.Dispatcher();
            if (dispatcher.HasThreadAccess()) {
                ApplyOpacityToElement(element);
            } else {
                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [element]() {
                    ApplyOpacityToElement(element);
                });
            }
        } catch (...) {}
    }
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_buttonsMutex);
    g_buttonRefs.clear();
}

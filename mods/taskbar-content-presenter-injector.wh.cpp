// ==WindhawkMod==
// @id              taskbar-content-presenter-injector
// @name            Taskbar ContentPresenter Injector
// @description     Injects a ContentPresenter into Taskbar.TaskListLabeledButtonPanel and Taskbar.TaskListButtonPanel
// @version         1.1
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar ContentPresenter Injector

This mod acts as an addon to the [Windows 11 Taskbar Styler mod](https://windhawk.net/mods/windows-11-taskbar-styler), enabling deeper customization of the taskbar, such as replacing icons with glyphs, by injecting a `ContentPresenter` named `CustomInjectedPresenter` into every `Taskbar.TaskListLabeledButtonPanel` and `Taskbar.TaskListButtonPanel`.

## Path to the injected element:

```Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel > Windows.UI.Xaml.Controls.ContentPresenter#CustomInjectedPresenter```

```Taskbar.ExperienceToggleButton > Taskbar.TaskListButtonPanel > Windows.UI.Xaml.Controls.ContentPresenter#CustomInjectedPresenter```
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

// Fix for conflict between Windows macro and WinRT method names
#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>

using namespace winrt::Windows::UI::Xaml;

// Global state tracking
std::atomic<bool> g_taskbarViewDllLoaded = false;

// Track Panels specifically for cleanup.
struct TrackedPanelRef {
    void* ptr; // Stored only for identification/deduplication
    winrt::weak_ref<Controls::Panel> ref;
};

std::vector<TrackedPanelRef> g_trackedPanels;
std::unordered_set<void*> g_trackedPanelPtrs;
std::mutex g_panelMutex;

// Cache the TaskbarFrame to allow triggering global scans from local events
winrt::weak_ref<FrameworkElement> g_cachedTaskbarFrame;

const std::wstring c_TargetPanelLabeled = L"Taskbar.TaskListLabeledButtonPanel";
const std::wstring c_TargetPanelButton = L"Taskbar.TaskListButtonPanel";
const std::wstring c_RootFrameName = L"Taskbar.TaskbarFrame";
const std::wstring c_InjectedControlName = L"CustomInjectedPresenter";

// -------------------------------------------------------------------------
// Original Function Pointers
// -------------------------------------------------------------------------
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;

using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateVisualStates_t ExperienceToggleButton_UpdateVisualStates_Original;

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t = void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;

// -------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------

// Helper to get FrameworkElement from native implementation pointer
FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

// Register a panel to be cleaned up later. Safe to call repeatedly.
void RegisterPanelForCleanup(Controls::Panel const& panel) {
    if (!panel) return;
    
    void* pAbi = winrt::get_abi(panel);
    
    std::lock_guard<std::mutex> lock(g_panelMutex);
    if (g_trackedPanelPtrs.find(pAbi) == g_trackedPanelPtrs.end()) {
        g_trackedPanelPtrs.insert(pAbi);
        g_trackedPanels.push_back({ pAbi, winrt::make_weak(panel) });
    }
}

// Helper: Check if the element is already injected
bool IsAlreadyInjected(Controls::Panel panel) {
    for (auto child : panel.Children()) {
        if (auto elem = child.try_as<FrameworkElement>()) {
            if (elem.Name() == c_InjectedControlName) {
                return true;
            }
        }
    }
    return false;
}

// Logic to inject the ContentPresenter
void InjectContentPresenterIntoPanel(FrameworkElement targetPanel) {
    if (!targetPanel) return;

    auto panel = targetPanel.try_as<Controls::Panel>();
    if (!panel) return;

    // Track this panel immediately for future cleanup
    RegisterPanelForCleanup(panel);

    if (IsAlreadyInjected(panel)) return;

    Controls::ContentPresenter presenter;
    presenter.Name(c_InjectedControlName);
    presenter.HorizontalAlignment(HorizontalAlignment::Stretch);
    presenter.VerticalAlignment(VerticalAlignment::Stretch);

    panel.Children().Append(presenter);
}

// Universal scanner: Checks current element and recurses
void ScanAndInjectRecursive(FrameworkElement element) {
    if (!element) return;

    std::wstring className = winrt::get_class_name(element).c_str();

    // Check if THIS element is one of our targets
    if (className == c_TargetPanelLabeled || className == c_TargetPanelButton) {
        InjectContentPresenterIntoPanel(element);
        // We generally don't need to look inside the panel itself for another panel
        return; 
    }

    // Recurse into children
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto childDependencyObject = Media::VisualTreeHelper::GetChild(element, i);
        auto child = childDependencyObject.try_as<FrameworkElement>();
        if (child) {
            ScanAndInjectRecursive(child);
        }
    }
}

// Checks if we have a cached TaskbarFrame. If not, walks up the tree from the 
// provided element to find it, caches it, and triggers a full scan.
void EnsureGlobalScanFromElement(FrameworkElement startNode) {
    // If we already have a valid global frame cached, we assume global scanning 
    // is being handled by the Layout hook or has already run.
    if (g_cachedTaskbarFrame.get()) {
        return;
    }

    try {
        FrameworkElement current = startNode;
        while (current) {
            std::wstring className = winrt::get_class_name(current).c_str();
            if (className == c_RootFrameName) {
                // Found the frame! Cache it.
                g_cachedTaskbarFrame = winrt::make_weak(current);
                
                // Trigger the global scan immediately.
                // This ensures that hitting one button populates the whole bar.
                ScanAndInjectRecursive(current);
                return;
            }
            
            // Walk up
            auto parent = Media::VisualTreeHelper::GetParent(current);
            current = parent.try_as<FrameworkElement>();
        }
    } catch (...) {
        // Safety catch for visual tree traversal issues
    }
}

// -------------------------------------------------------------------------
// Cleanup Helpers
// -------------------------------------------------------------------------

void RemoveInjectedFromPanel(Controls::Panel panel) {
    if (!panel) return;
    try {
        auto children = panel.Children();
        // Iterate backwards to safely remove
        for (int i = children.Size() - 1; i >= 0; i--) {
            auto child = children.GetAt(i);
            if (auto childFe = child.try_as<FrameworkElement>()) {
                if (childFe.Name() == c_InjectedControlName) {
                    children.RemoveAt(i);
                }
            }
        }
    } catch (...) {
        // Handle potential collection change errors or invalid state
    }
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

// Hook for Standard App Buttons
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (auto elem = GetFrameworkElementFromNative(pThis)) {
        ScanAndInjectRecursive(elem); // Local injection
        EnsureGlobalScanFromElement(elem); // Try to upgrade to global injection
    }
}

void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    if (auto elem = GetFrameworkElementFromNative(pThis)) {
        ScanAndInjectRecursive(elem);
        EnsureGlobalScanFromElement(elem);
    }
}

// Hook for System Buttons (Search, Widgets, etc.)
void WINAPI ExperienceToggleButton_UpdateVisualStates_Hook(void* pThis) {
    ExperienceToggleButton_UpdateVisualStates_Original(pThis);
    if (auto elem = GetFrameworkElementFromNative(pThis)) {
        ScanAndInjectRecursive(elem);
        EnsureGlobalScanFromElement(elem);
    }
}

// Global Layout Hook
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);

    auto taskbarFrame = GetFrameworkElementFromNative(pThis);
    if (!taskbarFrame) return;

    // Cache this pointer if we haven't already
    if (!g_cachedTaskbarFrame.get()) {
        g_cachedTaskbarFrame = winrt::make_weak(taskbarFrame);
    }

    ScanAndInjectRecursive(taskbarFrame);
}

// -------------------------------------------------------------------------
// Initialization Logic
// -------------------------------------------------------------------------

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateVisualStates(void))"},
            &ExperienceToggleButton_UpdateVisualStates_Original,
            ExperienceToggleButton_UpdateVisualStates_Hook,
            true // Optional
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
            &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
        }
    };

    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
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
        
        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        
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

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Injector Mod v1.3");

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Injector Mod - Cleaning up %d tracked panels", g_trackedPanels.size());

    std::lock_guard<std::mutex> lock(g_panelMutex);
    
    for (auto& tracked : g_trackedPanels) {
        if (auto panel = tracked.ref.get()) {
            // We must run cleanup on the UI thread.
            auto dispatcher = panel.Dispatcher();
            
            if (dispatcher.HasThreadAccess()) {
                RemoveInjectedFromPanel(panel);
            } else {
                try {
                    // Block until cleanup is done on the UI thread to ensure code is still valid
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [panel]() {
                        RemoveInjectedFromPanel(panel);
                    }).get();
                } catch (...) {
                    Wh_Log(L"Failed to run cleanup on dispatcher for a panel");
                }
            }
        }
    }
    
    g_trackedPanels.clear();
    g_trackedPanelPtrs.clear();
    g_cachedTaskbarFrame = nullptr;
}

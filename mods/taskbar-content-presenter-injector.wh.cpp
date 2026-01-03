// ==WindhawkMod==
// @id              taskbar-content-presenter-injector
// @name            Taskbar ContentPresenter Injector
// @description     Injects a ContentPresenter into Taskbar.TaskListLabeledButtonPanel and Taskbar.TaskListButtonPanel
// @version         1.2
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

using namespace winrt::Windows::UI::Xaml;

// Global state tracking
std::atomic<bool> g_taskbarViewDllLoaded = false;

struct TrackedPanelRef {
    winrt::weak_ref<Controls::Panel> ref;
};

std::vector<TrackedPanelRef> g_trackedPanels;
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

// -------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------

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

void RegisterPanelForCleanup(Controls::Panel const& panel) {
    if (!panel) return;
    
    void* pAbi = winrt::get_abi(panel);
    
    std::lock_guard<std::mutex> lock(g_panelMutex);

    // OPTIMIZATION: Prune dead references while scanning for duplicates.
    auto it = g_trackedPanels.begin();
    while (it != g_trackedPanels.end()) {
        auto existing = it->ref.get();
        if (!existing) {
            it = g_trackedPanels.erase(it);
        } else {
            if (winrt::get_abi(existing) == pAbi) {
                return; // Already tracked
            }
            ++it;
        }
    }
    
    g_trackedPanels.push_back({ winrt::make_weak(panel) });
}

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

void InjectContentPresenterIntoPanel(FrameworkElement targetPanel) {
    if (!targetPanel) return;

    auto panel = targetPanel.try_as<Controls::Panel>();
    if (!panel) return;

    RegisterPanelForCleanup(panel);

    if (IsAlreadyInjected(panel)) return;

    Controls::ContentPresenter presenter;
    presenter.Name(c_InjectedControlName);
    presenter.HorizontalAlignment(HorizontalAlignment::Stretch);
    presenter.VerticalAlignment(VerticalAlignment::Stretch);

    panel.Children().Append(presenter);
}

void ScanAndInjectRecursive(FrameworkElement element) {
    if (!element) return;

    auto className = winrt::get_class_name(element);

    if (className == c_TargetPanelLabeled || className == c_TargetPanelButton) {
        InjectContentPresenterIntoPanel(element);
        return; 
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto childDependencyObject = Media::VisualTreeHelper::GetChild(element, i);
        auto child = childDependencyObject.try_as<FrameworkElement>();
        if (child) {
            ScanAndInjectRecursive(child);
        }
    }
}

void EnsureGlobalScanFromElement(FrameworkElement startNode) {
    if (g_cachedTaskbarFrame.get()) return;

    try {
        FrameworkElement current = startNode;
        while (current) {
            auto className = winrt::get_class_name(current);
            if (className == c_RootFrameName) {
                g_cachedTaskbarFrame = winrt::make_weak(current);
                ScanAndInjectRecursive(current);
                return;
            }
            auto parent = Media::VisualTreeHelper::GetParent(current);
            current = parent.try_as<FrameworkElement>();
        }
    } catch (...) {}
}

// -------------------------------------------------------------------------
// Cleanup Helpers
// -------------------------------------------------------------------------

void RemoveInjectedFromPanel(Controls::Panel panel) {
    if (!panel) return;
    try {
        auto children = panel.Children();
        for (int i = children.Size() - 1; i >= 0; i--) {
            auto child = children.GetAt(i);
            if (auto childFe = child.try_as<FrameworkElement>()) {
                if (childFe.Name() == c_InjectedControlName) {
                    children.RemoveAt(i);
                }
            }
        }
    } catch (...) {}
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

// Helper to reduce redundancy in hooks
void InjectForElement(void* pThis) {
    try {
        if (auto elem = GetFrameworkElementFromNative(pThis)) {
            ScanAndInjectRecursive(elem);
            if (!g_cachedTaskbarFrame.get()) {
                EnsureGlobalScanFromElement(elem);
            }
        }
    } catch (...) {}
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    InjectForElement(pThis);
}

void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    InjectForElement(pThis);
}

void WINAPI ExperienceToggleButton_UpdateVisualStates_Hook(void* pThis) {
    ExperienceToggleButton_UpdateVisualStates_Original(pThis);
    InjectForElement(pThis);
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
    Wh_Log(L"Initializing Taskbar Injector Mod v1.6");

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
    Wh_Log(L"Uninitializing Taskbar Injector Mod");

    std::vector<TrackedPanelRef> localPanels;
    {
        std::lock_guard<std::mutex> lock(g_panelMutex);
        localPanels = std::move(g_trackedPanels);
    }
    
    for (auto& tracked : localPanels) {
        if (auto panel = tracked.ref.get()) {
            auto dispatcher = panel.Dispatcher();
            if (dispatcher.HasThreadAccess()) {
                RemoveInjectedFromPanel(panel);
            } else {
                try {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [panel]() {
                        RemoveInjectedFromPanel(panel);
                    }).get();
                } catch (...) {}
            }
        }
    }
    
    g_cachedTaskbarFrame = nullptr;
}

// ==WindhawkMod==
// @id              taskbar-content-presenter-injector
// @name            Taskbar ContentPresenter Injector
// @description     Injects a ContentPresenter into Taskbar.TaskListLabeledButtonPanel and Taskbar.TaskListButtonPanel
// @version         1.3
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

## Limitation
Due to how Windows 11 calculates taskbar dimensions, if you want to change the Width of the Taskbar panels in Taskbar Styler, you must also define a MinWidth to apply the change.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <atomic>
#include <mutex>
#include <map>
#include <set>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_taskbarViewDllLoaded = false;

// --- Global Trackers ---
struct PendingHook {
    winrt::weak_ref<Controls::Panel> panelRef;
    winrt::event_token token;
};

std::map<void*, winrt::weak_ref<Controls::Panel>> g_trackedPanels;
std::mutex g_panelMutex;

std::map<void*, PendingHook> g_pendingHooks;
std::set<void*> g_scannedFrames; // Replaces g_cachedTaskbarFrame
std::mutex g_pendingMutex;

std::atomic<bool> g_scanPending = false;

constexpr std::wstring_view c_TargetPanelLabeled  = L"Taskbar.TaskListLabeledButtonPanel";
constexpr std::wstring_view c_TargetPanelButton   = L"Taskbar.TaskListButtonPanel";
constexpr std::wstring_view c_RootFrameName       = L"Taskbar.TaskbarFrame";
constexpr std::wstring_view c_InjectedControlName = L"CustomInjectedPresenter";

// -------------------------------------------------------------------------
// Original Function Pointers
// -------------------------------------------------------------------------
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void*);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;

using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void*);
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
    
    // O(log N) insertion/update
    g_trackedPanels[pAbi] = winrt::make_weak(panel);
    
    // Throttle pruning to prevent O(N^2) bottleneck during recursive scans
    static int insertCount = 0;
    if (++insertCount % 10 == 0) {
        for (auto it = g_trackedPanels.begin(); it != g_trackedPanels.end(); ) {
            if (!it->second.get()) {
                it = g_trackedPanels.erase(it);
            } else {
                ++it;
            }
        }
    }
}

bool IsAlreadyInjected(Controls::Panel panel) {
    for (auto child : panel.Children()) {
        if (auto elem = child.try_as<FrameworkElement>()) {
            if (elem.Name() == c_InjectedControlName) return true;
        }
    }
    return false;
}

// -------------------------------------------------------------------------
// Deferred Injection: Waits for LayoutUpdated to guarantee the
// panel is fully built with physical dimensions before touching it.
// -------------------------------------------------------------------------
void InjectContentPresenterIntoPanel(FrameworkElement targetPanel) {
    if (!targetPanel) return;

    auto panel = targetPanel.try_as<Controls::Panel>();
    if (!panel) return;

    RegisterPanelForCleanup(panel);

    if (IsAlreadyInjected(panel)) return;

    // If the panel is already physically rendered, inject immediately and skip the event overhead
    if (targetPanel.ActualWidth() > 0 && targetPanel.ActualHeight() > 0) {
        Controls::ContentPresenter presenter;
        presenter.Name(c_InjectedControlName);
        presenter.HorizontalAlignment(HorizontalAlignment::Stretch);
        presenter.VerticalAlignment(VerticalAlignment::Stretch);
        panel.Children().Append(presenter);
        return;
    }

    // Deferred event path
    void* pAbi = winrt::get_abi(panel);
    
    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        if (auto it = g_pendingHooks.find(pAbi); it != g_pendingHooks.end()) {
            if (it->second.panelRef.get()) return; 
        }
        g_pendingHooks[pAbi] = { winrt::weak_ref<Controls::Panel>(), winrt::event_token{} };
    }

    auto weakPanel = winrt::make_weak(panel);
    auto tokenHolder = std::make_shared<winrt::event_token>();

    try {
        // Swap LayoutUpdated for SizeChanged to eliminate global event spam
        *tokenHolder = targetPanel.SizeChanged(
            [weakPanel, tokenHolder, pAbi](winrt::Windows::Foundation::IInspectable const&,
                                           winrt::Windows::UI::Xaml::SizeChangedEventArgs const&) {
                
                auto p = weakPanel.get();
                if (!p) {
                    std::lock_guard<std::mutex> lock(g_pendingMutex);
                    g_pendingHooks.erase(pAbi);
                    return;
                }

                if (p.ActualWidth() > 0 && p.ActualHeight() > 0) {
                    p.SizeChanged(*tokenHolder); // Safely Unsubscribe
                    
                    {
                        std::lock_guard<std::mutex> lock(g_pendingMutex);
                        g_pendingHooks.erase(pAbi);
                    }

                    if (!IsAlreadyInjected(p)) {
                        Controls::ContentPresenter presenter;
                        presenter.Name(c_InjectedControlName);
                        presenter.HorizontalAlignment(HorizontalAlignment::Stretch);
                        presenter.VerticalAlignment(VerticalAlignment::Stretch);
                        p.Children().Append(presenter);
                    }
                }
            });
    } catch (...) {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        g_pendingHooks.erase(pAbi);
        return; 
    }

    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        if (g_pendingHooks.count(pAbi)) {
            g_pendingHooks[pAbi] = { weakPanel, *tokenHolder };
        }
    }
}

void ScanAndInjectRecursive(FrameworkElement element) {
    if (!element) return;

    auto className = winrt::get_class_name(element);
    if (className == c_TargetPanelLabeled || className == c_TargetPanelButton) {
        InjectContentPresenterIntoPanel(element);
        return;
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child) ScanAndInjectRecursive(child);
    }
}

void ScheduleScanAsync(FrameworkElement startNode) {
    if (!startNode) return;
    
    if (g_scanPending.exchange(true)) return;

    auto weak = winrt::make_weak(startNode);

    try {
        startNode.Dispatcher().RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [weak]() {
                g_scanPending = false; 

                if (auto node = weak.get()) {
                    FrameworkElement current = node;
                    while (current) {
                        if (winrt::get_class_name(current) == c_RootFrameName) {
                            void* frameAbi = winrt::get_abi(current);
                            {
                                std::lock_guard<std::mutex> lock(g_pendingMutex);
                                // If we already scanned this monitor's taskbar, we're done!
                                if (g_scannedFrames.count(frameAbi)) return;
                                g_scannedFrames.insert(frameAbi);
                            }
                            ScanAndInjectRecursive(current);
                            return;
                        }
                        auto parent = Media::VisualTreeHelper::GetParent(current);
                        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
                    }
                    ScanAndInjectRecursive(node);
                }
            });
    } catch (...) {
        g_scanPending = false; 
    }
}

// -------------------------------------------------------------------------
// Cleanup
// -------------------------------------------------------------------------
void RemoveInjectedFromPanel(Controls::Panel panel) {
    if (!panel) return;
    try {
        auto children = panel.Children();
        for (int i = (int)children.Size() - 1; i >= 0; i--) {
            if (auto fe = children.GetAt(i).try_as<FrameworkElement>()) {
                if (fe.Name() == c_InjectedControlName) children.RemoveAt(i);
            }
        }
    } catch (...) {}
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
void InjectForElement(void* pThis) {
    try {
        if (auto elem = GetFrameworkElementFromNative(pThis)) {
            ScanAndInjectRecursive(elem); // Fast local injection
            ScheduleScanAsync(elem);      // Will silently return if monitor is already scanned
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
// Init / Uninit
// -------------------------------------------------------------------------
bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
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
            true
        }
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {

        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Injector Mod");

    if (HMODULE m = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    } else {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Injector Mod");

    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        g_scannedFrames.clear(); // Clear cached monitors
    }

    // 2. Remove injected ContentPresenters
    std::map<void*, winrt::weak_ref<Controls::Panel>> localTracked;
    {
        std::lock_guard<std::mutex> lock(g_panelMutex);
        localTracked = std::move(g_trackedPanels);
    }

    for (auto& [pAbi, weakRef] : localTracked) {
        if (auto panel = weakRef.get()) {
            auto dispatcher = panel.Dispatcher();
            auto cleanupFn = [panel]() { RemoveInjectedFromPanel(panel); };

            if (dispatcher.HasThreadAccess()) {
                cleanupFn();
            } else {
                try {
                    dispatcher.RunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                        cleanupFn).get();
                } catch (...) {}
            }
        }
    }
}

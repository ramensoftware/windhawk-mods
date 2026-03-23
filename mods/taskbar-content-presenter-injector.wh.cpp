// ==WindhawkMod==
// @id              taskbar-content-presenter-injector
// @name            Taskbar ContentPresenter Injector
// @description     Injects a ContentPresenter into Taskbar.TaskListLabeledButtonPanel and Taskbar.TaskListButtonPanel
// @version         1.4
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar ContentPresenter Injector

This mod acts as an addon to the Windows 11 Taskbar Styler mod, enabling deeper
customization of the taskbar, such as replacing icons with glyphs, by injecting
a `ContentPresenter` named `CustomInjectedPresenter` into every
`Taskbar.TaskListLabeledButtonPanel` and `Taskbar.TaskListButtonPanel`.

## Injected element paths

```
Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel > Windows.UI.Xaml.Controls.ContentPresenter#CustomInjectedPresenter
Taskbar.ExperienceToggleButton > Taskbar.TaskListButtonPanel > Windows.UI.Xaml.Controls.ContentPresenter#CustomInjectedPresenter
```

## Performance

- **Scan throttle** — hooks fire dozens of times per second during taskbar
  activity. A 250 ms cooldown with a lock-free CAS ensures only one scan runs
  per window, dropping redundant scans from ~40–60/s to at most 4/s.
- **Class name cache** — `winrt::get_class_name` is a COM call issued once per
  visual-tree node per scan. Results are stored in an `unordered_map` keyed by
  ABI pointer and protected by a `shared_mutex`, so concurrent readers never
  block each other. The cache is invalidated automatically when the taskbar tree
  is recreated.
- **Zero-allocation comparisons** — target class names and the injected
  element name are `constexpr std::wstring_view` constants. No heap allocation
  occurs during string comparisons.
- **Loaded event token tracking** — the `winrt::event_token` returned by each
  `Loaded` subscription is stored alongside the panel reference. On unload,
  every subscription is revoked before cleanup, preventing orphaned event
  handlers if Windows recycles a panel multiple times.
- **Zombie frame detection** — the cached `TaskbarFrame` is validated with
  `IsLoaded()` before each scan. A stale pointer is discarded and a fresh
  frame is located by walking the visual tree upward, fixing the disappearing
  button regression introduced in Windows 11 Insider Preview 10.0.26120.4250.
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
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

constexpr std::wstring_view c_TargetPanelLabeled = L"Taskbar.TaskListLabeledButtonPanel";
constexpr std::wstring_view c_TargetPanelButton  = L"Taskbar.TaskListButtonPanel";
constexpr std::wstring_view c_RootFrameName       = L"Taskbar.TaskbarFrame";
constexpr std::wstring_view c_InjectedControlName = L"CustomInjectedPresenter";

std::unordered_map<void*, std::wstring> g_classNameCache;
std::shared_mutex                        g_classNameCacheMutex;

std::wstring_view GetCachedClassName(FrameworkElement const& elem) {
    void* key = winrt::get_abi(elem);
    {
        std::shared_lock lock(g_classNameCacheMutex);
        auto it = g_classNameCache.find(key);
        if (it != g_classNameCache.end()) return it->second;
    }
    // FIX #1: winrt::get_class_name returns winrt::hstring, not std::wstring.
    // Convert explicitly via std::wstring_view, which hstring implicitly provides.
    winrt::hstring hname = winrt::get_class_name(elem);
    std::wstring name{ static_cast<std::wstring_view>(hname) };
    {
        std::unique_lock lock(g_classNameCacheMutex);
        auto [it, inserted] = g_classNameCache.emplace(key, std::move(name));
        return it->second;
    }
}

void InvalidateClassNameCache() {
    std::unique_lock lock(g_classNameCacheMutex);
    g_classNameCache.clear();
}

constexpr int64_t c_ScanCooldownMs = 250;
std::atomic<int64_t> g_lastScanTime{ 0 };

bool ShouldScan() {
    using namespace std::chrono;
    int64_t now  = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    int64_t last = g_lastScanTime.load(std::memory_order_relaxed);
    if (now - last < c_ScanCooldownMs) return false;
    return g_lastScanTime.compare_exchange_strong(last, now, std::memory_order_relaxed);
}

std::atomic<bool> g_taskbarViewDllLoaded = false;

struct TrackedPanelRef {
    winrt::weak_ref<Controls::Panel> weakPanel;
    winrt::event_token               loadedToken{};
};

std::vector<TrackedPanelRef> g_trackedPanels;
std::mutex                    g_panelMutex;

winrt::weak_ref<FrameworkElement> g_cachedTaskbarFrame;

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void*);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;

using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void*);
ExperienceToggleButton_UpdateVisualStates_t ExperienceToggleButton_UpdateVisualStates_Original;

void InjectContentPresenterIntoPanel(FrameworkElement targetPanel);
void ScanAndInjectRecursive(FrameworkElement element);

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

bool RegisterPanelForCleanup(Controls::Panel const& panel, winrt::event_token loadedToken) {
    if (!panel) return false;
    void* pAbi = winrt::get_abi(panel);
    std::lock_guard lock(g_panelMutex);
    auto it = g_trackedPanels.begin();
    while (it != g_trackedPanels.end()) {
        auto existing = it->weakPanel.get();
        if (!existing) { it = g_trackedPanels.erase(it); continue; }
        if (winrt::get_abi(existing) == pAbi) return false;
        ++it;
    }
    g_trackedPanels.push_back({ winrt::make_weak(panel), loadedToken });
    return true;
}

bool IsAlreadyInjected(Controls::Panel const& panel) {
    for (auto const& child : panel.Children()) {
        if (auto elem = child.try_as<FrameworkElement>()) {
            if (elem.Name() == c_InjectedControlName) return true;
        }
    }
    return false;
}

void InjectContentPresenterIntoPanel(FrameworkElement targetPanel) {
    if (!targetPanel) return;
    auto panel = targetPanel.try_as<Controls::Panel>();
    if (!panel) return;
    if (IsAlreadyInjected(panel)) return;

    winrt::event_token token = targetPanel.Loaded(
        [](winrt::Windows::Foundation::IInspectable const& sender, RoutedEventArgs const&) {
            try {
                if (auto fe = sender.try_as<FrameworkElement>()) {
                    InvalidateClassNameCache();
                    g_cachedTaskbarFrame = nullptr;
                    InjectContentPresenterIntoPanel(fe);
                }
            } catch (...) {}
        });

    RegisterPanelForCleanup(panel, token);

    Controls::ContentPresenter presenter;
    presenter.Name(winrt::hstring{ c_InjectedControlName });
    presenter.HorizontalAlignment(HorizontalAlignment::Stretch);
    presenter.VerticalAlignment(VerticalAlignment::Stretch);
    panel.Children().Append(presenter);
    Wh_Log(L"Injected into: %s", std::wstring{ GetCachedClassName(targetPanel) }.c_str());
}

void ScanAndInjectRecursive(FrameworkElement element) {
    if (!element) return;
    auto const& className = GetCachedClassName(element);
    if (className == c_TargetPanelLabeled || className == c_TargetPanelButton) {
        InjectContentPresenterIntoPanel(element);
        return;
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto dep = Media::VisualTreeHelper::GetChild(element, i);
        if (auto child = dep.try_as<FrameworkElement>()) {
            ScanAndInjectRecursive(child);
        }
    }
}

void EnsureGlobalScanFromElement(FrameworkElement startNode) {
    if (auto cached = g_cachedTaskbarFrame.get()) {
        try {
            if (cached.IsLoaded()) {
                ScanAndInjectRecursive(cached);
                return;
            }
        } catch (...) {}
        Wh_Log(L"TaskbarFrame zombie — invalidating cache");
        g_cachedTaskbarFrame = nullptr;
        InvalidateClassNameCache();
    }
    try {
        FrameworkElement current = startNode;
        while (current) {
            if (GetCachedClassName(current) == c_RootFrameName) {
                g_cachedTaskbarFrame = winrt::make_weak(current);
                Wh_Log(L"TaskbarFrame cached");
                ScanAndInjectRecursive(current);
                return;
            }
            auto parent = Media::VisualTreeHelper::GetParent(current);
            current = parent.try_as<FrameworkElement>();
        }
    } catch (...) {}
}

void RemoveInjectedFromPanel(Controls::Panel const& panel) {
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

void InjectForElement(void* pThis) {
    if (!ShouldScan()) return;
    try {
        if (auto elem = GetFrameworkElementFromNative(pThis)) {
            ScanAndInjectRecursive(elem);
            EnsureGlobalScanFromElement(elem);
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

bool HookTaskbarViewDllSymbols(HMODULE module) {
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

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Injector Mod v1.4");
    if (HMODULE m = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    } else {
        HMODULE kb    = GetModuleHandle(L"kernelbase.dll");
        auto    pLoad = (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        // FIX #2: Wh_SetFunctionHookT is deprecated — use WindhawkUtils::SetFunctionHook instead.
        WindhawkUtils::SetFunctionHook(pLoad, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Injector Mod");
    std::vector<TrackedPanelRef> localPanels;
    {
        std::lock_guard lock(g_panelMutex);
        localPanels = std::move(g_trackedPanels);
    }
    for (auto& tracked : localPanels) {
        auto panel = tracked.weakPanel.get();
        if (!panel) continue;
        auto cleanup = [panel, token = tracked.loadedToken]() {
            try {
                if (auto fe = panel.try_as<FrameworkElement>()) {
                    if (token.value != 0) fe.Loaded(token);
                }
            } catch (...) {}
            RemoveInjectedFromPanel(panel);
        };
        auto dispatcher = panel.Dispatcher();
        if (dispatcher.HasThreadAccess()) {
            cleanup();
        } else {
            try {
                dispatcher.RunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                    cleanup).get();
            } catch (...) {}
        }
    }
    g_cachedTaskbarFrame = nullptr;
    InvalidateClassNameCache();
}

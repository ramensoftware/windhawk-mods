// ==WindhawkMod==
// @id              taskbar-dock-animation
// @name            Taskbar Dock Animation (Ultimate)
// @description     Animates taskbar icons on mouse hover like in macOS
// @description:uk-UA Анімація іконок панелі завдань, як в macOS, при наведенні
// @version         0.7
// @author          Ph0en1x-dev
// @github          https://github.com/Ph0en1x-dev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Dock Animation

This mod adds a macOS-like taskbar animation.  
The current version is **experimental** and not yet fully stable.  
A few issues are still being worked on:
- Icons are sometimes clipped by the taskbar.
- Upscaled icons may appear slightly blurry.
- You may need to restart Explorer and wait about 30 seconds for the effects to apply.

At this stage, since the mod is still in active testing, it’s recommended to **keep the maximum scale around 130%** to avoid visual glitches.  
You can experiment with the **radius** value to achieve the best result for your setup until proper fixes for the issues above are found.

---

## 🖼️ Preview
![Taskbar Dock Animation Preview](https://raw.githubusercontent.com/Ph0en1x-dev/edu_db_labs/master/Записування%20з%20екрана%202025-11-02%20235836.gif)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- MaxScale: 160
  $name: Maximum scale (%)
  $description: How large an icon can grow (e.g., 160 = +60%)
- EffectRadius: 100
  $name: Effect radius (px)
  $description: Distance from the cursor where the animation is applied
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <UIAutomation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <string_view>
#include <map>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;

// Settings container
struct {
    double maxScale;
    int effectRadius;
} g_settings;

// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_fastPathHooksApplied = false;

// Per-monitor context
struct TaskbarIconInfo {
    FrameworkElement element{ nullptr };
    double originalCenterX = 0.0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    FrameworkElement taskbarFrame{ nullptr };
    std::vector<TaskbarIconInfo> icons;
    winrt::event_token pointerMovedToken;
    winrt::event_token pointerExitedToken;
};

// One context per monitor (keyed by XAML instance pointer)
std::map<void*, DockAnimationContext> g_contexts;

// Unclip only the taskbar icon container (TaskListButtonPanel) without changing taskbar size or window behavior.
void DisableTaskbarIconClipping() {
    try {
        HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
        if (!hTaskbar) return;

        FrameworkElement root{ nullptr };
        winrt::copy_from_abi(root, hTaskbar);
        if (!root) return;

        std::function<void(FrameworkElement)> disableClipRecursively =
            [&](FrameworkElement element) {
                if (!element) return;

                auto className = winrt::get_class_name(element);
                if (std::wstring_view(className).find(L"TaskListButtonPanel") != std::wstring_view::npos) {
                    element.Clip(nullptr);
                    Wh_Log(L"DockAnimation: Clip cleared for TaskListButtonPanel");
                    return;
                }

                int count = VisualTreeHelper::GetChildrenCount(element);
                for (int i = 0; i < count; i++) {
                    auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                    disableClipRecursively(child);
                }
            };

        disableClipRecursively(root);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: DisableTaskbarIconClipping failed: %s", e.message().c_str());
    }
}

// Prototypes
void LoadSettings();
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void FindAllTaskbarButtonsRecursive(FrameworkElement element, DockAnimationContext& ctx);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);

// Simple visual tree helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    double t = distance / radius;
    return (maxScale - 1.0) * (cos(t * 3.14159) + 1.0) / 2.0 + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx) {
    for (auto& iconInfo : ctx.icons) {
        double distance = std::abs(mouseX - iconInfo.originalCenterX);
        double newScale = CalculateScale(distance, g_settings.effectRadius, g_settings.maxScale);
        auto transform = iconInfo.element.RenderTransform().try_as<ScaleTransform>();
        if (transform) {
            transform.ScaleX(newScale);
            transform.ScaleY(newScale);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    try {
        for (auto& iconInfo : icons) {
            auto transform = iconInfo.element.RenderTransform().try_as<ScaleTransform>();
            if (transform) {
                transform.ScaleX(1.0);
                transform.ScaleY(1.0);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error in ResetAllIconScales: %s", e.message().c_str());
    }
}

// Event handlers
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }
        if (ctx.icons.empty()) return;

        double mouseX = args.GetCurrentPoint(ctx.taskbarFrame).Position().X;
        ApplyAnimation(mouseX, ctx);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        ResetAllIconScales(it->second.icons);
        it->second.isInitialized = false;
        it->second.icons.clear();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

// Discover taskbar buttons and cache their centers
void FindAllTaskbarButtonsRecursive(FrameworkElement element, DockAnimationContext& ctx) {
    auto className = winrt::get_class_name(element);

    if (std::wstring_view(className).find(L"TaskListButton") != std::wstring_view::npos) {
        try {
            if (!element.RenderTransform() || !element.RenderTransform().try_as<ScaleTransform>()) {
                auto scaleTransform = ScaleTransform();
                scaleTransform.CenterX(0.5);
                scaleTransform.CenterY(0.5);
                element.RenderTransform(scaleTransform);
            }
            element.RenderTransformOrigin({ 0.5, 1.0 });

            auto transform = element.TransformToVisual(ctx.taskbarFrame);
            auto point = transform.TransformPoint({ 0, 0 });

            TaskbarIconInfo info;
            info.element = element;
            info.originalCenterX = point.X + (element.ActualWidth() / 2.0);
            ctx.icons.push_back(info);
        } catch (...) {}
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child) FindAllTaskbarButtonsRecursive(child, ctx);
    }
}

void RefreshIconPositions(DockAnimationContext& ctx) {
    ctx.icons.clear();
    try {
        FindAllTaskbarButtonsRecursive(ctx.taskbarFrame, ctx);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during recursive search: %s", e.message().c_str());
    }
    Wh_Log(L"DockAnimation: Found %d icons.", (int)ctx.icons.size());
}

// Attach pointer hooks for a given XAML taskbar frame (per monitor)
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;

        ctx.pointerMovedToken = taskbarFrame.PointerMoved(
            [pThis](auto const&, Input::PointerRoutedEventArgs const& args) {
                OnTaskbarPointerMoved(pThis, args);
            }
        );
        ctx.pointerExitedToken = taskbarFrame.PointerExited(
            [pThis](auto const&, Input::PointerRoutedEventArgs const&) {
                OnTaskbarPointerExited(pThis);
            }
        );

        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered.", pThis);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to attach mouse events for %p: %s", pThis, e.message().c_str());
    }
}

// Hooking
using TaskbarFrame_MeasureOverride_t = int(WINAPI*)(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize
);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;

int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    bool shouldInitialize = (g_contexts.find(pThis) == g_contexts.end());

    if (shouldInitialize && pThis) {
        try {
            FrameworkElement taskbarFrame{ nullptr };
            winrt::copy_from_abi(taskbarFrame, pThis);
            if (taskbarFrame) InitializeAnimationHooks(pThis, taskbarFrame);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in MeasureOverride: %s", e.message().c_str());
        }
    }
    return TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
}

// Settings and module loading
void LoadSettings() {
    g_settings.maxScale = (double)Wh_GetIntSetting(L"MaxScale") / 100.0;
    if (g_settings.maxScale < 1.0) g_settings.maxScale = 1.6;
    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
     // Taskbar.View.dll
     WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"
            },
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewDllHooks, ARRAYSIZE(taskbarViewDllHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded for TaskbarFrame_MeasureOverride.");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Lazy hook path (if Taskbar.View.dll loads later)
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"DockAnimation: Taskbar.View.dll loaded, hooking symbols...");
            if (HookTaskbarViewDllSymbols(module)) {
                Wh_ApplyHookOperations();
                Wh_Log(L"DockAnimation: Hooks applied.");
            }
        }
    }
    return module;
}

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"DockAnimation: Wh_ModInit");
    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        // Fast path (module already loaded)
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
        g_fastPathHooksApplied = false;
    } else {
        // Lazy path (wait for LoadLibrary of Taskbar.View.dll)
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original
        );
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");
    if (g_taskbarViewDllLoaded && !g_fastPathHooksApplied) {
        Wh_ApplyHookOperations();
        g_fastPathHooksApplied = true;
    }
    DisableTaskbarIconClipping();
}

// Cleanup
void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit");
    try {
        for (auto& pair : g_contexts) {
            auto& ctx = pair.second;
            if (ctx.taskbarFrame) {
                ctx.taskbarFrame.PointerMoved(ctx.pointerMovedToken);
                ctx.taskbarFrame.PointerExited(ctx.pointerExitedToken);
            }
        }
        g_contexts.clear();
        g_taskbarViewDllLoaded = false;
        g_fastPathHooksApplied = false;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during cleanup: %s", e.message().c_str());
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();
    try {
        for (auto& pair : g_contexts) {
            auto& ctx = pair.second;
            ctx.isInitialized = false;
            ResetAllIconScales(ctx.icons);
            ctx.icons.clear();
        }
    } catch (...) {}
}


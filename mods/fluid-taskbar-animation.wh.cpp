// ==WindhawkMod==
// @id              fluid-taskbar-animation
// @name            Fluid Taskbar Animation
// @description     Brings smooth, macOS-inspired fluid animations to your taskbar icons. High performance with zero stuttering.
// @description:zh-TW 為工作列圖示帶來流暢的 macOS 風格縮放動畫，高效能且無閃爍。
// @version         1.4.1
// @author          tky-kevin (based on work by Ph0en1x-dev)
// @github          https://github.com/tky-kevin
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluid Taskbar Animation

This mod adds a macOS-like fluid animation to the Windows taskbar. Icons scale smoothly as you hover over them, creating a more responsive and premium desktop experience.

### Key Features:
- **Requirement:** Windows 11 or newer.
- **Fluid Scaling:** Smooth interpolation (Lerp) for icon transitions.
- **High-Performance:** Efficient "Dirty Check" system ensures minimal CPU usage.
- **Bounce Effect:** Optional bounce animation when the mouse lingers on an icon.
- **Live Refresh:** Instantly adapts to opening, closing, or reordering apps.
- **Customizable:** Adjust speed, radius, scale, and more in settings.

---

## Settings Advice
- **Animation Speed:** Controls the responsiveness.
  - Recommended: **20** (Default)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- MaxScale: 160
  $name: Maximum scale (%)
  $description: The maximum size of the icon on hover (e.g., 160 = 1.6x size).
- EffectRadius: 100
  $name: Effect radius (px)
  $description: Controls the distance of the cursor's influence on surrounding icons.
- SpacingFactor: 50
  $name: Spacing intensity (%)
  $description: How much to push neighboring icons apart during expansion.
- AnimationSpeed: 20
  $name: Animation Fluidity (%)
  $description: "Lower is smoother/slower, higher is snappier (Recommended: 20)."
- BounceEnabled: true
  $name: Enable Bounce Effect
  $description: Add a playful subtle bounce when your mouse stays on an icon.
- BounceDelay: 100
  $name: Bounce delay (ms)
  $description: Wait time after centering an icon before starting the bounce.
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
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Input.h>
#include <vector>
#include <atomic>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>
#include <chrono>
#define M_PI 3.14159265358979323846

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Automation;

// Settings container
struct {
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double lerpFactor;
    bool bounceEnabled;
} g_settings;

// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime = std::chrono::steady_clock::now();
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -9999.0;
std::atomic<bool> g_isBouncing = false;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Mouse state tracking
std::atomic<bool> g_isMouseInside = false;

// Helper: Linear Interpolation
double Lerp(double start, double end, double factor) {
    return start + (end - start) * factor;
}

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    
    // Current animation state (for smoothing)
    double currentScaleX = 1.0;
    double currentScaleY = 1.0;
    double currentTranslateX = 0.0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    
    // Performance Optimization: Cache container for dirty checks
    winrt::weak_ref<FrameworkElement> iconContainer; 
    double lastTotalWidth = 0.0;
    int lastChildCount = 0;
    std::chrono::steady_clock::time_point lastLayoutRefreshTime = std::chrono::steady_clock::now();

    std::vector<TaskbarIconInfo> icons;
};

// One context per monitor
std::map<void*, DockAnimationContext> g_contexts;

// Prototypes
void LoadSettings();
void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame);
void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args);
void OnTaskbarPointerExited(void* pThis_key);
void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons);
void RefreshIconPositions(DockAnimationContext& ctx);
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                    winrt::Windows::Foundation::IInspectable const&);

HMODULE GetTaskbarViewModuleHandle();
bool HookTaskbarViewDllSymbols(HMODULE module);

// Helpers
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element, PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

// Animation math
double CalculateTargetScale(double distance, double radius, double maxScale) {
    if (radius <= 0 || distance > radius) return 1.0;
    double t = distance / radius;
    return (maxScale - 1.0) * (cos(t * M_PI) + 1.0) / 2.0 + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx) {
    const double spacingFactor = g_settings.spacingFactor;
    const double lerpFactor = g_settings.lerpFactor;
    
    if (!g_isMouseInside.load()) {
        mouseX = -99999.0;
    }

    std::vector<double> targetScales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;

    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    // 1. Calculate TARGET values
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;

        double distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        targetScales[i] = CalculateTargetScale(distance,
                                                g_settings.effectRadius,
                                                g_settings.maxScale);

        extraWidths[i] = (targetScales[i] - 1.0) * element.ActualWidth() * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateY = 0.0;

    if (g_settings.bounceEnabled && g_isMouseInside.load() && g_isBouncing.load() && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * M_PI);

        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT);
        bounceTranslateY = normalizedWave * BOUNCE_TRANSLATE_Y;
    }

    // 2. Apply Smoothing
    double cumulativeShift = 0.0;
    bool allSettled = true;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto& icon = ctx.icons[i];
        auto element = icon.element.get();
        if (!element) continue;

        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;

        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;

        // --- Wave Animation ---
        double targetScale = targetScales[i];
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double targetTranslateX = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];

        icon.currentScaleX = Lerp(icon.currentScaleX, targetScale, lerpFactor);
        icon.currentScaleY = Lerp(icon.currentScaleY, targetScale, lerpFactor);
        icon.currentTranslateX = Lerp(icon.currentTranslateX, targetTranslateX, lerpFactor);

        waveScale.ScaleX(icon.currentScaleX);
        waveScale.ScaleY(icon.currentScaleY);
        waveTranslate.X(icon.currentTranslateX);

        // --- Bounce Animation ---
        double targetBounceScale = 1.0;
        double targetBounceY = 0.0;

        if (i == closestIconIndex && g_isBouncing) {
            targetBounceScale = bounceScaleFactor;
            targetBounceY = bounceTranslateY;
        }

        bounceScale.ScaleX(targetBounceScale);
        bounceScale.ScaleY(targetBounceScale);
        bounceTranslate.Y(targetBounceY);

        if (!g_isMouseInside.load()) {
             if (std::abs(icon.currentScaleX - 1.0) > 0.001 || 
                 std::abs(icon.currentTranslateX) > 0.1) {
                 allSettled = false;
             }
        } else {
            allSettled = false;
        }
    }

    if (!g_isMouseInside.load() && allSettled) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: All icons settled. Stopped render loop.");
            g_activeContextKey.store(nullptr);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
    for (auto& icon : icons) {
        icon.currentScaleX = 1.0;
        icon.currentScaleY = 1.0;
        icon.currentTranslateX = 0.0;
    }
    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) continue;
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) continue;
            
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) { waveScale.ScaleX(1.0); waveScale.ScaleY(1.0); }
            if (waveTranslate) { waveTranslate.X(0.0); }
            if (bounceScale) { bounceScale.ScaleX(1.0); bounceScale.ScaleY(1.0); }
            if (bounceTranslate) { bounceTranslate.Y(0.0); }
        }
    } catch (...) {}
}

void OnTaskbarPointerMoved(void* pThis_key, Input::PointerRoutedEventArgs const& args) {
    try {
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;
        auto& ctx = it->second;

        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;

        bool wasOutside = !g_isMouseInside.load();
        g_isMouseInside.store(true);

        // Initial setup only. Live updates are handled in OnCompositionTargetRendering now.
        if (wasOutside || !ctx.isInitialized) {
            if (!ctx.isInitialized) {
                 ResetAllIconScales(ctx.icons);
                 RefreshIconPositions(ctx);
                 ctx.isInitialized = true;
            }
        }
        
        double mouseX = args.GetCurrentPoint(frame).Position().X;
        g_activeContextKey.store(pThis_key);

        if (std::abs(mouseX - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mouseX;

        if (!g_isRenderingHooked.exchange(true)) {
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (Enter).");
        }

    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        g_isMouseInside.store(false);
        g_isBouncing.store(false);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

void RefreshIconPositions(DockAnimationContext& ctx) {
    // Preserve old map to keep animation states (smoothness)
    std::map<void*, TaskbarIconInfo> oldIconsMap;
    for(const auto& icon : ctx.icons) {
        auto elem = icon.element.get();
        if(elem) {
            void* ptr = winrt::get_abi(elem);
            oldIconsMap[ptr] = icon;
        }
    }

    ctx.icons.clear();

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

    // Detect taskbar position once per refresh
    float originY = 1.0f;
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        RECT rc;
        GetWindowRect(hTaskbar, &rc);
        if (rc.top == 0) originY = 0.0f; // Taskbar is at the top
    }

    auto processButtonElement = [&](FrameworkElement const& element) {
        try {
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            bool needsRebuild = true;

            if (tg && tg.Children().Size() >= 4) {
                if (tg.Children().GetAt(0).try_as<ScaleTransform>() &&
                    tg.Children().GetAt(1).try_as<TranslateTransform>() &&
                    tg.Children().GetAt(2).try_as<ScaleTransform>() &&
                    tg.Children().GetAt(3).try_as<TranslateTransform>()) {
                    needsRebuild = false;
                }
            }

            if (needsRebuild) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());
                tg.Children().Append(TranslateTransform());
                tg.Children().Append(ScaleTransform());
                tg.Children().Append(TranslateTransform());
                element.RenderTransform(tg);
            }

            element.RenderTransformOrigin({0.5f, originY});

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            TaskbarIconInfo info;
            info.element = element;
            info.originalCenterX = point.X + (element.ActualWidth() / 2.0);
            
            // STATE PRESERVATION:
            // If this icon was already known, inherit its current visual state 
            // so we don't snap/flicker during the refresh.
            void* ptr = winrt::get_abi(element);
            auto it = oldIconsMap.find(ptr);
            if (it != oldIconsMap.end()) {
                info.currentScaleX = it->second.currentScaleX;
                info.currentScaleY = it->second.currentScaleY;
                info.currentTranslateX = it->second.currentTranslateX;
            } else {
                info.currentScaleX = 1.0;
                info.currentScaleY = 1.0;
                info.currentTranslateX = 0.0;
            }
            
            ctx.icons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in processButtonElement: %s", e.message().c_str());
        }
    };

    try {
        FrameworkElement repeater = ctx.iconContainer.get();
        
        // If not cached or lost, find it again
        if (!repeater) {
            repeater = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
            if (!repeater) repeater = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
            if (!repeater) repeater = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
            ctx.iconContainer = repeater;
        }

        if (repeater) {
            // Update cache vars for dirty checking
            ctx.lastTotalWidth = repeater.ActualWidth();
            ctx.lastChildCount = VisualTreeHelper::GetChildrenCount(repeater);
            ctx.lastLayoutRefreshTime = std::chrono::steady_clock::now();

            int count = VisualTreeHelper::GetChildrenCount(repeater);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
                if (child) {
                    auto automationId = AutomationProperties::GetAutomationId(child);
                    std::wstring_view idStr(automationId);
                    if (idStr == L"StartButton" || idStr == L"TaskViewButton") continue;
                    processButtonElement(child);
                }
            }
        } else {
            // Fallback for older Windows versions or unusual layouts
            std::function<void(FrameworkElement)> recurse =
                [&](FrameworkElement element) {
                    if (!element) return;
                    auto automationId = AutomationProperties::GetAutomationId(element);
                    std::wstring_view idStr(automationId);
                    if (idStr == L"StartButton" || idStr == L"TaskViewButton") return;

                    auto className = winrt::get_class_name(element);
                    if (std::wstring_view(className).find(L"TaskListButton") != std::wstring_view::npos) {
                        processButtonElement(element);
                    }
                    int count = VisualTreeHelper::GetChildrenCount(element);
                    for (int i = 0; i < count; i++) {
                        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
                        if (child) recurse(child);
                    }
                };
            recurse(taskbarFrame);
        }

        std::sort(ctx.icons.begin(), ctx.icons.end(),
                  [](const TaskbarIconInfo& a, const TaskbarIconInfo& b) {
                      return a.originalCenterX < b.originalCenterX;
                  });
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error during icon search: %s", e.message().c_str());
    }
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        g_contexts[pThis] = std::move(ctx);
        Wh_Log(L"DockAnimation: Monitor %p registered (hook-based).", pThis);
    } catch (...) {}
}

void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                    winrt::Windows::Foundation::IInspectable const&) {
    try {
        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr && !g_isMouseInside.load()) {
             if (g_isRenderingHooked.exchange(false)) {
                Media::CompositionTarget::Rendering(g_renderingToken);
             }
             return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) return;

        auto& ctx = it->second;
        if (ctx.icons.empty()) {
            if (ctx.isInitialized) RefreshIconPositions(ctx);
            return;
        }

        // --- Live Update Logic (High Performance) ---
        bool needsRefresh = false;
        auto container = ctx.iconContainer.get();
        auto now = std::chrono::steady_clock::now();

        // 1. Fast Check: Size or Count changed? (Covers opening/closing apps instantly)
        if (container) {
            double w = container.ActualWidth();
            int c = VisualTreeHelper::GetChildrenCount(container);
            // Use a small epsilon for float comparison, though ActualWidth usually stays stable
            if (std::abs(w - ctx.lastTotalWidth) > 0.5 || c != ctx.lastChildCount) {
                needsRefresh = true;
            }
        }

        // 2. Slow Check: Reordering (Every 500ms)
        // If mouse is moving, user might be dragging icons.
        if (!needsRefresh) {
            auto msSinceRefresh = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    now - ctx.lastLayoutRefreshTime).count();
            if (msSinceRefresh > 500) {
                needsRefresh = true;
            }
        }

        if (needsRefresh) {
             RefreshIconPositions(ctx);
        }
        // ---------------------------------------------

        auto elapsedSinceMove =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_lastSignificantMoveTime)
                .count();

        if (g_isMouseInside.load() && elapsedSinceMove > g_settings.bounceDelay) {
            if (!g_isBouncing.exchange(true)) {
                g_bounceStartTime = now;
            }
        } else {
            g_isBouncing.store(false);
        }

        double mouseX = g_lastMouseX.load();
        ApplyAnimation(mouseX, ctx);
    } catch (...) {}
}

using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() { return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs); };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return original();
    
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args) return original();

    if (g_contexts.find(pThis) == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
    }

    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() { return TaskbarFrame_OnPointerExited_Original(pThis, pArgs); };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return original();

    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    g_settings.maxScale = (double)Wh_GetIntSetting(L"MaxScale") / 100.0;
    if (g_settings.maxScale < 1.0) g_settings.maxScale = 1.6;

    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;

    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;

    g_settings.bounceDelay = Wh_GetIntSetting(L"BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;

    g_settings.bounceEnabled = Wh_GetIntSetting(L"BounceEnabled") != 0;
    
    int speed = Wh_GetIntSetting(L"AnimationSpeed"); 
    if (speed <= 0) speed = 20;
    g_settings.lerpFactor = (double)speed / 100.0;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        { { LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))" },
          &TaskbarFrame_OnPointerMoved_Original, TaskbarFrame_OnPointerMoved_Hook },
        { { LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))" },
          &TaskbarFrame_OnPointerExited_Original, TaskbarFrame_OnPointerExited_Hook },
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) Wh_ApplyHookOperations();
            }
        }
    }
    return module;
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded.store(true);
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        WindhawkUtils::SetFunctionHook((decltype(&LoadLibraryExW))GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "LoadLibraryExW"), LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_taskbarViewDllLoaded.load()) g_hooksApplied.store(true);
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);
struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    PVOID procParam;
};

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }
    
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int n, WPARAM w, LPARAM l) -> LRESULT {
        if (n == HC_ACTION && ((CWPSTRUCT*)l)->message == msg) {
            auto* param = (RunFromWindowThreadParam*)((CWPSTRUCT*)l)->lParam;
            param->proc(param->procParam);
        }
        return CallNextHookEx(nullptr, n, w, l);
    }, nullptr, dwThreadId);
    
    if (!hook) return false;
    
    RunFromWindowThreadParam param = { proc, procParam };
    SendMessage(hWnd, msg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

void Wh_ModBeforeUninit() {
    g_activeContextKey.store(nullptr);
    g_isBouncing.store(false);
    g_isMouseInside.store(false); 
    
    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    } catch (...) {}

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        RunFromWindowThread(hTaskbar, [](PVOID) {
            try {
                for (auto& pair : g_contexts) ResetAllIconScales(pair.second.icons);
                g_contexts.clear();
            } catch (...) {}
        }, nullptr);
    } else {
        g_contexts.clear();
    }
    g_taskbarViewDllLoaded.store(false);
    g_hooksApplied.store(false);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    try {
        for (auto& pair : g_contexts) {
            pair.second.isInitialized = false;
            pair.second.icons.clear();
        }
        g_isBouncing.store(false);
    } catch (...) {}
}
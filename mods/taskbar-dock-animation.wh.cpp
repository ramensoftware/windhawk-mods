// ==WindhawkMod==
// @id              taskbar-dock-animation
// @name              Taskbar Dock Animation
// @description       Animates taskbar icons on mouse hover like in macOS
// @description:uk-UA Анімація іконок панелі завдань, як в macOS, при наведенні
// @version           1.7
// @author            Ph0en1x-dev
// @github            https://github.com/Ph0en1x-dev
// @include           explorer.exe
// @architecture      x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lwindowsapp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Dock Animation

This mod adds a macOS-like taskbar animation.
The current version is **Beta-Testing** and not yet fully stable.

**Requires Windows 11 or newer to function.**

---

### ⚠️ Known Issues & Limitations
* Icons are sometimes clipped by the taskbar.
* Upscaled icons may appear slightly blurry.

"Taskbar height and icon size" Users: If you are using this mod to create a minimalistic taskbar, please keep the Max Scale at or below 130% to avoid clipping issues. This recommendation applies to other scaling mods as well.
Dell / Alienware Users: If you encounter specific issues on these devices, please reach out to me directly or open an issue so we can investigate together.
StartAllBack Compatibility: This mod does not and will not support StartAllBack. Supporting it would require a complete rewrite of the codebase, which is currently out of scope.
Feedback: Feel free to suggest new animation ideas or curves in the mod topic or via direct message!

You can experiment with the **radius** value to achieve the best result for your setup until proper fixes for the issues above are found.

---

## 🖼️ Preview
![Taskbar Dock Animation Preview](https://raw.githubusercontent.com/Ph0en1x-dev/Hlam/refs/heads/main/Screen-recording-2025-11-06-155028.gif)
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- AnimationType: 0
  $name: Animation Curve
  $name:uk-UA: Крива анімації
  $description: Choose the feel of the scaling (0 = Cosine (Smooth), 1 = Linear, 2 = Exponential (Snappy)
  $description:uk-UA: Оберіть тип анімації (0 = Косинус (Плавно), 1 = Лінійна, 2 = Експоненціальна (Швидко)
- MaxScale: 130
  $name: Maximum scale (%)
  $name:uk-UA: Максимальний розмір (%)
  $description: How large an icon can grow (e.g., 160 = +60%)
  $description:uk-UA: Наскільки сильно іконка може збільшитись (напр., 160 = +60%)
- EffectRadius: 100
  $name: Effect radius (px)
  $name:uk-UA: Радіус ефекту (пкс)
  $description: Distance from the cursor where the animation is applied
  $description:uk-UA: Дистанція від курсора, на якій застосовується анімація
- SpacingFactor: 50
  $name: Spacing sensitivity (%)
  $name:uk-UA: Чутливість до проміжку (%)
  $description: How much neighboring icons move apart during animation (default 50%)
  $description:uk-UA: Наскільки сусідні іконки розсуваються під час анімації (за замовчуванням 50%)
- BounceDelay: 500
  $name: Bounce start delay (ms)
  $name:uk-UA: Затримка початку "дихання" (мс)
  $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 100мс)
- FocusDuration: 200
  $name: Focus animation duration (ms)
  $name:uk-UA: Тривалість анімації фокусу (мс)
  $description: Duration of the fade-in and fade-out animation (default 150ms)
  $description:uk-UA: Тривалість анімації плавного з'явлення та згасання (за замовчуванням 150мс)
- MirrorForTopTaskbar: false
  $name: Mirror animation for top taskbar
  $name:uk-UA: Дзеркальна анімація для верхньої панелі
  $description: Enable this if your taskbar is at the top of the screen to make icons animate downwards.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань знаходиться вгорі, щоб іконки анімувалися вниз.
- DisableVerticalBounce: false
  $name: Disable vertical "bounce" animation
  $name:uk-UA: Вимкнути вертикальне "дихання"
  $description: Disables the up/down "breathing" animation. Useful for vertical taskbars.
  $description:uk-UA: Вимикає анімацію "дихання" вгору/вниз. Корисно для вертикальних панелей.
- TaskbarLabelsMode: false
  $name: Taskbar Labels Compatibility
  $name:uk-UA: Сумісність із підписами (Labels)
  $description: Enable this if your taskbar shows window titles (rectangular icons) to fix animation centering.
  $description:uk-UA: Увімкніть, якщо ваша панель завдань відображає назви вікон (прямокутні кнопки), щоб виправити центрування.
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
#include <map>
#include <algorithm>
#include <chrono>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;

// Settings container
struct {
    int animationType;
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
    double focusDuration;
    bool mirrorAnimation;
    bool disableVerticalBounce;
    bool taskbarLabelsMode;
} g_settings;

// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Animation Loop Globals
winrt::event_token g_renderingToken;
std::atomic<bool> g_isRenderingHooked = false;
std::atomic<void*> g_activeContextKey = nullptr;
std::chrono::steady_clock::time_point g_lastSignificantMoveTime;
const double MOUSE_STOP_THRESHOLD = 0.5;

// Bounce Animation Globals
std::atomic<double> g_lastMouseX = -1.0;
std::atomic<bool> g_isBouncing = false;
std::chrono::steady_clock::time_point g_bounceStartTime;
const double BOUNCE_PERIOD_MS = 1200.0;
const double BOUNCE_SCALE_AMOUNT = 0.05;
const double BOUNCE_TRANSLATE_Y = -4.0;

// Focus/Fade Animation Globals
std::atomic<bool> g_isMouseInside = false;
std::atomic<double> g_animationIntensity = 0.0;
std::chrono::steady_clock::time_point g_lastRenderTime;

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    std::vector<TaskbarIconInfo> icons;
};


// One context per monitor (keyed by XAML instance pointer)
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

// Simple visual tree helpers
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

FrameworkElement FindChildByClassName(FrameworkElement element,
                                          PCWSTR className) {
    if (!element) return nullptr;
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Animation math (cosine falloff)
double CalculateScale(double distance, double radius, double maxScale) {
    if (distance > radius) return 1.0;
    
    double t = distance / radius;
    double factor = 0.0;
    
    switch (g_settings.animationType) {
        case 1: // Linear
            factor = 1.0 - t;
            break;
        case 2: // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0: // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) factor = 0;
    return (maxScale - 1.0) * factor + 1.0;
}

void ApplyAnimation(double mouseX, DockAnimationContext& ctx, double intensity) {
    const double spacingFactor = g_settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = g_settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        double distance = 0.0;
        if (g_settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX; 
            double iconEnd = iconStart + ctx.icons[i].elementWidth; 
            if (mouseX < iconStart) {
                distance = iconStart - mouseX; 
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd; 
            } else {
                distance = 0.0; 
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = CalculateScale(distance, (double)g_settings.effectRadius, g_settings.maxScale);
        extraWidths[i] = (scales[i] - 1.0) * element.ActualWidth() * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateY = 0.0;

    if (g_isBouncing && closestIconIndex != (size_t)-1 &&
        minDistance < (g_settings.effectRadius / 2.0)) {       
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - g_bounceStartTime)
                            .count();
        double t = std::fmod(elapsed_ms, BOUNCE_PERIOD_MS) / BOUNCE_PERIOD_MS;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * BOUNCE_SCALE_AMOUNT);
        bounceTranslateY = normalizedWave * BOUNCE_TRANSLATE_Y;       
        if (g_settings.mirrorAnimation) {
            bounceTranslateY *= -1.0;
        }
    }

    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) continue;
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) continue;
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) continue;
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double currentScale = 1.0 + (scales[i] - 1.0) * intensity;
        double currentTranslateX = finalShift * intensity;
        waveScale.ScaleX(currentScale);
        waveScale.ScaleY(currentScale);
        waveTranslate.X(currentTranslateX);

        if (i == closestIconIndex && g_isBouncing) {
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!g_settings.disableVerticalBounce) {
                bounceTranslate.Y(bounceTranslateY);
            } else {
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void ResetAllIconScales(std::vector<TaskbarIconInfo>& icons) {
    if (icons.empty()) return;
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

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.Y(0.0);
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
        auto frame = ctx.taskbarFrame.get();
        if (!frame) return;       
        g_isMouseInside = true;
        g_activeContextKey = pThis_key;
        double mouseX = args.GetCurrentPoint(frame).Position().X;
        if (std::abs(mouseX - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mouseX;
        if (!g_isRenderingHooked.exchange(true)) {
            g_lastRenderTime = std::chrono::steady_clock::now();
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Started render loop (FocusIn).");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerMoved: %s", e.message().c_str());
    }
}

void OnTaskbarPointerExited(void* pThis_key) {
    try {
        if (g_activeContextKey.load() == pThis_key) {
            g_isMouseInside = false;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

FrameworkElement FindChildByClassNamePartial(FrameworkElement element, PCWSTR partialName) {
    if (!element) return nullptr;
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) != std::wstring_view::npos;
    });
}

// Builds or refreshes the icon list and sets up transforms for animation (scale + translate).
void RefreshIconPositions(DockAnimationContext& ctx) {
    ctx.icons.clear();
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;
    auto processButtonElement = [&](FrameworkElement const& element) {
        try {
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());
                tg.Children().Append(TranslateTransform());
                tg.Children().Append(ScaleTransform());
                tg.Children().Append(TranslateTransform());
                element.RenderTransform(tg);
            }
            float yOrigin = g_settings.disableVerticalBounce ? 0.5f : (g_settings.mirrorAnimation ? 0.0f : 1.0f);           
            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});            
            TaskbarIconInfo info;
            info.element = element;
            info.elementWidth = element.ActualWidth(); 
            if (g_settings.taskbarLabelsMode) {                
                float iconCenterPx = 24.0f;
                float iconCenterProportion = (info.elementWidth > 0) ? (iconCenterPx / (float)info.elementWidth) : 0.5f;                
                element.RenderTransformOrigin({ iconCenterProportion, yOrigin });
                info.originalCenterX = point.X; 
            } else {
                element.RenderTransformOrigin({ 0.5f, yOrigin });
                info.originalCenterX = point.X + (info.elementWidth / 2.0);
            }
            ctx.icons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in processButtonElement: %s", e.message().c_str());
        }
    };

    try {
        FrameworkElement repeater = FindChildByClassNamePartial(taskbarFrame, L"TaskbarFrameRepeater");
        if (!repeater) repeater = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
        if (!repeater) repeater = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
        if (repeater) {
            int count = VisualTreeHelper::GetChildrenCount(repeater);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
                if (child) processButtonElement(child);
            }
        } else {
            std::function<void(FrameworkElement)> recurse = [&](FrameworkElement element) {
                if (!element) return;
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
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to initialize context for %p: %s",
               pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        auto now = std::chrono::steady_clock::now();
        double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now - g_lastRenderTime)
                               .count();
        g_lastRenderTime = now;
        double intensityChange = deltaTime / g_settings.focusDuration;
        double currentIntensity = g_animationIntensity.load();
        if (g_isMouseInside) {
            // FocusIn
            currentIntensity = std::min(1.0, currentIntensity + intensityChange);
        } else {
            // FocusOut
            currentIntensity = std::max(0.0, currentIntensity - intensityChange);
        }
        g_animationIntensity = currentIntensity;
        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            if (currentIntensity <= 0.0) {
                 Media::CompositionTarget::Rendering(g_renderingToken);
                 g_isRenderingHooked = false;
                 Wh_Log(L"DockAnimation: Stopped render loop (Key=null, Intensity=0).");
            }
            return;
        }
        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end()) {
            return;
        }
        auto& ctx = it->second;
        if (!g_isMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            g_isRenderingHooked = false;
            ResetAllIconScales(ctx.icons); 
            ctx.isInitialized = false;
            ctx.icons.clear();
            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;
            Wh_Log(L"DockAnimation: Stopped render loop (FocusOut complete).");
            return;
        }
        if (!ctx.isInitialized) {
             RefreshIconPositions(ctx);
             ctx.isInitialized = true;
        }
        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            if (ctx.icons.empty()) return;
        }
        auto elapsedSinceMove =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_lastSignificantMoveTime)
                .count();
        if (g_isMouseInside && elapsedSinceMove > g_settings.bounceDelay) {
            if (!g_isBouncing.exchange(true)) {
                g_bounceStartTime = now;
            }
        } else {
            g_isBouncing = false;
        }
        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) mouseX = 0.0; 
        ApplyAnimation(mouseX, ctx, currentIntensity);
    } catch (...) {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
        }
    }
}

// Hooking
// Pointer event hooks (new approach: activates mod immediately)
using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(), winrt::put_abi(args));
    if (!args)
        return original();
    // Initialize animation hooks only once
    if (g_contexts.find(pThis) == g_contexts.end()) {
        InitializeAnimationHooks(pThis, element);
        Wh_Log(L"DockAnimation: Initialized via OnPointerMoved (%s)", className.c_str());
    }
    // Forward event to existing logic
    OnTaskbarPointerMoved(pThis, args);
    return original();
}

using TaskbarFrame_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;

int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    };
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element)
        return original();
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskbarFrame")
        return original();
    OnTaskbarPointerExited(pThis);
    return original();
}

void LoadSettings() {
    int rawScale = Wh_GetIntSetting(L"MaxScale");

    if (rawScale < 101) {
        rawScale = 101;
    } else if (rawScale > 220) {
        rawScale = 220;
    }
    g_settings.animationType = Wh_GetIntSetting(L"AnimationType");
    g_settings.maxScale = (double)rawScale / 100.0;
    g_settings.effectRadius = Wh_GetIntSetting(L"EffectRadius");
    if (g_settings.effectRadius <= 0) g_settings.effectRadius = 100;
    g_settings.spacingFactor = (double)Wh_GetIntSetting(L"SpacingFactor") / 100.0;
    if (g_settings.spacingFactor < 0.0) g_settings.spacingFactor = 0.5;
    g_settings.bounceDelay = Wh_GetIntSetting(L"BounceDelay");
    if (g_settings.bounceDelay < 0) g_settings.bounceDelay = 100;
    g_settings.focusDuration = (double)Wh_GetIntSetting(L"FocusDuration");
    if (g_settings.focusDuration <= 0) g_settings.focusDuration = 150.0;
    g_settings.mirrorAnimation = (bool)Wh_GetIntSetting(L"MirrorForTopTaskbar");
    g_settings.disableVerticalBounce = (bool)Wh_GetIntSetting(L"DisableVerticalBounce");
    g_settings.taskbarLabelsMode = (bool)Wh_GetIntSetting(L"TaskbarLabelsMode");
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
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
        },
    };
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"DockAnimation: HookSymbols failed.");
        return false;
    }
    Wh_Log(L"DockAnimation: HookSymbols succeeded (Pointer events only).");
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
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                    Wh_Log(L"DockAnimation: Hooks applied (slow path).");
                }
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
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) return FALSE;
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(
            pKernelBaseLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"DockAnimation: Wh_ModAfterInit");

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
        Wh_Log(L"DockAnimation: Hooks already applied by Windhawk (fast path).");
    }
}

typedef void (*RunFromWindowThreadProc_t)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

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
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"DockAnimation: Wh_ModBeforeUninit (safe cleanup)");

    g_activeContextKey = nullptr;
    g_isBouncing = false;
    g_isMouseInside = false;
    g_animationIntensity = 0.0;

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }
    }
    catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT error unhooking rendering: %s",
               e.message().c_str());
    }

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        RunFromWindowThread(
            hTaskbar,
            [](PVOID) {
                try {
                    for (auto& pair : g_contexts) {
                        auto& ctx = pair.second;
                        ResetAllIconScales(ctx.icons);
                    }
                    g_contexts.clear();
                    Wh_Log(L"DockAnimation: UI contexts cleaned up.");
                }
                catch (...) {}
            },
            nullptr);
    }
    else {
        g_contexts.clear();
    }
    g_taskbarViewDllLoaded = false;
    g_hooksApplied = false;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"DockAnimation: Settings changed.");
    LoadSettings();
    try {
        for (auto& pair : g_contexts) {
            auto& ctx = pair.second;
            ctx.isInitialized = false;
            ctx.icons.clear();
        }
        g_isBouncing = false;
    } catch (...) {
    }
}


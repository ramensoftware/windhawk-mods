// ==WindhawkMod==
// @id              taskbar-dock-animation
// @name            Taskbar Dock Animation
// @description     Animates taskbar icons on mouse hover like in macOS
// @description:uk-UA Анімація іконок панелі завдань, як в macOS, при наведенні
// @version         0.9
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
![Taskbar Dock Animation Preview](https://raw.githubusercontent.com/Ph0en1x-dev/edu_db_labs/master/Записування%20з%20екрана%202025-11-06%20155028.gif)
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
- SpacingFactor: 50
  $name: Spacing sensitivity (%)
  $description: How much neighboring icons move apart during animation (default 50%)
- BounceDelay: 100
  $name: Bounce start delay (ms)
  $name:uk-UA: Затримка початку "дихання" (мс)
  $description: How long to wait after the cursor stops to start the bounce animation (default 100ms)
  $description:uk-UA: Скільки чекати після зупинки курсора перед початком анімації "дихання" (за замовчуванням 100мс)
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
#include <algorithm>
#include <chrono>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;

// Settings container
struct {
    double maxScale;
    int effectRadius;
    double spacingFactor;
    int bounceDelay;
} g_settings;

// Global flags
std::atomic<bool> g_taskbarViewDllLoaded = false;

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

// Per-monitor context
struct TaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    std::vector<TaskbarIconInfo> icons;
    winrt::event_token pointerMovedToken;
    winrt::event_token pointerExitedToken;
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

        double distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        scales[i] = CalculateScale(distance,
                                   g_settings.effectRadius,
                                   g_settings.maxScale);

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

        double newScale = scales[i];
        waveScale.ScaleX(newScale);
        waveScale.ScaleY(newScale);

        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        waveTranslate.X(finalShift);

        cumulativeShift += extraWidths[i];

        if (i == closestIconIndex && g_isBouncing) {
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);
            bounceTranslate.Y(bounceTranslateY);
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

        if (!ctx.isInitialized) {
            RefreshIconPositions(ctx);
            ctx.isInitialized = true;
        }

        double mouseX = args.GetCurrentPoint(frame).Position().X;
        g_activeContextKey = pThis_key;

        if (std::abs(mouseX - g_lastMouseX.load()) > MOUSE_STOP_THRESHOLD) {
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_lastMouseX = mouseX;
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

        if (g_activeContextKey.load() == pThis_key) {
            g_activeContextKey = nullptr;
            g_lastMouseX = -1.0;
            g_isBouncing = false;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: HRESULT Error in OnTaskbarPointerExited: %s", e.message().c_str());
    }
}

void RefreshIconPositions(DockAnimationContext& ctx) {
    ctx.icons.clear();

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) return;

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

            element.RenderTransformOrigin({ 0.5, 1.0 });

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({ 0, 0 });

            TaskbarIconInfo info;
            info.element = element;
            info.originalCenterX = point.X + (element.ActualWidth() / 2.0);
            ctx.icons.push_back(info);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in processButtonElement: %s", e.message().c_str());
        }
    };

    try {
        // Try to use TaskbarFrameRepeater children (faster path)
        FrameworkElement repeater = EnumChildElements(
            taskbarFrame,
            [](FrameworkElement child) {
                auto className = winrt::get_class_name(child);
                return std::wstring_view(className).find(L"TaskbarFrameRepeater") != std::wstring_view::npos;
            });

        if (repeater) {
            int count = VisualTreeHelper::GetChildrenCount(repeater);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
                if (!child) continue;
                processButtonElement(child);
            }
        } else {
            // Fallback: recursive search for TaskListButton
            std::function<void(FrameworkElement)> recurse =
                [&](FrameworkElement element) {
                    if (!element) return;

                    auto className = winrt::get_class_name(element);
                    if (std::wstring_view(className).find(L"TaskListButton") != std::wstring_view::npos) {
                        processButtonElement(element);
                    }

                    int childrenCount = VisualTreeHelper::GetChildrenCount(element);
                    for (int i = 0; i < childrenCount; i++) {
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

    Wh_Log(L"DockAnimation: Found and sorted %d icons.", (int)ctx.icons.size());
}

void InitializeAnimationHooks(void* pThis, FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;

        ctx.pointerMovedToken = taskbarFrame.PointerMoved(
            [pThis](auto const&, Input::PointerRoutedEventArgs const& args) {
                OnTaskbarPointerMoved(pThis, args);
            });
        ctx.pointerExitedToken = taskbarFrame.PointerExited(
            [pThis](auto const&, Input::PointerRoutedEventArgs const&) {
                OnTaskbarPointerExited(pThis);
            });

        g_contexts[pThis] = std::move(ctx);

        if (!g_isRenderingHooked.exchange(true)) {
            g_renderingToken = Media::CompositionTarget::Rendering(OnCompositionTargetRendering);
            g_lastSignificantMoveTime = std::chrono::steady_clock::now();
            Wh_Log(L"DockAnimation: Hooked CompositionTarget::Rendering.");
        }

        Wh_Log(L"DockAnimation: Monitor %p registered.", pThis);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockAnimation: Failed to attach mouse events for %p: %s", pThis, e.message().c_str());
    }
}

// Main animation loop (pulse)
void OnCompositionTargetRendering(winrt::Windows::Foundation::IInspectable const&,
                                  winrt::Windows::Foundation::IInspectable const&) {
    try {
        void* pThis_key = g_activeContextKey.load();
        if (pThis_key == nullptr) {
            g_isBouncing = false;
            return;
        }

        auto it = g_contexts.find(pThis_key);
        if (it == g_contexts.end() || !it->second.isInitialized) {
            return;
        }

        auto& ctx = it->second;
        if (ctx.icons.empty()) {
            if (ctx.isInitialized) {
                RefreshIconPositions(ctx);
            }
            return;
        }

        auto elapsedSinceMove =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - g_lastSignificantMoveTime)
                .count();

        if (elapsedSinceMove > g_settings.bounceDelay) {
            if (!g_isBouncing.exchange(true)) {
                g_bounceStartTime = std::chrono::steady_clock::now();
            }
        } else {
            g_isBouncing = false;
        }

        double mouseX = g_lastMouseX.load();
        if (mouseX == -1.0) return;

        ApplyAnimation(mouseX, ctx);
    } catch (...) {
    }
}

// Hooking
using TaskbarFrame_MeasureOverride_t = int(WINAPI*)(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;

int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    bool isNewContext = (g_contexts.find(pThis) == g_contexts.end());

    if (isNewContext && pThis) {
        try {
            FrameworkElement taskbarFrame{ nullptr };
            winrt::copy_from_abi(taskbarFrame, pThis);
            if (taskbarFrame) {
                InitializeAnimationHooks(pThis, taskbarFrame);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"DockAnimation: HRESULT error in MeasureOverride init: %s", e.message().c_str());
        }
    } else if (!isNewContext) {
        try {
            auto it = g_contexts.find(pThis);
            if (it != g_contexts.end()) {
                if (it->second.isInitialized) {
                    it->second.isInitialized = false;
                    it->second.icons.clear();
                }
            }
        } catch (...) {
            Wh_Log(L"DockAnimation: Failed to invalidate cache in MeasureOverride");
        }
    }

    return TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
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
    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
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
        Wh_ApplyHookOperations();
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

    try {
        if (g_isRenderingHooked.exchange(false)) {
            Media::CompositionTarget::Rendering(g_renderingToken);
            Wh_Log(L"DockAnimation: Unhooked CompositionTarget::Rendering.");
        }

        HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
        if (hTaskbar) {
            RunFromWindowThread(
                hTaskbar,
                [](PVOID) {
                    for (auto& pair : g_contexts) {
                        auto& ctx = pair.second;
                        auto frame = ctx.taskbarFrame.get();
                        if (frame) {
                            frame.PointerMoved(ctx.pointerMovedToken);
                            frame.PointerExited(ctx.pointerExitedToken);
                            ResetAllIconScales(ctx.icons);
                        }
                    }
                    g_contexts.clear();
                },
                nullptr);
        }

        g_taskbarViewDllLoaded = false;
        g_isBouncing = false;
        g_activeContextKey = nullptr;
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
            ctx.icons.clear();
        }
        g_isBouncing = false;
    } catch (...) {
    }
}

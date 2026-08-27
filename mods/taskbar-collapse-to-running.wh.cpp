// ==WindhawkMod==
// @id              taskbar-collapse-to-running
// @name            Taskbar collapse to running apps
// @description     Hides pinned taskbar icons whose app is not running. Hover, rest on, or click the empty taskbar to reveal them.
// @version         1.0.0
// @author          Lars
// @github          https://github.com/LarsGudm
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar collapse to running apps

Hides pinned taskbar icons whose app is not running. Icons hide and unhide in
place, so pinned order is never touched.

![Demo](https://i.imgur.com/jgDv8Su.gif)

(Hovering effect not included. Check out "Taskbar Dock Animation" by Ph0en1x-dev for that!)

Running state is read straight from the taskbar's own buttons, so detection
is exact and language-independent, and the mod reacts the moment an app opens
or closes. While nothing is happening, it does nothing at all.

## Revealing

* **Click** (default) — left-click empty taskbar to toggle; right-click stays
  Windows'.
* **Hover** — rest the cursor on empty taskbar; collapses again a grace period
  after the cursor leaves.
* **Rest** — hover, but only below **Rest speed**, so sweeping past does nothing.
* **Start** — optional, off by default: everything shows while Start is open.
* **Hotkey** (`Ctrl+Alt+T`) and the Collapse checkbox flip the whole mod,
  always instantly.

**Padding around elements** keeps reveals clear of every taskbar element,
icons and tray alike; **Reveal delay** requires the cursor to stay put first.

## Notes

* Windows 11 only. Win+number still counts pinned items the way Windows does.
* Built and tested on Windows 11 build 26200 with a left-aligned,
  single-monitor taskbar. Other builds can name taskbar internals differently.
* Revealing is shared across monitors: revealing one taskbar reveals them all.
* With Click, a click on empty taskbar reaches other mods bound to the same
  spot too, so one click can trigger both.
* Disabling the mod restores hidden icons and the animations it stripped, with
  one exception: Windows' icon appear/disappear animation on buttons the mod
  actually hid stays off until Explorer restarts, because that property cannot
  be read back to restore it.

Licensed MIT. Thanks to https://easings.net/ for the easing curve values.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RevealTrigger: click
  $name: Reveal trigger
  $options:
  - click: Click
  - rest: Rest
  - hover: Hover
  - never: None, hotkey only
- RevealOnStart: false
  $name: Reveal while Start is open
  $description: >-
    Opening Start, by the Windows key or the Start button, shows every icon and
    keeps them shown until Start closes. Works with any reveal trigger.
- RevealDelayMs: 30
  $name: Reveal delay (ms)
  $description: >-
    How long the cursor must stay in the empty taskbar area before the icons come back.
- RestSpeedPxPerSec: 300
  $name: Rest speed (px/s)
  $description: >-
    How slow the cursor must be moving across empty taskbar area to count as resting (only applies to: Reveal trigger > Rest)
- Collapsed: true
  $name: Collapse taskbar
  $description: >-
    Hide pinned icons whose app is not running. The hotkey flips the same
    switch, but only until settings are saved again, which restores this box.
- ElementPaddingPx: 24
  $name: Padding around elements (px)
  $description: >-
    Safe area around every taskbar element to prevent unwanted transitions.
- HoverGraceMs: 150
  $name: Grace period (ms)
  $description: How long the cursor may be off the taskbar before it collapses again.
- AnimationMode: spacing
  $name: Reveal animation
  $description: >-
    How revealing and collapsing are animated.
  $options:
  - none: None
  - spacing: Accordion
- AnimationCurve: circ
  $name: Easing curve
  $options:
  - sine: easeInOutSine
  - cubic: easeInOutCubic
  - circ: easeInOutCirc
- AnimationDurationMs: 160
  $name: Animation length (ms)
- AnimationAmplitudePct: 40
  $name: Animation amplitude (%)
  $description: >-
    How far the accordion stretches, as a percentage of the width the hidden
    icons actually take up. 100 stretches by exactly the space they occupy, so
    the animation scales itself to how many icons are pinned. 0 to 200.
- Hotkey: Ctrl+Alt+T
  $name: Toggle hotkey
  $description: Modifiers Ctrl, Alt, Shift, Win plus one of A-Z, 0-9, F1-F24, Space. Leave empty for no hotkey.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shobjidl.h>

// winbase.h's GetCurrentTime macro collides with the XAML animation headers.
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::Point;
using winrt::Windows::Foundation::Numerics::float3;

typedef void (*RunFromWindowThreadProc_t)(PVOID);
void RunOnAllTaskbarThreads(RunFromWindowThreadProc_t proc, PVOID param);
bool PointerClearsSurfaces(void* key, Point framePt);
void ApplyOnThisThreadNow();
void WakeAllFramesAsync();

// Pointer-event gap after which the cursor counts as parked.
constexpr double kSpeedSampleStaleMs = 150.0;

// Tick rate while something is timed; the timer never runs otherwise.
constexpr int kTimerIntervalMs = 100;

struct EasingCurve {
    double x1;
    double y1;
    double x2;
    double y2;
};

// Thanks to https://easings.net/ for the easing curve values.
constexpr EasingCurve kEaseInOutSine{0.37, 0.0, 0.63, 1.0};
constexpr EasingCurve kEaseInOutCubic{0.65, 0.0, 0.35, 1.0};
constexpr EasingCurve kEaseInOutCirc{0.85, 0.0, 0.15, 1.0};

// An id, so a settings change can never be read half-updated.
enum class CurveId { Sine, Cubic, Circ };

EasingCurve const& CurveFor(CurveId id) {
    switch (id) {
        case CurveId::Cubic:
            return kEaseInOutCubic;
        case CurveId::Circ:
            return kEaseInOutCirc;
        default:
            return kEaseInOutSine;
    }
}

enum class AnimationMode { None, Spacing };
enum class RevealTrigger { Never, Hover, Rest, Click };

// GapClose: hide an icon instantly, hold its gap open until the taskbar
// stops churning, then ease the row shut.
enum class AnimKind { Accordion, GapClose };

// Quiet time after the last button churn before the gap eases shut, and the
// longest the gap may hold open waiting for it.
constexpr double kGapSettleMs = 250.0;
constexpr double kGapHoldMaxMs = 1000.0;

struct {
    std::atomic<RevealTrigger> revealTrigger;
    std::atomic<bool> revealOnStart;
    std::atomic<int> restSpeedPxPerSec;
    std::atomic<int> revealDelayMs;
    std::atomic<int> elementPaddingPx;
    std::atomic<int> hoverGraceMs;
    std::atomic<AnimationMode> animationMode;
    std::atomic<CurveId> animationCurve;
    std::atomic<int> animationDurationMs;
    std::atomic<int> animationAmplitudePct;
} g_settings;

std::atomic<bool> g_unloading = false;
std::atomic<bool> g_taskbarViewDllLoaded = false;

std::atomic<bool> g_collapseEnabled = true;

std::atomic<bool> g_revealed = false;

// Reveal caused by Start opening; pinned against grace aging until it closes.
std::atomic<bool> g_revealedByStart = false;

std::atomic<ULONGLONG> g_lastCursorInsideTick = 0;

std::atomic<ULONGLONG> g_emptyHoverSinceTick = 0;

// Rest speed from pointer-event deltas; a stale sample reads as zero (parked).
std::atomic<double> g_cursorSpeedPxS = 0;
std::atomic<double> g_speedSampleMs = 0;
std::atomic<bool> g_speedMeasured = false;
std::atomic<bool> g_lastPointerQualified = false;
POINT g_speedSamplePrevPos = {};

void* g_lastQualifiedKey = nullptr;
Point g_lastQualifiedFramePt = {};

// Last TaskListButton state churn; the gap-close hold waits for quiet.
std::atomic<double> g_lastButtonChurnMs = 0;

// Bumped by hotkey/settings; a new value applies once, without animation.
std::atomic<uint64_t> g_instantApplyGen = 0;

// Wakes queued but not yet run; unload drains this to zero.
std::atomic<int> g_pendingWakes = 0;

// Sink callbacks in flight; teardown waits for zero (Unadvise does not fence).
std::atomic<int> g_sinkCallbacks = 0;

// Serializes start/stop across Windhawk's thread and the LoadLibraryExW hook.
std::mutex g_hotkeyThreadMutex;
HANDLE g_hotkeyThread = nullptr;
DWORD g_hotkeyThreadId = 0;
// Signalled once the worker owns a message queue, so WM_QUIT cannot be lost.
HANDLE g_hotkeyThreadReady = nullptr;

struct DeanimatedButton {
    winrt::weak_ref<FrameworkElement> button;
    Media::Animation::TransitionCollection originalTransitions{nullptr};
    winrt::Windows::UI::Composition::ImplicitAnimationCollection
        originalImplicit{nullptr};
};

// Owned by its taskbar's UI thread; only dispatcher is read cross-thread,
// under g_framesMutex.
struct FrameContext {
    void* key = nullptr;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    winrt::weak_ref<FrameworkElement> frame;
    winrt::weak_ref<FrameworkElement> repeater;
    DispatcherTimer timer{nullptr};
    winrt::event_token tickToken{};
    DWORD threadId = 0;
    // Buttons this mod hid, and therefore the only ones it may show again.
    std::vector<winrt::weak_ref<FrameworkElement>> hiddenByUs;
    std::vector<winrt::weak_ref<FrameworkElement>> lastButtons;
    std::vector<DeanimatedButton> deanimated;

    // Visibility state currently on screen; -1 until the first apply.
    int appliedCollapse = -1;
    bool animActive = false;
    AnimKind animKind = AnimKind::Accordion;
    bool animTargetCollapse = false;
    double animStartMs = 0;
    // Snapshot at StartAnimation, so both accordion halves use one value.
    double animAmplitude = 0;
    bool animSwapped = false;
    std::vector<float> animGapOffsets;
    // GapClose: collapsed at start, held transparent until the run ends.
    std::vector<std::pair<FrameworkElement, double>> gapHidden;
    std::vector<FrameworkElement> animButtons;
    // Idle icons' width while visible; hidden ones measure zero.
    double idleWidth = 0;
    winrt::event_token renderingToken{};
    bool renderingHooked = false;
    // Coalesces UpdateVisualStates bursts; cleared when the apply runs.
    bool applyPosted = false;
    // Retry budget for an apply that found no buttons (taskbar mid-rebuild).
    int applyRetries = 0;
    uint64_t instantGenSeen = 0;
};

std::mutex g_framesMutex;

// Never destroyed: CRT shutdown would release XAML from the wrong thread.
[[clang::no_destroy]] std::optional<std::unordered_map<void*, FrameContext>>
    g_frames{std::in_place};

FrameContext* GetFrameContext(void* key);

// Animation clock; GetTickCount64's ~16 ms quantum reads as stutter.
double NowMs() {
    static const double frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return (double)f.QuadPart;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency;
}

// ---------------------------------------------------------------- visual tree

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> const& enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

void CollectTaskListButtons(FrameworkElement root,
                            int depth,
                            std::vector<FrameworkElement>& out) {
    if (!root || depth < 0) {
        return;
    }

    EnumChildElements(root, [&](FrameworkElement child) {
        auto classNameHstring = winrt::get_class_name(child);
        std::wstring_view className(classNameHstring);
        if (className.find(L"TaskListButton") != std::wstring_view::npos &&
            className.find(L"Panel") == std::wstring_view::npos) {
            out.push_back(child);
        } else {
            CollectTaskListButtons(child, depth - 1, out);
        }
        return false;
    });
}

// The taskbar's own running state; a mandatory symbol, so a build missing it
// refuses to load rather than mis-hide.
using TaskListButton_get_IsRunning_t = HRESULT(__cdecl*)(void* pThis,
                                                         bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

// Fails open: hide only on a positive answer from the exact runtime class.
bool TaskListButton_IsRunning(FrameworkElement taskListButtonElement) {
    if (!TaskListButton_get_IsRunning_Original ||
        winrt::get_class_name(taskListButtonElement) !=
            L"Taskbar.TaskListButton") {
        return true;
    }
    bool isRunning = false;
    if (FAILED(TaskListButton_get_IsRunning_Original(
            winrt::get_abi(taskListButtonElement
                               .as<winrt::Windows::Foundation::IUnknown>()),
            &isRunning))) {
        return true;
    }
    return isRunning;
}

// ------------------------------------------------------------------- applying

bool TakeFromHiddenSet(std::vector<winrt::weak_ref<FrameworkElement>>& hidden,
                       FrameworkElement const& element,
                       bool remove) {
    bool found = false;

    for (size_t i = 0; i < hidden.size();) {
        auto resolved = hidden[i].get();
        if (!resolved) {
            hidden.erase(hidden.begin() + i);
            continue;
        }

        if (resolved == element) {
            found = true;
            if (remove) {
                hidden.erase(hidden.begin() + i);
                continue;
            }
        }

        i++;
    }

    return found;
}

bool CursorOverAnyTaskbar() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return false;
    }

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hRoot, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return true;
    }

    // Popups on a taskbar thread count as taskbar, or grace collapses menus.
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hRoot, &processId);
    if (threadId && processId == GetCurrentProcessId()) {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.threadId == threadId) {
                return true;
            }
        }
    }

    return false;
}

// No getter, so irreversible: only call for buttons actually being hidden.
void ClearImplicitShowHide(FrameworkElement const& button) {
    try {
        Hosting::ElementCompositionPreview::SetImplicitShowAnimation(button,
                                                                     nullptr);
        Hosting::ElementCompositionPreview::SetImplicitHideAnimation(button,
                                                                     nullptr);
    } catch (winrt::hresult_error const&) {
    }
}

void DeanimateButton(FrameContext& ctx, FrameworkElement button) {
    // Every pass, not strip-once: one stripped too early slides forever.
    DeanimatedButton* known = nullptr;
    for (size_t i = 0; i < ctx.deanimated.size();) {
        auto resolved = ctx.deanimated[i].button.get();
        if (!resolved) {
            ctx.deanimated.erase(ctx.deanimated.begin() + i);
            continue;
        }
        if (resolved == button) {
            known = &ctx.deanimated[i];
        }
        i++;
    }

    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(button);
    auto transitions = button.Transitions();
    auto implicit_ = visual ? visual.ImplicitAnimations() : nullptr;

    if (!known) {
        DeanimatedButton entry;
        entry.button = winrt::make_weak(button);
        entry.originalTransitions = transitions;
        entry.originalImplicit = implicit_;
        ctx.deanimated.push_back(entry);
    } else {
        if (transitions && !known->originalTransitions) {
            known->originalTransitions = transitions;
        }
        if (implicit_ && !known->originalImplicit) {
            known->originalImplicit = implicit_;
        }
    }

    if (transitions) {
        button.Transitions(nullptr);
    }
    if (visual && implicit_) {
        visual.ImplicitAnimations(nullptr);
    }
}

void ReanimateButtons(FrameContext& ctx) {
    for (auto& entry : ctx.deanimated) {
        auto button = entry.button.get();
        if (!button) {
            continue;
        }

        button.Transitions(entry.originalTransitions);
        if (auto visual =
                Hosting::ElementCompositionPreview::GetElementVisual(button)) {
            visual.ImplicitAnimations(entry.originalImplicit);
        }
    }
    ctx.deanimated.clear();
}

// ------------------------------------------------------------------ easing

// One axis of a cubic bezier whose endpoints are fixed at 0 and 1.
double BezierAxis(double p1, double p2, double u) {
    double v = 1 - u;
    return 3 * p1 * v * v * u + 3 * p2 * v * u * u + u * u * u;
}

double CubicBezierEase(EasingCurve const& curve, double t) {
    if (t <= 0) {
        return 0;
    }
    if (t >= 1) {
        return 1;
    }

    double lo = 0;
    double hi = 1;
    for (int i = 0; i < 24; i++) {
        double mid = (lo + hi) * 0.5;
        if (BezierAxis(curve.x1, curve.x2, mid) < t) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return BezierAxis(curve.y1, curve.y2, (lo + hi) * 0.5);
}

// ---------------------------------------------------------------- animation

// Leftmost held still; deliberately no re-centre compensation.
void ApplySpacing(std::vector<FrameworkElement> const& buttons,
                  double totalExtra) {
    std::vector<FrameworkElement const*> visible;
    for (auto const& button : buttons) {
        if (button.Visibility() == Visibility::Visible) {
            visible.push_back(&button);
        } else {
            button.Translation(float3{0, 0, 0});
        }
    }

    int gaps = (int)visible.size() - 1;
    double perGap = gaps > 0 ? totalExtra / gaps : 0;

    for (int i = 0; i < (int)visible.size(); i++) {
        visible[i]->Translation(float3{(float)(i * perGap), 0, 0});
    }
}

void ClearSpacing(std::vector<FrameworkElement> const& buttons) {
    for (auto const& button : buttons) {
        button.Translation(float3{0, 0, 0});
    }
}

// With deferHide set, hides are only faded; CollapseDeferredHides lands them.
void SetCollapseState(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    bool collapse,
    std::vector<std::pair<FrameworkElement, double>>* deferHide = nullptr) {
    double idleVisibleWidth = 0;
    bool idleSetComplete = true;

    for (auto const& button : buttons) {
        bool weHidIt = TakeFromHiddenSet(ctx.hiddenByUs, button, false);
        bool running = TaskListButton_IsRunning(button);
        bool shouldHide = collapse && !running;

        // Measured pre-flip; a partial idle set must not overwrite the cache.
        if (!running) {
            if (button.Visibility() == Visibility::Visible) {
                idleVisibleWidth += button.ActualWidth();
            } else {
                idleSetComplete = false;
            }
        }

        if (shouldHide) {
            // Windows' own hides stay unrecorded; recorded ones re-assert.
            if (button.Visibility() == Visibility::Visible) {
                if (deferHide) {
                    deferHide->emplace_back(button, button.Opacity());
                    button.Opacity(0);
                } else {
                    ClearImplicitShowHide(button);
                    button.Visibility(Visibility::Collapsed);
                    if (!weHidIt) {
                        ctx.hiddenByUs.push_back(winrt::make_weak(button));
                    }
                }
            }
        } else if (weHidIt) {
            button.Visibility(Visibility::Visible);
            TakeFromHiddenSet(ctx.hiddenByUs, button, true);
        }
    }

    if (idleSetComplete && idleVisibleWidth > 0.5) {
        ctx.idleWidth = idleVisibleWidth;
    }

    ctx.appliedCollapse = collapse ? 1 : 0;
}

void OnRenderingTick(void* key);

void HookRendering(FrameContext& ctx, void* key) {
    if (ctx.renderingHooked) {
        return;
    }
    ctx.renderingToken = Media::CompositionTarget::Rendering(
        [key](IInspectable const&, IInspectable const&) {
            OnRenderingTick(key);
        });
    ctx.renderingHooked = true;
}

void UnhookRendering(FrameContext& ctx) {
    if (!ctx.renderingHooked) {
        return;
    }
    Media::CompositionTarget::Rendering(ctx.renderingToken);
    ctx.renderingHooked = false;
}

// Tree order is not visual order; sort by on-screen X, hidden to the back.
void SortButtonsByPosition(std::vector<FrameworkElement>& buttons,
                           FrameworkElement frame) {
    std::vector<std::pair<double, FrameworkElement>> keyed;
    keyed.reserve(buttons.size());

    for (auto& button : buttons) {
        double x = std::numeric_limits<double>::max();
        if (button.Visibility() == Visibility::Visible) {
            try {
                x = button.TransformToVisual(frame)
                        .TransformPoint(Point{0, 0})
                        .X;
            } catch (winrt::hresult_error const&) {
            }
        }
        keyed.emplace_back(x, button);
    }

    std::stable_sort(keyed.begin(), keyed.end(),
                     [](auto const& a, auto const& b) {
                         return a.first < b.first;
                     });

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i] = keyed[i].second;
    }
}

// Lands the deferred hides for real, kept transparent until the run ends so
// a re-show by Windows renders empty instead of flashing.
void CollapseDeferredHides(
    FrameContext& ctx,
    std::vector<std::pair<FrameworkElement, double>>& deferHide) {
    for (auto& entry : deferHide) {
        try {
            if (entry.first.Visibility() == Visibility::Visible) {
                ClearImplicitShowHide(entry.first);
                entry.first.Visibility(Visibility::Collapsed);
                if (!TakeFromHiddenSet(ctx.hiddenByUs, entry.first, false)) {
                    ctx.hiddenByUs.push_back(winrt::make_weak(entry.first));
                }
                ctx.gapHidden.push_back(std::move(entry));
            } else {
                // Windows hid it itself; not ours to keep.
                entry.first.Opacity(entry.second);
            }
        } catch (winrt::hresult_error const&) {
        }
    }
    deferHide.clear();
}

void StopAnimation(FrameContext& ctx) {
    // Collapsed since the start, so the opacity restore is invisible here.
    for (auto& entry : ctx.gapHidden) {
        try {
            entry.first.Opacity(entry.second);
        } catch (winrt::hresult_error const&) {
        }
    }
    ctx.gapHidden.clear();
    ctx.animActive = false;
    ctx.animSwapped = false;
    UnhookRendering(ctx);
    ClearSpacing(ctx.animButtons);
    ctx.animButtons.clear();
    ctx.animGapOffsets.clear();
}

void StartAnimation(FrameContext& ctx,
                    std::vector<FrameworkElement> buttons,
                    bool targetCollapse) {
    ctx.animActive = true;
    ctx.animKind = AnimKind::Accordion;
    ctx.animTargetCollapse = targetCollapse;
    ctx.animStartMs = NowMs();
    ctx.animAmplitude =
        ctx.idleWidth * (double)g_settings.animationAmplitudePct / 100.0;
    ctx.animSwapped = false;
    ctx.animButtons = std::move(buttons);
    ctx.animGapOffsets.clear();
    if (auto frame = ctx.frame.get()) {
        SortButtonsByPosition(ctx.animButtons, frame);
    }
    HookRendering(ctx, ctx.key);
}

// Freeze survivors at their pre-collapse X, in this same pass.
void MeasureGapOffsets(FrameContext& ctx, FrameworkElement frame) {
    std::vector<FrameworkElement> survivors;
    std::vector<double> oldX;
    for (auto const& button : ctx.animButtons) {
        if (button.Visibility() != Visibility::Visible) {
            continue;
        }
        try {
            double x =
                button.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
            survivors.push_back(button);
            oldX.push_back(x);
        } catch (winrt::hresult_error const&) {
        }
    }
    ctx.animButtons = std::move(survivors);
    ctx.animGapOffsets.clear();
    if (ctx.animButtons.empty()) {
        return;
    }

    frame.UpdateLayout();

    // A re-show landing inside that layout pass reads as gaps already
    // closed and would kill the hold with the icon back; repair, lay out
    // again.
    bool reShown = false;
    for (auto& weak : ctx.hiddenByUs) {
        if (auto button = weak.get()) {
            if (button.Visibility() == Visibility::Visible) {
                ClearImplicitShowHide(button);
                button.Visibility(Visibility::Collapsed);
                reShown = true;
            }
        }
    }
    if (reShown) {
        Wh_Log(L"GapClose: re-show during gap measurement, repaired");
        frame.UpdateLayout();
    }

    bool anyMoved = false;
    for (size_t i = 0; i < ctx.animButtons.size(); i++) {
        double offset = 0;
        try {
            double newX = ctx.animButtons[i]
                              .TransformToVisual(frame)
                              .TransformPoint(Point{0, 0})
                              .X;
            offset = oldX[i] - newX;
        } catch (winrt::hresult_error const&) {
        }
        if (offset > 0.5 || offset < -0.5) {
            anyMoved = true;
        } else {
            offset = 0;
        }
        ctx.animGapOffsets.push_back((float)offset);
    }
    if (!anyMoved) {
        ctx.animGapOffsets.clear();
        return;
    }

    for (size_t i = 0; i < ctx.animButtons.size(); i++) {
        ctx.animButtons[i].Translation(float3{ctx.animGapOffsets[i], 0, 0});
    }
}

// The hide lands here and now; survivors freeze mid-gap in the same pass.
// The rendering tick waits out Windows' close churn, then eases the gap shut.
void StartGapCloseAnimation(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    std::vector<std::pair<FrameworkElement, double>> deferHide) {
    ctx.animButtons = buttons;
    ctx.animGapOffsets.clear();
    ctx.animActive = true;
    ctx.animKind = AnimKind::GapClose;
    // Matches the applied state so the tick keeps its hands off while it runs.
    ctx.animTargetCollapse = ctx.appliedCollapse == 1;
    ctx.animStartMs = NowMs();
    ctx.animSwapped = false;
    CollapseDeferredHides(ctx, deferHide);
    if (auto frame = ctx.frame.get()) {
        MeasureGapOffsets(ctx, frame);
    }
    if (ctx.animGapOffsets.empty()) {
        // No gap to close (rightmost icons); the hide already landed.
        StopAnimation(ctx);
        return;
    }
    HookRendering(ctx, ctx.key);
}

// Never runs mid-animation; reports false so pending work can retry.
bool ApplyToFrame(FrameContext& ctx, bool collapse, bool instantRequested) {
    auto frame = ctx.frame.get();
    if (!frame) {
        return false;
    }

    std::vector<FrameworkElement> buttons;
    if (auto repeater = ctx.repeater.get()) {
        CollectTaskListButtons(repeater, 3, buttons);
    }
    if (buttons.empty()) {
        CollectTaskListButtons(frame, 8, buttons);
        if (!buttons.empty()) {
            if (auto parent = Media::VisualTreeHelper::GetParent(buttons[0])
                                  .try_as<FrameworkElement>()) {
                ctx.repeater = winrt::make_weak(parent);
            }
        }
    }

    ctx.lastButtons.clear();
    for (auto& button : buttons) {
        ctx.lastButtons.push_back(winrt::make_weak(button));
    }

    if (buttons.empty()) {
        return false;
    }

    if (!g_unloading) {
        if (!g_collapseEnabled && ctx.hiddenByUs.empty()) {
            ReanimateButtons(ctx);
        } else {
            for (auto& button : buttons) {
                DeanimateButton(ctx, button);
            }
        }
    }

    bool instant = instantRequested || ctx.appliedCollapse < 0 ||
                   g_settings.animationMode == AnimationMode::None;

    if (!instant && (collapse ? 1 : 0) != ctx.appliedCollapse) {
        StartAnimation(ctx, std::move(buttons), collapse);
        return true;
    }

    bool animateGaps = !instant && !g_unloading &&
                       g_settings.animationMode == AnimationMode::Spacing &&
                       (collapse ? 1 : 0) == ctx.appliedCollapse;
    std::vector<std::pair<FrameworkElement, double>> deferHide;
    SetCollapseState(ctx, buttons, collapse,
                     animateGaps ? &deferHide : nullptr);
    if (!deferHide.empty()) {
        StartGapCloseAnimation(ctx, buttons, std::move(deferHide));
    }
    return true;
}

// One curve spans the run; the icon swap sits at its steepest midpoint.
void OnRenderingTick(void* key) {
    FrameContext* ctx = GetFrameContext(key);
    if (!ctx || !ctx->animActive) {
        return;
    }

    if (g_unloading || ctx->animButtons.empty()) {
        StopAnimation(*ctx);
        return;
    }

    double duration = (double)g_settings.animationDurationMs;
    double t = duration > 0
                   ? std::clamp((NowMs() - ctx->animStartMs) / duration, 0.0, 1.0)
                   : 1.0;
    double eased = CubicBezierEase(CurveFor(g_settings.animationCurve), t);

    if (ctx->animKind == AnimKind::GapClose) {
        // Windows' close churn can re-show what just got hidden, any frame;
        // re-assert every frame. gapHidden stays transparent for the whole
        // run, so a lost visibility race still renders empty.
        try {
            bool repaired = false;
            for (auto& entry : ctx->gapHidden) {
                if (entry.first.Opacity() != 0.0) {
                    entry.first.Opacity(0);
                }
            }
            for (auto& weak : ctx->hiddenByUs) {
                if (auto button = weak.get()) {
                    if (button.Visibility() == Visibility::Visible) {
                        ClearImplicitShowHide(button);
                        button.Visibility(Visibility::Collapsed);
                        repaired = true;
                    }
                }
            }
            // Same-frame layout, or survivors render once at shifted spots.
            if (repaired) {
                if (auto frame = ctx->frame.get()) {
                    frame.UpdateLayout();
                }
            }
        } catch (winrt::hresult_error const&) {
        }

        if (!ctx->animSwapped) {
            // Gap held open; the ease starts once the taskbar goes quiet.
            double now = NowMs();
            if (now - g_lastButtonChurnMs.load() < kGapSettleMs &&
                now - ctx->animStartMs < kGapHoldMaxMs) {
                return;
            }
            ctx->animSwapped = true;
            ctx->animStartMs = now;
            return;
        }

        double closeT =
            duration > 0
                ? std::clamp((NowMs() - ctx->animStartMs) / duration, 0.0, 1.0)
                : 1.0;
        if (closeT >= 1.0) {
            StopAnimation(*ctx);
            return;
        }
        double factor =
            1.0 - CubicBezierEase(CurveFor(g_settings.animationCurve), closeT);
        for (size_t i = 0; i < ctx->animButtons.size(); i++) {
            ctx->animButtons[i].Translation(
                float3{(float)(ctx->animGapOffsets[i] * factor), 0, 0});
        }
        return;
    }

    if (t >= 0.5 && !ctx->animSwapped) {
        SetCollapseState(*ctx, ctx->animButtons, ctx->animTargetCollapse);
        // The flip can prompt fresh animations while the tick is paused.
        for (auto& button : ctx->animButtons) {
            DeanimateButton(*ctx, button);
        }
        if (auto frame = ctx->frame.get()) {
            frame.UpdateLayout();
            SortButtonsByPosition(ctx->animButtons, frame);
            if (!ctx->animTargetCollapse) {
                double width = 0;
                for (auto& button : ctx->animButtons) {
                    if (button.Visibility() == Visibility::Visible &&
                        !TaskListButton_IsRunning(button)) {
                        width += button.ActualWidth();
                    }
                }
                if (width > 0.5) {
                    ctx->idleWidth = width;
                }
            }
        }
        ctx->animSwapped = true;
    }

    double amplitude = ctx->animAmplitude;

    if (t >= 1.0) {
        StopAnimation(*ctx);
        return;
    }

    double direction = ctx->animTargetCollapse ? -1.0 : 1.0;
    double totalExtra = ctx->animSwapped
                            ? -direction * amplitude * (1 - eased)
                            : direction * amplitude * eased;

    ApplySpacing(ctx->animButtons, totalExtra);
}

void RestoreFrame(FrameContext& ctx) {
    StopAnimation(ctx);

    for (auto& weak : ctx.lastButtons) {
        if (auto button = weak.get()) {
            button.Translation(float3{0, 0, 0});
        }
    }

    for (auto& weak : ctx.hiddenByUs) {
        if (auto button = weak.get()) {
            button.Visibility(Visibility::Visible);
        }
    }
    ctx.hiddenByUs.clear();
    ReanimateButtons(ctx);
}

void OnTimerTick(void* key) {
    FrameContext* ctx = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        auto it = g_frames->find(key);
        if (it == g_frames->end()) {
            return;
        }
        ctx = &it->second;
    }

    if (!ctx->frame.get()) {
        // The rendering hook must go too, or it dangles after unload.
        StopAnimation(*ctx);
        std::lock_guard<std::mutex> guard(g_framesMutex);
        if (ctx->timer) {
            ctx->timer.Stop();
            ctx->timer.Tick(ctx->tickToken);
        }
        g_frames->erase(key);
        return;
    }

    if (g_unloading) {
        RestoreFrame(*ctx);
        return;
    }

    ULONGLONG now = GetTickCount64();
    RevealTrigger trigger = g_settings.revealTrigger;
    bool hoverLike =
        trigger == RevealTrigger::Hover || trigger == RevealTrigger::Rest;
    bool cursorInside = CursorOverAnyTaskbar();
    // A flip must reach every monitor; only a posted wake does that now.
    bool flipped = false;

    if (g_revealed) {
        if (!g_collapseEnabled ||
            (trigger == RevealTrigger::Never && !g_revealedByStart)) {
            g_revealed = false;
            g_revealedByStart = false;
            g_emptyHoverSinceTick = 0;
            flipped = true;
        } else if (g_revealedByStart) {
            // Pinned while Start is open; the worker ends it on close.
            g_lastCursorInsideTick = now;
        } else if (hoverLike) {
            // Only hover ages out on grace; a click reveal stays.
            if (cursorInside) {
                g_lastCursorInsideTick = now;
            } else if (now - g_lastCursorInsideTick >
                       (ULONGLONG)g_settings.hoverGraceMs) {
                g_revealed = false;
                g_emptyHoverSinceTick = 0;
                flipped = true;
            }
        }
    } else if (hoverLike && g_collapseEnabled) {
        // The tick has the last word on the latch; exit events can be missed.
        if (!cursorInside) {
            g_lastPointerQualified = false;
            g_emptyHoverSinceTick = 0;
        }

        if (trigger == RevealTrigger::Rest) {
            double speed = NowMs() - g_speedSampleMs > kSpeedSampleStaleMs
                               ? 0.0
                               : g_cursorSpeedPxS.load();
            if (speed > (double)g_settings.restSpeedPxPerSec) {
                g_emptyHoverSinceTick = 0;
            } else if (g_emptyHoverSinceTick == 0 && g_lastPointerQualified &&
                       cursorInside) {
                // Parked cursors produce no events; arm off the last check.
                g_emptyHoverSinceTick = now;
            }
        }

        ULONGLONG since = g_emptyHoverSinceTick;
        if (since != 0 && cursorInside &&
            now - since >= (ULONGLONG)g_settings.revealDelayMs) {
            if (PointerClearsSurfaces(g_lastQualifiedKey,
                                      g_lastQualifiedFramePt)) {
                g_revealed = true;
                flipped = true;
            } else {
                g_lastPointerQualified = false;
            }
            g_emptyHoverSinceTick = 0;
        }
    }

    if (flipped) {
        WakeAllFramesAsync();
    }

    bool desired = g_collapseEnabled && !g_revealed;

    // Consumed only on a successful apply.
    uint64_t instantGen = g_instantApplyGen;
    bool instant = ctx->instantGenSeen != instantGen;

    // Watchdog: Rendering stops when the bar is not being composed.
    if (ctx->animActive &&
        NowMs() - ctx->animStartMs >
            (double)g_settings.animationDurationMs * 3.0 + 1000.0) {
        StopAnimation(*ctx);
    }

    // A running animation toward the right state owns the buttons.
    if (ctx->animActive && (instant || desired != ctx->animTargetCollapse)) {
        StopAnimation(*ctx);
    }
    if (!ctx->animActive) {
        if (ApplyToFrame(*ctx, desired, instant)) {
            ctx->instantGenSeen = instantGen;
            ctx->applyRetries = 0;
        } else if (ctx->applyRetries > 0) {
            ctx->applyRetries--;
        } else if (instant || ctx->appliedCollapse < 0) {
            ctx->applyRetries = 20;
        }
    }

    // The timer runs only while something needs a clock; everything else
    // arrives as events. Reveal terms are global: cost, not correctness.
    bool timing = ctx->animActive || g_emptyHoverSinceTick != 0 ||
                  (g_revealed && hoverLike && !g_revealedByStart) ||
                  (hoverLike && g_collapseEnabled && !g_revealed &&
                   g_lastPointerQualified) ||
                  ctx->applyRetries > 0;
    if (timing != ctx->timer.IsEnabled()) {
        if (timing) {
            ctx->timer.Start();
        } else {
            ctx->timer.Stop();
        }
    }
}

// The decrement is tied to the delegate's LIFETIME, so a dropped wake still
// counts down. Increment first: a throwing ctor calls the deleter.
bool PostApply(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher) {
    g_pendingWakes++;
    auto pending =
        std::shared_ptr<void>(nullptr, [](void*) { g_pendingWakes--; });
    // The count must never rise after unload's drain saw zero.
    if (g_unloading) {
        return false;
    }
    try {
        dispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [pending]() {
                if (!g_unloading) {
                    try {
                        ApplyOnThisThreadNow();
                    } catch (...) {
                    }
                }
            });
        return true;
    } catch (winrt::hresult_error const&) {
        return false;
    }
}

// Timer created stopped; the first apply is posted, never run inline.
void RegisterFrame(void* key, FrameworkElement frame) {
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);

        // Under the lock: must not insert a live timer after cleanup ran.
        if (g_unloading) {
            return;
        }

        auto it = g_frames->find(key);
        if (it != g_frames->end()) {
            if (it->second.frame.get()) {
                return;
            }
            StopAnimation(it->second);
            if (it->second.timer) {
                it->second.timer.Stop();
                it->second.timer.Tick(it->second.tickToken);
            }
            g_frames->erase(it);
        }

        FrameContext ctx;
        ctx.key = key;
        ctx.instantGenSeen = g_instantApplyGen;
        ctx.frame = winrt::make_weak(frame);
        ctx.dispatcher = frame.Dispatcher();
        ctx.threadId = GetCurrentThreadId();
        ctx.applyPosted = true;

        ctx.timer = DispatcherTimer();
        ctx.timer.Interval(std::chrono::milliseconds(kTimerIntervalMs));
        ctx.tickToken = ctx.timer.Tick([key](IInspectable const&,
                                             IInspectable const&) {
            OnTimerTick(key);
        });

        dispatcher = ctx.dispatcher;
        Wh_Log(L"Registered taskbar frame on thread %u", ctx.threadId);
        g_frames->insert_or_assign(key, std::move(ctx));
    }
    // Outside the lock: the one COM call this function makes.
    if (!PostApply(dispatcher)) {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        auto it = g_frames->find(key);
        if (it != g_frames->end()) {
            it->second.applyPosted = false;
        }
    }
}

bool IsFrameRegistered(void* key) {
    std::lock_guard<std::mutex> guard(g_framesMutex);
    return g_frames->find(key) != g_frames->end();
}

// Stable addresses, single-thread owners; the pointer outlives the lock.
FrameContext* GetFrameContext(void* key) {
    std::lock_guard<std::mutex> guard(g_framesMutex);
    auto it = g_frames->find(key);
    return it == g_frames->end() ? nullptr : &it->second;
}

void ApplyOnThisThreadNow() {
    std::vector<void*> keys;
    DWORD threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.threadId == threadId) {
                ctx.applyPosted = false;
                keys.push_back(key);
            }
        }
    }

    for (void* key : keys) {
        OnTimerTick(key);
    }
}

// OnTimerTick stops them again once nothing is being timed.
void StartTimersOnThisThread() {
    DWORD threadId = GetCurrentThreadId();
    std::lock_guard<std::mutex> guard(g_framesMutex);
    for (auto& [key, ctx] : *g_frames) {
        if (ctx.threadId == threadId && ctx.timer && !ctx.timer.IsEnabled()) {
            ctx.timer.Start();
        }
    }
}

void WakeAllFramesAsync() {
    if (g_unloading) {
        return;
    }

    std::vector<winrt::Windows::UI::Core::CoreDispatcher> dispatchers;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.dispatcher) {
                dispatchers.push_back(ctx.dispatcher);
            }
        }
    }

    for (auto& dispatcher : dispatchers) {
        PostApply(dispatcher);
    }
}

// --------------------------------------------------------------------- hover

bool IsPointerOverEmptySpace(Input::PointerRoutedEventArgs const& args) {
    auto source = args.OriginalSource().try_as<DependencyObject>();

    for (int i = 0; source && i < 16; i++) {
        if (auto element = source.try_as<FrameworkElement>()) {
            auto classNameHstring = winrt::get_class_name(element);
            std::wstring_view className(classNameHstring);
            if (className == L"Taskbar.TaskbarFrame") {
                break;
            }
            if (className.find(L"Button") != std::wstring_view::npos ||
                className.find(L"SystemTray") != std::wstring_view::npos) {
                return false;
            }
        }
        source = Media::VisualTreeHelper::GetParent(source);
    }

    return true;
}

bool IsPointerClearOfButtons(FrameContext& ctx,
                             FrameworkElement frame,
                             Point point,
                             double padding) {
    for (auto& weak : ctx.lastButtons) {
        auto button = weak.get();
        if (!button || button.Visibility() != Visibility::Visible) {
            continue;
        }

        double width = button.ActualWidth();
        double height = button.ActualHeight();
        if (width <= 0.5 || height <= 0.5) {
            continue;
        }

        Point origin{0, 0};
        try {
            origin = button.TransformToVisual(frame).TransformPoint(origin);
        } catch (winrt::hresult_error const&) {
            continue;
        }

        if (point.X >= origin.X - padding &&
            point.X <= origin.X + width + padding &&
            point.Y >= origin.Y - padding &&
            point.Y <= origin.Y + height + padding) {
            return false;
        }
    }

    return true;
}

// From the bar's root, so tray/Start/widgets get the same clearance.
bool ProbeStripIsClear(FrameworkElement frame, Point framePt, double padding) {
    Point hostPt = framePt;
    FrameworkElement root = frame;
    try {
        hostPt = frame.TransformToVisual(nullptr).TransformPoint(framePt);
        for (auto parent = frame; parent;) {
            parent = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>();
            if (parent) {
                root = parent;
            }
        }
    } catch (winrt::hresult_error const&) {
        return true;
    }

    double offsets[3] = {-padding, 0, padding};
    for (double dx : offsets) {
        Point probe{(float)(hostPt.X + dx), hostPt.Y};
        try {
            auto hits = Media::VisualTreeHelper::FindElementsInHostCoordinates(
                probe, root);
            for (auto const& hit : hits) {
                auto element = hit.try_as<FrameworkElement>();
                if (!element) {
                    continue;
                }
                auto classNameHstring = winrt::get_class_name(element);
                std::wstring_view className(classNameHstring);
                if (className.find(L"Button") != std::wstring_view::npos ||
                    className.find(L"SystemTray") != std::wstring_view::npos ||
                    className.find(L"TaskItemThumbnail") !=
                        std::wstring_view::npos) {
                    return false;
                }
            }
        } catch (winrt::hresult_error const&) {
        }
    }

    return true;
}

// Cheap per-move half: enough to arm or clear the dwell.
bool PointerQualifiesForReveal(void* key,
                               Input::PointerRoutedEventArgs const& args) {
    if (!IsPointerOverEmptySpace(args)) {
        return false;
    }

    g_lastQualifiedKey = key;
    g_lastQualifiedFramePt = Point{0, 0};

    if (g_settings.elementPaddingPx <= 0) {
        return true;
    }

    FrameContext* ctx = GetFrameContext(key);
    FrameworkElement frame = ctx ? ctx->frame.get() : nullptr;
    if (!ctx || !frame) {
        return true;
    }

    Point framePt = args.GetCurrentPoint(frame).Position();
    if (!IsPointerClearOfButtons(*ctx, frame, framePt,
                                 g_settings.elementPaddingPx)) {
        return false;
    }

    g_lastQualifiedFramePt = framePt;
    return true;
}

// Expensive half, run once at the flip rather than per move.
bool PointerClearsSurfaces(void* key, Point framePt) {
    if (g_settings.elementPaddingPx <= 0) {
        return true;
    }

    FrameContext* ctx = GetFrameContext(key);
    FrameworkElement frame = nullptr;
    if (ctx) {
        // Fail open: any tick may resolve another thread's frame here.
        try {
            frame = ctx->frame.get();
        } catch (winrt::hresult_error const&) {
            return true;
        }
    }
    if (!frame) {
        return true;
    }

    // A different window within the padding is a seam; probes past the bar's
    // own window are the screen edge or the next monitor, not a seam.
    POINT screenPt;
    if (GetCursorPos(&screenPt)) {
        HWND hAt = WindowFromPoint(screenPt);
        if (hAt) {
            UINT dpi = GetDpiForWindow(hAt);
            int paddingPx =
                MulDiv(g_settings.elementPaddingPx, dpi ? (int)dpi : 96, 96);
            HWND hBar = GetAncestor(hAt, GA_ROOT);
            RECT barRect{};
            bool haveRect = hBar && GetWindowRect(hBar, &barRect);
            POINT probes[2] = {{screenPt.x - paddingPx, screenPt.y},
                               {screenPt.x + paddingPx, screenPt.y}};
            for (POINT const& probe : probes) {
                if (haveRect &&
                    (probe.x < barRect.left || probe.x >= barRect.right)) {
                    continue;
                }
                if (WindowFromPoint(probe) != hAt) {
                    return false;
                }
            }
        }
    }

    return ProbeStripIsClear(frame, framePt, g_settings.elementPaddingPx);
}

// QI to one interface: raw hook pointers differ per interface slot.
void* FrameKeyFromThis(void* pThis, FrameworkElement* elementOut) {
    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return nullptr;
    }

    void* key = winrt::get_abi(element);
    *elementOut = std::move(element);
    return key;
}

// Catch-all per hook: an exception across the COM ABI kills explorer.

using TaskbarFrame_OnPointerMoved_t = int(__cdecl*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;
void HandlePointerMoved(void* pThis, void* pArgs) {
    if (g_unloading) {
        return;
    }

    FrameworkElement element = nullptr;
    void* key = FrameKeyFromThis(pThis, &element);
    if (!key) {
        return;
    }

    if (!IsFrameRegistered(key)) {
        if (winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") {
            return;
        }
        RegisterFrame(key, element);
    }

    g_lastCursorInsideTick = GetTickCount64();

    RevealTrigger trigger = g_settings.revealTrigger;
    if ((trigger != RevealTrigger::Hover && trigger != RevealTrigger::Rest) ||
        !g_collapseEnabled || g_revealed) {
        return;
    }

    // Under 1 ms would divide into garbage; a gap over stale is a pause
    // whose first event has no measurement and must not arm anything.
    if (trigger == RevealTrigger::Rest) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            double nowMs = NowMs();
            double prevMs = g_speedSampleMs.load();
            double dt = nowMs - prevMs;
            if (prevMs == 0 || dt > kSpeedSampleStaleMs) {
                g_cursorSpeedPxS = 0;
                g_speedMeasured = false;
                g_speedSampleMs = nowMs;
                g_speedSamplePrevPos = pt;
                return;
            }
            if (dt >= 1.0) {
                double dx = (double)(pt.x - g_speedSamplePrevPos.x);
                double dy = (double)(pt.y - g_speedSamplePrevPos.y);
                double instant = sqrt(dx * dx + dy * dy) * 1000.0 / dt;
                // Seeded, never averaged with the reset zero.
                g_cursorSpeedPxS =
                    g_speedMeasured
                        ? (g_cursorSpeedPxS.load() + instant) * 0.5
                        : instant;
                g_speedMeasured = true;
                g_speedSampleMs = nowMs;
                g_speedSamplePrevPos = pt;
            }
        }
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return;
    }

    if (!PointerQualifiesForReveal(key, args)) {
        g_lastPointerQualified = false;
        g_emptyHoverSinceTick = 0;
        return;
    }
    g_lastPointerQualified = true;
    StartTimersOnThisThread();

    if (trigger == RevealTrigger::Rest &&
        g_cursorSpeedPxS > (double)g_settings.restSpeedPxPerSec) {
        g_emptyHoverSinceTick = 0;
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG since = g_emptyHoverSinceTick;
    if (since == 0) {
        g_emptyHoverSinceTick = now;
        since = now;
    }
    if (now - since < (ULONGLONG)g_settings.revealDelayMs) {
        return;
    }

    if (!PointerClearsSurfaces(key, g_lastQualifiedFramePt)) {
        g_lastPointerQualified = false;
        g_emptyHoverSinceTick = 0;
        return;
    }

    g_revealed = true;
    g_emptyHoverSinceTick = 0;
    ApplyOnThisThreadNow();
    // Other monitors' taskbars have no running timer to notice the flip.
    WakeAllFramesAsync();
}

int __cdecl TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    int ret = TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    try {
        HandlePointerMoved(pThis, pArgs);
    } catch (...) {
    }
    return ret;
}

// Exits bubble from children and other pointers too; a position still
// inside the frame keeps the state.
using TaskbarFrame_OnPointerExited_t = int(__cdecl*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;
void HandlePointerExited(void* pThis, void* pArgs) {
    if (g_unloading) {
        return;
    }

    bool stillInside = false;
    FrameworkElement element = nullptr;
    FrameKeyFromThis(pThis, &element);
    if (element && pArgs) {
        Input::PointerRoutedEventArgs args = nullptr;
        ((IUnknown*)pArgs)
            ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                             winrt::put_abi(args));
        if (args) {
            try {
                Point pt = args.GetCurrentPoint(element).Position();
                stillInside = pt.X >= 0 && pt.Y >= 0 &&
                              pt.X < element.ActualWidth() &&
                              pt.Y < element.ActualHeight();
            } catch (winrt::hresult_error const&) {
            }
        }
    }

    if (!stillInside) {
        g_lastPointerQualified = false;
        g_emptyHoverSinceTick = 0;
    }
}

int __cdecl TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    int ret = TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    try {
        HandlePointerExited(pThis, pArgs);
    } catch (...) {
    }
    return ret;
}

// Released, not pressed: dragging off and letting go must not count.
using TaskbarFrame_OnPointerReleased_t = int(__cdecl*)(void* pThis,
                                                       void* pArgs);
TaskbarFrame_OnPointerReleased_t TaskbarFrame_OnPointerReleased_Original;
void HandlePointerReleased(void* pThis, void* pArgs) {
    if (g_unloading || g_settings.revealTrigger != RevealTrigger::Click ||
        !g_collapseEnabled) {
        return;
    }

    FrameworkElement element = nullptr;
    void* key = FrameKeyFromThis(pThis, &element);
    if (!key || !IsFrameRegistered(key)) {
        return;
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return;
    }

    // Left button only: right-click belongs to the taskbar's context menu.
    if (args.GetCurrentPoint(nullptr).Properties().PointerUpdateKind() !=
        winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
        return;
    }

    if (!PointerQualifiesForReveal(key, args) ||
        !PointerClearsSurfaces(key, g_lastQualifiedFramePt)) {
        return;
    }

    // Atomic flip: two taskbars' release hooks can race on this.
    bool revealed = g_revealed.load();
    while (!g_revealed.compare_exchange_weak(revealed, !revealed)) {
    }
    g_emptyHoverSinceTick = 0;
    ApplyOnThisThreadNow();
    // Other monitors' taskbars have no running timer to notice the flip.
    WakeAllFramesAsync();
}

int __cdecl TaskbarFrame_OnPointerReleased_Hook(void* pThis, void* pArgs) {
    int ret = TaskbarFrame_OnPointerReleased_Original(pThis, pArgs);
    try {
        HandlePointerReleased(pThis, pArgs);
    } catch (...) {
    }
    return ret;
}

// Second discovery path, for taskbars the cursor has not visited yet.
using TaskbarFrame_MeasureOverride_t =
    int(__cdecl*)(void* pThis,
                  winrt::Windows::Foundation::Size size,
                  winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
void HandleFrameMeasure(void* pThis) {
    // Layout hot path: cheapest guard first.
    if (g_unloading) {
        return;
    }

    FrameworkElement element = nullptr;
    void* key = FrameKeyFromThis(pThis, &element);
    if (!key || IsFrameRegistered(key)) {
        return;
    }

    if (winrt::get_class_name(element) == L"Taskbar.TaskbarFrame") {
        RegisterFrame(key, element);
    }
}

int __cdecl TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    try {
        HandleFrameMeasure(pThis);
    } catch (...) {
    }
    return ret;
}

// Fired on button state changes (launch/close/active). The apply is posted,
// never inline: heavy work in the update pass risks rerendering loops.
using TaskListButton_UpdateVisualStates_t = void(__cdecl*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void __cdecl TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    g_lastButtonChurnMs = NowMs();
    if (g_unloading) {
        return;
    }
    try {
        winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
        DWORD threadId = GetCurrentThreadId();
        {
            std::lock_guard<std::mutex> guard(g_framesMutex);
            for (auto& [key, ctx] : *g_frames) {
                if (ctx.threadId == threadId && ctx.dispatcher) {
                    if (ctx.applyPosted) {
                        return;
                    }
                    ctx.applyPosted = true;
                    dispatcher = ctx.dispatcher;
                    break;
                }
            }
        }
        if (dispatcher && !PostApply(dispatcher)) {
            // Failed post: un-latch, or this thread goes deaf to events.
            std::lock_guard<std::mutex> guard(g_framesMutex);
            for (auto& [key, ctx] : *g_frames) {
                if (ctx.threadId == threadId) {
                    ctx.applyPosted = false;
                }
            }
        }
    } catch (...) {
    }
}

// -------------------------------------------------------------------- hotkey

bool ParseHotkey(std::wstring spec, UINT* modifiers, UINT* vk) {
    *modifiers = 0;
    *vk = 0;

    size_t pos = 0;
    while (pos <= spec.size()) {
        size_t plus = spec.find(L'+', pos);
        std::wstring token = spec.substr(
            pos, plus == std::wstring::npos ? std::wstring::npos : plus - pos);

        size_t first = token.find_first_not_of(L" \t");
        size_t last = token.find_last_not_of(L" \t");
        token = (first == std::wstring::npos)
                    ? L""
                    : token.substr(first, last - first + 1);
        for (auto& c : token) {
            c = towlower(c);
        }

        if (!token.empty()) {
            if (token == L"ctrl" || token == L"control") {
                *modifiers |= MOD_CONTROL;
            } else if (token == L"alt") {
                *modifiers |= MOD_ALT;
            } else if (token == L"shift") {
                *modifiers |= MOD_SHIFT;
            } else if (token == L"win") {
                *modifiers |= MOD_WIN;
            } else if (token == L"space") {
                if (*vk) {
                    return false;
                }
                *vk = VK_SPACE;
            } else if (token.size() == 1 &&
                       ((token[0] >= L'a' && token[0] <= L'z') ||
                        (token[0] >= L'0' && token[0] <= L'9'))) {
                if (*vk) {
                    return false;
                }
                *vk = towupper(token[0]);
            } else if (token[0] == L'f' && token.size() >= 2 &&
                       token.size() <= 3) {
                int n = _wtoi(token.c_str() + 1);
                if (n < 1 || n > 24 || *vk) {
                    return false;
                }
                *vk = VK_F1 + n - 1;
            } else {
                return false;
            }
        }

        if (plus == std::wstring::npos) {
            break;
        }
        pos = plus + 1;
    }

    return *vk != 0;
}

// Never waits on a taskbar thread, or unload could strand the worker.
void ToggleCollapse() {
    // Atomic flip: LoadSettings writes this concurrently on settings saves.
    bool prev = g_collapseEnabled.load();
    while (!g_collapseEnabled.compare_exchange_weak(prev, !prev)) {
    }
    bool enabled = !prev;
    g_revealed = false;
    g_revealedByStart = false;
    g_emptyHoverSinceTick = 0;
    g_lastPointerQualified = false;
    g_instantApplyGen++;
    Wh_Log(L"Hotkey toggled collapse to %d", (int)enabled);
    WakeAllFramesAsync();
}

void OnStartMenuVisibility(bool visible) {
    if (g_unloading || !g_settings.revealOnStart || !g_collapseEnabled) {
        return;
    }

    if (visible) {
        bool wasRevealed = g_revealed.exchange(true);
        RevealTrigger trigger = g_settings.revealTrigger;
        bool hoverLike = trigger == RevealTrigger::Hover ||
                         trigger == RevealTrigger::Rest;
        // Adopt an in-flight hover reveal or grace ages it out under Start;
        // a click reveal stays its own, so Start-close cannot cancel it.
        if (!wasRevealed || hoverLike) {
            g_revealedByStart = true;
        }
        if (!wasRevealed) {
            Wh_Log(L"Start menu opened, revealing");
            WakeAllFramesAsync();
        }
        return;
    }

    if (g_revealedByStart.exchange(false)) {
        // Hover hands over to grace; click/hotkey reveals end with Start.
        RevealTrigger trigger = g_settings.revealTrigger;
        bool hoverLike = trigger == RevealTrigger::Hover ||
                         trigger == RevealTrigger::Rest;
        if (hoverLike && CursorOverAnyTaskbar()) {
            g_lastCursorInsideTick = GetTickCount64();
        } else {
            g_revealed = false;
        }
        Wh_Log(L"Start menu closed");
        WakeAllFramesAsync();
    }
}

// Callbacks arrive on COM worker threads; only thread-safe state here.
class StartVisibilitySink final : public IAppVisibilityEvents {
   public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        if (!ppvObject) {
            return E_POINTER;
        }
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IAppVisibilityEvents)) {
            *ppvObject = static_cast<IAppVisibilityEvents*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = (ULONG)InterlockedDecrement(&m_refCount);
        if (remaining == 0) {
            delete this;
        }
        return remaining;
    }

    HRESULT STDMETHODCALLTYPE
    AppVisibilityOnMonitorChanged(HMONITOR,
                                  MONITOR_APP_VISIBILITY,
                                  MONITOR_APP_VISIBILITY) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    LauncherVisibilityChange(BOOL currentVisibleState) override {
        // Counted so teardown can wait out a call in flight.
        g_sinkCallbacks++;
        try {
            OnStartMenuVisibility(currentVisibleState != FALSE);
        } catch (...) {
        }
        g_sinkCallbacks--;
        return S_OK;
    }

   private:
    LONG m_refCount = 1;
};

DWORD WINAPI HotkeyThreadProc(LPVOID param) {
    // Create and announce the queue before anything slow: WM_QUIT must land.
    MSG queuePrimer;
    PeekMessage(&queuePrimer, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_hotkeyThreadReady);

    // A raw thread entry: an escaping exception would terminate explorer.
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (winrt::hresult_error const&) {
        Wh_Log(L"init_apartment failed, hotkey thread not running");
        return 0;
    }

    ULONG_PTR packed = (ULONG_PTR)param;
    UINT modifiers = (UINT)(packed >> 16);
    UINT vk = (UINT)(packed & 0xFFFF);

    bool hotkeyRegistered =
        vk != 0 && RegisterHotKey(nullptr, 1, modifiers | MOD_NOREPEAT, vk);
    if (vk != 0 && !hotkeyRegistered) {
        Wh_Log(L"RegisterHotKey failed, error %u", GetLastError());
    }

    winrt::com_ptr<IAppVisibility> appVisibility;
    DWORD adviseCookie = 0;
    StartVisibilitySink* sink = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(AppVisibility), nullptr,
                                   CLSCTX_ALL,
                                   IID_PPV_ARGS(appVisibility.put())))) {
        sink = new StartVisibilitySink();
        if (FAILED(appVisibility->Advise(sink, &adviseCookie))) {
            adviseCookie = 0;
            Wh_Log(L"IAppVisibility::Advise failed");
        }
    } else {
        Wh_Log(L"AppVisibility unavailable, start menu reveal inactive");
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == 1) {
            ToggleCollapse();
        }
    }

    if (appVisibility && adviseCookie) {
        appVisibility->Unadvise(adviseCookie);
    }
    // A callback that slipped past Unadvise may still be running.
    while (g_sinkCallbacks.load() > 0) {
        Sleep(1);
    }
    if (sink) {
        sink->Release();
    }
    appVisibility = nullptr;
    if (hotkeyRegistered) {
        UnregisterHotKey(nullptr, 1);
    }

    winrt::uninit_apartment();
    return 0;
}

void StartHotkeyThread() {
    std::lock_guard<std::mutex> guard(g_hotkeyThreadMutex);
    if (g_hotkeyThread) {
        return;
    }

    auto spec = WindhawkUtils::StringSetting::make(L"Hotkey");
    UINT modifiers = 0;
    UINT vk = 0;
    bool valid = ParseHotkey(spec.get(), &modifiers, &vk);

    if (!valid) {
        if (*spec.get()) {
            Wh_Log(L"Hotkey spec not understood, hotkey disabled");
        }
        modifiers = 0;
        vk = 0;
    }
    if (!valid && !g_settings.revealOnStart) {
        return;
    }

    g_hotkeyThreadReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hotkeyThreadReady) {
        return;
    }

    ULONG_PTR packed = ((ULONG_PTR)modifiers << 16) | vk;
    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, (LPVOID)packed,
                                  0, &g_hotkeyThreadId);
    if (!g_hotkeyThread) {
        CloseHandle(g_hotkeyThreadReady);
        g_hotkeyThreadReady = nullptr;
    }
}

// Unbounded on purpose: a surviving thread would resume in a freed image.
void StopHotkeyThread() {
    std::lock_guard<std::mutex> guard(g_hotkeyThreadMutex);
    if (!g_hotkeyThread) {
        return;
    }

    WaitForSingleObject(g_hotkeyThreadReady, INFINITE);
    PostThreadMessage(g_hotkeyThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hotkeyThread, INFINITE);

    CloseHandle(g_hotkeyThread);
    CloseHandle(g_hotkeyThreadReady);
    g_hotkeyThread = nullptr;
    g_hotkeyThreadReady = nullptr;
    g_hotkeyThreadId = 0;
}

// ------------------------------------------------------- taskbar UI threads

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

// Do not call holding g_framesMutex: blocks on each target thread.
void RunOnAllTaskbarThreads(RunFromWindowThreadProc_t proc, PVOID param) {
    std::vector<HWND> windows;
    std::vector<DWORD> threadIds;

    auto consider = [&](HWND hWnd) {
        DWORD processId = 0;
        DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
        if (!threadId || processId != GetCurrentProcessId()) {
            return;
        }
        for (DWORD seen : threadIds) {
            if (seen == threadId) {
                return;
            }
        }
        threadIds.push_back(threadId);
        windows.push_back(hWnd);
    };

    // Iterated: the first Shell_TrayWnd can belong to another process.
    HWND hPrimary = nullptr;
    while ((hPrimary = FindWindowEx(nullptr, hPrimary, L"Shell_TrayWnd",
                                    nullptr))) {
        consider(hPrimary);
    }
    HWND hSecondary = nullptr;
    while ((hSecondary = FindWindowEx(nullptr, hSecondary,
                                      L"Shell_SecondaryTrayWnd", nullptr))) {
        consider(hSecondary);
    }

    for (HWND hWnd : windows) {
        RunFromWindowThread(hWnd, proc, param);
    }
}

void CleanupOnThisThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<void*> keys;

    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.threadId == threadId) {
                keys.push_back(key);
            }
        }
    }

    std::lock_guard<std::mutex> guard(g_framesMutex);
    for (void* key : keys) {
        auto it = g_frames->find(key);
        if (it == g_frames->end()) {
            continue;
        }

        // Runs inside a CALLWNDPROC hook or dispatcher lambda; an escaping
        // throw is fatal or hangs unload. The erase must always run.
        try {
            if (it->second.timer) {
                it->second.timer.Stop();
                it->second.timer.Tick(it->second.tickToken);
            }
            RestoreFrame(it->second);
        } catch (winrt::hresult_error const&) {
        }
        g_frames->erase(it);
    }
}

// ------------------------------------------------------------------ lifetime

void LoadSettings() {
    g_settings.revealOnStart = Wh_GetIntSetting(L"RevealOnStart") != 0;

    auto trigger = WindhawkUtils::StringSetting::make(L"RevealTrigger");
    g_settings.revealTrigger = RevealTrigger::Click;
    if (wcscmp(trigger.get(), L"hover") == 0) {
        g_settings.revealTrigger = RevealTrigger::Hover;
    } else if (wcscmp(trigger.get(), L"rest") == 0) {
        g_settings.revealTrigger = RevealTrigger::Rest;
    } else if (wcscmp(trigger.get(), L"never") == 0) {
        g_settings.revealTrigger = RevealTrigger::Never;
    }

    g_settings.restSpeedPxPerSec =
        std::clamp(Wh_GetIntSetting(L"RestSpeedPxPerSec"), 10, 10000);

    g_settings.revealDelayMs =
        std::clamp(Wh_GetIntSetting(L"RevealDelayMs"), 0, 5000);
    g_settings.elementPaddingPx =
        std::clamp(Wh_GetIntSetting(L"ElementPaddingPx"), 0, 200);
    g_settings.hoverGraceMs =
        std::clamp(Wh_GetIntSetting(L"HoverGraceMs"), 0, 5000);

    g_settings.animationDurationMs =
        std::clamp(Wh_GetIntSetting(L"AnimationDurationMs"), 0, 5000);
    g_settings.animationAmplitudePct =
        std::clamp(Wh_GetIntSetting(L"AnimationAmplitudePct"), 0, 200);

    auto mode = WindhawkUtils::StringSetting::make(L"AnimationMode");
    g_settings.animationMode = wcscmp(mode.get(), L"spacing") == 0
                                   ? AnimationMode::Spacing
                                   : AnimationMode::None;

    auto curve = WindhawkUtils::StringSetting::make(L"AnimationCurve");
    g_settings.animationCurve = CurveId::Sine;
    if (wcscmp(curve.get(), L"cubic") == 0) {
        g_settings.animationCurve = CurveId::Cubic;
    } else if (wcscmp(curve.get(), L"circ") == 0) {
        g_settings.animationCurve = CurveId::Circ;
    }

    g_collapseEnabled = Wh_GetIntSetting(L"Collapsed") != 0;
    g_revealed = false;
    g_revealedByStart = false;
    g_emptyHoverSinceTick = 0;
    g_lastPointerQualified = false;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerReleased(void *))"},
            &TaskbarFrame_OnPointerReleased_Original,
            TaskbarFrame_OnPointerReleased_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
            true,
        },
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

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed, mod will not load");
        return false;
    }

    Wh_Log(
        L"HookSymbols ok (OnPointerMoved=%d OnPointerReleased=%d "
        L"OnPointerExited=%d MeasureOverride=%d)",
        (int)(TaskbarFrame_OnPointerMoved_Original != nullptr),
        (int)(TaskbarFrame_OnPointerReleased_Original != nullptr),
        (int)(TaskbarFrame_OnPointerExited_Original != nullptr),
        (int)(TaskbarFrame_MeasureOverride_Original != nullptr));
    if (!TaskbarFrame_OnPointerReleased_Original) {
        Wh_Log(L"OnPointerReleased hook missing, click reveal will not work");
    }
    return true;
}

// Catches a Taskbar.View.dll that loads after this mod does.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        // The exchange makes this exactly-once; no second flag needed.
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
            StartHotkeyThread();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
        // Only the shell explorer loads Taskbar.View.dll; other instances
        // must not grab the hotkey or the Start watcher.
        StartHotkeyThread();
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        if (!kernelBaseModule) {
            Wh_Log(L"kernelbase.dll not found");
            return FALSE;
        }
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))
            GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW) {
            Wh_Log(L"LoadLibraryExW not found in kernelbase.dll");
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_taskbarViewDllLoaded) {
        return;
    }

    // Retry for a load that raced the LoadLibraryExW hook going live.
    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule && !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_ApplyHookOperations();
            StartHotkeyThread();
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    StopHotkeyThread();
    LoadSettings();
    if (g_taskbarViewDllLoaded) {
        StartHotkeyThread();
    }

    g_instantApplyGen++;
    // The timers are stopped while idle, so the new settings must be pushed.
    WakeAllFramesAsync();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;
    StopHotkeyThread();

    RunOnAllTaskbarThreads([](PVOID) { CleanupOnThisThread(); }, nullptr);

    // Second sweep: every timer must stop on its own thread.
    std::vector<DWORD> leftoverThreads;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            leftoverThreads.push_back(ctx.threadId);
        }
    }
    for (DWORD threadId : leftoverThreads) {
        HWND hAny = nullptr;
        EnumThreadWindows(
            threadId,
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                *(HWND*)lParam = hWnd;
                return FALSE;
            },
            (LPARAM)&hAny);
        if (hAny) {
            RunFromWindowThread(
                hAny, [](PVOID) { CleanupOnThisThread(); }, nullptr);
        }
    }

    // Last resort: the context's dispatcher cleans up on its own thread.
    std::vector<std::pair<winrt::Windows::UI::Core::CoreDispatcher, DWORD>>
        stragglers;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.dispatcher) {
                stragglers.emplace_back(ctx.dispatcher, ctx.threadId);
            }
        }
    }

    for (auto& [dispatcher, threadId] : stragglers) {
        HANDLE hThread = OpenThread(SYNCHRONIZE, FALSE, threadId);
        if (!hThread) {
            continue;
        }
        if (WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0) {
            // Already gone: nothing on that thread can ever run the lambda.
            CloseHandle(hThread);
            continue;
        }
        HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (!done) {
            CloseHandle(hThread);
            continue;
        }
        bool queued = false;
        {
            // Signalled on delegate DESTRUCTION, even if dropped uninvoked.
            auto signal = std::shared_ptr<void>(
                nullptr, [done](void*) { SetEvent(done); });
            try {
                dispatcher.RunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                    [signal]() {
                        try {
                            CleanupOnThisThread();
                        } catch (...) {
                        }
                    });
                queued = true;
            } catch (winrt::hresult_error const&) {
            }
        }
        if (queued) {
            // Unbounded: giving up lets the lambda run in a freed image. On
            // thread death the event is deliberately leaked (delegate owns it).
            HANDLE waits[2] = {done, hThread};
            if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) ==
                WAIT_OBJECT_0) {
                CloseHandle(done);
            }
        } else {
            CloseHandle(done);
        }
        CloseHandle(hThread);
    }

    // Lifetime-tied counter: a dropped wake still counts down. Unbounded.
    while (g_pendingWakes.load() > 0) {
        Sleep(10);
    }

    // Leftovers stay referenced (safer than a wrong-thread release). Never
    // reset() the map: hooks still run unlocked g_unloading checks.
    std::lock_guard<std::mutex> guard(g_framesMutex);
    if (!g_frames->empty()) {
        Wh_Log(L"%u taskbar contexts could not be cleaned up",
               (unsigned)g_frames->size());
    }
}

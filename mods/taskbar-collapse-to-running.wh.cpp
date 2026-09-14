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

*Hovering effect not included. Check out "Taskbar Dock Animation" by Ph0en1x-dev!

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
* Built with Claude and tested on Windows 11 build 26200.
* Thanks to https://easings.net/ for the easing curve values.
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
    How long the cursor must stay in the empty taskbar area before the icons
    come back (only applies to: Reveal trigger > Hover / Rest)
- RestSpeedPxPerSec: 300
  $name: Rest speed (px/s)
  $description: >-
    How slow the cursor must be moving across empty taskbar area to count as resting (only applies to: Reveal trigger > Rest)
- Collapsed: true
  $name: Collapse taskbar
  $description: >-
    Hide pinned icons whose app is not running. The hotkey flips the same
    switch, but only until settings are saved again, which restores this box.
- ElementPaddingPx: 12
  $name: Padding around elements (px)
  $description: >-
    Safe area around every taskbar element to prevent unwanted transitions.
- HoverGraceMs: 150
  $name: Grace period (ms)
  $description: >-
    How long the cursor may be off the taskbar before it collapses again
    (only applies to: Reveal trigger > Hover / Rest)
- AnimationMode: slide
  $name: Reveal animation
  $description: >-
    How revealing and collapsing are animated. Slide moves the icons to their
    new spots; None flips them instantly.
  $options:
  - none: None
  - slide: Slide
- AnimationCurve: circ
  $name: Easing curve
  $options:
  - sine: easeInOutSine
  - cubic: easeInOutCubic
  - circ: easeInOutCirc
- AnimationDurationMs: 160
  $name: Animation length (ms)
- SnappinessPct: 40
  $name: Snappiness (%)
  $description: >-
    How much of the motion the mid-transition cut takes. The icons travel to
    their new spots, and this share of that journey is skipped at the cut,
    where the icons swapping hides the jump. 0 is fully smooth, 100 is
    maximum snap.
- Hotkey: Ctrl+Alt+T
  $name: Toggle hotkey
  $description: Modifiers Ctrl, Alt, Shift, Win plus one of A-Z, 0-9, F1-F24, Space. Leave empty for no hotkey.
- DiagLog: false
  $name: Diagnostic log
  $description: >-
    Write the animation engine's internal state to the Windhawk log on every
    run. Very verbose; only for bug reports.
- DiagStray: false
  $name: Stray highlight log
  $description: >-
    Log each stray highlight box the mod removes, and any that slips through
    mid-run. Silent while nothing is wrong.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shobjidl.h>

// winbase.h's GetCurrentTime macro collides with the XAML animation headers.
#undef GetCurrentTime

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.h>
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
void WakeAllFramesAsync(bool skipCurrentThread = false);
void StartHotkeyThread();
void NudgeTaskbarsForDiscovery();
bool PostApply(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher);

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

enum class AnimationMode { None, Slide };
enum class RevealTrigger { Never, Hover, Rest, Click };

// GapClose: hide an icon instantly, hold its gap open a beat, then ease the
// row shut.
enum class AnimKind { Accordion, GapClose };

// How long a closed icon's gap holds before easing shut: enough for
// Windows' close churn to pass, short enough to read as one motion.
constexpr double kGapHoldMs = 250.0;

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
    std::atomic<int> snappinessPct;
} g_settings;

// Diagnostic output, off unless its setting is on. Argument evaluation is
// skipped too, so a disabled line costs nothing.
std::atomic<bool> g_diagLog{false};
std::atomic<bool> g_diagStray{false};
#define DIAG_LOG(...)                    \
    do {                                 \
        if (g_diagLog.load()) {          \
            Wh_Log(__VA_ARGS__);         \
        }                                \
    } while (0)
#define STRAY_LOG(...)                   \
    do {                                 \
        if (g_diagStray.load()) {        \
            Wh_Log(__VA_ARGS__);         \
        }                                \
    } while (0)

std::atomic<bool> g_unloading = false;
std::atomic<bool> g_taskbarViewDllLoaded = false;

std::atomic<bool> g_collapseEnabled = true;

std::atomic<bool> g_revealed = false;

// Reveal caused by Start opening; pinned against grace aging until it closes.
std::atomic<bool> g_revealedByStart = false;

// Sticky reveal survives explorer restarts. Start-pinned reveals do not
// count, and a restored hover reveal just ages out on grace.
std::atomic<int> g_lastSavedRevealed = -1;

void PersistRevealState() {
    int value = g_revealed && !g_revealedByStart ? 1 : 0;
    if (g_lastSavedRevealed.exchange(value) != value) {
        Wh_SetIntValue(L"RevealedState", value);
    }
}

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

// The slide: one TranslateTransform per strip element, riding in the
// element's RenderTransform. Never UIElement.Translation: Windows animates
// that facade itself, and on a button where Windows has also enabled the
// composition translation channel, every zero crossing of the facade tears
// XAML's transform expressions down and back up. One lost race there draws
// the icon a slot off until Explorer restarts.
struct SlideEntry {
    winrt::weak_ref<FrameworkElement> element;
    Media::TranslateTransform transform{nullptr};
    // Container the transform rides in. Taskbar Dock Animation keeps its own
    // TransformGroup here, so ours is appended to a foreign group rather than
    // replacing it; a plain foreign transform gets wrapped in a group of ours.
    Media::TransformGroup group{nullptr};
    Media::Transform wrapped{nullptr};
    bool groupOurs = false;
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
    // Abandoned containers hidden on sight. Tracked so the judgement is
    // never permanent: one that turns out to be a real button gets shown
    // again the moment it has an icon.
    std::vector<winrt::weak_ref<FrameworkElement>> phantomHidden;
    std::vector<winrt::weak_ref<FrameworkElement>> lastButtons;
    std::vector<DeanimatedButton> deanimated;
    std::vector<SlideEntry> slides;

    // Visibility state currently on screen; -1 until the first apply.
    int appliedCollapse = -1;
    // When a collapse last landed hides; arms the ghost guard for a beat.
    double collapseLandedMs = 0;
    // Diag: at-rest row sample due after a run ends.
    double diagSampleMs = 0;
    bool animActive = false;
    AnimKind animKind = AnimKind::Accordion;
    bool animTargetCollapse = false;
    double animStartMs = 0;
    // Accordion playback table, computed before the first frame renders:
    // start and destination for every strip element, fold order per phase.
    // The tick only interpolates; reality diverging from the table kills
    // the run and lands the target instantly.
    struct PlanEntry {
        FrameworkElement element{nullptr};
        double startX = 0;
        double finalX = 0;
        // On the bar per phase (folds/renders). Windows parks overflowed
        // icons off-screen instead of hiding them, so these are not raw
        // Visibility; actual Visibility is tracked separately for the
        // divergence guard.
        bool visibleBefore = false;
        bool visibleAfter = false;
        bool visiblePhase1 = false;
        // Off the bar in both phases — in the overflow throughout, so it has
        // no spot to animate to or from. Left alone: never folded, never
        // counted, never translated.
        bool animate = true;
    };
    std::vector<PlanEntry> animPlan;
    // Strip child count when the run began; a change is divergence.
    int animChildCount = -1;
    bool animSwapped = false;
    // Collapse run ended; the real hides land on the next dispatcher pass.
    bool animFinalizePending = false;
    std::vector<float> animGapOffsets;
    struct GapHidden {
        FrameworkElement element{nullptr};
        double opacity = 1;
        double maxWidth = 0;
    };
    // GapClose: collapsed at start, held transparent and widthless until the
    // run ends, so a re-show by Windows neither draws nor takes space. The
    // accordion reveal reuses it for appearing icons held transparent until
    // the apex; StopAnimation restores whatever is left either way.
    std::vector<GapHidden> gapHidden;
    std::vector<FrameworkElement> animButtons;
    winrt::event_token renderingToken{};
    bool renderingHooked = false;
    // Coalesces UpdateVisualStates bursts; cleared when the apply runs.
    bool applyPosted = false;
    // Retry budget for an apply that found no buttons (taskbar mid-rebuild).
    // Armed once per generation, or a buttonless frame re-arms forever.
    int applyRetries = 0;
    uint64_t retriesArmedGen = std::numeric_limits<uint64_t>::max();
    uint64_t instantGenSeen = 0;
};

std::mutex g_framesMutex;

// Never destroyed: CRT shutdown would release XAML from the wrong thread.
[[clang::no_destroy]] std::optional<std::unordered_map<void*, FrameContext>>
    g_frames{std::in_place};

FrameContext* GetFrameContext(void* key);
bool HasVisibleIcon(FrameworkElement root, int depth);
int HidePhantomButtons(FrameContext& ctx,
                       FrameworkElement repeater,
                       FrameworkElement frame,
                       PCWSTR where);

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
    if (!GetClassNameW(hRoot, className, ARRAYSIZE(className))) {
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

    if (transitions || (visual && implicit_)) {
        try {
            DIAG_LOG(L"[diag] strip T=%d I=%d %s", transitions ? 1 : 0,
                   (visual && implicit_) ? 1 : 0,
                   winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(button).c_str());
        } catch (winrt::hresult_error const&) {
        }
    }
    if (transitions) {
        button.Transitions(nullptr);
    }
    if (visual && implicit_) {
        visual.ImplicitAnimations(nullptr);
    }
}

// Windows' own show/hide and reposition animations fight our choreography,
// so they come off for the length of a run and go back on when it ends.
// Held only that long on purpose: a container Windows is animating out gets
// parked off-screen by the end of that animation, and killing it permanently
// strands the container in the row, visible and half-built.
void DeanimateRunSet(FrameContext& ctx,
                     std::vector<FrameworkElement> const& buttons) {
    for (auto const& button : buttons) {
        DeanimateButton(ctx, button);
    }
    // Start and search recentre with the row, so they go still too.
    if (auto repeater = ctx.repeater.get()) {
        EnumChildElements(repeater, [&](FrameworkElement child) {
            DeanimateButton(ctx, child);
            return false;
        });
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

// Identity, not interface pointer: the same object answers with a different
// pointer per interface.
bool SameObject(IInspectable const& a, IInspectable const& b) {
    if (!a || !b) {
        return !a && !b;
    }
    return a.as<winrt::Windows::Foundation::IUnknown>() ==
           b.as<winrt::Windows::Foundation::IUnknown>();
}

SlideEntry* FindSlide(FrameContext& ctx, FrameworkElement const& element) {
    SlideEntry* found = nullptr;
    for (size_t i = 0; i < ctx.slides.size();) {
        auto resolved = ctx.slides[i].element.get();
        if (!resolved) {
            ctx.slides.erase(ctx.slides.begin() + i);
            continue;
        }
        if (resolved == element) {
            found = &ctx.slides[i];
        }
        i++;
    }
    return found;
}

// Seats the slide transform where XAML reads it. Re-checked on every write:
// another mod can replace RenderTransform at any time (Taskbar Dock
// Animation does), and a write into a stranded transform moves nothing.
SlideEntry* EnsureSlide(FrameContext& ctx, FrameworkElement const& element) {
    SlideEntry* entry = FindSlide(ctx, element);
    if (!entry) {
        SlideEntry fresh;
        fresh.element = winrt::make_weak(element);
        fresh.transform = Media::TranslateTransform();
        ctx.slides.push_back(std::move(fresh));
        entry = &ctx.slides.back();
    }
    auto current = element.RenderTransform();
    bool attached = entry->group ? SameObject(current, entry->group)
                                 : SameObject(current, entry->transform);
    if (attached) {
        return entry;
    }
    entry->group = nullptr;
    entry->wrapped = nullptr;
    entry->groupOurs = false;
    if (!current) {
        element.RenderTransform(entry->transform);
    } else if (auto group = current.try_as<Media::TransformGroup>()) {
        group.Children().Append(entry->transform);
        entry->group = group;
    } else {
        auto wrapper = Media::TransformGroup();
        wrapper.Children().Append(current);
        wrapper.Children().Append(entry->transform);
        element.RenderTransform(wrapper);
        entry->group = wrapper;
        entry->wrapped = current;
        entry->groupOurs = true;
    }
    return entry;
}

// The only writer of the slide. A zero for an element never slid is a no-op,
// so landings and sweeps do not seat transforms on untouched buttons.
void SetSlideX(FrameContext& ctx, FrameworkElement const& element, double x) {
    try {
        if (x == 0 && !FindSlide(ctx, element)) {
            return;
        }
        EnsureSlide(ctx, element)->transform.X(x);
    } catch (winrt::hresult_error const&) {
    }
}

double GetSlideX(FrameContext& ctx, FrameworkElement const& element) {
    try {
        if (auto entry = FindSlide(ctx, element)) {
            return entry->transform.X();
        }
    } catch (winrt::hresult_error const&) {
    }
    return 0;
}

// Undoes every seat: nothing of the mod's may stay in a RenderTransform.
void DetachSlides(FrameContext& ctx) {
    for (auto& entry : ctx.slides) {
        auto element = entry.element.get();
        if (!element) {
            continue;
        }
        try {
            entry.transform.X(0);
            auto current = element.RenderTransform();
            if (entry.group) {
                if (SameObject(current, entry.group)) {
                    if (entry.groupOurs) {
                        element.RenderTransform(entry.wrapped);
                    } else {
                        auto children = entry.group.Children();
                        uint32_t index = 0;
                        if (children.IndexOf(entry.transform, index)) {
                            children.RemoveAt(index);
                        }
                    }
                }
            } else if (SameObject(current, entry.transform)) {
                element.RenderTransform(nullptr);
            }
        } catch (winrt::hresult_error const&) {
        }
    }
    ctx.slides.clear();
}

// A parked overflow container sits far off the bar. Windows' domain: its
// running state is the husk's, not the app's, so it is never judged.
bool IsParkedOffBar(FrameContext& ctx, FrameworkElement const& button) {
    auto frame = ctx.frame.get();
    if (!frame) {
        return false;
    }
    try {
        double x =
            button.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
        double frameWidth = frame.ActualWidth();
        return x < -1 || (frameWidth > 0 && x > frameWidth);
    } catch (winrt::hresult_error const&) {
        return false;
    }
}

// With deferHide set, hides are only faded; CollapseDeferredHides lands them.
void SetCollapseState(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    bool collapse,
    std::vector<std::pair<FrameworkElement, double>>* deferHide = nullptr) {
    int diagHid = 0;
    int diagDeferred = 0;
    int diagShown = 0;
    for (auto const& button : buttons) {
        bool weHidIt = TakeFromHiddenSet(ctx.hiddenByUs, button, false);
        bool running = TaskListButton_IsRunning(button);
        bool shouldHide = collapse && !running;

        if (shouldHide && IsParkedOffBar(ctx, button)) {
            continue;
        }

        if (shouldHide) {
            // Windows' own hides stay unrecorded; recorded ones re-assert.
            if (button.Visibility() == Visibility::Visible) {
                if (deferHide) {
                    deferHide->emplace_back(button, button.Opacity());
                    button.Opacity(0);
                    diagDeferred++;
                } else {
                    button.Visibility(Visibility::Collapsed);
                    if (!weHidIt) {
                        ctx.hiddenByUs.push_back(winrt::make_weak(button));
                    }
                    diagHid++;
                }
            }
        } else if (weHidIt) {
            // No icon and not running: a container Windows retired after
            // we hid it. Shown, it draws its leftover plate over the icon
            // in that slot now. Record kept; it returns once it runs.
            if (!running && !HasVisibleIcon(button, 3)) {
                if (!collapse && ctx.appliedCollapse == 1) {
                    try {
                        STRAY_LOG(L"Kept hidden (no icon): %s",
                                  winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(button).c_str());
                    } catch (winrt::hresult_error const&) {
                    }
                }
            } else {
                button.Visibility(Visibility::Visible);
                TakeFromHiddenSet(ctx.hiddenByUs, button, true);
                diagShown++;
            }
        }
    }
    if (diagHid || diagDeferred || diagShown) {
        DIAG_LOG(L"[diag] SetCollapseState collapse=%d hid=%d deferred=%d "
               L"shown=%d",
               (int)collapse, diagHid, diagDeferred, diagShown);
    }

    // Fresh hides free space, and Windows' overflow drain follows; arm the
    // ghost guard so a surfacing non-running icon dies before it renders.
    if (collapse && (diagHid > 0 || diagDeferred > 0)) {
        ctx.collapseLandedMs = NowMs();
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

// Lands the deferred hides for real, kept transparent until the run ends so
// a re-show by Windows renders empty instead of flashing.
void CollapseDeferredHides(
    FrameContext& ctx,
    std::vector<std::pair<FrameworkElement, double>>& deferHide) {
    for (auto& entry : deferHide) {
        try {
            if (entry.first.Visibility() == Visibility::Visible) {
                entry.first.Visibility(Visibility::Collapsed);
                if (!TakeFromHiddenSet(ctx.hiddenByUs, entry.first, false)) {
                    ctx.hiddenByUs.push_back(winrt::make_weak(entry.first));
                }
                FrameContext::GapHidden held;
                held.element = entry.first;
                held.opacity = entry.second;
                held.maxWidth = entry.first.MaxWidth();
                entry.first.MaxWidth(0);
                ctx.gapHidden.push_back(std::move(held));
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
    if (ctx.animActive) {
        DIAG_LOG(L"[diag] StopAnimation kind=%d target=%d", (int)ctx.animKind,
               (int)ctx.animTargetCollapse);
        if (g_diagLog.load()) {
            ctx.diagSampleMs = NowMs() + 400.0;
        }
    }
    // Collapsed since the start, so these restores are invisible here.
    for (auto& entry : ctx.gapHidden) {
        try {
            entry.element.MaxWidth(entry.maxWidth);
            entry.element.Opacity(entry.opacity);
        } catch (winrt::hresult_error const&) {
        }
    }
    ctx.gapHidden.clear();
    ctx.animActive = false;
    ctx.animSwapped = false;
    ctx.animFinalizePending = false;
    UnhookRendering(ctx);
    // Per element: one dead element must not strand the rest slid.
    for (auto const& button : ctx.animButtons) {
        SetSlideX(ctx, button, 0);
    }
    for (auto& entry : ctx.animPlan) {
        SetSlideX(ctx, entry.element, 0);
    }
    ctx.animPlan.clear();
    ctx.animChildCount = -1;
    ctx.animButtons.clear();
    ctx.animGapOffsets.clear();
}

// The first visible icon image inside a button: the glyph the eye tracks.
FrameworkElement FindVisibleIcon(FrameworkElement root, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    FrameworkElement found = nullptr;
    EnumChildElements(root, [&](FrameworkElement child) {
        try {
            if (child.Visibility() == Visibility::Visible &&
                winrt::get_class_name(child) ==
                    L"Windows.UI.Xaml.Controls.Image") {
                found = child;
                return true;
            }
        } catch (winrt::hresult_error const&) {
        }
        found = FindVisibleIcon(child, depth - 1);
        return found != nullptr;
    });
    return found;
}

// The promoted-slot divider: a hairline rectangle spanning the button.
FrameworkElement FindDividerRect(FrameworkElement root, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    FrameworkElement found = nullptr;
    EnumChildElements(root, [&](FrameworkElement child) {
        try {
            if (child.Visibility() == Visibility::Visible &&
                child.ActualWidth() < 2.0 && child.ActualHeight() >= 24.0 &&
                winrt::get_class_name(child) ==
                    L"Windows.UI.Xaml.Shapes.Rectangle") {
                found = child;
                return true;
            }
        } catch (winrt::hresult_error const&) {
        }
        found = FindDividerRect(child, depth - 1);
        return found != nullptr;
    });
    return found;
}

int CountStripChildren(FrameContext& ctx) {
    auto repeater = ctx.repeater.get();
    if (!repeater) {
        return -1;
    }
    int count = 0;
    EnumChildElements(repeater, [&](FrameworkElement) {
        count++;
        return false;
    });
    return count;
}

// Computes the whole run before the first frame renders: flip to the
// destination, lay out, measure, flip back — invisible, all in this pass.
// Elements visible on only one side get the missing coordinate from their
// measured neighbours' displacement, interpolated once, here. The tick
// then only plays the table back.
bool BuildAnimPlan(FrameContext& ctx,
                   FrameworkElement frame,
                   bool targetCollapse) {
    ctx.animPlan.clear();
    ctx.animChildCount = CountStripChildren(ctx);

    std::vector<FrameworkElement> elements;
    if (auto repeater = ctx.repeater.get()) {
        EnumChildElements(repeater, [&](FrameworkElement child) {
            elements.push_back(child);
            return false;
        });
    }
    if (elements.empty()) {
        elements = ctx.animButtons;
        ctx.animChildCount = (int)elements.size();
    }

    double frameWidth = 0;
    try {
        frameWidth = frame.ActualWidth();
    } catch (winrt::hresult_error const&) {
    }

    // On the bar: visible and inside the strip. An icon the row pushed past
    // its edge is parked off-screen, still Visible, so it counts as absent —
    // there is no spot on screen to animate it to or from. x is written
    // either way, and overwritten by the synthesis below when it is needed.
    auto measure = [&](FrameworkElement const& element, double* x) {
        if (element.Visibility() != Visibility::Visible) {
            return false;
        }
        try {
            *x = element.TransformToVisual(frame)
                     .TransformPoint(Point{0, 0})
                     .X;
            double width = element.ActualWidth();
            bool onBar = frameWidth <= 0 ||
                         (*x >= -1 && *x + width <= frameWidth + 1);
            // The divider visual state widens the button beside the chevron
            // and shifts its glyph inside, so the button edge lies about
            // where the icon is. Track the glyph itself when there is one.
            if (onBar) {
                if (auto icon = FindVisibleIcon(element, 3)) {
                    *x = icon.TransformToVisual(frame)
                             .TransformPoint(Point{0, 0})
                             .X;
                }
            }
            return onBar;
        } catch (winrt::hresult_error const&) {
            return false;
        }
    };

    DIAG_LOG(L"[diag] plan target=%d frameWidth=%.1f children=%d elements=%d",
           (int)targetCollapse, frameWidth, ctx.animChildCount,
           (int)elements.size());
    int diagIndex = 0;
    for (auto const& element : elements) {
        FrameContext::PlanEntry entry;
        entry.element = element;
        entry.visibleBefore = measure(element, &entry.startX);
        try {
            float3 diagOff{0, 0, 0};
            float diagM41 = 0;
            if (auto visual = g_diagLog.load()
                                  ? Hosting::ElementCompositionPreview::
                                        GetElementVisual(element)
                                  : nullptr) {
                diagOff = visual.Offset();
                diagM41 = visual.TransformMatrix().m41;
            }
            DIAG_LOG(L"[diag] %d before onBar=%d x=%.1f w=%.1f off=%.1f "
                   L"m41=%.1f %s",
                   diagIndex, (int)entry.visibleBefore, entry.startX,
                   element.ActualWidth(), diagOff.x, diagM41,
                   winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(element).c_str());
        } catch (winrt::hresult_error const&) {
        }
        diagIndex++;
        ctx.animPlan.push_back(std::move(entry));
    }

    bool priorCollapse = ctx.appliedCollapse == 1;
    SetCollapseState(ctx, ctx.animButtons, targetCollapse);
    // Overflow membership cascades: hiding icons can free enough room that
    // Windows pulls others back out of the overflow menu, which re-runs
    // layout again. One pass measures a half-settled row, so the slide
    // animates to spots Windows then overrules. Settle it fully first.
    for (int pass = 0; pass < 3; pass++) {
        frame.UpdateLayout();
    }
    // The flip can seat a retired container back in the row, on top of a
    // real icon; the pre-plan check ran while it was still collapsed.
    if (auto repeater = ctx.repeater.get()) {
        if (HidePhantomButtons(ctx, repeater, frame, L"plan") > 0) {
            frame.UpdateLayout();
        }
    }
    try {
        DIAG_LOG(L"[diag] after layout: children=%d frameWidth=%.1f",
               CountStripChildren(ctx), frame.ActualWidth());
    } catch (winrt::hresult_error const&) {
    }
    diagIndex = 0;
    for (auto& entry : ctx.animPlan) {
        entry.visibleAfter = measure(entry.element, &entry.finalX);
        entry.animate = entry.visibleBefore || entry.visibleAfter;
        try {
            DIAG_LOG(L"[diag] %d after  onBar=%d x=%.1f w=%.1f animate=%d",
                   diagIndex, (int)entry.visibleAfter, entry.finalX,
                   entry.element.ActualWidth(), (int)entry.animate);
        } catch (winrt::hresult_error const&) {
        }
        diagIndex++;
    }
    // A chevron leaving the bar dies at frame one: the collapsed row has no
    // overflow, so the control is Windows' to retire. Held invisible in its
    // slot (no reflow mid-plan), never folded; the landing takes it.
    if (targetCollapse) {
        for (auto& entry : ctx.animPlan) {
            if (!entry.animate || !entry.visibleBefore || entry.visibleAfter) {
                continue;
            }
            try {
                if (winrt::get_class_name(entry.element) !=
                    L"Taskbar.OverflowToggleButton") {
                    continue;
                }
                FrameContext::GapHidden held;
                held.element = entry.element;
                held.opacity = entry.element.Opacity();
                held.maxWidth = entry.element.MaxWidth();
                entry.element.Opacity(0);
                ctx.gapHidden.push_back(std::move(held));
                entry.animate = false;
            } catch (winrt::hresult_error const&) {
            }
        }
    }
    // The promoted-slot divider: only the widened button beside the chevron
    // carries one. The icon slides on; its line dies at frame one, and the
    // collapsed row has no promoted slot for it to return to.
    if (targetCollapse) {
        for (auto& entry : ctx.animPlan) {
            if (!entry.animate || !entry.visibleBefore) {
                continue;
            }
            try {
                if (entry.element.ActualWidth() <= 34.0 ||
                    winrt::get_class_name(entry.element) !=
                        L"Taskbar.TaskListButton") {
                    continue;
                }
                if (auto divider = FindDividerRect(entry.element, 3)) {
                    FrameContext::GapHidden held;
                    held.element = divider;
                    held.opacity = divider.Opacity();
                    held.maxWidth = divider.MaxWidth();
                    divider.Opacity(0);
                    ctx.gapHidden.push_back(std::move(held));
                }
            } catch (winrt::hresult_error const&) {
            }
        }
    }
    if (targetCollapse) {
        // Collapse animates over the old layout; put it back. A reveal
        // keeps the final layout live from frame one instead.
        SetCollapseState(ctx, ctx.animButtons, priorCollapse);
        for (int pass = 0; pass < 3; pass++) {
            frame.UpdateLayout();
        }
    }

    // haveStart: one-sided entries that only have startX get finalX synth'd
    // (disappearing); the other call fills startX for appearing ones.
    auto fill = [&](bool haveStart) {
        std::vector<FrameContext::PlanEntry*> known;
        std::vector<FrameContext::PlanEntry*> missing;
        for (auto& entry : ctx.animPlan) {
            if (!entry.animate) {
                continue;
            }
            if (entry.visibleBefore && entry.visibleAfter) {
                known.push_back(&entry);
            } else if (haveStart ? entry.visibleBefore : entry.visibleAfter) {
                missing.push_back(&entry);
            }
        }
        auto key = [&](FrameContext::PlanEntry* entry) {
            return haveStart ? entry->startX : entry->finalX;
        };
        auto deltaOf = [&](FrameContext::PlanEntry* entry) {
            return haveStart ? entry->finalX - entry->startX
                             : entry->startX - entry->finalX;
        };
        std::stable_sort(
            known.begin(), known.end(),
            [&](FrameContext::PlanEntry* a, FrameContext::PlanEntry* b) {
                return key(a) < key(b);
            });
        for (auto* entry : missing) {
            double k = key(entry);
            FrameContext::PlanEntry* left = nullptr;
            FrameContext::PlanEntry* right = nullptr;
            for (auto* candidate : known) {
                if (key(candidate) <= k) {
                    left = candidate;
                } else {
                    right = candidate;
                    break;
                }
            }
            double delta = 0;
            if (left && right) {
                double span = key(right) - key(left);
                double u = span > 0.5 ? (k - key(left)) / span : 0.5;
                delta = deltaOf(left) + (deltaOf(right) - deltaOf(left)) * u;
            } else if (left) {
                delta = deltaOf(left);
            } else if (right) {
                delta = deltaOf(right);
            }
            if (haveStart) {
                entry->finalX = entry->startX + delta;
            } else {
                entry->startX = entry->finalX + delta;
            }
        }
    };
    fill(true);
    fill(false);

    // Icons on the bar in a phase; only their absence matters, for the
    // anchor below.
    auto countOnBar = [&](bool beforePhase) {
        int count = 0;
        for (auto& entry : ctx.animPlan) {
            bool visible =
                beforePhase ? entry.visibleBefore : entry.visibleAfter;
            if (!visible || !entry.animate) {
                continue;
            }
            for (auto const& button : ctx.animButtons) {
                if (button == entry.element) {
                    count++;
                    break;
                }
            }
        }
        return count;
    };

    // With no icons at all on one side of the cut, the ones appearing (or
    // vanishing) have no neighbour to take a position from, so the synthesis
    // above leaves them where they land and they pop in place. Fan them out
    // of — or into — the right edge of whatever stays on screen, which is
    // Start, so the motion still reads as a fold.
    auto anchorToVisible = [&](bool beforePhase) {
        double anchor = 0;
        bool have = false;
        for (auto& entry : ctx.animPlan) {
            if (!entry.animate ||
                (beforePhase ? !entry.visibleBefore : !entry.visibleAfter)) {
                continue;
            }
            double right = beforePhase ? entry.startX : entry.finalX;
            try {
                right += entry.element.ActualWidth();
            } catch (winrt::hresult_error const&) {
            }
            if (!have || right > anchor) {
                anchor = right;
                have = true;
            }
        }
        if (!have) {
            return;
        }
        for (auto& entry : ctx.animPlan) {
            bool otherSideOnly =
                beforePhase ? (!entry.visibleBefore && entry.visibleAfter)
                            : (entry.visibleBefore && !entry.visibleAfter);
            if (!otherSideOnly || !entry.animate) {
                continue;
            }
            if (beforePhase) {
                entry.startX = anchor;
            } else {
                entry.finalX = anchor;
            }
        }
    };
    if (countOnBar(true) == 0) {
        anchorToVisible(true);
    }
    if (countOnBar(false) == 0) {
        anchorToVisible(false);
    }

    // Reveal: appearing icons hold their slots transparently until the apex
    // flips their opacity — a composition-only cut that cannot tear against
    // the translations. Survivors are frozen at their old visuals here,
    // before anything renders.
    if (!targetCollapse) {
        for (auto& entry : ctx.animPlan) {
            if (!entry.animate) {
                continue;
            }
            try {
                if (!entry.visibleBefore && entry.visibleAfter) {
                    FrameContext::GapHidden held;
                    held.element = entry.element;
                    held.opacity = entry.element.Opacity();
                    held.maxWidth = entry.element.MaxWidth();
                    entry.element.Opacity(0);
                    ctx.gapHidden.push_back(std::move(held));
                    SetSlideX(ctx, entry.element, 0);
                } else if (entry.visibleBefore) {
                    SetSlideX(ctx, entry.element,
                              entry.startX - entry.finalX);
                }
            } catch (winrt::hresult_error const&) {
            }
        }
    }
    // Seat every slide here, in dispatcher context, not in the first tick.
    for (auto& entry : ctx.animPlan) {
        if (entry.animate) {
            try {
                EnsureSlide(ctx, entry.element);
            } catch (winrt::hresult_error const&) {
            }
        }
    }
    for (auto& entry : ctx.animPlan) {
        try {
            entry.visiblePhase1 =
                entry.element.Visibility() == Visibility::Visible;
        } catch (winrt::hresult_error const&) {
            entry.visiblePhase1 = entry.visibleBefore;
        }
    }

    // The flips above storm UpdateVisualStates, and Windows re-arms its
    // reposition animations off it — inside this same pass, after the
    // apply's strip. Stripped again last, or the commit animates.
    DeanimateRunSet(ctx, ctx.animButtons);

    return !ctx.animPlan.empty();
}

bool HasVisibleIcon(FrameworkElement root, int depth) {
    if (!root || depth < 0) {
        return false;
    }
    bool found = false;
    EnumChildElements(root, [&](FrameworkElement child) {
        if (found) {
            return false;
        }
        try {
            if (child.Visibility() == Visibility::Visible &&
                winrt::get_class_name(child) ==
                    L"Windows.UI.Xaml.Controls.Image") {
                found = true;
                return false;
            }
        } catch (winrt::hresult_error const&) {
        }
        if (HasVisibleIcon(child, depth - 1)) {
            found = true;
        }
        return false;
    });
    return found;
}

// A container the taskbar started tearing down and left in the row: a spare
// task button, icon already dropped, sitting on top of a real one instead of
// parked off-screen. It draws its background plate over that icon — the
// stray highlight. Nothing legitimately puts two buttons at one spot, and a
// real one always has its icon, so the pair identifies it beyond doubt.
// Hiding it is what the teardown would have done; Windows shows the
// container again itself if it reuses it.
// Undoes a phantom judgement that turned out to be wrong. A container the
// taskbar really was discarding never gets its icon back; a real button
// whose icon was merely still loading does, and comes straight back.
void RestoreRepairedPhantoms(FrameContext& ctx, FrameworkElement frame) {
    for (size_t i = 0; i < ctx.phantomHidden.size();) {
        auto element = ctx.phantomHidden[i].get();
        if (!element) {
            ctx.phantomHidden.erase(ctx.phantomHidden.begin() + i);
            continue;
        }
        // Restore the moment the reason to hide it stops holding: it has an
        // icon after all, or it is no longer in the row. Nothing this mod
        // hides on suspicion may outlive the suspicion.
        bool restore = false;
        PCWSTR why = L"threw";
        double x = 0;
        try {
            x = element.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
            double frameWidth = frame.ActualWidth();
            if (HasVisibleIcon(element, 3)) {
                why = L"has icon";
                restore = true;
            } else if (x < 0 || (frameWidth > 0 && x > frameWidth)) {
                why = L"off bar";
                restore = true;
            }
        } catch (winrt::hresult_error const&) {
            restore = true;
        }
        if (!restore) {
            i++;
            continue;
        }
        std::wstring name = L"?";
        try {
            name = std::wstring(winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(element));
        } catch (winrt::hresult_error const&) {
        }
        try {
            STRAY_LOG(L"Phantom restored (%s) at x=%.1f: %s", why, x,
                      name.c_str());
            element.Visibility(Visibility::Visible);
        } catch (winrt::hresult_error const&) {
        }
        ctx.phantomHidden.erase(ctx.phantomHidden.begin() + i);
    }
}

int HidePhantomButtons(FrameContext& ctx,
                       FrameworkElement repeater,
                       FrameworkElement frame,
                       PCWSTR where) {
    struct Candidate {
        FrameworkElement element{nullptr};
        double x = 0;
        bool hasIcon = false;
    };
    std::vector<Candidate> row;

    EnumChildElements(repeater, [&](FrameworkElement child) {
        try {
            if (child.Visibility() != Visibility::Visible) {
                return false;
            }
            auto classNameHstring = winrt::get_class_name(child);
            std::wstring_view className(classNameHstring);
            if (className.find(L"TaskListButton") == std::wstring_view::npos ||
                className.find(L"Panel") != std::wstring_view::npos) {
                return false;
            }
            Candidate candidate;
            candidate.element = child;
            candidate.x =
                child.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
            // Only judge what is actually in the row. Parked containers and
            // overflow-menu entries live thousands of pixels off-screen, and
            // they are none of this check's business.
            double frameWidth = frame.ActualWidth();
            if (candidate.x < 0 || (frameWidth > 0 && candidate.x > frameWidth)) {
                return false;
            }
            candidate.hasIcon = HasVisibleIcon(child, 3);
            row.push_back(std::move(candidate));
        } catch (winrt::hresult_error const&) {
        }
        return false;
    });

    int hidden = 0;
    for (auto& candidate : row) {
        if (candidate.hasIcon) {
            continue;
        }
        for (auto& other : row) {
            if (other.element == candidate.element || !other.hasIcon) {
                continue;
            }
            if (std::abs(candidate.x - other.x) > 1.0) {
                continue;
            }
            // Names first, on their own: the hide must not hang on a log.
            std::wstring candName = L"?";
            std::wstring otherName = L"?";
            try {
                candName = std::wstring(winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(candidate.element));
                otherName = std::wstring(winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(other.element));
            } catch (winrt::hresult_error const&) {
            }
            try {
                STRAY_LOG(L"Phantom hidden (%s) at x=%.1f: %s over %s", where,
                          candidate.x, candName.c_str(), otherName.c_str());
                candidate.element.Visibility(Visibility::Collapsed);
                ctx.phantomHidden.push_back(
                    winrt::make_weak(candidate.element));
                hidden++;
            } catch (winrt::hresult_error const&) {
            }
            break;
        }
    }
    return hidden;
}

// Checks the row against both ways this mod can leave an icon mispositioned:
// an offset it never cleared, and a width it pinned to zero and never put
// back — the second reads as an icon squashed to nothing behind its
// neighbour, and no offset sweep would ever see it. Repairs and names
// whichever it finds. Elements deliberately held for a running animation
// are skipped.
int VerifyRowSettled(FrameContext& ctx, FrameworkElement root, int depth) {
    if (!root || depth < 0) {
        return 0;
    }

    int repairs = 0;
    EnumChildElements(root, [&](FrameworkElement child) {
        bool held = false;
        for (auto& entry : ctx.gapHidden) {
            if (entry.element == child) {
                held = true;
                break;
            }
        }

        if (!held) {
            try {
                double slide = GetSlideX(ctx, child);
                if (slide != 0) {
                    auto name = winrt::Windows::UI::Xaml::Automation::
                        AutomationProperties::GetName(child);
                    Wh_Log(L"Settle repair: slide %.1f on %s (%s)", slide,
                           winrt::get_class_name(child).c_str(), name.c_str());
                    SetSlideX(ctx, child, 0);
                    repairs++;
                }
                // Windows' own channel; read for the log, never written.
                auto translation = child.Translation();
                if (translation.x != 0 || translation.y != 0) {
                    DIAG_LOG(L"[diag] facade offset %.1f,%.1f on %s",
                             translation.x, translation.y,
                             winrt::get_class_name(child).c_str());
                }
                // Shown but fully transparent: it holds its slot and stays
                // clickable while drawing nothing — a gap in the row. Only
                // this mod fades a taskbar element, so this one is ours.
                if (child.Visibility() == Visibility::Visible &&
                    child.Opacity() == 0.0) {
                    auto name = winrt::Windows::UI::Xaml::Automation::
                        AutomationProperties::GetName(child);
                    Wh_Log(L"Settle repair: zero opacity on %s (%s)",
                           winrt::get_class_name(child).c_str(), name.c_str());
                    child.ClearValue(UIElement::OpacityProperty());
                    repairs++;
                }
                // Only this mod pins a taskbar element to zero width, so a
                // visible one still pinned is ours and never restored.
                if (child.Visibility() == Visibility::Visible &&
                    child.MaxWidth() == 0.0) {
                    auto name = winrt::Windows::UI::Xaml::Automation::
                        AutomationProperties::GetName(child);
                    Wh_Log(L"Settle repair: zero width on %s (%s)",
                           winrt::get_class_name(child).c_str(), name.c_str());
                    child.ClearValue(FrameworkElement::MaxWidthProperty());
                    repairs++;
                }
            } catch (winrt::hresult_error const&) {
            }
        }

        repairs += VerifyRowSettled(ctx, child, depth - 1);
        return false;
    });
    return repairs;
}

// Composition-side reads XAML cannot see: the hand-off visual and the
// internal visuals above it (the first parent carries the Translation
// facade). Read only. anim letters: A = an animation is bound, - = none.
void DiagDumpVisualChain(FrameworkElement const& button, PCWSTR tag) {
    using namespace winrt::Windows::UI::Composition;
    try {
        auto visual =
            Hosting::ElementCompositionPreview::GetElementVisual(button);
        if (!visual) {
            return;
        }
        auto name = winrt::Windows::UI::Xaml::Automation::AutomationProperties::
            GetName(button);
        auto bound = [](CompositionObject const& object,
                        PCWSTR property) -> wchar_t {
            try {
                return object.TryGetAnimationController(property) ? L'A'
                                                                  : L'-';
            } catch (winrt::hresult_error const&) {
                return L'?';
            }
        };
        float3 slide{0, 0, 0};
        int status =
            (int)visual.Properties().TryGetVector3(L"Translation", slide);
        DIAG_LOG(L"[diag] %s chain v: psT=%d/%.1f op=%.2f sc=%.2f vis=%d "
                 L"w=%.1f clip=%d anim[T,O,S,P,M]=%c%c%c%c%c %s",
                 tag, status, slide.x, visual.Opacity(), visual.Scale().x,
                 (int)visual.IsVisible(), visual.Size().x,
                 visual.Clip() ? 1 : 0, bound(visual.Properties(), L"Translation"),
                 bound(visual, L"Offset"), bound(visual, L"Scale"),
                 bound(visual, L"Opacity"), bound(visual, L"TransformMatrix"),
                 name.c_str());
        ContainerVisual parent = visual.Parent();
        for (int level = 1; parent && level <= 3; level++) {
            DIAG_LOG(L"[diag] %s chain p%d: off=%.1f m41=%.1f op=%.2f vis=%d "
                     L"clip=%d anim[O,M]=%c%c %s",
                     tag, level, parent.Offset().x,
                     parent.TransformMatrix().m41, parent.Opacity(),
                     (int)parent.IsVisible(), parent.Clip() ? 1 : 0,
                     bound(parent, L"Offset"), bound(parent, L"TransformMatrix"),
                     winrt::get_class_name(parent).c_str());
            parent = parent.Parent();
        }
        if (auto icon = FindVisibleIcon(button, 3)) {
            auto translation = icon.Translation();
            DIAG_LOG(L"[diag] %s chain icon: tx=%.1f ty=%.1f op=%.2f", tag,
                     translation.x, translation.y, icon.Opacity());
        }
    } catch (winrt::hresult_error const&) {
    }
}

void DiagDumpRow(FrameContext& ctx, PCWSTR tag) {
    if (!g_diagLog.load()) {
        return;
    }
    auto frame = ctx.frame.get();
    auto repeater = ctx.repeater.get();
    if (!frame || !repeater) {
        return;
    }
    double frameWidth = 0;
    try {
        frameWidth = frame.ActualWidth();
    } catch (winrt::hresult_error const&) {
    }
    EnumChildElements(repeater, [&](FrameworkElement child) {
        try {
            bool visible = child.Visibility() == Visibility::Visible;
            double x = child.TransformToVisual(frame)
                           .TransformPoint(Point{0, 0})
                           .X;
            auto translation = child.Translation();
            double opacity = child.Opacity();
            double maxWidth = child.MaxWidth();
            bool parked = x < -1 || (frameWidth > 0 && x > frameWidth);
            if (visible && !parked &&
                winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
                float3 off{0, 0, 0};
                float m41 = 0;
                if (auto visual = Hosting::ElementCompositionPreview::
                        GetElementVisual(child)) {
                    off = visual.Offset();
                    m41 = visual.TransformMatrix().m41;
                }
                DIAG_LOG(L"[diag] %s comp: x=%.1f sx=%.1f tx=%.1f off=%.1f "
                         L"m41=%.1f %s",
                       tag, x, GetSlideX(ctx, child), translation.x, off.x,
                       m41,
                       winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(child).c_str());
                DiagDumpVisualChain(child, tag);
            }
            bool anomaly = visible && (translation.x != 0 || opacity == 0.0 ||
                                       maxWidth == 0.0 || parked);
            if (anomaly) {
                DIAG_LOG(L"[diag] %s: x=%.1f tx=%.1f op=%.2f mw=%.0f %s", tag,
                       x, translation.x, opacity,
                       maxWidth > 1e9 ? -1.0 : maxWidth,
                       winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(child).c_str());
            }
        } catch (winrt::hresult_error const&) {
        }
        return false;
    });
}

// Lands a finished collapse run: the real hides, the translation zeroing
// and the opacity restores in one dispatcher pass, off the render clock —
// where commits have proven atomic (the reveal's start-of-run flip). Every
// step is fenced so a throw cannot strand a partial landing, and a
// follow-up pass re-checks for stragglers.
void FinalizeCollapseRun(FrameContext& ctx) {
    ctx.animFinalizePending = false;
    DIAG_LOG(L"[diag] finalize: plan=%d gapHidden=%d hiddenByUs=%d",
           (int)ctx.animPlan.size(), (int)ctx.gapHidden.size(),
           (int)ctx.hiddenByUs.size());
    // Windows re-arms its reposition animations between the apex and here;
    // strip them again, or the layout flip glides or strands the survivors.
    for (auto& entry : ctx.animPlan) {
        try {
            DeanimateButton(ctx, entry.element);
        } catch (winrt::hresult_error const&) {
        }
    }
    std::vector<FrameworkElement> buttons = ctx.animButtons;
    try {
        SetCollapseState(ctx, buttons, true);
    } catch (winrt::hresult_error const&) {
        Wh_Log(L"Collapse finalize: SetCollapseState threw");
    }
    // The hides storm UpdateVisualStates and Windows re-arms off it before
    // this pass commits. Flush the storm, strip what it armed; only then
    // clear the translations, so the commit lands dead.
    if (auto frame = ctx.frame.get()) {
        try {
            frame.UpdateLayout();
        } catch (winrt::hresult_error const&) {
        }
    }
    // The flush pulls Windows' overflow drain in synchronously, and the
    // drain can surface freshly drained non-running icons. Hide them in
    // this same pass, or they render for a frame and a gap-close chases
    // them; flush again so the strip sees the settled row.
    try {
        SetCollapseState(ctx, buttons, true);
    } catch (winrt::hresult_error const&) {
    }
    if (auto frame = ctx.frame.get()) {
        try {
            frame.UpdateLayout();
        } catch (winrt::hresult_error const&) {
        }
    }
    DeanimateRunSet(ctx, buttons);
    StopAnimation(ctx);
    // Belt, from the frame rather than the row: the cached container can be
    // the wrong one, and this must not depend on it being right.
    if (auto frame = ctx.frame.get()) {
        if (int repaired = VerifyRowSettled(ctx, frame, 5)) {
            Wh_Log(L"Collapse finalize: verify repaired %d", repaired);
        }
        if (auto repeater = ctx.repeater.get()) {
            HidePhantomButtons(ctx, repeater, frame, L"finalize");
        }
    }
    DiagDumpRow(ctx, L"finalize-end");
    // And one verification pass right behind it.
    PostApply(ctx.dispatcher);
}

void DiagDumpChrome(FrameworkElement root,
                    FrameworkElement frame,
                    int depth) {
    if (!g_diagLog.load() || !root || depth < 0) {
        return;
    }
    EnumChildElements(root, [&](FrameworkElement child) {
        try {
            auto cls = winrt::get_class_name(child);
            std::wstring_view v(cls);
            bool realButton = v.size() >= 6 &&
                              v.substr(v.size() - 6) == L"Button";
            double w = child.ActualWidth();
            double x = child.TransformToVisual(frame)
                           .TransformPoint(Point{0, 0})
                           .X;
            DIAG_LOG(L"[diag] chrome d%d x=%.1f w=%.1f h=%.1f vis=%d op=%.2f %s",
                   depth, x, w, child.ActualHeight(),
                   (int)(child.Visibility() == Visibility::Visible),
                   child.Opacity(), cls.c_str());
            bool wide = w > 34.0 && w < 100.0 && x > -1;
            if (!realButton || wide) {
                DiagDumpChrome(child, frame, depth - 1);
            }
        } catch (winrt::hresult_error const&) {
        }
        return false;
    });
}

// Stray-highlight hunt: a visible button container with no icon is the
// only XAML-side trace of the box. Its plate brush can read transparent
// while stale content still draws, so every field the mod acts on is
// printed. Silent while the strip is healthy.
void DiagDumpStrays(FrameContext& ctx, PCWSTR tag) {
    if (!g_diagStray.load()) {
        return;
    }
    auto frame = ctx.frame.get();
    auto repeater = ctx.repeater.get();
    if (!frame || !repeater) {
        return;
    }
    double frameWidth = 0;
    try {
        frameWidth = frame.ActualWidth();
    } catch (winrt::hresult_error const&) {
    }
    EnumChildElements(repeater, [&](FrameworkElement child) {
        try {
            if (child.Visibility() != Visibility::Visible ||
                winrt::get_class_name(child) != L"Taskbar.TaskListButton" ||
                HasVisibleIcon(child, 3)) {
                return false;
            }
            double x =
                child.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
            if (x < -1 || (frameWidth > 0 && x > frameWidth)) {
                return false;
            }
            // First lit plate brush, if the container still holds one.
            wchar_t plate[16] = L"none";
            std::function<bool(FrameworkElement, int)> findPlate =
                [&](FrameworkElement root, int depth) {
                    if (!root || depth < 0) {
                        return false;
                    }
                    return EnumChildElements(root, [&](FrameworkElement node) {
                        if (auto border = node.try_as<Controls::Border>()) {
                            auto bg = border.Background();
                            auto solid =
                                bg ? bg.try_as<Media::SolidColorBrush>()
                                   : nullptr;
                            if (solid && solid.Color().A > 0) {
                                auto c = solid.Color();
                                swprintf_s(plate, L"%02X%02X%02X%02X", c.A,
                                           c.R, c.G, c.B);
                                return true;
                            }
                        }
                        return findPlate(node, depth - 1);
                    }) != nullptr;
                };
            findPlate(child, 3);
            bool phantom = false;
            for (auto& weak : ctx.phantomHidden) {
                if (weak.get() == child) {
                    phantom = true;
                    break;
                }
            }
            Wh_Log(L"[stray] %s: x=%.1f op=%.2f running=%d ours=%d "
                   L"phantom=%d plate=%s %s",
                   tag, x, child.Opacity(),
                   (int)TaskListButton_IsRunning(child),
                   (int)TakeFromHiddenSet(ctx.hiddenByUs, child, false),
                   (int)phantom, plate,
                   winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(child).c_str());
        } catch (winrt::hresult_error const&) {
        }
        return false;
    });
}

void StartAnimation(FrameContext& ctx,
                    std::vector<FrameworkElement> buttons,
                    bool targetCollapse) {
    DIAG_LOG(L"[diag] StartAnimation target=%d buttons=%d",
           (int)targetCollapse, (int)buttons.size());
    if (auto frame = ctx.frame.get()) {
        DiagDumpChrome(frame, frame, 8);
    }
    ctx.animActive = true;
    ctx.animKind = AnimKind::Accordion;
    ctx.animTargetCollapse = targetCollapse;
    ctx.animStartMs = NowMs();
    ctx.animSwapped = false;
    ctx.animButtons = std::move(buttons);
    bool planned = false;
    if (auto frame = ctx.frame.get()) {
        // No pre-sort: the plan assigns fold order from measured X itself.
        planned = BuildAnimPlan(ctx, frame, targetCollapse);
    }
    if (!planned) {
        // No table, no playback: land the target instantly.
        SetCollapseState(ctx, ctx.animButtons, targetCollapse);
        StopAnimation(ctx);
        return;
    }
    DiagDumpStrays(ctx, L"run-start");
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
            // Running is a genuine launch, not a re-show; let it by.
            if (button.Visibility() == Visibility::Visible &&
                !TaskListButton_IsRunning(button)) {
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
        SetSlideX(ctx, ctx.animButtons[i], ctx.animGapOffsets[i]);
    }
}

// The hide lands here and now; survivors freeze mid-gap in the same pass.
// The rendering tick waits out Windows' close churn, then eases the gap shut.
void StartGapCloseAnimation(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    std::vector<std::pair<FrameworkElement, double>> deferHide) {
    DIAG_LOG(L"[diag] StartGapClose deferHide=%d", (int)deferHide.size());
    // Hold everything the repeater lays out — on a centred bar Start and
    // search shift too — so the whole strip freezes and eases as one.
    ctx.animButtons.clear();
    if (auto repeater = ctx.repeater.get()) {
        EnumChildElements(repeater, [&](FrameworkElement child) {
            ctx.animButtons.push_back(child);
            return false;
        });
    }
    if (ctx.animButtons.empty()) {
        ctx.animButtons = buttons;
    }
    ctx.animChildCount = CountStripChildren(ctx);
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
    float diagMax = 0;
    for (float offset : ctx.animGapOffsets) {
        if (std::abs(offset) > std::abs(diagMax)) {
            diagMax = offset;
        }
    }
    DIAG_LOG(L"[diag] StartGapClose survivors=%d maxOffset=%.1f",
           (int)ctx.animButtons.size(), diagMax);
    // The hide and the freeze committed by this pass re-armed Windows'
    // animations mid-pass; strip last so the commit lands dead.
    DeanimateRunSet(ctx, ctx.animButtons);
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
        // A rebuild (a DPI change, say) can leave the cached container alive
        // but detached, still handing back stale buttons. Re-discover then.
        bool attached = false;
        try {
            attached = Media::VisualTreeHelper::GetParent(repeater) != nullptr;
        } catch (winrt::hresult_error const&) {
        }
        if (attached) {
            CollectTaskListButtons(repeater, 3, buttons);
        } else {
            ctx.repeater = nullptr;
        }
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

    // Before anything measures or counts: an abandoned container is a
    // visible button to every line below, so it would join the plan, skew
    // the counts and draw its plate over a real icon.
    RestoreRepairedPhantoms(ctx, frame);
    if (auto repeater = ctx.repeater.get()) {
        if (HidePhantomButtons(ctx, repeater, frame, L"apply") > 0) {
            buttons.clear();
            CollectTaskListButtons(repeater, 3, buttons);
        }
    }

    // Only replace the known-good set on success: a rebuild that walks up
    // empty must not leave IsPointerClearOfButtons with nothing to test.
    if (buttons.empty()) {
        return false;
    }

    ctx.lastButtons.clear();
    for (auto& button : buttons) {
        ctx.lastButtons.push_back(winrt::make_weak(button));
    }

    // Windows animates these same icons, and a hidden one keeps its old spot
    // — so anything it plays lands from the wrong place and fights whatever
    // we are doing. Its animations stay off for as long as the mod manages
    // the row, and come back when it stops.
    if (!g_unloading) {
        if (!g_collapseEnabled && ctx.hiddenByUs.empty()) {
            ReanimateButtons(ctx);
            DetachSlides(ctx);
        } else {
            DeanimateRunSet(ctx, buttons);
        }
    }

    bool instant = instantRequested || ctx.appliedCollapse < 0 ||
                   g_settings.animationMode == AnimationMode::None;

    DIAG_LOG(L"[diag] apply collapse=%d applied=%d instant=%d buttons=%d",
           (int)collapse, ctx.appliedCollapse, (int)instant,
           (int)buttons.size());

    if (!instant && (collapse ? 1 : 0) != ctx.appliedCollapse) {
        StartAnimation(ctx, std::move(buttons), collapse);
        return true;
    }

    bool animateGaps = !instant && !g_unloading &&
                       g_settings.animationMode == AnimationMode::Slide &&
                       (collapse ? 1 : 0) == ctx.appliedCollapse;
    std::vector<std::pair<FrameworkElement, double>> deferHide;
    SetCollapseState(ctx, buttons, collapse,
                     animateGaps ? &deferHide : nullptr);
    if (!deferHide.empty()) {
        StartGapCloseAnimation(ctx, buttons, std::move(deferHide));
    } else if (!ctx.animActive) {
        // Steady state: a recycled container can resurface carrying a stale
        // offset or width from an earlier run. Row only — this runs on every
        // taskbar event, so it stays shallow; the landing does the deep one.
        if (auto repeater = ctx.repeater.get()) {
            VerifyRowSettled(ctx, repeater, 0);
        }
    }
    DiagDumpRow(ctx, L"apply-end");
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

    // Divergence, shared by both animations: an element joining or leaving
    // the strip means the plan being played back is stale.
    int childCount = CountStripChildren(*ctx);
    bool countDiverged = ctx->animChildCount >= 0 && childCount >= 0 &&
                         childCount != ctx->animChildCount;

    if (ctx->animKind == AnimKind::GapClose) {
        // A held survivor vanishing counts too; land instantly either way.
        if (countDiverged) {
            DIAG_LOG(L"[diag] gapclose diverge: children %d -> %d",
                   ctx->animChildCount, childCount);
            StopAnimation(*ctx);
            return;
        }
        try {
            for (auto& button : ctx->animButtons) {
                if (button.Visibility() != Visibility::Visible) {
                    DIAG_LOG(L"[diag] gapclose diverge: survivor hidden");
                    StopAnimation(*ctx);
                    return;
                }
            }
        } catch (winrt::hresult_error const&) {
            StopAnimation(*ctx);
            return;
        }
        // Windows' close churn can re-show what just got hidden, any frame;
        // re-assert every frame. gapHidden stays transparent for the whole
        // run, so a lost visibility race still renders empty.
        try {
            bool repaired = false;
            for (size_t i = 0; i < ctx->gapHidden.size();) {
                auto& entry = ctx->gapHidden[i];
                // Relaunched mid-run: the button is Windows' again.
                if (TaskListButton_IsRunning(entry.element)) {
                    entry.element.MaxWidth(entry.maxWidth);
                    entry.element.Opacity(entry.opacity);
                    ctx->gapHidden.erase(ctx->gapHidden.begin() + i);
                    continue;
                }
                if (entry.element.Opacity() != 0.0) {
                    entry.element.Opacity(0);
                }
                if (entry.element.MaxWidth() != 0.0) {
                    entry.element.MaxWidth(0);
                }
                i++;
            }
            for (auto& weak : ctx->hiddenByUs) {
                if (auto button = weak.get()) {
                    // Running is a genuine launch, not a re-show; let it by.
                    if (button.Visibility() == Visibility::Visible &&
                        !TaskListButton_IsRunning(button)) {
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
            // Gap holds open for a fixed beat, then the ease takes over.
            double now = NowMs();
            if (now - ctx->animStartMs < kGapHoldMs) {
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
            SetSlideX(*ctx, ctx->animButtons[i],
                      ctx->animGapOffsets[i] * factor);
        }
        return;
    }

    // Accordion playback. Reality must match the table; an element joining,
    // leaving, or flipping phase off-script kills the run and lands the
    // target instantly.
    bool diverged = countDiverged;
    if (diverged) {
        DIAG_LOG(L"[diag] accordion diverge: children %d -> %d",
               ctx->animChildCount, childCount);
    }
    if (!diverged) {
        try {
            for (auto& entry : ctx->animPlan) {
                // Overflowed throughout: Windows moves it in and out of the
                // overflow on its own clock, and none of that is off-script.
                if (!entry.animate) {
                    continue;
                }
                // Neither direction flips Visibility mid-run, so the
                // phase-one snapshot is the expectation for the whole run.
                if ((entry.element.Visibility() == Visibility::Visible) !=
                    entry.visiblePhase1) {
                    try {
                        DIAG_LOG(L"[diag] accordion diverge: vis flip on %s "
                               L"expect=%d swapped=%d",
                               winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(entry.element).c_str(),
                               (int)entry.visiblePhase1,
                               (int)ctx->animSwapped);
                    } catch (winrt::hresult_error const&) {
                    }
                    diverged = true;
                    break;
                }
            }
        } catch (winrt::hresult_error const&) {
            DIAG_LOG(L"[diag] accordion diverge: threw");
            diverged = true;
        }
    }
    if (diverged) {
        // Land first, then clean: the reverse order flashes the full row.
        // Stripped again first, or the landing glides on Windows' re-armed
        // animations.
        bool target = ctx->animTargetCollapse;
        std::vector<FrameworkElement> buttons = ctx->animButtons;
        for (auto& entry : ctx->animPlan) {
            try {
                DeanimateButton(*ctx, entry.element);
            } catch (winrt::hresult_error const&) {
            }
        }
        SetCollapseState(*ctx, buttons, target);
        StopAnimation(*ctx);
        return;
    }

    if (t >= 0.5 && !ctx->animSwapped) {
        ctx->animSwapped = true;
        DiagDumpStrays(*ctx, L"apex");
        // The apex cut is opacity only, both ways; the layout never moves
        // mid-run. Reveal flips the appearing icons in; collapse flips the
        // disappearing ones out, and their real hides land at the end.
        if (ctx->animTargetCollapse) {
            for (auto& entry : ctx->animPlan) {
                if (!entry.animate) {
                    continue;
                }
                try {
                    if (entry.visibleBefore && !entry.visibleAfter) {
                        FrameContext::GapHidden held;
                        held.element = entry.element;
                        held.opacity = entry.element.Opacity();
                        held.maxWidth = entry.element.MaxWidth();
                        entry.element.Opacity(0);
                        ctx->gapHidden.push_back(std::move(held));
                    }
                } catch (winrt::hresult_error const&) {
                }
            }
        } else {
            for (auto& entry : ctx->gapHidden) {
                try {
                    entry.element.Opacity(entry.opacity);
                } catch (winrt::hresult_error const&) {
                }
            }
            ctx->gapHidden.clear();
        }
        // The flip can prompt fresh animations while the tick is paused.
        for (auto& button : ctx->animButtons) {
            DeanimateButton(*ctx, button);
        }
    }

    if (t >= 1.0) {
        DiagDumpStrays(*ctx, L"run-end");
        if (ctx->animTargetCollapse) {
            // Land the layout off the render clock; the next dispatcher
            // pass commits hides, translations and opacities together.
            ctx->animFinalizePending = true;
            UnhookRendering(*ctx);
            PostApply(ctx->dispatcher);
            return;
        }
        StopAnimation(*ctx);
        return;
    }

    // Every element rides its measured travel on one clock. Snappiness cuts
    // that share of the journey out at the apex, where the icon swap hides
    // the jump — the whole animation, for every alignment and icon count.
    double skip = (double)g_settings.snappinessPct / 100.0;
    double progress = eased * (1 - skip) + (ctx->animSwapped ? skip : 0);

    try {
        for (auto& entry : ctx->animPlan) {
            bool visible = ctx->animSwapped ? entry.visibleAfter
                                            : entry.visibleBefore;
            // Overflowed icons keep whatever spot the layout gives them.
            if (!visible || !entry.animate) {
                SetSlideX(*ctx, entry.element, 0);
                continue;
            }
            // One continuous expression per direction, no apex switch:
            // reveal runs on the final layout throughout, collapse on the
            // starting layout throughout.
            double travel =
                ctx->animTargetCollapse
                    ? (entry.finalX - entry.startX) * progress
                    : (entry.startX - entry.finalX) * (1 - progress);
            SetSlideX(*ctx, entry.element, travel);
        }
    } catch (winrt::hresult_error const&) {
    }
}

void RestoreFrame(FrameContext& ctx) {
    StopAnimation(ctx);
    DetachSlides(ctx);

    for (auto& weak : ctx.hiddenByUs) {
        if (auto button = weak.get()) {
            button.Visibility(Visibility::Visible);
        }
    }
    ctx.hiddenByUs.clear();
    // Nothing this mod hid may outlive it, phantom judgements included.
    for (auto& weak : ctx.phantomHidden) {
        if (auto button = weak.get()) {
            button.Visibility(Visibility::Visible);
        }
    }
    ctx.phantomHidden.clear();
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
        // Full teardown: a Tick handler must not outlive the unload sweeps.
        RestoreFrame(*ctx);
        std::lock_guard<std::mutex> guard(g_framesMutex);
        if (ctx->timer) {
            ctx->timer.Stop();
            ctx->timer.Tick(ctx->tickToken);
        }
        g_frames->erase(key);
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

    // A finished collapse lands here, in dispatcher context.
    if (ctx->animFinalizePending) {
        FinalizeCollapseRun(*ctx);
    }
    if (ctx->diagSampleMs > 0 && NowMs() >= ctx->diagSampleMs) {
        ctx->diagSampleMs = 0;
        DiagDumpRow(*ctx, L"settled");
    }

    // Watchdog: Rendering stops when the bar is not being composed.
    if (ctx->animActive &&
        NowMs() - ctx->animStartMs >
            (double)g_settings.animationDurationMs * 3.0 + 1000.0) {
        DIAG_LOG(L"[diag] watchdog fired");
        // Land the target, or the next apply starts the same animation
        // again. Idempotent for a reveal that already landed.
        if (ctx->animKind == AnimKind::Accordion) {
            SetCollapseState(*ctx, ctx->animButtons, ctx->animTargetCollapse);
        }
        StopAnimation(*ctx);
    }

    // A running animation toward the right state owns the buttons. Killing
    // one mid-flight lands instantly: a toggle faster than the animation
    // is an instant flip, never a second animation.
    bool killed = false;
    if (ctx->animActive && (instant || desired != ctx->animTargetCollapse)) {
        StopAnimation(*ctx);
        killed = true;
    }
    if (!ctx->animActive) {
        if (ApplyToFrame(*ctx, desired, instant || killed)) {
            ctx->instantGenSeen = instantGen;
            ctx->applyRetries = 0;
        } else if (ctx->applyRetries > 0) {
            ctx->applyRetries--;
        } else if (ctx->retriesArmedGen != instantGen &&
                   (instant || ctx->appliedCollapse < 0)) {
            ctx->applyRetries = 20;
            ctx->retriesArmedGen = instantGen;
        }
    }

    // The timer runs only while something needs a clock; everything else
    // arrives as events. Reveal terms are global: cost, not correctness.
    bool timing = ctx->animActive || ctx->diagSampleMs > 0 ||
                  g_emptyHoverSinceTick != 0 ||
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
    // Late Taskbar.View.dll loads skip the loader-lock path; catch up here.
    StartHotkeyThread();
}

// A dead frame at a recycled address must not block re-registration.
bool IsFrameRegistered(void* key) {
    std::lock_guard<std::mutex> guard(g_framesMutex);
    auto it = g_frames->find(key);
    return it != g_frames->end() && it->second.frame.get() != nullptr;
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

void WakeAllFramesAsync(bool skipCurrentThread) {
    if (g_unloading) {
        return;
    }

    // Every reveal flip funnels through here; unload's restore is guarded
    // out above, so the pre-unload state is what survives.
    PersistRevealState();

    DWORD threadId = GetCurrentThreadId();
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> dispatchers;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            // Callers that already applied inline skip themselves.
            if (skipCurrentThread && ctx.threadId == threadId) {
                continue;
            }
            if (ctx.dispatcher) {
                dispatchers.push_back(ctx.dispatcher);
            }
        }
    }

    for (auto& dispatcher : dispatchers) {
        PostApply(dispatcher);
    }
}

// ---------------------------------------------------------------- diagnostic
// DIAGNOSTIC BUILD ONLY — strip before submitting. Ctrl+Alt+D dumps every
// taskbar button's subtree with the properties a stuck highlight would show
// up in. Changes nothing, so unlike a screenshot or a twirl it does not
// clear the ghost. One dump holds the stuck button and the healthy ones
// together, so the difference between them names the element.

void DumpButtonSubtree(FrameworkElement element,
                       FrameworkElement frame,
                       int depth,
                       int maxDepth) {
    if (!element || depth > maxDepth) {
        return;
    }

    try {
        std::wstring indent((size_t)depth * 2, L' ');
        auto className = winrt::get_class_name(element);
        auto name = winrt::Windows::UI::Xaml::Automation::AutomationProperties::
            GetName(element);
        double x = -99999;
        try {
            x = element.TransformToVisual(frame).TransformPoint(Point{0, 0}).X;
        } catch (winrt::hresult_error const&) {
        }
        auto translation = element.Translation();
        // RenderTransform is a different channel from Translation, and it is
        // the one other taskbar mods move icons with.
        double renderX = 0;
        try {
            if (auto transform = element.RenderTransform()) {
                if (auto single = transform.try_as<Media::TranslateTransform>()) {
                    renderX = single.X();
                } else if (auto group =
                               transform.try_as<Media::TransformGroup>()) {
                    for (auto const& part : group.Children()) {
                        if (auto move = part.try_as<Media::TranslateTransform>()) {
                            renderX += move.X();
                        }
                    }
                }
            }
        } catch (winrt::hresult_error const&) {
        }
        Wh_Log(L"%s%s [%s] vis=%d op=%.2f w=%.1f h=%.1f x=%.1f tx=%.1f "
               L"rtx=%.1f maxw=%.1f",
               indent.c_str(), className.c_str(), name.c_str(),
               element.Visibility() == Visibility::Visible ? 1 : 0,
               element.Opacity(), element.ActualWidth(),
               element.ActualHeight(), x, translation.x, renderX,
               element.MaxWidth());
    } catch (winrt::hresult_error const&) {
        return;
    }

    EnumChildElements(element, [&](FrameworkElement child) {
        DumpButtonSubtree(child, frame, depth + 1, maxDepth);
        return false;
    });
}

void DumpHighlightsOnThisThread() {
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

    for (void* key : keys) {
        FrameContext* ctx = GetFrameContext(key);
        if (!ctx) {
            continue;
        }
        auto frame = ctx->frame.get();
        auto repeater = ctx->repeater.get();
        if (!frame || !repeater) {
            continue;
        }

        // Where the cursor actually is: a lit plate on a button it is not
        // over is the anomaly.
        double cursorX = -1;
        POINT pt;
        if (GetCursorPos(&pt)) {
            try {
                auto local = frame.TransformToVisual(nullptr);
                auto origin = local.TransformPoint(Point{0, 0});
                cursorX = pt.x - origin.X;
            } catch (winrt::hresult_error const&) {
            }
        }
        Wh_Log(L"=== HIGHLIGHT DUMP cursorX=%.1f ===", cursorX);
        EnumChildElements(repeater, [&](FrameworkElement child) {
            DumpButtonSubtree(child, frame, 0, 4);
            return false;
        });
        Wh_Log(L"=== END HIGHLIGHT DUMP ===");
    }
}

void RequestHighlightDumpAsync() {
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
        // Same lifetime-tied counter as PostApply, so unload still drains.
        g_pendingWakes++;
        auto pending =
            std::shared_ptr<void>(nullptr, [](void*) { g_pendingWakes--; });
        try {
            dispatcher.RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                [pending]() {
                    if (!g_unloading) {
                        try {
                            DumpHighlightsOnThisThread();
                        } catch (...) {
                        }
                    }
                });
        } catch (winrt::hresult_error const&) {
        }
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
    WakeAllFramesAsync(true);
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

// Acts on release; where the release lands is all that decides.
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
    WakeAllFramesAsync(true);
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
    if (g_unloading) {
        return;
    }
    // Ghost guard: right after a collapse lands, the drain can surface a
    // non-running pinned icon for a frame before the posted apply hides it.
    // This hook runs inside the drain's own pass — the one spot early
    // enough to hide it before it renders or opens a gap. Running icons,
    // parked husks, and everything outside the window pass untouched.
    try {
        if (g_collapseEnabled && !g_revealed) {
            FrameworkElement element = nullptr;
            FrameKeyFromThis(pThis, &element);
            // Several frames can share a thread; the button is judged
            // against its own frame, found by walking up from it.
            FrameContext* ctx = nullptr;
            if (element) {
                std::vector<FrameContext*> candidates;
                DWORD threadId = GetCurrentThreadId();
                {
                    std::lock_guard<std::mutex> guard(g_framesMutex);
                    for (auto& [key, frameCtx] : *g_frames) {
                        if (frameCtx.threadId == threadId) {
                            candidates.push_back(&frameCtx);
                        }
                    }
                }
                auto node = Media::VisualTreeHelper::GetParent(element)
                                .try_as<FrameworkElement>();
                for (int depth = 0; node && !ctx && depth < 12; depth++) {
                    for (auto* candidate : candidates) {
                        if (candidate->frame.get() == node) {
                            ctx = candidate;
                            break;
                        }
                    }
                    node = Media::VisualTreeHelper::GetParent(node)
                               .try_as<FrameworkElement>();
                }
            }
            if (ctx && ctx->appliedCollapse == 1 &&
                NowMs() - ctx->collapseLandedMs < 1000.0) {
                if (winrt::get_class_name(element) ==
                        L"Taskbar.TaskListButton" &&
                    element.Visibility() == Visibility::Visible &&
                    !TaskListButton_IsRunning(element) &&
                    !IsParkedOffBar(*ctx, element)) {
                    DIAG_LOG(L"[diag] ghost hidden on sight");
                    element.Visibility(Visibility::Collapsed);
                    if (!TakeFromHiddenSet(ctx->hiddenByUs, element, false)) {
                        ctx->hiddenByUs.push_back(winrt::make_weak(element));
                    }
                }
            }
        }
    } catch (...) {
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

// DIAG: Windows' own moves on the buttons, timestamped against the mod's
// phases by the log clock. Log only; every symbol optional. float3 by value
// travels by pointer on x64, so the hooks take pointers.
using SharedAnimations_StartItemTranslationAnimation_t =
    void(__cdecl*)(void* pThis, void** element, float3* from, float3* to);
SharedAnimations_StartItemTranslationAnimation_t
    SharedAnimations_StartItemTranslationAnimation_Original;
void __cdecl SharedAnimations_StartItemTranslationAnimation_Hook(void* pThis,
                                                                void** element,
                                                                float3* from,
                                                                float3* to) {
    if (g_diagLog.load()) {
        try {
            winrt::Windows::UI::Xaml::UIElement target{nullptr};
            winrt::copy_from_abi(target, *element);
            DIAG_LOG(L"[diag] win StartItemTranslationAnimation from=%.1f,%.1f "
                     L"to=%.1f,%.1f %s %s",
                     from->x, from->y, to->x, to->y,
                     winrt::get_class_name(target).c_str(),
                     winrt::Windows::UI::Xaml::Automation::AutomationProperties::
                         GetName(target)
                             .c_str());
        } catch (...) {
        }
    }
    SharedAnimations_StartItemTranslationAnimation_Original(pThis, element,
                                                            from, to);
}

using TaskbarFrame_SetRepositionAnimations_t = void(__cdecl*)(void* pThis);
TaskbarFrame_SetRepositionAnimations_t
    TaskbarFrame_SetRepositionAnimations_Original;
void __cdecl TaskbarFrame_SetRepositionAnimations_Hook(void* pThis) {
    DIAG_LOG(L"[diag] win SetRepositionAnimations");
    TaskbarFrame_SetRepositionAnimations_Original(pThis);
}

using TaskListButton_UpdateTransitions_t = void(__cdecl*)(void* pThis,
                                                          bool enabled,
                                                          int kind);
TaskListButton_UpdateTransitions_t TaskListButton_UpdateTransitions_Original;
void __cdecl TaskListButton_UpdateTransitions_Hook(void* pThis,
                                                   bool enabled,
                                                   int kind) {
    DIAG_LOG(L"[diag] win UpdateTransitions this=%p enabled=%d kind=%d", pThis,
             (int)enabled, kind);
    TaskListButton_UpdateTransitions_Original(pThis, enabled, kind);
}

using TaskListButton_SetTransitionAnimations_t = void(__cdecl*)(void* pThis);
TaskListButton_SetTransitionAnimations_t
    TaskListButton_SetTransitionAnimations_Original;
void __cdecl TaskListButton_SetTransitionAnimations_Hook(void* pThis) {
    DIAG_LOG(L"[diag] win SetTransitionAnimations this=%p", pThis);
    TaskListButton_SetTransitionAnimations_Original(pThis);
}

using TaskListButton_put_TransitionsFrozen_t = int(__cdecl*)(void* pThis,
                                                             bool frozen);
TaskListButton_put_TransitionsFrozen_t
    TaskListButton_put_TransitionsFrozen_Original;
int __cdecl TaskListButton_put_TransitionsFrozen_Hook(void* pThis,
                                                      bool frozen) {
    DIAG_LOG(L"[diag] win TransitionsFrozen abi=%p frozen=%d", pThis,
             (int)frozen);
    return TaskListButton_put_TransitionsFrozen_Original(pThis, frozen);
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

    // Nothing registered: the wake reached no one; nudge discovery.
    bool anyFrames;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        anyFrames = !g_frames->empty();
    }
    if (!anyFrames) {
        NudgeTaskbarsForDiscovery();
    }
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
        // One log so a build where IAppVisibility goes silent is diagnosable.
        static std::atomic<bool> seen = false;
        if (!seen.exchange(true)) {
            Wh_Log(L"Start visibility events active");
        }
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
    PeekMessageW(&queuePrimer, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
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

    // DIAGNOSTIC BUILD ONLY — strip before submitting.
    bool dumpRegistered = RegisterHotKey(
        nullptr, 2, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'D');
    Wh_Log(L"Highlight dump hotkey Ctrl+Alt+D registered=%d",
           (int)dumpRegistered);

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
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == 1) {
            ToggleCollapse();
        } else if (msg.message == WM_HOTKEY && msg.wParam == 2) {
            RequestHighlightDumpAsync();
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
    if (dumpRegistered) {
        UnregisterHotKey(nullptr, 2);
    }

    winrt::uninit_apartment();
    return 0;
}

void StartHotkeyThread() {
    // try_lock, never block: this runs from RegisterFrame, i.e. inside a
    // taskbar layout pass, and a settings save holds this mutex across an
    // unbounded join. Opportunistic — the next registration retries.
    std::unique_lock<std::mutex> guard(g_hotkeyThreadMutex, std::try_to_lock);
    if (!guard.owns_lock()) {
        return;
    }
    // The unloading check closes the race with StopHotkeyThread: a start
    // that lost this lock to unload must not spawn into a dying image.
    if (g_hotkeyThread || g_unloading) {
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

    g_hotkeyThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
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
    PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
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
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

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

    HHOOK hook = SetWindowsHookExW(
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
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

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
    while ((hPrimary = FindWindowExW(nullptr, hPrimary, L"Shell_TrayWnd",
                                    nullptr))) {
        consider(hPrimary);
    }
    HWND hSecondary = nullptr;
    while ((hSecondary = FindWindowExW(nullptr, hSecondary,
                                      L"Shell_SecondaryTrayWnd", nullptr))) {
        consider(hSecondary);
    }

    for (HWND hWnd : windows) {
        RunFromWindowThread(hWnd, proc, param);
    }
}

// Kicks TrayUI::_HandleSettingChange down its DPI path (the taskbar-icon-size
// pattern), whose recompute re-measures the XAML frame and fires the hooked
// MeasureOverride — discovery for taskbars idle since before the mod loaded.
void NudgeTaskbarsForDiscovery() {
    auto nudge = [](HWND hWnd) {
        DWORD processId = 0;
        if (GetWindowThreadProcessId(hWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            SendMessageTimeoutW(hWnd, WM_SETTINGCHANGE,
                                SPI_SETLOGICALDPIOVERRIDE, 0,
                                SMTO_ABORTIFHUNG, 1000, nullptr);
        }
    };
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, L"Shell_TrayWnd", nullptr))) {
        nudge(hWnd);
    }
    hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, L"Shell_SecondaryTrayWnd",
                                 nullptr))) {
        nudge(hWnd);
    }
}

void CleanupOnThisThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<FrameContext> mine;

    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto it = g_frames->begin(); it != g_frames->end();) {
            if (it->second.threadId == threadId) {
                mine.push_back(std::move(it->second));
                it = g_frames->erase(it);
            } else {
                ++it;
            }
        }
    }

    // Outside the lock: XAML teardown can re-enter hooks that take it. The
    // contexts die here, on their owning thread. Runs inside a CALLWNDPROC
    // hook or dispatcher lambda; an escaping throw is fatal or hangs unload.
    for (auto& ctx : mine) {
        try {
            if (ctx.timer) {
                ctx.timer.Stop();
                ctx.timer.Tick(ctx.tickToken);
            }
            RestoreFrame(ctx);
        } catch (winrt::hresult_error const&) {
        }
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
    g_settings.snappinessPct =
        std::clamp(Wh_GetIntSetting(L"SnappinessPct"), 0, 100);

    auto mode = WindhawkUtils::StringSetting::make(L"AnimationMode");
    g_settings.animationMode = wcscmp(mode.get(), L"slide") == 0
                                   ? AnimationMode::Slide
                                   : AnimationMode::None;

    auto curve = WindhawkUtils::StringSetting::make(L"AnimationCurve");
    g_settings.animationCurve = CurveId::Sine;
    if (wcscmp(curve.get(), L"cubic") == 0) {
        g_settings.animationCurve = CurveId::Cubic;
    } else if (wcscmp(curve.get(), L"circ") == 0) {
        g_settings.animationCurve = CurveId::Circ;
    }

    g_diagLog = Wh_GetIntSetting(L"DiagLog") != 0;
    g_diagStray = Wh_GetIntSetting(L"DiagStray") != 0;

    g_collapseEnabled = Wh_GetIntSetting(L"Collapsed") != 0;
    g_revealed = false;
    g_revealedByStart = false;
    g_emptyHoverSinceTick = 0;
    g_lastPointerQualified = false;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
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
        // DIAG hooks, log only.
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::SharedAnimations::StartItemTranslationAnimation(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::Foundation::Numerics::float3,struct winrt::Windows::Foundation::Numerics::float3)const )",
             LR"(public: void __cdecl winrt::Taskbar::implementation::SharedAnimations::StartItemTranslationAnimation(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::Foundation::Numerics::float3,struct winrt::Windows::Foundation::Numerics::float3)const)"},
            &SharedAnimations_StartItemTranslationAnimation_Original,
            SharedAnimations_StartItemTranslationAnimation_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SetRepositionAnimations(void))"},
            &TaskbarFrame_SetRepositionAnimations_Original,
            TaskbarFrame_SetRepositionAnimations_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateTransitions(bool,enum winrt::Taskbar::TaskbarButtonTransitionKind))"},
            &TaskListButton_UpdateTransitions_Original,
            TaskListButton_UpdateTransitions_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::SetTransitionAnimations(void))"},
            &TaskListButton_SetTransitionAnimations_Original,
            TaskListButton_SetTransitionAnimations_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskbarButton>::put_TransitionsFrozen(bool))"},
            &TaskListButton_put_TransitionsFrozen_Original,
            TaskListButton_put_TransitionsFrozen_Hook,
            true,
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
    Wh_Log(L"Diag hooks (translate=%d reposition=%d updateTransitions=%d "
           L"setTransitions=%d frozen=%d)",
           (int)(SharedAnimations_StartItemTranslationAnimation_Original !=
                 nullptr),
           (int)(TaskbarFrame_SetRepositionAnimations_Original != nullptr),
           (int)(TaskListButton_UpdateTransitions_Original != nullptr),
           (int)(TaskListButton_SetTransitionAnimations_Original != nullptr),
           (int)(TaskListButton_put_TransitionsFrozen_Original != nullptr));
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
        // The exchange makes this exactly-once; no second flag needed. The
        // hotkey thread is NOT started here: this runs under the loader
        // lock, and a new thread blocks on it. RegisterFrame starts it.
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // Sticky reveal from before the last unload or restart.
    g_revealed = Wh_GetIntValue(L"RevealedState", 0) != 0;
    g_lastSavedRevealed = g_revealed ? 1 : 0;

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
        // Only the shell explorer loads Taskbar.View.dll; other instances
        // must not grab the hotkey or the Start watcher.
        StartHotkeyThread();
    } else {
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
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
    if (!g_taskbarViewDllLoaded) {
        // Retry for a load that raced the LoadLibraryExW hook going live.
        HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
        if (taskbarViewModule && !g_taskbarViewDllLoaded.exchange(true)) {
            if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                Wh_ApplyHookOperations();
                StartHotkeyThread();
            }
        }
    }

    // An already-idle taskbar fires no hook on its own.
    if (g_taskbarViewDllLoaded) {
        NudgeTaskbarsForDiscovery();
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

    // Nothing registered yet: the wake reached no one; nudge discovery.
    bool anyFrames;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        anyFrames = !g_frames->empty();
    }
    if (!anyFrames && g_taskbarViewDllLoaded) {
        NudgeTaskbarsForDiscovery();
    }
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
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!done) {
            CloseHandle(hThread);
            continue;
        }
        bool queued = false;
        {
            // Signalled on delegate DESTRUCTION, even if dropped uninvoked.
            auto signal = std::shared_ptr<void>(
                nullptr, [done](void*) { SetEvent(done); });
            // Bounded retry: a transient failure gets more chances, but a
            // dispatcher that is permanently shut down must not spin here.
            // Its thread cannot run timer ticks either, so giving up is safe.
            for (int attempt = 0; attempt < 5 && !queued; attempt++) {
                if (attempt && WaitForSingleObject(hThread, 10) ==
                                   WAIT_OBJECT_0) {
                    break;
                }
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
            if (!queued) {
                Wh_Log(L"Straggler cleanup could not be queued");
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

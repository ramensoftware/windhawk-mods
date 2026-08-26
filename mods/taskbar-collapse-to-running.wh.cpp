// ==WindhawkMod==
// @id              taskbar-collapse-to-running
// @name            Taskbar collapse to running apps
// @description     Hides pinned taskbar icons whose app is not running. Hover, rest on, or click the empty taskbar to reveal them.
// @version         1.0.0
// @author          Lars
// @github          https://github.com/LarsGudm
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

## Running detection

Two signals, either one keeps an icon visible: the button's accessibility name
("… 1 running window") and the running-indicator element. Non-English Windows:
change **Running-app text in button names**, or clear it.

## Notes

* Windows 11 only. Win+number still counts pinned items the way Windows does.
* After 2 quiet minutes checks drop to the sleep rate; any taskbar activity
  wakes it instantly.
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
  - hover: Hover
  - rest: Rest
  - click: Click
  - never: None, hotkey only
- RevealOnStart: false
  $name: Reveal while Start is open
  $description: >-
    Opening Start, by the Windows key or the Start button, shows every icon and
    keeps them shown until Start closes. Works with any reveal trigger and
    wakes the mod from sleep.
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
- RefreshIntervalMs: 250
  $name: Refresh interval (ms)
  $description: >-
    How often running state and cursor position are re-checked. Whether an app
    is running changes at human speed, so this can be slow; it only matters
    while a reveal, dwell or animation is being timed. 10 to 2000.
- SleepEnabled: true
  $name: Sleep when idle
  $description: >-
    After 2 minutes with no taskbar activity, drop to the sleep rate below.
    Touching the taskbar wakes it instantly, so nothing feels slower.
- SleepIntervalMs: 1000
  $name: Sleep check rate (ms)
  $description: How often to check while asleep. 500 to 3000.
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
- RunningNameMarker: running window
  $name: Running-app text in button names
  $description: >-
    A button whose accessibility name contains this text counts as running,
    whatever its icons look like. Windows writes names such as
    "Notepad - 1 running window pinned". Change this if your Windows is not in
    English, or clear it to rely on the running indicator alone. A pinned app
    whose own name contains this text always counts as running.
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
#include <winrt/Windows.UI.Xaml.Automation.h>
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

// Quiet time on the taskbar before the tick drops to the sleep rate.
constexpr ULONGLONG kSleepAfterMs = 2 * 60 * 1000;

// Named element Windows creates inside a task button only while its app runs.
constexpr PCWSTR kRunningIndicatorName = L"RunningIndicator";

// A CSS cubic-bezier(x1, y1, x2, y2) curve; endpoints are fixed at 0 and 1.
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

// Settings store the curve as an id, not the struct: a settings change on
// another thread must not be readable half-updated mid-animation.
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

// Accordion is the reveal/collapse transition. GapClose handles an icon
// hidden mid-steady-state, when its app closes: the icon is faded but keeps
// its slot for the first half, so layout itself holds the gap and there are
// no translations for the taskbar to fight over, then the midpoint collapses
// it and the row eases shut.
enum class AnimKind { Accordion, GapClose };

// Scalars are atomic: LoadSettings writes them on Windhawk's thread while
// taskbar threads read them mid-tick.
struct {
    std::atomic<RevealTrigger> revealTrigger;
    std::atomic<bool> revealOnStart;
    std::atomic<bool> collapsed;
    std::atomic<int> restSpeedPxPerSec;
    std::atomic<int> revealDelayMs;
    std::atomic<int> elementPaddingPx;
    std::atomic<int> hoverGraceMs;
    std::atomic<int> refreshIntervalMs;
    std::atomic<bool> sleepEnabled;
    std::atomic<int> sleepIntervalMs;
    std::atomic<AnimationMode> animationMode;
    std::atomic<CurveId> animationCurve;
    std::atomic<int> animationDurationMs;
    std::atomic<int> animationAmplitudePct;
    std::wstring runningNameMarker;
} g_settings;

// Guards runningNameMarker only.
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading = false;
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_hooksApplied = false;

// Whether icons should be collapsed; the settings checkbox and the hotkey both write it.
std::atomic<bool> g_collapseEnabled = true;

// Whether hover is currently holding every icon visible.
std::atomic<bool> g_revealed = false;

// Whether the current reveal was caused by the Start menu opening; it pins the
// reveal against grace aging until Start closes.
std::atomic<bool> g_revealedByStart = false;

// Tick when the cursor was last over a taskbar; the grace period counts from here.
std::atomic<ULONGLONG> g_lastCursorInsideTick = 0;

// Tick when the cursor entered the empty area, or 0 when it is not there.
std::atomic<ULONGLONG> g_emptyHoverSinceTick = 0;

// Tick of the last taskbar activity (pointer, layout, hotkey, settings); the
// sleep countdown runs from here.
std::atomic<ULONGLONG> g_lastActivityTick = 0;

// True while ticks run at the sleep rate; the pointer hook uses it to snap
// the timer back to full speed on the first touch.
std::atomic<bool> g_sleeping = false;

// Rest trigger: cursor speed in px/s, measured between refresh ticks from
// screen coordinates, and whether the last pointer event sat in the
// qualifying empty area. Together they let a parked cursor finish a reveal
// after pointer events have stopped coming.
std::atomic<double> g_cursorSpeedPxS = 0;
std::atomic<bool> g_lastPointerQualified = false;
double g_speedSamplePrevMs = 0;
POINT g_speedSamplePrevPos = {};

// Bumped by the hotkey and settings changes; a context seeing a new value
// applies once without animation. Lets other threads request an instant apply
// without touching contexts or blocking on their message queues.
std::atomic<uint64_t> g_instantApplyGen = 0;

// Wakes queued to taskbar dispatchers that have not run yet; unload waits for
// zero so no queued lambda outlives the DLL.
std::atomic<int> g_pendingWakes = 0;

// Start-menu callbacks currently executing; teardown waits for zero after
// Unadvise, which does not fence a callback already in flight.
std::atomic<int> g_sinkCallbacks = 0;

HANDLE g_hotkeyThread = nullptr;
DWORD g_hotkeyThreadId = 0;
// Signalled once the worker owns a message queue, so WM_QUIT cannot be posted
// before one exists and be dropped.
HANDLE g_hotkeyThreadReady = nullptr;

// Per-button state: the animations stripped from it, the originals to put
// back, and its resolved running indicator so the tick need not re-walk the
// subtree. The indicator is created lazily by Windows, so a null cache entry
// is re-resolved rather than trusted.
struct DeanimatedButton {
    winrt::weak_ref<FrameworkElement> button;
    Media::Animation::TransitionCollection originalTransitions{nullptr};
    winrt::Windows::UI::Composition::ImplicitAnimationCollection
        originalImplicit{nullptr};
    winrt::weak_ref<FrameworkElement> runningIndicator;
};

// State for one taskbar, touched only by that taskbar's own UI thread, except
// dispatcher, which other threads copy under g_framesMutex to queue wakes.
struct FrameContext {
    // This context's key in g_frames, so callbacks can find their way back.
    void* key = nullptr;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    winrt::weak_ref<FrameworkElement> frame;
    winrt::weak_ref<FrameworkElement> repeater;
    DispatcherTimer timer{nullptr};
    winrt::event_token tickToken{};
    DWORD threadId = 0;
    int intervalMs = 0;
    // Buttons this mod hid, and therefore the only ones it may show again.
    std::vector<winrt::weak_ref<FrameworkElement>> hiddenByUs;
    // Last seen buttons, for the pointer hook's padding test.
    std::vector<winrt::weak_ref<FrameworkElement>> lastButtons;
    std::vector<DeanimatedButton> deanimated;

    // Visibility state currently on screen; -1 until the first apply.
    int appliedCollapse = -1;
    bool animActive = false;
    AnimKind animKind = AnimKind::Accordion;
    // Collapse state the running animation is heading towards.
    bool animTargetCollapse = false;
    double animStartMs = 0;
    // Amplitude for this run, snapshotted at StartAnimation so both halves
    // of the accordion stretch by the same width.
    double animAmplitude = 0;
    // Whether the midpoint icon swap has happened yet.
    bool animSwapped = false;
    // GapClose only: offsets back to pre-collapse positions, measured at the
    // midpoint and eased to zero over the second half.
    std::vector<float> animGapOffsets;
    // GapClose only: buttons faded out but not yet collapsed, with their
    // original opacities; emptied when the midpoint collapses them for real.
    std::vector<std::pair<FrameworkElement, double>> pendingHide;
    // Strong refs for the animation's few hundred ms, so every frame works on
    // the same set without re-walking the tree. Emptied when it stops.
    std::vector<FrameworkElement> animButtons;
    // Width the idle icons occupy, measured while they are visible. Hidden
    // icons measure zero, so this cache is what the amplitude scales from.
    double idleWidth = 0;
    winrt::event_token renderingToken{};
    bool renderingHooked = false;

    // Last g_instantApplyGen this context consumed.
    uint64_t instantGenSeen = 0;
};

std::mutex g_framesMutex;

// Never destroyed: the contexts hold UI-thread-affine XAML objects, and at
// process shutdown the CRT would release them from the wrong thread with the
// XAML core already gone. Real release happens in CleanupOnThisThread.
[[clang::no_destroy]] std::optional<std::unordered_map<void*, FrameContext>>
    g_frames{std::in_place};

FrameContext* GetFrameContext(void* key);

// Millisecond clock for animation timing. GetTickCount64's ~16 ms quantum is
// too coarse for a ~120 ms animation and reads as stutter.
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

std::wstring CopyRunningMarker() {
    std::lock_guard<std::mutex> guard(g_settingsMutex);
    return g_settings.runningNameMarker;
}

// ---------------------------------------------------------------- visual tree

// Calls back for each direct child, stopping at the first callback that returns true.
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

// Single walk finding the indicator: an exact RunningIndicator wins, any other
// name containing "Running" is the fallback. Do not widen the fallback to
// "Indicator": it would match ProgressIndicator and misread idle apps.
void FindRunningIndicatorWalk(FrameworkElement element,
                              int depth,
                              FrameworkElement& exact,
                              FrameworkElement& partial) {
    if (!element || depth < 0 || exact) {
        return;
    }

    EnumChildElements(element, [&](FrameworkElement child) {
        auto nameHstring = child.Name();
        std::wstring_view name(nameHstring);
        if (name == kRunningIndicatorName) {
            exact = child;
            return true;
        }
        if (!partial && name.find(L"Running") != std::wstring_view::npos) {
            partial = child;
        }
        FindRunningIndicatorWalk(child, depth - 1, exact, partial);
        return (bool)exact;
    });
}

FrameworkElement FindRunningIndicator(FrameworkElement button) {
    FrameworkElement exact = nullptr;
    FrameworkElement partial = nullptr;
    FindRunningIndicatorWalk(button, 6, exact, partial);
    return exact ? exact : partial;
}

// Collects app buttons, excluding the panel classes nested inside each one.
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

bool ContainsNoCase(std::wstring_view haystack, std::wstring_view needle) {
    if (needle.empty() || needle.size() > haystack.size()) {
        return false;
    }

    auto it = std::search(haystack.begin(), haystack.end(), needle.begin(),
                          needle.end(), [](wchar_t a, wchar_t b) {
                              return towlower(a) == towlower(b);
                          });
    return it != haystack.end();
}

// True while the indicator element exists and is positively shown. The caller
// passes its cached indicator, and gets back whatever was resolved so the
// subtree walk happens once per button rather than once per tick.
bool IndicatorSaysRunning(FrameworkElement button,
                          FrameworkElement& indicatorInOut) {
    FrameworkElement indicator = indicatorInOut;
    if (!indicator) {
        indicator = FindRunningIndicator(button);
        indicatorInOut = indicator;
    }
    if (!indicator) {
        return false;
    }
    if (indicator.Visibility() != Visibility::Visible) {
        return false;
    }
    if (indicator.Opacity() <= 0.01) {
        return false;
    }

    // Size is a layout result and collapsed buttons are never laid out. Do not
    // drop the laid-out guard: a hidden button could then never report running
    // again, and would stay hidden for as long as its app lives.
    bool buttonLaidOut = button.Visibility() == Visibility::Visible &&
                         button.ActualWidth() > 0.5;
    if (buttonLaidOut &&
        (indicator.ActualWidth() <= 0.5 || indicator.ActualHeight() <= 0.5)) {
        return false;
    }

    return true;
}

// True if either the indicator element or the accessibility name says running.
// Do not make this an AND: hiding must require both signals to agree, or a
// running app disappears from the taskbar. Indicator goes first so the name,
// which allocates a string, is only fetched for idle-looking buttons.
bool IsButtonRunning(FrameworkElement button,
                     std::wstring_view marker,
                     FrameworkElement& indicatorInOut) {
    if (IndicatorSaysRunning(button, indicatorInOut)) {
        return true;
    }

    if (!marker.empty()) {
        auto name = Automation::AutomationProperties::GetName(button);
        if (ContainsNoCase(std::wstring_view(name), marker)) {
            return true;
        }
    }

    return false;
}

// ------------------------------------------------------------------- applying

// Reports whether the set holds this element, dropping dead entries as it goes,
// and removes the match when remove is set.
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

    // Jump lists and thumbnail previews are separate popup windows, but they
    // live on the taskbar's own UI thread. They count as taskbar, or the grace
    // period would collapse the bar under a menu the user is navigating.
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

// Clears the implicit show/hide animations, which otherwise keep rendering a
// hidden button's ghost until their farewell animation finishes. They have no
// getter, so this cannot be undone; call it only for buttons actually being
// hidden, never for every button on every pass.
void ClearImplicitShowHide(FrameworkElement const& button) {
    try {
        Hosting::ElementCompositionPreview::SetImplicitShowAnimation(button,
                                                                     nullptr);
        Hosting::ElementCompositionPreview::SetImplicitHideAnimation(button,
                                                                     nullptr);
    } catch (winrt::hresult_error const&) {
    }
}

// Clears both animation paths on a button: the XAML transition and the
// composition implicit animations, keeping the originals for ReanimateButtons.
void DeanimateButton(FrameContext& ctx, FrameworkElement button) {
    // Runs every pass. Do not reduce this to strip-once-per-button: Windows
    // installs a button's animations at its own pace, and one stripped too
    // early keeps its reorder slide forever. The ledger exists only so
    // unloading can put the originals back; dead entries are pruned here.
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
        // Animations that appeared after first sight still belong to Windows;
        // capture them so unload restores the real originals.
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

// Maps linear progress through a CSS-style cubic-bezier curve.
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

// Spreads totalExtra across the gaps between visible buttons, holding the
// leftmost one still. Windows re-centres the group itself, so the group is
// deliberately not shifted to compensate: the correction would dwarf the
// amplitude and read as the whole row charging across the screen.
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

// Hides or shows the managed buttons, recording only the ones it hid itself.
// With deferHide set, a button to hide is faded in place instead and handed
// back for FinalizePendingHides to collapse at the animation midpoint.
void SetCollapseState(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    bool collapse,
    std::vector<std::pair<FrameworkElement, double>>* deferHide = nullptr) {
    std::wstring marker = CopyRunningMarker();
    double idleVisibleWidth = 0;
    bool idleSetComplete = true;

    for (auto const& button : buttons) {
        bool weHidIt = TakeFromHiddenSet(ctx.hiddenByUs, button, false);

        DeanimatedButton* state = nullptr;
        for (auto& entry : ctx.deanimated) {
            if (entry.button.get() == button) {
                state = &entry;
                break;
            }
        }

        FrameworkElement indicator =
            state ? state->runningIndicator.get() : nullptr;
        bool running = IsButtonRunning(button, marker, indicator);
        if (state && indicator && !state->runningIndicator.get()) {
            state->runningIndicator = winrt::make_weak(indicator);
        }

        bool shouldHide = collapse && !running;

        // Measured before any visibility flip below, while layout is real. A
        // pass that sees an already-hidden idle button measures a partial
        // set, which must not overwrite the full-set cache.
        if (!running) {
            if (button.Visibility() == Visibility::Visible) {
                idleVisibleWidth += button.ActualWidth();
            } else {
                idleSetComplete = false;
            }
        }

        if (shouldHide) {
            // Buttons Windows already hid stay unrecorded, so they are never
            // restored on this mod's behalf. A recorded hide is re-asserted
            // in case the taskbar re-showed it, or it could stick as visible
            // forever.
            if (button.Visibility() == Visibility::Visible) {
                if (deferHide) {
                    deferHide->emplace_back(button, button.Opacity());
                    button.Opacity(0);
                } else {
                    // Re-stripped at the moment it matters: Windows can have
                    // re-installed the hide animation since first sight, and
                    // its ghost is what neighbours would slide across.
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

// Tree order is not visual order: the taskbar's repeater recycles elements,
// so a newly pinned app can land anywhere in the child list. Spacing is dealt
// out by list index, so the list is sorted by on-screen position; hidden
// buttons sink to the back and get re-sorted after the swap shows them.
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

// Completes deferred hides: restores each faded button's opacity and
// collapses it for real, recording it as ours to restore unless Windows got
// to it first.
void FinalizePendingHides(FrameContext& ctx) {
    for (auto& entry : ctx.pendingHide) {
        try {
            entry.first.Opacity(entry.second);
            if (entry.first.Visibility() == Visibility::Visible) {
                ClearImplicitShowHide(entry.first);
                entry.first.Visibility(Visibility::Collapsed);
                if (!TakeFromHiddenSet(ctx.hiddenByUs, entry.first, false)) {
                    ctx.hiddenByUs.push_back(winrt::make_weak(entry.first));
                }
            }
        } catch (winrt::hresult_error const&) {
        }
    }
    ctx.pendingHide.clear();
}

// Stops any animation; deferred hides complete instantly rather than revert,
// since the hidden state is where the interrupted animation was heading.
void StopAnimation(FrameContext& ctx) {
    FinalizePendingHides(ctx);
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

// Midpoint of a gap close: freezes every visible survivor at its pre-collapse
// position. Positions are sampled before layout runs, layout is forced, and
// the per-button offsets back to the old spots are applied immediately, so
// this very frame already holds. Offsets are measured, before layout against
// after, so any taskbar alignment and any number of simultaneous gaps come
// out right.
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

// Begins the two-phase gap close for buttons hidden mid-steady-state. They
// are already faded by SetCollapseState and the row has not moved; the
// rendering tick collapses them at the midpoint and eases the gap shut.
void StartGapCloseAnimation(
    FrameContext& ctx,
    std::vector<FrameworkElement> const& buttons,
    std::vector<std::pair<FrameworkElement, double>> pendingHide) {
    ctx.animButtons = buttons;
    ctx.animGapOffsets.clear();
    ctx.pendingHide = std::move(pendingHide);
    ctx.animActive = true;
    ctx.animKind = AnimKind::GapClose;
    // Matches the applied state so the tick keeps its hands off while it runs.
    ctx.animTargetCollapse = ctx.appliedCollapse == 1;
    ctx.animStartMs = NowMs();
    ctx.animSwapped = false;
    HookRendering(ctx, ctx.key);
}

// Brings one taskbar in line with the requested collapse state. Never runs
// while an animation is active; OnTimerTick stops the animation first.
// Reports whether it could act, so a pending instant request survives an
// empty button walk during a taskbar rebuild.
bool ApplyToFrame(FrameContext& ctx, bool collapse, bool instantRequested) {
    auto frame = ctx.frame.get();
    if (!frame) {
        return false;
    }

    // The buttons' container, remembered from the last successful walk, keeps
    // the collection off the full-tree search.
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
            // Collapse is switched off and nothing is left hidden: the
            // taskbar gets its own animations back while the mod idles.
            // Stripping resumes the moment collapse returns. A mere reveal
            // keeps stripping, since a collapse can follow at any moment.
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

    // Steady-state hides, an app closing while collapsed, close their gap
    // with the easing instead of snapping.
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

// Drives one frame of the accordion. Spacing is the only thing animated.
//
// One easing curve spans the whole duration and the icon swap sits at its
// midpoint, which is its steepest part. Revealing, the collapsed set stretches
// open, the full set cuts in squeezed shut, then relaxes to its natural
// spacing. Collapsing runs that backwards: the full set squeezes shut, the
// collapsed set cuts in stretched open, then closes to its natural spacing.
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
        if (t >= 1.0) {
            StopAnimation(*ctx);
            return;
        }
        // First half: the doomed buttons are faded but still laid out, so
        // layout itself holds the gap and Windows' own close flourish plays
        // out invisibly, with no translations to fight over. The midpoint
        // collapses them for real; the second half eases the survivors home.
        if (t < 0.5) {
            return;
        }
        if (!ctx->animSwapped) {
            ctx->animSwapped = true;
            FinalizePendingHides(*ctx);
            if (auto frame = ctx->frame.get()) {
                MeasureGapOffsets(*ctx, frame);
            }
            if (ctx->animGapOffsets.empty()) {
                StopAnimation(*ctx);
                return;
            }
        }
        double closeT = (t - 0.5) * 2.0;
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
        // The visibility flip can prompt Windows to install animations while
        // the reconcile tick is paused, so strip again right here.
        for (auto& button : ctx->animButtons) {
            DeanimateButton(*ctx, button);
        }
        // Forcing layout gives the new visible set real positions now, so the
        // re-sort and this frame's offsets land in true left-to-right order,
        // and a reveal's just-shown icons get their widths into the cache.
        if (auto frame = ctx->frame.get()) {
            frame.UpdateLayout();
            SortButtonsByPosition(ctx->animButtons, frame);
            if (!ctx->animTargetCollapse) {
                std::wstring marker = CopyRunningMarker();
                double width = 0;
                for (auto& button : ctx->animButtons) {
                    FrameworkElement indicator = nullptr;
                    if (button.Visibility() == Visibility::Visible &&
                        !IsButtonRunning(button, marker, indicator)) {
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

    // Amplitude scales with the width the hidden icons actually occupy, so the
    // animation adapts to how many icons are pinned. Snapshotted at
    // StartAnimation so both halves stretch by one value; the swap only
    // rewrites idleWidth for later runs.
    double amplitude = ctx->animAmplitude;

    if (t >= 1.0) {
        StopAnimation(*ctx);
        return;
    }

    // Collapsing is the reveal played backwards, so every offset flips sign.
    double direction = ctx->animTargetCollapse ? -1.0 : 1.0;
    double totalExtra = ctx->animSwapped
                            ? -direction * amplitude * (1 - eased)
                            : direction * amplitude * eased;

    ApplySpacing(ctx->animButtons, totalExtra);
}

// Puts one taskbar back exactly as it was found.
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

// Drives one taskbar: ages out the hover reveal, then applies the result.
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
        // The rendering hook must go too, or its handler outlives the entry,
        // forces a render every vsync, and dangles after the mod unloads.
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

    // The fast rate only buys anything while something is timing out: an
    // animation, a dwell being counted, or a hover reveal aging on its grace
    // period. A click reveal is stable, so it does not hold the rate up.
    bool timingSomething = ctx->animActive || g_emptyHoverSinceTick != 0 ||
                           (g_revealed && hoverLike && !g_revealedByStart);
    bool sleep = g_settings.sleepEnabled && !timingSomething &&
                 now - g_lastActivityTick >= kSleepAfterMs;
    g_sleeping = sleep;

    int wantedIntervalMs = (timingSomething || !sleep)
                               ? g_settings.refreshIntervalMs.load()
                               : g_settings.sleepIntervalMs.load();
    if (ctx->intervalMs != wantedIntervalMs) {
        ctx->intervalMs = wantedIntervalMs;
        ctx->timer.Interval(std::chrono::milliseconds(wantedIntervalMs));
    }

    // Cursor speed from screen-space deltas between ticks. The dt guard keeps
    // a second taskbar's tick in the same instant from producing garbage.
    if (trigger == RevealTrigger::Rest) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            double nowMs = NowMs();
            double dt = nowMs - g_speedSamplePrevMs;
            if (g_speedSamplePrevMs > 0 && dt >= 5.0) {
                double dx = (double)(pt.x - g_speedSamplePrevPos.x);
                double dy = (double)(pt.y - g_speedSamplePrevPos.y);
                g_cursorSpeedPxS = sqrt(dx * dx + dy * dy) * 1000.0 / dt;
            }
            if (dt >= 5.0 || g_speedSamplePrevMs == 0) {
                g_speedSamplePrevMs = nowMs;
                g_speedSamplePrevPos = pt;
            }
        }
    }

    if (g_revealed) {
        if (!g_collapseEnabled ||
            (trigger == RevealTrigger::Never && !g_revealedByStart)) {
            g_revealed = false;
            g_revealedByStart = false;
            g_emptyHoverSinceTick = 0;
        } else if (g_revealedByStart) {
            // Pinned while Start is open; the worker thread ends it on close.
            // Keeping the grace timestamp fresh hands over cleanly if the
            // cursor is on the taskbar when Start goes away.
            g_lastCursorInsideTick = now;
        } else if (hoverLike) {
            // A click reveal stays until it is clicked away, so only hovering
            // ages out on the grace period.
            if (CursorOverAnyTaskbar()) {
                g_lastCursorInsideTick = now;
            } else if (now - g_lastCursorInsideTick >
                       (ULONGLONG)g_settings.hoverGraceMs) {
                g_revealed = false;
                g_emptyHoverSinceTick = 0;
            }
        }
    } else if (hoverLike && g_collapseEnabled) {
        if (trigger == RevealTrigger::Rest) {
            if (g_cursorSpeedPxS > (double)g_settings.restSpeedPxPerSec) {
                // Moving too fast resets the dwell; resting time is continuous.
                g_emptyHoverSinceTick = 0;
            } else if (g_emptyHoverSinceTick == 0 && g_lastPointerQualified &&
                       CursorOverAnyTaskbar()) {
                // A parked cursor produces no pointer events, so the dwell is
                // armed here, trusting the last event's position check.
                g_emptyHoverSinceTick = now;
            }
        }

        // Completes a reveal for a cursor that has stopped moving and so no
        // longer produces the pointer events that would finish the delay.
        ULONGLONG since = g_emptyHoverSinceTick;
        if (since != 0) {
            if (!CursorOverAnyTaskbar()) {
                g_emptyHoverSinceTick = 0;
            } else if (now - since >= (ULONGLONG)g_settings.revealDelayMs) {
                g_revealed = true;
                g_emptyHoverSinceTick = 0;
            }
        }
    }

    bool desired = g_collapseEnabled && !g_revealed;

    // At most one pending instant-apply request, consumed on a successful
    // apply so an empty button walk does not swallow it.
    uint64_t instantGen = g_instantApplyGen;
    bool instant = ctx->instantGenSeen != instantGen;

    // While the accordion runs toward the right state, leave it alone: the
    // rendering callback owns the buttons, and reconciliation here would fight
    // it for them. A flipped target or an instant request stops it first.
    if (ctx->animActive) {
        if (!instant && desired == ctx->animTargetCollapse) {
            return;
        }
        StopAnimation(*ctx);
    }

    if (ApplyToFrame(*ctx, desired, instant)) {
        ctx->instantGenSeen = instantGen;
    }
}

// Adopts a taskbar and starts its timer; runs on that taskbar's UI thread.
void RegisterFrame(void* key, FrameworkElement frame) {
    std::lock_guard<std::mutex> guard(g_framesMutex);

    // Checked under the lock: a hook that passed its unlocked g_unloading
    // check must not insert a live timer after the cleanup sweeps ran.
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
    ctx.intervalMs = g_settings.refreshIntervalMs;

    ctx.timer = DispatcherTimer();
    ctx.timer.Interval(std::chrono::milliseconds(ctx.intervalMs));
    ctx.tickToken = ctx.timer.Tick(
        [key](IInspectable const&, IInspectable const&) { OnTimerTick(key); });
    ctx.timer.Start();

    (*g_frames)[key] = ctx;

    Wh_Log(L"Registered taskbar frame on thread %u", ctx.threadId);
}

bool IsFrameRegistered(void* key) {
    std::lock_guard<std::mutex> guard(g_framesMutex);
    return g_frames->find(key) != g_frames->end();
}

// Map entries keep a stable address and each context belongs to a single UI
// thread, so the returned pointer stays valid after the lock is released.
FrameContext* GetFrameContext(void* key) {
    std::lock_guard<std::mutex> guard(g_framesMutex);
    auto it = g_frames->find(key);
    return it == g_frames->end() ? nullptr : &it->second;
}

// Applies to the taskbars owned by the calling thread, skipping the tick's wait.
void ApplyOnThisThreadNow() {
    std::vector<void*> keys;
    DWORD threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.threadId == threadId) {
                keys.push_back(key);
            }
        }
    }

    for (void* key : keys) {
        OnTimerTick(key);
    }
}

// Queues a wake on every taskbar's own UI thread. Non-blocking, so it is safe
// from the worker thread; a hung taskbar just processes it late.
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
        g_pendingWakes++;
        try {
            dispatcher.RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                []() {
                    // The decrement must run even if the apply throws, or
                    // unload's drain wait stalls on the count.
                    if (!g_unloading) {
                        try {
                            ApplyOnThisThreadNow();
                        } catch (...) {
                        }
                    }
                    g_pendingWakes--;
                });
        } catch (winrt::hresult_error const&) {
            g_pendingWakes--;
        }
    }
}

// --------------------------------------------------------------------- hover

// True when nothing button-like sits between the hit-tested element and the frame.
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

// True when the point clears every visible button by padding, in frame coordinates.
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

// Hit-tests the point and its padded neighbours from the bar's root, so Start,
// widgets, and the tray, a sibling of the task-area frame, get the same
// clearance as task icons without knowing their shapes.
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

// True when this pointer position is somewhere a reveal may start from.
bool PointerQualifiesForReveal(void* key,
                               Input::PointerRoutedEventArgs const& args) {
    if (!IsPointerOverEmptySpace(args)) {
        return false;
    }

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

    // Catches bar content living on a separate surface, which no visual-tree
    // probe can reach: a different window within the padding is a seam, not
    // empty space. The padding is in DIPs and these are screen pixels, so it
    // is scaled, or the guard shrinks on a high-DPI display. Probes that land
    // outside the bar's own window are the screen edge or the next monitor,
    // not a seam, or the bar's outer ends could never start a reveal.
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

// The frame's identity across hooks. The hooks see different interface
// pointers for the same TaskbarFrame object, and keying on them raw would
// register the same taskbar twice — two timers and two animation drivers
// fighting over the same buttons. QI to one shared interface canonicalizes.
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

// The hooks below fill raw COM ABI vtable slots, so each body lives in a
// helper wrapped in a catch-all: an exception escaping a slot would unwind
// into XAML and take explorer down.

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

    ULONGLONG pointerNow = GetTickCount64();
    g_lastCursorInsideTick = pointerNow;
    g_lastActivityTick = pointerNow;

    // First touch of a sleeping taskbar restores the fast tick immediately,
    // before any reveal logic can depend on its cadence.
    if (g_sleeping.exchange(false)) {
        ApplyOnThisThreadNow();
    }

    RevealTrigger trigger = g_settings.revealTrigger;
    if ((trigger != RevealTrigger::Hover && trigger != RevealTrigger::Rest) ||
        !g_collapseEnabled || g_revealed) {
        return;
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

    g_revealed = true;
    g_emptyHoverSinceTick = 0;
    ApplyOnThisThreadNow();
}

int __cdecl TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    int ret = TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    try {
        HandlePointerMoved(pThis, pArgs);
    } catch (...) {
    }
    return ret;
}

// Leaving the frame ends any pending qualification, so resting on the tray or
// beyond it cannot inherit one from the last touch of bare taskbar. Exits
// also bubble up from child elements and other pointers while the cursor is
// still inside, so a position still within the frame keeps the state.
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

// Toggles the reveal on a click landing on bare taskbar. Released rather than
// pressed, so dragging off the taskbar and letting go does not count.
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

    if (!PointerQualifiesForReveal(key, args)) {
        return;
    }

    // Atomic flip: two taskbars' release hooks can race on this.
    bool revealed = g_revealed.load();
    while (!g_revealed.compare_exchange_weak(revealed, !revealed)) {
    }
    g_emptyHoverSinceTick = 0;
    ApplyOnThisThreadNow();
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
    // This is the layout hot path, so a frame already adopted is dismissed on
    // raw pointer compares, before any QueryInterface or lock. The pointer is
    // stable per frame for this interface slot. Primary and secondary
    // taskbars share one UI thread, so a few per-thread slots keep the fast
    // path hot with multiple monitors.
    thread_local void* knownFrames[4] = {};
    thread_local unsigned knownNext = 0;
    if (g_unloading) {
        return;
    }
    for (void* known : knownFrames) {
        if (known == pThis) {
            return;
        }
    }

    // Deliberately does not count as activity: the taskbar remeasures for
    // clocks, badges and tray churn, which would keep the mod awake forever.
    // A sleeping tick still notices icon changes within one slow interval.
    FrameworkElement element = nullptr;
    void* key = FrameKeyFromThis(pThis, &element);
    if (!key) {
        return;
    }
    if (IsFrameRegistered(key)) {
        knownFrames[knownNext++ & 3] = pThis;
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

// -------------------------------------------------------------------- hotkey

// Parses "Ctrl+Alt+T" into RegisterHotKey arguments; false if there is no key.
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

// Flips state and queues non-blocking wakes; never waits on a taskbar thread,
// or unloading the mod could strand the worker against a hung message queue.
void ToggleCollapse() {
    // Atomic flip: LoadSettings writes this concurrently on settings saves.
    bool prev = g_collapseEnabled.load();
    while (!g_collapseEnabled.compare_exchange_weak(prev, !prev)) {
    }
    bool enabled = !prev;
    g_revealed = false;
    g_revealedByStart = false;
    g_emptyHoverSinceTick = 0;
    g_lastActivityTick = GetTickCount64();
    g_instantApplyGen++;
    Wh_Log(L"Hotkey toggled collapse to %d", (int)enabled);
    WakeAllFramesAsync();
}

void OnStartMenuVisibility(bool visible) {
    if (g_unloading || !g_settings.revealOnStart || !g_collapseEnabled) {
        return;
    }

    if (visible) {
        if (!g_revealed.exchange(true)) {
            g_revealedByStart = true;
            g_lastActivityTick = GetTickCount64();
            Wh_Log(L"Start menu opened, revealing");
            WakeAllFramesAsync();
        }
        return;
    }

    if (g_revealedByStart.exchange(false)) {
        // With the cursor on the taskbar, hover-like triggers hand the reveal
        // to the normal grace rules instead of collapsing under a click in
        // progress. Click and hotkey-only have no aging rules that could ever
        // end it, so for them the reveal ends with Start.
        RevealTrigger trigger = g_settings.revealTrigger;
        bool hoverLike = trigger == RevealTrigger::Hover ||
                         trigger == RevealTrigger::Rest;
        if (hoverLike && CursorOverAnyTaskbar()) {
            g_lastCursorInsideTick = GetTickCount64();
        } else {
            g_revealed = false;
        }
        g_lastActivityTick = GetTickCount64();
        Wh_Log(L"Start menu closed");
        WakeAllFramesAsync();
    }
}

// Shell-provided notification for the Start menu opening and closing; the
// callbacks arrive on COM worker threads and touch only thread-safe state.
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
        // Counted so teardown can wait out a call in flight; Unadvise alone
        // does not fence one that already started. The catch keeps both the
        // COM ABI and the decrement safe.
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

// Owns the hotkey registration and the Start-menu watcher; WM_QUIT ends it.
DWORD WINAPI HotkeyThreadProc(LPVOID param) {
    // Force the message queue into existence and announce it before anything
    // slow, so teardown's WM_QUIT always lands.
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
    // A callback that slipped past Unadvise may still be running; wait it out
    // before the sink can be freed or the mod unloaded.
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

// Waits without a timeout on purpose: the mod is unloaded the moment teardown
// returns, so a thread still running here would resume inside a freed image.
void StopHotkeyThread() {
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

// Runs proc on the thread owning hWnd and waits for it to finish.
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

// Runs proc once per taskbar UI thread. Do not call while holding
// g_framesMutex: it blocks until each target thread has run proc.
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

    // Iterated rather than a single FindWindow: the first Shell_TrayWnd on
    // the desktop can belong to another process.
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

// Stops the timers and restores the taskbars owned by the calling thread.
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

        if (it->second.timer) {
            it->second.timer.Stop();
            it->second.timer.Tick(it->second.tickToken);
        }
        RestoreFrame(it->second);
        g_frames->erase(it);
    }
}

// ------------------------------------------------------------------ lifetime

void LoadSettings() {
    g_settings.collapsed = Wh_GetIntSetting(L"Collapsed") != 0;

    g_settings.revealOnStart = Wh_GetIntSetting(L"RevealOnStart") != 0;

    auto trigger = WindhawkUtils::StringSetting::make(L"RevealTrigger");
    g_settings.revealTrigger = RevealTrigger::Hover;
    if (wcscmp(trigger.get(), L"rest") == 0) {
        g_settings.revealTrigger = RevealTrigger::Rest;
    } else if (wcscmp(trigger.get(), L"click") == 0) {
        g_settings.revealTrigger = RevealTrigger::Click;
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
    g_settings.refreshIntervalMs =
        std::clamp(Wh_GetIntSetting(L"RefreshIntervalMs"), 10, 2000);

    g_settings.sleepEnabled = Wh_GetIntSetting(L"SleepEnabled") != 0;
    g_settings.sleepIntervalMs =
        std::clamp(Wh_GetIntSetting(L"SleepIntervalMs"), 500, 3000);

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

    auto marker = WindhawkUtils::StringSetting::make(L"RunningNameMarker");
    {
        std::lock_guard<std::mutex> guard(g_settingsMutex);
        g_settings.runningNameMarker = marker.get();
    }

    g_collapseEnabled = g_settings.collapsed.load();
    g_revealed = false;
    g_revealedByStart = false;
    g_emptyHoverSinceTick = 0;
    g_lastPointerQualified = false;
    g_lastActivityTick = GetTickCount64();
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
        if (HookTaskbarViewDllSymbols(module)) {
            if (!g_hooksApplied.exchange(true)) {
                Wh_ApplyHookOperations();
            }
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
        // Only the explorer.exe that owns the taskbar loads Taskbar.View.dll;
        // other instances must not grab the hotkey or the Start watcher.
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
        g_hooksApplied = true;
        return;
    }

    // Try again in case Taskbar.View.dll loaded between Wh_ModInit's check
    // and the LoadLibraryExW hook going live.
    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (taskbarViewModule && !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
            if (!g_hooksApplied.exchange(true)) {
                Wh_ApplyHookOperations();
            }
            StartHotkeyThread();
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    StopHotkeyThread();
    LoadSettings();
    // Never started in explorer.exe instances that do not own the taskbar.
    if (g_taskbarViewDllLoaded) {
        StartHotkeyThread();
    }

    g_instantApplyGen++;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;
    StopHotkeyThread();

    RunOnAllTaskbarThreads([](PVOID) { CleanupOnThisThread(); }, nullptr);

    // Second sweep for contexts whose taskbar window the class-name search
    // missed (mid-recreate): every context's timer must be stopped on its own
    // thread, or it fires after the DLL is gone.
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

    // Last resort for a context with no reachable window: ask its dispatcher
    // to clean up and wait for it. Timers and rendering hooks must come off on
    // their own thread, and dropping the map here would neither stop them nor
    // release their XAML objects safely.
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> stragglers;
    {
        std::lock_guard<std::mutex> guard(g_framesMutex);
        for (auto& [key, ctx] : *g_frames) {
            if (ctx.dispatcher) {
                stragglers.push_back(ctx.dispatcher);
            }
        }
    }

    for (auto& dispatcher : stragglers) {
        HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (!done) {
            continue;
        }
        bool queued = false;
        try {
            dispatcher.RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                [done]() {
                    CleanupOnThisThread();
                    SetEvent(done);
                });
            queued = true;
        } catch (winrt::hresult_error const&) {
        }
        // Unbounded on purpose: giving up would let the queued lambda run
        // inside a freed image. A wedged taskbar hangs the unload instead,
        // which is recoverable.
        if (!queued || WaitForSingleObject(done, INFINITE) == WAIT_OBJECT_0) {
            CloseHandle(done);
        }
    }

    // Wakes still queued to taskbar dispatchers hold code from this DLL; the
    // threads are pumping (they just serviced the cleanup) and no new wakes
    // can be queued. Unbounded on purpose: proceeding would let a queued
    // lambda run inside a freed image.
    while (g_pendingWakes.load() > 0) {
        Sleep(10);
    }

    // Anything still present could not be reached on its own thread; leaving
    // its references alive is safer than releasing them from here. An empty
    // map can go completely, buckets and all.
    std::lock_guard<std::mutex> guard(g_framesMutex);
    if (g_frames->empty()) {
        g_frames.reset();
    } else {
        Wh_Log(L"%u taskbar contexts could not be cleaned up",
               (unsigned)g_frames->size());
    }
}

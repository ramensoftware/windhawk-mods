// ==WindhawkMod==
// @id              taskbar-centered-start-split-icons
// @name            Taskbar Start Button Centered Origin
// @description     Pins the Start button to the true horizontal center of the screen, and splits running-app taskbar buttons into two groups flanking it based on which side of the screen each window is currently on (Windows 11 only)
// @version         0.1.0
// @author          rick
// @github          https://github.com/rycalvo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Centered Start button with position-split taskbar icons

Pins the Windows/Start button to the exact horizontal center of the primary
monitor, regardless of how many taskbar icons are present. Running-app
taskbar buttons are split into two groups that flank the Start button:

- A button goes to the **left** group if its window's current on-screen
  position is left of screen-center.
- A button goes to the **right** group if its window is right of
  screen-center.
- Pinned-but-not-running apps have no window to check the position of, so
  they're classified by the `leftApps`/`rightApps` name lists below, falling
  back to `unresolvedAppsDefaultSide` if unlisted. Within whichever side
  they land on, `pinnedAppsAnchor` picks whether they sit at the far edge
  or right next to Start.

When you drag a window across the center line of the screen, its taskbar
button switches sides to follow it. Side-switching is driven by a global
window-location-change listener and is best-effort: it happens shortly
after a drag/move settles, not on every intermediate pixel of the drag.

Search, Task View and Widgets can either stay at the far left edge, or move
right next to Start on whichever side you prefer.

## Known limitations (please read before reporting issues)

- **Windows 11 only.** Windows 10's taskbar has no XAML layer to hook into.
- **Primary monitor only.** Screen-center math and window-side classification
  use the primary monitor. Taskbars on secondary displays are not specially
  handled by this mod (their icons keep the default layout Windows gives
  them) - the positioning hook explicitly checks which taskbar's XAML tree
  an element belongs to and leaves anything outside the primary's alone.
- **Undocumented internals.** This mod hooks private, unversioned classes
  inside `taskbar.dll` and `Taskbar.View.dll` (via symbols resolved from
  Microsoft's public symbol server at runtime, not hardcoded offsets). A
  Windows update can change these internals and break the mod until it's
  updated. If that happens, disable the mod rather than filing against
  explorer.exe crashing.
- The "resolve which HWND a taskbar button represents" step reuses a
  technique from other taskbar-reordering mods (synchronously reporting a
  sentinel "click" to the taskbar's internal click handler, which is
  intercepted before it does anything, to read back the window handle). It
  runs on a periodic timer rather than inline during layout, and Arrange
  only ever reads whatever the timer has already cached - running it
  synchronously from inside the taskbar's own layout pass was the
  confirmed cause of an explorer.exe crash (specifically when Windows'
  "show taskbar apps on" setting is anything other than "All taskbars",
  since that's when a window moving across monitors structurally adds/
  removes taskbar buttons rather than just repositioning them).

## Changelog

**Initial build (Aug 2026)**
- Start button pinned to the primary monitor's true center, independent of
  the number of taskbar icons.
- Running-app icons split left/right of Start by live window position, with
  drag-follow (a button switches sides shortly after its window crosses the
  center line).
- `taskListOrder`: icons ordered by distance from center, or by native
  taskbar order.
- Minimized windows keep their last known side instead of jumping (a
  minimized window's reported position is off-screen nonsense, so it's
  frozen at wherever it was before minimizing).
- `leftApps`/`rightApps` name-based overrides, plus
  `unresolvedAppsDefaultSide` for pinned-but-not-running apps with no
  window to classify by.
- `systemButtonsPlacement`/`systemButtonsAdjacentSide`: Search, Task View
  and Widgets can sit at the far-left edge or adjacent to Start.

**Feature follow-ups**
- Support for "Combine taskbar buttons: Always" (grouped icons bind to a
  different internal view model than individual windows, so this needed
  its own resolution path).
- Negative caching for HWND resolution, so a pinned-but-not-running app
  (which will never resolve to a window) isn't retried on every single
  layout pass.
- Self-healing taskbar-window lookup, so the mod doesn't stay permanently
  inert if Windhawk happens to inject before the taskbar itself exists yet
  (observed right after a fresh boot).
- `unresolvedAppsDefaultSide` gained a `contralateral-to-system-buttons`
  option.
- `pinnedAppsAnchor` setting: pinned-not-running apps can sit at the outer
  edge of their side, or right next to Start.

**Stability: cross-monitor moves**
Moving a window to a second monitor repeatedly crashed explorer.exe across
several iterations, each surfacing a different unsafe call path within the
same underlying class of bug: a synchronous WinRT/XAML call made while the
taskbar's own internal button list is mid-structural-mutation - reproducible
specifically when Windows' "show taskbar apps on" setting isn't "All
taskbars," since only then does a cross-monitor move add or remove taskbar
buttons rather than just repositioning them. Root-caused via WinDbg
crash-dump analysis after a few rounds of counter-based guessing didn't
fully close it. Fixes along the way:
- Corrected an unsafe cast that could throw while a fresh secondary-monitor
  taskbar tree was still under construction.
- Removed every forced-synchronous layout pass in favor of always deferring
  to the XAML dispatcher's own next tick.
- Scoped all positioning strictly to the primary taskbar's own XAML tree (a
  process-wide hook was also seeing, and mis-positioning, secondary-monitor
  taskbars).
- Moved taskbar-button HWND resolution off the live layout pass entirely and
  onto a periodic timer.
- Added a brief "settling window" after any detected taskbar button-count
  change, during which the mod holds off on the specific operations that
  were still unsafe mid-mutation.

**Settling-window polish**
Once crash-free, the settling window itself produced two follow-on cosmetic
issues, both fixed the same day:
- The Start button no longer keeps re-centering while its neighbors are held
  still during settling (previously caused a brief icons-over-Start overlap
  on every cross-monitor move).
- Fixed a bookkeeping bug that could make the settling window continuously
  re-arm itself, making the taskbar look permanently reverted until some
  unrelated event happened to nudge it back into place - now guaranteed to
  resolve shortly after the triggering change instead.
- Instead of snapping to Windows' native layout for the duration of the
  settling window, the mod now holds every icon at its last known position
  and only lets a genuinely new button show up unpositioned - much less
  visually distracting on ordinary events like opening a new app.

## Disclosures

I am not a software developer. The present mod was developed using the
Claude Code extension in VS Code. I cannot verify the external integrity of
this mod on other systems and do not take responsibility for issues that
may arise for its use. This mod was created for my own interests and
shared for targeted development by members of the Windhawk community.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gapPx: 12
  $name: Gap size (px)
  $description: >-
    Horizontal gap, in pixels, between the Start button and each flanking
    group of app icons (and between Start and Search/Task View/Widgets, if
    those are set to sit adjacent to Start below).
- systemButtonsPlacement: far-left
  $name: Search / Task View / Widgets placement
  $options:
    - far-left: Far left edge of the taskbar
    - adjacent-start: Right next to the Start button
  $description: >-
    Where the Search, Task View and Widgets buttons go. "Far left edge" keeps
    them out of the way of the centered layout entirely. "Right next to
    Start" tucks them against one side of the Start button instead (pick
    which side below).
- systemButtonsAdjacentSide: left
  $name: Side to place them on
  $options:
    - left: Left of Start
    - right: Right of Start
  $description: >-
    Only used when the placement above is "Right next to Start".
- leftApps: ""
  $name: Force these apps to the left
  $description: >-
    Comma-separated list of app name fragments (matched case-insensitively
    against each taskbar button's accessible name, e.g. "chrome, notepad")
    that should always be placed left of Start, regardless of where their
    window is. Mainly useful for pinned apps that aren't running, since
    those have no window position to classify by.
- rightApps: ""
  $name: Force these apps to the right
  $description: Same idea as leftApps, but for the right side.
- unresolvedAppsDefaultSide: left
  $name: Default side for unclassified apps
  $options:
    - left: Left of Start
    - right: Right of Start
    - contralateral-to-system-buttons: Opposite side from Search/Task View/Widgets
  $description: >-
    Fallback for any taskbar button that isn't matched by leftApps/rightApps
    and whose window position can't be determined (mainly pinned-but-not-
    running apps you haven't listed above). "Opposite side from Search/Task
    View/Widgets" tracks the systemButtonsAdjacentSide setting below when
    those are placed next to Start; if they're instead left at the
    taskbar's far-left edge, that still counts as their "side" for this
    purpose, so it resolves to the right.
- taskListOrder: distance-from-center
  $name: App icon ordering within each side
  $options:
    - distance-from-center: Closer-to-center windows sit closer to Start
    - taskbar-order: Preserve the existing taskbar order
  $description: >-
    How same-side app icons are ordered relative to each other. "Closer to
    center" ranks each window by how close its own center is to the
    screen's center line, so the nearest one ends up right next to Start.
    Pinned/overridden apps with no live window position sort to whichever
    end pinnedAppsAnchor below picks. Minimized windows keep the position
    they last had before minimizing, rather than jumping around.
- pinnedAppsAnchor: outer-edge
  $name: Pinned (not running) app position within their side
  $options:
    - outer-edge: Far end, away from Start
    - adjacent-to-start: Right next to Start
  $description: >-
    Where a pinned-but-not-running app's icon sits relative to the running
    apps on the same side (leftApps/rightApps-forced apps included, since
    those also have no window to rank by distance). Only has an effect
    when "App icon ordering" above is "Closer to center" - with "Preserve
    existing taskbar order" there's no outer-edge/adjacent-to-Start
    distinction to begin with.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include <commctrl.h>
#include <dwmapi.h>
#include <roapi.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

// ============================================================================
// Settings
// ============================================================================

enum class Side { Left, Right };

enum class SystemButtonsPlacement { FarLeft, AdjacentStart };

enum class TaskListOrder { DistanceFromCenter, TaskbarOrder };

enum class UnresolvedAppsDefaultSide {
    Left,
    Right,
    ContralateralToSystemButtons,
};

// Where a pinned-but-not-running app's icon sits within its side's icon
// group, relative to the other (running, position-classified) icons on the
// same side. Only meaningful when taskListOrder is DistanceFromCenter -
// TaskbarOrder has no notion of "outer edge" vs "adjacent to Start" to
// begin with, since it doesn't rank icons by distance at all.
enum class PinnedAppsAnchor { OuterEdge, AdjacentToStart };

struct {
    double gapPx;
    SystemButtonsPlacement systemButtonsPlacement;
    Side systemButtonsAdjacentSide;
    std::vector<std::wstring> leftApps;
    std::vector<std::wstring> rightApps;
    UnresolvedAppsDefaultSide unresolvedAppsDefaultSide;
    TaskListOrder taskListOrder;
    PinnedAppsAnchor pinnedAppsAnchor;
} g_settings;

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = towlower(c);
    }
    return s;
}

// Splits a comma-separated settings string into trimmed, lowercased,
// non-empty fragments for case-insensitive substring matching later.
std::vector<std::wstring> ParseAppList(PCWSTR raw) {
    std::vector<std::wstring> result;
    if (!raw) {
        return result;
    }

    std::wstring s(raw);
    size_t start = 0;
    while (start <= s.size()) {
        size_t comma = s.find(L',', start);
        size_t end = (comma == std::wstring::npos) ? s.size() : comma;

        size_t first = s.find_first_not_of(L" \t", start);
        size_t last = s.find_last_not_of(L" \t", end == start ? end : end - 1);
        if (first != std::wstring::npos && first < end && last != std::wstring::npos &&
            last >= first) {
            result.push_back(ToLower(s.substr(first, last - first + 1)));
        }

        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }

    return result;
}

Side ParseSide(PCWSTR value, Side fallback) {
    if (value && wcscmp(value, L"right") == 0) {
        return Side::Right;
    }
    if (value && wcscmp(value, L"left") == 0) {
        return Side::Left;
    }
    return fallback;
}

UnresolvedAppsDefaultSide ParseUnresolvedAppsDefaultSide(PCWSTR value) {
    if (value && wcscmp(value, L"right") == 0) {
        return UnresolvedAppsDefaultSide::Right;
    }
    if (value && wcscmp(value, L"contralateral-to-system-buttons") == 0) {
        return UnresolvedAppsDefaultSide::ContralateralToSystemButtons;
    }
    return UnresolvedAppsDefaultSide::Left;
}

// Resolves the effective side for pinned-but-not-running apps that aren't
// matched by leftApps/rightApps, per the unresolvedAppsDefaultSide setting.
// The "contralateral" mode mirrors Search/Task View/Widgets' placement:
// opposite of systemButtonsAdjacentSide when they sit next to Start, or the
// right if they're left at the taskbar's far-left edge instead (still
// their "side" for this purpose, since that edge is unambiguously left of
// everything else).
Side ResolveUnresolvedAppsDefaultSide() {
    switch (g_settings.unresolvedAppsDefaultSide) {
        case UnresolvedAppsDefaultSide::Right:
            return Side::Right;
        case UnresolvedAppsDefaultSide::ContralateralToSystemButtons:
            if (g_settings.systemButtonsPlacement ==
                    SystemButtonsPlacement::AdjacentStart &&
                g_settings.systemButtonsAdjacentSide == Side::Right) {
                return Side::Left;
            }
            return Side::Right;
        case UnresolvedAppsDefaultSide::Left:
        default:
            return Side::Left;
    }
}

PinnedAppsAnchor ParsePinnedAppsAnchor(PCWSTR value) {
    if (value && wcscmp(value, L"adjacent-to-start") == 0) {
        return PinnedAppsAnchor::AdjacentToStart;
    }
    return PinnedAppsAnchor::OuterEdge;
}

// The distance-from-center order key for a pinned-but-not-running app (one
// with no window to measure a real distance from). Made -infinity instead
// of the default +infinity when pinnedAppsAnchor is AdjacentToStart, since
// ComputeTaskListButtonX's byDistance sort treats a smaller orderKey as
// closer to Start - nothing can beat -infinity, so these always end up
// innermost rather than outermost.
double PinnedAppOrderKey() {
    return g_settings.pinnedAppsAnchor == PinnedAppsAnchor::AdjacentToStart
               ? -std::numeric_limits<double>::infinity()
               : std::numeric_limits<double>::infinity();
}

void LoadSettings() {
    g_settings.gapPx = Wh_GetIntSetting(L"gapPx");

    PCWSTR placement = Wh_GetStringSetting(L"systemButtonsPlacement");
    g_settings.systemButtonsPlacement =
        (placement && wcscmp(placement, L"adjacent-start") == 0)
            ? SystemButtonsPlacement::AdjacentStart
            : SystemButtonsPlacement::FarLeft;
    Wh_FreeStringSetting(placement);

    PCWSTR adjacentSide = Wh_GetStringSetting(L"systemButtonsAdjacentSide");
    g_settings.systemButtonsAdjacentSide = ParseSide(adjacentSide, Side::Left);
    Wh_FreeStringSetting(adjacentSide);

    PCWSTR leftApps = Wh_GetStringSetting(L"leftApps");
    g_settings.leftApps = ParseAppList(leftApps);
    Wh_FreeStringSetting(leftApps);

    PCWSTR rightApps = Wh_GetStringSetting(L"rightApps");
    g_settings.rightApps = ParseAppList(rightApps);
    Wh_FreeStringSetting(rightApps);

    PCWSTR defaultSide = Wh_GetStringSetting(L"unresolvedAppsDefaultSide");
    g_settings.unresolvedAppsDefaultSide =
        ParseUnresolvedAppsDefaultSide(defaultSide);
    Wh_FreeStringSetting(defaultSide);

    PCWSTR taskListOrder = Wh_GetStringSetting(L"taskListOrder");
    g_settings.taskListOrder =
        (taskListOrder && wcscmp(taskListOrder, L"taskbar-order") == 0)
            ? TaskListOrder::TaskbarOrder
            : TaskListOrder::DistanceFromCenter;
    Wh_FreeStringSetting(taskListOrder);

    PCWSTR pinnedAppsAnchor = Wh_GetStringSetting(L"pinnedAppsAnchor");
    g_settings.pinnedAppsAnchor = ParsePinnedAppsAnchor(pinnedAppsAnchor);
    Wh_FreeStringSetting(pinnedAppsAnchor);
}

// ============================================================================
// Globals
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_inTaskbarArrangeOverride;

// GetTickCount64() deadline until which IUIElement_Arrange_Hook skips its
// own repeater-traversal-based computation entirely (Start, task list
// buttons, and the Search/TaskView/Widgets cluster alike) and instead
// holds each element at its last legitimately-computed X via
// ArrangeSettled/g_lastArrangedX (see that comment for why holding
// position, not falling through to Windows' native layout, is both safe
// and far less visually jarring). Set whenever the ArrangeOverride hook
// detects the task list button count changed since the last pass (see its
// call site) - i.e. right when a button is being structurally inserted
// into or removed from an ItemsRepeater's data source, since that's the
// confirmed trigger condition for a recurring explorer.exe crash (only
// reproduces when Windows' "show taskbar apps on" setting causes a window
// moving across monitors to change a taskbar's button *set*, not just
// coordinates - see ResolveAndCacheButtonHwnd's comment for the fuller
// history).
//
// Moving HWND resolution off the Arrange hook onto a timer measurably
// helped (a confirmed crash-free stretch of real use, not just luck) but
// didn't fully close the hole - the crash still recurs on cross-monitor
// moves. Every synchronous WinRT call this file makes from inside Arrange
// has, so far, turned out to belong to the same vulnerable class once
// something structurally mutates the repeater mid-traversal, and none of
// them ever show up as a catchable exception (see g_primaryXamlRootIdentity
// and ResolveAndCacheButtonHwnd's comments for why - WinUI stows and
// defers the report instead of throwing where the C++ call actually is).
// Rather than hunt down and individually defer every remaining traversal
// one crash at a time, this suppresses ALL of them for a settling window
// after any detected structural change.
//
// Start's own positioning (ComputeStartButtonX) never touches a repeater,
// so it isn't part of the crash-prone class and doesn't strictly need to be
// gated - but it used to be left ungated anyway, and that was itself a bug:
// with only its neighbors frozen, Start kept snapping to the forced screen
// center every pass while nothing reserved that space around it, producing
// a visible icons-overlaying-Start glitch on every structural change (e.g.
// a window moving to another monitor). Gating Start too makes the whole
// taskbar hold one consistent, already-correct-looking state through the
// settling window instead of that mismatched hybrid.
//
// A pass genuinely needs to run once this deadline elapses, to give
// everything a fresh Arrange reflecting whatever actually changed (new
// button included) - see g_taskListSettlingRecoveryPending for how that's
// actually guaranteed (the InvalidateTaskbarLayout() call made at the same
// call site this is armed does NOT do it: that call lands, and gets
// immediately re-skipped, *inside* the window it just opened, not after
// it).
thread_local ULONGLONG g_taskListSettlingUntil;

// Set alongside g_taskListSettlingUntil whenever it's (re)armed; cleared
// once a guaranteed post-settling InvalidateTaskbarLayout() has actually
// fired (see ButtonHwndResolveTimerProc). Without this, recovery depended
// entirely on some unrelated window happening to generate a location-change
// event (or a taskbar button hwnd happening to need re-resolution) after
// the settling window elapsed - both opportunistic, neither guaranteed -
// which is what let the taskbar visibly sit un-refreshed (existing buttons
// held at their last position, anything newly inserted mid-settling still
// at its native one - see ArrangeSettled) for well past 1 second after a
// cross-monitor window move, until something incidental finally kicked it
// back into place.
thread_local bool g_taskListSettlingRecoveryPending;

HWND g_hTaskbarWnd;
HWINEVENTHOOK g_locationChangeHook;
std::atomic<int> g_winEventRawCount;
std::atomic<int> g_winEventInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

// ============================================================================
// Generic taskbar/XAML helpers
// (traversal helpers adapted from the "Start button always on the left"
// Windhawk mod, which uses the same VisualTreeHelper/ItemsRepeater walking
// pattern to reach taskbar elements)
// ============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& enumCallback) {
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

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                       PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Returns the ItemsRepeater's realized (non-virtualized) children in order.
std::vector<FrameworkElement> GetRepeaterChildElements(
    FrameworkElement repeaterElement) {
    std::vector<FrameworkElement> result;

    auto repeater =
        repeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return result;
    }

    auto itemsSourceView = repeater.ItemsSourceView();
    int count = itemsSourceView ? itemsSourceView.Count() : 0;

    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index);
        if (!element) {
            continue;  // Virtualized away, not currently realized.
        }

        auto child = element.try_as<FrameworkElement>();
        if (child) {
            result.push_back(child);
        }
    }

    return result;
}

// Returns `element`'s sibling FrameworkElements (element excluded), in
// visual-tree order. Works whether the parent is an ItemsRepeater (uses the
// data-bound realized items, same as GetRepeaterChildElements) or a plain
// panel (falls back to raw VisualTreeHelper enumeration). Task list buttons'
// exact parent container isn't confirmed to be the same repeater that hosts
// the Start button, so this is deliberately hierarchy-agnostic rather than
// assuming ItemsRepeater everywhere.
std::vector<FrameworkElement> GetSiblingElements(FrameworkElement element) {
    std::vector<FrameworkElement> result;

    auto parent =
        Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    if (!parent) {
        return result;
    }

    if (parent.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()) {
        return GetRepeaterChildElements(parent);
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childrenCount; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            result.push_back(child);
        }
    }

    return result;
}

using RunFromWindowThreadProc_t = std::function<void()>;

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc) {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc();
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* proc = (RunFromWindowThreadProc_t*)cwp->lParam;
                    (*proc)();
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&proc);

    UnhookWindowsHookEx(hook);

    return true;
}

// ============================================================================
// taskbar.dll: locating the taskbar's XamlRoot
// (verbatim technique from the "Start button always on the left" mod)
// ============================================================================

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

    // The offset of the XAML element pointer inside TaskbarHost isn't
    // exposed by any symbol, so it's read out of the prologue of a
    // neighboring function that's known to access it at a fixed offset.
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight, using default offset");
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                            winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// Raw IUnknown* for `xamlRoot`, valid ONLY as an opaque identity token for
// pointer comparison - never dereferenced as a live object. QueryInterface
// to IUnknown is what gives this COM identity semantics (guaranteed stable
// and unique per underlying object, regardless of which interface either
// side started from), the same guarantee g_buttonHwndCache already relies
// on by keying off winrt::get_abi(element) elsewhere in this file.
void* XamlRootIdentity(XamlRoot xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }
    return winrt::get_abi(xamlRoot.try_as<IUnknown>());
}

// Identity of the PRIMARY taskbar's XamlRoot, resolved lazily and cached
// (GetTaskbarXamlRoot is too expensive - GetProp, GetWindowLongPtr, a
// vftable walk, a cross-DLL call - to redo on every Arrange call). Exists
// so IUIElement_Arrange_Hook can tell whether an element it's about to
// reposition actually belongs to the primary taskbar's own XAML tree.
//
// This hook replaces a single process-wide IUIElement::Arrange vtable
// slot, so it fires for every taskbar instance's tree - including a
// secondary monitor's, once any window (and its taskbar button) lives
// there. Without this check, a secondary-monitor taskbar button was being
// arranged using position math computed relative to the PRIMARY monitor's
// center (every Compute*ButtonX function implicitly assumes "the"
// taskbar is g_hTaskbarWnd) - handing that tree a Rect it has no valid
// context for. That's exactly the kind of internal invariant violation
// WinUI's own ReportUnhandledError/fail-fast machinery exists to catch:
// confirmed via a WinDbg crash dump analysis (STATUS_STOWED_EXCEPTION,
// CXcpDispatcher::Tick -> ReportUnhandledError -> RaiseFailFastException)
// showing the fail-fast originates entirely inside Windows' own XAML
// dispatcher, on a LATER tick than whatever triggered it - which is also
// why no try/catch anywhere in this file ever saw it: WinUI's internal
// ABI-boundary handling stows the error before it ever reaches our frame
// as a catchable C++ exception, regardless of where the catch is placed.
std::atomic<void*> g_primaryXamlRootIdentity;

// ============================================================================
// taskbar.dll: resolving the HWND behind a taskbar button
//
// Individual (ungrouped) button:
//   TaskListButton (XAML) -> TaskListWindowViewModel -> WindowsUdk ITaskItem
//   -> (sentinel "click" through the taskbar's own click handler, which we
//      intercept before it acts on it) -> native ITaskItem -> HWND.
//
// Grouped button (when "Combine taskbar buttons" is Always - each button can
// represent multiple windows of one app, and isn't bound to a
// TaskListWindowViewModel at all, so the above fails at the very first
// step):
//   TaskListButton (XAML) -> TaskListGroupViewModel -> (a second sentinel,
//   piggybacked on TaskListGroupViewModel::IsMultiWindow's internal call to
//   ITaskGroup::IsRunning, which we intercept the same way) -> WindowsUdk
//   ITaskGroup -> (sentinel "click" again, this time on the group) -> native
//   ITaskGroup -> its internal task-items array (located by exploiting
//   CTaskGroup::GetNumItems' known-trivial implementation - see
//   GetTaskItemsArray) -> first item's HWND, used as the group's
//   representative position.
//
// Both of these are the same techniques used by taskbar-reordering and
// per-app-volume-control mods to map a button back to a window/windows;
// here they're reused read-only, purely to find out where a window
// currently is on screen.
// ============================================================================

void* CImmersiveTaskItem_vftable;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                     void* taskGroup,
                                                     void* taskItem,
                                                     void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;

WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskItem;
void* g_clickSentinel_TaskGroup;

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              void** launcherOptions) {
    if (launcherOptions && *launcherOptions == (void*)&g_clickSentinel) {
        g_clickSentinel_TaskItem = taskItem;
        g_clickSentinel_TaskGroup = taskGroup;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
}

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void(WINAPI*)(void* resultPut, void* uiElement);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                             void** result);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void(WINAPI*)(void* resultPut, void* uiElement);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems_Original;

// CTaskGroup::GetNumItems' entire body is just `return
// DPA_GetPtrCount(this->taskItemsArray);`, i.e. reading one int off `this`
// at a fixed-but-undocumented offset. Calling it with a fake "this" that's
// actually an array of pointers-to-sequential-ints turns that read into a
// self-reporting probe: whatever offset it reads becomes the returned
// value, revealing the real offset without needing the struct layout.
// Adapted from the per-app volume control mod, which needs the same task
// group -> item list access for an unrelated reason.
size_t GetTaskItemsArrayOffset() {
    static size_t offset = [] {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }
        return CTaskGroup_GetNumItems_Original
                   ? (size_t)CTaskGroup_GetNumItems_Original(arrayOfIntPtrs)
                   : 0;
    }();
    return offset;
}

HDPA GetTaskItemsArray(void* taskGroup) {
    if (!CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }
    return (HDPA)((void**)taskGroup)[GetTaskItemsArrayOffset()];
}

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// Diagnostics only: which stage of the WinRT -> native resolution chain
// last failed, and how often, so a systemic failure (e.g. a different
// view-model type when taskbar buttons are grouped/combined) is visible
// without guessing.
struct ResolveStats {
    int success = 0;
    int failViewModelNull = 0;
    int failGetTaskItem = 0;
    int failSentinelNoItem = 0;
    int groupSuccess = 0;
    int groupFailViewModelNull = 0;
    int groupFailSentinelNoGroup = 0;
    int groupFailNoItems = 0;
};
ResolveStats g_resolveStats;

HWND GetWindowFromNativeTaskItem(void* nativeTaskItem) {
    if (!nativeTaskItem) {
        return nullptr;
    }

    if (CImmersiveTaskItem_vftable &&
        *(void**)nativeTaskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow_Original
                   ? CImmersiveTaskItem_GetWindow_Original(nativeTaskItem)
                   : nullptr;
    }

    return CWindowTaskItem_GetWindow_Original
               ? CWindowTaskItem_GetWindow_Original(nativeTaskItem)
               : nullptr;
}

HWND ResolveHwndFromIndividualTaskItem(FrameworkElement element) {
    if (!TryGetItemFromContainer_TaskListWindowViewModel_Original ||
        !TaskListWindowViewModel_get_TaskItem_Original ||
        !TaskItem_ReportClicked_Original) {
        return nullptr;
    }

    IUnknown* elementAbi = (IUnknown*)winrt::get_abi(element);

    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &elementAbi);
    if (!windowViewModel) {
        g_resolveStats.failViewModelNull++;
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    if (FAILED(TaskListWindowViewModel_get_TaskItem_Original(
            windowViewModel.get(), windowsUdkTaskItem.put_void())) ||
        !windowsUdkTaskItem) {
        g_resolveStats.failGetTaskItem++;
        return nullptr;
    }

    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                     &g_clickSentinel);

    void* nativeTaskItem = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = nullptr;
    if (!nativeTaskItem) {
        g_resolveStats.failSentinelNoItem++;
        return nullptr;
    }

    g_resolveStats.success++;
    return GetWindowFromNativeTaskItem(nativeTaskItem);
}

// Grouped button (all windows of one app collapsed under a single icon,
// e.g. "Combine taskbar buttons" set to Always) - see the resolution
// overview comment above this section for the full chain.
HWND ResolveHwndFromTaskGroup(FrameworkElement element) {
    if (!TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original ||
        !TaskGroup_ReportClicked_Original || !CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }

    IUnknown* elementAbi = (IUnknown*)winrt::get_abi(element);

    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &elementAbi);
    if (!groupViewModel) {
        g_resolveStats.groupFailViewModelNull++;
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    // IsMultiWindow's implementation happens to call ITaskGroup::IsRunning
    // internally, which is hooked above to capture its `this` (the native
    // WindowsUdk task group) instead of really answering the question. The
    // -1 adjusts from the interface pointer QueryInterface handed back to
    // the adjacent vtable IsMultiWindow actually needs - a fixed ABI detail
    // of this object, not a magic number specific to this mod.
    TaskListGroupViewModel_IsMultiWindow_Original(
        (void**)groupViewModel.get() - 1);
    g_captureTaskGroup = false;

    void* windowsUdkTaskGroup = g_capturedTaskGroup;
    g_capturedTaskGroup = nullptr;
    if (!windowsUdkTaskGroup) {
        g_resolveStats.groupFailSentinelNoGroup++;
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    void* nativeTaskGroup = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = nullptr;
    if (!nativeTaskGroup) {
        g_resolveStats.groupFailSentinelNoGroup++;
        return nullptr;
    }

    HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
    if (!taskItemsArray || DPA_GetPtrCount(taskItemsArray) <= 0) {
        g_resolveStats.groupFailNoItems++;
        return nullptr;
    }

    // The group's first window stands in for the whole group's position -
    // Windows itself doesn't expose a more meaningful "primary" window.
    void* taskItem = DPA_GetPtr(taskItemsArray, 0);
    HWND hwnd = GetWindowFromNativeTaskItem(taskItem);
    if (hwnd) {
        g_resolveStats.groupSuccess++;
    }
    return hwnd;
}

HWND ResolveHwndFromTaskListButton(FrameworkElement element) {
    HWND hwnd = ResolveHwndFromIndividualTaskItem(element);
    if (hwnd) {
        return hwnd;
    }

    return ResolveHwndFromTaskGroup(element);
}

// Per-button HWND cache, keyed by the XAML element's ABI pointer. Avoids
// running the (relatively expensive) resolution chain above on every single
// layout pass for every button - only when we don't have a cached handle, or
// the cached one has stopped being a valid window.
//
// Also negatively caches failures with a short TTL: a pinned-but-not-running
// app's task group legitimately has zero windows, so its resolution will
// fail forever until it's actually launched - without this, that button's
// full multi-sentinel resolution chain gets retried on every single arrange
// pass (confirmed via g_resolveStats climbing into the hundreds within a
// couple of seconds for just one such button).
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
    ULONGLONG lastAttempt = 0;
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

// Actually runs the resolution chain and updates the cache. Returns
// whether the cached HWND changed (added, removed, or replaced) - used by
// the timer that calls this (see ResolvePendingButtonHwnds) to decide
// whether a relayout is worth triggering.
//
// Deliberately ONLY ever called from that timer, never from inside
// GetButtonHwnd/an active Arrange pass - see the mod's own readme "Known
// limitations" section, which flagged this exact risk before it was ever
// hit in practice: the click-sentinel technique this chain uses
// interacts with the taskbar's own internal click-handling machinery, and
// running it synchronously while XAML is mid-layout - specifically while
// a button is being structurally inserted into or removed from an
// ItemsRepeater's data source, which happens whenever Windows' "show
// taskbar apps on" setting is anything other than "All taskbars" and a
// window moves across monitors - was confirmed (via WinDbg crash dump
// analysis across several explorer.exe crash-loop sessions) to be
// reachable in a way that fails fast with STATUS_STOWED_EXCEPTION. Only
// "All taskbars" mode was stable, because there a window moving between
// monitors never changes any taskbar's button set - no new/removed button
// ever needs fresh resolution inline during layout. Moving resolution to
// a timer restores the "normal" context this technique already assumed
// (a standalone event, not nested inside a live layout pass), matching
// where the pointer-release-based technique it's adapted from actually
// runs in the mods it's borrowed from.
bool ResolveAndCacheButtonHwnd(FrameworkElement element) {
    void* key = winrt::get_abi(element);
    HWND previous = nullptr;
    auto it = g_buttonHwndCache.find(key);
    if (it != g_buttonHwndCache.end()) {
        previous = it->second.hwnd;
    }

    HWND hwnd = ResolveHwndFromTaskListButton(element);
    g_buttonHwndCache[key] = {hwnd, GetTickCount64()};
    return hwnd != previous;
}

// Read-only: never triggers resolution itself, only ever reads whatever
// ResolveAndCacheButtonHwnd (via the timer) has already cached. A button
// with no cache entry yet, or one whose cached window has since closed,
// reads as unresolved here and falls back to the default-side
// classification until the next timer tick catches up - see
// ResolveAndCacheButtonHwnd's comment for why this can never call the
// resolution chain directly.
HWND GetButtonHwnd(FrameworkElement element) {
    void* key = winrt::get_abi(element);

    auto it = g_buttonHwndCache.find(key);
    if (it == g_buttonHwndCache.end()) {
        return nullptr;
    }
    if (it->second.hwnd && !IsWindow(it->second.hwnd)) {
        return nullptr;
    }
    return it->second.hwnd;
}

// ResolvePendingButtonHwnds (which actually walks TaskListButtons and
// calls ResolveAndCacheButtonHwnd above) is defined later, right after
// FindTaskbarFrameRepeater and IsTaskListButton - it depends on both.
// Forward-declared here so StartButtonHwndResolveTimer (Mod lifecycle
// section) can reference it before that point.
void ResolvePendingButtonHwnds();

// ============================================================================
// Screen-position math
// ============================================================================

struct WindowClassification {
    Side side;
    // Distance in screen px between the window's center and the screen's
    // center line. Only meaningful for ordering same-side icons relative
    // to each other; unresolved/overridden buttons get +infinity so they
    // sort to the outer edge.
    double distanceFromCenter = std::numeric_limits<double>::infinity();
};

// A minimized window's GetWindowRect returns a nonsense off-screen position
// (classically around (-32000,-32000)), which would otherwise always
// classify it as "left". Instead, freeze at the last known classification
// from before it was minimized. Not pruned on window close - see note at
// GetButtonHwnd's cache for why that's an acceptable tradeoff here too.
std::unordered_map<HWND, WindowClassification> g_lastKnownWindowClassification;

WindowClassification ClassifyByWindowPositionCached(HWND hwnd) {
    if (IsIconic(hwnd)) {
        auto it = g_lastKnownWindowClassification.find(hwnd);
        if (it != g_lastKnownWindowClassification.end()) {
            return it->second;
        }
        return {ResolveUnresolvedAppsDefaultSide()};
    }

    RECT wr;
    HMONITOR mon = MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!hwnd || !GetWindowRect(hwnd, &wr) || !GetMonitorInfo(mon, &mi)) {
        return {ResolveUnresolvedAppsDefaultSide()};
    }

    double windowCenterX = (wr.left + wr.right) / 2.0;
    double screenCenterX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2.0;

    WindowClassification result;
    result.side = windowCenterX < screenCenterX ? Side::Left : Side::Right;
    result.distanceFromCenter = std::abs(windowCenterX - screenCenterX);

    g_lastKnownWindowClassification[hwnd] = result;
    return result;
}

// Taskbar buttons expose their app's display name as an accessibility
// property (used by screen readers); reused here as a stable identifier
// that works even for pinned-but-not-running apps, which have no HWND.
std::wstring GetButtonAccessibleName(FrameworkElement element) {
    auto name = Automation::AutomationProperties::GetName(element);
    if (!name.empty()) {
        return ToLower(std::wstring(name.c_str()));
    }

    auto elementName = element.Name();
    if (!elementName.empty()) {
        return ToLower(std::wstring(elementName.c_str()));
    }

    return L"";
}

bool ContainsAnyFragment(const std::wstring& haystack,
                         const std::vector<std::wstring>& needles) {
    if (haystack.empty()) {
        return false;
    }

    for (auto& needle : needles) {
        if (!needle.empty() && haystack.find(needle) != std::wstring::npos) {
            return true;
        }
    }

    return false;
}

struct ButtonClassification {
    Side side;
    double orderKey = std::numeric_limits<double>::infinity();
    bool hwndResolved = false;
};

// Classification priority: explicit user override by app name, then live
// window position, then the configured default (mainly hit by pinned apps
// that aren't running and weren't listed in leftApps/rightApps).
ButtonClassification ClassifyTaskListButton(FrameworkElement element) {
    std::wstring name = GetButtonAccessibleName(element);

    if (ContainsAnyFragment(name, g_settings.leftApps)) {
        return {Side::Left, PinnedAppOrderKey()};
    }
    if (ContainsAnyFragment(name, g_settings.rightApps)) {
        return {Side::Right, PinnedAppOrderKey()};
    }

    HWND hwnd = GetButtonHwnd(element);
    if (hwnd) {
        WindowClassification wc = ClassifyByWindowPositionCached(hwnd);
        return {wc.side, wc.distanceFromCenter, true};
    }

    return {ResolveUnresolvedAppsDefaultSide(), PinnedAppOrderKey()};
}

// X coordinate (in the taskbar repeater's local DIPs) of the primary
// monitor's horizontal center.
double GetMonitorCenterXLocal() {
    HMONITOR mon = MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi)) {
        return 0;
    }

    RECT taskbarRect;
    if (!GetWindowRect(g_hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(g_hTaskbarWnd);
    double scale = dpi ? (96.0 / dpi) : 1.0;

    double centerScreenPx = (mi.rcMonitor.left + mi.rcMonitor.right) / 2.0;
    double localPx = centerScreenPx - taskbarRect.left;
    return localPx * scale;
}

double FullFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + element.ActualWidth() + m.Right;
}

// ============================================================================
// Element identification
// ============================================================================

enum class SystemButton {
    None,
    Start,
    Widgets,
    Search,
    TaskView,
};

SystemButton IdentifySystemButton(FrameworkElement element) {
    auto className = winrt::get_class_name(element);

    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId =
            Automation::AutomationProperties::GetAutomationId(element);
        if (automationId == L"StartButton") {
            return SystemButton::Start;
        }
        if (automationId == L"TaskViewButton") {
            return SystemButton::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {
        if (element.Name() == L"AugmentedEntryPointButton") {
            return SystemButton::Widgets;
        }
    } else if (className == L"Taskbar.TaskbarExtensionElement") {
        return SystemButton::Search;
    }

    return SystemButton::None;
}

bool IsTaskListButton(FrameworkElement element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

// ============================================================================
// Layout: computing target X positions
// ============================================================================

double ComputeStartButtonX(FrameworkElement startElement) {
    double centerX = GetMonitorCenterXLocal();
    return centerX - startElement.ActualWidth() / 2.0;
}

int SystemButtonRank(SystemButton b) {
    switch (b) {
        case SystemButton::Search:
            return 0;
        case SystemButton::TaskView:
            return 1;
        case SystemButton::Widgets:
            return 2;
        default:
            return -1;
    }
}

// Total footprint of Search+TaskView+Widgets together, used both to lay
// them out and to reserve room for them next to Start (in adjacent mode) so
// task list buttons on that side don't overlap them. Updated whenever any
// of the three arranges; read by task list button placement.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

double SystemButtonClusterWidth(FrameworkElement repeater) {
    double total = 0;
    for (auto& child : GetRepeaterChildElements(repeater)) {
        if (SystemButtonRank(IdentifySystemButton(child)) >= 0) {
            total += FullFootprintWidth(child);
        }
    }
    return total;
}

// Places Search/TaskView/Widgets either at the taskbar's far left edge, or
// immediately adjacent to one side of the Start button, per
// g_settings.systemButtonsPlacement.
double ComputeSystemButtonX(FrameworkElement repeater,
                            FrameworkElement targetElement,
                            SystemButton target,
                            double startCenterX,
                            double startWidth) {
    int targetRank = SystemButtonRank(target);
    if (targetRank < 0) {
        return 0;
    }

    double widthBefore = 0;
    for (auto& child : GetRepeaterChildElements(repeater)) {
        int r = SystemButtonRank(IdentifySystemButton(child));
        if (r >= 0 && r < targetRank) {
            widthBefore += FullFootprintWidth(child);
        }
    }

    if (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft) {
        g_lastLeftSystemClusterWidth = 0;
        g_lastRightSystemClusterWidth = 0;
        return 8 + widthBefore;  // small left margin
    }

    double clusterWidth = SystemButtonClusterWidth(repeater);
    double gap = g_settings.gapPx;
    double ownWidth = FullFootprintWidth(targetElement);

    if (g_settings.systemButtonsAdjacentSide == Side::Left) {
        g_lastLeftSystemClusterWidth = clusterWidth;
        g_lastRightSystemClusterWidth = 0;
        // Stack right-to-left outward from Start: lowest rank closest.
        double widthAfter = clusterWidth - widthBefore - ownWidth;
        return startCenterX - startWidth / 2.0 - gap - widthAfter - ownWidth;
    }

    g_lastLeftSystemClusterWidth = 0;
    g_lastRightSystemClusterWidth = clusterWidth;
    return startCenterX + startWidth / 2.0 + gap + widthBefore;
}

// Updated whenever the Start button's own Arrange runs. Read by task list
// button arrangement so it doesn't need to assume Start lives in the same
// container as the task buttons (unconfirmed - see GetSiblingElements).
double g_lastStartWidth = 48;

// Diagnostics only: counts what got overridden during the current
// ArrangeOverride pass, logged (throttled) from the pass's own hook.
struct ArrangePassStats {
    int totalArrangeCalls = 0;
    int startHits = 0;
    int pinnedHits = 0;
    int taskListHits = 0;
    int taskListHwndResolved = 0;
    int taskListLeft = 0;
    int taskListRight = 0;
    int qiFailures = 0;
    int noParent = 0;
    int exceptions = 0;
    int wrongTaskbarSkipped = 0;
    int settlingSkipped = 0;
};
// thread_local: the ArrangeOverride hook runs once per taskbar instance's
// XAML tree, process-wide - with a second monitor enabled that's the
// primary taskbar and secondary-monitor taskbars potentially interleaving
// on different threads. A single shared instance here was a data race
// (unsynchronized concurrent reset/read of a non-atomic struct) that also
// fed directly into the button-count self-correction logic below.
thread_local ArrangePassStats g_passStats;

double ComputeTaskListButtonX(FrameworkElement target,
                               double startCenterX) {
    ButtonClassification targetInfo = ClassifyTaskListButton(target);
    if (targetInfo.hwndResolved) {
        g_passStats.taskListHwndResolved++;
    }
    if (targetInfo.side == Side::Left) {
        g_passStats.taskListLeft++;
    } else {
        g_passStats.taskListRight++;
    }

    void* targetAbi = winrt::get_abi(target);
    bool byDistance =
        g_settings.taskListOrder == TaskListOrder::DistanceFromCenter;

    double sameSideWidthBefore = 0;
    if (byDistance) {
        // Rank by distance from screen-center rather than taskbar order, so
        // the icon nearest Start corresponds to the window nearest center.
        // Pointer-value tiebreak keeps ties stable frame to frame.
        for (auto& child : GetSiblingElements(target)) {
            if (!IsTaskListButton(child) || winrt::get_abi(child) == targetAbi) {
                continue;
            }
            ButtonClassification childInfo = ClassifyTaskListButton(child);
            if (childInfo.side != targetInfo.side) {
                continue;
            }
            bool closer =
                childInfo.orderKey < targetInfo.orderKey ||
                (childInfo.orderKey == targetInfo.orderKey &&
                 winrt::get_abi(child) < targetAbi);
            if (closer) {
                sameSideWidthBefore += FullFootprintWidth(child);
            }
        }
    } else {
        for (auto& child : GetSiblingElements(target)) {
            if (winrt::get_abi(child) == targetAbi) {
                break;
            }
            if (!IsTaskListButton(child)) {
                continue;
            }
            if (ClassifyTaskListButton(child).side == targetInfo.side) {
                sameSideWidthBefore += FullFootprintWidth(child);
            }
        }
    }

    double width = FullFootprintWidth(target);
    double gap = g_settings.gapPx;
    double startWidth = g_lastStartWidth;

    bool adjacent =
        g_settings.systemButtonsPlacement == SystemButtonsPlacement::AdjacentStart;
    double leftExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Left)
                            ? (g_lastLeftSystemClusterWidth + gap)
                            : 0;
    double rightExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Right)
                             ? (g_lastRightSystemClusterWidth + gap)
                             : 0;

    if (targetInfo.side == Side::Left) {
        return startCenterX - startWidth / 2.0 - gap - leftExtra -
               sameSideWidthBefore - width;
    }

    return startCenterX + startWidth / 2.0 + gap + rightExtra +
           sameSideWidthBefore;
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Live drag-follow section); forward-declared here because
// the ArrangeOverride hook below uses it to self-correct when the button
// set changes (new pin, app launched/closed) - see the comment at its call
// site for why that's needed.
//
// Only ever marks the taskbar's layout dirty (InvalidateArrange/
// InvalidateMeasure) - never forces a synchronous UpdateLayout() call. See
// the comment on the definition for why: a forced synchronous pass here
// was the confirmed cause of three separate explorer.exe crash-loop
// incidents, each via a different call path landing while the thread was
// already nested inside XAML-internal layout activity.
void InvalidateTaskbarLayout();

// Defined later (Mod lifecycle section); forward-declared here so
// EnsureTaskbarWnd (below) can start the drag-follow WinEventHook as soon
// as the taskbar window resolves, whether that happens at normal startup
// or late (see EnsureTaskbarWnd's comment).
void StartWinEventHook();

// Defined later (Mod lifecycle section); forward-declared here for the
// same reason as StartWinEventHook above - starts the timer that drives
// ResolvePendingButtonHwnds.
void StartButtonHwndResolveTimer();

// g_hTaskbarWnd is normally resolved once, in Wh_ModAfterInit. But if
// Windhawk injects into explorer.exe before Shell_TrayWnd has been created
// yet - observed after a fresh boot, never after a manual mod disable/
// re-enable since the taskbar is already fully up by then - that one-shot
// lookup fails and g_hTaskbarWnd stays null forever, which silently
// disables all positioning (IUIElement_Arrange_Hook and the position math
// below both require it). Retried once per ArrangeOverride pass instead
// (see call site) so the mod self-heals as soon as the taskbar exists,
// rather than needing a manual toggle to re-run Wh_ModAfterInit.
HWND EnsureTaskbarWnd() {
    if (g_hTaskbarWnd) {
        return g_hTaskbarWnd;
    }

    g_hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (g_hTaskbarWnd) {
        Wh_Log(L"Resolved taskbar window: %p", g_hTaskbarWnd);
        StartWinEventHook();
        StartButtonHwndResolveTimer();
    }

    return g_hTaskbarWnd;
}

FrameworkElement FindTaskbarFrameRepeater(FrameworkElement anyDescendant) {
    FrameworkElement child = anyDescendant;
    if (child && (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return child;
    }
    return nullptr;
}

// Walks the primary taskbar's current TaskListButtons and (re)resolves any
// whose cache entry is missing, stale (window closed), or past the
// negative-cache TTL - see ResolveAndCacheButtonHwnd's comment for the
// full story on why this runs on a timer (StartButtonHwndResolveTimer,
// Mod lifecycle section) instead of inline during Arrange.
//
// Guarded against running while nested inside an active Arrange pass on
// this thread - defense in depth, not the primary protection. Windows
// generally doesn't deliver WM_TIMER re-entrantly mid-callback on the
// same thread, but there's no hard guarantee of that documented, and the
// check is nearly free.
void ResolvePendingButtonHwnds() {
    if (g_inTaskbarArrangeOverride || !g_hTaskbarWnd) {
        return;
    }

    XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
    if (!xamlRoot) {
        return;
    }

    FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement repeater = FindTaskbarFrameRepeater(content);
    if (!repeater) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    bool anyChanged = false;

    for (auto& child : GetRepeaterChildElements(repeater)) {
        if (!IsTaskListButton(child)) {
            continue;
        }

        void* key = winrt::get_abi(child);
        auto it = g_buttonHwndCache.find(key);
        bool needsResolve = it == g_buttonHwndCache.end();
        if (!needsResolve) {
            needsResolve = it->second.hwnd
                               ? !IsWindow(it->second.hwnd)
                               : (now - it->second.lastAttempt >= 2000);
        }

        if (needsResolve && ResolveAndCacheButtonHwnd(child)) {
            anyChanged = true;
        }
    }

    if (anyChanged) {
        InvalidateTaskbarLayout();
    }
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;

// Last X each element (Start, a system button, or a task list button) was
// legitimately arranged at by this mod - keyed by the XAML element's ABI
// pointer, same identity technique as g_buttonHwndCache. Written only from
// the non-gated path below (a real ComputeXX call, outside the settling
// window); read only from the gated path, so a settling pass can hold
// every already-known element exactly where it last legitimately was
// instead of visibly snapping to Windows' native layout for up to a
// second and back. Y/Width/Height still come from the CURRENT pass's
// incoming rect, not this cache - those can legitimately change pass to
// pass (DPI, taskbar height) independent of X, and freezing them too could
// look wrong across e.g. a DPI change. A brand-new element (no entry yet -
// necessarily the case for whatever just triggered this settling window,
// since a just-inserted button can't have a prior legitimate X) has
// nothing to fall back to and still shows at its native position until
// settling clears, same as before this cache existed.
//
// Not pruned when a button disappears - same acceptable tradeoff as
// g_buttonHwndCache; a stale entry just sits unread forever if that exact
// element (identified by ABI pointer) never reappears.
std::unordered_map<void*, double> g_lastArrangedX;

// Used by every gated branch in IUIElement_Arrange_Hook below - see
// g_lastArrangedX's comment. Deliberately does no repeater traversal (the
// actual crash-prone operation the settling window guards against): a
// map lookup and, at most, one more IUIElement::Arrange call, identical in
// shape to what original() already does.
HRESULT ArrangeSettled(void* pThis,
                       winrt::Windows::Foundation::Rect rect,
                       void* key) {
    auto it = g_lastArrangedX.find(key);
    if (it != g_lastArrangedX.end()) {
        rect.X = it->second;
    }
    return IUIElement_Arrange_Original(pThis, rect);
}

HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inTaskbarArrangeOverride || g_unloading || !g_hTaskbarWnd) {
        return original();
    }

    // This hook replaces the process-wide IUIElement::Arrange vtable slot,
    // so it's invoked directly by XAML's own native call sites for every
    // UIElement being arranged anywhere in explorer.exe - a raw ABI
    // boundary with no C++/WinRT exception translation on the other side.
    // A WinRT call throwing here (observed via a real crash: explorer.exe
    // faulting in Windows.UI.Xaml.dll with STATUS_STOWED_EXCEPTION,
    // 0xC000027B, at enabling a second monitor - a fresh secondary-taskbar
    // XAML tree briefly has elements with no parent yet mid-construction,
    // and .as<T>() throws on a null source where .try_as<T>() would just
    // return null) crosses that boundary uncaught and fail-fasts the whole
    // process. Catch anything and fall back to the real implementation
    // rather than let that happen again.
    try {
        g_passStats.totalArrangeCalls++;

        FrameworkElement element = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(element));
        if (!element) {
            g_passStats.qiFailures++;
            return original();
        }

        // Only reposition elements belonging to the PRIMARY taskbar's own
        // XAML tree - see the comment on g_primaryXamlRootIdentity for why
        // this check exists (it's the fix for a confirmed crash, not a
        // defensive nicety). Resolved lazily and cached; if it hasn't
        // resolved yet, fail safe by skipping rather than risking another
        // misapplied Rect.
        //
        // The resolution call itself is ONLY attempted when confirmed to be
        // running on the primary taskbar's own UI thread. element.XamlRoot()
        // is always safe regardless of thread (XAML only ever invokes
        // Arrange on an element's own thread), but GetTaskbarXamlRoot(
        // g_hTaskbarWnd) reaches across to the PRIMARY's XAML object
        // specifically - every other call site for it in this file
        // marshals via RunFromWindowThread first. This one can't (Arrange
        // needs a synchronous answer), so instead it skips the resolution
        // attempt entirely on the wrong thread and retries next call - it's
        // guaranteed to eventually run on the primary's own thread once
        // primary's own elements arrange, no marshaling needed. Skipping
        // this thread check on the first version of this fix was itself a
        // second, unmarshaled cross-apartment WinRT call - i.e. a fresh
        // instance of the very same crash class this whole check exists to
        // prevent, which is why it could still crash with
        // wrongTaskbarSkipped staying at 0 (the crash happened inside this
        // resolution call, before ever reaching that counter).
        void* primaryIdentity = g_primaryXamlRootIdentity.load();
        if (!primaryIdentity) {
            DWORD primaryThreadId = GetWindowThreadProcessId(g_hTaskbarWnd, nullptr);
            if (primaryThreadId != 0 && primaryThreadId == GetCurrentThreadId()) {
                primaryIdentity =
                    XamlRootIdentity(GetTaskbarXamlRoot(g_hTaskbarWnd));
                if (primaryIdentity) {
                    g_primaryXamlRootIdentity.store(primaryIdentity);
                }
            }
        }
        void* elementIdentity = XamlRootIdentity(element.XamlRoot());
        if (!primaryIdentity || !elementIdentity ||
            elementIdentity != primaryIdentity) {
            g_passStats.wrongTaskbarSkipped++;
            return original();
        }

        auto repeater = Media::VisualTreeHelper::GetParent(element)
                            .try_as<FrameworkElement>();
        if (!repeater) {
            g_passStats.noParent++;
            return original();
        }

        SystemButton systemButton = IdentifySystemButton(element);

        winrt::Windows::Foundation::Rect newRect = rect;

        if (systemButton == SystemButton::Start) {
            // Originally left ungated, on the reasoning that
            // ComputeStartButtonX never touches a repeater so it isn't part
            // of the crash-prone class the settling window guards against
            // (see g_taskListSettlingUntil's comment). That missed a
            // cosmetic consequence: gating the task list/system buttons but
            // NOT Start meant Start kept snapping to the forced screen
            // center every pass while its neighbors froze at wherever
            // native XAML layout put them (which doesn't reserve center
            // space for a repositioned Start) - the observed "icons overlay
            // the centered Start button" glitch on every structural change,
            // e.g. a window moving to another monitor. Gating Start too
            // makes the whole taskbar fall back to one consistent state
            // for the settling window instead of a mismatched hybrid;
            // it's still safe to gate since this is only skipping our own
            // Rect override, not adding a new repeater traversal.
            if (GetTickCount64() < g_taskListSettlingUntil) {
                g_passStats.settlingSkipped++;
                return ArrangeSettled(pThis, rect, winrt::get_abi(element));
            }
            g_passStats.startHits++;
            g_lastStartWidth = element.ActualWidth();
            newRect.X = ComputeStartButtonX(element);
            g_lastArrangedX[winrt::get_abi(element)] = newRect.X;
        } else if (systemButton == SystemButton::Search ||
                   systemButton == SystemButton::TaskView ||
                   systemButton == SystemButton::Widgets) {
            // See g_taskListSettlingUntil's comment: both this branch and
            // the task list one below call into repeater traversal
            // (SystemButtonClusterWidth / GetSiblingElements), which is
            // suppressed for a short window after any detected
            // structural change to the button set.
            if (GetTickCount64() < g_taskListSettlingUntil) {
                g_passStats.settlingSkipped++;
                return ArrangeSettled(pThis, rect, winrt::get_abi(element));
            }
            g_passStats.pinnedHits++;
            newRect.X = ComputeSystemButtonX(repeater, element, systemButton,
                                             GetMonitorCenterXLocal(),
                                             g_lastStartWidth);
            g_lastArrangedX[winrt::get_abi(element)] = newRect.X;
        } else if (IsTaskListButton(element)) {
            if (GetTickCount64() < g_taskListSettlingUntil) {
                g_passStats.settlingSkipped++;
                return ArrangeSettled(pThis, rect, winrt::get_abi(element));
            }
            g_passStats.taskListHits++;
            double startCenterX = GetMonitorCenterXLocal();
            newRect.X = ComputeTaskListButtonX(element, startCenterX);
            g_lastArrangedX[winrt::get_abi(element)] = newRect.X;
        } else {
            return original();
        }

        return IUIElement_Arrange_Original(pThis, newRect);
    } catch (...) {
        g_passStats.exceptions++;
        return original();
    }
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    static bool loggedFirstCall;
    if (!loggedFirstCall) {
        loggedFirstCall = true;
        Wh_Log(L"ArrangeOverride hook is firing (first call)");
    }

    g_passStats = {};

    EnsureTaskbarWnd();

    g_inTaskbarArrangeOverride = true;

    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);

    g_inTaskbarArrangeOverride = false;

    // The button count can change without any window moving (new pin, app
    // launched/closed), and the taskbar's own virtualization bookkeeping
    // doesn't reliably reconcile with our position overrides for a newly
    // realized element - the symptom is a new pin not appearing at all
    // until something forces a full fresh relayout. Self-correct by
    // invalidating whenever the count changes, which is enough for the new
    // element to settle on the XAML dispatcher's next tick. Guarded against
    // recursing more than one level deep.
    //
    // thread_local rather than a single global: this hook runs process-wide
    // for every taskbar instance's XAML tree (primary and, with a second
    // monitor enabled, secondary-monitor taskbars too), and those can run
    // on different threads with independently-changing button counts. A
    // shared global here raced across instances - one taskbar's pass could
    // see another's count change and misfire - which is exactly the
    // condition (taskList count oscillating 4->1->4 across two passes)
    // observed immediately preceding a crash-loop with a second monitor
    // enabled.
    static thread_local int lastTaskListButtonCount = -1;
    static thread_local bool inCountChangeRetry;
    int currentCount = g_passStats.taskListHits;
    // A settling-gated pass reads currentCount as a synthetic 0 (every
    // element that would've counted toward taskListHits returns early via
    // the settlingSkipped branch instead - see IUIElement_Arrange_Hook).
    // That's not a real reading of the button set, so it must never reach
    // the comparison below or get written into lastTaskListButtonCount:
    // doing so previously made the very moment settling naturally expired
    // - the first pass to see the real count again - look like a SECOND
    // structural change (real count vs. the stale synthetic 0), which
    // re-armed settling right back on, ad infinitum. That produced a
    // self-sustaining flash cycle with no further genuine trigger - the
    // taskbar kept re-blanking every few seconds long after the one actual
    // cross-monitor move, which is what made the mod look permanently
    // stuck/disabled rather than settling once and recovering.
    bool passWasGated = g_passStats.settlingSkipped > 0;
    if (!passWasGated && currentCount != lastTaskListButtonCount) {
        bool countChanged = lastTaskListButtonCount != -1;
        lastTaskListButtonCount = currentCount;
        if (countChanged) {
            // See g_taskListSettlingUntil's comment: suppress our own
            // repeater-traversal-based positioning for a bit after any
            // detected structural change, regardless of the
            // inCountChangeRetry guard below (that guard is only about
            // not recursing the invalidate call itself).
            g_taskListSettlingUntil = GetTickCount64() + 1000;
            g_taskListSettlingRecoveryPending = true;
            if (!inCountChangeRetry) {
                inCountChangeRetry = true;
                InvalidateTaskbarLayout();
                inCountChangeRetry = false;
            }
        }
    }

    static ULONGLONG lastStatsLog;
    ULONGLONG now = GetTickCount64();
    if (now - lastStatsLog > 2000) {
        lastStatsLog = now;
        Wh_Log(
            L"Arrange pass: total=%d start=%d pinned=%d taskList=%d "
            L"(hwndResolved=%d left=%d right=%d) qiFail=%d noParent=%d "
            L"wrongTaskbarSkipped=%d settlingSkipped=%d exceptions=%d | "
            L"winEvents: raw=%d invalidated=%d skippedReentrant=%d "
            L"invalidateExceptions=%d | resolve(individual): ok=%d "
            L"viewModelNull=%d getTaskItemFail=%d sentinelNoItem=%d | "
            L"resolve(group): ok=%d viewModelNull=%d sentinelNoGroup=%d "
            L"noItems=%d",
            g_passStats.totalArrangeCalls, g_passStats.startHits,
            g_passStats.pinnedHits, g_passStats.taskListHits,
            g_passStats.taskListHwndResolved, g_passStats.taskListLeft,
            g_passStats.taskListRight, g_passStats.qiFailures,
            g_passStats.noParent, g_passStats.wrongTaskbarSkipped,
            g_passStats.settlingSkipped, g_passStats.exceptions,
            (int)g_winEventRawCount,
            (int)g_winEventInvalidateCount, (int)g_invalidateSkippedReentrant,
            (int)g_invalidateExceptions, g_resolveStats.success,
            g_resolveStats.failViewModelNull, g_resolveStats.failGetTaskItem,
            g_resolveStats.failSentinelNoItem, g_resolveStats.groupSuccess,
            g_resolveStats.groupFailViewModelNull,
            g_resolveStats.groupFailSentinelNoGroup,
            g_resolveStats.groupFailNoItems);
        g_resolveStats = {};
    }

    return ret;
}

// ============================================================================
// Live drag-follow: force a taskbar relayout when a top-level window moves
// ============================================================================

// Guards against reentering InvalidateTaskbarLayout itself on the
// taskbar's own thread. WinEventProc can fire in rapid bursts (observed:
// thousands of raw events within seconds while something on screen is
// spamming EVENT_OBJECT_LOCATIONCHANGE).
//
// This lambda deliberately never calls UpdateLayout() - see the note on
// InvalidateTaskbarLayout below for why forcing a synchronous layout pass
// here is unsafe regardless of this guard.
thread_local bool g_inInvalidateTaskbarLayout;

// Marks the taskbar's layout dirty and lets the XAML dispatcher pick it up
// on its own next tick, rather than forcing an immediate synchronous
// UpdateLayout() call. An earlier version forced it (for drag-follow
// snappiness), gated behind a reentrancy guard and a forceImmediate flag
// that was false only for one specific call site. That was not enough:
// this function's callers include a raw OS callback (WinEventProc) that
// can itself fire while the thread is already nested inside XAML-internal
// layout activity for reasons outside this mod's control (e.g. a taskbar
// button structurally moving between two different monitors' XAML trees,
// not just changing coordinates within one). A forced UpdateLayout() in
// that state is a reentrant "layout cycle" from WinUI's perspective, and
// it fails fast with STATUS_STOWED_EXCEPTION - the confirmed cause of
// three separate explorer.exe crash-loop incidents (identical fault offset
// in Windows.UI.Xaml.dll every time), each via a different trigger path,
// even after two of those paths were individually guarded. Since new
// trigger paths kept appearing, the only fix that actually closes the
// whole class is to never force it anywhere: InvalidateArrange() +
// InvalidateMeasure() alone are always safe to call from any context, and
// the resulting delay before the dispatcher's own pass runs is at most
// one composition frame - not perceptible for a drag-follow feature that
// was already documented as best-effort ("shortly after a drag/move
// settles, not on every intermediate pixel").
//
// Also important: that STATUS_STOWED_EXCEPTION is a raw SEH RaiseException,
// not a thrown C++ exception. This mod's toolchain (Windhawk's clang/MinGW
// build, no /EHa-style async exception handling) cannot catch it with
// `catch(...)` no matter where it's placed - confirmed by
// g_invalidateExceptions staying 0 across multiple full crash-loop
// sessions. The try/catch below only covers genuine C++ exceptions from
// the WinRT calls in this lambda (e.g. QueryInterface failures surfaced as
// hresult_error) - it was never a safety net for the layout-cycle failure,
// and no try/catch placement ever will be. Don't reintroduce a forced
// UpdateLayout() here without a fundamentally different mechanism (e.g. a
// genuinely async PostMessage-deferred call that's guaranteed to run only
// once the current call stack has fully unwound).
void InvalidateTaskbarLayout() {
    if (!g_hTaskbarWnd) {
        return;
    }

    bool posted = RunFromWindowThread(g_hTaskbarWnd, [] {
        if (g_inInvalidateTaskbarLayout) {
            g_invalidateSkippedReentrant++;
            return;
        }
        g_inInvalidateTaskbarLayout = true;

        try {
            XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"InvalidateTaskbarLayout: GetTaskbarXamlRoot failed");
                g_inInvalidateTaskbarLayout = false;
                return;
            }

            FrameworkElement content =
                xamlRoot.Content().try_as<FrameworkElement>();
            FrameworkElement repeater = FindTaskbarFrameRepeater(content);
            if (!repeater) {
                Wh_Log(
                    L"InvalidateTaskbarLayout: FindTaskbarFrameRepeater "
                    L"failed");
                g_inInvalidateTaskbarLayout = false;
                return;
            }

            repeater.InvalidateArrange();
            repeater.InvalidateMeasure();
        } catch (...) {
            g_invalidateExceptions++;
        }

        g_inInvalidateTaskbarLayout = false;
    });
    if (!posted) {
        Wh_Log(L"InvalidateTaskbarLayout: RunFromWindowThread failed");
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hook,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD idEventThread,
                           DWORD dwmsEventTime) {
    g_winEventRawCount++;

    if (g_unloading || idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        !hwnd || hwnd == g_hTaskbarWnd) {
        return;
    }

    if (!IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd ||
        GetWindow(hwnd, GW_OWNER) != nullptr) {
        return;
    }

    static ULONGLONG lastInvalidate;
    ULONGLONG now = GetTickCount64();
    if (now - lastInvalidate < 150) {
        return;
    }
    lastInvalidate = now;

    g_winEventInvalidateCount++;
    InvalidateTaskbarLayout();
}

// ============================================================================
// Module/symbol hooking plumbing
// ============================================================================

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
            nullptr,
            true,
        },
    };

    bool ok = HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    Wh_Log(L"HookTaskbarDllSymbols: %s", ok ? L"OK" : L"FAILED");
    Wh_Log(L"  TaskItem::ReportClicked: %s",
           TaskItem_ReportClicked_Original ? L"resolved" : L"MISSING");
    Wh_Log(L"  CTaskGroup::GetNumItems: %s",
           CTaskGroup_GetNumItems_Original ? L"resolved" : L"MISSING");
    Wh_Log(L"  TaskGroup::ReportClicked: %s",
           TaskGroup_ReportClicked_Original ? L"resolved" : L"MISSING");
    return ok;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // All marked optional so HookSymbols doesn't abort the whole batch on
    // the first miss - we want a per-symbol report while bringing this mod
    // up on a new Windows build, not just a single opaque FAILED.
    //
    // Taskbar.View.dll, ExplorerExtensions.dll (see GetTaskbarViewModuleHandle
    // - which of the two is actually loaded depends on the Windows build)
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
            nullptr,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
            true,
        },
    };

    bool ok = HookSymbols(module, taskbarViewDllHooks, ARRAYSIZE(taskbarViewDllHooks));
    Wh_Log(L"HookTaskbarViewDllSymbols: %s", ok ? L"OK" : L"FAILED");
    Wh_Log(L"  ArrangeOverride: %s",
           TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original ? L"resolved"
                                                                        : L"MISSING");
    Wh_Log(L"  TryGetItemFromContainer<TaskListWindowViewModel>: %s",
           TryGetItemFromContainer_TaskListWindowViewModel_Original ? L"resolved"
                                                                     : L"MISSING");
    Wh_Log(L"  TaskListWindowViewModel::get_TaskItem: %s",
           TaskListWindowViewModel_get_TaskItem_Original ? L"resolved"
                                                          : L"MISSING");
    Wh_Log(L"  TryGetItemFromContainer<TaskListGroupViewModel>: %s",
           TryGetItemFromContainer_TaskListGroupViewModel_Original ? L"resolved"
                                                                    : L"MISSING");
    Wh_Log(L"  TaskListGroupViewModel::IsMultiWindow: %s",
           TaskListGroupViewModel_IsMultiWindow_Original ? L"resolved"
                                                          : L"MISSING");
    Wh_Log(L"  ITaskGroup::IsRunning: %s",
           ITaskGroup_IsRunning_Original ? L"resolved" : L"MISSING");
    return ok;
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
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

// WINEVENT_OUTOFCONTEXT delivers its callback on whichever thread called
// SetWinEventHook, and only if that thread pumps messages. Wh_ModAfterInit
// doesn't reliably run on the taskbar's own message-pumping UI thread (the
// raw=0 counter in the logs confirms the callback never fired at all), so
// registration is marshaled onto g_hTaskbarWnd's thread via
// RunFromWindowThread, same technique already used for XAML tree access.
void StartWinEventHook() {
    if (g_locationChangeHook || !g_hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(g_hTaskbarWnd, [] {
        if (g_locationChangeHook) {
            return;
        }

        // Deliberately NOT using WINEVENT_SKIPOWNPROCESS: File Explorer
        // windows often run inside explorer.exe's own process, and that
        // flag would silently drop their location-change events too. The
        // taskbar's own windows are already excluded explicitly in
        // WinEventProc.
        g_locationChangeHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        Wh_Log(L"StartWinEventHook: handle=%p (registered on taskbar thread "
               L"%lu)",
               g_locationChangeHook, GetCurrentThreadId());
        if (!g_locationChangeHook) {
            Wh_Log(L"Failed to register location-change hook - live "
                   L"drag-follow will not work, but everything else still "
                   L"will");
        }
    });
}

void StopWinEventHook() {
    if (!g_locationChangeHook) {
        return;
    }

    HWINEVENTHOOK hook = g_locationChangeHook;
    g_locationChangeHook = nullptr;

    if (g_hTaskbarWnd) {
        RunFromWindowThread(g_hTaskbarWnd, [hook] { UnhookWinEvent(hook); });
    } else {
        UnhookWinEvent(hook);
    }
}

// Arbitrary, only needs to be unique for calls against g_hTaskbarWnd.
constexpr UINT_PTR kButtonHwndResolveTimerId = 1;

void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime) {
    // See g_taskListSettlingRecoveryPending's comment: guarantee a fresh
    // Arrange pass actually happens once the settling window is over,
    // instead of leaving recovery to whatever unrelated event happens to
    // invalidate layout next. At most 500ms of extra staleness on top of
    // the settling window itself, which is already the existing polling
    // granularity for hwnd resolution below.
    if (g_taskListSettlingRecoveryPending &&
        GetTickCount64() >= g_taskListSettlingUntil) {
        g_taskListSettlingRecoveryPending = false;
        InvalidateTaskbarLayout();
    }

    ResolvePendingButtonHwnds();
}

// SetTimer's callback fires on whichever thread registered it, so this is
// marshaled onto g_hTaskbarWnd's own thread just like StartWinEventHook -
// both KillTimer and (for reliability) SetTimer itself are documented to
// need to run on the thread that owns the timer.
void StartButtonHwndResolveTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(g_hTaskbarWnd, [] {
        SetTimer(g_hTaskbarWnd, kButtonHwndResolveTimerId, 500,
                 ButtonHwndResolveTimerProc);
    });
}

void StopButtonHwndResolveTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(g_hTaskbarWnd, [] {
        KillTimer(g_hTaskbarWnd, kButtonHwndResolveTimerId);
    });
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        Wh_Log(L"Taskbar view module already loaded at init time");
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, will hook on load");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                       "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    EnsureTaskbarWnd();
    Wh_Log(L"g_hTaskbarWnd = %p, g_taskbarViewDllLoaded = %d", g_hTaskbarWnd,
           (int)g_taskbarViewDllLoaded);

    InvalidateTaskbarLayout();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    StopWinEventHook();
    StopButtonHwndResolveTimer();

    InvalidateTaskbarLayout();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    InvalidateTaskbarLayout();

    return TRUE;
}
